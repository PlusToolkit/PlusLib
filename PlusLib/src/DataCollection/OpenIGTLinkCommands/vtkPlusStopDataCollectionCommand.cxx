/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/ 

#include "PlusConfigure.h"

#include "vtkPlusStopDataCollectionCommand.h"
#include "igtlStringMessage1.h"

vtkCxxRevisionMacro( vtkPlusStopDataCollectionCommand, "$Revision: 1.0 $" );
vtkStandardNewMacro( vtkPlusStopDataCollectionCommand ); 

//----------------------------------------------------------------------------
void vtkPlusStopDataCollectionCommand::PrintSelf( ostream& os, vtkIndent indent )
{
	this->Superclass::PrintSelf( os, indent );
}

//----------------------------------------------------------------------------
bool vtkPlusStopDataCollectionCommand::CanExecute( std::string str )
{
  if ( str.compare( this->StringRepresentation ) == 0 )
    {
    return true;
    }
  return false;
}

//----------------------------------------------------------------------------
bool vtkPlusStopDataCollectionCommand::Execute()
{
  PlusStatus status = this->DataCollector->Stop();
  if ( status == PLUS_FAIL )
    {
    LOG_WARNING( "Data collector could not be stopped." );
    return false;
    }
  return true;
}

//----------------------------------------------------------------------------
std::string vtkPlusStopDataCollectionCommand::GetStringRepresentation()
{
  return this->StringRepresentation;
}

//----------------------------------------------------------------------------
vtkPlusStopDataCollectionCommand::vtkPlusStopDataCollectionCommand()
{
  this->StringRepresentation = "StopDataCollection";
  this->AddSupportedCommandString( this->StringRepresentation );
}

//----------------------------------------------------------------------------
vtkPlusStopDataCollectionCommand::~vtkPlusStopDataCollectionCommand()
{
}
