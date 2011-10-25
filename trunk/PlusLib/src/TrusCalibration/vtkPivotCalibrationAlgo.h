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
#include "vtkTransform.h"
#include "vtkTracker.h"

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

	/*!
	  Read tool calibration matrix from XML data element
	  \param aConfig Root XML data element containing the tool calibration
	  \param aType Type of the tool with the calibration
	  \return Success flag
	*/
	PlusStatus ReadConfiguration(vtkXMLDataElement* aConfig, TRACKER_TOOL_TYPE aType);

  /*!
	  Save result to XML data element
	  \param aConfig Root XML data element containing the tool calibration
	  \param aType The result is saved in the first tool element of this type
	  \return Success flag
	*/
	PlusStatus WriteConfiguration(vtkXMLDataElement* aConfig, TRACKER_TOOL_TYPE aType);

public:

	vtkGetMacro(CalibrationError, double);

  vtkGetObjectMacro(TooltipToToolTransform, vtkTransform); 

  vtkGetVector3Macro(TooltipPosition, double);

protected:

  vtkSetObjectMacro(TooltipToToolTransform, vtkTransform);

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
	vtkTransform*				TooltipToToolTransform;

	/*! Uncertainty (standard deviation), error of the calibration result in mm */
	double							CalibrationError;

  /*! Minimizer algorithm object */
  vtkAmoebaMinimizer* Minimizer;

  /*! Array of the input points */
  vtkDoubleArray*     CalibrationArray;

};

#endif
