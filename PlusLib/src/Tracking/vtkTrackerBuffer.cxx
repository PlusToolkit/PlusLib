#include "PlusConfigure.h"

#include "vtkTrackerBuffer.h"
#include "vtkMath.h"
#include "vtkMatrix4x4.h"
#include "vtkDoubleArray.h"
#include "vtkIntArray.h"
#include "vtkUnsignedLongLongArray.h"
#include "vtkCriticalSection.h"
#include "vtkObjectFactory.h"


///----------------------------------------------------------------------------
//						TrackerBufferItem
//----------------------------------------------------------------------------
TrackerBufferItem::TrackerBufferItem()
{
  this->Matrix = vtkSmartPointer<vtkMatrix4x4>::New(); 
  this->Status = TR_OK; 
}

//----------------------------------------------------------------------------
TrackerBufferItem::~TrackerBufferItem()
{
}

//----------------------------------------------------------------------------
TrackerBufferItem::TrackerBufferItem(const TrackerBufferItem &trackerBufferItem)
{
  this->Matrix = vtkSmartPointer<vtkMatrix4x4>::New(); 
  this->Status = TR_OK; 
  *this = trackerBufferItem; 
}

//----------------------------------------------------------------------------
TrackerBufferItem& TrackerBufferItem::operator=(TrackerBufferItem const& trackerBufferItem)
{
  // Handle self-assignment
  if (this == &trackerBufferItem)
  {
    return *this;
  }

  this->Status = trackerBufferItem.Status; 
  this->Matrix->DeepCopy( trackerBufferItem.Matrix ); 

  this->FilteredTimeStamp = trackerBufferItem.FilteredTimeStamp; 
  this->UnfilteredTimeStamp = trackerBufferItem.UnfilteredTimeStamp; 
  this->Index = trackerBufferItem.Index; 
  this->Uid = trackerBufferItem.Uid; 

  return *this;
}

