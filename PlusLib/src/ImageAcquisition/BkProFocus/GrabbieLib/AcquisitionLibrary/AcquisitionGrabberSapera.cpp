#include <algorithm>

#include "BkmDataFile.h"
#include "LogLibrary.h"

#include "AcquisitionGrabberSapera.h"
#include "AcquisitionInjector.h"
#include "AcquisitionSettings.h"
#include "SaperaProcessor.h"

/// <summary>
/// The function crops val so that it is in the range 
/// [minVal;maxVal]. If val is in the range, val is returned.
/// </summary>
/// <tparam name="T">       A primitive type (int, float, etc.) </tparam>
/// <param name="val">      The value to clamp </param>
/// <param name="minValue"> The minimum value. </param>
/// <param name="maxValue"> The maximum value. </param>
/// <returns>Val clamped to the range [minVal,maxVal]</returns>
template<typename T> T Clamp(T val, T minValue, T maxValue)
{
    assert(minValue <= maxValue);
    // this method is x2 faster than using two ifs
    return std::max<T>(std::min<T>(val, maxValue), minValue);
}

AcquisitionGrabberSapera::AcquisitionGrabberSapera()
    : settings(NULL), researchInterface()
    , saperaLocation(NULL), saperaAcquisition(NULL), saperaBuffer(NULL), saperaProcessor(NULL), saperaTransfer(NULL)
    , acquisitionServerName(NULL), acquisitionDeviceIndex(-1), saperaInitialized(false)
{
    this->acquisitionServerName = new char[CORSERVER_MAX_STRLEN];
}

AcquisitionGrabberSapera::~AcquisitionGrabberSapera()
{
    delete[] acquisitionServerName;
    DestroySapera();
}

bool AcquisitionGrabberSapera::Init(const AcquisitionSettings& acquisitionSettings)
{
    this->settings = &acquisitionSettings;

	SapManager::SetDisplayStatusMode(SapManager::StatusLog);

    if(saperaInitialized)
    {
        DestroySapera();
    }

    saperaInitialized = false;
    // setup the research interface
    if( !researchInterface.OpenConnection(acquisitionSettings) || !researchInterface.SetupConnection(acquisitionSettings) || !researchInterface.CloseConnection() )
    {
        // Unable to properly start the research interface, so quit
        LogPrintf(std::string(__FUNCTION__) + _T(" : Unable to start the Research Interface.\n"));
        LogPrintf(_T("Error: ") + researchInterface.GetError() + _T("\n"));
        researchInterface.ResetError();
        return false;
    }
    if(!FindSaperaDevice() || !AllocateSapera())
    {
        // Unable to initialize Sapera, so quit - the functions already registered the error.
        return false;
    }
    saperaInitialized = true;
    return true;
}

bool AcquisitionGrabberSapera::Destroy()
{
    return DestroySapera();
}

bool AcquisitionGrabberSapera::StartGrabbing(AcquisitionInjector* acquisitionInjector)
{
    // validate status
    if (!IsInitialized())
        return false;

    // setup sapera processor
    saperaProcessor->LoadAcquisitionSettings(*settings);
    if (acquisitionInjector != NULL)
    {
        acquisitionInjector->PrepareInjection(this->settings->GetRFLineLength(), this->saperaBuffer->GetHeight(), this->saperaBuffer->GetWidth());
    }
    saperaProcessor->SetAcquisitionInjector(acquisitionInjector);

    // start grabbing
    BOOL result = FALSE;
    if (settings->GetFramesToGrab() > 0)
    {
        // compute the actual number of Sapera frames to grab, as this may differ from
        // the number of frames that the user wants to grab.
        int saperaFramesToGrab = settings->CalculateNumberOfAcquisitionBufferFrames();
		LogPrintf("Frames to capture %d\n", saperaFramesToGrab);
        result = saperaTransfer->Snap(saperaFramesToGrab);
    }
    else
    {
        result = saperaTransfer->Grab();
    }
    return result == TRUE;
}

bool AcquisitionGrabberSapera::StopGrabbing()
{
    BOOL result = saperaTransfer->Freeze();
    saperaTransfer->Wait(500);
    // consider clearing sapera buffers after grabbing is stopped
    // ... alternatively, the saperaTransfer->Init() method can be 
    // called (when starting a grab?) as it resets the buffers.
    saperaProcessor->GetAcquisitionInjector()->CleanupInjection();
	LogPrintf("Frames captured %d\n", this->saperaProcessor->GetFramesCaptured());
    return result == TRUE;
}

