/*=========================================================================

Program:   AtamaiTracking for VTK
Module:    $RCSfile: vtkTrackerBuffer.cxx,v $
Creator:   David Gobbi <dgobbi@atamai.com>
Language:  C++
Author:    $Author: dgobbi $
Date:      $Date: 2008/06/14 21:16:18 $
Version:   $Revision: 1.6 $

==========================================================================

Copyright (c) 2000-2005 Atamai, Inc.

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
#include <stdio.h>
#include <stdlib.h>
#include <sstream>

#include "vtkTrackerBuffer.h"
#include "vtkMath.h"
#include "vtkMatrix4x4.h"
#include "vtkDoubleArray.h"
#include "vtkIntArray.h"
#include "vtkUnsignedLongLongArray.h"
#include "vtkCriticalSection.h"
#include "vtkObjectFactory.h"



//----------------------------------------------------------------------------
vtkTrackerBuffer* vtkTrackerBuffer::New()
{
	// First try to create the object from the vtkObjectFactory
	vtkObject* ret = vtkObjectFactory::CreateInstance("vtkTrackerBuffer");
	if(ret)
	{
		return (vtkTrackerBuffer*)ret;
	}
	// If the factory was unable to create the object, then create it here.
	return new vtkTrackerBuffer;
}

//----------------------------------------------------------------------------
vtkTrackerBuffer::vtkTrackerBuffer()
{
	this->MatrixArray = vtkDoubleArray::New();
	this->MatrixArray->SetNumberOfComponents(16);
	this->FlagArray = vtkIntArray::New();
	this->FrameNumberArray = vtkUnsignedLongLongArray::New(); 
	this->FilteredTimeStampArray = vtkDoubleArray::New();
	this->UnfilteredTimeStampArray = vtkDoubleArray::New();
	this->BufferSize = 0; 

	this->LocalTimeOffset = 0.0; 

	this->Mutex = vtkCriticalSection::New();

	this->ToolCalibrationMatrix = NULL;
	this->WorldCalibrationMatrix = NULL;

	this->SetBufferSize(1000); 

}

//----------------------------------------------------------------------------
void vtkTrackerBuffer::DeepCopy(vtkTrackerBuffer *buffer)
{
	LOG_TRACE("vtkTrackerBuffer::DeepCopy");
	this->SetBufferSize(buffer->GetBufferSize());

	this->CurrentIndex = buffer->CurrentIndex;
	this->NumberOfItems = buffer->NumberOfItems;
	this->CurrentTimeStamp = buffer->CurrentTimeStamp;
	this->LocalTimeOffset = buffer->LocalTimeOffset; 

	LOG_DEBUG("Copy tracker buffer arrays");
	for (int i = 0; i < this->BufferSize; i++)
	{
		this->MatrixArray->SetTuple(i, buffer->MatrixArray->GetTuple(i));
		this->FlagArray->SetValue(i, buffer->FlagArray->GetValue(i));
		this->FrameNumberArray->SetValue(i, buffer->FrameNumberArray->GetValue(i));
		this->FilteredTimeStampArray->SetValue(i, buffer->FilteredTimeStampArray->GetValue(i));
		this->UnfilteredTimeStampArray->SetValue(i, buffer->UnfilteredTimeStampArray->GetValue(i));
	}

	LOG_DEBUG("Copy tracker buffer matrices");
	vtkMatrix4x4 *tmatrix = vtkMatrix4x4::New();
	tmatrix->DeepCopy(buffer->GetToolCalibrationMatrix());
	this->SetToolCalibrationMatrix(tmatrix);
	tmatrix->Delete();
	vtkMatrix4x4 *wmatrix = vtkMatrix4x4::New();
	wmatrix->DeepCopy(buffer->GetWorldCalibrationMatrix());
	this->SetWorldCalibrationMatrix(wmatrix);
	wmatrix->Delete();
}

//----------------------------------------------------------------------------
vtkTrackerBuffer::~vtkTrackerBuffer()
{
	if ( this->MatrixArray != NULL ) 
	{
		this->MatrixArray->Delete();
		this->MatrixArray = NULL; 
	}

	if ( this->FlagArray != NULL ) 
	{
		this->FlagArray->Delete();
		this->FlagArray = NULL; 
	}

	if ( this->FrameNumberArray != NULL )
	{
		this->FrameNumberArray->Delete(); 
		this->FrameNumberArray = NULL; 
	}

	if ( this->FilteredTimeStampArray != NULL )
	{
		this->FilteredTimeStampArray->Delete();
		this->FilteredTimeStampArray = NULL; 
	}

	if ( this->UnfilteredTimeStampArray != NULL )
	{
		this->UnfilteredTimeStampArray->Delete();
		this->UnfilteredTimeStampArray = NULL; 
	}


	if ( this->Mutex != NULL ) 
	{
		this->Mutex->Delete();
		this->Mutex = NULL; 
	}

	if ( this->WorldCalibrationMatrix != NULL )
	{
		this->WorldCalibrationMatrix->Delete();
		this->WorldCalibrationMatrix = NULL; 
	}

	if ( this->ToolCalibrationMatrix != NULL )
	{
		this->ToolCalibrationMatrix->Delete();
		this->ToolCalibrationMatrix = NULL; 
	}
}

//----------------------------------------------------------------------------
void vtkTrackerBuffer::PrintSelf(ostream& os, vtkIndent indent)
{
	vtkObject::PrintSelf(os,indent);

	os << indent << "BufferSize: " << this->BufferSize << "\n";
	os << indent << "NumberOfItems: " << this->NumberOfItems << "\n";
	os << indent << "Local time offset: " << this->LocalTimeOffset << "\n";
	os << indent << "ToolCalibrationMatrix: " << this->ToolCalibrationMatrix << "\n";
	if (this->ToolCalibrationMatrix)
	{
		this->ToolCalibrationMatrix->PrintSelf(os,indent.GetNextIndent());
	}  
	os << indent << "WorldCalibrationMatrix: " << this->WorldCalibrationMatrix << "\n";
	if (this->WorldCalibrationMatrix)
	{
		this->WorldCalibrationMatrix->PrintSelf(os,indent.GetNextIndent());
	}
}

//----------------------------------------------------------------------------
void vtkTrackerBuffer::SetBufferSize(int n)
{
	LOG_TRACE("vtkTrackerBuffer::SetBufferSize: " << n);
	if (n == this->BufferSize)
	{	
		LOG_DEBUG("No need to change tracker buffer size"); 
		return;
	}

	// right now, there is no effort made to save the previous contents
	this->NumberOfItems = 0;
	this->CurrentIndex = -1;
	this->CurrentTimeStamp = 0.0;

	this->BufferSize = n;
	this->MatrixArray->SetNumberOfTuples(this->BufferSize);
	this->FlagArray->SetNumberOfValues(this->BufferSize);
	this->FrameNumberArray->SetNumberOfValues(this->BufferSize); 
	this->FilteredTimeStampArray->SetNumberOfValues(this->BufferSize);
	this->UnfilteredTimeStampArray->SetNumberOfValues(this->BufferSize);

	for (int i = 0; i < n; i++)
	{
		vtkSmartPointer<vtkMatrix4x4> matrix = vtkSmartPointer<vtkMatrix4x4>::New(); 
		matrix->Zero(); 
		this->MatrixArray->SetTuple(i, *matrix->Element); 
		this->FlagArray->SetValue(i, 0);
		this->FrameNumberArray->SetValue(i, 0); 
		this->FilteredTimeStampArray->SetValue(i, 0.0);
		this->UnfilteredTimeStampArray->SetValue(i, 0.0);
	} 

	this->Modified();
}  

//----------------------------------------------------------------------------
void vtkTrackerBuffer::AddItem(vtkMatrix4x4 *matrix, long flags, unsigned long frameNumber, double unfilteredTimestamp, double filteredTimestamp)
{
	if (filteredTimestamp <= this->CurrentTimeStamp)
	{
		return;
	}
	this->CurrentTimeStamp = filteredTimestamp;

	if (++this->CurrentIndex >= this->BufferSize)
	{
		this->CurrentIndex = 0;
		this->NumberOfItems = this->BufferSize;
	}

	if (this->CurrentIndex > this->NumberOfItems)
	{
		this->NumberOfItems = this->CurrentIndex + 1;
	}

	this->MatrixArray->SetTuple(this->CurrentIndex, *matrix->Element);
	this->FlagArray->SetValue(this->CurrentIndex, flags);
	this->FrameNumberArray->SetValue(this->CurrentIndex, frameNumber); 
	this->FilteredTimeStampArray->SetValue(this->CurrentIndex, filteredTimestamp);
	this->UnfilteredTimeStampArray->SetValue(this->CurrentIndex, unfilteredTimestamp);

	/*LOG_TRACE("=== New transform at: " << std::fixed << filteredTimestamp); */

	this->Modified();
}

