/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"
#include "igsioTrackedFrame.h"
#include "vtkObjectFactory.h"
#include "vtkPlusChannel.h"
#include "vtkPlusDataSource.h"
#include "vtkPlusSequenceIO.h"
#include "vtkIGSIOTrackedFrameList.h"
#include "vtkIGSIOTransformRepository.h"
#include "vtkPlusVirtualVolumeReconstructor.h"
#include "vtkPlusVolumeReconstructor.h"
#include "vtksys/SystemTools.hxx"

//----------------------------------------------------------------------------

vtkStandardNewMacro(vtkPlusVirtualVolumeReconstructor);

static const int MAX_ALLOWED_RECONSTRUCTION_LAG_SEC = 3.0; // if the reconstruction lags more than this then it'll skip frames to catch up

//----------------------------------------------------------------------------
vtkPlusVirtualVolumeReconstructor::vtkPlusVirtualVolumeReconstructor()
  : vtkPlusDevice()
  , m_LastAlreadyRecordedFrameTimestamp(UNDEFINED_TIMESTAMP)
  , m_NextFrameToBeRecordedTimestamp(0.0)
  , m_SamplingFrameRate(8)
  , RequestedFrameRate(0.0)
  , m_TimeWaited(0.0)
  , m_LastUpdateTime(0.0)
  , TotalFramesRecorded(0)
  , EnableReconstruction(false)
  , VolumeReconstructorAccessMutex(vtkSmartPointer<vtkIGSIORecursiveCriticalSection>::New())
{
  // The data capture thread will be used to regularly read the frames and write to disk
  this->StartThreadForInternalUpdates = true;

  this->VolumeReconstructor = vtkSmartPointer<vtkPlusVolumeReconstructor>::New();
  this->TransformRepository = vtkSmartPointer<vtkIGSIOTransformRepository>::New();
}

//----------------------------------------------------------------------------
vtkPlusVirtualVolumeReconstructor::~vtkPlusVirtualVolumeReconstructor()
{
}

