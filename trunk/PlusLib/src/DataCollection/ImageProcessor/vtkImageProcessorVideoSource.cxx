/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"
#include "vtkImageProcessorVideoSource.h"
#include "TrackedFrame.h"
#include "vtkBoneEnhancer.h"
#include "vtkObjectFactory.h"
#include "vtkPlusChannel.h"
#include "vtkPlusDataSource.h"
#include "vtkTrackedFrameList.h"
#include "vtkTrackedFrameProcessor.h"
#include "vtkTransformRepository.h"
#include "vtksys/SystemTools.hxx"

//----------------------------------------------------------------------------

vtkStandardNewMacro(vtkImageProcessorVideoSource);

//----------------------------------------------------------------------------
vtkImageProcessorVideoSource::vtkImageProcessorVideoSource()
: vtkPlusDevice()
, LastProcessedInputDataTimestamp(0)
, EnableProcessing(true)
, ProcessingAlgorithmAccessMutex(vtkSmartPointer<vtkRecursiveCriticalSection>::New())
, GracePeriodLogLevel(vtkPlusLogger::LOG_LEVEL_DEBUG)
, ProcessorAlgorithm(NULL)
{
  this->MissingInputGracePeriodSec=2.0;

  // Create transform repository
  this->TransformRepository = vtkTransformRepository::New();
  
  // The data capture thread will be used to regularly read the frames and process them
  this->StartThreadForInternalUpdates = true;
}

//----------------------------------------------------------------------------
vtkImageProcessorVideoSource::~vtkImageProcessorVideoSource()
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
void vtkImageProcessorVideoSource::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}

