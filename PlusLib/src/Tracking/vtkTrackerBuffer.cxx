#include "PlusConfigure.h"
#include "PlusMath.h"
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
  this->SetMaxAllowedTimeDifference(0.5); 
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
void vtkTrackerBuffer::SetAveragedItemsForFiltering(int averagedItemsForFiltering)
{
  this->TrackerBuffer->SetAveragedItemsForFiltering(averagedItemsForFiltering); 
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
  TrackerBufferItem* newObjectInBuffer = this->TrackerBuffer->GetBufferItemFromBufferIndex(bufferIndex); 
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

  TrackerBufferItem* trackerItem = this->TrackerBuffer->GetBufferItemFromUid(uid); 

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
ItemStatus vtkTrackerBuffer::GetNextValidItemUid(BufferItemUidType startUid, FindDirection dir, BufferItemUidType& foundUid )
{ 
  foundUid=startUid; // by default return the start uid (if cannot find a better uid)
  int uidIncrement=1;
  if (dir==DIR_BACKWARD)
  {
    uidIncrement=-1;
  }
  BufferItemUidType uid=startUid+uidIncrement;
  while (uid>=this->GetOldestItemUidInBuffer()
    && uid<=this->GetLatestItemUidInBuffer())
  { 
    TrackerBufferItem* bufferItem=this->TrackerBuffer->GetBufferItemFromUid(uid);
    if (bufferItem!=NULL && bufferItem->GetStatus()==TR_OK)
    {
      // found a valid item
      foundUid=uid;
      return ITEM_OK;
    }
    // found an item, but it's not valid, try the next one
    uid+=uidIncrement;
  }  
  if (dir==DIR_BACKWARD)
  {
    return ITEM_NOT_AVAILABLE_ANYMORE;
  }
  return ITEM_NOT_AVAILABLE_YET;
}

//----------------------------------------------------------------------------
ItemStatus vtkTrackerBuffer::GetClosestValidItemUid(double time, BufferItemUidType &closestValidUid)
{
  // itemA is the item that is the closest to the requested time
  // get its uid and time
  BufferItemUidType closestItemUid(0); 
  ItemStatus status = this->TrackerBuffer->GetItemUidFromTime(time, closestItemUid); 
  if ( status != ITEM_OK )
  {
    double lt(0); 
    this->TrackerBuffer->GetLatestTimeStamp(lt); 
    LOG_INFO("Latest timestamp: " << std::fixed << lt ); 
    LOG_WARNING("Failed to get tracker buffer item from time: " << std::fixed << time); 
    return status; 
  }
  TrackerBufferItem closestItem; 
  status = this->GetTrackerBufferItem(closestItemUid, &closestItem); 
  if ( status != ITEM_OK )
  {
    LOG_ERROR("Failed to get tracker buffer item with Uid: " << closestItemUid );
    return status; 
  }
  // If the status is OK then we found the closest valid item
  if ( closestItem.GetStatus() == TR_OK )
  {
    // the closest item is valid
    closestValidUid=closestItemUid;
    return ITEM_OK;
  }

  // The closest item is not valid (e.g., out of view), so search for 
  // the closest valid item

  LOG_TRACE("The closest element (uid="<<closestItemUid<<") to the requested timestamp ("<<time<<") is invalid");

  BufferItemUidType validItemBeforeUid=0;
  BufferItemUidType validItemAfterUid=0;
  bool foundValidElementBefore = GetNextValidItemUid(closestItemUid, DIR_BACKWARD, validItemBeforeUid)== ITEM_OK;
  bool foundValidElementAfter = GetNextValidItemUid(closestItemUid, DIR_FORWARD, validItemAfterUid)== ITEM_OK;

  if (!validItemBeforeUid && !validItemAfterUid)
  {
    LOG_ERROR("Cannot find any valid element in the buffer");  
    closestValidUid=-1;
    return ITEM_UNKNOWN_ERROR;
  }
  else if (validItemBeforeUid && !validItemAfterUid)
  {
    closestValidUid=validItemBeforeUid;
  }
  else if (!validItemBeforeUid && validItemAfterUid)
  {
    closestValidUid=validItemAfterUid;
  }
  else
  {
    // found a valid item before and after itemA, choose the one that is closer to the requested time
    double validItemBeforeClosestTime(0);
    double validItemAfterClosestTime(0);
    this->TrackerBuffer->GetTimeStamp(validItemBeforeUid, validItemBeforeClosestTime); 
    this->TrackerBuffer->GetTimeStamp(validItemAfterUid, validItemAfterClosestTime); 
    if (fabs(time-validItemBeforeClosestTime) < fabs(time-validItemAfterClosestTime))
    {
      // before is closer
      closestValidUid=validItemBeforeUid;
    }
    else
    {
      // after is closer
      closestValidUid=validItemAfterUid;
    }
  }    
  
  return ITEM_OK;
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

  // itemA is the item that is the closest to the requested time
  // get its uid and time
  BufferItemUidType itemAuid(0);   
  ItemStatus status=GetClosestValidItemUid(time, itemAuid);
  if ( status != ITEM_OK )
  {
    LOG_ERROR("Failed to get any valid item in the buffer");
    this->TrackerBuffer->Unlock();
    return status; 
  }

  TrackerBufferItem itemA; 
  status = this->GetTrackerBufferItem(itemAuid, &itemA, calibratedItem); 
  if ( status != ITEM_OK )
  {
    LOG_ERROR("Failed to get tracker buffer item with Uid: " << itemAuid );
    this->TrackerBuffer->Unlock();
    return status; 
  }

  double itemAtime(0);
  status = this->TrackerBuffer->GetTimeStamp(itemAuid, itemAtime); 
  if ( status != ITEM_OK )
  {
    LOG_WARNING("Failed to get tracker buffer timestamp with Uid: " << itemAuid ); 
    this->TrackerBuffer->Unlock();
    return status; 
  }

  // By default return the closest element (if cannot do or no need for interpolation)
  bufferItem->DeepCopy(&itemA);

  // If the time difference is negligible then don't interpolate
  const double NEGLIGIBLE_TIME_DIFFERENCE=0.00001; // in seconds
  if (fabs(itemAtime-time)<NEGLIGIBLE_TIME_DIFFERENCE)
  {
    //No need for interpolation, it's very close to the closest element
    this->TrackerBuffer->Unlock();
    return ITEM_OK;
  }

  // If the closest item is too far, then we don't do interpolation 
  if ( fabs(itemAtime-time)>this->GetMaxAllowedTimeDifference() )
  {
    LOG_ERROR("vtkTrackerBuffer: Cannot perform interpolation, time difference is too big " << std::fixed << fabs(itemAtime-time) << " ( " << itemAtime << ", " << time << ")." );    
    this->TrackerBuffer->Unlock();
    return ITEM_UNKNOWN_ERROR;
  }

  // Find the closest item on the other side of the timescale (so that time is between itemAtime and itemBtime) 
  BufferItemUidType itemBuid(0);
  if (time < itemAtime)
  {
    if ( GetNextValidItemUid(itemAuid, DIR_BACKWARD, itemBuid)!= ITEM_OK )
    {
      // This is just a debug because if there is no valid item, then the TrackerStatus is not OK, so we won't use it 
      LOG_DEBUG("vtkTrackerBuffer: Cannot perform interpolation, there is no available valid item before the closest item" << std::fixed << " ( closest time = " << itemAtime << ", requested time = " << time << "). Using the closest item." );    
      this->TrackerBuffer->Unlock();
      return ITEM_OK;
    }
  }
  else // itemAtime <= time
  {
    if ( GetNextValidItemUid(itemAuid, DIR_FORWARD, itemBuid)!= ITEM_OK )
    {
      // This is just a debug because if there is no valid item, then the TrackerStatus is not TR_OK, so we won't use it 
      LOG_DEBUG("vtkTrackerBuffer: Cannot perform interpolation, there is no available valid item after the closest item" << std::fixed << " ( closest time = " << itemAtime << ", requested time = " << time << "). Using the closest item." );    
      this->TrackerBuffer->Unlock();
      return ITEM_OK;
    }
  }

  // Get item B details
  double itemBtime(0); 
  status = this->TrackerBuffer->GetTimeStamp(itemBuid, itemBtime); 
  if ( status != ITEM_OK )
  {
    LOG_WARNING("Cannot do interpolation: Failed to get tracker buffer timestamp with Uid: " << itemBuid ); 
    this->TrackerBuffer->Unlock();
    return status; 
  }

  // If the next closest item is too far, then we don't do interpolation 
  if ( fabs(itemBtime-time)>this->GetMaxAllowedTimeDifference() )
  {
    LOG_ERROR("vtkTrackerBuffer: Cannot perform interpolation, time difference is too big " << std::fixed << fabs(itemBtime-time) << " ( " << itemBtime << ", " << time << ")." );    
    this->TrackerBuffer->Unlock();
    return ITEM_UNKNOWN_ERROR;
  }

  TrackerBufferItem itemB; 
  status = this->GetTrackerBufferItem(itemBuid, &itemB, calibratedItem); 
  if ( status != ITEM_OK )
  {
    LOG_WARNING("Failed to get tracker buffer item with Uid: " << itemBuid ); 
    this->TrackerBuffer->Unlock();
    return status; 
  }
  if ( itemB.GetStatus() != TR_OK )
  {
    LOG_WARNING("Cannot get a second element (uid="<<itemBuid<<") on the other side of the requested time ("<< std::fixed <<  time 
      <<"). Just use the closest element (uid=" << std::dec <<itemAuid <<", time=" << std::fixed << itemAtime<<").");
    this->TrackerBuffer->Unlock();
    return ITEM_OK; 
  }

  // We can unlock the buffer, we have all the data that we need to use
  this->TrackerBuffer->Unlock();

  //============== Get item weights ==================

  double itemAweight=fabs(itemBtime-time)/fabs(itemAtime-itemBtime);
  double itemBweight=1-itemAweight;

  //============== Get transform matrices ==================

  vtkSmartPointer<vtkMatrix4x4> itemAmatrix=vtkSmartPointer<vtkMatrix4x4>::New();
  if (itemA.GetMatrix(itemAmatrix)!=PLUS_SUCCESS)
  {
    LOG_ERROR("Failed to get item A matrix"); 
    return ITEM_UNKNOWN_ERROR;
  }
  double matrixA[3][3] = {0};
  double xyzA[3] = {0};
  for (int i = 0; i < 3; i++)
  {
    matrixA[i][0] = itemAmatrix->GetElement(i,0);
    matrixA[i][1] = itemAmatrix->GetElement(i,1);
    matrixA[i][2] = itemAmatrix->GetElement(i,2);
    xyzA[i] = itemAmatrix->GetElement(i,3);
  }  

  vtkSmartPointer<vtkMatrix4x4> itemBmatrix=vtkSmartPointer<vtkMatrix4x4>::New();
  if (itemB.GetMatrix(itemBmatrix)!=PLUS_SUCCESS)
  {
    LOG_ERROR("Failed to get item B matrix"); 
    return ITEM_UNKNOWN_ERROR;
  }
  double matrixB[3][3] = {0};
  double xyzB[3] = {0};
  for (int i = 0; i < 3; i++)
  {
    matrixB[i][0] = itemBmatrix->GetElement(i,0);
    matrixB[i][1] = itemBmatrix->GetElement(i,1);
    matrixB[i][2] = itemBmatrix->GetElement(i,2);
    xyzB[i] = itemBmatrix->GetElement(i,3);
  }

  //============== Interpolate rotation ==================

  double matrixAquat[4]= {0};
  vtkMath::Matrix3x3ToQuaternion(matrixA, matrixAquat);
  double matrixBquat[4]= {0};
  vtkMath::Matrix3x3ToQuaternion(matrixB, matrixBquat);
  double interpolatedRotationQuat[4]= {0};
  PlusMath::Slerp(interpolatedRotationQuat, itemBweight, matrixAquat, matrixBquat, false);
  double interpolatedRotation[3][3] = {0};
  vtkMath::QuaternionToMatrix3x3(interpolatedRotationQuat, interpolatedRotation);

  vtkSmartPointer<vtkMatrix4x4> interpolatedMatrix = vtkSmartPointer<vtkMatrix4x4>::New(); 
  for (int i = 0; i < 3; i++)
  {
    interpolatedMatrix->Element[i][0] = interpolatedRotation[i][0];
    interpolatedMatrix->Element[i][1] = interpolatedRotation[i][1];
    interpolatedMatrix->Element[i][2] = interpolatedRotation[i][2];
    interpolatedMatrix->Element[i][3] = xyzA[i]*itemAweight + xyzB[i]*itemBweight;
    //fprintf(stderr, "%f %f %f %f\n", xyz0[i], xyz1[i],  matrix->Element[i][3], f);
  } 

  bufferItem->SetMatrix(interpolatedMatrix); 

  //============== Interpolate time ==================

  bufferItem->SetFilteredTimestamp(time);

  double itemAunfilteredTimestamp = itemA.GetUnfilteredTimestamp(0); 
  double itemBunfilteredTimestamp = itemB.GetUnfilteredTimestamp(0); 
  double interpolatedUnfilteredTimestamp = itemAunfilteredTimestamp*itemAweight + itemBunfilteredTimestamp*itemBweight;
  bufferItem->SetUnfilteredTimestamp(interpolatedUnfilteredTimestamp); 

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

