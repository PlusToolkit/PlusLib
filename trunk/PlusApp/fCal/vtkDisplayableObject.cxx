/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/ 

#include "vtkDisplayableObject.h"

#include "vtkActor.h"
#include "vtkXMLUtilities.h"
#include "vtkObjectFactory.h"

//-----------------------------------------------------------------------------

vtkStandardNewMacro(vtkDisplayableObject);

vtkCxxSetObjectMacro(vtkDisplayableObject, Actor, vtkActor);

//-----------------------------------------------------------------------------

vtkDisplayableObject::vtkDisplayableObject()
{
  this->STLModelFileName = NULL;
  this->ModelToObjectTransform = NULL;
  this->Actor = NULL;
  this->ObjectCoordinateFrame = NULL;

  this->LastOpacity = 1.0;

  vtkSmartPointer<vtkActor> toolActor = vtkSmartPointer<vtkActor>::New();
  this->SetActor(toolActor);

  vtkSmartPointer<vtkTransform> ModelToObjectTransform = vtkSmartPointer<vtkTransform>::New();
  ModelToObjectTransform->Identity();
  this->SetModelToObjectTransform(ModelToObjectTransform);

  this->Displayable = true;
}

//-----------------------------------------------------------------------------

vtkDisplayableObject::~vtkDisplayableObject()
{
  this->SetSTLModelFileName(NULL);
  this->SetModelToObjectTransform(NULL);
  this->SetActor(NULL);
}

//-----------------------------------------------------------------------------

bool vtkDisplayableObject::IsDisplayable()
{
  return ((this->Actor->GetMapper() != NULL) && this->Displayable);
}

//-----------------------------------------------------------------------------

PlusStatus vtkDisplayableObject::ReadConfiguration(vtkXMLDataElement* aConfig)
{
  LOG_TRACE("vtkDisplayableObject::ReadConfiguration");

  // Object coordinate frame name
  const char* objectCoordinateFrame = aConfig->GetAttribute("ObjectCoordinateFrame");
  if (objectCoordinateFrame == NULL)
  {
	  LOG_ERROR("ObjectCoordinateFrame is not specified in DisplayableObject element of the configuration!");
    return PLUS_FAIL;     
  }
  this->SetObjectCoordinateFrame(objectCoordinateFrame);

  vtkXMLDataElement* modelElement = aConfig->FindNestedElementWithName("Model");
  if (modelElement == NULL)
  {
    LOG_ERROR("Unable to find Model element for DisplayableObject '" << objectCoordinateFrame << "' section of the configuration!"); 
    return PLUS_FAIL;     
  }

  // World coordinate frame name
  const char* modelFileName = modelElement->GetAttribute("File");
  if (!modelFileName)
  {
    LOG_WARNING("Model file name not found for '" << objectCoordinateFrame << "' - no model displayed");
    return PLUS_FAIL;
  }
  else if (STRCASECMP(modelFileName, "") != 0) // If model file name is not empty
  {
    this->SetSTLModelFileName(modelFileName);

    // ModelToObjectTransform stays identity if no model file has been found
		double ModelToObjectTransformMatrixValue[16] = {0};
		if ( modelElement->GetVectorAttribute("ModelToObjectTransform", 16, ModelToObjectTransformMatrixValue) )
		{
      this->ModelToObjectTransform->Identity();
      this->ModelToObjectTransform->Concatenate(ModelToObjectTransformMatrixValue);
		}
  }

  return PLUS_SUCCESS;
}
