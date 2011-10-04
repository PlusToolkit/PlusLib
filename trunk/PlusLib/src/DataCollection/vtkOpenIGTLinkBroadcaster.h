
#ifndef OPENIGTLINKBROADCASTER_H
#define OPENIGTLINKBROADCASTER_H


#include <string>
#include <vector>

#include "vtkObject.h"

#include "igtlOSUtil.h"
#include "igtlPositionMessage.h"
#include "igtlClientSocket.h"

#include "vtkDataCollector.h"
#include "vtkTrackedFrameList.h"



/**
 * Stores information necessary for broadcasting non reference tools.
 */
struct IgtToolInfo
{
  std::string                  ToolName;
  int                          TrackerPortNumber;
  igtl::ClientSocket::Pointer  Socket;
};


/**
 * Sockets have to be stored separately. Exactly one socket has to be created
 * for a host:port combination. Even if more tools are broadcasted to this address.
 */
struct SocketInfo
{
  std::string                 Host;
  int                         Port;
  igtl::ClientSocket::Pointer Socket;
};



/**
 * Broadcasts tracking data and ultrasound images through OpenIGTLink network protocol.
 * Input is read from the configuration xml file for DataCollector.
 * To broadcast a tracker tool, specify a SendTo attribute in the tool tag.
 * E.g. <Tool Name="Probe" PortNumber="0" SendTo="localhost:18944"></Tool>
 */
class
VTK_EXPORT 
vtkOpenIGTLinkBroadcaster
: public vtkObject
{
public:
  
  enum Status {
    STATUS_OK,
    STATUS_NOT_INITIALIZED,
    STATUS_NOT_TRACKING,
    STATUS_HOST_NOT_FOUND,
    STATUS_SEND_ERROR,
    STATUS_MISSING_DEFAULT_TOOL, 
	STATUS_MISSING_TRACKED_FRAME
  };
  
  static vtkOpenIGTLinkBroadcaster *New();
	vtkTypeRevisionMacro( vtkOpenIGTLinkBroadcaster, vtkObject );
	virtual void PrintSelf( ostream& os, vtkIndent indent );
  
  
  Status SetDataCollector( vtkDataCollector* dataCollector );
  Status Initialize( std::string &strError );
  
  Status SendMessages( std::string strError );
  Status SendMessages();
  
  
protected:
  
  vtkOpenIGTLinkBroadcaster();
	virtual ~vtkOpenIGTLinkBroadcaster();
  
  
private:
	
  vtkOpenIGTLinkBroadcaster( const vtkOpenIGTLinkBroadcaster& );
	void operator=( const vtkOpenIGTLinkBroadcaster& );
  
  void SendImageMessage( TrackedFrame* trackedFrame, std::string strError );
  
  
  Status             InternalStatus;
  vtkDataCollector*  DataCollector;
  
  
  std::vector< IgtToolInfo > NonReferenceToolInfos;  // List of tools.
  std::vector< SocketInfo >  SocketInfos;            // List of sockets.
  
};


#endif

