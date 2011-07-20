/*=========================================================================

Program:   Visualization Toolkit
Module:    $RCSfile: vtkFreehandUltrasound2.cxx,v $
Language:  C++
Date:      $Date: 2009/07/20 18:48:07 $
Version:   $Revision: 1.47 $
Thanks:    Thanks to David G. Gobbi who developed this class. 
Thanks:    Thanks to Danielle Pace who developed this class.

==========================================================================

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

#include "vtkFreehandUltrasound2.h"
#include "vtkFreehandUltrasound2Helper.h"
#include "vtkObjectFactory.h"
#include "vtkVideoBuffer.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkMath.h"
#include "vtkCriticalSection.h"
#include "vtkMutexLock.h"
#include "vtkXMLUtilities.h"
#include "vtkXMLDataElement.h"
#include "vtkTimestampedCircularBuffer.h"
#include "vtkIndent.h"

vtkCxxRevisionMacro(vtkFreehandUltrasound2, "$Revision: 1.47 $");
vtkStandardNewMacro(vtkFreehandUltrasound2);

// for keeping track of threading information
struct vtkFreehand2ThreadStruct
{
	vtkFreehandUltrasound2 *Filter;
	vtkImageData   *Input;
	vtkImageData   *Output;
	vtkImageData   *Accumulator;
	vtkMatrix4x4   *IndexMatrix;
};

//----------------------------------------------------------------------------
vtkFreehandUltrasound2::vtkFreehandUltrasound2()
{

	// one PixelCount for each threadId, where 0 <= threadId < 4
	this->PixelCount[0] = 0;
	this->PixelCount[1] = 0;
	this->PixelCount[2] = 0;
	this->PixelCount[3] = 0;

	// basics
	this->Slice = NULL;
	this->AccumulationBuffers = NULL;
	this->VideoSource = NULL;
	this->TrackerTool = NULL;
	this->VideoLag = 0.0;
    this->VideoBufferUid = 0; 

	// this will force ClearOutput() to run, which will allocate the output
	// and the accumulation buffer(s)
	this->NeedsClear = 1;

	// parameters for fan/image
	this->OutputOrigin[0] = -127.5;
	this->OutputOrigin[1] = -127.5;
	this->OutputOrigin[2] = -127.5;

	this->OutputSpacing[0] = 1.0;
	this->OutputSpacing[1] = 1.0;
	this->OutputSpacing[2] = 1.0;

	this->OutputExtent[0] = 0;
	this->OutputExtent[1] = 255;
	this->OutputExtent[2] = 0;
	this->OutputExtent[3] = 255;
	this->OutputExtent[4] = 0;
	this->OutputExtent[5] = 255;

	this->ClipRectangle[0] = -1e8;
	this->ClipRectangle[1] = -1e8;
	this->ClipRectangle[2] = +1e8;
	this->ClipRectangle[3] = +1e8;

	this->FanAngles[0] = 0.0;
	this->FanAngles[1] = 0.0;
	this->FanOrigin[0] = 0.0;
	this->FanOrigin[1] = 0.0;
	this->FanDepth = +1e8;

	this->ImageFlippedOff(); 

	// reconstruction options
	this->InterpolationMode = VTK_FREEHAND_NEAREST;
	this->Compounding = 0;
	this->Optimization = 2;

	// reconstruction transformations
	this->SliceAxes = NULL;
	this->SliceTransform = vtkTransform::New();
	this->IndexMatrix = NULL;
	//this->LastIndexMatrix = NULL;

	// fan rotation
	this->Rotating = 0; // default off
	this->FanRotation = 0;
	this->PreviousFanRotation = 0;
	this->RotatingThreshold1 = 0;
	this->RotatingThreshold2 = 0;
	this->RotatingShiftX = 0;
	this->RotatingShiftY = 0;
	this->RotationClipper = vtkImageClip::New();
	this->RotationThresholder = vtkImageThreshold::New();
	this->MaximumRotationChange = 20;

	// threading - one thread for each CPU is used for the reconstruction
	this->ActiveFlagLock = vtkCriticalSection::New();
	this->Threader = vtkMultiThreader::New();
	this->NumberOfThreads = this->Threader->GetNumberOfThreads();
#ifdef DEBUG_RECONSTRUCTION
	// Disable multithreading to make debugging of reconstruction easier
	this->NumberOfThreads = 1;
#endif
	this->ReconstructionThreadId = -1;

	// for running the reconstruction
	this->TrackerBuffer = NULL;
	this->ReconstructionRate = 0;
	this->RealTimeReconstruction = 0;
	this->ReconstructionFrameCount = 0;

	// pipeline setup
	this->SetNumberOfInputPorts(0);
	// sets the number of output volumes, creates the output ports on the VTK 5
	// pipeline, and creates the accumulation buffer
	this->SetupOutputVolumes();
	this->SetupAccumulationBuffers();
	
	this->ReconstructionFinishedOn();
}

//----------------------------------------------------------------------------
vtkFreehandUltrasound2::~vtkFreehandUltrasound2()
{
	this->StopRealTimeReconstruction();

	this->SetSlice(NULL);
	this->SetVideoSource(NULL);
	this->SetTrackerTool(NULL);
	this->SetSliceAxes(NULL);
	this->SetSliceTransform(NULL);

	if (this->AccumulationBuffers)
	{
		if (this->AccumulationBuffers[0])
		{
			this->AccumulationBuffers[0]->Delete();
		}
		delete [] this->AccumulationBuffers;
	}

	if (this->IndexMatrix)
	{
		this->IndexMatrix->Delete();
	}
	/*if (this->LastIndexMatrix)
	{
	this->LastIndexMatrix->Delete();
	}*/

	if (this->RotationClipper)
	{
		this->RotationClipper->Delete();
	}
	if (this->RotationThresholder)
	{
		this->RotationThresholder->Delete();
	}

	if (this->ActiveFlagLock)
	{
		this->ActiveFlagLock->Delete();
	}
	if (this->Threader)
	{
		this->Threader->Delete();
	}
}

//----------------------------------------------------------------------------
void vtkFreehandUltrasound2::PrintSelf(ostream& os, vtkIndent indent)
{
	this->Superclass::PrintSelf(os,indent);

	os << indent << "Slice: " << this->Slice << "\n";
	if (this->Slice)
	{
		this->Slice->PrintSelf(os,indent.GetNextIndent());
	}
	os << indent << "AccumulationBuffers: " << this->AccumulationBuffers << "\n";
	if (this->AccumulationBuffers)
	{
		os << indent << "AccumulationBuffers[0]:\n";
		this->AccumulationBuffers[0]->PrintSelf(os,indent.GetNextIndent());
	}
	os << indent << "VideoSource: " << this->VideoSource << "\n";
	if (this->VideoSource)
	{
		this->VideoSource->PrintSelf(os,indent.GetNextIndent());
	}
	os << indent << "TrackerTool: " << this->TrackerTool << "\n";
	if (this->TrackerTool)
	{
		this->TrackerTool->PrintSelf(os,indent.GetNextIndent());
	}
	os << indent << "TrackerBuffer: " << this->TrackerBuffer << "\n";
	if (this->TrackerBuffer)
	{
		this->TrackerBuffer->PrintSelf(os,indent.GetNextIndent());
	}
	os << indent << "VideoLag: " << this->VideoLag << "\n";
	os << indent << "NeedsClear: " << (this->NeedsClear ? "Yes\n":"No\n");
	os << indent << "OutputOrigin: " << this->OutputOrigin[0] << " " <<
		this->OutputOrigin[1] << " " << this->OutputOrigin[2] << "\n";
	os << indent << "OutputSpacing: " << this->OutputSpacing[0] << " " <<
		this->OutputSpacing[1] << " " << this->OutputSpacing[2] << "\n";
	os << indent << "OutputExtent: " << this->OutputExtent[0] << " " <<
		this->OutputExtent[1] << " " << this->OutputExtent[2] << " " <<
		this->OutputExtent[3] << " " << this->OutputExtent[4] << " " <<
		this->OutputExtent[5] << "\n";
	os << indent << "ClipRectangle: " << this->ClipRectangle[0] << " " <<
		this->ClipRectangle[1] << " " << this->ClipRectangle[2] << " " <<
		this->ClipRectangle[3] << "\n";
	os << indent << "FanAngles: " << this->FanAngles[0] << " " <<
		this->FanAngles[1] << "\n";
	os << indent << "FanOrigin: " << this->FanOrigin[0] << " " <<
		this->FanOrigin[1] << "\n";
	os << indent << "FanDepth: " << this->FanDepth << "\n";
	os << indent << "InterpolationMode: " << this->GetInterpolationModeAsString() << "\n";
	os << indent << "Optimization: " << (this->Optimization ? "On\n":"Off\n");
	os << indent << "Compounding: " << (this->Compounding ? "On\n":"Off\n");
	os << indent << "SliceAxes: " << this->SliceAxes << "\n";
	if (this->SliceAxes)
	{
		this->SliceAxes->PrintSelf(os,indent.GetNextIndent());
	}
	os << indent << "SliceTransform: " << this->SliceTransform << "\n";
	if (this->SliceTransform)
	{
		this->SliceTransform->PrintSelf(os,indent.GetNextIndent());
	}
	os << indent << "IndexMatrix: " << this->IndexMatrix << "\n";
	if (this->IndexMatrix)
	{
		this->IndexMatrix->PrintSelf(os,indent.GetNextIndent());
	}
	/*os << indent << "LastIndexMatrix: " << this->LastIndexMatrix << "\n";
	if (this->LastIndexMatrix)
	{
	this->LastIndexMatrix->PrintSelf(os,indent.GetNextIndent());
	}*/
	os << indent << "Rotating: " << (this->Rotating ? "On\n":"Off\n");
	os << indent << "FanRotation: " << this->FanRotation << "\n";
	os << indent << "PreviousFanRotation: " << this->PreviousFanRotation << "\n";
	os << indent << "MaximumRotationChange: " << this->MaximumRotationChange << "\n";
	os << indent << "RotatingThreshold1: " << this->RotatingThreshold1 << "\n";
	os << indent << "RotatingThreshold2: " << this->RotatingThreshold2 << "\n";
	os << indent << "RotatingShiftX: " << this->RotatingShiftX << "\n";
	os << indent << "RotatingShiftY: " << this->RotatingShiftY << "\n";
	os << indent << "RotationClipper: " << this->RotationClipper << "\n";
	if (this->RotationClipper)
	{
		this->RotationClipper->PrintSelf(os,indent.GetNextIndent());
	}
	os << indent << "RotationThresholder: " << this->RotationThresholder << "\n";
	if (this->RotationThresholder)
	{
		this->RotationThresholder->PrintSelf(os,indent.GetNextIndent());
	}
	os << indent << "ImageFlipped: " << (this->ImageFlipped ? "On\n":"Off\n");
	os << indent << "NumberOfThreads: " << this->NumberOfThreads << "\n";
	os << indent << "ReconstructionThreadId: " << this->ReconstructionThreadId << "\n";
	os << indent << "Reconstruction Rate: " << this->ReconstructionRate << "\n";
	os << indent << "Realtime Reconstruction: " << (this->RealTimeReconstruction ? "On\n":"Off\n");
	os << indent << "Reconstruction Frame Count: " << this->ReconstructionFrameCount << "\n";
}


