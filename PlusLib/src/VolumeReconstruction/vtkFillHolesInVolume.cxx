/*=========================================================================

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

#include <math.h>

vtkStandardNewMacro(vtkFillHolesInVolume);

struct FillHoleThreadFunctionInfoStruct
{
	vtkImageData* ReconstructedVolume;
	vtkImageData* Accumulator;
  int Compounding;
};


//----------------------------------------------------------------------------
// Construct an instance of vtkFillHolesInVolume fitler.
vtkFillHolesInVolume::vtkFillHolesInVolume()
{
  this->SetNumberOfInputPorts(1);
  this->SetNumberOfOutputPorts(1);
  this->Compounding=0;
}


//----------------------------------------------------------------------------
void vtkFillHolesInVolume::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
  /*
  os << indent << "HandleBoundaries: " << this->HandleBoundaries << "\n";
  os << indent << "Dimensionality: " << this->Dimensionality << "\n";
  */
}

//----------------------------------------------------------------------------
// This method is passed a region that holds the image extent of this filters
// input, and changes the region to hold the image extent of this filters
// output.
int vtkFillHolesInVolume::RequestInformation (
  vtkInformation* vtkNotUsed(request),
  vtkInformationVector** inputVector,
  vtkInformationVector* outputVector)
{  
  int extent[6];

  // get the info objects
  vtkInformation* outInfo = outputVector->GetInformationObject(0);
  vtkInformation* inInfo = inputVector[0]->GetInformationObject(0);

  // invalid setting, it has not been set, so default to whole Extent
  inInfo->Get(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT(), 
              extent);
  
  outInfo->Set(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT(), 
               extent, 6);

  return 1;
}


