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

#include "vtkPasteSliceIntoVolume.h"
#include "vtkPasteSliceIntoVolumeHelper.h"
#include "vtkObjectFactory.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkMath.h"
#include "vtkCriticalSection.h"
#include "vtkMutexLock.h"
#include "vtkSignalBox.h"
#include "vtkXMLUtilities.h"
#include "vtkXMLDataElement.h"
#include "vtkBMPWriter.h"
#include "vtkTimestampedCircularBuffer.h"
#include "vtkVideoBuffer.h"
#include "vtkIndent.h"

vtkCxxRevisionMacro(vtkPasteSliceIntoVolume, "$Revisions: 1.0 $");
vtkStandardNewMacro(vtkPasteSliceIntoVolume);

struct InsertSliceThreadFunctionInfoStruct
{
  vtkImageData* InputFrameImage;
  vtkMatrix4x4* TransformImageToReference;
	vtkImageData* OutputVolume;
	vtkImageData* Accumulator;
  vtkPasteSliceIntoVolume::OptimizationType Optimization;
  int Compounding;
  vtkPasteSliceIntoVolume::InterpolationType InterpolationMode;

  double ClipRectangleOrigin[2];
  double ClipRectangleSize[2];
  double FanAngles[2];
  double FanOrigin[2];
  double FanDepth;
};

struct FillHoleThreadFunctionInfoStruct
{
	vtkImageData* ReconstructedVolume;
	vtkImageData* Accumulator;
  int Compounding;
};


//----------------------------------------------------------------------------
vtkPasteSliceIntoVolume::vtkPasteSliceIntoVolume()
{
  this->ReconstructedVolume=vtkImageData::New();
  this->AccumulationBuffer=vtkImageData::New();
  this->Threader=vtkMultiThreader::New();

  this->OutputOrigin[0] = 0;
	this->OutputOrigin[1] = 0;
	this->OutputOrigin[2] = 0;

	this->OutputSpacing[0] = 1.0;
	this->OutputSpacing[1] = 1.0;
	this->OutputSpacing[2] = 1.0;

	this->OutputExtent[0] = 0;
	this->OutputExtent[1] = 255;
	this->OutputExtent[2] = 0;
	this->OutputExtent[3] = 255;
	this->OutputExtent[4] = 0;
	this->OutputExtent[5] = 255;

	this->ClipRectangleOrigin[0] = 0;
	this->ClipRectangleOrigin[1] = 0;
	this->ClipRectangleSize[0] = 0;
	this->ClipRectangleSize[1] = 0;

	this->FanAngles[0] = 0.0;
	this->FanAngles[1] = 0.0;
	this->FanOrigin[0] = 0.0;
	this->FanOrigin[1] = 0.0;
	this->FanDepth = 1.0;

	// reconstruction options
	this->InterpolationMode = NEAREST_NEIGHBOR_INTERPOLATION;
	this->Optimization = FULL_OPTIMIZATION;
  this->Compounding = 0;

  this->NumberOfThreads=0; // 0 means not set, the default number of threads will be used
}

//----------------------------------------------------------------------------
vtkPasteSliceIntoVolume::~vtkPasteSliceIntoVolume()
{  
  if (this->ReconstructedVolume)
  {
    this->ReconstructedVolume->Delete();
    this->ReconstructedVolume=NULL;
  }
  if (this->AccumulationBuffer)
  {
    this->AccumulationBuffer->Delete();
    this->AccumulationBuffer=NULL;
  }
  if (this->Threader)
  {
    this->Threader->Delete();
    this->Threader=NULL;
  }
}

