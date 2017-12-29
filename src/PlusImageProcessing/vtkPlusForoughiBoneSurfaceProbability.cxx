/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"

#include "vtkPlusForoughiBoneSurfaceProbability.h"

// VTK includes
#include "vtkImageCast.h"
#include <vtkIntArray.h>
#include <vtkNew.h>
#include <vtkObjectFactory.h>
#include <vtkImageData.h>
#include <vtkDoubleArray.h>
#include <vtkTimerLog.h>

// STD includes
#include <cassert>

// Other includes
#include "mkl.h"
#ifdef NDEBUG
  #include <omp.h>
#endif

#define MKL_FREE_IF_NULL(var) \
  if (var != NULL) \
  { \
    mkl_free(var); \
    var = NULL; \
  }

vtkStandardNewMacro(vtkPlusForoughiBoneSurfaceProbability);

//----------------------------------------------------------------------------
vtkPlusForoughiBoneSurfaceProbability::vtkPlusForoughiBoneSurfaceProbability()
{
  this->BlurredVSBLoG = 3;
  this->BoneThreshold = 0.4;
  this->ShadowSigma = 6.0;
  this->ShadowVSIntensity = 5;
  this->SmoothingSigma = 5.0;
  this->TransducerMargin = 60;

  this->KernelUpdateRequested = true;

  this->GaussianKernelSize = 0;
  this->FrameSize[0] = 0;
  this->FrameSize[1] = 0;
  this->FrameSize[2] = 1;

  this->MklGaussianBuffer = NULL;
  this->MklLaplacianOfGaussianBuffer = NULL;
  this->MklGaussianBufferTemp = NULL;
  this->MklLaplacianOfGaussianBufferTemp = NULL;
  this->MklReflectionNumberBuffer = NULL;
  this->MklShadowValueBuffer = NULL;
  this->MklShadowModel = NULL;
  this->MklGaussianKernel = NULL;
  this->MklLaplacianKernel = NULL;
}

//----------------------------------------------------------------------------
vtkPlusForoughiBoneSurfaceProbability::~vtkPlusForoughiBoneSurfaceProbability()
{
  DeleteKernels();
  // mkl_free_buffers();
}

