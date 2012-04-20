/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

/*=========================================================================
The following copyright notice is applicable to parts of this file:

Copyright (c) 2000-2007 Atamai, Inc.
Copyright (c) 2008-2009 Danielle Pace

Use, modification and redistribution of the software, in source or
binary forms, are permitted provided that the following terms and
conditions are met:

1) Redistribution of the source code, in verbatim or modified
form, must retain the above copyright notice, this license,
the following disclaimer, and any notices that refer to this
license and/or the following disclaimer.

2) Redistribution in binary form must include the above copyright
notice, a copy of this license and the following disclaimer
in the documentation or with other materials provided with the
distribution.

3) Modified copies of the source code must be clearly marked as such,
and must not be misrepresented as verbatim copies of the source code.

THE COPYRIGHT HOLDERS AND/OR OTHER PARTIES PROVIDE THE SOFTWARE "AS IS"
WITHOUT EXPRESSED OR IMPLIED WARRANTY INCLUDING, BUT NOT LIMITED TO,
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
PURPOSE.  IN NO EVENT SHALL ANY COPYRIGHT HOLDER OR OTHER PARTY WHO MAY
MODIFY AND/OR REDISTRIBUTE THE SOFTWARE UNDER THE TERMS OF THIS LICENSE
BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, LOSS OF DATA OR DATA BECOMING INACCURATE
OR LOSS OF PROFIT OR BUSINESS INTERRUPTION) ARISING IN ANY WAY OUT OF
THE USE OR INABILITY TO USE THE SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGES.
=========================================================================*/

#ifndef __vtkFillHolesInVolume_h
#define __vtkFillHolesInVolume_h

#include "vtkThreadedImageAlgorithm.h"

/*!
  /struct vtkFillHolesInVolumeKernel
  /brief Holds information about a user-specified kernel

  Holds information about a user-specified kernel
*/
struct vtkFillHolesInVolumeKernel {
  int size[3];
  float stdev[3];
  float minRatio;
};

/*!
  \class vtkFillHolesInVolume
  \brief Fill holes in a volume reconstructed from slices.

  This class has not been tested extensively and it may not work properly!
  Fill holes in the output by using the weighted average of the
  surrounding voxels.  If Compounding is off, then all hit voxels
  are weighted equally. 
  These papers could be useful to study when reviewing/updating the algorithm:
  http://www.irma-international.org/viewtitle/46058/
  http://www.3dmed.net/paper/daiyakang_TITB.pdf

  Warning: The class was part of the SynchroGrab library and ported into
  Plus, but has not been tested and probably needs some work to fill the holes correctly.
  See https://www.assembla.com/spaces/plus/tickets/285-hole-filling-after-volume-reconstruction-doesn-t-seem-to-be-effective

  \sa vtkPasteSliceIntoVolume
  \ingroup PlusLibVolumeReconstruction
*/
class VTK_EXPORT vtkFillHolesInVolume : public vtkThreadedImageAlgorithm
{
public:
  static vtkFillHolesInVolume *New();
  vtkTypeMacro(vtkFillHolesInVolume,vtkThreadedImageAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent);
    
  /*! Get current compounding setting */
  vtkGetMacro(Compounding,int);

  /*!
    Turn on or off the compounding (default on, which means
    that scans will be compounded where they overlap instead
    of just considering the last acquired slice.
  */
  vtkSetMacro(Compounding,int);

  /*!
    Get the index'th kernel that is to be tried, index ranging from 0 (first kernel)
	up to NumKernels-1 (last kernel).
  */
  //vtkFillHolesInVolumeKernel GetKernel(int index);

  /*!
    Set the index'th kernel that is to be tried, index ranging from 0 (first kernel)
	up to NumKernels-1 (last kernel).
  */
  void SetKernel(int index, vtkFillHolesInVolumeKernel& kernel);

  /*!
    Allocate memory for all of the kernels that are to be tried. NumKernels must be 
	set first (see SetNumKernels)
  */
  void AllocateKernels();

  /*!
    Get the number of kernels that are to be tried on the data.
  */
  //int GetNumKernels();

  /*!
    Get the number of kernels that are to be tried on the data.
  */
  void SetNumKernels(int n);

  /*! Set the input volume (reconstructed volume, with holes) */
  void SetReconstructedVolume(vtkImageData *reconstructedVolume);

  /*! 
    Set the input accumulation buffer, which indicates how many slices
    were inserted at each particular voxel.
  */
  void SetAccumulationBuffer(vtkImageData *accumulationBuffer);

  unsigned int* calculateGaussianMatrix(const int& kernelIndex);

  /*!
	Perform Interpolation between the nearest NxNxN voxels, and store at 
	the address provided in returnVal. Returns 1 on success and 0 on failure.
  */
  /*template <class T>
  int weightedAverageOverNeighborhood( T* inputData,             // contains the dataset being interpolated between
											  unsigned short* accData, // contains the weights of each voxel
											  int* inputOffsets,       // contains the indexing offsets between adjacent x,y,z
											  int* accOffsets,
											  const int& inputComp,	   // the component index of interest
											  int* bounds,             // the boundaries of the volume, outputExtent
											  const int& neighborSize, // The size of the neighborhood, odd positive integer
											  int* thisPixel,		   // The x,y,z coordinates of the voxel being calculated
											  T& returnVal);           // The value of the pixel being calculated (unknown)*/

  template <class T>
  double weightedAverageOverNeighborhoodWithGaussian(T* inputData, // contains the dataset being interpolated between
											  unsigned short* accData, // contains the weights of each voxel
											  vtkIdType* inputOffsets,       // contains the indexing offsets between adjacent x,y,z
											  vtkIdType* accOffsets,
											  const int& inputComp,	   // the component index of interest
											  int* bounds,             // the boundaries of the volume, outputExtent
											  const int& neighborSize, // The size of the neighborhood, odd positive integer
											  unsigned int* kernel,    // the gaussian kernel matrix
											  int* thisPixel,		   // The x,y,z coordinates of the voxel being calculated
											  T& returnVal);           // The value of the pixel being calculated (unknown)


protected:
  vtkFillHolesInVolume();
  ~vtkFillHolesInVolume();

  /*!
    This method is passed a region that holds the image extent of this filters
    input, and changes the region to hold the image extent of this filters
    output.
  */
  virtual int RequestInformation (vtkInformation*,
                                  vtkInformationVector**,
                                  vtkInformationVector*);
  
  /*! This method computes the input extent necessary to generate the output */
  virtual int RequestUpdateExtent(vtkInformation*,
                                  vtkInformationVector**,
                                  vtkInformationVector*);

  template <class T>
  void vtkFillHolesInVolumeExecute(vtkImageData *inVolData,
								   T *inVolPtr,
								   vtkImageData *accData,
								   unsigned short *accPtr, 
								   vtkImageData *outData, 
								   T *outPtr,
								   int outExt[6],
								   int id);

  /*!
    This method contains a switch statement that calls the correct
    templated function for the input data type.  The output data
    must match input type.
  */
  virtual void ThreadedRequestData(vtkInformation *request, 
    vtkInformationVector **inputVector, 
    vtkInformationVector *outputVector,
    vtkImageData ***inData, 
    vtkImageData **outData,
    int extent[6], int threadId);

  static VTK_THREAD_RETURN_TYPE FillHoleThreadFunction( void *arg );

  int Compounding;
  int NumKernels;
  vtkFillHolesInVolumeKernel* Kernels;

private:
  vtkFillHolesInVolume(const vtkFillHolesInVolume&);  // Not implemented.
  void operator=(const vtkFillHolesInVolume&);  // Not implemented.
};

#endif



