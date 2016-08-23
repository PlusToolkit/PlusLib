/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

#include "PlusConfigure.h"
#include "PlusMath.h"
#include "vtkPlusRfProcessor.h"
#include "vtkObjectFactory.h"
#include "vtkPlusRfToBrightnessConvert.h"
#include "vtkPlusUsScanConvertLinear.h"
#include "vtkPlusUsScanConvertCurvilinear.h"
#include "vtkImageData.h"

//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkPlusRfProcessor);
//----------------------------------------------------------------------------
const char* vtkPlusRfProcessor::RF_PROCESSOR_TAG_NAME = "RfProcessing";
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
vtkPlusRfProcessor::vtkPlusRfProcessor()
{
  this->RfToBrightnessConverter=vtkPlusRfToBrightnessConvert::New();
  this->ScanConverter=NULL;  
}

//----------------------------------------------------------------------------
vtkPlusRfProcessor::~vtkPlusRfProcessor()
{
  SetScanConverter(NULL);
  this->RfToBrightnessConverter->Delete();
  this->RfToBrightnessConverter=NULL;  
}

//----------------------------------------------------------------------------
void vtkPlusRfProcessor::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusRfProcessor::SetRfFrame(vtkImageData* rfFrame, US_IMAGE_TYPE imageType)
{
  this->RfToBrightnessConverter->SetInputData(rfFrame);
  this->RfToBrightnessConverter->SetImageType(imageType);
  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
vtkImageData* vtkPlusRfProcessor::GetBrightnessConvertedImage()
{
  this->RfToBrightnessConverter->Update();
  return this->RfToBrightnessConverter->GetOutput();
}

//-----------------------------------------------------------------------------
vtkImageData* vtkPlusRfProcessor::GetBrightnessScanConvertedImage()
{
  if (this->ScanConverter==NULL)
  {
    LOG_ERROR("Scan converter is not defined, skipping scan conversion");
    return GetBrightnessConvertedImage();
  }
  this->ScanConverter->Update();
  return this->ScanConverter->GetOutput();
}

//-----------------------------------------------------------------------------
void vtkPlusRfProcessor::SetScanConverter(vtkPlusUsScanConvert* scanConverter)
{
  if (scanConverter==this->ScanConverter)
  {
    return;
  }
  if (this->ScanConverter!=NULL)
  {
    this->ScanConverter->UnRegister(this);
    this->ScanConverter=NULL;
  }    
  this->ScanConverter=scanConverter;
  if (scanConverter!=NULL)
  {
    this->ScanConverter->Register(this);
    this->ScanConverter->SetInputConnection(this->RfToBrightnessConverter->GetOutputPort());
  }
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusRfProcessor::ReadConfiguration(vtkXMLDataElement* rfProcessingElement)
{
  XML_VERIFY_ELEMENT(rfProcessingElement, "RfProcessing");

  PlusStatus status=PLUS_SUCCESS;

  vtkXMLDataElement* brightnessConversionElement = rfProcessingElement->FindNestedElementWithName("RfToBrightnessConversion"); 
  if (brightnessConversionElement)
  {
    if (this->RfToBrightnessConverter->ReadConfiguration(brightnessConversionElement)!=PLUS_SUCCESS)
    {
      status=PLUS_FAIL;
    }
  }

  // Create a UsScanConversion object of the subclass that corresponds to the TransducerGeometry element
  vtkXMLDataElement* scanConversionElement = rfProcessingElement->FindNestedElementWithName("ScanConversion"); 
  if (scanConversionElement != NULL)
  {
    const char* transducerGeometryStr = scanConversionElement->GetAttribute("TransducerGeometry"); 
    if ( transducerGeometryStr != NULL) 
    {
      if (this->ScanConverter!=NULL && STRCASECMP(transducerGeometryStr, this->ScanConverter->GetTransducerGeometry())==0)
      {
        // we already have a scan converter object of the correct type
      }
      else
      {
        // need to create a new scan converter object
        vtkSmartPointer<vtkPlusUsScanConvertLinear> scanConvertLinear=vtkSmartPointer<vtkPlusUsScanConvertLinear>::New();
        vtkSmartPointer<vtkPlusUsScanConvertCurvilinear> scanConvertCurvilinear=vtkSmartPointer<vtkPlusUsScanConvertCurvilinear>::New();
        if (STRCASECMP(transducerGeometryStr, scanConvertLinear->GetTransducerGeometry())==0)
        {
          SetScanConverter(scanConvertLinear);
        }
        else if (STRCASECMP(transducerGeometryStr, scanConvertCurvilinear->GetTransducerGeometry())==0)
        {
          SetScanConverter(scanConvertCurvilinear);
        }
        else
        {
          LOG_ERROR("Failed to create scan converter for geometry: "<<transducerGeometryStr);
          SetScanConverter(NULL);
          status=PLUS_FAIL;
        }
      }
      if (this->ScanConverter != NULL && this->ScanConverter->ReadConfiguration(scanConversionElement)!=PLUS_SUCCESS)
      {
        status=PLUS_FAIL;
      }
    }
    else
    {
      LOG_ERROR("TransducerGeometry attribute of scan conversion is missing, no scan conversion will be performed");
      SetScanConverter(NULL);
      status=PLUS_FAIL;
    }
  }  
  else
  {
    LOG_WARNING("ScanConversion element is missing, no scan conversion will be performed");
    SetScanConverter(NULL);
  }

  return status;
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusRfProcessor::WriteConfiguration(vtkXMLDataElement* rfElement)
{
  // RfProcessing = rfElement
  //  - BrightnessConversion
  //  - ScanConversion

  XML_VERIFY_ELEMENT(rfElement, "RfProcessing");
  XML_FIND_NESTED_ELEMENT_CREATE_IF_MISSING(brightnessConversionElement, rfElement, "RfToBrightnessConversion");
  XML_FIND_NESTED_ELEMENT_CREATE_IF_MISSING(scanConversionElement, rfElement, "ScanConversion");

  PlusStatus status(PLUS_SUCCESS);

  if ( this->RfToBrightnessConverter->WriteConfiguration(brightnessConversionElement) != PLUS_SUCCESS )
  {
    status = PLUS_FAIL;
  }  

  if (this->ScanConverter != NULL)
  {
    if ( this->ScanConverter->WriteConfiguration(scanConversionElement) != PLUS_SUCCESS)
    {
      status = PLUS_FAIL;
    }
  }
  else
  {
    scanConversionElement->SetAttribute("TransducerGeometry", "UNKNOWN"); 
    LOG_ERROR("Unknown transducer geometry");
    status = PLUS_FAIL;
  }

  return status;
}

//-----------------------------------------------------------------------------
const char* vtkPlusRfProcessor::GetRfProcessorTagName()
{
  return vtkPlusRfProcessor::RF_PROCESSOR_TAG_NAME;
}
