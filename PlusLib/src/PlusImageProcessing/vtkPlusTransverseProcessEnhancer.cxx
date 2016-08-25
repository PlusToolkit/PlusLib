
#include <cmath>
#include "PlusConfigure.h"
#include "PlusMath.h"
#include "PlusVideoFrame.h"
#include "PlusTrackedFrame.h"
#include "vtkImageAccumulate.h"
#include "vtkImageCast.h"
#include "vtkObjectFactory.h"
#include "vtkPlusTrackedFrameList.h"
#include "vtkPlusTransverseProcessEnhancer.h"
#include "vtkPlusUsScanConvertCurvilinear.h"
#include "vtkPlusUsScanConvertLinear.h"

#include "vtkImageGaussianSmooth.h"
#include "vtkImageThreshold.h"

vtkStandardNewMacro(vtkPlusTransverseProcessEnhancer);


vtkPlusTransverseProcessEnhancer::vtkPlusTransverseProcessEnhancer()
: Thresholder(vtkSmartPointer<vtkImageThreshold>::New()),
GaussianSmooth(vtkSmartPointer<vtkImageGaussianSmooth>::New()),
LinesImage(vtkSmartPointer<vtkImageData>::New()),
SmoothedImage(vtkSmartPointer<vtkImageData>::New()),
ShadowValues(vtkSmartPointer<vtkImageData>::New()),
ProcessedLinesImage(vtkSmartPointer<vtkImageData>::New()),
linesImageList(vtkSmartPointer<vtkPlusTrackedFrameList>::New()),
IntermediateImageList(vtkSmartPointer<vtkPlusTrackedFrameList>::New()),
ProcessedLinesImageList(vtkSmartPointer<vtkPlusTrackedFrameList>::New()),
NumberOfScanLines(0),
NumberOfSamplesPerScanLine(0)
{
  //this->Thresholder->SetInValue(0);
  //this->Thresholder->SetOutValue(255);
  
  this->ThresholdingEnabled = NULL;
  this->ThresholdInValue = NULL;
  this->ThresholdOutValue = NULL;
  this->LowerThreshold = NULL;
  this->UpperThreshold = NULL;
  
  this->GaussianEnabled = NULL;
  this->SetGaussianStdDev(NULL);
  this->SetGaussianKernelSize(NULL);
  this->GaussianSmooth->SetDimensionality(2);

  this->LinesImage->SetExtent(0,0,0,0,0,0);
  this->ShadowValues->SetExtent(0,0,0,0,0,0);
  this->ProcessedLinesImage->SetExtent(0,0,0,0,0,0);

  this->currentFrameMean = 0.0;
  this->currentFrameStDev = 0.0;
  this->currentFrameMax = 0.0;
  this->currentFrameMin = 255.0;

  this->LinesImageFileName.clear();
  this->IntermediateImageFileName.clear();
  this->ProcessedLinesImageFileName.clear();
}


vtkPlusTransverseProcessEnhancer::~vtkPlusTransverseProcessEnhancer()
{
  if ( ! this->LinesImageFileName.empty() )
  {
    LOG_INFO("Writing lines image sequence");
    this->linesImageList->SaveToSequenceMetafile(this->LinesImageFileName, US_IMG_ORIENT_MF, false);
  }

  if (!this->IntermediateImageFileName.empty())
  {
    LOG_INFO("Writing intermediate image");
    this->IntermediateImageList->SaveToSequenceMetafile(this->IntermediateImageFileName, US_IMG_ORIENT_MF, false);
  }

  if ( ! this->ProcessedLinesImageFileName.empty() )
  {
    LOG_INFO("Writing processed lines image sequence");
    this->ProcessedLinesImageList->SaveToSequenceMetafile(this->ProcessedLinesImageFileName, US_IMG_ORIENT_MF, false);
  }
}


void vtkPlusTransverseProcessEnhancer::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}


