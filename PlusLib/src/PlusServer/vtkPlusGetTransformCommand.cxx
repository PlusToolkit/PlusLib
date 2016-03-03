/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"
#include "vtkDataCollector.h"
#include "vtkPlusCommandProcessor.h"
#include "vtkPlusGetTransformCommand.h"
#include "vtkTransformRepository.h"

vtkStandardNewMacro( vtkPlusGetTransformCommand );

static const char GET_TRANSFORM_CMD[] = "GetTransform";

//----------------------------------------------------------------------------
vtkPlusGetTransformCommand::vtkPlusGetTransformCommand()
: TransformName(NULL)
{
}

//----------------------------------------------------------------------------
vtkPlusGetTransformCommand::~vtkPlusGetTransformCommand()
{
  this->SetTransformName(NULL);
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
std::string vtkPlusGetTransformCommand::GetDescription(const char* commandName)
{ 
  std::string desc;
  if (commandName==NULL || STRCASECMP(commandName, GET_TRANSFORM_CMD))
  {
    desc+=GET_TRANSFORM_CMD;
    desc+=": Retrieve the details of a transform in the remote transform repository.";
  }
  return desc;
}

//----------------------------------------------------------------------------
void vtkPlusGetTransformCommand::PrintSelf( ostream& os, vtkIndent indent )
{
  this->Superclass::PrintSelf( os, indent );
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusGetTransformCommand::ReadConfiguration(vtkXMLDataElement* aConfig)
{  
  if (vtkPlusCommand::ReadConfiguration(aConfig)!=PLUS_SUCCESS)
  {
    return PLUS_FAIL;
  }
  XML_READ_STRING_ATTRIBUTE_REQUIRED(TransformName, aConfig);
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusGetTransformCommand::WriteConfiguration(vtkXMLDataElement* aConfig)
{  
  if (vtkPlusCommand::WriteConfiguration(aConfig)!=PLUS_SUCCESS)
  {
    return PLUS_FAIL;
  }  

  aConfig->SetAttribute("TransformName", this->GetTransformName());
  
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusGetTransformCommand::Execute()
{
  LOG_INFO("vtkPlusGetTransformCommand::Execute:");

  std::string baseMessageString = std::string("GetTransform (") + (this->GetTransformName()?this->GetTransformName():"undefined") + ")";
  std::string warningString;

  if( this->GetTransformRepository() == NULL )
  {
    this->QueueCommandResponse(baseMessageString + " failed: invalid transform repository",PLUS_FAIL);
    return PLUS_FAIL;
  }

  PlusTransformName aName;
  aName.SetTransformName(this->GetTransformName());

  if( this->GetTransformRepository()->IsExistingTransform(aName) != PLUS_SUCCESS)
  {
    this->QueueCommandResponse(baseMessageString + " failed. Transform not found.", PLUS_FAIL);
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

  std::stringstream ss;
  ss << "<name=\"" << aName.GetTransformName() << "\" value=\"";
  for( int i = 0; i < 4; ++i )
  {
    for( int j = 0; j < 4; ++j)
    {
      ss << value->GetElement(i, j);
      if( i * j < 9 )
      {
        ss << " ";
      }
    }
  }
  ss << "\" persistent=\"" << (persistent ? "true" : "false") << "\" date=\"" << date << "\" error=\"" << error << "\"/>";
  this->QueueCommandResponse(baseMessageString + ss.str(),PLUS_SUCCESS);
  return PLUS_SUCCESS;
}