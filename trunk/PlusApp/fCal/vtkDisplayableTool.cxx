/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/ 

#include "vtkDisplayableTool.h"

#include "vtkActor.h"
#include "vtkXMLUtilities.h"
#include "vtkObjectFactory.h"

//-----------------------------------------------------------------------------

vtkStandardNewMacro(vtkDisplayableTool);

vtkCxxSetObjectMacro(vtkDisplayableTool, Actor, vtkActor);

//-----------------------------------------------------------------------------

vtkDisplayableTool::vtkDisplayableTool()
{
  this->STLModelFileName = NULL;
  this->ModelToToolTransform = NULL;
  this->Actor = NULL;

  vtkSmartPointer<vtkActor> toolActor = vtkSmartPointer<vtkActor>::New();
  this->SetActor(toolActor);

  vtkSmartPointer<vtkTransform> modelToToolTransform = vtkSmartPointer<vtkTransform>::New();
  modelToToolTransform->Identity();
  this->SetModelToToolTransform(modelToToolTransform);

  this->Displayable = true;
}

//-----------------------------------------------------------------------------

vtkDisplayableTool::~vtkDisplayableTool()
{
  this->SetSTLModelFileName(NULL);
  this->SetModelToToolTransform(NULL);
  this->SetActor(NULL);
}

//-----------------------------------------------------------------------------

bool vtkDisplayableTool::IsDisplayable()
{
  return ((this->Actor->GetMapper() != NULL) && this->Displayable);
}

//-----------------------------------------------------------------------------

PlusStatus vtkDisplayableTool::ReadConfiguration(vtkXMLDataElement* aConfig)
{
  LOG_TRACE("vtkDisplayableTool::ReadConfiguration");

  // Tool coordinate frame name
  const char* toolCoordinateFrame = aConfig->GetAttribute("ToolCoordinateFrame");
  if (toolCoordinateFrame == NULL)
  {
	  LOG_ERROR("ToolCoordinateFrame is not specified in DisplayableTool element of the configuration!");
    return PLUS_FAIL;     
  }

  // World coordinate frame name
  const char* worldCoordinateFrame = aConfig->GetAttribute("WorldCoordinateFrame");
  if (worldCoordinateFrame == NULL)
  {
	  LOG_ERROR("WorldCoordinateFrame is not specified in DisplayableTool element of the configuration!");
    return PLUS_FAIL;     
  }

  //this->ToolToWorldTransformName.

  vtkXMLDataElement* modelElement = aConfig->FindNestedElementWithName("Model");
  if (modelElement == NULL)
  {
    LOG_ERROR("Unable to find Model element in DisplayableTool section of the configuration!"); 
    return PLUS_FAIL;     
  }

  // World coordinate frame name
  const char* modelFileName = modelElement->GetAttribute("File");
  if (!modelFileName)
  {
    LOG_WARNING("Model file name not found - no model displayed");
    return PLUS_FAIL;
  }
  else if (STRCASECMP(modelFileName, "") != 0) // If model file name is not empty
  {
    // ModelToToolTransform stays identity if no model file has been found
		double modelToToolTransformMatrixValue[16] = {0};
		if ( modelElement->GetVectorAttribute("ModelToToolTransform", 16, modelToToolTransformMatrixValue) )
		{
			this->ModelToToolTransform->SetMatrix(modelToToolTransformMatrixValue);
		}
  }

  return PLUS_SUCCESS;
}