//----------------------------------------------------------------------------
// This method computes the input extent necessary to generate the output.
int vtkFillHolesInVolume::RequestUpdateExtent (
  vtkInformation* vtkNotUsed(request),
  vtkInformationVector** inputVector,
  vtkInformationVector* outputVector)
{
  int wholeExtent[6];

  // get the info objects
  vtkInformation* outInfo = outputVector->GetInformationObject(0);
  vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);

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
void vtkFillHolesInVolumeExecute(vtkFillHolesInVolume *self,
                                      vtkImageData *inVolData, T *inVolPtr,
                                      vtkImageData *accData, unsigned short *accPtr,      
                                      vtkImageData *outData, T *outPtr,
                                      int outExt[6], int id)
{
  /*
  int maxC, maxX, maxY, maxZ;
  vtkIdType inIncX, inIncY, inIncZ;
  vtkIdType outIncX, outIncY, outIncZ;
  unsigned long count = 0;
  unsigned long target;
  int axesNum;
  int *wholeExtent;
  vtkIdType *inIncs;
  double r[3], d, sum;
  int useZMin, useZMax, useYMin, useYMax, useXMin, useXMax;
  int *inExt = inData->GetExtent();

  // find the region to loop over
  maxC = outData->GetNumberOfScalarComponents();
  maxX = outExt[1] - outExt[0];
  maxY = outExt[3] - outExt[2]; 
  maxZ = outExt[5] - outExt[4];
  target = static_cast<unsigned long>((maxZ+1)*(maxY+1)/50.0);
  target++;

  // Get the dimensionality of the gradient.
  axesNum = self->GetDimensionality();
  
  // Get increments to march through data 
  inData->GetContinuousIncrements(outExt, inIncX, inIncY, inIncZ);
  outData->GetContinuousIncrements(outExt, outIncX, outIncY, outIncZ);

  // get some other info we need
  inIncs = inData->GetIncrements(); 
  wholeExtent = inData->GetExtent(); 

  // Move the starting pointer to the correct location.
  inPtr += (outExt[0]-inExt[0])*inIncs[0] +
           (outExt[2]-inExt[2])*inIncs[1] +
           (outExt[4]-inExt[4])*inIncs[2];

  // Loop through ouput pixels
  for (idxZ = 0; idxZ <= maxZ; idxZ++)
    {
    useZMin = ((idxZ + outExt[4]) <= wholeExtent[4]) ? 0 : -inIncs[2];
    useZMax = ((idxZ + outExt[4]) >= wholeExtent[5]) ? 0 : inIncs[2];
    for (idxY = 0; !self->AbortExecute && idxY <= maxY; idxY++)
      {
      if (!id) 
        {
        if (!(count%target))
          {
          self->UpdateProgress(count/(50.0*target));
          }
        count++;
        }
      useYMin = ((idxY + outExt[2]) <= wholeExtent[2]) ? 0 : -inIncs[1];
      useYMax = ((idxY + outExt[2]) >= wholeExtent[3]) ? 0 : inIncs[1];
      for (idxX = 0; idxX <= maxX; idxX++)
        {
        useXMin = ((idxX + outExt[0]) <= wholeExtent[0]) ? 0 : -inIncs[0];
        useXMax = ((idxX + outExt[0]) >= wholeExtent[1]) ? 0 : inIncs[0];
        for (idxC = 0; idxC < maxC; idxC++)
          {
          // do X axis
          d = static_cast<double>(inPtr[useXMin]);
          d -= static_cast<double>(inPtr[useXMax]);
          d *= r[0]; // multiply by the data spacing
          sum = d * d;
          // do y axis
          d = static_cast<double>(inPtr[useYMin]);
          d -= static_cast<double>(inPtr[useYMax]);
          d *= r[1]; // multiply by the data spacing
          sum += (d * d);
          if (axesNum == 3)
            {
            // do z axis
            d = static_cast<double>(inPtr[useZMin]);
            d -= static_cast<double>(inPtr[useZMax]);
            d *= r[2]; // multiply by the data spacing
            sum += (d * d);
            }
          *outPtr = static_cast<T>(sqrt(sum));
          outPtr++;
          inPtr++;
          }
        }
      outPtr += outIncY;
      inPtr += inIncY;
      }
    outPtr += outIncZ;
    inPtr += inIncZ;
    }

    */                                  
                                    


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
	
	int incX, incY, incZ;
	int accIncX, accIncY, accIncZ;
	int startX, endX, numscalars;
	int c;

	// clip the extent by 1 voxel width relative to whole extent
	int *outWholeExt = outData->GetWholeExtent();
	int extent[6];
	for (int a = 0; a < 3; a++)
	{
		extent[2*a] = outExt[2*a];
		if (extent[2*a] == outWholeExt[2*a])
		{
			extent[2*a]++;
		}
		extent[2*a+1] = outExt[2*a+1];
		if (extent[2*a+1] == outWholeExt[2*a+1])
		{
			extent[2*a+1]--;
		}
	}

	// get increments for output and for accumulation buffer
	outData->GetIncrements(incX, incY, incZ);
	accIncX = 1;
	accIncY = incY/incX;
	accIncZ = incZ/incX;
	// number of components not including the alpha channel
	numscalars = outData->GetNumberOfScalarComponents() - 1;

	T *alphaPtr = outPtr + numscalars;
	T *outPtrZ, *outPtrY, *outPtrX;
	unsigned short *accPtrZ, *accPtrY, *accPtrX;

	// go through all voxels except the edge voxels
	for (int idZ = extent[4]; idZ <= extent[5]; idZ++)
	{
		outPtrZ = outPtr + (idZ - outExt[4])*incZ;
		accPtrZ = accPtr + (idZ - outExt[4])*accIncZ;

		for (int idY = extent[2]; idY <= extent[3]; idY++)
		{
			outPtrY = outPtrZ + (idY - outExt[2])*incY;
			accPtrY = accPtrZ + (idY - outExt[2])*accIncY;

			// find entry point
			alphaPtr = outPtrY + numscalars;
			for (startX = outExt[0]; startX <= outExt[1]; startX++)
			{
				// check the point on the row as well as the 4-connected voxels
				// break when alpha component is nonzero
				if ( (*alphaPtr != 0) |
					(*(alphaPtr-incY) != 0) | (*(alphaPtr+incY) != 0) |
					(*(alphaPtr-incZ) != 0) | (*(alphaPtr+incZ) != 0))
				{
					break;
				}
				alphaPtr += incX;
			}

			if (startX > outExt[1])
			{ // the whole row is empty, do nothing
				continue;
			}

			// find exit point
			alphaPtr = outPtrY + (outExt[1]-outExt[0])*incX + numscalars;
			for (endX = outExt[1]; endX >= outExt[0]; endX--)
			{
				// check the point on the row as well as the 4-connected voxels 
				if ((*alphaPtr!=0)|
					(*(alphaPtr-incY)!=0) | (*(alphaPtr+incY)!=0) |
					(*(alphaPtr-incZ)!=0) | (*(alphaPtr+incZ)!=0))
				{
					break;
				}
				alphaPtr -= incX;
			}

			// go through the row, skip first and last voxel in row
			if (startX == outWholeExt[0])
			{
				startX++;
			}
			if (endX == outWholeExt[1])
			{
				endX--;
			}
			outPtrX = outPtrY + (startX - outExt[0])*incX;
			accPtrX = accPtrY + (startX - outExt[0])*accIncX;

			for (int idX = startX; idX <= endX; idX++)
			{
				// only do this for voxels that haven't been hit
				if (outPtrX[numscalars] == 0)
				{ 
					double sum[32];
					for (c = 0; c < numscalars; c++) 
					{
						sum[c] = 0;
					}
					double asum = 0; 
					int n = 0;
					int nmin = 14; // half of the connected voxels plus one
					T *blockPtr;
					unsigned short *accBlockPtr;

					// for accumulation buffer
					// sum the pixel values for the 3x3x3 block
					// NOTE turned off for now
					if (0) // (accPtr)
					{ // use accumulation buffer to do weighted average
						for (int k = -accIncZ; k <= accIncZ; k += accIncZ)
						{
							for (int j = -accIncY; j <= accIncY; j += accIncY)
							{
								for (int i = -accIncX; i <= accIncX; i += accIncX)
								{
									int inc = j + k + i;
									blockPtr = outPtrX + inc*incX;
									accBlockPtr = accPtrX + inc;
									if (blockPtr[numscalars] == 255)
									{
										n++;
										for (c = 0; c < numscalars; c++)
										{ // use accumulation buffer as weight
											sum[c] += blockPtr[c]*(*accBlockPtr);
										}
										asum += *accBlockPtr;
									}
								}
							}
						}

						// if less than half the neighbors have data, use larger block
						if (n <= nmin && idX != startX && idX != endX &&
							idX - outWholeExt[0] > 2 && outWholeExt[1] - idX > 2 &&
							idY - outWholeExt[2] > 2 && outWholeExt[3] - idY > 2 &&
							idZ - outWholeExt[4] > 2 && outWholeExt[5] - idZ > 2)
						{
							// weigh inner block by a factor of four (multiply three,
							// plus we will be counting it again as part of the 5x5x5
							// block)
							asum *= 3;
							for (c = 0; c < numscalars; c++) 
							{
								sum[c]*= 3;
							}        
							nmin = 63;
							n = 0;
							for (int k = -accIncZ*2; k <= accIncZ*2; k += accIncZ)
							{
								for (int j = -accIncY*2; j <= accIncY*2; j += accIncY)
								{
									for (int i = -accIncX*2; i <= accIncX*2; i += accIncX)
									{
										int inc = j + k + i;
										blockPtr = outPtrX + inc*incX;
										accBlockPtr = accPtrX + inc;
										// use accumulation buffer as weight
										if (blockPtr[numscalars] == 255)
										{ 
											n++;
											for (c = 0; c < numscalars; c++)
											{
												sum[c] += blockPtr[c]*(*accBlockPtr);
											}
											asum += *accBlockPtr; 
										}
									}
								}
							}
						}
					}
					// END TURNED OFF FOR NOW

					// no accumulation buffer
					else 
					{
						for (int k = -incZ; k <= incZ; k += incZ)
						{
							for (int j = -incY; j <= incY; j += incY)
							{
								for (int i = -incX; i <= incX; i += incX)
								{
									blockPtr = outPtrX + j + k + i;
									if (blockPtr[numscalars] == 255)
									{
										n++;
										for (int c = 0; c < numscalars; c++)
										{
											sum[c] += blockPtr[c];
										}
									}
								}
							}
						}
						asum = n;

						// if less than half the neighbors have data, use larger block,
						// and count inner 3x3 block again to weight it by 2
						if (n <= nmin && idX != startX && idX != endX &&
							idX - outWholeExt[0] > 2 && outWholeExt[1] - idX > 2 &&
							idY - outWholeExt[2] > 2 && outWholeExt[3] - idY > 2 &&
							idZ - outWholeExt[4] > 2 && outWholeExt[5] - idZ > 2)
						{ 
							// weigh inner block by a factor of four (multiply three,
							// plus we will be counting it again as part of the 5x5x5
							// block)
							asum *= 3;
							for (c = 0; c < numscalars; c++) 
							{
								sum[c]*= 3;
							}
							nmin = 63;
							n = 0;
							for (int k = -incZ*2; k <= incZ*2; k += incZ)
							{
								for (int j = -incY*2; j <= incY*2; j += incY)
								{
									for (int i = -incX*2; i <= incX*2; i += incX)
									{
										blockPtr = outPtrX + j + k + i;
										if (blockPtr[numscalars] == 255)
										{
											n++;
											for (int c = 0; c < numscalars; c++)
											{
												sum[c] += blockPtr[c];
											}
										}
									}
								}
							}
							asum += n;
						}
					}

					// if more than half of neighboring voxels are occupied, then fill
					if (n >= nmin)
					{
						for (int c = 0; c < numscalars; c++)
						{
							vtkUltraRound(sum[c]/asum, outPtrX[c]);
						}
						// set alpha to 1 now, change to 255 later
						outPtrX[numscalars] = 1;
					}
				}
				outPtrX += incX;
			}
		}
	}

	// change alpha value '1' to value '255'
	alphaPtr = outPtr + numscalars;
	// go through all voxels this time
	for (int idZ = outExt[4]; idZ <= outExt[5]; idZ++)
	{
		for (int idY = outExt[2]; idY <= outExt[3]; idY++)
		{
			for (int idX = outExt[0]; idX <= outExt[1]; idX++)
			{
				// convert '1' to 255
				if (*alphaPtr == 1)
				{
					*alphaPtr = 255;
				}
				alphaPtr += incX;
			}
			// add the continuous increment
			alphaPtr += (incY - (outExt[1]-outExt[0]+1)*incX);
		}
		// add the continuous increment
		alphaPtr += (incZ - (outExt[3]-outExt[2]+1)*incY);
	}



}


