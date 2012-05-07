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

void FillHolesInVolumeElement::setupAsGaussian(int size, float stdev, float minRatio)
{
  this->type = FillHolesInVolumeElement ::HFTYPE_GAUSSIAN;
  this->size = size;
  this->stdev = stdev;
  this->minRatio = minRatio; 
  kernel = NULL; 
  allocateGaussianMatrix();
}

//----------------------------------------------------------------------------

void FillHolesInVolumeElement::allocateGaussianMatrix()
{

  if (kernel != NULL)
    delete[] kernel;
  kernel = new unsigned int[size*size*size];

	double range = (size-1)/2.;
	const double Pi = 3.1415926535897932384626433832795;

	// divisors in the exponent
	double divisor = stdev*stdev*2.0;

	// divisor in the non-exponent
	double termDivisor = pow(2*Pi,3./2) * pow(stdev,3);

	double min(0);
	int index(0);
	for (int z = 0; z < size; z++)
	{
		for (int y = 0; y < size; y++)
		{
			for (int x = 0; x < size; x++)
			{
				double eExp = -(3*(pow((double)x-range,2)/divisor));
				double calcVal = 1./(termDivisor)*exp(eExp);
				if (!min)
					min = calcVal;
				kernel[index] = PlusMath::Round(calcVal/min);
				index++;
			}
		}
	}

}

//----------------------------------------------------------------------------