//----------------------------------------------------------------------------
void vtkTrackerBuffer::GetMatrix(vtkMatrix4x4 *matrix, int i)
{
	i = ((this->CurrentIndex - i) % this->BufferSize);

	if (i < 0)
	{
		i += this->BufferSize;
	}

	this->MatrixArray->GetTuple(i,*matrix->Element);
	matrix->Modified();
}

//----------------------------------------------------------------------------
void vtkTrackerBuffer::GetCalibratedMatrix(vtkMatrix4x4 *matrix, int i)
{
	i = ((this->CurrentIndex - i) % this->BufferSize);

	if (i < 0)
	{
		i += this->BufferSize;
	}

	this->MatrixArray->GetTuple(i,*matrix->Element);
	matrix->Modified();

	if (this->ToolCalibrationMatrix)
	{
		vtkMatrix4x4::Multiply4x4(matrix,
			this->ToolCalibrationMatrix,
			matrix);
	}

	if (this->WorldCalibrationMatrix)
	{
		vtkMatrix4x4::Multiply4x4(this->WorldCalibrationMatrix,
			matrix,
			matrix);
	}
}

//----------------------------------------------------------------------------
long vtkTrackerBuffer::GetFlags(int i)
{
	i = ((this->CurrentIndex - i) % this->BufferSize);

	if (i < 0)
	{
		i += this->BufferSize;
	}

	return this->FlagArray->GetValue(i);
}

