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
  \class SocketInfo 
  \brief Stores information about a socket connection.
  
  Sockets have to be stored separately. Exactly one socket has to be created
  for a host:port combination. Even if more tools are broadcasted to this address. 

  \ingroup PlusLibDataCollection 
*/
struct SocketInfo
{
  SocketInfo()
  {
    this->Host.clear(); 
    this->Port = 0; 
    this->Socket = NULL; 
  }
  std::string                 Host;
  int                         Port;
  igtl::ClientSocket::Pointer Socket;
};

/*!
  \class IgtToolInfo 
  \brief Stores information necessary for broadcasting tools.
  \ingroup PlusLibDataCollection
*/
struct IgtToolInfo
{
  IgtToolInfo()
  {
    this->Name.clear(); 
    this->Valid = false; 
    this->Paused = false; 
  }

  std::string                  Name;
  SocketInfo                   IgtlSocketInfo;
  bool                         Valid; 
  bool                         Paused; 
};

/*!
  \class vtkOpenIGTLinkBroadcaster 
  \brief Broadcasts tracking data and ultrasound images through OpenIGTLink network protocol.

  Input is read from the device set configuration xml file.
  To broadcast a transform, add a new element under the DataCollection/OpenIGTLink element
  Example:
  \code
  <Transform Name="ProbeToReference" SendTo="127.0.0.1:1111" />
  \endcode

  \ingroup PlusLibDataCollection
*/
class VTK_EXPORT vtkOpenIGTLinkBroadcaster : public vtkObject
{
public:
  
  static vtkOpenIGTLinkBroadcaster *New();
	vtkTypeRevisionMacro( vtkOpenIGTLinkBroadcaster, vtkObject );
	virtual void PrintSelf( ostream& os, vtkIndent indent );

  /*! Disconnect sockets from server */
  void DisconnectSockets(); 

  /*! Set data collector instance */
  void SetDataCollector( vtkDataCollector* dataCollector );

  /*! Get transforms tool info */
  std::vector<IgtToolInfo> GetToolInfos() { return this->ToolInfos; } 

  /*! Get image info */
  IgtToolInfo GetImageInfo() { return this->ImageInfo; }

  /*! Add new transform for broadcasting */ 
  PlusStatus AddTransformForBroadcasting( const char* aTransformName, const char* aSendToLink ); 

  /*! Remove broadcasted transform tool info from container */ 
  PlusStatus RemoveBroadcastedToolInfo(const IgtToolInfo& toolInfo); 

  /*! Invert broadcasted tool pause status */ 
  PlusStatus ChangeBroadcastedToolPauseStatus(const IgtToolInfo& toolInfo); 

  /*! Broadcast image */
  PlusStatus AddImageForBroadcasting( const char* aName, const char* aSendToLink ); 

  /* Stop image bradcasting */
  PlusStatus RemoveBroadcastedImageInfo(); 

  /*! Invert broadcasted image pause status */ 
  PlusStatus ChangeImageToolPauseStatus(); 

  /*! Initialize broadcaster */
  PlusStatus Initialize();

  /*! Read configuration */
  PlusStatus ReadConfiguration(vtkXMLDataElement* aConfig);

  /*! Send messages */
  PlusStatus SendMessages();

protected:

  vtkOpenIGTLinkBroadcaster();
	virtual ~vtkOpenIGTLinkBroadcaster();

  /*! Get socket information, create new socket and connect if not yet connected */ 
  PlusStatus GetSocketInfoFromSendToLink( const char* sendToLink, SocketInfo& socketInfo ); 

  /*! Send image message */
  PlusStatus SendImageMessage( TrackedFrame* trackedFrame );

  vtkOpenIGTLinkBroadcaster( const vtkOpenIGTLinkBroadcaster& );

  void operator=( const vtkOpenIGTLinkBroadcaster& );

private:
  
  /*! Data collector instance */
  vtkDataCollector*  DataCollector;

  /*! List of tools */
  std::vector< IgtToolInfo >  ToolInfos; 

  /*! List of sockets */
  std::vector< SocketInfo >   SocketInfos;            

  /*! Image info */
  IgtToolInfo ImageInfo;

  /*! Transform repository */
  vtkTransformRepository*     TransformRepository;
};


#endif

