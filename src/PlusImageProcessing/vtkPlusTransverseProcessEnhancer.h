/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkPlusTransverseProcessEnhancer_h
#define __vtkPlusTransverseProcessEnhancer_h

// Local includes
#include "vtkPlusImageProcessingExport.h"
#include "vtkPlusTrackedFrameProcessor.h"

// VTK includes
#include <vtkSmartPointer.h>
#include <vtkSetGet.h>

class vtkImageData;
class vtkImageThreshold;
class vtkImageGaussianSmooth;
class vtkImageSobel2D;
class vtkImageIslandRemoval2D;
class vtkImageDilateErode3D;
class vtkPlusUsScanConvert;

/*!
  \class vtkPlusTransverseProcessEnhancer
  \brief Improves bone surface visibility in ultrasound images
  \ingroup PlusLibImageProcessingAlgo
*/
class vtkPlusImageProcessingExport vtkPlusTransverseProcessEnhancer : public vtkPlusTrackedFrameProcessor
{
public:
  static vtkPlusTransverseProcessEnhancer* New();
  vtkTypeMacro(vtkPlusTransverseProcessEnhancer, vtkPlusTrackedFrameProcessor);
  virtual void PrintSelf(ostream& os, vtkIndent indent);

  /*! Update output frame from input frame */
  virtual PlusStatus ProcessFrame(PlusTrackedFrame* inputFrame, PlusTrackedFrame* outputFrame);

  /*! Read configuration from xml data */
  virtual PlusStatus ReadConfiguration(vtkXMLDataElement* processingElement);

  /*! Write configuration to xml data */
  virtual PlusStatus WriteConfiguration(vtkXMLDataElement* processingElement);

  /*! Get the Type attribute of the configuration element */
  virtual const char* GetProcessorTypeName() { return "vtkPlusTransverseProcessEnhancer"; };

  /*! Set optional output file name for sub-sampled input image sequence */
  void SetLinesImageFileName(const std::string& fileName) { this->LinesImageFileName = fileName; };
  
  /*! Set optional output file name for transverse process acoustic shadow images */
  void SetShadowImageFileName(const std::string& fileName) { this->ShadowImageFileName = fileName; };

  /*! Set optional output file name for partially process images */
  void SetIntermediateImageFileName(const std::string& fileName) { this->IntermediateImageFileName = fileName; };

  /*! Set optional output file name for processed sub-sampled image sequence */
  void SetProcessedLinesImageFileName(const std::string& fileName) { this->ProcessedLinesImageFileName = fileName; };

  vtkPlusTrackedFrameList* LinesFrameList;
  vtkPlusTrackedFrameList* ShadowFrameList; // For debugging and development.
  vtkPlusTrackedFrameList* IntermediateFrameList; // For debugging and development
  vtkPlusTrackedFrameList* ProcessedLinesFrameList;  // For debugging and development.

  vtkGetObjectMacro(LinesFrameList, vtkPlusTrackedFrameList);
  vtkGetObjectMacro(ShadowFrameList, vtkPlusTrackedFrameList);
  vtkGetObjectMacro(IntermediateFrameList, vtkPlusTrackedFrameList);
  vtkGetObjectMacro(ProcessedLinesFrameList, vtkPlusTrackedFrameList);

protected:

  vtkSmartPointer<vtkPlusUsScanConvert>     ScanConverter;
  vtkSmartPointer<vtkImageThreshold>        Thresholder;
  vtkSmartPointer<vtkImageGaussianSmooth>   GaussianSmooth;           // Trying to incorporate existing GaussianSmooth vtkThreadedAlgorithm class
  vtkSmartPointer<vtkImageSobel2D>          EdgeDetector;
  vtkSmartPointer<vtkImageThreshold>        ImageBinarizer;
  vtkSmartPointer<vtkImageData>             BinaryImageForMorphology;
  vtkSmartPointer<vtkImageIslandRemoval2D>  IslandRemover;
  vtkSmartPointer<vtkImageDilateErode3D>    ImageEroder;

  vtkSetMacro(NumberOfScanLines, int);
  vtkGetMacro(NumberOfScanLines, int);

  vtkSetMacro(NumberOfSamplesPerScanLine, int);
  vtkGetMacro(NumberOfSamplesPerScanLine, int);

  vtkSetMacro(ConvertToLinesImageEnabled, bool);
  vtkGetMacro(ConvertToLinesImageEnabled, bool);
  vtkBooleanMacro(ConvertToLinesImageEnabled, bool);
  void ConvertToLinesImage(PlusVideoFrame* InputFrame);

  vtkSetMacro(GaussianEnabled, bool);
  vtkGetMacro(GaussianEnabled, bool);
  vtkBooleanMacro(GaussianEnabled, bool);
  void Gaussian(vtkImageData* InputImage);

  void SetGaussianStdDev(double GaussianStdDev) { this->GaussianSmooth->SetStandardDeviation(GaussianStdDev); };
  void SetGaussianKernelSize(int GaussianKernelSize) { this->GaussianSmooth->SetRadiusFactor(GaussianKernelSize); };

