/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/ 

#ifndef OPENIGTLINKBROADCASTER_H
#define OPENIGTLINKBROADCASTER_H

#include <string>
#include <vector>

#include "vtkObject.h"

#include "igtlOSUtil.h"
#include "igtlPositionMessage.h"
#include "igtlClientSocket.h"

#include "vtkDataCollector.h"

class vtkTransformRepository;


/*!
  \class IgtToolInfo 
  \brief Stores information necessary for broadcasting non reference tools.
  \ingroup PlusLibDataCollection
*/
struct IgtToolInfo
{
  std::string                  TransformName;
  igtl::ClientSocket::Pointer  Socket;
};


/*!
  \class SocketInfo 
  \brief Stores information about a socket connection.
  
  Sockets have to be stored separately. Exactly one socket has to be created
  for a host:port combination. Even if more tools are broadcasted to this address. 

  \ingroup PlusLibDataCollection 
*/
struct SocketInfo
{
  std::string                 Host;
  int                         Port;
  igtl::ClientSocket::Pointer Socket;
};


/*!
  \class vtkOpenIGTLinkBroadcaster 
  \brief Broadcasts tracking data and ultrasound images through OpenIGTLink network protocol.

  Input is read from the device set configuration xml file.
  To broadcast a transform, add a new element under the DataCollection/OpenIGTLink element
  E.g. <Transform Name="ProbeToReference" SendTo="127.0.0.1:1111" />

  \ingroup PlusLibDataCollection
*/
class VTK_EXPORT vtkOpenIGTLinkBroadcaster : public vtkObject
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

  void SetApplyStylusCalibration( bool apply );

  Status SetDataCollector( vtkDataCollector* dataCollector );
  Status Initialize( std::string &strError ); // TODO It would be better to return with the regular PlusStatus. The error message is already logged in this function so there's no need to repeat that in the application, it's enough to know that it failed.
  PlusStatus ReadConfiguration(vtkXMLDataElement* aConfig);

  /*!
    \return Internal status. If it is HOST_NOT_FOUND, strError will be filled with the host:port address not found.
  */
  Status SendMessages( std::string strError );
  Status SendMessages();


protected:

  vtkOpenIGTLinkBroadcaster();
	virtual ~vtkOpenIGTLinkBroadcaster();


private:

  vtkOpenIGTLinkBroadcaster( const vtkOpenIGTLinkBroadcaster& );
	void operator=( const vtkOpenIGTLinkBroadcaster& );

  void SendImageMessage( TrackedFrame* trackedFrame, std::string strError );
  
  PlusStatus GetSocketInfoFromSendToLink( const char* sendToLink, SocketInfo& socketInfo ); 
  
  Status             InternalStatus;
  vtkDataCollector*  DataCollector;
  bool               ApplyStylusCalibration;
  
  /*! List of tools */
  std::vector< IgtToolInfo >  ToolInfos; 

  /*! List of sockets */
  std::vector< SocketInfo >   SocketInfos;            

  /*! Socket for image */
  igtl::ClientSocket::Pointer ImageSocket;

  /*! Transform repository */
  vtkTransformRepository*     TransformRepository;
};


#endif

