/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkTimestampedCircularBuffer_h
#define __vtkTimestampedCircularBuffer_h

#include "PlusConfigure.h"
#include "StreamBufferItem.h"
#include "vtkObject.h"
#include "vtkTypeTemplate.h"
#include <deque>

#include "vnl/vnl_matrix.h"
#include "vnl/vnl_vector.h"

#include <float.h> // for DBL_MAX

class vtkRecursiveCriticalSection;
class vtkTable;

/*!
  Constant value used for indicating that a timestamp has an unknown value.
  It can be used to indicate that the timestamp has to be generated from the current time.
*/

enum ItemStatus { ITEM_OK, ITEM_NOT_AVAILABLE_YET, ITEM_NOT_AVAILABLE_ANYMORE, ITEM_UNKNOWN_ERROR };


/*!
  \class vtkTimestampedCircularBuffer
  \brief This class stores an fixed number of timestamped items.
  It provides element retrieval based on timestamp, temporal filtering and interpolation, etc.
  \ingroup PlusLibCommon
*/
class vtkTimestampedCircularBuffer: public vtkObject
{
public:  
  static vtkTimestampedCircularBuffer *New();
  void PrintSelf(ostream& os, vtkIndent indent);

  /*!
   Set/Get the size of the buffer, i.e. the maximum number of
   video frames that it will hold.  The default is 30.
  */
  virtual PlusStatus SetBufferSize(int n);
  virtual inline int GetBufferSize() { return this->BufferItemContainer.size(); }; 

  /*!
    Get the number of items in the list (this is not the same as
    the buffer size, but is rather the number of transforms that
    have been added to the list).  This will never be greater than
    the BufferSize.
  */
  vtkGetMacro(NumberOfItems, int);

  /*!
    Given a timestamp, compute the nearest frame UID
    This assumes that the times motonically increase
  */
  virtual ItemStatus GetItemUidFromTime(const double time, BufferItemUidType& uid );

  /*! Get the most recent frame UID that is already in the buffer */
  virtual BufferItemUidType GetLatestItemUidInBuffer() 
  { 
    this->Lock(); 
    BufferItemUidType latestUid = this->LatestItemUid;
    this->Unlock(); 
    return latestUid; 
  }

  /*! Get the oldest frame UID in the buffer  */
  virtual BufferItemUidType GetOldestItemUidInBuffer() 
  { 
    this->Lock(); 
    // LatestItemUid - ( NumberOfItems - 1 ) is the oldest element in the buffer
    BufferItemUidType oldestUid = this->LatestItemUid - (this->NumberOfItems - 1);
    this->Unlock();
    return oldestUid; 
  } 
  
  /*! Get timestamp by frame UID associated with the buffer item  */
  virtual ItemStatus GetLatestTimeStamp(double &timestamp)
  {
    return this->GetTimeStamp(this->GetLatestItemUidInBuffer(), timestamp);
  }

  virtual ItemStatus GetOldestTimeStamp(double &timestamp)
  {
    // The oldest item may be removed from the buffer at any moment
    // therefore we need to retrieve its UID and timestamp within a single lock
    this->Lock(); 
    // LatestItemUid - ( NumberOfItems - 1 ) is the oldest element in the buffer
    BufferItemUidType oldestUid = ( this->LatestItemUid - (this->NumberOfItems - 1) );
    ItemStatus status = this->GetTimeStamp(oldestUid, timestamp);
    this->Unlock();
    return status;
  }

  virtual ItemStatus GetTimeStamp(const BufferItemUidType uid, double &timestamp) { return this->GetFilteredTimeStamp(uid, timestamp); }
  virtual ItemStatus GetFilteredTimeStamp(const BufferItemUidType uid, double &filteredTimestamp); 
  virtual ItemStatus GetUnfilteredTimeStamp(const BufferItemUidType uid, double &unfilteredTimestamp);
  
