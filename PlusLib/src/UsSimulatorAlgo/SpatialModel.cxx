/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/
#include "PlusConfigure.h"
#include "SpatialModel.h"



SpatialModel::SpatialModel()
{
  this->Name = NULL; 
  this->ModelFileName = NULL; 
  this->DensityKGPerM3 = 0; 
  this->SoundVelocityMPerSec = 0; 
  this->AttenuationCoefficientNpPerCm = 0; 
  this->ScatterCoeffcientNpPerCm = 0; 
  this->SpecularReflectionRatio = 0; 
}

PlusStatus SpatialModel::ReadConfiguration(vtkXMLDataElement* config)
{ 
  if ( config == NULL )
  {
    LOG_DEBUG("Unable to configure SpatialModel(XML data element is NULL)"); 
    return PLUS_FAIL; 
  }
  vtkXMLDataElement* sceneDescriptionElement = config->FindNestedElementWithName("SceneDescription"); 
  if (sceneDescriptionElement == NULL)
  {
    LOG_DEBUG("Unable to find SpatialModel element in XML tree, will use default values!");    
    return PLUS_FAIL;
  }

  PlusStatus status=PLUS_SUCCESS;

  //Set data elements
  
  vtkXMLDataElement* spatialModelElement = sceneDescriptionElement->FindNestedElementWithName("SpatialModel"); 
  if (sceneDescriptionElement != NULL)
  {
    this->Name = sceneDescriptionElement->GetAttribute("Name"); 
    this->ModelFileName = sceneDescriptionElement->GetAttribute("ModelFileName");; 
    
    double densityKGPerM3 = 0; 
    if(sceneDescriptionElement->GetScalarAttribute("DensityKGPerM3",densityKGPerM3))
    {
      this->DensityKGPerM3 = densityKGPerM3;  
    }
    
    double soundVelocityMperSec = 0;
    if(sceneDescriptionElement->GetScalarAttribute("SoundVelocityMPerSec",soundVelocityMperSec)) 
    {
      this->SoundVelocityMPerSec = soundVelocityMperSec;
    }
    
    double attenuationCoefficientNpPerCm = 0;
    if(sceneDescriptionElement->GetScalarAttribute("AttenuationCoefficientNpPerCm",attenuationCoefficientNpPerCm)) 
    {
      this->AttenuationCoefficientNpPerCm = attenuationCoefficientNpPerCm;
    }
    
    double scatterCoeffcientNpPerCm = 0;
    if(sceneDescriptionElement->GetScalarAttribute("ScatterCoeffcientNpPerCm",scatterCoeffcientNpPerCm)) 
    {
      this->ScatterCoeffcientNpPerCm = scatterCoeffcientNpPerCm;
    }
  
    double specularReflectionRatio = 0;
    if(sceneDescriptionElement->GetScalarAttribute("SpecularReflectionRatio",specularReflectionRatio)) 
    {
      this->SpecularReflectionRatio = specularReflectionRatio;
    }
    
     
  }

  return status;
}