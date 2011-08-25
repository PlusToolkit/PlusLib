
#include "vtkPlusOpenIGTLinkServer.h"



vtkCxxRevisionMacro( vtkPlusOpenIGTLinkServer, "$Revision: 1.0 $" );
vtkStandardNewMacro( vtkPlusOpenIGTLinkServer ); 



void
vtkPlusOpenIGTLinkServer
::PrintSelf( ostream& os, vtkIndent indent )
{
	this->Superclass::PrintSelf( os, indent );
}



void
vtkPlusOpenIGTLinkServer
::SetDataCollector( vtkDataCollector* dataCollector )
{
  this->DataCollector = dataCollector;
}



int
vtkPlusOpenIGTLinkServer
::Initialize( std::string &strError )
{
  if (    this->DataCollector == NULL
       || this->DataCollector->GetTracker() == NULL )
    {
    LOG_ERROR( "Tried to initialize vtkPlusOpenIGTLinkServer without DataCollector." );
    return 1;
    }
  
  return 0;
}



PlusStatus
vtkPlusOpenIGTLinkServer
::Start()
{
  
  return PLUS_SUCCESS;
}



PlusStatus
vtkPlusOpenIGTLinkServer
::Stop()
{
  
  return PLUS_SUCCESS;
}



/**
 * Protected constructor.
 */
vtkPlusOpenIGTLinkServer
::vtkPlusOpenIGTLinkServer()
{
  this->DataCollector = NULL;
}



/**
 * Destructor.
 */
vtkPlusOpenIGTLinkServer
::~vtkPlusOpenIGTLinkServer()
{
  
}
