/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkUsScanConvertCurvilinear_h
#define __vtkUsScanConvertCurvilinear_h

#include "vtkThreadedImageAlgorithm.h"

/*!
\class vtkUsScanConvertCurvilinear
\brief This class performs scan conversion from scan lines for curvilinear probes
\ingroup RfProcessingAlgo
*/ 
class VTK_EXPORT vtkUsScanConvertCurvilinear : public vtkThreadedImageAlgorithm
{
public:
  static vtkUsScanConvertCurvilinear *New();
  vtkTypeMacro(vtkUsScanConvertCurvilinear,vtkThreadedImageAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent);
  
  /*! Set the output image spacing (mm/pixel) */
  vtkSetVector3Macro(OutputImageSpacing,double);
  /*! Get the output image spacing (mm/pixel) */
  vtkGetVector3Macro(OutputImageSpacing,double);

  /*! Read configuration from xml data. The scanConversionElement is typically in DataCollction/ImageAcquisition/RfProcessing. */
  virtual PlusStatus ReadConfiguration(vtkXMLDataElement* scanConversionElement);   

  /*! Write configuration to xml data. The scanConversionElement is typically in DataCollction/ImageAcquisition/RfProcessing. */
  virtual PlusStatus WriteConfiguration(vtkXMLDataElement* scanConversionElement);   
  
  struct InterpolatedPoint
  {
    /*! Weighting coefficients that used to construct the output pixel from 4 input pixels */
    double weightCoefficients[4];
    /*! Position of the first input pixel that is used to construct the output point (in the sample line matrix). The 3 others are one row/column away. */
    int inputPixelIndex;
    // Position of the output pixel (in the image matrix) */
    int outputPixelIndex;
  };

  /*! Retrieve the InterpolatedPointArray (used internally by the thread function) */
  const std::vector<InterpolatedPoint> & GetInterpolatedPointArray() { return this->InterpolatedPointArray; };

protected:
  vtkUsScanConvertCurvilinear();
  virtual ~vtkUsScanConvertCurvilinear();

  virtual int SplitExtent(int splitExt[6], int startExt[6], int num, int total);
  
  virtual int RequestInformation(vtkInformation *, vtkInformationVector **, vtkInformationVector *);

  virtual int RequestUpdateExtent (vtkInformation*, vtkInformationVector**, vtkInformationVector*);
  
  virtual void AllocateOutputData(vtkImageData *output, int *uExtent);

  virtual void ThreadedRequestData(vtkInformation *request,
                           vtkInformationVector **inputVector,
                           vtkInformationVector *outputVector,
                           vtkImageData ***inData,
                           vtkImageData **outData,
                           int outExt[6],
                           int id);

  /*! Extent of the output image, in pixels. Only the first four values are used. */
  int OutputImageExtent[6];

  /*! Spacing of the output image, in mm/pixel. Only the first two values are used. */
  double OutputImageSpacing[3];

  /*! Depth for start of output image, in mm */
  double OutputImageStartDepthMm;

  /*! Depth for start of scanline, in mm */
  double RadiusStartMm;
  /*! Depth for end of scanline, in mm */
  double RadiusStopMm;

  /*! Angle for first scanline in image, in degrees */
  double ThetaStartDeg;
  /*! Angle for last scanline in image, in degrees */
  double ThetaStopDeg;

  /*! Intensity scaling factor from envelope to image */
  double OutputIntensityScaling;

  /*! Each element of this array defines the computation of a pixel in the output (scan converted) image.  */
  std::vector<InterpolatedPoint> InterpolatedPointArray;

  int InterpInputImageExtent[6];
  double InterpRadiusStartMm;
  double InterpRadiusStopMm;
  double InterpThetaStartDeg;
  double InterpThetaStopDeg;
  int InterpOutputImageExtent[6];
  double InterpOutputImageSpacing[3];
  double InterpOutputImageStartDepthMm;
  double InterpIntensityScaling;

  /*! 
    Computes the InterpolatedPointArray from the method arguments. The array is not recomputed if
    the input arguments are the same as last time.
  */  
  void ComputeInterpolatedPointArray(
    int *inputImageExtent, double radiusStartMm, double radiusStopMm, double thetaStartDeg, double thetaStopDeg,
    int *outputImageExtent, double *outputImageSpacing, double outputImageStartDepthMm, double intensityScaling
  );

private:
  vtkUsScanConvertCurvilinear(const vtkUsScanConvertCurvilinear&);  // Not implemented.
  void operator=(const vtkUsScanConvertCurvilinear&);  // Not implemented.
};

#endif