//----------------------------------------------------------------------------
unsigned long vtkTrackerBuffer::GetFrameNumber(int i)
{
	i = ((this->CurrentIndex - i) % this->BufferSize);

	if (i < 0)
	{
		i += this->BufferSize;
	}

	return this->FrameNumberArray->GetValue(i);
}

//----------------------------------------------------------------------------
double vtkTrackerBuffer::GetFilteredTimeStamp(int i)
{
	i = ((this->CurrentIndex - i) % this->BufferSize);

	if (i < 0)
	{
		i += this->BufferSize;
	}

	double globalTimestamp = this->FilteredTimeStampArray->GetValue(i) + this->LocalTimeOffset; 
	return globalTimestamp; 
}

//----------------------------------------------------------------------------
double vtkTrackerBuffer::GetUnfilteredTimeStamp(int i)
{
	i = ((this->CurrentIndex - i) % this->BufferSize);

	if (i < 0)
	{
		i += this->BufferSize;
	}

	double globalTimestamp = this->UnfilteredTimeStampArray->GetValue(i) + this->LocalTimeOffset; 
	return globalTimestamp; 
}

//----------------------------------------------------------------------------
// do a simple divide-and-conquer search for the transform
// that best matches the given timestamp
int vtkTrackerBuffer::GetIndexFromTime(double time)
{
	int lo = (this->NumberOfItems > 0 ? this->NumberOfItems - 1 : this->NumberOfItems);
	int hi = 0;

	double tlo = this->GetTimeStamp(lo);
	double thi = this->GetTimeStamp(hi);

	if (time <= tlo)
	{
		return lo;
	}
	else if (time >= thi)
	{
		return hi;
	}

	for (;;)
	{
		if (lo-hi == 1)
		{
			if (time - tlo > thi - time)
			{
				return hi;
			}
			else
			{
				return lo;
			}
		}

		int mid = (lo+hi)/2;
		double tmid = this->GetTimeStamp(mid);

		if (time < tmid)
		{
			hi = mid;
			thi = tmid;
		}
		else
		{
			lo = mid;
			tlo = tmid;
		}
	}
}

