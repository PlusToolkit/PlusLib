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

  // Virtual stream mixers output only one stream
  vtkPlusStream* GetStream() const;

  bool GetTrackingDataAvailable() const;
  bool GetVideoDataAvailable() const;
  PlusStatus GetMostRecentTimestamp( double & ts ) const;
  PlusStatus GetTrackedFrameList( double& aTimestamp, vtkTrackedFrameList* aTrackedFrameList, int aMaxNumberOfFramesToAdd );
  virtual PlusStatus GetTrackedFrame(double timestamp, TrackedFrame *trackedFrame);
  virtual PlusStatus GetTrackedFrame(TrackedFrame *trackedFrame);
  virtual PlusStatus Reset();

  virtual double GetAcquisitionRate() const;

  /*! Return the most recent synchronized timestamp in the buffers */
  virtual PlusStatus GetMostRecentTimestamp(double &ts); 

  /*! Return the oldest synchronized timestamp in the buffers */
  virtual PlusStatus GetOldestTimestamp(double &ts); 

  /*! 
    Get the tracked frame from devices by time with each tool transforms
    \param time The closes frame to this timestamp will be retrieved
    \param trackedFrame The output where the tracked frame information will be copied
  */
  virtual PlusStatus GetTrackedFrameByTime(double time, TrackedFrame* trackedFrame); 

  /*!
    Get the tracked frame list from devices since time specified
    \param aTimestamp The oldest timestamp we search for in the buffer. If -1 get all frames in the time range since the most recent timestamp. Out parameter - changed to timestamp of last added frame
    \param aTrackedFrameList Tracked frame list used to get the newly acquired frames into. The new frames are appended to the tracked frame.
    \param aSamplingRateSec Sampling rate for getting the frames in seconds (timestamps are in seconds too)
    \param maxTimeLimitSec Maximum time spent in the function (in sec)
  */
  virtual PlusStatus GetTrackedFrameListSampled(double& aTimestamp, vtkTrackedFrameList* aTrackedFrameList, double aSamplingRateSec, double maxTimeLimitSec=-1); 

  /*! Get the closest tracked frame timestamp to the specified time */
  double GetClosestTrackedFrameTimestampByTime(double time);

protected:
  /*! Get number of tracked frames between two given timestamps (inclusive) */
  int GetNumberOfFramesBetweenTimestamps(double aTimestampFrom, double aTimestampTo);

  virtual PlusStatus InternalUpdate();

  vtkVirtualStreamMixer();
  virtual ~vtkVirtualStreamMixer();

private:
  vtkVirtualStreamMixer(const vtkVirtualStreamMixer&);  // Not implemented.
  void operator=(const vtkVirtualStreamMixer&);  // Not implemented. 
};

#endif
