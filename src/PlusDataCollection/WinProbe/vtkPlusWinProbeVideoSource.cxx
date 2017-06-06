/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"
#include "vtkPlusWinProbeVideoSource.h"
#include "vtkPlusChannel.h"
#include "vtkPlusDataSource.h"
#include "vtkPlusUSImagingParameters.h"

#include "TestSessionWrapper.h"
#include "UltraVisionManagedDll.h"

#include <algorithm>
#include <PlusMath.h>

vtkStandardNewMacro(vtkPlusWinProbeVideoSource);

// ----------------------------------------------------------------------------
void vtkPlusWinProbeVideoSource::PrintSelf(ostream& os, vtkIndent indent)
{
    this->Superclass::PrintSelf(os, indent);
    os << indent << "TransducerID: " << this->m_transducerID << std::endl;
    os << indent << "Frozen: " << this->IsFrozen() << std::endl;
    os << indent << "Voltage: " << static_cast<unsigned>(this->GetVoltage()) << std::endl;
    os << indent << "Frequency: " << this->GetTxTxFrequency() << std::endl;
    os << indent << "Depth: " << this->GetSSDepth() << std::endl;

    os << indent << "CustomFields: " << std::endl;
    vtkIndent indent2 = indent.GetNextIndent();
    PlusTrackedFrame::FieldMapType::iterator it;
    for (it = m_customFields.begin(); it != m_customFields.end(); ++it)
    {
        os << indent2 << it->first << ": " << it->second << std::endl;
    }

}

// ----------------------------------------------------------------------------
PlusStatus vtkPlusWinProbeVideoSource::ReadConfiguration(vtkXMLDataElement* rootConfigElement)
{
    LOG_TRACE("vtkPlusWinProbeVideoSource::ReadConfiguration");
    XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_READING(deviceConfig, rootConfigElement);

    XML_READ_STRING_ATTRIBUTE_REQUIRED(TransducerID, deviceConfig);
    XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(float, TxTxFrequency, deviceConfig);
    XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(float, SSDepth, deviceConfig);
    //XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(uint8_t, Voltage, deviceConfig);
    //above macro is not defined for uint8_t, so we implement it manually below:
    unsigned long tmpValue = 0;
    if (deviceConfig->GetScalarAttribute("Voltage", tmpValue))
    {
        this->SetVoltage(tmpValue);
    }

    return PLUS_SUCCESS;
}

// ----------------------------------------------------------------------------
PlusStatus vtkPlusWinProbeVideoSource::WriteConfiguration(vtkXMLDataElement* rootConfigElement)
{
    XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_WRITING(deviceConfig, rootConfigElement);

    deviceConfig->SetAttribute("TransducerID", this->m_transducerID.c_str());
    deviceConfig->SetFloatAttribute("TxTxFrequency", this->GetTxTxFrequency());
    deviceConfig->SetFloatAttribute("SSDepth", this->GetSSDepth());
    deviceConfig->SetUnsignedLongAttribute("Voltage", this->GetVoltage());

    return PLUS_SUCCESS;
}

// ----------------------------------------------------------------------------
vtkPlusWinProbeVideoSource *thisPtr = NULL;

//this callback function is invoked after each frame is ready
int __stdcall frameCallback(int length, char * ptr)
{
    thisPtr->FrameCallback(length, ptr);
    return length;
}