//----------------------------------------------------------------------------
// a mathematical helper function
static void vtkMatrix3x3ToQuaternion(const double A[3][3], double quat[4])
{
	double trace = 1.0 + A[0][0] + A[1][1] + A[2][2];
	double s;

	if (trace > 0.0001)
	{
		s = sqrt(trace);
		quat[0] = 0.5*s;
		s = 0.5/s; 
		quat[1] = (A[2][1] - A[1][2])*s;
		quat[2] = (A[0][2] - A[2][0])*s;
		quat[3] = (A[1][0] - A[0][1])*s;
	}
	else if (A[0][0] > A[1][1] && A[0][0] > A[2][2])
	{
		s = sqrt(1.0 + A[0][0] - A[1][1] - A[2][2]);
		quat[1] = 0.5*s;
		s = 0.5/s;
		quat[0] = (A[2][1] - A[1][2])*s;
		quat[2] = (A[1][0] + A[0][1])*s;
		quat[3] = (A[0][2] + A[2][0])*s;
	}
	else if (A[1][1] > A[2][2])
	{
		s = sqrt(1.0 - A[0][0] + A[1][1] - A[2][2]);
		quat[2] = 0.5*s;
		s = 0.5/s;
		quat[0] = (A[0][2] - A[2][0])*s;
		quat[1] = (A[1][0] + A[0][1])*s;
		quat[3] = (A[2][1] + A[1][2])*s;
	}
	else
	{
		s = sqrt(1.0 - A[0][0] - A[1][1] + A[2][2]);
		quat[3] = 0.5*s;
		s = 0.5/s;
		quat[0] = (A[1][0] - A[0][1])*s;
		quat[1] = (A[0][2] + A[2][0])*s;
		quat[2] = (A[2][1] + A[1][2])*s;
	}
}

static void vtkQuaternionToMatrix3x3(const double quat[4], double A[3][3])
{
	double ww = quat[0]*quat[0];
	double wx = quat[0]*quat[1];
	double wy = quat[0]*quat[2];
	double wz = quat[0]*quat[3];

	double xx = quat[1]*quat[1];
	double yy = quat[2]*quat[2];
	double zz = quat[3]*quat[3];

	double xy = quat[1]*quat[2];
	double xz = quat[1]*quat[3];
	double yz = quat[2]*quat[3];

	double rr = xx + yy + zz;
	// normalization factor, just in case quaternion was not normalized
	double f = 1.0/(ww + rr);
	double s = (ww - rr)*f;
	f *= 2;

	A[0][0] = xx*f + s;
	A[1][0] = (xy + wz)*f;
	A[2][0] = (xz - wy)*f;

	A[0][1] = (xy - wz)*f;
	A[1][1] = yy*f + s;
	A[2][1] = (yz + wx)*f;

	A[0][2] = (xz + wy)*f;
	A[1][2] = (yz - wx)*f;
	A[2][2] = zz*f + s;
}