//****************************************************************************
// BASICS FOR 3D RECONSTRUCTION
//****************************************************************************

//----------------------------------------------------------------------------
// Set the image slice that will be inserted into the output volume
void vtkFreehandUltrasound2::SetSlice(vtkImageData *slice)
{  
	if(this->VideoSource)
	{
		this->Slice = this->VideoSource->GetOutput();
	}
	else if(slice)
	{
		this->Slice = slice;
	}
}

//----------------------------------------------------------------------------
// Get the image slice that will be inserted into the output volume
vtkImageData* vtkFreehandUltrasound2::GetSlice()
{
	if(this->VideoSource)
	{
		return this->VideoSource->GetOutput(); 
	}
	else
	{
		return this->Slice;
	}
}

//----------------------------------------------------------------------------
// Set the video source to input the slices from
vtkCxxSetObjectMacro(vtkFreehandUltrasound2,VideoSource,vtkVideoSource2);

//----------------------------------------------------------------------------
// Set the tracker tool to input transforms from
vtkCxxSetObjectMacro(vtkFreehandUltrasound2,TrackerTool,vtkTrackerTool);

//----------------------------------------------------------------------------
// Get the reconstructed volume
vtkImageData *vtkFreehandUltrasound2::GetOutput()
{
	if(this->GetOutputDataObject(0))
	{
		return vtkImageData::SafeDownCast(this->GetOutputDataObject(0));
	}
	else
	{
		return NULL;
	}
}

//----------------------------------------------------------------------------
// Get the accumulation buffer
// Will be NULL if we are not compounding
vtkImageData *vtkFreehandUltrasound2::GetAccumulationBuffer()
{
	return this->AccumulationBuffers[0];
}

//----------------------------------------------------------------------------
// Clear the output volume and the accumulation buffer
// (basically just calls InternalInternalClearOutput)
void vtkFreehandUltrasound2::ClearOutput()
{
	// if we are not currently reconstructing...
	if (this->ReconstructionThreadId == -1)
	{
		this->NeedsClear = 1;
		this->GetOutput()->UpdateInformation();
		vtkImageData* outData = this->GetOutput();
		vtkImageData* accData = this->GetAccumulationBuffer();
		this->InternalInternalClearOutput(outData, accData);
	}
	this->Modified();
}

//----------------------------------------------------------------------------
// Clear the output volume and the accumulation buffer
// with no check for whether we are reconstructing
// (basically just calls InternalInternalClearOutput)
void vtkFreehandUltrasound2::InternalClearOutput()
{
	vtkImageData* outData = this->GetOutput();
	vtkImageData* accData = this->GetAccumulationBuffer();
	this->InternalInternalClearOutput(outData, accData);
}

//----------------------------------------------------------------------------
// Actually clear the output volume and accumulation buffer
void vtkFreehandUltrasound2::InternalInternalClearOutput(vtkImageData* outData, vtkImageData* accData)
{
	this->NeedsClear = 0;

	this->InternalClearOutputHelperForOutput(outData);
	this->InternalClearOutputHelperForAccumulation(accData);
	/*if (this->LastIndexMatrix)
	{
	this->LastIndexMatrix->Delete();
	this->LastIndexMatrix = NULL;
	}*/

	this->SetPixelCount(0,0);
	this->SetPixelCount(1,0);
	this->SetPixelCount(2,0);
	this->SetPixelCount(3,0);
}

//----------------------------------------------------------------------------
// Actually clear the output volume
void vtkFreehandUltrasound2::InternalClearOutputHelperForOutput(vtkImageData* outData)
{
	// Set everything in the output (within the output extent) to intensity 0
	int *outExtent = this->OutputExtent;
	if (outData)
	{
		outData->SetExtent(outExtent);
		outData->AllocateScalars();
		void *outPtr = outData->GetScalarPointerForExtent(outExtent);
		memset(outPtr,0,((outExtent[1]-outExtent[0]+1)*
			(outExtent[3]-outExtent[2]+1)*
			(outExtent[5]-outExtent[4]+1)*
			outData->GetScalarSize()*outData->GetNumberOfScalarComponents()));
	}
}

//----------------------------------------------------------------------------
// Actually clear the accumulation buffer
void vtkFreehandUltrasound2::InternalClearOutputHelperForAccumulation(vtkImageData* accData)
{
	// if we are compounding, then clear the accumulation buffer too
	int *outExtent = this->OutputExtent;
	if (accData)
	{
		accData->SetExtent(outExtent);
		accData->AllocateScalars();
		void *accPtr = accData->GetScalarPointerForExtent(outExtent);
		memset(accPtr,0,((outExtent[1]-outExtent[0]+1)*
			(outExtent[3]-outExtent[2]+1)*
			(outExtent[5]-outExtent[4]+1)*
			accData->GetScalarSize()*accData->GetNumberOfScalarComponents()));
	}
}

//****************************************************************************
// SET/GET IMAGING PARAMETERS
//****************************************************************************

//----------------------------------------------------------------------------
// Set compounding setting
void vtkFreehandUltrasound2::SetCompounding(int compound)
{

	// ignore if we are already reconstructing
	if (this->ReconstructionThreadId != -1)
	{
		return;
	}

	this->Compounding = compound;

	// turn compounding on
	if (compound)
	{
		this->SetupAccumulationBuffers();
	}
	// turn compounding off
	else
	{
		this->DeleteAccumulationBuffers();
	}
}

//****************************************************************************
// RECONSTRUCTION EXECUTION - BASICS
//****************************************************************************

//----------------------------------------------------------------------------
// Set the axes of the slice to insert into the reconstruction volume,
// relative the (x,y,z) axes of the reconstruction volume itself.
// This is typically the transform from the tracking system
vtkCxxSetObjectMacro(vtkFreehandUltrasound2,SliceAxes,vtkMatrix4x4);

//----------------------------------------------------------------------------
// Together with the slice axes, the slice transform transforms from the local
// coodinate system of the slice to the coordinate system of the output volume
vtkCxxSetObjectMacro(vtkFreehandUltrasound2,SliceTransform,vtkLinearTransform);

//----------------------------------------------------------------------------
// The transform matrix supplied by the user converts output coordinates
// to input coordinates.  Instead, to speed up the pixel lookup, the following
// function provides a matrix which converts output pixel indices to input
// pixel indices.
// Saves the result to this->IndexMatrix
vtkMatrix4x4 *vtkFreehandUltrasound2::GetIndexMatrix()
{
	return this->GetIndexMatrixHelper(this->GetSliceAxes(), this->SliceTransform);
}

//----------------------------------------------------------------------------
// Does the actual work for GetIndexMatrix, to give a matrix that converts output
// pixel indices to input pixel indices
// Saves the result to this->IndexMatrix
vtkMatrix4x4 *vtkFreehandUltrasound2::GetIndexMatrixHelper(vtkMatrix4x4* sliceAxes, vtkLinearTransform* sliceTransform)
{

	if (this->IndexMatrix == NULL)
	{
		this->IndexMatrix = vtkMatrix4x4::New();
	}

	// get the origin and spacing for the input slice and the output volume
	vtkFloatingPointType inOrigin[3];
	vtkFloatingPointType inSpacing[3];
	vtkFloatingPointType outOrigin[3];
	vtkFloatingPointType outSpacing[3];

	this->GetSlice()->GetSpacing(inSpacing);
	this->GetSlice()->GetOrigin(inOrigin);
	this->GetOutput()->GetSpacing(outSpacing);
	this->GetOutput()->GetOrigin(outOrigin);

	// make the transformations we'll need
	vtkTransform *transform = vtkTransform::New();
	vtkMatrix4x4 *inMatrix = vtkMatrix4x4::New();
	vtkMatrix4x4 *outMatrix = vtkMatrix4x4::New();

	// Equation we'll make incrementally for transform:
	// transform = outMatrix * (sliceTransform * sliceAxes) * inMatrix

	// transform = sliceAxes
	if (sliceAxes)
	{
		transform->SetMatrix(sliceAxes);
	}

  vtkSmartPointer< vtkMatrix4x4 > sliceTransformMatrix = vtkSmartPointer< vtkMatrix4x4 >::New();
  sliceTransform->GetMatrix(sliceTransformMatrix);

	// transform = sliceTransform * sliceAxes
	// sliceAxes == sliceTransform, so we shouldn't multiply them
	/*if (sliceTransform)
	{
	transform->PostMultiply();
	transform->Concatenate(sliceTransform->GetMatrix());
	}
	*/

	int isIdentity = vtkIsIdentityMatrix(transform->GetMatrix());

	// outMatrix takes OutputData indices to OutputData coordinates,
	// inMatrix takes InputData coordinates to InputData indices
	for (int i = 0; i < 3; i++) 
	{
		if (inSpacing[i] != outSpacing[i] || inOrigin[i] != outOrigin[i])
		{
			isIdentity = 0;
		}
		inMatrix->Element[i][i] = inSpacing[i];
		inMatrix->Element[i][3] = inOrigin[i];
		outMatrix->Element[i][i] = 1.0f/outSpacing[i];
		outMatrix->Element[i][3] = -outOrigin[i]/outSpacing[i];
	}

	// transform = outMatrix * (sliceTransform * sliceAxes) * inMatrix
	if (!isIdentity)
	{
		transform->PostMultiply();
		transform->Concatenate(outMatrix);
		transform->PreMultiply();
		transform->Concatenate(inMatrix);
	}

	// save the transform's matrix in this->IndexMatrix
	transform->GetMatrix(this->IndexMatrix);

	transform->Delete();
	inMatrix->Delete();
	outMatrix->Delete();

	return this->IndexMatrix;
}

//----------------------------------------------------------------------------
// Set the number of pixels inserted by a particular threadId
void  vtkFreehandUltrasound2::SetPixelCount(int threadId, int count)
{
	if( threadId < 4 && threadId >= 0)
	{
		this->PixelCount[threadId] = count;
	}
}

//----------------------------------------------------------------------------
// Increment the number of pixels inserted by a particular threadId by a
// specified number of pixels
void  vtkFreehandUltrasound2::IncrementPixelCount(int threadId, int increment)
{
	if( threadId < 4 && threadId >= 0)
	{
		this->PixelCount[threadId] += increment;
	}
}

//----------------------------------------------------------------------------
// Get the total number of pixels inserted by all threads
int vtkFreehandUltrasound2::GetPixelCount()
{
	return ( this->PixelCount[0] + this->PixelCount[1] +
		this->PixelCount[2] + this->PixelCount[3] );
}

