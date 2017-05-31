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


#include <direct.h>
//----------------------------------------------------------------------------

vtkStandardNewMacro(vtkPlusTransverseProcessEnhancer);

//----------------------------------------------------------------------------

vtkPlusTransverseProcessEnhancer::vtkPlusTransverseProcessEnhancer()
: ScanConverter(NULL),
  ConvertToLinesImage(false),
  NumberOfScanLines(0),
  NumberOfSamplesPerScanLine(0),

  RadiusStartMm(0),
  RadiusStopMm(0),
  ThetaStartDeg(0),
  ThetaStopDeg(0),

  Thresholder(NULL),
  GaussianEnabled(false),
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

  ReturnToFanImage(false),
  CurrentFrameMean(0.0),
  CurrentFrameStDev(0.0),
  CurrentFrameScalarComponentMax(0.0),
  CurrentFrameScalarComponentMin(255.0),

  ThresholdingEnabled(false),

  EdgeDetectorEnabled(false),
  ConversionImage(NULL),
  IslandRemovalEnabled(false),
  IslandAreaThreshold(-1),
  ErosionEnabled(false),
  DilationEnabled(false),
  ReconvertBinaryToGreyscale(false),

  IntermediateImage(NULL),
  LinesImage(NULL),
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

  this->SetDilationKernelSize(0, 0);
  this->SetErosionKernelSize(5, 5);
  this->SetGaussianStdDev(7.0);
  this->SetGaussianKernelSize(7.0);
  this->GaussianSmooth->SetDimensionality(2);

  this->ConversionImage = vtkSmartPointer<vtkImageData>::New();
  this->ConversionImage->SetExtent(0, 0, 0, 0, 0, 0);

  this->BinaryImageForMorphology->SetExtent(0, 0, 0, 0, 0, 0);

  this->ImageBinarizer->SetInValue(255);
  this->ImageBinarizer->SetOutValue(0);
  this->ImageBinarizer->ThresholdBetween(10, 255);
  this->IslandRemover->SetIslandValue(255);
  this->IslandRemover->SetReplaceValue(0);
  this->IslandRemover->SetAreaThreshold(0);

  this->ImageEroder->SetKernelSize(this->ErosionKernelSize[0], this->ErosionKernelSize[1], 1);
  this->ImageEroder->SetErodeValue(100);        // 100, so that it will do nothing on binary image with values 0 and 255, until respecified
  this->ImageEroder->SetDilateValue(100);

  this->IntermediateImage = vtkSmartPointer<vtkImageData>::New();
  this->LinesImage = vtkSmartPointer<vtkImageData>::New();
  this->UnprocessedLinesImage = vtkSmartPointer<vtkImageData>::New();
  this->ShadowImage = vtkSmartPointer<vtkImageData>::New();

  this->IntermediateImage->SetExtent(0, 0, 0, 0, 0, 0);
  this->LinesImage->SetExtent(0, 0, 0, 0, 0, 0);
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

        XML_READ_SCALAR_ATTRIBUTE_REQUIRED(int, GaussianKernelSize, gaussianParameters);
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
  }

  XML_READ_SCALAR_ATTRIBUTE_REQUIRED(int, NumberOfScanLines, processingElement)
    XML_READ_SCALAR_ATTRIBUTE_REQUIRED(int, NumberOfSamplesPerScanLine, processingElement)

    int rfImageExtent[6] = { 0, this->NumberOfSamplesPerScanLine - 1, 0, this->NumberOfScanLines - 1, 0, 0 };
  this->ScanConverter->SetInputImageExtent(rfImageExtent);

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
  dilationParameters->SetVectorAttribute("DilationKernelSize", 2, this->DilationKernelSize); //DilationKernelRadiusPixel

  XML_WRITE_BOOL_ATTRIBUTE(ReconvertBinaryToGreyscale, imageProcessingOperations);

  XML_WRITE_BOOL_ATTRIBUTE(ReturnToFanImage, imageProcessingOperations);

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
// Fills the lines image by subsampling the input image along scanlines.
// Also computes pixel statistics.
void vtkPlusTransverseProcessEnhancer::FillLinesImage(vtkSmartPointer<vtkPlusUsScanConvert> scanConverter, vtkSmartPointer<vtkImageData> inputImageData)
{
  int* linesImageExtent = scanConverter->GetInputImageExtent();
  int lineLengthPx = linesImageExtent[1] - linesImageExtent[0] + 1;
  int numScanLines = linesImageExtent[3] - linesImageExtent[2] + 1;

  // For calculating pixel intensity mean and variance. Algorithm taken from:
  // https://en.wikipedia.org/wiki/Algorithms_for_calculating_variance#Online_algorithm

  double mean = 0.0;
  double M2 = 0.0;
  long pixelCount = 0;
  double value = 0.0;
  double delta = 0.0;
  this->CurrentFrameScalarComponentMax = 0.0;
  this->CurrentFrameScalarComponentMin = 255.0;

  int* inputExtent = inputImageData->GetExtent();
  for (int scanLine = 0; scanLine < numScanLines; scanLine++)
  {
    double start[4] = { 0 };
    double end[4] = { 0 };
    scanConverter->GetScanLineEndPoints(scanLine, start, end);

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
      value = inputImageData->GetScalarComponentAsDouble(pixelCoordX, pixelCoordY, 0, 0);
      this->LinesImage->SetScalarComponentFromFloat(pointIndex, scanLine, 0, 0, value);

      if (this->CurrentFrameScalarComponentMax < value)
      {
        this->CurrentFrameScalarComponentMax = value;
      }
      if (this->CurrentFrameScalarComponentMin > value)
      {
        this->CurrentFrameScalarComponentMin = value;
      }

      ++pixelCount;
      delta = value - mean;
      mean = mean + delta / pixelCount;
      M2 = M2 + delta * (value - mean);
    }
  }

  this->CurrentFrameMean = mean;
  this->CurrentFrameStDev = std::sqrt(M2 / (pixelCount - 1));
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
      if (inputValue > lineMaxSoFar) { lineMaxSoFar = inputValue; }

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
void vtkPlusTransverseProcessEnhancer::VectorImageToUchar(vtkSmartPointer<vtkImageData> inputImage, vtkSmartPointer<vtkImageData> ConversionImage)
{
  unsigned char* vInput = 0;
  unsigned char* vOutput = 0;
  unsigned char edgeDetectorOutput0;
  unsigned char edgeDetectorOutput1;
  float output = 0.0;     // Keep this in [0..255] instead [0..1] for possible future optimization.

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
      output = (edgeDetectorOutput0 + edgeDetectorOutput1) / 2;                                         // Not mathematically correct, but a quick approximation of sqrt(x^2 + y^2)
      if (output > 255) { (*vOutput) = 255; }
      else if (output < 0) { (*vOutput) = 0; }
      else { (*vOutput) = (unsigned char)output; }
    }
  }
  inputImage->Modified();
}

