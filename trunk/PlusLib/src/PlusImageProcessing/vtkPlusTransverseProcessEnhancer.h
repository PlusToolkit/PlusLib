#ifndef __vtkPlusTransverseProcessEnhancer_h
#define __vtkPlusTransverseProcessEnhancer_h

#include "vtkPlusImageProcessingExport.h"
#include "vtkSmartPointer.h"
#include "vtkPlusTrackedFrameProcessor.h"

//#include "vtkImageAlgorithm.h"


class vtkImageCast;
class vtkImageData;
class vtkImageThreshold;
class vtkImageGaussianSmooth;  
class vtkPlusUsScanConvert;

/*!
  \class vtkPlusTransverseProcessEnhancer 
  \brief Improves bone surface visibility in ultrasound images
  \ingroup PlusLibImageProcessingAlgo
*/ 
class vtkPlusImageProcessingExport vtkPlusTransverseProcessEnhancer : public vtkPlusTrackedFrameProcessor
{
public:
  static vtkPlusTransverseProcessEnhancer *New();
  vtkTypeMacro(vtkPlusTransverseProcessEnhancer , vtkPlusTrackedFrameProcessor);
  virtual void PrintSelf(ostream& os, vtkIndent indent); 

   /*! Update output frame from input frame */
  virtual PlusStatus ProcessFrame(PlusTrackedFrame* inputFrame, PlusTrackedFrame* outputFrame);
 
  /*! Read configuration from xml data */
  virtual PlusStatus ReadConfiguration(vtkXMLDataElement* processingElement); 

  /*! Write configuration to xml data */
  virtual PlusStatus WriteConfiguration(vtkXMLDataElement* processingElement);
  
  /*! Get the tag name of the configuration element */
  static const char* GetTagName() { return "Processor"; };        // Unnecessary -- inherited from vtkPlusTrackedFrameProcessor
  
  /*! Get the Type attribute of the configuration element */
  virtual const char* GetProcessorTypeName() { return "vtkPlusTransverseProcessEnhancer"; };

  /*! Get/Set thresholding parameter */
  virtual void SetThreshold(double threshold);
  virtual double GetThreshold();

  /*! Set optional output file name for subsampled input image sequence */
  void SetLinesImageFileName( std::string fileName );

  void SetIntermediateImageFileName(std::string fileName);

  /*! Set optional output file name for processed subsampled image sequence */
  void SetProcessedLinesImageFileName( std::string fileName );

  void SetNumberOfScanLines(int numScanLines);
  void SetNumberOfSamplesPerScanLine(int numSamples);

  void SetGaussianEnabled(bool Enabled);
  void SetGaussianStdDev(double GaussianStdDev);
  void SetGaussianKernelSize(int GaussianKernelSize);

  void SetThresholdingEnabled(bool Enabled);
  void SetThresholdInValue(double NewThresholdInValue);
  void SetThresholdOutValue(double NewThresholdOutValue);
  void SetLowerThreshold(double LowerThreshold);
  void SetUpperThreshold(double UpperThreshold);

protected:
  vtkPlusTransverseProcessEnhancer();
  virtual ~vtkPlusTransverseProcessEnhancer();

  /*! Helper function for drawing a line on a VTK image */
  void DrawLine(vtkImageData* imageData, int* imageExtent, double* start, double* end, int numberOfPoints);

  /*! Draws scanlines on the image - for testing and debugging */
  void DrawScanLines(vtkPlusUsScanConvert* scanConverter, vtkImageData* imageData);

  void FillLinesImage(vtkPlusUsScanConvert* scanConverter, vtkImageData* inputImageData);
  void ProcessLinesImage();
  //void GaussianBlur();
  void FillShadowValues();

  void ComputeHistogram( vtkImageData* imageData );

  vtkSmartPointer<vtkPlusUsScanConvert> ScanConverter;
  vtkSmartPointer<vtkImageThreshold> Thresholder;
  
  //vtkSmartPointer<vtkImageAlgorithm> Image
  vtkSmartPointer<vtkImageGaussianSmooth> GaussianSmooth;           // Trying to incorporate existing GaussianSmooth vtkThreadedAlgorithm class

  int NumberOfScanLines;
  int NumberOfSamplesPerScanLine;

  // Descriptive statistics of current image intensity.

  double currentFrameMean;
  double currentFrameStDev;
  float currentFrameMax;
  float currentFrameMin;
  
  // Image processing parameters, defined in config file
  bool GaussianEnabled;
  double GaussianStdDev;
  double GaussianKernelSize;

  bool ThresholdingEnabled;
  double ThresholdInValue;
  double ThresholdOutValue;
  double LowerThreshold;
  double UpperThreshold;
  
  vtkSmartPointer<vtkImageData> SmoothedImage;

  std::string LinesImageFileName;
  vtkSmartPointer<vtkImageData> LinesImage; // Image for pixels (uchar) along scan lines only
  vtkSmartPointer<vtkPlusTrackedFrameList> linesImageList;
  
  std::string IntermediateImageFileName; 
  vtkSmartPointer<vtkImageData> ShadowValues; // Pixels (float) store probability of belonging to shadow
  vtkSmartPointer<vtkPlusTrackedFrameList> IntermediateImageList; // For debugging and development.

  std::string ProcessedLinesImageFileName;
  vtkSmartPointer<vtkImageData> ProcessedLinesImage;
  vtkSmartPointer<vtkPlusTrackedFrameList> ProcessedLinesImageList;
}; 

#endif
