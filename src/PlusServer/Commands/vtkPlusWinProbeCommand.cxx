/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"
#include "vtkPlusWinProbeCommand.h"
#include "WinProbe/vtkPlusWinProbeVideoSource.h"

#include "vtkPlusDataCollector.h"

vtkStandardNewMacro(vtkPlusWinProbeCommand);

namespace
{
  static const std::string WINPROBE_CMD = "WinProbeCommand";

}

//----------------------------------------------------------------------------
vtkPlusWinProbeCommand::vtkPlusWinProbeCommand()
  : ResponseExpected(true)
{
  this->SetName(WINPROBE_CMD);
  this->RequestedParameters.clear();
}

//----------------------------------------------------------------------------
vtkPlusWinProbeCommand::~vtkPlusWinProbeCommand()
{
  this->RequestedParameters.clear();
}

//----------------------------------------------------------------------------
void vtkPlusWinProbeCommand::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

//----------------------------------------------------------------------------
void vtkPlusWinProbeCommand::SetNameToWinProbeDevice()
{
  this->SetName(WINPROBE_CMD);
}

//----------------------------------------------------------------------------
void vtkPlusWinProbeCommand::GetCommandNames(std::list<std::string>& cmdNames)
{
  cmdNames.clear();
  cmdNames.push_back(WINPROBE_CMD);
}

//----------------------------------------------------------------------------
std::string vtkPlusWinProbeCommand::GetDescription(const std::string& commandName)
{
  std::string desc;
  if (commandName.empty() || igsioCommon::IsEqualInsensitive(commandName, WINPROBE_CMD))
  {
    desc += WINPROBE_CMD;
    desc += ": Send text data to WinProbe device.";
  }
  return desc;
}

//----------------------------------------------------------------------------
std::string vtkPlusWinProbeCommand::GetDeviceId() const
{
  return this->DeviceId;
}

//----------------------------------------------------------------------------
void vtkPlusWinProbeCommand::SetDeviceId(const std::string& deviceId)
{
  this->DeviceId = deviceId;
}

//----------------------------------------------------------------------------
std::string vtkPlusWinProbeCommand::GetCommandName() const
{
  return this->CommandName;
}

//----------------------------------------------------------------------------
void vtkPlusWinProbeCommand::SetCommandName(const std::string& text)
{
  this->CommandName = text;
}

//----------------------------------------------------------------------------
std::string vtkPlusWinProbeCommand::GetCommandValue() const
{
  return this->CommandValue;
}

//----------------------------------------------------------------------------
void vtkPlusWinProbeCommand::SetCommandValue(const std::string& text)
{
    this->CommandValue = text;
}

//----------------------------------------------------------------------------
std::string vtkPlusWinProbeCommand::GetCommandIndex() const
{
  return this->CommandIndex;
}

