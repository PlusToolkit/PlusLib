
#include "vtkPlusOpenIGTLinkClient.h"

#include "vtkMultiThreader.h"

#include "igtlMessageHeader.h"
#include "igtlOSUtil.h"
#include "igtlServerSocket.h"

#include "igtlStringMessage1.h"
#include "vtkPlusCommand.h"



vtkCxxRevisionMacro( vtkPlusOpenIGTLinkClient, "$Revision: 1.0 $" );
vtkStandardNewMacro( vtkPlusOpenIGTLinkClient ); 



static
void*
vtkCommunicationThread( vtkMultiThreader::ThreadInfo* data )
{
  vtkPlusOpenIGTLinkClient* self = (vtkPlusOpenIGTLinkClient*)( data->UserData );
  
  
    // Prepare and send request.
  
  igtl::StringMessage1::Pointer stringMessage = igtl::StringMessage1::New();
    stringMessage->SetDeviceName( "PlusClient" );
    stringMessage->SetString( self->ActiveCommand->GetStringRepresentation() );
    stringMessage->Pack();
    
  self->ClientSocket->Send( stringMessage->GetPackPointer(), stringMessage->GetPackSize() );
  
   
    // Wait for a response.
  
  igtl::MessageHeader::Pointer headerMsg = igtl::MessageHeader::New();
  headerMsg->InitPack();
  int rs = self->ClientSocket->Receive( headerMsg->GetPackPointer(), headerMsg->GetPackSize() );
  if ( rs == 0 )
    {
    LOG_WARNING( "Connection closed." );
    self->ClientSocket->CloseSocket();
    return NULL;
    }
  if ( rs != headerMsg->GetPackSize() )
    {
    LOG_ERROR( "Message size information and actual data size don't match." ); 
    return NULL;
    }
  
  headerMsg->Unpack();
  
  //debug
  std::cout << "Client received message from device type: " << headerMsg->GetDeviceType() << std::endl;
  
  self->ActiveCommand->ProcessResponse( headerMsg, self->ClientSocket );
  
  
  self->ActiveCommand = NULL;
  self->CommandInProgress = false;
  return NULL;
}



int
vtkPlusOpenIGTLinkClient
::ConnectToServer()
{
  int r = this->ClientSocket->ConnectToServer( this->ServerAddress.c_str(), this->NetworkPort );
  
  if ( r != 0 )
    {
    LOG_WARNING( "Cannot connect to the server." );
    return r;
    }
  
  return r;
}



int
vtkPlusOpenIGTLinkClient
::StartDataCollector()
{
  return 0;
}



int
vtkPlusOpenIGTLinkClient
::StopDataCollector()
{
  return 0;
}



bool
vtkPlusOpenIGTLinkClient
::StartCommand( vtkPlusCommand* command )
{
  if ( this->CommandInProgress == true )
    {
    return false;
    }
  
  this->ActiveCommand = command;
  this->CommandInProgress = true;
  this->ThreadId = this->Threader->SpawnThread( (vtkThreadFunctionType)&vtkCommunicationThread, this );
  
  return true;
}



void
vtkPlusOpenIGTLinkClient
::PrintSelf( ostream& os, vtkIndent indent )
{
	this->Superclass::PrintSelf( os, indent );
}



/**
 * Protected constructor.
 */
vtkPlusOpenIGTLinkClient
::vtkPlusOpenIGTLinkClient()
{
  this->NetworkPort = -1;
  this->ThreadId = -1;
  this->CommandInProgress = false;
  this->ActiveCommand = NULL;
  
  this->Threader = vtkMultiThreader::New();
  this->ClientSocket = igtl::ClientSocket::New();
}



/**
 * Destructor.
 */
vtkPlusOpenIGTLinkClient
::~vtkPlusOpenIGTLinkClient()
{
  
}
