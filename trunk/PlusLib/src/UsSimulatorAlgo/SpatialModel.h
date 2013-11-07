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
 
  void SetFrequencyMhz(double frequencyMhz);

  void SetSoundVelocityMPerSec(double soundVelocityMPerSec);

  void SetDensityKgPerM3(double densityKgPerM3);

  void SetAttenuationCoefficientDbPerCmMhz(double attenuationCoefficientDbPerCmMhz);

  void SetIncomingIntensityWattsPerCm2(double incomingIntensityWattsPerCm2); 

  double GetIncomingIntensityWattsPerCm2(); 

  double GetAcousticImpedanceMegarayls();
  
  /*! 
    Computes relative intensities inside the model
    \param incidentIntensity Dimensionless value, if there is 100% reflection at the surface then this
      fraction of the beam intensity would be sensed at the transducer. It includes the effect of attenuation of both incoming and reflected direction.
    \param transmittedIntensity: intensity when the beam leaves the model
  */
  void CalculateIntensity(std::vector<double>& reflectedIntensity, int numberOfFilledPixels, double distanceBetweenScanlineSamplePointsMm,
    double previousModelAcousticImpedanceMegarayls, double incidentIntensity, double &transmittedIntensity);

private:

  /*! Identifying name of Model*/
  const char* Name; 
  
  /*! Path for the model file */
  const char* ModelFileName; 

  //TODO: /*! ModelToReferenceTransform */ ... is not read in previously, probably not needed, keep an eye on it

  
  /*! Ultrasound frequency */
  double ImagingFrequencyMhz; 
  
  /*! Density in kg/m^3 of the material the model is comprised of */
  double DensityKgPerM3; 

  /*! Velocity of sound in the model material (m/s) */
  double SoundVelocityMPerSec; 

  /*! Attenuation coefficient of the model material (dB / cm / MHz). Includes both absorption and scattering. */
  double AttenuationCoefficientDbPerCmMhz;

  /*!
    The fraction of the attenuated intensity that is reflected (backscattered) to all directions.
    Rough surfaces have higher values (range: 0.0-1.0).
    Usually not just scattering occurs (there are various ways of scattering) but also absorption, so the value is much less than 1.0.
  */
  double BackscatterDiffuseReflectionCoefficient;

  /*!
    The fraction of the attenuated intensity that is reflected (backscattered) in a mirror-like way.
    Smooth, polished surfaces have higher values (range: 0.0-1.0).
    A value of one means all the attenuation is caused by a perfect reflection.    
    Usually not just scattering occurs (there are various ways of scattering) but also absorption, so the value is much less than 1.0.
    This value is not used now, so no specular reflection is assumed.
    */
  double BackscatterSpecularReflectionCoefficient;

   /*! transformation matrix from the native model coordinate system to spatial model coordinate system used to display all models */
  vtkMatrix4x4* ModelFileToSpatialModelTransform;

};

#endif
