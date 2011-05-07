
#ifndef OPENIGTLINKBROADCASTER_H
#define OPENIGTLINKBROADCASTER_H


#include <vector>

#include "vtkObject.h"

#include "igtlOSUtil.h"
#include "igtlPositionMessage.h"
#include "igtlClientSocket.h"

#include "vtkDataCollector.h"



class
VTK_EXPORT 
vtkOpenIGTLinkBroadcaster
: public vtkObject
{
public:
  
  static vtkOpenIGTLinkBroadcaster *New();
	vtkTypeRevisionMacro( vtkOpenIGTLinkBroadcaster, vtkObject );
	virtual void PrintSelf( ostream& os, vtkIndent indent );
  
  
  vtkSetObjectMacro( DataCollector, vtkDataCollector );
  
  void AddSocket( std::string host, unsigned int port );
  void SendMessages();
  
  
protected:
  
  vtkOpenIGTLinkBroadcaster();
	virtual ~vtkOpenIGTLinkBroadcaster();
  
  
private:
	
  vtkOpenIGTLinkBroadcaster( const vtkOpenIGTLinkBroadcaster& );
	void operator=( const vtkOpenIGTLinkBroadcaster& );
  
  std::vector< igtl::ClientSocket::Pointer > Sockets;
  vtkDataCollector* DataCollector;
};


#endif

