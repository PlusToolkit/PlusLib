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

//----------------------------------------------------------------------------
vtkVirtualDiscCapture::vtkVirtualDiscCapture()
: vtkPlusDevice()
, m_RecordedFrames(vtkTrackedFrameList::New())
, m_RecordingLastAlreadyRecordedFrameTimestamp(UNDEFINED_TIMESTAMP)
, m_RecordingNextFrameToBeRecordedTimestamp(0.0)
, m_SamplingFrameRate(8)
, RequestedFrameRate(0.0)
, ActualFrameRate(0.0)
, m_RecordingFirstFrameIndexInThisSegment(0.0)
, m_TimeWaited(0.0)
, m_LastUpdateTime(0.0)
, m_Filename("")
, m_Writer(vtkMetaImageSequenceIO::New())
, m_EnableFileCompression(false)
, m_HeaderPrepared(false)
, m_TotalFramesRecorded(0)
, EnableCapturing(true)
, WriterAccessMutex(vtkSmartPointer<vtkRecursiveCriticalSection>::New())
{
  m_RecordedFrames->SetValidationRequirements(REQUIRE_UNIQUE_TIMESTAMP); 
  this->AcquisitionRate = vtkPlusDevice::VIRTUAL_DEVICE_FRAME_RATE;

  // The data capture thread will be used to regularly read the frames and write to disk
  this->StartThreadForInternalUpdates = true;
}

//----------------------------------------------------------------------------
vtkVirtualDiscCapture::~vtkVirtualDiscCapture()
{
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
  this->EnableCapturing=false;
  PlusStatus status=CloseFile();
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
  std::string path = vtksys::SystemTools::GetFilenamePath(m_Filename); 
  if (!path.empty())
  {
    path += "/";
  }
  else
  {
    path = std::string(vtkPlusConfig::GetInstance()->GetOutputDirectory()) + "/";
    std::stringstream ss;
    ss << path << m_Filename;
    m_Filename = ss.str();
  }

  m_Writer->SetFileName(m_Filename.c_str());

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
  dimensions[2] = m_TotalFramesRecorded;
  dimSizeStr << dimensions[0] << " " << dimensions[1] << " " << dimensions[2];
  m_Writer->GetTrackedFrameList()->SetCustomString("DimSize", dimSizeStr.str().c_str());
  m_Writer->UpdateFieldInImageHeader("DimSize");

  m_Writer->FinalizeHeader();

  m_Writer->Close();

  m_Writer->SetTrackedFrameList(NULL);

  std::string path = vtksys::SystemTools::GetFilenamePath(m_Filename); 
  std::string filename = vtksys::SystemTools::GetFilenameWithoutExtension(m_Filename); 
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
  double startTimeSec = vtkAccurateTimer::GetSystemTime();

  m_TimeWaited += startTimeSec - m_LastUpdateTime;

  if( m_TimeWaited < GetSamplingPeriodSec() )
  {
    // Nothing to do yet
    return PLUS_SUCCESS;
  }

  LOG_INFO("time waited: " << m_TimeWaited );
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
  int nbFramesBefore = m_RecordedFrames->GetNumberOfTrackedFrames();
  if ( this->OutputChannels[0]->GetTrackedFrameListSampled(m_RecordingLastAlreadyRecordedFrameTimestamp, m_RecordingNextFrameToBeRecordedTimestamp, m_RecordedFrames, requestedFramePeriodSec, maxProcessingTimeSec) != PLUS_SUCCESS )
  {
    LOG_ERROR("Error while getting tracked frame list from data collector during capturing. Last recorded timestamp: " << std::fixed << m_RecordingNextFrameToBeRecordedTimestamp ); 
  }
  int nbFramesAfter = m_RecordedFrames->GetNumberOfTrackedFrames();

  if (!this->EnableCapturing)
  {
    // While this thread was working on getting the frames, capturing was disabled, so cancel the update now
    return PLUS_SUCCESS;
  }

  // Compute the average frame rate from the ratio of recently acquired frames
  int frame1Index = m_RecordedFrames->GetNumberOfTrackedFrames() - 1; // index of the latest frame
  int frame2Index = frame1Index - this->RequestedFrameRate * 5.0 - 1; // index of an earlier acquired frame (go back by approximately 5 seconds + one frame)
  if (frame2Index < m_RecordingFirstFrameIndexInThisSegment)
  {
    // make sure we stay in the current recording segment
    frame2Index = m_RecordingFirstFrameIndexInThisSegment;
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

  PlusLockGuard<vtkRecursiveCriticalSection> writerLock(this->WriterAccessMutex);
  if (!this->EnableCapturing)
  {
    // While this thread was waiting for the unlock, capturing was disabled, so cancel the update now
    return PLUS_SUCCESS;
  }

  if( !m_HeaderPrepared )
  {
    if( m_Writer->PrepareHeader() != PLUS_SUCCESS )
    {
      LOG_ERROR("Unable to prepare header.");
      this->Disconnect();
      return PLUS_FAIL;
    }
    m_HeaderPrepared = true;
  }
  
  if( m_Writer->AppendImagesToHeader() != PLUS_SUCCESS )
  {
    LOG_ERROR("Unable to append image data to header.");
    this->Disconnect();
    return PLUS_FAIL;
  }
  if( m_Writer->AppendImages() != PLUS_SUCCESS )
  {
    LOG_ERROR("Unable to append images. Stopping recording at timestamp: " << m_RecordingLastAlreadyRecordedFrameTimestamp );
    this->Disconnect();
    return PLUS_FAIL;
  }

  m_TotalFramesRecorded += m_RecordedFrames->GetNumberOfTrackedFrames();
  this->ClearRecordedFrames();

  // Check whether the recording needed more time than the sampling interval
  double recordingTimeSec = vtkAccurateTimer::GetSystemTime() - startTimeSec;
  if (recordingTimeSec > GetSamplingPeriodSec())
  {
    LOG_WARNING("Recording of frames takes too long time (" << recordingTimeSec << "sec instead of the allocated " << GetSamplingPeriodSec() << "sec). This can cause slow-down of the application and non-uniform sampling. Reduce the acquisition rate or sampling rate to resolve the problem.");
  }
  double recordingLagSec = vtkAccurateTimer::GetSystemTime() - m_RecordingNextFrameToBeRecordedTimestamp;

  if (recordingLagSec > MAX_ALLOWED_RECORDING_LAG_SEC)
  {
    LOG_ERROR("Recording cannot keep up with the acquisition. Skip " << recordingLagSec << " seconds of the data stream to catch up.");
    m_RecordingNextFrameToBeRecordedTimestamp = vtkAccurateTimer::GetSystemTime();
  }

  m_LastUpdateTime = vtkAccurateTimer::GetSystemTime();

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

PlusStatus vtkVirtualDiscCapture::CompressFile()
{
  vtkSmartPointer<vtkMetaImageSequenceIO> reader = vtkSmartPointer<vtkMetaImageSequenceIO>::New();
  reader->SetFileName(m_Filename.c_str());

  LOG_DEBUG("Read input sequence metafile: " << m_Filename ); 

  if (reader->Read() != PLUS_SUCCESS)
  {    
    LOG_ERROR("Couldn't read sequence metafile: " <<  m_Filename ); 
    return PLUS_FAIL;
  }  

  // Now write to disc using compression
  reader->SetUseCompression(true);
  reader->SetFileName(m_Filename.c_str());

  if (reader->Write() != PLUS_SUCCESS)
  {    
    LOG_ERROR("Couldn't write sequence metafile: " <<  reader->GetFileName() ); 
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
  this->m_Writer->SetFileName(m_Filename.c_str());
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
