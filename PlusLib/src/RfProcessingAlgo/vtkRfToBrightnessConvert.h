/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkRfToBrightnessConvert_h
#define __vtkRfToBrightnessConvert_h

#include "vtkThreadedImageAlgorithm.h"

/*!
\class vtkRfToBrightnessConvert
\brief This class converts ultrasound RF data to brightness values
\ingroup RfProcessingAlgo
*/ 
class VTK_EXPORT vtkRfToBrightnessConvert : public vtkThreadedImageAlgorithm
{
public:
  static vtkRfToBrightnessConvert *New();
  vtkTypeMacro(vtkRfToBrightnessConvert,vtkThreadedImageAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent);
  
  /*! Read configuration from xml data */
  virtual PlusStatus ReadConfiguration(vtkXMLDataElement* config); 
  
protected:
  vtkRfToBrightnessConvert();
  ~vtkRfToBrightnessConvert() {};
  
  void ThreadedRequestData( vtkInformation *request,
                            vtkInformationVector **inputVector,
                            vtkInformationVector *outputVector,
                            vtkImageData ***inData, vtkImageData **outData,
                            int outExt[6], int id);

  virtual void PrepareHilbertTransform();
  virtual PlusStatus ComputeHilbertTransform(short *hilbertTransformOutput, short *input, int npt);
  virtual void ComputeAmplitude(short *ampl, short *inputSignal, short *inputSignalHilbertTransformed, int npt);

  /*! Scaling of the brightness output. Higher value means brighter image. */
  double BrightnessScale;

  /*! Number of the Hilbert transform convolution filter coefficients. Higher number results in better approximation but longer computation time. */
  int NumberOfHilberFilterCoeffs;

  /*! Coefficients of the Hilbert transform, computed from the NumberOfHilberFilterCoeffs */
  std::vector<double> HilbertTransformCoeffs;

private:
  vtkRfToBrightnessConvert(const vtkRfToBrightnessConvert&);  // Not implemented.
  void operator=(const vtkRfToBrightnessConvert&);  // Not implemented.
};

#endif