// ----------------------------------------------------------------------------
void vtkPlusWinProbeVideoSource::FrameCallback(int length, char * ptr)
{
    vtkPlusDataSource* aSource = NULL;
    if (this->GetFirstActiveOutputVideoSource(aSource) != PLUS_SUCCESS)
    {
        LOG_ERROR("Unable to retrieve the video source in the capturing device.");
        return;
    }

    //Header:
    //3 bytes of counter
    //1 byte of spatial compounding angle number
    //3 bytes of counter
    //1 byte of spatial compounding angle number
    //4 bytes of time stamp
    //4 bytes of timestamp
    //timestamp counters are ticks since Execute on sampling frequency GetADCSamplingRate

    uint32_t* timeStampCounter = reinterpret_cast<uint32_t*>(ptr + 8);
    double timestamp = *timeStampCounter / m_ADCfrequency;
    if (*timeStampCounter > wraparoundTSC)
    {
        m_wrapTimeStampCounter = true;
    }
    else if (m_wrapTimeStampCounter) //time to wrap it around
    {
        m_timestampOffset = vtkPlusAccurateTimer::GetSystemTime() - timestamp;
        m_wrapTimeStampCounter = false;
        LOG_DEBUG("Wrapping around time-stamp counter. Leftover fraction: " << timestamp);
    }
    m_lastTimestamp = timestamp + m_timestampOffset;
    LOG_DEBUG("Frame: " << FrameNumber << ". Timestamp: " << m_lastTimestamp);

    assert(length = m_samplesPerLine*m_transducerCount*sizeof(uint16_t) + 256); //frame + header and footer
    uint16_t * frame = reinterpret_cast<uint16_t *>(ptr + 16);
    const float logFactor = 22.992952214167854304798799603468f; // =255/ln(2^16)
    uint8_t * bModeBuffer=new uint8_t[m_samplesPerLine*m_transducerCount];

#pragma omp parallel for
    for (int t = 0; t < m_transducerCount; t++)
    {
        for (int s = 0; s < m_samplesPerLine; s++)
        {
            bModeBuffer[s*m_transducerCount + t] = static_cast<uint8_t>
                (logFactor*std::log(float(1 + frame[t*m_samplesPerLine + s])));
        }
    }


    int frameSize[3] = { m_transducerCount,m_samplesPerLine,1 };

    if (aSource->AddItem(bModeBuffer,
        aSource->GetInputImageOrientation(),
        frameSize, VTK_UNSIGNED_CHAR,
        1, US_IMG_BRIGHTNESS, 0,
        this->FrameNumber,
        m_lastTimestamp,
        m_lastTimestamp, //no timestamp filtering needed
        &this->m_customFields) != PLUS_SUCCESS)
    {
        LOG_WARNING("Error adding item to video source " << aSource->GetSourceId());
    }

    delete bModeBuffer;
    this->FrameNumber++;
    this->Modified();
}

void vtkPlusWinProbeVideoSource::AdjustBufferSize()
{
    vtkPlusDataSource* aSource = NULL;
    if (this->GetFirstActiveOutputVideoSource(aSource) != PLUS_SUCCESS)
    {
        LOG_ERROR("Unable to retrieve the video source in the capturing device.");
        return;
    }

    int frameSize[3] = { m_transducerCount,m_samplesPerLine,1 };

    LOG_DEBUG("Set up image buffer for WinProbe");
    aSource->SetPixelType(VTK_UNSIGNED_CHAR);
    aSource->SetImageType(US_IMG_BRIGHTNESS);
    aSource->SetInputFrameSize(frameSize);

    LOG_INFO("Frame size: " << frameSize[0] << "x" << frameSize[1]
        << ", pixel type: " << vtkImageScalarTypeNameMacro(aSource->GetPixelType())
        << ", buffer image orientation: "
        << PlusVideoFrame::GetStringFromUsImageOrientation(aSource->GetInputImageOrientation()));
}

void vtkPlusWinProbeVideoSource::AdjustSpacing()
{
    this->CurrentPixelSpacingMm[0] = this->GetTransducerWidth() / (m_transducerCount - 1);
    this->CurrentPixelSpacingMm[1] = m_depth / (m_samplesPerLine - 1);
    this->CurrentPixelSpacingMm[2] = 1.0;

    this->m_customFields.clear();
    std::ostringstream spacingStream;
    unsigned int numSpaceDimensions = 3;
    for (unsigned int i = 0; i < numSpaceDimensions; ++i)
    {
        spacingStream << this->CurrentPixelSpacingMm[i];
        if (i != numSpaceDimensions - 1)
        {
            spacingStream << " ";
        }
    }
    this->m_customFields["ElementSpacing"] = spacingStream.str();
    LOG_DEBUG("Adjusted spacing: " << spacingStream.str());
}

// ----------------------------------------------------------------------------
vtkPlusWinProbeVideoSource::vtkPlusWinProbeVideoSource()
{
    this->RequireImageOrientationInConfiguration = true;

    AdjustSpacing();
    //AdjustBufferSize();

    Callback funcPtr = &frameCallback;
    thisPtr = this;
    WPSetCallback(funcPtr);
    WPInitialize();
}

// ----------------------------------------------------------------------------
vtkPlusWinProbeVideoSource::~vtkPlusWinProbeVideoSource()
{
    if (this->Connected)
    {
        this->Disconnect();
    }
}

// ----------------------------------------------------------------------------
PlusStatus vtkPlusWinProbeVideoSource::InternalConnect()
{
    LOG_DEBUG("Connect to WinProbe");
    WPConnect();
    WPLoadDefault();
    WPSetTransducerID(this->m_transducerID.c_str());
    m_ADCfrequency = GetADCSamplingRate();
    m_transducerCount = GetSSElementCount();
    SetCompoundAngleCount(0);
    SetPendingRecreateTables(true);

    return PLUS_SUCCESS;
}

// ----------------------------------------------------------------------------
PlusStatus vtkPlusWinProbeVideoSource::InternalDisconnect()
{
    LOG_DEBUG("Disconnect from WinProbe");
    if (IsRecording())
    {
        this->InternalStopRecording();
    }
    WPDisconnect();
    return PLUS_SUCCESS;
}