  virtual bool GetLatestItemHasValidVideoData();
  virtual bool GetLatestItemHasValidTransformData();


  /*! Get the index assigned by the data acuiqisition system (usually a counter) from the buffer by frame UID. */
  virtual ItemStatus GetIndex(const BufferItemUidType uid, unsigned long &index); 

  /*!
    Given a timestamp, compute the nearest buffer index 
    This assumes that the times motonically increase
  */
  virtual ItemStatus GetBufferIndexFromTime(const double time, int& bufferIndex );

  /*!
    Make this buffer into a copy of another buffer.  You should
    Lock both of the buffers before doing this.
  */
  virtual void DeepCopy(vtkTimestampedCircularBuffer* buffer); 

  /*!  Set the local time offset in seconds (global = local + offset) */
  vtkSetMacro(LocalTimeOffsetSec, double); 
  /*!  Get the local time offset in seconds (global = local + offset) */
  vtkGetMacro(LocalTimeOffsetSec, double);

  /*!
    Get the frame rate from the buffer based on the number of frames in the buffer
    and the elapsed time.
    Ideal frame rate shows the mean of the frame periods in the buffer based on the frame 
    number difference (a.k.a. the device frame rate, a.k.a. the frame rate that would have been achieved
    if frames were not dropped).
    If framePeriodStdevSecPtr is not null, then the standard deviation of the frame period is computed as well (in seconds) and
    stored at the specified address.
  */
  virtual double GetFrameRate(bool ideal = false, double *framePeriodStdevSecPtr=NULL );

  /*! Clear buffer (set the buffer pointer to the first element) */
  virtual void Clear(); 

  /*!
    Lock the buffer: this should be done before changing or accessing
    the data in the buffer if the buffer is being used from multiple
    threads.  
  */
  inline void Lock() { this->Mutex->Lock(); };
  /*!
    Unlock the buffer: this should be done before changing or accessing
    the data in the buffer if the buffer is being used from multiple
    threads.  
  */
  inline void Unlock() { this->Mutex->Unlock(); };

  /*!
    Get next writable buffer object
    INTERNAL USE ONLY! Need to lock buffer until we use the buffer index 
  */
  virtual StreamBufferItem* GetBufferItemPointerFromBufferIndex(const int bufferIndex); 

  /*!
    Get next writable buffer object
    INTERNAL USE ONLY! Need to lock buffer until we use the buffer index 
  */
  virtual ItemStatus GetBufferItemPointerFromUid(const BufferItemUidType uid, StreamBufferItem* &itemPtr);

  virtual PlusStatus PrepareForNewItem(const double timestamp, BufferItemUidType& newFrameUid, int& bufferIndex); 

  /*!
    Create filtered and unfiltered timestamp for accurate timing of the buffer item.
    The timing may be inaccurate because the timestamp is attached to the item when Plus receives it
    and so the timestamp is affected by data transfer speed (which may slightly vary).
    A line is fitted to the index and timestamp of the last (AveragedItemsForFiltering) items.
    The filtered timestamp is the time value that corresponds to the frame index according to the fitted line.
    If the filtered timestamp is very different from the non-filtered timestamp then 
    filteredTimestampProbablyValid will be false and it is recommended not to use that item,
    because its timestamp is probably incorrect.
  */
  virtual PlusStatus CreateFilteredTimeStampForItem(unsigned long itemIndex, double inUnfilteredTimestamp, double &outFilteredTimestamp, bool &filteredTimestampProbablyValid); 

  /*! Add values to the timestamp report. If reporting is not enabled then no values will be added. This should only be called if an item is added without calling CreateFilteredTimeStampForItem. */
  void AddToTimeStampReport(unsigned long itemIndex, double unfilteredTimestamp, double filteredTimestamp);

  /*! Get the table report of the timestamped buffer. To fill this table TimeStampReporting has to be enabled.  */
  PlusStatus GetTimeStampReportTable(vtkTable* timeStampReportTable); 

