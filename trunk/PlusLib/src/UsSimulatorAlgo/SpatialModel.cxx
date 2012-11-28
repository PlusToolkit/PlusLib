/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/
#include "PlusConfigure.h"
#include "SpatialModel.h"
#include "vtkObjectFactory.h"
const double NEPERTODECIBELCONVERSIONCONSTANT = 8.686;

SpatialModel::SpatialModel()
{
  this->Name = NULL; 
  this->ModelFileName = NULL; 
  this->ModelFileToSpatialModelMatrix =NULL; 
  this->DensityKgPerM3 = 0; 
  this->SoundVelocityMPerSec = 0; 
  this->AttenuationCoefficientNpPerCm = 0; 
  this->ScatterCoefficientNpPerCm = 0; 
  this->SpecularReflectionRatio = 0; 
  this->FrequencyMHz = 0; 
  this->MaxIntensityWattsPerCm2 = 0;
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
    
   
    double vectorModelFileToSpatialModelMatrix[16]={0}; 
    if ( spatialModelElement->GetVectorAttribute("ModelFileToSpatialModelTransform", 16, vectorModelFileToSpatialModelMatrix) )
    {
      this->ModelFileToSpatialModelMatrix->DeepCopy(vectorModelFileToSpatialModelMatrix); 
    }
    
    double densityKGPerM3 = 0; 
    if(sceneDescriptionElement->GetScalarAttribute("DensityKgPerM3",densityKGPerM3))
    {
      this->DensityKgPerM3 = densityKGPerM3;  
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
    
    double scatterCoefficientNpPerCm = 0;
    if(sceneDescriptionElement->GetScalarAttribute("ScatterCoefficientNpPerCm",scatterCoefficientNpPerCm)) 
    {
      this->ScatterCoefficientNpPerCm = scatterCoefficientNpPerCm;
    }
  
    double specularReflectionRatio = 0;
    if(sceneDescriptionElement->GetScalarAttribute("SpecularReflectionRatio",specularReflectionRatio)) 
    {
      this->SpecularReflectionRatio = specularReflectionRatio;
    }
    
     
  }

  return status;
}

void SpatialModel::setFrequencyMHz(double frequencyMHz)
{
  this->FrequencyMHz = frequencyMHz; 
}


void SpatialModel::setMaxIntensityWattsPerCm2(double maxIntensityWattsPerCm2)
{
  this->MaxIntensityWattsPerCm2 = maxIntensityWattsPerCm2; 
}

double SpatialModel::getMaxIntensityWattsPerCm2()
{
  return this->MaxIntensityWattsPerCm2;
}

double SpatialModel::calculateIntensity(double acousticImpedenceNeighbouringMaterial, double distanceUSWaveTravelledCm)
{
  double intensityAttenuationCoefficientNpPerCmPerHz = 0;
  intensityAttenuationCoefficientNpPerCmPerHz= this->AttenuationCoefficientNpPerCm *2;
  // convert to intensityAttentuationCoefficient... TODO: check math, this method may be too naive ( equations in Ultrasound Physics and INstrumentation pg 25 may
  // have been misinterpreted)
  
  double intensityAttenuationCoefficientdBPerCmPerHz = 0; 
  intensityAttenuationCoefficientdBPerCmPerHz = intensityAttenuationCoefficientdBPerCmPerHz * NEPERTODECIBELCONVERSIONCONSTANT; 
  
  double intensityLossDuringWaveTransmissionDecibels = 0; 
 return intensityLossDuringWaveTransmissionDecibels = intensityAttenuationCoefficientdBPerCmPerHz*this->FrequencyMHz*distanceUSWaveTravelledCm;

  double totalIntensityLoss = 0; 

 // if(acousticImpedenceNeighbouringMaterial= this->


}