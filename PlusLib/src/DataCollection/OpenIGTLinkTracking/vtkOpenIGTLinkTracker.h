/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkOpenIGTLinkTracker_h
#define __vtkOpenIGTLinkTracker_h

#include "PlusConfigure.h"
#include "vtkPlusDevice.h"
#include "igtlClientSocket.h"
#include "igtlMessageBase.h"

/*!
\class vtkOpenIGTLinkTracker 
\brief OpenIGTLink tracker client  

\ingroup PlusLibTracking
*/
class VTK_EXPORT vtkOpenIGTLinkTracker : public vtkPlusDevice
{
public:

  static vtkOpenIGTLinkTracker *New();
  vtkTypeMacro( vtkOpenIGTLinkTracker,vtkPlusDevice );
  void PrintSelf( ostream& os, vtkIndent indent );

  /*! OpenIGTLink version. */
  virtual std::string GetSdkVersion();

  /*! Connect to device */
  PlusStatus InternalConnect();

  /*! Disconnect from device */
  virtual PlusStatus InternalDisconnect();

  /*! Probe to see if the tracking system is present on the specified address. */
  PlusStatus Probe();

  /*! Get an update from the tracking system and push the new transforms to the tools. This function is called by the tracker thread.*/
  PlusStatus InternalUpdate();

  /*! Read configuration from xml data */
  virtual PlusStatus ReadConfiguration( vtkXMLDataElement* config ); 
  
  /*! Write configuration to xml data */
  virtual PlusStatus WriteConfiguration(vtkXMLDataElement* config);  

  virtual bool IsTracker() const { return true; }

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

  /*! Set IGTL CRC check flag (0: disabled, 1: enabled) */ 
  vtkSetMacro(IgtlMessageCrcCheckEnabled, int); 
  /*! Get IGTL CRC check flag (0: disabled, 1: enabled) */ 
  vtkGetMacro(IgtlMessageCrcCheckEnabled, int); 

  /*! Get the ReconnectOnNoData flag */
  vtkGetMacro(ReconnectOnReceiveTimeout, bool);

  /*! 
    Set the internal tracker coordinate system name that is send to the tracker
    when tracking start is requested using an STT_TDATA message.
  */ 
  vtkSetStringMacro(TrackerInternalCoordinateSystemName); 
  /*! Get the internal tracker coordinate system name */ 
  vtkGetStringMacro(TrackerInternalCoordinateSystemName); 

protected:
  vtkOpenIGTLinkTracker();
  ~vtkOpenIGTLinkTracker();

  /*! 
    Start the tracking system.  The tracking system is brought from its ground state into full tracking mode.
    The device will only be reset if communication cannot be established without a reset.
  */
  PlusStatus InternalStartRecording();

  /*! Stop the tracking system and bring it back to its ground state: Initialized, not tracking */
  PlusStatus InternalStopRecording();

  /*! Process a TDATA message (add all the received transforms to the buffers) */
  PlusStatus ProcessTDataMessage(igtl::MessageHeader::Pointer headerMsg);

  /*!
    Store the latest transforms again in the buffers with the provided timestamp.
    If no transforms are defined then identity transform will be stored.
    If there is a transform defined already with the same timestamp then it will not be overwritten.
  */
  PlusStatus StoreMostRecentTransformValues(double unfilteredTimestamp);

  /*! Set the ReconnectOnNoData flag */
  vtkSetMacro(ReconnectOnReceiveTimeout, bool);

  /*! OpenIGTLink message type */
  char* MessageType; 

  /*! OpenIGTLink server address */ 
  char* ServerAddress; 

  /*! OpenIGTLink server port */ 
  int ServerPort; 

  /*! Number of retry attempts for message sending to and receiving from the server */ 
  int NumberOfRetryAttempts; 

  /*! Delay between retry attempts */ 
  int DelayBetweenRetryAttemptsSec; 

  /*! Flag for IGTL CRC check (0: disabled, 1: enabled) */ 
  int IgtlMessageCrcCheckEnabled; 

  /*! OpenIGTLink client socket */ 
  igtl::ClientSocket::Pointer ClientSocket;

  /*! Attempt a reconnection if no data is received */
  bool ReconnectOnReceiveTimeout;

  /*!
    Internal tracker coordinate system name that is send to the tracker when tracking start is requested
    using an STT_TDATA message.
  */
  char* TrackerInternalCoordinateSystemName;

  /*! Use the last known transform value if not received a new value. Useful for servers that only notify about changes in the transforms. */
  bool UseLastTransformsOnReceiveTimeout;

private:  
  
  vtkOpenIGTLinkTracker( const vtkOpenIGTLinkTracker& );
  void operator=( const vtkOpenIGTLinkTracker& );    
};

#endif
