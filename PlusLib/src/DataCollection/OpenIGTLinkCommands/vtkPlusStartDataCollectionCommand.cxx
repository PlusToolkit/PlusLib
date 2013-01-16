/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

#include "PlusConfigure.h"

#include "vtkPlusCommandProcessor.h"
#include "vtkPlusStartDataCollectionCommand.h"

vtkStandardNewMacro( vtkPlusStartDataCollectionCommand );

//----------------------------------------------------------------------------
vtkPlusStartDataCollectionCommand::vtkPlusStartDataCollectionCommand()
{
}

//----------------------------------------------------------------------------
vtkPlusStartDataCollectionCommand::~vtkPlusStartDataCollectionCommand()
{
}

//----------------------------------------------------------------------------
void vtkPlusStartDataCollectionCommand::PrintSelf( ostream& os, vtkIndent indent )
{
  this->Superclass::PrintSelf( os, indent );
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusStartDataCollectionCommand::Execute()
{
//  PlusStatus status = this->CommandProcessor->GetDataCollector()->Start();
    PlusStatus status=PLUS_SUCCESS;
  if ( status == PLUS_FAIL )
  {
    LOG_INFO("vtkPlusStartDataCollectionCommand::Execute: Data collector could not be started");
    SetCommandCompleted(PLUS_FAIL,"Data collector could not be started");
  }
  else
  {
    LOG_INFO("vtkPlusStartDataCollectionCommand::Execute: Data collector started");
    SetCommandCompleted(PLUS_SUCCESS,"Data collector started");
  }
  return PLUS_SUCCESS;
}