bool AcquisitionGrabberSapera::SaveBuffer(const char* pathToFile, int startFrame, int endFrame)
{
    // validate state
    if (this->IsGrabbing() || this->saperaBuffer ==  NULL || this->saperaProcessor == NULL || this->saperaProcessor->GetFramesCaptured() < 1)
    {
        return false;
    }

    // validate input
    if (pathToFile == NULL || startFrame > this->saperaBuffer->GetIndex()  || endFrame > this->saperaBuffer->GetIndex())
    {
        return false;
    }
    
    // build header
    BkmDf::FrameSizeInfo frameSizeInfo;
    BkmDf::UltrasoundDataInfo ultrasoundDataInfo;

    // TODO: Expand data format so it is possible to query values from it (cleaner)
    int numberOfSamples = this->settings->GetRFLineLength() / BkmDf::sampleFormatInfoTable[BkmDf::SAMPLE_FORMAT_INT16X2].numValues; 
    int numberOfLines = this->settings->GetLinesPerFrame();
    BkmDf::FrameDimUtil::SetDefaultBmodeIQdataUltraView(&frameSizeInfo, numberOfSamples, numberOfLines);
	frameSizeInfo.dimInfo[0].headerLen = 0;
	frameSizeInfo.dimInfo[0].numElem += 1;
    frameSizeInfo.dimInfo[0].padLen = this->saperaBuffer->GetPitch() - frameSizeInfo.dimInfo[0].headerLen - frameSizeInfo.dimInfo[0].numElem * BkmDf::sampleFormatInfoTable[BkmDf::SAMPLE_FORMAT_INT16X2].bytesPerSample;
    assert(frameSizeInfo.dimInfo[0].padLen >= 0);
    frameSizeInfo.frameLen = BkmDf::FrameDimUtil::CalcFrameSize(frameSizeInfo);
    assert(frameSizeInfo.frameLen > 0);

    BkmDf::UltrasoundDataUtil::SetBmodeInputBuff(&ultrasoundDataInfo, 1);

    BkmDf::BkmDataFile bkmFile;
    if (!bkmFile.OpenCreate(pathToFile, frameSizeInfo, ultrasoundDataInfo))
    {
        // unable to open file
        return false;
    }

    // Compute the frame indices to save. After this code has run, 
    // the startFrame will be the first frame in the buffer to save, 
    // and endFrame will be the index just after the last frame to save.
    int capacity = this->saperaBuffer->GetCount();
    if (startFrame < 0 || endFrame < 0)
    {
        if (this->saperaProcessor->GetFramesCaptured() > capacity)
        {
            startFrame = this->saperaBuffer->GetIndex() + 1;
        }
        else
        {
            // if a full circle has not been completed, compute the start index
            // add 1 as the buffer is zero indexed, whereas the frame count is not
            startFrame = this->saperaBuffer->GetIndex() + 1 - this->saperaProcessor->GetFramesCaptured();
            if (startFrame < 0)
            {
                startFrame += capacity;
            }
        }
        endFrame = (this->saperaBuffer->GetIndex()+1)%capacity;
    }
    else
    {
        startFrame = startFrame%capacity;
        endFrame = (endFrame+1)%capacity;
    }

    // save frame data
    void* frameData = NULL;
    int i = startFrame;
    bool success = true;
    
    // a do-while loop is used as startFrame and endFrame will be equal in 
    // the first iteration if the whole buffer is to be saved.
    do
    {
        if (this->saperaBuffer->GetAddress(i, &frameData) != TRUE)
        {
            // something went wrong when loading the data
            assert(false);
            success = false;
            // handle error by breaking out of loop, so file can be closed and return false
            break;
        }

        // TODO: Investigate if this frame matches midlevel mode of target stream
        // At the moment only b-mode is supported and sent over the research interface
        // but this may change.

        bkmFile.AppendRawFrame(frameData);
        this->saperaBuffer->ReleaseAddress(frameData);

        i = (i+1)%capacity;
    } while(i != endFrame);

    bkmFile.Close();

    return success;
}

bool AcquisitionGrabberSapera::IsInitialized() const
{
    return saperaInitialized;    
}

bool AcquisitionGrabberSapera::IsHardwareAvailable() const
{
    return IsSaperaAvailable();
}

bool AcquisitionGrabberSapera::IsGrabbing() const
{
    return saperaTransfer != NULL && saperaTransfer->IsGrabbing();
}

SapBuffer* AcquisitionGrabberSapera::GetBuffer() const
{
    return saperaBuffer;
}

bool AcquisitionGrabberSapera::IsSaperaAvailable()
{
    int serverCount = SapManager::GetServerCount();
    for (int serverIndex = 0; serverIndex < serverCount; ++serverIndex)
    {
        if (SapManager::GetResourceCount(serverIndex, SapManager::ResourceAcq) != 0)
        {
            return true;
        }
    }
    return false;
}

