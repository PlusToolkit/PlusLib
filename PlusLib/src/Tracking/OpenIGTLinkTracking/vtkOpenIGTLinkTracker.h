/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkOpenIGTLinkTracker_h
#define __vtkOpenIGTLinkTracker_h

#include "vtkTracker.h"

class vtkTrackerBuffer; 

/*!
\class vtkOpenIGTLinkTracker 
\brief OpenIGTLink client  

On connect, it sends the PlusServer a StartTracking plus command.
After that, PlusServer sends tracking data to this client as long as the connection is alive.
On "start tracking" this client starts buffering the incoming transforms.
On "stop tracking" it stops buffering.
On disconnect, it releases socket connection to server.
When PlusServer detects this, stops the tracking hardware.

\ingroup PlusLibTracking
*/
class VTK_EXPORT vtkOpenIGTLinkTracker : public vtkTracker
{
public:

	static vtkOpenIGTLinkTracker *New();
	vtkTypeMacro( vtkOpenIGTLinkTracker,vtkTracker );
	void PrintSelf( ostream& os, vtkIndent indent );

	/*! Connect to device */
	PlusStatus Connect();

	/*! Disconnect from device */
	virtual PlusStatus Disconnect();

	/*! Probe to see if the tracking system is present on the specified serial port. */
	PlusStatus Probe();

	/*! Get an update from the tracking system and push the new transforms to the tools. This function is called by the tracker thread.*/
	PlusStatus InternalUpdate();

	/*! Get number of device names */
	vtkGetMacro(NumberOfSensors, int);
   
protected:
  
	vtkOpenIGTLinkTracker();
	~vtkOpenIGTLinkTracker();

	/*! Initialize the tracking device */
	PlusStatus InitOpenIGTLinkTracker();

	/*! 
    Start the tracking system.  The tracking system is brought from its ground state into full tracking mode.
    The device will only be reset if communication cannot be established without a reset.
  */
	PlusStatus InternalStartTracking();

	/*! Stop the tracking system and bring it back to its ground state: Initialized, not tracking, at 9600 Baud. */
	PlusStatus InternalStopTracking();
  
 
private:  // Definitions.
	
private:  // Functions.
  
  vtkOpenIGTLinkTracker( const vtkOpenIGTLinkTracker& );
	void operator=( const vtkOpenIGTLinkTracker& );  
  
private:  // Variables.
	
	unsigned int FrameNumber;
	int NumberOfSensors; 
  
};

#endif
