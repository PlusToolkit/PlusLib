/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/ 

#include "vtkDisplayableObject.h"

#include "vtkActor.h"
#include "vtkImageActor.h"
#include "vtkAxesActor.h"
#include "vtkPolyDataMapper.h"
#include "vtkProperty.h"
#include "vtkSTLReader.h"
#include "vtkCylinderSource.h"
#include "vtkConeSource.h"
#include "vtkAppendPolyData.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkXMLUtilities.h"
#include "vtkObjectFactory.h"

//-----------------------------------------------------------------------------

vtkCxxRevisionMacro(vtkDisplayableObject, "$Revision: 1.0$");

vtkCxxSetObjectMacro(vtkDisplayableObject, Actor, vtkProp3D);

//----------------------------------------------------------------------------

vtkDisplayableObject* vtkDisplayableObject::New(const char* aType)
{
  if (aType==NULL)
  {
    LOG_ERROR("vtkDisplayableObject::New failed. Object type is not specified");
    return NULL;
  }
  if (STRCASECMP(aType, "Model") == 0)
  {
    return vtkDisplayableModel::New();
  }
  else if (STRCASECMP(aType, "Image") == 0)
  {
    return vtkDisplayableImage::New();
  }
  else if (STRCASECMP(aType, "Axes") == 0)
  {
    return vtkDisplayableAxes::New();
  }
  LOG_ERROR("vtkDisplayableObject::New failed. Unkonwn object type: "<<aType);
  return NULL;
}

//-----------------------------------------------------------------------------

vtkDisplayableObject::vtkDisplayableObject()
{
  this->Actor = NULL;
  this->ObjectCoordinateFrame = NULL;

  this->LastOpacity = 1.0;

  this->Displayable = true;
}

//-----------------------------------------------------------------------------

vtkDisplayableObject::~vtkDisplayableObject()
{
  this->SetActor(NULL);
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

  return PLUS_SUCCESS;
}



//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

vtkCxxRevisionMacro(vtkDisplayableModel, "$Revision: 1.0$");
vtkStandardNewMacro(vtkDisplayableModel);

//-----------------------------------------------------------------------------

vtkDisplayableModel::vtkDisplayableModel()
  : vtkDisplayableObject()
{
  this->STLModelFileName = NULL;
  this->ModelToObjectTransform = NULL;

  vtkSmartPointer<vtkTransform> ModelToObjectTransform = vtkSmartPointer<vtkTransform>::New();
  ModelToObjectTransform->Identity();
  this->SetModelToObjectTransform(ModelToObjectTransform);
}

//-----------------------------------------------------------------------------

vtkDisplayableModel::~vtkDisplayableModel()
{
  this->SetSTLModelFileName(NULL);
  this->SetModelToObjectTransform(NULL);
}

//-----------------------------------------------------------------------------

