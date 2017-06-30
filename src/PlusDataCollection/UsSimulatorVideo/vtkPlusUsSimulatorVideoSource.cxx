/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"
#include "vtkImageData.h"
#include "vtkObjectFactory.h"
#include "vtkPlusTrackedFrameList.h"
#include "vtkPlusChannel.h"
#include "vtkPlusDataSource.h"
#include "vtkPlusUsSimulatorVideoSource.h"
#include "vtkPlusTransformRepository.h"
#include "vtkPlusUsImagingParameters.h"
#include "vtkPlusUsScanConvertLinear.h"
#include "vtkPlusUsScanConvertCurvilinear.h"

vtkStandardNewMacro(vtkPlusUsSimulatorVideoSource);

//----------------------------------------------------------------------------
vtkPlusUsSimulatorVideoSource::vtkPlusUsSimulatorVideoSource()
: UsSimulator(NULL)
, LastProcessedTrackingDataTimestamp(0)
, GracePeriodLogLevel(vtkPlusLogger::LOG_LEVEL_DEBUG)
{
  // Create and set up US simulator
  vtkSmartPointer<vtkPlusUsSimulatorAlgo> usSimulator = vtkSmartPointer<vtkPlusUsSimulatorAlgo>::New();
  this->SetUsSimulator(usSimulator);

  // Create transform repository
  vtkSmartPointer<vtkPlusTransformRepository> transformRepository = vtkSmartPointer<vtkPlusTransformRepository>::New();
  this->GetUsSimulator()->SetTransformRepository(transformRepository);

  this->RequireImageOrientationInConfiguration = true;

  // No callback function provided by the device, so the data capture thread will be used to poll the hardware and add new items to the buffer
  this->StartThreadForInternalUpdates = true;
}

//----------------------------------------------------------------------------
vtkPlusUsSimulatorVideoSource::~vtkPlusUsSimulatorVideoSource()
{ 
  if (!this->Connected)
  {
    this->Disconnect();
  }

  this->SetUsSimulator(NULL);
}

