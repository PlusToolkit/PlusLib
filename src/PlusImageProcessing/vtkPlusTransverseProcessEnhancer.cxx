/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

// Local includes
#include "PlusConfigure.h"
#include "PlusMath.h"
#include "PlusTrackedFrame.h"
#include "PlusVideoFrame.h"
#include "vtkPlusTrackedFrameList.h"
#include "vtkPlusTransverseProcessEnhancer.h"
#include "vtkPlusUsScanConvertCurvilinear.h"
#include "vtkPlusUsScanConvertLinear.h"

// VTK includes
#include <vtkImageAccumulate.h>
#include <vtkImageCast.h>
#include <vtkImageDilateErode3D.h>
#include <vtkImageGaussianSmooth.h>
#include <vtkImageIslandRemoval2D.h>
#include <vtkImageSobel2D.h>
#include <vtkImageThreshold.h>
#include <vtkObjectFactory.h>

#include "vtkImageAlgorithm.h"

#include <cmath>


//----------------------------------------------------------------------------

vtkStandardNewMacro(vtkPlusTransverseProcessEnhancer);

//----------------------------------------------------------------------------

vtkPlusTransverseProcessEnhancer::vtkPlusTransverseProcessEnhancer()
  : ScanConverter(NULL),
  ConvertToLinesImage(true),
  NumberOfScanLines(0),
  NumberOfSamplesPerScanLine(0),

  RadiusStartMm(0),
  RadiusStopMm(0),
  ThetaStartDeg(0),
  ThetaStopDeg(0),

  Thresholder(NULL),
  GaussianEnabled(true),
  ThresholdInValue(0.0),
  ThresholdOutValue(255.0),
  LowerThreshold(0.0),
  UpperThreshold(0.0),

  GaussianSmooth(NULL),
  EdgeDetector(NULL),
  ImageBinarizer(NULL),
  BinaryImageForMorphology(NULL),
  IslandRemover(NULL),
  ImageEroder(NULL),
  ImageDialator(NULL),

  ReturnToFanImage(true),
  CurrentFrameMean(0.0),
  CurrentFrameStDev(0.0),
  CurrentFrameScalarComponentMax(0.0),
  CurrentFrameScalarComponentMin(255.0),

  ThresholdingEnabled(true),

  EdgeDetectorEnabled(true),
  ConversionImage(NULL),
  IslandRemovalEnabled(true),
  IslandAreaThreshold(-1),
  ErosionEnabled(true),
  DilationEnabled(true),
  ReconvertBinaryToGreyscale(true),

  IntermediateImage(NULL),
  LinesImage(NULL),
  ProcessedLinesImage(NULL),
  UnprocessedLinesImage(NULL),
  ShadowImage(NULL)
{
  this->Thresholder = vtkSmartPointer<vtkImageThreshold>::New();

  this->GaussianSmooth = vtkSmartPointer<vtkImageGaussianSmooth>::New();
  this->EdgeDetector = vtkSmartPointer<vtkImageSobel2D>::New();
  this->ImageBinarizer = vtkSmartPointer<vtkImageThreshold>::New();
  this->BinaryImageForMorphology = vtkSmartPointer<vtkImageData>::New();
  this->IslandRemover = vtkSmartPointer<vtkImageIslandRemoval2D>::New();
  this->ImageEroder = vtkSmartPointer<vtkImageDilateErode3D>::New();
  this->ImageDialator = vtkSmartPointer<vtkImageDilateErode3D>::New();

  this->SetDilationKernelSize(1, 1);
  this->SetErosionKernelSize(5, 5);
  this->SetGaussianStdDev(7.0);
  this->SetGaussianKernelSize(7.0);
  this->GaussianSmooth->SetDimensionality(2);

  this->ConversionImage = vtkSmartPointer<vtkImageData>::New();
  this->ConversionImage->SetExtent(0, 0, 0, 0, 0, 0);

  this->BinaryImageForMorphology->SetExtent(0, 0, 0, 0, 0, 0);

  this->ImageBinarizer->SetInValue(255);
  this->ImageBinarizer->SetOutValue(0);
  this->ImageBinarizer->ThresholdBetween(55, 255);
  this->IslandRemover->SetIslandValue(255);
  this->IslandRemover->SetReplaceValue(0);
  this->IslandRemover->SetAreaThreshold(0);

  this->ImageEroder->SetKernelSize(this->ErosionKernelSize[0], this->ErosionKernelSize[1], 1);
  this->ImageEroder->SetErodeValue(255);
  this->ImageEroder->SetDilateValue(0);

  this->ImageDialator->SetKernelSize(this->DilationKernelSize[0], this->DilationKernelSize[1], 1);
  this->ImageDialator->SetErodeValue(0);
  this->ImageDialator->SetDilateValue(255);

  this->IntermediateImage = vtkSmartPointer<vtkImageData>::New();
  this->LinesImage = vtkSmartPointer<vtkImageData>::New();
  this->ProcessedLinesImage = vtkSmartPointer<vtkImageData>::New();
  this->UnprocessedLinesImage = vtkSmartPointer<vtkImageData>::New();
  this->ShadowImage = vtkSmartPointer<vtkImageData>::New();

  this->IntermediateImage->SetExtent(0, 0, 0, 0, 0, 0);
  this->LinesImage->SetExtent(0, 0, 0, 0, 0, 0);
  this->ProcessedLinesImage->SetExtent(0, 0, 0, 0, 0, 0);
  this->UnprocessedLinesImage->SetExtent(0, 0, 0, 0, 0, 0);
  this->ShadowImage->SetExtent(0, 0, 0, 0, 0, 0);

  this->IntermediateImageMap.clear();
}

//----------------------------------------------------------------------------
vtkPlusTransverseProcessEnhancer::~vtkPlusTransverseProcessEnhancer()
{
  // Make sure contained smart pointers are deleted
  this->IntermediateImageMap.clear();
}

