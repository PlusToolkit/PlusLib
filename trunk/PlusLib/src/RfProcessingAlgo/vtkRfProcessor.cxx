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
vtkCxxRevisionMacro(vtkRfProcessor, "$Revision: 1.0 $");
vtkStandardNewMacro(vtkRfProcessor); 

//----------------------------------------------------------------------------
vtkRfProcessor::vtkRfProcessor()
{
  this->TransducerGeometry=TRANSDUCER_UNKNOWN;
  this->RfToBrightnessConverter=vtkRfToBrightnessConvert::New();
  this->ScanConverterLinear=vtkUsScanConvertLinear::New();  
  this->ScanConverterCurvilinear=vtkUsScanConvertCurvilinear::New();  

  // Set the brightness input to both scan converters.
  // Only that will actually perform the scan conversion computation that will be asked for output.
  this->ScanConverterLinear->SetInputConnection(this->RfToBrightnessConverter->GetOutputPort());
  this->ScanConverterCurvilinear->SetInputConnection(this->RfToBrightnessConverter->GetOutputPort());  

  this->TransducerName = NULL;
}

//----------------------------------------------------------------------------
vtkRfProcessor::~vtkRfProcessor()
{
  this->ScanConverterCurvilinear->Delete();
  this->ScanConverterCurvilinear=NULL;
  this->ScanConverterLinear->Delete();
  this->ScanConverterLinear=NULL;
  this->RfToBrightnessConverter->Delete();
  this->RfToBrightnessConverter=NULL;  
  if(this->TransducerName)
  {
    delete [] this->TransducerName;
    this->TransducerName = NULL;
  }
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
void vtkRfProcessor::SetStartDepthMm(double startDepth)
{
  this->ScanConverterLinear->SetOutputImageStartDepthMm(startDepth);
  this->ScanConverterCurvilinear->SetOutputImageStartDepthMm(startDepth);
}

//-----------------------------------------------------------------------------
void vtkRfProcessor::SetStopDepthMm(double stopDepth)
{
  this->ScanConverterLinear->SetImagingDepthMm(stopDepth);
  this->ScanConverterCurvilinear->SetRadiusStopMm(stopDepth);
}

//-----------------------------------------------------------------------------
void vtkRfProcessor::SetStartAngleDeg(double startAngle) 
{
  this->ScanConverterCurvilinear->SetThetaStartDeg(startAngle);
}

//-----------------------------------------------------------------------------
void vtkRfProcessor::SetStopAngleDeg(double stopAngle)
{
  this->ScanConverterCurvilinear->SetThetaStopDeg(stopAngle);
}

//-----------------------------------------------------------------------------
void vtkRfProcessor::SetRadiusOfCurvatureMm(double roc)
{
  this->ScanConverterCurvilinear->SetRadiusStartMm(roc);
}

//-----------------------------------------------------------------------------
void vtkRfProcessor::SetTransducerWidthMm(double tw)
{
  this->ScanConverterLinear->SetTransducerWidthMm(tw);
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
  if (rfProcessingElement == NULL)
  {
    LOG_DEBUG("Unable to find RfProcessing element in XML tree, will use default values!");    
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
        if (this->ScanConverterLinear->ReadConfiguration(scanConversionElement)!=PLUS_SUCCESS)
        {
          status=PLUS_FAIL;
        }
      }
      else if (STRCASECMP(transducerGeometryStr, "CURVILINEAR")==0)
      {
        LOG_DEBUG("Transducer geometry set: Curvilinear"); 
        SetTransducerGeometry(TRANSDUCER_CURVILINEAR); 
        if (this->ScanConverterCurvilinear->ReadConfiguration(scanConversionElement)!=PLUS_SUCCESS)
        {
          status=PLUS_FAIL;
        }
      }
      else
      {
        LOG_ERROR("Unsupported transducer geometry requested: "<<transducerGeometryStr);
        SetTransducerGeometry(TRANSDUCER_UNKNOWN); 
      }
    }

    const char* transducerNameStr = scanConversionElement->GetAttribute("TransducerName"); 
    if ( transducerNameStr != NULL) 
    {
      this->SetTransducerName(transducerNameStr);
    }
  }  

  return status;
}

