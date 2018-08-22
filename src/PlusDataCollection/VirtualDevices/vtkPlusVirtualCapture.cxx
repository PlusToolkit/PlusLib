/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"
#include "PlusTrackedFrame.h"
#include "vtkPlusMetaImageSequenceIO.h"
#include "vtkObjectFactory.h"
#include "vtkPlusChannel.h"
#include "vtkPlusDataSource.h"
#include "vtkPlusSequenceIO.h"
#include "vtkPlusTrackedFrameList.h"
#include "vtkPlusVirtualCapture.h"
#include "vtksys/SystemTools.hxx"

//----------------------------------------------------------------------------

vtkStandardNewMacro(vtkPlusVirtualCapture);

//----------------------------------------------------------------------------

namespace
{
  static const double WARNING_RECORDING_LAG_SEC = 1.0; // if the recording lags more than this then a warning message will be displayed
  static const double MAX_ALLOWED_RECORDING_LAG_SEC = 3.0; // if the recording lags more than this then it'll skip frames to catch up
  static const unsigned int DISABLE_FRAME_BUFFER = std::numeric_limits<unsigned int>::max();
}

//----------------------------------------------------------------------------
vtkPlusVirtualCapture::vtkPlusVirtualCapture()
  : vtkPlusDevice()
  , RecordedFrames(vtkPlusTrackedFrameList::New())
  , LastAlreadyRecordedFrameTimestamp(UNDEFINED_TIMESTAMP)
  , NextFrameToBeRecordedTimestamp(0.0)
  , RequestedFrameRate(15.0)
  , ActualFrameRate(0.0)
  , FirstFrameIndexInThisSegment(0)
  , TimeWaited(0.0)
  , LastUpdateTime(0.0)
  , CurrentFilename("")
  , BaseFilename("TrackedImageSequence.nrrd")
  , Writer(NULL)
  , EnableFileCompression(false)
  , IsHeaderPrepared(false)
  , TotalFramesRecorded(0)
  , EnableCapturingOnStart(false)
  , EnableCapturing(false)
  , FrameBufferSize(DISABLE_FRAME_BUFFER)
  , IsData3D(false)
  , WriterAccessMutex(vtkSmartPointer<vtkPlusRecursiveCriticalSection>::New())
  , GracePeriodLogLevel(vtkPlusLogger::LOG_LEVEL_DEBUG)
{
  this->AcquisitionRate = 30.0;
  this->MissingInputGracePeriodSec = 2.0;
  this->RecordedFrames->SetValidationRequirements(REQUIRE_UNIQUE_TIMESTAMP);

  // The data capture thread will be used to regularly read the frames and write to disk
  this->StartThreadForInternalUpdates = true;
}

//----------------------------------------------------------------------------
vtkPlusVirtualCapture::~vtkPlusVirtualCapture()
{
  if (IsHeaderPrepared)
  {
    this->CloseFile();
  }

  if (RecordedFrames != NULL)
  {
    this->RecordedFrames->Delete();
    this->RecordedFrames = NULL;
  }

  if (Writer != NULL)
  {
    this->Writer->Delete();
    this->Writer = NULL;
  }
}

