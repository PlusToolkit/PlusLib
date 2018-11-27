/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __VTKPLUSOPENIGTLINKCLIENT_H
#define __VTKPLUSOPENIGTLINKCLIENT_H

#include "vtkPlusServerExport.h"

// Local includes
#include "vtkPlusCommand.h"
#include "vtkPlusIgtlMessageFactory.h"

// OpenIGTLink includes
#include <igtlClientSocket.h>
#include <igtlCommandMessage.h>
#include <igtlMessageHeader.h>
#include <igtlOSUtil.h>

// VTK includes
#include <vtkObject.h>

// STL includes
#include <deque>
#include <string>

class vtkMultiThreader;
class vtkIGSIORecursiveCriticalSection;

/*!
  \class vtkPlusOpenIGTLinkClient
  \brief This class provides a network interface to access Plus functions as an OpenIGTLink client.

  It connects to a Plus server, sends requests and receives responses.

  \ingroup PlusLibPlusServer
*/
class vtkPlusServerExport vtkPlusOpenIGTLinkClient : public vtkObject
{
public:
  static vtkPlusOpenIGTLinkClient* New();
  vtkTypeMacro(vtkPlusOpenIGTLinkClient, vtkObject);
  virtual void PrintSelf(ostream& os, vtkIndent indent);

  vtkSetMacro(ServerPort, int);
  vtkGetMacroConst(ServerPort, int);

  vtkGetStdStringMacro(ServerHost);
  vtkSetStdStringMacro(ServerHost);

  vtkSetMacro(ServerIGTLVersion, int);
  vtkGetMacroConst(ServerIGTLVersion, int);

  /*! If timeoutSec<0 then connection will be attempted multiple times until successfully connected or the timeout elapse */
  PlusStatus Connect(double timeoutSec = -1);

  /*! Disconnect from the connected server */
  PlusStatus Disconnect();

  /*! Send a command to the connected server */
  PlusStatus SendCommand(vtkPlusCommand* command);

  /*! Send a packed message to the connected server */
  PlusStatus SendMessage(igtl::MessageBase::Pointer packedMessage);

  /*! Wait for a command reply */
  PlusStatus ReceiveReply(PlusStatus& result,
                          int32_t& outOriginalCommandId,
                          std::string& outErrorString,
                          std::string& outContent,
                          igtl::MessageBase::MetaDataMap& outParameters,
                          std::string& outCommandName,
                          double timeoutSec = 0);

  void Lock();
  void Unlock();

  /*!
    This method can be overridden in child classes to process received messages.
    Note that this method is executed from the data receiver thread and not the
    main thread.
    If the message body is read then this method should return true.
    If the message is not read then this method should return false (and the
    message body will be skipped).
  */
  virtual bool OnMessageReceived(igtl::MessageHeader::Pointer messageHeader)
  {
    return false;
  }

protected:
  vtkPlusOpenIGTLinkClient();
  virtual ~vtkPlusOpenIGTLinkClient();

  /*! Thread-safe method that allows child classes to read data from the socket */
  int SocketReceive(void* data, int length);

  /*! Thread for receiving control data from clients */
  static void* DataReceiverThread(vtkMultiThreader::ThreadInfo* data);

protected:
  /*! igtl Factory for message sending */
  vtkSmartPointer<vtkPlusIgtlMessageFactory>        IgtlMessageFactory;

  std::pair<bool, bool>                             DataReceiverActive;

  int                                               DataReceiverThreadId;

  /*! vtkMultiThreader instance for controlling threads */
  vtkSmartPointer<vtkMultiThreader>                 Threader;

  /*! Mutex instance for safe data access */
  vtkSmartPointer<vtkIGSIORecursiveCriticalSection>  Mutex;
  vtkSmartPointer<vtkIGSIORecursiveCriticalSection>  SocketMutex;

  igtl::ClientSocket::Pointer                       ClientSocket;

  igtlUint32                                        LastGeneratedCommandId;

  std::deque<igtl::MessageBase::Pointer>            Replies;

  int                                               ServerPort;
  std::string                                       ServerHost;

  // IGTL protocol version of the server
  int                                               ServerIGTLVersion;

  static const float                                CLIENT_SOCKET_TIMEOUT_SEC;

private:
  vtkPlusOpenIGTLinkClient(const vtkPlusOpenIGTLinkClient&);
  void operator=(const vtkPlusOpenIGTLinkClient&);
};

#endif
