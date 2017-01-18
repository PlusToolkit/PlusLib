/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

// Local includes
#include "PlusConfigure.h"
#include "PlusCommon.h"
#include "PlusMath.h"
#include "PlusTrackedFrame.h"
#include "vtkPlusUsScanConvertCurvilinear.h"
#include "vtkPlusUsScanConvertLinear.h"
#include "vtkPlusBoneEnhancer.h"
#ifdef PLUS_USE_INTEL_MKL
#include "vtkPlusForoughiBoneSurfaceProbability.h"
#else
#include <vtkImageThreshold.h>
#endif

// VTK includes
#include <vtkObjectFactory.h>
#include <vtkImageCast.h>

//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkPlusBoneEnhancer);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
vtkPlusBoneEnhancer::vtkPlusBoneEnhancer()
#ifdef PLUS_USE_INTEL_MKL
  : CastToDouble(vtkSmartPointer<vtkImageCast>::New())
  , CastToUnsignedChar(vtkSmartPointer<vtkImageCast>::New())
  , BoneSurfaceFilter(vtkSmartPointer<vtkPlusForoughiBoneSurfaceProbability>::New())
#else
  : Thresholder(vtkSmartPointer<vtkImageThreshold>::New())
#endif
{
#ifdef PLUS_USE_INTEL_MKL
  this->CastToDouble->SetOutputScalarTypeToDouble();
  this->CastToUnsignedChar->SetOutputScalarTypeToUnsignedChar();
  this->BoneSurfaceFilter->SetInputConnection(this->CastToDouble->GetOutputPort());
  this->CastToUnsignedChar->SetInputConnection(this->BoneSurfaceFilter->GetOutputPort());
#else
  this->SetThreshold(128);
  this->Thresholder->SetInValue(20);
  this->Thresholder->SetOutValue(200);
#endif
}

//----------------------------------------------------------------------------
vtkPlusBoneEnhancer::~vtkPlusBoneEnhancer()
{
}

//----------------------------------------------------------------------------
void vtkPlusBoneEnhancer::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusBoneEnhancer::ReadConfiguration(vtkXMLDataElement* processingElement)
{
  XML_VERIFY_ELEMENT(processingElement, this->GetTagName());
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(double, Threshold, processingElement);

  this->ScanConverter = NULL;
  vtkXMLDataElement* scanConversionElement = processingElement->FindNestedElementWithName("ScanConversion");
  if (scanConversionElement != NULL)
  {
    // Call scanline generator with appropriate scanconvert
    const char* transducerGeometry = scanConversionElement->GetAttribute("TransducerGeometry");
    if (transducerGeometry == NULL)
    {
      LOG_ERROR("Scan converter TransducerGeometry is undefined");
      return PLUS_FAIL;
    }
    vtkSmartPointer<vtkPlusUsScanConvert> scanConverter;
    if (STRCASECMP(transducerGeometry, "CURVILINEAR") == 0)
    {
      this->ScanConverter = vtkSmartPointer<vtkPlusUsScanConvert>::Take(vtkPlusUsScanConvertCurvilinear::New());
    }
    else if (STRCASECMP(transducerGeometry, "LINEAR") == 0)
    {
      this->ScanConverter = vtkSmartPointer<vtkPlusUsScanConvert>::Take(vtkPlusUsScanConvertLinear::New());
    }
    else
    {
      LOG_ERROR("Invalid scan converter TransducerGeometry: " << transducerGeometry);
      return PLUS_FAIL;
    }
    this->ScanConverter->ReadConfiguration(scanConversionElement);
  }

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusBoneEnhancer::WriteConfiguration(vtkXMLDataElement* processingElement)
{
  XML_VERIFY_ELEMENT(processingElement, this->GetTagName());
  processingElement->SetDoubleAttribute("Threshold", this->GetThreshold());
  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
void vtkPlusBoneEnhancer::SetThreshold(double threshold)
{
#ifndef PLUS_USE_INTEL_MKL
  this->Thresholder->ThresholdByLower(threshold);
#endif
}

//-----------------------------------------------------------------------------
double vtkPlusBoneEnhancer::GetThreshold()
{
#ifdef PLUS_USE_INTEL_MKL
  return 0;
#else
  return this->Thresholder->GetLowerThreshold();
#endif
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusBoneEnhancer::ProcessFrame(PlusTrackedFrame* inputFrame, PlusTrackedFrame* outputFrame)
{
  // Get input image
  PlusVideoFrame* inputImage = inputFrame->GetImageData();

#ifdef PLUS_USE_INTEL_MKL
  // Generate output image
  this->CastToDouble->SetInputData(inputImage->GetImage());
  this->CastToUnsignedChar->Update();
  // Write output image
  PlusVideoFrame* outputImage = outputFrame->GetImageData();
  outputImage->DeepCopyFrom(this->CastToUnsignedChar->GetOutput());
#else
  // Generate output image
  //  1. threshold the image
  this->Thresholder->SetInputData(inputImage->GetImage());
  this->Thresholder->Update();

  int* rfImageExtent = this->ScanConverter->GetInputImageExtent();
  int numOfSamplesPerScanline = rfImageExtent[1] - rfImageExtent[0] + 1;
  int numOfScanlines = rfImageExtent[3] - rfImageExtent[2] + 1;

  //  2. draw scanlines on it
  if (this->ScanConverter.GetPointer() != NULL)
  {
    PlusCommon::PixelLineList lines;
    for (int scanLine = 0; scanLine < rfImageExtent[3] - rfImageExtent[2] + 1; scanLine++)
    {
      double start[4] = { 0 };
      double end[4] = { 0 };
      this->ScanConverter->GetScanLineEndPoints(scanLine, start, end);
      PlusCommon::PixelPoint startPoint = { static_cast<unsigned int>(std::round(start[0])), static_cast<unsigned int>(std::round(start[1])), static_cast<unsigned int>(std::round(start[2])) };
      PlusCommon::PixelPoint endPoint = { static_cast<unsigned int>(std::round(end[0])), static_cast<unsigned int>(std::round(end[1])), static_cast<unsigned int>(std::round(end[2])) };
      lines.push_back(PlusCommon::PixelLine(startPoint, endPoint));
    }

    if (!lines.empty())
    {
      PlusCommon::DrawScanLines(rfImageExtent, 255, lines, this->Thresholder->GetOutput());
    }
  }

  // Write output image
  PlusVideoFrame* outputImage = outputFrame->GetImageData();
  outputImage->DeepCopyFrom(this->Thresholder->GetOutput());
#endif

  return PLUS_SUCCESS;
}