/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

#include "PlusConfigure.h"
#include "PlusMath.h"
#include "TrackedFrame.h"
#include "vtkBoneEnhancer.h"
#include "vtkForoughiBoneSurfaceProbability.h"
#include "vtkObjectFactory.h"
#include "vtkUsScanConvertCurvilinear.h"
#include "vtkUsScanConvertLinear.h"
#include "vtkImageCast.h"

//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkBoneEnhancer);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
vtkBoneEnhancer::vtkBoneEnhancer()
: BoneSurfaceFilter(vtkSmartPointer<vtkForoughiBoneSurfaceProbability>::New())
, CastToDouble(vtkSmartPointer<vtkImageCast>::New())
, CastToUnsignedChar(vtkSmartPointer<vtkImageCast>::New())
{
  this->CastToDouble->SetOutputScalarTypeToDouble();
  this->CastToUnsignedChar->SetOutputScalarTypeToUnsignedChar();

  this->BoneSurfaceFilter->SetInputConnection(this->CastToDouble->GetOutputPort());
  this->CastToUnsignedChar->SetInputConnection(this->BoneSurfaceFilter->GetOutputPort());
}

//----------------------------------------------------------------------------
vtkBoneEnhancer::~vtkBoneEnhancer()
{
}

//----------------------------------------------------------------------------
void vtkBoneEnhancer::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}

//-----------------------------------------------------------------------------
PlusStatus vtkBoneEnhancer::ReadConfiguration(vtkXMLDataElement* processingElement)
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
    vtkSmartPointer<vtkUsScanConvert> scanConverter;
    if (STRCASECMP(transducerGeometry,"CURVILINEAR") == 0)
    {
      this->ScanConverter = vtkSmartPointer<vtkUsScanConvert>::Take(vtkUsScanConvertCurvilinear::New());
    }
    else if (STRCASECMP(transducerGeometry, "LINEAR") == 0)
    {
      this->ScanConverter = vtkSmartPointer<vtkUsScanConvert>::Take(vtkUsScanConvertLinear::New());
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
PlusStatus vtkBoneEnhancer::WriteConfiguration(vtkXMLDataElement* processingElement)
{
  XML_VERIFY_ELEMENT(processingElement, this->GetTagName());
  processingElement->SetDoubleAttribute("Threshold", this->GetThreshold());
  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
void vtkBoneEnhancer::SetThreshold(double threshold)
{
  //this->Thresholder->ThresholdByLower(threshold);
}

//-----------------------------------------------------------------------------
double vtkBoneEnhancer::GetThreshold()
{
  //return this->Thresholder->GetLowerThreshold();
  return 0;
}

//----------------------------------------------------------------------------
void vtkBoneEnhancer::DrawLine(vtkImageData* imageData, int* imageExtent, double* start, double* end, int numberOfPoints)
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
void vtkBoneEnhancer::DrawScanLines(vtkUsScanConvert* scanConverter, vtkImageData* imageData)
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
PlusStatus vtkBoneEnhancer::ProcessFrame(TrackedFrame* inputFrame, TrackedFrame* outputFrame)
{
  // Get input image
  PlusVideoFrame* inputImage = inputFrame->GetImageData();

  this->CastToDouble->SetInputData_vtk5compatible(inputImage->GetImage());
  this->CastToUnsignedChar->Update();

  PlusVideoFrame* outputImage = outputFrame->GetImageData();
  outputImage->DeepCopyFrom(this->CastToUnsignedChar->GetOutput());

  return PLUS_SUCCESS;
}
