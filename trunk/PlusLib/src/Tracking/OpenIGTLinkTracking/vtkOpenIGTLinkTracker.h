/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkOpenIGTLinkTracker_h
#define __vtkOpenIGTLinkTracker_h

#include "vtkTracker.h"

class vtkTrackerBuffer; 



/**
 * This is an OpenIGTLink client.
 * On connect, it sends the PlusServer a StartTracking plus command.
 * After that, PlusServer sends tracking data to this client as long as the connection is alive.
 * On "start tracking" this client starts buffering the incoming transforms.
 * On "stop tracking" it stops buffering.
 * On disconnect, it releases socket connection to server.
 * When PlusServer detects this, stops the tracking hardware.
 */
class
VTK_EXPORT
vtkOpenIGTLinkTracker : public vtkTracker
{
public:

	static vtkOpenIGTLinkTracker *New();
	vtkTypeMacro( vtkOpenIGTLinkTracker,vtkTracker );
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
	// Get number of device names
	vtkGetMacro(NumberOfSensors, int);
 
  
protected:
  
	vtkOpenIGTLinkTracker();
	~vtkOpenIGTLinkTracker();

	// Description:
	// Initialize the tracking device
	PlusStatus InitOpenIGTLinkTracker();

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
	
	// typedef std::map< std::string, std::vector < double > >  TrackerToolTransformContainerType;
	
	
private:  // Functions.
  
  vtkOpenIGTLinkTracker( const vtkOpenIGTLinkTracker& );
	void operator=( const vtkOpenIGTLinkTracker& );  
	
  
  
private:  // Variables.
  	
	
	unsigned int FrameNumber;
	int NumberOfSensors; 
  
};

#endif
