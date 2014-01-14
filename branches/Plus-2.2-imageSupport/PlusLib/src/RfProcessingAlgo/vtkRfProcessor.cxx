/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

#include "PlusMath.h"
#include "PlusXmlUtils.h"
#include "vtkRfProcessor.h"
#include "vtkObjectFactory.h"
#include "vtkRfToBrightnessConvert.h"
#include "vtkUsScanConvertLinear.h"
#include "vtkUsScanConvertCurvilinear.h"
#include "vtkImageData.h"

//----------------------------------------------------------------------------
vtkCxxRevisionMacro(vtkRfProcessor, "$Revision: 1.1 $");
vtkStandardNewMacro(vtkRfProcessor);
//----------------------------------------------------------------------------
const char* vtkRfProcessor::RF_PROCESSOR_TAG_NAME = "RfProcessing";
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
vtkRfProcessor::vtkRfProcessor()
{
  this->RfToBrightnessConverter=vtkRfToBrightnessConvert::New();
  this->ScanConverter=NULL;  
}

//----------------------------------------------------------------------------
vtkRfProcessor::~vtkRfProcessor()
{
  SetScanConverter(NULL);
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
  this->RfToBrightnessConverter->Update();
  return this->RfToBrightnessConverter->GetOutput();
}

//-----------------------------------------------------------------------------
vtkImageData* vtkRfProcessor::GetBrightessScanConvertedImage()
{
  if (this->ScanConverter==NULL)
  {
    LOG_ERROR("Scan converter is not defined, skipping scan conversion");
    return GetBrightessConvertedImage();
  }
  this->ScanConverter->Update();
  return this->ScanConverter->GetOutput();
}

//-----------------------------------------------------------------------------
void vtkRfProcessor::SetScanConverter(vtkUsScanConvert* scanConverter)
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
PlusStatus vtkRfProcessor::ReadConfiguration(vtkXMLDataElement* rfProcessingElement)
{
  if ( rfProcessingElement == NULL )
  {
    LOG_DEBUG("Unable to configure vtkRfProcessor! (XML data element is NULL)"); 
    return PLUS_FAIL; 
  }

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
        vtkSmartPointer<vtkUsScanConvertLinear> scanConvertLinear=vtkSmartPointer<vtkUsScanConvertLinear>::New();
        vtkSmartPointer<vtkUsScanConvertCurvilinear> scanConvertCurvilinear=vtkSmartPointer<vtkUsScanConvertCurvilinear>::New();
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
PlusStatus vtkRfProcessor::WriteConfiguration(vtkXMLDataElement* rfElement)
{
  if ( rfElement == NULL )
  {
    LOG_DEBUG("Unable to configure vtkRfProcessor! (XML data element is NULL)"); 
    return PLUS_FAIL; 
  }

  // RfProcessing
  //  - BrightnessConversion
  //  - ScanConversion
  vtkXMLDataElement* brightnessConversionElement = PlusXmlUtils::GetNestedElementWithName(rfElement, "RfToBrightnessConversion"); 
  if (brightnessConversionElement == NULL) 
  {  
    return PLUS_FAIL;
  }  
  vtkXMLDataElement* scanConversionElement = PlusXmlUtils::GetNestedElementWithName(rfElement, "ScanConversion"); 
  if (scanConversionElement == NULL) 
  {  
    return PLUS_FAIL;
  }  

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
const char* vtkRfProcessor::GetRfProcessorTagName()
{
  return vtkRfProcessor::RF_PROCESSOR_TAG_NAME;
}
