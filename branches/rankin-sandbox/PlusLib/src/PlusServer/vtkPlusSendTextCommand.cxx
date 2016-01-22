/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

#include "PlusConfigure.h"
#include "vtkPlusSendTextCommand.h"

#include "vtkDataCollector.h"

vtkStandardNewMacro( vtkPlusSendTextCommand );

static const char SEND_TEXT_CMD[] = "SendText";

//----------------------------------------------------------------------------
vtkPlusSendTextCommand::vtkPlusSendTextCommand()
: DeviceId(NULL)
, Text(NULL)
, ResponseText(NULL)
, ResponseExpected(true)
{
  // It handles only one command, set its name by default
  this->SetName(SEND_TEXT_CMD);
}

//----------------------------------------------------------------------------
vtkPlusSendTextCommand::~vtkPlusSendTextCommand()
{
  this->SetDeviceId(NULL);
  this->SetText(NULL);
  this->SetResponseText(NULL);
}

//----------------------------------------------------------------------------
void vtkPlusSendTextCommand::SetNameToSendText()
{ 
  this->SetName(SEND_TEXT_CMD);
}

//----------------------------------------------------------------------------
void vtkPlusSendTextCommand::GetCommandNames(std::list<std::string>& cmdNames)
{ 
  cmdNames.clear(); 
  cmdNames.push_back(SEND_TEXT_CMD);
}

//----------------------------------------------------------------------------
std::string vtkPlusSendTextCommand::GetDescription(const char* commandName)
{ 
  std::string desc;
  if (commandName == NULL || STRCASECMP(commandName, SEND_TEXT_CMD))
  {
    desc += SEND_TEXT_CMD;
    desc += ": Send text data to the device.";
  }
  return desc;
}

//----------------------------------------------------------------------------
void vtkPlusSendTextCommand::PrintSelf( ostream& os, vtkIndent indent )
{
  this->Superclass::PrintSelf( os, indent );
  os << indent << "Text: " << this->Text;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusSendTextCommand::ReadConfiguration(vtkXMLDataElement* aConfig)
{  
  if (vtkPlusCommand::ReadConfiguration(aConfig)!=PLUS_SUCCESS)
  {
    return PLUS_FAIL;
  }
  XML_READ_STRING_ATTRIBUTE_OPTIONAL(DeviceId, aConfig);
  XML_READ_STRING_ATTRIBUTE_OPTIONAL(Text, aConfig);
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusSendTextCommand::WriteConfiguration(vtkXMLDataElement* aConfig)
{  
  if (vtkPlusCommand::WriteConfiguration(aConfig)!=PLUS_SUCCESS)
  {
    return PLUS_FAIL;
  }  
  XML_WRITE_STRING_ATTRIBUTE_IF_NOT_NULL(DeviceId, aConfig);
  XML_WRITE_STRING_ATTRIBUTE_IF_NOT_NULL(Text, aConfig);
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusSendTextCommand::Execute()
{
  LOG_DEBUG("vtkPlusSendTextCommand::Execute: "<<(this->Name?this->Name:"(undefined)")
    <<", device: "<<(this->DeviceId==NULL?"(undefined)":this->DeviceId)
    <<", text: "<<(this->Text==NULL?"(undefined)":this->Text) );

  vtkDataCollector* dataCollector=GetDataCollector();
  if (dataCollector==NULL)
  {
    this->QueueStringResponse("vtkPlusSendTextCommand command failed, invalid data collector",PLUS_FAIL);
    return PLUS_FAIL;
  }

  // Get device pointer
  if (this->DeviceId==NULL)
  {
    this->QueueStringResponse("vtkPlusSendTextCommand command failed, no DeviceId specified",PLUS_FAIL);
    return PLUS_FAIL;
  }
  vtkPlusDevice* device=NULL;
  if (dataCollector->GetDevice(device, this->DeviceId)!=PLUS_SUCCESS)
  {
    this->QueueStringResponse(std::string("vtkPlusSendTextCommand failed: device ")
      +(this->DeviceId==NULL?"(undefined)":this->DeviceId)+" is not found",PLUS_FAIL);
      return PLUS_FAIL;
  }

  // Send text (and receive response)
  std::string textToSend;
  std::string response;
  if (GetText()!=NULL)
  {
    textToSend = GetText();
  }
  PlusStatus status=device->SendText(GetText(), GetResponseExpected() ? &response : NULL );
  SetResponseText(response.c_str());
  if (status!=PLUS_SUCCESS)
  {
    this->QueueStringResponse(std::string("vtkPlusSendTextCommand failed: failed to send text '") + GetText() + "'"
      + " to device " +(this->DeviceId==NULL?"(undefined)":this->DeviceId),PLUS_FAIL);
      return PLUS_FAIL;
  }
  this->QueueStringResponse(response,PLUS_SUCCESS);
  return PLUS_SUCCESS;
}
