/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

#ifndef __vtkvtkPhantomRegistrationAlgo_h
#define __vtkvtkPhantomRegistrationAlgo_h

#include "PlusConfigure.h"

#include "vtkObject.h"
#include "vtkMatrix4x4.h"
#include "vtkPoints.h"

class vtkTransformRepository;
class vtkXMLDataElement;

//-----------------------------------------------------------------------------

/*!
  \class vtkPhantomRegistrationAlgo 
  \brief Landmark registration to determine the Phantom pose relative to the attached marker (PhantomReference).
  \ingroup PlusLibCalibrationAlgorithm
*/
class vtkPhantomRegistrationAlgo : public vtkObject
{
public:
  vtkTypeRevisionMacro(vtkPhantomRegistrationAlgo,vtkObject);
	static vtkPhantomRegistrationAlgo *New();

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
  std::string GetDefinedLandmarkName(int aIndex) { return this->DefinedLandmarkNames[aIndex]; };

public:

  vtkGetMacro(RegistrationError, double);

  vtkGetObjectMacro(PhantomToReferenceTransformMatrix, vtkMatrix4x4); 
  vtkSetObjectMacro(PhantomToReferenceTransformMatrix, vtkMatrix4x4);

  vtkGetObjectMacro(DefinedLandmarks, vtkPoints);
  vtkGetObjectMacro(RecordedLandmarks, vtkPoints);

  vtkGetStringMacro(PhantomCoordinateFrame);
  vtkGetStringMacro(ReferenceCoordinateFrame);
  vtkGetStringMacro(StylusTipCoordinateFrame);

protected:
  /*! Compute the registration error */
  PlusStatus ComputeError();

protected:
  /*! Sets the known landmark points positions (defined in the Phantom coordinate system) */
  vtkSetObjectMacro(DefinedLandmarks, vtkPoints);

  /*! Sets the landmark points that were recorded by a stylus */
  vtkSetObjectMacro(RecordedLandmarks, vtkPoints);

  vtkSetStringMacro(PhantomCoordinateFrame);
  vtkSetStringMacro(ReferenceCoordinateFrame);
  vtkSetStringMacro(StylusTipCoordinateFrame);

protected:
	vtkPhantomRegistrationAlgo();
	virtual	~vtkPhantomRegistrationAlgo();

protected:
	/*! Point array holding the defined landmarks from the configuration file */
	vtkPoints*								DefinedLandmarks;

	/*! Names of the defined phantom landmarks from the configuration file */
	std::vector<std::string>	DefinedLandmarkNames;

	/*! Point array holding the recorded landmarks */
	vtkPoints*								RecordedLandmarks;

  /*! Phantom to reference transform matrix - the result of the registration */
	vtkMatrix4x4*							PhantomToReferenceTransformMatrix;

	/*! The mean error of the landmark registration in mm */
	double									  RegistrationError;

  /*! Name of the phantom coordinate frame (eg. Phantom) */
  char*                     PhantomCoordinateFrame;

  /*! Name of the reference coordinate frame (eg. Reference) */
  char*                     ReferenceCoordinateFrame;

  /*! Name of the stylus tip coordinate frame (eg. StylusTip) */
  char*                     StylusTipCoordinateFrame;

};

#endif