bool AcquisitionGrabberSapera::FindSaperaDevice()
{
    int serverCount = SapManager::GetServerCount();
    if (serverCount == 0)
    {
        LogPrintf(std::string(__FUNCTION__) + _T(" : No camera link board found! \n"));
        return false;
    }

    // Scan the servers to find those that support acquisition
    for (int serverIndex = 0; serverIndex < serverCount; ++serverIndex)
    {
        if (SapManager::GetResourceCount(serverIndex, SapManager::ResourceAcq) != 0)
        {
            // This server supports acquisition
            SapManager::GetServerName(serverIndex, this->acquisitionServerName, CORSERVER_MAX_STRLEN);

            // Find a suitable acquisition device on the server
            int deviceCount = SapManager::GetResourceCount(this->acquisitionServerName, SapManager::ResourceAcq);
            char deviceName[CORPRM_GETSIZE(CORACQ_PRM_LABEL)];
            for (int deviceIndex = 0; deviceIndex < deviceCount; ++deviceIndex)
            {
                this->acquisitionDeviceIndex = deviceIndex;
                SapManager::GetResourceName(this->acquisitionServerName, SapManager::ResourceAcq, deviceIndex, deviceName, sizeof(deviceName));
                char* p = strstr(deviceName,"Mono");
                if (p != NULL)
                {
                    // a suitable device has been found
                    return true;
                }
            }
        }
    }

    // no acquisition boards were found, so return an error
    LogPrintf(std::string(__FUNCTION__) + _T(": No acquisition board found! \n"));
    return false;
}

bool AcquisitionGrabberSapera::AllocateSapera()
{
    // sapera objects is decending into an unknown state
    saperaInitialized = false;

    // assert all sapera objects have been garbage collected
    assert(saperaLocation == NULL || saperaAcquisition == NULL || saperaBuffer == NULL || saperaProcessor == NULL || saperaTransfer == NULL);

    // construct sapera objects
    saperaLocation =  new SapLocation(acquisitionServerName, acquisitionDeviceIndex); 
	saperaAcquisition =  new SapAcquisition(*saperaLocation, settings->GetCcfFile());
    // the buffer loads capture card capabilities from the acquisition object. 
	saperaBuffer = new SapBuffer(this->settings->GetFramesInGrabberBuffer(), saperaAcquisition);
	saperaProcessor = new SaperaProcessor(saperaBuffer); 
	saperaTransfer = new SapAcqToBuf(saperaAcquisition, saperaBuffer, SaperaProcessor::TransferCallback, saperaProcessor);

    if(saperaLocation == NULL || saperaAcquisition == NULL || saperaBuffer == NULL || saperaProcessor == NULL || saperaTransfer == NULL)
    {
        // allocation failed, so destroy objects and return
        LogPrintf(std::string(__FUNCTION__) + _T(" : Failed to allocate Sapera objects \n"));
        delete saperaLocation;
        delete saperaAcquisition;
        delete saperaBuffer;
        delete saperaProcessor;
        delete saperaTransfer;

        saperaLocation = NULL;
        saperaAcquisition = NULL;
        saperaBuffer = NULL;
        saperaProcessor = NULL;
        saperaTransfer = NULL;

        return false;
    }

    // initialize sapera objects using Create()
    if (saperaAcquisition != NULL && saperaAcquisition->Create())
    {
        // set output format to 16 monochromatic
        saperaAcquisition->SetParameter(CORACQ_PRM_OUTPUT_FORMAT, CORACQ_VAL_OUTPUT_FORMAT_MONO16);

        // The different streams can have different number of lines per frame and number
        // of samples per line. We need to ensure memory in each frame for each stream, but
        // while keeping the bound as close as possible to maximize memory utilization.
        int cropMax = 0;
        int cropMin = 0;
        int cropMult = 0;
        int currentVal = 0;
        int linesInFrame = this->settings->GetLinesPerFrame();
        int bytesPerLine = this->settings->GetRFLineLength() + 4;   // SIN: Add 2 additional bytes for header information

        // set height
        saperaAcquisition->GetParameter(CORACQ_PRM_CROP_HEIGHT, &currentVal);
        saperaAcquisition->GetCapability(CORACQ_CAP_CROP_HEIGHT_MAX, &cropMax);
        saperaAcquisition->GetCapability(CORACQ_CAP_CROP_HEIGHT_MIN, &cropMin);
        saperaAcquisition->GetCapability(CORACQ_CAP_CROP_HEIGHT_MULT, &cropMult);
        if(linesInFrame%cropMult)
        {
            linesInFrame = linesInFrame + cropMult -(linesInFrame%cropMult); // dist [min;max] must be a multiple of mult
        }
        linesInFrame = Clamp<int>(linesInFrame, cropMin, cropMax);
        if(currentVal < linesInFrame)
        {
            // Expand active region
            saperaAcquisition->SetParameter(CORACQ_PRM_VACTIVE, linesInFrame);
            saperaAcquisition->SetParameter(CORACQ_PRM_CROP_HEIGHT, linesInFrame);
        }
        else
        {
            // Shrink active region
            saperaAcquisition->SetParameter(CORACQ_PRM_CROP_HEIGHT, linesInFrame);
            saperaAcquisition->SetParameter(CORACQ_PRM_VACTIVE, linesInFrame);
        }

        // set width
        saperaAcquisition->GetParameter(CORACQ_PRM_CROP_WIDTH, &currentVal);
        saperaAcquisition->GetCapability(CORACQ_CAP_CROP_WIDTH_MAX, &cropMax);
        saperaAcquisition->GetCapability(CORACQ_CAP_CROP_WIDTH_MIN, &cropMin);
        saperaAcquisition->GetCapability(CORACQ_CAP_CROP_WIDTH_MULT, &cropMult);
        if(bytesPerLine%cropMult)
        {
            bytesPerLine = bytesPerLine + cropMult - (bytesPerLine%cropMult); // dist [min;max] must be a multiple of mult
        }
        bytesPerLine = Clamp<int>(bytesPerLine, cropMin, cropMax);
        if(currentVal < bytesPerLine)
        {
            // Expand active region
            saperaAcquisition->SetParameter(CORACQ_PRM_HACTIVE, bytesPerLine);
            saperaAcquisition->SetParameter(CORACQ_PRM_CROP_WIDTH, bytesPerLine);
        }
        else
        {
            // Shrink active region
            saperaAcquisition->SetParameter(CORACQ_PRM_CROP_WIDTH, bytesPerLine);
            saperaAcquisition->SetParameter(CORACQ_PRM_HACTIVE, bytesPerLine);
        }
    }
    else
    {
        LogPrintf(std::string(__FUNCTION__) + _T(" : Could not create Acquisition object \n"));
        return false;
    }

    if (saperaBuffer == NULL || !saperaBuffer->Create())
    {
        LogPrintf(std::string(__FUNCTION__) + _T(" : Could not create Buffer object \n"));
        return false;
    }

    if (saperaTransfer == NULL || !saperaTransfer->Create())
    {
        LogPrintf(std::string(__FUNCTION__) + _T(" : Could not create Transfer object \n"));
        return false;
    }

    if (saperaProcessor == NULL || !saperaProcessor->Create() || !saperaProcessor->LoadBufferSettings())
    {
        LogPrintf(std::string(__FUNCTION__) + _T(" : Could not create Processor object \n"));
        return false;
    }
    saperaInitialized = true;
    return true;
}