PlusStatus vtkPlusTransverseProcessEnhancer::ReadConfiguration(vtkXMLDataElement* processingElement)
{
  XML_VERIFY_ELEMENT(processingElement, this->GetTagName());
  //XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(int, Threshold, processingElement);

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
    else
    {
      LOG_INFO("Scan converter is defined.");
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
  else
  {
    LOG_INFO("ScanConversion section not found in config file!");
  }
  
  // Try to make image processing parameters modifiable without rebuilding
  vtkXMLDataElement* ImageProcessingOperations = processingElement->FindNestedElementWithName("ImageProcessingOperations");
  if (ImageProcessingOperations != NULL)                                                        // This verification might not be strictly necessary
  {
    XML_READ_BOOL_ATTRIBUTE_OPTIONAL(GaussianEnabled, ImageProcessingOperations);   
    if(this->GaussianEnabled != NULL)         // Might be irellevant if set in initialization
    {
      if(this->GaussianEnabled)
      {
        vtkXMLDataElement* GaussianParameters = ImageProcessingOperations->FindNestedElementWithName("GaussianSmoothing");
        XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(double, GaussianStdDev, GaussianParameters);
        if(this->GaussianStdDev == NULL)
        {
          LOG_INFO("GaussianStdDev not instantiated in config file - using default value of 7");
          this->SetGaussianStdDev(7);
        }
        else LOG_INFO("GaussianStdDev set to " << this->GaussianStdDev << " in config file");
        
        XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(int, GaussianKernelSize, GaussianParameters);
        if(this->GaussianKernelSize == NULL)
        {
          LOG_INFO("GaussianKernelSize not instantiate in config file - using default value of 7");
          this->SetGaussianKernelSize(7);
        }
        else LOG_INFO("GaussianKernelSize set to " << this->GaussianKernelSize << "  in config file");
      }
      else LOG_INFO("GaussianEnabled set to false");
      }
    else
    {
      LOG_INFO("GaussianEnabled parameter not found, setting it to false");
      this->GaussianEnabled = false;                 // Set to false in undefined case to stay out of vtkImageGaussianSmoothin()
    }
   
    XML_READ_BOOL_ATTRIBUTE_OPTIONAL(ThresholdingEnabled, ImageProcessingOperations);
    if(this->ThresholdingEnabled != NULL)
    {
      if(this->ThresholdingEnabled)
      {
        vtkXMLDataElement* ThresholdingParameters = ImageProcessingOperations->FindNestedElementWithName("Thresholding");
        XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(double, ThresholdInValue, ThresholdingParameters);
        if(this->ThresholdInValue == NULL) this->SetThresholdInValue(0);
        
        XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(double, ThresholdOutValue, ThresholdingParameters);
        if(this->ThresholdOutValue == NULL) this->SetThresholdOutValue(255);
        
        XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(double, LowerThreshold, ThresholdingParameters);
        XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(double, UpperThreshold, ThresholdingParameters);
        if(this->LowerThreshold == NULL && this->UpperThreshold == NULL)
        {
          LOG_INFO("Thresholding operation requesting but bounds not set - thresholding will not be performed");
          this->ThresholdingEnabled = false;
        }
        else if(this->UpperThreshold == NULL)
        {
          LOG_INFO("Lower threshold set at " << this->LowerThreshold);
        }
        else if(this->LowerThreshold == NULL)
        {
          LOG_INFO("Upper threshold set at " << this->UpperThreshold);
        }
        else
        {
          LOG_INFO("Windowed threshold to be perfomed between " << this->LowerThreshold << " and " << this->UpperThreshold);
        }
      }
    }
    else
    {
      LOG_INFO("ThresholdingEnabled parameter not found, setting to false");
      this->ThresholdingEnabled = false;
    }
  }
  else
  {
    LOG_ERROR("Image processing operations not defined in processor config file.");
    return PLUS_FAIL;
  }
  
  XML_READ_SCALAR_ATTRIBUTE_REQUIRED(int, NumberOfScanLines, processingElement)
  XML_READ_SCALAR_ATTRIBUTE_REQUIRED(int, NumberOfSamplesPerScanLine, processingElement)
  
  int rfImageExtent[6] = {0,this->NumberOfSamplesPerScanLine-1,0,this->NumberOfScanLines-1,0,0};
  this->ScanConverter->SetInputImageExtent(rfImageExtent);

  // Allocate lines image.

  int* linesImageExtent = this->ScanConverter->GetInputImageExtent();
  
  LOG_DEBUG("Lines image extent: "
            << linesImageExtent[0] << ", " << linesImageExtent[1]
            << ", " << linesImageExtent[2] << ", " << linesImageExtent[3]
            << ", " << linesImageExtent[4] << ", " << linesImageExtent[5]);
  
  this->LinesImage->SetExtent(linesImageExtent);
  this->LinesImage->AllocateScalars(VTK_UNSIGNED_CHAR, 1);

  this->ShadowValues->SetExtent(linesImageExtent);
  this->ShadowValues->AllocateScalars(VTK_FLOAT, 1);

  this->ProcessedLinesImage->SetExtent(linesImageExtent);
  this->ProcessedLinesImage->AllocateScalars(VTK_UNSIGNED_CHAR, 1);

  // Lines image list is only for debugging and testing ideas.
  
  this->linesImageList->Clear();
  this->IntermediateImageList->Clear();
  this->ProcessedLinesImageList->Clear();

  return PLUS_SUCCESS;
}


PlusStatus vtkPlusTransverseProcessEnhancer::WriteConfiguration(vtkXMLDataElement* processingElement)
{
  XML_VERIFY_ELEMENT(processingElement, this->GetTagName());
  processingElement->SetDoubleAttribute("Threshold", this->GetThreshold());
  return PLUS_SUCCESS;
}


void vtkPlusTransverseProcessEnhancer::SetThreshold(double threshold)
{
  this->Thresholder->ThresholdByLower(threshold);
}


double vtkPlusTransverseProcessEnhancer::GetThreshold()
{
  return this->Thresholder->GetLowerThreshold();
}


void vtkPlusTransverseProcessEnhancer::DrawLine(vtkImageData* imageData, int* imageExtent, double* start, double* end, int numberOfPoints)
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
      imageData->SetScalarComponentFromFloat(pixelCoordX, pixelCoordY, 0, 0, 0);
      continue;
    }
    float value = imageData->GetScalarComponentAsFloat(pixelCoordX, pixelCoordY, 0, 0);
    if (value < (this->currentFrameMean + 2 * this->currentFrameStDev))
    {
      imageData->SetScalarComponentFromFloat(pixelCoordX, pixelCoordY, 0, 0, DRAWING_COLOR);
    }
  }
}