//----------------------------------------------------------------------------
// If a pixel in MaskImage is > 0, the corresponding pixel in InputImage will remain unchanged, otherwise it will be set to 0
void vtkPlusTransverseProcessEnhancer::ImageConjunction(vtkSmartPointer<vtkImageData> InputImage, vtkSmartPointer<vtkImageData> MaskImage)
{
  // Images must be of the same dimension, an should already be, I should check this though
  unsigned char* inputPixelPointer = 0;
  unsigned char* maskImagePixel = 0;

  int dims[3] = { 0, 0, 0 };
  this->LinesImage->GetDimensions(dims);      // This will be the same as InputImage, as long as InputImage is converted to linesImage previously

  for (int y = 0; y < dims[1]; y++)
  {
    // Initialize variables for a new scan line.

    for (int x = dims[0] - 1; x >= 0; x--)   // Go towards transducer
    {
      if (static_cast<unsigned char>(MaskImage->GetScalarComponentAsFloat(x, y, 0, 0)) > 0)
      {
        // Do nothing
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
  PlusVideoFrame* inputImage = inputFrame->GetImageData();
  
  if (this->ScanConverter.GetPointer() == NULL)
  {
    return PLUS_FAIL;
  }

  if (this->ConvertToLinesImage)
  {
    this->ScanConverter->SetInputData(inputImage->GetImage());
    this->ScanConverter->Update();
    // Generate lines image.
    this->FillLinesImage(this->ScanConverter, inputImage->GetImage());
    this->ProcessLinesImage();
    this->AddIntermediateImage("_01Lines_1PostProcessLinesImage", this->IntermediateImage);
    this->FillShadowValues();
    this->AddIntermediateImage("_01Lines_2PostFillShadowValues", this->ShadowImage);
    this->IntermediateImage->DeepCopy(this->LinesImage);
    this->AddIntermediateImage("_01Lines_3FilterEnd", this->IntermediateImage);
  }
  else
  {
    this->IntermediateImage->DeepCopy(inputImage->GetImage());
  }


  if (this->ThresholdingEnabled)
  {
    this->Thresholder->SetInputData(this->IntermediateImage);
    this->Thresholder->Update();
    this->IntermediateImage->DeepCopy(this->Thresholder->GetOutput());
    this->AddIntermediateImage("_02Threshold_1FilterEnd", this->IntermediateImage);
  }

  if (this->GaussianEnabled)
  {
    this->GaussianSmooth->SetInputData(this->IntermediateImage);
    this->GaussianSmooth->Update();
    this->IntermediateImage->DeepCopy(this->GaussianSmooth->GetOutput());
    this->IntermediateImage->Modified();
    this->AddIntermediateImage("_03Gaussian_1FilterEnd", this->IntermediateImage);
  }

  // Store current itermediately processed image for pixel value retrieval after operations requiring binarization
  this->UnprocessedLinesImage->DeepCopy(this->IntermediateImage);

  if (this->EdgeDetectorEnabled)
  {
    this->EdgeDetector->SetInputData(this->IntermediateImage);
    this->EdgeDetector->Update();
    this->AddIntermediateImage("_04EdgeDetector_1PostEdgeDetectorUpdate", this->EdgeDetector->GetOutput());
    this->VectorImageToUchar(this->EdgeDetector->GetOutput(), this->ConversionImage);
    this->IntermediateImage->DeepCopy(this->ConversionImage);
    this->IntermediateImage->Modified();
    this->AddIntermediateImage("_04EdgeDetector_2FilterEnd", this->IntermediateImage);
  }

  // If we are to perform any morphological operations, we must binarize the image
  if (this->IslandRemovalEnabled || this->ErosionEnabled || this->DilationEnabled)
  {
    this->ImageBinarizer->SetInputData(this->IntermediateImage);
    this->ImageBinarizer->Update();
    this->BinaryImageForMorphology->DeepCopy(this->ImageBinarizer->GetOutput());
    this->AddIntermediateImage("_05BinaryImageForMorphology_1FilterEnd", this->BinaryImageForMorphology);

    if (this->IslandRemovalEnabled)
    {
      this->IslandRemover->SetInputData(this->BinaryImageForMorphology);
      this->IslandRemover->Update();
      this->BinaryImageForMorphology->DeepCopy(this->IslandRemover->GetOutput());
      this->AddIntermediateImage("_06Island_1FilterEnd", this->IslandRemover->GetOutput());
    }
    if (this->ErosionEnabled)
    {
      this->ImageEroder->SetErodeValue(255);
      this->ImageEroder->SetDilateValue(0);             // We must dilate that which isn't eroded, for erosion to be possible
      this->ImageEroder->SetKernelSize(this->ErosionKernelSize[0], this->ErosionKernelSize[1], 1);
      this->ImageEroder->SetInputData(this->BinaryImageForMorphology);
      this->ImageEroder->Update();
      this->BinaryImageForMorphology->DeepCopy(this->ImageEroder->GetOutput());
      this->AddIntermediateImage("_07Erosion_1FilterEnd", this->ImageEroder->GetOutput());

      //reset the values
      this->ImageEroder->SetErodeValue(100);
      this->ImageEroder->SetDilateValue(100);
    }
    if (this->DilationEnabled)
    {
      this->ImageEroder->SetDilateValue(255);
      this->ImageEroder->SetErodeValue(0);
      this->ImageEroder->SetKernelSize(this->DilationKernelSize[0], this->DilationKernelSize[1], 1);
      this->ImageEroder->SetInputData(this->BinaryImageForMorphology);
      this->ImageEroder->Update();
      this->BinaryImageForMorphology->DeepCopy(this->ImageEroder->GetOutput());
      this->AddIntermediateImage("_08Dilation_1FilterEnd", this->ImageEroder->GetOutput());

      //reset the values
      this->ImageEroder->SetDilateValue(100);
      this->ImageEroder->SetErodeValue(100);
    }
    if (this->ReconvertBinaryToGreyscale)
    {
      ImageConjunction(this->UnprocessedLinesImage, this->BinaryImageForMorphology);           // Currently, inputImage is the output of the edge detector, not original pixels
      this->IntermediateImage->DeepCopy(this->UnprocessedLinesImage);
      this->AddIntermediateImage("_09ReconvertBinaryToGreyscale_1FilterEnd", this->IntermediateImage);
    }
    else
    {
      this->IntermediateImage->DeepCopy(this->BinaryImageForMorphology);
    }
  }


  PlusVideoFrame* outputImage = outputFrame->GetImageData();
  if (this->ReturnToFanImage)
  {
    this->ScanConverter->SetInputData(this->IntermediateImage);
    this->ScanConverter->Update();
    outputImage->DeepCopyFrom(this->ScanConverter->GetOutput());
    this->AddIntermediateImage("_10ReturnToFanImage_1FilterEnd", this->ScanConverter->GetOutput());
  }
  else
  {
    outputImage->DeepCopyFrom(this->IntermediateImage);
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusTransverseProcessEnhancer::SaveAllIntermediateResultsToFile()
{
  /*
  Finds and saves all intermediate images that have been recorded.
  Saves the images by calling this->SaveIntermediateResultToFile()
  Returns PLUS_FAIL if this->SaveIntermediateResultToFile() encounters an error occured during this
  process, returns PLUS_SUCCESS otherwise.
  */

  for (int postfixIndex = this->IntermediatePostfixes.size() - 1; postfixIndex >= 0; postfixIndex -= 1){
    if (this->SaveIntermediateResultToFile(this->IntermediatePostfixes.at(postfixIndex)) == PLUS_FAIL)
    {
      return PLUS_FAIL;
    }
  }
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusTransverseProcessEnhancer::SaveIntermediateResultToFile(char* fileNamePostfix)
{
  /*
  Takes a postfix as an argument and saves the intermediate image associated with that postfix
  Returns PLUS_FAIL if an error occured during this process, returns PLUS_SUCCESS otherwise
  */

  std::map<char*, vtkSmartPointer<vtkPlusTrackedFrameList> >::iterator indexIterator = this->IntermediateImageMap.find(fileNamePostfix);
  if (indexIterator != this->IntermediateImageMap.end())
  {

    //Try to save the intermediate image
    if (this->IntermediateImageMap[fileNamePostfix]->SaveToSequenceMetafile(IntermediateImageFileName + std::string(fileNamePostfix) + ".mha", US_IMG_ORIENT_MF, false) == PLUS_FAIL)
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

  if (this->SaveIntermediateResults)
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


//----------------------------------------------------------------------------
void vtkPlusTransverseProcessEnhancer::SetGaussianStdDev(double gaussianStdDev)
{
  this->GaussianStdDev = gaussianStdDev;
  this->GaussianSmooth->SetStandardDeviation(gaussianStdDev);
}

//----------------------------------------------------------------------------
void vtkPlusTransverseProcessEnhancer::SetGaussianKernelSize(int gaussianKernelSize)
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
