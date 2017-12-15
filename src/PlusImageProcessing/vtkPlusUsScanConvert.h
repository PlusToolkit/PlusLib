/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkPlusUsScanConvert_h
#define __vtkPlusUsScanConvert_h

#include "vtkPlusImageProcessingExport.h"
#include "vtkThreadedImageAlgorithm.h"

/*!
\class vtkPlusUsScanConvert
\brief This is a base class for defining a common scan conversion algorithm interface for all kinds of probes
\ingroup PlusLibImageProcessingAlgo
*/
class vtkPlusImageProcessingExport vtkPlusUsScanConvert : public vtkThreadedImageAlgorithm
{
public:
  vtkTypeMacro(vtkPlusUsScanConvert, vtkThreadedImageAlgorithm);
  virtual void PrintSelf(ostream& os, vtkIndent indent) VTK_OVERRIDE;

  virtual const char* GetTransducerGeometry() = 0;

  /*! Set the transducer model name (e.g., BK_8848-axial) */
  vtkSetStringMacro(TransducerName);
  /*! Get the transducer model name (e.g., BK_8848-axial) */
  vtkGetStringMacro(TransducerName);

  /*! Set the output image spacing (mm/pixel) */
  vtkSetVector3Macro(OutputImageSpacing, double);
  /*! Get the output image spacing (mm/pixel) */
  vtkGetVector3Macro(OutputImageSpacing, double);

  /*! Read configuration from xml data. The scanConversionElement is typically in DataCollction/ImageAcquisition/RfProcessing. */
  virtual PlusStatus ReadConfiguration(vtkXMLDataElement* scanConversionElement);

  /*! Write configuration to xml data. The scanConversionElement is typically in DataCollction/ImageAcquisition/RfProcessing. */
  virtual PlusStatus WriteConfiguration(vtkXMLDataElement* scanConversionElement);

  vtkGetVector6Macro(OutputImageExtent, int);
  vtkGetVector6Macro(InputImageExtent, int);
  vtkSetVector6Macro(InputImageExtent, int);

  /*! Get the output image size (in pixel) */
  virtual FrameSizeType GetOutputImageSizePixel();

  /*!
    Get the scan converted image. Need to set the inputs and call Update() before calling this method.
    It is overridden here, because the GetOutput() method in vtkImageAlgorithm is not virtual.
  */
  virtual vtkImageData* GetOutput() = 0;

  /*! It is overridden here, because the GetOutput() method in vtkImageAlgorithm is not virtual. */
  virtual void SetInputData(vtkDataObject* input) { vtkThreadedImageAlgorithm::SetInputData(input); };

  /*!
    Get the start and end point of the selected scanline. Setting of the input image or at least the input image extent is required before calling this method.
    \param scanLineIndex Index of the scanline. Starts with 0 (the scanline closest to the marked side of the transducer)
    \param scanlineStartPoint_OutputImage Starting point of the scanline (near the transducer surface), in output image coordinate frame (in pixels)
    \param scanlineEndPoint_OutputImage Last point of the scanline (far from the transducer surface), in output image coordinate frame (in pixels)
  */
  virtual PlusStatus GetScanLineEndPoints(int scanLineIndex, double scanlineStartPoint_OutputImage[4], double scanlineEndPoint_OutputImage[4]) = 0;

  /*! Get the distance between two sample points in the scanline, in mm. Setting of the input image or at least the input image extent is required before calling this method. */
  virtual double GetDistanceBetweenScanlineSamplePointsMm() = 0;

protected:
  vtkPlusUsScanConvert();
  virtual ~vtkPlusUsScanConvert();

  /*! Transducer model name */
  char* TransducerName;

  /*! Extent of the output image, in pixels. Only the first four values are used. */
  int OutputImageExtent[6];

  /*!
    Position of the transducer's middle element in the MF coordinate system (in pixels).
    Usually the first component is the half of the output image size and the second component is close to 0.
  */
  double TransducerCenterPixel[2];

  /*!
    True if the TransducerCenterPixel is specified.
    If not specified then the default values are used (defaults are specified in subclasses).
  */
  bool TransducerCenterPixelSpecified;

  /*! Spacing of the output image, in mm/pixel. Only the first two values are used. */
  double OutputImageSpacing[3];

  /*!
    Extent of the input non-scanconverted image, in pixels. Only the first four values are used.
    The values are updated when Update() is called.
    InputImageExtent[1]-InputImageExtent[0]+1 is the scanline length (number of samples in a scanline)
    InputImageExtent[3]-InputImageExtent[2]+1 is the number of scanlines
  */
  int InputImageExtent[6];

private:
  vtkPlusUsScanConvert(const vtkPlusUsScanConvert&);  // Not implemented.
  void operator=(const vtkPlusUsScanConvert&);  // Not implemented.
};

#endif
