/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/
#ifndef __SpatialModel_h
#define __SpatialModel_h

#include <deque>
#include <string>

#include "vtkSmartPointer.h"
#include "vtkImageData.h"
#include "vtkMatrix4x4.h"
#include "vtkModifiedBSPTree.h"

class VTK_EXPORT SpatialModel
{
public:

  struct LineIntersectionInfo
  {
    LineIntersectionInfo()
    {
      Model=NULL;
      IntersectionDistanceFromStartPointMm=0;
      IntersectionIncidenceAngleDeg=0;
    }
    SpatialModel* Model;
    // Distance between the line starting point and the intersection point, in mm
    double IntersectionDistanceFromStartPointMm;
    // Angle between the intersecting line and the surface normal (0 if the line is parallel to the surface normal)
    double IntersectionIncidenceAngleDeg;
  };

  SpatialModel(); 
  virtual ~SpatialModel(); 

  /*! Copy constructor. Performs shallow-copy. */
  SpatialModel( const SpatialModel& model);
  /*! Assignment operator. Performs shallow-copy. */
  void operator=( const SpatialModel& model);

  virtual PlusStatus ReadConfiguration(vtkXMLDataElement* spatialModelElement);

  /*! Set the surface model file name (STL or VTP). It can be used to override the model file name specified in the XML configuration. */
  void SetModelFile(const char* modelFile);

  /*! Set US imaging frequency for generated image */ 
  void SetImagingFrequencyMhz(double frequencyMhz);

  const std::string& GetName() { return this->Name; };
  const std::string& GetObjectCoordinateFrame() { return this->ObjectCoordinateFrame; };
  vtkMatrix4x4* GetModelToObjectTransform() { return this->ModelToObjectTransform; };

  /*!
    Get all the intersection points of the model and a line. Input and output points are all in Model coordinate system.
    The results are appended to the lineIntersections structure.
    If the line starts inside the model then the first intersection position is 0.
    The unit of the reference coordinate system must be in mm.
  */
  void GetLineIntersections(std::deque<LineIntersectionInfo>& lineIntersections, double* scanLineStartPoint_Reference, double* scanLineEndPoint_Reference, vtkMatrix4x4* referenceToObjectMatrix);

  double GetAcousticImpedanceMegarayls();
  
  /*! 
    Computes relative intensities inside the model
    \param incidentIntensity Dimensionless value, if there is 100% reflection at the surface then this
      fraction of the beam intensity would be sensed at the transducer. It includes the effect of attenuation of both incoming and reflected direction.
    \param transmittedIntensity: intensity when the beam leaves the model
  */
  void CalculateIntensity(std::vector<double>& reflectedIntensity, int numberOfFilledPixels, double distanceBetweenScanlineSamplePointsMm,
    double previousModelAcousticImpedanceMegarayls, double incidentIntensity, double &transmittedIntensity);  

protected:

  void SetPolyData(vtkPolyData* polyData);
  void SetModelLocalizer(vtkModifiedBSPTree* modelLocalizer);
  void SetModelToObjectTransform(vtkMatrix4x4* modelToObjectTransform);  

  PlusStatus UpdateModelFile();

private:

  //PlusStatus LoadModel(const std::string& absoluteImagePath);

  /*! Identifying name of Model*/
  std::string Name; 
  
  /*! Name of the STL or VTP model file. If no ModelFile is defined then it means that the model is everywhere (background material). */
  std::string ModelFile;

  /*! It is true if the model file needs to be updated (re-read from the ModelFile) */
  bool ModelFileNeedsUpdate;

  /*!
    Transformation matrix from the native coordinate system (as points are stored in the file)
    to spatial object coordinate system (ObjectCoordinateFrame)
  */
  vtkMatrix4x4* ModelToObjectTransform;

  /*! This variable defines the name of the spatial object's coordinate frame */
  std::string ObjectCoordinateFrame;

  //TODO: /*! ModelToReferenceTransform */ ... is not read in previously, probably not needed, keep an eye on it
  
  /*! Ultrasound frequency */
  double ImagingFrequencyMhz; 
  
  /*! Density in kg/m^3 of the material the model is comprised of */
  double DensityKgPerM3; 

  /*! Velocity of sound in the model material (m/s) */
  double SoundVelocityMPerSec; 

  /*! Attenuation coefficient of the model material (dB / cm / MHz). Includes both absorption and scattering. Positive number. */
  double AttenuationCoefficientDbPerCmMhz;

  /*!
    Attenuation coefficient of the surface reflection (dB / mm). Positive number.
    We model the reflection from the surface as intensity reflected from a small region with an exponential decay.
    The decay coefficient refers to the dispersion of the signal in the material and the signal processing electronics.
  */
  double SurfaceReflectionIntensityDecayDbPerMm;

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

  vtkModifiedBSPTree* ModelLocalizer;

  /*! Surface mesh. Points are stored in the Model coordinate system (as in the input file) */
  vtkPolyData* PolyData;

};

#endif
