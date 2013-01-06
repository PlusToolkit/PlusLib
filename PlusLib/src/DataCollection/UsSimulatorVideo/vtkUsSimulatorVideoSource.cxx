/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"
#include "vtkImageData.h"
#include "vtkObjectFactory.h"
#include "vtkPlusStreamBuffer.h"
#include "vtkTrackedFrameList.h"
#include "vtkPlusStreamTool.h"
#include "vtkUsSimulatorVideoSource.h"

vtkCxxRevisionMacro(vtkUsSimulatorVideoSource, "$Revision: 1.0$");
vtkStandardNewMacro(vtkUsSimulatorVideoSource);

//----------------------------------------------------------------------------
vtkUsSimulatorVideoSource::vtkUsSimulatorVideoSource()
{
  this->Tracker = NULL;

  // Create and set up US simulator
  this->UsSimulator = NULL;
  vtkSmartPointer<vtkUsSimulatorAlgo> usSimulator = vtkSmartPointer<vtkUsSimulatorAlgo>::New();
  this->SetUsSimulator(usSimulator);

  // Create transform repository
  this->TransformRepository = NULL;
  vtkSmartPointer<vtkTransformRepository> transformRepository = vtkSmartPointer<vtkTransformRepository>::New();
  this->SetTransformRepository(transformRepository);

  this->RequireDeviceImageOrientationInDeviceSetConfiguration = true;
  this->RequireFrameBufferSizeInDeviceSetConfiguration = true;
  this->RequireAcquisitionRateInDeviceSetConfiguration = false;
  this->RequireAveragedItemsForFilteringInDeviceSetConfiguration = false;
  this->RequireLocalTimeOffsetSecInDeviceSetConfiguration = false;
  this->RequireUsImageOrientationInDeviceSetConfiguration = true;
  this->RequireRfElementInDeviceSetConfiguration = true;
}

//----------------------------------------------------------------------------
vtkUsSimulatorVideoSource::~vtkUsSimulatorVideoSource()
{ 
  if (!this->Connected)
  {
    this->Disconnect();
  }

  this->SetTracker(NULL);
  this->SetUsSimulator(NULL);
  this->SetTransformRepository(NULL);
}

//----------------------------------------------------------------------------
void vtkUsSimulatorVideoSource::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}

//----------------------------------------------------------------------------
PlusStatus vtkUsSimulatorVideoSource::InternalGrab()
{
  //LOG_TRACE("vtkUsSimulatorVideoSource::InternalGrab");

  if (!this->Tracker)
  {
    LOG_ERROR("Tracker not set to US simulator video source!");
    return PLUS_FAIL;
  }

  // Get latest tracker timestamp
  double latestTrackerTimestamp = 0;
  
  vtkPlusStreamTool* firstActiveTool = NULL; 
  if ( this->GetTracker()->GetFirstActiveTool(firstActiveTool) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to get most recent timestamp from tracker buffer - there is no active tool!"); 
    return PLUS_FAIL; 
  }

  vtkPlusStreamBuffer* trackerBuffer = firstActiveTool->GetBuffer(); 
  if (trackerBuffer->GetNumberOfItems()==0)
  {
    LOG_DEBUG("The tracking buffer is empty, we cannot generate a simulated image yet");
    return PLUS_SUCCESS;
  }

  BufferItemUidType uid = trackerBuffer->GetLatestItemUidInBuffer(); 
  if ( uid > 1 )
  {
    // Always use the latestItemUid - 1 to be able to interpolate transforms
    uid = uid - 1; 
  }

  if ( trackerBuffer->GetTimeStamp(uid, latestTrackerTimestamp) != ITEM_OK )
  {
    LOG_WARNING("Unable to get timestamp from default tool tracker buffer with UID: " << uid); 
    return PLUS_FAIL;
  }

  // The sampling rate is constant, so to have a constant frame rate we have to increase the FrameNumber by a constant.
  // For simplicity, we increase it always by 1.
  this->FrameNumber++;

  // Get image to tracker transform from the tracker
  TrackedFrame trackedFrame;
  if (this->Tracker->GetTrackedFrame(latestTrackerTimestamp, trackedFrame) != PLUS_SUCCESS)
  {
    LOG_ERROR("Unable to get tracked frame from the tracker with timestamp" << latestTrackerTimestamp);
    return PLUS_FAIL;
  }

  if ( this->TransformRepository->SetTransforms(trackedFrame) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to set repository transforms from tracked frame!"); 
    return PLUS_FAIL;
  }

  PlusTransformName referenceToImageTransformName(this->UsSimulator->GetReferenceCoordinateFrame(), this->UsSimulator->GetImageCoordinateFrame());
  vtkSmartPointer<vtkMatrix4x4> referenceToImageTransformMatrix = vtkSmartPointer<vtkMatrix4x4>::New();   
  if (this->TransformRepository->GetTransform(referenceToImageTransformName, referenceToImageTransformMatrix) != PLUS_SUCCESS)
  {
    std::string strTransformName; 
    referenceToImageTransformName.GetTransformName(strTransformName); 
    LOG_ERROR("Failed to get transform from repository: " << strTransformName ); 
    return PLUS_FAIL;
  }

  // Get the simulated US image
  this->UsSimulator->SetModelToImageMatrix(referenceToImageTransformMatrix);
  this->UsSimulator->Update();

  PlusStatus status = this->GetBuffer()->AddItem(
    this->UsSimulator->GetOutput(), this->GetDeviceImageOrientation(), US_IMG_BRIGHTNESS, this->FrameNumber, latestTrackerTimestamp, latestTrackerTimestamp);

  this->Modified();
  return status;
}

//----------------------------------------------------------------------------
PlusStatus vtkUsSimulatorVideoSource::InternalConnect()
{
  LOG_TRACE("vtkUsSimulatorVideoSource::InternalConnect"); 

  // Set to default MF internal image orientation
  this->SetDeviceImageOrientation(US_IMG_ORIENT_MF); 

  this->GetBuffer()->Clear();
  int frameSize[2]={0,0};
  if (this->UsSimulator->GetFrameSize(frameSize)!=PLUS_SUCCESS)
  {
    LOG_ERROR("Failed to initialize buffer, frame size is unknown");
    return PLUS_FAIL;
  }
  this->GetBuffer()->SetFrameSize(frameSize);

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkUsSimulatorVideoSource::InternalDisconnect()
{
  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkUsSimulatorVideoSource::ReadConfiguration(vtkXMLDataElement* config)
{
  LOG_TRACE("vtkUsSimulatorVideoSource::ReadConfiguration"); 
  if ( config == NULL )
  {
    LOG_ERROR("Unable to configure Saved Data video source! (XML data element is NULL)"); 
    return PLUS_FAIL; 
  }

  // Read superclass configuration
  Superclass::ReadConfiguration(config); 

  vtkXMLDataElement* imageAcquisitionConfig = this->FindThisDeviceElement(config);
  if (imageAcquisitionConfig == NULL) 
  {
    LOG_ERROR("Unable to find US simulator device element in configuration XML structure!");
    return PLUS_FAIL;
  }

  // Read US simulator configuration
  if ( !this->UsSimulator
    || this->UsSimulator->ReadConfiguration(config) != PLUS_SUCCESS)
  {
    LOG_ERROR("Failed to read US simulator configuration!");
    return PLUS_FAIL;
  }

  // Read transform repository configuration
  if ( !this->TransformRepository
    || this->TransformRepository->ReadConfiguration(config) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to read transform repository configuration!"); 
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}