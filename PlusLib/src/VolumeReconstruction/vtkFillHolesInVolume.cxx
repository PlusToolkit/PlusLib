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

#include "PlusConfigure.h"
#include "PlusMath.h"

#include "vtkFillHolesInVolume.h"

#include "vtkDataArray.h"
#include "vtkImageData.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkPointData.h"
#include "vtkImageExtractComponents.h"
#include "vtkMetaImageWriter.h"

#include <math.h>

static const int INPUT_PORT_RECONSTRUCTED_VOLUME=0;
static const int INPUT_PORT_ACCUMULATION_BUFFER=1;

#ifndef OPAQUE_ALPHA
static const unsigned char OPAQUE_ALPHA=255;
#endif

///////////

vtkStandardNewMacro(vtkFillHolesInVolume);

struct FillHoleThreadFunctionInfoStruct
{
	vtkImageData* ReconstructedVolume;
	vtkImageData* Accumulator;
  int Compounding;
};


//----------------------------------------------------------------------------
vtkFillHolesInVolume::vtkFillHolesInVolume()
{
  this->SetNumberOfInputPorts(2);
  this->SetNumberOfOutputPorts(1);
  this->Compounding=0;
  Kernels = NULL;
}

//----------------------------------------------------------------------------
vtkFillHolesInVolume::~vtkFillHolesInVolume()
{
	if (Kernels != NULL)
		delete[] Kernels;
}

//----------------------------------------------------------------------------
void vtkFillHolesInVolume::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
  os << indent << "Compounding: " << this->Compounding<< "\n";
}

//----------------------------------------------------------------------------
int vtkFillHolesInVolume::RequestInformation(vtkInformation* vtkNotUsed(request),
  vtkInformationVector** inputVector, vtkInformationVector* outputVector)
{  
  int extent[6];

  // get the info objects
  vtkInformation* outInfo = outputVector->GetInformationObject(0);
  vtkInformation* inInfo = inputVector[INPUT_PORT_RECONSTRUCTED_VOLUME]->GetInformationObject(0);

  // invalid setting, it has not been set, so default to whole Extent
  inInfo->Get(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT(), extent);
  
  outInfo->Set(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT(), extent, 6);

  return 1;
}


//----------------------------------------------------------------------------
int vtkFillHolesInVolume::RequestUpdateExtent (vtkInformation* vtkNotUsed(request),
  vtkInformationVector** inputVector, vtkInformationVector* outputVector)
{
  int wholeExtent[6];

  // get the info objects
  vtkInformation* outInfo = outputVector->GetInformationObject(0);
  vtkInformation *inInfo = inputVector[INPUT_PORT_RECONSTRUCTED_VOLUME]->GetInformationObject(0);

  // invalid setting, it has not been set, so default to whole Extent
  inInfo->Get(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT(), 
              wholeExtent);
  int inUExt[6]; 
  outInfo->Get(vtkStreamingDemandDrivenPipeline::UPDATE_EXTENT(), inUExt);

  inInfo->Set(vtkStreamingDemandDrivenPipeline::UPDATE_EXTENT(), inUExt, 6);  

  return 1;
}

//----------------------------------------------------------------------------
/*template <class T>
int vtkFillHolesInVolume::weightedAverageOverNeighborhood(T* inputData,// contains the dataset being interpolated between
											  unsigned short* accData, // contains the weights of each voxel
											  int* inputOffsets,       // contains the indexing offsets between adjacent x,y,z
											  int* accOffsets,
											  const int& inputComp,	   // the component index of interest
											  int* bounds,             // the boundaries of the volume, outputExtent
											  const int& neighborSize, // The size of the neighborhood, odd positive integer
											  int* thisPixel,		   // The x,y,z coordinates of the voxel being calculated
											  T& returnVal)           // The value of the pixel being calculated (unknown)
{

	if (neighborSize%2 != 1 || neighborSize <= 1)
	{
		LOG_ERROR("vtkFillHolesInVolume::weightedAverageOverNeighborhood: 'neighborSize' must be a positive odd integer greater or equal to 3");
		return 0;
	}

	// set the x, y, and z range
	int range = (neighborSize-1)/2; // so with N = 3, our range is x-1 through x+1, and so on
	int minX = thisPixel[0] - range;
	int minY = thisPixel[1] - range;
	int minZ = thisPixel[2] - range;
	int maxX = thisPixel[0] + range;
	int maxY = thisPixel[1] + range;
	int maxZ = thisPixel[2] + range;

	unsigned long int sumIntensities(0); // unsigned long because these rise in value quickly
	unsigned long int sumAccumulator(0);

	for (int x = minX; x <= maxX; x++)
	{
		for (int y = minY; y <= maxY; y++)
		{
			for (int z = minZ; z <= maxZ; z++)
			{
				if (x <= bounds[1] && x >= bounds[0] &&
					y <= bounds[3] && y >= bounds[2] &&
					z <= bounds[5] && z >= bounds[4] ) // check bounds
				{
					int volIndex = inputOffsets[0]*x+inputOffsets[1]*y+inputOffsets[2]*z+inputComp;
					int accIndex =   accOffsets[0]*x+  accOffsets[1]*y+  accOffsets[2]*z;
					sumIntensities += inputData[volIndex] * accData[accIndex];
					sumAccumulator +=   accData[accIndex];
				} // end boundary check
			} // end z loop
		} // end y loop
	} // end x loop

	if (sumAccumulator == 0) // no voxels set in the area
		return 0;

	// TODO: Overflow protection
	returnVal = (T)(sumIntensities/sumAccumulator);

	return 1;

}*/

