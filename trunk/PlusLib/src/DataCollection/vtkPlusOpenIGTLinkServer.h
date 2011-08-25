
#ifndef __VTKPLUSOPENIGTLINKSERVER_H
#define __VTKPLUSOPENIGTLINKSERVER_H


#include <string>
#include <vector>

#include "vtkObject.h"

#include "igtlOSUtil.h"
#include "igtlPositionMessage.h"
#include "igtlClientSocket.h"

#include "vtkDataCollector.h"



/**
 * This class provides a network interface for Plus functions
 * as an OpenIGTLink server.
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
  
  
  void SetDataCollector( vtkDataCollector* dataCollector );
  int Initialize( std::string &strError );
  
  PlusStatus Start();
  PlusStatus Stop();
  
  
protected:
  
  vtkPlusOpenIGTLinkServer();
  virtual ~vtkPlusOpenIGTLinkServer();
  
  
private:
	
  vtkPlusOpenIGTLinkServer( const vtkPlusOpenIGTLinkServer& );
  void operator=( const vtkPlusOpenIGTLinkServer& );
  
  
  vtkDataCollector*  DataCollector;
  
  
};


#endif

