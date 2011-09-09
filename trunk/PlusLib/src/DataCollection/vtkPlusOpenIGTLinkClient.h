
#ifndef __VTKPLUSOPENIGTLINKCLIENT_H
#define __VTKPLUSOPENIGTLINKCLIENT_H


#include <string>
#include <vector>

#include "vtkObject.h"

#include "igtlOSUtil.h"
#include "igtlClientSocket.h"

#include "vtkDataCollector.h"
#include "vtkPlusCommand.h"


class vtkMutexLock;



/**
 * This class provides a network interface to access Plus functions
 * as an OpenIGTLink client.
 * It connects to a Plus server, sends requests and receives responses.
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
  
  vtkSetMacro( NetworkPort, int );
  vtkSetMacro( ServerAddress, std::string );
  
  vtkSetObjectMacro( ActiveCommand, vtkPlusCommand );
  
  int ConnectToServer();
  int StartDataCollector();
  int StopDataCollector();
  
  bool SendCommand( vtkPlusCommand* command );
  
  void Lock();
  void Unlock();
  
  
protected:
  
  vtkPlusOpenIGTLinkClient();
  virtual ~vtkPlusOpenIGTLinkClient();
  
  
private:
	
  vtkPlusOpenIGTLinkClient( const vtkPlusOpenIGTLinkClient& );
  void operator=( const vtkPlusOpenIGTLinkClient& );
  
  vtkMultiThreader*  Threader;
  vtkMutexLock*      Mutex;
  
  igtl::ClientSocket::Pointer ClientSocket;
  vtkMutexLock*               SocketMutex;
  
  int         NetworkPort;
  std::string ServerAddress;
  int         ThreadId;
  bool        CommandInProgress;
  
  vtkPlusCommand* ActiveCommand;
  
};


#endif

