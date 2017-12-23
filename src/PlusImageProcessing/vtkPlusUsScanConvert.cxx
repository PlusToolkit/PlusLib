/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"

#include "vtkPlusUsScanConvert.h"

#include "vtkObjectFactory.h"


//----------------------------------------------------------------------------
vtkPlusUsScanConvert::vtkPlusUsScanConvert()
{
  this->TransducerName = NULL;
  this->InputImageExtent[0] = 0;
  this->InputImageExtent[1] = -1;
  this->InputImageExtent[2] = 0;
  this->InputImageExtent[3] = -1;
  this->InputImageExtent[4] = 0; // not used
  this->InputImageExtent[5] = -1; // not used
  this->OutputImageExtent[0] = 0;
  this->OutputImageExtent[1] = 599;
  this->OutputImageExtent[2] = 0;
  this->OutputImageExtent[3] = 799;
  this->OutputImageExtent[4] = 0; // not used
  this->OutputImageExtent[5] = 0; // not used
  this->OutputImageSpacing[0] = 0.2;
  this->OutputImageSpacing[1] = 0.2;
  this->OutputImageSpacing[2] = 1.0; // not used
  this->TransducerCenterPixelSpecified = false;
  this->TransducerCenterPixel[0] = 0;
  this->TransducerCenterPixel[1] = 0;
}

//----------------------------------------------------------------------------
vtkPlusUsScanConvert::~vtkPlusUsScanConvert()
{
  SetTransducerName(NULL);
}

void vtkPlusUsScanConvert::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
  os << indent << "TransducerName: (" << (this->TransducerName == NULL ? "" : this->TransducerName) << "\n";
  os << indent << "OutputImageExtent: ("
     << this->OutputImageExtent[0] << ", " << this->OutputImageExtent[1] << ", "
     << this->OutputImageExtent[2] << ", " << this->OutputImageExtent[3] << ")\n";
  os << indent << "OutputImageSpacing: (" << this->OutputImageSpacing[0] << ", " << this->OutputImageSpacing[1] << ")\n";
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusUsScanConvert::ReadConfiguration(vtkXMLDataElement* scanConversionElement)
{
  LOG_TRACE("vtkPlusUsScanConvert::ReadConfiguration");
  XML_VERIFY_ELEMENT(scanConversionElement, "ScanConversion");

  const char* transducerGeometry = scanConversionElement->GetAttribute("TransducerGeometry");
  if (transducerGeometry == NULL)
  {
    LOG_ERROR("Cannot read vtkPlusUsScanConvert configuration: TransducerGeometry is unknown");
    return PLUS_FAIL;
  }
  if (STRCASECMP(transducerGeometry, this->GetTransducerGeometry()) != 0)
  {
    LOG_ERROR("Cannot read vtkPlusUsScanConvert configuration: TransducerGeometry is expected to be " << this->GetTransducerGeometry()
              << ", but found " << transducerGeometry << " instead");
    return PLUS_FAIL;
  }

  XML_READ_CSTRING_ATTRIBUTE_OPTIONAL(TransducerName, scanConversionElement);

  double outputImageSpacing[2] = {0};
  if (scanConversionElement->GetVectorAttribute("OutputImageSpacingMmPerPixel", 2, outputImageSpacing))
  {
    this->OutputImageSpacing[0] = outputImageSpacing[0];
    this->OutputImageSpacing[1] = outputImageSpacing[1];
    this->OutputImageSpacing[2] = 1;
  }

  int outputImageSize[2] = {0};
  if (scanConversionElement->GetVectorAttribute("OutputImageSizePixel", 2, outputImageSize))
  {
    this->OutputImageExtent[0] = 0;
    this->OutputImageExtent[1] = outputImageSize[0] - 1;
    this->OutputImageExtent[2] = 0;
    this->OutputImageExtent[3] = outputImageSize[1] - 1;
    this->OutputImageExtent[4] = 0;
    this->OutputImageExtent[5] = 0;
  }

  double transducerCenterPixel[2] = {0};
  if (scanConversionElement->GetVectorAttribute("TransducerCenterPixel", 2, transducerCenterPixel))
  {
    this->TransducerCenterPixelSpecified = true;
    this->TransducerCenterPixel[0] = transducerCenterPixel[0];
    this->TransducerCenterPixel[1] = transducerCenterPixel[1];
  }

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusUsScanConvert::WriteConfiguration(vtkXMLDataElement* scanConversionElement)
{
  LOG_TRACE("vtkPlusUsScanConvert::WriteConfiguration");

  XML_VERIFY_ELEMENT(scanConversionElement, "ScanConversion");

  scanConversionElement->SetAttribute("TransducerGeometry", GetTransducerGeometry());
  scanConversionElement->SetAttribute("TransducerName", this->TransducerName);
  scanConversionElement->SetVectorAttribute("OutputImageSpacingMmPerPixel", 2, this->OutputImageSpacing);

  int outputImageSize[2] =
  {
    this->OutputImageExtent[1] - this->OutputImageExtent[0] + 1,
    this->OutputImageExtent[3] - this->OutputImageExtent[2] + 1
  };
  scanConversionElement->SetVectorAttribute("OutputImageSizePixel", 2, outputImageSize);

  if (this->TransducerCenterPixelSpecified)
  {
    scanConversionElement->SetVectorAttribute("TransducerCenterPixel", 2, this->TransducerCenterPixel);
  }

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
FrameSizeType vtkPlusUsScanConvert::GetOutputImageSizePixel()
{
  FrameSizeType frameSize = {static_cast<unsigned int>(this->OutputImageExtent[1] - this->OutputImageExtent[0] + 1),
                             static_cast<unsigned int>(this->OutputImageExtent[3] - this->OutputImageExtent[2] + 1),
                             static_cast<unsigned int>(this->OutputImageExtent[5] - this->OutputImageExtent[4] + 1)
                            };
  return frameSize;
}