//----------------------------------------------------------------------------
// Interpolate the matrix for the given timestamp from the two nearest
// transforms in the buffer.
// The rotation is interpolated with SLERP interpolation, and the
// position is interpolated with linear interpolation.
// The flags are the logical 'or' of the two transformations that
// are used in the interpolation.
long vtkTrackerBuffer::GetFlagsAndCalibratedMatrixFromTime(vtkMatrix4x4 *matrix,
												 double time)
{
	int index0 = this->GetIndexFromTime(time);
	int index1 = index0;
	double indexTime = this->GetTimeStamp(index0);
	double f = indexTime - time;
	double matrix0[3][3];
	double matrix1[3][3];
	double xyz0[3];
	double xyz1[3];
	double quaternion[4];
	long flags0;
	long flags1;
	int i;

	// time difference should be 500 milliseconds or less
	if (f < -0.5 || f > 0.5)
	{
		//fprintf(stderr,"vtkTrackerBuffer: time diff is %f (%f %f)\n", f, indexTime, time);
		f = 0.0;    
	}
	// figure out what values to interpolate between,
	// convert f into a value between 0 and 1
	else if (f > 0)
	{
		f = f/(indexTime - this->GetTimeStamp(index0 + 1));
		index1 = index0 + 1;
	}
	else if (f < 0)
	{
		if (index0 == 0)
		{
			index1 = 0;
			f = 0.0;
		}
		else
		{
			f = 1.0 + f/(this->GetTimeStamp(index0 - 1) - indexTime);
			index1 = index0;
			index0 = index0 - 1;
		}
	}

	//fprintf(stderr,"indices: %i %i %f\n", index0, index1, f);

	flags0 = this->GetFlags(index0);
	flags1 = this->GetFlags(index1);

	this->GetCalibratedMatrix(matrix, index0);
	for (i = 0; i < 3; i++)
	{
		matrix0[i][0] = matrix->GetElement(i,0);
		matrix0[i][1] = matrix->GetElement(i,1);
		matrix0[i][2] = matrix->GetElement(i,2);
		xyz0[i] = matrix->GetElement(i,3);
	}
	this->GetCalibratedMatrix(matrix, index1);
	for (i = 0; i < 3; i++)
	{
		matrix1[i][0] = matrix->GetElement(i,0);
		matrix1[i][1] = matrix->GetElement(i,1);
		matrix1[i][2] = matrix->GetElement(i,2);
		xyz1[i] = matrix->GetElement(i,3);
	}

	vtkMath::Transpose3x3(matrix0, matrix0);
	vtkMath::Multiply3x3(matrix1, matrix0, matrix1);
	vtkMath::Transpose3x3(matrix0, matrix0);
	vtkMatrix3x3ToQuaternion(matrix1, quaternion);

	double s = sqrt(quaternion[1]*quaternion[1] +
		quaternion[2]*quaternion[2] +
		quaternion[3]*quaternion[3]);
	double angle = atan2(s, quaternion[0]) * f;
	quaternion[0] = cos(angle);

	if (s > 0.00001)
	{
		s = sin(angle)/s;
		quaternion[1] = quaternion[1]*s;
		quaternion[2] = quaternion[2]*s;
		quaternion[3] = quaternion[3]*s;
	}
	else
	{ // use small-angle approximation for sin to avoid
		//  division by very small value
		quaternion[1] = quaternion[1]*f;
		quaternion[2] = quaternion[2]*f;
		quaternion[3] = quaternion[3]*f;
	}

	vtkQuaternionToMatrix3x3(quaternion, matrix1);
	vtkMath::Multiply3x3(matrix1, matrix0, matrix1);

	for (i = 0; i < 3; i++)
	{
		matrix->Element[i][0] = matrix1[i][0];
		matrix->Element[i][1] = matrix1[i][1];
		matrix->Element[i][2] = matrix1[i][2];
		matrix->Element[i][3] = xyz0[i]*(1.0 - f) + xyz1[i]*f;
		//fprintf(stderr, "%f %f %f %f\n", xyz0[i], xyz1[i],  matrix->Element[i][3], f);
	} 
	matrix->Modified();

	return (flags0 | flags1);
}

