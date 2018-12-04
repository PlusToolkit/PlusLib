/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"
#include "vtkPlusImageProcessorVideoSource.h"
#include "igsioTrackedFrame.h"
#include "vtkPlusBoneEnhancer.h"
#include "vtkPlusTransverseProcessEnhancer.h"
#include "vtkObjectFactory.h"
#include "vtkPlusChannel.h"
#include "vtkPlusDataSource.h"
#include "vtkIGSIOTrackedFrameList.h"
#include "vtkPlusTrackedFrameProcessor.h"
#include "vtkIGSIOTransformRepository.h"
#include "vtksys/SystemTools.hxx"

//----------------------------------------------------------------------------

vtkStandardNewMacro(vtkPlusImageProcessorVideoSource);

//----------------------------------------------------------------------------
vtkPlusImageProcessorVideoSource::vtkPlusImageProcessorVideoSource()
  : vtkPlusDevice()
  , LastProcessedInputDataTimestamp(0)
  , EnableProcessing(true)
  , ProcessingAlgorithmAccessMutex(vtkSmartPointer<vtkIGSIORecursiveCriticalSection>::New())
  , GracePeriodLogLevel(vtkIGSIOLogger::LOG_LEVEL_DEBUG)
  , ProcessorAlgorithm(NULL)
{
  this->MissingInputGracePeriodSec = 2.0;

  // Create transform repository
  this->TransformRepository = vtkIGSIOTransformRepository::New();

  // The data capture thread will be used to regularly read the frames and process them
  this->StartThreadForInternalUpdates = true;
}

//----------------------------------------------------------------------------
vtkPlusImageProcessorVideoSource::~vtkPlusImageProcessorVideoSource()
{
  if (this->TransformRepository)
  {
    this->TransformRepository->Delete();
    this->TransformRepository = NULL;
  }
  if (this->ProcessorAlgorithm)
  {
    this->ProcessorAlgorithm->Delete();
    this->ProcessorAlgorithm = NULL;
  }
}