PlusStatus vtkDisplayableModel::ReadConfiguration(vtkXMLDataElement* aConfig)
{
  LOG_TRACE("vtkDisplayableModel::ReadConfiguration");

	if (Superclass::ReadConfiguration(aConfig) != PLUS_SUCCESS)
  {
    LOG_ERROR("Unable to read basic displayable object configuration!");
    return PLUS_FAIL;
  }

  const char* modelFileName = aConfig->GetAttribute("File");
  if (!modelFileName)
  {
    LOG_WARNING("Model file name not found for '" << this->ObjectCoordinateFrame << "' - no model displayed");
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

  // Load model
  this->Displayable = false;

  std::string objectName(this->ObjectCoordinateFrame);

  // Check if file name exists and file can be found
  std::string stlFileName = "";
  if ( (this->STLModelFileName == NULL) || (STRCASECMP(this->STLModelFileName, "") == 0) )
  {
    LOG_ERROR("No STL file name found for displayable object (" << objectName << ")");
    return PLUS_FAIL;
  }

  std::string searchResult = vtkPlusConfig::GetFirstFileFoundInConfigurationDirectory(this->STLModelFileName);
  if (STRCASECMP("", searchResult.c_str()) == 0)
  {
    LOG_WARNING("Displayable object (" << this->ObjectCoordinateFrame << ") model file is not found with name: " << this->STLModelFileName);
  }
  else
  {
    stlFileName = searchResult;
  }

  // Handle missing object models if possible
  if (stlFileName.empty())
  {
    if (objectName.find("Stylus") != std::string::npos) // If the tool name contains stylus but there is no model for it
    {
      LOG_INFO("No stylus model file found - default model will be displayed");

      if (SetDefaultStylusModel() != PLUS_SUCCESS)
      {
        LOG_WARNING("Failed to load default stylus model for displayable object '" << this->ObjectCoordinateFrame << "'");
      }

      this->Displayable = true;
    }
  }
  else
  // Load STL model as usual
  {
    vtkSmartPointer<vtkSTLReader> stlReader = vtkSmartPointer<vtkSTLReader>::New();
    stlReader->SetFileName(stlFileName.c_str());

    vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInputConnection(stlReader->GetOutputPort());

    vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();

    actor->SetMapper(mapper);
    this->SetActor(actor);
    this->SetOpacity( this->LastOpacity );

    if (objectName.find("Stylus") != std::string::npos) // Stylus is always black
    {
      this->SetColor(0.0, 0.0, 0.0);
    }

    this->Displayable = true;
  }

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

bool vtkDisplayableModel::IsDisplayable()
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

void vtkDisplayableModel::SetColor(double aR, double aG, double aB)
{
  LOG_TRACE("vtkDisplayableModel::SetColor(" << aR << ", " << aG << ", " << aB << ")");

  vtkActor* actor = dynamic_cast<vtkActor*>(this->Actor);
  if (actor)
  {
    actor->GetProperty()->SetColor(aR, aG, aB);
  }
  else
  {
    LOG_WARNING("Invalid actor - cannot set color!");
  }
}

//-----------------------------------------------------------------------------

void vtkDisplayableModel::SetOpacity(double aOpacity)
{
  LOG_TRACE("vtkDisplayableModel::SetOpacity(" << aOpacity << ")");

  vtkActor* actor = dynamic_cast<vtkActor*>(this->Actor);
  if (actor)
  {
    actor->GetProperty()->SetOpacity(aOpacity);
    return;
  }
  else
  {
    LOG_WARNING("Invalid actor - cannot set opacity!");
  }
}

//-----------------------------------------------------------------------------

double vtkDisplayableModel::GetOpacity()
{
  vtkActor* actor = dynamic_cast<vtkActor*>(this->Actor);
  if (actor)
  {
    return actor->GetProperty()->GetOpacity();
  }
  else
  {
    LOG_WARNING("Invalid actor - cannot get opacity!");
  }

  return -1.0;
}

//-----------------------------------------------------------------------------

PlusStatus vtkDisplayableModel::SetDefaultStylusModel()
{
  LOG_TRACE("vtkDisplayableModel::SetDefaultStylusModel");

  vtkActor* actor = dynamic_cast<vtkActor*>(this->Actor);
  if (actor == NULL)
  {
    LOG_ERROR("Unable to load stylus model to an uninitialized or invalid actor!");
    return PLUS_FAIL;
  }

  // Create default model
  vtkSmartPointer<vtkPolyDataMapper> stylusMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
  vtkSmartPointer<vtkCylinderSource> stylusBigCylinderSource = vtkSmartPointer<vtkCylinderSource>::New();
  stylusBigCylinderSource->SetRadius(3.0); // mm
  stylusBigCylinderSource->SetHeight(120.0); // mm
  stylusBigCylinderSource->SetCenter(0.0, 150.0, 0.0);
  vtkSmartPointer<vtkCylinderSource> stylusSmallCylinderSource = vtkSmartPointer<vtkCylinderSource>::New();
  stylusSmallCylinderSource->SetRadius(1.5); // mm
  stylusSmallCylinderSource->SetHeight(80.0); // mm
  stylusSmallCylinderSource->SetCenter(0.0, 50.0, 0.0);
  vtkSmartPointer<vtkConeSource> resultConeSource = vtkSmartPointer<vtkConeSource>::New();
  resultConeSource->SetRadius(1.5); // mm
  resultConeSource->SetHeight(10.0); //mm
  vtkSmartPointer<vtkTransform> coneTransform = vtkSmartPointer<vtkTransform>::New();
  coneTransform->Identity();
  coneTransform->RotateZ(-90.0);
  coneTransform->Translate(-5.0, 0.0, 0.0);
  vtkSmartPointer<vtkTransformPolyDataFilter> coneTransformFilter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
  coneTransformFilter->AddInputConnection(resultConeSource->GetOutputPort());
  coneTransformFilter->SetTransform(coneTransform);

  vtkSmartPointer<vtkAppendPolyData> appendFilter = vtkSmartPointer<vtkAppendPolyData>::New();
  appendFilter->AddInputConnection(stylusBigCylinderSource->GetOutputPort());
  appendFilter->AddInputConnection(stylusSmallCylinderSource->GetOutputPort());
  appendFilter->AddInputConnection(coneTransformFilter->GetOutputPort());
  vtkSmartPointer<vtkTransform> stylusTransform = vtkSmartPointer<vtkTransform>::New();
  stylusTransform->Identity();
  stylusTransform->RotateZ(90.0);
  vtkSmartPointer<vtkTransformPolyDataFilter> stylusTransformFilter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
  stylusTransformFilter->AddInputConnection(appendFilter->GetOutputPort());
  stylusTransformFilter->SetTransform(stylusTransform);

  stylusMapper->SetInputConnection(stylusTransformFilter->GetOutputPort());
  actor->SetMapper(stylusMapper);
  actor->GetProperty()->SetColor(0.0, 0.0, 0.0);
  actor->SetVisibility(false);

  return PLUS_SUCCESS;
}



//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

vtkCxxRevisionMacro(vtkDisplayableImage, "$Revision: 1.0$");
vtkStandardNewMacro(vtkDisplayableImage);

//-----------------------------------------------------------------------------

vtkDisplayableImage::vtkDisplayableImage()
  : vtkDisplayableObject()
{
}

//-----------------------------------------------------------------------------

vtkDisplayableImage::~vtkDisplayableImage()
{
}

//-----------------------------------------------------------------------------

bool vtkDisplayableImage::IsDisplayable()
{
  return this->Displayable;
}

//-----------------------------------------------------------------------------

void vtkDisplayableImage::SetOpacity(double aOpacity)
{
  LOG_TRACE("vtkDisplayableImage::SetOpacity(" << aOpacity << ")");

  vtkImageActor* imageActor = dynamic_cast<vtkImageActor*>(this->Actor);
  if (imageActor)
  {
    imageActor->SetOpacity(aOpacity);
  }
  else
  {
    LOG_WARNING("Invalid actor - cannot set opacity!");
  }
}

//-----------------------------------------------------------------------------

double vtkDisplayableImage::GetOpacity()
{
  vtkImageActor* imageActor = dynamic_cast<vtkImageActor*>(this->Actor);
  if (imageActor)
  {
    return imageActor->GetOpacity();
  }
  else
  {
    LOG_WARNING("Invalid actor - cannot get opacity!");
  }

  return -1.0;
}



//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

vtkCxxRevisionMacro(vtkDisplayableAxes, "$Revision: 1.0$");
vtkStandardNewMacro(vtkDisplayableAxes);

//-----------------------------------------------------------------------------

vtkDisplayableAxes::vtkDisplayableAxes()
  : vtkDisplayableObject()
{
  vtkSmartPointer<vtkAxesActor> axesActor = vtkSmartPointer<vtkAxesActor>::New();
  axesActor->SetShaftTypeToCylinder();
  axesActor->SetXAxisLabelText("X");
  axesActor->SetYAxisLabelText("Y");
  axesActor->SetZAxisLabelText("Z");
  axesActor->SetAxisLabels(0);
  axesActor->SetTotalLength(50, 50, 50);
  this->SetActor(axesActor);
}

//-----------------------------------------------------------------------------

vtkDisplayableAxes::~vtkDisplayableAxes()
{
}

//-----------------------------------------------------------------------------

bool vtkDisplayableAxes::IsDisplayable()
{
  return this->Displayable;
}

//-----------------------------------------------------------------------------

void vtkDisplayableAxes::SetOpacity(double aOpacity)
{
  LOG_TRACE("vtkDisplayableAxes::SetOpacity(" << aOpacity << ")");

  LOG_DEBUG("Cannot set opacity of axes actor!");
}

//-----------------------------------------------------------------------------

double vtkDisplayableAxes::GetOpacity()
{
  vtkAxesActor* axesActor = dynamic_cast<vtkAxesActor*>(this->Actor);
  if (axesActor)
  {
    return axesActor->GetVisibility() ? 1.0 : 0.0;
  }
  else
  {
    LOG_WARNING("Invalid actor - cannot get opacity!");
  }

  return -1.0;
}
