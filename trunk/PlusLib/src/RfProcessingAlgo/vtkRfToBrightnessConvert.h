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

  virtual void PrepareHilbertTransform();

  /*! Compute the Hilbert transform (90 deg phase shift) of a signal */
  virtual PlusStatus ComputeHilbertTransform(short *hilbertTransformOutput, short *input, int npt);
  
  /*! Compute amplitude from the original and Hilbert transformed RF data. npt is the number of samples in the input signal */
  virtual void ComputeAmplitudeILineQLine(unsigned char *ampl, short *inputSignal, short *inputSignalHilbertTransformed, int npt);
  
  /*! Compute amplitude from IQ encoded RF data. npt is the number of IQ pairs * 2. */
  virtual void ComputeAmplitudeIqLine(unsigned char *ampl, short *inputSignal, const int npt);

  /*! Scaling of the brightness output. Higher value means brighter image. */
  double BrightnessScale;

  /*! Number of the Hilbert transform convolution filter coefficients. Higher number results in better approximation but longer computation time. */
  int NumberOfHilberFilterCoeffs;

  /*! Coefficients of the Hilbert transform, computed from the NumberOfHilberFilterCoeffs */
  std::vector<double> HilbertTransformCoeffs;

  /*! Image type (RF_IQ_LINE, RF_I_LINE_Q_LINE, ...) */
  US_IMAGE_TYPE ImageType;

private:
  vtkRfToBrightnessConvert(const vtkRfToBrightnessConvert&);  // Not implemented.
  void operator=(const vtkRfToBrightnessConvert&);  // Not implemented.
};

#endif