//----------------------------------------------------------------------------
PlusStatus TrackerBufferItem::DeepCopy(TrackerBufferItem* trackerBufferItem)
{
  if ( trackerBufferItem == NULL )
  {
    LOG_ERROR("Failed to deep copy tracker buffer item - buffer item NULL!"); 
    return PLUS_FAIL; 
  }

  this->Status = trackerBufferItem->Status; 
  this->Matrix->DeepCopy( trackerBufferItem->Matrix ); 
  this->FilteredTimeStamp = trackerBufferItem->FilteredTimeStamp; 
  this->UnfilteredTimeStamp = trackerBufferItem->UnfilteredTimeStamp; 
  this->Index = trackerBufferItem->Index; 
  this->Uid = trackerBufferItem->Uid; 

  return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
PlusStatus TrackerBufferItem::SetMatrix(vtkMatrix4x4* matrix)
{
  if ( matrix == NULL ) 
  {
    LOG_ERROR("Failed to set matrix - input matrix is NULL!"); 
    return PLUS_FAIL; 
  }

  this->Matrix->DeepCopy(matrix); 

  return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
PlusStatus TrackerBufferItem::GetMatrix(vtkMatrix4x4* outputMatrix)
{
  if ( outputMatrix == NULL ) 
  {
    LOG_ERROR("Failed to copy matrix - output matrix is NULL!"); 
    return PLUS_FAIL; 
  }

  outputMatrix->DeepCopy(this->Matrix);

  return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
//						vtkTrackerBuffer
//----------------------------------------------------------------------------
vtkTrackerBuffer* vtkTrackerBuffer::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkTrackerBuffer");
  if(ret)
  {
    return (vtkTrackerBuffer*)ret;
  }
  // If the factory was unable to create the object, then create it here.
  return new vtkTrackerBuffer;
}

//----------------------------------------------------------------------------
vtkTrackerBuffer::vtkTrackerBuffer()
{
  this->TrackerBuffer = vtkTimestampedCircularBuffer<TrackerBufferItem>::New(); 
  this->ToolCalibrationMatrix = NULL;
  this->WorldCalibrationMatrix = NULL;

  this->SetBufferSize(500); 
}

//----------------------------------------------------------------------------
void vtkTrackerBuffer::DeepCopy(vtkTrackerBuffer *buffer)
{
  LOG_TRACE("vtkTrackerBuffer::DeepCopy");
  this->SetBufferSize(buffer->GetBufferSize());
  this->TrackerBuffer->DeepCopy( buffer->TrackerBuffer ); 
  this->SetToolCalibrationMatrix( buffer->GetToolCalibrationMatrix() ); 
  this->SetWorldCalibrationMatrix(buffer->GetWorldCalibrationMatrix());
}

//----------------------------------------------------------------------------
vtkTrackerBuffer::~vtkTrackerBuffer()
{
  this->SetToolCalibrationMatrix(NULL); 
  this->SetWorldCalibrationMatrix(NULL); 

  if ( this->TrackerBuffer != NULL )
  {
    this->TrackerBuffer->Delete(); 
    this->TrackerBuffer = NULL; 
  }
}

//----------------------------------------------------------------------------
void vtkTrackerBuffer::PrintSelf(ostream& os, vtkIndent indent)
{
  vtkObject::PrintSelf(os,indent);

  os << indent << "TrackerBuffer: " << this->TrackerBuffer << "\n";
  if ( this->TrackerBuffer )
  {
    this->TrackerBuffer->PrintSelf(os,indent.GetNextIndent());
  }

  os << indent << "ToolCalibrationMatrix: " << this->ToolCalibrationMatrix << "\n";
  if (this->ToolCalibrationMatrix)
  {
    this->ToolCalibrationMatrix->PrintSelf(os,indent.GetNextIndent());
  }  
  os << indent << "WorldCalibrationMatrix: " << this->WorldCalibrationMatrix << "\n";
  if (this->WorldCalibrationMatrix)
  {
    this->WorldCalibrationMatrix->PrintSelf(os,indent.GetNextIndent());
  }

}

//----------------------------------------------------------------------------
int vtkTrackerBuffer::GetBufferSize()
{
  return this->TrackerBuffer->GetBufferSize(); 
}

//----------------------------------------------------------------------------
PlusStatus vtkTrackerBuffer::SetBufferSize(int bufsize)
{
  return this->TrackerBuffer->SetBufferSize(bufsize); 
}

//----------------------------------------------------------------------------
void vtkTrackerBuffer::SetNumberOfAveragedItems(double numberOfAveragedItems)
{
  this->TrackerBuffer->SetNumberOfAveragedItems(numberOfAveragedItems); 
}

//----------------------------------------------------------------------------
void vtkTrackerBuffer::SetStartTime( double startTime)
{
  this->TrackerBuffer->SetStartTime(startTime); 
}

//----------------------------------------------------------------------------
double vtkTrackerBuffer::GetStartTime()
{
  return this->TrackerBuffer->GetStartTime(); 
}

//----------------------------------------------------------------------------
PlusStatus vtkTrackerBuffer::GetTimeStampReportTable(vtkTable* timeStampReportTable) 
{
  return this->TrackerBuffer->GetTimeStampReportTable(timeStampReportTable); 
}

//----------------------------------------------------------------------------
PlusStatus vtkTrackerBuffer::AddTimeStampedItem(vtkMatrix4x4 *matrix, TrackerStatus status, unsigned long frameNumber, double unfilteredTimestamp)
{
  double filteredTimestamp(0); 
  if ( this->TrackerBuffer->CreateFilteredTimeStampForItem(frameNumber, unfilteredTimestamp, filteredTimestamp) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to create filtered timestamp for buffer item with item index: " << frameNumber ); 
    return PLUS_FAIL; 
  }
  return this->AddTimeStampedItem(matrix, status, frameNumber, unfilteredTimestamp, filteredTimestamp); 

}

//----------------------------------------------------------------------------
PlusStatus vtkTrackerBuffer::AddTimeStampedItem(vtkMatrix4x4 *matrix, TrackerStatus status, unsigned long frameNumber, double unfilteredTimestamp, double filteredTimestamp)
{

  if ( matrix  == NULL )
  {
    LOG_ERROR( "vtkTrackerBuffer: Unable to add NULL matrix to tracker buffer!"); 
    return PLUS_FAIL; 
  }

  int bufferIndex(0); 
  BufferItemUidType itemUid; 

  this->TrackerBuffer->Lock(); 
  if ( this->TrackerBuffer->PrepareForNewItem(filteredTimestamp, itemUid, bufferIndex) != PLUS_SUCCESS )
  {
    this->TrackerBuffer->Unlock(); 
    // Just a debug message, because we want to avoid unnecessary warning messages if the timestamp is the same as last one
    LOG_DEBUG( "vtkTrackerBuffer: Failed to prepare for adding new frame to tracker buffer!"); 
    return PLUS_FAIL; 
  }

  // get the pointer to the correct location in the tracker buffer, where this data needs to be copied
  TrackerBufferItem* newObjectInBuffer = this->TrackerBuffer->GetBufferItem(bufferIndex); 
  if ( newObjectInBuffer == NULL )
  {
    this->TrackerBuffer->Unlock(); 
    LOG_ERROR( "vtkTrackerBuffer: Failed to get pointer to tracker buffer object from the tracker buffer for the new frame!"); 
    return PLUS_FAIL; 
  }

  PlusStatus itemStatus = newObjectInBuffer->SetMatrix(matrix);
  newObjectInBuffer->SetStatus( status ); 
  newObjectInBuffer->SetFilteredTimestamp( filteredTimestamp ); 
  newObjectInBuffer->SetUnfilteredTimestamp( unfilteredTimestamp ); 
  newObjectInBuffer->SetIndex( frameNumber ); 
  newObjectInBuffer->SetUid( itemUid ); 
  this->TrackerBuffer->Unlock(); 

  return itemStatus; 
}

//----------------------------------------------------------------------------
ItemStatus vtkTrackerBuffer::GetLatestTimeStamp( double& latestTimestamp )
{
  return this->TrackerBuffer->GetLatestTimeStamp(latestTimestamp); 
}

//----------------------------------------------------------------------------
ItemStatus vtkTrackerBuffer::GetOldestTimeStamp( double& oldestTimestamp )
{
  return this->TrackerBuffer->GetOldestTimeStamp(oldestTimestamp); 
}

//----------------------------------------------------------------------------
ItemStatus vtkTrackerBuffer::GetTimeStamp( BufferItemUidType uid, double& timestamp)
{
  return this->TrackerBuffer->GetTimeStamp(uid, timestamp); 
}

//----------------------------------------------------------------------------
ItemStatus vtkTrackerBuffer::GetTrackerBufferItem(BufferItemUidType uid, TrackerBufferItem* bufferItem, bool calibratedItem /*= false*/)
{
  if ( bufferItem == NULL )
  {
    LOG_ERROR("Unable to copy tracker buffer item into a NULL tracker buffer item!"); 
    return ITEM_UNKNOWN_ERROR; 
  }

  ItemStatus status = this->TrackerBuffer->GetFrameStatus(uid); 
  if ( status != ITEM_OK )
  {
    if (  status == ITEM_NOT_AVAILABLE_ANYMORE )
    {
      LOG_WARNING("Failed to get tracker buffer item: tracker item not available anymore"); 
    }
    else if (  status == ITEM_NOT_AVAILABLE_YET )
    {
      LOG_WARNING("Failed to get tracker buffer item: tracker item not available yet"); 
    }
    else
    {
      LOG_WARNING("Failed to get tracker buffer item!"); 
    }
    return status; 
  }

  TrackerBufferItem* trackerItem = this->TrackerBuffer->GetBufferItem(uid); 

  if ( bufferItem->DeepCopy(trackerItem) != PLUS_SUCCESS )
  {
    LOG_WARNING("Failed to copy tracker item!"); 
    return ITEM_UNKNOWN_ERROR; 
  }

  // Apply Tool calibration and World calibration matrix to tool matrix if desired 
  if ( calibratedItem ) 
  {
    
    vtkSmartPointer<vtkMatrix4x4> toolMatrix=vtkSmartPointer<vtkMatrix4x4>::New();
    if (trackerItem->GetMatrix(toolMatrix)!=PLUS_SUCCESS)
    {
      LOG_ERROR("Failed to get toolMatrix"); 
      return ITEM_UNKNOWN_ERROR;
    }

    if (this->ToolCalibrationMatrix)
    {
      vtkMatrix4x4::Multiply4x4(toolMatrix, this->ToolCalibrationMatrix, toolMatrix);
    }

    if (this->WorldCalibrationMatrix)
    {
      vtkMatrix4x4::Multiply4x4(this->WorldCalibrationMatrix, toolMatrix, toolMatrix);
    }
    bufferItem->SetMatrix(toolMatrix); 
  }

  // Check the status again to make sure the writer didn't change it
  return this->TrackerBuffer->GetFrameStatus(uid); 
}

//----------------------------------------------------------------------------
// Interpolate the matrix for the given timestamp from the two nearest
// transforms in the buffer.
// The rotation is interpolated with SLERP interpolation, and the
// position is interpolated with linear interpolation.
// The flags are the logical 'or' of the two transformations that
// are used in the interpolation.
ItemStatus vtkTrackerBuffer::GetTrackerBufferItemFromTime( double time, TrackerBufferItem* bufferItem, bool calibratedItem /*= false*/)
{
  this->TrackerBuffer->Lock(); 
  BufferItemUidType uid_0(0), uid_1(0); 
  ItemStatus status = this->TrackerBuffer->GetItemUidFromTime(time, uid_0); 
  if ( status != ITEM_OK )
  {
    double lt(0); 
    this->TrackerBuffer->GetLatestTimeStamp(lt); 
    LOG_INFO("Latest timestamp: " << std::fixed << lt ); 
    LOG_WARNING("Failed to get tracker buffer item from time: " << std::fixed << time); 
    this->TrackerBuffer->Unlock();
    return status; 
  }

  uid_1 = uid_0; 

  double indexTime(0); 
  status = this->TrackerBuffer->GetTimeStamp(uid_0, indexTime); 
  if ( status != ITEM_OK )
  {
    LOG_WARNING("Failed to get tracker buffer timestamp with Uid: " << uid_0 ); 
    this->TrackerBuffer->Unlock();
    return status; 
  }

  double f = indexTime - time;

  // time difference should be 500 milliseconds or less
  if (f < -0.5 || f > 0.5)
  {
    LOG_WARNING("vtkTrackerBuffer: time difference is " << std::fixed << f << " ( " << indexTime << ", " << time);
    f = 0.0;    
  }
  // figure out what values to interpolate between, convert f into a value between 0 and 1
  else if (f > 0)
  {
    if ( uid_0 == this->TrackerBuffer->GetOldestItemUidInBuffer() )
    {
      uid_1 = uid_0; 
      f = 0.0;
    }
    else
    {
      uid_1 = uid_0 - 1;
    }
    double oldertime(0);  
    status = this->TrackerBuffer->GetTimeStamp(uid_1, oldertime); 
    if ( status != ITEM_OK )
    {
      LOG_WARNING("Failed to get tracker buffer timestamp with Uid: " << uid_1 ); 
      this->TrackerBuffer->Unlock();
      return status; 
    }
    f = f/(indexTime - oldertime);
  }
  else if (f < 0)
  {
    if (uid_0 == this->GetLatestItemUidInBuffer() )
    {
      uid_1 = uid_0;
      f = 0.0;
    }
    else
    {
      uid_1 = uid_0; 
      uid_0 = uid_0 + 1; 
      double newertime(0);  
      status = this->TrackerBuffer->GetTimeStamp(uid_0, newertime); 
      if ( status != ITEM_OK )
      {
        LOG_WARNING("Failed to get tracker buffer timestamp with Uid: " << uid_0 ); 
        this->TrackerBuffer->Unlock();
        return status; 
      }

      f = 1.0 + f/(newertime - indexTime);
    }
  }

  // We can unlock the buffer, we have all the UID that we need to use
  this->TrackerBuffer->Unlock();

  //============== item 0 ==================
  TrackerBufferItem item_0; 
  status = this->GetTrackerBufferItem(uid_0, &item_0, calibratedItem); 
  if ( status != ITEM_OK )
  {
    LOG_WARNING("Failed to get tracker buffer item with Uid: " << uid_0 ); 
    return status; 
  }

  vtkSmartPointer<vtkMatrix4x4> item_0Matrix=vtkSmartPointer<vtkMatrix4x4>::New();
  if (item_0.GetMatrix(item_0Matrix)!=PLUS_SUCCESS)
  {
    LOG_ERROR("Failed to get item_0"); 
    return ITEM_UNKNOWN_ERROR;
  }

  double matrix0[3][3] = {0};
  double xyz0[3] = {0};
  for (int i = 0; i < 3; i++)
  {
    matrix0[i][0] = item_0Matrix->GetElement(i,0);
    matrix0[i][1] = item_0Matrix->GetElement(i,1);
    matrix0[i][2] = item_0Matrix->GetElement(i,2);
    xyz0[i] = item_0Matrix->GetElement(i,3);
  }

  //============== item 1 ==================
  TrackerBufferItem item_1; 
  status = this->GetTrackerBufferItem(uid_1, &item_1, calibratedItem); 
  if ( status != ITEM_OK )
  {
    LOG_WARNING("Failed to get tracker buffer item with Uid: " << uid_1 ); 
    return status; 
  }

  vtkSmartPointer<vtkMatrix4x4> item_1Matrix=vtkSmartPointer<vtkMatrix4x4>::New();
  if (item_1.GetMatrix(item_1Matrix)!=PLUS_SUCCESS)
  {
    LOG_ERROR("Failed to get item_1"); 
    return ITEM_UNKNOWN_ERROR;
  }

  double matrix1[3][3] = {0};
  double xyz1[3] = {0};
  for (int i = 0; i < 3; i++)
  {
    matrix1[i][0] = item_1Matrix->GetElement(i,0);
    matrix1[i][1] = item_1Matrix->GetElement(i,1);
    matrix1[i][2] = item_1Matrix->GetElement(i,2);
    xyz1[i] = item_1Matrix->GetElement(i,3);
  }


  double quaternion[4]= {0};
  vtkMath::Transpose3x3(matrix0, matrix0);
  vtkMath::Multiply3x3(matrix1, matrix0, matrix1);
  vtkMath::Transpose3x3(matrix0, matrix0);
  vtkMath::Matrix3x3ToQuaternion(matrix1, quaternion);

  double s = sqrt(quaternion[1]*quaternion[1] +
    quaternion[2]*quaternion[2] +
    quaternion[3]*quaternion[3]);
  double angle = atan2(s, quaternion[0]) * f;
  quaternion[0] = cos(angle);

  if (s > 0.00001)
  {
    s = sin(angle)/s;
    quaternion[1] = quaternion[1]*s;
    quaternion[2] = quaternion[2]*s;
    quaternion[3] = quaternion[3]*s;
  }
  else
  { // use small-angle approximation for sin to avoid
    //  division by very small value
    quaternion[1] = quaternion[1]*f;
    quaternion[2] = quaternion[2]*f;
    quaternion[3] = quaternion[3]*f;
  }

  vtkMath::QuaternionToMatrix3x3(quaternion, matrix1);
  vtkMath::Multiply3x3(matrix1, matrix0, matrix1);

  vtkSmartPointer<vtkMatrix4x4> interpolatedMatrix = vtkSmartPointer<vtkMatrix4x4>::New(); 
  for (int i = 0; i < 3; i++)
  {
    interpolatedMatrix->Element[i][0] = matrix1[i][0];
    interpolatedMatrix->Element[i][1] = matrix1[i][1];
    interpolatedMatrix->Element[i][2] = matrix1[i][2];
    interpolatedMatrix->Element[i][3] = xyz0[i]*(1.0 - f) + xyz1[i]*f;
    //fprintf(stderr, "%f %f %f %f\n", xyz0[i], xyz1[i],  matrix->Element[i][3], f);
  } 

  if ( abs(item_0.GetTimestamp(0) - time) < abs(item_1.GetTimestamp(0) - time) )
  {
    bufferItem->DeepCopy(&item_0); 
  }
  else
  {
    bufferItem->DeepCopy(&item_1); 
  }

  bufferItem->SetMatrix(interpolatedMatrix); 

  // Interpolate the timestamps
  double filteredTimestamp = bufferItem->GetFilteredTimestamp(0); 
  double unfilteredTimestamp = bufferItem->GetUnfilteredTimestamp(0); 

  bufferItem->SetFilteredTimestamp(time); 
  bufferItem->SetUnfilteredTimestamp(time - (filteredTimestamp - unfilteredTimestamp) ); 

  // If the status is not OK, use that status
  if ( item_0.GetStatus() != TR_OK )
  {
    bufferItem->SetStatus(item_0.GetStatus()); 
  }

  if ( item_1.GetStatus() != TR_OK )
  {
    bufferItem->SetStatus(item_1.GetStatus()); 
  }

  return ITEM_OK; 
}

//----------------------------------------------------------------------------
void vtkTrackerBuffer::Clear()
{
  this->TrackerBuffer->Clear();  
}

//----------------------------------------------------------------------------
void vtkTrackerBuffer::SetLocalTimeOffset(double offset)
{
  this->TrackerBuffer->SetLocalTimeOffset(offset); 
}

//----------------------------------------------------------------------------
double vtkTrackerBuffer::GetLocalTimeOffset()
{
  return this->TrackerBuffer->GetLocalTimeOffset(); 
}
