/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

#include "PlusConfigure.h"

#include "vtkPlusCommandProcessor.h"
#include "vtkPlusConoProbeLinkCommand.h"
#include "vtkOptimetConoProbeMeasurer.h"
#include "vtkDataCollector.h"

vtkStandardNewMacro( vtkPlusConoProbeLinkCommand );

static const char SHOW_CMD[] = "ShowProbeDialog";

//----------------------------------------------------------------------------
vtkPlusConoProbeLinkCommand::vtkPlusConoProbeLinkCommand()
: ConoProbeDeviceId(NULL)
{
}

//----------------------------------------------------------------------------
vtkPlusConoProbeLinkCommand::~vtkPlusConoProbeLinkCommand()
{
  SetConoProbeDeviceId(NULL);
}

//----------------------------------------------------------------------------
void vtkPlusConoProbeLinkCommand::SetNameToShow() { SetName(SHOW_CMD); }

//----------------------------------------------------------------------------
void vtkPlusConoProbeLinkCommand::GetCommandNames(std::list<std::string> &cmdNames)
{ 
  cmdNames.clear(); 
  cmdNames.push_back(SHOW_CMD);
}

//----------------------------------------------------------------------------
std::string vtkPlusConoProbeLinkCommand::GetDescription(const char* commandName)
{ 
  std::string desc;
  if (commandName==NULL || STRCASECMP(commandName, SHOW_CMD))
  {
    desc+=SHOW_CMD;
    desc+=": Opens the Probe Dialog part of the Optimet Smart32 SDK. ConoProbeDeviceId: ID of the ConoProbe device.";
  }
  return desc;
}

//----------------------------------------------------------------------------
void vtkPlusConoProbeLinkCommand::PrintSelf( ostream& os, vtkIndent indent )
{
  this->Superclass::PrintSelf( os, indent );
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusConoProbeLinkCommand::ReadConfiguration(vtkXMLDataElement* aConfig)
{  
  if (vtkPlusCommand::ReadConfiguration(aConfig) != PLUS_SUCCESS)
  {
    return PLUS_FAIL;
  }

  SetConoProbeDeviceId(aConfig->GetAttribute("ConoProbeDeviceId"));

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusConoProbeLinkCommand::WriteConfiguration(vtkXMLDataElement* aConfig)
{  
  if (vtkPlusCommand::WriteConfiguration(aConfig)!=PLUS_SUCCESS)
  {
    return PLUS_FAIL;
  }  
  
  XML_WRITE_STRING_ATTRIBUTE_REMOVE_IF_NULL(ConoProbeDeviceId, aConfig);

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
vtkOptimetConoProbeMeasurer* vtkPlusConoProbeLinkCommand::GetConoProbeDevice(const char* conoProbeDeviceID)
{
  vtkDataCollector* dataCollector=GetDataCollector();
  if (dataCollector==NULL)
  {
    LOG_ERROR("Data collector is invalid");    
    return NULL;
  }
  vtkOptimetConoProbeMeasurer* conoProbeDevice = NULL;
  if (conoProbeDeviceID!=NULL)
  {
    // ConoProbe device ID is specified
    vtkPlusDevice* device=NULL;
    if (dataCollector->GetDevice(device, conoProbeDeviceID)!=PLUS_SUCCESS)
    {
      LOG_ERROR("No OptimetConoProbe has been found by the name "<<conoProbeDeviceID);
      return NULL;
    }
    // device found
    conoProbeDevice = vtkOptimetConoProbeMeasurer::SafeDownCast(device);
    if (conoProbeDevice==NULL)
    {
      // wrong type
      LOG_ERROR("The specified device "<<conoProbeDeviceID<<" is not VirtualStreamCapture");
      return NULL;
    }
  }
  else
  {
    // No ConoProbe device id is specified, auto-detect the first one and use that
    for( DeviceCollectionConstIterator it = dataCollector->GetDeviceConstIteratorBegin(); it != dataCollector->GetDeviceConstIteratorEnd(); ++it )
    {
      conoProbeDevice = vtkOptimetConoProbeMeasurer::SafeDownCast(*it);
      if (conoProbeDevice!=NULL)
      {      
        // found a recording device
        break;
      }
    }
    if (conoProbeDevice==NULL)
    {
      LOG_ERROR("No OptimetConoProbe has been found");
      return NULL;
    }
  }  
  return conoProbeDevice;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusConoProbeLinkCommand::Execute()
{
  LOG_INFO("vtkPlusConoProbeLinkCommand::Execute:");

  if (this->Name==NULL)
  {
    this->QueueStringResponse("Command failed, no command name specified",PLUS_FAIL);
    return PLUS_FAIL;
  }

  vtkOptimetConoProbeMeasurer *conoProbeDevice=GetConoProbeDevice(this->ConoProbeDeviceId);
  if (conoProbeDevice==NULL)
  {
    this->QueueStringResponse(std::string("OptimetConoProbe has not been found (")
      + (this->ConoProbeDeviceId ? this->ConoProbeDeviceId : "auto-detect") + "), "+this->Name+" failed", PLUS_FAIL);
    return PLUS_FAIL;
  }    

  std::string responseMessageBase = std::string("OptimetConoProbe (")+conoProbeDevice->GetDeviceId()+") "+this->Name;
  LOG_INFO("vtkPlusConoProbeLinkCommand::Execute: "<<this->Name);
  if (STRCASECMP(this->Name, SHOW_CMD)==0)
  {
    if (conoProbeDevice->ShowProbeDialog())
    {
		this->QueueStringResponse(responseMessageBase + " successful", PLUS_SUCCESS);
      return PLUS_SUCCESS;
    }
    this->QueueStringResponse(responseMessageBase + " failed", PLUS_FAIL);
    return PLUS_FAIL;
  }

  this->QueueStringResponse(responseMessageBase + " unknown command: " + this->Name,PLUS_FAIL);
  return PLUS_FAIL;
}
