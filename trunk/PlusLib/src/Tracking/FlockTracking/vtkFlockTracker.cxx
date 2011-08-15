/*=========================================================================

Program:   AtamaiTracking for VTK
Module:    $RCSfile: vtkFlockTracker.cxx,v $
Creator:   David Gobbi <dgobbi@atamai.com>
Language:  C++
Author:    $Author: glehmann $
Date:      $Date: 2006/11/10 18:31:42 $
Version:   $Revision: 1.2 $

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

#include <limits.h>
#include <float.h>
#include <math.h>
#include "flock.h"
#include "vtkMath.h"
#include "vtkTransform.h"
#include "vtkFlockTracker.h"
#include "vtkTrackerTool.h"
#include "vtkTimerLog.h"
#include "vtkObjectFactory.h"

// maximum calibrated range for flock is 48 inches, this is in millimetres
#define VTK_FLOCK_RANGE 1219.2

//----------------------------------------------------------------------------
vtkFlockTracker* vtkFlockTracker::New()
{
	// First try to create the object from the vtkObjectFactory
	vtkObject* ret = vtkObjectFactory::CreateInstance("vtkFlockTracker");
	if(ret)
	{
		return (vtkFlockTracker*)ret;
	}
	// If the factory was unable to create the object, then create it here.
	return new vtkFlockTracker;
}

//----------------------------------------------------------------------------
vtkFlockTracker::vtkFlockTracker()
{
	this->SendMatrix = vtkMatrix4x4::New();
	this->SerialPort = 1;  // default serial port is COM1
	this->BaudRate = 115200;
	this->Mode = FB_THREAD;
	this->Flock = fbNew();
	this->SetNumberOfTools(14);
	this->NumberOfBirds = 0;
	this->CurrentFrameNumber = 0;
}

//----------------------------------------------------------------------------
vtkFlockTracker::~vtkFlockTracker() 
{
	if (this->Tracking)
	{
		this->StopTracking();
	}
	fbDelete(this->Flock);
	this->SendMatrix->Delete();
}

//----------------------------------------------------------------------------
void vtkFlockTracker::PrintSelf(ostream& os, vtkIndent indent)
{
	vtkTracker::PrintSelf(os,indent);

	os << indent << "SerialPort: " << this->SerialPort << "\n";
	os << indent << "BaudRate: " << this->BaudRate << "\n";
	os << indent << "SendMatrix: " << this->SendMatrix << "\n";
	this->SendMatrix->PrintSelf(os,indent.GetNextIndent());
}  

//----------------------------------------------------------------------------
PlusStatus vtkFlockTracker::Probe()
{
	int errnum,baud;

	if (this->Tracking)
	{
		return PLUS_SUCCESS;
	}

	switch (this->BaudRate)
	{
	case 2400: baud = FB_2400; break; 
	case 4800: baud = FB_4800; break; 
	case 9600: baud = FB_9600; break; 
	case 19200: baud = FB_19200; break; 
	case 38400: baud = FB_38400; break; 
	case 57600: baud = FB_57600; break; 
	case 115200: baud = FB_115200; break;
	default:
		LOG_ERROR("Illegal baud rate");
		return PLUS_FAIL;
	} 

	// check all baud rates
	for (int ibaud = 0; ibaud < 7; ibaud++) 
	{
		int realbaud = (baud + ibaud) % 7;

		fbOpen(this->Flock,fbDeviceName(this->SerialPort-1),realbaud,this->Mode);
		errnum = fbGetError(this->Flock);
		if (errnum == FB_MODE_ERROR)
		{
			this->Mode = FB_NOTHREAD;
			fbOpen(this->Flock,fbDeviceName(this->SerialPort-1),realbaud,this->Mode);
			errnum = fbGetError(this->Flock);
		}
		if (errnum == FB_COM_ERROR || errnum == FB_OPEN_ERROR)
		{  // no device attached to port, or no permission to use port
			break;
		}
		else if (errnum)
		{  // there is something on the port, try next baud rate
			continue;
		}

		// check information about the flock
		// our FOB rev. number is 3.63
		// int rev = fbExamineValue(this->Flock,FB_REVISION);
		// fprintf(stderr,"FB_REVISION: %d.%d\n",(rev & 0xff),((rev >> 8) & 0xff));
		// the rate is 103.09
		// int rate = fbExamineValue(this->Flock,FB_RATE);
		// fprintf(stderr,"FB_RATE: %.2f\n",rate/256.0);

		// probe was successful
		fbClose(this->Flock);
		switch (realbaud)
		{
		case FB_2400: this->BaudRate = 2400; break; 
		case FB_4800: this->BaudRate = 4800; break; 
		case FB_9600: this->BaudRate = 9600; break; 
		case FB_19200: this->BaudRate = 19200; break; 
		case FB_38400: this->BaudRate = 38400; break; 
		case FB_57600: this->BaudRate = 57600; break; 
		case FB_115200: this->BaudRate = 115200; break; 
		}
		return PLUS_SUCCESS;
	}

	// probe failed
	LOG_ERROR("All Flock connection attempts have failed");
	return PLUS_FAIL;
} 

//----------------------------------------------------------------------------
PlusStatus vtkFlockTracker::InternalStartTracking()
{
	int errnum,i,baud;
	char status[128];

	switch (this->BaudRate)
	{
	case 2400: baud = FB_2400; break; 
	case 4800: baud = FB_4800; break; 
	case 9600: baud = FB_9600; break; 
	case 19200: baud = FB_19200; break; 
	case 38400: baud = FB_38400; break; 
	case 57600: baud = FB_57600; break; 
	case 115200: baud = FB_115200; break;
	default:
		LOG_ERROR("Illegal baud rate "<<this->BaudRate);
		return PLUS_FAIL;
	}

	if (!this->Tracking)
	{
		fbOpen(this->Flock,fbDeviceName(this->SerialPort-1),baud,this->Mode);
		errnum = fbGetError(this->Flock);
		if (errnum == FB_MODE_ERROR)
		{
			this->Mode = FB_NOTHREAD;
			fbOpen(this->Flock,fbDeviceName(this->SerialPort-1),baud,this->Mode);
			errnum = fbGetError(this->Flock);
		}
		if (errnum)
		{
			LOG_ERROR(fbGetErrorMessage(this->Flock));
			return PLUS_FAIL;
		}

		fbExamineValueBytes(this->Flock,FB_FBB_STATUS,status);
		if (fbGetError(this->Flock))
		{
			LOG_ERROR(fbGetErrorMessage(this->Flock));
			return PLUS_FAIL;
		}

		for (i = 0; i < this->NumberOfTools; i++)
		{
			// fprintf(stderr,"bird %d status %x\n",i+1,status[i]);
			if (status[i] == 0)
			{
				this->NumberOfBirds = i;
				break;
			}
		}

		if (this->NumberOfBirds > 0)
		{ // group mode is available
			fbFBBReset(this->Flock);
			if (fbGetError(this->Flock))
			{
				LOG_ERROR(fbGetErrorMessage(this->Flock));
				return PLUS_FAIL;
			}
			fbFBBAutoConfig(this->Flock,this->NumberOfBirds);
			if (fbGetError(this->Flock))
			{
				LOG_ERROR(fbGetErrorMessage(this->Flock));
				return PLUS_FAIL;
			}
			fbChangeValue(this->Flock,FB_GROUP_MODE,1);
			if (fbGetError(this->Flock))
			{
				LOG_ERROR(fbGetErrorMessage(this->Flock));
				return PLUS_FAIL;
			}
			for (i = 0; i < this->NumberOfBirds; i++)
			{
				// change to upper hemisphere (the most useful, IMHO)
				fbRS232ToFBB(this->Flock,i+1);
				fbSetHemisphere(this->Flock,FB_UPPER);

				// the type is "0000" plus the bird number
				std::ostringstream birdType; 
				birdType << "0000" << i+1; 				 
				this->Tools[i]->SetToolName(birdType.str().c_str());

				this->Tools[i]->SetToolType(TRACKER_TOOL_GENERAL); // TODO: probably it would be necessary to set certain birds as reference, probe, etc.

				// get the revision number		
				fbRS232ToFBB(this->Flock,i+1);
				int rev = fbExamineValue(this->Flock,FB_REVISION);
				rev = ((rev >> 8) & 0x00ff) | ((rev << 8) & 0xff00);
				int majorRev=(rev & 0xff00) >> 8;
				int minorRev=(rev & 0x00ff);
				std::ostringstream revision_number; 
				revision_number << majorRev << "." << minorRev;
				this->Tools[i]->SetToolRevision(revision_number.str().c_str());

				// manufacturer
				this->Tools[i]->SetToolManufacturer("ASCENSION");

				// for part number use the model ID
				char model_id[16];
				fbRS232ToFBB(this->Flock,i+1);
				fbExamineValueBytes(this->Flock,FB_IDENTIFICATION,model_id);
				model_id[11] = '\0';
				this->Tools[i]->SetToolPartNumber(model_id);

				// get the serial number
				if (rev >= (3 << 8) + 67)
				{
					fbRS232ToFBB(this->Flock,i+1);
					int sn = fbExamineValue(this->Flock,FB_SERIAL_NUMBER);
					std::ostringstream serial_number; 
					serial_number << sn; 				 
					this->Tools[i]->SetToolSerialNumber(serial_number.str().c_str());
				}

				if (fbGetError(this->Flock))
				{
					LOG_ERROR(fbGetErrorMessage(this->Flock));
					return PLUS_FAIL;
				}
			}
		}
		else
		{ // standalone 
			this->NumberOfBirds = 1;
			fbSetHemisphere(this->Flock,FB_UPPER);

			this->Tools[0]->SetToolType(TRACKER_TOOL_GENERAL);

			// get the revision number		
			int rev = fbExamineValue(this->Flock,FB_REVISION);
			rev = ((rev >> 8) & 0x00ff) | ((rev << 8) & 0xff00);
			int majorRev=(rev & 0xff00) >> 8;
			int minorRev=(rev & 0x00ff);
			std::ostringstream revision_number; 
			revision_number << majorRev << "." << minorRev;
			this->Tools[0]->SetToolRevision(revision_number.str().c_str());

			// manufacturer
			this->Tools[0]->SetToolManufacturer("ASCENSION");

			// for part number use the model ID
			char model_id[16];
			fbExamineValueBytes(this->Flock,FB_IDENTIFICATION,model_id);
			model_id[11] = '\0';
			this->Tools[0]->SetToolPartNumber(model_id);

			// get the serial number
			if (rev >= (3 << 8) + 67)
			{
				int sn = fbExamineValue(this->Flock,FB_SERIAL_NUMBER);
				std::ostringstream serial_number; 
				serial_number << sn; 				 
				this->Tools[0]->SetToolSerialNumber(serial_number.str().c_str());
			}
		}
	}

	if (this->Mode != FB_NOTHREAD)
	{
		fbStream(this->Flock);
	}
	if (fbGetError(this->Flock))
	{
		LOG_ERROR(fbGetErrorMessage(this->Flock));
		return PLUS_FAIL;
	}

	return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkFlockTracker::InternalStopTracking()
{
	if (this->Mode != FB_NOTHREAD)
	{
		fbEndStream(this->Flock);
	}
	if (fbGetError(this->Flock))
	{
		LOG_ERROR(fbGetErrorMessage(this->Flock));
	}
	fbClose(this->Flock);
	if (fbGetError(this->Flock))
	{
		LOG_ERROR(fbGetErrorMessage(this->Flock));
		return PLUS_FAIL;
	}

	return PLUS_SUCCESS;
}  

//----------------------------------------------------------------------------
PlusStatus vtkFlockTracker::InternalUpdate()
{
	this->CurrentFrameNumber++;

	int i,j,bird,tool;
	int errnum = 0;
	float array[9];
	float refmat[4][4];
	float toolmat[4][4];

	float (*xyz)[3] = new float[this->NumberOfBirds][3];
	float (*zyx)[3] = new float[this->NumberOfBirds][3];
	long *flags = new long[this->NumberOfBirds];
	double *timestamps = new double[this->NumberOfBirds];

#if (VTK_MAJOR_VERSION < 5)
	double timestamp = vtkTimerLog::GetCurrentTime();
#else
	double timestamp = vtkTimerLog::GetUniversalTime();
#endif

	// initialize the flags to 'out-of-view'
	for (tool = 0; tool < this->NumberOfBirds; tool++)
	{ 
		timestamps[tool] = timestamp;
		flags[tool] = TR_OUT_OF_VIEW;
	}

	// use point mode if not multithreading
	if (this->Mode == FB_NOTHREAD)
	{
		fbPoint(this->Flock);
	}

	// update each bird
	for (tool = 0; tool < this->NumberOfBirds; tool++) 
	{
		fbUpdate(this->Flock);
		bird = fbGetBird(this->Flock)-1;
		// if fbGetBird() returns 0, then a phase error has occurred
		if (bird >= 0)
		{
			timestamps[bird] = fbGetTime(this->Flock);      
			fbGetPosition(this->Flock,xyz[bird]);
			fbGetAngles(this->Flock,zyx[bird]);
			flags[bird] = TR_OK;
			if (fbGetButton(this->Flock))
			{
				flags[bird] |= TR_SWITCH1_IS_ON;
			}
			// fprintf(stderr,"bird = %d, NumberOfBirds = %d, timestamp = %f\n",bird,this->NumberOfBirds,this->UpdateTimeStamp);
		}
	}

	if ((errnum = fbGetError(this->Flock)) != 0)
	{
		if (errnum == FB_PHASE_ERROR)
		{
			LOG_WARNING(fbGetErrorMessage(this->Flock));
		}
		else 
		{
			LOG_ERROR(fbGetErrorMessage(this->Flock));
		}
		delete [] timestamps;
		delete [] flags;
		delete [] xyz;
		delete [] zyx;
		return PLUS_FAIL;
	}

	// handle the reference tool first
	if (this->GetReferenceToolNumber() >= 0 && this->GetReferenceToolNumber() < this->NumberOfBirds) 
	{
		tool = this->GetReferenceToolNumber();
		flags[tool] = 0;

		fbMatrixFromAngles(array,zyx[tool]);

		for (i = 0; i < 3; i++)
		{
			for (j = 0; j < 3; j++)
			{
				refmat[i][j] = array[3*i + j];
			}
			refmat[3][i] = 0.0;
			refmat[i][3] = xyz[tool][i];
		}
		refmat[3][3] = 1.0;

		if (xyz[tool][0]*xyz[tool][0] +
			xyz[tool][1]*xyz[tool][1] +
			xyz[tool][2]*xyz[tool][2] > VTK_FLOCK_RANGE*VTK_FLOCK_RANGE)
		{
			flags[tool] = TR_OUT_OF_VOLUME;
		}
	}
	else if (this->GetReferenceToolNumber() >= this->NumberOfBirds)
	{ // reference tool doesn't actually exist!
		flags[this->GetReferenceToolNumber()] = TR_MISSING | TR_OUT_OF_VIEW;
		for (i = 0; i < 3; i++)
		{
			for (j = 0; j < 3; j++)
			{
				refmat[i][j] = 0;
			}
			refmat[i][i] = 1.0;
			refmat[3][i] = 0.0;
			refmat[i][3] = 0.0;
		}
	}

	for (tool = 0; tool < this->NumberOfBirds; tool++) 
	{
		flags[tool] = 0;

		fbMatrixFromAngles(array,zyx[tool]);

		for (i = 0; i < 3; i++)
		{
			for (j = 0; j < 3; j++)
			{
				toolmat[i][j] = array[3*i + j];
			}
			toolmat[3][i] = 0.0;
			toolmat[i][3] = xyz[tool][i];
		}
		toolmat[3][3] = 1.0;

		// check if tool is within optimal tracking range
		if (xyz[tool][0]*xyz[tool][0] +
			xyz[tool][1]*xyz[tool][1] +
			xyz[tool][2]*xyz[tool][2] > VTK_FLOCK_RANGE*VTK_FLOCK_RANGE)
		{
			flags[tool] |= TR_OUT_OF_VOLUME;
		}

		if (this->GetReferenceToolNumber() >= 0 && tool != this->GetReferenceToolNumber())
		{
			flags[tool] |= flags[this->GetReferenceToolNumber()] \
				& (TR_MISSING | TR_OUT_OF_VIEW | TR_OUT_OF_VOLUME);

			// multiply by the inverse of the reference tool matrix,
			// taking advantage of the orthogonality of the reference matrix
			for (i = 0; i < 3; i++)
			{
				for (j = 0; j < 3; j++)
				{
					this->SendMatrix->SetElement(i,j,
						toolmat[i][0]*refmat[j][0] +
						toolmat[i][1]*refmat[j][1] +
						toolmat[i][2]*refmat[j][2] +
						toolmat[i][3]*refmat[j][3]); 
				}
				this->SendMatrix->SetElement(i,3,toolmat[i][3] - refmat[i][3]);
				this->SendMatrix->SetElement(3,i,0.0);
			}
			this->SendMatrix->SetElement(3,3,1.0);
		}
		else
		{
			for (i = 0; i < 4; i++)
			{
				for (j = 0; j < 4; j++)
				{
					this->SendMatrix->SetElement(i,j,toolmat[i][j]);
				}
			}
		}
		//this->ToolUpdate(tool,this->SendMatrix,flags[tool],timestamps[tool]);
		this->ToolTimeStampedUpdate(tool, this->SendMatrix, (TrackerStatus) flags[tool], this->CurrentFrameNumber, timestamps[tool]);
	}
	// for tools beyond the max
	this->SendMatrix->Identity();
	for (tool = this->NumberOfBirds; tool < this->NumberOfTools; tool++) 
	{
		this->ToolTimeStampedUpdate(tool, this->SendMatrix, (TrackerStatus)(TR_MISSING | TR_OUT_OF_VIEW), this->CurrentFrameNumber, timestamp);
	}

	delete [] timestamps;
	delete [] flags;
	delete [] xyz;
	delete [] zyx;
	return PLUS_SUCCESS;
}
