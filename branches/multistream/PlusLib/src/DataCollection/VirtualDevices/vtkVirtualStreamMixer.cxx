/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#include "vtkObjectFactory.h"
#include "vtkVirtualStreamMixer.h"

//----------------------------------------------------------------------------

vtkCxxRevisionMacro(vtkVirtualStreamMixer, "$Revision: 1.0$");
vtkStandardNewMacro(vtkVirtualStreamMixer);

//----------------------------------------------------------------------------
vtkVirtualStreamMixer::vtkVirtualStreamMixer()
: vtkPlusDevice()
{
}

//----------------------------------------------------------------------------
vtkVirtualStreamMixer::~vtkVirtualStreamMixer()
{
}

//----------------------------------------------------------------------------
void vtkVirtualStreamMixer::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}

//----------------------------------------------------------------------------
bool vtkVirtualStreamMixer::GetTrackingDataAvailable() const
{
  // TODO : implement this
  return true;
}

//----------------------------------------------------------------------------
bool vtkVirtualStreamMixer::GetVideoDataAvailable() const
{
  // TODO : implement this
  return true;
}

//----------------------------------------------------------------------------
PlusStatus vtkVirtualStreamMixer::GetMostRecentTimestamp( double & ts ) const
{
  // TODO : implement this
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkVirtualStreamMixer::GetTrackedFrameList( double& aTimestamp, vtkTrackedFrameList* aTrackedFrameList, int aMaxNumberOfFramesToAdd )
{
  // TODO : implement this
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkVirtualStreamMixer::GetTrackedFrame( double timestamp, TrackedFrame *trackedFrame )
{
  // TODO : implement this
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkVirtualStreamMixer::GetTrackedFrame( TrackedFrame *trackedFrame )
{
  // TODO : implement this
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkVirtualStreamMixer::GetTrackedFrameByTime(double time, TrackedFrame* trackedFrame)
{
  LOG_TRACE("vtkDataCollector::GetTrackedFrameByTime");
  double synchronizedTime = 0; 

  // For each in stream, get the timestamp
  // If all streams can return a tracked frame for the timestamp, success
  /*
  if ( this->GetVideoEnabled() && this->GetVideoSource() )
  {
    if ( this->GetVideoSource()->GetTrackedFrame(time, trackedFrame) != PLUS_SUCCESS )
    {
      LOG_ERROR("Failed to get tracked frame from video source by time: " << std::fixed << time );
      return PLUS_FAIL;
    }

    synchronizedTime = trackedFrame->GetTimestamp();
  }

  if ( this->GetTrackingEnabled() && this->GetTracker() != NULL )
  {
    if ( !this->GetVideoEnabled() )
    {
      synchronizedTime = time;
    }

    if ( this->GetTracker()->GetTrackedFrame(synchronizedTime, trackedFrame) != PLUS_SUCCESS )
    {
      LOG_ERROR("Failed to get all transforms from tracker at: " << std::fixed << synchronizedTime ); 
      return PLUS_FAIL; 
    }
  }

  // Save tracked frame timestamp 
  trackedFrame->SetTimestamp(synchronizedTime); 

  // Save frame timestamp
  std::ostringstream strTimestamp; 
  strTimestamp << std::fixed << trackedFrame->GetTimestamp(); 
  trackedFrame->SetCustomFrameField("Timestamp", strTimestamp.str()); 
*/
  return PLUS_SUCCESS; 
}