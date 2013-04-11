/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#include "TrackedFrame.h"
#include "vtkObjectFactory.h"
#include "vtkPlusBuffer.h"
#include "vtkPlusDataSource.h"
#include "vtkVirtualDiscCapture.h"
#include "vtksys/SystemTools.hxx"

//----------------------------------------------------------------------------

vtkCxxRevisionMacro(vtkVirtualDiscCapture, "$Revision: 1.0$");
vtkStandardNewMacro(vtkVirtualDiscCapture);

static const int MAX_ALLOWED_RECORDING_LAG_SEC = 3.0; // if the recording lags more than this then it'll skip frames to catch up
static const int DISABLE_FRAME_BUFFER = -1;

//----------------------------------------------------------------------------
vtkVirtualDiscCapture::vtkVirtualDiscCapture()
: vtkPlusDevice()
, m_RecordedFrames(vtkTrackedFrameList::New())
, m_LastAlreadyRecordedFrameTimestamp(UNDEFINED_TIMESTAMP)
, m_NextFrameToBeRecordedTimestamp(0.0)
, m_SamplingFrameRate(8)
, RequestedFrameRate(0.0)
, ActualFrameRate(0.0)
, m_FirstFrameIndexInThisSegment(0.0)
, m_TimeWaited(0.0)
, m_LastUpdateTime(0.0)
, m_Filename("")
, m_OriginalFilename("")
, m_Writer(vtkMetaImageSequenceIO::New())
, m_EnableFileCompression(false)
, m_HeaderPrepared(false)
, TotalFramesRecorded(0)
, EnableCapturing(true)
, FrameBufferSize(DISABLE_FRAME_BUFFER)
, WriterAccessMutex(vtkSmartPointer<vtkRecursiveCriticalSection>::New())
{
  m_RecordedFrames->SetValidationRequirements(REQUIRE_UNIQUE_TIMESTAMP); 

  // The data capture thread will be used to regularly read the frames and write to disk
  this->StartThreadForInternalUpdates = true;
}

//----------------------------------------------------------------------------
vtkVirtualDiscCapture::~vtkVirtualDiscCapture()
{
  if( m_HeaderPrepared )
  {
    this->CloseFile();
  }

  if (m_RecordedFrames != NULL) {
    m_RecordedFrames->Delete();
    m_RecordedFrames = NULL;
  }

  if( m_Writer != NULL )
  {
    m_Writer->Delete();
    m_Writer = NULL;
  }
}

//----------------------------------------------------------------------------
void vtkVirtualDiscCapture::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}

