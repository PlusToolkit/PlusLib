/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

#include "PlusConfigure.h"
#include "vtkDataCollector.h"
#include "vtkPlusCommandProcessor.h"
#include "vtkPlusUpdateTransformCommand.h"
#include "vtkTransformRepository.h"
#include "vtkVirtualDiscCapture.h"

vtkStandardNewMacro( vtkPlusUpdateTransformCommand );

static const char UPDATE_TRANSFORM_CMD[] = "UpdateTransform";

//----------------------------------------------------------------------------
vtkPlusUpdateTransformCommand::vtkPlusUpdateTransformCommand()
: TransformName(NULL)
, TransformValue(vtkMatrix4x4::New())
, hasMatrix(false)
, TransformPersistent(false)
, hasPersistent(false)
, TransformError(-1.0)
, hasError(false)
, TransformDate(NULL)
{
  this->SetTransformDate("");
}

//----------------------------------------------------------------------------
vtkPlusUpdateTransformCommand::~vtkPlusUpdateTransformCommand()
{
  this->SetTransformName(NULL);
  this->SetTransformValue(NULL);
  this->SetTransformDate(NULL);
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
std::string vtkPlusUpdateTransformCommand::GetDescription(const char* commandName)
{ 
  std::string desc;
  if (commandName==NULL || STRCASECMP(commandName, UPDATE_TRANSFORM_CMD))
  {
    desc+=UPDATE_TRANSFORM_CMD;
    desc+=": Update the details of a transform in the remote transform repository.";
  }
  return desc;
}

//----------------------------------------------------------------------------
void vtkPlusUpdateTransformCommand::PrintSelf( ostream& os, vtkIndent indent )
{
  this->Superclass::PrintSelf( os, indent );
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusUpdateTransformCommand::ReadConfiguration(vtkXMLDataElement* aConfig)
{  
  if (vtkPlusCommand::ReadConfiguration(aConfig)!=PLUS_SUCCESS)
  {
    return PLUS_FAIL;
  }

  this->SetTransformName(aConfig->GetAttribute("TransformName"));

  if( aConfig->GetAttribute("TransformValue") != NULL )
  {
    std::vector<std::string> elems;
    PlusCommon::SplitStringIntoTokens(aConfig->GetAttribute("TransformValue"), ' ', elems);
    for (int i = 0; i < 4; i++)
    {
      for (int j = 0; j < 4; j++)
      {
        double value;
        std::stringstream stream(elems[i*4+j]);
        stream >> value;
        this->TransformValue->SetElement(i, j, value);
      }
    }
    this->hasMatrix = true;
  }
  else
  {
    this->hasMatrix = false;
  }

  if( aConfig->GetAttribute("TransformPersistent") != NULL )
  {
    this->SetTransformPersistent(STRCASECMP(aConfig->GetAttribute("TransformPersistent"), "TRUE") == 0);
    this->hasPersistent = true;
  }
  double error;
  if( aConfig->GetScalarAttribute("TransformError", error) )
  {
    aConfig->GetScalarAttribute("TransformError", error);
    this->SetTransformError(error);
    this->hasError = true;
  }
  this->SetTransformDate(aConfig->GetAttribute("TransformDate"));

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusUpdateTransformCommand::WriteConfiguration(vtkXMLDataElement* aConfig)
{  
  if (vtkPlusCommand::WriteConfiguration(aConfig)!=PLUS_SUCCESS)
  {
    return PLUS_FAIL;
  }  

  // Common parameters
  aConfig->SetAttribute("Name",this->Name);

  // Start parameters
  aConfig->SetAttribute("TransformName", this->GetTransformName());

  if( hasMatrix )
  {
    std::stringstream ss;
    for (int i = 0; i < 4; i++)
    {
      for (int j = 0; j < 4; j++)
      {
        ss << this->GetTransformValue()->GetElement(i, j) << " ";
      }
    }
    std::string valueStringRepresentation = ss.str();
    valueStringRepresentation = PlusCommon::Trim(valueStringRepresentation.c_str());
    aConfig->SetAttribute("TransformValue", valueStringRepresentation.c_str());
  }

  std::string date(this->GetTransformDate());
  if( !date.empty() )
  {
    aConfig->SetAttribute("TransformDate", this->GetTransformDate());
  }

  if( this->hasError )
  {
    aConfig->SetDoubleAttribute("TransformError", this->GetTransformError());
  }
  if( this->hasPersistent )
  {
    aConfig->SetAttribute("TransformPersistent", this->GetTransformPersistent() ? "TRUE" : "FALSE");
  }
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusUpdateTransformCommand::Execute()
{
  if (this->Name == NULL)
  {
    LOG_ERROR("Command failed, no command name specified");
    this->SetCommandCompleted(PLUS_FAIL, "Command failed, no command name specified");
    return PLUS_FAIL;
  }

  PlusStatus status = PLUS_SUCCESS;
  std::string reply = std::string("UpdateTransform (") + TransformName + 
    ") " + this->Name + " ";  

  LOG_INFO("vtkPlusUpdateTransformCommand::Execute: " << this->Name);

  if (STRCASECMP(this->Name, UPDATE_TRANSFORM_CMD) == 0)
  {    
    if( this->GetTransformRepository() != NULL )
    {
      vtkMatrix4x4* aMatrix = vtkMatrix4x4::New();
      bool persistent;
      PlusTransformName aName;
      aName.SetTransformName(this->GetTransformName());
      if( this->GetTransformRepository()->IsExistingTransform(aName) == PLUS_SUCCESS &&
        this->GetTransformRepository()->GetTransformPersistent(aName, persistent) && 
        !persistent && this->GetTransformPersistent() )
      {
        reply += "WARNING: replacing non-persistent transform with a persistent transform, ";
      }
      if( this->hasMatrix )
      {
        this->GetTransformRepository()->SetTransform(aName, this->TransformValue);
      }
      if( this->hasPersistent )
      {
        this->GetTransformRepository()->SetTransformPersistent(aName, this->GetTransformPersistent());
      }
      std::string date(this->GetTransformDate());
      if( !date.empty() )
      {
        this->GetTransformRepository()->SetTransformDate(aName, this->GetTransformDate());
      }
      if( this->hasError )
      {
        this->GetTransformRepository()->SetTransformError(aName, this->GetTransformError());
      }
    }
    else
    {
      reply += "NULL transform repository, ";
      status = PLUS_FAIL;
    }
  }
  else
  {
    reply += "unknown command, ";
    status = PLUS_FAIL;
  }

  if (status == PLUS_SUCCESS)
  {
    reply += "completed successfully";
  }
  else
  {
    reply += "failed";
  }
  this->SetCommandCompleted(status,reply);
  return status;
}
