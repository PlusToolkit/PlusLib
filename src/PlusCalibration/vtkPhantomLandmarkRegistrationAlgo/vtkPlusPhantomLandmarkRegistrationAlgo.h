/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

#ifndef __vtkPlusPhantomLandmarkRegistrationAlgo_h
#define __vtkPlusPhantomLandmarkRegistrationAlgo_h

#include "PlusConfigure.h"

#include "vtkPlusCalibrationExport.h"

#include "vtkMatrix4x4.h"
#include "vtkObject.h"
#include "vtkPoints.h"

//class vtkIGSIOTransformRepository;
class vtkXMLDataElement;

//-----------------------------------------------------------------------------

/*!
\class vtkPlusPhantomLandmarkRegistrationAlgo 
\brief Landmark registration to determine the Phantom pose relative to the attached marker (PhantomReference).
\ingroup PlusLibCalibrationAlgorithm
*/
class vtkPlusCalibrationExport vtkPlusPhantomLandmarkRegistrationAlgo : public vtkObject
{
public:
  vtkTypeMacro(vtkPlusPhantomLandmarkRegistrationAlgo,vtkObject);
  static vtkPlusPhantomLandmarkRegistrationAlgo *New();

public:
  /*!
  Performs landmark registration to determine transformation from phantom reference to phantom
  \param aTransformRepository Transform repository to save the results into
  */
  PlusStatus LandmarkRegister(vtkIGSIOTransformRepository* aTransformRepository = NULL);

  /*!
  Read phantom definition (landmarks)
  \param aConfig Root XML data element containing the tool calibration
  */
  PlusStatus ReadConfiguration(vtkXMLDataElement* aConfig);

  /*!
  Gets defined landmark name
  \param aIndex Index of the landmark
  \return Name string
  */
  std::string GetDefinedLandmarkName(int aIndex) { return this->DefinedLandmarkNames[aIndex]; };

  /*! Get configuration element name */
  static std::string GetConfigurationElementName() { return vtkPlusPhantomLandmarkRegistrationAlgo::ConfigurationElementName; };

  /*!Get minimum distance between any two landmarks [mm] */
  double GetMinimunDistanceBetweenTwoLandmarksMm();

  /*! Get the defined landmarks "centroid" (it is actually just the average of the landmarks position, pretty close to phantom centroid) in phantom coordinates system*/
  void GetDefinedLandmarksCentroid_Phantom(double landmarksCentroid_Phantom[4]);

  /*! Get the defined landmarks "centroid" (it is actually just the average of the landmarks position, pretty close to phantom centroid) in reference coordinates system*/
  void GetDefinedLandmarksCentroid_Reference(double landmarksAverage_Reference[4]);

  /*!
    Get the camera position (in reference coordinates system) of the next landmark to be added. The position is 500 mm away from the next landmark along 
    the "centroid"-next landmark direction
  */
  void GetLandmarkCameraPosition_Reference(int index, double cameraPosition_Reference[4]);

  /*! Get the defined landmark at index in the reference coordinates system */
  void GetDefinedLandmark_Reference(int index, double cameraPosition_Reference[4]);

    /*! Print recorded landmarks in Phantom coordinates system*/
  void PrintRecordedLandmarks_Phantom();

public:

  vtkGetMacro(RegistrationErrorMm, double);

  vtkGetObjectMacro(PhantomToReferenceTransformMatrix, vtkMatrix4x4); 
  vtkSetObjectMacro(PhantomToReferenceTransformMatrix, vtkMatrix4x4);

  vtkGetObjectMacro(DefinedLandmarks_Phantom, vtkPoints);
  vtkGetObjectMacro(RecordedLandmarks_Reference, vtkPoints);

  vtkGetStringMacro(PhantomCoordinateFrame);
  vtkGetStringMacro(ReferenceCoordinateFrame);
  vtkGetStringMacro(StylusTipCoordinateFrame);

protected:
  /*! Compute the registration error */
  PlusStatus ComputeError();

protected:
  /*! Sets the known landmark points positions (defined in the Phantom coordinate system) */
  vtkSetObjectMacro(DefinedLandmarks_Phantom, vtkPoints);

  /*! Sets the landmark points that were recorded by a stylus (defined in the Reference coordinate system)*/
  vtkSetObjectMacro(RecordedLandmarks_Reference, vtkPoints);

  vtkSetStringMacro(PhantomCoordinateFrame);
  vtkSetStringMacro(ReferenceCoordinateFrame);
  vtkSetStringMacro(StylusTipCoordinateFrame);

protected:
  vtkPlusPhantomLandmarkRegistrationAlgo();
  virtual  ~vtkPlusPhantomLandmarkRegistrationAlgo();

protected:
  /*! Minimum distance between any two landmarks [mm] */
  double minimumDistanceBetweenTwoLandmarksMm;

  /*! Point array holding the defined landmarks from the configuration file */
  vtkPoints* DefinedLandmarks_Phantom;

  /*! Names of the defined phantom landmarks from the configuration file */
  std::vector<std::string>  DefinedLandmarkNames;

  /*! Point array holding the recorded landmarks */
  vtkPoints* RecordedLandmarks_Reference;

  /*! Phantom to reference transform matrix - the result of the registration */
  vtkMatrix4x4* PhantomToReferenceTransformMatrix;

  /*! The mean error of the landmark registration in mm */
  double RegistrationErrorMm;

  /*! Name of the phantom coordinate frame (eg. Phantom) */
  char* PhantomCoordinateFrame;

  /*! Name of the reference coordinate frame (eg. Reference) */
  char* ReferenceCoordinateFrame;

  /*! Name of the stylus tip coordinate frame (eg. StylusTip) */
  char* StylusTipCoordinateFrame;

  /*! Name of the phantom registration configuration element */
  static std::string ConfigurationElementName;

};

#endif