void vtkPlusTransverseProcessEnhancer::DrawScanLines(vtkPlusUsScanConvert* scanConverter, vtkImageData* imageData)
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


/**
 * Fills the lines image by subsampling the input image along scanlines.
 * Also computes pixel statistics.
 */
void vtkPlusTransverseProcessEnhancer::FillLinesImage(vtkPlusUsScanConvert* scanConverter, vtkImageData* inputImageData)
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
  this->currentFrameMax = 0.0;
  this->currentFrameMin = 255.0;
  
  int* inputExtent = inputImageData->GetExtent();
  for (int scanLine = 0; scanLine < numScanLines; scanLine ++ )
  {
    double start[4] = {0};
    double end[4] = {0};
    scanConverter->GetScanLineEndPoints(scanLine, start, end);

    double directionVectorX = static_cast<double>(end[0]-start[0])/(lineLengthPx-1);
    double directionVectorY = static_cast<double>(end[1]-start[1])/(lineLengthPx-1);
    for (int pointIndex=0; pointIndex<lineLengthPx; ++pointIndex)
    {
      int pixelCoordX = start[0] + directionVectorX * pointIndex;
      int pixelCoordY = start[1] + directionVectorY * pointIndex;
      if ( pixelCoordX<inputExtent[0] ||  pixelCoordX>inputExtent[1]
           || pixelCoordY<inputExtent[2] ||  pixelCoordY>inputExtent[3] )
      {
        this->LinesImage->SetScalarComponentFromFloat(pointIndex, scanLine, 0, 0, 0);
        continue; // outside of the specified extent
      }
      value = inputImageData->GetScalarComponentAsDouble(pixelCoordX, pixelCoordY, 0, 0);
      this->LinesImage->SetScalarComponentFromFloat(pointIndex, scanLine, 0, 0, value);

      if (this->currentFrameMax < value)
      {
        this->currentFrameMax = value;
      }
      if (this->currentFrameMin > value)
      {
        this->currentFrameMin = value;
      }

      ++ pixelCount;
      delta = value - mean;
      mean = mean + delta / pixelCount;
      M2 = M2 + delta * ( value - mean );
    }
  }

  this->currentFrameMean = mean;
  this->currentFrameStDev = std::sqrt( M2 / ( pixelCount - 1 ));
}


