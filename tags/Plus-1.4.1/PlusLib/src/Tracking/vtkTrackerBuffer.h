/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkTrackerBuffer_h
#define __vtkTrackerBuffer_h

#include "PlusConfigure.h"
#include "vtkObject.h"
#include "vtkMatrix4x4.h"
#include "vtkTimestampedCircularBuffer.h"
#include "vtkTracker.h"

class vtkTrackedFrameList;
class TrackedFrame; 

/*!
\class TrackerBufferItem 
\brief Timestamped tracking buffer item  
\ingroup PlusLibTracking
*/
class VTK_EXPORT TrackerBufferItem : public TimestampedBufferItem
{
public:

  TrackerBufferItem(); 
  ~TrackerBufferItem(); 
  TrackerBufferItem(const TrackerBufferItem& TrackerBufferItem); 
  TrackerBufferItem& TrackerBufferItem::operator=(TrackerBufferItem const& trackerBufferItem); 

  /*! Copy tracker buffer item */
  PlusStatus DeepCopy(TrackerBufferItem* trackerBufferItem); 

  /*! Set tracker matrix */
  PlusStatus SetMatrix(vtkMatrix4x4* matrix); 
  /*! Get tracker matrix */
  PlusStatus GetMatrix(vtkMatrix4x4* outputMatrix);

  /*! Set tracker item status */
  void SetStatus(ToolStatus status) { this->Status = status; }  
  /*! Get tracker item status */
  ToolStatus GetStatus() const { return this->Status; }

protected:
  vtkSmartPointer<vtkMatrix4x4> Matrix;
  ToolStatus Status;       
}; 

/*!
\class vtkTrackerBuffer 
\brief Buffer for 3D tracking 

Timestamped circular buffer for storing 3D tracking information and
filtered and non filtered timestamps. 

\ingroup PlusLibTracking
*/
class VTK_EXPORT vtkTrackerBuffer : public vtkObject
{
public:
  /*! Timestamp filtering options */
  enum TIMESTAMP_FILTERING_OPTION
  {
    READ_FILTERED_AND_UNFILTERED_TIMESTAMPS = 0,
    READ_UNFILTERED_COMPUTE_FILTERED_TIMESTAMPS,
    READ_FILTERED_IGNORE_UNFILTERED_TIMESTAMPS
  };

  vtkTypeMacro(vtkTrackerBuffer,vtkObject);
  static vtkTrackerBuffer *New();
  void PrintSelf(ostream& os, vtkIndent indent);

  /*! Set the size of the buffer, all new transforms are set to unity. */
  PlusStatus SetBufferSize(int n);
  virtual int GetBufferSize();

  /*! Set number of items used for timestamp filtering (with LSQR mimimizer) */
  virtual void SetAveragedItemsForFiltering(int averagedItemsForFiltering); 

  /*! Set recording start time */
  virtual void SetStartTime( double startTime ); 
  /*! Get recording start time */
  virtual double GetStartTime(); 

  /*! Get the table report of the timestamped buffer */
  virtual PlusStatus GetTimeStampReportTable(vtkTable* timeStampReportTable); 

  /*! Get the number of items in the buffer */
  int GetNumberOfItems() { return this->TrackerBuffer->GetNumberOfItems(); };

  /*! Add a matrix plus status to the list.  If the timestamp is less than or equal to the previous timestamp, then nothing  will be done. */
  PlusStatus AddTimeStampedItem(vtkMatrix4x4 *matrix, ToolStatus status, unsigned long frameNumber, double unfilteredTimestamp);
  PlusStatus AddTimeStampedItem(vtkMatrix4x4 *matrix, ToolStatus status, unsigned long frameNumber, double unfilteredTimestamp, double filteredTimestamp);

  /*! Get tracker item from buffer.  */
  virtual ItemStatus GetTrackerBufferItem(BufferItemUidType uid, TrackerBufferItem* bufferItem);
  
  /*! Get the latest tracker buffer item */
  virtual ItemStatus GetLatestTrackerBufferItem(TrackerBufferItem* bufferItem) { return this->GetTrackerBufferItem( this->GetLatestItemUidInBuffer(), bufferItem); }; 
  
  /*! Get the oldest tracker buffer item */
  virtual ItemStatus GetOldestTrackerBufferItem(TrackerBufferItem* bufferItem) { return this->GetTrackerBufferItem( this->GetOldestItemUidInBuffer(), bufferItem); }; 

  /*! Tracker item temporal interpolation type */
  enum TrackerItemTemporalInterpolationType
  {
    EXACT_TIME, /*!< only returns the item if the requested timestamp exactly matches the timestamp of an existing element */
    INTERPOLATED /*!< returns interpolated transform (requires valid transform at the requested timestamp) */
  };