//----------------------------------------------------------------------------
void vtkPlusForoughiBoneSurfaceProbability::SimpleExecute(vtkImageData* input, vtkImageData* output)
{
  // Allocate output image
  output->SetExtent(input->GetExtent());
#if (VTK_MAJOR_VERSION < 6)
  output->SetScalarType(input->GetScalarType());
  output->SetNumberOfScalarComponents(input->GetNumberOfScalarComponents());
  output->AllocateScalars();
#else
  output->AllocateScalars(input->GetScalarType(), input->GetNumberOfScalarComponents());
#endif

  int* inputExtent = input->GetExtent();
  if ((inputExtent[1] - inputExtent[0] + 1) != this->FrameSize[0] || (inputExtent[3] - inputExtent[2] + 1) != this->FrameSize[1])
  {
    this->FrameSize[0] = static_cast<unsigned int>(inputExtent[1] - inputExtent[0] + 1);
    this->FrameSize[1] = static_cast<unsigned int>(inputExtent[3] - inputExtent[2] + 1);
    this->FrameSize[2] = 1;
    this->KernelUpdateRequested = true;
  }
  if (this->KernelUpdateRequested)
  {
    UpdateKernels();
    this->KernelUpdateRequested = false;
  }

  int* dims = input->GetDimensions();
  unsigned int nx = this->FrameSize[0];
  unsigned int ny = this->FrameSize[1];
  unsigned int sliceSize = this->FrameSize[0] * this->FrameSize[1];

  vtkSmartPointer<vtkTimerLog> timer = vtkSmartPointer<vtkTimerLog>::New();

  // Loop through each slice
  for (unsigned int sliceIdx = inputExtent[4]; sliceIdx <= inputExtent[5]; ++sliceIdx)
  {
    LOG_INFO("---------------");
    // Index of slice in buffer
    double* inputSlicePtr = static_cast<double*>(input->GetScalarPointer(0, 0, sliceIdx));
    double* outputSlicePtr = static_cast<double*>(output->GetScalarPointer(0, 0, sliceIdx));

    // If slice has not all zero pixels...
    //if (GetMaxPixelValue(inputSlicePtr, sliceSize) > 0) // this is expensive and always true (except error cases)
    {
      // Convolve with Gaussian kernel and normalize result between zero and one
      timer->StartTimer();
      Conv2(inputSlicePtr, this->MklGaussianKernel, this->MklGaussianBufferTemp, this->MklGaussianBuffer, static_cast<int>(this->FrameSize[0]), static_cast<int>(this->FrameSize[1]), this->GaussianKernelSize, this->GaussianKernelSize);
      timer->StopTimer();
      LOG_INFO("Conv2 1: " << timer->GetElapsedTime());

      timer->StartTimer();
      Normalize(this->MklGaussianBuffer, sliceSize, false);
      timer->StopTimer();
      LOG_INFO("Normalize 1: " << timer->GetElapsedTime());

      // Convolve blurred image with Laplacian kernel
      timer->StartTimer();
      Conv2(this->MklGaussianBuffer, this->MklLaplacianKernel, this->MklLaplacianOfGaussianBufferTemp, this->MklLaplacianOfGaussianBuffer, static_cast<int>(this->FrameSize[0]), static_cast<int>(this->FrameSize[1]), 3, 3);
      timer->StopTimer();
      LOG_INFO("Conv2 2: " << timer->GetElapsedTime());

      // Main loop calculating reflection number and shadow value
      timer->StartTimer();
      double sumG = 0;
      double sumGI = 0;
      double sumHist = 0;
      int i, pixelIdx, x, y;
#ifdef NDEBUG
      #pragma omp parallel for reduction(+:sumG,sumGI, sumHist), private(i, x, pixelIdx)
#endif
      for (y = 0; y < ny; ++y)
      {
        for (x = 0; x < nx; ++x)
        {
          pixelIdx = x + y * nx;

          // Only include pixels with intensity value larger than a specified threshold
          if (this->MklGaussianBuffer[pixelIdx] >= this->BoneThreshold && pixelIdx > this->TransducerMargin * nx)
          {
            // Set outermost border pixels to zero and exclude negative pixels
            if ((x == nx - 1 || x == 0 || y == ny - 1 || y == 0) || this->MklLaplacianOfGaussianBuffer[pixelIdx] <= 0)
            {
              this->MklLaplacianOfGaussianBuffer[pixelIdx] = 0.0;
            }
            else
            {
              // Divide by small number to increase image intensity (What! :)
              this->MklLaplacianOfGaussianBuffer[pixelIdx] = this->MklLaplacianOfGaussianBuffer[pixelIdx] / 0.005;
            }

            // Calculate reflection number
            this->MklReflectionNumberBuffer[pixelIdx] = pow(this->MklGaussianBuffer[pixelIdx], this->BlurredVSBLoG) + this->MklLaplacianOfGaussianBuffer[pixelIdx];

            // Calculate shadow value
            sumG = 0;
            sumGI = 0;
            for (i = y; i < ny; ++i)
            {
              sumG += this->MklShadowModel[i - y];
              sumGI += this->MklShadowModel[i - y] * this->MklGaussianBuffer[x + i * nx];
            }
            this->MklShadowValueBuffer[pixelIdx] = sumGI / sumG;
          }
          else
          {
            this->MklReflectionNumberBuffer[pixelIdx] = 0.0;
            this->MklShadowValueBuffer[pixelIdx] = 0.0;
          }
        }
      }

      timer->StopTimer();
      LOG_INFO("Main processing: " << timer->GetElapsedTime());

      // Normalize both reflection numbers and shadow values
      timer->StartTimer();
      Normalize(this->MklReflectionNumberBuffer, sliceSize, false);
      Normalize(this->MklShadowValueBuffer, sliceSize, true);
      timer->StopTimer();
      LOG_INFO("Normalize 2x: " << timer->GetElapsedTime());

      // Calculate BSP
      timer->StartTimer();
      vdPowx(sliceSize, this->MklShadowValueBuffer, this->ShadowVSIntensity, this->MklShadowValueBuffer);
      vdMul(sliceSize, this->MklShadowValueBuffer, this->MklReflectionNumberBuffer, outputSlicePtr);
      timer->StopTimer();
      LOG_INFO("Non-linear transform: " << timer->GetElapsedTime());

      // Normalize BSP
      timer->StartTimer();
      Normalize(outputSlicePtr, sliceSize, false, 255);
      timer->StopTimer();
      LOG_INFO("Normalize 3: " << timer->GetElapsedTime());
    }
  }
}

