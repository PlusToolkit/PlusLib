/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

#include "PlusConfigure.h"
#include "vtkDataCollector.h"
#include "vtkPlusRequestDeviceIDsCommand.h"

vtkStandardNewMacro( vtkPlusRequestDeviceIDsCommand );

static const char REQUEST_DEVICE_ID_CMD[]="RequestDeviceIDs";

//----------------------------------------------------------------------------
vtkPlusRequestDeviceIDsCommand::vtkPlusRequestDeviceIDsCommand()
{
}

//----------------------------------------------------------------------------
vtkPlusRequestDeviceIDsCommand::~vtkPlusRequestDeviceIDsCommand()
{

}

//----------------------------------------------------------------------------
void vtkPlusRequestDeviceIDsCommand::SetNameToRequestDeviceIDs() { SetName(REQUEST_DEVICE_ID_CMD); }

//----------------------------------------------------------------------------
void vtkPlusRequestDeviceIDsCommand::GetCommandNames(std::list<std::string> &cmdNames)
{ 
  cmdNames.clear(); 
  cmdNames.push_back(REQUEST_DEVICE_ID_CMD);
}

//----------------------------------------------------------------------------
std::string vtkPlusRequestDeviceIDsCommand::GetDescription(const char* commandName)
{ 
  std::string desc;
  if (commandName == NULL || STRCASECMP(commandName, REQUEST_DEVICE_ID_CMD) )
  {
    desc += REQUEST_DEVICE_ID_CMD;
    desc += ": Request the list of device IDs presently available.";
  }
  return desc;
}

//----------------------------------------------------------------------------
void vtkPlusRequestDeviceIDsCommand::PrintSelf( ostream& os, vtkIndent indent )
{
  this->Superclass::PrintSelf( os, indent );
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusRequestDeviceIDsCommand::Execute()
{
  if (this->Name==NULL)
  {
    LOG_ERROR("Command failed, no command name specified");
    SetCommandCompleted(PLUS_FAIL, "Command failed, no command name specified");
    return PLUS_FAIL;
  }

  vtkDataCollector* dataCollector = GetDataCollector();
  if (dataCollector == NULL)
  {
    LOG_ERROR("Data collector is invalid");    
    return PLUS_FAIL;
  }  

  PlusStatus status = PLUS_SUCCESS;
  std::stringstream reply;
  reply << this->Name << " ";

  if (STRCASECMP(this->Name, REQUEST_DEVICE_ID_CMD) == 0)
  {    
    DeviceCollection aCollection;
    if( dataCollector->GetDevices(aCollection) == PLUS_SUCCESS )
    {
      for( DeviceCollectionIterator it = aCollection.begin(); it != aCollection.end(); ++it )
      {
        vtkPlusDevice* aDevice = *it;
        reply << aDevice->GetDeviceId();
        if (it != (aCollection.end() - 1) )
        {
          reply << ",";
        }
      }
    }
    else
    {
      LOG_ERROR("Unable to retrieve device list.");
      reply << "Unable to retrieve device list, failed.";
      status = PLUS_FAIL;
    }    
  }
  else
  {
    reply << "Unknown command, failed.";
    SetCommandCompleted(PLUS_FAIL, reply.str());
  }

  SetCommandCompleted(status, reply.str());
  return status;
}
