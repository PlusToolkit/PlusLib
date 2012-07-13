/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/ 

#include "PlusMath.h"
#include "vtkRfProcessor.h"
#include "vtkObjectFactory.h"
#include "vtkRfToBrightnessConvert.h"
#include "vtkUsScanConvertLinear.h"
#include "vtkUsScanConvertCurvilinear.h"
#include "vtkImageData.h"
#include "vtkImageCast.h"

//----------------------------------------------------------------------------
vtkCxxRevisionMacro(vtkRfProcessor, "$Revision: 1.0 $");
vtkStandardNewMacro(vtkRfProcessor); 

//----------------------------------------------------------------------------
vtkRfProcessor::vtkRfProcessor()
{
  this->TransducerGeometry=TRANSDUCER_UNKNOWN;
  this->RfToBrightnessConverter=vtkRfToBrightnessConvert::New();
  this->ImageCaster=vtkImageCast::New();
  this->ScanConverterLinear=vtkUsScanConvertLinear::New();  
  this->ScanConverterCurvilinear=vtkUsScanConvertCurvilinear::New();  
  
  this->ImageCaster->SetOutputScalarType(VTK_UNSIGNED_CHAR);
  this->ImageCaster->SetInputConnection(this->RfToBrightnessConverter->GetOutputPort());
  
  // Set the brightness input to both scan converters.
  // Only that will actually perform the scan conversion computation that will be asked for output.
  this->ScanConverterLinear->SetInputConnection(this->ImageCaster->GetOutputPort());
  this->ScanConverterCurvilinear->SetInputConnection(this->ImageCaster->GetOutputPort());  
}

//----------------------------------------------------------------------------
vtkRfProcessor::~vtkRfProcessor()
{
  this->ScanConverterCurvilinear->Delete();
  this->ScanConverterCurvilinear=NULL;
  this->ScanConverterLinear->Delete();
  this->ScanConverterLinear=NULL;
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
  switch (this->TransducerGeometry)
  {
  case TRANSDUCER_LINEAR:
    this->ScanConverterLinear->Update();
    return this->ScanConverterLinear->GetOutput();
  case TRANSDUCER_CURVILINEAR:
    this->ScanConverterCurvilinear->Update();
    return this->ScanConverterCurvilinear->GetOutput();
  default:
    LOG_ERROR("Unknown transducer geometry: "<<this->TransducerGeometry<<", skipping scan conversion");
    return GetBrightessConvertedImage();
  }
}

//-----------------------------------------------------------------------------
PlusStatus vtkRfProcessor::ReadConfiguration(vtkXMLDataElement* config)
{
  if ( config == NULL )
  {
    LOG_DEBUG("Unable to configure vtkRfProcessor! (XML data element is NULL)"); 
    return PLUS_FAIL; 
  }
  vtkXMLDataElement* rfProcessingElement = config->FindNestedElementWithName("RfProcessing"); 
  if (rfProcessingElement != NULL)
  {
    vtkXMLDataElement* scanConversionElement = rfProcessingElement->FindNestedElementWithName("ScanConversion"); 
    if (scanConversionElement != NULL)
    {
      const char* transducerGeometryStr = scanConversionElement->GetAttribute("TransducerGeometry"); 
      if ( transducerGeometryStr != NULL) 
      {
        if (STRCASECMP(transducerGeometryStr, "LINEAR")==0)
        {
          LOG_DEBUG("Transducer geometry set: Linear"); 
          SetTransducerGeometry(TRANSDUCER_LINEAR); 
        }
        else if (STRCASECMP(transducerGeometryStr, "CURVILINEAR")==0)
        {
          LOG_DEBUG("Transducer geometry set: Curvilinear"); 
          SetTransducerGeometry(TRANSDUCER_CURVILINEAR); 
        }
        else
        {
          LOG_ERROR("Unsupported transducer geometry requested: "<<transducerGeometryStr);
          SetTransducerGeometry(TRANSDUCER_UNKNOWN); 
        }
      }
    }  
  }
  else
  {
    LOG_DEBUG("Unable to find RfProcessing element in XML tree, will use default values!");     
  }

  PlusStatus status=PLUS_SUCCESS;
  if (this->RfToBrightnessConverter->ReadConfiguration(config)!=PLUS_SUCCESS)
  {
    status=PLUS_FAIL;
  }
  if (this->ScanConverterLinear->ReadConfiguration(config)!=PLUS_SUCCESS)
  {
    status=PLUS_FAIL;
  }
  if (this->ScanConverterCurvilinear->ReadConfiguration(config)!=PLUS_SUCCESS)
  {
    status=PLUS_FAIL;
  }
  
  return status;
}