//-----------------------------------------------------------------------------
void vtkPlusForoughiBoneSurfaceProbability::UpdateKernels()
{
  DeleteKernels();

  this->GaussianKernelSize = floor(this->SmoothingSigma * 3) * 2 + 1;

  // Allocating memory for matrices aligned on 64-byte boundary for better performance
  this->MklGaussianBuffer = (double*)mkl_malloc(this->FrameSize[0] * this->FrameSize[1] * sizeof(double), 64);
  this->MklLaplacianOfGaussianBuffer = (double*)mkl_malloc(this->FrameSize[0] * this->FrameSize[1] * sizeof(double), 64);
  this->MklGaussianBufferTemp = (double*)mkl_malloc((this->FrameSize[0] + GaussianKernelSize - 1) * (this->FrameSize[1]  + GaussianKernelSize - 1) * sizeof(double), 64);
  this->MklLaplacianOfGaussianBufferTemp = (double*)mkl_malloc((this->FrameSize[0] + 2) * (this->FrameSize[1]  + 2) * sizeof(double), 64);
  this->MklReflectionNumberBuffer = (double*)mkl_malloc(this->FrameSize[0] * this->FrameSize[1] * sizeof(double), 64);
  this->MklShadowValueBuffer = (double*)mkl_malloc(this->FrameSize[0] * this->FrameSize[1] * sizeof(double), 64);
  this->MklShadowModel = (double*)mkl_malloc(this->FrameSize[1] * sizeof(double), 64);
  this->MklGaussianKernel = (double*)mkl_malloc(GaussianKernelSize * GaussianKernelSize * sizeof(double), 64);
  this->MklLaplacianKernel = (double*)mkl_malloc(3 * 3 * sizeof(double), 64);

  // Calculate shadow model
  for (int i = 0; i < this->FrameSize[1]; ++i)
  {
    if (i < this->FrameSize[1] - 5)
    {
      this->MklShadowModel[i] = 1 - exp(- (i * i - 1) / (2 * this->ShadowSigma * this->ShadowSigma));
    }
    else
    {
      this->MklShadowModel[i] = 0.0;
    }
  }

  // Calculate Gaussian kernel
  int idx = 0;
  int intervall = (GaussianKernelSize - 1) / 2;
  for (double x = -intervall; x <= intervall; ++x)
  {
    for (double y = -intervall; y <= intervall; ++y)
    {
      this->MklGaussianKernel [idx] = exp(-((x * x) / (2 * this->SmoothingSigma * this->SmoothingSigma) + (y * y) / (2 * this->SmoothingSigma * this->SmoothingSigma)));
      ++idx;
    }
  }

  // Calculate Laplacian kernel
  this->MklLaplacianKernel[0] = 0;
  this->MklLaplacianKernel[1] = -1;
  this->MklLaplacianKernel[2] = 0;
  this->MklLaplacianKernel[3] = -1;
  this->MklLaplacianKernel[4] = 4;
  this->MklLaplacianKernel[5] = -1;
  this->MklLaplacianKernel[6] = 0;
  this->MklLaplacianKernel[7] = -1;
  this->MklLaplacianKernel[8] = 0;
}

