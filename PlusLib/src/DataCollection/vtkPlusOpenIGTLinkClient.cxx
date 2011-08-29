
#include "vtkPlusOpenIGTLinkClient.h"

#include "vtkMultiThreader.h"

#include "igtlMessageHeader.h"
#include "igtlOSUtil.h"
#include "igtlServerSocket.h"

#include "igtlStringMessage1.h"



vtkCxxRevisionMacro( vtkPlusOpenIGTLinkClient, "$Revision: 1.0 $" );
vtkStandardNewMacro( vtkPlusOpenIGTLinkClient ); 



static
void*
vtkCommunicationThread( vtkMultiThreader::ThreadInfo* data )
{

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
