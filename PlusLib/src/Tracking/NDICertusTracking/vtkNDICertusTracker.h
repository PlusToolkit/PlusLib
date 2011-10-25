/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

/*=========================================================================
The following copyright notice is applicable to parts of this file:

Copyright (c) 2000-2005 Atamai, Inc.

Use, modification and redistribution of the software, in source or
binary forms, are permitted provided that the following terms and
conditions are met:

1) Redistribution of the source code, in verbatim or modified
form, must retain the above copyright notice, this license,
the following disclaimer, and any notices that refer to this
license and/or the following disclaimer.  

2) Redistribution in binary form must include the above copyright
notice, a copy of this license and the following disclaimer
in the documentation or with other materials provided with the
distribution.

3) Modified copies of the source code must be clearly marked as such,
and must not be misrepresented as verbatim copies of the source code.

THE COPYRIGHT HOLDERS AND/OR OTHER PARTIES PROVIDE THE SOFTWARE "AS IS"
WITHOUT EXPRESSED OR IMPLIED WARRANTY INCLUDING, BUT NOT LIMITED TO,
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
PURPOSE.  IN NO EVENT SHALL ANY COPYRIGHT HOLDER OR OTHER PARTY WHO MAY
ODIFY AND/OR REDISTRIBUTE THE SOFTWARE UNDER THE TERMS OF THIS LICENSE
BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, LOSS OF DATA OR DATA BECOMING INACCURATE
OR LOSS OF PROFIT OR BUSINESS INTERRUPTION) ARISING IN ANY WAY OUT OF
THE USE OR INABILITY TO USE THE SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGES.

=========================================================================*/
// .NAME vtkNDICertusTracker - VTK interface for the NDI Optotrak Certus
// .SECTION Description
// The vtkNDICertusTracker class provides an interface to the Optotrak
// Certus (Northern Digital Inc., Waterloo, Canada), utilizing the PCI
// interface card.
// .SECTION see also
// vtkNDITracker vtkTrackerTool


#ifndef __vtkNDICertusTracker_h
#define __vtkNDICertusTracker_h

#include "vtkTracker.h"
#include "ndicapi.h"
#include <map>

// the number of tools this class can handle
#define VTK_CERTUS_NTOOLS 12

class VTK_EXPORT vtkNDICertusTracker : public vtkTracker
{
public:

	static vtkNDICertusTracker *New();
	vtkTypeMacro(vtkNDICertusTracker,vtkTracker);
	void PrintSelf(ostream& os, vtkIndent indent);

	// Description:
	// Connect to device
	PlusStatus Connect();

	// Description:
	// Disconnect from device 
	virtual PlusStatus Disconnect();

	// Description:
	// Probe to check whether there is an attached Certus system that
	// is able to track.  After Probe is called, you can call
	// GetVersion() to get information about the attached Certus system.
	PlusStatus Probe();

	// Description:
	// Get the a string (perhaps a long one) describing the type and version
	// of the device.
	vtkGetStringMacro(Version);

	// Description:
	// Get an update from the tracking system and push the new transforms
	// to the tools.  This should only be used within vtkTracker.cxx.
	PlusStatus InternalUpdate();
	//PlusStatus Update();

protected:
	vtkNDICertusTracker();
	~vtkNDICertusTracker();

  // Description:
	// Set the version information.
	vtkSetStringMacro(Version);

	// Description:
	// Start the tracking system.  The tracking system is brought from
	// its ground state into full tracking mode.  The device will
	// only be reset if communication cannot be established without
	// a reset.
	PlusStatus InternalStartTracking();
	PlusStatus StartTracking();

	// Description:
	// Stop the tracking system and bring it back to its ground state:
	// Initialized, not tracking, at 9600 Baud.
	PlusStatus InternalStopTracking();
	PlusStatus StopTracking();

	// Description:
	// Cause the device to beep the specified number of times.
	PlusStatus InternalBeep(int n);

	// Description:
	// Set the specified tool LED to the specified state.
	PlusStatus InternalSetToolLED(int tool, int led, int state);

	// Description:
	// Initialize communication with the Certus system.
	PlusStatus InitializeCertusSystem();

	// Description:
	// Terminate communication with the Certus system.
	PlusStatus ShutdownCertusSystem();

	// Description:
	// Activate the markers for tracking.
	PlusStatus ActivateCertusMarkers();

	// Description:
	// Deactivate all markers.
	PlusStatus DeActivateCertusMarkers();

	// Description:
	// Methods for detecting which ports have tools in them, and
	// auto-enabling those tools.
	PlusStatus EnableToolPorts();
	PlusStatus DisableToolPorts();

	// Description:
	// Find the tool for a specific port handle (-1 if not found).
	int GetToolFromHandle(int handle);

	char *Version;

	int NumberOfMarkers;
	int NumberOfRigidBodies;

	vtkMatrix4x4 *SendMatrix;

	int PortEnabled[VTK_CERTUS_NTOOLS];
	int PortHandle[VTK_CERTUS_NTOOLS];
	std::map<int,int> RigidBodyMap;

private:
	vtkNDICertusTracker(const vtkNDICertusTracker&);
	void operator=(const vtkNDICertusTracker&);  
};

#endif





