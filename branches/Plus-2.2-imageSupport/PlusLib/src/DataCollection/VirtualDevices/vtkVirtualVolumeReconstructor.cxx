/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#include "vtkVirtualVolumeReconstructor.h"
#include "TrackedFrame.h"
#include "vtkObjectFactory.h"
#include "vtkPlusBuffer.h"
#include "vtkPlusDataSource.h"
#include "vtkVolumeReconstructor.h"
#include "vtkTransformRepository.h"

#include "vtksys/SystemTools.hxx"

//----------------------------------------------------------------------------

vtkCxxRevisionMacro(vtkVirtualVolumeReconstructor, "$Revision: 1.0$");
vtkStandardNewMacro(vtkVirtualVolumeReconstructor);

static const int MAX_ALLOWED_RECONSTRUCTION_LAG_SEC = 3.0; // if the reconstruction lags more than this then it'll skip frames to catch up
static const int DISABLE_FRAME_BUFFER = -1;

static const int MAX_NUMBER_OF_FRAMES_ADDED_PER_UPDATE=30;

//----------------------------------------------------------------------------
vtkVirtualVolumeReconstructor::vtkVirtualVolumeReconstructor()
: vtkPlusDevice()
, m_LastAlreadyRecordedFrameTimestamp(UNDEFINED_TIMESTAMP)
, m_NextFrameToBeRecordedTimestamp(0.0)
, m_SamplingFrameRate(8)
, RequestedFrameRate(0.0)
, ActualFrameRate(0.0)
, m_FirstFrameIndexInThisSegment(0.0)
, m_TimeWaited(0.0)
, m_LastUpdateTime(0.0)
, TotalFramesRecorded(0)
, EnableReconstruction(false)
, VolumeReconstructorAccessMutex(vtkSmartPointer<vtkRecursiveCriticalSection>::New())
, OutputVolFilename(NULL)
, OutputVolDeviceName(NULL)
{
  // The data capture thread will be used to regularly read the frames and write to disk
  this->StartThreadForInternalUpdates = true;

  this->VolumeReconstructor=vtkSmartPointer<vtkVolumeReconstructor>::New();
  this->TransformRepository=vtkSmartPointer<vtkTransformRepository>::New();
}

//----------------------------------------------------------------------------
vtkVirtualVolumeReconstructor::~vtkVirtualVolumeReconstructor()
{
  SetOutputVolFilename(NULL);
  SetOutputVolDeviceName(NULL);
}

//----------------------------------------------------------------------------
void vtkVirtualVolumeReconstructor::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}

//----------------------------------------------------------------------------
PlusStatus vtkVirtualVolumeReconstructor::ReadConfiguration( vtkXMLDataElement* rootConfig)
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

  const char* EnableReconstruction = deviceElement->GetAttribute("EnableReconstruction");
  if( EnableReconstruction != NULL )
  {
    this->EnableReconstruction = STRCASECMP(EnableReconstruction, "true") == 0 ? true : false;
  }

  this->SetOutputVolFilename(deviceElement->GetAttribute("OutputVolFilename"));
  this->SetOutputVolDeviceName(deviceElement->GetAttribute("OutputVolDeviceName"));

  PlusLockGuard<vtkRecursiveCriticalSection> writerLock(this->VolumeReconstructorAccessMutex);
  this->VolumeReconstructor->ReadConfiguration(deviceElement);

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkVirtualVolumeReconstructor::WriteConfiguration( vtkXMLDataElement* rootConfig)
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

  deviceElement->SetAttribute("EnableReconstruction", this->EnableReconstruction ? "TRUE" : "FALSE" );

  deviceElement->SetAttribute("OutputVolFilename",this->OutputVolFilename);
  deviceElement->SetAttribute("OutputVolDeviceName",this->OutputVolDeviceName);

  PlusLockGuard<vtkRecursiveCriticalSection> writerLock(this->VolumeReconstructorAccessMutex);
  this->VolumeReconstructor->WriteConfiguration(deviceElement->FindNestedElementWithName("VolumeReconstruction"));

  return PLUS_SUCCESS;
}


