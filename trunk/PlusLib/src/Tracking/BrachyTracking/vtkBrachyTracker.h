#ifndef __vtkBrachyTracker_h
#define __vtkBrachyTracker_h

#include "PlusConfigure.h"
#include "vtkTracker.h"

class BrachyStepper; 
class CmsBrachyStepper; 
class vtkTransform;


class VTK_EXPORT vtkBrachyTracker : public vtkTracker
{
public:

	enum BRACHY_STEPPER_TOOL
	{
		PROBEHOME_TO_PROBE_TRANSFORM=0, 
		TEMPLATEHOME_TO_TEMPLATE_TRANSFORM, 
		RAW_ENCODER_VALUES, 
		NUMBER_OF_BRACHY_TOOLS
	}; 

	enum ENCODER_MATRIX_ROW
	{
		ROW_PROBE_POSITION=0, 
		ROW_PROBE_ROTATION=1, 
		ROW_TEMPLATE_POSITION=2
	}; 

	static vtkBrachyTracker *New();
	vtkTypeMacro(vtkBrachyTracker,vtkTracker);
	void PrintSelf(ostream& os, vtkIndent indent);

	// Description:
	// Connect to device
	PlusStatus Connect();

	// Description:
	// Disconnect from device 
	virtual PlusStatus Disconnect();

  // Description:
	// Probe to see if the tracking system is present on the
	// specified serial port.  If the SerialPort is set to -1,
	// then all serial ports will be checked.
	PlusStatus Probe();

	// Description:
	// Reset the calibration state of the stepper
	// After this, you should call CalibrateStepper() for calibration 
	void ResetCalibration(); 

	// Description:
	// Calibrates the stepper with the 9 steps factory specified calibration 
	// process. It will returns true, if the calibration done. 
	PlusStatus CalibrateStepper( std::string &calibMsg );

	// Description:
	// Get the buffer element values of each tool in a string list by timestamp. 
	PlusStatus GetTrackerToolBufferStringList(double timestamp, 
		std::map<std::string, std::string> &toolsBufferMatrices, 
		std::map<std::string, std::string> &toolsCalibrationMatrices, 
		std::map<std::string, std::string> &toolsStatuses,
		bool calibratedTransform = false); 

	// Description:
	// Get stepper encoder values from the buffer by UID
	PlusStatus GetStepperEncoderValues( BufferItemUidType uid, double &probePosition, double &probeRotation, double &templatePosition, TrackerStatus &status ); 

	// Description:
	// Get stepper encoder values in a particular timestamp, where the timestamp is
	// in system time as returned by vtkAccurateTimer::GetSystemTime().
	PlusStatus GetStepperEncoderValues(double timestamp, double &probePosition, double &probeRotation, double &templatePosition, TrackerStatus &status ); 
	
	// Description:
	// Get probe home to probe transform from the buffer, where '0' is the most recent and
	// (NumberOfItems-1) is the oldest.
	PlusStatus GetProbeHomeToProbeTransform( BufferItemUidType uid, vtkMatrix4x4* probeHomeToProbeMatrix, TrackerStatus &status, bool calibratedTransform = false ); 

	// Description:
	// Get probe home to probe transform in a particular timestamp, where the timestamp is
	// in system time as returned by vtkAccurateTimer::GetSystemTime().
	PlusStatus GetProbeHomeToProbeTransform( double timestamp, vtkMatrix4x4* probeHomeToProbeMatrix, TrackerStatus &status, bool calibratedTransform = false ); 

	// Description:
	// Get template home to template transform from the buffer, where '0' is the most recent and
	// (NumberOfItems-1) is the oldest.
	PlusStatus GetTemplateHomeToTemplateTransform( BufferItemUidType uid, vtkMatrix4x4* templateHomeToTemplateMatrix, TrackerStatus &status, bool calibratedTransform = false ); 

	// Description:
	// Get template home to template transform in a particular timestamp, where the timestamp is
	// in system time as returned by vtkAccurateTimer::GetSystemTime().
	PlusStatus GetTemplateHomeToTemplateTransform( double timestamp, vtkMatrix4x4* templateHomeToTemplateMatrix, TrackerStatus &status, bool calibratedTransform = false );

	// Description:
	// Get raw encoder values transform from the buffer
	PlusStatus GetRawEncoderValuesTransform( BufferItemUidType uid, vtkMatrix4x4* rawEncoderValuesTransform, TrackerStatus &status ); 