bool AcquisitionGrabberSapera::DestroySapera()
{
    std::string failures = _T("");

    // stop grabbing if transfer is in effect
    if( saperaTransfer != NULL && saperaTransfer->IsGrabbing() == TRUE )
    {
        saperaTransfer->Freeze();
        if( saperaTransfer->Wait(500) != TRUE)
        {
            saperaTransfer->Abort();
        }
        assert( saperaTransfer->IsGrabbing() == FALSE );
    }

    if( !DestroySaperaObject(&saperaProcessor) )
    {
        failures.Append(_T("saperaProcessor, "));
    }
    if( !DestroySaperaObject(&saperaTransfer) )
    {
        failures.Append(_T("saperaTransfer, "));
    }
    if( !DestroySaperaObject(&saperaBuffer) )
    {
        failures.Append(_T("saperaBuffer, "));
    }
    if( !DestroySaperaObject(&saperaAcquisition) )
    {
        failures.Append(_T("saperaAcquisition, "));
    }
    delete saperaLocation;
    saperaLocation = NULL;
    if( failures.GetLength() > 0 )
    {
        assert(false);
        LogPrintf(std::string(__FUNCTION__) + _T(" : Unable to Destroy ") + failures + _T("\n"));
        return false;
    }
    return true;
}

template<typename SaperaObjectType>
bool AcquisitionGrabberSapera::DestroySaperaObject(SaperaObjectType** saperaObject)
{
    if(*saperaObject != NULL)
    {
        // TODO: Consider calling delete even if destroy fails
        // can we handle a failing destroy?
        if((*saperaObject)->Destroy() == TRUE)
        {
            delete (*saperaObject);
            (*saperaObject) = NULL;
        }
        else
        {
            return false;
        }
    }
    return true;
}