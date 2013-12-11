/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

#include "PlusConfigure.h"
#include "vtkDataCollector.h"
#include "vtkPlusChannel.h"
#include "vtkPlusRequestIdsCommand.h"

vtkStandardNewMacro( vtkPlusRequestIdsCommand );

static const char REQUEST_CHANNEL_ID_CMD[]="RequestChannelIds";
static const char REQUEST_DEVICE_ID_CMD[]="RequestDeviceIds";

//----------------------------------------------------------------------------
vtkPlusRequestIdsCommand::vtkPlusRequestIdsCommand()
: DeviceType(NULL)
{
}

//----------------------------------------------------------------------------
vtkPlusRequestIdsCommand::~vtkPlusRequestIdsCommand()
{

}

//----------------------------------------------------------------------------
void vtkPlusRequestIdsCommand::SetNameToRequestChannelIds() { SetName(REQUEST_CHANNEL_ID_CMD); }
void vtkPlusRequestIdsCommand::SetNameToRequestDeviceIds() { SetName(REQUEST_DEVICE_ID_CMD); }

//----------------------------------------------------------------------------
void vtkPlusRequestIdsCommand::GetCommandNames(std::list<std::string> &cmdNames)
{ 
  cmdNames.clear(); 
  cmdNames.push_back(REQUEST_CHANNEL_ID_CMD);
  cmdNames.push_back(REQUEST_DEVICE_ID_CMD);
}

//----------------------------------------------------------------------------
std::string vtkPlusRequestIdsCommand::GetDescription(const char* commandName)
{ 
  std::string desc;
  if (commandName == NULL || STRCASECMP(commandName, REQUEST_CHANNEL_ID_CMD) )
  {
    desc += REQUEST_CHANNEL_ID_CMD;
    desc += ": Request the list of channels for all devices.";
  }
  if (commandName == NULL || STRCASECMP(commandName, REQUEST_DEVICE_ID_CMD) )
  {
    desc += REQUEST_DEVICE_ID_CMD;
    desc += ": Request the list of devices. Attributes: DeviceType: restrict the returned list of devices to a specific type (VirtualDiscCapture, VirtualVolumeReconstructor, etc.)";
  }
  return desc;
}

//----------------------------------------------------------------------------
void vtkPlusRequestIdsCommand::PrintSelf( ostream& os, vtkIndent indent )
{
  this->Superclass::PrintSelf( os, indent );
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusRequestIdsCommand::ReadConfiguration(vtkXMLDataElement* aConfig)
{  
  if (vtkPlusCommand::ReadConfiguration(aConfig) != PLUS_SUCCESS)
  {
    return PLUS_FAIL;
  }

  this->SetDeviceType(aConfig->GetAttribute("DeviceType"));
 
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusRequestIdsCommand::WriteConfiguration(vtkXMLDataElement* aConfig)
{  
  if (vtkPlusCommand::WriteConfiguration(aConfig) != PLUS_SUCCESS)
  {
    return PLUS_FAIL;
  }

  if (this->DeviceType!=NULL)
  {
    aConfig->SetAttribute("DeviceType",this->DeviceType);
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusRequestIdsCommand::Execute()
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

  bool addSeparator=false;
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
          if (addSeparator)
          {
            reply << ",";
          }
          reply << aChannel->GetChannelId();
          addSeparator=true;
        }
      }
    }
  }
  else if (STRCASECMP(this->Name, REQUEST_DEVICE_ID_CMD) == 0)
  {
    for( DeviceCollectionConstIterator deviceIt = aCollection.begin(); deviceIt != aCollection.end(); ++deviceIt)
    {
      vtkPlusDevice* aDevice = *deviceIt;
      if (aDevice==NULL)
      {
        continue;
      }
      std::string deviceClassName;
      if (this->DeviceType!=NULL)
      {
        deviceClassName=std::string("vtk")+this->DeviceType;
      }
      if (deviceClassName.empty() || aDevice->GetClassName()==deviceClassName)
      {
        if (addSeparator)
        {
          reply << ",";
        }
        reply << aDevice->GetDeviceId();
        addSeparator=true;
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