//----------------------------------------------------------------------------
PlusStatus vtkImageProcessorVideoSource::ReadConfiguration( vtkXMLDataElement* rootConfigElement)
{
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_READING(deviceConfig, rootConfigElement);
  XML_READ_BOOL_ATTRIBUTE_OPTIONAL(EnableProcessing, deviceConfig);

  // Read transform repository configuration
  if (this->TransformRepository->ReadConfiguration(rootConfigElement) != PLUS_SUCCESS )
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
  for (int nestedElemIndex=0; nestedElemIndex<numberOfNestedElements; ++nestedElemIndex) 
  {
    vtkXMLDataElement* processorElement = deviceConfig->GetNestedElement(nestedElemIndex);

    if ((processorElement == NULL) || (STRCASECMP(vtkTrackedFrameProcessor::GetTagName(), processorElement->GetName()))) 
    {
      // not a processor element, ignore it
      continue;
    }

    if (this->ProcessorAlgorithm != NULL)
    {
      LOG_WARNING("Multiple "<<processorElement->GetName()<<" elements found in ImageProcessor configuration. Only the first one is used, all others are ignored");
      break;
    }

    // Verify type
    const char* processorType = processorElement->GetAttribute("Type");
    if (processorType==NULL)
    {
      LOG_ERROR("Type attribute of Processor element is missing");
      return PLUS_FAIL;
    }

    // Instantiate processor corresponding to the specified type
    vtkSmartPointer<vtkBoneEnhancer> boneEnhancer = vtkSmartPointer<vtkBoneEnhancer>::New();
    if (!(STRCASECMP(boneEnhancer->GetProcessorTypeName(), processorType))) 
    {
      boneEnhancer->SetTransformRepository(this->TransformRepository);
      boneEnhancer->ReadConfiguration(processorElement);
      this->ProcessorAlgorithm = boneEnhancer;
      this->ProcessorAlgorithm->Register(this);
    }
    else
    {
      LOG_ERROR("Unkwnon processor type: "<<processorType);
      return PLUS_FAIL;
    }
  }
  
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkImageProcessorVideoSource::WriteConfiguration( vtkXMLDataElement* rootConfig)
{
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_WRITING(deviceElement, rootConfig);
  deviceElement->SetAttribute("EnableCapturing", this->EnableProcessing ? "TRUE" : "FALSE" );
  
  // Write processor elements
  if (this->ProcessorAlgorithm!=NULL)
  {
     vtkXMLDataElement* processorElement = PlusXmlUtils::GetNestedElementWithName(deviceElement,vtkTrackedFrameProcessor::GetTagName());
     if (processorElement == NULL)
     {
       LOG_ERROR("Cannot find "<<vtkTrackedFrameProcessor::GetTagName()<<" element in XML tree!");
       return PLUS_FAIL;
     }
     this->ProcessorAlgorithm->WriteConfiguration(processorElement);
  }
  else
  {
    // Remove processor elements
    vtkXMLDataElement* processorElement = NULL;
    while (processorElement = deviceElement->FindNestedElementWithName(vtkTrackedFrameProcessor::GetTagName()))
    {
      deviceElement->RemoveNestedElement(processorElement);
    }
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkImageProcessorVideoSource::InternalConnect()
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
    LOG_WARNING("vtkImageProcessorVideoSource acquisition rate is not known");
  }

  this->LastProcessedInputDataTimestamp = 0;

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkImageProcessorVideoSource::InternalDisconnect()
{ 
  PlusLockGuard<vtkRecursiveCriticalSection> writerLock(this->ProcessingAlgorithmAccessMutex);
  this->EnableProcessing = false;  
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkImageProcessorVideoSource::InternalUpdate()
{
  if (!this->EnableProcessing)
  {
    // Capturing is disabled
    return PLUS_SUCCESS;
  }

  if( this->InputChannels.size() != 1 )
  {
    LOG_ERROR("ImageProcessor device requires exactly 1 input stream (that contains video data). Check configuration.");
    return PLUS_FAIL;
  }

  if( this->HasGracePeriodExpired() )
  {
    this->GracePeriodLogLevel = vtkPlusLogger::LOG_LEVEL_WARNING;
  }

  // Get image to tracker transform from the tracker (only request 1 frame, the latest)
  if (!this->InputChannels[0]->GetVideoDataAvailable())
  {
    LOG_DYNAMIC("Processed data is not generated, as no video data is available yet. Device ID: " << this->GetDeviceId(), this->GracePeriodLogLevel ); 
    return PLUS_SUCCESS;
  }
  double oldestTrackingTimestamp(0);
  if (this->InputChannels[0]->GetOldestTimestamp(oldestTrackingTimestamp) == PLUS_SUCCESS)
  {
    if (this->LastProcessedInputDataTimestamp < oldestTrackingTimestamp)
    {
      LOG_INFO("Simulated US image generation started. No tracking data was available between " << this->LastProcessedInputDataTimestamp << "-" << oldestTrackingTimestamp <<
        "sec, therefore no simulated images were generated during this time period.");
      this->LastProcessedInputDataTimestamp = oldestTrackingTimestamp;
    }
  }
  vtkSmartPointer<vtkTrackedFrameList> trackingFrames = vtkSmartPointer<vtkTrackedFrameList>::New();  
  if ( this->InputChannels[0]->GetTrackedFrameList(this->LastProcessedInputDataTimestamp, trackingFrames, 1) != PLUS_SUCCESS )
  {
    LOG_ERROR("Error while getting tracked frame list. Last recorded timestamp: " << std::fixed << this->LastProcessedInputDataTimestamp << ". Device ID: " << this->GetDeviceId() ); 
    this->LastProcessedInputDataTimestamp = vtkAccurateTimer::GetSystemTime(); // forget about the past, try to add frames that are acquired from now on
    return PLUS_FAIL;
  }
  if (trackingFrames->GetNumberOfTrackedFrames() < 1)
  {
    LOG_TRACE("Processed data is not generated, as no data is available. Device ID: " << this->GetDeviceId()); 
    return PLUS_FAIL;
  }
  TrackedFrame* trackedFrame = trackingFrames->GetTrackedFrame(0);
  if (trackedFrame == NULL)
  {
    LOG_ERROR("Error while getting tracked frame from data collector. Last recorded timestamp: " << std::fixed << this->LastProcessedInputDataTimestamp << ". Device ID: " << this->GetDeviceId() ); 
    return PLUS_FAIL;
  }

  
  if( this->OutputChannels.empty() )
  {
    LOG_ERROR("No output channels defined" );
    return PLUS_FAIL;
  }
  vtkPlusChannel* outputChannel=this->OutputChannels[0];
  double latestFrameAlreadyAddedTimestamp=0;
  outputChannel->GetMostRecentTimestamp(latestFrameAlreadyAddedTimestamp);

  this->ProcessorAlgorithm->SetInputFrames(trackingFrames);

  if (this->ProcessorAlgorithm->Update()!=PLUS_SUCCESS)
  {
    return PLUS_FAIL;
  }

  vtkPlusDataSource* aSource(NULL);
  if( outputChannel->GetVideoSource(aSource) != PLUS_SUCCESS )
  {
    LOG_ERROR("Unable to retrieve the video source in the USSimulator device.");
    return PLUS_FAIL;
  }

  PlusStatus status = PLUS_SUCCESS;

  vtkTrackedFrameList* processedFrames = this->ProcessorAlgorithm->GetOutputFrames();
  if (processedFrames==NULL)
  {
    LOG_ERROR("Failed to retrieve processed frames");
    return PLUS_FAIL;
  }

  for (int i=0; i<processedFrames->GetNumberOfTrackedFrames(); i++)
  {
    TrackedFrame* trackedFrame = processedFrames->GetTrackedFrame(i);
    // Generate unique frame number (not used for filtering, so the actual increment value does not matter)
    this->FrameNumber++;
    // Get latest tracker timestamp
    double frameTimestamp = trackedFrame->GetTimestamp();
    if (latestFrameAlreadyAddedTimestamp>=frameTimestamp)
    {
      // processed data has been already generated for this timestamp
      continue;
    }

    // If the buffer is empty, set the pixel type and frame size to the first received properties 
    if ( aSource->GetNumberOfItems() == 0 )
    {
      PlusVideoFrame* videoFrame=trackedFrame->GetImageData();
      if (videoFrame==NULL)
      {
        LOG_ERROR("Invalid video frame received, cannot use it to initialize the video buffer");
        return PLUS_FAIL;
      }
      aSource->SetPixelType( videoFrame->GetVTKScalarPixelType() );
      aSource->SetNumberOfScalarComponents( videoFrame->GetNumberOfScalarComponents() );
      aSource->SetImageType( videoFrame->GetImageType() );
      aSource->SetInputFrameSize( trackedFrame->GetFrameSize() );
    }

    TrackedFrame::FieldMapType customFields=trackedFrame->GetCustomFields();
    if (aSource->AddItem(trackedFrame->GetImageData(), this->FrameNumber, frameTimestamp, frameTimestamp, &customFields)!=PLUS_SUCCESS)
    {
      status = PLUS_FAIL;
    }
  }

  this->Modified();
  return status;
}

//-----------------------------------------------------------------------------
PlusStatus vtkImageProcessorVideoSource::NotifyConfigured()
{
  if( this->OutputChannels.empty() )
  {
    LOG_ERROR("No output channels defined for ImageProcessor" );
    this->SetCorrectlyConfigured(false);
    return PLUS_FAIL;
  }

  if( this->OutputChannels.size() > 1 )
  {
    LOG_WARNING("ImageProcessor is expecting one output channel and there are " << this->OutputChannels.size() << " channels. First output channel will be used.");
  }

  if( this->InputChannels.empty() )
  {
    LOG_ERROR("No input channel is set for ImageProcessor");
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
void vtkImageProcessorVideoSource::SetEnableProcessing( bool aValue )
{
  bool processingStartsNow = (!this->EnableProcessing && aValue);
  this->EnableProcessing = aValue;
  
  if (processingStartsNow)
  {
    this->LastProcessedInputDataTimestamp = 0.0;
    this->RecordingStartTime = vtkAccurateTimer::GetSystemTime(); // reset the starting time for the grace period
  }
}
