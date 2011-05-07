
#include "vtkOpenIGTLinkBroadcaster.h"


#include "PlusConfigure.h"

#include "vtkMatrix4x4.h"
#include "vtkObjectFactory.h"
#include "vtkSmartPointer.h"

#include "igtlTransformMessage.h"



vtkCxxRevisionMacro( vtkOpenIGTLinkBroadcaster, "$Revision: 1.0 $" );
vtkStandardNewMacro( vtkOpenIGTLinkBroadcaster ); 



vtkOpenIGTLinkBroadcaster
::vtkOpenIGTLinkBroadcaster()
{
  this->DataCollector = NULL;
}



vtkOpenIGTLinkBroadcaster
::~vtkOpenIGTLinkBroadcaster()
{
  std::vector< igtl::ClientSocket::Pointer >::iterator it;
  for ( it = this->Sockets.begin(); it != this->Sockets.end(); ++ it )
    {
    (*it)->CloseSocket();
    }
  this->Sockets.clear();
}



void
vtkOpenIGTLinkBroadcaster
::PrintSelf( ostream& os, vtkIndent indent )
{
	this->Superclass::PrintSelf( os, indent );
}



void
vtkOpenIGTLinkBroadcaster
::AddSocket( std::string host, unsigned int port )
{
    // Try to set up connection.
  
  igtl::ClientSocket::Pointer socket = igtl::ClientSocket::New();
  int r = socket->ConnectToServer( host.c_str(), port );
  if ( r != 0 )
    {
    LOG_WARNING( "Failed to connect to OpenIGTLink server." );
    }
  
  this->Sockets.push_back( socket );
}



void
vtkOpenIGTLinkBroadcaster
::SendMessages()
{
  if ( this->DataCollector == NULL )
    {
    LOG_WARNING( "Tried to send OpenIGTLink messages without specifying a DataCollector." );
    return;
    }
  
  if ( ! this->DataCollector->GetTracker()->IsTracking() )
    {
    LOG_WARNING( "Tried to send OpenIGTLink messages without starting the tracker." );
    return;
    }
  
  
    // Read the transform from the DataCollector.
  
  vtkSmartPointer< vtkMatrix4x4 > probeToTrackerMatrix = vtkSmartPointer< vtkMatrix4x4 >::New();
  double timestamp( 0 ); 
	long flags( 0 ); 
  
  this->DataCollector->GetTransformWithTimestamp( probeToTrackerMatrix, timestamp, flags, this->DataCollector->GetMainToolNumber() ); 
  
  if ( flags & ( TR_MISSING | TR_OUT_OF_VIEW ) ) 
    {
    LOG_WARNING( "Tracker out of view..." );
    return;
    }
  else if ( flags & ( TR_REQ_TIMEOUT ) ) 
    {
    LOG_WARNING( "Tracker request timeout..." );
    return;
    } 
  
  
    // Prepare the OpenIGTLink transform message.
  
  igtl::TransformMessage::Pointer transformMessage = igtl::TransformMessage::New();
  igtl::Matrix4x4 igtlMatrix;
  
  for ( int row = 0; row < 4; ++ row )
    {
    for ( int col = 0; col < 4; ++ col )
      {
      igtlMatrix[ row ][ col ] = probeToTrackerMatrix->GetElement( row, col );
      }
    }
  
  transformMessage->SetDeviceName( "Probe" );
  transformMessage->SetMatrix( igtlMatrix );
  transformMessage->Pack();
  
  
    // Send the OpenIGTLink transform message to all hosts.
  
  std::vector< igtl::ClientSocket::Pointer >::iterator it;
  for ( it = this->Sockets.begin(); it != this->Sockets.end(); ++ it )
    {
    (*it)->Send( transformMessage->GetPackPointer(), transformMessage->GetPackSize() );
    }
  
}

