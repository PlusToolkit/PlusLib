/* ============================================================================

File: vtkFakeTracker.h
Author: Kyle Charbonneau <kcharbon@imaging.robarts.ca>
Language: C++
Description: 
This class represents a fake tracking system with tools that have
predetermined behaviour. This allows someonew who doesn't have access to
a tracking system to test code that relies on having one active.

============================================================================ */

#ifndef __vtkFakeTracker_h
#define __vtkFakeTracker_h

#include "vtkTracker.h"

enum FakeTrackerMode
{
	FakeTrackerMode_Undefined = 0,
	FakeTrackerMode_Default,
  FakeTrackerMode_SmoothMove,
	FakeTrackerMode_PivotCalibration,
	FakeTrackerMode_RecordPhantomLandmarks
};

class vtkTransform;

class VTK_EXPORT vtkFakeTracker : public vtkTracker
{
public:
	static vtkFakeTracker *New();
	vtkTypeMacro(vtkFakeTracker,vtkTracker);

	// Description:
	// Connect to device
	PlusStatus Connect();

	// Description:
	// Disconnect from device 
	PlusStatus Disconnect();

	// Description:
	// Checks availibility of tracker (this always returns 1)
	PlusStatus Probe();

	// Description:
	// Read configuration from xml data
	PlusStatus ReadConfiguration(vtkXMLDataElement* config); 

	// Set fake mode (tool info for fake tools)
	void SetMode(FakeTrackerMode);

	vtkSetMacro(SerialPort, int);
	vtkGetMacro(SerialPort, int);

	vtkSetMacro(Counter, int);

protected:
	// Description:
	// Start the tracking system.
	PlusStatus InternalStartTracking();

	// Description:
	// Stop the tracking system.
	PlusStatus InternalStopTracking();

	// Description:
	// Get an update from the tracking system and push the new transforms
	// to the tools.  This should only be used within vtkTracker.cxx.
	PlusStatus InternalUpdate();

	// Description:
	// Constructor
	vtkFakeTracker();

	// Description:
	// Destructor
	~vtkFakeTracker();

protected:
	int Frame;

	vtkTransform *InternalTransform;

	int SerialPort;

	FakeTrackerMode Mode;

	int RandomSeed;

	int Counter;
};


#endif
