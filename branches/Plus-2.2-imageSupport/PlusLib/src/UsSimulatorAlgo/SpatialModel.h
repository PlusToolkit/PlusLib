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

/*!
  \class SpatialModel 
  \brief Class to store all properties of a simulated object.
  \ingroup PlusLibUsSimulatorAlgo
*/
class VTK_EXPORT SpatialModel
{
public:

  struct LineIntersectionInfo
  {
    LineIntersectionInfo()
    {
      Model=NULL;
      IntersectionDistanceFromStartPointMm=0;
      IntersectionIncidenceAngleRad=0;
    }
    SpatialModel* Model;
    // Distance between the line starting point and the intersection point, in mm
    double IntersectionDistanceFromStartPointMm;
    // Angle between the intersecting line and the surface normal (0 if the line is parallel to the surface normal)
    double IntersectionIncidenceAngleRad;
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

  void SetReferenceToObjectTransform(vtkMatrix4x4* referenceToObjectTransform);

  /*!
    Get all the intersection points of the model and a line. Input and output points are all in Model coordinate system.
    The results are appended to the lineIntersections structure.
    If the line starts inside the model then the first intersection position is 0.
    The unit of the reference coordinate system must be in mm.
  */
  void GetLineIntersections(std::deque<LineIntersectionInfo>& lineIntersections, double* scanLineStartPoint_Reference, double* scanLineEndPoint_Reference);

  double GetAcousticImpedanceMegarayls();
  
  /*! 
    Computes relative intensities inside the model
    \param incidentIntensity Dimensionless value, if there is 100% reflection at the surface then this
      fraction of the beam intensity would be sensed at the transducer. It includes the effect of attenuation of both incoming and reflected direction.
    \param transmittedIntensity: intensity when the beam leaves the model
  */
  void CalculateIntensity(std::vector<double>& reflectedIntensity, int numberOfFilledPixels, double distanceBetweenScanlineSamplePointsMm,
    double previousModelAcousticImpedanceMegarayls, double incidentIntensity, double &transmittedIntensity, double incidenceAngleRad);

protected:

  void SetPolyData(vtkPolyData* polyData);
  void SetModelLocalizer(vtkModifiedBSPTree* modelLocalizer);
  void SetModelToObjectTransform(vtkMatrix4x4* modelToObjectTransform);

  PlusStatus UpdateModelFile();
  void UpdatePrecomputedAttenuations(double intensityTransmittedFractionPerPixelTwoWay, int numberOfElements);

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

  /*!
    Transformation matrix from the reference coordinate system (that is used by external methods)
    to the object coordinate system (ObjectCoordinateFrame).
    This transform is not serialized to/from the XML configuration.
  */
  vtkMatrix4x4* ReferenceToObjectTransform;

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
    TransducerSpatialModelMaxOverlapMm determines how far from the transducer surface (inside the transducer) we start searching
    for scanline/SpatialModel intersections.
    If the value is too small then inside/outside segments may be inverted for the overlapping model.
    If the value is too large then it may slightly slow down the simulation. 
    When it is decided if a certain part of a scanline is inside or outside of a model it is assumed that
    after 1 (or 3, 5, 7, ...) intersections we are inside the model and after 2 (or 4, 6, 8, ...) intersections we are outside the model.
    This is only true if the scanline start point is outside every spatial model. This is always true in theory, but in practice, due to
    slight registration errors or deformations the transducer surface may get inside a spatial model by a few mm.
    To avoid inversion of inside/outside, we start the scanline a few mm away from the true transducer surface so that we can detect intersections
    that are very close to the transducer surface.
  */
  double TransducerSpatialModelMaxOverlapMm;

  /*!
    The fraction of the attenuated intensity that is reflected (backscattered) in a mirror-like way.
    Smooth, polished surfaces have higher values (range: 0.0-1.0).
    A value of one means all the attenuation is caused by a perfect reflection.    
    Usually not just scattering occurs (there are various ways of scattering) but also absorption, so the value is much less than 1.0.
  */
  double SurfaceSpecularReflectionCoefficient;

  /*!
    The fraction of the attenuated intensity that is reflected (backscattered) in an incidence-angle indepentent way.
  */
  double SurfaceDiffuseReflectionCoefficient;

  vtkModifiedBSPTree* ModelLocalizer;

  /*! Surface mesh. Points are stored in the Model coordinate system (as in the input file) */
  vtkPolyData* PolyData;

  /*! List of attenuations: intensityTransmittedFractionPerPixelTwoWay, intensityTransmittedFractionPerPixelTwoWay^2, intensityTransmittedFractionPerPixelTwoWay^3, ... */
  std::vector<double> PrecomputedAttenuations;

};

#endif
