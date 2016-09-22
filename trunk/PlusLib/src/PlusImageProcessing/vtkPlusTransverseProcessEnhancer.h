/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkPlusTransverseProcessEnhancer_h
#define __vtkPlusTransverseProcessEnhancer_h

#include "vtkPlusImageProcessingExport.h"

// Local includes
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
  vtkTypeMacro( vtkPlusTransverseProcessEnhancer, vtkPlusTrackedFrameProcessor );
  virtual void PrintSelf( ostream& os, vtkIndent indent );

  /*! Update output frame from input frame */
  virtual PlusStatus ProcessFrame( PlusTrackedFrame* inputFrame, PlusTrackedFrame* outputFrame );

  /*! Read configuration from xml data */
  virtual PlusStatus ReadConfiguration( vtkXMLDataElement* processingElement );

  /*! Write configuration to xml data */
  virtual PlusStatus WriteConfiguration( vtkXMLDataElement* processingElement );

  /*! Get the Type attribute of the configuration element */
  virtual const char* GetProcessorTypeName() { return "vtkPlusTransverseProcessEnhancer"; };

  /*! Set optional output file name for sub-sampled input image sequence */
  void SetLinesImageFileName( const std::string& fileName );

  void SetIntermediateImageFileName( const std::string& fileName );

  /*! Set optional output file name for processed sub-sampled image sequence */
  void SetProcessedLinesImageFileName( const std::string& fileName );

  vtkSetMacro( ConvertToLinesImage, bool );
  vtkGetMacro( ConvertToLinesImage, bool );
  vtkBooleanMacro( ConvertToLinesImage, bool );

  vtkSetMacro( NumberOfScanLines, int );
  vtkGetMacro( NumberOfScanLines, int );

  vtkSetMacro( NumberOfSamplesPerScanLine, int );
  vtkGetMacro( NumberOfSamplesPerScanLine, int );

  vtkSetMacro( GaussianEnabled, bool );
  vtkGetMacro( GaussianEnabled, bool );
  vtkBooleanMacro( GaussianEnabled, bool );

  void SetGaussianStdDev( double GaussianStdDev );
  void SetGaussianKernelSize( int GaussianKernelSize );

  vtkSetMacro( ThresholdingEnabled, bool );
  vtkGetMacro( ThresholdingEnabled, bool );
  vtkBooleanMacro( ThresholdingEnabled, bool );

  void SetThresholdInValue( double NewThresholdInValue );
  void SetThresholdOutValue( double NewThresholdOutValue );
  void SetLowerThreshold( double LowerThreshold );
  void SetUpperThreshold( double UpperThreshold );

  vtkSetMacro( EdgeDetectorEnabled, bool );
  vtkGetMacro( EdgeDetectorEnabled, bool );
  vtkBooleanMacro( EdgeDetectorEnabled, bool );

  vtkSetMacro( IslandRemovalEnabled, bool );
  vtkGetMacro( IslandRemovalEnabled, bool );
  vtkBooleanMacro( IslandRemovalEnabled, bool );

  void SetIslandAreaThreshold(int islandAreaThreshold);

  vtkSetMacro(ErosionEnabled, bool);
  vtkGetMacro(ErosionEnabled, bool);
  vtkBooleanMacro(ErosionEnabled, bool);

  void SetErosionKernelSize(int KernelSize[2]);

  vtkSetMacro(DilationEnabled, bool);
  vtkGetMacro(DilationEnabled, bool);
  vtkBooleanMacro(DilationEnabled, bool);

  void SetDilationKernelSize(int KernelSize[2]);

  vtkSetMacro(ReconvertBinaryToGreyscale, bool);
  vtkGetMacro(ReconvertBinaryToGreyscale, bool);
  vtkBooleanMacro(ReconvertBinaryToGreyscale, bool);

  vtkSetMacro( ReturnToFanImage, bool );
  vtkGetMacro( ReturnToFanImage, bool );
  vtkBooleanMacro( ReturnToFanImage, bool );


protected:
  vtkPlusTransverseProcessEnhancer();
  virtual ~vtkPlusTransverseProcessEnhancer();

  /*! Helper function for drawing a line on a VTK image */
  void DrawLine( vtkImageData* imageData, int* imageExtent, double* start, double* end, int numberOfPoints );

  /*! Draws scanlines on the image - for testing and debugging */
  void DrawScanLines( vtkPlusUsScanConvert* scanConverter, vtkImageData* imageData );

  void FillLinesImage( vtkPlusUsScanConvert* scanConverter, vtkImageData* inputImageData );
  void ProcessLinesImage();
  void VectorImageToUchar( vtkImageData* inputImage, vtkImageData* ConversionImage );
  void FillShadowValues();

  void ComputeHistogram( vtkImageData* imageData );

  vtkSmartPointer<vtkPlusUsScanConvert> ScanConverter;
  vtkSmartPointer<vtkImageThreshold> Thresholder;

  //vtkSmartPointer<vtkImageAlgorithm> Image
  vtkSmartPointer<vtkImageGaussianSmooth> GaussianSmooth;           // Trying to incorporate existing GaussianSmooth vtkThreadedAlgorithm class

  vtkSmartPointer<vtkImageSobel2D> EdgeDetector;

  vtkSmartPointer<vtkImageThreshold> ImageBinarizer;
  vtkSmartPointer<vtkImageData> BinaryImageForMorphology;
  vtkSmartPointer<vtkImageIslandRemoval2D> IslandRemover;
  void ImageConjunction(vtkImageData * InputImage, vtkImageData * MaskImage);

  vtkSmartPointer<vtkImageDilateErode3D> ImageEroder;
  //vtkSmartPointer<vtkImageDilateErode3D> ImageDilater;

  //vtkSmartPointer<vtkImageCast> DoubleToUchar;
  //vtkSmartPointer<vtkImageShiftScale> ImageDataConverter;

  bool ConvertToLinesImage;
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
  //vtkSmartPointer<vtkImageData> SmoothedImage;
  vtkSmartPointer<vtkImageData> ConversionImage;

  bool IslandRemovalEnabled;
  int IslandAreaThreshold;

  bool ErosionEnabled;
  int ErosionKernelSize[2];

  bool DilationEnabled;
  int DilationKernelSize[2];

  bool ReconvertBinaryToGreyscale;

  std::string LinesImageFileName;
  vtkSmartPointer<vtkImageData> LinesImage; // Image for pixels (uchar) along scan lines only
  vtkSmartPointer<vtkPlusTrackedFrameList> LinesImageList;

  vtkSmartPointer<vtkImageData> UnprocessedLinesImage;  // Used to retrieve original pixel values from some point before binarization

  std::string IntermediateImageFileName;
  vtkSmartPointer<vtkImageData> ShadowValues; // Pixels (float) store probability of belonging to shadow
  vtkSmartPointer<vtkPlusTrackedFrameList> IntermediateImageList; // For debugging and development.

  std::string ProcessedLinesImageFileName;
  vtkSmartPointer<vtkImageData> ProcessedLinesImage;
  vtkSmartPointer<vtkPlusTrackedFrameList> ProcessedLinesImageList;
};

#endif
