/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkUsScanConvert_h
#define __vtkUsScanConvert_h

#include "vtkThreadedImageAlgorithm.h"

/*!
\class vtkUsScanConvert
\brief This class performs scan conversion from scan lines for curvilinear probes
\ingroup RfProcessingAlgo
*/ 
class VTK_EXPORT vtkUsScanConvert : public vtkThreadedImageAlgorithm
{
public:
  static vtkUsScanConvert *New();
  vtkTypeMacro(vtkUsScanConvert,vtkThreadedImageAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent);
  
  /*! Set the output image spacing (mm/pixel) */
  vtkSetVector3Macro(OutputImageSpacing,double);
  /*! Get the output image spacing (mm/pixel) */
  vtkGetVector3Macro(OutputImageSpacing,double);
    
protected:
  vtkUsScanConvert();
  virtual ~vtkUsScanConvert();

  virtual int SplitExtent(int splitExt[6], int startExt[6], int num, int total);
  
  virtual int RequestInformation(vtkInformation *,
                                 vtkInformationVector **,
                                 vtkInformationVector *);
  
  void ThreadedRequestData(vtkInformation *request,
                           vtkInformationVector **inputVector,
                           vtkInformationVector *outputVector,
                           vtkImageData ***inData,
                           vtkImageData **outData,
                           int outExt[6],
                           int id);


  int OutputImageExtent[6];
  double OutputImageSpacing[3];

  /*! Depth for start of output image, in mm */
  double ImageStartDepthMm;
  /*! Size of output image, in mm */
  double ImageSizeMm;

  /*! Depth for start of scanline, in mm */
  double RadiusStartMm;
  /*! Sampling interval within a scanline, in mm */
  double RadiusDeltaMm;

  /*! Angle for first scanline in image, in degrees */
  double ThetaStartDeg;
  /*! Angle between scanlines in image, in degrees */
  double ThetaDeltaDeg;

  /*! Intensity scaling factor from envelope to image */
  double OutputIntensityScaling;
  
  int InterpolationTableSize;

private:
  vtkUsScanConvert(const vtkUsScanConvert&);  // Not implemented.
  void operator=(const vtkUsScanConvert&);  // Not implemented.
};

#endif




