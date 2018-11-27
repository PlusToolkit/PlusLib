/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

// Local includes
#include "PlusConfigure.h"
#include "PlusMath.h"
#include "vtkPlusBoneEnhancer.h"
#include "vtkPlusUsScanConvertCurvilinear.h"
#include "vtkPlusUsScanConvertLinear.h"
#include "vtkPlusSequenceIO.h"

// VTK includes
#include <vtkImageDilateErode3D.h>
#include <vtkImageGaussianSmooth.h>
#include <vtkImageIslandRemoval2D.h>
#include <vtkImageSobel2D.h>
#include <vtkImageThreshold.h>
#include <vtkObjectFactory.h>
#include "vtkImageAlgorithm.h"

#include <igsioTrackedFrame.h>
#include <igsioVideoFrame.h>
#include <vtkIGSIOTrackedFrameList.h>


#include <cmath>

//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkPlusBoneEnhancer);

//----------------------------------------------------------------------------
vtkPlusBoneEnhancer::vtkPlusBoneEnhancer()
: ScanConverter(NULL),
  NumberOfScanLines(0),
  NumberOfSamplesPerScanLine(0),

  RadiusStartMm(0),
  RadiusStopMm(0),
  ThetaStartDeg(0),
  ThetaStopDeg(0),

  GaussianSmooth(NULL),
  EdgeDetector(NULL),
  ImageBinarizer(NULL),
  BinaryImageForMorphology(NULL),
  IslandRemover(NULL),
  ImageEroder(NULL),
  ImageDialator(NULL),

  ConversionImage(NULL),
  IslandAreaThreshold(-1),
  BoneOutlineDepthPx(3), // Note: this only changes the appearance/thickness of the 3D model. Different numbers do not change what is or is not marked as bone.
  BonePushBackPx(9),     // Horizontal distance between where a shadow is located, and where the bone begins

  LinesImage(NULL),
  ProcessedLinesImage(NULL),
  FirstFrame(true),

  SaveIntermediateResults(false)
{

  this->GaussianSmooth = vtkSmartPointer<vtkImageGaussianSmooth>::New();    // Used to smooth the image
  this->EdgeDetector = vtkSmartPointer<vtkImageSobel2D>::New();             // Used to outline edges of the image
  this->ImageBinarizer = vtkSmartPointer<vtkImageThreshold>::New();         // Used to convert into a binary image
  this->BinaryImageForMorphology = vtkSmartPointer<vtkImageData>::New();    // The Binary image
  this->IslandRemover = vtkSmartPointer<vtkImageIslandRemoval2D>::New();    // Used to remove islands (small isolated groups of pixels)
  this->ImageEroder = vtkSmartPointer<vtkImageDilateErode3D>::New();        // Used to Erode the image
  this->ImageDialator = vtkSmartPointer<vtkImageDilateErode3D>::New();      // Used to Dilate the image


  // Set the default parameters for the filters mentioned above
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

  this->LinesImage = vtkSmartPointer<vtkImageData>::New();
  this->ProcessedLinesImage = vtkSmartPointer<vtkImageData>::New();

  this->LinesImage->SetExtent(0, 0, 0, 0, 0, 0);
  this->ProcessedLinesImage->SetExtent(0, 0, 0, 0, 0, 0);

  this->IntermediateImageMap.clear();
}

//----------------------------------------------------------------------------
vtkPlusBoneEnhancer::~vtkPlusBoneEnhancer()
{
  // Make sure contained smart pointers are deleted
  this->IntermediateImageMap.clear();
  this->IntermediatePostfixes.clear();
}