//----------------------------------------------------------------------------
void vtkPlusUsSimulatorVideoSource::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusUsSimulatorVideoSource::InternalUpdate()
{
  //LOG_TRACE("vtkPlusUsSimulatorVideoSource::InternalUpdate");

  if( this->InputChannels.size() != 1 )
  {
    LOG_ERROR("vtkPlusUsSimulatorVideoSource device requires exactly 1 input stream (that contains the tracking data). Check configuration.");
    return PLUS_FAIL;
  }

  if( this->HasGracePeriodExpired() )
  {
    this->GracePeriodLogLevel = vtkPlusLogger::LOG_LEVEL_WARNING;
  }

  // Get image to tracker transform from the tracker (only request 1 frame, the latest)
  vtkSmartPointer<vtkPlusTrackedFrameList> trackingFrames = vtkSmartPointer<vtkPlusTrackedFrameList>::New();  
  if (!this->InputChannels[0]->GetTrackingDataAvailable())
  {
    LOG_DEBUG("Simulated US image is not generated, as no tracking data is available yet. Device ID: " << this->GetDeviceId() ); 
    return PLUS_SUCCESS;
  }
  double oldestTrackingTimestamp(0);
  if (this->InputChannels[0]->GetOldestTimestamp(oldestTrackingTimestamp) == PLUS_SUCCESS)
  {
    if (this->LastProcessedTrackingDataTimestamp < oldestTrackingTimestamp)
    {
      LOG_INFO("Simulated US image generation started. No tracking data was available between " << this->LastProcessedTrackingDataTimestamp << "-" << oldestTrackingTimestamp <<
        "sec, therefore no simulated images were generated during this time period.");
      this->LastProcessedTrackingDataTimestamp = oldestTrackingTimestamp;
    }
  }
  if ( this->InputChannels[0]->GetTrackedFrameList(this->LastProcessedTrackingDataTimestamp, trackingFrames, 1) != PLUS_SUCCESS )
  {
    LOG_ERROR("Error while getting tracked frame list from data collector during capturing. Last recorded timestamp: " << std::fixed << this->LastProcessedTrackingDataTimestamp << ". Device ID: " << this->GetDeviceId() ); 
    this->LastProcessedTrackingDataTimestamp = vtkPlusAccurateTimer::GetSystemTime(); // forget about the past, try to add frames that are acquired from now on
    return PLUS_FAIL;
  }
  if (trackingFrames->GetNumberOfTrackedFrames() < 1)
  {
    LOG_DYNAMIC("Simulated US image generation is skipped, as as no updated tracking data has become available since the last generated image (at "<<this->LastProcessedTrackingDataTimestamp<<"). Probably the tracker device acquisition rate is lower than the simulator acquisition rate. Device ID: " << this->GetDeviceId(), this->GracePeriodLogLevel );
    return PLUS_FAIL;
  }
  PlusTrackedFrame* trackedFrame = trackingFrames->GetTrackedFrame(0);
  if (trackedFrame == NULL)
  {
    LOG_ERROR("Error while getting tracked frame from data collector during capturing. Last recorded timestamp: " << std::fixed << this->LastProcessedTrackingDataTimestamp << ". Device ID: " << this->GetDeviceId() ); 
    return PLUS_FAIL;
  }

  // Get latest tracker timestamp
  double latestTrackerTimestamp = trackedFrame->GetTimestamp();
  
  if( this->OutputChannels.empty() )
  {
    LOG_ERROR("No output channels defined" );
    return PLUS_FAIL;
  }
  vtkPlusChannel* outputChannel=this->OutputChannels[0];
  double latestFrameAlreadyAddedTimestamp=0;
  outputChannel->GetMostRecentTimestamp(latestFrameAlreadyAddedTimestamp);
  if (latestFrameAlreadyAddedTimestamp>=latestTrackerTimestamp)
  {
    // simulated frame has been already generated for this timestamp
    return PLUS_SUCCESS;
  }

  // The sampling rate is constant, so to have a constant frame rate we have to increase the FrameNumber by a constant.
  // For simplicity, we increase it always by 1.
  this->FrameNumber++;
  
  if ( this->UsSimulator->GetTransformRepository()->SetTransforms(*trackedFrame) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to set repository transforms from tracked frame!"); 
    return PLUS_FAIL;
  }

  // Get the simulated US image  
  this->UsSimulator->Modified(); // Signal that the transforms have changed so we need to recompute
  this->UsSimulator->Update();

  vtkPlusDataSource* aSource(NULL);
  if( outputChannel->GetVideoSource(aSource) != PLUS_SUCCESS )
  {
    LOG_ERROR("Unable to retrieve the video source in the USSimulator device.");
    return PLUS_FAIL;
  }

  PlusStatus status = aSource->AddItem(
    this->UsSimulator->GetOutput(), aSource->GetInputImageOrientation(), US_IMG_BRIGHTNESS, this->FrameNumber, latestTrackerTimestamp, latestTrackerTimestamp);

  this->Modified();
  return status;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusUsSimulatorVideoSource::InternalConnect()
{
  LOG_TRACE("vtkPlusUsSimulatorVideoSource::InternalConnect"); 

  if( this->OutputChannels.empty() )
  {
    LOG_ERROR("No output channels defined" );
    return PLUS_FAIL;
  }
  vtkPlusChannel* outputChannel=this->OutputChannels[0];

  vtkPlusDataSource* aSource(NULL);
  if( outputChannel->GetVideoSource(aSource) != PLUS_SUCCESS )
  {
    LOG_ERROR("Unable to retrieve the video source in the USSimulator device.");
    return PLUS_FAIL;
  }

  // Set to default MF output image orientation
  aSource->SetOutputImageOrientation(US_IMG_ORIENT_MF);
  aSource->Clear();
  int frameSize[3]={0,0,1};
  if (this->UsSimulator->GetFrameSize(frameSize)!=PLUS_SUCCESS)
  {
    LOG_ERROR("Failed to initialize buffer, frame size is unknown");
    return PLUS_FAIL;
  }
  aSource->SetInputFrameSize(frameSize);
  
  this->LastProcessedTrackingDataTimestamp = 0;

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusUsSimulatorVideoSource::InternalDisconnect()
{
  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusUsSimulatorVideoSource::ReadConfiguration(vtkXMLDataElement* rootConfigElement)
{
  LOG_TRACE("vtkPlusUsSimulatorVideoSource::ReadConfiguration"); 
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_READING(deviceConfig, rootConfigElement);

  // Read US simulator configuration
  if ( !this->UsSimulator
    || this->UsSimulator->ReadConfiguration(deviceConfig) != PLUS_SUCCESS)
  {
    LOG_ERROR("Failed to read US simulator configuration!");
    return PLUS_FAIL;
  }

  // Read transform repository configuration
  if ( !this->UsSimulator->GetTransformRepository()
    || this->UsSimulator->GetTransformRepository()->ReadConfiguration(rootConfigElement) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to read transform repository configuration!"); 
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusUsSimulatorVideoSource::NotifyConfigured()
{
  if( this->OutputChannels.size() > 1 )
  {
    LOG_WARNING("vtkPlusUsSimulatorVideoSource is expecting one output channel and there are " << this->OutputChannels.size() << " channels. First output channel will be used.");
  }

  if( this->OutputChannels.empty() )
  {
    LOG_ERROR("No output channels defined for vtkPlusUsSimulatorVideoSource. Cannot proceed." );
    this->SetCorrectlyConfigured(false);
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusUsSimulatorVideoSource::RequestImagingParameterChange()
{
  //TODO: Make this dynamic
  if (this->RequestedImagingParameters->IsSet(vtkPlusUsImagingParameters::KEY_DEPTH))
  {
    vtkPlusUsScanConvert* scanConverter = this->UsSimulator->GetRfProcessor()->GetScanConverter();
    vtkPlusUsScanConvertLinear* linearScanConverter = vtkPlusUsScanConvertLinear::SafeDownCast(scanConverter);
    vtkPlusUsScanConvertCurvilinear* curvilinearScanConverter = vtkPlusUsScanConvertCurvilinear::SafeDownCast(scanConverter);
    if (linearScanConverter)
    {
      linearScanConverter->SetImagingDepthMm(this->RequestedImagingParameters->GetDepthMm());
    }
    else if (curvilinearScanConverter)
    {
      curvilinearScanConverter->SetRadiusStopMm(
        curvilinearScanConverter->GetRadiusStartMm() + this->RequestedImagingParameters->GetDepthMm() );
    }
  }
  else if (this->RequestedImagingParameters->IsSet(vtkPlusUsImagingParameters::KEY_FREQUENCY))
  {
    this->UsSimulator->SetFrequencyMhz(this->RequestedImagingParameters->GetFrequencyMhz());
  }

  return PLUS_SUCCESS;
}
