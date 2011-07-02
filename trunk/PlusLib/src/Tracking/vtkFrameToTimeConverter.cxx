/*=========================================================================

Program:   AtamaiTracking for VTK
Module:    $RCSfile: vtkFrameToTimeConverter.cxx,v $
Creator:   David Gobbi <dgobbi@atamai.com>
Language:  C++
Author:    $Author: dgobbi $
Date:      $Date: 2005/08/08 19:42:54 $
Version:   $Revision: 1.3 $

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
#include <limits.h>
#include <float.h>
#include <math.h>
#include "vtkFrameToTimeConverter.h"
#include "vtkObjectFactory.h"

//----------------------------------------------------------------------------
vtkFrameToTimeConverter* vtkFrameToTimeConverter::New()
{
	// First try to create the object from the vtkObjectFactory
	vtkObject* ret = vtkObjectFactory::CreateInstance("vtkFrameToTimeConverter");
	if(ret)
	{
		return (vtkFrameToTimeConverter*)ret;
	}
	// If the factory was unable to create the object, then create it here.
	return new vtkFrameToTimeConverter;
}

//----------------------------------------------------------------------------
vtkFrameToTimeConverter::vtkFrameToTimeConverter()
{
	this->NominalFrequency = 100.0;
	this->LastTimeStamp = 0;
	this->LastUnfilteredTimeStamp = 0;
	this->LastFrameCount = 0;
	this->EstimatedFramePeriod = 0;
	// maximum allowed difference of the frame delay compared to average delay, in seconds
	this->MaximumFramePeriodJitter = 0.100; 

#ifdef PLUS_PRINT_TRACKER_TIMESTAMP_DEBUG_INFO 
	this->DebugInfoStream.open ("TrackerBufferTimestamps.txt", ios::out);
	this->DebugInfoStream << "FrameNumber\tFrameNumberDifference\tUnfilteredTimestamp\tFilteredTimestamp\tUnfilteredTimeDifference\tSamplingPeriod\tEstimatedFramePeriod" << std::endl; 
	this->DebugInfoStream.flush(); 
#endif

}

//----------------------------------------------------------------------------
vtkFrameToTimeConverter::~vtkFrameToTimeConverter()
{
#ifdef PLUS_PRINT_TRACKER_TIMESTAMP_DEBUG_INFO 
	this->DebugInfoStream.close();
#endif

}

//----------------------------------------------------------------------------
void vtkFrameToTimeConverter::PrintSelf(ostream& os, vtkIndent indent)
{
	vtkObject::PrintSelf(os,indent);

	os << indent << "NominalFrequency: " << this->NominalFrequency << "\n";
	os << indent << "LastFrame: " << this->LastFrameCount << "\n";
	os << indent << "InstantaneousFrequency:" <<
		this->GetInstantaneousFrequency() << "\n";
}

//----------------------------------------------------------------------------
void vtkFrameToTimeConverter::Initialize()
{
	this->LastTimeStamp = 0;
	this->LastUnfilteredTimeStamp = 0; 
	this->LastFrameCount = 0;
	this->EstimatedFramePeriod = 0;
}

//----------------------------------------------------------------------------
// for accurate timing of the transformation: an exponential moving average
// is computed to smooth out the jitter in the times that are returned by the system clock:
// EstimatedFramePeriod[t] = EstimatedFramePeriod[t-1] * (1-SmoothingFactor) + FramePeriod[t] * SmoothingFactor
// Smaller SmoothingFactor results leads to less jitter.
void vtkFrameToTimeConverter::GetTimeStampForFrame(unsigned long framecount, double &unfilteredTimestamp, double &filteredTimestamp)
{
	double timestamp = vtkAccurateTimer::GetSystemTime(); 
	unfilteredTimestamp = timestamp; 

	if ( this->LastFrameCount == 0 )
	{
		this->LastFrameCount = framecount; 
		this->LastTimeStamp = timestamp; 
		this->LastUnfilteredTimeStamp = unfilteredTimestamp; 
	}
	
	if ( framecount == this->LastFrameCount )
	{
		unfilteredTimestamp = this->LastUnfilteredTimeStamp; 
		filteredTimestamp = this->LastTimeStamp; 
		return; 
	}

	double smoothingFactor = 0.001;
	const unsigned int numberOfAveragedPeriods = 20; 

	unsigned long frameCountDiff = framecount - this->LastFrameCount; 
	double frameperiod = (timestamp - this->LastTimeStamp)/ frameCountDiff;

	if ( this->AveragedFramePeriods.size() < numberOfAveragedPeriods ) 
	{
		if ( this->LastTimeStamp == 0 ) 
		{
			this->AveragedFramePeriods.clear(); 
			this->EstimatedFramePeriod = 0; 
		}
		else if ( frameCountDiff > 1 )
		{
			if ( AveragedFramePeriods.size() > 0 ) 
			{
				// we have some lost frames, ignore the previous frame period
				AveragedFramePeriods.pop_back(); 
			}
		}
		else
		{
			AveragedFramePeriods.push_back(frameperiod); 
		}

		double diffUnfilteredTimestamp = ((timestamp - this->LastUnfilteredTimeStamp)/ frameCountDiff);
		this->LastTimeStamp = timestamp;
		this->LastUnfilteredTimeStamp = timestamp; 
		this->LastFrameCount = framecount;
		filteredTimestamp = this->LastTimeStamp;
#ifdef PLUS_PRINT_TRACKER_TIMESTAMP_DEBUG_INFO 
		// FrameNumber	FrameNumberDifference	UnfilteredTimestamp	FilteredTimestamp	UnfilteredTimeDifference	FramePeriod	EstimatedFramePeriod	
		this->DebugInfoStream << std::fixed << framecount << "\t" << frameCountDiff << "\t" << unfilteredTimestamp << "\t" << filteredTimestamp << "\t" 
			<< diffUnfilteredTimestamp << "\t" << frameperiod << "\t" << this->EstimatedFramePeriod << std::endl; 
		this->DebugInfoStream.flush(); 
#endif
		return;
	}
	else if ( this->AveragedFramePeriods.size() == numberOfAveragedPeriods )
	{
		this->EstimatedFramePeriod = 0; 
		for ( int i = 0; i < numberOfAveragedPeriods; i++ )
		{
			this->EstimatedFramePeriod += this->AveragedFramePeriods[i] / numberOfAveragedPeriods; 
		}
		AveragedFramePeriods.push_back(this->EstimatedFramePeriod); 
	}

	double diffUnfilteredTimestamp = ((timestamp - this->LastUnfilteredTimeStamp)/ frameCountDiff);; 
	this->LastUnfilteredTimeStamp = timestamp; 

	this->LastTimeStamp += ((framecount - this->LastFrameCount)*this->EstimatedFramePeriod);

	// check the difference between the system clock and the
	// 'predicted' time for this framecount
	double diffperiod = (timestamp - this->LastTimeStamp);

	if (diffperiod < -MaximumFramePeriodJitter || diffperiod > MaximumFramePeriodJitter )
	{ 
		// Frame delay is very large, most probably because the frame transfer 
		// was delayed, therefore the current time doesn't represent well the actual acquisition time.
		// Ignore this outlier and use the average delay instead.
		this->LastTimeStamp = timestamp;
		this->LastFrameCount = framecount;
		filteredTimestamp = this->LastTimeStamp;

#ifdef PLUS_PRINT_TRACKER_TIMESTAMP_DEBUG_INFO 
		// FrameNumber	FrameNumberDifference	UnfilteredTimestamp	FilteredTimestamp	UnfilteredTimeDifference	FramePeriod	EstimatedFramePeriod	
		this->DebugInfoStream << std::fixed << framecount << "\t" << frameCountDiff << "\t" << unfilteredTimestamp << "\t" << filteredTimestamp << "\t" 
			<< diffUnfilteredTimestamp << "\t" << frameperiod << "\t" << this->EstimatedFramePeriod << std::endl; 
		this->DebugInfoStream.flush(); 
#endif

		return; 
	}

	// update our estimate of the current frame period based on the
	// measured period 
	this->EstimatedFramePeriod = this->EstimatedFramePeriod * (1-smoothingFactor) + frameperiod * smoothingFactor;
	this->LastFrameCount = framecount;
	filteredTimestamp = this->LastTimeStamp;

#ifdef PLUS_PRINT_TRACKER_TIMESTAMP_DEBUG_INFO 
	// FrameNumber	FrameNumberDifference	UnfilteredTimestamp	FilteredTimestamp	UnfilteredTimeDifference	FramePeriod	EstimatedFramePeriod	
	this->DebugInfoStream << std::fixed << framecount << "\t" << frameCountDiff << "\t" << unfilteredTimestamp << "\t" << filteredTimestamp << "\t" 
		<< diffUnfilteredTimestamp << "\t" << frameperiod << "\t" << this->EstimatedFramePeriod << std::endl; 
	this->DebugInfoStream.flush(); 
#endif

	return; 

}

//----------------------------------------------------------------------------
double vtkFrameToTimeConverter::GetInstantaneousFrequency()
{
	return 1.0/this->EstimatedFramePeriod;
}

//----------------------------------------------------------------------------
void vtkFrameToTimeConverter::DeepCopy(vtkFrameToTimeConverter *converter)
{
    this->SetNominalFrequency( converter->GetNominalFrequency() ); 
    this->LastTimeStamp = converter->LastTimeStamp; 
    this->LastUnfilteredTimeStamp = converter->LastUnfilteredTimeStamp; 
    this->LastFrameCount = converter->LastFrameCount; 
    this->EstimatedFramePeriod = converter->EstimatedFramePeriod; 
    this->AveragedFramePeriods = converter->AveragedFramePeriods; 
    this->MaximumFramePeriodJitter = converter->MaximumFramePeriodJitter;
}




