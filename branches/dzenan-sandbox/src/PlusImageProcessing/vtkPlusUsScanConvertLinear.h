/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkPlusUsScanConvertLinear_h
#define __vtkPlusUsScanConvertLinear_h

#include "vtkPlusImageProcessingExport.h"
#include "vtkPlusUsScanConvert.h"

class vtkAlgorithmOutput;
class vtkImageReslice;
class vtkImageData;

/*!
\class vtkPlusUsScanConvertLinear
\brief This class performs scan conversion from scan lines for curvilinear probes
\ingroup PlusLibImageProcessingAlgo
*/ 
class vtkPlusImageProcessingExport vtkPlusUsScanConvertLinear : public vtkPlusUsScanConvert
{
public:
  static vtkPlusUsScanConvertLinear *New();
  vtkTypeMacro(vtkPlusUsScanConvertLinear,vtkPlusUsScanConvert);
  virtual void PrintSelf(ostream& os, vtkIndent indent) VTK_OVERRIDE;

  virtual const char* GetTransducerGeometry() { return "LINEAR"; }
  
  /*! 
    Set the connection for the input image containing the brightness lines.
    The input image orientation must be FM.
  */
  virtual void SetInputConnection(vtkAlgorithmOutput* input);
  virtual void SetInputData(vtkDataObject *input);

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
  vtkPlusUsScanConvertLinear();
  virtual ~vtkPlusUsScanConvertLinear();

  /*! Image depth covered by an RF scanline, in mm */
  double ImagingDepthMm;
  /*! Image width covered by the transducer (distance between the first and last RF scanlines), in mm */
  double TransducerWidthMm;

  /*! Reslice class that performs the necessary resampling */
  vtkImageReslice* ImageReslice;

private:
  vtkPlusUsScanConvertLinear(const vtkPlusUsScanConvertLinear&);  // Not implemented.
  void operator=(const vtkPlusUsScanConvertLinear&);  // Not implemented.
};

#endif




