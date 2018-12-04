/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"
#include "igtl_header.h"
#include "vtkPlusCommandProcessor.h"
#include "vtkPlusDataCollector.h"
#include "vtkPlusGetTransformCommand.h"
#include "vtkIGSIOTransformRepository.h"

vtkStandardNewMacro(vtkPlusGetTransformCommand);

namespace
{
  static const std::string GET_TRANSFORM_CMD = "GetTransform";
}

//----------------------------------------------------------------------------
vtkPlusGetTransformCommand::vtkPlusGetTransformCommand()
{
}

//----------------------------------------------------------------------------
vtkPlusGetTransformCommand::~vtkPlusGetTransformCommand()
{
}

//----------------------------------------------------------------------------
void vtkPlusGetTransformCommand::SetNameToGetTransform()
{
  this->SetName(GET_TRANSFORM_CMD);
}

//----------------------------------------------------------------------------
void vtkPlusGetTransformCommand::GetCommandNames(std::list<std::string>& cmdNames)
{
  cmdNames.clear();
  cmdNames.push_back(GET_TRANSFORM_CMD);
}

//----------------------------------------------------------------------------
std::string vtkPlusGetTransformCommand::GetDescription(const std::string& commandName)
{
  std::string desc;
  if (commandName.empty() || igsioCommon::IsEqualInsensitive(commandName, GET_TRANSFORM_CMD))
  {
    desc += GET_TRANSFORM_CMD;
    desc += ": Retrieve the details of a transform in the remote transform repository.";
  }
  return desc;
}

//----------------------------------------------------------------------------
void vtkPlusGetTransformCommand::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusGetTransformCommand::ReadConfiguration(vtkXMLDataElement* aConfig)
{
  if (vtkPlusCommand::ReadConfiguration(aConfig) != PLUS_SUCCESS)
  {
    return PLUS_FAIL;
  }
  XML_READ_STRING_ATTRIBUTE_REQUIRED(TransformName, aConfig);
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusGetTransformCommand::WriteConfiguration(vtkXMLDataElement* aConfig)
{
  if (vtkPlusCommand::WriteConfiguration(aConfig) != PLUS_SUCCESS)
  {
    return PLUS_FAIL;
  }

  XML_WRITE_STRING_ATTRIBUTE(TransformName, aConfig);

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusGetTransformCommand::Execute()
{
  LOG_INFO("vtkPlusGetTransformCommand::Execute:");

  std::string baseMessageString = std::string("GetTransform (") + (!this->GetTransformName().empty() ? this->GetTransformName() : "undefined") + ")";
  std::string warningString;

  if (this->GetTransformRepository() == NULL)
  {
    this->QueueCommandResponse(PLUS_FAIL, "Command failed. See error message.", baseMessageString + " Failed: invalid transform repository.");
    return PLUS_FAIL;
  }

  igsioTransformName aName;
  aName.SetTransformName(this->GetTransformName());

  if (this->GetTransformRepository()->IsExistingTransform(aName) != PLUS_SUCCESS)
  {
    this->QueueCommandResponse(PLUS_FAIL, "Command failed. See error message.", baseMessageString + " Failed. Transform not found.");
    return PLUS_SUCCESS;
  }

  bool persistent;
  this->GetTransformRepository()->GetTransformPersistent(aName, persistent);
  vtkSmartPointer<vtkMatrix4x4> value = vtkSmartPointer<vtkMatrix4x4>::New();
  this->GetTransformRepository()->GetTransform(aName, value);
  std::string date;
  this->GetTransformRepository()->GetTransformDate(aName, date);
  double error;
  this->GetTransformRepository()->GetTransformError(aName, error);
  std::ostringstream errorStringStream;
  errorStringStream << error;

  std::ostringstream valueStringStream;
  if (!this->RespondWithCommandMessage)
  {
    valueStringStream << "<name=\"" << aName.GetTransformName() << "\" value=\"";
  }
  for (int i = 0; i < 4; ++i)
  {
    for (int j = 0; j < 4; ++j)
    {
      valueStringStream << value->GetElement(i, j);
      if (i * j < 9)
      {
        valueStringStream << " ";
      }
    }
  }
  if (!this->RespondWithCommandMessage)
  {
    valueStringStream << "\" persistent=\"" << (persistent ? "true" : "false") << "\" date=\"" << date << "\" error=\"" << error << "\"/>";
    this->QueueCommandResponse(PLUS_SUCCESS, baseMessageString + valueStringStream.str());
  }
  else
  {
    igtl::MessageBase::MetaDataMap parameters;
    parameters[aName.GetTransformName()] = std::pair<IANA_ENCODING_TYPE, std::string>(IANA_TYPE_US_ASCII, valueStringStream.str());
    parameters["persistent"] = std::pair<IANA_ENCODING_TYPE, std::string>(IANA_TYPE_US_ASCII, persistent ? "true" : "false");
    parameters["date"] = std::pair<IANA_ENCODING_TYPE, std::string>(IANA_TYPE_US_ASCII, date);
    parameters["error"] = std::pair<IANA_ENCODING_TYPE, std::string>(IANA_TYPE_US_ASCII, errorStringStream.str());
    this->QueueCommandResponse(PLUS_SUCCESS, baseMessageString + " Command successful.", "", &parameters);
  }
  return PLUS_SUCCESS;
}