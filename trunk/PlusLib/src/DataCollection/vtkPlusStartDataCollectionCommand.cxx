
#include "vtkPlusStartDataCollectionCommand.h"

#include "igtlStringMessage1.h"



vtkCxxRevisionMacro( vtkPlusStartDataCollectionCommand, "$Revision: 1.0 $" );

vtkStandardNewMacro( vtkPlusStartDataCollectionCommand ); 



void
vtkPlusStartDataCollectionCommand
::PrintSelf( ostream& os, vtkIndent indent )
{
	this->Superclass::PrintSelf( os, indent );
}


bool
vtkPlusStartDataCollectionCommand
::CanExecute( std::string str )
{
  if ( str.compare( this->StringRepresentation ) == 0 )
    {
    return true;
    }
  return false;
}

  

bool
vtkPlusStartDataCollectionCommand
::Execute()
{
  PlusStatus status = this->DataCollector->Start();
  if ( status == PLUS_FAIL )
    {
    LOG_WARNING( "Data collector could not be started." );
    return false;
    }
  return true;
}



std::string
vtkPlusStartDataCollectionCommand
::GetStringRepresentation()
{
  return this->StringRepresentation;
}



void
vtkPlusStartDataCollectionCommand
::ProcessResponse( igtl::MessageBase::Pointer header, igtl::ClientSocket::Pointer clientSocket )
{
  
    // Check if response is the expected type.
  
  if ( strcmp( header->GetDeviceType(), "STRING1" ) != 0 )
    {
    LOG_WARNING( "Response from server is not STRING1" );
    clientSocket->Skip( header->GetBodySizeToRead() );
    return;
    }
  
  igtl::StringMessage1::Pointer strResponse = igtl::StringMessage1::New();
  strResponse->SetMessageHeader( header );
  strResponse->AllocatePack();
  clientSocket->Receive( strResponse->GetPackBodyPointer(), strResponse->GetPackBodySize() );
  strResponse->Unpack();
  
  std::cout << "Message: " << strResponse->GetString() << std::endl;
  
  //TODO: Not finished.
}



/**
 * Constructor.
 */
vtkPlusStartDataCollectionCommand
::vtkPlusStartDataCollectionCommand()
{
  this->StringRepresentation = "StartDataCollection";
  this->AddSupportedCommandString( this->StringRepresentation );
}



/**
 * Destructor.
 */
vtkPlusStartDataCollectionCommand
::~vtkPlusStartDataCollectionCommand()
{
  
}

