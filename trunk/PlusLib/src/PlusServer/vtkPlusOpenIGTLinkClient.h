/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/ 

#ifndef __VTKPLUSOPENIGTLINKCLIENT_H
#define __VTKPLUSOPENIGTLINKCLIENT_H

#include <string>
#include <vector>

#include "vtkObject.h"

#include "igtlOSUtil.h"
#include "igtlClientSocket.h"

#include "vtkDataCollector.h"
#include "vtkPlusCommand.h"

class vtkMultiThreader; 
class vtkRecursiveCriticalSection;

/*!
  \class vtkPlusOpenIGTLinkClient 
  \brief This class provides a network interface to access Plus functions as an OpenIGTLink client.

  It connects to a Plus server, sends requests and receives responses.  
  
  \ingroup PlusLibDataCollection
*/
class
VTK_EXPORT 
vtkPlusOpenIGTLinkClient
: public vtkObject
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
  
  
protected:
  
  vtkPlusOpenIGTLinkClient();
  virtual ~vtkPlusOpenIGTLinkClient();

  /*! Thread for receiveing control data from clients */ 
  static void* DataReceiverThread( vtkMultiThreader::ThreadInfo* data );

  
private:
	
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
