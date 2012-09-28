/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/ 

#include "PlusConfigure.h"
#include "vtkPlusCommand.h"
#include "vtkVersion.h"

vtkCxxRevisionMacro( vtkPlusCommand, "$Revision: 1.0 $" );

//----------------------------------------------------------------------------
void vtkPlusCommand::PrintSelf( ostream& os, vtkIndent indent )
{
	this->Superclass::PrintSelf( os, indent );
}

//----------------------------------------------------------------------------
const vtkPlusCommand::CommandStringsType vtkPlusCommand::GetSupportedCommandStrings() const
{
  return this->CommandStrings;
}

//----------------------------------------------------------------------------
const char* vtkPlusCommand::GetVTKSourceVersion()
{
  return VTK_SOURCE_VERSION;
}

//----------------------------------------------------------------------------
const char* vtkPlusCommand::GetDescription()
{
  return "VTK PlusCommand";
}

//----------------------------------------------------------------------------
vtkPlusCommand::vtkPlusCommand()
{
  this->DataCollector = NULL;
  
  this->StringRepresentation.clear(); 
}

//----------------------------------------------------------------------------
vtkPlusCommand::~vtkPlusCommand()
{
  
}

//----------------------------------------------------------------------------
void vtkPlusCommand::AddSupportedCommandString( std::string commandString )
{
  this->CommandStrings.push_back( commandString );
}
