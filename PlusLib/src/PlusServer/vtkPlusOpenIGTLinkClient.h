/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/ 

#ifndef __VTKPLUSOPENIGTLINKCLIENT_H
#define __VTKPLUSOPENIGTLINKCLIENT_H

#include "igtlClientSocket.h"
#include "igtlMessageHeader.h"
#include "igtlOSUtil.h"
#include "vtkDataCollector.h"
#include "vtkMultiThreader.h"
#include "vtkObject.h"
#include "vtkPlusCommand.h"
#include <string>
#include <vector>

class vtkMultiThreader; 
class vtkRecursiveCriticalSection;

/*!
  \class vtkPlusOpenIGTLinkClient 
  \brief This class provides a network interface to access Plus functions as an OpenIGTLink client.

  It connects to a Plus server, sends requests and receives responses.  
  
  \ingroup PlusLibPlusServer
*/
class VTK_EXPORT vtkPlusOpenIGTLinkClient : public vtkObject
{
public:
  
  static vtkPlusOpenIGTLinkClient *New();
  vtkTypeRevisionMacro( vtkPlusOpenIGTLinkClient, vtkObject );
  virtual void PrintSelf( ostream& os, vtkIndent indent );
  
  vtkSetMacro( ServerPort, int );
  vtkSetStringMacro( ServerHost );
    
  PlusStatus Connect();
  PlusStatus Disconnect();
  
  PlusStatus SendCommand( vtkPlusCommand* command );
  /*! Wait for a command reply */
  PlusStatus ReceiveReply(std::string &replyStr, double timeoutSec=0);
  
  void Lock();
  void Unlock();

  /*!
    This method can be overridden in child classes to process received messages.
    Note that this method is executed from the data receiver thread and not the
    main thread.
    If the message body is read then this method should return true.
    If the meessage is not read then this method should return false (and the
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
  
private:

  /*! Thread for receiveing control data from clients */ 
  static void* DataReceiverThread( vtkMultiThreader::ThreadInfo* data );

  vtkPlusOpenIGTLinkClient( const vtkPlusOpenIGTLinkClient& );
  void operator=( const vtkPlusOpenIGTLinkClient& );  

  std::pair<bool,bool> DataReceiverActive;

  int  DataReceiverThreadId;

  /*! Multithreader instance for controlling threads */ 
  vtkSmartPointer<vtkMultiThreader> Threader;

  /*! Mutex instance for safe data access */ 
  vtkSmartPointer<vtkRecursiveCriticalSection> Mutex;
  vtkSmartPointer<vtkRecursiveCriticalSection> SocketMutex;
  
  igtl::ClientSocket::Pointer ClientSocket;

  std::deque<std::string> Replies;

  int         ServerPort;
  char*       ServerHost;
  
};

#endif
