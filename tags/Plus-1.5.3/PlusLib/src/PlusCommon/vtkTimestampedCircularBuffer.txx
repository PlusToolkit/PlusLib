
template<class BufferItemType>
vtkTimestampedCircularBuffer<BufferItemType>* vtkTimestampedCircularBuffer<BufferItemType>::New()
{
  vtkObject* ret = vtkObjectFactory::CreateInstance(typeid(vtkTimestampedCircularBuffer<BufferItemType>).name());
  if(ret)
    {
    return static_cast<vtkTimestampedCircularBuffer<BufferItemType>*>(ret);
    }
  return new vtkTimestampedCircularBuffer<BufferItemType>();
}


//----------------------------------------------------------------------------
template<class BufferItemType>
vtkTimestampedCircularBuffer<BufferItemType>::vtkTimestampedCircularBuffer()
{
	this->BufferItemContainer.resize(0); 
	this->Mutex = vtkCriticalSection::New();
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

  this->StartTime = 0; 

}

//----------------------------------------------------------------------------
template<class BufferItemType>
vtkTimestampedCircularBuffer<BufferItemType>::~vtkTimestampedCircularBuffer()
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
template<class BufferItemType>
void vtkTimestampedCircularBuffer<BufferItemType>::PrintSelf(ostream& os, vtkIndent indent)
{
	this->Superclass::PrintSelf(os,indent);

	os << indent << "BufferSize: " << this->GetBufferSize() << "\n";
	os << indent << "NumberOfItems: " << this->NumberOfItems << "\n";
	os << indent << "CurrentTimeStamp: " << this->CurrentTimeStamp << "\n";
	os << indent << "Local time offset: " << this->LocalTimeOffsetSec << "\n";
	os << indent << "Latest Item Uid: " << this->LatestItemUid << "\n";

}

//----------------------------------------------------------------------------
template<class BufferItemType>
void vtkTimestampedCircularBuffer<BufferItemType>::Lock()
{   
  //PRTL_CRITICAL_SECTION critSec=PRTL_CRITICAL_SECTION(((char*)&(this->Mutex))+0xEC);
	this->Mutex->Lock();
}

//----------------------------------------------------------------------------
template<class BufferItemType>
void vtkTimestampedCircularBuffer<BufferItemType>::Unlock()
{
  //PRTL_CRITICAL_SECTION critSec=PRTL_CRITICAL_SECTION(((char*)&(this->Mutex))+0xEC);
	this->Mutex->Unlock();
}

