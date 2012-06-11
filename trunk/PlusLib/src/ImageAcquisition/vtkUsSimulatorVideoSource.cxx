/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"
#include "vtkUsSimulatorVideoSource.h"
#include "vtkImageData.h"
#include "vtkObjectFactory.h"
#include "vtkVideoBuffer.h"
#include "vtkTrackedFrameList.h"


vtkCxxRevisionMacro(vtkUsSimulatorVideoSource, "$Revision: 1.0$");
vtkStandardNewMacro(vtkUsSimulatorVideoSource);

//----------------------------------------------------------------------------
vtkUsSimulatorVideoSource::vtkUsSimulatorVideoSource()
{
  this->UsSimulator = NULL;
  vtkSmartPointer<vtkUsSimulatorAlgo> usSimulator = vtkSmartPointer<vtkUsSimulatorAlgo>::New();
  this->SetUsSimulator(usSimulator);

  this->TransformRepository = NULL;
  vtkSmartPointer<vtkTransformRepository> transformRepository = vtkSmartPointer<vtkTransformRepository>::New();
  this->SetTransformRepository(transformRepository);
}

//----------------------------------------------------------------------------
vtkUsSimulatorVideoSource::~vtkUsSimulatorVideoSource()
{ 
  if (!this->Connected)
  {
    this->Disconnect();
  }

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

  // Compute elapsed time since we restarted the timer and the current timestamp
  double elapsedTime = vtkAccurateTimer::GetSystemTime() - this->GetBuffer()->GetStartTime(); 

  // The sampling rate is constant, so to have a constant frame rate we have to increase the FrameNumber by a constant.
  // For simplicity, we increase it always by 1.
  this->FrameNumber++;

  // Get image to tracker transform from the tracker
  TrackedFrame trackedFrame;
  if (this->Tracker->GetTrackedFrame(elapsedTime, &trackedFrame) != PLUS_SUCCESS)
  {
    LOG_ERROR("Unable to get tracked frame from the tracker with timestamp" << elapsedTime);
    return PLUS_FAIL;
  }

  if ( this->TransformRepository->SetTransforms(trackedFrame) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to set repository transforms from tracked frame!"); 
    return PLUS_FAIL;
  }

  PlusTransformName imageToReferenceTransformName(this->UsSimulator->GetImageCoordinateFrame(), this->UsSimulator->GetReferenceCoordinateFrame());
  vtkSmartPointer<vtkMatrix4x4> imageToReferenceTransformMatrix = vtkSmartPointer<vtkMatrix4x4>::New();   
  if (this->TransformRepository->GetTransform(imageToReferenceTransformName, imageToReferenceTransformMatrix) != PLUS_SUCCESS)
  {
    std::string strTransformName; 
    imageToReferenceTransformName.GetTransformName(strTransformName); 
    LOG_ERROR("Failed to get transform from repository: " << strTransformName ); 
    return PLUS_FAIL;
  }

  // Get the simulated US image
  this->UsSimulator->SetModelToImageMatrix(imageToReferenceTransformMatrix);
  this->UsSimulator->Update();

  PlusStatus status = this->Buffer->AddItem(
    this->UsSimulator->GetOutput(), this->GetUsImageOrientation(), this->FrameNumber);

  this->Modified();
  return status;
}

//----------------------------------------------------------------------------
PlusStatus vtkUsSimulatorVideoSource::InternalConnect()
{
  LOG_TRACE("vtkUsSimulatorVideoSource::InternalConnect"); 

  // Set to default MF internal image orientation (sequence metafile reader always converts it to MF)
  this->SetUsImageOrientation(US_IMG_ORIENT_MF); 

  this->Buffer->Clear();
  this->Buffer->SetFrameSize( this->UsSimulator->GetFrameSize() ); 

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

	vtkXMLDataElement* dataCollectionConfig = config->FindNestedElementWithName("DataCollection");
	if (dataCollectionConfig == NULL)
  {
    LOG_ERROR("Cannot find DataCollection element in XML tree!");
		return PLUS_FAIL;
	}

  vtkXMLDataElement* imageAcquisitionConfig = dataCollectionConfig->FindNestedElementWithName("ImageAcquisition"); 
  if (imageAcquisitionConfig == NULL) 
  {
    LOG_ERROR("Unable to find ImageAcquisition element in configuration XML structure!");
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

//-----------------------------------------------------------------------------
PlusStatus vtkUsSimulatorVideoSource::WriteConfiguration(vtkXMLDataElement* config)
{
  LOG_TRACE("vtkUsSimulatorVideoSource::WriteConfiguration"); 

  // Write superclass configuration
  Superclass::WriteConfiguration(config); 

  if ( config == NULL )
  {
    LOG_ERROR("Config is invalid");
    return PLUS_FAIL;
  }

  vtkXMLDataElement* dataCollectionConfig = config->FindNestedElementWithName("DataCollection");
  if (dataCollectionConfig == NULL)
  {
    LOG_ERROR("Cannot find DataCollection element in XML tree!");
    return PLUS_FAIL;
  }

  vtkXMLDataElement* imageAcquisitionConfig = dataCollectionConfig->FindNestedElementWithName("ImageAcquisition"); 
  if (imageAcquisitionConfig == NULL) 
  {
    LOG_ERROR("Cannot find ImageAcquisition element in XML tree!");
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}

