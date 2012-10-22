/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkRfToBrightnessConvert_h
#define __vtkRfToBrightnessConvert_h

#include "vtkThreadedImageAlgorithm.h"
#include "PlusVideoFrame.h"

/*!
\class vtkRfToBrightnessConvert
\brief This class converts ultrasound RF data to brightness values

The filter computes brightness values by envelope detection and dynamic range
compression of the RF data.

Envelope detection (estimation of the amplitude of the RF signal) is performed by computing
the Euclidean norm of the RF signal (in-phase, I) and a 90deg phase shifted version of the
RF signal (quadrature, Q). The Q signal may be provided by the acquisition system or can be
computed from the I signal by a Hilbert transform.

Dynamic range compression converts the 16-bit input signal to 8-bit by a non-linear function.
In this filter the compressedSignal=sqrt(sqrt(envelopeDetected))*BrightnessScale function is used.
A log function is also frequently used for dynamic range compression. The sqrt(sqrt(.)) function was
chosen because it provides a somewhat more linear mapping than log(.) function for the input data
range (16 bits).

The input image type must be VTK_SHORT (signed 16-bit) and the output image type
is always VTK_UNSIGNED_CHAR (unsigned 8-bit).

\ingroup RfProcessingAlgo
*/ 
class VTK_EXPORT vtkRfToBrightnessConvert : public vtkThreadedImageAlgorithm
{
public:
  static vtkRfToBrightnessConvert *New();
  vtkTypeMacro(vtkRfToBrightnessConvert,vtkThreadedImageAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent);
  
  /*! Read configuration from xml data. The rfToBrightnessElement is typically in DataCollction/ImageAcquisition/RfProcessing. */
  virtual PlusStatus ReadConfiguration(vtkXMLDataElement* rfToBrightnessElement); 

  /*! Write configuration to xml data. The rfToBrightnessElement is typically in DataCollction/ImageAcquisition/RfProcessing. */
  virtual PlusStatus WriteConfiguration(vtkXMLDataElement* rfToBrightnessElement); 

  /*! Specify image type (RF data encoding type) */
  vtkSetMacro(ImageType, US_IMAGE_TYPE);
  vtkGetMacro(ImageType, US_IMAGE_TYPE);

protected:
  vtkRfToBrightnessConvert();
  ~vtkRfToBrightnessConvert();
  
  virtual int RequestInformation(vtkInformation*,
                                 vtkInformationVector**,
                                 vtkInformationVector* outputVector);

  void ThreadedRequestData( vtkInformation *request,
                            vtkInformationVector **inputVector,
                            vtkInformationVector *outputVector,
                            vtkImageData ***inData, vtkImageData **outData,
                            int outExt[6], int id);

  /*! Compute the Hilbert transform coefficients. Used by the ComputeHilbertTransform method. */
  virtual void ComputeHilbertTransformCoeffs();

  /*! Compute the Hilbert transform (90 deg phase shift) of a signal */
  virtual PlusStatus ComputeHilbertTransform(short *hilbertTransformOutput, short *input, int npt);
  
  /*! Compute amplitude from the original and Hilbert transformed RF data. npt is the number of samples in the input signal */
  virtual void ComputeAmplitudeILineQLine(unsigned char *ampl, short *inputSignal, short *inputSignalHilbertTransformed, int npt);
  
  /*! Compute amplitude from IQ encoded RF data. npt is the number of IQ pairs * 2. */
  virtual void ComputeAmplitudeIqLine(unsigned char *ampl, short *inputSignal, const int npt);

  /*! Scaling of the brightness output. Higher value means brighter image. */
  double BrightnessScale;

  /*! Number of the Hilbert transform convolution filter coefficients. Higher number results in better approximation but longer computation time. */
  int NumberOfHilbertFilterCoeffs;

  /*! Coefficients of the Hilbert transform, computed from the NumberOfHilbertFilterCoeffs */
  std::vector<double> HilbertTransformCoeffs;

  /*! Image type (RF_IQ_LINE, RF_I_LINE_Q_LINE, ...) */
  US_IMAGE_TYPE ImageType;

private:
  vtkRfToBrightnessConvert(const vtkRfToBrightnessConvert&);  // Not implemented.
  void operator=(const vtkRfToBrightnessConvert&);  // Not implemented.
};

#endif



