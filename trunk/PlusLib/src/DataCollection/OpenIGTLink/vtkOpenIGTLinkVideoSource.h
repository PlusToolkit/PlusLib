/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkOpenIGTLinkVideoSource_h
#define __vtkOpenIGTLinkVideoSource_h

#include "PlusConfigure.h"
#include "vtkPlusDevice.h"
#include "igtlClientSocket.h"
#include "igtlMessageBase.h"

/*!
  \class vtkOpenIGTLinkVideoSource 
  \brief VTK interface for video input from OpenIGTLink image message

  vtkOpenIGTLinkVideoSource is a class for providing video input interfaces between VTK and OpenIGTLink ready video device. 

  \ingroup PlusLibDataCollection
*/ 
class VTK_EXPORT vtkOpenIGTLinkVideoSource : public vtkPlusDevice
{
public:

  static vtkOpenIGTLinkVideoSource *New();
  vtkTypeMacro(vtkOpenIGTLinkVideoSource,vtkPlusDevice);
  void PrintSelf(ostream& os, vtkIndent indent);   

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

  virtual bool IsTracker() const { return false; }

  PlusStatus SetImageMessageEmbeddedTransformName(const char* nameString);

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

  /*! Verify the device is correctly configured */
  virtual PlusStatus NotifyConfigured();

protected:
  vtkOpenIGTLinkVideoSource();
  virtual ~vtkOpenIGTLinkVideoSource();

  /*! Reconnect the client socket. Used when the connection is established or there is a socket error. */
  PlusStatus ClientSocketReconnect();

  /*!
    Receive an OpenITGLink message header.
    Returns PLUS_FAIL if there was a socket error.
    The headerMsg is NULL is no data is received.
  */
  PlusStatus ReceiveMessageHeader(igtl::MessageHeader::Pointer &headerMsg);

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
    
  /*! Name of the transform that is supplied with the IMAGE OpenIGTLink message */ 
  PlusTransformName ImageMessageEmbeddedTransformName;
private:

  vtkOpenIGTLinkVideoSource(const vtkOpenIGTLinkVideoSource&);  // Not implemented.
  void operator=(const vtkOpenIGTLinkVideoSource&);  // Not implemented.
};

#endif