//----------------------------------------------------------------------------
// This method contains a switch statement that calls the correct
// templated function for the input data type.  The output data
// must match input type.
void vtkFillHolesInVolume::ThreadedRequestData(vtkInformation *request, 
    vtkInformationVector **inputVector, 
    vtkInformationVector *outputVector,
    vtkImageData ***inData, 
    vtkImageData **outData,
    int outExt[6], int threadId)
{
  vtkImageData* outVolData = outData[0];  
  void *outVolPtr = outVolData->GetScalarPointerForExtent(outExt);  

  vtkImageData* inVolData=inData[0][0];
  void *inVolPtr = inVolData->GetScalarPointer();

  vtkImageData* inAccData=inData[0][1];
  void *inAccPtr = inAccData->GetScalarPointer();

  // this filter expects that input is the same type as output.
  if (inVolData->GetScalarType() != outVolData->GetScalarType())
    {
    vtkErrorMacro(<< "Execute: input data type, " 
                  << inVolData->GetScalarType()
                  << ", must match out ScalarType " 
                  << outVolData->GetScalarType());
    return;
    }
  
  switch (inVolData->GetScalarType())
    {
    vtkTemplateMacro(
      vtkFillHolesInVolumeExecute(this, 
                                       inVolData, static_cast<VTK_TT *>(inVolPtr),
                                       inAccData, static_cast<unsigned short *>(inAccPtr),
                                       outVolData,
                                       static_cast<VTK_TT *>(outVolPtr), outExt,
                                       threadId));
    default:
      vtkErrorMacro(<< "Execute: Unknown ScalarType");
      return;
    }
}



