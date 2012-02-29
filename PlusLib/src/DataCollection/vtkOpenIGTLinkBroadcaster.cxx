/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

#include "PlusConfigure.h"

#include "vtkOpenIGTLinkBroadcaster.h"

#include <sstream>
#include <string>

#include "vtkImageData.h"
#include "vtkMatrix4x4.h"
#include "vtkObjectFactory.h"
#include "vtkSmartPointer.h"
#include "PlusVideoFrame.h"
#include "vtkTrackedFrameList.h"
#include "TrackedFrame.h"
#include "vtkTransformRepository.h"

#include "igtlImageMessage.h"
#include "igtlTransformMessage.h"

vtkCxxRevisionMacro( vtkOpenIGTLinkBroadcaster, "$Revision: 1.0 $" );
vtkStandardNewMacro( vtkOpenIGTLinkBroadcaster ); 

//----------------------------------------------------------------------------
vtkOpenIGTLinkBroadcaster::vtkOpenIGTLinkBroadcaster()
{
  this->DataCollector = NULL;
  this->TransformRepository = NULL;
  this->ImageInfo.Name = "Image";
}

//----------------------------------------------------------------------------
vtkOpenIGTLinkBroadcaster::~vtkOpenIGTLinkBroadcaster()
{
  this->DisconnectSockets(); 

  if (this->TransformRepository != NULL)
  {
    this->TransformRepository->Delete();
    this->TransformRepository = NULL;
  } 
}

//----------------------------------------------------------------------------
void vtkOpenIGTLinkBroadcaster::DisconnectSockets()
{
  
  for ( int i = 0; i < this->SocketInfos.size(); ++ i )
  {
    this->SocketInfos[ i ].Socket->CloseSocket();
  }
  this->SocketInfos.clear(); 
  this->ToolInfos.clear();
  
  if ( this->ImageInfo.IgtlSocketInfo.Socket.IsNotNull() )
  {
    //this->ImageInfo.IgtlSocketInfo.Socket->CloseSocket(); 
    this->ImageInfo.IgtlSocketInfo.Socket = NULL; 
  }
}

//----------------------------------------------------------------------------
void vtkOpenIGTLinkBroadcaster::PrintSelf( ostream& os, vtkIndent indent )
{
  this->Superclass::PrintSelf( os, indent );
}

