/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"
#include "vtkTimestampedCircularBuffer.h"

#include "vtkDoubleArray.h"
#include "vtkRecursiveCriticalSection.h"
#include "vtkTable.h"
#include "vtkVariantArray.h"

vtkStandardNewMacro(vtkTimestampedCircularBuffer);

//----------------------------------------------------------------------------
vtkTimestampedCircularBuffer::vtkTimestampedCircularBuffer()
{
  this->BufferItemContainer.resize(0); 
  this->Mutex = vtkRecursiveCriticalSection::New();
  this->NumberOfItems = 0;
  this->WritePointer = 0;
  this->CurrentTimeStamp = 0.0;
  this->LocalTimeOffsetSec = 0.0; 
  this->LatestItemUid = 0; 

  this->FilterContainerIndexVector.set_size(0); 
  this->FilterContainerTimestampVector.set_size(0); 
  this->FilterContainersOldestIndex = 0; 
  this->FilterContainersNumberOfValidElements = 0; 

  this->AveragedItemsForFiltering = 20;
  this->MaxAllowedFilteringTimeDifference=0.500; // sec
   
  this->TimeStampReportTable = NULL; 
  this->TimeStampReporting = false;

  this->TimeStampLogging = false;
  
  this->StartTime = 0; 
  
  this->NegligibleTimeDifferenceSec=1e-5;

}

//----------------------------------------------------------------------------
vtkTimestampedCircularBuffer::~vtkTimestampedCircularBuffer()
{ 
  this->BufferItemContainer.clear(); 

  this->NumberOfItems = 0;
  if ( this->Mutex != NULL )
  {
    this->Mutex->Delete();
    this->Mutex = NULL; 
  }

  if ( this->TimeStampReportTable != NULL )
  {
    this->TimeStampReportTable->Delete();
    this->TimeStampReportTable = NULL; 
  }

}

//----------------------------------------------------------------------------
void vtkTimestampedCircularBuffer::PrintSelf(ostream& os, vtkIndent indent)
{
  //this->Superclass::PrintSelf(os,indent);

  os << indent << "BufferSize: " << this->GetBufferSize() << "\n";
  os << indent << "NumberOfItems: " << this->NumberOfItems << "\n";
  os << indent << "CurrentTimeStamp: " << this->CurrentTimeStamp << "\n";
  os << indent << "Local time offset: " << this->LocalTimeOffsetSec << "\n";
  os << indent << "Latest Item Uid: " << this->LatestItemUid << "\n";
}

