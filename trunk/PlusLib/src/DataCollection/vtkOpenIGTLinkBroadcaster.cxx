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
  this->InternalStatus = STATUS_NOT_INITIALIZED;
  this->ApplyStylusCalibration = false;
  this->TransformRepository = NULL;
  this->ImageSocket = NULL;
}

//----------------------------------------------------------------------------
vtkOpenIGTLinkBroadcaster::~vtkOpenIGTLinkBroadcaster()
{
  for ( int i = 0; i < this->SocketInfos.size(); ++ i )
  {
    this->SocketInfos[ i ].Socket->CloseSocket();
  }

  this->SocketInfos.clear();

  if (this->TransformRepository != NULL)
  {
    this->TransformRepository->Delete();
    this->TransformRepository = NULL;
  } 
}

//----------------------------------------------------------------------------
void vtkOpenIGTLinkBroadcaster::PrintSelf( ostream& os, vtkIndent indent )
{
  this->Superclass::PrintSelf( os, indent );
}

//----------------------------------------------------------------------------
vtkOpenIGTLinkBroadcaster::Status vtkOpenIGTLinkBroadcaster::Initialize( std::string &strError )
{
  LOG_TRACE("vtkOpenIGTLinkBroadcaster::Initialize");

  if ( this->DataCollector == NULL || this->DataCollector->GetTrackingEnabled() == false )
  {
    LOG_ERROR( "Tried to initialize vtkOpenIGTLinkBroadcaster without valid DataCollector" );
    this->InternalStatus = STATUS_NOT_INITIALIZED;
    return this->InternalStatus;
  }

  // Create transform repository
  this->TransformRepository = vtkTransformRepository::New();
  if (this->TransformRepository == NULL)
  {
    LOG_ERROR( "Unable to create TransformRepository!" );
    this->InternalStatus = STATUS_NOT_INITIALIZED;
    return this->InternalStatus;
  }

  this->InternalStatus = STATUS_OK;
  return this->InternalStatus;
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

    SocketInfo socketInfo; 
    const char* sendToLink = transformDataElement->GetAttribute("SendTo");
    if ( (sendToLink == NULL)
      || (GetSocketInfoFromSendToLink(sendToLink, socketInfo) != PLUS_SUCCESS) )
    {
	    LOG_WARNING("OpenIGTLink Transform element does not contain SendTo attribute - it cannot be broadcasted!");
      continue;
    }

    if ( STRCASECMP(transformDataElement->GetName(), "Transform") == 0 )
    {
      const char* transformName = transformDataElement->GetAttribute("Name");
      if (transformName == NULL)
      {
	      LOG_WARNING("OpenIGTLink Transform element does not have a Name element - it cannot be broadcasted!");
        continue;
      }

      IgtToolInfo info;
      info.Socket = socketInfo.Socket;
      info.TransformName = transformName;

      this->ToolInfos.push_back( info );
    }
    else if ( STRCASECMP(transformDataElement->GetName(), "Image") == 0 )
    {
      this->ImageSocket = socketInfo.Socket;
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
void vtkOpenIGTLinkBroadcaster::SetApplyStylusCalibration( bool apply )
{
  LOG_TRACE("vtkOpenIGTLinkBroadcaster::SetApplyStylusCalibration(" << (apply?"TRUE":"FALSE") << ")");

  this->ApplyStylusCalibration = apply;
}

//----------------------------------------------------------------------------
vtkOpenIGTLinkBroadcaster::Status vtkOpenIGTLinkBroadcaster::SetDataCollector( vtkDataCollector* dataCollector )
{
  this->DataCollector = dataCollector;

  return this->InternalStatus;
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
  if ( socket->ConnectToServer( hostname.c_str(), port ) )
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
vtkOpenIGTLinkBroadcaster::Status vtkOpenIGTLinkBroadcaster::SendMessages()
{
  //LOG_TRACE("vtkOpenIGTLinkBroadcaster::SendMessages");

  std::string str;
  return SendMessages( str );
}

//----------------------------------------------------------------------------
vtkOpenIGTLinkBroadcaster::Status vtkOpenIGTLinkBroadcaster::SendMessages( std::string strError )
{
  //LOG_TRACE("vtkOpenIGTLinkBroadcaster::SendMessages");

  // Check status and possible errors.

  if ( this->InternalStatus == STATUS_NOT_INITIALIZED )
  {
    LOG_WARNING( "Broadcaster not initialized." );
    return this->InternalStatus;
  }

  if ( this->DataCollector == NULL )
  {
    LOG_WARNING( "Tried to send OpenIGTLink messages without a proper DataCollector." );
    this->InternalStatus = STATUS_NOT_INITIALIZED;
    return this->InternalStatus;
  }

  if ( ! this->DataCollector->GetTrackingEnabled() )
  {
    LOG_WARNING( "Tried to send OpenIGTLink messages without starting the tracker." );
    this->InternalStatus = STATUS_NOT_TRACKING;
    return this->InternalStatus;
  }


  TrackedFrame trackedFrame;
  this->DataCollector->GetTrackedFrame( &trackedFrame );

  if ( this->TransformRepository->SetTransforms(trackedFrame) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to set current transforms to transform repository!"); 
    this->InternalStatus = STATUS_NOT_INITIALIZED;
    return this->InternalStatus;
  }

  double timestamp = trackedFrame.GetTimestamp();


  // Read the transforms to be broadcasted
  for ( int igtIndex = 0; igtIndex < this->ToolInfos.size(); ++ igtIndex )
  {
    vtkSmartPointer< vtkMatrix4x4 > transformMatrix  = vtkSmartPointer< vtkMatrix4x4 >::New();

    const char* transformNameStr = this->ToolInfos[ igtIndex ].TransformName.c_str();
    igtl::ClientSocket::Pointer socket = this->ToolInfos[ igtIndex ].Socket;
        
    PlusTransformName transformName; 
    if ( transformName.SetTransformName(transformNameStr) != PLUS_SUCCESS )
    {
      LOG_ERROR("Failed to set transform name " << transformNameStr); 
      continue;
    }

    bool valid = false;
    if ( this->TransformRepository->GetTransform(transformName, transformMatrix, &valid) != PLUS_SUCCESS )
    {
      LOG_ERROR("Cannot get frame transform '" << transformNameStr << "' from tracked frame!");
      continue;
    }
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

    igtl::TransformMessage::Pointer transformMessage = igtl::TransformMessage::New();
    transformMessage->SetMatrix( igtlMatrix );
    transformMessage->SetTimeStamp( igtlTime );
    transformMessage->SetDeviceName( transformName.From().c_str() );
    transformMessage->Pack();

    int success = socket->Send( transformMessage->GetPackPointer(), transformMessage->GetPackSize() );

    if ( success == 0 )
    {
      LOG_WARNING( "Could not broadcast transform: " << transformNameStr );
    }
  }


  // If we should broadcast the image slice too, set up the image container.
  if ( this->DataCollector->GetVideoEnabled() )
  {
    this->SendImageMessage( &trackedFrame, strError );
  }


  this->InternalStatus = STATUS_OK;
  return this->InternalStatus;
}

//----------------------------------------------------------------------------
void vtkOpenIGTLinkBroadcaster::SendImageMessage( TrackedFrame* trackedFrame, std::string strError )
{
  //LOG_TRACE("vtkOpenIGTLinkBroadcaster::SendImageMessage");

  if ( this->DataCollector == NULL )
  {
    LOG_ERROR("Invalid data collector, cannot send message!");
    return;
  }

  igtl::ClientSocket::Pointer defaultSocket = this->ImageSocket;

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

  int success = defaultSocket->Send( imageMessage->GetPackPointer(), imageMessage->GetPackSize() );

  if ( success == 0 ) // TODO redundancy
  {
    LOG_ERROR( "Could not send image through OpenIGTLink port" );
    this->InternalStatus = STATUS_SEND_ERROR;
    return;
  }
  else
  {
    this->InternalStatus = STATUS_OK;
  }
}
