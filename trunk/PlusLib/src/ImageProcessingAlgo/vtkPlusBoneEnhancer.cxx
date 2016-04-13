/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

#include "PlusConfigure.h"
#include "PlusMath.h"
#include "PlusTrackedFrame.h"
#include "vtkPlusBoneEnhancer.h"
#ifdef PLUS_USE_INTEL_MKL
  #include "vtkPlusForoughiBoneSurfaceProbability.h"
#else
  #include "vtkImageThreshold.h"
#endif
#include "vtkObjectFactory.h"
#include "vtkPlusUsScanConvertCurvilinear.h"
#include "vtkPlusUsScanConvertLinear.h"
#include "vtkImageCast.h"

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
  this->Superclass::PrintSelf(os,indent);
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
    if (transducerGeometry==NULL)
    {
      LOG_ERROR("Scan converter TransducerGeometry is undefined");
      return PLUS_FAIL;
    }
    vtkSmartPointer<vtkPlusUsScanConvert> scanConverter;
    if (STRCASECMP(transducerGeometry,"CURVILINEAR") == 0)
    {
      this->ScanConverter = vtkSmartPointer<vtkPlusUsScanConvert>::Take(vtkPlusUsScanConvertCurvilinear::New());
    }
    else if (STRCASECMP(transducerGeometry, "LINEAR") == 0)
    {
      this->ScanConverter = vtkSmartPointer<vtkPlusUsScanConvert>::Take(vtkPlusUsScanConvertLinear::New());
    }
    else
    {
      LOG_ERROR("Invalid scan converter TransducerGeometry: "<<transducerGeometry);
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

//----------------------------------------------------------------------------
void vtkPlusBoneEnhancer::DrawLine(vtkImageData* imageData, int* imageExtent, double* start, double* end, int numberOfPoints)
{
  const float DRAWING_COLOR = 255;
  double directionVectorX = static_cast<double>(end[0]-start[0])/(numberOfPoints-1);
  double directionVectorY = static_cast<double>(end[1]-start[1])/(numberOfPoints-1);
  for (int pointIndex=0; pointIndex<numberOfPoints; ++pointIndex)
  {
    int pixelCoordX = start[0] + directionVectorX * pointIndex;
    int pixelCoordY = start[1] + directionVectorY * pointIndex;
    if (pixelCoordX<imageExtent[0] ||  pixelCoordX>imageExtent[1]
    || pixelCoordY<imageExtent[2] ||  pixelCoordY>imageExtent[3])
    {
      // outside of the specified extent
      continue;
    }
    imageData->SetScalarComponentFromFloat(pixelCoordX, pixelCoordY, 0, 0, DRAWING_COLOR);
  }
}

//----------------------------------------------------------------------------
void vtkPlusBoneEnhancer::DrawScanLines(vtkPlusUsScanConvert* scanConverter, vtkImageData* imageData)
{
  int *rfImageExtent = scanConverter->GetInputImageExtent();
  int numOfSamplesPerScanline = rfImageExtent[1]-rfImageExtent[0]+1;
  int numOfScanlines = rfImageExtent[3]-rfImageExtent[2]+1;

  int* outputExtent = imageData->GetExtent();
  for (int scanLine = 0; scanLine < numOfScanlines; scanLine++)
  {
    double start[4] = {0};
    double end[4] = {0};
    scanConverter->GetScanLineEndPoints(scanLine,start,end);
    DrawLine(imageData, outputExtent, start, end, numOfSamplesPerScanline);
  }
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusBoneEnhancer::ProcessFrame(PlusTrackedFrame* inputFrame, PlusTrackedFrame* outputFrame)
{
  // Get input image
  PlusVideoFrame* inputImage = inputFrame->GetImageData();
  
#ifdef PLUS_USE_INTEL_MKL
  // Generate output image
  this->CastToDouble->SetInputData_vtk5compatible(inputImage->GetImage());
  this->CastToUnsignedChar->Update();
  // Write output image
  PlusVideoFrame* outputImage = outputFrame->GetImageData();
  outputImage->DeepCopyFrom(this->CastToUnsignedChar->GetOutput());
#else
  // Generate output image
  //  1. threshold the image
  this->Thresholder->SetInputData_vtk5compatible(inputImage->GetImage());
  this->Thresholder->Update();
  //  2. draw scanlines on it
  if (this->ScanConverter.GetPointer()!=NULL)
  {
    const int numOfScanlines = 50; // number of scanlines
    const int numOfSamplesPerScanline = 100; // number of dots drawn per scanline
    int rfImageExtent[6] = {0,numOfSamplesPerScanline-1,0,numOfScanlines-1,0,0};
    this->ScanConverter->SetInputImageExtent(rfImageExtent);
    DrawScanLines(this->ScanConverter, this->Thresholder->GetOutput());
  }
  // Write output image
  PlusVideoFrame* outputImage = outputFrame->GetImageData();
  outputImage->DeepCopyFrom(this->Thresholder->GetOutput());    
#endif

  return PLUS_SUCCESS;
}
