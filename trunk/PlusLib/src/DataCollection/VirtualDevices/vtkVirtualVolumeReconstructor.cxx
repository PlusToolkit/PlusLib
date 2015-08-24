/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"
#include "vtkVirtualVolumeReconstructor.h"
#include "TrackedFrame.h"
#include "vtkObjectFactory.h"
#include "vtkPlusChannel.h"
#include "vtkPlusDataSource.h"
#include "vtkTrackedFrameList.h"
#include "vtkVolumeReconstructor.h"
#include "vtkTransformRepository.h"

#include "vtksys/SystemTools.hxx"

//----------------------------------------------------------------------------

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
PlusStatus vtkVirtualVolumeReconstructor::ReadConfiguration( vtkXMLDataElement* rootConfigElement)
{
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_READING(deviceConfig, rootConfigElement);

  XML_READ_BOOL_ATTRIBUTE_OPTIONAL(EnableReconstruction, deviceConfig);
  XML_READ_STRING_ATTRIBUTE_OPTIONAL(OutputVolFilename, deviceConfig);
  XML_READ_STRING_ATTRIBUTE_OPTIONAL(OutputVolDeviceName, deviceConfig);

  PlusLockGuard<vtkRecursiveCriticalSection> writerLock(this->VolumeReconstructorAccessMutex);
  this->VolumeReconstructor->ReadConfiguration(deviceConfig);

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkVirtualVolumeReconstructor::WriteConfiguration( vtkXMLDataElement* rootConfig)
{
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_WRITING(deviceElement, rootConfig);

  deviceElement->SetAttribute("EnableReconstruction", this->EnableReconstruction ? "TRUE" : "FALSE" );

  deviceElement->SetAttribute("OutputVolFilename",this->OutputVolFilename);
  deviceElement->SetAttribute("OutputVolDeviceName",this->OutputVolDeviceName);

  PlusLockGuard<vtkRecursiveCriticalSection> writerLock(this->VolumeReconstructorAccessMutex);
  this->VolumeReconstructor->WriteConfiguration(deviceElement);

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
    LOG_ERROR(this->GetDeviceId() << ": Unable to add " << nbFramesRecorded << " frames for volume reconstruction");
    return PLUS_FAIL;
  }

  this->TotalFramesRecorded += nbFramesRecorded;

  // Check whether the reconstruction needed more time than the sampling interval
  double recordingTimeSec = vtkAccurateTimer::GetSystemTime() - startTimeSec;
  if (recordingTimeSec > GetSamplingPeriodSec())
  {
    LOG_WARNING("Volume reconstruction of the acquired "<<nbFramesRecorded<<" frames takes too long time (" << recordingTimeSec << "sec instead of the allocated " << GetSamplingPeriodSec() << "sec). This can cause slow-down of the application and non-uniform sampling. Reduce the image acquisition rate, output size, or image clip rectangle size to resolve the problem.");
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
  if (this->EnableReconstruction==aValue)
  {
    // Reconstruction is already started/stopped, no change needed
    return;
  }
  
  if( aValue )
  {
    // starting/resuming...
    // set the recording start time to add frames from now on
    m_LastUpdateTime = 0.0;
    m_TimeWaited = 0.0;
    m_LastAlreadyRecordedFrameTimestamp = UNDEFINED_TIMESTAMP;
    m_NextFrameToBeRecordedTimestamp = 0.0;
    this->EnableReconstruction = true;
  }
  else
  {
    // stopping/suspending...
    this->EnableReconstruction = aValue;
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
PlusStatus vtkVirtualVolumeReconstructor::GetReconstructedVolumeFromFile(const char* inputSeqFilename, vtkImageData* reconstructedVolume, std::string& errorMessage)
{
  errorMessage.clear();

  // Read image sequence
  if (inputSeqFilename==NULL)
  {    
    errorMessage="Volume reconstruction failed, InputSeqFilename has not been defined";
    LOG_INFO(errorMessage);
    return PLUS_FAIL;
  }
  vtkSmartPointer<vtkTrackedFrameList> trackedFrameList = vtkSmartPointer<vtkTrackedFrameList>::New(); 
  std::string inputImageSeqFileFullPath=vtkPlusConfig::GetInstance()->GetOutputPath(inputSeqFilename);
  if (trackedFrameList->ReadFromSequenceMetafile(inputImageSeqFileFullPath.c_str())==PLUS_FAIL)
  {    
    errorMessage="Volume reconstruction failed, unable to open input file specified in InputSeqFilename"+inputImageSeqFileFullPath;
    LOG_INFO(errorMessage);
    return PLUS_FAIL;
  } 

  PlusLockGuard<vtkRecursiveCriticalSection> writerLock(this->VolumeReconstructorAccessMutex);

  // Determine volume extents automatically
  std::string errorDetail;
  if ( this->VolumeReconstructor->SetOutputExtentFromFrameList(trackedFrameList, this->TransformRepository, errorDetail) != PLUS_SUCCESS )
  {
    errorMessage="vtkPlusReconstructVolumeCommand::Execute: failed, could not set up output volume - "+errorDetail;
    LOG_INFO(errorMessage);
    return PLUS_FAIL;
  }
  // Paste slices  
  if (AddFrames(trackedFrameList)!=PLUS_SUCCESS)
  {
    errorMessage="vtkPlusReconstructVolumeCommand::Execute: failed, add frames failed";
    LOG_INFO(errorMessage);
    return PLUS_FAIL;
  }
  // Get output
  if (GetReconstructedVolume(reconstructedVolume, errorMessage)!=PLUS_SUCCESS)
  {
    LOG_INFO(errorMessage);
    return PLUS_FAIL;
  }
  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkVirtualVolumeReconstructor::GetReconstructedVolume(vtkImageData* reconstructedVolume, std::string& errorMessage, bool applyHoleFilling/*=true*/)
{
  errorMessage.clear();
  PlusLockGuard<vtkRecursiveCriticalSection> writerLock(this->VolumeReconstructorAccessMutex);
  bool oldFillHoles = this->VolumeReconstructor->GetFillHoles();
  if (!applyHoleFilling)
  {
    this->VolumeReconstructor->SetFillHoles(false);
  }
  PlusStatus status = this->VolumeReconstructor->ExtractGrayLevels(reconstructedVolume);
  if (!applyHoleFilling)
  {
    this->VolumeReconstructor->SetFillHoles(oldFillHoles);
  }

  if (status != PLUS_SUCCESS)
  {
    errorMessage="Extracting gray levels failed";
    LOG_ERROR(errorMessage);
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
  this->TransformRepository->DeepCopy(sharedTransformRepository,false);
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
