/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

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
	// Get an update from the tracking system and push the new transforms
	// to the tools.  This should only be used within vtkTracker.cxx.
	PlusStatus InternalUpdate();

	// Description:
	// Read/write tracker configuration to xml data
	PlusStatus ReadConfiguration(vtkXMLDataElement* config); 
	PlusStatus WriteConfiguration(vtkXMLDataElement* config); 

	// Description:
	// Set/get SequenceMetafile name with path with tracking buffer data 
	vtkSetStringMacro(SequenceMetafile);
	vtkGetStringMacro(SequenceMetafile);

  // Description: 
  // Set/get loop start time 
  // itemTimestamp = loopStartTime + (actualTimestamp - startTimestamp) % loopTime 
	vtkSetMacro(LoopStartTime, double); 
	vtkGetMacro(LoopStartTime, double); 

  // Description: 
  // Set/get loop time 
  // itemTimestamp = loopStartTime + (actualTimestamp - startTimestamp) % loopTime 
	vtkSetMacro(LoopTime, double); 
	vtkGetMacro(LoopTime, double); 

	//! Description 
	// Flag to to enable saved dataset reply
	// If it's enabled, the video source will continuously play saved data
	vtkGetMacro(ReplayEnabled, bool);
	vtkSetMacro(ReplayEnabled, bool);
	vtkBooleanMacro(ReplayEnabled, bool);

  //! Description 
  // Get local tracker buffer 
  vtkGetObjectMacro(LocalTrackerBuffer, vtkTrackerBuffer); 

protected:
	vtkSavedDataTracker();
	~vtkSavedDataTracker();

	// Description:
	// Initialize the tracking device
	PlusStatus InitSavedDataTracker();

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

	char* SequenceMetafile; 
	bool ReplayEnabled; 

	vtkTrackerBuffer* LocalTrackerBuffer; 
	
	bool Initialized;
  long FrameNumber; 

  double LoopStartTime; 
  double LoopTime; 
	
	
private:
	vtkSavedDataTracker(const vtkSavedDataTracker&);
	void operator=(const vtkSavedDataTracker&);  
};

#endif
