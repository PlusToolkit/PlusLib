#ifndef FREEHANDCALIBRATIONCONTROLLER_H
#define FREEHANDCALIBRATIONCONTROLLER_H

#include "AbstractToolboxController.h"

#include "vtkProbeCalibrationController.h"

//-----------------------------------------------------------------------------

/*!
* \brief Control operations for freehand calibration toolbox - singleton class
*/
class FreehandCalibrationController : public AbstractToolboxController
{
public:
	/*!
	* \brief Instance getter for the singleton class
	* \return Instance object
	*/
	static FreehandCalibrationController* GetInstance();

	/*!
	* \brief Destructor
	*/
	virtual	~FreehandCalibrationController();

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
	bool IsTemporalCalibrationDone();
	void SaveCalibrationResults(std::string aFile);

protected:
	/*!
	* \brief Constructor
	*/
	FreehandCalibrationController();

protected:
	//TODO
	vtkProbeCalibrationController* m_ProbeCalibrationController;
	bool m_TemporalCalibrationDone;

private:
	//! Instance of the singleton
	static FreehandCalibrationController*	m_Instance;
};

#endif