//----------------------------------------------------------------------------
void vtkPlusWinProbeCommand::SetCommandIndex(const std::string& index)
{
    this->CommandIndex = index;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusWinProbeCommand::ReadConfiguration(vtkXMLDataElement* aConfig)
{
  this->RequestedParameters.clear();
  if (vtkPlusCommand::ReadConfiguration(aConfig) != PLUS_SUCCESS)
  {
      return PLUS_FAIL;
  }

  this->SetDeviceId(aConfig->GetAttribute("DeviceId"));
  LOG_INFO(aConfig->GetNumberOfNestedElements());

  for(int elemIndex = 0; elemIndex < aConfig->GetNumberOfNestedElements(); ++elemIndex)
  {
    vtkXMLDataElement* currentElem = aConfig->GetNestedElement(elemIndex);
    if (igsioCommon::IsEqualInsensitive(currentElem->GetName(), "Parameter"))
    {
      const char* parameterName = currentElem->GetAttribute("Name");
      const char* parameterValue = currentElem->GetAttribute("Value");
      const char* parameterIndex = currentElem->GetAttribute("Index");
      if (!parameterName) // Index and Value arent always needed
      {
        LOG_ERROR("Unable to find required Name attribute in " << (currentElem->GetName() ? currentElem->GetName() : "(undefined)") << " element in SetUsParameter command");
        continue;
      }
      LOG_INFO("Adding " << parameterName << " to execution list")

      std::map<std::string, std::string> param_values;
      if (parameterValue)
      {
        param_values["Value"] = parameterValue;
      }
      if (parameterIndex)
      {
        param_values["Index"] = parameterIndex;
      }
      std::pair<std::string, std::map<std::string, std::string>> parameter;
      parameter.first = parameterName;
      parameter.second = param_values;
      RequestedParameters.push_back(parameter);
    }
  }
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusWinProbeCommand::WriteConfiguration(vtkXMLDataElement* aConfig)
{
  if (vtkPlusCommand::WriteConfiguration(aConfig) != PLUS_SUCCESS)
  {
      return PLUS_FAIL;
  }

  XML_WRITE_STRING_ATTRIBUTE_IF_NOT_EMPTY(DeviceId, aConfig);

  // Write parameters as nested elements
  std::list<std::pair<std::string, std::map<std::string, std::string>>>::iterator paramIt;
  for (paramIt = this->RequestedParameters.begin(); paramIt != this->RequestedParameters.end(); ++paramIt)
  {
    vtkSmartPointer<vtkXMLDataElement> paramElem = vtkSmartPointer<vtkXMLDataElement>::New();
    paramElem->SetName("Parameter");
    paramElem->SetAttribute("Name", paramIt->first.c_str());
    std::map<std::string, std::string> param_attributes = paramIt->second;
    std::map<std::string, std::string>::iterator attribIt;
    for(attribIt = param_attributes.begin(); attribIt != param_attributes.end(); ++attribIt)
    {
      paramElem->SetAttribute(attribIt->first.c_str(), attribIt->second.c_str());
    }
    aConfig->AddNestedElement(paramElem);
  }
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusWinProbeCommand::Execute()
{
  LOG_DEBUG("vtkPlusWinProbeCommand::Execute: " << (!this->CommandName.empty() ? this->CommandName : "(undefined)")
            << ", device: " << (this->DeviceId.empty() ? "(undefined)" : this->DeviceId)
            << ", value: " << (this->CommandValue.empty() ? "(undefined)" : this->CommandValue)
            << ", index: " << (this->CommandIndex.empty() ? "(undefined)" : this->CommandIndex));

  // Data Collector
  vtkPlusDataCollector* dataCollector = GetDataCollector();
  if (dataCollector == NULL)
  {
    this->QueueCommandResponse(PLUS_FAIL, "Command failed. See error message.", "Invalid data collector.");
    return PLUS_FAIL;
  }

  // Get device pointer
  if (this->DeviceId.empty())
  {
    this->QueueCommandResponse(PLUS_FAIL, "Command failed. See error message.", "No DeviceId specified.");
    return PLUS_FAIL;
  }
  vtkPlusDevice* usDevice = NULL;
  if (dataCollector->GetDevice(usDevice, this->DeviceId) != PLUS_SUCCESS)
  {
    this->QueueCommandResponse(PLUS_FAIL, "Command failed. See error message.", std::string("Device ")
                               + (this->DeviceId.empty() ? "(undefined)" : this->DeviceId) + std::string(" is not found."));
    return PLUS_FAIL;
  }
  vtkPlusWinProbeVideoSource* device = dynamic_cast<vtkPlusWinProbeVideoSource*>(usDevice);

  // CommandName
  if (!igsioCommon::IsEqualInsensitive(this->Name, WINPROBE_CMD))
  {
    this->QueueCommandResponse(PLUS_FAIL, "Command failed. See error message.", "Unknown command name: " + this->Name + ".");
    return PLUS_FAIL;
  }

  std::map < std::string, std::pair<IANA_ENCODING_TYPE, std::string> > metaData;
  std::string resultString = "<CommandReply>";
  std::string error = "";
  std::string res = "";
  PlusStatus status = PLUS_SUCCESS;
  bool hasFailure = false;

  std::list<std::pair<std::string, std::map<std::string, std::string>>>::iterator paramIt;
  for (paramIt = this->RequestedParameters.begin(); paramIt != this->RequestedParameters.end(); ++paramIt)
  {
    std::string parameterName = paramIt->first;
    // Map of command parameter and value to be returned via meta data.
    // For Gets, this will return the value and for sets, this will return the success
    // For example: "GetMDepth", "10" or "SetARFITxCycleCount", "SUCCESS"
    std::map<std::string, std::string> attribs = paramIt->second;

    resultString += "<Parameter Name=\"" + parameterName + "\"";

    // Search for command
    PlusStatus status;

    // Assign value and index
    std::string value = attribs["Value"];
    int index;
    try {  // try to cast index to number, but index may not always exist
      index = std::stoi(attribs["Index"]);
    }
    catch (const std::invalid_argument& ia) {
      index = -1;
    }

    // Option 1: Get
    if (igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::GET_FREEZE)
        || igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::GET_TGC)
        || igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::GET_B_MULTIFOCAL_ZONE_COUNT)
        || igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::GET_FOCAL_DEPTH)
        || igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::GET_ARFI_FOCAL_DEPTH)
        || igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::GET_FIRST_GAIN_VALUE)
        || igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::GET_TGC_OVERALL_GAIN)
        || igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::GET_SPATIAL_COMPOUND_ENABLED)
        || igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::GET_SPATIAL_COMPOUND_ANGLE)
        || igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::GET_SPATIAL_COMPOUND_COUNT)
        || igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::GET_MMODE_ENABLED)
        || igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::GET_M_REVOLVING_ENABLED)
        || igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::GET_RF_MODE_ENABLED)
        || igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::GET_MPR_FREQUENCY)
        || igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::GET_M_LINE_INDEX)
        || igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::GET_M_LINE_COUNT)
        || igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::GET_M_WIDTH)
        || igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::GET_M_DEPTH)
        || igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::GET_DECIMATION)
        || igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::GET_B_PRF)
        || igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::GET_B_FRAME_RATE_LIMIT)
        || igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::GET_B_HARMONIC_ENABLED)
        || igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::GET_B_BUBBLE_CONTRAST_ENABLED)
        || igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::GET_B_AMPLITUDE_MODULATION_ENABLED)
        || igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::GET_B_TRANSMIT_LOCKED)
        || igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::GET_B_TRANSMIT_CURRENT)
        || igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::GET_B_TRANSMIT_CYCLE_COUNT)
        || igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::GET_B_TRANSMIT_FNUMBER)
        || igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::GET_B_APODIZATION_FNUMBER)
        || igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::GET_B_BUBBLE_DESTRUCTION_ENABLED)
        || igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::GET_B_BUBBLE_DESTRUCTION_CYCLE_COUNT)
        || igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::GET_B_TX_FILTER_COEFFICIENT_SET)
        || igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::GET_TRANSDUCER_INTERNAL_ID)
        || igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::GET_ARFI_ENABLED)
        || igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::GET_ARFI_START_SAMPLE)
        || igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::GET_ARFI_STOP_SAMPLE)
        || igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::GET_ARFI_PRE_PUSH_LINE_REPEAT_COUNT)
        || igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::GET_ARFI_POST_PUSH_LINE_REPEAT_COUNT)
        || igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::GET_ARFI_INTER_SET_DELAY)
        || igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::GET_ARFI_INTER_PUSH_DELAY)
        || igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::GET_ARFI_LINE_TIMER)
        || igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::GET_ARFI_TX_CYCLE_COUNT)
        || igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::GET_ARFI_TX_CYCLE_WIDTH)
        || igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::GET_ARFI_TX_TX_CYCLE_COUNT)
        || igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::GET_ARFI_TX_TX_CYCLE_WIDTH)
        || igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::GET_ARFI_PUSH_CONFIG)
        || igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::GET_FPGA_REV_DATE_STRING)
        || igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::GET_X8BF_ENABLED))
    {
      if (igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::GET_FREEZE))
        res = device->IsFrozen() ? "True" : "False";
      else if (igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::GET_TGC))
        res = std::to_string(device->GetTimeGainCompensation(index));
      else if (igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::GET_B_MULTIFOCAL_ZONE_COUNT))
        res = std::to_string(device->GetBMultiFocalZoneCount());
      else if (igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::GET_FOCAL_DEPTH))
          res = std::to_string(device->GetFocalPointDepth(index));
      else if (igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::GET_ARFI_FOCAL_DEPTH))
          res = std::to_string(device->GetARFIFocalPointDepth(index));
      else if (igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::GET_FIRST_GAIN_VALUE))
          res = std::to_string(device->GetFirstGainValue());
      else if (igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::GET_TGC_OVERALL_GAIN))
          res = std::to_string(device->GetOverallTimeGainCompensation());
      else if (igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::GET_SPATIAL_COMPOUND_ENABLED))
          res = device->GetSpatialCompoundEnabled() ? "True" : "False";
      else if (igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::GET_SPATIAL_COMPOUND_ANGLE))
          res = std::to_string(device->GetSpatialCompoundAngle());
      else if (igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::GET_SPATIAL_COMPOUND_COUNT))
          res = std::to_string(device->GetSpatialCompoundCount());
      else if (igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::GET_MMODE_ENABLED))
          res = device->GetMModeEnabled() ? "True" : "False";
      else if (igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::GET_M_REVOLVING_ENABLED))
          res = device->GetMRevolvingEnabled() ? "True" : "False";
      else if (igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::GET_RF_MODE_ENABLED))
          res = device->GetBRFEnabled() ? "True" : "False";
      else if (igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::GET_MPR_FREQUENCY))
          res = std::to_string(device->GetMPRFrequency());
      else if (igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::GET_M_LINE_INDEX))
          res = std::to_string(device->GetMLineIndex());
      else if (igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::GET_M_LINE_COUNT))
          res = std::to_string(device->GetMAcousticLineCount());
      else if (igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::GET_M_WIDTH))
          res = std::to_string(device->GetMWidth());
      else if (igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::GET_M_DEPTH))
          res = std::to_string(device->GetMDepth());
      else if (igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::GET_DECIMATION))
          res = std::to_string(device->GetSSDecimation());
      else if (igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::GET_B_FRAME_RATE_LIMIT))
          res = std::to_string(device->GetBFrameRateLimit());
      else if (igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::GET_B_PRF))
          res = std::to_string(device->GetBPRF());
      else if (igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::GET_B_HARMONIC_ENABLED))
          res = device->GetBHarmonicEnabled() ? "True" : "False";
      else if (igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::GET_B_BUBBLE_CONTRAST_ENABLED))
          res = device->GetBBubbleContrastEnabled() ? "True" : "False";
      else if (igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::GET_B_AMPLITUDE_MODULATION_ENABLED))
          res = device->GetBAmplitudeModulationEnabled() ? "True" : "False";
      else if (igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::GET_B_TRANSMIT_LOCKED))
          res = device->GetBTransmitLocked() ? "True" : "False";
      else if (igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::GET_B_TRANSMIT_CURRENT))
          res = std::to_string(device->GetBTransmitCurrent());
      else if (igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::GET_B_TRANSMIT_CYCLE_COUNT))
          res = std::to_string(device->GetBTransmitCycleCount());
      else if (igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::GET_B_TRANSMIT_FNUMBER))
          res = std::to_string(device->GetBTransmitFNumber());
      else if (igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::GET_B_APODIZATION_FNUMBER))
          res = std::to_string(device->GetBApodizationFNumber());
      else if (igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::GET_B_BUBBLE_DESTRUCTION_ENABLED))
          res = device->GetBBubbleDestructionEnabled() ? "True" : "False";
      else if (igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::GET_B_BUBBLE_DESTRUCTION_CYCLE_COUNT))
          res = std::to_string(device->GetBBubbleDestructionCycleCount());
      else if (igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::GET_B_TX_FILTER_COEFFICIENT_SET))
          res = std::to_string(device->GetBTXFilterCoefficientSet());
      else if (igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::GET_TRANSDUCER_INTERNAL_ID))
          res = std::to_string(device->GetTransducerInternalID());
      else if (igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::GET_ARFI_ENABLED))
          res = device->GetARFIEnabled() ? "True" : "False";
      else if (igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::GET_ARFI_START_SAMPLE))
          res = std::to_string(device->GetARFIStartSample());
      else if (igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::GET_ARFI_STOP_SAMPLE))
          res = std::to_string(device->GetARFIStopSample());
      else if (igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::GET_ARFI_PRE_PUSH_LINE_REPEAT_COUNT))
          res = std::to_string(device->GetARFIPrePushLineRepeatCount());
      else if (igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::GET_ARFI_POST_PUSH_LINE_REPEAT_COUNT))
          res = std::to_string(device->GetARFIPostPushLineRepeatCount());
      else if (igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::GET_ARFI_INTER_SET_DELAY))
          res = std::to_string(device->GetARFIInterSetDelay());
      else if (igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::GET_ARFI_INTER_PUSH_DELAY))
          res = std::to_string(device->GetARFIInterPushDelay());
      else if (igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::GET_ARFI_LINE_TIMER))
          res = std::to_string(device->GetARFILineTimer());
      else if (igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::GET_ARFI_TX_CYCLE_COUNT))
          res = std::to_string(device->GetARFITxCycleCount());
      else if (igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::GET_ARFI_TX_CYCLE_WIDTH))
          res = std::to_string(device->GetARFITxCycleWidth());
      else if (igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::GET_ARFI_TX_TX_CYCLE_COUNT))
          res = std::to_string(device->GetARFITxTxCycleCount());
      else if (igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::GET_ARFI_TX_TX_CYCLE_WIDTH))
          res = std::to_string(device->GetARFITxTxCycleWidth());
      else if (igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::GET_ARFI_PUSH_CONFIG))
          res = device->GetARFIPushConfigurationString();
      else if (igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::GET_FPGA_REV_DATE_STRING))
          res = device->GetFPGARevDateString();
      else if (igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::GET_X8BF_ENABLED))
          res = device->GetARFIIsX8BFEnabled() ? "True" : "False";

      if (res != "")
      {
        status = PLUS_SUCCESS;
        resultString += " Success=\"true\"";
        if (index != -1)
          resultString += " Index=\"" + std::to_string(index) + "\"";
        resultString += " Value=\"" + res + "\"";
        metaData[parameterName] = std::make_pair(IANA_TYPE_US_ASCII, res);
      }
      else
      {
        status = PLUS_FAIL;
        resultString += " Success=\"false\"";
      }
    }

    // Option 2: Set
    else if (igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::SET_FREEZE)
             || igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::SET_TGC)
             || igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::SET_B_MULTIFOCAL_ZONE_COUNT)
             || igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::SET_FOCAL_DEPTH)
             || igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::SET_ARFI_FOCAL_DEPTH)
             || igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::SET_FIRST_GAIN_VALUE)
             || igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::SET_TGC_OVERALL_GAIN)
             || igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::SET_SPATIAL_COMPOUND_ENABLED)
             || igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::SET_SPATIAL_COMPOUND_COUNT)
             || igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::SET_MMODE_ENABLED)
             || igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::SET_M_REVOLVING_ENABLED)
             || igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::SET_RF_MODE_ENABLED)
             || igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::SET_MPR_FREQUENCY)
             || igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::SET_M_LINE_COUNT)
             || igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::SET_M_WIDTH)
             || igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::SET_M_DEPTH)
             || igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::SET_DECIMATION)
             || igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::SET_B_FRAME_RATE_LIMIT)
             || igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::SET_B_HARMONIC_ENABLED)
             || igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::SET_B_BUBBLE_CONTRAST_ENABLED)
             || igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::SET_B_AMPLITUDE_MODULATION_ENABLED)
             || igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::SET_B_TRANSMIT_LOCKED)
             || igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::SET_B_TRANSMIT_CURRENT)
             || igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::SET_B_TRANSMIT_CYCLE_COUNT)
             || igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::SET_B_TRANSMIT_FNUMBER)
             || igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::SET_B_APODIZATION_FNUMBER)
             || igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::SET_B_BUBBLE_DESTRUCTION_ENABLED)
             || igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::SET_B_BUBBLE_DESTRUCTION_CYCLE_COUNT)
             || igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::SET_B_TX_FILTER_COEFFICIENT_SET)
             || igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::SET_ARFI_ENABLED)
             || igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::SET_ARFI_START_SAMPLE)
             || igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::SET_ARFI_STOP_SAMPLE)
             || igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::SET_ARFI_PRE_PUSH_LINE_REPEAT_COUNT)
             || igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::SET_ARFI_POST_PUSH_LINE_REPEAT_COUNT)
             || igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::SET_ARFI_INTER_SET_DELAY)
             || igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::SET_ARFI_INTER_PUSH_DELAY)
             || igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::SET_ARFI_LINE_TIMER)
             || igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::SET_ARFI_TX_CYCLE_COUNT)
             || igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::SET_ARFI_TX_CYCLE_WIDTH)
             || igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::SET_ARFI_TX_TX_CYCLE_COUNT)
             || igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::SET_ARFI_TX_TX_CYCLE_WIDTH)
             || igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::SET_ARFI_PUSH_CONFIG))
    {
      if (value == "")
      {
        status = PLUS_FAIL;
        std::string success = "false";
        std::string status_msg = "MISSING INFO";
        error += "Insufficient information for \"" + parameterName + "\"\n";
        resultString += " Success=\"" + success + "\"";
        metaData[parameterName] = std::make_pair(IANA_TYPE_US_ASCII, status_msg);
        continue;
      }

      if (igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::SET_FREEZE))
      {
        bool set = igsioCommon::IsEqualInsensitive(value, "true") ? true : false;
        status = device->FreezeDevice(set);
      }
      else if (igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::SET_TGC))
      {
        double tgc_value = stod(value);
        status = device->SetTimeGainCompensation(index, tgc_value);
      }
      else if (igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::SET_B_MULTIFOCAL_ZONE_COUNT))
      {
        int32_t count = stoi(value);
        status = device->SetBMultiFocalZoneCount(count);
      }
      else if (igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::SET_FOCAL_DEPTH))
      {
        float depth_value = stof(value);
        status = device->SetFocalPointDepth(index, depth_value);
      }
      else if (igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::SET_ARFI_FOCAL_DEPTH))
      {
        float depth_value = stof(value);
        status = device->SetARFIFocalPointDepth(index, depth_value);
      }
      else if (igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::SET_FIRST_GAIN_VALUE))
      {
        double gain_value = stod(value);
        status = device->SetFirstGainValue(gain_value);
      }
      else if (igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::SET_TGC_OVERALL_GAIN))
      {
        double tgc_value = stod(value);
        status = device->SetOverallTimeGainCompensation(tgc_value);
      }
      else if (igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::SET_SPATIAL_COMPOUND_ENABLED))
      {
        bool set = igsioCommon::IsEqualInsensitive(value, "true") ? true : false;
        device->SetSpatialCompoundEnabled(set);
        status = PLUS_SUCCESS;
      }
      else if (igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::SET_SPATIAL_COMPOUND_COUNT))
      {
        int32_t count = stoi(value);
        device->SetSpatialCompoundCount(count);
        status = PLUS_SUCCESS;
      }
      else if (igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::SET_MMODE_ENABLED))
      {
        bool set = igsioCommon::IsEqualInsensitive(value, "true") ? true : false;
        device->SetMModeEnabled(set);
        status = PLUS_SUCCESS;
      }
      else if (igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::SET_M_REVOLVING_ENABLED))
      {
        bool set = igsioCommon::IsEqualInsensitive(value, "true") ? true : false;
        device->SetMRevolvingEnabled(set);
        status = PLUS_SUCCESS;
      }
      else if (igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::SET_RF_MODE_ENABLED))
      {
        bool set = igsioCommon::IsEqualInsensitive(value, "true") ? true : false;
        device->SetBRFEnabled(set);
        status = PLUS_SUCCESS;
      }
      else if (igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::SET_MPR_FREQUENCY))
      {
        int32_t frequency = stoi(value);
        device->SetMPRFrequency(frequency);
        status = PLUS_SUCCESS;
      }
      else if (igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::SET_M_LINE_COUNT))
      {
        int32_t count = stoi(value);
        device->SetMAcousticLineCount(count);
        status = PLUS_SUCCESS;
      }
      else if (igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::SET_M_WIDTH))
      {
        int32_t width = stoi(value);
        device->SetMWidth(width);
        status = PLUS_SUCCESS;
      }
      else if (igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::SET_M_DEPTH))
      {
        int32_t depth = stoi(value);
        device->SetMDepth(depth);
        status = PLUS_SUCCESS;
      }
      else if (igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::SET_DECIMATION))
      {
        int32_t val = stoi(value);
        status = device->SetSSDecimation(val);
        status = PLUS_SUCCESS;
      }
      else if (igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::SET_B_FRAME_RATE_LIMIT))
      {
        int32_t val = stoi(value);
        device->SetBFrameRateLimit(val);
        status = PLUS_SUCCESS;
      }
      else if (igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::SET_B_HARMONIC_ENABLED))
      {
        bool set = igsioCommon::IsEqualInsensitive(value, "true") ? true : false;
        device->SetBHarmonicEnabled(set);
        status = PLUS_SUCCESS;
      }
      else if (igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::SET_B_BUBBLE_CONTRAST_ENABLED))
      {
        bool set = igsioCommon::IsEqualInsensitive(value, "true") ? true : false;
        device->SetBBubbleContrastEnabled(set);
        status = PLUS_SUCCESS;
      }
      else if (igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::SET_B_AMPLITUDE_MODULATION_ENABLED))
      {
        bool set = igsioCommon::IsEqualInsensitive(value, "true") ? true : false;
        device->SetBAmplitudeModulationEnabled(set);
        status = PLUS_SUCCESS;
      }
      else if (igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::SET_B_TRANSMIT_LOCKED))
      {
        bool set = igsioCommon::IsEqualInsensitive(value, "true") ? true : false;
        device->SetBTransmitLocked(set);
        status = PLUS_SUCCESS;
      }
      else if (igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::SET_B_TRANSMIT_CURRENT))
      {
        int val = stoi(value);
        device->SetBTransmitCurrent(val);
        status = PLUS_SUCCESS;
      }
      else if (igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::SET_B_TRANSMIT_CYCLE_COUNT))
      {
        uint16_t val = stoi(value);
        device->SetBTransmitCycleCount(val);
        status = PLUS_SUCCESS;
      }
      else if (igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::SET_B_TRANSMIT_FNUMBER))
      {
        double val = stod(value);
        device->SetBTransmitFNumber(val);
        status = PLUS_SUCCESS;
      }
      else if (igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::SET_B_APODIZATION_FNUMBER))
      {
        double val = stod(value);
        device->SetBApodizationFNumber(val);
        status = PLUS_SUCCESS;
      }
      else if (igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::SET_B_BUBBLE_DESTRUCTION_ENABLED))
      {
        bool set = igsioCommon::IsEqualInsensitive(value, "true") ? true : false;
        device->SetBBubbleDestructionEnabled(set);
        status = PLUS_SUCCESS;
      }
      else if (igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::SET_B_BUBBLE_DESTRUCTION_CYCLE_COUNT))
      {
        uint8_t val = stod(value);
        device->SetBBubbleDestructionCycleCount(val);
        status = PLUS_SUCCESS;
      }
      else if (igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::SET_B_TX_FILTER_COEFFICIENT_SET))
      {
        int32_t val = stod(value);
        device->SetBTXFilterCoefficientSet(val);
        status = PLUS_SUCCESS;
      }
      else if (igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::SET_ARFI_ENABLED))
      {
        bool set = igsioCommon::IsEqualInsensitive(value, "true") ? true : false;
        device->SetARFIEnabled(set);
        status = PLUS_SUCCESS;
      }
      else if (igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::SET_ARFI_START_SAMPLE))
      {
        int32_t val = stoi(value);
        device->SetARFIStartSample(val);
        status = PLUS_SUCCESS;
      }
      else if (igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::SET_ARFI_STOP_SAMPLE))
      {
        int32_t val = stoi(value);
        device->SetARFIStopSample(val);
        status = PLUS_SUCCESS;
      }
      else if (igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::SET_ARFI_PRE_PUSH_LINE_REPEAT_COUNT))
      {
        int32_t val = stoi(value);
        status = device->SetARFIPrePushLineRepeatCount(val);
      }
      else if (igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::SET_ARFI_POST_PUSH_LINE_REPEAT_COUNT))
      {
        int32_t val = stoi(value);
        status = device->SetARFIPostPushLineRepeatCount(val);
      }
      else if (igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::SET_ARFI_INTER_SET_DELAY))
      {
        int32_t val = stoi(value);
        status = device->SetARFIInterSetDelay(val);
      }
      else if (igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::SET_ARFI_INTER_PUSH_DELAY))
      {
        int32_t val = stoi(value);
        status = device->SetARFIInterPushDelay(val);
      }
      else if (igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::SET_ARFI_LINE_TIMER))
      {
        int32_t val = stoi(value);
        status = device->SetARFILineTimer(val);
      }
      else if (igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::SET_ARFI_TX_CYCLE_COUNT))
      {
        int32_t val = stoi(value);
        status = device->SetARFITxCycleCount(val);
      }
      else if (igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::SET_ARFI_TX_CYCLE_WIDTH))
      {
        int32_t val = stoi(value);
        status = device->SetARFITxCycleWidth(val);
      }
      else if (igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::SET_ARFI_TX_TX_CYCLE_COUNT))
      {
        int32_t val = stoi(value);
        status = device->SetARFITxTxCycleCount(val);
      }
      else if (igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::SET_ARFI_TX_TX_CYCLE_WIDTH))
      {
        int32_t val = stoi(value);
        status = device->SetARFITxTxCycleWidth(val);
      }
      else if (igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::SET_ARFI_PUSH_CONFIG))
      {
        device->SetARFIPushConfigurationString(value);
        status = PLUS_SUCCESS;
      }

      if (status == PLUS_SUCCESS)
      {
        std::string success = "true";
        std::string status_msg = "SUCCESS";
        resultString += " Success=\"" + success + "\"";
        metaData[parameterName] = std::make_pair(IANA_TYPE_US_ASCII, status_msg);
      }
      else
      {
        std::string success = "false";
        std::string status_msg = "FAIL";
        resultString += " Success=\"" + success + "\"";
        metaData[parameterName] = std::make_pair(IANA_TYPE_US_ASCII, status_msg);
      }
    }

    else if (igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::SET_M_LINE_INDEX))
    {
      device->SetMLineIndex(index);
      status = PLUS_SUCCESS;

      std::string status_msg = "SUCCESS";
      resultString += " Success=\"true\"";
      metaData[parameterName] = std::make_pair(IANA_TYPE_US_ASCII, status_msg);
    }

    else if (igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::SET_ALL_FOCAL_DEPTHS))
    {
      std::istringstream ss(value);
      std::string val;
      status = PLUS_SUCCESS;
      for(int i = 0; i < 4; i++)
      {
        ss >> val;
        status = status == PLUS_SUCCESS ? device->SetFocalPointDepth(i, stof(val)) : PLUS_FAIL;
        if (status != PLUS_SUCCESS)
        {
          error += "Error setting Focal Depth " + std::to_string(i) + " to " + val + "\n";
          break;
        }
      }

      std::string success = status == PLUS_SUCCESS ? "true" : "false";
      std::string status_msg = status == PLUS_SUCCESS ? "SUCCESS" : "FAIL";
      resultString += " Success=\"" + success + "\"";
      metaData[parameterName] = std::make_pair(IANA_TYPE_US_ASCII, status_msg);
    }
    else if (igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::SET_ALL_ARFI_FOCAL_DEPTHS))
    {
      std::istringstream ss(value);
      std::string val;
      status = PLUS_SUCCESS;
      for(int i = 0; i < 6; i++)
      {
        ss >> val;
        status = status == PLUS_SUCCESS ? device->SetARFIFocalPointDepth(i, stof(val)) : PLUS_FAIL;
        if (status != PLUS_SUCCESS)
        {
          error += "Error setting Focal Depth " + std::to_string(i) + " to " + val + "\n";
          break;
        }
      }

      std::string success = status == PLUS_SUCCESS ? "true" : "false";
      std::string status_msg = status == PLUS_SUCCESS ? "SUCCESS" : "FAIL";
      resultString += " Success=\"" + success + "\"";
      metaData[parameterName] = std::make_pair(IANA_TYPE_US_ASCII, status_msg);
    }
    else if (igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::IS_SCANNING))
    {
      res = device->IsScanning() ? "True" : "False";

      status = PLUS_SUCCESS;
      resultString += " Success=\"true\"";
      resultString += " Value=\"" + res + "\"";
      metaData[parameterName] = std::make_pair(IANA_TYPE_US_ASCII, res);
    }
    else if (igsioCommon::IsEqualInsensitive(parameterName, vtkPlusWinProbeVideoSource::UV_SEND_COMMAND))
    {
      status = device->SendCommand(value.c_str());

      std::string success = status == PLUS_SUCCESS ? "true" : "false";
      std::string status_msg = status == PLUS_SUCCESS ? "SUCCESS" : "FAIL";
      resultString += " Success=\"" + success + "\"";
      metaData[parameterName] = std::make_pair(IANA_TYPE_US_ASCII, success);
    }
    else
    {
      status = PLUS_FAIL;
      resultString += " Success=\"false\"";
      error += "No parameter named \"" + parameterName + "\"\n";
      metaData[parameterName] = std::make_pair(IANA_TYPE_US_ASCII, "Unknown Parameter");
    }
    if (status != PLUS_SUCCESS)
      hasFailure = true;
    resultString += "/>\n";
    }
  resultString += "</CommandReply>";
  if (hasFailure)
    status = PLUS_FAIL;

  vtkSmartPointer<vtkPlusCommandRTSCommandResponse> commandResponse = vtkSmartPointer<vtkPlusCommandRTSCommandResponse>::New();
  commandResponse->UseDefaultFormatOff();
  commandResponse->SetClientId(this->ClientId);
  commandResponse->SetOriginalId(this->Id);
  commandResponse->SetDeviceName(this->DeviceName);
  commandResponse->SetCommandName(this->GetName());
  commandResponse->SetStatus(status);
  commandResponse->SetRespondWithCommandMessage(this->RespondWithCommandMessage);
  commandResponse->SetErrorString(error);
  commandResponse->SetResultString(resultString);
  commandResponse->SetParameters(metaData);
  this->CommandResponseQueue.push_back(commandResponse);
  return PLUS_SUCCESS;

}