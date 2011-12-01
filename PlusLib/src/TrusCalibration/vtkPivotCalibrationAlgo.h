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

public:
  PlusStatus Initialize();

  /*!
    Insert acquired point to calibration point list
    \param aToolToReferenceTransformMatrix New calibration point (tool to reference transform)
	*/
  PlusStatus InsertNextCalibrationPoint(vtkSmartPointer<vtkMatrix4x4> aToolToReferenceTransformMatrix);

  /*! Calibrate tooltip (call the minimizer and set the result) */
  PlusStatus DoTooltipCalibration();

	/*!
    Get calibration result string to display
	  \return Calibration result - stylus tip to stylus translation - string
	*/
	std::string GetTooltipToToolTranslationString();

public:

	vtkGetMacro(CalibrationError, double);

  vtkGetObjectMacro(TooltipToToolTransformMatrix, vtkMatrix4x4); 

  vtkGetVector3Macro(TooltipPosition, double);

protected:

  vtkSetObjectMacro(TooltipToToolTransformMatrix, vtkMatrix4x4);

  vtkSetObjectMacro(Minimizer, vtkAmoebaMinimizer);

  vtkSetObjectMacro(CalibrationArray, vtkDoubleArray);

protected:
	vtkPivotCalibrationAlgo();
	virtual	~vtkPivotCalibrationAlgo();

protected:
  /*! Callback function for the minimizer (function to minimize) */
  friend void vtkTrackerToolCalibrationFunction(void *userData);

protected:
  /*! Tool tip position in reference frame */
  double              TooltipPosition[3];

	/*! Tooltip to tool transform - the result of the calibration */
	vtkMatrix4x4*				TooltipToToolTransformMatrix;

	/*! Uncertainty (standard deviation), error of the calibration result in mm */
	double							CalibrationError;

  /*! Minimizer algorithm object */
  vtkAmoebaMinimizer* Minimizer;

  /*! Array of the input points */
  vtkDoubleArray*     CalibrationArray;

};

#endif
