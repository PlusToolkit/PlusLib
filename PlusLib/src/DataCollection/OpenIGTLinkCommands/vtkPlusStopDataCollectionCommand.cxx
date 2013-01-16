/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

#include "PlusConfigure.h"

#include "vtkPlusCommandProcessor.h"
#include "vtkPlusStopDataCollectionCommand.h"

vtkStandardNewMacro( vtkPlusStopDataCollectionCommand );

//----------------------------------------------------------------------------
vtkPlusStopDataCollectionCommand::vtkPlusStopDataCollectionCommand()
{
}

//----------------------------------------------------------------------------
vtkPlusStopDataCollectionCommand::~vtkPlusStopDataCollectionCommand()
{
}

//----------------------------------------------------------------------------
void vtkPlusStopDataCollectionCommand::PrintSelf( ostream& os, vtkIndent indent )
{
  this->Superclass::PrintSelf( os, indent );
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusStopDataCollectionCommand::Execute()
{
  //PlusStatus status = this->CommandProcessor->GetDataCollector()->Stop();
  PlusStatus status=PLUS_SUCCESS;
  if ( status == PLUS_FAIL )
  {
    LOG_INFO("vtkPlusStopDataCollectionCommand::Execute: Data collector could not be stopped");
    SetCommandCompleted(PLUS_FAIL,"Data collector could not be stopped");
  }
  else
  {
    LOG_INFO("vtkPlusStopDataCollectionCommand::Execute: Data collector stopped");
    SetCommandCompleted(PLUS_SUCCESS,"Data collector stopped");
  }
  return PLUS_SUCCESS;
}