//----------------------------------------------------------------------------
PlusStatus vtkVirtualDiscCapture::ReadConfiguration( vtkXMLDataElement* rootConfig)
{
  if( Superclass::ReadConfiguration(rootConfig) == PLUS_FAIL )
  {
    return PLUS_FAIL;
  }

  vtkXMLDataElement* deviceElement = this->FindThisDeviceElement(rootConfig);
  if (deviceElement == NULL) 
  {
    LOG_ERROR("Cannot find 3dConnexion element in XML tree!");
    return PLUS_FAIL;
  }

  const char * filename = deviceElement->GetAttribute("Filename");
  if( filename != NULL )
  {
    m_Filename = std::string(filename);
    m_OriginalFilename = std::string(filename);
  }

  const char* comp = deviceElement->GetAttribute("EnableFileCompression");
  if( comp != NULL )
  {
    m_EnableFileCompression = STRCASECMP(comp, "true") == 0 ? true : false;
  }

  const char* enableCapturing = deviceElement->GetAttribute("EnableCapturing");
  if( enableCapturing != NULL )
  {
    this->EnableCapturing = STRCASECMP(enableCapturing, "true") == 0 ? true : false;
  }

  double requestedFrameRate;
  if( deviceElement->GetScalarAttribute("RequestedFrameRate", requestedFrameRate) )
  {
    this->SetRequestedFrameRate(requestedFrameRate);
  }
  else
  {
    this->SetRequestedFrameRate(15.0);
  }

  int frameBufferSize;
  if( deviceElement->GetScalarAttribute("FrameBufferSize", frameBufferSize) && frameBufferSize > 0 )
  {
    // This is a buffered disc capture device
    this->SetFrameBufferSize(frameBufferSize);
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkVirtualDiscCapture::WriteConfiguration( vtkXMLDataElement* rootConfig)
{
  if( Superclass::WriteConfiguration(rootConfig) == PLUS_FAIL )
  {
    return PLUS_FAIL;
  }

  vtkXMLDataElement* deviceElement = this->FindThisDeviceElement(rootConfig);
  if (deviceElement == NULL) 
  {
    LOG_ERROR("Cannot find VirtualDiscCapture element in XML tree!");
    return PLUS_FAIL;
  }

  deviceElement->SetAttribute("EnableCapture", this->EnableCapturing ? "TRUE" : "FALSE" );
  deviceElement->SetDoubleAttribute("RequestedFrameRate", this->GetRequestedFrameRate() );

  return PLUS_SUCCESS;
}


//----------------------------------------------------------------------------
PlusStatus vtkVirtualDiscCapture::InternalConnect()
{
  bool lowestRateKnown=false;
  double lowestRate=30; // just a usual value (FPS)
  for( ChannelContainerConstIterator it = this->InputChannels.begin(); it != this->InputChannels.end(); ++it )
  {
    vtkPlusChannel* anInputStream = (*it);
    if( anInputStream->GetOwnerDevice()->GetAcquisitionRate() < lowestRate || !lowestRateKnown)
    {
      lowestRate = anInputStream->GetOwnerDevice()->GetAcquisitionRate();
      lowestRateKnown=true;
    }
  }
  if (lowestRateKnown)
  {
    this->AcquisitionRate = lowestRate;
  }
  else
  {
    LOG_WARNING("vtkVirtualDiscCapture acquisition rate is not known");
  }

  if (OpenFile() != PLUS_SUCCESS)
  {
    return PLUS_FAIL;
  }

  m_LastUpdateTime = vtkAccurateTimer::GetSystemTime();

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkVirtualDiscCapture::InternalDisconnect()
{ 
  this->EnableCapturing = false;

  // If outstanding frames to be written, deal with them
  if( m_RecordedFrames->GetNumberOfTrackedFrames() != 0 && m_HeaderPrepared )
  {
    if( m_Writer->AppendImagesToHeader() != PLUS_SUCCESS )
    {
      LOG_ERROR("Unable to append image data to header.");
      this->Disconnect();
      return PLUS_FAIL;
    }
    if( m_Writer->AppendImages() != PLUS_SUCCESS )
    {
      LOG_ERROR("Unable to append images. Stopping recording at timestamp: " << m_LastAlreadyRecordedFrameTimestamp );
      this->Disconnect();
      return PLUS_FAIL;
    }

    this->ClearRecordedFrames();
  }
  PlusStatus status = CloseFile();
  return status;
}

//----------------------------------------------------------------------------
PlusStatus vtkVirtualDiscCapture::OpenFile()
{
  if ( m_Filename.empty() )
  {
    LOG_ERROR("vtkVirtualDiscCapture: Cannot open file, filename is not specified");
    return PLUS_FAIL;
  }

  PlusLockGuard<vtkRecursiveCriticalSection> writerLock(this->WriterAccessMutex);

  // Because this virtual device continually appends data to the file, we cannot do live compression
  m_Writer->SetUseCompression(false);
  m_Writer->SetTrackedFrameList(m_RecordedFrames);

  // Save config file next to the tracked frame list
  std::string fullPath=vtkPlusConfig::GetInstance()->GetOutputPath(m_Filename);
  m_Writer->SetFileName(fullPath.c_str());

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkVirtualDiscCapture::CloseFile()
{
  // Fix the header to write the correct number of frames
  PlusLockGuard<vtkRecursiveCriticalSection> writerLock(this->WriterAccessMutex);

  if (!m_HeaderPrepared)
  {
    // nothing has been prepared, so nothing to finalize
    return PLUS_SUCCESS;
  }

  std::ostringstream dimSizeStr; 
  int dimensions[3]={0};
  dimensions[0] = m_Writer->GetDimensions()[0];
  dimensions[1] = m_Writer->GetDimensions()[1];
  dimensions[2] = TotalFramesRecorded;
  dimSizeStr << dimensions[0] << " " << dimensions[1] << " " << dimensions[2];
  m_Writer->GetTrackedFrameList()->SetCustomString("DimSize", dimSizeStr.str().c_str());
  m_Writer->UpdateFieldInImageHeader("DimSize");

  m_Writer->FinalizeHeader();

  // Do we have any outstanding unwritten data?
  if( m_RecordedFrames->GetNumberOfTrackedFrames() != 0 )
  {
    this->WriteFrames(true);
  }

  m_Writer->Close();

  std::string fullPath=vtkPlusConfig::GetInstance()->GetOutputPath(m_Filename);
  std::string path = vtksys::SystemTools::GetFilenamePath(fullPath); 
  std::string filename = vtksys::SystemTools::GetFilenameWithoutExtension(fullPath); 
  std::string configFileName = path + "/" + filename + "_config.xml";
  PlusCommon::PrintXML(configFileName.c_str(), vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationData());

  if( m_EnableFileCompression )
  {
    if( this->CompressFile() != PLUS_SUCCESS )
    {
      LOG_ERROR("Unable to compress file.");
      return PLUS_FAIL;
    }
  }

  m_HeaderPrepared = false;
  this->TotalFramesRecorded = 0;
  m_RecordedFrames->Clear();

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------

PlusStatus vtkVirtualDiscCapture::InternalUpdate()
{
  if (!this->EnableCapturing)
  {
    // Capturing is disabled
    return PLUS_SUCCESS;
  }

  if( m_LastUpdateTime == 0.0 )
  {
    m_LastUpdateTime = vtkAccurateTimer::GetSystemTime();
  }
  if( m_NextFrameToBeRecordedTimestamp == 0.0 )
  {
    m_NextFrameToBeRecordedTimestamp = vtkAccurateTimer::GetSystemTime();
  }
  double startTimeSec = vtkAccurateTimer::GetSystemTime();

  m_TimeWaited += startTimeSec - m_LastUpdateTime;

  if( m_TimeWaited < GetSamplingPeriodSec() )
  {
    // Nothing to do yet
    return PLUS_SUCCESS;
  }

  m_TimeWaited = 0.0;

  double maxProcessingTimeSec = GetSamplingPeriodSec() * 2.0; // put a hard limit on the max processing time to make sure the application remains responsive during recording
  double requestedFramePeriodSec = 0.1;
  if (this->RequestedFrameRate > 0)
  {
    requestedFramePeriodSec = 1.0 / this->RequestedFrameRate;
  }
  else
  {
    LOG_WARNING("RequestedFrameRate is invalid");
  }
  
  PlusLockGuard<vtkRecursiveCriticalSection> writerLock(this->WriterAccessMutex);
  if (!this->EnableCapturing)
  {
    // While this thread was waiting for the unlock, capturing was disabled, so cancel the update now
    return PLUS_SUCCESS;
  }

  int nbFramesBefore = m_RecordedFrames->GetNumberOfTrackedFrames();
  if ( this->OutputChannels[0]->GetTrackedFrameListSampled(m_LastAlreadyRecordedFrameTimestamp, m_NextFrameToBeRecordedTimestamp, m_RecordedFrames, requestedFramePeriodSec, maxProcessingTimeSec) != PLUS_SUCCESS )
  {
    LOG_ERROR("Error while getting tracked frame list from data collector during capturing. Last recorded timestamp: " << std::fixed << m_NextFrameToBeRecordedTimestamp ); 
  }
  int nbFramesAfter = m_RecordedFrames->GetNumberOfTrackedFrames();

  if( this->WriteFrames() != PLUS_SUCCESS )
  {
    LOG_ERROR(this->GetDeviceId() << ": Unable to write " << nbFramesAfter - nbFramesBefore << " frames.");
    return PLUS_FAIL;
  }

  this->TotalFramesRecorded += nbFramesAfter - nbFramesBefore;

  // Check whether the recording needed more time than the sampling interval
  double recordingTimeSec = vtkAccurateTimer::GetSystemTime() - startTimeSec;
  if (recordingTimeSec > GetSamplingPeriodSec())
  {
    LOG_WARNING("Recording of frames takes too long time (" << recordingTimeSec << "sec instead of the allocated " << GetSamplingPeriodSec() << "sec). This can cause slow-down of the application and non-uniform sampling. Reduce the acquisition rate or sampling rate to resolve the problem.");
  }
  double recordingLagSec = vtkAccurateTimer::GetSystemTime() - m_NextFrameToBeRecordedTimestamp;

  if (recordingLagSec > MAX_ALLOWED_RECORDING_LAG_SEC)
  {
    LOG_ERROR("Recording cannot keep up with the acquisition. Skip " << recordingLagSec << " seconds of the data stream to catch up.");
    m_NextFrameToBeRecordedTimestamp = vtkAccurateTimer::GetSystemTime();
  }

  m_LastUpdateTime = vtkAccurateTimer::GetSystemTime();

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

PlusStatus vtkVirtualDiscCapture::CompressFile()
{
  vtkSmartPointer<vtkMetaImageSequenceIO> reader = vtkSmartPointer<vtkMetaImageSequenceIO>::New();
  std::string fullPath=vtkPlusConfig::GetInstance()->GetOutputPath(m_Filename);
  reader->SetFileName(fullPath.c_str());

  LOG_DEBUG("Read input sequence metafile: " << fullPath ); 

  if (reader->Read() != PLUS_SUCCESS)
  {    
    LOG_ERROR("Couldn't read sequence metafile: " <<  fullPath ); 
    return PLUS_FAIL;
  }  

  // Now write to disc using compression
  reader->SetUseCompression(true);
  reader->SetFileName(fullPath.c_str());

  if (reader->Write() != PLUS_SUCCESS)
  {    
    LOG_ERROR("Couldn't write sequence metafile: " <<  fullPath ); 
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkVirtualDiscCapture::NotifyConfigured()
{
  if( this->OutputChannels.size() > 0 )
  {
    LOG_WARNING("vtkVirtualDiscCapture is expecting no output channel(s) and there are " << this->OutputChannels.size() << " channels. Output channel information will be dropped.");
    this->OutputChannels.clear();
  }

  if( this->InputChannels.size() == 0 )
  {
    LOG_ERROR("No input channel sent to vtkVirtualDiscCapture. Unable to save anything.");
    return PLUS_FAIL;
  }

  // GetTrackedFrame reads from the OutputChannels
  // For now, place the input stream as an output stream so its data is read
  this->OutputChannels.push_back(this->InputChannels[0]);

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
void vtkVirtualDiscCapture::SetFilename( const char* filename )
{
  m_Filename = std::string(filename);
  std::string fullPath=vtkPlusConfig::GetInstance()->GetOutputPath(m_Filename);
  this->m_Writer->SetFileName(fullPath.c_str());
}

//-----------------------------------------------------------------------------
bool vtkVirtualDiscCapture::HasUnsavedData() const
{
  return m_HeaderPrepared;
}

//-----------------------------------------------------------------------------
PlusStatus vtkVirtualDiscCapture::ClearRecordedFrames()
{
  m_RecordedFrames->Clear();

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
void vtkVirtualDiscCapture::InternalWriteOutputChannels( vtkXMLDataElement* rootXMLElement )
{
  // Do not write anything out, disc capture devices don't have output channels in the config
}

//-----------------------------------------------------------------------------
double vtkVirtualDiscCapture::GetMaximumFrameRate()
{
  LOG_TRACE("vtkVirtualDiscCapture::GetMaximumFrameRate");

  return this->GetAcquisitionRate();
}

//-----------------------------------------------------------------------------
double vtkVirtualDiscCapture::GetSamplingPeriodSec()
{
  double samplingPeriodSec = 0.1;
  if (m_SamplingFrameRate > 0)
  {
    samplingPeriodSec = 1.0 / m_SamplingFrameRate;
  }
  else
  {
    LOG_WARNING("m_SamplingFrameRate value is invalid " << m_SamplingFrameRate << ". Use default sampling period of " << samplingPeriodSec << " sec");
  }
  return samplingPeriodSec;
}

//-----------------------------------------------------------------------------
void vtkVirtualDiscCapture::SetEnableCapturing( bool aValue )
{
  this->EnableCapturing = aValue;

  if( aValue )
  {
    m_LastUpdateTime = 0.0;
    m_TimeWaited = 0.0;
    m_LastAlreadyRecordedFrameTimestamp = UNDEFINED_TIMESTAMP;
    m_NextFrameToBeRecordedTimestamp = 0.0;
    m_FirstFrameIndexInThisSegment = 0.0;
  }
}

//-----------------------------------------------------------------------------
void vtkVirtualDiscCapture::SetRequestedFrameRate( double aValue )
{
  LOG_TRACE("vtkVirtualDiscCapture::SetRequestedFrameRate(" << aValue << ")"); 

  double maxFrameRate = this->GetMaximumFrameRate();

  if( aValue > maxFrameRate )
  {
    aValue = maxFrameRate;
  }
  RequestedFrameRate = aValue;

  LOG_INFO("Requested frame rate changed to " << RequestedFrameRate );
}

//-----------------------------------------------------------------------------
double vtkVirtualDiscCapture::GetAcquisitionRate() const
{
  if( this->InputChannels.size() <= 0 )
  {
    return VIRTUAL_DEVICE_FRAME_RATE;
  }
  return this->InputChannels[0]->GetOwnerDevice()->GetAcquisitionRate();
}

//-----------------------------------------------------------------------------
PlusStatus vtkVirtualDiscCapture::Reset()
{
  {
    PlusLockGuard<vtkRecursiveCriticalSection> writerLock(this->WriterAccessMutex);

    this->SetEnableCapturing(false);

    if( m_HeaderPrepared )
    {
      // Change the filename to a temporary filename
      std::string tempFilename;
      if( PlusCommon::CreateTemporaryFilename(tempFilename, "") != PLUS_SUCCESS )
      {
        LOG_ERROR("Unable to create temporary file. Check write access.");
      }
      else
      {
        // Risky, file with extension ".mha" might exist... no way to use windows utility to change extension
        // In reality, probably will never be an issue
        std::string mhaFilename(tempFilename);
        mhaFilename.replace(mhaFilename.end()-3, mhaFilename.end(), "mha");
        this->m_Writer->SetFileName(mhaFilename.c_str());

        this->m_Writer->Close();

        vtksys::SystemTools::RemoveFile(tempFilename.c_str());
        vtksys::SystemTools::RemoveFile(mhaFilename.c_str());
      }
    }

    this->ClearRecordedFrames();
    this->m_Writer->GetTrackedFrameList()->Clear();
    m_HeaderPrepared = false;
    TotalFramesRecorded = 0;
  }

  this->m_Filename = this->m_OriginalFilename;
  if( this->OpenFile() != PLUS_SUCCESS )
  {
    LOG_ERROR("Unable to reset device " << this->GetDeviceId() << ".");
    return PLUS_FAIL;
  }

  m_LastUpdateTime = vtkAccurateTimer::GetSystemTime();

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
bool vtkVirtualDiscCapture::IsFrameBuffered() const
{
  return this->FrameBufferSize != DISABLE_FRAME_BUFFER;
}

//-----------------------------------------------------------------------------
PlusStatus vtkVirtualDiscCapture::TakeSnapshot()
{
  if( this->EnableCapturing )
  {
    LOG_ERROR(this->GetDeviceId() << ": Cannot take snapshot while the device is recording.");
    return PLUS_FAIL;
  }

  TrackedFrame trackedFrame;
  if( this->OutputChannels[0]->GetTrackedFrame(&trackedFrame) != PLUS_SUCCESS )
  {
    LOG_ERROR(this->GetDeviceId() << ": Failed to get tracked frame for the snapshot!");
    return PLUS_FAIL;
  }

  // Check if there are any valid transforms
  std::vector<PlusTransformName> transformNames;
  trackedFrame.GetCustomFrameTransformNameList(transformNames);
  bool validFrame = false;

  if (transformNames.size() == 0)
  {
    validFrame = true;
  }
  else
  {
    for (std::vector<PlusTransformName>::iterator it = transformNames.begin(); it != transformNames.end(); ++it)
    {
      TrackedFrameFieldStatus status = FIELD_INVALID;
      trackedFrame.GetCustomFrameTransformStatus(*it, status);

      if ( status == FIELD_OK )
      {
        validFrame = true;
        break;
      }
    }
  }

  if ( !validFrame )
  {
    LOG_WARNING(this->GetDeviceId() << ": Unable to record tracked frame: All the tool transforms are invalid!"); 
    return PLUS_FAIL;
  }

  // Add tracked frame to the list
  if (m_RecordedFrames->AddTrackedFrame(&trackedFrame, vtkTrackedFrameList::SKIP_INVALID_FRAME) != PLUS_SUCCESS)
  {
    LOG_WARNING(this->GetDeviceId() << ": Frame could not be added because validation failed!");
    return PLUS_FAIL;
  }
  
  if( this->WriteFrames() != PLUS_SUCCESS )
  {
    LOG_ERROR(this->GetDeviceId() << ": Unable to write frames while taking a snapshot.");
    return PLUS_FAIL;
  }

  this->TotalFramesRecorded += 1;

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkVirtualDiscCapture::WriteFrames(bool force)
{
  if( !m_HeaderPrepared && m_RecordedFrames->GetNumberOfTrackedFrames() != 0 )
  {
    if( m_Writer->PrepareHeader() != PLUS_SUCCESS )
    {
      LOG_ERROR("Unable to prepare header.");
      this->Disconnect();
      return PLUS_FAIL;
    }
    m_HeaderPrepared = true;
  }

  // Compute the average frame rate from the ratio of recently acquired frames
  int frame1Index = m_RecordedFrames->GetNumberOfTrackedFrames() - 1; // index of the latest frame
  int frame2Index = frame1Index - this->RequestedFrameRate * 5.0 - 1; // index of an earlier acquired frame (go back by approximately 5 seconds + one frame)
  if (frame2Index < m_FirstFrameIndexInThisSegment)
  {
    // make sure we stay in the current recording segment
    frame2Index = m_FirstFrameIndexInThisSegment;
  }
  if (frame1Index > frame2Index)
  {   
    TrackedFrame* frame1 = m_RecordedFrames->GetTrackedFrame(frame1Index);
    TrackedFrame* frame2 = m_RecordedFrames->GetTrackedFrame(frame2Index);
    if (frame1 != NULL && frame2 != NULL)
    {
      double frameTimeDiff = frame1->GetTimestamp() - frame2->GetTimestamp();
      if (frameTimeDiff > 0)
      {
        this->ActualFrameRate = (frame1Index - frame2Index) / frameTimeDiff;
      }
      else
      {
        this->ActualFrameRate = 0;
      }
    }    
  }

  if( m_RecordedFrames->GetNumberOfTrackedFrames() == 0 )
  {
    return PLUS_SUCCESS;
  }

  if( force || !this->IsFrameBuffered() || 
    ( this->IsFrameBuffered() && m_RecordedFrames->GetNumberOfTrackedFrames() > this->GetFrameBufferSize() ) )
  {
    if( m_Writer->AppendImagesToHeader() != PLUS_SUCCESS )
    {
      LOG_ERROR("Unable to append image data to header.");
      this->Disconnect();
      return PLUS_FAIL;
    }
    if( m_Writer->AppendImages() != PLUS_SUCCESS )
    {
      LOG_ERROR("Unable to append images. Stopping recording at timestamp: " << m_LastAlreadyRecordedFrameTimestamp );
      this->Disconnect();
      return PLUS_FAIL;
    }

    this->ClearRecordedFrames();
  }

  return PLUS_SUCCESS;
}