//----------------------------------------------------------------------------
void vtkPlusImageProcessorVideoSource::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusImageProcessorVideoSource::ReadConfiguration(vtkXMLDataElement* rootConfigElement)
{
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_READING(deviceConfig, rootConfigElement);
  XML_READ_BOOL_ATTRIBUTE_OPTIONAL(EnableProcessing, deviceConfig);

  // Read transform repository configuration
  if (this->TransformRepository->ReadConfiguration(rootConfigElement) != PLUS_SUCCESS)
  {
    LOG_ERROR("Failed to read transform repository configuration");
    return PLUS_FAIL;
  }

  // Instantiate processor(s)
  if (this->ProcessorAlgorithm)
  {
    this->ProcessorAlgorithm->Delete();
    this->ProcessorAlgorithm = NULL;
  }
  int numberOfNestedElements = deviceConfig->GetNumberOfNestedElements();
  for (int nestedElemIndex = 0; nestedElemIndex < numberOfNestedElements; ++nestedElemIndex)
  {
    vtkXMLDataElement* processorElement = deviceConfig->GetNestedElement(nestedElemIndex);

    if ((processorElement == NULL) || (STRCASECMP(vtkPlusTrackedFrameProcessor::GetTagName(), processorElement->GetName())))
    {
      // not a processor element, ignore it
      continue;
    }

    if (this->ProcessorAlgorithm != NULL)
    {
      LOG_WARNING("Multiple " << processorElement->GetName() << " elements found in ImageProcessor configuration. Only the first one is used, all others are ignored");
      break;
    }

    // Verify type
    const char* processorType = processorElement->GetAttribute("Type");
    if (processorType == NULL)
    {
      LOG_ERROR("Type attribute of Processor element is missing");
      return PLUS_FAIL;
    }

    // Instantiate processor corresponding to the specified type
    vtkSmartPointer<vtkPlusBoneEnhancer> boneEnhancer = vtkSmartPointer<vtkPlusBoneEnhancer>::New();
    vtkSmartPointer<vtkPlusTransverseProcessEnhancer> TransverseProcessEnhancer = vtkSmartPointer<vtkPlusTransverseProcessEnhancer>::New();
    if (!(STRCASECMP(boneEnhancer->GetProcessorTypeName(), processorType)))
    {
      boneEnhancer->SetTransformRepository(this->TransformRepository);
      boneEnhancer->ReadConfiguration(processorElement);
      this->ProcessorAlgorithm = boneEnhancer;
      this->ProcessorAlgorithm->Register(this);
      break;                  // If only one processor is allowed per ImageProcessor class, we can break out when we find it.
    }
    else if (!(STRCASECMP(TransverseProcessEnhancer->GetProcessorTypeName(), processorType)))
    {
      TransverseProcessEnhancer->SetTransformRepository(this->TransformRepository);
      TransverseProcessEnhancer->ReadConfiguration(processorElement);
      this->ProcessorAlgorithm = TransverseProcessEnhancer;
      this->ProcessorAlgorithm->Register(this);
      break;
    }
    else
    {
      LOG_ERROR("Unknown processor type: " << processorType);
      return PLUS_FAIL;
    }
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusImageProcessorVideoSource::WriteConfiguration(vtkXMLDataElement* rootConfig)
{
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_WRITING(deviceElement, rootConfig);
  deviceElement->SetAttribute("EnableCapturing", this->EnableProcessing ? "TRUE" : "FALSE");

  // Write processor elements
  if (this->ProcessorAlgorithm != NULL)
  {
    vtkXMLDataElement* processorElement = igsioXmlUtils::GetNestedElementWithName(deviceElement, vtkPlusTrackedFrameProcessor::GetTagName());
    if (processorElement == NULL)
    {
      LOG_ERROR("Cannot find " << vtkPlusTrackedFrameProcessor::GetTagName() << " element in XML tree!");
      return PLUS_FAIL;
    }
    this->ProcessorAlgorithm->WriteConfiguration(processorElement);
  }
  else
  {
    // Remove processor elements
    vtkXMLDataElement* processorElement = NULL;
    while ((processorElement = deviceElement->FindNestedElementWithName(vtkPlusTrackedFrameProcessor::GetTagName())))
    {
      deviceElement->RemoveNestedElement(processorElement);
    }
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusImageProcessorVideoSource::InternalConnect()
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
    LOG_WARNING("vtkPlusImageProcessorVideoSource acquisition rate is not known");
  }

  this->LastProcessedInputDataTimestamp = 0;

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusImageProcessorVideoSource::InternalDisconnect()
{
  igsioLockGuard<vtkIGSIORecursiveCriticalSection> writerLock(this->ProcessingAlgorithmAccessMutex);
  this->EnableProcessing = false;
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusImageProcessorVideoSource::InternalUpdate()
{
  if (!this->EnableProcessing)
  {
    // Capturing is disabled
    return PLUS_SUCCESS;
  }

  if (this->InputChannels.size() != 1)
  {
    LOG_ERROR("ImageProcessor device requires exactly 1 input stream (that contains video data). Check configuration.");
    return PLUS_FAIL;
  }

  if (this->HasGracePeriodExpired())
  {
    this->GracePeriodLogLevel = vtkIGSIOLogger::LOG_LEVEL_WARNING;
  }

  // Get image to tracker transform from the tracker (only request 1 frame, the latest)
  if (!this->InputChannels[0]->GetVideoDataAvailable())
  {
    LOG_DYNAMIC("Processed data is not generated, as no video data is available yet. Device ID: " << this->GetDeviceId(), this->GracePeriodLogLevel);
    return PLUS_SUCCESS;
  }
  double oldestTrackingTimestamp(0);
  if (this->InputChannels[0]->GetOldestTimestamp(oldestTrackingTimestamp) == PLUS_SUCCESS)
  {
    if (this->LastProcessedInputDataTimestamp < oldestTrackingTimestamp)
    {
      LOG_INFO("Processed image generation started. No tracking data was available between " << this->LastProcessedInputDataTimestamp << "-" << oldestTrackingTimestamp <<
               "sec, therefore no processed images were generated during this time period.");
      this->LastProcessedInputDataTimestamp = oldestTrackingTimestamp;
    }
  }
  igsioTrackedFrame trackedFrame;
  if (this->InputChannels[0]->GetTrackedFrame(trackedFrame) != PLUS_SUCCESS)
  {
    LOG_ERROR("Error while getting latest tracked frame. Last recorded timestamp: " << std::fixed << this->LastProcessedInputDataTimestamp << ". Device ID: " << this->GetDeviceId());
    this->LastProcessedInputDataTimestamp = vtkIGSIOAccurateTimer::GetSystemTime(); // forget about the past, try to add frames that are acquired from now on
    return PLUS_FAIL;
  }

  LOG_TRACE("Image to be processed: timestamp=" << trackedFrame.GetTimestamp());

  if (this->OutputChannels.empty())
  {
    LOG_ERROR("No output channels defined");
    return PLUS_FAIL;
  }
  vtkPlusChannel* outputChannel = this->OutputChannels[0];
  double latestFrameAlreadyAddedTimestamp = 0;
  outputChannel->GetMostRecentTimestamp(latestFrameAlreadyAddedTimestamp);

  double frameTimestamp = trackedFrame.GetTimestamp();
  if (latestFrameAlreadyAddedTimestamp >= frameTimestamp)
  {
    // processed data has been already generated for this timestamp
    return PLUS_SUCCESS;
  }

  vtkSmartPointer<vtkIGSIOTrackedFrameList> trackingFrames = vtkSmartPointer<vtkIGSIOTrackedFrameList>::New();
  trackingFrames->AddTrackedFrame(&trackedFrame);
  this->ProcessorAlgorithm->SetInputFrames(trackingFrames);
  if (this->ProcessorAlgorithm->Update() != PLUS_SUCCESS)
  {
    return PLUS_FAIL;
  }

  vtkPlusDataSource* aSource(NULL);
  if (outputChannel->GetVideoSource(aSource) != PLUS_SUCCESS)
  {
    LOG_ERROR("Unable to retrieve the video source in the image processor device.");
    return PLUS_FAIL;
  }

  PlusStatus status = PLUS_SUCCESS;

  vtkIGSIOTrackedFrameList* processedFrames = this->ProcessorAlgorithm->GetOutputFrames();
  if (processedFrames == NULL || processedFrames->GetNumberOfTrackedFrames() < 1)
  {
    LOG_ERROR("Failed to retrieve processed frame");
    return PLUS_FAIL;
  }

  igsioTrackedFrame* processedTrackedFrame = processedFrames->GetTrackedFrame(0);
  // Generate unique frame number (not used for filtering, so the actual increment value does not matter)
  this->FrameNumber++;

  // If the buffer is empty, set the pixel type and frame size to the first received properties
  if (aSource->GetNumberOfItems() == 0)
  {
    igsioVideoFrame* videoFrame = processedTrackedFrame->GetImageData();
    if (videoFrame == NULL)
    {
      LOG_ERROR("Invalid video frame received, cannot use it to initialize the video buffer");
      return PLUS_FAIL;
    }
    aSource->SetPixelType(videoFrame->GetVTKScalarPixelType());
    unsigned int numberOfScalarComponents(1);
    if (videoFrame->GetNumberOfScalarComponents(numberOfScalarComponents) != PLUS_SUCCESS)
    {
      LOG_ERROR("Unable to retrieve number of scalar components.");
      return PLUS_FAIL;
    }
    aSource->SetNumberOfScalarComponents(numberOfScalarComponents);
    aSource->SetImageType(videoFrame->GetImageType());
    aSource->SetInputFrameSize(processedTrackedFrame->GetFrameSize());
  }

  igsioTrackedFrame::FieldMapType customFields = processedTrackedFrame->GetCustomFields();
  if (aSource->AddItem(processedTrackedFrame->GetImageData(), this->FrameNumber, frameTimestamp, frameTimestamp, &customFields) != PLUS_SUCCESS)
  {
    status = PLUS_FAIL;
  }

  this->Modified();
  return status;
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusImageProcessorVideoSource::NotifyConfigured()
{
  if (this->OutputChannels.empty())
  {
    LOG_ERROR("No output channels defined for ImageProcessor");
    this->SetCorrectlyConfigured(false);
    return PLUS_FAIL;
  }

  if (this->OutputChannels.size() > 1)
  {
    LOG_WARNING("ImageProcessor is expecting one output channel and there are " << this->OutputChannels.size() << " channels. First output channel will be used.");
  }

  if (this->InputChannels.empty())
  {
    LOG_ERROR("No input channel is set for ImageProcessor");
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
void vtkPlusImageProcessorVideoSource::SetEnableProcessing(bool aValue)
{
  bool processingStartsNow = (!this->EnableProcessing && aValue);
  this->EnableProcessing = aValue;

  if (processingStartsNow)
  {
    this->LastProcessedInputDataTimestamp = 0.0;
    this->RecordingStartTime = vtkIGSIOAccurateTimer::GetSystemTime(); // reset the starting time for the grace period
  }
}
