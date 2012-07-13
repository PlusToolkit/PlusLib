/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkUsScanConvertLinear_h
#define __vtkUsScanConvertLinear_h

class vtkAlgorithmOutput;
class vtkImageReslice;
class vtkImageData;

/*!
\class vtkUsScanConvertLinear
\brief This class performs scan conversion from scan lines for curvilinear probes
\ingroup RfProcessingAlgo
*/ 
class VTK_EXPORT vtkUsScanConvertLinear : public vtkObject
{
public:
  static vtkUsScanConvertLinear *New();
  vtkTypeMacro(vtkUsScanConvertLinear,vtkObject);
  void PrintSelf(ostream& os, vtkIndent indent);
  
  /*! 
    Set the connection for the input image containing the brightness lines.
    The input image orientation must be FM.
  */
  virtual void SetInputConnection(vtkAlgorithmOutput* input);

  /*! Bring this algorithm's outputs up-to-date. */
  virtual void Update();

  /*! Get the scan-converted output image. The output image orientation is MF. */
  virtual vtkImageData* GetOutput();

  /*! Read configuration from xml data */
  virtual PlusStatus ReadConfiguration(vtkXMLDataElement* config);   
   
protected:
  vtkUsScanConvertLinear();
  virtual ~vtkUsScanConvertLinear();

  /*! Extent of the output image, in pixels. Only the first four values are used. */
  int OutputImageExtent[6];

  /*! Spacing of the output image, in mm/pixel. Only the first two values are used. */
  double OutputImageSpacing[3];

  /*! Depth for start of output image, in mm */
  double OutputImageStartDepthMm;

  /*! Image depth covered by an RF scanline, in mm */
  double ImagingDepthMm;
  /*! Image width covered by the transducer (distance between the first and last RF scanlines), in mm */
  double TransducerWidthMm;

  /*! Reslice class that performs the necessary resampling */
  vtkImageReslice* ImageReslice;

private:
  vtkUsScanConvertLinear(const vtkUsScanConvertLinear&);  // Not implemented.
  void operator=(const vtkUsScanConvertLinear&);  // Not implemented.
};

#endif




