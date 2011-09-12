
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



/**
 * This class provides a network interface for Plus functions
 * as an OpenIGTLink server.
 * After the server is started, it waits for clients sending request messages of type
 * "igtl::StringMessage1", and sends a respond to each request.
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
  
  
protected:
  
  vtkPlusOpenIGTLinkServer();
  virtual ~vtkPlusOpenIGTLinkServer();
  
  
private:
	
  vtkPlusOpenIGTLinkServer( const vtkPlusOpenIGTLinkServer& );
  void operator=( const vtkPlusOpenIGTLinkServer& );
  
  int WaitForConnection();
  void ReceiveController();
  void React( std::string input );
  
  igtl::ClientSocket::Pointer ClientSocket;
  igtl::ServerSocket::Pointer ServerSocket;
  
  vtkDataCollector*  DataCollector;
  vtkMultiThreader*  Threader;
  vtkMutexLock*      Mutex;
  
  int  NetworkPort;
  bool Active;
  int  ThreadId;
  
};


#endif

