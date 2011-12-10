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
  
  static vtkOpenIGTLinkBroadcaster *New();
	vtkTypeRevisionMacro( vtkOpenIGTLinkBroadcaster, vtkObject );
	virtual void PrintSelf( ostream& os, vtkIndent indent );

  void SetApplyStylusCalibration( bool apply );

  void SetDataCollector( vtkDataCollector* dataCollector );

  PlusStatus Initialize();

  PlusStatus ReadConfiguration(vtkXMLDataElement* aConfig);

  PlusStatus SendMessages();

protected:

  vtkOpenIGTLinkBroadcaster();
	virtual ~vtkOpenIGTLinkBroadcaster();

  PlusStatus GetSocketInfoFromSendToLink( const char* sendToLink, SocketInfo& socketInfo ); 

  PlusStatus SendImageMessage( TrackedFrame* trackedFrame );

  vtkOpenIGTLinkBroadcaster( const vtkOpenIGTLinkBroadcaster& );

  void operator=( const vtkOpenIGTLinkBroadcaster& );

private:
  
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

