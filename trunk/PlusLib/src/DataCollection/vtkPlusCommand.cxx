
#include "vtkPlusCommand.h"



vtkCxxRevisionMacro( vtkPlusCommand, "$Revision: 1.0 $" );



void
vtkPlusCommand
::PrintSelf( ostream& os, vtkIndent indent )
{
	this->Superclass::PrintSelf( os, indent );
}



const vtkPlusCommand::CommandStringsType
vtkPlusCommand
::GetSupportedCommandStrings() const
{
  return this->CommandStrings;
}



/**
 * Constructor.
 */
vtkPlusCommand
::vtkPlusCommand()
{
  this->DataCollector = NULL;
  
  this->StringRepresentation = "";
}



/**
 * Destructor.
 */
vtkPlusCommand
::~vtkPlusCommand()
{
  
}



void
vtkPlusCommand
::AddSupportedCommandString( std::string commandString )
{
  this->CommandStrings.push_back( commandString );
}