//----------------------------------------------------------------------------
// convert the ClipRectangle (which is in millimetre coordinates) into a
// clip extent that can be applied to the input data - number of pixels (+ or -)
// from the origin (the z component is copied from the inExt parameter)
// 
// clipExt = {x0, x1, y0, y1, z0, z1} <-- the "output" of this function is to
//                                        change this array
// inOrigin = {x, y, z} <-- the origin in mm
// inSpacing = {x, y, z} <-- the spacing in mm
// inExt = {x0, x1, y0, y1, z0, z1} <-- min/max possible extent, in pixels
void vtkFreehandUltrasound2::GetClipExtent(int clipExt[6],
										   vtkFloatingPointType inOrigin[3],
										   vtkFloatingPointType inSpacing[3],
										   const int inExt[6])
{
	// Map the clip rectangle (millimetres) to pixels
	// --> number of pixels (+ or -) from the origin
	int x0 = (int)ceil((this->GetClipRectangle()[0]-inOrigin[0])/inSpacing[0]);
	int x1 = (int)floor((this->GetClipRectangle()[2]-inOrigin[0])/inSpacing[0]);
	int y0 = (int)ceil((this->GetClipRectangle()[1]-inOrigin[1])/inSpacing[1]);
	int y1 = (int)floor((this->GetClipRectangle()[3]-inOrigin[1])/inSpacing[1]);

	// Make sure that x0 <= x1 and y0 <= y1
	if (x0 > x1)
	{
		int tmp = x0; x0 = x1; x1 = tmp;
	}
	if (y0 > y1)
	{
		int tmp = y0; y0 = y1; y1 = tmp;
	}

	// make sure the clip extent lies within the input extent
	if (x0 < inExt[0])
	{
		x0 = inExt[0];
	}
	if (x1 > inExt[1])
	{
		x1 = inExt[1];
	}
	// clip extent was outside of range of input extent
	if (x0 > x1)
	{
		x0 = inExt[0];
		x1 = inExt[0]-1;
	}

	if (y0 < inExt[2])
	{
		y0 = inExt[2];
	}
	if (y1 > inExt[3])
	{
		y1 = inExt[3];
	}
	// clip extent was outside of range of input extent
	if (y0 > y1)
	{
		y0 = inExt[2];
		y1 = inExt[2]-1;
	}

	// Set the clip extent
	clipExt[0] = x0;
	clipExt[1] = x1;
	clipExt[2] = y0;
	clipExt[3] = y1;
	clipExt[4] = inExt[4];
	clipExt[5] = inExt[5];
}

//****************************************************************************
// STARTING/STOPPING RECONSTRUCTION
//****************************************************************************

//----------------------------------------------------------------------------
// Things to do before real-time reconstruction - in the base class we do
// practically nothing!  This is overridden in derived classes
int vtkFreehandUltrasound2::InitializeRealTimeReconstruction()
{

	if (this->VideoSource)
	{
		if (!this->VideoSource->GetRecording())
		{
			LOG_WARNING("video wasn't recording; starting recording for you");
			this->VideoSource->StartRecording();
		}
	}

	if (this->TrackerTool)
	{
		if (!this->TrackerTool->GetTracker()->IsTracking())
		{
			LOG_WARNING("tracker wasn't started, starting tracking for you");
			this->TrackerTool->GetTracker()->StartTracking();
		}
	}

	this->GetOutput()->Update();
	return 1;
}

//----------------------------------------------------------------------------
// Things to do after real-time reconstruction - in the base class we do
// nothing!  This is overridden in derived classes
void vtkFreehandUltrasound2::UninitializeRealTimeReconstruction()
{
	this->PreviousFanRotation = this->FanRotation;
}

//----------------------------------------------------------------------------
// Start doing real-time reconstruction from the video source.
// This will spawn a thread that does the reconstruction in the
// background
void vtkFreehandUltrasound2::StartRealTimeReconstruction()
{

	// if the real time reconstruction isn't already running...
	if (this->ReconstructionThreadId == -1)
	{
		this->RealTimeReconstruction = 1; // we are doing realtime reconstruction

		// Setup the slice axes matrix
		vtkMatrix4x4 *matrix = vtkMatrix4x4::New();
		this->SetSliceAxes(matrix);
		matrix->Delete();

		// initialize the real time reconstruction - this is overridden in derived
		// classes
		if (this->InitializeRealTimeReconstruction())
		{
			this->InternalExecuteInformation();
      this->ReconstructionFinishedOff();    
			this->ReconstructionThreadId = \
				this->Threader->SpawnThread((vtkThreadFunctionType)\
				&vtkReconstructionThread,
				this);
		}
		else
		{
			LOG_ERROR("Could not initialize real-time reconstruction ... stopping");
		}
	}
}

//----------------------------------------------------------------------------
// Stop the real-time reconstruction
void vtkFreehandUltrasound2::StopRealTimeReconstruction()
{  
	// if a reconstruction is currently running...
	if (this->ReconstructionThreadId != -1)
	{
		int killingThread = this->ReconstructionThreadId;
		this->Threader->TerminateThread(killingThread);
		this->ReconstructionThreadId = -1;
		if (this->TrackerTool)
		{
			// the vtkTrackerBuffer should be locked before changing or
			// accessing the data in the buffer if the buffer is being used from
			// multiple threads
			this->TrackerBuffer->DeepCopy(this->TrackerTool->GetBuffer());
		}
		// save this off so that we can grab slice parameters later
		if (this->VideoSource)
		{
			this->SetSlice(this->VideoSource->GetOutput());
		}

		this->UninitializeRealTimeReconstruction();
	}
}

//----------------------------------------------------------------------------
// Things to do before non-real-time reconstruction.  This is overridden in
// derived classes
int vtkFreehandUltrasound2::InitializeReconstruction()
{
    this->VideoBufferUid = this->VideoSource->GetBuffer()->GetOldestItemUidInBuffer(); 
	this->GetOutput()->Update();
	return 1;
}

//----------------------------------------------------------------------------
// Things to do after non real-time reconstruction - in the base class we do
// nothing!  This is overridden in derived classes
void vtkFreehandUltrasound2::UninitializeReconstruction()
{
	this->PreviousFanRotation = this->FanRotation;
}

