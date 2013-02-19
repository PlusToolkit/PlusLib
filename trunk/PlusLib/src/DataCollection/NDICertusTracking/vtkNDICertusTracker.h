/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkNDICertusTracker_h
#define __vtkNDICertusTracker_h

#include "vtkPlusDevice.h"
#include "ndicapi.h"
#include <map>

/*! the number of tools this class can handle */
#define VTK_CERTUS_NTOOLS 12

/*!
\class vtkNDICertusTracker 
\brief Interface for the NDI Optotrak Certus tracking device 

The vtkNDICertusTracker class provides an interface to the Optotrak
Certus (Northern Digital Inc., Waterloo, Canada)

\ingroup PlusLibTracking
*/
class VTK_EXPORT vtkNDICertusTracker : public vtkPlusDevice
{
public:

	static vtkNDICertusTracker *New();
	vtkTypeMacro(vtkNDICertusTracker,vtkPlusDevice);
	void PrintSelf(ostream& os, vtkIndent indent);

  /*! Hardware device SDK version. */
  virtual std::string GetSdkVersion(); 

	/*! Connect to device */
	virtual PlusStatus InternalConnect();

	/*! Disconnect from device */
	virtual PlusStatus InternalDisconnect();

	/*! 
    Probe to check whether there is an attached Certus system that
	  is able to track.  After Probe is called, you can call
	  GetVersion() to get information about the attached Certus system.
  */
	PlusStatus Probe();

	/*! Get the a string (perhaps a long one) describing the type and version of the device. */
	vtkGetStringMacro(Version);

	/*! Get an update from the tracking system and push the new transforms to the tools. */
	PlusStatus InternalUpdate();

protected:
	vtkNDICertusTracker();
	~vtkNDICertusTracker();

  /*! Set the version information. */
	vtkSetStringMacro(Version);

	/*! 
    Start the tracking system.  The tracking system is brought from
	  its ground state into full tracking mode.  The device will
	  only be reset if communication cannot be established without
	  a reset.
  */
	PlusStatus InternalStartRecording();

	/*! Stop the tracking system and bring it back to its ground state: Initialized, not tracking, at 9600 Baud. */
	PlusStatus InternalStopRecording();

	/*! Cause the device to beep the specified number of times. */
	PlusStatus InternalBeep(int n);

	/*! Set the specified tool LED to the specified state. */
	PlusStatus InternalSetToolLED(const char* portName, int led, int state);

  /*! Set the specified tool LED to the specified state. */
	PlusStatus InternalSetToolLED(int portNumber, int led, int state);

	/*! Initialize communication with the Certus system. */
	PlusStatus InitializeCertusSystem();

	/*! Terminate communication with the Certus system. */
	PlusStatus ShutdownCertusSystem();

	/*! Activate the markers for tracking. */
	PlusStatus ActivateCertusMarkers();

	/*! Deactivate all markers. */
	PlusStatus DeActivateCertusMarkers();

	/*! Methods for detecting which ports have tools in them, and auto-enabling those tools. */
	PlusStatus EnableToolPorts();
	PlusStatus DisableToolPorts();

	/*! Find the tool for a specific port handle (-1 if not found). */
	int GetToolFromHandle(int handle);

  /*! Version information */
	char *Version;

  /*! Number of markers */
	int NumberOfMarkers;

  /*! Number of rigid bodies */
	int NumberOfRigidBodies;

  /*! Internal matrix used for storing the actual tool matrix */
	vtkMatrix4x4 *SendMatrix;

  /*! Container used for storing enabled tools */
	int PortEnabled[VTK_CERTUS_NTOOLS];

  /*! Container used for storing tool handles */
	int PortHandle[VTK_CERTUS_NTOOLS];

  /*! Container used for storing rigid body maps */
  std::map<int,int> RigidBodyMap;

private:
	vtkNDICertusTracker(const vtkNDICertusTracker&);
	void operator=(const vtkNDICertusTracker&);  
};

#endif