//----------------------------------------------------------------------------
// Interpolate the matrix for the given timestamp from the two nearest
// transforms in the buffer.
// The rotation is interpolated with SLERP interpolation, and the
// position is interpolated with linear interpolation.
// The flags are the logical 'or' of the two transformations that
// are used in the interpolation.
long vtkTrackerBuffer::GetFlagsAndMatrixFromTime(vtkMatrix4x4 *matrix,
															 double time)
{
	int index0 = this->GetIndexFromTime(time);
	int index1 = index0;
	double indexTime = this->GetTimeStamp(index0);
	double f = indexTime - time;
	double matrix0[3][3];
	double matrix1[3][3];
	double xyz0[3];
	double xyz1[3];
	double quaternion[4];
	long flags0;
	long flags1;
	int i;

	// time difference should be 500 milliseconds or less
	if (f < -0.5 || f > 0.5)
	{
		//fprintf(stderr,"vtkTrackerBuffer: time diff is %f (%f %f)\n", f, indexTime, time);
		f = 0.0;    
	}
	// figure out what values to interpolate between,
	// convert f into a value between 0 and 1
	else if (f > 0)
	{
		f = f/(indexTime - this->GetTimeStamp(index0 + 1));
		index1 = index0 + 1;
	}
	else if (f < 0)
	{
		if (index0 == 0)
		{
			index1 = 0;
			f = 0.0;
		}
		else
		{
			f = 1.0 + f/(this->GetTimeStamp(index0 - 1) - indexTime);
			index1 = index0;
			index0 = index0 - 1;
		}
	}

	//fprintf(stderr,"indices: %i %i %f\n", index0, index1, f);

	flags0 = this->GetFlags(index0);
	flags1 = this->GetFlags(index1);

	this->GetMatrix(matrix, index0);
	for (i = 0; i < 3; i++)
	{
		matrix0[i][0] = matrix->GetElement(i,0);
		matrix0[i][1] = matrix->GetElement(i,1);
		matrix0[i][2] = matrix->GetElement(i,2);
		xyz0[i] = matrix->GetElement(i,3);
	}
	this->GetMatrix(matrix, index1);
	for (i = 0; i < 3; i++)
	{
		matrix1[i][0] = matrix->GetElement(i,0);
		matrix1[i][1] = matrix->GetElement(i,1);
		matrix1[i][2] = matrix->GetElement(i,2);
		xyz1[i] = matrix->GetElement(i,3);
	}

	vtkMath::Transpose3x3(matrix0, matrix0);
	vtkMath::Multiply3x3(matrix1, matrix0, matrix1);
	vtkMath::Transpose3x3(matrix0, matrix0);
	vtkMatrix3x3ToQuaternion(matrix1, quaternion);

	double s = sqrt(quaternion[1]*quaternion[1] +
		quaternion[2]*quaternion[2] +
		quaternion[3]*quaternion[3]);
	double angle = atan2(s, quaternion[0]) * f;
	quaternion[0] = cos(angle);

	if (s > 0.00001)
	{
		s = sin(angle)/s;
		quaternion[1] = quaternion[1]*s;
		quaternion[2] = quaternion[2]*s;
		quaternion[3] = quaternion[3]*s;
	}
	else
	{ // use small-angle approximation for sin to avoid
		//  division by very small value
		quaternion[1] = quaternion[1]*f;
		quaternion[2] = quaternion[2]*f;
		quaternion[3] = quaternion[3]*f;
	}

	vtkQuaternionToMatrix3x3(quaternion, matrix1);
	vtkMath::Multiply3x3(matrix1, matrix0, matrix1);

	for (i = 0; i < 3; i++)
	{
		matrix->Element[i][0] = matrix1[i][0];
		matrix->Element[i][1] = matrix1[i][1];
		matrix->Element[i][2] = matrix1[i][2];
		matrix->Element[i][3] = xyz0[i]*(1.0 - f) + xyz1[i]*f;
		//fprintf(stderr, "%f %f %f %f\n", xyz0[i], xyz1[i],  matrix->Element[i][3], f);
	} 
	matrix->Modified();

	return (flags0 | flags1);
}

//----------------------------------------------------------------------------
void vtkTrackerBufferWriteMatrix(FILE *file, const double *matrix)
{
	for (int i = 0; i < 4; i++)
	{
		fprintf(file,"%15.12f %15.12f %15.12f %15.9f",
			matrix[4*i],matrix[4*i+1],matrix[4*i+2],matrix[4*i+3]);
		if (i < 3)
		{
			fprintf(file,"\n");
		}
		else
		{
			fprintf(file,";\n\n");
		}
	}
}

//----------------------------------------------------------------------------
void vtkTrackerBufferWriteRecord(FILE *file, double timestamp, long flags,
								 const double *matrix)
{
	fprintf(file,"%14.3f ",timestamp);
	fprintf(file,"%4.4lx ",flags);
	fprintf(file,"%8.2f %8.2f %8.2f ",
		matrix[4*0+3],matrix[4*1+3],matrix[4*2+3]);

	for (int i = 0; i < 3; i++)
	{
		fprintf(file,"%15.13f %15.13f %15.13f",
			matrix[4*i],matrix[4*i+1],matrix[4*i+2]);
		if (i < 2)
		{
			fprintf(file," ");
		}
		else
		{
			fprintf(file,";\n");
		}
	}
}

