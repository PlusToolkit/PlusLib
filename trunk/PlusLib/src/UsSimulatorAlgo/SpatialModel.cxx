/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/
#include "PlusConfigure.h"
#include "SpatialModel.h"
#include "vtkObjectFactory.h"

// If fraction of the transmitted beam intensity is smaller then this value then we consider the beam to be completely absorbed
const double MINIMUM_BEAM_INTENSITY=1e-6;

SpatialModel::SpatialModel()
{
  this->Name = NULL; 
  this->ModelFileName = NULL; 
  this->ModelFileToSpatialModelTransform =NULL; 
  this->DensityKgPerM3 = 0; 
  this->SoundVelocityMPerSec = 0; 
  this->AttenuationCoefficientDbPerCmMhz = 0;
  this->BackscatterDiffuseReflectionCoefficient = 0;
  this->BackscatterSpecularReflectionCoefficient = 0; 
  this->ImagingFrequencyMhz = 0;  
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
    
   
    double vectorModelFileToSpatialModelTransform[16]={0}; 
    if ( spatialModelElement->GetVectorAttribute("ModelFileToSpatialModelTransform", 16, vectorModelFileToSpatialModelTransform) )
    {
      this->ModelFileToSpatialModelTransform->DeepCopy(vectorModelFileToSpatialModelTransform); 
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
    
    double attenuationCoefficientDbPerCmMhz = 0;
    if(sceneDescriptionElement->GetScalarAttribute("AttenuationCoefficientDbPerCmMhz",attenuationCoefficientDbPerCmMhz)) 
    {
      this->AttenuationCoefficientDbPerCmMhz = attenuationCoefficientDbPerCmMhz;
    }
    
    double diffuseReflectionCoefficient = 0;
    if(sceneDescriptionElement->GetScalarAttribute("BackscatterDiffuseReflectionCoefficient", diffuseReflectionCoefficient)) 
    {
      this->BackscatterDiffuseReflectionCoefficient = diffuseReflectionCoefficient;
    }
  
    double specularReflectionCoefficient = 0;
    if(sceneDescriptionElement->GetScalarAttribute("BackscatterSpecularReflectionCoefficient",specularReflectionCoefficient)) 
    {
      this->BackscatterSpecularReflectionCoefficient = specularReflectionCoefficient;
    }    
     
  }

  return status;
}

void SpatialModel::SetFrequencyMhz(double frequencyMhz)
{
  this->ImagingFrequencyMhz = frequencyMhz; 
}

void SpatialModel::SetAttenuationCoefficientDbPerCmMhz(double attenuationCoefficientDbPerCmMhz)
{
  this->AttenuationCoefficientDbPerCmMhz = attenuationCoefficientDbPerCmMhz;
}

void SpatialModel::SetDensityKgPerM3(double densityKgPerM3)
{
  this->DensityKgPerM3 = densityKgPerM3; 
}

void SpatialModel::SetSoundVelocityMPerSec(double soundVelocityMPerSec)
{
  this->SoundVelocityMPerSec = soundVelocityMPerSec; 
}

double SpatialModel::GetAcousticImpedanceMegarayls()
{
  double acousticImpedanceRayls = this->DensityKgPerM3 * this->SoundVelocityMPerSec; // kg / (s * m2)
  return acousticImpedanceRayls * 1e-6; // megarayls
}

void SpatialModel::CalculateIntensity(std::vector<double>& reflectedIntensity, int numberOfFilledPixels, double distanceBetweenScanlineSamplePointsMm, double previousModelAcousticImpedanceMegarayls, double incidentIntensity, double &transmittedIntensity)
{
  if (numberOfFilledPixels<=0)
  {
    reflectedIntensity.clear();
    return;
  }

  reflectedIntensity.resize(numberOfFilledPixels);

  // Compute reflection from the surface of the previous and this model
  double acousticImpedanceMegarayls = GetAcousticImpedanceMegarayls();
  double intensityReflectionCoefficient = /* reflected beam intensity / incident beam intensity */
    (previousModelAcousticImpedanceMegarayls - acousticImpedanceMegarayls)*(previousModelAcousticImpedanceMegarayls - acousticImpedanceMegarayls)
    /(previousModelAcousticImpedanceMegarayls + acousticImpedanceMegarayls)/(previousModelAcousticImpedanceMegarayls + acousticImpedanceMegarayls);
  double surfaceReflectedBeamIntensity = incidentIntensity * intensityReflectionCoefficient;
  double surfaceTransmittedBeamIntensity = incidentIntensity - surfaceReflectedBeamIntensity;

  // Compute attenuation within this model
  double intensityAttenuationCoefficientdBPerPixel = this->AttenuationCoefficientDbPerCmMhz*(distanceBetweenScanlineSamplePointsMm/10.0)*this->ImagingFrequencyMhz;
  double intensityAttenuationCoefficientPerPixel = /* transmitted beam intensity / incident beam intensity */
    pow(10.0,intensityAttenuationCoefficientdBPerPixel/10.0);
  double intensityTransmittanceCoefficientPerPixel = (1-intensityAttenuationCoefficientPerPixel); // how big fraction of the intensity is transmitted over one voxel
  double intensityTransmittanceCoefficientPerPixelTwoWay = (1-intensityAttenuationCoefficientPerPixel)*(1-intensityAttenuationCoefficientPerPixel); // takes into account both propagation directions
  
  // We put all the reflected beam intensity in the first (surface) pixel
  reflectedIntensity[0] =
    surfaceReflectedBeamIntensity // reflected from the surface
    + surfaceTransmittedBeamIntensity * intensityAttenuationCoefficientPerPixel * this->BackscatterDiffuseReflectionCoefficient; // backscattering from the firs pixel's region

  transmittedIntensity = surfaceTransmittedBeamIntensity * intensityTransmittanceCoefficientPerPixelTwoWay;

  for(int currentPixelInFilledPixels = 1; currentPixelInFilledPixels<numberOfFilledPixels; currentPixelInFilledPixels++)  
  {
    // The beam intensity is very close to 0, so we terminate early instead of computing all the remaining miniscule values
    if (transmittedIntensity<MINIMUM_BEAM_INTENSITY)
    {
      transmittedIntensity=0;
      for (int i=currentPixelInFilledPixels; i<numberOfFilledPixels; i++)
      {
        reflectedIntensity[i]=0.0;
      }
      break;
    }

    // a fraction of the attenuation is caused by backscattering, the backscattering is sensed by the transducer
    reflectedIntensity[currentPixelInFilledPixels] = transmittedIntensity * intensityAttenuationCoefficientPerPixel * this->BackscatterDiffuseReflectionCoefficient;

    transmittedIntensity *= intensityTransmittanceCoefficientPerPixelTwoWay;    
  }
}
