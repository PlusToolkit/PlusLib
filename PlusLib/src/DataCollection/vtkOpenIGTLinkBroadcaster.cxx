
#include "vtkOpenIGTLinkBroadcaster.h"


#include "PlusConfigure.h"

#include "vtkBMPWriter.h" // debug
#include "vtkImageData.h"
#include "vtkMatrix4x4.h"
#include "vtkObjectFactory.h"
#include "vtkSmartPointer.h"

#include "vtkTrackerTool.h"

#include "igtlImageMessage.h"
#include "igtlTransformMessage.h"



vtkCxxRevisionMacro( vtkOpenIGTLinkBroadcaster, "$Revision: 1.0 $" );
vtkStandardNewMacro( vtkOpenIGTLinkBroadcaster ); 



vtkOpenIGTLinkBroadcaster::Status
vtkOpenIGTLinkBroadcaster
::SetDataCollector( vtkDataCollector* dataCollector )
{
  this->DataCollector = dataCollector;
  
  return this->InternalStatus;
}



vtkOpenIGTLinkBroadcaster::Status
vtkOpenIGTLinkBroadcaster
::Initialize( std::string &strError )
{
  if (    this->DataCollector == NULL
       || this->DataCollector->GetTracker() == NULL )
    {
    LOG_ERROR( "Tried to initialize vtkOpenIGTLinkBroadcaster without DataCollector." );
    this->InternalStatus = STATUS_NOT_INITIALIZED;
    return this->InternalStatus;
    }
  
  
    // Check default tool port.
  
  int defaultToolPort = this->DataCollector->GetTracker()->GetDefaultTool();
  
  if ( defaultToolPort < 0 )
    {
    LOG_ERROR( "Default tool port number missing in DataCollector." );
    this->InternalStatus = STATUS_MISSING_DEFAULT_TOOL;
    return this->InternalStatus;
    }
  
  
    // Check SendTo address for default tool.
  
  vtkTrackerTool* defaultTool = this->DataCollector->GetTracker()->GetTool( defaultToolPort );
  const char* constCharSendTo = defaultTool->GetSendToLink();
   
  if ( constCharSendTo == NULL )
    {
    LOG_ERROR( "No SendTo address defined for default tracker tool." );
    this->InternalStatus = STATUS_NOT_INITIALIZED;
    return this->InternalStatus;
    }
  
  std::string strSendTo( constCharSendTo );

  char* charSendTo = new char[ strSendTo.size() + 1 ];
  strcpy( charSendTo, strSendTo.c_str() );
  const char* hostname = strtok( charSendTo, ":");
  char*       charPort = strtok( NULL, ":\n" );
  int         port = atoi( charPort );
  
  
  if ( hostname == NULL || charPort == NULL || port == 0 )
    {
    charSendTo = defaultTool->GetSendToLink();
    LOG_ERROR( "Could not connect to OpenIGTLink host: " << defaultTool->GetSendToLink() );
    this->InternalStatus = STATUS_HOST_NOT_FOUND;
    strError = std::string( defaultTool->GetSendToLink() );
    return this->InternalStatus;
    }
  
  
    // Try to connect.
  
  int fail = this->DefaultSocket->ConnectToServer( hostname, port );
  
  if ( fail )
    {
    LOG_ERROR( "Could not connect to OpenIGTLink host: " << defaultTool->GetSendToLink() );
    this->InternalStatus = STATUS_HOST_NOT_FOUND;
    strError = defaultTool->GetSendToLink();
    return this->InternalStatus;
    }
  
  
    // Everything worked.
  
  this->InternalStatus = STATUS_OK;
  return this->InternalStatus;
}



  // Constructor.

vtkOpenIGTLinkBroadcaster
::vtkOpenIGTLinkBroadcaster()
{
  this->DataCollector = NULL;
  this->InternalStatus = STATUS_NOT_INITIALIZED;
  this->DefaultSocket = igtl::ClientSocket::New();
}



  // Destructor.

vtkOpenIGTLinkBroadcaster
::~vtkOpenIGTLinkBroadcaster()
{
  this->DefaultSocket->CloseSocket();
}



void
vtkOpenIGTLinkBroadcaster
::PrintSelf( ostream& os, vtkIndent indent )
{
	this->Superclass::PrintSelf( os, indent );
}



vtkOpenIGTLinkBroadcaster::Status
vtkOpenIGTLinkBroadcaster
::SendMessages()
{
  std::string str;
  return SendMessages( str );
}



/**
 * @returns Internal status. If it is HOST_NOT_FOUND, strError will be filled
 *          with the host:port address not found.
 * 
 */