void vtkPlusTransverseProcessEnhancer::ProcessLinesImage()   // Temporarily modified by Ben to darken the image a bit
{
  // Parameters

  float thresholdSdFactor = 1.8;
  float nearFactor = 0.4;
  
  int dims[3] = {0, 0, 0};
  this->LinesImage->GetDimensions( dims );

  // Define the threshold value for bone candidate points.

  double dThreshold = this->currentFrameMean + thresholdSdFactor * this->currentFrameStDev;
  unsigned char threshold = 255;
  //if ( dThreshold < 255 ) threshold = (unsigned char)dThreshold;
  
  // Compute mapping factor for values above threshold (T).
  // Mapping [T,255] to [25%,100%], that is [64,255].
  // Output = delta x 191 / (255 - T ) + 64
  // where delta = PixelValue - T

  //float mappingFactor = 191.0 / (255.0 - threshold);
  //float mappingShift = 64.0;

  // Define threshold and factor for pixel locations close to transducer.

  int xClose = int( dims[0] * nearFactor );
  float xFactor = 1.0 / xClose;

  // Iterate all pixels.

  unsigned char* vInput = 0;
  unsigned char* vOutput = 0;
  float* vShadow = 0;
  bool foundInThisLine = false;
  bool decreaseAfterFound = false;
  unsigned char lastValue = 0;
  float output = 0.0;     // Keep this in [0..255] instead [0..1] for possible future optimization.
  
  this->FillShadowValues();

  // Save shadow image

  PlusVideoFrame shadowVideoFrame;
  shadowVideoFrame.DeepCopyFrom(this->ShadowValues);
  PlusTrackedFrame shadowTrackedFrame;
  shadowTrackedFrame.SetImageData(shadowVideoFrame);
  this->IntermediateImageList->AddTrackedFrame(&shadowTrackedFrame);

  for(int y = 0; y < dims[1]; y++)        
  {
    // Initialize variables for a new scan line.
    
    for(int x = dims[0] - 1; x >= 0; x--) // Go towards transducer
    {
      vInput = static_cast<unsigned char*>(this->LinesImage->GetScalarPointer(x,y,0));
      vOutput = static_cast<unsigned char*>(this->ProcessedLinesImage->GetScalarPointer(x,y,0));
      output = (*vInput);
      if(output > 255) (*vOutput) = 255;
      else if(output < 0) (*vOutput) = 0;
      else (*vOutput) = (unsigned char)output;
    }
  }
  this->ProcessedLinesImage->Modified();

}

void vtkPlusTransverseProcessEnhancer::FillShadowValues()
{
  int dims[3] = {0, 0, 0};
  this->LinesImage->GetDimensions( dims );

  float lineMeanSoFar = 0.0;
  float lineMaxSoFar = 0.0;
  int nSoFar = 0;

  unsigned char* vInput = 0;
  float* vOutput = 0;

  for (int y = 0; y < dims[1]; y ++ )
  {
    // Initialize variables for new scan line.
    lineMeanSoFar = 0.0;
    lineMaxSoFar = 0.0;
    nSoFar = 0;
    float shadowValue = 0.0;

    for (int x = dims[0] - 1; x >= 0; x--) // Go towards transducer.
    {
      vInput = static_cast<unsigned char*>(this->LinesImage->GetScalarPointer(x, y, 0));
      vOutput = static_cast<float*>(this->ShadowValues->GetScalarPointer(x, y, 0));

      unsigned char inputValue = (*vInput);

      nSoFar++;
      float diffFromMean = inputValue - lineMeanSoFar;
      lineMeanSoFar = lineMeanSoFar + diffFromMean / nSoFar;
      if (inputValue > lineMaxSoFar) lineMaxSoFar = inputValue;

      shadowValue = 1.0 - (lineMaxSoFar / this->currentFrameMax);
      
      *vOutput = shadowValue;
    }
  }
}


PlusStatus vtkPlusTransverseProcessEnhancer::ProcessFrame(PlusTrackedFrame* inputFrame, PlusTrackedFrame* outputFrame)
{
  PlusVideoFrame* inputImage = inputFrame->GetImageData();
  
  
  if (this->ScanConverter.GetPointer() == NULL)
  {
    return PLUS_FAIL;
  }

  /*
  // Generate lines image.

  this->FillLinesImage(this->ScanConverter, inputImage->GetImage());
  
  // Save lines image for debugging.

  this->linesImageList->AddTrackedFrame(inputFrame); // TODO: How to create a new tracked frame in PLUS?
  this->ProcessedLinesImageList->AddTrackedFrame(inputFrame);
  PlusTrackedFrame* linesFrame = this->linesImageList->GetTrackedFrame(this->linesImageList->GetNumberOfTrackedFrames() - 1);
  linesFrame->GetImageData()->DeepCopyFrom(this->LinesImage);
  */

  // Perform Gaussian smooth on lines image
  if(this->GaussianEnabled)
  {
    this->GaussianSmooth->SetInputData(inputImage->GetImage());                 // Perform GaussianSmooth on original fan image for maximum information content
    this->GaussianSmooth->Update();
    (inputImage)->DeepCopyFrom(this->GaussianSmooth->GetOutput());
  }

  if (this->ThresholdingEnabled)
  {
    this->Thresholder->SetInputData(inputImage->GetImage());
    this->Thresholder->Update();
    (inputImage)->DeepCopyFrom(this->Thresholder->GetOutput());
  }

  // Set final output image data
  PlusVideoFrame* outputImage = outputFrame->GetImageData();
  outputImage->DeepCopy(inputImage);
  //(outputImage)->DeepCopyFrom(this->Thresholder->GetOutput());
  
  
  
  
  //this->ProcessLinesImage();

  //PlusTrackedFrame* processedLinesFrame = this->ProcessedLinesImageList->GetTrackedFrame(this->ProcessedLinesImageList->GetNumberOfTrackedFrames() - 1);
  //processedLinesFrame->GetImageData()->DeepCopyFrom(this->ProcessedLinesImage);
  
  // Draw scan lines on the output image.
  // DrawScanLines(this->ScanConverter, inputImage->GetImage());

  // Convert the lines image back to original geometry.

  //this->ScanConverter->SetInputData( this->ProcessedLinesImage );
  //this->ScanConverter->Update();
 
  return PLUS_SUCCESS;
}