//----------------------------------------------------------------------------
PlusStatus vtkTimestampedCircularBuffer::PrepareForNewItem(const double timestamp, BufferItemUidType& newFrameUid, int& bufferIndex)
{
  PlusLockGuard< vtkTimestampedCircularBuffer > bufferGuardedLock(this);

  if ( timestamp <= this->CurrentTimeStamp )
  {
    LOG_DEBUG("Need to skip newly added frame - new timestamp ("<< std::fixed << timestamp << ") is not newer than the last one (" << this->CurrentTimeStamp << ")!"); 
    return PLUS_FAIL; 
  }

  // Increase frame unique ID
  newFrameUid = ++this->LatestItemUid; 
  bufferIndex = this->WritePointer; 
  this->CurrentTimeStamp = timestamp; 

  this->NumberOfItems++;
  if (this->NumberOfItems > this->GetBufferSize())
  {
    this->NumberOfItems = this->GetBufferSize();
  }
  // Increase the write pointer
  if ( ++this->WritePointer >= this->GetBufferSize() )
  {
    this->WritePointer = 0; 
  }

  return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
// Sets the buffer size, and copies the maximum number of the most current old
// frames and timestamps
PlusStatus vtkTimestampedCircularBuffer::SetBufferSize(int newBufferSize)
{
  if (newBufferSize < 0)
  {
    LOG_ERROR("SetBufferSize: invalid buffer size");
    return PLUS_FAIL;
  }
  
  PlusLockGuard< vtkTimestampedCircularBuffer > bufferGuardedLock(this);

  if (newBufferSize == this->GetBufferSize() && newBufferSize != 0)
  {
    return PLUS_SUCCESS;
  }

  if ( this->GetBufferSize() == 0 )
  {
    for ( int i = 0; i < newBufferSize; i++ )
    {
      StreamBufferItem emptyBufferItem; 
      this->BufferItemContainer.push_back(emptyBufferItem); 
    }
    this->WritePointer = 0;
    this->NumberOfItems = 0;
    this->CurrentTimeStamp = 0.0;
  }
  // if the new buffer is bigger than the old buffer
  else if ( this->GetBufferSize() < newBufferSize )
  {
    std::deque<StreamBufferItem>::iterator it = this->BufferItemContainer.begin() + this->WritePointer; 
    const int numberOfNewBufferObjects = newBufferSize - this->GetBufferSize(); 
    for ( int i = 0; i < numberOfNewBufferObjects; ++i )
    {
      StreamBufferItem emptyBufferItem;
      it = this->BufferItemContainer.insert(it, emptyBufferItem); 
    }
  }
  // if the new buffer is smaller than the old buffer
  else if ( this->GetBufferSize() > newBufferSize )
  {
    // delete the oldest buffer objects
    int oldBufferSize = this->GetBufferSize();
    for (int i = 0; i < oldBufferSize - newBufferSize; ++i)
    {
      std::deque<StreamBufferItem>::iterator it = this->BufferItemContainer.begin() + this->WritePointer; 
      this->BufferItemContainer.erase(it); 
      if ( this->WritePointer >= this->GetBufferSize() )
      {
        this->WritePointer = 0; 
      }
    }
  }

  // update the number of items
  if (this->NumberOfItems > this->GetBufferSize())
  {
    this->NumberOfItems = this->GetBufferSize();
  }

  this->Modified();

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
ItemStatus vtkTimestampedCircularBuffer::GetBufferItemPointerFromUid(const BufferItemUidType uid, StreamBufferItem* &itemPtr) 
{ 
  // the caller must have locked the buffer
  BufferItemUidType oldestUid = this->LatestItemUid - (this->NumberOfItems - 1);
  if ( uid < oldestUid ) 
  {
    LOG_WARNING("Buffer item is not in the buffer (Uid: " << uid << ")!"); 
    itemPtr = NULL;
    return ITEM_NOT_AVAILABLE_ANYMORE;
  }
  else if ( uid > this->LatestItemUid )
  {
    LOG_WARNING("Buffer item is not in the buffer (Uid: " << uid << ")!"); 
    itemPtr = NULL;
    return ITEM_NOT_AVAILABLE_YET;
  }
  int bufferIndex = (this->WritePointer - 1) - (this->LatestItemUid - uid);  
  if ( bufferIndex < 0 )
  {
    bufferIndex += this->BufferItemContainer.size();
  }
  itemPtr = &this->BufferItemContainer[bufferIndex];
  return ITEM_OK;
}

//----------------------------------------------------------------------------
StreamBufferItem* vtkTimestampedCircularBuffer::GetBufferItemPointerFromBufferIndex(const int bufferIndex) 
{ 
  // the caller must have locked the buffer
  if (this->GetBufferSize() <= 0 
    || bufferIndex >= this->GetBufferSize() 
    || bufferIndex < 0 )
  {
    LOG_ERROR("Failed to get buffer item with buffer index - index is out of range (bufferIndex: " << bufferIndex << ")."); 
    return NULL;
  }
  return &this->BufferItemContainer[bufferIndex]; 
}

//----------------------------------------------------------------------------
ItemStatus vtkTimestampedCircularBuffer::GetFilteredTimeStamp(const BufferItemUidType uid, double &filteredTimestamp)
{
  PlusLockGuard< vtkTimestampedCircularBuffer > bufferGuardedLock(this);
  StreamBufferItem* itemPtr = NULL;
  ItemStatus status = GetBufferItemPointerFromUid(uid, itemPtr);
  if ( status!=ITEM_OK ) 
  {
    filteredTimestamp = 0;
    return status;
  }
  filteredTimestamp = itemPtr->GetFilteredTimestamp(this->LocalTimeOffsetSec);
  return status;
}

//----------------------------------------------------------------------------
ItemStatus vtkTimestampedCircularBuffer::GetUnfilteredTimeStamp(const BufferItemUidType uid, double &unfilteredTimestamp)
{ 
  PlusLockGuard< vtkTimestampedCircularBuffer > bufferGuardedLock(this);
  StreamBufferItem* itemPtr = NULL;
  ItemStatus status = GetBufferItemPointerFromUid(uid, itemPtr);
  if ( status!=ITEM_OK ) 
  {
    unfilteredTimestamp = 0;
    return status;
  }
  unfilteredTimestamp = itemPtr->GetUnfilteredTimestamp(this->LocalTimeOffsetSec);
  return status;
}

//----------------------------------------------------------------------------
bool vtkTimestampedCircularBuffer::GetLatestItemHasValidVideoData()
{ 
  PlusLockGuard< vtkTimestampedCircularBuffer > bufferGuardedLock(this);
  if (this->NumberOfItems<1)
  {
    return false;
  }
  int latestItemBufferIndex = (this->WritePointer>0) ? (this->WritePointer - 1) : (this->BufferItemContainer.size()-1);
  return this->BufferItemContainer[latestItemBufferIndex].HasValidVideoData();
}

//----------------------------------------------------------------------------
bool vtkTimestampedCircularBuffer::GetLatestItemHasValidTransformData()
{ 
  PlusLockGuard< vtkTimestampedCircularBuffer > bufferGuardedLock(this);
  if (this->NumberOfItems<1)
  {
    return false;
  }
  int latestItemBufferIndex = (this->WritePointer>0) ? (this->WritePointer - 1) : (this->BufferItemContainer.size()-1);
  return this->BufferItemContainer[latestItemBufferIndex].HasValidTransformData();
}

//----------------------------------------------------------------------------
ItemStatus vtkTimestampedCircularBuffer::GetIndex(const BufferItemUidType uid, unsigned long &index)
{ 
  PlusLockGuard< vtkTimestampedCircularBuffer > bufferGuardedLock(this);
  StreamBufferItem* itemPtr = NULL;
  ItemStatus status = GetBufferItemPointerFromUid(uid, itemPtr);
  if ( status!=ITEM_OK ) 
  {
    index = 0;
    return status;
  }
  index = itemPtr->GetIndex();
  return status;
}


//----------------------------------------------------------------------------
ItemStatus vtkTimestampedCircularBuffer::GetBufferIndexFromTime(const double time, int& bufferIndex )
{
  PlusLockGuard< vtkTimestampedCircularBuffer > bufferGuardedLock(this);
  bufferIndex = -1;

  BufferItemUidType itemUid=0;
  ItemStatus itemStatus = this->GetItemUidFromTime(time, itemUid); 
  if ( itemStatus != ITEM_OK )
  {
    LOG_WARNING("Buffer item is not in the buffer (time: " << std::fixed << time << ")!"); 
    return itemStatus; 
  } 
  
  bufferIndex = (this->WritePointer - 1) - (this->LatestItemUid - itemUid);
  if ( bufferIndex < 0 )
  {
    bufferIndex += this->BufferItemContainer.size();
  }
  return ITEM_OK;
}

//----------------------------------------------------------------------------
// do a simple divide-and-conquer search for the transform
// that best matches the given timestamp
ItemStatus vtkTimestampedCircularBuffer::GetItemUidFromTime(const double time, BufferItemUidType& uid )
{
  PlusLockGuard< vtkTimestampedCircularBuffer > bufferGuardedLock(this);

  if (this->NumberOfItems==1)
  {
    // There is only one item, it's the closest one to any timestamp
    uid=this->LatestItemUid; 
    return ITEM_OK;
  }

  BufferItemUidType lo = this->LatestItemUid - (this->NumberOfItems - 1); // oldest item UID
  BufferItemUidType hi = this->LatestItemUid; // latest item UID

  // minimum time
  // This method is called often, therefore instead of calling this->GetTimeStamp(lo, tlo) we perform low-level operations to get the timestamp
  int loBufferIndex = (this->WritePointer - 1) - (this->LatestItemUid - lo);  
  if ( loBufferIndex < 0 )
  {
    loBufferIndex += this->BufferItemContainer.size();
  }
  double tlo = this->BufferItemContainer[loBufferIndex].GetFilteredTimestamp(this->LocalTimeOffsetSec);

  // This method is called often, therefore instead of calling this->GetTimeStamp(hi, thi) we perform low-level operations to get the timestamp
  int hiBufferIndex = (this->WritePointer - 1) - (this->LatestItemUid - hi);  
  if ( hiBufferIndex < 0 )
  {
    hiBufferIndex += this->BufferItemContainer.size();
  }
  double thi = this->BufferItemContainer[hiBufferIndex].GetFilteredTimestamp(this->LocalTimeOffsetSec);

  // If the timestamp is slightly out of range then still accept it
  // (due to errors in conversions there could be slight differences)
  if (time < tlo-this->NegligibleTimeDifferenceSec)
  {
    return ITEM_NOT_AVAILABLE_ANYMORE; 
  }
  else if (time > thi+this->NegligibleTimeDifferenceSec)
  {
    return ITEM_NOT_AVAILABLE_YET;
  }

  for (;;)
  {
    if (hi-lo <= 1)
    {
      if (time - tlo > thi - time)
      {
        uid = hi; 
        return ITEM_OK;
      }
      else
      {
        uid = lo; 
        return ITEM_OK;
      }
    }

    int mid = (lo+hi)/2;

    // This is a hot loop, therefore instead of calling this->GetTimeStamp(mid, tmid) we perform low-level operations to get the timestamp
    int midBufferIndex = (this->WritePointer - 1) - (this->LatestItemUid - mid);  
    if ( midBufferIndex < 0 )
    {
      midBufferIndex += this->BufferItemContainer.size();
    }
    double tmid = this->BufferItemContainer[midBufferIndex].GetFilteredTimestamp(this->LocalTimeOffsetSec);

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
void vtkTimestampedCircularBuffer::DeepCopy(vtkTimestampedCircularBuffer* buffer)
{
  buffer->Lock(); 
  this->Lock(); 
  this->WritePointer = buffer->WritePointer;
  this->NumberOfItems = buffer->NumberOfItems;
  this->CurrentTimeStamp = buffer->CurrentTimeStamp;
  this->LocalTimeOffsetSec = buffer->LocalTimeOffsetSec;
  this->LatestItemUid = buffer->LatestItemUid; 
  this->StartTime = buffer->StartTime; 
  this->AveragedItemsForFiltering = buffer->AveragedItemsForFiltering; 
  this->FilterContainersNumberOfValidElements = buffer->FilterContainersNumberOfValidElements; 
  this->FilterContainersOldestIndex = buffer->FilterContainersOldestIndex; 
  this->FilterContainerTimestampVector = buffer->FilterContainerTimestampVector; 
  this->FilterContainerIndexVector = buffer->FilterContainerIndexVector; 

  this->BufferItemContainer = buffer->BufferItemContainer; 
  this->Unlock(); 
  buffer->Unlock(); 
}

//----------------------------------------------------------------------------
void vtkTimestampedCircularBuffer::Clear()
{
  this->Lock(); 
  this->WritePointer = 0; 
  this->NumberOfItems = 0; 
  this->CurrentTimeStamp = 0; 
  this->LatestItemUid = 0;
  this->Unlock();
}

//----------------------------------------------------------------------------
double vtkTimestampedCircularBuffer::GetFrameRate(bool ideal /*=false*/, double *framePeriodStdevSecPtr /* =NULL */)
{
  // TODO: Start the frame rate computation from the latest frame UID with using a few seconds of items in the buffer
  bool cannotComputeIdealFrameRateDueToInvalidFrameNumbers=false;
  
  std::vector<double> framePeriods; 
  for ( BufferItemUidType frame = this->GetLatestItemUidInBuffer(); frame > this->GetOldestItemUidInBuffer(); --frame )
  {
    double time(0); 
    if ( this->GetTimeStamp(frame, time) != ITEM_OK )
    {
      continue; 
    }

    unsigned long framenum(0); 
    if ( this->GetIndex(frame, framenum) != ITEM_OK)
    {
      continue; 
    }
    
    double prevtime(0); 
    if ( this->GetTimeStamp(frame - 1, prevtime) != ITEM_OK )
    {
      continue; 
    }
    
    unsigned long prevframenum(0); 
    if ( this->GetIndex(frame - 1, prevframenum) != ITEM_OK)
    {
      continue; 
    }
     
    double frameperiod = (time - prevtime); 
    int frameDiff = framenum - prevframenum;

    if ( ideal )
    {
      if (frameDiff > 0)
      {
        frameperiod /= (1.0 * frameDiff);
      }
      else
      {
        // the same frame number was set for different frame indexes; this should not happen (probably no frame number is available)
        cannotComputeIdealFrameRateDueToInvalidFrameNumbers=true;        
      }
    }

    if ( frameperiod > 0 )
    {
      framePeriods.push_back(frameperiod); 
    }
  }
  
  if (cannotComputeIdealFrameRateDueToInvalidFrameNumbers)
  {
    LOG_WARNING("Cannot compute ideal frame rate acurately, as frame numbers are invalid or missing");
  }

  const int numberOfFramePeriods =  framePeriods.size(); 
  if (numberOfFramePeriods<1)
  {
    LOG_WARNING("Failed to compute frame rate. Not enough samples.");
    return 0;
  }
  
  double samplingPeriod(0); 
  for ( int i = 0; i < numberOfFramePeriods; i++ )
  {
    samplingPeriod += framePeriods[i];
  }
  samplingPeriod /= 1.0 * numberOfFramePeriods;

  double frameRate(0); 
  if ( samplingPeriod != 0 )
  {
    frameRate = 1.0/samplingPeriod;
  }
  
  if (framePeriodStdevSecPtr!=NULL)
  {
    // Standard deviation of sampling period
    // stdev = sqrt ( 1/N * sum[ (xi-mean)^2 ] ) = sqrt ( 1/N * sumOfXiMeanDiffSquare )
    double sumOfXiMeanDiffSquare=0;
    for ( int i = 0; i < numberOfFramePeriods; i++ )
    {
      sumOfXiMeanDiffSquare += (framePeriods[i]-samplingPeriod)*(framePeriods[i]-samplingPeriod);
    }    
    double framePeriodStdev = sqrt (sumOfXiMeanDiffSquare/numberOfFramePeriods);
    (*framePeriodStdevSecPtr) = framePeriodStdev;
  }

  return frameRate; 
}

//----------------------------------------------------------------------------
// for accurate timing of the frame: an exponential moving average
// is computed to smooth out the jitter in the times that are returned by the system clock:
PlusStatus vtkTimestampedCircularBuffer::CreateFilteredTimeStampForItem(unsigned long itemIndex, double inUnfilteredTimestamp, double &outFilteredTimestamp, bool &filteredTimestampProbablyValid)
{
  this->Lock();   
  filteredTimestampProbablyValid=true;

  if ( this->FilterContainerIndexVector.size() != this->AveragedItemsForFiltering 
    || this->FilterContainerTimestampVector.size() != this->AveragedItemsForFiltering )
  {
    // this call set elements to null
    this->FilterContainerIndexVector.set_size(this->AveragedItemsForFiltering); 
    this->FilterContainerTimestampVector.set_size(this->AveragedItemsForFiltering); 
    this->FilterContainersOldestIndex = 0; 
    this->FilterContainersNumberOfValidElements = 0; 
  }

  // We store the last AveragedItemsForFiltering unfiltered timestamp and item indexes, because these are used for computing the filtered timestamp.
  if ( this->AveragedItemsForFiltering > 1 )
  {
    this->FilterContainerIndexVector(this->FilterContainersOldestIndex) = itemIndex; 
    this->FilterContainerTimestampVector[this->FilterContainersOldestIndex] = inUnfilteredTimestamp; 
    this->FilterContainersNumberOfValidElements++; 
    this->FilterContainersOldestIndex++; 

    if ( this->FilterContainersNumberOfValidElements > this->AveragedItemsForFiltering )
    {
      this->FilterContainersNumberOfValidElements = this->AveragedItemsForFiltering; 
    }

    if ( this->FilterContainersOldestIndex >= this->AveragedItemsForFiltering )
    {
      this->FilterContainersOldestIndex = 0; 
    }
  }
  
  // If we don't have enough unfiltered timestamps or we don't want to use afiltering then just use the unfiltered timestamps
  if ( this->AveragedItemsForFiltering < 2 || this->FilterContainersNumberOfValidElements < this->AveragedItemsForFiltering )
  {  
    outFilteredTimestamp = inUnfilteredTimestamp; 
    AddToTimeStampReport(itemIndex, inUnfilteredTimestamp, outFilteredTimestamp);  
    this->Unlock(); 
    return PLUS_SUCCESS; 
  }
  
  // The items are acquired periodically, with quite accurate frame periods. The data is not timestamped
  // by the source, only Plus attaches a timestamp when it receives the data. The timestamp that Plus attaches
  // (the unfiltered timestamp) may be inaccurate, due to random delays in transferring the data.
  // Without the random delays (and if the acquisition frame rate is constant) the itemIndex vs. timestamp function would be a straight line.
  // With the random delays small spikes appear on this line, causing inaccuracies.
  // Get rid of the small spikes and get a smooth straight line by fitting a line (timestamp = itemIndex * framePeriod + timeOffset) to the
  // itemIndex vs. unfiltered timestamp function and compute the current filtered timestamp
  // by extrapolation of this line to the current item index.
  // The line parameters computed by linear regression. 
  //
  // timestamp = framePeriod * itemIndex+ timeOffset
  //   x = itemIndex
  //   y = timestamp
  //   a = framePeriod
  //   b = timeOffset
  //
  // Ordinary least squares estimation:
  //   y(i) = a * x(i) + b;
  //   a = sum( (x(i)-xMean) * (y(i)-yMean) ) / sum( (x(i)-xMean) * (x(i)-xMean) )
  //   b = yMean - a*xMean
  // 

  double xMean=this->FilterContainerIndexVector.mean();
  double yMean=this->FilterContainerTimestampVector.mean();
  double covarianceXY=0;
  double varianceX=0;
  for (int i=this->FilterContainerTimestampVector.size()-1; i>=0; i--)
  {
    double xiMinusXmean=(this->FilterContainerIndexVector(i)-xMean);
    covarianceXY+=xiMinusXmean*(this->FilterContainerTimestampVector(i)-yMean);
    varianceX+=xiMinusXmean*xiMinusXmean;    
  }
  double a=covarianceXY/varianceX;
  double b=yMean-a*xMean;

  outFilteredTimestamp = a * itemIndex + b; 

  if (this->TimeStampLogging)
  {
    LOG_TRACE("timestamps = [" << std::fixed << this->FilterContainerTimestampVector << "];");
    LOG_TRACE("frameindexes = [" << std::fixed << this->FilterContainerIndexVector << "];");
  }

  AddToTimeStampReport(itemIndex, inUnfilteredTimestamp, outFilteredTimestamp);  

  if (fabs(outFilteredTimestamp-inUnfilteredTimestamp)>this->MaxAllowedFilteringTimeDifference)
  { 
    // Write current timestamps and frame indexes to the log to allow investigation of the problem
    filteredTimestampProbablyValid=false;
    LOG_DEBUG("Difference between unfiltered timestamp is larger than the threshold. The unfiltered timestamp may be incorrect."
      << " Unfiltered timestamp: "<<inUnfilteredTimestamp<<", filtered timestamp: "<<outFilteredTimestamp<<", difference: "<<fabs(outFilteredTimestamp-inUnfilteredTimestamp)<<", threshold: "<<this->MaxAllowedFilteringTimeDifference<<"."
      << " timestamps = [" << std::fixed << this->FilterContainerTimestampVector << "];"
      << " frameindexes = [" << std::fixed << this->FilterContainerIndexVector << "];");  
  }

  this->Unlock(); 
  return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
PlusStatus vtkTimestampedCircularBuffer::GetTimeStampReportTable(vtkTable* timeStampReportTable)
{
  if ( timeStampReportTable == NULL )
  {
      LOG_ERROR("Failed to get timestamp report table from buffer - input table is NULL!"); 
      return PLUS_FAIL; 
  }

  if ( this->TimeStampReportTable == NULL )
  {
      LOG_ERROR("Failed to get timestamp report table from buffer - table is NULL!"); 
      return PLUS_FAIL; 
  }

  this->Lock(); 
  timeStampReportTable->DeepCopy(this->TimeStampReportTable); 
  this->Unlock(); 

  return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
void vtkTimestampedCircularBuffer::AddToTimeStampReport(unsigned long itemIndex, double unfilteredTimestamp, double filteredTimestamp)
{
  if (!this->TimeStampReporting)
  {
    // no reporting is needed
    return;
  }
      
  if ( this->TimeStampReportTable == NULL )
  {
    this->TimeStampReportTable = vtkTable::New();   

    const char* colFrameNumberName = "FrameNumber"; 
    vtkSmartPointer<vtkDoubleArray> colFrameNumber = vtkSmartPointer<vtkDoubleArray>::New(); 
    colFrameNumber->SetName(colFrameNumberName); 
    this->TimeStampReportTable->AddColumn(colFrameNumber); 

    const char* colUnfilteredTimestampName = "UnfilteredTimestamp"; 
    vtkSmartPointer<vtkDoubleArray> colUnfilteredTimestamp = vtkSmartPointer<vtkDoubleArray>::New(); 
    colUnfilteredTimestamp->SetName(colUnfilteredTimestampName); 
    this->TimeStampReportTable->AddColumn(colUnfilteredTimestamp); 

    const char* colFilteredTimestampName = "FilteredTimestamp"; 
    vtkSmartPointer<vtkDoubleArray> colFilteredTimestamp = vtkSmartPointer<vtkDoubleArray>::New(); 
    colFilteredTimestamp->SetName(colFilteredTimestampName); 
    this->TimeStampReportTable->AddColumn(colFilteredTimestamp); 
  }

  // create a new row for the timestamp report table 
  vtkSmartPointer<vtkVariantArray> timeStampReportTableRow = vtkSmartPointer<vtkVariantArray>::New();  
  
  timeStampReportTableRow->InsertNextValue(itemIndex); 
  timeStampReportTableRow->InsertNextValue(unfilteredTimestamp - this->StartTime); 
  timeStampReportTableRow->InsertNextValue(filteredTimestamp - this->StartTime); 

  this->TimeStampReportTable->InsertNextRow(timeStampReportTableRow);

}