//----------------------------------------------------------------------------
void vtkPasteSliceIntoVolume::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
  if (this->ReconstructedVolume)
	{
		os << indent << "ReconstructedVolume:\n";
		this->ReconstructedVolume->PrintSelf(os,indent.GetNextIndent());
	}
	if (this->AccumulationBuffer)
	{
		os << indent << "AccumulationBuffer:\n";
		this->AccumulationBuffer->PrintSelf(os,indent.GetNextIndent());
	}
	os << indent << "OutputOrigin: " << this->OutputOrigin[0] << " " <<
		this->OutputOrigin[1] << " " << this->OutputOrigin[2] << "\n";
	os << indent << "OutputSpacing: " << this->OutputSpacing[0] << " " <<
		this->OutputSpacing[1] << " " << this->OutputSpacing[2] << "\n";
	os << indent << "OutputExtent: " << this->OutputExtent[0] << " " <<
		this->OutputExtent[1] << " " << this->OutputExtent[2] << " " <<
		this->OutputExtent[3] << " " << this->OutputExtent[4] << " " <<
		this->OutputExtent[5] << "\n";
	os << indent << "ClipRectangleOrigin: " << this->ClipRectangleOrigin[0] << " " <<
		this->ClipRectangleOrigin[1] << "\n";
  os << indent << "ClipRectangleSize: " << this->ClipRectangleSize[0] << " " <<
		this->ClipRectangleSize[1] << "\n";
	os << indent << "FanAngles: " << this->FanAngles[0] << " " <<
		this->FanAngles[1] << "\n";
	os << indent << "FanOrigin: " << this->FanOrigin[0] << " " <<
		this->FanOrigin[1] << "\n";
	os << indent << "FanDepth: " << this->FanDepth << "\n";
  os << indent << "InterpolationMode: " << this->GetInterpolationModeAsString(this->InterpolationMode) << "\n";
	os << indent << "Optimization: " << this->GetOptimizationModeAsString(this->Optimization) << "\n";
  os << indent << "Compounding: " << (this->Compounding ? "On\n":"Off\n");
  os << indent << "NumberOfThreads: ";
  if (this->NumberOfThreads>0)
  {
    os << this->NumberOfThreads << "\n";
  }
  else
  {
    os << "default\n";
  }
}

//****************************************************************************
// BASICS FOR 4D RECONSTRUCTION
//****************************************************************************

//----------------------------------------------------------------------------
// Get the reconstruction volume (for port 0)
// Important that this be for port 0 and not for phase 0, because
// GetIndexMatrixHelper in vtkPasteSliceIntoVolume uses GetOutput() to figure
// out output spacing - want to ensure that an output volume exists, even
// if we are selecting which phases to reconstruct and the output for phase 0
// is turned off
vtkImageData *vtkPasteSliceIntoVolume::GetReconstructedVolume()
{
  return this->ReconstructedVolume;
}

//----------------------------------------------------------------------------
// Get the accumulation buffer (for port 0)
// Will be NULL if we are not compounding
vtkImageData *vtkPasteSliceIntoVolume::GetAccumulationBuffer()
{
  return this->AccumulationBuffer;
}