	// Description:
	// Get raw encoder values transform in a particular timestamp, where the timestamp is
	// in system time as returned by vtkAccurateTimer::GetSystemTime().
	PlusStatus GetRawEncoderValuesTransform( double timestamp, vtkMatrix4x4* rawEncoderValuesTransform, TrackerStatus &status); 

	// Description:
	// Get the a string (perhaps a long one) describing the type and version
	// of the device.
	vtkGetStringMacro(ModelVersion);

 	// Description:
	// Get stepper model number
	vtkGetStringMacro(ModelNumber);

	// Description:
	// Get stepper serial number 
	vtkGetStringMacro(ModelSerialNumber);

	// Description:
	// Set which serial port to use, COM1 through COM4.
	vtkSetMacro(SerialPort, unsigned long);
	vtkGetMacro(SerialPort, unsigned long);

	// Description:
	// Set the desired baud rate.  Default: 9600.  
	vtkSetMacro(BaudRate, unsigned long);
	vtkGetMacro(BaudRate, unsigned long);

  // Description:
	// Get stepper calibration algorithm version 
	vtkGetStringMacro(CalibrationAlgorithmVersion);

  // Description:
	// Get stepper calibration date
	vtkGetStringMacro(CalibrationDate);
  
	// Description:
	// Set/get probe translation axis orientation vector[Tx, Ty, 1]
	vtkSetVector3Macro(ProbeTranslationAxisOrientation, double); 
	vtkGetVector3Macro(ProbeTranslationAxisOrientation, double); 

	// Description:
	// Set/get probe rotation axis orientation vector[Trx, Try, 1]
	vtkSetVector3Macro(ProbeRotationAxisOrientation, double); 
	vtkGetVector3Macro(ProbeRotationAxisOrientation, double); 

	// Description:
	// Set/get template translation axis orientation vector[Tx, Ty, 1]
	vtkSetVector3Macro(TemplateTranslationAxisOrientation, double); 
	vtkGetVector3Macro(TemplateTranslationAxisOrientation, double); 

	// Description:
	// Set/get probe rotation encoder scale
	vtkSetMacro(ProbeRotationEncoderScale, double); 
	vtkGetMacro(ProbeRotationEncoderScale, double); 

	// Description:
	// Get an update from the tracking system and push the new transforms
	// to the tools.  This should only be used within vtkTracker.cxx.
	PlusStatus InternalUpdate();

	// Description:
	// Read/write BrachyStepper configuration to xml data
	PlusStatus ReadConfiguration(vtkXMLDataElement* config); 
	PlusStatus WriteConfiguration(vtkXMLDataElement* config); 

protected:
	vtkBrachyTracker();
	~vtkBrachyTracker();

	// Description:
	// Set the stepper model version information.
	vtkSetStringMacro(ModelVersion);

  // Description:
	// Set the stepper model number
	vtkSetStringMacro(ModelNumber);

  // Description:
	// Set the stepper serial number
	vtkSetStringMacro(ModelSerialNumber);

  // Description:
	// Set stepper calibration algorithm version 
	vtkSetStringMacro(CalibrationAlgorithmVersion);

  // Description:
	// Set stepper calibration date
	vtkSetStringMacro(CalibrationDate);
  
	// Description:
	// Start the tracking system.  The tracking system is brought from
	// its ground state into full tracking mode.  The device will
	// only be reset if communication cannot be established without
	// a reset.
	PlusStatus InternalStartTracking();

	// Description:
	// Stop the tracking system and bring it back to its ground state:
	// Initialized, not tracking, at 9600 Baud.
	PlusStatus InternalStopTracking();

	// Description:
	// Initialize the tracking device
	PlusStatus InitBrachyTracker();

	BrachyStepper *Device;

	char *ModelVersion;
	char *ModelNumber; 
	char *ModelSerialNumber; 

	unsigned long SerialPort; 
	unsigned long BaudRate;

  //========== Stepper calibration ==================
  
  // Stepper calibration algorithm version 
  char* CalibrationAlgorithmVersion; 

  // Stepper calibration data
  char* CalibrationDate; 

	// Probe translation axis orientation vector[Tx, Ty, 1]
	double ProbeTranslationAxisOrientation[3]; 

	// Probe rotation axis orientation vector[Trx, Try, 1]
	double ProbeRotationAxisOrientation[3]; 

	// Template translation axis orientation vector[Tx, Ty, 1]
	double TemplateTranslationAxisOrientation[3]; 

	double ProbeRotationEncoderScale; 

private:
	vtkBrachyTracker(const vtkBrachyTracker&);
	void operator=(const vtkBrachyTracker&);  
};

#endif
