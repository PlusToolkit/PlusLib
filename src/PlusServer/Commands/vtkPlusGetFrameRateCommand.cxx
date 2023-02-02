/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"
#include "vtkPlusGetFrameRateCommand.h"

#include "vtkPlusDataCollector.h"
#include "vtkPlusChannel.h"
#include "vtkPlusDataSource.h"

vtkStandardNewMacro(vtkPlusGetFrameRateCommand);

namespace
{
  static const std::string GET_FRAME_RATE_CMD = "GetFrameRate";
}

//----------------------------------------------------------------------------
vtkPlusGetFrameRateCommand::vtkPlusGetFrameRateCommand()
  : ResponseExpected(true)
{
  // It handles only one command, set its name by default
  this->SetName(GET_FRAME_RATE_CMD);
}

//----------------------------------------------------------------------------
vtkPlusGetFrameRateCommand::~vtkPlusGetFrameRateCommand()
{
}

//----------------------------------------------------------------------------
void vtkPlusGetFrameRateCommand::SetNameToGetFrameRate()
{
  this->SetName(GET_FRAME_RATE_CMD);
}

//----------------------------------------------------------------------------
void vtkPlusGetFrameRateCommand::GetCommandNames(std::list<std::string>& cmdNames)
{
  cmdNames.clear();
  cmdNames.push_back(GET_FRAME_RATE_CMD);
}

//----------------------------------------------------------------------------
std::string vtkPlusGetFrameRateCommand::GetDescription(const std::string& commandName)
{
  std::string desc;
  if (commandName.empty() || igsioCommon::IsEqualInsensitive(commandName, GET_FRAME_RATE_CMD))
  {
    desc += GET_FRAME_RATE_CMD;
    desc += ": Get frame rate from a data channel.";
  }
  return desc;
}

//----------------------------------------------------------------------------
std::string vtkPlusGetFrameRateCommand::GetChannelId() const
{
  return this->ChannelId;
}

//----------------------------------------------------------------------------
void vtkPlusGetFrameRateCommand::SetChannelId(const std::string& channelId)
{
  this->ChannelId = channelId;
}

//----------------------------------------------------------------------------
void vtkPlusGetFrameRateCommand::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusGetFrameRateCommand::ReadConfiguration(vtkXMLDataElement* aConfig)
{
  if (vtkPlusCommand::ReadConfiguration(aConfig) != PLUS_SUCCESS)
  {
    return PLUS_FAIL;
  }
  XML_READ_STRING_ATTRIBUTE_OPTIONAL(ChannelId, aConfig);
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusGetFrameRateCommand::WriteConfiguration(vtkXMLDataElement* aConfig)
{
  if (vtkPlusCommand::WriteConfiguration(aConfig) != PLUS_SUCCESS)
  {
    return PLUS_FAIL;
  }
  XML_WRITE_STRING_ATTRIBUTE_IF_NOT_EMPTY(ChannelId, aConfig);
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusGetFrameRateCommand::Execute()
{
  LOG_DEBUG("vtkPlusGetFrameRateCommand::Execute: " << (!this->Name.empty() ? this->Name : "(undefined)")
            << ", channel: " << (this->ChannelId.empty() ? "(undefined)" : this->ChannelId));

  vtkPlusDataCollector* dataCollector = GetDataCollector();
  if (dataCollector == NULL)
  {
    this->QueueCommandResponse(PLUS_FAIL, "Command failed. See error message.", "Invalid data collector.");
    return PLUS_FAIL;
  }

  // Get device pointer
  if (this->ChannelId.empty())
  {
    this->QueueCommandResponse(PLUS_FAIL, "Command failed. See error message.", "No ChannelId specified.");
    return PLUS_FAIL;
  }
  vtkPlusChannel* channel = NULL;
  if (dataCollector->GetChannel(channel, this->ChannelId) != PLUS_SUCCESS)
  {
    this->QueueCommandResponse(PLUS_FAIL, "Command failed. See error message.", std::string("Channel ")
                               + (this->ChannelId.empty() ? "(undefined)" : this->ChannelId) + std::string(" is not found."));
    return PLUS_FAIL;
  }

  // Get frame rate from corresponding video source
  vtkPlusDataSource* dataSource = NULL;
  channel->GetVideoSource(dataSource);
  double stddev;
  double framerate = dataSource->GetFrameRate(false, &stddev);

  std::map < std::string, std::pair<IANA_ENCODING_TYPE, std::string> > metaData;

  std::stringstream response;
  std::string error = "";
  response << "<CommandReply FrameRate=" << std::to_string(framerate) << " FrameRateStdDev=" << std::to_string(stddev) << " />";
  metaData["FrameRate"] = std::make_pair(IANA_TYPE_US_ASCII, std::to_string(framerate));
  metaData["FrameRateStdDev"] = std::make_pair(IANA_TYPE_US_ASCII, std::to_string(stddev));

  vtkSmartPointer<vtkPlusCommandRTSCommandResponse> commandResponse = vtkSmartPointer<vtkPlusCommandRTSCommandResponse>::New();
  commandResponse->UseDefaultFormatOff();
  commandResponse->SetClientId(this->ClientId);
  commandResponse->SetOriginalId(this->Id);
  commandResponse->SetCommandName(this->GetName());
  commandResponse->SetStatus(PLUS_SUCCESS);
  commandResponse->SetRespondWithCommandMessage(this->RespondWithCommandMessage);
  commandResponse->SetErrorString(error);
  commandResponse->SetResultString(response.str());
  commandResponse->SetParameters(metaData);
  this->CommandResponseQueue.push_back(commandResponse);

  return PLUS_SUCCESS;
}
