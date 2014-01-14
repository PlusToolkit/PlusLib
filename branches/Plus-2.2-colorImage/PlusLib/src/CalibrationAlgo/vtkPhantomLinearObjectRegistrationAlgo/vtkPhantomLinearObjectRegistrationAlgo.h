/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

#ifndef __vtkPhantomLinearObjectRegistrationAlgo_h
#define __vtkPhantomLinearObjectRegistrationAlgo_h

#include "Plane.h"
#include "Planes.h"
#include "Reference.h"
#include "References.h"
#include "PlusConfigure.h"
#include "vtkDoubleArray.h"
#include "vtkMatrix4x4.h"
#include "vtkObject.h"
#include "vtkPoints.h"

class vtkTransformRepository;
class vtkXMLDataElement;

//-----------------------------------------------------------------------------

/*!
  \class vtkPhantomLinearObjectRegistrationAlgo 
  \brief Landmark registration to determine the Phantom pose relative to the attached marker (PhantomReference).
  \ingroup PlusLibCalibrationAlgorithm
*/
class VTK_EXPORT vtkPhantomLinearObjectRegistrationAlgo : public vtkObject
{
public:
  vtkTypeRevisionMacro(vtkPhantomLinearObjectRegistrationAlgo,vtkObject);
  static vtkPhantomLinearObjectRegistrationAlgo *New();

public:
  /*!
    Performs landmark registration to determine transformation from phantom reference to phantom
    \param aTransformRepository Transform repository to save the results into
  */
  PlusStatus Register(vtkTransformRepository* aTransformRepository = NULL);

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
  std::string GetDefinedPlaneName(int aIndex) { return this->DefinedPlaneNames[aIndex]; };

  Planes* GetDefinedPlanesFromXML();

  Planes* GetUserRecordedPlanes();

  PlusStatus InsertNextCalibrationPoint(vtkMatrix4x4* aMarkerToReferenceTransformMatrix);

  PlusStatus Initialize();

  /*! Get configuration element name */
  static std::string GetConfigurationElementName() { return vtkPhantomLinearObjectRegistrationAlgo::ConfigurationElementName; };

public:

  vtkGetMacro(RegistrationError, double);

  vtkGetObjectMacro(PhantomToReferenceTransformMatrix, vtkMatrix4x4); 
  vtkSetObjectMacro(PhantomToReferenceTransformMatrix, vtkMatrix4x4);

  vtkGetObjectMacro(RecordedPoints, vtkPoints);

  vtkGetStringMacro(PhantomCoordinateFrame);
  vtkGetStringMacro(ReferenceCoordinateFrame);
  vtkGetStringMacro(StylusTipCoordinateFrame);

protected:
  /*! Compute the registration error */
  PlusStatus ComputeError();

  /*Sets the planes that are described in the configuration file (defined in the phantom coordinate system)*/
  void SetDefinedPlanesFromXML(Planes* planes);

  /*! Sets the planes that were recorded by a stylus */
  void SetRecordedPlanes(Planes* planes);

protected:
  /*! Sets the points that were recorded by a stylus */
  vtkSetObjectMacro(RecordedPoints, vtkPoints);

  vtkSetObjectMacro(MarkerToReferenceTransformMatrixArray, vtkDoubleArray);

  vtkSetStringMacro(PhantomCoordinateFrame);
  vtkSetStringMacro(ReferenceCoordinateFrame);
  vtkSetStringMacro(StylusTipCoordinateFrame);

protected:
  vtkPhantomLinearObjectRegistrationAlgo();
  virtual  ~vtkPhantomLinearObjectRegistrationAlgo();

protected:
  /*Names of the planes defined in the configuration file*/
  std::vector<std::string>  DefinedPlaneNames;

  /*! Planes array holding the predefined planes from the configuration file*/
  Planes                    DefinedPlanes;

  /*! Plane array holding the recorded planes */
  Planes                    RecordedPlanes;

  /*Names of the references defined in the configuration file*/
  std::vector<std::string>  DefinedReferenceNames;
  
  /*! References array holding the predefined references from the configuration file*/
  References                DefinedReferences;

  /*! Reference array holding the recorded references */
  References                RecordedReferences;
  
  /*! Point array holding the recorded points */
  vtkPoints*                RecordedPoints;

  /*! Phantom to reference transform matrix - the result of the registration */
  vtkMatrix4x4*              PhantomToReferenceTransformMatrix;

  /*! The mean error of the landmark registration in mm */
  double                    RegistrationError;

  /*! Name of the phantom coordinate frame (eg. Phantom) */
  char*                     PhantomCoordinateFrame;

  /*! Name of the reference coordinate frame (eg. Reference) */
  char*                     ReferenceCoordinateFrame;

  /*! Name of the stylus tip coordinate frame (eg. StylusTip) */
  char*                     StylusTipCoordinateFrame;

  /*! Name of the phantom registration configuration element */
  static std::string        ConfigurationElementName;

  /*! Array of the input points */
  vtkDoubleArray*           MarkerToReferenceTransformMatrixArray;

};

#endif
