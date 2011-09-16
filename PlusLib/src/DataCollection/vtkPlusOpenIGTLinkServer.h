
#ifndef __VTKPLUSOPENIGTLINKSERVER_H
#define __VTKPLUSOPENIGTLINKSERVER_H


#include <string>
#include <vector>

#include "vtkMutexLock.h"
#include "vtkObject.h"

#include "igtlClientSocket.h"
#include "igtlOSUtil.h"
#include "igtlPositionMessage.h"
#include "igtlServerSocket.h"

#include "vtkDataCollector.h"
#include "vtkIGTLMessageQueue.h"



/**
 * This class provides a network interface for Plus functions as an OpenIGTLink server.
 * After the server is started, it waits for a client sending request messages of type
 * "igtl::StringMessage1". These strings are translated into Plus Commands, queued and executed
 * on a first-come-first-serve basis.
 */
class
VTK_EXPORT 
vtkPlusOpenIGTLinkServer
: public vtkObject
{
public:
  
  static vtkPlusOpenIGTLinkServer *New();
  vtkTypeRevisionMacro( vtkPlusOpenIGTLinkServer, vtkObject );
  virtual void PrintSelf( ostream& os, vtkIndent indent );
  
  vtkSetMacro( NetworkPort, int );
  
  vtkGetMacro( Active, bool );
  
  
  void SetDataCollector( vtkDataCollector* dataCollector );
  int Initialize( std::string &strError );
  
  PlusStatus Start();
  PlusStatus Stop();
  static void* vtkCommunicationThread( vtkMultiThreader::ThreadInfo* data );
  
  void ExecuteNextCommand();

  
protected:
  
  vtkPlusOpenIGTLinkServer();
  virtual ~vtkPlusOpenIGTLinkServer();
  
  
private:
	
  vtkPlusOpenIGTLinkServer( const vtkPlusOpenIGTLinkServer& );
  void operator=( const vtkPlusOpenIGTLinkServer& );
  
  void WaitForConnection();
  void ReceiveController();
  void React( std::string input );
  
  igtl::ClientSocket::Pointer ClientSocket;
  igtl::ServerSocket::Pointer ServerSocket;
  
  vtkDataCollector*    DataCollector;
  vtkIGTLMessageQueue* MessageQueue;
  
  vtkMultiThreader*  Threader;
  vtkMutexLock*      Mutex;
  
  int  NetworkPort;
  bool Active;
  int  ThreadId;
  
};


#endif