//----------------------------------------------------------------------------
void vtkPlusBoneEnhancer::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusBoneEnhancer::ReadConfiguration(vtkSmartPointer<vtkXMLDataElement> processingElement)
{
  XML_VERIFY_ELEMENT(processingElement, this->GetTagName());

  //Read things in the ScanConversion tag
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

    // Read SaveIntermediateResults tag
    vtkSmartPointer<vtkXMLDataElement> saveIntermediateResultsBool = imageProcessingOperations->FindNestedElementWithName("SaveIntermediateResults");
    if (saveIntermediateResultsBool == NULL)
    {
      LOG_WARNING("Unable to locate SaveIntermediateResults element. Using default value: " << std::boolalpha << (this->SaveIntermediateResults));
    }
    else
    {
      XML_READ_BOOL_ATTRIBUTE_OPTIONAL(SaveIntermediateResults, saveIntermediateResultsBool);
    }
    
    // Read tags related to the Gaussian filter
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

    // Read tags related to Island Removal
    vtkSmartPointer<vtkXMLDataElement> islandRemovalParameters = imageProcessingOperations->FindNestedElementWithName("IslandRemoval");
    if (islandRemovalParameters == NULL)
    {
      LOG_WARNING("Unable to locate IslandRemoval parameters element. Using default values.");
    }
    else
    {
      XML_READ_SCALAR_ATTRIBUTE_REQUIRED(int, IslandAreaThreshold, islandRemovalParameters);
    }

    // Read tags related to Erosion
    vtkSmartPointer<vtkXMLDataElement> erosionParameters = imageProcessingOperations->FindNestedElementWithName("Erosion");
    if (erosionParameters == NULL)
    {
      LOG_WARNING("Unable to locate Erosion paramters element. Using default values.");
    }
    else
    {
      XML_READ_VECTOR_ATTRIBUTE_REQUIRED(int, 2, ErosionKernelSize, erosionParameters);
    }

    // Read tags related to Dialation
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
  else
  {
    // If this section in not in the xml file, use all filters with default values
    LOG_INFO("ImageProcessingOperations section not found in config file");
    LOG_INFO("Enabling all filters and using default values.");
  }

  // Read tags related to scan lines
  XML_READ_SCALAR_ATTRIBUTE_REQUIRED(int, NumberOfScanLines, processingElement);
  XML_READ_SCALAR_ATTRIBUTE_REQUIRED(int, NumberOfSamplesPerScanLine, processingElement);

  int rfImageExtent[6] = { 0, this->NumberOfSamplesPerScanLine - 1, 0, this->NumberOfScanLines - 1, 0, 0 };
  this->ScanConverter->SetInputImageExtent(rfImageExtent);

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
// Writes the parameters that were used to a config file
PlusStatus vtkPlusBoneEnhancer::WriteConfiguration(vtkSmartPointer<vtkXMLDataElement> processingElement)
{
  XML_VERIFY_ELEMENT(processingElement, this->GetTagName());

  //Write the parameters for filters to the scanner's properties to the output config file
  processingElement->SetAttribute("Type", this->GetProcessorTypeName());
  processingElement->SetIntAttribute("NumberOfScanLines", NumberOfScanLines);
  processingElement->SetIntAttribute("NumberOfSamplesPerScanLine", NumberOfSamplesPerScanLine);

  XML_FIND_NESTED_ELEMENT_CREATE_IF_MISSING(scanConversionElement, processingElement, "ScanConversion");
  this->ScanConverter->WriteConfiguration(scanConversionElement);
  scanConversionElement->SetDoubleAttribute("RadiusStartMm", this->RadiusStartMm);
  scanConversionElement->SetDoubleAttribute("RadiusStopMm", this->RadiusStopMm);
  scanConversionElement->SetIntAttribute("ThetaStartDeg", this->ThetaStartDeg);
  scanConversionElement->SetIntAttribute("ThetaStopDeg", this->ThetaStopDeg);

  //Write the parameters for filters to the output config file
  XML_FIND_NESTED_ELEMENT_CREATE_IF_MISSING(imageProcessingOperations, processingElement, "ImageProcessingOperations");

  XML_FIND_NESTED_ELEMENT_CREATE_IF_MISSING(saveIntermediateResultsBool, imageProcessingOperations, "SaveIntermediateResults");
  XML_WRITE_BOOL_ATTRIBUTE(SaveIntermediateResults, saveIntermediateResultsBool)

  XML_FIND_NESTED_ELEMENT_CREATE_IF_MISSING(gaussianParameters, imageProcessingOperations, "GaussianSmoothing");
  gaussianParameters->SetDoubleAttribute("GaussianStdDev", this->GaussianStdDev);
  gaussianParameters->SetDoubleAttribute("GaussianKernelSize", this->GaussianKernelSize);

  XML_FIND_NESTED_ELEMENT_CREATE_IF_MISSING(islandRemovalParameters, imageProcessingOperations, "IslandRemoval");
  islandRemovalParameters->SetIntAttribute("IslandAreaThreshold", IslandAreaThreshold);

  XML_FIND_NESTED_ELEMENT_CREATE_IF_MISSING(erosionParameters, imageProcessingOperations, "Erosion");
  erosionParameters->SetVectorAttribute("ErosionKernelSize", 2, this->ErosionKernelSize);

  XML_FIND_NESTED_ELEMENT_CREATE_IF_MISSING(dilationParameters, imageProcessingOperations, "Dilation");
  dilationParameters->SetVectorAttribute("DilationKernelSize", 2, this->DilationKernelSize);

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusBoneEnhancer::ProcessImageExtents()
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

  //Set up variables related to image extents
  int dims[3] = { 0, 0, 0 };
  this->LinesImage->GetDimensions(dims);

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
// Fills the lines image by subsampling the input image along scanlines.
// Also computes pixel statistics.
void vtkPlusBoneEnhancer::FillLinesImage(vtkSmartPointer<vtkImageData> inputImageData)
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

  double directionVectorX;
  double directionVectorY;
  int pixelCoordX;
  int pixelCoordY;

  int* inputExtent = inputImageData->GetExtent();
  for (int scanLine = 0; scanLine < numScanLines; ++scanLine)
  {
    double start[4] = { 0, 0, 0, 0 };
    double end[4] = { 0, 0, 0, 0 };
    ScanConverter->GetScanLineEndPoints(scanLine, start, end);

    directionVectorX = static_cast<double>(end[0] - start[0]) / (lineLengthPx - 1);
    directionVectorY = static_cast<double>(end[1] - start[1]) / (lineLengthPx - 1);
    for (int pointIndex = 0; pointIndex < lineLengthPx; ++pointIndex)
    {
      pixelCoordX = start[0] + directionVectorX * pointIndex;
      pixelCoordY = start[1] + directionVectorY * pointIndex;
      if (pixelCoordX < inputExtent[0] || pixelCoordX > inputExtent[1]
        || pixelCoordY < inputExtent[2] || pixelCoordY > inputExtent[3])
      {
        this->LinesImage->SetScalarComponentFromFloat(pointIndex, scanLine, 0, 0, 0);
        continue; // outside of the specified extent
      }
      currentValue = inputImageData->GetScalarComponentAsDouble(pixelCoordX, pixelCoordY, 0, 0);
      this->LinesImage->SetScalarComponentFromFloat(pointIndex, scanLine, 0, 0, currentValue);

      ++pixelCount;
      valueMeanDiff = currentValue - mean;
      mean = mean + valueMeanDiff / pixelCount;
      sumSquareDiff = sumSquareDiff + valueMeanDiff * (currentValue - mean);
    }
  }
}

//----------------------------------------------------------------------------
void vtkPlusBoneEnhancer::VectorImageToUchar(vtkSmartPointer<vtkImageData> inputImage)
{
  unsigned char* vOutput = 0;
  unsigned char edgeDetectorOutput0;
  unsigned char edgeDetectorOutput1;
  float output = 0.0;     // Keep this in [0..255] instead [0..1] for possible future optimization.
  float output2 = 0.0;

  int dims[3] = { 0, 0, 0 };
  this->LinesImage->GetDimensions(dims);
  this->ConversionImage->SetExtent(this->LinesImage->GetExtent());
  this->ConversionImage->AllocateScalars(VTK_UNSIGNED_CHAR, 1);
  for (int y = dims[1] - 1; y >= 0; --y)
  {
    // Initialize variables for a new scan line.

    for (int x = dims[0] - 1; x >= 0; --x)   // Go towards transducer
    {
      edgeDetectorOutput0 = static_cast<unsigned char>(inputImage->GetScalarComponentAsFloat(x, y, 0, 0));
      edgeDetectorOutput1 = static_cast<unsigned char>(inputImage->GetScalarComponentAsFloat(x, y, 0, 1));
      vOutput = static_cast<unsigned char*>(this->ConversionImage->GetScalarPointer(x, y, 0));
      output = (float)(edgeDetectorOutput0 + edgeDetectorOutput1) / (float)2;                                         // Not mathematically correct, but a quick approximation of sqrt(x^2 + y^2)

      *vOutput = (unsigned char)std::max(0, std::min(255, (int)output));
    }
  }
}

//----------------------------------------------------------------------------
/*
Takes a vtkSmartPointer<vtkImageData> as an argument and modifies it such that all images in a row
that have a bone shadow behind it are removed
*/
void vtkPlusBoneEnhancer::MarkShadowOutline(vtkSmartPointer<vtkImageData> inputImage)
{
  int dims[3] = { 0, 0, 0 };
  inputImage->GetDimensions(dims);

  int keepInfoCounter;
  bool foundBone;
  unsigned char* vOutput;

  int lastVistedValue = 0;

  //Setup variables for recording bone areas
  std::map<std::string, int> currentBoneArea;
  int boneAreaStart = dims[1] - 1;  //The y coordinate of where the bone outline starts
  int boneDepthSum = 0;             //The sum of the x coordinates of each pixel in the bone outline
  int boneMaxDepth = dims[0] - 1;   //The x coordinate of the right-most pixel in the bone outline
  int boneMinDepth = 0;             //The x coordinate of the left-most pixel in the bone outline
  int boneAreaDifferenceSlope = 3;  //If two pixels are seperated by this value or greater in the x coordinate, they are marked as seperate bones

  for (int y = dims[1] - 1; y >= 0; --y)
  {

    //When an image is detected, keep up to this many pixles after it
    keepInfoCounter = this->BoneOutlineDepthPx + this->BonePushBackPx;
    foundBone = false;

    for (int x = dims[0] - 1; x >= 0; --x)
    {
      vOutput = static_cast<unsigned char*>(inputImage->GetScalarPointer(x, y, 0));

      //If an image is detected
      if (*vOutput != 0)
      {
        if (keepInfoCounter == 0 || keepInfoCounter > this->BoneOutlineDepthPx)
        {
          *vOutput = 0;
        }

        if (keepInfoCounter == this->BoneOutlineDepthPx + this->BonePushBackPx)
        {
          if (foundBone == false)
          {
            //found the first bone
            foundBone = true;

            //the two bone pixels are far enough appart, save them as being parts of different bone areas
            if (std::abs(x - lastVistedValue) >= boneAreaDifferenceSlope  && y != dims[1] - 1)
            {
              //check if the preveous area had any bone
              if (boneDepthSum != 0)
              {
                //Save info related to where the bone area
                currentBoneArea["depth"] = boneDepthSum / (boneAreaStart - y);                  // Store the outline's average x-coordinate
                currentBoneArea["xMax"] = boneMaxDepth;                                         // Store the outline's maximum x-coordinate (Used for efficiency)
                currentBoneArea["xMin"] = std::max(boneMinDepth - this->BoneOutlineDepthPx, 0); // Store the outline's minimum x-coordinate (Used for efficiency)
                currentBoneArea["yMax"] = boneAreaStart;                                        // Store the outline's maximum y-coordinate
                currentBoneArea["yMin"] = y + 1;                                                // Store the outline's minimum y-coordinate
                this->BoneAreasInfo.push_back(currentBoneArea);
                currentBoneArea.clear();
              }
              boneAreaStart = y;
              boneDepthSum = 0;
              boneMaxDepth = x;
              boneMinDepth = x;
            }
            else
            {
              if (x > boneMaxDepth)
              {
                boneMaxDepth = x;
              }
              if (x < boneMinDepth)
              {
                boneMinDepth = x;
              }
            }
            boneDepthSum += x;
            lastVistedValue = x;

          }
        }
      }
      if (foundBone == true && keepInfoCounter != 0)
      {
        if (keepInfoCounter <= this->BoneOutlineDepthPx && *vOutput == 0)
        {
          *vOutput = 255;
        }
        keepInfoCounter--;
      }
    }

    //if no bones were found on this row, but there was a bone before this, save it
    if (foundBone == false)
    {
      lastVistedValue = 0;
      if (boneDepthSum != 0)
      {
        //Save info related to where the bone area
        currentBoneArea["depth"] = boneDepthSum / (boneAreaStart - y);                  // Store the outline's average x-coordinate
        currentBoneArea["xMax"] = boneMaxDepth;                                         // Store the outline's maximum x-coordinate (Used for efficiency)
        currentBoneArea["xMin"] = std::max(boneMinDepth - this->BoneOutlineDepthPx, 0); // Store the outline's minimum x-coordinate (Used for efficiency)
        currentBoneArea["yMax"] = boneAreaStart;                                        // Store the outline's maximum y-coordinate
        currentBoneArea["yMin"] = y + 1;                                                // Store the outline's minimum y-coordinate
        this->BoneAreasInfo.push_back(currentBoneArea);
        boneDepthSum = 0;
        currentBoneArea.clear();
      }
      boneMaxDepth = dims[0] - 1;
      boneMinDepth = 0;

      boneAreaStart = y - 1;
    }
  }

  //save the last bone that goes off-screen
  if (boneDepthSum != 0)
  {
    //Save info related to where the bone area
    currentBoneArea["depth"] = boneDepthSum / (boneAreaStart + 1);                  // Store the outline's average x-coordinate
    currentBoneArea["xMax"] = boneMaxDepth;                                         // Store the outline's maximum x-coordinate (Used for efficiency)
    currentBoneArea["xMin"] = std::max(boneMinDepth - this->BoneOutlineDepthPx, 0); // Store the outline's minimum x-coordinate (Used for efficiency)
    currentBoneArea["yMax"] = boneAreaStart;                                        // Store the outline's maximum y-coordinate
    currentBoneArea["yMin"] = 0;                                                    // Store the outline's minimum y-coordinate
    this->BoneAreasInfo.push_back(currentBoneArea);
    currentBoneArea.clear();
  }
}

//----------------------------------------------------------------------------
//a way of threasholding based on the standard deviation of a row
void vtkPlusBoneEnhancer::ThresholdViaStdDeviation(vtkSmartPointer<vtkImageData> inputImage)
{
  int fatLayerToCut = 20; //The area of fat too close to the transducer should not be considered

  float vInput = 0;
  unsigned char* vOutput = 0;

  int dims[3] = { 0, 0, 0 };
  inputImage->GetDimensions(dims);

  int max;

  //values used to calculate the standard deviation
  int pixelSum;
  int squearSum;
  float pixelAverage;
  float meanDiffSum;
  float meanDiffAverage;
  float thresholdValue;

  for (int y = dims[1] - 1; y >= 0; --y)
  {
    max = 0;

    pixelSum = 0;
    squearSum = 0;
    pixelAverage = 0;

    //determine the average, sum, and max of the row
    for (int x = dims[0] - 1; x >= fatLayerToCut; --x)
    {
      vInput = inputImage->GetScalarComponentAsFloat(x, y, 0, 0);
      pixelSum += vInput;
      squearSum += vInput * vInput;

      if (vInput > max)
      {
        max = vInput;
      }
    }
    pixelAverage = pixelSum / (dims[0] - fatLayerToCut);

    //determine the standard deviation of the row
    meanDiffSum = squearSum + (dims[0] - fatLayerToCut) * pixelAverage * pixelAverage + (-2 * pixelAverage * pixelSum);
    meanDiffAverage = meanDiffSum / (dims[0] - fatLayerToCut);
    thresholdValue = max - 3 * pow(meanDiffAverage, 0.5f);


    //if a pixel's value is too low, remove it
    if (pixelSum != 0)
    {
      for (int x = dims[0] - 1; x >= 0; --x)
      {
        vOutput = static_cast<unsigned char*>(inputImage->GetScalarPointer(x, y, 0));
        if (*vOutput < thresholdValue && *vOutput != 0)
        {
          *vOutput = 0;
        }
      }
    }
  }
}

//----------------------------------------------------------------------------
// If a pixel in MaskImage is > 0, the corresponding pixel in InputImage will remain unchanged, otherwise it will be set to 0
void vtkPlusBoneEnhancer::ImageConjunction(vtkSmartPointer<vtkImageData> InputImage, vtkSmartPointer<vtkImageData> MaskImage)
{
  // Images must be of the same dimension, an should already be, I should check this though
  unsigned char* inputPixelPointer = 0;

  int dims[3] = { 0, 0, 0 };
  this->LinesImage->GetDimensions(dims);      // This will be the same as InputImage, as long as InputImage is converted to linesImage previously

  for (int y = dims[1] - 1; y >= 0; --y)
  {
    // Initialize variables for a new scan line.

    for (int x = dims[0] - 1; x >= 0; --x)   // Go towards transducer
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
// Processes a given frame and marks potential bone areas.
PlusStatus vtkPlusBoneEnhancer::ProcessFrame(igsioTrackedFrame* inputFrame, igsioTrackedFrame* outputFrame)
{
  //Process the input into a linear image
  vtkSmartPointer<vtkImageData> intermediateImage = this->UnprocessedFrameToLinearImage(inputFrame);
  //Remove noise and mark all possible bones
  this->RemoveNoise(intermediateImage);
  //Reconvert the image back into a fan-image and return it
  this->LinearToFanImage(intermediateImage, outputFrame);
  return PLUS_SUCCESS;
}

void vtkPlusBoneEnhancer::LinearToFanImage(vtkSmartPointer<vtkImageData> inputImage, igsioTrackedFrame* outputFrame)
{

  //Setup so that the image can be converted into a fan-image
  this->ProcessedLinesImage->DeepCopy(inputImage);
  igsioVideoFrame* outputImage = outputFrame->GetImageData();
  this->ScanConverter->SetInputData(this->ProcessedLinesImage);
  this->ScanConverter->SetOutput(inputImage);
  this->ScanConverter->Update();

  outputImage->DeepCopyFrom(inputImage);
}

//----------------------------------------------------------------------------
// takes an unprocessed frame image and returns it as a linear image
vtkSmartPointer<vtkImageData> vtkPlusBoneEnhancer::UnprocessedFrameToLinearImage(igsioTrackedFrame* inputFrame)
{
  if (this->FirstFrame == true)
  {
    //set up variables for future loops
    this->ProcessImageExtents();
    this->FirstFrame = false;
  }
  this->BoneAreasInfo.clear();

  igsioVideoFrame* inputImage = inputFrame->GetImageData();
  //an image used to transport output between filters
  vtkSmartPointer<vtkImageData> intermediateImage = vtkSmartPointer<vtkImageData>::New();

  //Convert the image to a readable non-fan image
  this->ScanConverter->SetInputData(inputImage->GetImage());
  // Generate lines image.
  if (this->SaveIntermediateResults)
  {
    this->AddIntermediateFromFilter("_01Lines_1PreFillLines", this->ScanConverter);
  }
  this->FillLinesImage(inputImage->GetImage());
  if (this->SaveIntermediateResults)
  {
    this->AddIntermediateImage("_01Lines_2FilterEnd", this->LinesImage);
  }
  intermediateImage->DeepCopy(this->LinesImage);

  return intermediateImage;
}

//----------------------------------------------------------------------------
// Takes an Ultrasound image and removes all noise, then marks all potential 
// bone areas using a white outline.
void vtkPlusBoneEnhancer::RemoveNoise(vtkSmartPointer<vtkImageData> inputImage)
{

  //Threashold the image based on the standard deviation of a pixel's columns
  this->ThresholdViaStdDeviation(inputImage);
  if (this->SaveIntermediateResults)
  {
    this->AddIntermediateImage("_02Threshold_1FilterEnd", inputImage);
  }

  //Use gaussian smoothing
  this->GaussianSmooth->SetInputData(inputImage);
  if (this->SaveIntermediateResults)
  {
    this->AddIntermediateFromFilter("_03Gaussian_1FilterEnd", this->GaussianSmooth);
  }

  //Edge detection
  this->EdgeDetector->SetInputConnection(this->GaussianSmooth->GetOutputPort());
  this->EdgeDetector->Update();
  this->VectorImageToUchar(this->EdgeDetector->GetOutput());
  if (this->SaveIntermediateResults)
  {
    this->AddIntermediateImage("_04EdgeDetector_1FilterEnd", this->ConversionImage);
  }

  // Since we perform morphological operations, we must binarize the image
  this->ImageBinarizer->SetInputData(this->ConversionImage);
  if (this->SaveIntermediateResults)
  {
    this->AddIntermediateFromFilter("_05BinaryImageForMorphology_1FilterEnd", this->ImageBinarizer);
  }

  //Remove small clusters of pixels
  this->IslandRemover->SetInputConnection(this->ImageBinarizer->GetOutputPort());
  this->IslandRemover->Update();
  if (this->SaveIntermediateResults)
  {
    this->AddIntermediateImage("_06Island_1FilterEnd", this->IslandRemover->GetOutput());
  }

  //Erode the image
  this->ImageEroder->SetKernelSize(this->ErosionKernelSize[0], this->ErosionKernelSize[1], 1);
  this->ImageEroder->SetInputConnection(this->IslandRemover->GetOutputPort());
  if (this->SaveIntermediateResults)
  {
    this->AddIntermediateFromFilter("_07Erosion_1FilterEnd", this->ImageEroder);
  }

  //Dilate the image
  this->ImageDialator->SetKernelSize(this->DilationKernelSize[0], this->DilationKernelSize[1], 1);
  this->ImageDialator->SetInputConnection(this->ImageEroder->GetOutputPort());
  this->ImageDialator->Update();
  this->BinaryImageForMorphology->DeepCopy(this->ImageDialator->GetOutput());
  if (this->SaveIntermediateResults)
  {
    this->AddIntermediateImage("_08Dilation_1FilterEnd", this->BinaryImageForMorphology);
  }

  //Detect each possible bone area, then subject it to various tests to confirm if it is valid
  this->MarkShadowOutline(this->BinaryImageForMorphology);
  if (this->SaveIntermediateResults)
  {
    this->AddIntermediateImage("_09PostFilters_1ShadowOutline", this->BinaryImageForMorphology);
  }

  // Save all stored intermediate images to mha files in output
  if (this->SaveIntermediateResults)
  {
    this->SaveAllIntermediateResultsToFile();
  }
  
  inputImage->DeepCopy(this->BinaryImageForMorphology);
}


//----------------------------------------------------------------------------
/*
Finds and saves all intermediate images that have been recorded.
Saves the images by calling this->SaveIntermediateResultToFile()
Returns PLUS_FAIL if this->SaveIntermediateResultToFile() encounters an error occured during this
process, returns PLUS_SUCCESS otherwise.
*/
PlusStatus vtkPlusBoneEnhancer::SaveAllIntermediateResultsToFile()
{
  for (int postfixIndex = this->IntermediatePostfixes.size() - 1; postfixIndex >= 0; postfixIndex -= 1)
  {
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
PlusStatus vtkPlusBoneEnhancer::SaveIntermediateResultToFile(char* fileNamePostfix)
{
  std::map<char*, vtkSmartPointer<vtkIGSIOTrackedFrameList> >::iterator indexIterator = this->IntermediateImageMap.find(fileNamePostfix);
  if (indexIterator != this->IntermediateImageMap.end())
  {
    //Try to save the intermediate image
    if (vtkPlusSequenceIO::Write(IntermediateImageFileName + "_Plus" + std::string(fileNamePostfix) + ".mha", this->IntermediateImageMap[fileNamePostfix], US_IMG_ORIENT_MF, false) == PLUS_FAIL)
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

//----------------------------------------------------------------------------
void vtkPlusBoneEnhancer::AddIntermediateImage(char* fileNamePostfix, vtkSmartPointer<vtkImageData> image)
{
  if (fileNamePostfix == "")
  {
    LOG_WARNING("The empty string was given as an intermediate image file postfix.");
  }

  // See if the intermediate image should be created
  std::map<char*, vtkSmartPointer<vtkIGSIOTrackedFrameList> >::iterator indexIterator = this->IntermediateImageMap.find(fileNamePostfix);
  if (indexIterator != this->IntermediateImageMap.end()){}
  else
  {
    // Create if not found
    this->IntermediateImageMap[fileNamePostfix] = vtkIGSIOTrackedFrameList::New();

    this->IntermediatePostfixes.push_back(fileNamePostfix);
  }

  //Add the current frame to its vtkIGSIOTrackedFrameList
  igsioVideoFrame linesVideoFrame;
  linesVideoFrame.DeepCopyFrom(image);
  igsioTrackedFrame linesTrackedFrame;
  linesTrackedFrame.SetImageData(linesVideoFrame);
  this->IntermediateImageMap[fileNamePostfix]->AddTrackedFrame(&linesTrackedFrame);
}

//----------------------------------------------------------------------------
// Given a vtk filter, get the image that would display at that point and save it
void vtkPlusBoneEnhancer::AddIntermediateFromFilter(char* fileNamePostfix, vtkImageAlgorithm* imageFilter)
{
  if (fileNamePostfix == "")
  {
    LOG_WARNING("The empty string was given as an intermediate image file postfix.");
  }

  vtkSmartPointer<vtkImageData> tempOutputImage = vtkSmartPointer<vtkImageData>::New();
  imageFilter->SetOutput(tempOutputImage);
  imageFilter->Update();
  this->AddIntermediateImage(fileNamePostfix, tempOutputImage);
}

//----------------------------------------------------------------------------
void vtkPlusBoneEnhancer::SetGaussianStdDev(double gaussianStdDev)
{
  this->GaussianStdDev = gaussianStdDev;
  this->GaussianSmooth->SetStandardDeviation(gaussianStdDev);
}

//----------------------------------------------------------------------------
void vtkPlusBoneEnhancer::SetGaussianKernelSize(double gaussianKernelSize)
{
  this->GaussianKernelSize = gaussianKernelSize;
  this->GaussianSmooth->SetRadiusFactor(gaussianKernelSize);
}

//----------------------------------------------------------------------------
void vtkPlusBoneEnhancer::SetIslandAreaThreshold(int islandAreaThreshold)
{
  this->IslandAreaThreshold = islandAreaThreshold;
  if (islandAreaThreshold < 0)
  {
    this->IslandRemover->SetAreaThreshold(0);
  }
  else
  {
    this->IslandRemover->SetAreaThreshold(islandAreaThreshold);
  }
}