//----------------------------------------------------------------------------
// Write the tracking information to a file
void vtkTrackerBuffer::WriteToFile(const char *filename)
{
	double *elements;
	int n;
	long flags;
	double timestamp;
	vtkMatrix4x4 *matrix = vtkMatrix4x4::New();
	FILE *file;

	file = fopen(filename,"w");

	if (file == 0)
	{
		vtkErrorMacro( << "can't open file " << filename);
		return;
	}

	fprintf(file,"# vtkTrackerBuffer output\n\n");

	fprintf(file,"ToolCalibrationMatrix = \n");
	elements = *this->GetToolCalibrationMatrix()->Element;
	vtkTrackerBufferWriteMatrix(file, elements);

	fprintf(file,"WorldCalibrationMatrix = \n");
	elements = *this->GetWorldCalibrationMatrix()->Element;
	vtkTrackerBufferWriteMatrix(file, elements);


	fprintf(file,"\nTimestamp \tFlags\tTx\tTy\tTz\tRotation Matrix\n");
	n = this->GetNumberOfItems();
	while (--n >= 0)
	{
		timestamp = this->GetTimeStamp(n);
		flags = this->GetFlags(n);
		this->GetMatrix(matrix,n);
		elements = *(matrix->Element);
		vtkTrackerBufferWriteRecord(file, timestamp, flags, elements);
	}

	fclose(file);

	matrix->Delete();
}

//----------------------------------------------------------------------------
char *vtkTrackerBufferEatWhitespace(char *text)
{
	int i = 0;

	for (i = 0; i < 128; i++)
	{
		switch (*text)
		{
		case ' ':
		case '\t':
		case '\r':
		case '\n':
			text++;
			break;
		default:
			return text;
			break;
		}
	}

	return 0;
}

