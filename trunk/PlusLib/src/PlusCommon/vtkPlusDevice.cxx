/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"
#include "vtkPlusDevice.h"

#include "vtkMultiThreader.h" 

//----------------------------------------------------------------------------
vtkPlusDevice::vtkPlusDevice()
{
  this->Recording = 0;
  this->AcquisitionRate = -1.0; 

  this->Threader = vtkMultiThreader::New();
  this->ThreadId = -1;
}

//----------------------------------------------------------------------------
vtkPlusDevice::~vtkPlusDevice()
{
  DELETE_IF_NOT_NULL(this->Threader); 
}

//----------------------------------------------------------------------------
void vtkPlusDevice::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);

  os << indent << "SDK version: " << this->GetSdkVersion() << "\n";
  os << indent << "AcquisitionRate: " << this->AcquisitionRate << "\n";
  os << indent << "Recording: " << (this->Recording ? "On\n" : "Off\n");
}

//----------------------------------------------------------------------------
std::string vtkPlusDevice::GetSdkVersion()
{
  // Base class version is the same as the Plus library version
  std::string ver = std::string("Plus-") + std::string(PLUSLIB_VERSION); 
  return ver; 
}

//----------------------------------------------------------------------------
bool vtkPlusDevice::IsResettable()
{
  // By default, devices cannot be reset.
  return false;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusDevice::Reset()
{
  // By default, do nothing
  return PLUS_SUCCESS;
}