//----------------------------------------------------------------------------
void vtkPlusVirtualCapture::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusVirtualCapture::ReadConfiguration(vtkXMLDataElement* rootConfigElement)
{
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_READING(deviceConfig, rootConfigElement);
  const char* deviceType = deviceConfig->GetAttribute("Type");
  if (deviceType && deviceType != std::string("VirtualCapture"))
  {
    LOG_WARNING("Device type \"" << deviceType << "\" is deprecated. Use \"VirtualCapture\" instead.");
    deviceConfig->SetAttribute("Type", "VirtualCapture");
  }

  XML_READ_STRING_ATTRIBUTE_OPTIONAL(BaseFilename, deviceConfig);
  XML_READ_BOOL_ATTRIBUTE_OPTIONAL(EnableFileCompression, deviceConfig);
  XML_READ_BOOL_ATTRIBUTE_OPTIONAL(EnableCapturingOnStart, deviceConfig);
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(double, RequestedFrameRate, deviceConfig);
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(int, FrameBufferSize, deviceConfig);

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusVirtualCapture::WriteConfiguration(vtkXMLDataElement* rootConfig)
{
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_WRITING(deviceElement, rootConfig);
  deviceElement->SetAttribute("EnableCapturing", this->EnableCapturing ? "TRUE" : "FALSE");
  deviceElement->SetAttribute("EnableFileCompression", this->EnableFileCompression ? "TRUE" : "FALSE");
  deviceElement->SetAttribute("EnableCaptureOnStart", this->EnableCapturingOnStart ? "TRUE" : "FALSE");
  deviceElement->SetDoubleAttribute("RequestedFrameRate", this->GetRequestedFrameRate());

  return PLUS_SUCCESS;
}


//----------------------------------------------------------------------------
PlusStatus vtkPlusVirtualCapture::InternalConnect()
{
  if (OpenFile() != PLUS_SUCCESS)
  {
    return PLUS_FAIL;
  }

  if (this->GetEnableCapturingOnStart())
  {
    this->SetEnableCapturing(true);
  }

  this->LastUpdateTime = vtkPlusAccurateTimer::GetSystemTime();

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusVirtualCapture::InternalDisconnect()
{
  this->EnableCapturing = false;

  // If outstanding frames to be written, deal with them
  if (this->RecordedFrames->GetNumberOfTrackedFrames() != 0 && this->IsHeaderPrepared)
  {
    if (this->Writer->AppendImagesToHeader() != PLUS_SUCCESS)
    {
      LOG_ERROR("Unable to append image data to header.");
      this->Disconnect();
      return PLUS_FAIL;
    }
    if (this->Writer->WriteImages() != PLUS_SUCCESS)
    {
      LOG_ERROR("Unable to append images. Stopping recording at timestamp: " << this->LastAlreadyRecordedFrameTimestamp);
      this->Disconnect();
      return PLUS_FAIL;
    }

    this->ClearRecordedFrames();
  }
  PlusStatus status = this->CloseFile();
  return status;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusVirtualCapture::OpenFile(const char* aFilename)
{
  PlusLockGuard<vtkPlusRecursiveCriticalSection> writerLock(this->WriterAccessMutex);

  // Because this virtual device continually appends data to the file, we cannot do live compression
  if (aFilename == NULL || strlen(aFilename) == 0)
  {
    std::string filenameRoot = vtksys::SystemTools::GetFilenameWithoutExtension(this->BaseFilename);
    std::string ext = vtksys::SystemTools::GetFilenameExtension(this->BaseFilename);
    if (ext.empty())
    {
      // default to nrrd
      ext = ".nrrd";
    }
    else if (vtkPlusMetaImageSequenceIO::CanWriteFile(this->BaseFilename) && this->GetEnableFileCompression())
    {
      // they've requested mhd/mha with compression, no can do, yet
      LOG_WARNING("Compressed saving of metaimage file requested. This is not supported. Reverting to uncompressed mha.");
      this->SetEnableFileCompression(false);
    }
    this->CurrentFilename = filenameRoot + "_" + vtksys::SystemTools::GetCurrentDateTime("%Y%m%d_%H%M%S") + ext;
    aFilename = this->CurrentFilename.c_str();
  }
  else
  {
    if (vtkPlusMetaImageSequenceIO::CanWriteFile(aFilename) && this->GetEnableFileCompression())
    {
      // they've requested mhd/mha with compression, no can do, yet
      LOG_WARNING("Compressed saving of metaimage file requested. This is not supported. Reverting to uncompressed mha.");
      this->SetEnableFileCompression(false);
    }
    this->CurrentFilename = aFilename;
  }

  this->Writer = vtkPlusSequenceIO::CreateSequenceHandlerForFile(aFilename);
  if (!this->Writer)
  {
    LOG_ERROR("Could not create writer for file: " << aFilename);
    return PLUS_FAIL;
  }

  this->Writer->SetUseCompression(this->EnableFileCompression);
  this->Writer->SetTrackedFrameList(this->RecordedFrames);
  // Need to set the filename before finalizing header, because the pixel data file name depends on the file extension
  this->Writer->SetFileName(vtkPlusConfig::GetInstance()->GetOutputPath(aFilename));

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusVirtualCapture::CloseFile(const char* aFilename /* = NULL */, std::string* resultFilename /* = NULL */)
{
  // Fix the header to write the correct number of frames
  PlusLockGuard<vtkPlusRecursiveCriticalSection> writerLock(this->WriterAccessMutex);

  if (!this->IsHeaderPrepared)
  {
    // nothing has been prepared, so nothing to finalize
    return PLUS_SUCCESS;
  }

  if (aFilename != NULL && strlen(aFilename) != 0)
  {
    // Need to set the filename before finalizing header, because the pixel data file name depends on the file extension
    this->Writer->SetFileName(vtkPlusConfig::GetInstance()->GetOutputPath(aFilename));
    this->CurrentFilename = aFilename;
  }

  // Do we have any outstanding unwritten data?
  if (this->RecordedFrames->GetNumberOfTrackedFrames() != 0)
  {
    this->WriteFrames(true);
  }

  this->Writer->UpdateDimensionsCustomStrings(this->TotalFramesRecorded, this->GetIsData3D());
  this->Writer->UpdateFieldInImageHeader(this->Writer->GetDimensionSizeString());
  this->Writer->UpdateFieldInImageHeader(this->Writer->GetDimensionKindsString());
  this->Writer->FinalizeHeader();

  if (resultFilename != NULL)
  {
    (*resultFilename) = this->Writer->GetFileName();
  }

  this->Writer->Close();

  std::string fullPath = vtkPlusConfig::GetInstance()->GetOutputPath(this->CurrentFilename);
  std::string path = vtksys::SystemTools::GetFilenamePath(fullPath);
  std::string filename = vtksys::SystemTools::GetFilenameWithoutExtension(fullPath);
  std::string configFileName = path + "/" + filename + "_config.xml";
  PlusCommon::XML::PrintXML(configFileName.c_str(), vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationData());

  this->IsHeaderPrepared = false;
  this->TotalFramesRecorded = 0;
  this->RecordedFrames->Clear();

  if (this->OpenFile() != PLUS_SUCCESS)
  {
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------

PlusStatus vtkPlusVirtualCapture::InternalUpdate()
{
  if (!this->EnableCapturing)
  {
    // Capturing is disabled
    return PLUS_SUCCESS;
  }

  double samplingPeriodSec = 0.1;
  if (this->AcquisitionRate > 0)
  {
    samplingPeriodSec = 1.0 / this->AcquisitionRate;
  }
  else
  {
    LOG_WARNING("AcquisitionRate value is invalid " << this->AcquisitionRate << ". Use default sampling period of " << samplingPeriodSec << " sec");
  }

  if (this->LastUpdateTime == 0.0)
  {
    this->LastUpdateTime = vtkPlusAccurateTimer::GetSystemTime();
  }
  if (this->NextFrameToBeRecordedTimestamp == 0.0)
  {
    this->NextFrameToBeRecordedTimestamp = vtkPlusAccurateTimer::GetSystemTime();
  }
  double startTimeSec = vtkPlusAccurateTimer::GetSystemTime();

  this->TimeWaited += startTimeSec - LastUpdateTime;

  if (this->TimeWaited < samplingPeriodSec)
  {
    // Nothing to do yet
    return PLUS_SUCCESS;
  }

  this->TimeWaited = 0.0;

  double maxProcessingTimeSec = samplingPeriodSec * 2.0; // put a hard limit on the max processing time to make sure the application remains responsive during recording
  double requestedFramePeriodSec = 0.1;
  if (this->RequestedFrameRate > 0)
  {
    requestedFramePeriodSec = 1.0 / this->RequestedFrameRate;
  }
  else
  {
    LOG_WARNING("RequestedFrameRate is invalid");
  }

  if (this->HasGracePeriodExpired())
  {
    this->GracePeriodLogLevel = vtkPlusLogger::LOG_LEVEL_WARNING;
  }

  PlusLockGuard<vtkPlusRecursiveCriticalSection> writerLock(this->WriterAccessMutex);
  if (!this->EnableCapturing)
  {
    // While this thread was waiting for the unlock, capturing was disabled, so cancel the update now
    return PLUS_SUCCESS;
  }

  int nbFramesBefore = this->RecordedFrames->GetNumberOfTrackedFrames();
  if (this->GetInputTrackedFrameListSampled(this->LastAlreadyRecordedFrameTimestamp, this->NextFrameToBeRecordedTimestamp, this->RecordedFrames, requestedFramePeriodSec, maxProcessingTimeSec) != PLUS_SUCCESS)
  {
    LOG_ERROR("Error while getting tracked frame list from data collector during capturing. Last recorded timestamp: " << std::fixed << this->NextFrameToBeRecordedTimestamp);
  }
  int nbFramesAfter = this->RecordedFrames->GetNumberOfTrackedFrames();

  // Compute the average frame rate from the ratio of recently acquired frames
  int frame1Index = this->RecordedFrames->GetNumberOfTrackedFrames() - 1; // index of the latest frame
  int frame2Index = frame1Index - this->RequestedFrameRate * 5.0 - 1; // index of an earlier acquired frame (go back by approximately 5 seconds + one frame)
  if (frame2Index < this->FirstFrameIndexInThisSegment)
  {
    // make sure we stay in the current recording segment
    frame2Index = this->FirstFrameIndexInThisSegment;
  }
  if (frame1Index > frame2Index)
  {
    PlusTrackedFrame* frame1 = this->RecordedFrames->GetTrackedFrame(frame1Index);
    PlusTrackedFrame* frame2 = this->RecordedFrames->GetTrackedFrame(frame2Index);
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

  if (this->WriteFrames() != PLUS_SUCCESS)
  {
    LOG_ERROR(this->GetDeviceId() << ": Unable to write " << nbFramesAfter - nbFramesBefore << " frames.");
    return PLUS_FAIL;
  }

  this->TotalFramesRecorded += nbFramesAfter - nbFramesBefore;

  if (this->TotalFramesRecorded == 0)
  {
    // We haven't received any data so far
    LOG_DYNAMIC("No input data available to capture thread. Waiting until input data arrives.", this->GracePeriodLogLevel);
  }

  // Check whether the recording needed more time than the sampling interval
  double recordingTimeSec = vtkPlusAccurateTimer::GetSystemTime() - startTimeSec;
  double currentSystemTime = vtkPlusAccurateTimer::GetSystemTime();
  double recordingLagSec =  currentSystemTime - this->NextFrameToBeRecordedTimestamp;

  if (recordingTimeSec > samplingPeriodSec)
  {
    // Log too long recording as warning only if the recording is falling behind
    vtkPlusLogger::LogLevelType logLevel = (recordingLagSec > WARNING_RECORDING_LAG_SEC ? vtkPlusLogger::LOG_LEVEL_WARNING : vtkPlusLogger::LOG_LEVEL_DEBUG);
    LOG_DYNAMIC("Recording of frames takes too long time (" << recordingTimeSec << "sec instead of the allocated " << samplingPeriodSec << "sec, recording lags by " << recordingLagSec << "sec). This can cause slow-down of the application and non-uniform sampling. Reduce the acquisition rate or sampling rate to resolve the problem.", logLevel);
  }

  if (recordingLagSec > MAX_ALLOWED_RECORDING_LAG_SEC)
  {
    double acquisitionLagSec = recordingLagSec;
    double latestInputTimestamp = this->NextFrameToBeRecordedTimestamp;
    if (GetLatestInputItemTimestamp(latestInputTimestamp) == PLUS_SUCCESS)
    {
      acquisitionLagSec = currentSystemTime - latestInputTimestamp;
    }
    if (acquisitionLagSec < MAX_ALLOWED_RECORDING_LAG_SEC)
    {
      // Frames are available (because acquisitionLagSec < MAX_ALLOWED_RECORDING_LAG_SEC) but recording is falling behind
      // (because acquisitionLagSec < MAX_ALLOWED_RECORDING_LAG_SEC)
      LOG_ERROR("Recording cannot keep up with the acquisition. Skip " << recordingLagSec << " seconds of the data stream to catch up.");
    }
    this->NextFrameToBeRecordedTimestamp = vtkPlusAccurateTimer::GetSystemTime();
  }

  this->LastUpdateTime = vtkPlusAccurateTimer::GetSystemTime();

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusVirtualCapture::NotifyConfigured()
{
  if (!this->OutputChannels.empty())
  {
    LOG_WARNING("vtkPlusVirtualCapture is expecting no output channel(s) and there are " << this->OutputChannels.size() << " channels. Output channel information will be dropped.");
    this->OutputChannels.clear();
  }

  if (this->InputChannels.empty())
  {
    LOG_ERROR("No input channel set for vtkPlusVirtualCapture. Unable to save anything.");
    return PLUS_FAIL;
  }
  vtkPlusChannel* inputChannel = this->InputChannels[0];

  if (this->InputChannels.size() > 1)
  {
    LOG_WARNING("vtkPlusVirtualCapture is expecting one input channel and there are " << this->InputChannels.size() << " channels. First output channel will be used, all other are ignored.");
  }

  // GetTrackedFrame reads from the OutputChannels
  // For now, place the input stream as an output stream so its data is read
  this->OutputChannels.push_back(inputChannel);
  inputChannel->Register(this);   // this device uses this channel, too, se we need to update the reference count to avoid double delete in the destructor

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
bool vtkPlusVirtualCapture::HasUnsavedData() const
{
  return this->IsHeaderPrepared;
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusVirtualCapture::ClearRecordedFrames()
{
  this->RecordedFrames->Clear();

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
void vtkPlusVirtualCapture::InternalWriteOutputChannels(vtkXMLDataElement* rootXMLElement)
{
  // Do not write anything out, disc capture devices don't have output channels in the config
}

//----------------------------------------------------------------------------
void vtkPlusVirtualCapture::SetEnableFileCompression(bool aFileCompression)
{
  if (this->Writer != NULL)
  {
    this->Writer->SetUseCompression(aFileCompression);
  }

  this->EnableFileCompression = aFileCompression;
}

//-----------------------------------------------------------------------------
void vtkPlusVirtualCapture::SetEnableCapturing(bool aValue)
{
  this->EnableCapturing = aValue;

  if (this->EnableCapturing)
  {
    this->LastUpdateTime = 0.0;
    this->TimeWaited = 0.0;
    this->LastAlreadyRecordedFrameTimestamp = UNDEFINED_TIMESTAMP;
    this->NextFrameToBeRecordedTimestamp = 0.0;
    this->FirstFrameIndexInThisSegment = this->RecordedFrames->GetNumberOfTrackedFrames();
    this->RecordingStartTime = vtkPlusAccurateTimer::GetSystemTime(); // reset the starting time for the grace period
  }
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusVirtualCapture::Reset()
{
  {
    PlusLockGuard<vtkPlusRecursiveCriticalSection> writerLock(this->WriterAccessMutex);

    this->SetEnableCapturing(false);

    if (this->IsHeaderPrepared)
    {
      this->Writer->Discard();
    }

    this->ClearRecordedFrames();
    this->Writer->GetTrackedFrameList()->Clear();
    this->IsHeaderPrepared = false;
    this->TotalFramesRecorded = 0;
  }

  if (this->OpenFile() != PLUS_SUCCESS)
  {
    LOG_ERROR("Unable to reset device " << this->GetDeviceId() << ".");
    return PLUS_FAIL;
  }

  this->LastUpdateTime = vtkPlusAccurateTimer::GetSystemTime();

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
bool vtkPlusVirtualCapture::IsFrameBuffered() const
{
  return this->FrameBufferSize != DISABLE_FRAME_BUFFER;
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusVirtualCapture::TakeSnapshot()
{
  if (this->EnableCapturing)
  {
    LOG_ERROR(this->GetDeviceId() << ": Cannot take snapshot while the device is recording.");
    return PLUS_FAIL;
  }

  PlusTrackedFrame trackedFrame;
  if (this->GetInputTrackedFrame(trackedFrame) != PLUS_SUCCESS)
  {
    LOG_ERROR(this->GetDeviceId() << ": Failed to get tracked frame for the snapshot!");
    return PLUS_FAIL;
  }

  // Check if there are any valid transforms
  std::vector<PlusTransformName> transformNames;
  trackedFrame.GetFrameTransformNameList(transformNames);
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
      trackedFrame.GetFrameTransformStatus(*it, status);

      if (status == FIELD_OK)
      {
        validFrame = true;
        break;
      }
    }
  }

  if (!validFrame)
  {
    LOG_WARNING(this->GetDeviceId() << ": Unable to record tracked frame: All the tool transforms are invalid!");
    return PLUS_FAIL;
  }

  // Add tracked frame to the list
  // Snapshots are triggered manually, so the additional copying in AddTrackedFrame compared to TakeTrackedFrame is not relevant.
  if (this->RecordedFrames->AddTrackedFrame(&trackedFrame, vtkPlusTrackedFrameList::SKIP_INVALID_FRAME) != PLUS_SUCCESS)
  {
    LOG_WARNING(this->GetDeviceId() << ": Frame could not be added because validation failed");
    return PLUS_FAIL;
  }

  if (this->WriteFrames() != PLUS_SUCCESS)
  {
    LOG_ERROR(this->GetDeviceId() << ": Failed to write snapshot frame");
    return PLUS_FAIL;
  }

  this->TotalFramesRecorded += 1;

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusVirtualCapture::WriteFrames(bool force)
{
  if (!this->IsHeaderPrepared && this->RecordedFrames->GetNumberOfTrackedFrames() != 0)
  {
    if (this->Writer->PrepareHeader() != PLUS_SUCCESS)
    {
      LOG_ERROR("Unable to prepare header");
      this->StopRecording();
      return PLUS_FAIL;
    }
    this->IsHeaderPrepared = true;
  }

  if (this->RecordedFrames->GetNumberOfTrackedFrames() == 0)
  {
    return PLUS_SUCCESS;
  }

  this->SetIsData3D(this->RecordedFrames->GetTrackedFrame(0)->GetFrameSize()[2] > 1);

  if (force || !this->IsFrameBuffered() ||
      (this->IsFrameBuffered() && this->RecordedFrames->GetNumberOfTrackedFrames() > this->GetFrameBufferSize()))
  {
    if (this->Writer->AppendImagesToHeader() != PLUS_SUCCESS)
    {
      LOG_ERROR("Unable to append image data to header.");
      this->StopRecording();
      return PLUS_FAIL;
    }
    if (this->Writer->WriteImages() != PLUS_SUCCESS)
    {
      LOG_ERROR("Unable to append images. Stopping recording at timestamp: " << LastAlreadyRecordedFrameTimestamp);
      this->StopRecording();
      return PLUS_FAIL;
    }

    this->ClearRecordedFrames();
  }

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
int vtkPlusVirtualCapture::OutputChannelCount() const
{
  // Even though we fake one output channel for easy GetTrackedFrame ability,
  //  we shouldn't return actual output channel size
  return 0;
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusVirtualCapture::GetInputTrackedFrame(PlusTrackedFrame& aFrame)
{
  if (this->OutputChannels.empty())
  {
    LOG_ERROR("No output channels defined");
    return PLUS_FAIL;
  }

  return this->OutputChannels[0]->GetTrackedFrame(aFrame);
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusVirtualCapture::GetInputTrackedFrameListSampled(double& lastAlreadyRecordedFrameTimestamp, double& nextFrameToBeRecordedTimestamp, vtkPlusTrackedFrameList* recordedFrames, double requestedFramePeriodSec, double maxProcessingTimeSec)
{
  if (this->OutputChannels.empty())
  {
    LOG_ERROR("No output channels defined");
    return PLUS_FAIL;
  }

  return this->OutputChannels[0]->GetTrackedFrameListSampled(lastAlreadyRecordedFrameTimestamp, nextFrameToBeRecordedTimestamp, recordedFrames, requestedFramePeriodSec, maxProcessingTimeSec);
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusVirtualCapture::GetLatestInputItemTimestamp(double& timestamp)
{
  if (this->OutputChannels.empty())
  {
    LOG_ERROR("No output channels defined");
    return PLUS_FAIL;
  }
  return this->OutputChannels[0]->GetLatestTimestamp(timestamp);
}
