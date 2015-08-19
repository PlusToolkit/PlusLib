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
  this->SetTransformValue(static_cast<vtkMatrix4x4*>(NULL));
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
  XML_READ_STRING_ATTRIBUTE_REQUIRED(TransformName, aConfig);
  XML_READ_VECTOR_ATTRIBUTE_REQUIRED(double, 16, TransformValue, aConfig);
  XML_READ_BOOL_ATTRIBUTE_OPTIONAL(TransformPersistent, aConfig);
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(double, TransformError, aConfig);
  XML_READ_STRING_ATTRIBUTE_OPTIONAL(TransformDate, aConfig);
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

  XML_WRITE_STRING_ATTRIBUTE_IF_NOT_NULL(TransformDate, aConfig);

  if (this->GetTransformError()>=0)
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

  std::string baseMessageString = std::string("UpdateTransform (") + (this->GetTransformName()?this->GetTransformName():"undefined") + ")";
  std::string warningString;

  if( this->GetTransformRepository() == NULL )
  {
    this->QueueStringResponse(baseMessageString + " failed: invalid transform repository",PLUS_FAIL);
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
  
  if( this->GetTransformDate() )
  {
    this->GetTransformRepository()->SetTransformDate(aName, this->GetTransformDate());
  }
  if ( this->GetTransformError()>=0 )
  {
    this->GetTransformRepository()->SetTransformError(aName, this->GetTransformError());
  }

  this->QueueStringResponse(baseMessageString + " completed successfully" + warningString,PLUS_SUCCESS);
  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
void vtkPlusUpdateTransformCommand::SetTransformValue(double* matrixElements)
{
  vtkSmartPointer<vtkMatrix4x4> matrix = vtkSmartPointer<vtkMatrix4x4>::New();
  matrix->DeepCopy(matrixElements); 
  this->SetTransformValue(matrix);
}