template <class T>
bool FillHolesInVolumeElement::applyGaussian(
											  T* inputData,            // contains the dataset being interpolated between
											  unsigned short* accData, // contains the weights of each voxel
											  vtkIdType* inputOffsets, // contains the indexing offsets between adjacent x,y,z
											  vtkIdType* accOffsets,
											  const int& inputComp,	   // the component index of interest
											  int* bounds,             // the boundaries of the thread
                        int* wholeExtent,        // the boundaries of the volume, outputExtent
											  int* thisPixel,		       // The x,y,z coordinates of the voxel being calculated
											  T& returnVal)            // The value of the pixel being calculated (unknown)
{

	// set the x, y, and z range
	int range = (size-1)/2; // so with N = 3, our range is x-1 through x+1, and so on
	int minX = thisPixel[0] - range;
	int minY = thisPixel[1] - range;
	int minZ = thisPixel[2] - range;
	int maxX = thisPixel[0] + range;
	int maxY = thisPixel[1] + range;
	int maxZ = thisPixel[2] + range;

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
				if (x <= wholeExtent[1] && x >= wholeExtent[0] &&
					y <= wholeExtent[3] && y >= wholeExtent[2] &&
					z <= wholeExtent[5] && z >= wholeExtent[4] ) // check bounds
				{
					int accIndex =   accOffsets[0]*x+  accOffsets[1]*y+  accOffsets[2]*z;
					currentAccumulation = (unsigned long long)accData[accIndex];
					if (currentAccumulation) { // if the accumulation buffer for the voxel is non-zero
						int volIndex = inputOffsets[0]*x+inputOffsets[1]*y+inputOffsets[2]*z+inputComp;
						int kerIndex = size*size*(z-minZ)+size*(y-minY)+(x-minX);
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

  if ((double)numKnownVoxels/(size*size*size) >= minRatio)
  {
    returnVal = (T)(sumIntensities/sumAccumulator); // set it if and only if the min ratio is met
    return true;
  }

  // else failure
  return false;

}

//----------------------------------------------------------------------------

void FillHolesInVolumeElement::setupAsStick(int stickLengthLimit) {
  this->type = FillHolesInVolumeElement::HFTYPE_STICK;
  this->stickLengthLimit = stickLengthLimit;
  sticksList = NULL;
  allocateSticks();
}

//----------------------------------------------------------------------------

void FillHolesInVolumeElement::allocateSticks() {
  numSticks = 13;
  sticksList = new int[39];
  sticksList[ 0] = 1; sticksList[ 1] = 0; sticksList[ 2] = 0;
  sticksList[ 3] = 0; sticksList[ 4] = 1; sticksList[ 5] = 0;
  sticksList[ 6] = 0; sticksList[ 7] = 0; sticksList[ 8] = 1;
  sticksList[ 9] = 1; sticksList[10] = 1; sticksList[11] = 0;
  sticksList[12] = 1; sticksList[13] = 0; sticksList[14] = 1;
  sticksList[15] = 0; sticksList[16] = 1; sticksList[17] = 1;
  sticksList[18] = 1; sticksList[19] =-1; sticksList[20] = 0;
  sticksList[21] = 1; sticksList[22] = 0; sticksList[23] =-1;
  sticksList[24] = 0; sticksList[25] = 1; sticksList[26] =-1;
  sticksList[27] = 1; sticksList[28] = 1; sticksList[29] = 1;
  sticksList[30] =-1; sticksList[31] = 1; sticksList[32] = 1;
  sticksList[33] = 1; sticksList[34] =-1; sticksList[35] = 1;
  sticksList[36] =-1; sticksList[37] =-1; sticksList[38] = 1;
}

//----------------------------------------------------------------------------

template <class T>
bool FillHolesInVolumeElement::applySticks(
											  T* inputData,            // contains the dataset being interpolated between
											  unsigned short* accData, // contains the weights of each voxel
											  vtkIdType* inputOffsets, // contains the indexing offsets between adjacent x,y,z
											  vtkIdType* accOffsets,
											  const int& inputComp,	   // the component index of interest
											  int* bounds,             // the boundaries of the thread
                        int* wholeExtent,        // the boundaries of the volume, outputExtent
											  int* thisPixel,		       // The x,y,z coordinates of the voxel being calculated
											  T& returnVal)            // The value of the pixel being calculated (unknown)
{
  // extract coordinates
  int x(thisPixel[0]),y(thisPixel[1]),z(thisPixel[2]); // coordinates of the hole voxel
  int xtemp, ytemp, ztemp; // store the voxel along the current stick
  bool valid; // set to true when we've hit a filled voxel
  int fwdTrav, rvsTrav; // store the number of voxels that have been searched
  T fwdVal, rvsVal; // store the values at each end of the stick

  T values[13]; // TODO: change 13 to numSticks
  double scores[13];

  // try each stick direction
  for (int i = 0; i < numSticks; i++) {

    int baseStickIndex = i * 3; // 3 coordinates per stick, one for each dimension

    // evaluate forward direction to nearest filled voxel
    xtemp = x; ytemp = y; ztemp = z;
    valid = false;
    for (int j = 1; j + 1 <= stickLengthLimit; j++) {
      // traverse in forward direction
      xtemp = xtemp + sticksList[baseStickIndex  ];
      ytemp = ytemp + sticksList[baseStickIndex+1];
      ztemp = ztemp + sticksList[baseStickIndex+2];
      // check boundaries
			if (xtemp > wholeExtent[1] || xtemp < wholeExtent[0] ||
				  ytemp > wholeExtent[3] || ytemp < wholeExtent[2] ||
				  ztemp > wholeExtent[5] || ztemp < wholeExtent[4] ) // check bounds
        break;
      int accIndex =   accOffsets[0]*xtemp+  accOffsets[1]*ytemp+  accOffsets[2]*ztemp;
      if (accData[accIndex] != 0) { // this is a filled voxel
        fwdTrav = j;
        int volIndex = inputOffsets[0]*xtemp+inputOffsets[1]*ytemp+inputOffsets[2]*ztemp+inputComp;
        fwdVal = inputData[volIndex];
        valid = true;
        break;
      }
    } // end searching fwd direction

    // only do reverse direction if we found something forward
    if (!valid) {
      scores[i] = 0.0; // do this to say that this is a bad stick
      continue; // try next stick
    }

    // evaluate reverse direction to nearest filled voxel
    xtemp = x; ytemp = y; ztemp = z;
    valid = false;
    for (int j = 1; j + fwdTrav + 1 <= stickLengthLimit; j++) {
      // traverse in reverse direction
      xtemp = xtemp - sticksList[baseStickIndex  ];
      ytemp = ytemp - sticksList[baseStickIndex+1];
      ztemp = ztemp - sticksList[baseStickIndex+2];
      // check boundaries
			if (xtemp > wholeExtent[1] || xtemp < wholeExtent[0] ||
				  ytemp > wholeExtent[3] || ytemp < wholeExtent[2] ||
				  ztemp > wholeExtent[5] || ztemp < wholeExtent[4] ) // check bounds
        break;
      int accIndex =   accOffsets[0]*xtemp+  accOffsets[1]*ytemp+  accOffsets[2]*ztemp;
      if (accData[accIndex] != 0) { // this is a filled voxel
        rvsTrav = j;
        int volIndex = inputOffsets[0]*xtemp+inputOffsets[1]*ytemp+inputOffsets[2]*ztemp+inputComp;
        rvsVal = inputData[volIndex];
        valid = true;
        break;
      }
    } // end searching rvs direction

    // only calculate a score and a value if we found something in both directions
    if (!valid) {
      scores[i] = 0.0; // do this to say that this is a bad stick
      continue; // try next stick
    }

    // evaluate score and direction
    int totalDistance = fwdTrav + rvsTrav + 1;
    double weightFwd = (rvsTrav+1)/(double)totalDistance;
    double weightRvs = 1.0 - weightFwd;
    scores[i] = 1.0/totalDistance;
    values[i] = weightRvs*rvsVal + weightFwd*fwdVal;
  }

  // determine the highest score, and assign the corresponding value to the pixel
  double maxScore(0);
  for (int i = 0; i < numSticks; i++) {
    if (scores[i] > maxScore) {
      maxScore = scores[i];
      returnVal = values[i];
    }
  }

  if (maxScore == 0) { // indicates all sticks were bad
    returnVal = (T)0;
  	return false; // failure
  }

  return true; // else at least one stick was good, = success

}

//----------------------------------------------------------------------------
vtkFillHolesInVolume::vtkFillHolesInVolume()
{
  this->SetNumberOfInputPorts(2);
  this->SetNumberOfOutputPorts(1);
  this->Compounding=0;
  HFElements = NULL;
}

//----------------------------------------------------------------------------
vtkFillHolesInVolume::~vtkFillHolesInVolume()
{
	if (HFElements != NULL)
		delete[] HFElements;
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

	// get increments for volume and for accumulation buffer
  vtkIdType byteIncVol[3]={0}; //x,y,z
	outData->GetIncrements(byteIncVol[0],byteIncVol[1],byteIncVol[2]);
  vtkIdType byteIncAcc[3]={0}; //x,y,z
	accData->GetIncrements(byteIncAcc[0],byteIncAcc[1],byteIncAcc[2]);

	// this will store the position of the pixel being looked at currently
	int currentPos[3]; //x,y,z

	int numVolumeComponents = outData->GetNumberOfScalarComponents() - 1; // subtract 1 because of the alpha channel

  int* wholeExtent;
  wholeExtent = outData->GetExtent();

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
          outPtr[volAlphaIndex] = (T)0;
					for (int c = 0; c < numVolumeComponents; c++)
					{
            bool result(false);
						int volCompIndex = (currentPos[0]*byteIncVol[0])+(currentPos[1]*byteIncVol[1])+(currentPos[2]*byteIncVol[2])+c;
            for (int k = 0; k < NumHFElements; k++) // k is the index of the kernel being tried
						{
              switch (HFElements[k].type) {
              case FillHolesInVolumeElement::HFTYPE_GAUSSIAN:
                result = HFElements[k].applyGaussian(inVolPtr,accPtr,byteIncVol,byteIncAcc,c,outExt,wholeExtent,currentPos,outPtr[volCompIndex]);
							  break;
              case FillHolesInVolumeElement::HFTYPE_STICK:
                result = HFElements[k].applySticks(inVolPtr,accPtr,byteIncVol,byteIncAcc,c,outExt,wholeExtent,currentPos,outPtr[volCompIndex]);
							  break;
              }
							if (result) {
                outPtr[volAlphaIndex] = (T)OPAQUE_ALPHA;
								break;
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

//--------------------------------------------------------------------------------------
void vtkFillHolesInVolume::SetHFElement(int index, FillHolesInVolumeElement& element) {
  // universal
  HFElements[index].type = element.type;
  // gaussian
  HFElements[index].size = element.size;
  HFElements[index].stdev = element.stdev;
  HFElements[index].minRatio = element.minRatio;
  // sticks
  HFElements[index].stickLengthLimit = element.stickLengthLimit;
  switch (element.type) {
    case FillHolesInVolumeElement::HFTYPE_GAUSSIAN:
      HFElements[index].setupAsGaussian(element.size,element.stdev,element.minRatio);
      break;
    case FillHolesInVolumeElement::HFTYPE_STICK:
      HFElements[index].setupAsStick(element.stickLengthLimit);
      break;
  }
}

void vtkFillHolesInVolume::AllocateHFElements() {
	if (HFElements != NULL) {
		delete[] HFElements;
		HFElements = NULL;
	}
	HFElements = new FillHolesInVolumeElement[NumHFElements];
}

void vtkFillHolesInVolume::SetNumHFElements(int n) {
	NumHFElements = n;
}
//--------------------------------------------------------------------------------------

void vtkFillHolesInVolume::SetReconstructedVolume(vtkImageData *reconstructedVolume)
{
  SetInput(INPUT_PORT_RECONSTRUCTED_VOLUME, reconstructedVolume);
}

void vtkFillHolesInVolume::SetAccumulationBuffer(vtkImageData *accumulationBuffer)
{
  SetInput(INPUT_PORT_ACCUMULATION_BUFFER, accumulationBuffer);
}