//----------------------------------------------------------------------------
// Clear the output volume and the accumulation buffer
// (basically just calls InternalInternalClearOutput)
PlusStatus vtkPasteSliceIntoVolume::ResetOutput()
{   
  // Allocate memory for accumulation buffer and set all pixels to 0
  // Start with this buffer because if no compunding is needed then we release memory before allocating memory for the reconstructed image.

  vtkImageData* accData = this->GetAccumulationBuffer();
	if (accData==NULL)
  {
    LOG_ERROR("Accumulation buffer object is not created");
    return PLUS_FAIL;
  }

  int accExtent[6]={0}; // by default set the accumulation buffer to 0 size
  if (this->Compounding)
  {
    // we do compunding, so we need to have an accumulation buffer with the same size as the output image
    for (int i=0; i<6; i++)
    {
      accExtent[i]=this->OutputExtent[i];
    }
  }
  accData->SetExtent(accExtent);
  accData->SetOrigin(this->OutputOrigin);
  accData->SetSpacing(this->OutputSpacing);
  accData->SetScalarType(VTK_UNSIGNED_INT);
  accData->SetNumberOfScalarComponents(1);
  accData->AllocateScalars();
  void *accPtr = accData->GetScalarPointerForExtent(accExtent);
  if (accPtr==NULL)
  {
    LOG_ERROR("Cannot allocate memory for accumulation image extent: "<< accExtent[1]-accExtent[0] <<"x"<< accExtent[3]-accExtent[2] <<" x "<< accExtent[5]-accExtent[4]);
  }
  else
  {
    memset(accPtr,0,((accExtent[1]-accExtent[0]+1)*
      (accExtent[3]-accExtent[2]+1)*
      (accExtent[5]-accExtent[4]+1)*
      accData->GetScalarSize()*accData->GetNumberOfScalarComponents()));
  }

	// Allocate memory for the reconstructed image and set all pixels to 0

  vtkImageData* outData = this->ReconstructedVolume;
	if (outData==NULL)
  {
    LOG_ERROR("Output image object is not created");
    return PLUS_FAIL;
  }
  
  int *outExtent=this->OutputExtent;
  outData->SetExtent(outExtent);
  outData->SetOrigin(this->OutputOrigin);
  outData->SetSpacing(this->OutputSpacing);
	outData->SetScalarType(VTK_UNSIGNED_CHAR);
  outData->SetNumberOfScalarComponents(2); // first component: image intensity; second component: if the pixel was set (0 = not set (hole), 1 = set)
  outData->AllocateScalars();
  void *outPtr = outData->GetScalarPointerForExtent(outExtent);
  if (outPtr==NULL)
  {
    LOG_ERROR("Cannot allocate memory for output image extent: "<< outExtent[1]-outExtent[0] <<"x"<< outExtent[3]-outExtent[2] <<" x "<< outExtent[5]-outExtent[4]);
    return PLUS_FAIL;
  }
  else
  {
    memset(outPtr,0,((outExtent[1]-outExtent[0]+1)*
      (outExtent[3]-outExtent[2]+1)*
      (outExtent[5]-outExtent[4]+1)*
      outData->GetScalarSize()*outData->GetNumberOfScalarComponents()));
  }
  
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
// Set compounding setting
void vtkPasteSliceIntoVolume::SetCompounding(int compound)
{
	this->Compounding = compound;
  ResetOutput();
}

//****************************************************************************
// RECONSTRUCTION - OPTIMIZED
//****************************************************************************

//----------------------------------------------------------------------------
// Does the actual work of optimally inserting a slice, with optimization
// Basically, just calls Multithread()
PlusStatus vtkPasteSliceIntoVolume::InsertSlice(vtkImageData *image, vtkMatrix4x4* transformImageToReference)
{
	InsertSliceThreadFunctionInfoStruct str;
	str.InputFrameImage = image;
  str.TransformImageToReference = transformImageToReference;
  str.OutputVolume = this->ReconstructedVolume;
  str.Accumulator = this->AccumulationBuffer;
  str.Compounding = this->Compounding;
  str.InterpolationMode = this->InterpolationMode;
  str.Optimization = this->Optimization;
  str.ClipRectangleOrigin[0]=this->ClipRectangleOrigin[0];
  str.ClipRectangleOrigin[1]=this->ClipRectangleOrigin[1];
  str.ClipRectangleSize[0]=this->ClipRectangleSize[0];
  str.ClipRectangleSize[1]=this->ClipRectangleSize[1];
  str.FanAngles[0]=this->FanAngles[0];
  str.FanAngles[1]=this->FanAngles[1];
  str.FanOrigin[0]=this->FanOrigin[0];
  str.FanOrigin[1]=this->FanOrigin[1];
  str.FanDepth=this->FanDepth;

  if (this->NumberOfThreads>0)
  {
	  this->Threader->SetNumberOfThreads(this->NumberOfThreads);
  }
	this->Threader->SetSingleMethod(InsertSliceThreadFunction, &str);
	this->Threader->SingleMethodExecute();

	this->Modified();
  
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
VTK_THREAD_RETURN_TYPE vtkPasteSliceIntoVolume::InsertSliceThreadFunction( void *arg )
{
  vtkMultiThreader::ThreadInfo* threadInfo = static_cast<vtkMultiThreader::ThreadInfo *>(arg);
 	InsertSliceThreadFunctionInfoStruct *str = static_cast<InsertSliceThreadFunctionInfoStruct*>(threadInfo->UserData);

  // Compute what extent of the input image will be processed by this thread
	int threadId = threadInfo->ThreadID;
	int threadCount = threadInfo->NumberOfThreads;
  int inputFrameExtent[6];
  str->InputFrameImage->GetExtent(inputFrameExtent);
  int inputFrameExtentForCurrentThread[6];
	int totalUsedThreads = vtkPasteSliceIntoVolume::SplitSliceExtent(inputFrameExtentForCurrentThread, inputFrameExtent, threadId, threadCount);

	if (threadId >= totalUsedThreads)
	{
    //   don't use this thread. Sometimes the threads dont
	  //   break up very well and it is just as efficient to leave a 
	  //   few threads idle.
    return VTK_THREAD_RETURN_VALUE;
	}

	// this filter expects that input is the same type as output.
  if (str->InputFrameImage->GetScalarType() != str->OutputVolume->GetScalarType())
	{
		LOG_ERROR("OptimizedInsertSlice: input ScalarType (" << str->InputFrameImage->GetScalarType()<<") "
			<< " must match out ScalarType ("<<str->OutputVolume->GetScalarType()<<")");
		return VTK_THREAD_RETURN_VALUE;
	}

  // Get input frame extent and pointer
  vtkImageData *inData=str->InputFrameImage;  
	void *inPtr = inData->GetScalarPointerForExtent(inputFrameExtentForCurrentThread);
  // Get output volume extent and pointer
  vtkImageData *outData=str->OutputVolume;
  int *outExt = outData->GetExtent();
	void *outPtr = outData->GetScalarPointerForExtent(outExt);

  void *accPtr = NULL; 
	if (str->Compounding)
	{
		accPtr = str->Accumulator->GetScalarPointerForExtent(outExt);
	}

  /*

  // Transform chain:
  // VolumePixToImagePix = 
  //  = ImagePixFromVolumePix
  //  = ImagePixFromImage * ImageFromRef * RefFromVolumePix

  vtkSmartPointer<vtkTransform> tImagePixFromImage = vtkSmartPointer<vtkTransform>::New();
  tImagePixFromImage->Scale(1.0/str->InputFrameImage->GetSpacing()[0],
    1.0/str->InputFrameImage->GetSpacing()[1],
    1.0/str->InputFrameImage->GetSpacing()[2]);

  vtkSmartPointer<vtkTransform> tImageFromRef = vtkSmartPointer<vtkTransform>::New();
  tImageFromRef->SetMatrix(str->TransformImageToReference);
  tImageFromRef->Inverse();

  vtkSmartPointer<vtkTransform> tRefFromVolumePix = vtkSmartPointer<vtkTransform>::New();
  tRefFromVolumePix->Translate(str->OutputVolume->GetOrigin());
  tRefFromVolumePix->Scale(str->OutputVolume->GetSpacing());

  vtkSmartPointer<vtkTransform> tVolumePixToImagePix = vtkSmartPointer<vtkTransform>::New();
  tVolumePixToImagePix->Concatenate(tImagePixFromImage);
  tVolumePixToImagePix->Concatenate(tImageFromRef);
  tVolumePixToImagePix->Concatenate(tRefFromVolumePix);

  vtkSmartPointer<vtkMatrix4x4> mVolumePixToImagePix = vtkSmartPointer<vtkMatrix4x4>::New();
  tVolumePixToImagePix->GetMatrix(mVolumePixToImagePix);

  mVolumePixToImagePix->Invert();

  */

  // Transform chain:
  // ImagePixToVolumePix = 
  //  = VolumePixFromImagePix
  //  = VolumePixFromRef * RefFromImage * ImageFromImagePix 

  vtkSmartPointer<vtkTransform> tVolumePixFromRef = vtkSmartPointer<vtkTransform>::New();
  tVolumePixFromRef->Translate(str->OutputVolume->GetOrigin());
  tVolumePixFromRef->Scale(str->OutputVolume->GetSpacing());
  tVolumePixFromRef->Inverse();

  vtkSmartPointer<vtkTransform> tRefFromImage = vtkSmartPointer<vtkTransform>::New();
  tRefFromImage->SetMatrix(str->TransformImageToReference);

  vtkSmartPointer<vtkTransform> tImageFromImagePix = vtkSmartPointer<vtkTransform>::New();
  tImageFromImagePix->Scale(str->InputFrameImage->GetSpacing()); 

  vtkSmartPointer<vtkTransform> tImagePixToVolumePix = vtkSmartPointer<vtkTransform>::New();
  tImagePixToVolumePix->Concatenate(tVolumePixFromRef);
  tImagePixToVolumePix->Concatenate(tRefFromImage);
  tImagePixToVolumePix->Concatenate(tImageFromImagePix);

  vtkSmartPointer<vtkMatrix4x4> mImagePixToVolumePix = vtkSmartPointer<vtkMatrix4x4>::New();
  tImagePixToVolumePix->GetMatrix(mImagePixToVolumePix);

	if (str->Optimization == FULL_OPTIMIZATION)
	{
    // use fixed-point math
		// change transform matrix so that instead of taking 
		// input coords -> output coords it takes output indices -> input indices
		fixed newmatrix[4][4]; // fixed because optimization = 2
		for (int i = 0; i < 4; i++)
		{
      newmatrix[i][0] = mImagePixToVolumePix->GetElement(i,0);
			newmatrix[i][1] = mImagePixToVolumePix->GetElement(i,1);
			newmatrix[i][2] = mImagePixToVolumePix->GetElement(i,2);
			newmatrix[i][3] = mImagePixToVolumePix->GetElement(i,3);
		}

		switch (str->InputFrameImage->GetScalarType())
		{
		case VTK_SHORT:
			vtkOptimizedInsertSlice(outData, (short *)(outPtr), 
				(unsigned short *)(accPtr), 
				str->InputFrameImage, (short *)(inPtr), 
				inputFrameExtentForCurrentThread, newmatrix,
        str->ClipRectangleOrigin,str->ClipRectangleSize,
        str->FanAngles,str->FanOrigin,str->FanDepth, str->InterpolationMode);
			break;
		case VTK_UNSIGNED_SHORT:
			vtkOptimizedInsertSlice(outData,(unsigned short *)(outPtr),
				(unsigned short *)(accPtr), 
				str->InputFrameImage, (unsigned short *)(inPtr), 
				inputFrameExtentForCurrentThread, newmatrix,
        str->ClipRectangleOrigin,str->ClipRectangleSize,
        str->FanAngles,str->FanOrigin,str->FanDepth, str->InterpolationMode);
			break;
		case VTK_UNSIGNED_CHAR:
			vtkOptimizedInsertSlice(outData,(unsigned char *)(outPtr),
				(unsigned short *)(accPtr), 
				str->InputFrameImage, (unsigned char *)(inPtr), 
				inputFrameExtentForCurrentThread, newmatrix,
        str->ClipRectangleOrigin,str->ClipRectangleSize,
        str->FanAngles,str->FanOrigin,str->FanDepth, str->InterpolationMode);
			break;
		default:
			LOG_ERROR("OptimizedInsertSlice: Unknown input ScalarType");
			return VTK_THREAD_RETURN_VALUE;
		}
	}
  else
	{
    // if we are not using fixed point math for optimization = 2, we are either:
    // doing no optimization (0) OR
    // breaking into x, y, z components with no bounds checking for nearest neighbor (1)

		// change transform matrix so that instead of taking 
		// input coords -> output coords it takes output indices -> input indices
		double newmatrix[4][4];
		for (int i = 0; i < 4; i++)
		{
			newmatrix[i][0] = mImagePixToVolumePix->GetElement(i,0);
			newmatrix[i][1] = mImagePixToVolumePix->GetElement(i,1);
			newmatrix[i][2] = mImagePixToVolumePix->GetElement(i,2);
			newmatrix[i][3] = mImagePixToVolumePix->GetElement(i,3);
		}


    if (str->Optimization==PARTIAL_OPTIMIZATION)
    {
      switch (inData->GetScalarType())
      {
      case VTK_SHORT:
        vtkOptimizedInsertSlice(outData, (short *)(outPtr), 
          (unsigned short *)(accPtr), 
          inData, (short *)(inPtr), 
          inputFrameExtentForCurrentThread, newmatrix,
          str->ClipRectangleOrigin,str->ClipRectangleSize,
          str->FanAngles,str->FanOrigin,str->FanDepth, str->InterpolationMode);
        break;
      case VTK_UNSIGNED_SHORT:
        vtkOptimizedInsertSlice(outData,(unsigned short *)(outPtr),
          (unsigned short *)(accPtr), 
          inData, (unsigned short *)(inPtr), 
          inputFrameExtentForCurrentThread, newmatrix,
          str->ClipRectangleOrigin,str->ClipRectangleSize,
          str->FanAngles,str->FanOrigin,str->FanDepth, str->InterpolationMode);
        break;
      case VTK_UNSIGNED_CHAR:
        vtkOptimizedInsertSlice(outData,(unsigned char *)(outPtr),
          (unsigned short *)(accPtr), 
          inData, (unsigned char *)(inPtr), 
          inputFrameExtentForCurrentThread, newmatrix,
          str->ClipRectangleOrigin,str->ClipRectangleSize,
          str->FanAngles,str->FanOrigin,str->FanDepth, str->InterpolationMode);
        break;
      default:
        LOG_ERROR("OptimizedInsertSlice: Unknown input ScalarType");
      }
    }
    else
    {
      // no optimization
      switch (inData->GetScalarType())
      {
      case VTK_SHORT:
        vtkUnoptimizedInsertSlice(outData, (short *)(outPtr), 
          (unsigned short *)(accPtr), 
          inData, (short *)(inPtr), 
          inputFrameExtentForCurrentThread, mImagePixToVolumePix,
          str->ClipRectangleOrigin,str->ClipRectangleSize,
          str->FanAngles,str->FanOrigin,str->FanDepth, str->InterpolationMode);
        break;
      case VTK_UNSIGNED_SHORT:
        vtkUnoptimizedInsertSlice(outData,(unsigned short *)(outPtr),
          (unsigned short *)(accPtr), 
          inData, (unsigned short *)(inPtr), 
          inputFrameExtentForCurrentThread, mImagePixToVolumePix,
          str->ClipRectangleOrigin,str->ClipRectangleSize,
          str->FanAngles,str->FanOrigin,str->FanDepth, str->InterpolationMode);
        break;
      case VTK_UNSIGNED_CHAR:
        vtkUnoptimizedInsertSlice(outData,(unsigned char *)(outPtr),
          (unsigned short *)(accPtr), 
          inData, (unsigned char *)(inPtr), 
          inputFrameExtentForCurrentThread, mImagePixToVolumePix,
          str->ClipRectangleOrigin,str->ClipRectangleSize,
          str->FanAngles,str->FanOrigin,str->FanDepth, str->InterpolationMode);
        break;
      default:
        LOG_ERROR("UnoptimizedInsertSlice: Unknown input ScalarType");
      }
    }
	}

	return VTK_THREAD_RETURN_VALUE;
}

//----------------------------------------------------------------------------
// For streaming and threads.  Splits output update extent into num pieces.
// This method needs to be called num times.  Results must not overlap for
// consistent starting extent.  Subclass can override this method.
// This method returns the number of pieces resulting from a successful split.
// This can be from 1 to "requestedNumberOfThreads".  
// If 1 is returned, the extent cannot be split.
int vtkPasteSliceIntoVolume::SplitSliceExtent(int splitExt[6], int fullExt[6], int threadId, int requestedNumberOfThreads)
{
	
	int min, max; // the min and max indices of the axis of interest

	LOG_TRACE("SplitSliceExtent: ( " << fullExt[0] << ", " 
		<< fullExt[1] << ", "
		<< fullExt[2] << ", " << fullExt[3] << ", "
		<< fullExt[4] << ", " << fullExt[5] << "), " 
		<< threadId << " of " << requestedNumberOfThreads);

	// start with same extent
	memcpy(splitExt, fullExt, 6 * sizeof(int));

	// determine which axis we should split along - preference is z, then y, then x
	// as long as we can possibly split along that axis (i.e. as long as z0 != z1)
  int splitAxis = 2; // the axis we should split along, try with z first
	min = fullExt[4];
	max = fullExt[5];
	while (min == max)
	{
		--splitAxis;
		// we cannot split if the input extent is something like [50, 50, 100, 100, 0, 0]!
		if (splitAxis < 0)
		{ 
			LOG_DEBUG("Cannot split the extent to multiple threads");
			return 1;
		}
		min = fullExt[splitAxis*2];
		max = fullExt[splitAxis*2+1];
	}

	// determine the actual number of pieces that will be generated
	int range = max - min + 1;
	// split the range over the maximum number of threads
	int valuesPerThread = (int)ceil(range/(double)requestedNumberOfThreads);
	// figure out the largest thread id used
	int maxThreadIdUsed = (int)ceil(range/(double)valuesPerThread) - 1;
	// if we are in a thread that will work on part of the extent, then figure
	// out the range that this thread should work on
	if (threadId < maxThreadIdUsed)
	{
		splitExt[splitAxis*2] = splitExt[splitAxis*2] + threadId*valuesPerThread;
		splitExt[splitAxis*2+1] = splitExt[splitAxis*2] + valuesPerThread - 1;
	}
	if (threadId == maxThreadIdUsed)
	{
		splitExt[splitAxis*2] = splitExt[splitAxis*2] + threadId*valuesPerThread;
	}

	// return the number of threads used
	return maxThreadIdUsed + 1;
}

//****************************************************************************
// FILLING HOLES
//****************************************************************************

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


//****************************************************************************
// I/O
//****************************************************************************

char* vtkPasteSliceIntoVolume::GetInterpolationModeAsString(InterpolationType type)
{
  switch (type)
  {
    case NEAREST_NEIGHBOR_INTERPOLATION: return "NEAREST_NEIGHBOR";
    case LINEAR_INTERPOLATION: return "LINEAR";
    default:
      LOG_ERROR("Unknown interpolation option: "<<type);
      return "unknown";
  }
}

char* vtkPasteSliceIntoVolume::GetOptimizationModeAsString(OptimizationType type)
{
  switch (type)
  {
    case FULL_OPTIMIZATION: return "FULL";
    case PARTIAL_OPTIMIZATION: return "PARTIAL";
    case NO_OPTIMIZATION: return "NONE";
    default:
      LOG_ERROR("Unknown optimization option: "<<type);
      return "unknown";
  }
}


//----------------------------------------------------------------------------
// Get the XML element describing the freehand object
PlusStatus vtkPasteSliceIntoVolume::WriteConfiguration(vtkXMLDataElement *config)
{
  if ( config == NULL )
	{
		LOG_ERROR("Unable to write configuration from volume reconstructor! (XML data element is NULL)"); 
		return PLUS_FAIL; 
	}

	vtkSmartPointer<vtkXMLDataElement> reconConfig = config->FindNestedElementWithName("VolumeReconstruction");
	if (reconConfig == NULL)
  {
    vtkSmartPointer<vtkXMLDataElement> newReconConfig = vtkXMLDataElement::New();
    newReconConfig->SetName("VolumeReconstruction");
    config->AddNestedElement(newReconConfig);
    reconConfig = config->FindNestedElementWithName("VolumeReconstruction");
    if (reconConfig == NULL)
    {
      LOG_ERROR("Failed to add VolumeReconstruction element");
		  return PLUS_FAIL;
    }
	}

	// output parameters
	reconConfig->SetVectorAttribute("OutputSpacing", 3, this->OutputSpacing);
	reconConfig->SetVectorAttribute("OutputOrigin", 3, this->OutputOrigin);
	reconConfig->SetVectorAttribute("OutputExtent", 6, this->OutputExtent);

	// clipping parameters
	reconConfig->SetVectorAttribute("ClipRectangleOrigin", 2, this->ClipRectangleOrigin);
	reconConfig->SetVectorAttribute("ClipRectangleSize", 2, this->ClipRectangleSize);

	// fan parameters
  if (FanParametersDefined())
  {
	  reconConfig->SetVectorAttribute("FanAngles", 2, this->FanAngles);
	  reconConfig->SetVectorAttribute("FanOrigin", 2, this->FanOrigin);
	  reconConfig->SetDoubleAttribute("FanDepth", this->FanDepth);
  }

	// reconstruction options
  reconConfig->SetAttribute("Interpolation", this->GetInterpolationModeAsString(this->InterpolationMode));
  reconConfig->SetAttribute("Optimization", this->GetOptimizationModeAsString(this->Optimization));
  reconConfig->SetAttribute("Compounding", this->Compounding?"On":"Off");

  if (this->NumberOfThreads>0)
  {
    reconConfig->SetIntAttribute("NumberOfThreads", this->NumberOfThreads);
  }

	return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPasteSliceIntoVolume::ReadConfiguration(vtkXMLDataElement* aConfig)
{
  vtkSmartPointer<vtkXMLDataElement> reconConfig = aConfig->FindNestedElementWithName("VolumeReconstruction");
  if (reconConfig == NULL)
  {
    LOG_ERROR("No volume reconstruction is found in the XML tree!");
    return PLUS_FAIL;
  }

  // output volume parameters
  // origin and spacing is defined in the reference coordinate system
  reconConfig->GetVectorAttribute("OutputSpacing", 3, this->OutputSpacing);
  reconConfig->GetVectorAttribute("OutputOrigin", 3, this->OutputOrigin);
  reconConfig->GetVectorAttribute("OutputExtent", 6, this->OutputExtent);

  // clipping parameters
  reconConfig->GetVectorAttribute("ClipRectangleOrigin", 4, this->ClipRectangleOrigin);
  reconConfig->GetVectorAttribute("ClipRectangleSize", 4, this->ClipRectangleSize);

  // fan parameters
  reconConfig->GetVectorAttribute("FanAngles", 2, this->FanAngles);
  reconConfig->GetVectorAttribute("FanOrigin", 2, this->FanOrigin);
  reconConfig->GetScalarAttribute("FanDepth", this->FanDepth);

  // reconstruction options
  if (reconConfig->GetAttribute("Interpolation"))
  {
    if (STRCASECMP(reconConfig->GetAttribute("Interpolation"),
      GetInterpolationModeAsString(LINEAR_INTERPOLATION)) == 0)
    {
      this->SetInterpolationMode(LINEAR_INTERPOLATION);
    }
    else if (STRCASECMP(reconConfig->GetAttribute("Interpolation"), 
      GetInterpolationModeAsString(NEAREST_NEIGHBOR_INTERPOLATION)) == 0)
    {
      this->SetInterpolationMode(NEAREST_NEIGHBOR_INTERPOLATION);
    }
    else
    {
      LOG_ERROR("Unknown interpolation option: "<<reconConfig->GetAttribute("Interpolation")<<". Valid options: LINEAR, NEAREST_NEIGHBOR.");
    }
  }
  if (reconConfig->GetAttribute("Optimization"))
  {
    if (STRCASECMP(reconConfig->GetAttribute("Optimization"), 
      GetOptimizationModeAsString(FULL_OPTIMIZATION)) == 0)
    {
      this->SetOptimization(FULL_OPTIMIZATION);
    }
    else if (STRCASECMP(reconConfig->GetAttribute("Optimization"), 
      GetOptimizationModeAsString(PARTIAL_OPTIMIZATION)) == 0)
    {
      this->SetOptimization(PARTIAL_OPTIMIZATION);
    }
    else if (STRCASECMP(reconConfig->GetAttribute("Optimization"), 
      GetOptimizationModeAsString(NO_OPTIMIZATION)) == 0)
    {
      this->SetOptimization(NO_OPTIMIZATION);
    }
    else
    {
      LOG_ERROR("Unknown optimization option: "<<reconConfig->GetAttribute("Optimization")<<". Valid options: FULL, PARTIAL, NONE.");
    }
  }
  if (reconConfig->GetAttribute("Compounding"))
  {
    ((strcmp(reconConfig->GetAttribute("Compounding"), "On") == 0) ? this->SetCompounding(1) : this->SetCompounding(0));
  }

  reconConfig->GetScalarAttribute("NumberOfThreads", this->NumberOfThreads);

	return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
bool vtkPasteSliceIntoVolume::FanParametersDefined()
{
  return this->FanAngles[0] != 0.0 || this->FanAngles[1] != 0.0;
}
