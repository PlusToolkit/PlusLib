/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

#include "PlusConfigure.h"
#include "vtkDataCollector.h"
#include "vtkPlusChannel.h"
#include "vtkPlusRequestChannelIDsCommand.h"

vtkStandardNewMacro( vtkPlusRequestChannelIDsCommand );

static const char REQUEST_CHANNEL_ID_CMD[]="RequestChannelIDs";

//----------------------------------------------------------------------------
vtkPlusRequestChannelIDsCommand::vtkPlusRequestChannelIDsCommand()
{
}

//----------------------------------------------------------------------------
vtkPlusRequestChannelIDsCommand::~vtkPlusRequestChannelIDsCommand()
{

}

//----------------------------------------------------------------------------
void vtkPlusRequestChannelIDsCommand::SetNameToRequestChannelIDs() { SetName(REQUEST_CHANNEL_ID_CMD); }

//----------------------------------------------------------------------------
void vtkPlusRequestChannelIDsCommand::GetCommandNames(std::list<std::string> &cmdNames)
{ 
  cmdNames.clear(); 
  cmdNames.push_back(REQUEST_CHANNEL_ID_CMD);
}

//----------------------------------------------------------------------------
std::string vtkPlusRequestChannelIDsCommand::GetDescription(const char* commandName)
{ 
  std::string desc;
  if (commandName == NULL || STRCASECMP(commandName, REQUEST_CHANNEL_ID_CMD) )
  {
    desc += REQUEST_CHANNEL_ID_CMD;
    desc += ": Request the list of channels for all devices.";
  }
  return desc;
}

//----------------------------------------------------------------------------
void vtkPlusRequestChannelIDsCommand::PrintSelf( ostream& os, vtkIndent indent )
{
  this->Superclass::PrintSelf( os, indent );
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusRequestChannelIDsCommand::ReadConfiguration(vtkXMLDataElement* aConfig)
{  
  if (vtkPlusCommand::ReadConfiguration(aConfig) != PLUS_SUCCESS)
  {
    return PLUS_FAIL;
  }
 
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusRequestChannelIDsCommand::WriteConfiguration(vtkXMLDataElement* aConfig)
{  
  if (vtkPlusCommand::WriteConfiguration(aConfig) != PLUS_SUCCESS)
  {
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusRequestChannelIDsCommand::Execute()
{
  if (this->Name == NULL)
  {
    LOG_ERROR("Command failed, no command name specified");
    SetCommandCompleted(PLUS_FAIL, "Command failed, no command name specified");
    return PLUS_FAIL;
  }

  vtkDataCollector* dataCollector = this->GetDataCollector();
  if (dataCollector == NULL)
  {
    LOG_ERROR("Data collector is invalid");    
    SetCommandCompleted(PLUS_FAIL, "Command failed, no data collector");
    return PLUS_FAIL;
  }  
  DeviceCollection aCollection;
  if( dataCollector->GetDevices(aCollection) != PLUS_SUCCESS )
  {
    LOG_ERROR("Unable to retrieve devices.");
    SetCommandCompleted(PLUS_FAIL, "Unable to retrieve devices.");
    return PLUS_FAIL;
  }

  PlusStatus status = PLUS_SUCCESS;
  std::stringstream reply;
  reply << this->Name << " ";

  if (STRCASECMP(this->Name, REQUEST_CHANNEL_ID_CMD) == 0)
  {
    for( DeviceCollectionConstIterator deviceIt = aCollection.begin(); deviceIt != aCollection.end(); ++deviceIt)
    {
      vtkPlusDevice* aDevice = *deviceIt;
      if( aDevice->OutputChannelCount() > 0 )
      {
        for( ChannelContainerConstIterator it = aDevice->GetOutputChannelsStart(); it != aDevice->GetOutputChannelsEnd(); ++it )
        {
          vtkPlusChannel* aChannel = *it;
          reply << aChannel->GetChannelId();
          if (it != (aDevice->GetOutputChannelsEnd() - 1) )
          {
            reply << ",";
          }
        }

        if( deviceIt != (aCollection.end() - 1 ) )
        {
          reply << ",";
        }
      }
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