//----------------------------------------------------------------------------
// Start doing a non-real-time reconstruction.
// Start doing a reconstruction from the video frames stored
// in the VideoSource buffer.  You should first use 'Seek'
// on the VideoSource to rewind first.  Then the reconstruction
// will advance through n frames one by one until the
// reconstruction is complete.  The reconstruction
// is performed in the background.
// TODO NOT currently implemented for gated reconstruction,
// and not tested for non-gated reconstruction since David wrote it:
// Use StartRealTimeReconstruction instead
PlusStatus vtkFreehandUltrasound2::StartReconstruction(int frames)
{
	if (frames <= 0)
	{
    LOG_ERROR("No frames are available");
		return PLUS_FAIL;
	}

	// If the real-time reconstruction isn't running...
	if (this->ReconstructionThreadId == -1)
	{
		//fprintf(stderr, "Reconstruction Start\n");
		this->RealTimeReconstruction = 0; // doing buffered reconstruction
		this->ReconstructionFrameCount = frames;

		// Setup the slice axes matrix
		vtkMatrix4x4 *matrix = vtkMatrix4x4::New();
		this->SetSliceAxes(matrix);
		matrix->Delete();
#ifdef DEBUG_RECONSTRUCTION
		this->Threader->SetNumberOfThreads(1);
#endif
		// initialize the reconstruction - this is overridden in derived classes
		if (this->InitializeReconstruction())
		{  
		  this->ReconstructionFinishedOff();    
			this->ReconstructionThreadId = \
				this->Threader->SpawnThread((vtkThreadFunctionType)\
				&vtkReconstructionThread,
				this);
		}
		else
		{
			LOG_ERROR("Could not initialize reconstruction ... stopping");
      return PLUS_FAIL;
		}
	}
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
// Stop non-real-time reconstruction
// Returns the number of frames remaining to be reconstructed
int vtkFreehandUltrasound2::StopReconstruction()
{
	// if a reconstruction is running...
	if (this->ReconstructionThreadId != -1)
	{
		this->Threader->TerminateThread(this->ReconstructionThreadId);
		this->ReconstructionThreadId = -1;
		return this->ReconstructionFrameCount;
	}
	// save this off so that we can grab slice parameters later
	if (this->VideoSource)
	{
		this->SetSlice(this->VideoSource->GetOutput());
	}
	this->UninitializeReconstruction();
	return 0;
}

//----------------------------------------------------------------------------
// Setup the output volume
void vtkFreehandUltrasound2::SetupOutputVolumes()
{
	// create the output objects via the VTK 5 pipeline
	this->SetNumberOfOutputPorts(1);
}

//----------------------------------------------------------------------------
// Setup the accumulation buffer
void vtkFreehandUltrasound2::SetupAccumulationBuffers()
{
	// if we are creating the accumulation buffers for the first time
	if (this->AccumulationBuffers == NULL && this->Compounding)
	{
		this->AccumulationBuffers = new vtkImageData*[1];
		this->AccumulationBuffers[0] = vtkImageData::New();
	}
}

//----------------------------------------------------------------------------
// Deletes the accumulation buffers
void vtkFreehandUltrasound2::DeleteAccumulationBuffers()
{
	if (this->AccumulationBuffers)
	{
		if (this->AccumulationBuffers[0])
		{
			this->AccumulationBuffers[0]->Delete();
		}
		delete [] this->AccumulationBuffers;
		this->AccumulationBuffers = NULL;
	}
}

//****************************************************************************
// RECONSTRUCTION - OPTIMIZED
//****************************************************************************

//----------------------------------------------------------------------------
// Given the output volume and the accumulation buffer (if compounding, otherwise
// can be null), execute the filter algorithm to fill the output with the input
// slice
// Optimized by splitting into x,y,z components or with integer math
void vtkFreehandUltrasound2::OptimizedInsertSlice(vtkImageData* outData, vtkImageData* accData)
{
	this->OptimizedInsertSliceHelper(outData, accData, this->GetSlice(), this->GetIndexMatrix());
}

//----------------------------------------------------------------------------
// Does the actual work of optimally inserting a slice, with optimization
// Basically, just calls Multithread()
void vtkFreehandUltrasound2::OptimizedInsertSliceHelper(vtkImageData* outData, // output volume
														vtkImageData* accData, // accumulation buffer
														vtkImageData *inData, // input slice
														vtkMatrix4x4* indexMatrix) // index matrix
{

	if (this->ReconstructionThreadId == -1)
	{
		this->UpdateInformation();
	}

	if (this->NeedsClear)
	{
		this->InternalClearOutput();
	}

	this->ActiveFlagLock->Lock();
	// if doing non-real-time reconstruction, update the slice here
	// (otherwise, the slice is updated in vtkReconstructionThread
	// to ensure synchronization with the tracking)
	if (this->ReconstructionThreadId == -1)
	{
		int clipExt[6];
		this->GetClipExtent(clipExt, inData->GetOrigin(), inData->GetSpacing(),
			inData->GetWholeExtent()); // TODO clip extent implementation -
		inData->SetUpdateExtentToWholeExtent(); // TODO clip extent implementation - should set this to clip extent, as in 3D Panoramic
		inData->Update();
	}
	this->ActiveFlagLock->Unlock();
	this->MultiThread(inData, outData, accData, indexMatrix);
	this->Modified();
}

//----------------------------------------------------------------------------
// This mess is really a simple function. All it does is call
// the ThreadedSliceExecute method after setting the correct
// extent for this thread. Its just a pain to calculate
// the correct extent.
VTK_THREAD_RETURN_TYPE vtkFreehand2ThreadedExecute( void *arg )
{
	vtkFreehand2ThreadStruct *str;
	// the input slice extent, the input slice extent
	// for this thread, and the total number of pieces
	// the extent can be split into (i.e. the number of
	// threads we should use)
	int ext[6], splitExt[6], total; 
	int threadId, threadCount; 
	vtkImageData *input; // the slice input

	threadId = static_cast<vtkMultiThreader::ThreadInfo *>(arg)->ThreadID;
	threadCount = static_cast<vtkMultiThreader::ThreadInfo *>(arg)->NumberOfThreads;

	str = static_cast<vtkFreehand2ThreadStruct *> (static_cast<vtkMultiThreader::ThreadInfo *>(arg)->UserData);

	input = str->Input;
	input->GetUpdateExtent(ext); // TODO clip extent implementation - whole extent or update extent?

	// execute the actual method with appropriate extent
	// first find out how many pieces the extent can be split into and calculate
	// the extent for this thread (the splitExt)
	total = str->Filter->SplitSliceExtent(splitExt, ext, threadId, threadCount);

	// if we can use this thread, then call ThreadedSliceExecute
	if (threadId < total)
	{
		str->Filter->ThreadedSliceExecute(str->Input, str->Output,
			splitExt, threadId, str->Accumulator, str->IndexMatrix);
	}
	// else
	//   {
	//   otherwise don't use this thread. Sometimes the threads dont
	//   break up very well and it is just as efficient to leave a 
	//   few threads idle.
	//   }

	return VTK_THREAD_RETURN_VALUE;
}

//----------------------------------------------------------------------------
// Setup the threader, to run vtkFreehandUltrasound2ThreadedExecute
void vtkFreehandUltrasound2::MultiThread(vtkImageData *inData, // input slice
										 vtkImageData *outData, // output volume
										 vtkImageData *accData, // accumulation buffer
										 vtkMatrix4x4 *indexMatrix) // index buffer
{
	// set up a vtkFreehand2ThreadStruct (defined above)
	vtkFreehand2ThreadStruct str;
	str.Filter = this;
	str.Input = inData;
	str.Output = outData;
	str.Accumulator = accData;
	str.IndexMatrix = indexMatrix;

	this->Threader->SetNumberOfThreads(this->NumberOfThreads);
	this->Threader->SetSingleMethod(vtkFreehand2ThreadedExecute, &str);
	this->Threader->SingleMethodExecute();
}

//----------------------------------------------------------------------------
// For streaming and threads.  Splits output update extent into num pieces.
// This method needs to be called num times.  Results must not overlap for
// consistent starting extent.  Subclass can override this method.
// This method returns the number of pieces resulting from a successful split.
// This can be from 1 to "total".  
// If 1 is returned, the extent cannot be split.
int vtkFreehandUltrasound2::SplitSliceExtent(int splitExt[6], // TODO clip extent implementation - check these extents
											 int startExt[6],
											 int num,
											 int total)
{
	int splitAxis; // the axis we should split along
	int min, max; // the min and max indices of the axis of interest

	LOG_DEBUG("SplitSliceExtent: ( " << startExt[0] << ", " 
		<< startExt[1] << ", "
		<< startExt[2] << ", " << startExt[3] << ", "
		<< startExt[4] << ", " << startExt[5] << "), " 
		<< num << " of " << total);

	// start with same extent
	memcpy(splitExt, startExt, 6 * sizeof(int));

	// determine which axis we should split along - preference is z, then y, then x
	// as long as we can possibly split along that axis (i.e. as long as z0 != z1)
	splitAxis = 2; // at the end, will show whether we split along the z(2), y(1) or x(0) axis
	min = startExt[4];
	max = startExt[5];
	while (min == max)
	{
		--splitAxis;
		// we cannot split if the input extent is something like [50, 50, 100, 100, 0, 0]!
		if (splitAxis < 0)
		{ 
			LOG_DEBUG("  Cannot Split");
			return 1;
		}
		min = startExt[splitAxis*2];
		max = startExt[splitAxis*2+1];
	}

	// determine the actual number of pieces that will be generated
	int range = max - min + 1;
	// split the range over the maximum number of threads
	int valuesPerThread = (int)ceil(range/(double)total);
	// figure out the largest thread id used
	int maxThreadIdUsed = (int)ceil(range/(double)valuesPerThread) - 1;
	// if we are in a thread that will work on part of the extent, then figure
	// out the range that this thread should work on
	if (num < maxThreadIdUsed)
	{
		splitExt[splitAxis*2] = splitExt[splitAxis*2] + num*valuesPerThread;
		splitExt[splitAxis*2+1] = splitExt[splitAxis*2] + valuesPerThread - 1;
	}
	if (num == maxThreadIdUsed)
	{
		splitExt[splitAxis*2] = splitExt[splitAxis*2] + num*valuesPerThread;
	}

	// return the number of threads used
	return maxThreadIdUsed + 1;
}

//----------------------------------------------------------------------------
// This method is passed a input and output region, and executes the filter
// algorithm to fill the output from the input.
// It just executes a switch statement to call the correct vtkOptimizedInsertSlice
// function for the region's data types.
void vtkFreehandUltrasound2::ThreadedSliceExecute(vtkImageData *inData, // input slice
												  vtkImageData *outData, // output volume
												  int inExt[6], // input extent (could be split for this thread)
												  int threadId, // current thread id
												  vtkImageData *accData, // accumulation buffer
												  vtkMatrix4x4* indexMatrix) // index matrix // TODO clip extent implementation - check extents here
{

	// get scalar pointers for extents
	void *inPtr = inData->GetScalarPointerForExtent(inExt);
	int *outExt = this->OutputExtent;
	void *outPtr = outData->GetScalarPointerForExtent(outExt);
	void *accPtr = NULL; 

	if (this->Compounding)
	{
		accPtr = accData->GetScalarPointerForExtent(outExt);
	}
	else
	{
		accPtr = NULL;
	}

	LOG_DEBUG("OptimizedInsertSlice: inData = " << inData << ", outData = " << outData);

	// this filter expects that input is the same type as output.
	if (inData->GetScalarType() != outData->GetScalarType())
	{
		LOG_ERROR("OptimizedInsertSlice: input ScalarType, " 
			<< inData->GetScalarType()
			<< ", must match out ScalarType "<<outData->GetScalarType());
		return;
	}

	// use fixed-point math for optimization level 2
	if (this->GetOptimization() == 2)
	{
		// change transform matrix so that instead of taking 
		// input coords -> output coords it takes output indices -> input indices
		fixed newmatrix[4][4]; // fixed because optimization = 2
		for (int i = 0; i < 4; i++)
		{
			newmatrix[i][0] = indexMatrix->GetElement(i,0);
			newmatrix[i][1] = indexMatrix->GetElement(i,1);
			newmatrix[i][2] = indexMatrix->GetElement(i,2);
			newmatrix[i][3] = indexMatrix->GetElement(i,3);
		}

		switch (inData->GetScalarType())
		{
		case VTK_SHORT:
			vtkOptimizedInsertSlice(this, outData, (short *)(outPtr), 
				(unsigned short *)(accPtr), 
				inData, (short *)(inPtr), 
				inExt, newmatrix, threadId);
			break;
		case VTK_UNSIGNED_SHORT:
			vtkOptimizedInsertSlice(this,outData,(unsigned short *)(outPtr),
				(unsigned short *)(accPtr), 
				inData, (unsigned short *)(inPtr), 
				inExt, newmatrix, threadId);
			break;
		case VTK_UNSIGNED_CHAR:
			vtkOptimizedInsertSlice(this, outData,(unsigned char *)(outPtr),
				(unsigned short *)(accPtr), 
				inData, (unsigned char *)(inPtr), 
				inExt, newmatrix, threadId);
			break;
		default:
			LOG_ERROR("OptimizedInsertSlice: Unknown input ScalarType");
			return;
		}
	}

	// if we are not using fixed point math for optimization = 2, we are either:
	// doing no optimization (0) OR
	// breaking into x, y, z components with no bounds checking for nearest neighbor (1)
	else
	{
		// change transform matrix so that instead of taking 
		// input coords -> output coords it takes output indices -> input indices
		double newmatrix[4][4];
		for (int i = 0; i < 4; i++)
		{
			newmatrix[i][0] = indexMatrix->GetElement(i,0);
			newmatrix[i][1] = indexMatrix->GetElement(i,1);
			newmatrix[i][2] = indexMatrix->GetElement(i,2);
			newmatrix[i][3] = indexMatrix->GetElement(i,3);
		}

		switch (inData->GetScalarType())
		{
		case VTK_SHORT:
			vtkOptimizedInsertSlice(this, outData, (short *)(outPtr), 
				(unsigned short *)(accPtr), 
				inData, (short *)(inPtr), 
				inExt, newmatrix, threadId);
			break;
		case VTK_UNSIGNED_SHORT:
			vtkOptimizedInsertSlice(this,outData,(unsigned short *)(outPtr),
				(unsigned short *)(accPtr), 
				inData, (unsigned short *)(inPtr), 
				inExt, newmatrix, threadId);
			break;
		case VTK_UNSIGNED_CHAR:
			vtkOptimizedInsertSlice(this, outData,(unsigned char *)(outPtr),
				(unsigned short *)(accPtr), 
				inData, (unsigned char *)(inPtr), 
				inExt, newmatrix, threadId);
			break;
		default:
			LOG_ERROR("OptimizedInsertSlice: Unknown input ScalarType");
			return;
		}
	}
}

//****************************************************************************
// RECONSTRUCTION - NOT OPTIMIZED
//****************************************************************************

//----------------------------------------------------------------------------
// Given the output volume and the accumulation buffer (if compounding, otherwise
// can be null), execute the filter algorithm to fill the output with the input
// slice
// Non-optimized version
void vtkFreehandUltrasound2::InsertSlice(vtkImageData* outData, vtkImageData* accData)
{
	// if we are optimizing by either splitting into x, y, z components or with
	// integer math, then run the optimized insert slice function instead
	if (this->GetOptimization())
	{
		this->OptimizedInsertSlice(outData, accData);
		return;
	}

	this->InsertSliceHelper(outData, accData, this->GetSlice(), this->GetIndexMatrix());
}

//----------------------------------------------------------------------------
// Does the actual work of inserting a slice without optimization
// Basically, just calls vtkFreehandUltrasound2InsertSlice()
void vtkFreehandUltrasound2::InsertSliceHelper(vtkImageData* outData, // output volume
											   vtkImageData* accData, // accumulation buffer
											   vtkImageData *inData, // input slice
											   vtkMatrix4x4 *indexMatrix) // index matrix
{

	if (this->ReconstructionThreadId == -1)
	{
		this->InternalExecuteInformation();
	}

	if (this->NeedsClear)
	{
		this->InternalClearOutput();
	}

	this->ActiveFlagLock->Lock();
	// if doing non-real-time reconstruction, update the slice here
	// (otherwise, the slice is updated in vtkReconstructionThread
	// to ensure synchronization with the tracking)
	if (this->ReconstructionThreadId == -1)
	{
		int clipExt[6];
		this->GetClipExtent(clipExt, inData->GetOrigin(), inData->GetSpacing(),
			inData->GetWholeExtent());
		inData->SetUpdateExtentToWholeExtent(); // TODO clip extent implementation - should set this to clip extent, as in 3D Panoramic
		inData->Update();
	}
	this->ActiveFlagLock->Unlock();

	// get our pointers, extents, etc for the input, output and accumulation buffer
	int *inExt = inData->GetWholeExtent(); // TODO clip extent implementation - whole extent or update extent?
	int *outExt = this->OutputExtent;
	void *inPtr = inData->GetScalarPointerForExtent(inExt);
	void *outPtr = outData->GetScalarPointerForExtent(outExt);
	void *accPtr = NULL;

	if (this->Compounding)
	{
		accPtr = accData->GetScalarPointerForExtent(outExt);
	}
	else
	{
		accPtr = NULL;
	}

	// this filter expects that input is the same type as the output
	if (inData->GetScalarType() != outData->GetScalarType())
	{
		LOG_ERROR("InsertSlice: input ScalarType, " 
			<< inData->GetScalarType()
			<< ", must match out ScalarType " 
			<< outData->GetScalarType());
		return;
	}

	// never fully implemented, so comment it out
	/*// copy IndexMatrix into LastIndexMatrix
	if (this->LastIndexMatrix == 0)
	{
	this->LastIndexMatrix = vtkMatrix4x4::New();
	}
	this->LastIndexMatrix->DeepCopy(indexMatrix);*/

	// Now we will actually insert the slice...
	switch (inData->GetScalarType())
	{
	case VTK_SHORT:
		vtkFreehandUltrasound2InsertSlice(this, outData, (short *)(outPtr), 
			(unsigned short *)(accPtr), 
			inData, (short *)(inPtr), 
			inExt, indexMatrix);
		break;
	case VTK_UNSIGNED_SHORT:
		vtkFreehandUltrasound2InsertSlice(this,outData,(unsigned short *)(outPtr),
			(unsigned short *)(accPtr), 
			inData, (unsigned short *)(inPtr), 
			inExt, indexMatrix);
		break;
	case VTK_UNSIGNED_CHAR:
		vtkFreehandUltrasound2InsertSlice(this, outData,(unsigned char *)(outPtr),
			(unsigned short *)(accPtr), 
			inData, (unsigned char *)(inPtr), 
			inExt, indexMatrix);
		break;
	default:
		LOG_ERROR("InsertSlice: Unknown input ScalarType");
		return;
	}
}

//****************************************************************************
// USED BY THE MAIN RECONSTRUCTION LOOP
//****************************************************************************

//----------------------------------------------------------------------------
// Calculate how much time to sleep for the reconstruction thread
// In the base class, rely on the video source for timing
// Override in derived classes
double vtkFreehandUltrasound2::GetSleepTime()
{
	return (1.0 / this->VideoSource->GetFrameRate()); // 0.033
}

//----------------------------------------------------------------------------
// Calculate the current time
// In the base class, rely on the video source for timing
// Override in derived classes
double vtkFreehandUltrasound2::CalculateCurrentVideoTime(vtkImageData* inData)
{
	return this->VideoSource->GetFrameTimeStamp();
}

//----------------------------------------------------------------------------
// Update the input slice - returns whether the slice has to be inserted now,
// even if it is not a new slice
// Override in derived classes
PlusStatus vtkFreehandUltrasound2::UpdateSlice(vtkImageData* inData, int& insertNow)
{
    insertNow = 0; 
    if ( this->RealTimeReconstruction )
    {
        inData->Update();
    }
    else
    {
        if ( this->VideoBufferUid <= this->VideoSource->GetBuffer()->GetLatestItemUidInBuffer() )
        {
            VideoBufferItem bufferItem; 
            if ( this->VideoSource->GetBuffer()->GetVideoBufferItem( this->VideoBufferUid, &bufferItem) != PLUS_SUCCESS )
            {
                LOG_ERROR("Failed to get video item from buffer with UID: " << this->VideoBufferUid ); 
                return PLUS_FAIL; 
            }

            UsImageConverterCommon::ConvertItkImageToVtkImage(bufferItem.GetFrame(), inData); 
            inData->Modified(); 

            this->VideoBufferUid++; 
        }
        else
        {
            LOG_WARNING("We've reached the last frame in the buffer..."); 
            return PLUS_FAIL; 
        }
    }
  return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
// Update the accumulation buffers
// Override in derived classes
void vtkFreehandUltrasound2::UpdateAccumulationBuffers()
{
	vtkImageData* accData = this->GetAccumulationBuffer();
	if (this->Compounding && accData)
	{
		accData->SetUpdateExtentToWholeExtent(); // TODO clip extent implementation - clip extent?
		accData->Update();
	}
}

//----------------------------------------------------------------------------
// Inserts a single slice into the volume
// Override in derived classes
void vtkFreehandUltrasound2::ReconstructSlice(double timestamp, vtkImageData* inData)
{
	// note that this will call OptimimizedInsertSlice for you if you are using
	// optimization
	this->InsertSlice(this->GetOutput(), this->GetAccumulationBuffer());
}

//----------------------------------------------------------------------------
// Inserts an old slice (i.e. from memory or a buffer) before we work on the
// current slice - the base class doesn't support this though
// Override in derived classes
void vtkFreehandUltrasound2::ReconstructOldSlice(double timestamp, vtkImageData* inData)
{
	return;
}

//****************************************************************************
// VTK 5 PIPELINE
//****************************************************************************

//----------------------------------------------------------------------------
// Account for the MTime of the transform and its matrix when determining
// the MTime of the filter
// Choose the largest MTime of the superclass (vtkImageAlgorithm), the slice
// transform, or the transform's matrix
// [David's note to self: this made sense in vtkImageReslice, but does it make
//  any sense here?]
unsigned long int vtkFreehandUltrasound2::GetMTime()
{
	unsigned long mTime=this->Superclass::GetMTime();
	unsigned long time;

	if ( this->SliceTransform != NULL )
	{
		time = this->SliceTransform->GetMTime();
		mTime = ( time > mTime ? time : mTime );
		time = this->SliceTransform->GetMatrix()->GetMTime();
		mTime = ( time > mTime ? time : mTime );    
	}

	return mTime;
}

//----------------------------------------------------------------------------
// Define the input port information - import port 0 needs image data
int  vtkFreehandUltrasound2::FillInputPortInformation(
	int port, vtkInformation* info)
{
	if (port == 0)
	{
		info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkImageData");
	}

	return 0;
}

//----------------------------------------------------------------------------
// Define the output port information - all output ports produce vtkImageData
int vtkFreehandUltrasound2::FillOutputPortInformation(
	int vtkNotUsed(port), vtkInformation* info)
{
	info->Set(vtkDataObject::DATA_TYPE_NAME(), "vtkImageData");
	return 1;
}

//----------------------------------------------------------------------------
// We make sure that the REQUEST_DATA_NOT_GENERATED is set so that the data
// object is not initialized everytime an update is called
int vtkFreehandUltrasound2::ProcessRequest(vtkInformation* request,
										   vtkInformationVector** inputVector,
										   vtkInformationVector* outputVector)
{
	if(request->Has(vtkDemandDrivenPipeline::REQUEST_DATA_NOT_GENERATED()))
	{
		// Mark all outputs as not generated so that the executive does
		// not try to handle initialization/finalization of the outputs.
		// We will do it here.
		int phase;
		for (phase=0; phase < outputVector->GetNumberOfInformationObjects(); ++phase)
		{
			vtkInformation* outInfo = outputVector->GetInformationObject(phase);
			outInfo->Set(vtkDemandDrivenPipeline::DATA_NOT_GENERATED(), 1);
		}
	}

	// Calls to RequestInformation, RequestUpdateExtent and RequestData are
	// handled here, in vtkImageAlgorithm's ProcessRequest
	return this->Superclass::ProcessRequest(request, inputVector, outputVector);
}

//----------------------------------------------------------------------------
// Specify what the output data will look like.
// Whole extent, spacing and origin comes from user specifications for the
// output volume
// Scalar type and number of scalar components come from the input slice
// Also updates NeedsClear if anything has changed
int vtkFreehandUltrasound2::RequestInformation(
	vtkInformation* vtkNotUsed(request),
	vtkInformationVector** vtkNotUsed(inInfo),
	vtkInformationVector* outInfoVector)
{

	// to avoid conflict between the main application thread and the
	// realtime reconstruction thread
	if (this->ReconstructionThreadId == -1)
	{
		if (this->Compounding)
		{
			this->RequestInformationHelper(outInfoVector->GetInformationObject(0), this->GetOutput(), this->GetAccumulationBuffer());
		}
		else
		{
			this->RequestInformationHelper(outInfoVector->GetInformationObject(0), this->GetOutput(), NULL);
		}
	}
	return 1;
}

//----------------------------------------------------------------------------
// Does the actual work for request information
// Note very similar to InternalExecuteInformationHelper, but not combined
// into one function because of different "if" structure
void vtkFreehandUltrasound2::RequestInformationHelper(
	vtkInformation *outInfo,
	vtkImageData* getOutputData,
	vtkImageData* accData)
{

	vtkImageData *output = dynamic_cast<vtkImageData *>(outInfo->Get(vtkDataObject::DATA_OBJECT()));

	// get the old paramters - will compare with the current parameters to make sure
	// nothing's changed
	int oldwholeextent[6];
	vtkFloatingPointType oldspacing[3];
	vtkFloatingPointType oldorigin[3];
	int oldtype = output->GetScalarType();
	int oldncomponents = output->GetNumberOfScalarComponents();
	output->GetWholeExtent(oldwholeextent);
	output->GetSpacing(oldspacing);
	output->GetOrigin(oldorigin);

	// if we don't have a slice yet, then set the slice to be the output of the video source
	if (this->GetVideoSource())
	{
		if (this->GetSlice() == 0)
		{
			this->SetSlice(this->GetVideoSource()->GetOutput());
		} 
	} 

	// if we have a slice now...
	if (this->GetSlice())
	{
		// get the newest slice information - updating origin/spacing/extent from pipeline
		this->GetSlice()->UpdateInformation();

		vtkDataObject::SetPointDataActiveScalarInfo(outInfo, this->GetSlice()->GetScalarType(), this->GetSlice()->GetNumberOfScalarComponents()+1);

		// set up the output information here
		output->SetScalarType(this->GetSlice()->GetScalarType());
		output->SetNumberOfScalarComponents(this->GetSlice()->GetNumberOfScalarComponents()+1);
		outInfo->Set(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT(), this->OutputExtent, 6);
		outInfo->Set(vtkDataObject::SPACING(), this->OutputSpacing, 3);
		outInfo->Set(vtkDataObject::ORIGIN(), this->OutputOrigin, 3);
		output->SetExtent(this->OutputExtent);
		output->SetWholeExtent(this->OutputExtent);
		output->SetSpacing(this->OutputSpacing);
		output->SetOrigin(this->OutputOrigin);

		// if the output has changed, then we need to clear
		if (oldtype != output->GetScalarType() ||
			oldncomponents != output->GetNumberOfScalarComponents() ||
			oldwholeextent[0] != this->OutputExtent[0] ||
			oldwholeextent[1] != this->OutputExtent[1] ||
			oldwholeextent[2] != this->OutputExtent[2] ||
			oldwholeextent[3] != this->OutputExtent[3] ||
			oldwholeextent[4] != this->OutputExtent[4] ||
			oldwholeextent[5] != this->OutputExtent[5] ||
			oldspacing[0] != this->OutputSpacing[0] ||
			oldspacing[1] != this->OutputSpacing[1] ||
			oldspacing[2] != this->OutputSpacing[2] ||
			oldorigin[0] != this->OutputOrigin[0] ||
			oldorigin[1] != this->OutputOrigin[1] ||
			oldorigin[2] != this->OutputOrigin[2])
		{
			this->NeedsClear = 1;
		}

		// if we are compounding, then adjust the accumulation buffer too
		if (accData)
		{
			int *extent = accData->GetExtent();
			accData->SetWholeExtent(this->OutputExtent);
			accData->SetExtent(this->OutputExtent);
			accData->SetSpacing(this->OutputSpacing);
			accData->SetOrigin(this->OutputOrigin);
			accData->SetScalarType(getOutputData->GetScalarType());
			accData->SetUpdateExtent(this->OutputExtent);
			accData->Update();

			// if the accumulation buffer has changed, we need to clear
			if (extent[0] != this->OutputExtent[0] ||
				extent[1] != this->OutputExtent[1] ||
				extent[2] != this->OutputExtent[2] ||
				extent[3] != this->OutputExtent[3] ||
				extent[4] != this->OutputExtent[4] ||
				extent[5] != this->OutputExtent[5])
			{
				this->NeedsClear = 1;
			}

		}
	}
}

//----------------------------------------------------------------------------
// Gets the output ready to receive data, so we need to call it before the
// reconstruction starts.  Updates the information for the output and the
// accumulation buffer
// Whole extent, spacing and origin comes from user specifications for the
// output volume
// Scalar type and number of scalar components come from the input slice
// Also updates NeedsClear if anything has changed
void vtkFreehandUltrasound2::InternalExecuteInformation()
{
	if (this->Compounding)
	{
		this->InternalExecuteInformationHelper(this->GetOutput(), this->GetAccumulationBuffer());
	}
	else
	{
		this->InternalExecuteInformationHelper(this->GetOutput(), NULL);
	}
}

//----------------------------------------------------------------------------
//Does the actual work for internal execute information
// Note very similar to RequestInformationHelper, but not combined
// into one function because of different "if" structure
void vtkFreehandUltrasound2::InternalExecuteInformationHelper(vtkImageData *output, vtkImageData *accData)
{
	vtkInformation *outInfo = output->GetPipelineInformation();

	// get the old paramters - will compare with the current parameters to make sure
	// nothing's changed
	int oldwholeextent[6];
	vtkFloatingPointType oldspacing[3];
	vtkFloatingPointType oldorigin[3];
	int oldtype = output->GetScalarType();
	int oldncomponents = output->GetNumberOfScalarComponents();
	output->GetWholeExtent(oldwholeextent);
	output->GetSpacing(oldspacing);
	output->GetOrigin(oldorigin);

	// if we don't have a slice yet, then set the slice to be the output of the video source
	if (this->GetVideoSource())
	{
		if (this->GetSlice() == 0)
		{
			this->SetSlice( this->GetVideoSource()->GetOutput());
		}
	}

	// if we have a slice now...
	if (this->GetSlice())
	{
		this->GetSlice()->UpdateInformation();
	}    

	// set up the output information here
	output->SetScalarType(this->GetSlice()->GetScalarType());
	output->SetNumberOfScalarComponents(this->GetSlice()->GetNumberOfScalarComponents()+1);
	outInfo->Set(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT(), this->OutputExtent, 6);
	outInfo->Set(vtkDataObject::SPACING(), this->OutputSpacing, 3);
	outInfo->Set(vtkDataObject::ORIGIN(), this->OutputOrigin, 3);
	output->SetExtent(this->OutputExtent);
	output->SetWholeExtent(this->OutputExtent);
	output->SetSpacing(this->OutputSpacing);
	output->SetOrigin(this->OutputOrigin);

	// if the output has changed, then we need to clear
	if (oldtype != output->GetScalarType() ||
		oldncomponents != output->GetNumberOfScalarComponents() ||
		oldwholeextent[0] != this->OutputExtent[0] ||
		oldwholeextent[1] != this->OutputExtent[1] ||
		oldwholeextent[2] != this->OutputExtent[2] ||
		oldwholeextent[3] != this->OutputExtent[3] ||
		oldwholeextent[4] != this->OutputExtent[4] ||
		oldwholeextent[5] != this->OutputExtent[5] ||
		oldspacing[0] != this->OutputSpacing[0] ||
		oldspacing[1] != this->OutputSpacing[1] ||
		oldspacing[2] != this->OutputSpacing[2] ||
		oldorigin[0] != this->OutputOrigin[0] ||
		oldorigin[1] != this->OutputOrigin[1] ||
		oldorigin[2] != this->OutputOrigin[2])
	{
		this->NeedsClear = 1;
	}

	// if we are compounding, then adjust the accumulation buffer too
	if (accData)
	{
		int *extent = accData->GetExtent();
		accData->SetWholeExtent(this->OutputExtent);
		accData->SetExtent(this->OutputExtent);
		accData->SetSpacing(this->OutputSpacing);
		accData->SetOrigin(this->OutputOrigin);
		accData->SetScalarType(output->GetScalarType());
		accData->SetUpdateExtent(this->OutputExtent);
		accData->Update();

		// if the accumulation buffer has changed, we need to clear
		if (extent[0] != this->OutputExtent[0] ||
			extent[1] != this->OutputExtent[1] ||
			extent[2] != this->OutputExtent[2] ||
			extent[3] != this->OutputExtent[3] ||
			extent[4] != this->OutputExtent[4] ||
			extent[5] != this->OutputExtent[5])
		{
			this->NeedsClear = 1;
		}
	}
}

//----------------------------------------------------------------------------
// Asks for the input update extent necessary to produce a given output
// update extent.  Sets the update extent of the input information object
// to equal the whole extent of hte input information object - we need the
// entire whole extent of the input data object to generate the output
int  vtkFreehandUltrasound2::RequestUpdateExtent(
	vtkInformation *vtkNotUsed(request),
	vtkInformationVector **inputVector,
	vtkInformationVector *vtkNotUsed(outputVector))
{

	// TODO - hacked

	// This original code dies - input vector is null
	// Set the update extent of the input information object to equal the
	// whole extent of the input information object
	/*int inExt[6];
	vtkInformation *inInfo = inputVector[0]->GetInformationObject(0); 
	inInfo->Get(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT(), inExt); // get the whole extent of inInfo
	inInfo->Set(vtkStreamingDemandDrivenPipeline::UPDATE_EXTENT(), inExt, 6); // se the update extent of inInfo
	*/

	return 1;
}

//----------------------------------------------------------------------------
// This doesn't really do much, because this is an unconventional image filter
// In most VTK classes this method is responsible for calling Execute, but since
// the output data has already been generated it just fools the pipeline into
// thinking that Execute has been called
int vtkFreehandUltrasound2::RequestData(vtkInformation* request,
										vtkInformationVector **vtkNotUsed(inInfo),
										vtkInformationVector* outInfo)
{
	for (int i = 0; i < this->GetNumberOfOutputPorts(); i++)
	{

		vtkDataObject *outObject = 
			outInfo->GetInformationObject(i)->Get(vtkDataObject::DATA_OBJECT());

		// if we are not running a real time reconstruction and we need to clear,
		// then clear
		if (this->ReconstructionThreadId == -1 && this->NeedsClear == 1)
		{
			this->InternalClearOutput();
		}

		// This would have been done already in the call to ProcessRequest, so don't do it here
		outInfo->GetInformationObject(i)->Set(vtkDemandDrivenPipeline::DATA_NOT_GENERATED(), 1);

		// Set the flag for the data object associated with port 0 that data has been generated -
		// sets the data released flag to 0 and sets a new update time
		((vtkImageData *)outObject)->DataHasBeenGenerated();
	}

	return 1;
}

//----------------------------------------------------------------------------
// Compute the modified time for the pipeline - just returns the Mtime of the
// input slice
int vtkFreehandUltrasound2::ComputePipelineMTime(
	vtkInformation *vtkNotUsed(request),
	vtkInformationVector **vtkNotUsed(inInfoVec),
	vtkInformationVector *vtkNotUsed(outInfoVec),
	int requestFromOutputPort,
	unsigned long* mtime)
{
	if (this->GetSlice())
	{
		*mtime = this->GetSlice()->GetPipelineMTime(); 
	}
	return 1;
}

//****************************************************************************
// ROTATING PROBES
//****************************************************************************

//----------------------------------------------------------------------------
// Set whether we want to use a rotating probe
void vtkFreehandUltrasound2::SetRotating(int probe)
{
	if (this->ReconstructionThreadId != -1)
	{
		return;
	}

	this->Rotating = probe;

	// turning on rotation
	if (probe)
	{
		this->RotationClipper->ClipDataOn();
		this->RotationThresholder->ThresholdBetween(this->RotatingThreshold1, this->RotatingThreshold2);
		this->RotationThresholder->SetOutValue(1);
		this->RotationThresholder->SetInValue(0);
	}
	else
	{
		// turn off rotation and set to zero
		this->PreviousFanRotation = this->FanRotation;
		this->FanRotation = 0;
	}
}

//----------------------------------------------------------------------------
// Change the current fan rotation
void vtkFreehandUltrasound2::SetFanRotation(int rot)
{
	if (this->Rotating)
	{
		this->FanRotation = rot;
	}
}

//----------------------------------------------------------------------------
// Change the previous fan rotation
void vtkFreehandUltrasound2::SetPreviousFanRotation(int rot)
{
	if (this->Rotating)
	{
		this->PreviousFanRotation = rot;
	}
}

//----------------------------------------------------------------------------
// Sets the lower bound for the thresholder used to calculate rotation
void vtkFreehandUltrasound2::SetRotatingThreshold1(int thresh)
{
	this->RotatingThreshold1 = thresh;
	if (this->RotationThresholder)
	{
		this->RotationThresholder->ThresholdBetween(this->RotatingThreshold1, this->RotatingThreshold2);
		this->RotationThresholder->SetOutValue(1);
		this->RotationThresholder->SetInValue(0);
	}
}

//----------------------------------------------------------------------------
// Sets the upper bound for the thresholder used to calculate rotation
void vtkFreehandUltrasound2::SetRotatingThreshold2(int thresh)
{
	this->RotatingThreshold2 = thresh;
	if (this->RotationThresholder)
	{
		this->RotationThresholder->ThresholdBetween(this->RotatingThreshold1, this->RotatingThreshold2);
		this->RotationThresholder->SetOutValue(1);
		this->RotationThresholder->SetInValue(0);
	}
}

//----------------------------------------------------------------------------
// Does the actual thresholding for rotation detection
int vtkFreehandUltrasound2::GetFanRepresentationHelper(int x, int y)
{
	return this->RotationThresholder->GetOutput()->GetScalarComponentAsFloat(x, 479-y, 0, 0);
}

//----------------------------------------------------------------------------
// Returns an integer corresponding to the rotation value shown in the pixels
// for a single digit within the grabbed ultrasound frame.
// Returns -1 if it could not interpret the pixels
int vtkFreehandUltrasound2::GetFanRepresentation (int x, int y)
{
	int B = 0;
	int W = 1;
	int id1, id2, id3, id4;

	id1 = this->GetFanRepresentationHelper(x,y);
	id2 = this->GetFanRepresentationHelper(x-1,y);

	if (id1 == W && id2 == W)
	{
		//0,3,5,6,8,9
		id1 = this->GetFanRepresentationHelper(x-2,y+2);
		id2 = this->GetFanRepresentationHelper(x-2,y+3);
		if (id1 == W && id2 == W)
		{
			// 0,6,8
			id1 = this->GetFanRepresentationHelper(x+3,y-1);
			id2 = this->GetFanRepresentationHelper(x+3,y-2);
			if (id1 == W && id2 == W) {
				// 8, 0
				id1 = this->GetFanRepresentationHelper(x+1,y+3);
				id2 = this->GetFanRepresentationHelper(x+1,y-1);
				id3 = this->GetFanRepresentationHelper(x+4,y+1);
				id4 = this->GetFanRepresentationHelper(x-4,y);
				if (id1==B && id2==W && (id3==W || id4==W)) { return 0; }
				else if (id1==B && id2==B && id3==B) { return 8; }
				else { return -1; }  
			} 
			else if (id1==B && id2==B) { return 6; }
			else { return -1; }
		}
		else {
			//3,5,9

			id1 = this->GetFanRepresentationHelper(x-3,y-1);
			id2 = this->GetFanRepresentationHelper(x-3,y);
			id3 = this->GetFanRepresentationHelper(x+3,y-2);
			id4 = this->GetFanRepresentationHelper(x-2,y+2);
			if (id1==B && id2==B && id3==W) { return 3; }
			else if (id1==W && id2==W && id3==B) { 
				id1 = this->GetFanRepresentationHelper(x,y+1);
				id2 = this->GetFanRepresentationHelper(x-1,y+1);
				if (id1==W && id2==W) { return 5; }
				else { return -1; }
			}
			else if (id1==W && id2==W && id4==B) { return 9; }
			else { return -1;}
		}
	}
	else {
		//1,2,4,7 
		id1 = this->GetFanRepresentationHelper(x-1,y-4);
		id2 = this->GetFanRepresentationHelper(x-2,y-4);
		if (id1==W && id2==W) {
			//2,7
			id1 = this->GetFanRepresentationHelper(x+1,y+4);
			id2 = this->GetFanRepresentationHelper(x+1,y+5);

			if (id1==W && id2==W){ return 2; }
			else if (id1==B && id2==B) { return 7; }
			else { return -1; }
		} else {
			// 1,4
			id1 = this->GetFanRepresentationHelper(x-3,y+0);
			id2 = this->GetFanRepresentationHelper(x-3,y+1);
			id3 = this->GetFanRepresentationHelper(x-4,y-3);
			if (id1==W && id2==W && id3==B) {  return 4; }
			else if (id1==B && id2==B && id3==B) {                 
				id1 = this->GetFanRepresentationHelper(x+3,y+4);
				id2 = this->GetFanRepresentationHelper(x+3,y+5);
				if (id1==W && id2==W) { return 1; }
				else { return -1; }
			} else { return -1; }
		}
	}
	return -1;
}

//----------------------------------------------------------------------------
// CalculateFanRotationValue
// Finds the current fan rotation based on the frame grabbed US image
int vtkFreehandUltrasound2::CalculateFanRotationValue()
{

	// rotation digits (ex for rotation 158, d1 = 1, d2 = 5, d3 = 8)
	int d1, d2, d3;
	int array3x, array3y;
	int array2x, array2y;
	int array1x, array1y;
	int xShift = this->GetRotatingShiftX();
	int yShift = this->GetRotatingShiftY();

	// not flipped
	if (this->GetImageFlipped() == 0)
	{
		array3x = 73+xShift;
		array2x = 63+xShift;
		array1x = 53+xShift;
		array3y = 300+yShift;
		array2y = array3y;
		array1y = array3y;
	}
	// flipped
	else
	{
		array3x = 503+xShift;
		array2x = 493+xShift;
		array1x = 483+xShift;
		array3y = 132+yShift;
		array2y = array3y;
		array1y = array3y;
	}

	d3 = this->GetFanRepresentation(array3x, array3y);
	d2 = this->GetFanRepresentation(array2x, array2y);
	d1 = this->GetFanRepresentation(array1x, array1y);

	// combine rotation digits to return the rotation

	if (d3 >= 0)
	{
		if (d2 >=0)
		{
			if (d1 >=0)
			{
				return d1*100+d2*10+d3;
			}
			else
			{
				return d2*10+d3;
			}
		}
		else
		{
			return d3;
		}
	}
	else
	{
		return -1;
	}
}

//****************************************************************************
// FILLING HOLES
//****************************************************************************

//----------------------------------------------------------------------------
// Does the actual hole filling
template <class T>
static void vtkFreehandUltrasound2FillHolesInOutput(vtkFreehandUltrasound2 *self,
													vtkImageData *outData,
													T *outPtr,
													unsigned short *accPtr,
													int outExt[6])
{
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
// This method is passed a input and output region, and executes the filter
// algorithm to fill the output from the input.
// It just executes a switch statement to call the correct
// vtkFreehandUltrasound2FillHolesInOutput function for the region's data types.
void vtkFreehandUltrasound2::ThreadedFillExecute(vtkImageData *outData,  // output volume
												 int outExt[6], // output extent
												 int threadId, // current thread id
												 vtkImageData *accData) // accumulation buffer
{
	// get scalar pointers for extents
	void *outPtr = outData->GetScalarPointerForExtent(outExt);
	void *accPtr = NULL;

	if (this->Compounding)
	{
		accPtr = accData->GetScalarPointerForExtent(outExt);
	}

	switch (outData->GetScalarType())
	{
	case VTK_SHORT:
		vtkFreehandUltrasound2FillHolesInOutput(
			this, outData, (short *)(outPtr), 
			(unsigned short *)(accPtr), outExt);
		break;
	case VTK_UNSIGNED_SHORT:
		vtkFreehandUltrasound2FillHolesInOutput(
			this, outData, (unsigned short *)(outPtr),
			(unsigned short *)(accPtr), outExt);
		break;
	case VTK_UNSIGNED_CHAR:
		vtkFreehandUltrasound2FillHolesInOutput(
			this, outData,(unsigned char *)(outPtr),
			(unsigned short *)(accPtr), outExt); 
		break;
	default:
		LOG_ERROR("FillHolesInOutput: Unknown input ScalarType");
		return;
	}
}

//----------------------------------------------------------------------------
// This mess is really a simple function. All it does is call
// the ThreadedExecute method after setting the correct
// extent for this thread.  Its just a pain to calculate
// the correct extent.
VTK_THREAD_RETURN_TYPE vtkFreehand2ThreadedFillExecute( void *arg )
{
	vtkFreehand2ThreadStruct *str;
	// the input slice extent, the input slice extent
	// for this thread, and the total number of pieces
	// the extent can be split into (i.e. the number of
	// threads we should use)
	int ext[6], splitExt[6], total;
	int threadId, threadCount;
	vtkImageData *output;

	threadId = static_cast<vtkMultiThreader::ThreadInfo *>(arg)->ThreadID;
	threadCount = static_cast<vtkMultiThreader::ThreadInfo *>(arg)->NumberOfThreads;

	str = static_cast<vtkFreehand2ThreadStruct *> (static_cast<vtkMultiThreader::ThreadInfo *>(arg)->UserData);

	output = str->Output;
	output->GetExtent(ext);  // TODO clip extent implementation - whole extent or update extent?

	// execute the actual method with appropriate extent
	// first find out how many pieces the extent can be split into and calculate
	// the extent for this thread (the splitExt)
	total = str->Filter->SplitSliceExtent(splitExt, ext, threadId, threadCount);

	// if we can use this thread, then call ThreadedFillExecute
	if (threadId < total)
	{
		str->Filter->ThreadedFillExecute(str->Output, splitExt, threadId, str->Accumulator);
	}
	// else
	//   {
	//   otherwise don't use this thread. Sometimes the threads dont
	//   break up very well and it is just as efficient to leave a 
	//   few threads idle.
	//   }

	return VTK_THREAD_RETURN_VALUE;
}

//----------------------------------------------------------------------------
// Setup the threader, to run vtkFreehandUltrasound2ThreadedFillExecute
void vtkFreehandUltrasound2::MultiThreadFill(vtkImageData *outData, vtkImageData *accData)
{
	vtkFreehand2ThreadStruct str;

	// set up a vtkFreehand2ThreadStruct (defined above)
	str.Filter = this;
	str.Input = 0;
	str.Output = outData;
	str.Accumulator = accData;

	// run vtkFreehand2ThreadedFillExecute
	this->Threader->SetNumberOfThreads(this->NumberOfThreads);
	this->Threader->SetSingleMethod(vtkFreehand2ThreadedFillExecute, &str);
	this->Threader->SingleMethodExecute();
}

//----------------------------------------------------------------------------
// Fills holes in the output by using the weighted average of the surrounding
// voxels (see David Gobbi's thesis)
// Basically, just calls MultiThreadFill()
void vtkFreehandUltrasound2::FillHolesInOutput()
{
	this->UpdateInformation();
	if (this->NeedsClear)
	{
		this->InternalClearOutput();
	}

	vtkImageData *outData = this->GetOutput();
	vtkImageData *accData = this->GetAccumulationBuffer();
	this->MultiThreadFill(outData, accData);
	this->Modified(); 
}


//****************************************************************************
// I/O
//****************************************************************************

//----------------------------------------------------------------------------
// Get the XML element describing the freehand object
vtkXMLDataElement* vtkFreehandUltrasound2::MakeXMLElement()
{
	vtkXMLDataElement* elem = vtkXMLDataElement::New();
	elem->SetName("Freehand");

	// input slice parameters
	vtkImageData *image = this->GetSlice();
	vtkXMLDataElement* sliceParams = vtkXMLDataElement::New();
	sliceParams->SetName("SliceParameters");
	if (image)
	{
		image->UpdateInformation();
		sliceParams->SetVectorAttribute("SliceSpacing", 3, image->GetSpacing());
		sliceParams->SetVectorAttribute("SliceOrigin", 3, image->GetOrigin());
		sliceParams->SetVectorAttribute("SliceExtent", 6, image->GetExtent());
		sliceParams->SetIntAttribute("NumScalarComponents", image->GetNumberOfScalarComponents());
		//sliceParams->SetAttribute("ScalarType", image->GetScalarTypeAsString());
		//sliceParams->SetVectorAttribute("SliceUpdateExtent", 6, image->GetUpdateExtent());
		//sliceParams->SetVectorAttribute("SliceWholeExtent", 6, image->GetWholeExtent());
	}
	elem->AddNestedElement(sliceParams);

	// output parameters
	vtkXMLDataElement* outputParams = vtkXMLDataElement::New();
	outputParams->SetName("OutputParameters");
	outputParams->SetVectorAttribute("OutputSpacing", 3, this->OutputSpacing);
	outputParams->SetVectorAttribute("OutputOrigin", 3, this->OutputOrigin);
	outputParams->SetVectorAttribute("OutputExtent", 6, this->OutputExtent);
	elem->AddNestedElement(outputParams);

	// clipping parameters
	vtkXMLDataElement* clipParams = vtkXMLDataElement::New();
	clipParams->SetName("ClippingParameters");
	clipParams->SetVectorAttribute("ClipRectangle", 4, this->ClipRectangle);
	elem->AddNestedElement(clipParams);

	// fan parameters
	vtkXMLDataElement* fanParams = vtkXMLDataElement::New();
	fanParams->SetName("FanParameters");
	fanParams->SetVectorAttribute("FanAngles", 2, this->FanAngles);
	fanParams->SetVectorAttribute("FanOrigin", 2, this->FanOrigin);
	fanParams->SetDoubleAttribute("FanDepth", this->FanDepth);
	elem->AddNestedElement(fanParams);

	// reconstruction options
	vtkXMLDataElement* reconOptions = vtkXMLDataElement::New();
	reconOptions->SetName("ReconstructionOptions");
	reconOptions->SetAttribute("Interpolation", this->GetInterpolationModeAsString());
	(this->Optimization ? reconOptions->SetAttribute("Optimization", "On") : reconOptions->SetAttribute("Optimization", "Off"));
	(this->Compounding ? reconOptions->SetAttribute("Compounding", "On") : reconOptions->SetAttribute("Compounding", "Off"));
	elem->AddNestedElement(reconOptions);

	// spatial calibration
	double* elements = new double[16];
	vtkXMLDataElement *spatialParams = vtkXMLDataElement::New();
	spatialParams->SetName("SpatialCalibration");
	if (this->TrackerTool)
	{
		vtkMatrix4x4::DeepCopy(elements, this->TrackerTool->GetCalibrationMatrix());
		spatialParams->SetVectorAttribute("CalibrationMatrix", 16, elements);
	}
	elem->AddNestedElement(spatialParams);

	// temporal calibration
	vtkXMLDataElement* temporalParams = vtkXMLDataElement::New();
	temporalParams->SetName("TemporalCalibration");
	temporalParams->SetDoubleAttribute("VideoLag", this->VideoLag);
	elem->AddNestedElement(temporalParams);

	// rotation options
	vtkXMLDataElement* rotationOptions = vtkXMLDataElement::New();
	rotationOptions->SetName("RotationOptions");
	(this->Rotating ? rotationOptions->SetAttribute("Rotation", "On") : rotationOptions->SetAttribute("Rotation", "Off"));
	rotationOptions->SetIntAttribute("MaxRotationChange", this->MaximumRotationChange);
	rotationOptions->SetIntAttribute("Threshold1", this->RotatingThreshold1);
	rotationOptions->SetIntAttribute("Threshold2", this->RotatingThreshold2);
	rotationOptions->SetIntAttribute("ShiftX", this->RotatingShiftX);
	rotationOptions->SetIntAttribute("ShiftY", this->RotatingShiftY);
	(this->ImageFlipped ? rotationOptions->SetAttribute("ImageFlipped", "On") : rotationOptions->SetAttribute("ImageFlipped", "Off"));
	elem->AddNestedElement(rotationOptions);

	// buffer options
	vtkXMLDataElement* bufferOptions = vtkXMLDataElement::New();
	bufferOptions->SetName("BufferOptions");
	if (this->VideoSource)
	{
		bufferOptions->SetVectorAttribute("FrameSize", 3, this->VideoSource->GetFrameSize());
		bufferOptions->SetDoubleAttribute("VideoFrameRate", this->VideoSource->GetFrameRate());
		bufferOptions->SetIntAttribute("VideoBufferSize", this->VideoSource->GetFrameBufferSize());
	}
	if (this->TrackerBuffer)
	{
		bufferOptions->SetIntAttribute("TrackerBufferSize", this->TrackerBuffer->GetBufferSize());
	}
	elem->AddNestedElement(bufferOptions);

	// clean up
	sliceParams->Delete();
	outputParams->Delete();
	clipParams->Delete();
	fanParams->Delete();
	reconOptions->Delete();
	delete [] elements;
	spatialParams->Delete();
	temporalParams->Delete();
	rotationOptions->Delete();
	bufferOptions->Delete();

	return elem;
}

//----------------------------------------------------------------------------
// Save the freehand parameters in the (relative!) directory specified.
// The directory will be created if it doesn't exist, and the following files
// will be written inside it:
// freehand_summary.xml - a file with the freehand parameters within it
void vtkFreehandUltrasound2::SaveSummaryFile(const char *directory)
{

	if (this->ReconstructionThreadId != -1)
	{
		return;
	}

	int res;
#ifdef _WIN32
	res = _mkdir(directory);
#else
	int mode = 0777;
	res = mkdir(directory, mode);
#endif

	char path[512];
#ifdef _WIN32
	sprintf(path, "%s\\%s", directory, "freehand_summary.xml");
#else
	sprintf(path, "%s/%s", directory, "freehand_summary.xml");
#endif

	// get the XML element that describes the freehand object and write to file
	vtkXMLDataElement* elem = this->MakeXMLElement();
	vtkXMLUtilities::WriteElementToFile(elem, path);

	// clean up
	elem->Delete();
}

//----------------------------------------------------------------------------
// Read the freehand parameters from the filename specified in the (relative!)
// directory
// File should have been created using SaveSummaryFile()
PlusStatus vtkFreehandUltrasound2::ReadSummaryFile(const char *filename)
{

	if (this->ReconstructionThreadId != -1)
	{
    LOG_ERROR("Reconstruction is still active");
		return PLUS_FAIL;
	}

	// read in the freehand information
	vtkSmartPointer<vtkXMLDataElement> elem = vtkXMLUtilities::ReadElementFromFile(filename);

	// check to make sure we have the right element
	if (elem == NULL)
	{
		LOG_ERROR("ReadRawData - invalid file " << filename);
		return PLUS_FAIL;
	}

	if (strcmp(elem->GetName(), "Freehand") != 0)
	{
		LOG_ERROR("ReadRawData - invalid file " << filename);
		return PLUS_FAIL;
	}

	// slice parameters
	vtkXMLDataElement* sliceParams = elem->FindNestedElementWithName("SliceParameters");
	if (sliceParams)
	{
		if (this->VideoSource == NULL)
		{
			this->VideoSource = vtkVideoSource2::New();
		}

		if (this->TrackerTool == NULL)
		{
			this->TrackerTool = vtkTrackerTool::New();
		}

		if (this->VideoSource)
		{
			sliceParams->GetVectorAttribute("SliceSpacing", 3, this->VideoSource->GetDataSpacing());
			sliceParams->GetVectorAttribute("SliceOrigin", 3, this->VideoSource->GetDataOrigin());
			int outputFormat;
			sliceParams->GetScalarAttribute("NumScalarComponents", outputFormat);
		}

	}

	// output parameters
	vtkXMLDataElement* outputParams = elem->FindNestedElementWithName("OutputParameters");
	if (outputParams)
	{
		outputParams->GetVectorAttribute("OutputSpacing", 3, this->OutputSpacing);
		outputParams->GetVectorAttribute("OutputOrigin", 3, this->OutputOrigin);
		outputParams->GetVectorAttribute("OutputExtent", 6, this->OutputExtent);
	}

	// clipping parameters
	vtkXMLDataElement *clipParams = elem->FindNestedElementWithName("ClippingParameters");
	if (clipParams)
	{
		clipParams->GetVectorAttribute("ClipRectangle", 4, this->ClipRectangle);
	}

	// fan parameters
	vtkXMLDataElement *fanParams = elem->FindNestedElementWithName("FanParameters");
	if (fanParams)
	{
		fanParams->GetVectorAttribute("FanAngles", 2, this->FanAngles);
		fanParams->GetVectorAttribute("FanOrigin", 2, this->FanOrigin);
		fanParams->GetScalarAttribute("FanDepth", this->FanDepth);
	}

	// reconstruction options
	vtkXMLDataElement *reconOptions = elem->FindNestedElementWithName("ReconstructionOptions");
	if (reconOptions)
	{
		if (reconOptions->GetAttribute("Interpolation"))
		{
			if (strcmp(reconOptions->GetAttribute("Interpolation"), "Linear") == 0)
			{
				this->SetInterpolationModeToLinear();
			}
			else if (strcmp(reconOptions->GetAttribute("Interpolation"), "NearestNeighbor") == 0)
			{
				this->SetInterpolationModeToNearestNeighbor();
			}
		}
		if (reconOptions->GetAttribute("Optimization"))
		{
			((strcmp(reconOptions->GetAttribute("Optimization"), "On") == 0) ? this->SetOptimization(2) : this->SetOptimization(0));
		}
		if (reconOptions->GetAttribute("Compounding"))
		{
			((strcmp(reconOptions->GetAttribute("Compounding"), "On") == 0) ? this->CompoundingOn() : this->CompoundingOff());
		}
	}

	// reconstruction transforms
	double* elements = new double[16];

	// spatial calibration
	vtkXMLDataElement *spatialParams = elem->FindNestedElementWithName( "SpatialCalibration" );
	if (spatialParams)
	{
		if (this->TrackerTool)
		{
			spatialParams->GetVectorAttribute( "CalibrationMatrix", 16, elements );

			vtkSmartPointer< vtkMatrix4x4 > mImageToTool = vtkSmartPointer< vtkMatrix4x4 >::New();
			mImageToTool->DeepCopy( elements );

			vtkSmartPointer< vtkTransform > tImageToTool = vtkSmartPointer< vtkTransform >::New();
			tImageToTool->PostMultiply();
			tImageToTool->Identity();
			tImageToTool->SetMatrix( mImageToTool );
			tImageToTool->Update();


			this->TrackerTool->GetCalibrationMatrix()->DeepCopy( tImageToTool->GetMatrix() );
		}
	}
	delete [] elements;

	// temporal parameters
	vtkXMLDataElement* temporalParams = elem->FindNestedElementWithName("TemporalCalibration");
	if (temporalParams)
	{
		temporalParams->GetScalarAttribute("VideoLag", this->VideoLag);
	}

	// rotation options
	int tempi;
	vtkXMLDataElement* rotationOptions = elem->FindNestedElementWithName("RotationOptions");
	if (rotationOptions)
	{
		if (rotationOptions->GetAttribute("Rotation"))
		{
			((strcmp(rotationOptions->GetAttribute("Rotation"), "On") == 0) ? this->RotatingOn() : this->RotatingOff());
		}
		rotationOptions->GetScalarAttribute("MaxRotatingChange", this->MaximumRotationChange);
		rotationOptions->GetScalarAttribute("Threshold1", tempi);
		this->SetRotatingThreshold1(tempi);
		rotationOptions->GetScalarAttribute("Threshold2", tempi);
		this->SetRotatingThreshold2(tempi);
		rotationOptions->GetScalarAttribute("ShiftX", tempi);
		this->SetRotatingShiftX(tempi);
		rotationOptions->GetScalarAttribute("ShiftY", tempi);
		this->SetRotatingShiftY(tempi);
		if(rotationOptions->GetAttribute("ImageFlipped"))
		{
			((strcmp(rotationOptions->GetAttribute("ImageFlipped"), "On") == 0) ? this->ImageFlippedOn() : this->ImageFlippedOff());
		}
	}

	// buffer options
	vtkXMLDataElement* bufferOptions = elem->FindNestedElementWithName("BufferOptions");
	double tempd = 0;
	if (bufferOptions)
	{
		if (this->VideoSource)
		{
			bufferOptions->GetVectorAttribute("FrameSize", 3, this->VideoSource->GetFrameSize());
			tempd = this->VideoSource->GetFrameRate();
			bufferOptions->GetScalarAttribute("VideoFrameRate", tempd);
			this->VideoSource->SetFrameRate(tempd);
			tempi = this->VideoSource->GetFrameBufferSize();
			bufferOptions->GetScalarAttribute("VideoBufferSize", tempi);
            if ( this->VideoSource->SetFrameBufferSize(tempi) != PLUS_SUCCESS )
            {
                LOG_ERROR("Failed to set video buffer size!"); 
            }
		}
		if (this->TrackerBuffer)
		{
			tempi = this->VideoSource->GetFrameBufferSize();
			bufferOptions->GetScalarAttribute("TrackerBufferSize", tempi);
			this->TrackerBuffer->SetBufferSize(tempi);
		}
	}

	return PLUS_SUCCESS;
}
