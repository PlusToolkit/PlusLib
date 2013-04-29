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
 
  void SetFrequencyMHz(double frequencyMHz); 

  void SetSoundVelocityMPerSec(double soundVelocityMPerSec);

  void SetDensityKgPerM3(double densityKgPerM3);

  void SetAttenuationCoefficientNpPerCm(double attenuationCoefficientNpPerCm);

  void SetIncomingIntensityWattsPerCm2(double incomingIntensityWattsPerCm2); 

  double GetIncomingIntensityWattsPerCm2(); 

  double GetAcousticImpedance();
  
  // acousticImpedanceNeighbouringMaterial-> perform check to see if it is the same
void CalculateIntensity(std::vector<double>& intensities, int numberOfFilledPixels, double distanceBetweenScanlineSamplePointsMm, double previousModeAcousticImpedance, double incomingBeamIntensity, double outgoingBeamIntensity);

private:

  /*! Identifying name of Model*/
  const char* Name; 
  
  /*! Path for the model file */
  const char* ModelFileName; 

  //TODO: /*! ModelToReferenceTransform */ ... is not read in previously, probably not needed, keep an eye on it

  
  /*! Ultrasound frequency */
  double ImagingFrequencyMHz; 

   /*! Max Intensity*/
  double IncomingIntensityWattsPerCm2;
  
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

   /*! transformation matrix from the native model coordinate system to spatial model coordinate system used to display all models */
  vtkMatrix4x4* ModelFileToSpatialModelMatrix; 


};


#endif