//----------------------------------------------------------------------------
PlusStatus vtkVirtualVolumeReconstructor::InternalConnect()
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
    LOG_WARNING("vtkVirtualVolumeReconstructor acquisition rate is not known");
  }

  m_LastUpdateTime = vtkAccurateTimer::GetSystemTime();

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkVirtualVolumeReconstructor::InternalDisconnect()
{ 
  SetEnableReconstruction(false);
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkVirtualVolumeReconstructor::InternalUpdate()
{
  if (!this->EnableReconstruction)
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

  double maxProcessingTimeSec = GetSamplingPeriodSec() * 2.0; // put a hard limit on the max processing time to make sure the application remains responsive during reconstruction
  double requestedFramePeriodSec = 0.1;
  double requestedFrameRate = this->RequestedFrameRate;
  if (requestedFrameRate <= 0)
  {
    // no frame rate is specified, so use the same as the input's
    requestedFrameRate=this->AcquisitionRate;
  }
  if (requestedFrameRate > 0)
  {
    requestedFramePeriodSec = 1.0 / requestedFrameRate;
  }
  else
  {
    LOG_WARNING("RequestedFrameRate is invalid, use default: "<<1/requestedFramePeriodSec);
  }
  
  PlusLockGuard<vtkRecursiveCriticalSection> writerLock(this->VolumeReconstructorAccessMutex);
  if (!this->EnableReconstruction)
  {
    // While this thread was waiting for the unlock, capturing was disabled, so cancel the update now
    return PLUS_SUCCESS;
  }

  if( this->OutputChannels.empty() )
  {
    LOG_ERROR("No output channels defined" );
    return PLUS_FAIL;
  }
  vtkPlusChannel* outputChannel=this->OutputChannels[0];

  vtkSmartPointer<vtkTrackedFrameList> recordedFrames=vtkSmartPointer<vtkTrackedFrameList>::New();
  if ( outputChannel->GetTrackedFrameListSampled(m_LastAlreadyRecordedFrameTimestamp, m_NextFrameToBeRecordedTimestamp, recordedFrames, requestedFramePeriodSec, maxProcessingTimeSec) != PLUS_SUCCESS )
  {
    LOG_ERROR("Error while getting tracked frame list from data collector during volume reconstruction. Last recorded timestamp: " << std::fixed << m_NextFrameToBeRecordedTimestamp ); 
  }
  int nbFramesRecorded = recordedFrames->GetNumberOfTrackedFrames();

  if( this->AddFrames(recordedFrames) != PLUS_SUCCESS )
  {
    LOG_ERROR(this->GetDeviceId() << ": Unable to add " << recordedFrames->GetNumberOfTrackedFrames() << " frames for volume reconstruction");
    return PLUS_FAIL;
  }

  this->TotalFramesRecorded += recordedFrames->GetNumberOfTrackedFrames();

  // Check whether the reconstruction needed more time than the sampling interval
  double recordingTimeSec = vtkAccurateTimer::GetSystemTime() - startTimeSec;
  if (recordingTimeSec > GetSamplingPeriodSec())
  {
    LOG_WARNING("Volume reconstruction takes too long time (" << recordingTimeSec << "sec instead of the allocated " << GetSamplingPeriodSec() << "sec). This can cause slow-down of the application and non-uniform sampling. Reduce the acquisition rate or sampling rate to resolve the problem.");
  }
  double recordingLagSec = vtkAccurateTimer::GetSystemTime() - m_NextFrameToBeRecordedTimestamp;

  if (recordingLagSec > MAX_ALLOWED_RECONSTRUCTION_LAG_SEC)
  {
    LOG_ERROR("Volume reconstruction cannot keep up with the acquisition. Skip " << recordingLagSec << " seconds of the data stream to catch up.");
    m_NextFrameToBeRecordedTimestamp = vtkAccurateTimer::GetSystemTime();
  }

  m_LastUpdateTime = vtkAccurateTimer::GetSystemTime();
  
  return PLUS_SUCCESS;
}


//-----------------------------------------------------------------------------
PlusStatus vtkVirtualVolumeReconstructor::NotifyConfigured()
{
  if( !this->OutputChannels.empty() )
  {
    LOG_WARNING("vtkVirtualDiscCapture is expecting no output channel(s) and there are " << this->OutputChannels.size() << " channels. Output channel information will be dropped.");
    this->OutputChannels.clear();
  }

  if( this->InputChannels.empty() )
  {
    LOG_ERROR("No input channel sent to vtkVirtualDiscCapture. Unable to save anything.");
    return PLUS_FAIL;
  }
  vtkPlusChannel* inputChannel=this->InputChannels[0];

  // GetTrackedFrame reads from the OutputChannels
  // For now, place the input stream as an output stream so its data is read
  this->OutputChannels.push_back(inputChannel);
  inputChannel->Register(this); // this device uses this channel, too, se we need to update the reference count to avoid double delete in the destructor

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
void vtkVirtualVolumeReconstructor::SetEnableReconstruction( bool aValue )
{
  this->EnableReconstruction = aValue;

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
PlusStatus vtkVirtualVolumeReconstructor::Reset()
{
  PlusLockGuard<vtkRecursiveCriticalSection> writerLock(this->VolumeReconstructorAccessMutex);
  this->VolumeReconstructor->Reset();
  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
double vtkVirtualVolumeReconstructor::GetAcquisitionRate() const
{
  if( this->InputChannels.size() <= 0 )
  {
    return VIRTUAL_DEVICE_FRAME_RATE;
  }
  return this->InputChannels[0]->GetOwnerDevice()->GetAcquisitionRate();
}

//-----------------------------------------------------------------------------
int vtkVirtualVolumeReconstructor::OutputChannelCount() const
{
  // Even though we fake one output channel for easy GetTrackedFrame ability, 
  //  we shouldn't return actual output channel size
  return 0;
}

//-----------------------------------------------------------------------------
void vtkVirtualVolumeReconstructor::InternalWriteOutputChannels( vtkXMLDataElement* rootXMLElement )
{
  // Do not write anything out, disc capture devices don't have output channels in the config
}

//-----------------------------------------------------------------------------
PlusStatus vtkVirtualVolumeReconstructor::GetReconstructedVolumeFromFile(const char* inputSeqFilename, vtkImageData* reconstructedVolume, std::string& message)
{
  message.clear();

  // Read image sequence
  if (inputSeqFilename==NULL)
  {    
    message="Volume reconstruction failed, InputSeqFilename has not been defined";
    LOG_INFO(message);
    return PLUS_FAIL;
  }
  vtkSmartPointer<vtkTrackedFrameList> trackedFrameList = vtkSmartPointer<vtkTrackedFrameList>::New(); 
  std::string inputImageSeqFileFullPath=vtkPlusConfig::GetInstance()->GetOutputPath(inputSeqFilename);
  if (trackedFrameList->ReadFromSequenceMetafile(inputImageSeqFileFullPath.c_str())==PLUS_FAIL)
  {    
    message="Volume reconstruction failed, unable to open input file specified in InputSeqFilename"+inputImageSeqFileFullPath;
    LOG_INFO(message);
    return PLUS_FAIL;
  } 

  PlusLockGuard<vtkRecursiveCriticalSection> writerLock(this->VolumeReconstructorAccessMutex);

  // Determine volume extents automatically
  if ( this->VolumeReconstructor->SetOutputExtentFromFrameList(trackedFrameList, this->TransformRepository) != PLUS_SUCCESS )
  {
    message="vtkPlusReconstructVolumeCommand::Execute: failed, image or reference coordinate frame name is invalid";
    LOG_INFO(message);    
    return PLUS_FAIL;
  }
  // Paste slices  
  if (AddFrames(trackedFrameList)!=PLUS_SUCCESS)
  {
    message="vtkPlusReconstructVolumeCommand::Execute: failed, add frames failed";
    LOG_INFO(message);    
    return PLUS_FAIL;
  }
  // Get output
  if (GetReconstructedVolume(reconstructedVolume, message)!=PLUS_SUCCESS)
  {
    LOG_INFO(message);    
    return PLUS_FAIL;
  }
  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkVirtualVolumeReconstructor::GetReconstructedVolume(vtkImageData* reconstructedVolume, std::string& message)
{
  message.clear();
  PlusLockGuard<vtkRecursiveCriticalSection> writerLock(this->VolumeReconstructorAccessMutex);
  if (this->VolumeReconstructor->ExtractGrayLevels(reconstructedVolume) != PLUS_SUCCESS)
  {
    message="Extracting gray levels failed";
    LOG_ERROR(message);
    return PLUS_FAIL;
  }  
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkVirtualVolumeReconstructor::AddFrames(vtkTrackedFrameList* trackedFrameList)
{
  PlusLockGuard<vtkRecursiveCriticalSection> writerLock(this->VolumeReconstructorAccessMutex);

  PlusStatus status=PLUS_SUCCESS;
  const int numberOfFrames = trackedFrameList->GetNumberOfTrackedFrames(); 
  int numberOfFramesAddedToVolume=0;
  for ( int frameIndex = 0; frameIndex < numberOfFrames; frameIndex+=this->VolumeReconstructor->GetSkipInterval() )
  {
    LOG_TRACE("Adding frame to volume reconstructor: "<<frameIndex);
    TrackedFrame* frame = trackedFrameList->GetTrackedFrame( frameIndex );
    if ( this->TransformRepository->SetTransforms(*frame) != PLUS_SUCCESS )
    {
      LOG_ERROR("Failed to update transform repository with frame #" << frameIndex ); 
      status=PLUS_FAIL;
      continue; 
    }
    // Insert slice for reconstruction
    bool insertedIntoVolume=false;
    if ( this->VolumeReconstructor->AddTrackedFrame(frame, this->TransformRepository, &insertedIntoVolume ) != PLUS_SUCCESS )
    {
      LOG_ERROR("Failed to add tracked frame to volume with frame #" << frameIndex); 
      status=PLUS_FAIL;
      continue; 
    }
    if ( insertedIntoVolume )
    {
      numberOfFramesAddedToVolume++; 
    }
  }
  trackedFrameList->Clear(); 

  LOG_DEBUG("Number of frames added to the volume: " << numberOfFramesAddedToVolume << " out of " << numberOfFrames ); 
  
  return status;
}

//-----------------------------------------------------------------------------
double vtkVirtualVolumeReconstructor::GetSamplingPeriodSec()
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

//----------------------------------------------------------------------------
PlusStatus vtkVirtualVolumeReconstructor::UpdateTransformRepository(vtkTransformRepository* sharedTransformRepository)
{
  if (sharedTransformRepository==NULL)
  {
    LOG_ERROR("vtkVirtualVolumeReconstructor::UpdateTransformRepository: shared transform repository is invalid");
    return PLUS_FAIL;
  }
  // Create a copy of the transform repository to allow using it for volume reconstruction while being also used in other threads
  // TODO: protect transform repository with a mutex
  this->TransformRepository->DeepCopy(sharedTransformRepository);
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
void vtkVirtualVolumeReconstructor::SetOutputOrigin(double* origin)
{
  this->VolumeReconstructor->SetOutputOrigin(origin);
}

//----------------------------------------------------------------------------
void vtkVirtualVolumeReconstructor::SetOutputSpacing(double* spacing)
{
  this->VolumeReconstructor->SetOutputSpacing(spacing);
}

//----------------------------------------------------------------------------
void vtkVirtualVolumeReconstructor::SetOutputExtent(int* extent)
{
  this->VolumeReconstructor->SetOutputExtent(extent);
}
