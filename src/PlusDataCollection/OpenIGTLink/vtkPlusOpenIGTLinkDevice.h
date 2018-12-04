/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkPlusOpenIGTLinkDevice_h
#define __vtkPlusOpenIGTLinkDevice_h

#include "vtkPlusDataCollectionExport.h"
#include "PlusConfigure.h"
#include "vtkPlusDevice.h"

// IGTL includes
#include <igtlClientSocket.h>
#include <igtlMessageBase.h>

class vtkPlusIgtlMessageFactory;

/*!
  \class vtkPlusOpenIGTLinkDevice
  \brief Common base class for OpenIGTLink-based tracking and video devices

  \ingroup PlusLibDataCollection
*/
class vtkPlusDataCollectionExport vtkPlusOpenIGTLinkDevice : public vtkPlusDevice
{
public:
  vtkTypeMacro(vtkPlusOpenIGTLinkDevice, vtkPlusDevice);
  virtual void PrintSelf(ostream& os, vtkIndent indent) VTK_OVERRIDE;

  /*! OpenIGTLink version. */
  virtual std::string GetSdkVersion();

  /*! Connect to device */
  virtual PlusStatus InternalConnect();

  /*! Disconnect from device */
  virtual PlusStatus InternalDisconnect();

  /*! Probe to see if the tracking system is present on the specified address. */
  virtual PlusStatus Probe();

  /*! Read configuration from xml data */
  virtual PlusStatus ReadConfiguration(vtkXMLDataElement* config);

  /*! Write configuration to xml data */
  virtual PlusStatus WriteConfiguration(vtkXMLDataElement* config);

  /*! Send a message to the connected server */
  bool SendMessage(igtl::MessageBase::Pointer packedMessage);

  /*! Set OpenIGTLink message type */
  vtkSetStdStringMacro(MessageType);
  /*! Get OpenIGTLink message type */
  vtkGetStdStringMacro(MessageType);

  /*! Set image streams to be sent when message type is a type that sends an image */
  vtkSetMacro(ImageMessageEmbeddedTransformName, igsioTransformName);
  vtkSetMacro(ImageMessageEmbeddedTransformName, std::string);

  /*! Get image streams to be sent when message type is a type that sends an image */
  vtkGetMacro(ImageMessageEmbeddedTransformName, igsioTransformName);

  /*! Set OpenIGTLink server address */
  vtkSetStdStringMacro(ServerAddress);
  /*! Get OpenIGTLink server address */
  vtkGetStdStringMacro(ServerAddress);

  /*! Set OpenIGTLink server port */
  vtkSetMacro(ServerPort, int);
  /*! Get OpenIGTLink server port */
  vtkGetMacro(ServerPort, int);

  /*! Set OpenIGTLink socket receive timeout */
  vtkSetMacro(ReceiveTimeoutSec, double);
  /*! Get OpenIGTLink socket receive timeout */
  vtkGetMacro(ReceiveTimeoutSec, double);

  /*! Set OpenIGTLink socket send timeout */
  vtkSetMacro(SendTimeoutSec, double);
  /*! Get OpenIGTLink socket send timeout */
  vtkGetMacro(SendTimeoutSec, double);

  /*! Set IGTL CRC check flag (0: disabled, 1: enabled) */
  vtkSetMacro(IgtlMessageCrcCheckEnabled, int);
  /*! Get IGTL CRC check flag (0: disabled, 1: enabled) */
  vtkGetMacro(IgtlMessageCrcCheckEnabled, int);

  /*! Get the ReconnectOnNoData flag */
  vtkGetMacro(ReconnectOnReceiveTimeout, bool);

protected:
  vtkPlusOpenIGTLinkDevice();
  virtual ~vtkPlusOpenIGTLinkDevice();

  /*! Reconnect the client socket. Used when the connection is established or there is a socket error. */
  virtual PlusStatus ClientSocketReconnect();

  /*! Sends the requested message types when connection is established */
  virtual PlusStatus SendRequestedMessageTypes();

  /*!
    This method is called when receiving a message is timed out.
    If ReconnectOnReceiveTimeout is enabled then this method attempts to reconnect to the server.
  */
  void OnReceiveTimeout();

  /*!
    Calls ReceiveMessageHeader and logs the error and/or reconnect as needed.
  */
  void ReceiveMessageHeaderWithErrorHandling(igtl::MessageHeader::Pointer& headerMsg);

  /*!
    Receive an OpenITGLink message header.
    Returns PLUS_FAIL if there was a socket error.
    The headerMsg is NULL is no data is received.
  */
  virtual PlusStatus ReceiveMessageHeader(igtl::MessageHeader::Pointer& headerMsg);

  /*! Set the ReconnectOnReceiveTimeout flag */
  vtkSetMacro(ReconnectOnReceiveTimeout, bool);

  vtkSetMacro(UseReceivedTimestamps, bool);

  /*! OpenIGTLink message type */
  std::string MessageType;

  /*! Image stream to send when message type wants to send an image */
  igsioTransformName ImageMessageEmbeddedTransformName;

  /*! OpenIGTLink server address */
  std::string ServerAddress;

  /*! OpenIGTLink server port */
  int ServerPort;

  /*! How long a socket waits when receiving/sending before timing out */
  double ReceiveTimeoutSec;
  double SendTimeoutSec;

  /*! Flag for IGTL CRC check (0: disabled, 1: enabled) */
  int IgtlMessageCrcCheckEnabled;

  /*! Number of retry attempts for message sending to and receiving from the server */
  int NumberOfRetryAttempts;

  /*! Delay between retry attempts */
  double DelayBetweenRetryAttemptsSec;

  /* IGTL message factory*/
  vtkSmartPointer<vtkPlusIgtlMessageFactory> MessageFactory;

  /*! Control access to the socket */
  vtkSmartPointer<vtkIGSIORecursiveCriticalSection> SocketMutex;

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
  vtkPlusOpenIGTLinkDevice(const vtkPlusOpenIGTLinkDevice&);   // Not implemented.
  void operator=(const vtkPlusOpenIGTLinkDevice&);   // Not implemented.
};

#endif
