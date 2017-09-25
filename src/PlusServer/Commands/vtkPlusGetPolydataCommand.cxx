/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

// Local includes
#include "PlusConfigure.h"
#include "vtkPlusDataCollector.h"
#include "vtkPlusGetPolydataCommand.h"

// VTK includes
#include <vtkPolyData.h>
#include <vtkPolyDataReader.h>

namespace
{
  static const std::string GET_POLYDATA = "GetPolydata";
}

vtkStandardNewMacro(vtkPlusGetPolydataCommand);

//----------------------------------------------------------------------------
vtkPlusGetPolydataCommand::vtkPlusGetPolydataCommand()
  : PolydataId("")
{

}

//----------------------------------------------------------------------------
vtkPlusGetPolydataCommand::~vtkPlusGetPolydataCommand()
{
}

//----------------------------------------------------------------------------
void vtkPlusGetPolydataCommand::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusGetPolydataCommand::ReadConfiguration(vtkXMLDataElement* aConfig)
{
  // Get filename from attribute
  if (aConfig->GetAttribute("FileName") != nullptr)
  {
    this->PolydataId = aConfig->GetAttribute("FileName");
  }
  else
  {
    LOG_ERROR("Unable to find FileName attribute in " << this->GetName() << " command.");
    return PLUS_FAIL;
  }

  return Superclass::ReadConfiguration(aConfig);
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusGetPolydataCommand::WriteConfiguration(vtkXMLDataElement* aConfig)
{
  if (this->GetPolydataId().empty())
  {
    LOG_ERROR("PolydataId not set when WriteConfiguration was called.");
    return PLUS_FAIL;
  }
  aConfig->SetAttribute("FileName", this->GetPolydataId().c_str());

  return Superclass::WriteConfiguration(aConfig);
}

//----------------------------------------------------------------------------
void vtkPlusGetPolydataCommand::GetCommandNames(std::list<std::string>& cmdNames)
{
  cmdNames.clear();
  cmdNames.push_back(GET_POLYDATA);
}

//----------------------------------------------------------------------------
std::string vtkPlusGetPolydataCommand::GetDescription(const std::string& commandName)
{
  std::string desc;
  if (commandName.empty() || PlusCommon::IsEqualInsensitive(commandName, GET_POLYDATA))
  {
    desc += GET_POLYDATA;
    desc += "Acquire the polydata.";
  }
  return desc;
}

//----------------------------------------------------------------------------
void vtkPlusGetPolydataCommand::SetNameToGetPolydata()
{
  SetName(GET_POLYDATA);
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusGetPolydataCommand::Execute()
{
  std::string baseMessage = std::string("vtkPlusGetPolydataCommand ") + (!this->Name.empty() ? this->Name : "(undefined)")
                            + ", device: (" + (this->GetPolydataId().empty() ? "(undefined)" : this->GetPolydataId()) + ")";

  std::string errorString;
  if (PlusCommon::IsEqualInsensitive(this->Name, GET_POLYDATA))
  {
    if (this->ExecutePolydataReply(errorString) != PLUS_SUCCESS)
    {
      return PLUS_FAIL;
    }
    return PLUS_SUCCESS;
  }
  this->QueueCommandResponse(PLUS_FAIL, baseMessage + " Failed. See error message.", std::string("Unknown command name: ") + this->Name);
  return PLUS_FAIL;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusGetPolydataCommand::ExecutePolydataReply(std::string& outErrorString)
{
  bool loaded(false);

  vtkSmartPointer<vtkPolyDataReader> reader = vtkSmartPointer<vtkPolyDataReader>::New();

  vtkSmartPointer<vtkPlusCommandRTSCommandResponse> response = vtkSmartPointer<vtkPlusCommandRTSCommandResponse>::New();
  response->SetClientId(this->ClientId);
  response->SetOriginalId(this->GetId());
  response->SetStatus(PLUS_FAIL);
  outErrorString = "PLUS cannot load polydata.";

  if (vtksys::SystemTools::FileExists(this->PolydataId))
  {
    reader->SetFileName(this->PolydataId.c_str());
  }
  else
  {
    if (vtksys::SystemTools::FileExists(vtkPlusConfig::GetInstance()->GetModelPath(this->PolydataId)))
    {
      reader->SetFileName(vtkPlusConfig::GetInstance()->GetModelPath(this->PolydataId).c_str());
    }
    else
    {
      LOG_ERROR("Unable to locate file with name: " << this->PolydataId);
      outErrorString = std::string("Unable to locate file with name ") + this->PolydataId;
      response->SetErrorString(outErrorString);
      this->CommandResponseQueue.push_back(response);
      return PLUS_FAIL;
    }
  }

  reader->Update();
  auto polyData = reader->GetOutput();
  if (polyData != nullptr)
  {
    vtkSmartPointer<vtkPlusCommandPolydataResponse> response = vtkSmartPointer<vtkPlusCommandPolydataResponse>::New();
    response->SetClientId(this->ClientId);
    response->SetPolyDataName(this->GetPolydataId());
    response->SetPolyData(polyData);
    response->SetRespondWithCommandMessage(this->RespondWithCommandMessage);
    this->CommandResponseQueue.push_back(response);

    outErrorString = "";
    response->SetStatus(PLUS_SUCCESS);
  }

  response->SetErrorString(outErrorString);
  this->CommandResponseQueue.push_back(response);
  return polyData != nullptr ? PLUS_SUCCESS : PLUS_FAIL;
}