//----------------------------------------------------------------------------
// Read the tracking information from a file
void vtkTrackerBuffer::ReadFromFile(const char *filename)
{
	char text[256];
	char *cp;
	double elements[16];
	int state = 0;
	int i = 0;
	int line;
	long flags = 0;
	double timestamp = 0;
	int timestamp_warning = 0;
	vtkMatrix4x4 *matrix = vtkMatrix4x4::New();
	FILE *file;

	this->NumberOfItems = 0;
	this->CurrentIndex = 0;
	this->CurrentTimeStamp = 0.0;

	file = fopen(filename,"r");

	if (file == 0)
	{
		vtkErrorMacro( << "can't open file " << filename);
		return;
	}

	for (line = 1;; line++)
	{
		if (fgets(text, 256, file) == 0)
		{ // error or end of file
			if (i != 0)
			{
				vtkErrorMacro( << "bad data: " << filename << " line " << line);
			}
			break;
		}
		// eat leading whitespace
		cp = vtkTrackerBufferEatWhitespace(text);
		// skip over empty lines or comments
		if (cp == 0 || *cp == '\0' || *cp == '#')
		{
			continue;
		}

		if (strncmp(cp,"ToolCalibrationMatrix =",
			strlen("ToolCalibrationMatrix =")) == 0)
		{
			cp += strlen("ToolCalibrationMatrix =");
			state = 1;
			if (i != 0)
			{
				vtkErrorMacro( << "bad data: " << filename << " line " << line);
				break;
			}
		}

		if (strncmp(cp,"WorldCalibrationMatrix =",
			strlen("WorldCalibrationMatrix =")) == 0)
		{
			cp += strlen("WorldCalibrationMatrix =");
			state = 2;
			if (i != 0)
			{
				vtkErrorMacro( << "bad data: " << filename << " line " << line);
				break;
			}
		}

		for (;;i++)
		{
			cp = vtkTrackerBufferEatWhitespace(cp);
			if (cp == 0 || *cp == '\0' || *cp == '#')
			{
				break;
			}

			if (state == 0)
			{
				if (i == 0)
				{
					timestamp = strtod(cp, &cp);
				}
				else if (i == 1)
				{
					flags = strtoul(cp, &cp, 16);
				}
				else if (i < 5)
				{
					elements[(i-2)*4 + 3] = strtod(cp, &cp);
				}
				else if (i < 8)
				{
					elements[0 + (i-5)] = strtod(cp, &cp);
				}
				else if (i < 11)
				{
					elements[4 + (i-8)] = strtod(cp, &cp);
				}           
				else if (i < 14)
				{
					elements[8 + (i-11)] = strtod(cp, &cp);
				}
				else
				{
					if (i > 14 || *cp != ';')
					{
						vtkErrorMacro( << "bad data: " << filename << " line " << line);
						fclose(file);
						return;
					}
					elements[12] = elements[13] = elements[14] = 0.0;
					elements[15] = 1.0;
					matrix->DeepCopy(elements);
					if (timestamp <= this->CurrentTimeStamp && !timestamp_warning)
					{
						vtkWarningMacro( << filename << " line " << line << ": timestamps are not monotonically increasing, some data was ignored.");      
						timestamp_warning = 1;
					}
					this->AddItem(matrix, flags, line, timestamp, timestamp);
					i = 0;
					state = 0;
					break;
				}
			}
			else if (state == 1)
			{
				if (i < 16)
				{
					elements[i] = strtod(cp, &cp);
				}
				else
				{
					if (i > 16 || *cp != ';')
					{
						vtkErrorMacro( << "bad data: " << filename << " line " << line);
						fclose(file);
						return;
					}
					vtkMatrix4x4 *calmatrix = vtkMatrix4x4::New();
					calmatrix->DeepCopy(elements);
					this->SetToolCalibrationMatrix(calmatrix);
					calmatrix->Delete();
					i = 0;
					state = 0;
					break;
				}
			}
			else if (state == 2)
			{
				if (i < 16)
				{
					elements[i] = strtod(cp, &cp);
				}
				else
				{
					if (i > 16 || *cp != ';')
					{
						vtkErrorMacro( << "bad data: " << filename << " line " << line);
						fclose(file);
						return;
					}
					vtkMatrix4x4 *calmatrix = vtkMatrix4x4::New();
					calmatrix->DeepCopy(elements);
					this->SetWorldCalibrationMatrix(calmatrix);
					calmatrix->Delete();
					i = 0;
					state = 0;
					break;
				}
			}
		}
	}

	fclose(file);

	matrix->Delete();
}

//----------------------------------------------------------------------------
void vtkTrackerBuffer::Clear()
{
	this->CurrentIndex = -1; 
	this->NumberOfItems = 0; 
	this->CurrentTimeStamp = 0; 

}

//----------------------------------------------------------------------------
double vtkTrackerBuffer::GetFrameRate( bool ideal /*=false*/)
{
	std::vector<double> framePeriods; 
	
	for ( int frame = 0; frame < this->NumberOfItems - 1; frame++ )
	{
		double time = this->GetTimeStamp(frame); 
		double prevtime = this->GetTimeStamp(frame + 1); 
		unsigned long framenum = this->GetFrameNumber(frame); 
		unsigned long prevframenum = this->GetFrameNumber(frame + 1); 
		
		if ( time == 0 || prevtime == 0 )
		{
			continue; 
		}

		int frameDiff = framenum - prevframenum; 
		double frameperiod = (time - prevtime); 
		if ( ideal )
		{
			frameperiod /= (1.0 * frameDiff);
		}

		if ( frameperiod > 0 )
		{
			framePeriods.push_back(frameperiod); 
		}
	}

	const int numberOfFramePeriods =  framePeriods.size(); 
	double samplingPeriod(0); 
	for ( int i = 0; i < numberOfFramePeriods; i++ )
	{
		samplingPeriod += framePeriods[i] / (1.0 * numberOfFramePeriods); 
	}

	double frameRate(0); 
	if ( samplingPeriod != 0 )
	{
		frameRate = 1.0/samplingPeriod;
	}

	return frameRate;  
}



