/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"
#include "vtkPlusCommandResponse.h"

vtkStandardNewMacro(vtkPlusCommandRTSCommandResponse);
vtkStandardNewMacro(vtkPlusCommandImageMetaDataResponse);
vtkStandardNewMacro(vtkPlusCommandImageResponse);
vtkStandardNewMacro(vtkPlusCommandPolydataResponse);
vtkStandardNewMacro(vtkPlusCommandResponse);
vtkStandardNewMacro(vtkPlusCommandStringResponse);

//----------------------------------------------------------------------------
void vtkPlusCommandRTSCommandResponse::SetParameters(const igtl::MessageBase::MetaDataMap& values)
{
  this->Parameters = values;
}

//----------------------------------------------------------------------------
const igtl::MessageBase::MetaDataMap& vtkPlusCommandRTSCommandResponse::GetParameters() const
{
  return Parameters;
}