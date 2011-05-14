#ifndef __vtkFreehandCalibrationController_h
#define __vtkFreehandCalibrationController_h

#include "AbstractToolboxController.h"

#include "vtkCalibrationController.h"

#include "BrachyTRUSCalibrator.h"

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
	* \brief VTK PrintSelf method
	*/
	virtual void PrintSelf(ostream& os, vtkIndent indent); 

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

	//TODO--------------------------
	void StartTemporalCalibration();
	void StartSpatialCalibration();
	void SaveCalibrationResults(std::string aFile);
	static void UpdateProgress(int aPercent);

	virtual void SetUSImageFrameOriginInPixels(int originX, int originY); 
	virtual void SetUSImageFrameOriginInPixels(int* origin); 

	// Returns the calibrator
	BrachyTRUSCalibrator* GetCalibrator() { return CalibrationPhantom; }

	// Read XML based configuration of the calibration controller
	virtual void ReadConfiguration( const char* configFileNameWithPath ); 
	virtual void ReadConfiguration( vtkXMLDataElement* configData ); 

	//! Operation: Register phantom geometry for calibrator 
	virtual void RegisterPhantomGeometry();

	//TODO--------------------------

	// Set/Get macros for member variables
	vtkSetMacro(TemporalCalibrationDone, bool); 
	vtkGetMacro(TemporalCalibrationDone, bool); 
	vtkBooleanMacro(TemporalCalibrationDone, bool); 

	vtkSetMacro(ProgressPercent, int); 
	vtkGetMacro(ProgressPercent, int); 

	vtkGetMacro(EnableSystemLog, bool);
	vtkSetMacro(EnableSystemLog, bool);
	vtkBooleanMacro(EnableSystemLog, bool);

	vtkGetMacro(USImageFrameOriginXInPixels, int);
	vtkSetMacro(USImageFrameOriginXInPixels, int);
	vtkGetMacro(USImageFrameOriginYInPixels, int);
	vtkSetMacro(USImageFrameOriginYInPixels, int);

	vtkTypeRevisionMacro(vtkFreehandCalibrationController, vtkCalibrationController);

protected:
	/*!
	* \brief Constructor
	*/
	vtkFreehandCalibrationController();

protected:
	//TODO
	bool TemporalCalibrationDone;
	int ProgressPercent;
	//! Attribute: a reference to the calibration phantom
	BrachyTRUSCalibrator* CalibrationPhantom;
	//! Attribute: Flag to enable the calibration log file
	bool EnableSystemLog;
	//! Attributes: The US image frame origin (in pixels) - These are the US image frame origin in pixels W.R.T. the left-upper corner of the original image, with X pointing to the right (column) and Y pointing down to the bottom (row)
	int USImageFrameOriginXInPixels;
	int USImageFrameOriginYInPixels;

private:
	//! Instance of the singleton
	static vtkFreehandCalibrationController*	Instance;
};

#endif