vtkOpenIGTLinkBroadcaster::Status
vtkOpenIGTLinkBroadcaster
::SendMessages( std::string strError )
{
  if ( this->InternalStatus == STATUS_NOT_INITIALIZED )
    {
    LOG_WARNING( "Broadcaster not initialized." );
    return this->InternalStatus;
    }
  
  if ( this->DataCollector == NULL )
    {
    LOG_WARNING( "Tried to send OpenIGTLink messages without specifying a DataCollector." );
    this->InternalStatus = STATUS_NOT_INITIALIZED;
    return this->InternalStatus;
    }
  
  if ( ! this->DataCollector->GetTracker()->IsTracking() )
    {
    LOG_WARNING( "Tried to send OpenIGTLink messages without starting the tracker." );
    this->InternalStatus = STATUS_NOT_TRACKING;
    return this->InternalStatus;
    }
  
  
    // Read the default transform relative to the reference from the DataCollector.
  
  const int defaultTool = this->DataCollector->GetDefaultToolPortNumber();
  vtkSmartPointer< vtkMatrix4x4 > probeToTrackerMatrix = vtkSmartPointer< vtkMatrix4x4 >::New();
  double timestamp( 0 ); 
  long flags( 0 ); 
  
  vtkSmartPointer< vtkImageData > frameImage = vtkSmartPointer< vtkImageData >::New();
  
    
  if ( this->DataCollector->GetAcquisitionType() != SYNCHRO_VIDEO_NONE )
    {
    frameImage->SetDimensions( this->DataCollector->GetVideoSource()->GetFrameSize() );
    frameImage->SetOrigin( this->DataCollector->GetVideoSource()->GetDataOrigin() );
    frameImage->SetSpacing( this->DataCollector->GetVideoSource()->GetDataSpacing() );
    frameImage->SetScalarTypeToUnsignedChar();
    frameImage->AllocateScalars();
  
    if ( this->DataCollector->GetTrackedFrame( frameImage, probeToTrackerMatrix, flags, timestamp, defaultTool, true ) != PLUS_SUCCESS )
    {
	    LOG_WARNING( "Failed to get tracked frame..." );
	    this->InternalStatus = STATUS_MISSING_TRACKED_FRAME;
	    return this->InternalStatus;
    }
   
    if ( flags & ( TR_MISSING | TR_OUT_OF_VIEW ) ) 
      {
      LOG_WARNING( "Tracker out of view..." );
      this->InternalStatus = STATUS_NOT_TRACKING;
      return this->InternalStatus;
      }
    else if ( flags & ( TR_REQ_TIMEOUT ) ) 
      {
      LOG_WARNING( "Tracker request timeout..." );
      this->InternalStatus = STATUS_NOT_TRACKING;
      return this->InternalStatus;
      } 
    }
  else
    {
    this->DataCollector->GetTransformWithTimestamp( probeToTrackerMatrix, timestamp, flags, defaultTool, true );
    }
  
    // Prepare the transform matrices.
  
  igtl::Matrix4x4 igtlMatrix;
  
  for ( int row = 0; row < 4; ++ row )
    {
    for ( int col = 0; col < 4; ++ col )
      {
      igtlMatrix[ row ][ col ] = probeToTrackerMatrix->GetElement( row, col );
      }
    }
  
  
  igtl::TimeStamp::Pointer igtlFrameTime = igtl::TimeStamp::New();
  igtlFrameTime->SetTime( timestamp );
  
  
    // Create and send OpenIGTLink message.
  
  int success = 0;
  
  if ( this->DataCollector->GetAcquisitionType() != SYNCHRO_VIDEO_NONE )
    {
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
      imageMessage->SetDeviceName( this->DataCollector->GetTracker()->GetTool( defaultTool )->GetToolName() );
      imageMessage->SetSubVolume( subSizePixels, subOffset );
      imageMessage->AllocateScalars();
    
    unsigned char* igtlImagePointer = (unsigned char*)( imageMessage->GetScalarPointer() );
    unsigned char* vtkImagePointer = (unsigned char*)( frameImage->GetScalarPointer() );
    
    memcpy(igtlImagePointer, vtkImagePointer, imageMessage->GetImageSize());
    
    imageMessage->SetMatrix( igtlMatrix );
    imageMessage->SetTimeStamp( igtlFrameTime );
    imageMessage->Pack();
    
    success = this->DefaultSocket->Send( imageMessage->GetPackPointer(), imageMessage->GetPackSize() );
    }
  else
    {
    igtl::TransformMessage::Pointer transformMessage = igtl::TransformMessage::New();
    transformMessage->SetMatrix( igtlMatrix );
    transformMessage->SetTimeStamp( igtlFrameTime );
    transformMessage->SetDeviceName( this->DataCollector->GetTracker()->GetTool( defaultTool )->GetToolName() );
    transformMessage->Pack();
    
    success = this->DefaultSocket->Send( transformMessage->GetPackPointer(), transformMessage->GetPackSize() );
    }
  
  
  if ( success == 0 )
    {
    this->InternalStatus = STATUS_SEND_ERROR;
    return this->InternalStatus;
    }
  
  this->InternalStatus = STATUS_OK;
  return this->InternalStatus;
}

