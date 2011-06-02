#ifndef __vtkSavedDataTracker_h
#define __vtkSavedDataTracker_h

#include "vtkTracker.h"

class vtkTrackerBuffer; 

class VTK_EXPORT vtkSavedDataTracker : public vtkTracker
{
public:

	static vtkSavedDataTracker *New();
	vtkTypeMacro(vtkSavedDataTracker,vtkTracker);
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
	// Get an update from the tracking system and push the new transforms
	// to the tools.  This should only be used within vtkTracker.cxx.
	void InternalUpdate();

	// Description:
	// Read/write tracker configuration to xml data
	void ReadConfiguration(vtkXMLDataElement* config); 
	void WriteConfiguration(vtkXMLDataElement* config); 

	// Description:
	// Set/get SequenceMetafile name with path with tracking buffer data 
	vtkSetStringMacro(SequenceMetafile);
	vtkGetStringMacro(SequenceMetafile);

	// Description:
	// Set/get tracking start timestamp 
	vtkSetMacro(StartTimestamp, double); 
	vtkGetMacro(StartTimestamp, double); 
	
	//! Description 
	// Flag to to enable saved dataset reply
	// If it's enabled, the video source will continuously play saved data
	vtkGetMacro(ReplayEnabled, bool);
	vtkSetMacro(ReplayEnabled, bool);
	vtkBooleanMacro(ReplayEnabled, bool);

protected:
	vtkSavedDataTracker();
	~vtkSavedDataTracker();

	// Description:
	// Initialize the tracking device
	bool InitSavedDataTracker();

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

	char* SequenceMetafile; 
	double StartTimestamp; 
	bool ReplayEnabled; 

	vtkTrackerBuffer* LocalTrackerBuffer; 
	
	bool Initialized;
	
	
private:
	vtkSavedDataTracker(const vtkSavedDataTracker&);
	void operator=(const vtkSavedDataTracker&);  
};

#endif
