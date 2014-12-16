/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

#ifndef __vtkPivotCalibrationAlgo_h
#define __vtkPivotCalibrationAlgo_h

#include "PlusConfigure.h"

#include "vtkObject.h"
#include "vtkAmoebaMinimizer.h"
#include "vtkDoubleArray.h"
#include "vtkMatrix4x4.h"

class vtkTransformRepository;
class vtkXMLDataElement;

//-----------------------------------------------------------------------------

/*!
  \class vtkPivotCalibrationAlgo 
  \brief Pivot calibration algorithm to calibrate a stylus. It determines the position of the stylus tip relative to the marker attached to the stylus.
  \ingroup PlusLibCalibrationAlgorithm
*/
class vtkPivotCalibrationAlgo : public vtkObject
{
public:
  vtkTypeRevisionMacro(vtkPivotCalibrationAlgo,vtkObject);
	static vtkPivotCalibrationAlgo *New();

  /*! Initialize algorithm - clear calibration array */
  PlusStatus Initialize();

  /*!
  * Read configuration
  * \param aConfig Root element of the device set configuration
  */
  PlusStatus ReadConfiguration(vtkXMLDataElement* aConfig);

  /*!
    Insert acquired point to calibration point list
    \param aMarkerToReferenceTransformMatrix New calibration point (tool to reference transform)
	*/
  PlusStatus InsertNextCalibrationPoint(vtkSmartPointer<vtkMatrix4x4> aMarkerToReferenceTransformMatrix);

  /*!
    Calibrate (call the minimizer and set the result)
    \param aTransformRepository Transform repository to save the results into
  */
  PlusStatus DoPivotCalibration(vtkTransformRepository* aTransformRepository = NULL);

	/*!
    Get calibration result string to display
	  \return Calibration result (eg. stylus tip to stylus translation) string
	*/
	std::string GetPivotPointToMarkerTranslationString();

public:

	vtkGetMacro(CalibrationError, double);

  vtkGetObjectMacro(PivotPointToMarkerTransformMatrix, vtkMatrix4x4); 

  vtkGetVector3Macro(PivotPointPosition, double);

  vtkGetStringMacro(ObjectMarkerCoordinateFrame);
  vtkGetStringMacro(ReferenceCoordinateFrame);
  vtkGetStringMacro(ObjectPivotPointCoordinateFrame);

protected:

  vtkSetObjectMacro(PivotPointToMarkerTransformMatrix, vtkMatrix4x4);

  vtkSetObjectMacro(Minimizer, vtkAmoebaMinimizer);

  vtkSetObjectMacro(CalibrationArray, vtkDoubleArray);

  vtkSetStringMacro(ObjectMarkerCoordinateFrame);
  vtkSetStringMacro(ReferenceCoordinateFrame);
  vtkSetStringMacro(ObjectPivotPointCoordinateFrame);

protected:
	vtkPivotCalibrationAlgo();
	virtual	~vtkPivotCalibrationAlgo();

protected:
  /*! Callback function for the minimizer (function to minimize) */
  friend void vtkTrackerToolCalibrationFunction(void *userData);

protected:
	/*! Pivot point to marker transform (eg. stylus tip to stylus) - the result of the calibration */
	vtkMatrix4x4*				PivotPointToMarkerTransformMatrix;

	/*! Uncertainty (standard deviation), error of the calibration result in mm */
	double							CalibrationError;

  /*! Minimizer algorithm object */
  vtkAmoebaMinimizer* Minimizer;

  /*! Array of the input points */
  vtkDoubleArray*     CalibrationArray;

  /*! Name of the object marker coordinate frame (eg. Stylus) */
  char*               ObjectMarkerCoordinateFrame;

  /*! Name of the reference coordinate frame (eg. Reference) */
  char*               ReferenceCoordinateFrame;

  /*! Name of the object pivot point coordinate frame (eg. StylusTip) */
  char*               ObjectPivotPointCoordinateFrame;

  /*! Pivot point position in reference frame */
  double              PivotPointPosition[3];
};

#endif