//----------------------------------------------------------------------------
template <class T>
double vtkFillHolesInVolume::weightedAverageOverNeighborhoodWithGaussian(
											  T* inputData,            // contains the dataset being interpolated between
											  unsigned short* accData, // contains the weights of each voxel
											  vtkIdType* inputOffsets,       // contains the indexing offsets between adjacent x,y,z
											  vtkIdType* accOffsets,
											  const int& inputComp,	   // the component index of interest
											  int* bounds,             // the boundaries of the volume, outputExtent
											  const int& kernelIndex, // The size of the neighborhood, odd positive integer
											  unsigned int* kernel,    // the gaussian kernel matrix
											  int* thisPixel,		   // The x,y,z coordinates of the voxel being calculated
											  T& returnVal)            // The value of the pixel being calculated (unknown)
{

	// set the x, y, and z range
	int rangeX = (Kernels[kernelIndex].size[0]-1)/2; // so with N = 3, our range is x-1 through x+1, and so on
	int rangeY = (Kernels[kernelIndex].size[1]-1)/2;
	int rangeZ = (Kernels[kernelIndex].size[2]-1)/2;
	int minX = thisPixel[0] - rangeX;
	int minY = thisPixel[1] - rangeY;
	int minZ = thisPixel[2] - rangeZ;
	int maxX = thisPixel[0] + rangeX;
	int maxY = thisPixel[1] + rangeY;
	int maxZ = thisPixel[2] + rangeZ;

	unsigned long long sumIntensities(0); // unsigned long because these rise in value quickly
	unsigned long long sumAccumulator(0);
	unsigned long long currentAccumulation(0);
	int numKnownVoxels(0);

	for (int x = minX; x <= maxX; x++)
	{
		for (int y = minY; y <= maxY; y++)
		{
			for (int z = minZ; z <= maxZ; z++)
			{
				if (x <= bounds[1] && x >= bounds[0] &&
					y <= bounds[3] && y >= bounds[2] &&
					z <= bounds[5] && z >= bounds[4] ) // check bounds
				{
					int accIndex =   accOffsets[0]*x+  accOffsets[1]*y+  accOffsets[2]*z;
					currentAccumulation = (unsigned long long)accData[accIndex];
					if (currentAccumulation) {
						int volIndex = inputOffsets[0]*x+inputOffsets[1]*y+inputOffsets[2]*z+inputComp;
						int kerIndex = Kernels[kernelIndex].size[1]*Kernels[kernelIndex].size[0]*(z-minZ)+Kernels[kernelIndex].size[0]*(y-minY)+(x-minX);
						unsigned long long weight = currentAccumulation * (unsigned long long)kernel[kerIndex];
						sumIntensities += (unsigned long long)inputData[volIndex] * weight;
						sumAccumulator += weight;
						numKnownVoxels++;
					}
				} // end boundary check
			} // end z loop
		} // end y loop
	} // end x loop

	if (sumAccumulator == 0) // no voxels set in the area
		return 0;

	// TODO: Overflow protection
	returnVal = (T)(sumIntensities/sumAccumulator);

	return ((double)numKnownVoxels)/(Kernels[kernelIndex].size[0]*Kernels[kernelIndex].size[1]*Kernels[kernelIndex].size[2]);

}
//----------------------------------------------------------------------------

