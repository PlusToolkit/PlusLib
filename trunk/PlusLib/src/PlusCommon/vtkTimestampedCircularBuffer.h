#ifndef __vtkTimestampedCircularBuffer_h
#define __vtkTimestampedCircularBuffer_h

#include "PlusConfigure.h"
#include "PlusMath.h"
#include "vtkObject.h"
#include "vtkObjectFactory.h"
#include "vtkTypeTemplate.h"
#include <deque>
#include <vector>
#include <queue>
#include "vtkCriticalSection.h"
#include "vtkTable.h"
#include "vtkVariantArray.h"
#include "vtkDoubleArray.h"
#include "vtkStringArray.h"
#include <iostream>
#include <sstream>

#include "vnl/vnl_matrix.h"
#include "vnl/vnl_vector.h"

typedef unsigned __int64 BufferItemUidType;
enum ItemStatus { ITEM_OK, ITEM_NOT_AVAILABLE_YET, ITEM_NOT_AVAILABLE_ANYMORE, ITEM_UNKNOWN_ERROR };

class VTK_EXPORT TimestampedBufferItem
{
public:
	// Get timestamp for the current buffer item in global time (global = local + offset)
	double GetTimestamp( double localTimeOffset) { return this->GetFilteredTimestamp(localTimeOffset); }
	
	// Get filtered timestamp in global time (global = local + offset)
	double GetFilteredTimestamp( double localTimeOffset) { return this->FilteredTimeStamp + localTimeOffset; }
	
	// Set filtered timestamp 
	void SetFilteredTimestamp( double filteredTimestamp) { this->FilteredTimeStamp = filteredTimestamp; }

	// Get unfiltered timestamp in global time (global = local + offset)
	double GetUnfilteredTimestamp( double localTimeOffset) { return this->UnfilteredTimeStamp + localTimeOffset; }
	
	// Set unfiltered timestamp 
	void SetUnfilteredTimestamp( double unfilteredTimestamp) { this->UnfilteredTimeStamp = unfilteredTimestamp; }

	// Set/get index assigned by the data acuiqisition system (usually a counter)
	unsigned long GetIndex() { return this->Index; }; 
	void SetIndex(unsigned long index) { this->Index = index; }; 

	// Set/get unique identifier assigned by the storage buffer
	BufferItemUidType GetUid() { return this->Uid; }; 
	void SetUid(BufferItemUidType uid) { this->Uid = uid; }; 

protected: 
	TimestampedBufferItem() 
	{ 
		this->FilteredTimeStamp = 0;  
		this->UnfilteredTimeStamp = 0;  
		this->Index = 0;  
		this->Uid = 0; 
	}; 

	~TimestampedBufferItem() {}; 

	TimestampedBufferItem(const TimestampedBufferItem& timestampedBufferItem)
	{
		this->FilteredTimeStamp = timestampedBufferItem.FilteredTimeStamp; 
		this->UnfilteredTimeStamp = timestampedBufferItem.UnfilteredTimeStamp; 
		this->Index = timestampedBufferItem.Index; 
		this->Uid = timestampedBufferItem.Uid; 
	}

	double FilteredTimeStamp;
	double UnfilteredTimeStamp;

	// index assigned by the data acuiqisition system (usually a counter)
	unsigned long Index; 

	// unique identifier assigned by the storage buffer
	BufferItemUidType Uid; 
}; 

template <typename BufferItemType>
class VTK_EXPORT vtkTimestampedCircularBuffer: public vtkTypeTemplate<vtkTimestampedCircularBuffer<BufferItemType>, vtkObject>
{
public:	
	static vtkTimestampedCircularBuffer<BufferItemType> *New();
	//vtkTypeRevisionMacro(vtkTimestampedCircularBuffer,vtkObject);
	void PrintSelf(ostream& os, vtkIndent indent);

	// Description:
	// Set/Get the size of the buffer, i.e. the maximum number of
	// video frames that it will hold.  The default is 30.
	virtual PlusStatus SetBufferSize(int n);
	virtual int GetBufferSize(); 