//----------------------------------------------------------------------------
void vtkPlusTransverseProcessEnhancer::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusTransverseProcessEnhancer::ReadConfiguration(vtkSmartPointer<vtkXMLDataElement> processingElement)
{
  XML_VERIFY_ELEMENT(processingElement, this->GetTagName());

  vtkSmartPointer<vtkXMLDataElement> scanConversionElement = processingElement->FindNestedElementWithName("ScanConversion");
  if (scanConversionElement != NULL)
  {
    // Call scanline generator with appropriate scanconvert
    const char* transducerGeometry = scanConversionElement->GetAttribute("TransducerGeometry");
    if (transducerGeometry == NULL)
    {
      LOG_ERROR("Scan converter TransducerGeometry is undefined");
      return PLUS_FAIL;
    }
    else
    {
      LOG_INFO("Scan converter is defined.");
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

    XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(int, RadiusStartMm, scanConversionElement);
    XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(int, RadiusStopMm, scanConversionElement);
    XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(int, ThetaStartDeg, scanConversionElement);
    XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(int, ThetaStopDeg, scanConversionElement);
  }
  else
  {
    LOG_INFO("ScanConversion section not found in config file!");
  }

  // Read image processing options from configuration
  vtkXMLDataElement* imageProcessingOperations = processingElement->FindNestedElementWithName("ImageProcessingOperations");
  if (imageProcessingOperations != NULL)
  {
    XML_READ_BOOL_ATTRIBUTE_OPTIONAL(ConvertToLinesImage, imageProcessingOperations);
    if (this->ConvertToLinesImage)
    {
      // ScanConverter parameters
    }
    XML_READ_BOOL_ATTRIBUTE_OPTIONAL(ReturnToFanImage, imageProcessingOperations);
    XML_READ_BOOL_ATTRIBUTE_OPTIONAL(GaussianEnabled, imageProcessingOperations);
    if (this->GaussianEnabled)
    {
      vtkSmartPointer<vtkXMLDataElement> gaussianParameters = imageProcessingOperations->FindNestedElementWithName("GaussianSmoothing");
      if (gaussianParameters == NULL)
      {
        LOG_WARNING("Unable to locate GaussianSmoothing parameters element. Using default values.");
      }
      else
      {
        XML_READ_SCALAR_ATTRIBUTE_REQUIRED(double, GaussianStdDev, gaussianParameters);

        XML_READ_SCALAR_ATTRIBUTE_REQUIRED(double, GaussianKernelSize, gaussianParameters);
      }
    }

    XML_READ_BOOL_ATTRIBUTE_OPTIONAL(ThresholdingEnabled, imageProcessingOperations);
    if (this->ThresholdingEnabled)
    {
      vtkSmartPointer<vtkXMLDataElement> thresholdingParameters = imageProcessingOperations->FindNestedElementWithName("Thresholding");
      if (thresholdingParameters == NULL)
      {
        LOG_WARNING("Unable to locate Thresholding parameters element. Using default values.");
      }
      else
      {
        XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(double, ThresholdInValue, thresholdingParameters);
        XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(double, ThresholdOutValue, thresholdingParameters);

        XML_READ_SCALAR_ATTRIBUTE_REQUIRED(double, LowerThreshold, thresholdingParameters);
        XML_READ_SCALAR_ATTRIBUTE_REQUIRED(double, UpperThreshold, thresholdingParameters);
      }
    }
    XML_READ_BOOL_ATTRIBUTE_OPTIONAL(EdgeDetectorEnabled, imageProcessingOperations);       // No other parameters to set?

    XML_READ_BOOL_ATTRIBUTE_OPTIONAL(IslandRemovalEnabled, imageProcessingOperations);
    if (this->IslandRemovalEnabled)
    {
      vtkSmartPointer<vtkXMLDataElement> islandRemovalParameters = imageProcessingOperations->FindNestedElementWithName("IslandRemoval");
      if (islandRemovalParameters == NULL)
      {
        LOG_WARNING("Unable to locate IslandRemoval parameters element. Using default values.");
      }
      else
      {
        XML_READ_SCALAR_ATTRIBUTE_REQUIRED(int, IslandAreaThreshold, islandRemovalParameters);
      }
    }

    XML_READ_BOOL_ATTRIBUTE_OPTIONAL(ErosionEnabled, imageProcessingOperations);
    if (this->ErosionEnabled)
    {
      vtkSmartPointer<vtkXMLDataElement> erosionParameters = imageProcessingOperations->FindNestedElementWithName("Erosion");
      if (erosionParameters == NULL)
      {
        LOG_WARNING("Unable to locate Erosion paramters element. Using default values.");
      }
      else
      {
        XML_READ_VECTOR_ATTRIBUTE_REQUIRED(int, 2, ErosionKernelSize, erosionParameters);
      }
    }

    XML_READ_BOOL_ATTRIBUTE_OPTIONAL(DilationEnabled, imageProcessingOperations);
    if (this->DilationEnabled)
    {
      vtkSmartPointer<vtkXMLDataElement> dilationParameters = imageProcessingOperations->FindNestedElementWithName("Dilation");
      if (dilationParameters == NULL)
      {
        LOG_WARNING("Unable to locate Dilation parameters element. Using default values.");
      }
      else
      {
        XML_READ_VECTOR_ATTRIBUTE_REQUIRED(int, 2, DilationKernelSize, dilationParameters);
      }
    }

    XML_READ_BOOL_ATTRIBUTE_OPTIONAL(ReconvertBinaryToGreyscale, imageProcessingOperations);
  }
  else
  {
    LOG_INFO("ImageProcessingOperations section not found in config file!");
    LOG_INFO("Enabling all filters and using default values.");
    this->ConvertToLinesImage = true;
    this->GaussianEnabled = true;
    this->ThresholdingEnabled = true;
    this->EdgeDetectorEnabled = true;
    this->IslandRemovalEnabled = true;
    this->ErosionEnabled = true;
    this->DilationEnabled = true;
    this->ReconvertBinaryToGreyscale = true;
    this->ReturnToFanImage = true;
  }

  XML_READ_SCALAR_ATTRIBUTE_REQUIRED(int, NumberOfScanLines, processingElement);
  XML_READ_SCALAR_ATTRIBUTE_REQUIRED(int, NumberOfSamplesPerScanLine, processingElement);

  int rfImageExtent[6] = { 0, this->NumberOfSamplesPerScanLine - 1, 0, this->NumberOfScanLines - 1, 0, 0 };
  this->ScanConverter->SetInputImageExtent(rfImageExtent);

  return PLUS_SUCCESS;
}


//----------------------------------------------------------------------------
PlusStatus vtkPlusTransverseProcessEnhancer::WriteConfiguration(vtkSmartPointer<vtkXMLDataElement> processingElement)
{
  XML_VERIFY_ELEMENT(processingElement, this->GetTagName());

  processingElement->SetAttribute("Type", this->GetProcessorTypeName());
  processingElement->SetIntAttribute("NumberOfScanLines", NumberOfScanLines);
  processingElement->SetIntAttribute("NumberOfSamplesPerScanLine", NumberOfSamplesPerScanLine);

  XML_FIND_NESTED_ELEMENT_CREATE_IF_MISSING(scanConversionElement, processingElement, "ScanConversion");
  this->ScanConverter->WriteConfiguration(scanConversionElement);
  scanConversionElement->SetDoubleAttribute("RadiusStartMm", this->RadiusStartMm);
  scanConversionElement->SetDoubleAttribute("RadiusStopMm", this->RadiusStopMm);
  scanConversionElement->SetIntAttribute("ThetaStartDeg", this->ThetaStartDeg);
  scanConversionElement->SetIntAttribute("ThetaStopDeg", this->ThetaStopDeg);

  XML_FIND_NESTED_ELEMENT_CREATE_IF_MISSING(imageProcessingOperations, processingElement, "ImageProcessingOperations");

  XML_WRITE_BOOL_ATTRIBUTE(ConvertToLinesImage, imageProcessingOperations);

  XML_WRITE_BOOL_ATTRIBUTE(GaussianEnabled, imageProcessingOperations);
  XML_FIND_NESTED_ELEMENT_CREATE_IF_MISSING(gaussianParameters, imageProcessingOperations, "GaussianSmoothing");
  gaussianParameters->SetDoubleAttribute("GaussianStdDev", this->GaussianStdDev);
  gaussianParameters->SetDoubleAttribute("GaussianKernelSize", this->GaussianKernelSize);

  XML_WRITE_BOOL_ATTRIBUTE(ThresholdingEnabled, imageProcessingOperations);
  XML_FIND_NESTED_ELEMENT_CREATE_IF_MISSING(thresholdingParameters, imageProcessingOperations, "Thresholding");
  thresholdingParameters->SetDoubleAttribute("ThresholdInValue", ThresholdInValue);
  thresholdingParameters->SetDoubleAttribute("ThresholdOutValue", ThresholdOutValue);
  thresholdingParameters->SetDoubleAttribute("LowerThreshold", LowerThreshold);
  thresholdingParameters->SetDoubleAttribute("UpperThreshold", UpperThreshold);

  XML_WRITE_BOOL_ATTRIBUTE(EdgeDetectorEnabled, imageProcessingOperations);

  XML_WRITE_BOOL_ATTRIBUTE(IslandRemovalEnabled, imageProcessingOperations);
  XML_FIND_NESTED_ELEMENT_CREATE_IF_MISSING(islandRemovalParameters, imageProcessingOperations, "IslandRemoval");
  islandRemovalParameters->SetIntAttribute("IslandAreaThreshold", IslandAreaThreshold);

  XML_WRITE_BOOL_ATTRIBUTE(ErosionEnabled, imageProcessingOperations);
  XML_FIND_NESTED_ELEMENT_CREATE_IF_MISSING(erosionParameters, imageProcessingOperations, "Erosion");
  erosionParameters->SetVectorAttribute("ErosionKernelSize", 2, this->ErosionKernelSize);

  XML_WRITE_BOOL_ATTRIBUTE(DilationEnabled, imageProcessingOperations);
  XML_FIND_NESTED_ELEMENT_CREATE_IF_MISSING(dilationParameters, imageProcessingOperations, "Dilation");
  dilationParameters->SetVectorAttribute("DilationKernelSize", 2, this->DilationKernelSize);

  XML_WRITE_BOOL_ATTRIBUTE(ReconvertBinaryToGreyscale, imageProcessingOperations);

  XML_WRITE_BOOL_ATTRIBUTE(ReturnToFanImage, imageProcessingOperations);

  return PLUS_SUCCESS;
}


PlusStatus vtkPlusTransverseProcessEnhancer::ProcessImageExtents()
{

  // Allocate lines image.
  int* linesImageExtent = this->ScanConverter->GetInputImageExtent();

  LOG_DEBUG("Lines image extent: "
    << linesImageExtent[0] << ", " << linesImageExtent[1]
    << ", " << linesImageExtent[2] << ", " << linesImageExtent[3]
    << ", " << linesImageExtent[4] << ", " << linesImageExtent[5]);

  this->BinaryImageForMorphology->SetExtent(linesImageExtent);
  this->BinaryImageForMorphology->AllocateScalars(VTK_UNSIGNED_CHAR, 1);

  this->LinesImage->SetExtent(linesImageExtent);
  this->LinesImage->AllocateScalars(VTK_UNSIGNED_CHAR, 1);

  this->ShadowImage->SetExtent(linesImageExtent);
  this->ShadowImage->AllocateScalars(VTK_FLOAT, 1);

  this->IntermediateImage->SetExtent(linesImageExtent);
  this->IntermediateImage->AllocateScalars(VTK_UNSIGNED_CHAR, 1);

  //Set up variables related to image extents
  int dims[3] = { 0, 0, 0 };
  this->LinesImage->GetDimensions(dims);

  this->FoundBoneGrid.resize(dims[0], std::vector<int>());

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
// Fills the lines image by subsampling the input image along scanlines.
// Also computes pixel statistics.
void vtkPlusTransverseProcessEnhancer::FillLinesImage(vtkSmartPointer<vtkImageData> inputImageData)
{
  int* linesImageExtent = this->ScanConverter->GetInputImageExtent();
  int lineLengthPx = linesImageExtent[1] - linesImageExtent[0] + 1;
  int numScanLines = linesImageExtent[3] - linesImageExtent[2] + 1;

  // For calculating pixel intensity mean and variance. Algorithm taken from:
  // https://en.wikipedia.org/wiki/Algorithms_for_calculating_variance#Online_algorithm

  double mean = 0.0;
  double sumSquareDiff = 0.0; //named M2 in online notes
  long pixelCount = 0;
  double currentValue = 0.0; //temporary value for each loop. //Named value in online notes
  double valueMeanDiff = 0.0; //Named delta in online notes
  this->CurrentFrameScalarComponentMax = 0.0;
  this->CurrentFrameScalarComponentMin = 255.0;

  int* inputExtent = inputImageData->GetExtent();
  for (int scanLine = 0; scanLine < numScanLines; scanLine++)
  {
    double start[4] = { 0 };
    double end[4] = { 0 };
    ScanConverter->GetScanLineEndPoints(scanLine, start, end);

    double directionVectorX = static_cast<double>(end[0] - start[0]) / (lineLengthPx - 1);
    double directionVectorY = static_cast<double>(end[1] - start[1]) / (lineLengthPx - 1);
    for (int pointIndex = 0; pointIndex < lineLengthPx; ++pointIndex)
    {
      int pixelCoordX = start[0] + directionVectorX * pointIndex;
      int pixelCoordY = start[1] + directionVectorY * pointIndex;
      if (pixelCoordX < inputExtent[0] || pixelCoordX > inputExtent[1]
        || pixelCoordY < inputExtent[2] || pixelCoordY > inputExtent[3])
      {
        this->LinesImage->SetScalarComponentFromFloat(pointIndex, scanLine, 0, 0, 0);
        continue; // outside of the specified extent
      }
      currentValue = inputImageData->GetScalarComponentAsDouble(pixelCoordX, pixelCoordY, 0, 0);
      this->LinesImage->SetScalarComponentFromFloat(pointIndex, scanLine, 0, 0, currentValue);

      if (this->CurrentFrameScalarComponentMax < currentValue)
      {
        this->CurrentFrameScalarComponentMax = currentValue;
      }
      if (this->CurrentFrameScalarComponentMin > currentValue)
      {
        this->CurrentFrameScalarComponentMin = currentValue;
      }

      ++pixelCount;
      valueMeanDiff = currentValue - mean;
      mean = mean + valueMeanDiff / pixelCount;
      sumSquareDiff = sumSquareDiff + valueMeanDiff * (currentValue - mean);
    }
  }

  this->CurrentFrameMean = mean;
  this->CurrentFrameStDev = std::sqrt(sumSquareDiff / (pixelCount - 1));
}


//----------------------------------------------------------------------------
void vtkPlusTransverseProcessEnhancer::ProcessLinesImage()
{
  // Parameters
  float thresholdSdFactor = 1.8;
  float nearFactor = 0.4;

  int dims[3] = { 0, 0, 0 };
  this->LinesImage->GetDimensions(dims);

  // Define the threshold value for bone candidate points.
  double dThreshold = this->CurrentFrameMean + thresholdSdFactor * this->CurrentFrameStDev;
  unsigned char threshold = 255;
  if (dThreshold < 255) { threshold = (unsigned char)dThreshold; }

  // Compute mapping factor for values above threshold (T).
  // Mapping [T,255] to [25%,100%], that is [64,255].
  // where delta = PixelValue - T
  float mappingFactor = 191.0 / (255.0 - threshold);
  float mappingShift = 64.0;

  // Define threshold and factor for pixel locations close to transducer.
  int xClose = int(dims[0] * nearFactor);
  float xFactor = 1.0 / xClose;

  // Iterate all pixels.
  unsigned char* vInput = 0;
  unsigned char* vOutput = 0;
  float* vShadow = 0;
  bool foundInThisLine = false;
  bool decreaseAfterFound = false;
  unsigned char lastValue = 0;
  float output = 0.0;     // Keep this in [0..255] instead [0..1] for possible future optimization.

  // Populate this->ShadowImage pixel values with complement of this->LinesImage pixel relative brightness values
  this->FillShadowValues();

  // Following loop currently only serves as a bounds check, does not otherwise modify this->LinesImage pixel values before storing them in this->IntermediateImage
  for (int y = 0; y < dims[1]; y++)
  {
    // Initialize variables for a new scan line.
    for (int x = dims[0] - 1; x >= 0; x--)   // Go towards transducer
    {
      vInput = static_cast<unsigned char*>(this->LinesImage->GetScalarPointer(x, y, 0));
      vOutput = static_cast<unsigned char*>(this->IntermediateImage->GetScalarPointer(x, y, 0));
      output = (*vInput);

      //Make vOutput a maximum of 255, and at minimum 0
      if (output > 255) { (*vOutput) = 255; }
      else if (output < 0) { (*vOutput) = 0; }
      else { (*vOutput) = (unsigned char)output; }
    }
  }

  this->IntermediateImage->Modified();
}

//----------------------------------------------------------------------------
void vtkPlusTransverseProcessEnhancer::FillShadowValues()
{
  int dims[3] = { 0, 0, 0 };
  this->LinesImage->GetDimensions(dims);

  float lineMeanSoFar = 0.0;
  float lineMaxSoFar = 0.0;
  int nSoFar = 0;

  unsigned char* vInput = 0;
  float* vOutput = 0;

  // Go along each scan line in this->LinesImage, and set this->ShadowImage pixel values to complement of relative this->LinesImage pixel brightnesses
  for (int y = 0; y < dims[1]; y++)
  {
    // Initialize variables for new scan line.
    lineMeanSoFar = 0.0;
    lineMaxSoFar = 0.0;
    nSoFar = 0;
    float shadowValue = 0.0;

    for (int x = dims[0] - 1; x >= 0; x--)   // Go towards transducer.
    {
      vInput = static_cast<unsigned char*>(this->LinesImage->GetScalarPointer(x, y, 0));
      vOutput = static_cast<float*>(this->ShadowImage->GetScalarPointer(x, y, 0));

      unsigned char inputValue = (*vInput);

      nSoFar++;
      float diffFromMean = inputValue - lineMeanSoFar;
      lineMeanSoFar = lineMeanSoFar + diffFromMean / nSoFar;
      if (inputValue > lineMaxSoFar)
      {
        lineMaxSoFar = inputValue;
      }

      shadowValue = 1.0 - (lineMaxSoFar / this->CurrentFrameScalarComponentMax);

      *vOutput = shadowValue;
    }
  }
  this->ShadowImage->Modified();
  // Save shadow image
  PlusVideoFrame shadowVideoFrame;
  shadowVideoFrame.DeepCopyFrom(this->ShadowImage);
  PlusTrackedFrame shadowTrackedFrame;
  shadowTrackedFrame.SetImageData(shadowVideoFrame);
}

//----------------------------------------------------------------------------
void vtkPlusTransverseProcessEnhancer::VectorImageToUchar(vtkSmartPointer<vtkImageData> inputImage)
{
  unsigned char* vInput = 0;
  unsigned char* vOutput = 0;
  unsigned char edgeDetectorOutput0;
  unsigned char edgeDetectorOutput1;
  float output = 0.0;     // Keep this in [0..255] instead [0..1] for possible future optimization.
  float output2 = 0.0;

  int dims[3] = { 0, 0, 0 };
  this->LinesImage->GetDimensions(dims);
  this->ConversionImage->SetExtent(this->LinesImage->GetExtent());
  this->ConversionImage->AllocateScalars(VTK_UNSIGNED_CHAR, 1);
  for (int y = 0; y < dims[1]; y++)
  {
    // Initialize variables for a new scan line.

    for (int x = dims[0] - 1; x >= 0; x--)   // Go towards transducer
    {
      edgeDetectorOutput0 = static_cast<unsigned char>(inputImage->GetScalarComponentAsFloat(x, y, 0, 0));
      edgeDetectorOutput1 = static_cast<unsigned char>(inputImage->GetScalarComponentAsFloat(x, y, 0, 1));
      vOutput = static_cast<unsigned char*>(this->ConversionImage->GetScalarPointer(x, y, 0));
      output = (float)(edgeDetectorOutput0 + edgeDetectorOutput1) / (float)2;                                         // Not mathematically correct, but a quick approximation of sqrt(x^2 + y^2)

      if (output > 255) { (*vOutput) = 255; }
      else if (output < 0) { (*vOutput) = 0; }
      else { (*vOutput) = (unsigned char)output; }
    }
  }
}


/*
Takes a vtkSmartPointer<vtkImageData> as an argument and modifies it such that all images in a row
that have a bone shadow behind it are removed
*/
void vtkPlusTransverseProcessEnhancer::RemoveImagesPrecedingShadow(vtkSmartPointer<vtkImageData> inputImage)
{

  int dims[3] = { 0, 0, 0 };
  inputImage->GetDimensions(dims);

  int keepInfoCounter;
  unsigned char*  vOutput;

  for (int y = dims[1] - 1; y >= 0; y--)
  {
    //When an image is detected, keep up to this many pixles after it
    keepInfoCounter = 3; //TODO: replace this Megic Number with parameter in mm

    for (int x = dims[0] - 1; x >= 0; x--)
    {
      vOutput = static_cast<unsigned char*>(inputImage->GetScalarPointer(x, y, 0));

      //If an image is detected
      if (*vOutput != 0)
      {
        if (keepInfoCounter == 0)
        {
          *vOutput = 0;
        }
        else
        {
          keepInfoCounter--;
        }
      }

    }
  }
}


/*
Takes a vtkSmartPointer<vtkImageData> with clearly defined possible bone segments as an
argument and modifies it so the bone areas that are too close to the camera's edge are removed.
*/
void vtkPlusTransverseProcessEnhancer::RemoveOffCameraBones(vtkSmartPointer<vtkImageData> inputImage)
{

  int dims[3] = { 0, 0, 0 };
  inputImage->GetDimensions(dims);

  unsigned char* vOutput = 0;

  int distanceBuffer = 10;
  std::vector<std::vector<int>> boneAreas = this->FindBoneAreas(inputImage);
  int boneHalfLen;
  bool clearArea;

  for (int areaIndex = boneAreas.size() - 1; areaIndex >= 0; areaIndex--)
  {
    std::vector<int> currentArea = boneAreas.at(areaIndex);

    clearArea = false;
    boneHalfLen = ((currentArea.at(1) - currentArea.at(2)) + 1)  / 2;

    //check if the bone is to close too the scan's edge
    if (currentArea.at(1) + distanceBuffer >= dims[1] || currentArea.at(2) - distanceBuffer <= 0)
    {
      clearArea = true;
    }
    //check if given the size, the bone is too close to the scan's edge
    else if (boneHalfLen + currentArea.at(1) >= dims[1] - 1 || (currentArea.at(2) - 1) - boneHalfLen <= 0)
    {
      clearArea = true;
    }
    //check if the bone is too close/far from the transducer 
    else if (currentArea.at(0) < 20 || currentArea.at(0) > dims[0] - 20)
    {
      clearArea = true;
    }
    //check if the bone is to small
    else if (currentArea.at(1) - currentArea.at(2) <= 10)
    {
      clearArea = true;
    }

    //If it dosnt meet the criteria, remove the bones in this area
    if (clearArea == true)
    {
      for (int y = currentArea.at(1); y >= currentArea.at(2); y--)
      {
        for (int x = dims[0] - 1; x >= 0; x--)
        {
          vOutput = static_cast<unsigned char*>(inputImage->GetScalarPointer(x, y, 0));
          if (*vOutput != 0)
          {
            *vOutput = 0;
          }
        }
      }
    }
  }
}


/*
Takes an unmodified vtkSmartPointer<vtkImageData> of an ultrasound as its first argument, and a more
enhanced version of said image, with clearly defined possible bone segments as the second argument.
This function modifies the second argument so as to remove any bone segments that have a higher
amount of bone potential in the areas next to it than there is within the areas themselves.
*/
void vtkPlusTransverseProcessEnhancer::CompareShadowAreas(vtkSmartPointer<vtkImageData> originalImage, vtkSmartPointer<vtkImageData> inputImage)
{

  int dims[3] = { 0, 0, 0 };
  inputImage->GetDimensions(dims);

  float vInput = 0;
  unsigned char* vOutput = 0;

  int boneLen;
  int boneHalfLen;
  int sideArea;
  float aboveSum;
  float areaSum;
  float belowSum;

  std::vector<std::vector<int>> boneAreas = this->FindBoneAreas(inputImage);

  for (int areaIndex = boneAreas.size() - 1; areaIndex >= 0; areaIndex--)
  {
    std::vector<int> currentArea = boneAreas.at(areaIndex);

    aboveSum = 0;
    areaSum = 0;
    belowSum = 0;

    boneLen = (currentArea.at(1) - currentArea.at(2)) + 1;
    boneHalfLen = boneLen / 2;
    sideArea = (boneLen * currentArea.at(0)) / 2;

    //gather sum of shadow areas from above the area
    for (int y = std::min(currentArea.at(1) + boneHalfLen, dims[1] - 1); y > currentArea.at(1); y--)
    {
      for (int x = dims[0] - 1; x > currentArea.at(0); x--)
      {
        vInput = (originalImage->GetScalarComponentAsFloat(x, y, 0, 0));
        aboveSum += vInput;
      }
    }
    //gather sum of shadow areas from the area
    for (int y = currentArea.at(1); y >= currentArea.at(2); y--)
    {
      for (int x = dims[0] - 1; x > currentArea.at(0); x--)
      {
        vInput = (originalImage->GetScalarComponentAsFloat(x, y, 0, 0));
        areaSum += vInput;
      }
    }
    //gather sum of shadow areas from below the area
    for (int y = std::max(currentArea.at(2) - boneHalfLen, 0); y < currentArea.at(2); y++)
    {
      for (int x = dims[0] - 1; x > currentArea.at(0); x--)
      {
        vInput = (originalImage->GetScalarComponentAsFloat(x, y, 0, 0));
        belowSum += vInput;
      }
    }

    float aboveAvgShadow = aboveSum / sideArea;
    float areaAvgShadow = areaSum / (boneLen * currentArea.at(0));
    float belowAvgShadow = belowSum / sideArea;

    //If there is a higher amount of bones around it, remove the area
    if (aboveAvgShadow - areaAvgShadow <= (areaAvgShadow / 2) || belowAvgShadow - areaAvgShadow <= (areaAvgShadow / 2))
    {
      for (int y = currentArea.at(1); y >= currentArea.at(2); y--)
      {
        for (int x = dims[0] - 1; x >= 0; x--)
        {
          vOutput = static_cast<unsigned char*>(inputImage->GetScalarPointer(x, y, 0));
          if (*vOutput != 0)
          {
            *vOutput = 0;
          }
        }
      }
    }
  }
}


//Note: (1) is a bigger number than (2)

/*
Finds all the bone areas in the given vtkSmartPointer<vtkImageData>
The output is a vector whereby the first element is the average bone depth, and the second and
thrid elements refer to the start and end points of the bone area.
*/
std::vector<std::vector<int>> vtkPlusTransverseProcessEnhancer::FindBoneAreas(vtkSmartPointer<vtkImageData> inputImage)
{

  int dims[3] = { 0, 0, 0 };
  inputImage->GetDimensions(dims);

  unsigned char* vInput = 0;
  std::vector<std::vector<int>> intermediateOutput;
  std::vector<std::vector<int>> output;

  std::vector<std::vector<int>> boneMaxAreas;
  int bonesFoundCounter = 0;
  std::vector<int> boneNumIndexes;

  std::fill(begin(this->FoundBoneGrid), end(this->FoundBoneGrid), std::vector<int>(dims[1], -1));
  for (int y = dims[1] - 1; y >= 0; y--)
  {
    for (int x = dims[0] - 1; x >= 0; x--)
    {

      vInput = static_cast<unsigned char*>(inputImage->GetScalarPointer(x, y, 0));
      if (*vInput != 0)
      {
        std::vector<int> attachedIndex;

        //right
        if (x != dims[0] - 1)
        {
          if (FoundBoneGrid[x + 1][y] != -1)
          {
            if (std::find(attachedIndex.begin(), attachedIndex.end(), FoundBoneGrid[x + 1][y]) == attachedIndex.end())
            {
              attachedIndex.push_back(FoundBoneGrid[x + 1][y]);
            }
          }
        }

        //left
        if (x != 0)
        {
          if (FoundBoneGrid[x - 1][y] != -1)
          {
            if (std::find(attachedIndex.begin(), attachedIndex.end(), FoundBoneGrid[x - 1][y]) == attachedIndex.end())
            {
              attachedIndex.push_back(FoundBoneGrid[x - 1][y]);
            }
          }
        }

        //down
        if (y != dims[1] - 1)
        {
          if (FoundBoneGrid[x][y + 1] != -1)
          {
            if (std::find(attachedIndex.begin(), attachedIndex.end(), FoundBoneGrid[x][y + 1]) == attachedIndex.end())
            {
              attachedIndex.push_back(FoundBoneGrid[x][y + 1]);
            }
          }
        }

        //up
        if (y != 0)
        {
          if (FoundBoneGrid[x][y - 1] != -1)
          {
            if (std::find(attachedIndex.begin(), attachedIndex.end(), FoundBoneGrid[x][y - 1]) == attachedIndex.end())
            {
              attachedIndex.push_back(FoundBoneGrid[x][y - 1]);
            }
          }
        }

        /*
        //diagonals
        if (x != dims[0] - 1 && y != dims[1] - 1)
        {
          if (FoundBoneGrid[x + 1][y + 1] != -1)
          {
            if (std::find(attachedIndex.begin(), attachedIndex.end(), FoundBoneGrid[x + 1][y + 1]) == attachedIndex.end())
            {
              attachedIndex.push_back(FoundBoneGrid[x + 1][y + 1]);
            }
          }
        }
        if (x != 0 && y != dims[1] - 1)
        {
          if (FoundBoneGrid[x - 1][y + 1] != -1)
          {
            if (std::find(attachedIndex.begin(), attachedIndex.end(), FoundBoneGrid[x - 1][y + 1]) == attachedIndex.end())
            {
              attachedIndex.push_back(FoundBoneGrid[x - 1][y + 1]);
            }
          }
        }
        if (x != dims[0] - 1 && y != 0)
        {
          if (FoundBoneGrid[x + 1][y - 1] != -1)
          {
            if (std::find(attachedIndex.begin(), attachedIndex.end(), FoundBoneGrid[x + 1][y - 1]) == attachedIndex.end())
            {
              attachedIndex.push_back(FoundBoneGrid[x + 1][y - 1]);
            }
          }
        }
        if (x != 0 && y != 0)
        {
          if (FoundBoneGrid[x - 1][y - 1] != -1)
          {
            if (std::find(attachedIndex.begin(), attachedIndex.end(), FoundBoneGrid[x - 1][y - 1]) == attachedIndex.end())
            {
              attachedIndex.push_back(FoundBoneGrid[x - 1][y - 1]);
            }
          }
        }
        */

        if (attachedIndex.size() == 0)
        {
          //If there are no bones around, mark off the new area
          FoundBoneGrid[x][y] = bonesFoundCounter;
          std::vector<int> tempOutputVect;
          std::vector<int> tempBoneDepthVect;
          tempOutputVect.push_back(0);
          tempOutputVect.push_back(y);
          tempOutputVect.push_back(y);
          intermediateOutput.push_back(tempOutputVect);
          tempBoneDepthVect.push_back(x);
          boneNumIndexes.push_back(bonesFoundCounter);

          boneMaxAreas.push_back(tempBoneDepthVect);
          bonesFoundCounter += 1;
        }
        else if (attachedIndex.size() == 1)
        {
          //If there is one marked bone nearby, have this pixel join that area
          FoundBoneGrid[x][y] = attachedIndex[0];

          //mark off the bone depth
          if (intermediateOutput[boneNumIndexes[attachedIndex[0]]][2] > y)
          {
            boneMaxAreas[boneNumIndexes[attachedIndex[0]]].push_back(x);
            intermediateOutput[boneNumIndexes[attachedIndex[0]]][2] = y;
          }
          else
          {
            if (boneMaxAreas[boneNumIndexes[attachedIndex[0]]][y - intermediateOutput[boneNumIndexes[attachedIndex[0]]][2]] < x)
            {
              boneMaxAreas[boneNumIndexes[attachedIndex[0]]][y - intermediateOutput[boneNumIndexes[attachedIndex[0]]][2]] = x;
            }
          }
        }
        else
        {
          //If there are multiple different nearby bone areas, combine them into one area

          //determine what section they should all fuse to
          int sectionMin = boneNumIndexes[attachedIndex[attachedIndex.size() - 1]];
          for (int sectionsIndex = attachedIndex.size() - 1; sectionsIndex >= 0; sectionsIndex--)
          {
            if (intermediateOutput[boneNumIndexes[attachedIndex[sectionsIndex]]][1] > intermediateOutput[sectionMin][1])
            {
              sectionMin = boneNumIndexes[attachedIndex[sectionsIndex]];
            }
          }

          //fuse to the section
          for (int sectionsIndex = attachedIndex.size() - 1; sectionsIndex >= 0; sectionsIndex--)
          {
            if (boneNumIndexes[attachedIndex[sectionsIndex]] != sectionMin)
            {
              boneNumIndexes[attachedIndex[sectionsIndex]] = sectionMin;
            }
            if (boneNumIndexes[attachedIndex[sectionsIndex]] == sectionMin)
            {
              FoundBoneGrid[x][y] = attachedIndex[sectionsIndex];
            }
          }

          //mark off the bone depth
          if (intermediateOutput[boneNumIndexes[sectionMin]][2] > y)
          {
            boneMaxAreas[boneNumIndexes[sectionMin]].push_back(x);
            intermediateOutput[boneNumIndexes[sectionMin]][2] = y;
          }
          else
          {
            if (boneMaxAreas[boneNumIndexes[sectionMin]][y - intermediateOutput[boneNumIndexes[sectionMin]][2]] < x)
            {
              boneMaxAreas[boneNumIndexes[sectionMin]][y - intermediateOutput[boneNumIndexes[sectionMin]][2]] = x;
            }
          }
        }
      }
    }
  }

  int boneDepthSum;


  std::vector<int> alreadyAdded;
  for (int finalPosIndex = boneNumIndexes.size() - 1; finalPosIndex >= 0; finalPosIndex--)
  {
    if (std::find(alreadyAdded.begin(), alreadyAdded.end(), boneNumIndexes[finalPosIndex]) == alreadyAdded.end())
    {
      boneDepthSum = 0;
      for (int boneDepthIndex = boneMaxAreas[boneNumIndexes[finalPosIndex]].size() - 1; boneDepthIndex >= 0; boneDepthIndex--)
      {
        boneDepthSum += boneMaxAreas[boneNumIndexes[finalPosIndex]][boneDepthIndex];
      }
      intermediateOutput[boneNumIndexes[finalPosIndex]][0] = boneDepthSum / ((intermediateOutput[boneNumIndexes[finalPosIndex]][1] - intermediateOutput[boneNumIndexes[finalPosIndex]][2]) + 1);

      output.push_back(intermediateOutput[boneNumIndexes[finalPosIndex]]);
      alreadyAdded.push_back(boneNumIndexes[finalPosIndex]);
    }
  }
  return output;
}


void vtkPlusTransverseProcessEnhancer::StdDeviationThreshold(vtkSmartPointer<vtkImageData> inputImage) //Note: this method is closer then normal way of threashold
{

  float vInput = 0;
  unsigned char* vOutput = 0;

  int dims[3] = { 0, 0, 0 };
  inputImage->GetDimensions(dims);

  int max;

  int pixelSum1;
  float avg1;
  float pixelSum2;
  float avg2;
  float thresholdValue;
  for (int y = dims[1] - 1; y >= 0; y--)
  {
    max = 0;

    pixelSum1 = 0;
    avg1 = 0;

    for (int x = dims[0] - 1; x >= 0; x--)
    {
      vInput = inputImage->GetScalarComponentAsFloat(x, y, 0, 0);
      pixelSum1 += vInput;

      if (vInput > max)
      {
        max = vInput;
      }
    }
    avg1 = pixelSum1 / dims[0];

    pixelSum2 = 0;
    avg2 = 0;
    for (int x = dims[0] - 1; x >= 0; x--)
    {
      vInput = inputImage->GetScalarComponentAsFloat(x, y, 0, 0);
      pixelSum2 += pow(vInput - avg1, 2);
    }
    avg2 = pixelSum2 / dims[0];

    thresholdValue = max - 3 * pow(avg2, 0.5);

    for (int x = dims[0] - 1; x >= 0; x--)
    {
      vOutput = static_cast<unsigned char*>(inputImage->GetScalarPointer(x, y, 0));
      if (*vOutput < thresholdValue && *vOutput != 0)
      {
        *vOutput = 0;
      }
    }
  }
}


//----------------------------------------------------------------------------
// If a pixel in MaskImage is > 0, the corresponding pixel in InputImage will remain unchanged, otherwise it will be set to 0
void vtkPlusTransverseProcessEnhancer::ImageConjunction(vtkSmartPointer<vtkImageData> InputImage, vtkSmartPointer<vtkImageData> MaskImage)
{
  // Images must be of the same dimension, an should already be, I should check this though
  unsigned char* inputPixelPointer = 0;

  int dims[3] = { 0, 0, 0 };
  this->LinesImage->GetDimensions(dims);      // This will be the same as InputImage, as long as InputImage is converted to linesImage previously

  for (int y = 0; y < dims[1]; y++)
  {
    // Initialize variables for a new scan line.

    for (int x = 0; x < dims[0]; x++)   // Go towards transducer
    {
      if (static_cast<unsigned char>(MaskImage->GetScalarComponentAsFloat(x, y, 0, 0)) > 0)
      {
        //do nothing
      }
      else
      {
        inputPixelPointer = static_cast<unsigned char*>(InputImage->GetScalarPointer(x, y, 0));
        *inputPixelPointer = 0;
      }
    }
  }
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusTransverseProcessEnhancer::ProcessFrame(PlusTrackedFrame* inputFrame, PlusTrackedFrame* outputFrame)
{

  if (this->FirstFrame == true)
  {
    //set up variables for future loops
    this->ProcessImageExtents();
    this->FirstFrame = false;
  }



  this->FrameCounter += 1;

  PlusVideoFrame* inputImage = inputFrame->GetImageData();

  int dims[3] = { 0, 0, 0 };
  inputImage->GetImage()->GetDimensions(dims);

  if (this->ScanConverter.GetPointer() == NULL)
  {
    return PLUS_FAIL;
  }
  this->AddIntermediateImage("_00PreFilters", inputImage->GetImage());

  if (this->ConvertToLinesImage)
  {
    this->ScanConverter->SetInputData(inputImage->GetImage());
    // Generate lines image.
    this->AddIntermediateFromFilter("_01Lines_1PreFillLines", this->ScanConverter);
    this->FillLinesImage(inputImage->GetImage());
    this->AddIntermediateImage("_01Lines_2FilterEnd", this->LinesImage);
    this->IntermediateImage->DeepCopy(this->LinesImage);
  }
  else
  {
    this->IntermediateImage->DeepCopy(inputImage->GetImage());
  }

  //Save this image so that it can be used for comparason with the output image
  vtkSmartPointer<vtkImageData> originalImage = vtkSmartPointer<vtkImageData>::New();
  originalImage->DeepCopy(this->IntermediateImage);

  if (this->ThresholdingEnabled)
  {
    this->StdDeviationThreshold(this->IntermediateImage);
    this->AddIntermediateImage("_02Threshold_1StdDeviationVersionFilterEnd", this->IntermediateImage);

    //this->Thresholder->SetInputData(this->IntermediateImage);
    //this->Thresholder->Update();
    //this->AddIntermediateImage("_02Threshold_1FilterEnd", this->Thresholder->GetOutput());
  }

  if (this->GaussianEnabled)
  {
    this->GaussianSmooth->SetInputData(this->IntermediateImage);
    //this->GaussianSmooth->SetInputConnection(this->Thresholder->GetOutputPort());

    this->AddIntermediateFromFilter("_03Gaussian_1FilterEnd", this->GaussianSmooth);
  }

  if (this->EdgeDetectorEnabled)
  {
    this->EdgeDetector->SetInputConnection(this->GaussianSmooth->GetOutputPort());
    this->EdgeDetector->Update();
    this->VectorImageToUchar(this->EdgeDetector->GetOutput());
    this->AddIntermediateImage("_04EdgeDetector_1FilterEnd", this->ConversionImage);
  }

  // If we are to perform any morphological operations, we must binarize the image
  if (this->IslandRemovalEnabled || this->ErosionEnabled || this->DilationEnabled || this->ReconvertBinaryToGreyscale)
  {
    this->ImageBinarizer->SetInputData(this->ConversionImage);
    this->AddIntermediateFromFilter("_05BinaryImageForMorphology_1FilterEnd", this->ImageBinarizer);

    if (this->IslandRemovalEnabled)
    {
      this->IslandRemover->SetInputConnection(this->ImageBinarizer->GetOutputPort());

      this->IslandRemover->Update();
      this->AddIntermediateImage("_06Island_1FilterEnd", this->IslandRemover->GetOutput());
    }

    if (this->ErosionEnabled)
    {
      this->ImageEroder->SetKernelSize(this->ErosionKernelSize[0], this->ErosionKernelSize[1], 1);
      
      this->ImageEroder->SetInputConnection(this->IslandRemover->GetOutputPort());
      this->AddIntermediateFromFilter("_07Erosion_1FilterEnd", this->ImageEroder);
    }

    if (this->DilationEnabled)
    {
      this->ImageDialator->SetKernelSize(this->DilationKernelSize[0], this->DilationKernelSize[1], 1);

      this->ImageDialator->SetInputConnection(this->ImageEroder->GetOutputPort());
      this->ImageDialator->Update();
      this->BinaryImageForMorphology->DeepCopy(this->ImageDialator->GetOutput());
      this->AddIntermediateImage("_08Dilation_1PostUpdate", this->BinaryImageForMorphology);

      this->RemoveImagesPrecedingShadow(this->BinaryImageForMorphology);
      this->AddIntermediateImage("_08Dilation_2PostRemoveAfterImage", this->BinaryImageForMorphology);
      this->RemoveOffCameraBones(this->BinaryImageForMorphology);
      this->AddIntermediateImage("_08Dilation_3PostRemoveOffCamera", this->BinaryImageForMorphology);
      this->CompareShadowAreas(originalImage, this->BinaryImageForMorphology);
      this->AddIntermediateImage("_08Dilation_4PostCompareShadowAreas", this->BinaryImageForMorphology);
    }

    if (this->ReconvertBinaryToGreyscale)
    {
      // Currently, inputImage is the output of the edge detector, not original pixels
      this->UnprocessedLinesImage->DeepCopy(this->GaussianSmooth->GetOutput());
      this->ImageConjunction(this->UnprocessedLinesImage, this->BinaryImageForMorphology);

      this->AddIntermediateImage("_09ReconvertBinaryToGreyscale_1FilterEnd", this->UnprocessedLinesImage);
      this->IntermediateImage->DeepCopy(this->UnprocessedLinesImage);
    }
    else
    {
      this->IntermediateImage->DeepCopy(this->BinaryImageForMorphology);
    }
  }

  this->ProcessedLinesImage->DeepCopy(this->IntermediateImage);
  PlusVideoFrame processedVideoFrame;
  processedVideoFrame.DeepCopyFrom(this->ProcessedLinesImage);
  PlusTrackedFrame* processedTrackedFrame = inputFrame;
  processedTrackedFrame->SetImageData(processedVideoFrame);

  PlusVideoFrame* outputImage = outputFrame->GetImageData();
  if (this->ReturnToFanImage)
  {
    this->ScanConverter->SetInputData(this->ProcessedLinesImage);
    this->ScanConverter->SetOutput(this->IntermediateImage);
    this->ScanConverter->Update();

    outputImage->DeepCopyFrom(this->IntermediateImage);

    this->AddIntermediateImage("_10ReturnToFanImage_1FilterEnd", this->IntermediateImage);
  }
  else
  {
    outputImage->DeepCopyFrom(this->IntermediateImage);
  }

  //TODO: Move output back by a number of pixels

  return PLUS_SUCCESS;
}


//----------------------------------------------------------------------------
/*
Finds and saves all intermediate images that have been recorded.
Saves the images by calling this->SaveIntermediateResultToFile()
Returns PLUS_FAIL if this->SaveIntermediateResultToFile() encounters an error occured during this
process, returns PLUS_SUCCESS otherwise.
*/
PlusStatus vtkPlusTransverseProcessEnhancer::SaveAllIntermediateResultsToFile()
{
  for (int postfixIndex = this->IntermediatePostfixes.size() - 1; postfixIndex >= 0; postfixIndex -= 1){
    if (this->SaveIntermediateResultToFile(this->IntermediatePostfixes.at(postfixIndex)) == PLUS_FAIL)
    {
      return PLUS_FAIL;
    }
  }
  return PLUS_SUCCESS;
}


//----------------------------------------------------------------------------
/*
Takes a postfix as an argument and saves the intermediate image associated with that postfix
Returns PLUS_FAIL if an error occured during this process, returns PLUS_SUCCESS otherwise
*/
PlusStatus vtkPlusTransverseProcessEnhancer::SaveIntermediateResultToFile(char* fileNamePostfix)
{
  std::map<char*, vtkSmartPointer<vtkPlusTrackedFrameList> >::iterator indexIterator = this->IntermediateImageMap.find(fileNamePostfix);
  if (indexIterator != this->IntermediateImageMap.end())
  {

    //Try to save the intermediate image
    if (this->IntermediateImageMap[fileNamePostfix]->SaveToSequenceMetafile(IntermediateImageFileName + "_Plus" + std::string(fileNamePostfix) + ".mha", US_IMG_ORIENT_MF, false) == PLUS_FAIL)
    {
      LOG_ERROR("An issue occured when trying to save the intermediate image with the postfix: " << fileNamePostfix);
      return PLUS_FAIL;
    }
    else
    {
      LOG_INFO("Sucessfully wrote the intermediate image with the postfix: " << fileNamePostfix);
    }
  }

  return PLUS_SUCCESS;
}

void vtkPlusTransverseProcessEnhancer::AddIntermediateImage(char* fileNamePostfix, vtkSmartPointer<vtkImageData> image)
{
  if (fileNamePostfix == "")
  {
    LOG_WARNING("The empty string was given as an intermediate image file postfix.");
  }

  if (this->SaveIntermediateResults) //TODO: Do this check when calling this method, not inside it
  {

    // See if the intermediate image should be created
    std::map<char*, vtkSmartPointer<vtkPlusTrackedFrameList> >::iterator indexIterator = this->IntermediateImageMap.find(fileNamePostfix);
    if (indexIterator != this->IntermediateImageMap.end()){}
    else
    {
      // Create if not found
      this->IntermediateImageMap[fileNamePostfix] = vtkPlusTrackedFrameList::New();

      this->IntermediatePostfixes.push_back(fileNamePostfix);
    }

    //Add the current frame to its vtkPlusTrackedFrameList
    PlusVideoFrame linesVideoFrame;
    linesVideoFrame.DeepCopyFrom(image);
    PlusTrackedFrame linesTrackedFrame;
    linesTrackedFrame.SetImageData(linesVideoFrame);
    this->IntermediateImageMap[fileNamePostfix]->AddTrackedFrame(&linesTrackedFrame);
  }
}

//Given a vtk filter, get the image that would display at that point and save it
void vtkPlusTransverseProcessEnhancer::AddIntermediateFromFilter(char* fileNamePostfix, vtkImageAlgorithm* imageFilter)
{
  if (fileNamePostfix == "")
  {
    LOG_WARNING("The empty string was given as an intermediate image file postfix.");
  }

  if (this->SaveIntermediateResults) //TODO: Do this check when calling this method, not inside it
  {
    imageFilter->SetOutput(this->IntermediateImage);
    imageFilter->Update();

    this->AddIntermediateImage(fileNamePostfix, this->IntermediateImage);
  }
}


//----------------------------------------------------------------------------
void vtkPlusTransverseProcessEnhancer::SetGaussianStdDev(double gaussianStdDev)
{
  this->GaussianStdDev = gaussianStdDev;
  this->GaussianSmooth->SetStandardDeviation(gaussianStdDev);
}

//----------------------------------------------------------------------------
void vtkPlusTransverseProcessEnhancer::SetGaussianKernelSize(double gaussianKernelSize)
{
  this->GaussianKernelSize = gaussianKernelSize;
  this->GaussianSmooth->SetRadiusFactor(gaussianKernelSize);
}

//----------------------------------------------------------------------------
void vtkPlusTransverseProcessEnhancer::SetThresholdInValue(double thresholdInValue)
{
  this->ThresholdInValue = thresholdInValue;
  this->Thresholder->SetInValue(thresholdInValue);
}

//----------------------------------------------------------------------------
void vtkPlusTransverseProcessEnhancer::SetThresholdOutValue(double thresholdOutValue)
{
  this->ThresholdOutValue = thresholdOutValue;
  this->Thresholder->SetOutValue(thresholdOutValue);
}

//----------------------------------------------------------------------------
void vtkPlusTransverseProcessEnhancer::SetLowerThreshold(double lowerThreshold)
{
  this->LowerThreshold = lowerThreshold;
  if (this->UpperThreshold != 0.0)
  {
    this->Thresholder->ThresholdBetween(this->LowerThreshold, this->UpperThreshold);
  }
  else
  {
    this->Thresholder->ThresholdByLower(this->LowerThreshold);
  }
}

//----------------------------------------------------------------------------
void vtkPlusTransverseProcessEnhancer::SetUpperThreshold(double upperThreshold)
{
  this->UpperThreshold = upperThreshold;
  if (this->LowerThreshold != 0.0)
  {
    this->Thresholder->ThresholdBetween(this->LowerThreshold, this->UpperThreshold);
  }
  else
  {
    this->Thresholder->ThresholdByUpper(this->UpperThreshold);
  }
}

//----------------------------------------------------------------------------
void vtkPlusTransverseProcessEnhancer::SetIslandAreaThreshold(int islandAreaThreshold)
{
  this->IslandAreaThreshold = islandAreaThreshold;
  if (islandAreaThreshold < 0)
  {
    this->IslandRemover->SetAreaThreshold(0);
  }
  // Apparently, below statement always evaluates to true, nullifying the island removal process
  /*
  else if ( islandAreaThreshold > ( boundaries[0]*boundaries[1] ) )
  {
  this->IslandRemover->SetIslandValue( boundaries[0] * boundaries[1] );
  }
  */
  else
  {
    this->IslandRemover->SetAreaThreshold(islandAreaThreshold);
  }
}



//----------------------------------------------------------------------------
// TODO: Currently not used. If won't be used, delete.
void vtkPlusTransverseProcessEnhancer::ComputeHistogram(vtkSmartPointer<vtkImageData> imageData)
{
  vtkSmartPointer<vtkImageAccumulate> histogram = vtkSmartPointer<vtkImageAccumulate>::New();
  histogram->SetInputData(imageData);
  histogram->SetComponentExtent(1, 25, 0, 0, 0, 0);
  histogram->SetComponentOrigin(1, 0, 0);
  histogram->SetComponentSpacing(10, 0, 0);
  histogram->SetIgnoreZero(true);
  histogram->Update();
}