  /*! Get interpolated tracker item from buffer by time. */
  virtual ItemStatus GetTrackerBufferItemFromTime( double time, TrackerBufferItem* bufferItem, TrackerItemTemporalInterpolationType interpolation); 

  /*! Get latest timestamp in the buffer */
  virtual ItemStatus GetLatestTimeStamp( double& latestTimestamp ); 

  /*! Get oldest timestamp in the buffer */
  virtual ItemStatus GetOldestTimeStamp( double& oldestTimestamp ); 

  /*! Get video buffer item timestamp */
  virtual ItemStatus GetTimeStamp( BufferItemUidType uid, double& timestamp); 
  
  /*! Get the oldest buffer item unique ID */
  virtual BufferItemUidType GetOldestItemUidInBuffer() { return this->TrackerBuffer->GetOldestItemUidInBuffer(); }
  
  /*! Get the latest buffer item unique ID */
  virtual BufferItemUidType GetLatestItemUidInBuffer() { return this->TrackerBuffer->GetLatestItemUidInBuffer(); }
  
  /*! Get buffer item unique ID from time */
  virtual ItemStatus GetItemUidFromTime(double time, BufferItemUidType& uid) { return this->TrackerBuffer->GetItemUidFromTime(time, uid); }

  /*! Set maximum allowed time difference in seconds between the desired and the closest valid timestamp */
  vtkSetMacro(MaxAllowedTimeDifference, double); 
  /*! Get maximum allowed time difference in seconds between the desired and the closest valid timestamp */
  vtkGetMacro(MaxAllowedTimeDifference, double); 

  /*! Make this buffer into a copy of another buffer. You should Lock both of the buffers before doing this. */
  void DeepCopy(vtkTrackerBuffer *buffer);

  /*! 
  Get the frame rate from the buffer based on the number of frames in the buffer
  and the elapsed time.
  Ideal frame rate shows the mean of the frame periods in the buffer based on the frame 
  number difference (aka the device frame rate)
  */
  virtual double GetFrameRate( bool ideal = false) { return this->TrackerBuffer->GetFrameRate(ideal); }

  /*! Clear buffer (set the buffer pointer to the first element) */
  virtual void Clear(); 

  /*! Set the local time offset (global = local + offset) */
  virtual void SetLocalTimeOffset(double offset);
  /*! Get the local time offset (global = local + offset) */
  virtual double GetLocalTimeOffset();

  /*! 
  Copy a specified transform to a tracker buffer. It is useful when tracking-only data is stored in a
  metafile (with dummy image data), which is read by a sequence metafile reader, and the 
  result is needed as a vtkTrackerBuffer.
  If useFilteredTimestamps is true, then the filtered timestamps that are stored in the buffer
  will be copied to the tracker buffer. If useFilteredTimestamps is false, then only unfiltered timestamps
  will be copied to the tracker buffer and the tracker buffer will compute the filtered timestamps.
  */
  PlusStatus CopyTransformFromTrackedFrameList(vtkTrackedFrameList *sourceTrackedFrameList, TIMESTAMP_FILTERING_OPTION timestampFiltering, PlusTransformName& transformName);

protected:
  vtkTrackerBuffer();
  ~vtkTrackerBuffer();

  /*! Returns the two buffer items that are closest previous and next buffer items relative to the specified time. itemA is the closest item */
  PlusStatus GetPrevNextBufferItemFromTime(double time, TrackerBufferItem& itemA, TrackerBufferItem& itemB);

  /*! 
  Interpolate the matrix for the given timestamp from the two nearest transforms in the buffer.
  The rotation is interpolated with SLERP interpolation, and the position is interpolated with linear interpolation.
  The flags correspond to the closest element.
  */
  virtual ItemStatus GetInterpolatedTrackerBufferItemFromTime( double time, TrackerBufferItem* bufferItem); 

  /*! Get tracker buffer item from an exact timestamp */
  virtual ItemStatus GetTrackerBufferItemFromExactTime( double time, TrackerBufferItem* bufferItem); 
  
  /*! Get tracker buffer item from the closest timestamp */
  virtual ItemStatus GetTrackerBufferItemFromClosestTime( double time, TrackerBufferItem* bufferItem);

  /*! Circular buffer container */
  typedef vtkTimestampedCircularBuffer<TrackerBufferItem> TrackerBufferType;
  TrackerBufferType* TrackerBuffer; 

  /*! Maximum allowed time difference in seconds between the desired and the closest valid timestamp */
  double MaxAllowedTimeDifference;

private:
  vtkTrackerBuffer(const vtkTrackerBuffer&);
  void operator=(const vtkTrackerBuffer&);
};

#endif