/*
//----------------------------------------------------------------------------
// Does the actual hole filling
template <class T>
static void vtkPasteSliceIntoVolumeFillHolesInOutput(
													vtkImageData *outData,
													T *outPtr,
													unsigned short *accPtr,
													int outExt[6])
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
	int idX, idY, idZ;
	int incX, incY, incZ;
	int accIncX, accIncY, accIncZ;
	int startX, endX, numscalars;
	int c;

	// clip the extent by 1 voxel width relative to whole extent
	int *outWholeExt = outData->GetWholeExtent();
	int extent[6];
	for (int a = 0; a < 3; a++)
	{
		extent[2*a] = outExt[2*a];
		if (extent[2*a] == outWholeExt[2*a])
		{
			extent[2*a]++;
		}
		extent[2*a+1] = outExt[2*a+1];
		if (extent[2*a+1] == outWholeExt[2*a+1])
		{
			extent[2*a+1]--;
		}
	}

	// get increments for output and for accumulation buffer
	outData->GetIncrements(incX, incY, incZ);
	accIncX = 1;
	accIncY = incY/incX;
	accIncZ = incZ/incX;
	// number of components not including the alpha channel
	numscalars = outData->GetNumberOfScalarComponents() - 1;

	T *alphaPtr = outPtr + numscalars;
	T *outPtrZ, *outPtrY, *outPtrX;
	unsigned short *accPtrZ, *accPtrY, *accPtrX;

	// go through all voxels except the edge voxels
	for (idZ = extent[4]; idZ <= extent[5]; idZ++)
	{
		outPtrZ = outPtr + (idZ - outExt[4])*incZ;
		accPtrZ = accPtr + (idZ - outExt[4])*accIncZ;

		for (idY = extent[2]; idY <= extent[3]; idY++)
		{
			outPtrY = outPtrZ + (idY - outExt[2])*incY;
			accPtrY = accPtrZ + (idY - outExt[2])*accIncY;

			// find entry point
			alphaPtr = outPtrY + numscalars;
			for (startX = outExt[0]; startX <= outExt[1]; startX++)
			{
				// check the point on the row as well as the 4-connected voxels
				// break when alpha component is nonzero
				if (*alphaPtr |
					*(alphaPtr-incY) | *(alphaPtr+incY) |
					*(alphaPtr-incZ) | *(alphaPtr+incZ))
				{
					break;
				}
				alphaPtr += incX;
			}

			if (startX > outExt[1])
			{ // the whole row is empty, do nothing
				continue;
			}

			// find exit point
			alphaPtr = outPtrY + (outExt[1]-outExt[0])*incX + numscalars;
			for (endX = outExt[1]; endX >= outExt[0]; endX--)
			{
				// check the point on the row as well as the 4-connected voxels 
				if (*alphaPtr |
					*(alphaPtr-incY) | *(alphaPtr+incY) |
					*(alphaPtr-incZ) | *(alphaPtr+incZ))
				{
					break;
				}
				alphaPtr -= incX;
			}

			// go through the row, skip first and last voxel in row
			if (startX == outWholeExt[0])
			{
				startX++;
			}
			if (endX == outWholeExt[1])
			{
				endX--;
			}
			outPtrX = outPtrY + (startX - outExt[0])*incX;
			accPtrX = accPtrY + (startX - outExt[0])*accIncX;

			for (idX = startX; idX <= endX; idX++)
			{
				// only do this for voxels that haven't been hit
				if (outPtrX[numscalars] == 0)
				{ 
					double sum[32];
					for (c = 0; c < numscalars; c++) 
					{
						sum[c] = 0;
					}
					double asum = 0; 
					int n = 0;
					int nmin = 14; // half of the connected voxels plus one
					T *blockPtr;
					unsigned short *accBlockPtr;

					// for accumulation buffer
					// sum the pixel values for the 3x3x3 block
					// NOTE turned off for now
					if (0) // (accPtr)
					{ // use accumulation buffer to do weighted average
						for (int k = -accIncZ; k <= accIncZ; k += accIncZ)
						{
							for (int j = -accIncY; j <= accIncY; j += accIncY)
							{
								for (int i = -accIncX; i <= accIncX; i += accIncX)
								{
									int inc = j + k + i;
									blockPtr = outPtrX + inc*incX;
									accBlockPtr = accPtrX + inc;
									if (blockPtr[numscalars] == 255)
									{
										n++;
										for (c = 0; c < numscalars; c++)
										{ // use accumulation buffer as weight
											sum[c] += blockPtr[c]*(*accBlockPtr);
										}
										asum += *accBlockPtr;
									}
								}
							}
						}

						// if less than half the neighbors have data, use larger block
						if (n <= nmin && idX != startX && idX != endX &&
							idX - outWholeExt[0] > 2 && outWholeExt[1] - idX > 2 &&
							idY - outWholeExt[2] > 2 && outWholeExt[3] - idY > 2 &&
							idZ - outWholeExt[4] > 2 && outWholeExt[5] - idZ > 2)
						{
							// weigh inner block by a factor of four (multiply three,
							// plus we will be counting it again as part of the 5x5x5
							// block)
							asum *= 3;
							for (c = 0; c < numscalars; c++) 
							{
								sum[c]*= 3;
							}        
							nmin = 63;
							n = 0;
							for (int k = -accIncZ*2; k <= accIncZ*2; k += accIncZ)
							{
								for (int j = -accIncY*2; j <= accIncY*2; j += accIncY)
								{
									for (int i = -accIncX*2; i <= accIncX*2; i += accIncX)
									{
										int inc = j + k + i;
										blockPtr = outPtrX + inc*incX;
										accBlockPtr = accPtrX + inc;
										// use accumulation buffer as weight
										if (blockPtr[numscalars] == 255)
										{ 
											n++;
											for (c = 0; c < numscalars; c++)
											{
												sum[c] += blockPtr[c]*(*accBlockPtr);
											}
											asum += *accBlockPtr; 
										}
									}
								}
							}
						}
					}
					// END TURNED OFF FOR NOW

					// no accumulation buffer
					else 
					{
						for (int k = -incZ; k <= incZ; k += incZ)
						{
							for (int j = -incY; j <= incY; j += incY)
							{
								for (int i = -incX; i <= incX; i += incX)
								{
									blockPtr = outPtrX + j + k + i;
									if (blockPtr[numscalars] == 255)
									{
										n++;
										for (int c = 0; c < numscalars; c++)
										{
											sum[c] += blockPtr[c];
										}
									}
								}
							}
						}
						asum = n;

						// if less than half the neighbors have data, use larger block,
						// and count inner 3x3 block again to weight it by 2
						if (n <= nmin && idX != startX && idX != endX &&
							idX - outWholeExt[0] > 2 && outWholeExt[1] - idX > 2 &&
							idY - outWholeExt[2] > 2 && outWholeExt[3] - idY > 2 &&
							idZ - outWholeExt[4] > 2 && outWholeExt[5] - idZ > 2)
						{ 
							// weigh inner block by a factor of four (multiply three,
							// plus we will be counting it again as part of the 5x5x5
							// block)
							asum *= 3;
							for (c = 0; c < numscalars; c++) 
							{
								sum[c]*= 3;
							}
							nmin = 63;
							n = 0;
							for (int k = -incZ*2; k <= incZ*2; k += incZ)
							{
								for (int j = -incY*2; j <= incY*2; j += incY)
								{
									for (int i = -incX*2; i <= incX*2; i += incX)
									{
										blockPtr = outPtrX + j + k + i;
										if (blockPtr[numscalars] == 255)
										{
											n++;
											for (int c = 0; c < numscalars; c++)
											{
												sum[c] += blockPtr[c];
											}
										}
									}
								}
							}
							asum += n;
						}
					}

					// if more than half of neighboring voxels are occupied, then fill
					if (n >= nmin)
					{
						for (int c = 0; c < numscalars; c++)
						{
							vtkUltraRound(sum[c]/asum, outPtrX[c]);
						}
						// set alpha to 1 now, change to 255 later
						outPtrX[numscalars] = 1;
					}
				}
				outPtrX += incX;
			}
		}
	}

	// change alpha value '1' to value '255'
	alphaPtr = outPtr + numscalars;
	// go through all voxels this time
	for (idZ = outExt[4]; idZ <= outExt[5]; idZ++)
	{
		for (idY = outExt[2]; idY <= outExt[3]; idY++)
		{
			for (idX = outExt[0]; idX <= outExt[1]; idX++)
			{
				// convert '1' to 255
				if (*alphaPtr == 1)
				{
					*alphaPtr = 255;
				}
				alphaPtr += incX;
			}
			// add the continuous increment
			alphaPtr += (incY - (outExt[1]-outExt[0]+1)*incX);
		}
		// add the continuous increment
		alphaPtr += (incZ - (outExt[3]-outExt[2]+1)*incY);
	}
}
*/

