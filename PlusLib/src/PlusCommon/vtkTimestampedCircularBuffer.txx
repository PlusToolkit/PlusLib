
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
	this->LocalTimeOffset = 0.0; 
	this->LatestItemUid = 0; 
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
}

//----------------------------------------------------------------------------
template<class BufferItemType>
void vtkTimestampedCircularBuffer<BufferItemType>::PrintSelf(ostream& os, vtkIndent indent)
{
	this->Superclass::PrintSelf(os,indent);

	os << indent << "BufferSize: " << this->GetBufferSize() << "\n";
	os << indent << "NumberOfItems: " << this->NumberOfItems << "\n";
	os << indent << "CurrentTimeStamp: " << this->CurrentTimeStamp << "\n";
	os << indent << "Local time offset: " << this->LocalTimeOffset << "\n";
	os << indent << "Latest Item Uid: " << this->LatestItemUid << "\n";

}

//----------------------------------------------------------------------------
template<class BufferItemType>
void vtkTimestampedCircularBuffer<BufferItemType>::Lock()
{
	this->Mutex->Lock();
}

//----------------------------------------------------------------------------
template<class BufferItemType>
void vtkTimestampedCircularBuffer<BufferItemType>::Unlock()
{
	this->Mutex->Unlock();
}

//----------------------------------------------------------------------------
template<class BufferItemType>
PlusStatus vtkTimestampedCircularBuffer<BufferItemType>::PrepareForNewFrame(const double timestamp, BufferItemUidType& newFrameUid, int& bufferIndex)
{
	if ( timestamp <= this->CurrentTimeStamp )
	{
		LOG_DEBUG("Need to skip newly added frame - new timestamp ("<< std::fixed << timestamp << ") is older than the last one (" << this->CurrentTimeStamp << ")!"); 
		return PLUS_FAIL; 

	}

	// Increase frame unique ID
	//this->Lock();
	newFrameUid = this->LatestItemUid++; 
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
	//this->Unlock(); 

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
int vtkTimestampedCircularBuffer<BufferItemType>::GetBufferIndex( const BufferItemUidType uid )
{
	if ( this->GetFrameStatus( uid ) != ITEM_OK )
	{
		return -1; 
	}
	
	int readIndex = (this->WritePointer - 1) % this->GetBufferSize();
	if ( readIndex < 0 ) 
	{
		readIndex += this->GetBufferSize();
	}
	
	int bufferIndex = readIndex - (this->LatestItemUid - uid); 
	
	if ( bufferIndex < 0 )
	{
		bufferIndex += this->GetBufferSize(); 
	}
	
	return bufferIndex; 
}

//----------------------------------------------------------------------------
template<class BufferItemType>
BufferItemType* vtkTimestampedCircularBuffer<BufferItemType>::GetBufferItem(const BufferItemUidType uid) 
{ 
	const int bufferIndex = this->GetBufferIndex(uid); 
	return this->GetBufferItem(bufferIndex); 
}

//----------------------------------------------------------------------------
template<class BufferItemType>
BufferItemType* vtkTimestampedCircularBuffer<BufferItemType>::GetBufferItem(const int bufferIndex) 
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
	ItemStatus status = this->GetFrameStatus( uid ); 
	if ( status != ITEM_OK )
	{
		filteredTimestamp = 0.0; 
		return status; 
	}

	const int bufferIndex = this->GetBufferIndex( uid ); 
	filteredTimestamp = this->BufferItemContainer[bufferIndex].GetFilteredTimestamp(this->LocalTimeOffset); 
	return status;
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

	const int bufferIndex = this->GetBufferIndex( uid ); 
	unfilteredTimestamp = this->BufferItemContainer[bufferIndex].GetUnfilteredTimestamp(this->LocalTimeOffset); 
	return status;
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

	const int bufferIndex = this->GetBufferIndex( uid ); 
	index = this->BufferItemContainer[bufferIndex].GetIndex(); 
	return status;
}


//----------------------------------------------------------------------------
template<class BufferItemType>
ItemStatus vtkTimestampedCircularBuffer<BufferItemType>::GetBufferIndexFromTime(const double time, int& bufferIndex )
{
	BufferItemUidType itemUid(0); 
	ItemStatus itemStatus = this->GetItemUidFromTime(time, itemUid); 

	if ( itemStatus != ITEM_OK )
	{
		return itemStatus; 
	}
	
	bufferIndex = this->GetBufferIndex(itemUid); 
	return itemStatus; 
}


//----------------------------------------------------------------------------
// do a simple divide-and-conquer search for the transform
// that best matches the given timestamp
template<class BufferItemType>
ItemStatus vtkTimestampedCircularBuffer<BufferItemType>::GetItemUidFromTime(const double time, BufferItemUidType& uid )
{
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
		if (hi-lo == 1)
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
	this->LocalTimeOffset = buffer->LocalTimeOffset;
	this->LatestItemUid = buffer->LatestItemUid; 

	this->BufferItemContainer = buffer->BufferItemContainer; 
	this->Unlock(); 
	buffer->Unlock(); 
}

//----------------------------------------------------------------------------
template<class BufferItemType>
void vtkTimestampedCircularBuffer<BufferItemType>::Clear()
{
	this->WritePointer = 0; 
	this->NumberOfItems = 0; 
	this->CurrentTimeStamp = 0; 
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


