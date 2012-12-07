/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/
#ifndef __SpatialModel_h
#define __SpatialModel_h

#include "vtkSmartPointer.h"
#include "vtkImageData.h"
#include "vtkMatrix4x4.h"
#include "vtkModifiedBSPTree.h"


class VTK_EXPORT SpatialModel
{

public:
  SpatialModel(); 
  ~SpatialModel(); 
  virtual PlusStatus ReadConfiguration(vtkXMLDataElement* config);
   /*! Set US frequency for generated image */
 
  void setFrequencyMHz(double frequencyMHz); 

  void setMaxIntensityWattsPerCm2(double maxIntensityWattsPerCm2); 

  double getMaxIntensityWattsPerCm2(); 

  
  // acousticImpedenceNeighbouringMaterial-> perform check to see if it is the same
  double calculateIntensity(double acousticImpedenceNeighbouringMaterial, double distanceUSWaveTravelledCm);

private:

  /*! Identifying name of Model*/
  const char* Name; 
  
  /*! Path for the model file */
  const char* ModelFileName; 

  //TODO: /*! ModelToReferenceTransform */ ... is not read in previously, probably not needed, keep an eye on it

  
  /*! Ultrasound frequency */
  double FrequencyMHz; 

   /*! Max Intensity*/
  double MaxIntensityWattsPerCm2;
  
  /*! Density in kg/m^3 of the material the model is comprised of */
  double DensityKgPerM3; 

  /*! Velocity of sound in the model material ( m/s) */
  double SoundVelocityMPerSec; 

  /*! Attenuation coefficient of the model material ( nepers/cm)*/
  double AttenuationCoefficientNpPerCm; 

  /*! Scattering coefficient of the model material (nepers/cm)*/
  double ScatterCoefficientNpPerCm; 

  /*! Specular Reflection Ratio of model material*/
  double SpecularReflectionRatio; 

  vtkMatrix4x4* ModelFileToSpatialModelMatrix; 


};


#endif