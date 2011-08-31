
#ifndef __VTKPLUSOPENIGTLINKCLIENT_H
#define __VTKPLUSOPENIGTLINKCLIENT_H


#include <string>
#include <vector>

#include "vtkObject.h"

#include "igtlOSUtil.h"
#include "igtlClientSocket.h"

#include "vtkDataCollector.h"
#include "vtkPlusCommand.h"



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
  
  int ConnectToServer();
  int StartDataCollector();
  int StopDataCollector();
  
  bool StartCommand( vtkPlusCommand* command );
  
  friend static void* vtkCommunicationThread( vtkMultiThreader::ThreadInfo* data );
  
  
protected:
  
  vtkPlusOpenIGTLinkClient();
  virtual ~vtkPlusOpenIGTLinkClient();
  
  
private:
	
  vtkPlusOpenIGTLinkClient( const vtkPlusOpenIGTLinkClient& );
  void operator=( const vtkPlusOpenIGTLinkClient& );
  
  vtkMultiThreader*  Threader;
  
  igtl::ClientSocket::Pointer ClientSocket;
  
  int         NetworkPort;
  std::string ServerAddress;
  int         ThreadId;
  bool        CommandInProgress;
  
  vtkPlusCommand* ActiveCommand;
  
};


#endif

