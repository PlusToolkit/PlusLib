/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkOpenIGTLinkTracker_h
#define __vtkOpenIGTLinkTracker_h

#include "PlusConfigure.h"
#include "vtkTracker.h"
#include "igtlClientSocket.h"

/*!
\class vtkOpenIGTLinkTracker 
\brief OpenIGTLink tracker client  

\ingroup PlusLibTracking
*/
class VTK_EXPORT vtkOpenIGTLinkTracker : public vtkTracker
{
public:

	static vtkOpenIGTLinkTracker *New();
	vtkTypeMacro( vtkOpenIGTLinkTracker,vtkTracker );
	void PrintSelf( ostream& os, vtkIndent indent );

  /*! OpenIGTLink version. */
  virtual std::string GetSdkVersion();

	/*! Connect to device */
	PlusStatus Connect();

	/*! Disconnect from device */
	virtual PlusStatus Disconnect();

	/*! Probe to see if the tracking system is present on the specified address. */
	PlusStatus Probe();

	/*! Get an update from the tracking system and push the new transforms to the tools. This function is called by the tracker thread.*/
	PlusStatus InternalUpdate();

  /*! Read configuration from xml data */
	PlusStatus ReadConfiguration( vtkXMLDataElement* config ); 
	
  /*! Set OpenIGTLink message type */ 
  vtkSetStringMacro(MessageType); 
  /*! Get OpenIGTLink message type */ 
  vtkGetStringMacro(MessageType); 

  /*! Set OpenIGTLink server address */ 
  vtkSetStringMacro(ServerAddress); 
  /*! Get OpenIGTLink server address */ 
  vtkGetStringMacro(ServerAddress); 

  /*! Set OpenIGTLink server port */ 
  vtkSetMacro(ServerPort, int); 
  /*! Get OpenIGTLink server port */ 
  vtkGetMacro(ServerPort, int); 
   
protected:
  
	vtkOpenIGTLinkTracker();
	~vtkOpenIGTLinkTracker();

  /*! 
    Start the tracking system.  The tracking system is brought from its ground state into full tracking mode.
    The device will only be reset if communication cannot be established without a reset.
  */
	PlusStatus InternalStartTracking();

	/*! Stop the tracking system and bring it back to its ground state: Initialized, not tracking, at 9600 Baud. */
	PlusStatus InternalStopTracking();

  /*! OpenIGTLink message type */
  char* MessageType; 

  /*! OpenIGTLink server address */ 
  char* ServerAddress; 

  /*! OpenIGTLink server port */ 
  int ServerPort; 

  /*! Number of retry attempts for message sending to and receiving from the server */ 
  int NumberOfRetryAttempts; 

  /*! OpenIGTLink client socket */ 
  igtl::ClientSocket::Pointer ClientSocket;

private:  
  
  vtkOpenIGTLinkTracker( const vtkOpenIGTLinkTracker& );
	void operator=( const vtkOpenIGTLinkTracker& );    
};

#endif
