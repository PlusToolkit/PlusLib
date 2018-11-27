/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"
#include "vtkPlusDataCollector.h"
#include "vtkPlusCommandProcessor.h"
#include "vtkPlusUpdateTransformCommand.h"
#include "vtkIGSIOTransformRepository.h"
#include "vtkPlusVirtualCapture.h"

vtkStandardNewMacro(vtkPlusUpdateTransformCommand);

namespace
{
  static const std::string UPDATE_TRANSFORM_CMD = "UpdateTransform";
}

//----------------------------------------------------------------------------
vtkPlusUpdateTransformCommand::vtkPlusUpdateTransformCommand()
  : TransformValue(NULL)
  , TransformPersistent(true)
  , TransformError(-1.0)
{
}

//----------------------------------------------------------------------------
vtkPlusUpdateTransformCommand::~vtkPlusUpdateTransformCommand()
{
  this->SetTransformValue(static_cast<vtkMatrix4x4*>(NULL));
}

//----------------------------------------------------------------------------
void vtkPlusUpdateTransformCommand::SetNameToUpdateTransform()
{
  this->SetName(UPDATE_TRANSFORM_CMD);
}

//----------------------------------------------------------------------------
void vtkPlusUpdateTransformCommand::GetCommandNames(std::list<std::string>& cmdNames)
{
  cmdNames.clear();
  cmdNames.push_back(UPDATE_TRANSFORM_CMD);
}

//----------------------------------------------------------------------------
std::string vtkPlusUpdateTransformCommand::GetDescription(const std::string& commandName)
{
  std::string desc;
  if (commandName.empty() || igsioCommon::IsEqualInsensitive(commandName, UPDATE_TRANSFORM_CMD))
  {
    desc += UPDATE_TRANSFORM_CMD;
    desc += ": Update the details of a transform in the remote transform repository.";
  }
  return desc;
}

//----------------------------------------------------------------------------
void vtkPlusUpdateTransformCommand::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusUpdateTransformCommand::ReadConfiguration(vtkXMLDataElement* aConfig)
{
  if (vtkPlusCommand::ReadConfiguration(aConfig) != PLUS_SUCCESS)
  {
    return PLUS_FAIL;
  }
  XML_READ_STRING_ATTRIBUTE_OPTIONAL(TransformName, aConfig);
  XML_READ_VECTOR_ATTRIBUTE_REQUIRED(double, 16, TransformValue, aConfig);
  XML_READ_BOOL_ATTRIBUTE_OPTIONAL(TransformPersistent, aConfig);
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(double, TransformError, aConfig);
  XML_READ_CSTRING_ATTRIBUTE_OPTIONAL(TransformDate, aConfig);
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusUpdateTransformCommand::WriteConfiguration(vtkXMLDataElement* aConfig)
{
  if (vtkPlusCommand::WriteConfiguration(aConfig) != PLUS_SUCCESS)
  {
    return PLUS_FAIL;
  }

  aConfig->SetAttribute("TransformName", this->GetTransformName().c_str());

  if (this->GetTransformValue())
  {
    double vectorMatrix[16] = {0};
    vtkMatrix4x4::DeepCopy(vectorMatrix, GetTransformValue());
    aConfig->SetVectorAttribute("TransformValue", 16, vectorMatrix);
  }

  XML_WRITE_STRING_ATTRIBUTE_IF_NOT_EMPTY(TransformDate, aConfig);

  if (this->GetTransformError() >= 0)
  {
    aConfig->SetDoubleAttribute("TransformError", this->GetTransformError());
  }

  XML_WRITE_BOOL_ATTRIBUTE(TransformPersistent, aConfig);

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusUpdateTransformCommand::Execute()
{
  LOG_INFO("vtkPlusUpdateTransformCommand::Execute:");

  std::string baseMessageString = std::string("UpdateTransform (") + (!this->GetTransformName().empty() ? this->GetTransformName() : "undefined") + ")";
  std::string warningString;

  if (this->GetTransformRepository() == NULL)
  {
    this->QueueCommandResponse(PLUS_FAIL, "Command failed. See error message.", baseMessageString + " failed: invalid transform repository");
    return PLUS_FAIL;
  }

  igsioTransformName aName;
  aName.SetTransformName(this->GetTransformName());

  if (this->GetTransformRepository()->IsExistingTransform(aName) == PLUS_SUCCESS)
  {
    bool persistent = false;
    this->GetTransformRepository()->GetTransformPersistent(aName, persistent);
    if (!persistent && this->GetTransformPersistent())
    {
      warningString += " WARNING: replacing non-persistent transform with a persistent transform.";
    }
  }

  if (this->TransformValue)
  {
    this->GetTransformRepository()->SetTransform(aName, this->TransformValue);
  }
  else
  {
    warningString += " WARNING: transform is not specified.";
  }

  this->GetTransformRepository()->SetTransformPersistent(aName, this->GetTransformPersistent());

  if (!this->GetTransformDate().empty())
  {
    this->GetTransformRepository()->SetTransformDate(aName, this->GetTransformDate());
  }
  if (this->GetTransformError() >= 0)
  {
    this->GetTransformRepository()->SetTransformError(aName, this->GetTransformError());
  }

  this->QueueCommandResponse(PLUS_SUCCESS, baseMessageString + " completed successfully" + warningString);
  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
void vtkPlusUpdateTransformCommand::SetTransformValue(double* matrixElements)
{
  vtkSmartPointer<vtkMatrix4x4> matrix = vtkSmartPointer<vtkMatrix4x4>::New();
  matrix->DeepCopy(matrixElements);
  this->SetTransformValue(matrix);
}
