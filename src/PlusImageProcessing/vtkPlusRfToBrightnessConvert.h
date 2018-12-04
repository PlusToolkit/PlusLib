/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkPlusRfToBrightnessConvert_h
#define __vtkPlusRfToBrightnessConvert_h

#include "vtkPlusImageProcessingExport.h"
#include "vtkThreadedImageAlgorithm.h"
//#include "igsioVideoFrame.h" // for US_IMAGE_TYPE

/*!
\class vtkPlusRfToBrightnessConvert
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

\ingroup PlusLibImageProcessingAlgo
*/ 
class vtkPlusImageProcessingExport vtkPlusRfToBrightnessConvert : public vtkThreadedImageAlgorithm
{
public:
  static vtkPlusRfToBrightnessConvert *New();
  vtkTypeMacro(vtkPlusRfToBrightnessConvert,vtkThreadedImageAlgorithm);
  virtual void PrintSelf(ostream& os, vtkIndent indent) VTK_OVERRIDE;
  
  /*! Read configuration from xml data. The rfToBrightnessElement is typically in DataCollction/ImageAcquisition/RfProcessing. */
  virtual PlusStatus ReadConfiguration(vtkXMLDataElement* rfToBrightnessElement); 

  /*! Write configuration to xml data. The rfToBrightnessElement is typically in DataCollction/ImageAcquisition/RfProcessing. */
  virtual PlusStatus WriteConfiguration(vtkXMLDataElement* rfToBrightnessElement); 

  /*! Specify image type (RF data encoding type) */
  vtkSetMacro(ImageType, US_IMAGE_TYPE);
  vtkGetMacro(ImageType, US_IMAGE_TYPE);

  vtkSetMacro(NumberOfHilbertFilterCoeffs, int);
  vtkGetMacro(NumberOfHilbertFilterCoeffs, int);

  vtkSetMacro(BrightnessScale, double);
  vtkGetMacro(BrightnessScale, double);

protected:
  vtkPlusRfToBrightnessConvert();
  ~vtkPlusRfToBrightnessConvert();
  
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

  /*! Essentialy, a templated version of ThreadedRequestData */
  template<typename ScalarType>
  void ThreadedLineByLineHilbertTransform(int inExt[6], int outExt[6], vtkImageData ***inData, vtkImageData **outData, int threadId);

  /*! Compute the Hilbert transform (90 deg phase shift) of a signal */
  template<typename ScalarType>
  PlusStatus ComputeHilbertTransform(ScalarType *hilbertTransformOutput, ScalarType *input, int npt);
  
  /*! Compute amplitude from the original and Hilbert transformed RF data. npt is the number of samples in the input signal */
  template<typename ScalarType>
  void ComputeAmplitudeILineQLine(unsigned char *ampl, ScalarType *inputSignal, ScalarType *inputSignalHilbertTransformed, int npt);
  
  /*! Compute amplitude from IQ encoded RF data. npt is the number of IQ pairs * 2. */
  template<typename ScalarType>
  void ComputeAmplitudeIqLine(unsigned char *ampl, ScalarType *inputSignal, const int npt);

  /*! Scaling of the brightness output. Higher value means brighter image. */
  double BrightnessScale;

  /*! Number of the Hilbert transform convolution filter coefficients. Higher number results in better approximation but longer computation time. */
  int NumberOfHilbertFilterCoeffs;

  /*! Coefficients of the Hilbert transform, computed from the NumberOfHilbertFilterCoeffs */
  std::vector<double> HilbertTransformCoeffs;

  /*! Image type (RF_IQ_LINE, RF_I_LINE_Q_LINE, ...) */
  US_IMAGE_TYPE ImageType;

private:
  vtkPlusRfToBrightnessConvert(const vtkPlusRfToBrightnessConvert&);  // Not implemented.
  void operator=(const vtkPlusRfToBrightnessConvert&);  // Not implemented.
};

#endif



