#ifndef __vtkTrackerBuffer_h
#define __vtkTrackerBuffer_h

#include "PlusConfigure.h"
#include "vtkObject.h"
#include "vtkMatrix4x4.h"
#include "vtkTimestampedCircularBuffer.h"

class VTK_EXPORT TrackerBufferItem : public TimestampedBufferItem
{
public:

  TrackerBufferItem(); 
  ~TrackerBufferItem(); 
  TrackerBufferItem(const TrackerBufferItem& TrackerBufferItem); 
  TrackerBufferItem& TrackerBufferItem::operator=(TrackerBufferItem const& trackerBufferItem); 

  // Copy tracker buffer item 
  PlusStatus DeepCopy(TrackerBufferItem* trackerBufferItem); 

  // Set/get tracker matrix
  PlusStatus SetMatrix(vtkMatrix4x4* matrix); 
  PlusStatus GetMatrix(vtkMatrix4x4* outputMatrix);

  // Set/get tracker item status 
  void SetStatus(TrackerStatus status) { this->Status = status; }  
  TrackerStatus GetStatus() const { return this->Status; }

protected:
  vtkSmartPointer<vtkMatrix4x4> Matrix;
  TrackerStatus Status;       
}; 

class VTK_EXPORT vtkTrackerBuffer : public vtkObject
{
public:
  vtkTypeMacro(vtkTrackerBuffer,vtkObject);
  static vtkTrackerBuffer *New();

  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Set the size of the buffer, all new transforms are set to unity.
  PlusStatus SetBufferSize(int n);
  virtual int GetBufferSize();

  // Description:
  // Set/Get number of items used for timestamp filtering (with LSQR mimimizer)
  virtual void SetAveragedItemsForFiltering(int averagedItemsForFiltering); 

  // Description:
  // Set/get recording start time
  virtual void SetStartTime( double startTime ); 
  virtual double GetStartTime(); 

  // Description: 
  // Get the table report of the timestamped buffer 
  virtual PlusStatus GetTimeStampReportTable(vtkTable* timeStampReportTable); 

  // Description:
  // Get the number of items in the buffer
  int GetNumberOfItems() { return this->TrackerBuffer->GetNumberOfItems(); };

  // Description:
  // Add a matrix plus status to the list.  If the timestamp is
  // less than or equal to the previous timestamp, then nothing
  // will be done.
  PlusStatus AddTimeStampedItem(vtkMatrix4x4 *matrix, TrackerStatus status, unsigned long frameNumber, double unfilteredTimestamp);
  PlusStatus AddTimeStampedItem(vtkMatrix4x4 *matrix, TrackerStatus status, unsigned long frameNumber, double unfilteredTimestamp, double filteredTimestamp);

  // Description:
  // Get tracker item from buffer 
  // If calibratedItem true, it returns the calibrated tracker item ( apply ToolCalibrationMatrix and WorldCalibrationMatrix to tool calibration matrix)
  virtual ItemStatus GetTrackerBufferItem(BufferItemUidType uid, TrackerBufferItem* bufferItem, bool calibratedItem = false);
  virtual ItemStatus GetLatestTrackerBufferItem(TrackerBufferItem* bufferItem, bool calibratedItem = false) { return this->GetTrackerBufferItem( this->GetLatestItemUidInBuffer(), bufferItem, calibratedItem); }; 
  virtual ItemStatus GetOldestTrackerBufferItem(TrackerBufferItem* bufferItem, bool calibratedItem = false) { return this->GetTrackerBufferItem( this->GetOldestItemUidInBuffer(), bufferItem, calibratedItem); }; 

  // Description:
  // Get interpolated tracker item from buffer by time
  // If calibratedItem true, it returns the calibrated tracker item ( apply ToolCalibrationMatrix and WorldCalibrationMatrix to tool calibration matrix)
  virtual ItemStatus GetTrackerBufferItemFromTime( double time, TrackerBufferItem* bufferItem, bool calibratedItem = false); 

  // Description:
  // Get latest timestamp in the buffer 
  virtual ItemStatus GetLatestTimeStamp( double& latestTimestamp ); 

  // Description:
  // Get oldest timestamp in the buffer 
  virtual ItemStatus GetOldestTimeStamp( double& oldestTimestamp ); 

  // Description:
  // Get video buffer item timestamp 
  virtual ItemStatus GetTimeStamp( BufferItemUidType uid, double& timestamp); 

  // Description:
  // Get buffer item unique ID 
  virtual BufferItemUidType GetOldestItemUidInBuffer() { return this->TrackerBuffer->GetOldestItemUidInBuffer(); }
  virtual BufferItemUidType GetLatestItemUidInBuffer() { return this->TrackerBuffer->GetLatestItemUidInBuffer(); }
  virtual ItemStatus GetItemUidFromTime(double time, BufferItemUidType& uid) { return this->TrackerBuffer->GetItemUidFromTime(time, uid); }

  // Description:
  // Set a calibration matrices to be applied when GetMatrix() is called.
  vtkSetObjectMacro(ToolCalibrationMatrix,vtkMatrix4x4);
  vtkGetObjectMacro(ToolCalibrationMatrix,vtkMatrix4x4);
  vtkSetObjectMacro(WorldCalibrationMatrix,vtkMatrix4x4);
  vtkGetObjectMacro(WorldCalibrationMatrix,vtkMatrix4x4);

  // Description:
  // Set/get maximum allowed time difference in seconds between the desired and the closest valid timestamp
  vtkSetMacro(MaxAllowedTimeDifference, double); 
  vtkGetMacro(MaxAllowedTimeDifference, double); 

  // Description:
  // Make this buffer into a copy of another buffer.  You should
  // Lock both of the buffers before doing this.
  void DeepCopy(vtkTrackerBuffer *buffer);


  // Description:
  // Get the frame rate from the buffer based on the number of frames in the buffer
  // and the elapsed time.
  // Ideal frame rate shows the mean of the frame periods in the buffer based on the frame 
  // number difference (aka the device frame rate)
  virtual double GetFrameRate( bool ideal = false) { return this->TrackerBuffer->GetFrameRate(ideal); }

  // Description:
  // Clear buffer (set the buffer pointer to the first element)
  virtual void Clear(); 

  // Description:	
  // Get/Set the local time offset (global = local + offset)
  virtual void SetLocalTimeOffset(double offset);
  virtual double GetLocalTimeOffset();

protected:
  vtkTrackerBuffer();
  ~vtkTrackerBuffer();

  PlusStatus GetPrevNextBufferItemFromTime(double time, TrackerBufferItem& itemA, TrackerBufferItem& itemB, bool calibratedItem = false);

  vtkMatrix4x4 *ToolCalibrationMatrix;
  vtkMatrix4x4 *WorldCalibrationMatrix;

  typedef vtkTimestampedCircularBuffer<TrackerBufferItem> TrackerBufferType;
  TrackerBufferType* TrackerBuffer; 

  // Maximum allowed time difference in seconds between the desired and the closest valid timestamp
  double MaxAllowedTimeDifference;

private:
  vtkTrackerBuffer(const vtkTrackerBuffer&);
  void operator=(const vtkTrackerBuffer&);
};

#endif