// ----------------------------------------------------------------------------
void vtkPlusWinProbeVideoSource::Watchdog()
{
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    while (this->Recording)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(15));
        double now = vtkPlusAccurateTimer::GetSystemTime();
        if (now - m_lastTimestamp > 1.0)
        {
            SetPendingRecreateTables(true);
            LOG_INFO("Called SetPendingRecreateTables");
            m_timestampOffset = vtkPlusAccurateTimer::GetSystemTime();
            std::this_thread::sleep_for(std::chrono::milliseconds(3000));
        }
    }
}

// ----------------------------------------------------------------------------
PlusStatus vtkPlusWinProbeVideoSource::InternalStartRecording()
{
    //apply requested settings
    this->SetTxTxFrequency(m_frequency);
    this->SetVoltage(m_voltage);
    this->SetSSDepth(m_depth); //as a side-effect calls AdjustSpacing and AdjustBufferSize

    m_timestampOffset = vtkPlusAccurateTimer::GetSystemTime();
    WPExecute();
    if (sizeof(void *) == 4) //32 bits
    {
        m_watchdog32 = new std::thread(&vtkPlusWinProbeVideoSource::Watchdog, this);
    }
    return PLUS_SUCCESS;
}

// ----------------------------------------------------------------------------
PlusStatus vtkPlusWinProbeVideoSource::InternalStopRecording()
{
    WPStopScanning();
    if (sizeof(void *) == 4) //32 bits
    {
        m_watchdog32->join();
        delete m_watchdog32;
    }

    return PLUS_SUCCESS;
}

// ----------------------------------------------------------------------------
PlusStatus vtkPlusWinProbeVideoSource::FreezeDevice(bool freeze)
{

    if (!IsRecording() == freeze) //already in desired mode
    {
        return PLUS_SUCCESS;
    }

    if (IsRecording())
    {
        this->InternalStopRecording();
    }
    else
    {
        this->InternalStartRecording();
    }

    return PLUS_SUCCESS;
}

// ----------------------------------------------------------------------------
bool vtkPlusWinProbeVideoSource::IsFrozen()
{
    return !IsRecording();
}


// ----------------------------------------------------------------------------
PlusStatus vtkPlusWinProbeVideoSource::SetTxTxFrequency(float frequency)
{
    m_frequency = frequency;
    if (Connected)
    {
        ::SetTxTxFrequency(frequency);
        SetPendingRecreateTables(true);
        //what we requested might be only approximately satisfied
        m_frequency = ::GetTxTxFrequency();
    }
    return PLUS_SUCCESS;
}

float vtkPlusWinProbeVideoSource::GetTxTxFrequency()
{
    if (Connected)
    {
        m_frequency = ::GetTxTxFrequency();
    }
    return m_frequency;
}

PlusStatus vtkPlusWinProbeVideoSource::SetVoltage(uint8_t voltage)
{
    m_voltage = voltage;
    if (Connected)
    {
        ::SetVoltage(voltage);
        SetPendingRecreateTables(true);
        //what we requested might be only approximately satisfied
        m_voltage = ::GetVoltage();
    }
    return PLUS_SUCCESS;
}

uint8_t vtkPlusWinProbeVideoSource::GetVoltage()
{
    if (Connected)
    {
        m_voltage = ::GetVoltage();
    }
    return m_voltage;
}

PlusStatus vtkPlusWinProbeVideoSource::SetSSDepth(float depth)
{
    m_depth = depth;
    if (Connected)
    {
        ::SetSSDepth(depth);
        SetPendingRecreateTables(true);
        //what we requested might be only approximately satisfied
        m_depth = ::GetSSDepth();
        m_samplesPerLine = GetSSSamplesPerLine(); //this and decimation change depending on depth
        AdjustSpacing();
        AdjustBufferSize();
    }
    return PLUS_SUCCESS;
}

float vtkPlusWinProbeVideoSource::GetSSDepth()
{
    if (Connected)
    {
        m_depth = ::GetSSDepth();
    }
    return m_depth;
}

float vtkPlusWinProbeVideoSource::GetTransducerWidth()
{
    if (Connected)
    {
        m_width = ::GetTWidth();
    }
    return m_width;
}

const double * vtkPlusWinProbeVideoSource::GetCurrentPixelSpacingMm()
{
    return this->CurrentPixelSpacingMm;
}

PlusStatus vtkPlusWinProbeVideoSource::SetTransducerID(std::string guid)
{
    this->m_transducerID = guid;
    if (Connected)
    {
        WPSetTransducerID(guid.c_str());
        SetPendingRecreateTables(true);
    }
    return PLUS_SUCCESS;
}

std::string vtkPlusWinProbeVideoSource::GetTransducerID()
{
    return this->m_transducerID;
}
