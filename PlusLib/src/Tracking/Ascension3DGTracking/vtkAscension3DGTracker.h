
#ifndef __vtkAscension3DGTracker_h
#define __vtkAscension3DGTracker_h

#include "vtkTracker.h"

class vtkTrackerBuffer; 



class
VTK_EXPORT
vtkAscension3DGTracker : public vtkTracker
{
public:

	static vtkAscension3DGTracker *New();
	vtkTypeMacro( vtkAscension3DGTracker,vtkTracker );
	void PrintSelf( ostream& os, vtkIndent indent );

	// Description:
	// Connect to device
	PlusStatus Connect();

	// Description:
	// Disconnect from device 
	virtual PlusStatus Disconnect();

	// Description:
	// Probe to see if the tracking system is present on the
	// specified serial port.  If the SerialPort is set to -1, then all serial ports will be checked.
	PlusStatus Probe();

	// Description:
	// Get an update from the tracking system and push the new transforms
	// to the tools.  This should only be used within vtkTracker.cxx.
	PlusStatus InternalUpdate();

	// Description:
	// Read/write BrachyStepper configuration to xml data
	PlusStatus ReadConfiguration( vtkXMLDataElement* config ); 
	PlusStatus WriteConfiguration( vtkXMLDataElement* config );

	// Description:
	// Get number of sensors 
	vtkGetMacro(NumberOfSensors, int);
 
  
protected:
  
	vtkAscension3DGTracker();
	~vtkAscension3DGTracker();

	// Description:
	// Initialize the tracking device
	PlusStatus InitAscension3DGTracker();

	// Description:
	// Start the tracking system.  The tracking system is brought from its ground state into full tracking mode.
  // The device will only be reset if communication cannot be established without a reset.
	PlusStatus InternalStartTracking();

	// Description:
	// Stop the tracking system and bring it back to its ground state:
	// Initialized, not tracking, at 9600 Baud.
	PlusStatus InternalStopTracking();
  
	vtkTrackerBuffer* LocalTrackerBuffer; 
	
  
private:  // Definitions.
	
	enum {TRANSMITTER_OFF = -1};
	
	// typedef std::map< std::string, std::vector < double > >  TrackerToolTransformContainerType;
	
	
private:  // Functions.
  
  vtkAscension3DGTracker( const vtkAscension3DGTracker& );
	void operator=( const vtkAscension3DGTracker& );  
	
	PlusStatus CheckReturnStatus( int status );
  
  
private:  // Variables.
  	
	// TrackerToolTransformContainerType  ToolTransformBuffer;
	
	std::vector< bool > SensorSaturated;
	std::vector< bool > SensorAttached;
	std::vector< bool > SensorInMotion;
	
	bool TransmitterAttached;
	
	unsigned int FrameNumber;
	int NumberOfSensors; 
  
};

#endif