  vtkSetMacro(ThresholdingEnabled, bool);
  vtkGetMacro(ThresholdingEnabled, bool);
  vtkBooleanMacro(ThresholdingEnabled, bool);
  void Threshold(vtkImageData* InputImage);

  void SetThresholdInValue(double NewThresholdInValue) { this->Thresholder->SetInValue(NewThresholdInValue); };
  void SetThresholdOutValue(double NewThresholdOutValue) { this->Thresholder->SetOutValue(NewThresholdOutValue); };
  
  void SetLowerThreshold(double LowerThreshold);
  void SetUpperThreshold(double UpperThreshold);

  vtkSetMacro(EdgeDetectorEnabled, bool);
  vtkGetMacro(EdgeDetectorEnabled, bool);
  vtkBooleanMacro(EdgeDetectorEnabled, bool);
  void DetectEdges(vtkImageData* InputImage);

  void SetIslandAreaThreshold(int islandAreaThreshold);
  vtkSetMacro(IslandRemovalEnabled, bool);
  vtkGetMacro(IslandRemovalEnabled, bool);
  vtkBooleanMacro(IslandRemovalEnabled, bool);
  void RemoveIslands(vtkImageData* InputImage);

  vtkSetMacro(ErosionEnabled, bool);
  vtkGetMacro(ErosionEnabled, bool);
  vtkBooleanMacro(ErosionEnabled, bool);
  void Erode(vtkImageData* InputImage);

  vtkSetVector2Macro(ErosionKernelSize, int);
  vtkGetVector2Macro(ErosionKernelSize, int);

  vtkSetMacro(DilationEnabled, bool);
  vtkGetMacro(DilationEnabled, bool);
  vtkBooleanMacro(DilationEnabled, bool);
  void Dilate(vtkImageData* InputImage);

  vtkSetVector2Macro(DilationKernelSize, int);
  vtkGetVector2Macro(DilationKernelSize, int);

  vtkSetMacro(ReconvertBinaryToGreyscale, bool);
  vtkGetMacro(ReconvertBinaryToGreyscale, bool);
  vtkBooleanMacro(ReconvertBinaryToGreyscale, bool);

  vtkSetMacro(ReturnToFanImage, bool);
  vtkGetMacro(ReturnToFanImage, bool);
  vtkBooleanMacro(ReturnToFanImage, bool);

  vtkPlusTransverseProcessEnhancer();
  virtual ~vtkPlusTransverseProcessEnhancer();

  void FillLinesImage(vtkPlusUsScanConvert* scanConverter, vtkImageData* inputImageData);
  void ProcessLinesImage();
  void VectorImageToUchar(vtkImageData* inputImage, vtkImageData* ConversionImage);
  void FillShadowValues(vtkImageData* InputImage);

  void ComputeHistogram(vtkImageData* imageData);

  void ImageConjunction(vtkImageData* InputImage, vtkImageData* MaskImage);

  // Identifies which pixels in InputImage belong to transverse process surface and sets them to high in this->TrpImage
  void IdentifyTrpPixels(vtkImageData* InputImage);

//protected

  bool ConvertToLinesImageEnabled;
  int NumberOfScanLines;
  int NumberOfSamplesPerScanLine;
  bool ReturnToFanImage;

  // Descriptive statistics of current image intensity.
  double CurrentFrameMean;
  double CurrentFrameStDev;
  float CurrentFrameMax;
  float CurrentFrameMin;

  // Image processing parameters, defined in config file
  bool GaussianEnabled;
  double GaussianStdDev;
  int GaussianKernelSize;

  bool ThresholdingEnabled;
  double ThresholdInValue;
  double ThresholdOutValue;
  double LowerThreshold;
  double UpperThreshold;

  bool EdgeDetectorEnabled;
  vtkSmartPointer<vtkImageData> ConversionImage;

  bool IslandRemovalEnabled;
  int IslandAreaThreshold;

  bool ErosionEnabled;
  int ErosionKernelSize[2];

  bool DilationEnabled;
  int DilationKernelSize[2];

  bool ReconvertBinaryToGreyscale;

  bool SegmentShadowsEnabled;

  std::string LinesImageFileName;
  vtkSmartPointer<vtkImageData> LinesImage; // Image for pixels (uchar) along scan lines only
  
  vtkSmartPointer<vtkImageData> UnprocessedLinesImage;  // Used to retrieve original pixel values from some point before binarization

  std::string ShadowImageFileName;
  vtkSmartPointer<vtkImageData> ShadowImage; // Pixels (float) store probability of belonging to shadow
  
  std::string IntermediateImageFileName;
  vtkSmartPointer<vtkImageData> IntermediateImage; // Image after some of the processing operations have been applied

  std::string ProcessedLinesImageFileName;
  vtkSmartPointer<vtkImageData> ProcessedLinesImage;  // Image after all of the processing operations have been applied

  std::string TrpImage;       // The only lit pixels of this image are recognized as beloning to transverse process
  vtkSmartPointer<vtkImageData> TrpImage;

};

#endif