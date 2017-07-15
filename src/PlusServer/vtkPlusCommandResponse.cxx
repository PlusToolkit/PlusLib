/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"
#include "vtkPlusCommandResponse.h"

vtkStandardNewMacro(vtkPlusCommandCommandResponse);
vtkStandardNewMacro(vtkPlusCommandImageMetaDataResponse);
vtkStandardNewMacro(vtkPlusCommandImageResponse);
vtkStandardNewMacro(vtkPlusCommandPolydataResponse);
vtkStandardNewMacro(vtkPlusCommandResponse);
vtkStandardNewMacro(vtkPlusCommandStringResponse);

//----------------------------------------------------------------------------
void vtkPlusCommandCommandResponse::SetParameters(const std::map<std::string, std::string>& values)
{
  this->Parameters = values;
}

//----------------------------------------------------------------------------
const std::map<std::string, std::string>& vtkPlusCommandCommandResponse::GetParameters() const
{
  return Parameters;
}