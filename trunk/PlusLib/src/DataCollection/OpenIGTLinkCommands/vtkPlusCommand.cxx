/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/ 

#include "PlusConfigure.h"
#include "vtkPlusCommand.h"
#include "vtkPlusCommandProcessor.h"
#include "vtkVersion.h"

//----------------------------------------------------------------------------
vtkPlusCommand::vtkPlusCommand()
: Completed(false)
, CommandProcessor(NULL)
, ClientId(0)
{
}

//----------------------------------------------------------------------------
vtkPlusCommand::~vtkPlusCommand()
{  
}

//----------------------------------------------------------------------------
void vtkPlusCommand::PrintSelf( ostream& os, vtkIndent indent )
{
	this->Superclass::PrintSelf( os, indent );
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusCommand::ReadConfiguration(vtkXMLDataElement* aConfig)
{  
  if (aConfig==NULL)
  {
    LOG_ERROR("vtkPlusCommand::ReadConfiguration failed, input is NULL");
    return PLUS_FAIL;
  }
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusCommand::WriteConfiguration(vtkXMLDataElement* aConfig)
{  
  if (aConfig==NULL)
  {
    LOG_ERROR("vtkPlusCommand::WriteConfiguration failed, input is NULL");
    return PLUS_FAIL;
  }
  aConfig->SetName("Command");
  std::list<std::string> cmdNames;
  GetCommandNames(cmdNames);
  if (!cmdNames.empty())
  {
    aConfig->SetAttribute("Name",cmdNames.front().c_str());
  }
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
void vtkPlusCommand::SetCommandProcessor( vtkPlusCommandProcessor *processor )
{  
  this->CommandProcessor=processor;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusCommand::SetCommandCompleted(PlusStatus replyStatus, const std::string& replyString)
{
  if (this->CommandProcessor==NULL)
  {
    LOG_ERROR("vtkPlusCommand::SetCommandCompleted failed, command processor is invalid");
    return PLUS_FAIL;
  }
  else
  {
    this->CommandProcessor->QueueReply(this->ClientId, replyStatus, replyString);
  }
  this->Completed=true;
  return PLUS_FAIL;
}

//----------------------------------------------------------------------------
bool vtkPlusCommand::IsCompleted()
{  
  return this->Completed;
}

//----------------------------------------------------------------------------
void vtkPlusCommand::SetClientId(int clientId)
{  
  this->ClientId=clientId;
}