unsigned int* vtkFillHolesInVolume::calculateGaussianMatrix(const int& kernelIndex) // stdev as input based on hole size
																		      // kernel size is a compromise between image quality and computation time
{

    unsigned int* matrix = new unsigned int[(Kernels[kernelIndex].size[0]*Kernels[kernelIndex].size[1]*Kernels[kernelIndex].size[2])];
	// stdev for each of x, y, z
	double stdev[3];
	stdev[0] = Kernels[kernelIndex].stdev[0]; //x
	stdev[1] = Kernels[kernelIndex].stdev[1]; //y
	stdev[2] = Kernels[kernelIndex].stdev[2]; //z

	double rangeX = (Kernels[kernelIndex].size[0]-1)/2.;
	double rangeY = (Kernels[kernelIndex].size[1]-1)/2.;
	double rangeZ = (Kernels[kernelIndex].size[2]-1)/2.;
	const double Pi = 3.1415926535897932384626433832795;

	// divisors in the exponent
	double divisor[3];
	divisor[0] = Kernels[kernelIndex].stdev[0] * Kernels[kernelIndex].stdev[0] * 2.;
	divisor[1] = Kernels[kernelIndex].stdev[1] * Kernels[kernelIndex].stdev[1] * 2.;
	divisor[2] = Kernels[kernelIndex].stdev[2] * Kernels[kernelIndex].stdev[2] * 2.;

	// divisor in the non-exponent
	double termDivisor = pow(2*Pi,3./2) * stdev[0] * stdev[1] * stdev[2];

	double min(0);
	int index(0);
	for (int z = 0; z < Kernels[kernelIndex].size[2]; z++)
	{
		for (int y = 0; y < Kernels[kernelIndex].size[1]; y++)
		{
			for (int x = 0; x < Kernels[kernelIndex].size[0]; x++)
			{
				double xExp = pow((double)x-rangeX,2)/divisor[0];
				double yExp = pow((double)y-rangeY,2)/divisor[1];
				double zExp = pow((double)z-rangeZ,2)/divisor[2];
				double eExp = -(xExp + yExp + zExp);

				double calcVal = 1./(termDivisor)*exp(eExp);
				if (!min)
					min = calcVal;
				matrix[index] = PlusMath::Round(calcVal/min);
				index++;
			}
		}
	}
	return matrix;

}

//----------------------------------------------------------------------------
template <class T>
void vtkFillHolesInVolume::vtkFillHolesInVolumeExecute(vtkImageData *inVolData,
													   T *inVolPtr, 
													   vtkImageData *accData,
													   unsigned short *accPtr, 
													   vtkImageData *outData, 
													   T *outPtr,
													   int outExt[6], 
													   int id)
{

  if (outData==NULL || outData->GetScalarPointer()==NULL)
  {
    LOG_ERROR("vtkPasteSliceIntoVolumeFillHolesInOutput outData is invalid");
    return;
  }
  if (outPtr==NULL)
  {
    LOG_ERROR("vtkPasteSliceIntoVolumeFillHolesInOutput outPtr is invalid");
    return;
  }
  if (accPtr==NULL)
  {
    LOG_ERROR("vtkPasteSliceIntoVolumeFillHolesInOutput accPtr is invalid");
    return;
  }

	unsigned int** kernelFilter = new unsigned int*[NumKernels];
	for (int i = 0; i < NumKernels; i++) {
      kernelFilter[i] = calculateGaussianMatrix(i);
	}

	// get increments for volume and for accumulation buffer
  vtkIdType byteIncVol[3]={0}; //x,y,z
	outData->GetIncrements(byteIncVol[0],byteIncVol[1],byteIncVol[2]);
  vtkIdType byteIncAcc[3]={0}; //x,y,z
	accData->GetIncrements(byteIncAcc[0],byteIncAcc[1],byteIncAcc[2]);

	// this will store the position of the pixel being looked at currently
	int currentPos[3]; //x,y,z

	int numVolumeComponents = outData->GetNumberOfScalarComponents() - 1; // subtract 1 because of the alpha channel

	// iterate through each voxel. When the accumulation buffer is 0, fill that hole, and continue.
	for (currentPos[2] = outExt[4]; currentPos[2] <= outExt[5]; currentPos[2]++)
	{
		for (currentPos[1] = outExt[2]; currentPos[1] <= outExt[3]; currentPos[1]++)
		{
			for (currentPos[0] = outExt[0]; currentPos[0] <= outExt[1]; currentPos[0]++)
			{
				// accumulator index and volume alpha index should not depend on which individual component is being interpolated
				int accIndex = (currentPos[0]*byteIncAcc[0])+(currentPos[1]*byteIncAcc[1])+(currentPos[2]*byteIncAcc[2]);
				int volAlphaIndex = (currentPos[0]*byteIncVol[0])+(currentPos[1]*byteIncVol[1])+(currentPos[2]*byteIncVol[2])+numVolumeComponents;
				if (accPtr[accIndex] == 0) // if not hit by accumulation during vtkPasteSliceIntoVolume
				{
					bool alphaSet = false; // set true once alpha has been evaluated (only donce once)
					for (int c = 0; c < numVolumeComponents; c++)
					{
						// volume index for this component
						int volCompIndex = (currentPos[0]*byteIncVol[0])+(currentPos[1]*byteIncVol[1])+(currentPos[2]*byteIncVol[2])+c;
						double result(0);


						for (int k = 0; k < NumKernels; k++) // k is the index of the kernel being tried
						{
							result = weightedAverageOverNeighborhoodWithGaussian(inVolPtr,accPtr,byteIncVol,byteIncAcc,c,outExt,k,kernelFilter[k],currentPos,outPtr[volCompIndex]);
							if (result >= Kernels[k].minRatio) {
								break;
							} // end checking interpolation success
						}

						if (result < Kernels[NumKernels-1].minRatio) // if there are not enough voxels within range, just set this one (and its alpha) to zero
						{
							outPtr[volCompIndex] = 0;
						}

						if (!alphaSet)
						{
							alphaSet = true; // only do this check once
							if (result < Kernels[NumKernels-1].minRatio) // if there are no voxels within range, alpha should be 0
							{
								outPtr[volAlphaIndex] = 0;
							}
							else // found a result, should set the alpha channel to its maximum value
							{
								outPtr[volAlphaIndex] = (T)OPAQUE_ALPHA;
							} // end checking interpolation success
						}

					} // end component loop
				}
				else // if hit, just use the apparent value
				{
					for (int c = 0; c < numVolumeComponents; c++)
					{
						int volCompIndex = (currentPos[0]*byteIncVol[0])+(currentPos[1]*byteIncVol[1])+(currentPos[2]*byteIncVol[2])+c;
						outPtr[volCompIndex] = inVolPtr[volCompIndex];
					} // end component loop
					outPtr[volAlphaIndex] = (T)OPAQUE_ALPHA;
				} // end accumulation check
			} // end x loop
		} // end y loop
	} // end z loop

	for (int i = 0; i < NumKernels; i++) {
      delete[] kernelFilter[i];
	}
	delete[] kernelFilter;

}

