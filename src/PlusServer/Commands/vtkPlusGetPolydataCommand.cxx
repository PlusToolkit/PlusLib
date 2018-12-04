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
#include <vtkOBJReader.h>
#include <vtkPLYReader.h>
#include <vtkPolyData.h>
#include <vtkPolyDataReader.h>
#include <vtkSTLReader.h>

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
  else if (this->MetaData.find("FileName") != this->MetaData.end())
  {
    this->PolydataId = this->MetaData["FileName"].second;
  }
  else
  {
    LOG_ERROR("Unable to find FileName attribute or metadata in " << this->GetName() << " command.");
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
  if (commandName.empty() || igsioCommon::IsEqualInsensitive(commandName, GET_POLYDATA))
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
  if (igsioCommon::IsEqualInsensitive(this->Name, GET_POLYDATA))
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

  std::string finalFileName(this->PolydataId);
  if (vtksys::SystemTools::FileExists(this->PolydataId))
  {
    reader->SetFileName(this->PolydataId.c_str());
  }
  else
  {
    if (vtkPlusConfig::GetInstance()->FindModelPath(this->PolydataId, finalFileName) == PLUS_SUCCESS)
    {
      reader->SetFileName(finalFileName.c_str());
    }
    else
    {
      LOG_ERROR("Unable to locate file with name " << this->PolydataId);
      this->QueueCommandResponse(PLUS_FAIL, "Command failed.", std::string("Unable to locate file with name ") + this->PolydataId);
      return PLUS_FAIL;
    }
  }

  vtkSmartPointer<vtkAbstractPolyDataReader> polyReader = vtkSmartPointer<vtkSTLReader>::New();
  if (!reader->IsFilePolyData())
  {
    reader = nullptr; // This flags later code to recognize that it's not a .vtk file
    polyReader->SetFileName(finalFileName.c_str());
    polyReader->Update();
    if (polyReader->GetOutput() == nullptr)
    {
      polyReader = vtkSmartPointer<vtkOBJReader>::New();
      polyReader->SetFileName(finalFileName.c_str());
      polyReader->Update();
      if (polyReader->GetOutput() == nullptr)
      {
        polyReader = vtkSmartPointer<vtkPLYReader>::New();
        polyReader->SetFileName(finalFileName.c_str());
        polyReader->Update();
        if (polyReader->GetOutput() == nullptr)
        {
          LOG_ERROR("Unrecognized polydata file type: " << this->PolydataId);
          this->QueueCommandResponse(PLUS_FAIL, "Command failed.", std::string("Unrecognized polydata file type: ") + this->PolydataId);
          return PLUS_FAIL;
        }
      }
    }
  }
  else
  {
    reader->Update();
  }

  auto errorCode = (reader == nullptr ? polyReader->GetErrorCode() : reader->GetErrorCode());
  vtkPolyData* polyData = (reader == nullptr ? polyReader->GetOutput() : reader->GetOutput());
  if (errorCode != 0)
  {
    std::stringstream ss;
    ss << "Reader threw error: " << errorCode;
    this->QueueCommandResponse(PLUS_FAIL, "PlusServer", ss.str());
  }
  else if (polyData != nullptr)
  {
    vtkSmartPointer<vtkPlusCommandPolydataResponse> response = vtkSmartPointer<vtkPlusCommandPolydataResponse>::New();
    response->SetClientId(this->ClientId);
    response->SetPolyDataName(this->GetPolydataId());
    response->SetPolyData(polyData);
    response->SetRespondWithCommandMessage(this->RespondWithCommandMessage);
    this->CommandResponseQueue.push_back(response);

    std::string name = vtksys::SystemTools::GetFilenameName(this->PolydataId);
    this->QueueCommandResponse(PLUS_SUCCESS, name, "Command succeeded.");
    return PLUS_SUCCESS;
  }

  this->QueueCommandResponse(PLUS_FAIL, "Unable to load polydata.");
  return PLUS_FAIL;
}