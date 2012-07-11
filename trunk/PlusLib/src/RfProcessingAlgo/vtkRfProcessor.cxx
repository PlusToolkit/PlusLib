/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/ 

#include "PlusMath.h"
#include "vtkRfProcessor.h"
#include "vtkObjectFactory.h"
#include "vtkRfToBrightnessConvert.h"
#include "vtkUsScanConvert.h"
#include "vtkImageData.h"
#include "vtkImageCast.h"

//----------------------------------------------------------------------------
vtkCxxRevisionMacro(vtkRfProcessor, "$Revision: 1.0 $");
vtkStandardNewMacro(vtkRfProcessor); 

//----------------------------------------------------------------------------
vtkRfProcessor::vtkRfProcessor()
{
  this->RfToBrightnessConverter=vtkRfToBrightnessConvert::New();
  this->ImageCaster=vtkImageCast::New();
  this->ScanConverter=vtkUsScanConvert::New();  
  
  this->ImageCaster->SetOutputScalarType(VTK_UNSIGNED_CHAR);
  this->ImageCaster->SetInputConnection(this->RfToBrightnessConverter->GetOutputPort());
  this->ScanConverter->SetInputConnection(this->ImageCaster->GetOutputPort());
}

//----------------------------------------------------------------------------
vtkRfProcessor::~vtkRfProcessor()
{
  this->ScanConverter->Delete();
  this->ScanConverter=NULL;
  this->ImageCaster->Delete();
  this->ImageCaster=NULL;  
  this->RfToBrightnessConverter->Delete();
  this->RfToBrightnessConverter=NULL;  
}

//----------------------------------------------------------------------------
void vtkRfProcessor::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}

//-----------------------------------------------------------------------------
PlusStatus vtkRfProcessor::SetRfFrame(vtkImageData* rfFrame, US_IMAGE_TYPE imageType)
{
  this->RfToBrightnessConverter->SetInput(rfFrame);
  this->RfToBrightnessConverter->SetImageType(imageType);
  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
vtkImageData* vtkRfProcessor::GetBrightessConvertedImage()
{
  this->ImageCaster->Update();
  return this->ImageCaster->GetOutput();
}

//-----------------------------------------------------------------------------
vtkImageData* vtkRfProcessor::GetBrightessScanConvertedImage()
{
  this->ScanConverter->Update();
  //return this->RfToBrightnessConverter->GetOutput();
  return this->ScanConverter->GetOutput();
}

//-----------------------------------------------------------------------------
PlusStatus vtkRfProcessor::ReadConfiguration(vtkXMLDataElement* config)
{
  PlusStatus status=PLUS_SUCCESS;
  if (this->RfToBrightnessConverter->ReadConfiguration(config)!=PLUS_SUCCESS)
  {
    status=PLUS_FAIL;
  }
  if (this->ScanConverter->ReadConfiguration(config)!=PLUS_SUCCESS)
  {
    status=PLUS_FAIL;
  }
  return status;
}
