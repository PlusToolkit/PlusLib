
#include "vtkPlusOpenIGTLinkClient.h"

#include "vtkMultiThreader.h"
#include "vtkMutexLock.h"

#include "igtlMessageHeader.h"
#include "igtlOSUtil.h"
#include "igtlServerSocket.h"

#include "igtlStringMessage1.h"
#include "vtkPlusCommand.h"



vtkCxxRevisionMacro( vtkPlusOpenIGTLinkClient, "$Revision: 1.0 $" );
vtkStandardNewMacro( vtkPlusOpenIGTLinkClient ); 




int
vtkPlusOpenIGTLinkClient
::ConnectToServer()
{
  int r = this->ClientSocket->ConnectToServer( this->ServerAddress.c_str(), this->NetworkPort );
  
  if ( r != 0 )
    {
    LOG_ERROR( "Cannot connect to the server." );
    return r;
    }
  else
    {
    LOG_INFO( "Client successfully connected to server." );
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
::SendCommand( vtkPlusCommand* command )
{
  
    // Convert the command to a string message.
  
  std::string strToBeSent = command->GetStringRepresentation();
  
  igtl::StringMessage1::Pointer stringMessage = igtl::StringMessage1::New();
    stringMessage->SetDeviceName( "PlusClient" );
    stringMessage->SetString( strToBeSent );
    stringMessage->Pack();
  
  
    // Send the string message to the server.
  
  // int alive = this->ClientSocket->GetConnected();
  LOG_INFO( "Sending message: " << command->GetStringRepresentation() );
  int success = this->ClientSocket->Send( stringMessage->GetPackPointer(),
                                          stringMessage->GetPackSize() );
  
  if ( success == 1 )
    {
    return true;
    }
  
  LOG_WARNING( "OpenIGTLink client couldn't send command to server." );
  return false;
}



void
vtkPlusOpenIGTLinkClient
::Lock()
{
  this->Mutex->Lock();
}



void
vtkPlusOpenIGTLinkClient
::Unlock()
{
  this->Mutex->Unlock();
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
  this->Mutex = vtkMutexLock::New();
  this->ClientSocket = igtl::ClientSocket::New();
  this->SocketMutex = vtkMutexLock::New();
}



/**
 * Destructor.
 */
vtkPlusOpenIGTLinkClient
::~vtkPlusOpenIGTLinkClient()
{
  if ( this->Threader )
    {
    this->Threader->Delete();
    }
  
  if ( this->Mutex )
    {
    this->Mutex->Delete();
    }
    
  if ( this->SocketMutex )
    {
    this->SocketMutex->Delete();
    }
}
