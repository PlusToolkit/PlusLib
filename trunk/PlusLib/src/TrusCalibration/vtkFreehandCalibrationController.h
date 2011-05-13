#ifndef __vtkFreehandCalibrationController_h
#define __vtkFreehandCalibrationController_h

#include "AbstractToolboxController.h"

#include "vtkCalibrationController.h"

//-----------------------------------------------------------------------------

/*!
* \brief Control operations for freehand calibration toolbox - singleton class
*/
class vtkFreehandCalibrationController : public vtkCalibrationController, public AbstractToolboxController //TODOsajt a filet is atnevezni
{
public:
	/*!
	* \brief New
	*/
	static vtkFreehandCalibrationController *New();

	/*!
	* \brief Instance getter for the singleton class
	* \return Instance object
	*/
	static vtkFreehandCalibrationController* GetInstance();

	/*!
	* \brief Destructor
	*/
	virtual	~vtkFreehandCalibrationController();

	/*!
	* \brief Initialize - implementation of a pure virtual function
	*/
	void Initialize();

	/*!
	* \brief Clear - implementation of a pure virtual function
	*/
	void Clear();

	/*!
	* \brief Acquire new tracker positions and updates toolbox and canvas - implementation of a pure virtual function
	*/
	void DoAcquisition();

	/*!
	* \brief Start calibration - implementation of a pure virtual function
	*/
	void Start();

	/*!
	* \brief Stop calibration - implementation of a pure virtual function
	*/
	void Stop();

	//TODO
	void StartTemporalCalibration();
	void StartSpatialCalibration();
	void SaveCalibrationResults(std::string aFile);
	static void UpdateProgress(int aPercent);

	// Set/Get macros for member variables
	vtkSetMacro(TemporalCalibrationDone, bool); 
	vtkGetMacro(TemporalCalibrationDone, bool); 
	vtkBooleanMacro(TemporalCalibrationDone, bool); 

	vtkSetMacro(ProgressPercent, int); 
	vtkGetMacro(ProgressPercent, int); 

protected:
	/*!
	* \brief Constructor
	*/
	vtkFreehandCalibrationController();

protected:
	//TODO
	bool TemporalCalibrationDone;
	int ProgressPercent;

private:
	//! Instance of the singleton
	static vtkFreehandCalibrationController*	Instance;
};

#endif
