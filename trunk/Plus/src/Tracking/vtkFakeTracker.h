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
	FakeTrackerMode_PivotCalibration,
	FakeTrackerMode_RecordPhantomLandmarks
};

class vtkTransform;

class VTK_EXPORT vtkFakeTracker: public vtkTracker
{
public:
	static vtkFakeTracker *New();
	vtkTypeMacro(vtkFakeTracker,vtkObject);

	// Description:
	// Connect to device
	int Connect();

	// Description:
	// Disconnect from device 
	void Disconnect();

	// Description:
	// Checks availibility of tracker (this always returns 1)
	int Probe();

	// Description:
	// Read configuration from xml data
	void ReadConfiguration(vtkXMLDataElement* config); 

	// Set fake mode (tool info for fake tools)
	void SetMode(FakeTrackerMode);

	vtkSetMacro(SerialPort, int);
	vtkGetMacro(SerialPort, int);

	vtkSetMacro(MainTool, int);
	vtkGetMacro(MainTool, int);

	vtkSetMacro(Counter, int);

protected:
	// Description:
	// Start the tracking system.
	int InternalStartTracking();

	// Description:
	// Stop the tracking system.
	int InternalStopTracking();

	// Description:
	// Get an update from the tracking system and push the new transforms
	// to the tools.  This should only be used within vtkTracker.cxx.
	void InternalUpdate();

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

	int MainTool;

	int RandomSeed;

	int Counter;
};


#endif
