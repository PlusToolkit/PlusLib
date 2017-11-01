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
#include "vtkImageAlgorithm.h"

// VTK includes
#include <vtkSmartPointer.h>
#include <vtkSetGet.h>

class vtkImageData;
class vtkImageThreshold;
class vtkImageGaussianSmooth;
class vtkImageSobel2D;
class vtkImageIslandRemoval2D;
class vtkImageDilateErode3D;
//class vtkImageCast;
//class vtkImageShiftScale;
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
  virtual PlusStatus ReadConfiguration(vtkSmartPointer<vtkXMLDataElement> processingElement);

  /*! Write configuration to xml data */
  virtual PlusStatus WriteConfiguration(vtkSmartPointer<vtkXMLDataElement> processingElement);

  /*! Get the Type attribute of the configuration element */
  virtual const char* GetProcessorTypeName() { return "vtkPlusTransverseProcessEnhancer"; };

  /*! If optional output files for intermediate images should saved */
  vtkSetMacro(IntermediateImageFileName, std::string);
  vtkSetMacro(SaveIntermediateResults, bool);


  /*! Get and Set methods for variables related to the scanner used */
  vtkSetMacro(NumberOfScanLines, int);
  vtkGetMacro(NumberOfScanLines, int);

  vtkSetMacro(NumberOfSamplesPerScanLine, int);
  vtkGetMacro(NumberOfSamplesPerScanLine, int);

  vtkSetVector3Macro(MmToPixelFanImage, double);
  vtkGetVector3Macro(MmToPixelFanImage, double);

  vtkSetVector3Macro(MmToPixelLinesImage, double);
  vtkGetVector3Macro(MmToPixelLinesImage, double);

  vtkSetMacro(RadiusStartMm, int);
  vtkGetMacro(RadiusStartMm, int);

  vtkSetMacro(RadiusStopMm, int);
  vtkGetMacro(RadiusStopMm, int);

  vtkSetMacro(ThetaStartDeg, int);
  vtkGetMacro(ThetaStartDeg, int);

  vtkSetMacro(ThetaStopDeg, int);
  vtkGetMacro(ThetaStopDeg, int);


  /*! Get and Set methods for variables related to filter peramaters */
  void SetGaussianStdDev(double GaussianStdDev);
  void SetGaussianKernelSize(double GaussianKernelSize);

  void SetIslandAreaThreshold(int islandAreaThreshold);
  vtkGetMacro(IslandAreaThreshold, int);

  vtkSetVector2Macro(ErosionKernelSize, int);
  vtkGetVector2Macro(ErosionKernelSize, int);

  vtkSetVector2Macro(DilationKernelSize, int);
  vtkGetVector2Macro(DilationKernelSize, int);

  void ThresholdViaStdDeviation(vtkSmartPointer<vtkImageData> inputImage);

  vtkImageData* GetProcessedLinesImage() { return (this->ProcessedLinesImage); }


  ///Steps to note and eliminate false boen areas
  void MarkShadowOutline(vtkSmartPointer<vtkImageData> inputImage);
  void RemoveOffCameraBones(vtkSmartPointer<vtkImageData> inputImage);
  void CompareShadowAreas(vtkSmartPointer<vtkImageData> originalImage, vtkSmartPointer<vtkImageData> inputImage);

  ///Methods related to intermediate images
  std::map<char*, vtkSmartPointer<vtkPlusTrackedFrameList> > GetIntermediateImageMap() { return (this->IntermediateImageMap); };
  PlusStatus SaveAllIntermediateResultsToFile();
  PlusStatus SaveIntermediateResultToFile(char* fileNamePostfix);

protected:
  vtkPlusTransverseProcessEnhancer();
  virtual ~vtkPlusTransverseProcessEnhancer();

  void FillLinesImage(vtkSmartPointer<vtkImageData> inputImageData);
  void VectorImageToUchar(vtkSmartPointer<vtkImageData> inputImage);

  void ImageConjunction(vtkSmartPointer<vtkImageData> inputImage, vtkSmartPointer<vtkImageData> maskImage);

  void AddIntermediateImage(char* fileNamePostfix, vtkSmartPointer<vtkImageData> image);
  void AddIntermediateFromFilter(char* fileNamePostfix, vtkImageAlgorithm* imageAlgorithm);

protected:
  vtkSmartPointer<vtkPlusUsScanConvert>     ScanConverter;
  vtkSmartPointer<vtkImageGaussianSmooth>   GaussianSmooth;           // Trying to incorporate existing GaussianSmooth vtkThreadedAlgorithm class
  vtkSmartPointer<vtkImageSobel2D>          EdgeDetector;
  vtkSmartPointer<vtkImageThreshold>        ImageBinarizer;
  vtkSmartPointer<vtkImageData>             BinaryImageForMorphology;
  vtkSmartPointer<vtkImageIslandRemoval2D>  IslandRemover;
  vtkSmartPointer<vtkImageDilateErode3D>    ImageEroder;
  vtkSmartPointer<vtkImageDilateErode3D>    ImageDialator;

  int NumberOfScanLines;
  int NumberOfSamplesPerScanLine;
  bool ReturnToFanImage;

  double MmToPixelFanImage[3];
  double MmToPixelLinesImage[3];

  // Scan Conversion parameters, defined in config file
  int RadiusStartMm;
  int RadiusStopMm;
  int ThetaStartDeg;
  int ThetaStopDeg;

  // Image processing parameters, defined in config file
  double GaussianStdDev;
  double GaussianKernelSize;

  vtkSmartPointer<vtkImageData> ConversionImage;

  int IslandAreaThreshold;

  int ErosionKernelSize[2];

  int DilationKernelSize[2];

  int BoneOutlineDepthPx;
  int BonePushBackPx;

  bool SaveIntermediateResults;

  std::string IntermediateImageFileName;

  /// Image for pixels (uchar) along scan lines only
  vtkSmartPointer<vtkImageData> LinesImage;
  /// Used to retrieve original pixel values from some point before binarization
  vtkSmartPointer<vtkImageData> UnprocessedLinesImage;
  /// Pixels (float) store probability of belonging to shadow
  vtkSmartPointer<vtkImageData> ProcessedLinesImage;

  /// Image after some of the processing operations have been applied
  std::map<char*, vtkSmartPointer<vtkPlusTrackedFrameList> > IntermediateImageMap;

  std::vector<char*> IntermediatePostfixes;

  std::vector<std::map<std::string, int>> BoneAreasInfo;


private:
  virtual PlusStatus ProcessImageExtents();

private:
  bool FirstFrame;

};

#endif