//----------------------------------------------------------------------------
void vtkPlusVirtualVolumeReconstructor::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusVirtualVolumeReconstructor::ReadConfiguration(vtkXMLDataElement* rootConfigElement)
{
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_READING(deviceConfig, rootConfigElement);

  XML_READ_BOOL_ATTRIBUTE_OPTIONAL(EnableReconstruction, deviceConfig);
  XML_READ_CSTRING_ATTRIBUTE_OPTIONAL(OutputVolFilename, deviceConfig);
  XML_READ_CSTRING_ATTRIBUTE_OPTIONAL(OutputVolDeviceName, deviceConfig);

  igsioLockGuard<vtkIGSIORecursiveCriticalSection> writerLock(this->VolumeReconstructorAccessMutex);
  this->VolumeReconstructor->ReadConfiguration(deviceConfig);

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusVirtualVolumeReconstructor::WriteConfiguration(vtkXMLDataElement* rootConfig)
{
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_WRITING(deviceElement, rootConfig);

  deviceElement->SetAttribute("EnableReconstruction", this->EnableReconstruction ? "TRUE" : "FALSE");

  deviceElement->SetAttribute("OutputVolFilename", this->OutputVolFilename.c_str());
  deviceElement->SetAttribute("OutputVolDeviceName", this->OutputVolDeviceName.c_str());

  igsioLockGuard<vtkIGSIORecursiveCriticalSection> writerLock(this->VolumeReconstructorAccessMutex);
  this->VolumeReconstructor->WriteConfiguration(deviceElement);

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusVirtualVolumeReconstructor::InternalConnect()
{
  bool lowestRateKnown = false;
  double lowestRate = 30; // just a usual value (FPS)
  for (ChannelContainerConstIterator it = this->InputChannels.begin(); it != this->InputChannels.end(); ++it)
  {
    vtkPlusChannel* anInputStream = (*it);
    if (anInputStream->GetOwnerDevice()->GetAcquisitionRate() < lowestRate || !lowestRateKnown)
    {
      lowestRate = anInputStream->GetOwnerDevice()->GetAcquisitionRate();
      lowestRateKnown = true;
    }
  }
  if (lowestRateKnown)
  {
    this->AcquisitionRate = lowestRate;
  }
  else
  {
    LOG_WARNING("vtkPlusVirtualVolumeReconstructor acquisition rate is not known");
  }

  m_LastUpdateTime = vtkIGSIOAccurateTimer::GetSystemTime();

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusVirtualVolumeReconstructor::InternalDisconnect()
{
  SetEnableReconstruction(false);
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusVirtualVolumeReconstructor::InternalUpdate()
{
  if (!this->EnableReconstruction)
  {
    // Capturing is disabled
    return PLUS_SUCCESS;
  }

  if (m_LastUpdateTime == 0.0)
  {
    m_LastUpdateTime = vtkIGSIOAccurateTimer::GetSystemTime();
  }
  if (m_NextFrameToBeRecordedTimestamp == 0.0)
  {
    m_NextFrameToBeRecordedTimestamp = vtkIGSIOAccurateTimer::GetSystemTime();
  }
  double startTimeSec = vtkIGSIOAccurateTimer::GetSystemTime();

  m_TimeWaited += startTimeSec - m_LastUpdateTime;

  if (m_TimeWaited < GetSamplingPeriodSec())
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
    requestedFrameRate = this->AcquisitionRate;
  }
  if (requestedFrameRate > 0)
  {
    requestedFramePeriodSec = 1.0 / requestedFrameRate;
  }
  else
  {
    LOG_WARNING("RequestedFrameRate is invalid, use default: " << 1 / requestedFramePeriodSec);
  }

  igsioLockGuard<vtkIGSIORecursiveCriticalSection> writerLock(this->VolumeReconstructorAccessMutex);
  if (!this->EnableReconstruction)
  {
    // While this thread was waiting for the unlock, capturing was disabled, so cancel the update now
    return PLUS_SUCCESS;
  }

  if (this->OutputChannels.empty())
  {
    LOG_ERROR("No output channels defined");
    return PLUS_FAIL;
  }
  vtkPlusChannel* outputChannel = this->OutputChannels[0];

  vtkSmartPointer<vtkIGSIOTrackedFrameList> recordedFrames = vtkSmartPointer<vtkIGSIOTrackedFrameList>::New();
  if (outputChannel->GetTrackedFrameListSampled(m_LastAlreadyRecordedFrameTimestamp, m_NextFrameToBeRecordedTimestamp, recordedFrames, requestedFramePeriodSec, maxProcessingTimeSec) != PLUS_SUCCESS)
  {
    LOG_ERROR("Error while getting tracked frame list from data collector during volume reconstruction. Last recorded timestamp: " << std::fixed << m_NextFrameToBeRecordedTimestamp);
  }
  int nbFramesRecorded = recordedFrames->GetNumberOfTrackedFrames();

  if (this->AddFrames(recordedFrames) != PLUS_SUCCESS)
  {
    LOG_ERROR(this->GetDeviceId() << ": Unable to add " << nbFramesRecorded << " frames for volume reconstruction");
    return PLUS_FAIL;
  }

  this->TotalFramesRecorded += nbFramesRecorded;

  // Check whether the reconstruction needed more time than the sampling interval
  double recordingTimeSec = vtkIGSIOAccurateTimer::GetSystemTime() - startTimeSec;
  if (recordingTimeSec > GetSamplingPeriodSec())
  {
    LOG_WARNING("Volume reconstruction of the acquired " << nbFramesRecorded << " frames takes too long time (" << recordingTimeSec << "sec instead of the allocated " << GetSamplingPeriodSec() << "sec). This can cause slow-down of the application and non-uniform sampling. Reduce the image acquisition rate, output size, or image clip rectangle size to resolve the problem.");
  }
  double recordingLagSec = vtkIGSIOAccurateTimer::GetSystemTime() - m_NextFrameToBeRecordedTimestamp;

  if (recordingLagSec > MAX_ALLOWED_RECONSTRUCTION_LAG_SEC)
  {
    LOG_ERROR("Volume reconstruction cannot keep up with the acquisition. Skip " << recordingLagSec << " seconds of the data stream to catch up.");
    m_NextFrameToBeRecordedTimestamp = vtkIGSIOAccurateTimer::GetSystemTime();
  }

  m_LastUpdateTime = vtkIGSIOAccurateTimer::GetSystemTime();

  return PLUS_SUCCESS;
}


//-----------------------------------------------------------------------------
PlusStatus vtkPlusVirtualVolumeReconstructor::NotifyConfigured()
{
  if (!this->OutputChannels.empty())
  {
    LOG_WARNING("vtkPlusVirtualCapture is expecting no output channel(s) and there are " << this->OutputChannels.size() << " channels. Output channel information will be dropped.");
    this->OutputChannels.clear();
  }

  if (this->InputChannels.empty())
  {
    LOG_ERROR("No input channel sent to vtkPlusVirtualCapture. Unable to save anything.");
    return PLUS_FAIL;
  }
  vtkPlusChannel* inputChannel = this->InputChannels[0];

  // GetTrackedFrame reads from the OutputChannels
  // For now, place the input stream as an output stream so its data is read
  this->OutputChannels.push_back(inputChannel);
  inputChannel->Register(this);   // this device uses this channel, too, se we need to update the reference count to avoid double delete in the destructor

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
void vtkPlusVirtualVolumeReconstructor::SetEnableReconstruction(bool aValue)
{
  if (this->EnableReconstruction == aValue)
  {
    // Reconstruction is already started/stopped, no change needed
    return;
  }

  if (aValue)
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
PlusStatus vtkPlusVirtualVolumeReconstructor::Reset()
{
  igsioLockGuard<vtkIGSIORecursiveCriticalSection> writerLock(this->VolumeReconstructorAccessMutex);
  this->VolumeReconstructor->Reset();
  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
double vtkPlusVirtualVolumeReconstructor::GetAcquisitionRate() const
{
  if (this->InputChannels.size() <= 0)
  {
    return VIRTUAL_DEVICE_FRAME_RATE;
  }
  return this->InputChannels[0]->GetOwnerDevice()->GetAcquisitionRate();
}

//-----------------------------------------------------------------------------
int vtkPlusVirtualVolumeReconstructor::OutputChannelCount() const
{
  // Even though we fake one output channel for easy GetTrackedFrame ability,
  //  we shouldn't return actual output channel size
  return 0;
}

//-----------------------------------------------------------------------------
void vtkPlusVirtualVolumeReconstructor::InternalWriteOutputChannels(vtkXMLDataElement* rootXMLElement)
{
  // Do not write anything out, disc capture devices don't have output channels in the config
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusVirtualVolumeReconstructor::GetReconstructedVolumeFromFile(const std::string& inputSeqFilename, vtkImageData* reconstructedVolume, std::string& errorMessage)
{
  errorMessage.clear();

  // Read image sequence
  if (inputSeqFilename.empty())
  {
    errorMessage = "Volume reconstruction failed, InputSeqFilename has not been defined";
    LOG_INFO(errorMessage);
    return PLUS_FAIL;
  }
  vtkSmartPointer<vtkIGSIOTrackedFrameList> trackedFrameList = vtkSmartPointer<vtkIGSIOTrackedFrameList>::New();
  std::string inputImageSeqFileFullPath = vtkPlusConfig::GetInstance()->GetOutputPath(inputSeqFilename);
  if (vtkPlusSequenceIO::Read(inputImageSeqFileFullPath, trackedFrameList) != PLUS_SUCCESS)
  {
    errorMessage = "Volume reconstruction failed, unable to open input file specified in InputSeqFilename: " + inputImageSeqFileFullPath;
    LOG_INFO(errorMessage);
    return PLUS_FAIL;
  }

  igsioLockGuard<vtkIGSIORecursiveCriticalSection> writerLock(this->VolumeReconstructorAccessMutex);

  // Determine volume extents automatically
  std::string errorDetail;
  if (this->VolumeReconstructor->SetOutputExtentFromFrameList(trackedFrameList, this->TransformRepository, errorDetail) != PLUS_SUCCESS)
  {
    errorMessage = "vtkPlusReconstructVolumeCommand::Execute: failed, could not set up output volume - " + errorDetail;
    LOG_INFO(errorMessage);
    return PLUS_FAIL;
  }
  // Paste slices
  if (AddFrames(trackedFrameList) != PLUS_SUCCESS)
  {
    errorMessage = "vtkPlusReconstructVolumeCommand::Execute: failed, add frames failed";
    LOG_INFO(errorMessage);
    return PLUS_FAIL;
  }
  // Get output
  if (GetReconstructedVolume(reconstructedVolume, errorMessage) != PLUS_SUCCESS)
  {
    LOG_INFO(errorMessage);
    return PLUS_FAIL;
  }
  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusVirtualVolumeReconstructor::GetReconstructedVolume(vtkImageData* reconstructedVolume, std::string& outErrorMessage, bool applyHoleFilling/*=true*/)
{
  outErrorMessage.clear();
  igsioLockGuard<vtkIGSIORecursiveCriticalSection> writerLock(this->VolumeReconstructorAccessMutex);
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
    outErrorMessage = "Extracting gray levels failed";
    LOG_ERROR(outErrorMessage);
    return PLUS_FAIL;
  }
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusVirtualVolumeReconstructor::AddFrames(vtkIGSIOTrackedFrameList* trackedFrameList)
{
  igsioLockGuard<vtkIGSIORecursiveCriticalSection> writerLock(this->VolumeReconstructorAccessMutex);

  PlusStatus status = PLUS_SUCCESS;
  const int numberOfFrames = trackedFrameList->GetNumberOfTrackedFrames();
  int numberOfFramesAddedToVolume = 0;
  for (int frameIndex = 0; frameIndex < numberOfFrames; frameIndex += this->VolumeReconstructor->GetSkipInterval())
  {
    LOG_TRACE("Adding frame to volume reconstructor: " << frameIndex);
    igsioTrackedFrame* frame = trackedFrameList->GetTrackedFrame(frameIndex);
    if (this->TransformRepository->SetTransforms(*frame) != PLUS_SUCCESS)
    {
      LOG_ERROR("Failed to update transform repository with frame #" << frameIndex);
      status = PLUS_FAIL;
      continue;
    }
    // Insert slice for reconstruction
    bool insertedIntoVolume = false;
    if (this->VolumeReconstructor->AddTrackedFrame(frame, this->TransformRepository, &insertedIntoVolume) != PLUS_SUCCESS)
    {
      LOG_ERROR("Failed to add tracked frame to volume with frame #" << frameIndex);
      status = PLUS_FAIL;
      continue;
    }
    if (insertedIntoVolume)
    {
      numberOfFramesAddedToVolume++;
    }
  }
  trackedFrameList->Clear();

  LOG_DEBUG("Number of frames added to the volume: " << numberOfFramesAddedToVolume << " out of " << numberOfFrames);

  return status;
}

//-----------------------------------------------------------------------------
double vtkPlusVirtualVolumeReconstructor::GetSamplingPeriodSec()
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
PlusStatus vtkPlusVirtualVolumeReconstructor::UpdateTransformRepository(vtkIGSIOTransformRepository* sharedTransformRepository)
{
  if (sharedTransformRepository == NULL)
  {
    LOG_ERROR("vtkPlusVirtualVolumeReconstructor::UpdateTransformRepository: shared transform repository is invalid");
    return PLUS_FAIL;
  }
  // Create a copy of the transform repository to allow using it for volume reconstruction while being also used in other threads
  // TODO: protect transform repository with a mutex
  this->TransformRepository->DeepCopy(sharedTransformRepository, false);
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
void vtkPlusVirtualVolumeReconstructor::SetOutputOrigin(double* origin)
{
  this->VolumeReconstructor->SetOutputOrigin(origin);
}

//----------------------------------------------------------------------------
void vtkPlusVirtualVolumeReconstructor::SetOutputSpacing(double* spacing)
{
  this->VolumeReconstructor->SetOutputSpacing(spacing);
}

//----------------------------------------------------------------------------
void vtkPlusVirtualVolumeReconstructor::SetOutputExtent(int* extent)
{
  this->VolumeReconstructor->SetOutputExtent(extent);
}