//-----------------------------------------------------------------------------
PlusStatus vtkRfProcessor::WriteConfiguration(vtkXMLDataElement* config)
{
  if ( config == NULL )
  {
    LOG_DEBUG("Unable to configure vtkRfProcessor! (XML data element is NULL)"); 
    return PLUS_FAIL; 
  }

  // RfProcessing
  //  - BrightnessConversion
  //  - ScanConversion
  vtkXMLDataElement* rfProcessingElement = PlusXmlUtils::GetNestedElementWithName(config, "RfProcessing"); 
  if (rfProcessingElement == NULL) 
  {  
    return PLUS_FAIL;
  }
  vtkXMLDataElement* brightnessConversionElement = PlusXmlUtils::GetNestedElementWithName(rfProcessingElement, "RfToBrightnessConversion"); 
  if (brightnessConversionElement == NULL) 
  {  
    return PLUS_FAIL;
  }  
  vtkXMLDataElement* scanConversionElement = PlusXmlUtils::GetNestedElementWithName(rfProcessingElement, "ScanConversion"); 
  if (scanConversionElement == NULL) 
  {  
    return PLUS_FAIL;
  }  

  PlusStatus status=PLUS_SUCCESS;

  if (this->RfToBrightnessConverter->WriteConfiguration(brightnessConversionElement)!=PLUS_SUCCESS)
  {
    status=PLUS_FAIL;
  }  

  switch (this->TransducerGeometry)
  {
  case TRANSDUCER_LINEAR:
    scanConversionElement->SetAttribute("TransducerGeometry", "LINEAR"); 
    if (this->ScanConverterLinear->WriteConfiguration(scanConversionElement)!=PLUS_SUCCESS)
    {
      status=PLUS_FAIL;
    }
    break;
  case TRANSDUCER_CURVILINEAR:
    scanConversionElement->SetAttribute("TransducerGeometry", "CURVILINEAR"); 
    if (this->ScanConverterCurvilinear->WriteConfiguration(scanConversionElement)!=PLUS_SUCCESS)
    {
      status=PLUS_FAIL;
    }
    break;
  case TRANSDUCER_UNKNOWN:
    scanConversionElement->SetAttribute("TransducerGeometry", "UNKNOWN"); 
    break;
  default:
    scanConversionElement->SetAttribute("TransducerGeometry", "UNKNOWN"); 
    LOG_ERROR("Unknown transducer geometry: "<<this->TransducerGeometry);
    status=PLUS_FAIL;
  }

  scanConversionElement->SetAttribute("TransducerName", this->TransducerName);

  return status;
}

PlusStatus vtkRfProcessor::GetOutputImageSizePixel(int imageSize[2])
{
  int extent[6]={0,0,0,0,0,0};
  switch (this->TransducerGeometry)
  {
  case TRANSDUCER_LINEAR:    
    this->ScanConverterLinear->GetOutputImageExtent(extent);
    break;
  case TRANSDUCER_CURVILINEAR:
    this->ScanConverterCurvilinear->GetOutputImageExtent(extent);
    break;
  default:
    LOG_ERROR("Unknown transducer geometry: "<<this->TransducerGeometry);
    return PLUS_FAIL;
  }
  imageSize[0]=extent[1]-extent[0]+1;
  imageSize[1]=extent[3]-extent[2]+1;
  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkRfProcessor::GetImagingDepthMm(double& imagingDepthMm)
{
  switch (this->TransducerGeometry)
  {
  case TRANSDUCER_LINEAR:    
    imagingDepthMm=this->ScanConverterLinear->GetImagingDepthMm();
    return PLUS_SUCCESS;
  case TRANSDUCER_CURVILINEAR:
    imagingDepthMm=fabs(this->ScanConverterCurvilinear->GetRadiusStartMm()-this->ScanConverterCurvilinear->GetRadiusStopMm());
    return PLUS_SUCCESS;
  default:
    LOG_ERROR("Unknown transducer geometry: "<<this->TransducerGeometry);
    return PLUS_FAIL;
  }
}

//-----------------------------------------------------------------------------
PlusStatus vtkRfProcessor::GetOutputImageSpacingMm(double imageSpacing[2])
{
  double spacing3[3]={1.0,1.0,1.0};
  switch (this->TransducerGeometry)
  {
  case TRANSDUCER_LINEAR:    
    this->ScanConverterLinear->GetOutputImageSpacing(spacing3);
    break;
  case TRANSDUCER_CURVILINEAR:
    this->ScanConverterCurvilinear->GetOutputImageSpacing(spacing3);
    break;
  default:
    LOG_ERROR("Unknown transducer geometry: "<<this->TransducerGeometry);
    return PLUS_FAIL;
  }
  imageSpacing[0]=spacing3[0];
  imageSpacing[1]=spacing3[1];
  return PLUS_SUCCESS;
}
