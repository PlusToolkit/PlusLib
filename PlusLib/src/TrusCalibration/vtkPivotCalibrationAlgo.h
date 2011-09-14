#ifndef __vtkPivotCalibrationAlgo_h
#define __vtkPivotCalibrationAlgo_h

#include "vtkObject.h"
#include "vtkAmoebaMinimizer.h"
#include "vtkDoubleArray.h"
#include "vtkTransform.h"
#include "vtkTracker.h"

class vtkXMLDataElement;

//-----------------------------------------------------------------------------

/*!
* \brief Pivot calibration algorithm
*/
class vtkPivotCalibrationAlgo : public vtkObject
{
public:
  vtkTypeRevisionMacro(vtkPivotCalibrationAlgo,vtkObject);

  /*!
	* \brief New
	*/
	static vtkPivotCalibrationAlgo *New();

public:
  /*!
	* \brief Initialize calibration (empty calibration point list)
	*/
  PlusStatus Initialize();

  /*!
	* \brief Insert acquired point to calibration point list
  * \param aToolToReferenceTransformMatrix New calibration point (tool to reference transform)
	*/
  PlusStatus InsertNextCalibrationPoint(vtkSmartPointer<vtkMatrix4x4> aToolToReferenceTransformMatrix);

  /*!
	* \brief Calibrate tooltip (call the minimizer and set the result)
	*/
  PlusStatus DoTooltipCalibration();

	/*!
	* \brief Get calibration result string to display
	* \return Calibration result - stylus tip to stylus translation - string
	*/
	std::string GetTooltipToToolTranslationString();

	/*!
	* \brief Read tool calibration matrix from XML data element
	* \param aConfig Root XML data element containing the tool calibration
	* \param aType Type of the tool with the calibration
	* \return Success flag
	*/
	PlusStatus ReadConfiguration(vtkXMLDataElement* aConfig, TRACKER_TOOL_TYPE aType);

  /*!
	* \brief Save result to XML data element
	* \param aConfig Root XML data element containing the tool calibration
	* \param aType The result is saved in the first tool element of this type
	* \return Success flag
	*/
	PlusStatus WriteConfiguration(vtkXMLDataElement* aConfig, TRACKER_TOOL_TYPE aType);

public:
  // Set/Get macros
	vtkGetMacro(CalibrationError, double);

  vtkGetObjectMacro(TooltipToToolTransform, vtkTransform); 

  vtkGetVector3Macro(TooltipPosition, double);

protected:
  vtkSetObjectMacro(TooltipToToolTransform, vtkTransform);

  vtkSetObjectMacro(Minimizer, vtkAmoebaMinimizer);

  vtkSetObjectMacro(CalibrationArray, vtkDoubleArray);

protected:
	/*!
	* \brief Constructor
	*/
	vtkPivotCalibrationAlgo();

  /*!
	* \brief Destructor
	*/
	virtual	~vtkPivotCalibrationAlgo();

protected:
  /*!
	* \brief Callback function for the minimizer (function to minimize)
	*/
  friend void vtkTrackerToolCalibrationFunction(void *userData);

protected:
  //! Tool tip position in reference frame
  double              TooltipPosition[3];

	//! Tooltip to tool transform - the result of the calibration
	vtkTransform*				TooltipToToolTransform;

	//! Uncertainty (standard deviation), error of the calibration result in mm
	double							CalibrationError;

  //! Minimizer algorithm object
  vtkAmoebaMinimizer* Minimizer;

  //! Array of the input points
  vtkDoubleArray*     CalibrationArray;

};

#endif
