/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/ 

#include "vtkDisplayableObject.h"

#include "vtkActor.h"
#include "vtkImageActor.h"
#include "vtkMapper.h"
#include "vtkProperty.h"
#include "vtkXMLUtilities.h"
#include "vtkObjectFactory.h"

//-----------------------------------------------------------------------------

vtkStandardNewMacro(vtkDisplayableObject);

vtkCxxSetObjectMacro(vtkDisplayableObject, Actor, vtkProp3D);

//-----------------------------------------------------------------------------

vtkDisplayableObject::vtkDisplayableObject()
{
  this->STLModelFileName = NULL;
  this->ModelToObjectTransform = NULL;
  this->Actor = NULL;
  this->ObjectCoordinateFrame = NULL;

  this->LastOpacity = 1.0;

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
  bool mapperOK = false;
  vtkActor* actor = dynamic_cast<vtkActor*>(this->Actor);
  if ( (actor && actor->GetMapper()) || (!actor) )
  {
    mapperOK = true;
  }

  return ( mapperOK && this->Displayable );
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

  // Opacity
	double opacity = 0.0; 
	if ( aConfig->GetScalarAttribute("Opacity", opacity) )
	{
    this->LastOpacity = opacity;
	}

  // If model then read its properties
  const char* type = aConfig->GetAttribute("Type");
  if (STRCASECMP(type, "Model") == 0)
  {
    const char* modelFileName = aConfig->GetAttribute("File");
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
		  if ( aConfig->GetVectorAttribute("ModelToObjectTransform", 16, ModelToObjectTransformMatrixValue) )
		  {
        this->ModelToObjectTransform->Identity();
        this->ModelToObjectTransform->Concatenate(ModelToObjectTransformMatrixValue);
		  }
    }
  }
  else
  {
    LOG_ERROR("Invalid type! Should be 'Model'");
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

void vtkDisplayableObject::SetOpacity(double aOpacity)
{
  LOG_TRACE("vtkDisplayableObject::SetOpacity(" << aOpacity << ")");

  vtkActor* actor = dynamic_cast<vtkActor*>(this->Actor);
  if (actor)
  {
    actor->GetProperty()->SetOpacity(aOpacity);
    return;
  }

  vtkImageActor* imageActor = dynamic_cast<vtkImageActor*>(this->Actor);
  if (imageActor)
  {
    imageActor->SetOpacity(aOpacity);
  }
}

//-----------------------------------------------------------------------------

double vtkDisplayableObject::GetOpacity()
{
  vtkActor* actor = dynamic_cast<vtkActor*>(this->Actor);
  if (actor)
  {
    return actor->GetProperty()->GetOpacity();
  }

  vtkImageActor* imageActor = dynamic_cast<vtkImageActor*>(this->Actor);
  if (imageActor)
  {
    return imageActor->GetOpacity();
  }

  return -1.0;
}

//-----------------------------------------------------------------------------

void vtkDisplayableObject::SetColor(double aR, double aG, double aB)
{
  LOG_TRACE("vtkDisplayableObject::SetColor(" << aR << ", " << aG << ", " << aB << ")");

  vtkActor* actor = dynamic_cast<vtkActor*>(this->Actor);
  if (actor)
  {
    actor->GetProperty()->SetColor(aR, aG, aB);
  }
  else
  {
    LOG_WARNING("Cannot set color of image actor!");
  }
}

//-----------------------------------------------------------------------------

void vtkDisplayableObject::SetMapper(vtkMapper* aMapper)
{
  LOG_TRACE("vtkDisplayableObject::SetMapper");

  vtkActor* actor = dynamic_cast<vtkActor*>(this->Actor);
  if (actor)
  {
    actor->SetMapper(aMapper);
  }
  else
  {
    LOG_WARNING("Cannot set mapper to image actor!");
  }
}
