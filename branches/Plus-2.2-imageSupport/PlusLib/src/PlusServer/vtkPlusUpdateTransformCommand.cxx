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
, TransformValue(NULL)
, TransformPersistent(true)
, TransformError(-1.0)
, TransformDate(NULL)
{
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

  double transformMatrix[16]={0}; 
  if ( aConfig->GetVectorAttribute("TransformValue", 16, transformMatrix) )
  {
    vtkSmartPointer<vtkMatrix4x4> transform=vtkSmartPointer<vtkMatrix4x4>::New();
    transform->DeepCopy(transformMatrix);
    SetTransformValue(transform);
  }
  else
  {
    SetTransformValue(NULL);
  }

  if( aConfig->GetAttribute("TransformPersistent") != NULL )
  {
    this->SetTransformPersistent(STRCASECMP(aConfig->GetAttribute("TransformPersistent"), "TRUE") == 0);
  }

  double error=0.0;
  if( aConfig->GetScalarAttribute("TransformError", error) )
  {
    this->SetTransformError(error);
  }
  else
  {
    this->SetTransformError(-1.0);
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

  aConfig->SetAttribute("TransformName", this->GetTransformName());

  if (this->GetTransformValue())
  {
    double vectorMatrix[16]={0}; 
    vtkMatrix4x4::DeepCopy(vectorMatrix,GetTransformValue()); 
    aConfig->SetVectorAttribute("TransformValue", 16, vectorMatrix);
  }

  if (this->GetTransformDate())
  {
    aConfig->SetAttribute("TransformDate", this->GetTransformDate());
  }

  if (this->GetTransformError()>=0)
  {
    aConfig->SetDoubleAttribute("TransformError", this->GetTransformError());
  }
  
  aConfig->SetAttribute("TransformPersistent", this->GetTransformPersistent() ? "TRUE" : "FALSE");
  
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusUpdateTransformCommand::Execute()
{
  LOG_INFO("vtkPlusUpdateTransformCommand::Execute:");

  this->ResponseMessage=std::string("UpdateTransform (") + TransformName + ")";

  if( this->GetTransformRepository() == NULL )
  {
    this->ResponseMessage += " failed: invalid transform repository";
    return PLUS_FAIL;
  }

  PlusTransformName aName;
  aName.SetTransformName(this->GetTransformName());

  if( this->GetTransformRepository()->IsExistingTransform(aName) == PLUS_SUCCESS)
  {
    bool persistent=false;
    this->GetTransformRepository()->GetTransformPersistent(aName, persistent);
    if ( !persistent && this->GetTransformPersistent() )
    {
      this->ResponseMessage += " WARNING: replacing non-persistent transform with a persistent transform.";
    }
  }

  if (this->TransformValue)
  {
    this->GetTransformRepository()->SetTransform(aName, this->TransformValue);
  }
  else
  {
    this->ResponseMessage += " WARNING: transform is not specified.";
  }

  this->GetTransformRepository()->SetTransformPersistent(aName, this->GetTransformPersistent());
  
  if( this->GetTransformDate() )
  {
    this->GetTransformRepository()->SetTransformDate(aName, this->GetTransformDate());
  }
  if ( this->GetTransformError()>=0 )
  {
    this->GetTransformRepository()->SetTransformError(aName, this->GetTransformError());
  }

  this->ResponseMessage += " completed successfully";
  return PLUS_SUCCESS;
}