	// Description:
	// Get the number of items in the list (this is not the same as
	// the buffer size, but is rather the number of transforms that
	// have been added to the list).  This will never be greater than
	// the BufferSize.
	vtkGetMacro(NumberOfItems, int);

	// Description:
	// Given a timestamp, compute the nearest frame UID
	// This assumes that the times motonically increase
	virtual ItemStatus GetItemUidFromTime(const double time, BufferItemUidType& uid );

	// Description:
	// Get frame UID from buffer index
	virtual ItemStatus GetItemUidFromBufferIndex(const int bufferIndex, BufferItemUidType &uid ); 

	// Description:
	// Get the most recent frame UID that is already in the buffer
	virtual BufferItemUidType GetLatestItemUidInBuffer() 
	{ 
    this->Lock(); 
		BufferItemUidType latestUid = this->LatestItemUid;
		this->Unlock(); 
		return latestUid; 
	}

	// Description:
	// Get the oldest frame UID in the buffer 
	virtual BufferItemUidType GetOldestItemUidInBuffer() 
	{ 
    this->Lock(); 
		// LatestItemUid - ( NumberOfItems - 1 ) is the oldest element in the buffer
		BufferItemUidType oldestUid = ( this->GetLatestItemUidInBuffer() - (this->NumberOfItems - 1) ); 
    this->Unlock();
		return oldestUid; 
	} 
	
	// Description:
	// Get frame status by frame UID 
	virtual ItemStatus GetFrameStatus(const BufferItemUidType uid ); 

	// Description:
	// Get timestamp by frame UID associated with the buffer item 
	virtual ItemStatus GetLatestTimeStamp(double &timestamp) { return this->GetTimeStamp(this->GetLatestItemUidInBuffer(), timestamp); } ; 
  virtual ItemStatus GetOldestTimeStamp(double &timestamp) { return this->GetTimeStamp(this->GetOldestItemUidInBuffer(), timestamp); } ; 
	virtual ItemStatus GetTimeStamp(const BufferItemUidType uid, double &timestamp) { return this->GetFilteredTimeStamp(uid, timestamp); }
	virtual ItemStatus GetFilteredTimeStamp(const BufferItemUidType uid, double &filteredTimestamp); 
	virtual ItemStatus GetUnfilteredTimeStamp(const BufferItemUidType uid, double &unfilteredTimestamp); 

	// Description:
	// Get the index assigned by the data acuiqisition system (usually a counter) from the buffer by frame UID. 
	virtual ItemStatus GetIndex(const BufferItemUidType uid, unsigned long &index); 

	// Description:
	// Given a timestamp, compute the nearest buffer index 
	// This assumes that the times motonically increase
	virtual ItemStatus GetBufferIndexFromTime(const double time, int& bufferIndex );

	// Description:
	// Make this buffer into a copy of another buffer.  You should
	// Lock both of the buffers before doing this.
	virtual void DeepCopy(vtkTimestampedCircularBuffer<BufferItemType>* buffer); 

	// Description:	
	// Get/Set the local time offset (global = local + offset)
	vtkSetMacro(LocalTimeOffset, double); 
	vtkGetMacro(LocalTimeOffset, double);

	// Description:
	// Get the frame rate from the buffer based on the number of frames in the buffer
	// and the elapsed time.
	// Ideal frame rate shows the mean of the frame periods in the buffer based on the frame 
	// number difference (aka the device frame rate)
	virtual double GetFrameRate(bool ideal = false);

	// Description:
	// Clear buffer (set the buffer pointer to the first element)
	virtual void Clear(); 

	// Description:
	// Lock/Unlock the buffer: this should be done before changing or accessing
	// the data in the buffer if the buffer is being used from multiple
	// threads.  
	virtual void Lock();
	virtual void Unlock();