//----------------------------------------------------------------------------
PlusStatus vtkOpenIGTLinkBroadcaster::Initialize()
{
  LOG_TRACE("vtkOpenIGTLinkBroadcaster::Initialize");

  if ( this->DataCollector == NULL || this->DataCollector->GetTrackingEnabled() == false )
  {
    LOG_ERROR( "Unable to initialize vtkOpenIGTLinkBroadcaster without valid DataCollector" );
    return PLUS_FAIL;
  }

  // Create transform repository
  this->TransformRepository = vtkTransformRepository::New();
  if (this->TransformRepository == NULL)
  {
    LOG_ERROR( "Unable to create TransformRepository!" );
    return PLUS_FAIL;
  }

  this->DisconnectSockets(); 

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkOpenIGTLinkBroadcaster::ReadConfiguration(vtkXMLDataElement* aConfig)
{
  LOG_TRACE("vtkOpenIGTLinkBroadcaster::ReadConfiguration");

  if (aConfig == NULL)
  {
    LOG_ERROR("Unable to read configuration"); 
    return PLUS_FAIL; 
  }

  if (this->TransformRepository->ReadConfiguration(aConfig) != PLUS_SUCCESS)
  {
    LOG_ERROR("Failed to read transform repository configuration!");
    return PLUS_FAIL; 
  }

  vtkXMLDataElement* dataCollectionElement = aConfig->FindNestedElementWithName("DataCollection"); 
  if (dataCollectionElement == NULL)
  {
    LOG_ERROR("Unable to find DataCollection element in XML tree!"); 
    return PLUS_FAIL;     
  }

  vtkXMLDataElement* openIGTLinkElement = dataCollectionElement->FindNestedElementWithName("OpenIGTLink"); 
  if (openIGTLinkElement == NULL)
  {
    LOG_ERROR("Unable to find OpenIGTLink element in XML tree!"); 
    return PLUS_FAIL;     
  }

  int imageElements = 0;

  // Read transforms to broadcast
  for (int transform = 0; transform < openIGTLinkElement->GetNumberOfNestedElements(); ++transform)
  {
    vtkXMLDataElement* transformDataElement = openIGTLinkElement->GetNestedElement(transform); 

    const char* sendToLink = transformDataElement->GetAttribute("SendTo");

    if ( STRCASECMP(transformDataElement->GetName(), "Transform") == 0 )
    {
      const char* transformName = transformDataElement->GetAttribute("Name");
      if ( this->AddTransformForBroadcasting(transformName, sendToLink) != PLUS_SUCCESS )
      {
        LOG_ERROR("Failed to add transform for broadcasting!"); 
        continue; 
      }
    }
    else if ( STRCASECMP(transformDataElement->GetName(), "Image") == 0 )
    {
      if ( this->AddImageForBroadcasting("Image", sendToLink ) != PLUS_SUCCESS) 
      {
        LOG_ERROR("Failed to add image for broadcasting!"); 
        continue;
      }
      ++imageElements;
    }
  }

  if (imageElements > 1)
  {
    LOG_WARNING("More than one Image elements found, only the last one is broadcasted!");
  }

  LOG_DEBUG( "Number of broadcasted tools: " << this->ToolInfos.size() );
  LOG_DEBUG( "Image is broadcasted: " << (imageElements==0?"false":"true") );

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkOpenIGTLinkBroadcaster::AddImageForBroadcasting( const char* aName, const char* aSendToLink )
{
  if ( aSendToLink == NULL )
  {
    LOG_ERROR("Failed to add image for broadcasting - send to link is NULL!"); 
    return PLUS_FAIL; 
  }

  if ( aName == NULL )
  {
    LOG_ERROR("Failed to add image for broadcasting - name is NULL!"); 
    return PLUS_FAIL; 
  }

  SocketInfo socketInfo; 
  if ( this->GetSocketInfoFromSendToLink(aSendToLink, socketInfo) != PLUS_SUCCESS )
  {
    LOG_ERROR("OpenIGTLink image element does not contain SendTo attribute - it cannot be broadcasted!");
    return PLUS_FAIL;
  }

  this->ImageInfo.IgtlSocketInfo = socketInfo;
  this->ImageInfo.Name = aName; 

  return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
PlusStatus vtkOpenIGTLinkBroadcaster::AddTransformForBroadcasting( const char* aTransformName, const char* aSendToLink )
{
  if ( aTransformName == NULL )
  {
    LOG_ERROR("Failed to add transform for broadcasting - transform name is NULL!"); 
    return PLUS_FAIL; 
  }

  if ( aSendToLink == NULL )
  {
    LOG_ERROR("Failed to add transform for broadcasting - send to link is NULL!"); 
    return PLUS_FAIL; 
  }

  SocketInfo socketInfo; 
  if ( this->GetSocketInfoFromSendToLink(aSendToLink, socketInfo) != PLUS_SUCCESS )
  {
    LOG_ERROR("OpenIGTLink Transform element does not contain SendTo attribute - it cannot be broadcasted!");
    return PLUS_FAIL;
  }

  IgtToolInfo toolInfo; 
  toolInfo.IgtlSocketInfo = socketInfo;
  toolInfo.Name = aTransformName;

  this->ToolInfos.push_back( toolInfo );

  return PLUS_SUCCESS; 

}

//----------------------------------------------------------------------------
PlusStatus vtkOpenIGTLinkBroadcaster::RemoveBroadcastedImageInfo()
{
  this->ImageInfo.IgtlSocketInfo.Socket = NULL; 
  return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
PlusStatus vtkOpenIGTLinkBroadcaster::RemoveBroadcastedToolInfo(const IgtToolInfo& toolInfo)
{
  for ( std::vector< IgtToolInfo >::iterator it = this->ToolInfos.begin(); it != this->ToolInfos.end(); ++it )
  {
    std::ostringstream toolSendtoLink; 
    toolSendtoLink << (*it).IgtlSocketInfo.Host << ":" << (*it).IgtlSocketInfo.Port << std::ends; 

    if ( (*it).Name == toolInfo.Name 
      && (*it).IgtlSocketInfo.Host == toolInfo.IgtlSocketInfo.Host
      && (*it).IgtlSocketInfo.Port == toolInfo.IgtlSocketInfo.Port )
    {
      this->ToolInfos.erase(it); 
      return PLUS_SUCCESS; 
    }
  }

  LOG_WARNING("Unable to remove broadcasted transform: couldn't find transform name '"<< toolInfo.Name 
    <<"' with send to link '"<< toolInfo.IgtlSocketInfo.Host << ":" << toolInfo.IgtlSocketInfo.Port <<"'!"); 
  return PLUS_FAIL; 
}

//----------------------------------------------------------------------------
PlusStatus vtkOpenIGTLinkBroadcaster::ChangeImageToolPauseStatus()
{
  this->ImageInfo.Paused = !this->ImageInfo.Paused; 
  return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
PlusStatus vtkOpenIGTLinkBroadcaster::ChangeBroadcastedToolPauseStatus(const IgtToolInfo& toolInfo)
{
  for ( std::vector< IgtToolInfo >::iterator it = this->ToolInfos.begin(); it != this->ToolInfos.end(); ++it )
  {
    std::ostringstream toolSendtoLink; 
    toolSendtoLink << (*it).IgtlSocketInfo.Host << ":" << (*it).IgtlSocketInfo.Port << std::ends; 

    if ( (*it).Name == toolInfo.Name 
      && (*it).IgtlSocketInfo.Host == toolInfo.IgtlSocketInfo.Host
      && (*it).IgtlSocketInfo.Port == toolInfo.IgtlSocketInfo.Port )
    {
      (*it).Paused = !(*it).Paused; 
      return PLUS_SUCCESS; 
    }
  }

  LOG_WARNING("Unable to change broadcasted transform pause status: couldn't find transform name '"<< toolInfo.Name 
    <<"' with send to link '"<< toolInfo.IgtlSocketInfo.Host << ":" << toolInfo.IgtlSocketInfo.Port <<"'!"); 
  return PLUS_FAIL; 
}

//----------------------------------------------------------------------------
void vtkOpenIGTLinkBroadcaster::SetDataCollector( vtkDataCollector* dataCollector )
{
  this->DataCollector = dataCollector;
}

//----------------------------------------------------------------------------
PlusStatus vtkOpenIGTLinkBroadcaster::GetSocketInfoFromSendToLink( const char* sendToLink, SocketInfo& socketInfo )
{
  LOG_TRACE("vtkOpenIGTLinkBroadcaster::GetSocketInfoFromSendToLink");

  if ( sendToLink == NULL )
  {
    LOG_WARNING( "SendTo address could not be parsed if it's NULL!"); 
    return PLUS_FAIL; 
  }

  // Parse the SendTo string to get hostname and port
  std::istringstream iss(sendToLink);
  std::string hostname;
  getline(iss, hostname, ':');
  int port=0;
  iss >> port;    

  if ( hostname.empty() || port <= 0 )
  {
    LOG_WARNING( "SendTo address '"<< sendToLink << "' could not be parsed (hostname=" << hostname << ", port=" << port << ")" );
    return PLUS_FAIL;
  }      

  // Check if socket info already exists 
  for ( int socketIndex = 0; socketIndex < this->SocketInfos.size(); ++ socketIndex )
  {
    if ( hostname.compare(this->SocketInfos[ socketIndex ].Host) == 0
      && this->SocketInfos[ socketIndex ].Port == port )
    {
      // Socket found return with the socket info
      socketInfo = this->SocketInfos[ socketIndex ]; 
      return PLUS_SUCCESS; 
    }
  }

  // Could find socket in the list, create new socket
  igtl::ClientSocket::Pointer socket = igtl::ClientSocket::New();
  if ( socket->ConnectToServer( hostname.c_str(), port ) < 0 )
  {
    LOG_WARNING( "Could not connect to OpenIGTLink host: " << sendToLink );
    return PLUS_FAIL;
  }

  socketInfo.Host = hostname;
  socketInfo.Port = port;
  socketInfo.Socket = socket;

  this->SocketInfos.push_back( socketInfo );

  return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
PlusStatus vtkOpenIGTLinkBroadcaster::SendMessages()
{
  //LOG_TRACE("vtkOpenIGTLinkBroadcaster::SendMessages");

  // Check status and possible errors.

  if ( this->ToolInfos.size() == 0 && this->ImageInfo.IgtlSocketInfo.Socket.IsNull() )
  {
    LOG_DEBUG( "Nothing to broadcast..." );
    return PLUS_SUCCESS;
  }

  if ( this->DataCollector == NULL )
  {
    LOG_ERROR( "Unable to send OpenIGTLink messages without a proper DataCollector" );
    return PLUS_FAIL;
  }

  TrackedFrame trackedFrame;
  this->DataCollector->GetTrackedFrame( &trackedFrame );

  if ( this->TransformRepository->SetTransforms(trackedFrame) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to set current transforms to transform repository!"); 
    return PLUS_FAIL;
  }

  double timestamp = trackedFrame.GetTimestamp();

  int numberOfErrors = 0; 

  // Read the transforms to be broadcasted
  for ( int igtIndex = 0; igtIndex < this->ToolInfos.size(); ++ igtIndex )
  {
    if ( this->ToolInfos[ igtIndex ].Paused )
    {
      // Broadcasting paused, no need to send transform
      continue; 
    }

    vtkSmartPointer< vtkMatrix4x4 > transformMatrix  = vtkSmartPointer< vtkMatrix4x4 >::New();
    
    const char* transformNameStr = this->ToolInfos[ igtIndex ].Name.c_str();
    igtl::ClientSocket::Pointer socket = this->ToolInfos[ igtIndex ].IgtlSocketInfo.Socket;
        
    PlusTransformName transformName; 
    if ( transformName.SetTransformName(transformNameStr) != PLUS_SUCCESS )
    {
      this->ToolInfos[igtIndex].Valid = false; 
      LOG_ERROR("Failed to set transform name " << transformNameStr); 
      numberOfErrors++; 
      continue;
    }

    bool valid = false;
    if ( this->TransformRepository->GetTransform(transformName, transformMatrix, &valid) != PLUS_SUCCESS )
    {
      this->ToolInfos[igtIndex].Valid = false; 
      LOG_ERROR("Cannot get frame transform '" << transformNameStr << "' from tracked frame!");
      numberOfErrors++; 
      continue;
    }

    // Update transform status 
    this->ToolInfos[igtIndex].Valid = valid; 
		if ( !valid )
    {
      LOG_INFO( "Invalid transform: " << transformNameStr );
      continue;
    }

    // Prepare the igtl matrix and timestamp.

    igtl::Matrix4x4 igtlMatrix;


    for ( int row = 0; row < 4; ++ row )
    {
      for ( int col = 0; col < 4; ++ col )
      {
        // igtlMatrix[ row ][ col ] = mToolToTracker->GetElement( row, col );
        igtlMatrix[ row ][ col ] = transformMatrix->GetElement(row, col);
      }
    }

    igtl::TimeStamp::Pointer igtlTime = igtl::TimeStamp::New();
    igtlTime->SetTime( timestamp );


    // Create and send OpenIGTLink Message for non ref transforms.
    
    std::string strTransformName;
    transformName.GetTransformName( strTransformName );
    
    igtl::TransformMessage::Pointer transformMessage = igtl::TransformMessage::New();
    transformMessage->SetMatrix( igtlMatrix );
    transformMessage->SetTimeStamp( igtlTime );
    transformMessage->SetDeviceName( strTransformName.c_str() );
    transformMessage->Pack();

    int success = socket->Send( transformMessage->GetPackPointer(), transformMessage->GetPackSize() );

    if ( success == 0 )
    {
      LOG_WARNING( "Could not broadcast transform: " << transformNameStr );
      numberOfErrors++; 
    }
  }


  // If we should broadcast the image slice too, set up the image container.
  if ( this->SendImageMessage( &trackedFrame ) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to send image message!"); 
    numberOfErrors++; 
  }

  return ( numberOfErrors == 0 ? PLUS_SUCCESS : PLUS_FAIL );
}

//----------------------------------------------------------------------------
PlusStatus vtkOpenIGTLinkBroadcaster::SendImageMessage( TrackedFrame* trackedFrame )
{
  //LOG_TRACE("vtkOpenIGTLinkBroadcaster::SendImageMessage");

  if ( this->ImageInfo.Paused )
  {
    // Broadcasting paused, no need to send image
    return PLUS_SUCCESS; 
  }

  if ( this->ImageInfo.IgtlSocketInfo.Socket.IsNull()
    || !this->ImageInfo.IgtlSocketInfo.Socket->GetConnected() )
  {
    return PLUS_SUCCESS; 
  }

  if ( trackedFrame == NULL )
  {
    LOG_ERROR("Failed to send image message - tracked frame is NULL!"); 
    return PLUS_FAIL; 
  }

  this->ImageInfo.Valid = trackedFrame->GetImageData()->IsImageValid(); 

  if ( !trackedFrame->GetImageData()->IsImageValid() )
  {
    LOG_WARNING("Unable to send image message - image data is NOT valid!"); 
    return PLUS_FAIL; 
  }

  igtl::ClientSocket::Pointer defaultSocket = this->ImageInfo.IgtlSocketInfo.Socket;

  // Read the actual image data with transform.
  double timestamp = trackedFrame->GetTimestamp();

  vtkImageData* frameImage = trackedFrame->GetImageData()->GetVtkImage();

  igtl::TimeStamp::Pointer igtlFrameTime = igtl::TimeStamp::New();
  igtlFrameTime->SetTime( timestamp );


  // Convert matrix and time formats.

  igtl::Matrix4x4 igtlMatrix;

  // Create and send the image message.

  int    imageSizePixels[ 3 ] = { 0, 0, 0 };
  double imageSpacingMm[ 3 ] = { 0, 0, 0 };
  int    subSizePixels[ 3 ] = { 0, 0, 0 };
  int    subOffset[ 3 ] = { 0, 0, 0 };
  int    scalarType = igtl::ImageMessage::TYPE_UINT8;

  frameImage->GetDimensions( imageSizePixels );
  frameImage->GetSpacing( imageSpacingMm );
  frameImage->GetDimensions( subSizePixels );

  float spacingFloat[ 3 ];
  for ( int i = 0; i < 3; ++ i ) spacingFloat[ i ] = (float)imageSpacingMm[ i ];

  igtl::ImageMessage::Pointer imageMessage = igtl::ImageMessage::New();
  imageMessage->SetDimensions( imageSizePixels );
  imageMessage->SetSpacing( spacingFloat );
  imageMessage->SetScalarType( scalarType );
  imageMessage->SetDeviceName( "Ultrasound" );
  imageMessage->SetSubVolume( subSizePixels, subOffset );
  imageMessage->AllocateScalars();

  unsigned char* igtlImagePointer = (unsigned char*)( imageMessage->GetScalarPointer() );
  unsigned char* vtkImagePointer = (unsigned char*)( frameImage->GetScalarPointer() );

  memcpy(igtlImagePointer, vtkImagePointer, imageMessage->GetImageSize());

  imageMessage->SetMatrix( igtlMatrix );
  imageMessage->SetTimeStamp( igtlFrameTime );


  imageMessage->Pack();

  if ( defaultSocket->Send( imageMessage->GetPackPointer(), imageMessage->GetPackSize() ) == 0 )
  {
    LOG_ERROR( "Could not send image through OpenIGTLink port" );
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}
