/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkOpenIGTLinkDevice_h
#define __vtkOpenIGTLinkDevice_h

#include "PlusConfigure.h"
#include "vtkDataCollectionExport.h"

#include "vtkPlusDevice.h"
#include "igtlClientSocket.h"
#include "igtlMessageBase.h"

/*!
  \class vtkOpenIGTLinkDevice 
  \brief Common base class for OpenIGTLink-based tracking and video devices

  \ingroup PlusLibDataCollection
*/ 
class vtkDataCollectionExport vtkOpenIGTLinkDevice : public vtkPlusDevice
{
public:

  //static vtkOpenIGTLinkDevice *New();
  vtkTypeMacro(vtkOpenIGTLinkDevice,vtkPlusDevice);
  virtual void PrintSelf(ostream& os, vtkIndent indent);   

  /*! OpenIGTLink version. */
  virtual std::string GetSdkVersion(); 

  /*! Connect to device */
  virtual PlusStatus InternalConnect();

  /*! Disconnect from device */
  virtual PlusStatus InternalDisconnect();

  /*! Probe to see if the tracking system is present on the specified address. */
  virtual PlusStatus Probe();

  /*! Read configuration from xml data */
  virtual PlusStatus ReadConfiguration( vtkXMLDataElement* config ); 
  
  /*! Write configuration to xml data */
  virtual PlusStatus WriteConfiguration(vtkXMLDataElement* config);

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

protected:
  vtkOpenIGTLinkDevice();
  virtual ~vtkOpenIGTLinkDevice();

  /*! Reconnect the client socket. Used when the connection is established or there is a socket error. */
  virtual PlusStatus ClientSocketReconnect();

  /*! Sends the requested mesage types when connection is established */
  virtual PlusStatus SendRequestedMessageTypes();
  
  /*!
    This method is called when receiving a message is timed out.
    If ReconnectOnReceiveTimeout is enabled then this method attempts to reconnect to the server.
  */
  void OnReceiveTimeout();
  
  /*!
    Calls ReceiveMessageHeader and logs the error and/or reconnect as needed.
  */
  void ReceiveMessageHeaderWithErrorHandling(igtl::MessageHeader::Pointer &headerMsg);
  
  /*!
    Receive an OpenITGLink message header.
    Returns PLUS_FAIL if there was a socket error.
    The headerMsg is NULL is no data is received.
  */
  virtual PlusStatus ReceiveMessageHeader(igtl::MessageHeader::Pointer &headerMsg);

  /*! Set the ReconnectOnNoData flag */
  vtkSetMacro(ReconnectOnReceiveTimeout, bool);

  vtkSetMacro(UseReceivedTimestamps, bool);

  /*! OpenIGTLink message type */
  char* MessageType; 

  /*! OpenIGTLink server address */ 
  char* ServerAddress; 

  /*! OpenIGTLink server port */ 
  int ServerPort; 

  /*! Flag for IGTL CRC check (0: disabled, 1: enabled) */ 
  int IgtlMessageCrcCheckEnabled; 

  /*! Number of retry attempts for message sending to and receiving from the server */ 
  int NumberOfRetryAttempts; 

  /*! Delay between retry attempts */ 
  double DelayBetweenRetryAttemptsSec; 

  /*! OpenIGTLink client socket */ 
  igtl::ClientSocket::Pointer ClientSocket;

  /*! Attempt a reconnection if no data is received */
  bool ReconnectOnReceiveTimeout;

  /*!
    Use the timestamp embedded in the OpenIGTLink message (the timestamp is converted form the UTC time to system time).
    If it is false then the time of reception is used as timestamp.
  */
  bool UseReceivedTimestamps;
    
private:

  vtkOpenIGTLinkDevice(const vtkOpenIGTLinkDevice&);  // Not implemented.
  void operator=(const vtkOpenIGTLinkDevice&);  // Not implemented.
};

#endif