  /*! If TimeStampReporting is enabled then all filtered and unfiltered timestamp values will be saved in a table for diagnostic purposes. */
  vtkSetMacro(TimeStampReporting,bool);
  vtkGetMacro(TimeStampReporting,bool); 
  vtkBooleanMacro(TimeStampReporting,bool);  

  /*! If TimeStampLogging is enabled then the timestamps and frame indexes that are used for filtering will be logged at TRACE level for diagnostic purposes. */
  vtkSetMacro(TimeStampLogging,bool);
  vtkGetMacro(TimeStampLogging,bool); 
  vtkBooleanMacro(TimeStampLogging,bool);  
  
  /*! Set number of items used for timestamp filtering (with LSQR mimimizer) */
  vtkSetMacro(AveragedItemsForFiltering, unsigned int); 
  /*! Get number of items used for timestamp filtering (with LSQR mimimizer) */
  vtkGetMacro(AveragedItemsForFiltering, int); 

  /*! Set recording start time */
  vtkSetMacro(StartTime, double); 
  /*! Get recording start time */
  vtkGetMacro(StartTime, double); 

protected:
  vtkTimestampedCircularBuffer();
  ~vtkTimestampedCircularBuffer();

protected:
  vtkRecursiveCriticalSection *Mutex;

  int NumberOfItems;

  /*! Next image will be written here */
  int WritePointer;

  double CurrentTimeStamp;

  /*! Time offset of the buffer in seconds */
  double LocalTimeOffsetSec; 
  
  /*!
    This will be the UID of the next item that will be added.
    The UID is monotonously increasing for each new frame.
  */
  BufferItemUidType LatestItemUid; 
  
  std::deque<StreamBufferItem> BufferItemContainer; 

  /*! Matrix used for storing the last number of AveragedItemsForFiltering frame index */
  vnl_vector<double> FilterContainerIndexVector; 
  
  /*! Vector used for storing the last number of AveragedItemsForFiltering unfiltered timestamps */
  vnl_vector<double> FilterContainerTimestampVector; 
  
  /*! Pointer to the next item index to write in the containers (usually the oldest one) */
  int FilterContainersOldestIndex; 
  
  /*! Number of valid elements in the frame index and timestamp containers (maximum can be equal to AveragedItemsForFiltering) */
  int FilterContainersNumberOfValidElements; 

  /*! Number of averaged items used for filtering - read from config files */
  unsigned int AveragedItemsForFiltering; 

  /*!
    Maximum time difference that is allowed between filtered and the non-filtered timestamp (in seconds).
    If the filtered value differs too much from the non-filtered one, then it rejects the filtering result.
    This useful for making the timestamp filtering more robust (the LSQR fitting sometimes fails).
  */
  double MaxAllowedFilteringTimeDifference;

  /*! Acquisition start time */
  double StartTime; 

  /*! Table used for storing timestamp filtering results */
  vtkTable* TimeStampReportTable; 

  /*!
    If TimeStampReporting is enabled then all filtered and unfiltered timestamp values will be saved in
    a table. As the table is continuously growing it should be enabled only temporarily, for diagnostic purposes.
  */
  bool TimeStampReporting;
  /*! 
    If TimeStampReporting is enabled then all filtered and unfiltered timestamp values will be saved in a table for diagnostic purposes.
    It generates quite a lot of output in the logs, so it is recommended to use only for diagnostic purposes.
  */
  bool TimeStampLogging;

  /*!
    Due to numerical inaccuracies (e.g, saving a timestamp to a string and reading from it results in a slightly different value)
    it's better to use a tolerance value when making comparisons.
  */
  double NegligibleTimeDifferenceSec;

private:
  vtkTimestampedCircularBuffer(const vtkTimestampedCircularBuffer&);
  void operator=(const vtkTimestampedCircularBuffer&);
};

#endif
