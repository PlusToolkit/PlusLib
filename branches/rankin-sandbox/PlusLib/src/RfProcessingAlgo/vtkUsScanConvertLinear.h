/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkUsScanConvertLinear_h
#define __vtkUsScanConvertLinear_h

#include "vtkUsScanConvert.h"

class vtkAlgorithmOutput;
class vtkImageReslice;
class vtkImageData;

/*!
\class vtkUsScanConvertLinear
\brief This class performs scan conversion from scan lines for curvilinear probes
\ingroup PlusLibRfProcessingAlgo
*/ 
class VTK_EXPORT vtkUsScanConvertLinear : public vtkUsScanConvert
{
public:
  static vtkUsScanConvertLinear *New();
  vtkTypeMacro(vtkUsScanConvertLinear,vtkUsScanConvert);
  void PrintSelf(ostream& os, vtkIndent indent);

  virtual const char* GetTransducerGeometry() { return "LINEAR"; }
  
  /*! 
    Set the connection for the input image containing the brightness lines.
    The input image orientation must be FM.
  */
  virtual void SetInputConnection(vtkAlgorithmOutput* input);

  /*! Bring this algorithm's outputs up-to-date. */
  virtual void Update();

  /*! Get the scan-converted output image. The output image orientation is MF. */
  virtual vtkImageData* GetOutput();

  /*! Read configuration from xml data. The scanConversionElement is typically in DataCollction/ImageAcquisition/RfProcessing. */
  virtual PlusStatus ReadConfiguration(vtkXMLDataElement* scanConversionElement);   

  /*! Write configuration to xml data. The scanConversionElement is typically in DataCollction/ImageAcquisition/RfProcessing. */
  virtual PlusStatus WriteConfiguration(vtkXMLDataElement* scanConversionElement);   

  vtkSetMacro(ImagingDepthMm,double);
  vtkGetMacro(ImagingDepthMm,double);
  vtkSetMacro(TransducerWidthMm,double);

  /*! 
    Get the start and end point of the selected scanline
    transducer surface, the end point is far from the transducer surface.
    \param scanLineIndex Index of the scanline. Starts with 0 (the scanline closest to the marked side of the transducer)
    \param scanlineStartPoint_OutputImage Starting point of the scanline (near the transducer surface), in output image coordinate frame (in pixels)
    \param scanlineEndPoint_OutputImage Last point of the scanline (far from the transducer surface), in output image coordinate frame (in pixels)
  */
  virtual PlusStatus GetScanLineEndPoints(int scanLineIndex, double scanlineStartPoint_OutputImage[4],double scanlineEndPoint_OutputImage[4]);
   
  /*! Get the distance between two sample points in the scanline, in mm */
  virtual double GetDistanceBetweenScanlineSamplePointsMm();

protected:
  vtkUsScanConvertLinear();
  virtual ~vtkUsScanConvertLinear();

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