//----------------------------------------------------------------------------
void vtkFillHolesInVolume::ThreadedRequestData(vtkInformation *request, 
    vtkInformationVector **inputVector, 
    vtkInformationVector *outputVector,
    vtkImageData ***inData, 
    vtkImageData **outData,
    int outExt[6], int threadId)
{
  vtkImageData* outVolData = outData[0];  
  void *outVolPtr = outVolData->GetScalarPointer();  

  vtkImageData* inVolData=inData[0][0];
  void *inVolPtr = inVolData->GetScalarPointer();

  vtkImageData* inAccData=inData[1][0];
  void *inAccPtr = inAccData->GetScalarPointer();

  // this filter expects that input is the same type as output.
  if (inVolData->GetScalarType() != outVolData->GetScalarType())
    {
    LOG_ERROR("Execute: input data type, " 
              << inVolData->GetScalarType()
              << ", must match out ScalarType " 
              << outVolData->GetScalarType());
    return;
    }
  
  switch (inVolData->GetScalarType())
    {
      vtkTemplateMacro(
      vtkFillHolesInVolumeExecute(/*this,*/ 
                                       inVolData, static_cast<VTK_TT *>(inVolPtr),
                                       inAccData, static_cast<unsigned short *>(inAccPtr),
                                       outVolData,
                                       static_cast<VTK_TT *>(outVolPtr), outExt,
                                       threadId));
    default:
      LOG_ERROR("Execute: Unknown ScalarType");
      return;
    }
}

// TODO: Remove if not needed anymore
/*vtkFillHolesInVolumeKernel vtkFillHolesInVolume::GetKernel(int index) {
	return Kernels[index];
}*/

void vtkFillHolesInVolume::SetKernel(int index, vtkFillHolesInVolumeKernel& kernel) {
	for (int i = 0; i < 3; i++) {
		Kernels[index].size[i] = kernel.size[i];
		Kernels[index].stdev[i] = kernel.stdev[i];
	}
	Kernels[index].minRatio = kernel.minRatio;
}

void vtkFillHolesInVolume::AllocateKernels() {
	if (Kernels != NULL) {
		delete[] Kernels;
		Kernels = NULL;
	}
	Kernels = new vtkFillHolesInVolumeKernel[NumKernels];
}

// TODO: Remove if not needed
/*int vtkFillHolesInVolume::GetNumKernels() {
	return NumKernels;
}*/

void vtkFillHolesInVolume::SetNumKernels(int n) {
	NumKernels = n;
}

void vtkFillHolesInVolume::SetReconstructedVolume(vtkImageData *reconstructedVolume)
{
  SetInput(INPUT_PORT_RECONSTRUCTED_VOLUME, reconstructedVolume);
}

void vtkFillHolesInVolume::SetAccumulationBuffer(vtkImageData *accumulationBuffer)
{
  SetInput(INPUT_PORT_ACCUMULATION_BUFFER, accumulationBuffer);
}