//----------------------------------------------------------------------------
template<class BufferItemType>
PlusStatus vtkTimestampedCircularBuffer<BufferItemType>::PrepareForNewItem(const double timestamp, BufferItemUidType& newFrameUid, int& bufferIndex)
{
	if ( timestamp <= this->CurrentTimeStamp )
	{
		LOG_DEBUG("Need to skip newly added frame - new timestamp ("<< std::fixed << timestamp << ") is not newer than the last one (" << this->CurrentTimeStamp << ")!"); 
		return PLUS_FAIL; 
	}

	// Increase frame unique ID
	this->Lock();
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
	this->Unlock(); 

	return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
template<class BufferItemType>
int vtkTimestampedCircularBuffer<BufferItemType>::GetBufferSize()
{
	return this->BufferItemContainer.size(); 
}
//----------------------------------------------------------------------------
// Sets the buffer size, and copies the maximum number of the most current old
// frames and timestamps
template<class BufferItemType>
PlusStatus vtkTimestampedCircularBuffer<BufferItemType>::SetBufferSize(int bufsize)
{
	if (bufsize < 0)
	{
		LOG_ERROR("SetBufferSize: invalid buffer size");
		return PLUS_FAIL;
	}

	if (bufsize == this->GetBufferSize() && bufsize != 0)
	{
		return PLUS_SUCCESS;
	}

	this->Lock(); 

	if ( this->GetBufferSize() == 0 )
	{
		for ( int i = 0; i < bufsize; i++ )
		{
			BufferItemType emptyBufferItem; 
			this->BufferItemContainer.push_back(emptyBufferItem); 
		}
		this->WritePointer = 0;
		this->NumberOfItems = 0;
		this->CurrentTimeStamp = 0.0;
	}
	// if the new buffer is bigger than the old buffer
	else if ( this->GetBufferSize() < bufsize )
	{
		std::deque<BufferItemType>::iterator it = this->BufferItemContainer.begin() + this->WritePointer; 
		const int numberOfNewBufferObjects = bufsize - this->GetBufferSize(); 
		for ( int i = 0; i < numberOfNewBufferObjects; ++i )
		{
			BufferItemType emptyBufferItem;
			it = this->BufferItemContainer.insert(it, emptyBufferItem); 
		}
	}
	// if the new buffer is smaller than the old buffer
	else if ( this->GetBufferSize() > bufsize )
	{
		// delete the oldest buffer objects 
		for (int i = 0; i < this->GetBufferSize() - bufsize; ++i)
		{
			std::deque<BufferItemType>::iterator it = this->BufferItemContainer.begin() + this->WritePointer; 
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
	
	this->Unlock(); 

	this->Modified();
	
	return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
template<class BufferItemType>
ItemStatus vtkTimestampedCircularBuffer<BufferItemType>::GetFrameStatus(const BufferItemUidType uid )
{
	if ( uid < this->GetOldestItemUidInBuffer() ) 
	{
		return ITEM_NOT_AVAILABLE_ANYMORE; 
	}
	else if ( uid > this->GetLatestItemUidInBuffer() )
	{
		return ITEM_NOT_AVAILABLE_YET; 
	}
	else
	{
		return ITEM_OK; 
	}
}

//----------------------------------------------------------------------------
template<class BufferItemType>
ItemStatus vtkTimestampedCircularBuffer<BufferItemType>::GetItemUidFromBufferIndex( const int bufferIndex, BufferItemUidType &uid )
{
	if (this->GetBufferSize() <= 0 
		|| bufferIndex >= this->GetBufferSize() 
		|| bufferIndex < 0 )
	{
		LOG_ERROR("Unable to get item UID from buffer index! Buffer index is not valid (bufferIndex=" 
			<< bufferIndex << ", bufferSize=" << this->GetBufferSize() << ")." ); 
		uid = 0; 
	}
	else
	{
		this->Lock(); 
		uid = this->BufferItemContainer[bufferIndex].GetUid(); 
		this->Unlock(); 
	}

	return this->GetFrameStatus(uid); 
}

//----------------------------------------------------------------------------
template<class BufferItemType>
ItemStatus vtkTimestampedCircularBuffer<BufferItemType>::GetBufferIndex( const BufferItemUidType uid, int& bufferIndex )
{
	bufferIndex = -1; 
	
	// check the status before we get the buffer index 
	ItemStatus currentStatus = this->GetFrameStatus( uid ); 
	if ( currentStatus != ITEM_OK )
	{
		LOG_WARNING("Buffer item is not in the buffer (Uid: " << uid << ")!"); 
		return currentStatus; 
	}
	
	bufferIndex = (this->WritePointer - 1) - (this->GetLatestItemUidInBuffer() - uid); 
	
	if ( bufferIndex < 0 )
	{
		bufferIndex += this->GetBufferSize(); 
	}
	
	// return with the current status of the frame 
	return this->GetFrameStatus( uid ); 
}

//----------------------------------------------------------------------------
template<class BufferItemType>
BufferItemType* vtkTimestampedCircularBuffer<BufferItemType>::GetBufferItemFromUid(const BufferItemUidType uid) 
{ 
	int bufferIndex(0); 
	ItemStatus status = this->GetBufferIndex(uid, bufferIndex); 
	if ( status != ITEM_OK )
	{
		LOG_WARNING("Buffer item is not in the buffer (Uid: " << uid << ")!"); 
		return NULL; 
	}
	
	return this->GetBufferItemFromBufferIndex(bufferIndex); 
}

//----------------------------------------------------------------------------
template<class BufferItemType>
BufferItemType* vtkTimestampedCircularBuffer<BufferItemType>::GetBufferItemFromBufferIndex(const int bufferIndex) 
{ 
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
template<class BufferItemType>
ItemStatus vtkTimestampedCircularBuffer<BufferItemType>::GetFilteredTimeStamp(const BufferItemUidType uid, double &filteredTimestamp)
{
  if (this->NumberOfItems == 0)
  {
    filteredTimestamp = 0.0; 
		return ITEM_NOT_AVAILABLE_YET;
  }

	ItemStatus status = this->GetFrameStatus( uid ); 
	if ( status != ITEM_OK )
	{
		filteredTimestamp = 0.0; 
		return status; 
	}

	int bufferIndex(0); 
	status = this->GetBufferIndex( uid, bufferIndex ); 
	
	if ( status != ITEM_OK )
	{
		LOG_WARNING("Buffer item is not in the buffer (Uid: " << uid << ")!"); 
		return status; 
	}
	
	filteredTimestamp = this->BufferItemContainer[bufferIndex].GetFilteredTimestamp(this->LocalTimeOffsetSec); 
	
	// Check the status again to make sure the writer didn't change it
	return this->GetFrameStatus( uid );
}

//----------------------------------------------------------------------------
template<class BufferItemType>
ItemStatus vtkTimestampedCircularBuffer<BufferItemType>::GetUnfilteredTimeStamp(const BufferItemUidType uid, double &unfilteredTimestamp)
{ 
	ItemStatus status = this->GetFrameStatus( uid ); 
	if ( status != ITEM_OK )
	{
		unfilteredTimestamp = 0.0; 
		return status; 
	}

	int bufferIndex(0); 
	status = this->GetBufferIndex( uid, bufferIndex ); 
	
	if ( status != ITEM_OK )
	{
		LOG_WARNING("Buffer item is not in the buffer (Uid: " << uid << ")!"); 
		return status; 
	}
	
	unfilteredTimestamp = this->BufferItemContainer[bufferIndex].GetUnfilteredTimestamp(this->LocalTimeOffsetSec); 
	
	// Check the status again to make sure the writer didn't change it
	return this->GetFrameStatus( uid );
}


//----------------------------------------------------------------------------
template<class BufferItemType>
ItemStatus vtkTimestampedCircularBuffer<BufferItemType>::GetIndex(const BufferItemUidType uid, unsigned long &index)
{ 
	ItemStatus status = this->GetFrameStatus( uid ); 
	if ( status != ITEM_OK )
	{
		index = 0; 
		return status; 
	}

	int bufferIndex(0); 
	status = this->GetBufferIndex( uid, bufferIndex ); 
	
	if ( status != ITEM_OK )
	{
		LOG_WARNING("Buffer item is not in the buffer (Uid: " << uid << ")!"); 
		return status; 
	}
	
	index = this->BufferItemContainer[bufferIndex].GetIndex(); 
	
	// Check the status again to make sure the writer didn't change it
	return this->GetFrameStatus( uid );
}


//----------------------------------------------------------------------------
template<class BufferItemType>
ItemStatus vtkTimestampedCircularBuffer<BufferItemType>::GetBufferIndexFromTime(const double time, int& bufferIndex )
{
	BufferItemUidType itemUid(0); 
	ItemStatus itemStatus = this->GetItemUidFromTime(time, itemUid); 

	if ( itemStatus != ITEM_OK )
	{
		LOG_WARNING("Buffer item is not in the buffer (time: " << std::fixed << time << ")!"); 
		return itemStatus; 
	}
	
	return this->GetBufferIndex( itemUid, bufferIndex ); 
}


//----------------------------------------------------------------------------
// do a simple divide-and-conquer search for the transform
// that best matches the given timestamp
template<class BufferItemType>
ItemStatus vtkTimestampedCircularBuffer<BufferItemType>::GetItemUidFromTime(const double time, BufferItemUidType& uid )
{
  PlusLockGuard< vtkTimestampedCircularBuffer<BufferItemType> > bufferGuardedLock(this);

	BufferItemUidType lo = this->GetOldestItemUidInBuffer();
	BufferItemUidType hi = this->GetLatestItemUidInBuffer();

	double tlo(0); 
	// minimum time
	ItemStatus loStatus = this->GetTimeStamp(lo, tlo); 
	if ( loStatus != ITEM_OK )
	{
		LOG_WARNING("Unable to get lo timestamp for frame UID: " << lo ); 
		return loStatus; 
	}

	double thi(0); 
	// maximum time
	ItemStatus hiStatus = this->GetTimeStamp(hi, thi); 
	if ( hiStatus != ITEM_OK )
	{
		LOG_WARNING("Unable to get hi timestamp for frame UID: " << hi ); 
		return hiStatus; 
	} 

	if (time < tlo)
	{
		return ITEM_NOT_AVAILABLE_ANYMORE; 
	}
	else if (time > thi)
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
		double tmid(0);
		ItemStatus midStatus = this->GetTimeStamp(mid, tmid); 
		if ( midStatus != ITEM_OK )
		{
			LOG_WARNING("Unable to get mid timestamp for frame UID: " << tmid ); 
			return midStatus; 
		}

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
template<class BufferItemType>
void vtkTimestampedCircularBuffer<BufferItemType>::DeepCopy(vtkTimestampedCircularBuffer<BufferItemType>* buffer)
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
template<class BufferItemType>
void vtkTimestampedCircularBuffer<BufferItemType>::Clear()
{
	this->Lock(); 
	this->WritePointer = 0; 
	this->NumberOfItems = 0; 
	this->CurrentTimeStamp = 0; 
  this->LatestItemUid = 0;
	this->Unlock();
}

//----------------------------------------------------------------------------
template<class BufferItemType>
double vtkTimestampedCircularBuffer<BufferItemType>::GetFrameRate(bool ideal /*=false*/)
{
	// TODO: Start the frame rate computation from the latest frame UID with using a few seconds of items in the buffer
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
		if ( this->GetIndex(frame - 1, framenum) != ITEM_OK)
		{
			continue; 
		}

		int frameDiff = framenum - prevframenum; 
		double frameperiod = (time - prevtime); 
		if ( ideal )
		{
			frameperiod /= (1.0 * frameDiff);
		}

		if ( frameperiod > 0 )
		{
			framePeriods.push_back(frameperiod); 
		}
	}

	const int numberOfFramePeriods =  framePeriods.size(); 
	double samplingPeriod(0); 
	for ( int i = 0; i < numberOfFramePeriods; i++ )
	{
		samplingPeriod += framePeriods[i] / (1.0 * numberOfFramePeriods); 
	}

	double frameRate(0); 
	if ( samplingPeriod != 0 )
	{
		frameRate = 1.0/samplingPeriod;
	}

	return frameRate; 
}

//----------------------------------------------------------------------------
// for accurate timing of the frame: an exponential moving average
// is computed to smooth out the jitter in the times that are returned by the system clock:
// EstimatedFramePeriod[t] = EstimatedFramePeriod[t-1] * (1-SmoothingFactor) + FramePeriod[t] * SmoothingFactor
// Smaller SmoothingFactor results leads to less jitter.
template<class BufferItemType>
PlusStatus vtkTimestampedCircularBuffer<BufferItemType>::CreateFilteredTimeStampForItem(unsigned long itemIndex, double inUnfilteredTimestamp, double &outFilteredTimestamp, bool &filteredTimestampProbablyValid)
{
  this->Lock();   
  filteredTimestampProbablyValid=true;

  if ( this->TimeStampReportTable == NULL )
  {
    this->InitTimeStampReportTable(); 
  }

  // create a new row for the timestamp report table 
  vtkSmartPointer<vtkVariantArray> timeStampReportTableRow = vtkSmartPointer<vtkVariantArray>::New(); 

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
    // save the current results into the timestamp report table
    timeStampReportTableRow->InsertNextValue(itemIndex); 
    //timeStampReportTableRow->InsertNextValue(frameCountDiff); 
    std::ostringstream strUnfilteredTimestamp; 
    strUnfilteredTimestamp << std::fixed << inUnfilteredTimestamp - this->StartTime; 
    timeStampReportTableRow->InsertNextValue(vtkVariant(strUnfilteredTimestamp.str())); 

    std::ostringstream strFilteredTimestamp; 
    strFilteredTimestamp << std::fixed << outFilteredTimestamp - this->StartTime; 
    timeStampReportTableRow->InsertNextValue(vtkVariant(strFilteredTimestamp.str())); 
    //timeStampReportTableRow->InsertNextValue(diffUnfilteredTimestamp); 
    //timeStampReportTableRow->InsertNextValue(frameperiod); 
    //timeStampReportTableRow->InsertNextValue(this->EstimatedFramePeriod); 
    this->TimeStampReportTable->InsertNextRow(timeStampReportTableRow); 
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

  LOG_TRACE("timestamps = [" << std::fixed << this->FilterContainerTimestampVector << "];");
  LOG_TRACE("frameindexes = [" << std::fixed << this->FilterContainerIndexVector << "];");

  // save the current results into the timestamp report table
  timeStampReportTableRow->InsertNextValue(itemIndex); 
  //timeStampReportTableRow->InsertNextValue(frameCountDiff); 
  std::ostringstream strUnfilteredTimestamp; 
  strUnfilteredTimestamp << std::fixed << inUnfilteredTimestamp - this->StartTime; 
  timeStampReportTableRow->InsertNextValue(vtkVariant(strUnfilteredTimestamp.str())); 

  std::ostringstream strFilteredTimestamp; 
  strFilteredTimestamp << std::fixed << outFilteredTimestamp - this->StartTime; 
  timeStampReportTableRow->InsertNextValue(vtkVariant(strFilteredTimestamp.str())); 
  //timeStampReportTableRow->InsertNextValue(diffUnfilteredTimestamp); 
  //timeStampReportTableRow->InsertNextValue(frameperiod); 
  //timeStampReportTableRow->InsertNextValue(this->EstimatedFramePeriod); 
  this->TimeStampReportTable->InsertNextRow(timeStampReportTableRow); 

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
template<class BufferItemType>
void vtkTimestampedCircularBuffer<BufferItemType>::InitTimeStampReportTable()
{
  if ( this->TimeStampReportTable )
  {
    // Delete table just to make sure the col's are well prepared
    this->TimeStampReportTable->Delete(); 
  }

  this->TimeStampReportTable = vtkTable::New(); 

  const char* colFrameNumberName = "FrameNumber"; 
  vtkSmartPointer<vtkDoubleArray> colFrameNumber = vtkSmartPointer<vtkDoubleArray>::New(); 
  colFrameNumber->SetName(colFrameNumberName); 
  this->TimeStampReportTable->AddColumn(colFrameNumber); 

  /*const char* colFrameNumberDifferenceName = "FrameNumberDifference"; 
  vtkSmartPointer<vtkDoubleArray> colFrameNumberDifference = vtkSmartPointer<vtkDoubleArray>::New(); 
  colFrameNumberDifference->SetName(colFrameNumberDifferenceName); 
  this->TimeStampReportTable->AddColumn(colFrameNumberDifference); */

  const char* colUnfilteredTimestampName = "UnfilteredTimestamp"; 
  vtkSmartPointer<vtkStringArray> colUnfilteredTimestamp = vtkSmartPointer<vtkStringArray>::New(); 
  colUnfilteredTimestamp->SetName(colUnfilteredTimestampName); 
  this->TimeStampReportTable->AddColumn(colUnfilteredTimestamp); 

  const char* colFilteredTimestampName = "FilteredTimestamp"; 
  vtkSmartPointer<vtkStringArray> colFilteredTimestamp = vtkSmartPointer<vtkStringArray>::New(); 
  colFilteredTimestamp->SetName(colFilteredTimestampName); 
  this->TimeStampReportTable->AddColumn(colFilteredTimestamp); 

  /*const char* colUnfilteredTimeDifferenceName = "UnfilteredTimeDifference"; 
  vtkSmartPointer<vtkDoubleArray> colUnfilteredTimeDifference = vtkSmartPointer<vtkDoubleArray>::New(); 
  colUnfilteredTimeDifference->SetName(colUnfilteredTimeDifferenceName); 
  this->TimeStampReportTable->AddColumn(colUnfilteredTimeDifference); */

  /*const char* colSamplingPeriodName = "SamplingPeriod"; 
  vtkSmartPointer<vtkDoubleArray> colSamplingPeriod = vtkSmartPointer<vtkDoubleArray>::New(); 
  colSamplingPeriod->SetName(colSamplingPeriodName); 
  this->TimeStampReportTable->AddColumn(colSamplingPeriod); */

  /*const char* colEstimatedFramePeriodName = "EstimatedFramePeriod"; 
  vtkSmartPointer<vtkDoubleArray> colEstimatedFramePeriod = vtkSmartPointer<vtkDoubleArray>::New(); 
  colEstimatedFramePeriod->SetName(colEstimatedFramePeriodName); 
  this->TimeStampReportTable->AddColumn(colEstimatedFramePeriod); */

}
//----------------------------------------------------------------------------
template<class BufferItemType>
PlusStatus vtkTimestampedCircularBuffer<BufferItemType>::GetTimeStampReportTable(vtkTable* timeStampReportTable)
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