	// Description:
	// Get next writable buffer object
	// INTERNAL USE ONLY! Need to lock buffer until we use the buffer index 
	virtual BufferItemType* GetBufferItemFromBufferIndex(const int bufferIndex); 
	virtual BufferItemType* GetBufferItemFromUid(const BufferItemUidType uid); 

	virtual PlusStatus PrepareForNewItem(const double timestamp, BufferItemUidType& newFrameUid, int& bufferIndex); 

  // Description:
  // Create filtered and unfiltered timestamp for accurate timing of the buffer item.
  // The timing may be inaccurate because the timestamp is attached to the item when Plus receives it
  // and so the timestamp is affected by data transfer speed (which may slightly vary).
  // A line is fitted to the index and timestamp of the last (AveragedItemsForFiltering) items.
  // The filtered timestamp is the time value that corresponds to the frame index according to the fitted line.
  // If the filtered timestamp is very different from the non-filtered timestamp then 
  // filteredTimestampProbablyValid will be false and it is recommended not to use that item,
  // because its timestamp is probably incorrect.
  virtual PlusStatus CreateFilteredTimeStampForItem(unsigned long itemIndex, double inUnfilteredTimestamp, double &outFilteredTimestamp, bool &filteredTimestampProbablyValid); 
  
  // Description:
  // Set/Get number of items used for timestamp filtering (with LSQR mimimizer)
  vtkSetMacro(AveragedItemsForFiltering, int); 
  vtkGetMacro(AveragedItemsForFiltering, int); 

  // Description:
  // Recording start time
  vtkSetMacro(StartTime, double); 
  vtkGetMacro(StartTime, double); 
  
  // Description: 
  // Get the table report of the timestamped buffer 
  PlusStatus GetTimeStampReportTable(vtkTable* timeStampReportTable); 

protected:
	vtkTimestampedCircularBuffer();
	~vtkTimestampedCircularBuffer();

	// Description:
	// Get buffer index by frame UID - internal use only, the buffer should be locked 
	// Returns buffer index if the ItemStatus is ITEM_OK, otherwise -1; 
	virtual ItemStatus GetBufferIndex( BufferItemUidType uid, int& bufferIndex ); 

  // Description:
	// Initialize the timestamp report table by adding the proper cols
  virtual void InitTimeStampReportTable(); 

	vtkCriticalSection *Mutex;

	int NumberOfItems;
	// Next image will be written here
	int WritePointer;
	double CurrentTimeStamp;
	double LocalTimeOffset; 
	
  // This will be the UID of the next item that will be added.
  // The UID is monotonously increasing for each new frame.
	BufferItemUidType LatestItemUid; 
  
  std::deque<BufferItemType> BufferItemContainer; 

  // Matrix used for storing the last number of AveragedItemsForFiltering frame index 
  vnl_vector<double> FilterContainerIndexVector; 
  
  // Vector used for storing the last number of AveragedItemsForFiltering unfiltered timestamps
  vnl_vector<double> FilterContainerTimestampVector; 
  
  // Pointer to the next item index to write in the containers (usually the oldest one)
  int FilterContainersOldestIndex; 
  
  // Number of valid elements in the frame index and timestamp containers (maximum can be equal to AveragedItemsForFiltering)
  int FilterContainersNumberOfValidElements; 

  // Number of averaged items used for filtering - read from config files
  int AveragedItemsForFiltering; 

  // Maximum time difference that is allowed between filtered and the non-filtered timestamp (in seconds).
  // If the filtered value differs too much from the non-filtered one, then it rejects the filtering result.
  // This useful for making the timestamp filtering more robust (the LSQR fitting sometimes fails).
  double MaxAllowedFilteringTimeDifference;

  // Acquisition start time
  double StartTime; 

  // Table used for storing timestamp filtering results 
  vtkTable* TimeStampReportTable; 

private:
	vtkTimestampedCircularBuffer(const vtkTimestampedCircularBuffer&);
	void operator=(const vtkTimestampedCircularBuffer&);
};

#include "vtkTimestampedCircularBuffer.txx"

#endif