/**
 * TODO: Currently not used. If won't be used, delete.
 */
void vtkPlusTransverseProcessEnhancer::ComputeHistogram(vtkImageData* imageData)
{
  vtkSmartPointer<vtkImageAccumulate> histogram = vtkSmartPointer<vtkImageAccumulate>::New();
  histogram->SetInputData( imageData );
  histogram->SetComponentExtent(1,25,0,0,0,0);
  histogram->SetComponentOrigin(1,0,0);
  histogram->SetComponentSpacing(10,0,0);
  histogram->SetIgnoreZero(true);
  histogram->Update();
}


void vtkPlusTransverseProcessEnhancer::SetLinesImageFileName( std::string fileName )
{
  this->LinesImageFileName = fileName;
}


void vtkPlusTransverseProcessEnhancer::SetIntermediateImageFileName(std::string fileName)
{
  this->IntermediateImageFileName = fileName;
}


void vtkPlusTransverseProcessEnhancer::SetProcessedLinesImageFileName( std::string fileName )
{
  this->ProcessedLinesImageFileName = fileName;
}


void vtkPlusTransverseProcessEnhancer::SetNumberOfScanLines(int numScanLines)
{
  this->NumberOfScanLines = numScanLines;
}


void vtkPlusTransverseProcessEnhancer::SetNumberOfSamplesPerScanLine(int numSamples)
{
  this->NumberOfSamplesPerScanLine = numSamples;
}

void vtkPlusTransverseProcessEnhancer::SetGaussianEnabled(bool Enabled)
{
  this->GaussianEnabled = Enabled;
}

void vtkPlusTransverseProcessEnhancer::SetGaussianStdDev(double NewGaussianStdDev)
{
  this->GaussianStdDev = NewGaussianStdDev;
  this->GaussianSmooth->SetStandardDeviation(NewGaussianStdDev);
}

void vtkPlusTransverseProcessEnhancer::SetGaussianKernelSize(int NewGaussianKernelSize)
{
  this->GaussianKernelSize = NewGaussianKernelSize;
  this->GaussianSmooth->SetRadiusFactor(NewGaussianKernelSize);
}

void vtkPlusTransverseProcessEnhancer::SetThresholdingEnabled(bool Enabled)
{
  this->ThresholdingEnabled = Enabled;
}

void vtkPlusTransverseProcessEnhancer::SetThresholdInValue(double NewThresholdInValue)
{
  this->ThresholdInValue = NewThresholdInValue;
  this->Thresholder->SetInValue(NewThresholdInValue);
}

void vtkPlusTransverseProcessEnhancer::SetThresholdOutValue(double NewThresholdOutValue)
{
  this->ThresholdOutValue = NewThresholdOutValue;
  this->Thresholder->SetOutValue(NewThresholdOutValue);
}

void vtkPlusTransverseProcessEnhancer::SetLowerThreshold(double NewLowerThreshold)
{
  this->LowerThreshold = NewLowerThreshold;
  if (this->UpperThreshold != NULL) this->Thresholder->ThresholdBetween(LowerThreshold, this->UpperThreshold);
  else this->Thresholder->ThresholdByLower(LowerThreshold);
}

void vtkPlusTransverseProcessEnhancer::SetUpperThreshold(double NewUpperThreshold)
{
  this->UpperThreshold = NewUpperThreshold;
  if (this->LowerThreshold != NULL) this->Thresholder->ThresholdBetween(this->LowerThreshold, UpperThreshold);
  else this->Thresholder->ThresholdByUpper(UpperThreshold);
}