/*
//----------------------------------------------------------------------------
// This mess is really a simple function. All it does is call
// the ThreadedExecute method after setting the correct
// extent for this thread.  Its just a pain to calculate
// the correct extent.
VTK_THREAD_RETURN_TYPE vtkPasteSliceIntoVolume::FillHoleThreadFunction( void *arg )
{	
  vtkMultiThreader::ThreadInfo* threadInfo = static_cast<vtkMultiThreader::ThreadInfo *>(arg);
	FillHoleThreadFunctionInfoStruct *str = static_cast<FillHoleThreadFunctionInfoStruct *> (static_cast<vtkMultiThreader::ThreadInfo *>(arg)->UserData);

  // Compute what extent of the input image will be processed by this thread
	int threadId = threadInfo->ThreadID;
	int threadCount = threadInfo->NumberOfThreads;
  int outputExtent[6];
  str->ReconstructedVolume->GetExtent(outputExtent);
  int outputExtentForCurrentThread[6];
	int totalUsedThreads = vtkPasteSliceIntoVolume::SplitSliceExtent(outputExtentForCurrentThread, outputExtent, threadId, threadCount);

	// if we can use this thread, then call FillHoleThreadFunction
	if (threadId >= totalUsedThreads)
	{
    //   otherwise don't use this thread. Sometimes the threads dont
	  //   break up very well and it is just as efficient to leave a 
	  //   few threads idle.
    return VTK_THREAD_RETURN_VALUE;		
	}
  
  vtkImageData *accData=str->Accumulator;

  void *outPtr = str->ReconstructedVolume->GetScalarPointerForExtent(outputExtentForCurrentThread);
	void *accPtr = NULL;
	if (str->Compounding)
	{
		accPtr = accData->GetScalarPointerForExtent(outputExtentForCurrentThread);
	}

	switch (str->ReconstructedVolume->GetScalarType())
	{
	case VTK_SHORT:
		vtkPasteSliceIntoVolumeFillHolesInOutput(
			str->ReconstructedVolume, (short *)(outPtr), 
			(unsigned short *)(accPtr), outputExtentForCurrentThread);
		break;
	case VTK_UNSIGNED_SHORT:
		vtkPasteSliceIntoVolumeFillHolesInOutput(
			str->ReconstructedVolume, (unsigned short *)(outPtr),
			(unsigned short *)(accPtr), outputExtentForCurrentThread);
		break;
	case VTK_UNSIGNED_CHAR:
		vtkPasteSliceIntoVolumeFillHolesInOutput(
			str->ReconstructedVolume,(unsigned char *)(outPtr),
			(unsigned short *)(accPtr), outputExtentForCurrentThread); 
		break;
	default:
		LOG_ERROR("FillHolesInOutput: Unknown input ScalarType");
	}

	return VTK_THREAD_RETURN_VALUE;
}
*/

/*
//----------------------------------------------------------------------------
// Fills holes in the output by using the weighted average of the surrounding
// voxels (see David Gobbi's thesis)
// Basically, just calls MultiThreadFill()
void vtkPasteSliceIntoVolume::FillHolesInOutput()
{
	FillHoleThreadFunctionInfoStruct str;
  str.ReconstructedVolume = this->ReconstructedVolume;
  str.Accumulator = this->AccumulationBuffer;
  str.Compounding = this->Compounding;

	// run FillHoleThreadFunction
  if (this->NumberOfThreads>0)
  {
    this->Threader->SetNumberOfThreads(this->NumberOfThreads);
  }
  this->Threader->SetSingleMethod(FillHoleThreadFunction, &str);
	this->Threader->SingleMethodExecute();

	this->Modified(); 
}
*/


void vtkFillHolesInVolume::SetReconstructedVolume(vtkImageData *reconstructedVolume)
{
  SetInput(0, reconstructedVolume);
}

void vtkFillHolesInVolume::SetAccumulationBuffer(vtkImageData *accumulationBuffer)
{
  SetInput(1, accumulationBuffer);
}
