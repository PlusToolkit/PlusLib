/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkVirtualStreamMixer_h
#define __vtkVirtualStreamMixer_h

#include "vtkPlusDevice.h"
#include <string>

/*!
\class vtkVirtualStreamMixer 
\brief 

\ingroup PlusLibDataCollection
*/
class VTK_EXPORT vtkVirtualStreamMixer : public vtkPlusDevice
{
public:
  static vtkVirtualStreamMixer *New();
  vtkTypeRevisionMacro(vtkVirtualStreamMixer,vtkPlusDevice);
  void PrintSelf(ostream& os, vtkIndent indent);     

  virtual bool IsTracker() const { return false; }

  bool GetTrackingDataAvailable() const;
  bool GetVideoDataAvailable() const;
  PlusStatus GetMostRecentTimestamp( double & ts ) const;
  PlusStatus GetTrackedFrameList( double& aTimestamp, vtkTrackedFrameList* aTrackedFrameList, int aMaxNumberOfFramesToAdd );
  virtual PlusStatus GetTrackedFrame(double timestamp, TrackedFrame *trackedFrame);
  virtual PlusStatus GetTrackedFrame(TrackedFrame *trackedFrame);
  /*! 
    Get the tracked frame from devices by time with each tool transforms
    \param time The closes frame to this timestamp will be retrieved
    \param trackedFrame The output where the tracked frame information will be copied
  */
  virtual PlusStatus GetTrackedFrameByTime(double time, TrackedFrame* trackedFrame); 

protected:
  vtkVirtualStreamMixer();
  virtual ~vtkVirtualStreamMixer();

protected:
  vtkPlusStream::StreamBufferContainer InStreams;

private:
  vtkVirtualStreamMixer(const vtkVirtualStreamMixer&);  // Not implemented.
  void operator=(const vtkVirtualStreamMixer&);  // Not implemented. 
};

#endif
