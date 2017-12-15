/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#ifndef vtkPlusForoughiBoneSurfaceProbability_h
#define vtkPlusForoughiBoneSurfaceProbability_h

/*!
\class vtkPlusForoughiBoneSurfaceProbability
\brief This class computes bone surface probability by dynamic programming

The filter computes bone surface probability (BSP) by the dynamic programming as
proposed by Foroughi et al.

Foroughi, P., et al. (2007) Ultrasound bone segmentation using dynamic programming. IEEE Ultrason Symp 13(4):2523–2526.

Implemented (with some modifications) by Mikael Brudfors, March 2014.

The *Intel MKL* code uses double data at this moment, therefore input and output must be double scalar type image.

The module uses *OpenMP* and *Intel MKL*. A free trial of *Intel MKL* can be downloaded from here:

[https://software.intel.com/en-us/intel-mkl/try-buy](https://software.intel.com/en-us/intel-mkl/try-buy)

\ingroup PlusLibImageProcessingAlgo
*/

#include "vtkPlusImageProcessingExport.h"

#include "vtkSimpleImageToImageFilter.h"
#include "vtkSmartPointer.h"

class vtkPlusImageProcessingExport vtkPlusForoughiBoneSurfaceProbability : public vtkSimpleImageToImageFilter
{
public:
  static vtkPlusForoughiBoneSurfaceProbability* New();
  vtkTypeMacro(vtkPlusForoughiBoneSurfaceProbability, vtkSimpleImageToImageFilter);

  /*! Controls the ratio between the Gaussian blurring and the Laplacian of Gaussian. */
  vtkSetMacro(BlurredVSBLoG, int);
  vtkGetMacro(BlurredVSBLoG, int);

  /*! Defines the probability threshold for a pixel to be considered bone or not. */
  vtkSetMacro(BoneThreshold, double);
  vtkGetMacro(BoneThreshold, double);

  /*! Standard deviation of the Gaussian weighting function which models the transition of high intensity pixels close to bone surface to the dark pixels deeper under the bone. */
  vtkSetMacro(ShadowSigma, double);
  vtkGetMacro(ShadowSigma, double);

  /* Controls the ratio between the shadow map and the reflection number. */
  vtkSetMacro(ShadowVSIntensity, int);
  vtkGetMacro(ShadowVSIntensity, int);

  /*! Defines the size of the Gaussian kernel used for blurring. */
  vtkSetMacro(SmoothingSigma, double);
  vtkGetMacro(SmoothingSigma, double);

  /*! Defines the number of rows to exclude from the top part of the image (close to the transducer head). */
  vtkSetMacro(TransducerMargin, int);
  vtkGetMacro(TransducerMargin, int);

protected:
  vtkPlusForoughiBoneSurfaceProbability();
  virtual ~vtkPlusForoughiBoneSurfaceProbability();

  void UpdateKernels();
  void DeleteKernels();

  void Foroughi2007(double* inputBuffer, double* outputBuffer, double smoothingSigma, int transducerMargin, double shadowSigma, double boneThreshold, int blurredVSBLoG, int shadowVSIntensity, int nx, int ny, int nz);
  void Conv2(const double* inputBuffer, const double* kernelBuffer, double* tempBuffer, double* outputBuffer, int nx, int ny, int kx, int ky);
  void ResizeMatrix(const double* inputBuffer, double* outputBuffer, int xClipping, int yClipping, int xInputSize, int yInputSize);
  double GetMaxPixelValue(const double* buffer, int size);
  void Normalize(double* buffer, int size, bool doInverse, double maxValue = 1.0);

  virtual void SimpleExecute(vtkImageData* input, vtkImageData* output);

  int BlurredVSBLoG;
  double BoneThreshold;
  double ShadowSigma;
  int ShadowVSIntensity;
  double SmoothingSigma;
  int TransducerMargin;

  bool KernelUpdateRequested;

  int GaussianKernelSize;
  FrameSizeType FrameSize;

  double* MklGaussianBuffer;
  double* MklLaplacianOfGaussianBuffer;
  double* MklGaussianBufferTemp;
  double* MklLaplacianOfGaussianBufferTemp;
  double* MklReflectionNumberBuffer;
  double* MklShadowValueBuffer;
  double* MklShadowModel;
  double* MklGaussianKernel;
  double* MklLaplacianKernel;

private:
  vtkPlusForoughiBoneSurfaceProbability(const vtkPlusForoughiBoneSurfaceProbability&);  // Not implemented.
  void operator=(const vtkPlusForoughiBoneSurfaceProbability&);  // Not implemented.
};

#endif