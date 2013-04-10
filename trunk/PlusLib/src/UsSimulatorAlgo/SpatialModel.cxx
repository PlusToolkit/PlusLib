/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/
#include "PlusConfigure.h"
#include "SpatialModel.h"
#include "vtkObjectFactory.h"
double DECIBEL_PER_NEPER = 8.685889638;
double CONVERSION_TO_MEGARAYLS = 1000000;

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
  this->ImagingFrequencyMHz = 0; 
  this->IncomingIntensityWattsPerCm2 = 0;
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

void SpatialModel::SetFrequencyMHz(double frequencyMHz)
{
  this->ImagingFrequencyMHz = frequencyMHz; 
}


void SpatialModel::SetAttenuationCoefficientNpPerCm(double attenuationCoefficientNpPerCm)
{
  this->AttenuationCoefficientNpPerCm = attenuationCoefficientNpPerCm; 
}

void SpatialModel::SetDensityKgPerM3(double densityKgPerM3)
{
  this->DensityKgPerM3 = densityKgPerM3; 
}

void SpatialModel::SetSoundVelocityMPerSec(double soundVelocityMPerSec)
{
  this->SoundVelocityMPerSec = soundVelocityMPerSec; 
}

void SpatialModel::SetIncomingIntensityWattsPerCm2(double incomingIntensityWattsPerCm2)
{
  this->IncomingIntensityWattsPerCm2 = incomingIntensityWattsPerCm2; 
}

double SpatialModel::GetIncomingIntensityWattsPerCm2()
{
  return this->IncomingIntensityWattsPerCm2;
}

double SpatialModel::GetAcousticImpedance()
{
	return ((this->DensityKgPerM3 * this->SoundVelocityMPerSec)/ CONVERSION_TO_MEGARAYLS);
}

void SpatialModel::CalculateIntensity(std::vector<double>& intensities, int numberOfFilledPixels, double distanceBetweenScanlineSamplePointsMm, double previousModelAcousticImpedance, double incomingBeamIntensity, double outgoingBeamIntensity)
{
	
  // if beam intensity is 0 return 0
 // if (this->IncomingIntensityWattsPerCm2<1)
	//{
	//	return this->IncomingIntensityWattsPerCm2 ;
	//}
 
  for(int currentPixelInFilledPixels = 0; currentPixelInFilledPixels<numberOfFilledPixels; currentPixelInFilledPixels++)
  {
	
    double intensityAttenuationCoefficientNpPerCmPerHz = 0;
    intensityAttenuationCoefficientNpPerCmPerHz= this->AttenuationCoefficientNpPerCm *2;
    
    // convert to intensityAttentuationCoefficient.
    double intensityAttenuationCoefficientdBPerCmPerHz = 0; 
    intensityAttenuationCoefficientdBPerCmPerHz = intensityAttenuationCoefficientNpPerCmPerHz * DECIBEL_PER_NEPER* this->ImagingFrequencyMHz; 
    
    // calculate intensity loss for transmitted wave( mu* frequency*distance traveled), which is equal to the loss caused by the reflected wave
    double intensityLossDuringWaveTransmissionDecibels = 0; 
    intensityLossDuringWaveTransmissionDecibels = intensityAttenuationCoefficientdBPerCmPerHz*this->ImagingFrequencyMHz*(distanceBetweenScanlineSamplePointsMm*currentPixelInFilledPixels/10);

    //calcualte acoustic impedance from the denisty and velocity of the material 
    double acousticImpedanceMegarayles = (this->DensityKgPerM3 / this->SoundVelocityMPerSec); // / CONVERSION_TO_MEGARAYLS; 
   
    double totalIntensityLoss = 0; 

   
    // If the material hasn't changed from the previous pixel, do not incorporate reflection ( for now.. TODO: add reflection using phong illumination model)
    if(previousModelAcousticImpedance = acousticImpedanceMegarayles)
    {
      totalIntensityLoss = intensityLossDuringWaveTransmissionDecibels * 2; // 2 for reflected wave... still counts as magic number?
    }
    else
    {
      // calculate reflection   
      double reflection = pow((previousModelAcousticImpedance - acousticImpedanceMegarayles)/(previousModelAcousticImpedance + acousticImpedanceMegarayles),2)*100; 
      double intensityLossDueToReflection = 10 * log10(100/reflection); 
      totalIntensityLoss = (intensityLossDuringWaveTransmissionDecibels * 2) + intensityLossDueToReflection; 

    }
    //convert intensity to watts per cm^2
    
    totalIntensityLoss = totalIntensityLoss*-1; 
    double totalIntensityLossWattsPerCm2 = 0; 
    totalIntensityLossWattsPerCm2 = pow(10,totalIntensityLoss/10) * incomingBeamIntensity;
    // set outgoingBeamIntensity
    outgoingBeamIntensity = incomingBeamIntensity-totalIntensityLossWattsPerCm2;
    
    
    // if total intensity more than incoming intensity, return incoming intenstity ( so that when it is subtracted in from the intensity in UsSimulatorAlgo
    if(totalIntensityLossWattsPerCm2> incomingBeamIntensity)
    {
      intensities.push_back(0);// Intensity loss is greater than original intensity
    }

    else
    {
      intensities.push_back(outgoingBeamIntensity); 
    }
  }
}