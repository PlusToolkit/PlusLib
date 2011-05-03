#ifndef __vtkAMSTracker_h
#define __vtkAMSTracker_h

#include "vtkTracker.h"
#include "AMSStepper.h"

class vtkTransform;

class VTK_EXPORT vtkAMSTracker : public vtkTracker
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

	static vtkAMSTracker *New();
	vtkTypeMacro(vtkAMSTracker,vtkTracker);
	void PrintSelf(ostream& os, vtkIndent indent);

	// Description:
	// Connect to device
	int Connect();

	// Description:
	// Disconnect from device 
	virtual void Disconnect();

	// Description:
	// Probe to see if the tracking system is present on the
	// specified serial port.  If the SerialPort is set to -1,
	// then all serial ports will be checked.
	int Probe();

	// Description:
	// Reset the calibration state of the stepper
	// After this, you should call CalibrateStepper() for calibration 
	void ResetCalibration(); 

	// Description:
	// Calibrates the stepper with the 9 steps factory specified calibration 
	// process. It will returns true, if the calibration done. 
	bool CalibrateStepper( std::string &calibMsg );

	// Description:
	// Get the buffer element values of each tool in a string list by timestamp. 
	void GetTrackerToolBufferStringList(const double timestamp, std::vector<std::string> &toolNames, std::vector<std::string> &toolBufferValues, std::vector<std::string> &toolBufferStatuses); 

	// Description:
	// Get stepper encoder values from the buffer, where '0' is the most recent and
	// (NumberOfItems-1) is the oldest.
	void GetStepperEncoderValues( int bufferIndex, double &probePosition, double &probeRotation, double &templatePosition, long &flags ); 

	// Description:
	// Get stepper encoder values in a particular timestamp, where the timestamp is
	// in system time as returned by vtkAccurateTimer::GetSystemTime().
	void GetStepperEncoderValues(double timestamp, double &probePosition, double &probeRotation, double &templatePosition, long &flags ); 

	// Description:
	// Get probe position from the buffer, where '0' is the most recent and
	// (NumberOfItems-1) is the oldest.
	double GetProbePosition( int bufferIndex, long &flags ); 

	// Description:
	// Get probe position in a particular timestamp, where the timestamp is
	// in system time as returned by vtkAccurateTimer::GetSystemTime().
	double GetProbePosition( double timestamp, long &flags ); 

	// Description:
	//  Get probe rotation from the buffer, where '0' is the most recent and
	// (NumberOfItems-1) is the oldest.
	double GetProbeRotation( int bufferIndex, long &flags  ); 

	// Description:
	// Get probe rotation in a particular timestamp, where the timestamp is
	// in system time as returned by vtkAccurateTimer::GetSystemTime().
	double GetProbeRotation( double timestamp, long &flags  );

	// Description:
	// Get template position from the buffer, where '0' is the most recent and
	// (NumberOfItems-1) is the oldest.
	double GetTemplatePosition( int bufferIndex, long &flags  ); 

	// Description:
	// Get template position in a particular timestamp, where the timestamp is
	// in system time as returned by vtkAccurateTimer::GetSystemTime().
	double GetTemplatePosition( double timestamp, long &flags  ); 
	
	// Description:
	// Get probe home to probe transform from the buffer, where '0' is the most recent and
	// (NumberOfItems-1) is the oldest.
	vtkTransform* GetProbeHomeToProbeTransform( int bufferIndex, long &flags ); 

	// Description:
	// Get probe home to probe transform in a particular timestamp, where the timestamp is
	// in system time as returned by vtkAccurateTimer::GetSystemTime().
	vtkTransform* GetProbeHomeToProbeTransform( double timestamp, long &flags ); 

	// Description:
	// Get template home to template transform from the buffer, where '0' is the most recent and
	// (NumberOfItems-1) is the oldest.
	vtkTransform* GetTemplateHomeToTemplateTransform( int bufferIndex, long &flags ); 

	// Description:
	// Get template home to template transform in a particular timestamp, where the timestamp is
	// in system time as returned by vtkAccurateTimer::GetSystemTime().
	vtkTransform* GetTemplateHomeToTemplateTransform( double timestamp, long &flags );

	// Description:
	// Getraw encoder values transform from the buffer, where '0' is the most recent and
	// (NumberOfItems-1) is the oldest.
	vtkTransform* GetRawEncoderValuesTransform( int bufferIndex, long &flags ); 

	// Description:
	// Get raw encoder values transform in a particular timestamp, where the timestamp is
	// in system time as returned by vtkAccurateTimer::GetSystemTime().
	vtkTransform* GetRawEncoderValuesTransform( double timestamp, long &flags ); 

	// Description:
	// Get the a string (perhaps a long one) describing the type and version
	// of the device.
	vtkGetStringMacro(Version);

	// Description:
	// Set which serial port to use, COM1 through COM4.
	vtkSetStringMacro(SerialPort);
	vtkGetStringMacro(SerialPort);

	// Description:
	// Set the desired baud rate.  Default: 9600.  
	vtkSetMacro(BaudRate, unsigned long);
	vtkGetMacro(BaudRate, unsigned long);

	// Description:
	// Set/get stepper model number
	vtkSetMacro(ModelNumber, int);
	vtkGetMacro(ModelNumber, int);

	// Description:
	// Set/get stepper serial number 
	vtkSetMacro(SerialNumber, int);
	vtkGetMacro(SerialNumber, int);

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
	void InternalUpdate();

	// Description:
	// Read/write BrachyStepper configuration to xml data
	void ReadConfiguration(vtkXMLDataElement* config); 
	void WriteConfiguration(vtkXMLDataElement* config); 

protected:
	vtkAMSTracker();
	~vtkAMSTracker();

	// Description:
	// Set the version information.
	vtkSetStringMacro(Version);

	// Description:
	// Start the tracking system.  The tracking system is brought from
	// its ground state into full tracking mode.  The device will
	// only be reset if communication cannot be established without
	// a reset.
	int InternalStartTracking();

	// Description:
	// Stop the tracking system and bring it back to its ground state:
	// Initialized, not tracking, at 9600 Baud.
	int InternalStopTracking();

	// Description:
	// Initialize the tracking machine
	bool InitAMSTracker();

	AMSStepper *Device;

	char *Version;
	int ModelNumber; 
	int SerialNumber; 

	char* SerialPort; 
	unsigned long BaudRate;

	// Probe translation axis orientation vector[Tx, Ty, 1]
	double ProbeTranslationAxisOrientation[3]; 

	// Probe rotation axis orientation vector[Trx, Try, 1]
	double ProbeRotationAxisOrientation[3]; 

	// Template translation axis orientation vector[Tx, Ty, 1]
	double TemplateTranslationAxisOrientation[3]; 

	double ProbeRotationEncoderScale; 

	vtkTransform* ProbeHomeToProbeTransform; 
	vtkTransform* TemplateHomeToTemplateTransform; 
	vtkTransform* RawEncoderValuesTransform; 
	

private:
	vtkAMSTracker(const vtkAMSTracker&);
	void operator=(const vtkAMSTracker&);  
};

#endif