//-----------------------------------------------------------------------------
void vtkPlusForoughiBoneSurfaceProbability::DeleteKernels()
{
  // Free memory
  MKL_FREE_IF_NULL(this->MklGaussianBuffer);
  MKL_FREE_IF_NULL(this->MklLaplacianOfGaussianBuffer);
  MKL_FREE_IF_NULL(this->MklGaussianBufferTemp);
  MKL_FREE_IF_NULL(this->MklLaplacianOfGaussianBufferTemp);
  MKL_FREE_IF_NULL(this->MklReflectionNumberBuffer);
  MKL_FREE_IF_NULL(this->MklShadowValueBuffer);
  MKL_FREE_IF_NULL(this->MklShadowModel);
  MKL_FREE_IF_NULL(this->MklGaussianKernel);
  MKL_FREE_IF_NULL(this->MklLaplacianKernel);
}

//-----------------------------------------------------------------------------
// Performs a 2D convolution using Intel MKL defined by the kernel buffer.
void vtkPlusForoughiBoneSurfaceProbability::Conv2(const double* inputBuffer, const double* kernelBuffer, double* tempBuffer, double* outputBuffer, int nx, int ny, int kx, int ky)
{
  int inputShape[] = {nx, ny};
  int kernelShape[] = {kx, ky};
  int resultShape[] = {nx + kx - 1, ny + ky - 1};

  VSLConvTaskPtr task;
  int status = vsldConvNewTask(&task, VSL_CONV_MODE_AUTO, 2, inputShape, kernelShape, resultShape);

  status = vsldConvExec(task, inputBuffer, NULL, kernelBuffer, NULL, tempBuffer, NULL);
  vslConvDeleteTask(&task);

  vtkSmartPointer<vtkTimerLog> timer = vtkSmartPointer<vtkTimerLog>::New();
  timer->StartTimer();
  ResizeMatrix(tempBuffer, outputBuffer, kx, ky, nx + kx - 1, ny + ky - 1);
  timer->StopTimer();
  LOG_INFO("Resizematrix: " << timer->GetElapsedTime());
}

//-----------------------------------------------------------------------------
void vtkPlusForoughiBoneSurfaceProbability::ResizeMatrix(const double* inputBuffer, double* outputBuffer, int xClipping, int yClipping, int xInputSize, int yInputSize)
{
  int xStart = (xClipping + 2 - 1) / 2 - 1;
  int yStart = (yClipping + 2 - 1) / 2 - 1;
  int xStop = xInputSize - xStart;
  int yStop = yInputSize - yStart;

  int idx = 0;
  for (int y = 0; y < yInputSize; ++y)
  {
    for (int x = 0; x < xInputSize; ++x)
    {
      if (x >= xStart && x < xStop && y >= yStart && y < yStop)
      {
        outputBuffer[idx] = inputBuffer[x + y * xInputSize];
        ++idx;
      }
    }
  }
}

//-----------------------------------------------------------------------------
double vtkPlusForoughiBoneSurfaceProbability::GetMaxPixelValue(const double* buffer, int size)
{
  double maxPixelValue = 0;
  for (int i = 0; i < size; ++i)
  {
    if (buffer[i] > maxPixelValue)
    {
      maxPixelValue = buffer[i];
    }
  }
  return maxPixelValue;
}

//-----------------------------------------------------------------------------
void vtkPlusForoughiBoneSurfaceProbability::Normalize(double* buffer, int size, bool doInverse, double maxValue /*=1.0*/)
{
  double maxPixelValue = GetMaxPixelValue(buffer, size) / maxValue;

  if (!doInverse)
  {
    for (int i = 0; i < size; ++i)
    {
      buffer[i] =  buffer[i] / maxPixelValue;
    }
    return;
  }

  for (int i = 0; i < size; ++i)
  {
    buffer[i] = maxValue - buffer[i] / maxPixelValue;
  }
}