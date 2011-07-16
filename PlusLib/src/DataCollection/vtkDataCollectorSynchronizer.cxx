#include "PlusConfigure.h"
#include "vtkDataCollectorSynchronizer.h"
#include "vtkObjectFactory.h"
#include "vtkImageDifference.h"
#include "vtkImageExtractComponents.h"
#include "vtkSmartPointer.h"
#include "vtkImageImport.h"
#include "vtkBMPWriter.h"
#include "vtkImageResample.h"
#include "vtksys/SystemTools.hxx"
#include "vtkGnuplotExecuter.h"
#include "vtkHTMLGenerator.h"
#include "vtkVariantArray.h"

#include <algorithm>

vtkCxxRevisionMacro(vtkDataCollectorSynchronizer, "$Revision: 1.0 $");
vtkStandardNewMacro(vtkDataCollectorSynchronizer); 

//----------------------------------------------------------------------------
vtkDataCollectorSynchronizer::vtkDataCollectorSynchronizer()
{
  this->ProgressBarUpdateCallbackFunction = NULL; 

  this->BaseFrame = NULL; 
  this->SynchronizedOff(); 
  this->ThresholdMultiplier = 5; 
  this->SyncStartTime = 0.0; 
  this->StartupDelaySec = 0.0; 
  this->MinNumOfSyncSteps = 5; 
  this->SynchronizationTimeLength = 10; 
  this->StillFrameIndexInterval = 0; 
  this->StillFrameTimeInterval = 0; 

  this->VideoBuffer = NULL; 
  this->CurrentVideoBufferIndex = 0; 
  this->NumberOfAveragedFrames = 30; // Number of frames used for computing the mean difference between the images
  this->FrameDifferenceMean = 0; 
  this->FrameDifferenceThreshold = 0; 
  this->MinFrameThreshold = 5.0; 
  this->MaxFrameDifference = 2.0; 
  this->VideoOffset = 0.0; 

  this->TrackerBuffer = NULL;
  this->CurrentTrackerBufferIndex = 0; 
  this->NumberOfAveragedTransforms = 30; // Number of transforms used for computing the mean difference between the transforms
  this->MinTransformThreshold = 1.0; 
  this->MaxTransformDifference = 1.0; 
  this->TrackerOffset = 0.0; 

  this->SyncReportTable = NULL; 
}

//----------------------------------------------------------------------------
vtkDataCollectorSynchronizer::~vtkDataCollectorSynchronizer()
{
  this->SetBaseFrame(NULL); 

  this->SetTrackerBuffer(NULL); 
  this->SetVideoBuffer(NULL); 

  if ( this->SyncReportTable != NULL )
  {
    this->SyncReportTable->Delete(); 
    this->SyncReportTable = NULL; 
  }
}

//----------------------------------------------------------------------------
void vtkDataCollectorSynchronizer::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
} 



//----------------------------------------------------------------------------
PlusStatus vtkDataCollectorSynchronizer::Synchronize()
{
  LOG_TRACE("vtkDataCollectorSynchronizer::Synchronize"); 

  if ( this->SyncReportTable == NULL )
  {
    this->InitSyncReportTable(); 
  }

  if ( this->SyncStartTime < 1 )
  {
    double oldestTimestamp(0); 
    if ( this->TrackerBuffer->GetOldestTimeStamp(oldestTimestamp) != ITEM_OK )
    {
      LOG_ERROR("Failed to get oldest timestamp from tracker timestamp!"); 
      return PLUS_FAIL; 
    }

    this->SetSyncStartTime( oldestTimestamp + this->StartupDelaySec ); 
    LOG_DEBUG("Sync start time: " << this->SyncStartTime ); 
  }

  // Set the time and index interval where the frames and transforms should be unchanged
  this->StillFrameIndexInterval = floor(1.0*this->GetNumberOfAveragedTransforms()/2.0); 
  this->StillFrameTimeInterval = 1.0 * this->StillFrameIndexInterval / this->VideoBuffer->GetFrameRate(); 

  // Clear previous results 
  this->TransformTimestamp.clear(); 
  this->FrameTimestamp.clear(); 

  std::vector<double> movedTransformTimestamps; 

  if ( this->DetectTrackerMotions(movedTransformTimestamps) != PLUS_SUCCESS )
  {
    LOG_ERROR("Unable to detect tracker motions!"); 
    return PLUS_FAIL; 
  }

  if ( this->DetectVideoMotions(movedTransformTimestamps) != PLUS_SUCCESS )
  {
    LOG_ERROR("Unable to detect video motions!"); 
    return PLUS_FAIL; 
  }

  if ( this->ProgressBarUpdateCallbackFunction != NULL )
  {
    (*ProgressBarUpdateCallbackFunction)(100); 
  }

  // Print results to stdout 
  double posAcqMean(0), posAcqDeviation(0); 
  double posFrameRate = this->GetPositionAcquisitionFrameRate(posAcqMean, posAcqDeviation); 

  double imgAcqMean(0), imgAcqDeviation(0); 
  double imgFrameRate = this->GetImageAcquisitionFrameRate(imgAcqMean, imgAcqDeviation); 

  LOG_INFO("Image Acquisition Frame Rate:    " << imgFrameRate << "  Mean Image Acquisition Time:    " << 1000*imgAcqMean << "ms  Image Acquisition Deviation:    " << 1000*imgAcqDeviation <<"ms" ); 
  LOG_INFO("Position Acquisition Frame Rate: " << posFrameRate << "  Mean Position Acquisition Time: " << 1000*posAcqMean << "ms  Position Acquisition Deviation: " << 1000*posAcqDeviation <<"ms" ); 

  LOG_DEBUG("Video Offset: "); 
  for ( unsigned int i = 0; i < this->FrameTimestamp.size(); i++ )
  {
    LOG_DEBUG("\t\t" << 1000*(this->TransformTimestamp[i] - this->FrameTimestamp[i]) << " ms" ); 
  }


  const int numOfSuccessfulSyncSteps = this->FrameTimestamp.size(); 
  if (  numOfSuccessfulSyncSteps < MinNumOfSyncSteps )
  {
    LOG_WARNING("Number of synchronization periods were less than expected (" << this->FrameTimestamp.size() << " of " << MinNumOfSyncSteps << "): The result is not reliable!"); 
    this->SetTrackerOffset(0); 
    this->SetVideoOffset(0); 
  }
  else
  {
    this->RemoveOutliers(); 
    double meanVideoOffset(0), stdevVideoOffset(0), minVideoOffset(0), maxVideoOffset(0); 
    this->GetOffsetStatistics(meanVideoOffset, stdevVideoOffset, minVideoOffset, maxVideoOffset); 

    double meanAcqTimeDifference = (imgAcqMean - posAcqMean) / 2.0; 
    LOG_DEBUG("Mean Acq Time Difference = " << 1000*meanAcqTimeDifference << " ms"); 

    // TODO: check it later on, mantis #290: Subtract the mean acquisition time difference from the sync offset 
    //meanVideoOffset = meanVideoOffset - meanAcqTimeDifference; 

    this->SetTrackerOffset(0); 
    this->SetVideoOffset(meanVideoOffset); 
    LOG_INFO("Mean Video Offset = " << 1000*meanVideoOffset << " ms"); 
    LOG_INFO("Stdev Video Offset = " << 1000*stdevVideoOffset << " ms"); 
    LOG_INFO("Min Video Offset = " << 1000*minVideoOffset << " ms"); 
    LOG_INFO("Max Video Offset = " << 1000*maxVideoOffset << " ms");

    this->SynchronizedOn(); 
  }

  return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
PlusStatus vtkDataCollectorSynchronizer::DetectTrackerMotions(std::vector<double> &movedTransformTimestamps)
{
  LOG_TRACE("vtkDataCollectorSynchronizer::DetectTrackerMotions"); 

  BufferItemUidType trackerBufferIndex(0); 
  if ( this->TrackerBuffer->GetItemUidFromTime( this->SyncStartTime, trackerBufferIndex ) != ITEM_OK )
  {
    LOG_ERROR("Failed to get item UID from time: " << std::fixed << this->SyncStartTime ); 
    return PLUS_FAIL; 
  }

  BufferItemUidType stillPositionIndex = trackerBufferIndex + this->StillFrameIndexInterval;
  int syncStep(1); 

  // clear input vector 
  movedTransformTimestamps.clear(); 

  const BufferItemUidType latestTrackerUid = this->TrackerBuffer->GetLatestItemUidInBuffer(); 

  while ( trackerBufferIndex <= latestTrackerUid && stillPositionIndex <= latestTrackerUid)
  {
    LOG_TRACE("Tracker sync step: " << syncStep ); 

    if ( this->ProgressBarUpdateCallbackFunction != NULL )
    {
      const int percent = floor(100.0*(trackerBufferIndex - this->TrackerBuffer->GetOldestItemUidInBuffer()) / (1.0 * this->TrackerBuffer->GetBufferSize())); 
      (*ProgressBarUpdateCallbackFunction)(percent); 
    }

    // Set the initial index for still position 
    stillPositionIndex = trackerBufferIndex + this->StillFrameIndexInterval; 
    this->FindStillTransform(trackerBufferIndex, stillPositionIndex ); 

    if ( stillPositionIndex <= latestTrackerUid )
    {
      double stillTransformTimestamp(0); 
      if ( this->GetTrackerBuffer()->GetTimeStamp(trackerBufferIndex, stillTransformTimestamp) != ITEM_OK )
      {
        LOG_ERROR("Failed to get timestamp for tracker buffer item by UID: " << trackerBufferIndex); 
        trackerBufferIndex++;
        continue; 
      }

      if ( this->ComputeTransformThreshold( trackerBufferIndex ) != PLUS_SUCCESS )
      {
        LOG_WARNING("Failed to compute transform threshold..."); 
        trackerBufferIndex++; 
        continue; 
      }

      double movedTransformTimestamp(0); 
      if ( this->FindTransformMotionTimestamp( trackerBufferIndex, movedTransformTimestamp ) == PLUS_SUCCESS )
      {
        if ( movedTransformTimestamps.size() == 0 || movedTransformTimestamps.back() + this->StillFrameTimeInterval < movedTransformTimestamp)
        {
          movedTransformTimestamps.push_back(movedTransformTimestamp); 
        }
        else
        {
          LOG_DEBUG("This movement is too close to the previous one ( " << 1000*(movedTransformTimestamp - movedTransformTimestamps.back()) << "ms)"); 
        }

      }
    }
    syncStep++; 
  }

  return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
PlusStatus vtkDataCollectorSynchronizer::DetectVideoMotions(const std::vector<double> &movedTransformTimestamps)
{
  LOG_TRACE("vtkDataCollectorSynchronizer::DetectVideoMotions"); 

  BufferItemUidType videoBufferIndex = this->VideoBuffer->GetOldestItemUidInBuffer(); 

  LOG_DEBUG("Still Frame Search Interval: " << 1000*this->StillFrameTimeInterval << "ms"); 

  int videoSyncStep(1); 
  for ( unsigned int i = 0; i < movedTransformTimestamps.size() && videoBufferIndex <= this->VideoBuffer->GetLatestItemUidInBuffer(); i++ )
  {
    LOG_DEBUG("***************************************************************************************"); 
    LOG_DEBUG(std::fixed << "Next detected tracker motion timestamp: " << movedTransformTimestamps[i] ); 
    // Choose an initial timestamp to search for still images 
    double stillFrameTimestamp = movedTransformTimestamps[i] - this->StillFrameTimeInterval; 

    if ( this->FrameTimestamp.size() > 0 && stillFrameTimestamp < this->FrameTimestamp.back() )
    {
      // still frame timestamp cannot be less than the last moved frame timestamp 
      stillFrameTimestamp = this->FrameTimestamp.back() + 0.5; 
    }

    // Find the initial frame index 
    BufferItemUidType idx(0); 
    if ( this->VideoBuffer->GetItemUidFromTime( stillFrameTimestamp, idx) != ITEM_OK )
    {
      LOG_WARNING("vtkDataCollectorSynchronizer: Unable to get frame UID for timestamp: " << std::fixed << stillFrameTimestamp);
      continue; 
    }
    else
    {
      videoBufferIndex = idx; 
    }

    BufferItemUidType stillFrameIndex = videoBufferIndex + this->GetNumberOfAveragedFrames(); 
    if ( stillFrameIndex > this->VideoBuffer->GetLatestItemUidInBuffer() )
    {
      stillFrameIndex = this->VideoBuffer->GetLatestItemUidInBuffer(); 
    }

    // Find the timestamp of next possible movement 
    double nextMovedTimestamp = movedTransformTimestamps[i] + this->StillFrameTimeInterval; 
    /*if ( i + 1 < movedTransformTimestamps.size() )
    {
    nextMovedTimestamp = movedTransformTimestamps[i + 1] - this->StillFrameTimeInterval; 
    }*/

    // Find the next still frame 
    this->SetBaseFrame(NULL); 
    this->FindStillFrame(videoBufferIndex, stillFrameIndex ); 

    VideoBufferItem videoItem; 
    if ( this->VideoBuffer->GetVideoBufferItem(videoBufferIndex, &videoItem) != ITEM_OK )
    {
      LOG_WARNING("vtkDataCollectorSynchronizer: Unable to get video frame with frame UID: " << videoBufferIndex); 
      continue; 
    }

    const double localTimeOffset = 0; 
    stillFrameTimestamp = videoItem.GetTimestamp(localTimeOffset); 
    LOG_DEBUG("Still frame timestamp: " << std::fixed << stillFrameTimestamp); 

    if ( videoBufferIndex < this->VideoBuffer->GetLatestItemUidInBuffer() ) 
    {
      // Set the baseframe of the image compare 
      vtkSmartPointer<vtkImageData> baseframe = vtkSmartPointer<vtkImageData>::New(); 
      if ( this->CopyVideoFrame(baseframe, videoItem.GetFrame()) != PLUS_SUCCESS )
      {
        LOG_WARNING("vtkDataCollectorSynchronizer: Unable to copy image from video buffer!"); 
        continue; 
      }

      if ( baseframe == NULL )
      {
        LOG_WARNING("vtkDataCollectorSynchronizer: Unable to resize base frame if it's NULL - continue with next frame."); 
        continue; 
      }

      vtkSmartPointer<vtkImageData> baseframeRGB = vtkSmartPointer<vtkImageData>::New(); 
      this->ConvertFrameToRGB(baseframe, baseframeRGB, 0.5); 
      this->SetBaseFrame( baseframeRGB ); 

      // Compute the frame threshold
      if ( this->ComputeFrameThreshold( videoBufferIndex ) != PLUS_SUCCESS )
      { 
        LOG_WARNING("Failed to compute frame threshold..."); 
        continue; 
      }

      if ( this->VideoBuffer->GetVideoBufferItem(videoBufferIndex, &videoItem) != ITEM_OK )
      {
        LOG_WARNING("vtkDataCollectorSynchronizer: Unable to get video frame with frame UID: " << videoBufferIndex); 
        continue; 
      }

      double currentFrameTimestamp = videoItem.GetTimestamp(localTimeOffset); 

      if ( currentFrameTimestamp > movedTransformTimestamps[i] )
      {
        LOG_DEBUG("!!! Start frame timestamp is already over the moved transform timestamp (difference: " << 1000*(currentFrameTimestamp - movedTransformTimestamps[i]) << "ms)"); 
        continue; 
      }

      // Find the moved image timestamp 
      double movedFrameTimestamp(0); 
      if ( this->FindFrameTimestamp( videoBufferIndex, movedFrameTimestamp, nextMovedTimestamp ) )	
      {
        // Save the frame and transform timestamp 
        this->FrameTimestamp.push_back(movedFrameTimestamp); 
        this->TransformTimestamp.push_back(movedTransformTimestamps[i]); 

        LOG_DEBUG("==> Joint video and tracker motion detected!" ); 
        LOG_DEBUG("    Video motion timestamp: " << std::fixed << movedFrameTimestamp); 
        LOG_DEBUG("    Tracker motion timestamp: " << std::fixed << movedTransformTimestamps[i]); 
        LOG_DEBUG("    Tracker to video offset: " << std::fixed << 1000*(movedTransformTimestamps[i] - movedFrameTimestamp) << "ms"); 
      }
      else
      {
        // The result is not reliable
        LOG_DEBUG(">>>>>> Step " << videoSyncStep << " - Result is not reliable!"); 
      }
    }
    videoSyncStep++; 
  }

  return PLUS_SUCCESS; 
}


//----------------------------------------------------------------------------
void vtkDataCollectorSynchronizer::RemoveOutliers()
{
  LOG_TRACE("vtkDataCollectorSynchronizer::RemoveOutliers"); 
  double meanVideoOffset(0), stdevVideoOffset(0), minVideoOffset(0), maxVideoOffset(0); 
  this->GetOffsetStatistics(meanVideoOffset, stdevVideoOffset, minVideoOffset, maxVideoOffset); 

  int numOfElements = this->FrameTimestamp.size(); 
  for ( int i = numOfElements - 1; i >= 0; --i )
  {	
    double offset = this->TransformTimestamp[i] - this->FrameTimestamp[i]; 

    if ( abs(meanVideoOffset - offset) > 2.0 * stdevVideoOffset )
    {
      LOG_DEBUG("Remove offset outlier ("<< i << "): " << offset); 
      this->FrameTimestamp.erase( this->FrameTimestamp.begin() + i); 
      this->TransformTimestamp.erase( this->TransformTimestamp.begin() + i); 
    }
  }
}

//----------------------------------------------------------------------------
void vtkDataCollectorSynchronizer::GetOffsetStatistics(double &meanVideoOffset, double &stdevVideoOffset, double &minVideoOffset, double &maxVideoOffset)
{
  LOG_TRACE("vtkDataCollectorSynchronizer::GetOffsetStatistics"); 
  meanVideoOffset = 0; 
  for ( unsigned int i = 0; i < this->FrameTimestamp.size(); i++ )
  {
    double offset = this->TransformTimestamp[i] - this->FrameTimestamp[i]; 
    if ( i == 0 )
    {
      minVideoOffset = offset; 
      maxVideoOffset = offset; 
    }
    else if ( offset < minVideoOffset )
    {
      minVideoOffset = offset; 
    }
    else if ( offset > maxVideoOffset )
    {
      maxVideoOffset = offset; 
    }

    meanVideoOffset += offset / (1.0*this->FrameTimestamp.size()); 
  }	

  stdevVideoOffset = 0; 
  for ( unsigned int i = 0; i < this->FrameTimestamp.size(); i++ )
  {
    double offset = this->TransformTimestamp[i] - this->FrameTimestamp[i]; 
    stdevVideoOffset += pow((meanVideoOffset - offset), 2.0) / (1.0*this->FrameTimestamp.size()); 
  }	
  stdevVideoOffset = sqrt(stdevVideoOffset); 
}

//----------------------------------------------------------------------------
PlusStatus vtkDataCollectorSynchronizer::ComputeTransformThreshold( BufferItemUidType& bufferIndex )
{
  LOG_TRACE("vtkDataCollectorSynchronizer::ComputeTransformThreshold"); 
  int sizeOfAvgPositons(0); 
  std::vector< vtkSmartPointer<vtkTransform> > avgTransforms; 
  for ( bufferIndex; bufferIndex <= this->TrackerBuffer->GetLatestItemUidInBuffer() && sizeOfAvgPositons != this->NumberOfAveragedTransforms; bufferIndex++ )
  {
    TrackerBufferItem bufferItem; 
    if ( this->GetTrackerBuffer()->GetTrackerBufferItem(bufferIndex, &bufferItem, false) != ITEM_OK ) 
    {
      LOG_ERROR("Failed to get tracker buffer item with UID: " << bufferIndex ); 
      return PLUS_FAIL; 
    }

    if ( bufferItem.GetStatus() == TR_OK )
    {
      vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New(); 
      transform->SetMatrix( bufferItem.GetMatrix() ); 
      avgTransforms.push_back(transform); 
      sizeOfAvgPositons++; 
    }
  }

  const double numElements = avgTransforms.size(); 

  // compute the mean of the buffer for each matrix elements
  PositionContainer meanTransform; 
  meanTransform.Rx = meanTransform.Ry = meanTransform.Rz = 0; 
  meanTransform.Tx = meanTransform.Ty = meanTransform.Tz = 0; 

  for ( int i = 0; i < numElements; i++ )
  {
    double orient[3] = {0,0,0}; 
    avgTransforms[i]->GetOrientation(orient);

    double position[3] = {0,0,0};
    avgTransforms[i]->GetPosition(position); 

    meanTransform.Rx = meanTransform.Rx + (orient[0] / numElements) ; 
    meanTransform.Ry = meanTransform.Ry + (orient[1] / numElements) ; 
    meanTransform.Rz = meanTransform.Rz + (orient[2] / numElements) ; 

    meanTransform.Tx = meanTransform.Tx + (position[0] / numElements) ; 
    meanTransform.Ty = meanTransform.Ty + (position[1] / numElements) ; 
    meanTransform.Tz = meanTransform.Tz + (position[2] / numElements) ; 
  }

  this->PositionTransformMean = meanTransform; 

  // compute the deviation of the buffer for each matrix elements
  PositionContainer deviationTransform; 
  deviationTransform.Rx = deviationTransform.Ry = deviationTransform.Rz = 0; 
  deviationTransform.Tx = deviationTransform.Ty = deviationTransform.Tz = 0; 

  for ( int i = 0; i < numElements; i++ )
  {
    double orient[3] = {0,0,0}; 
    avgTransforms[i]->GetOrientation(orient);

    double position[3] = {0,0,0};
    avgTransforms[i]->GetPosition(position); 

    deviationTransform.Rx = deviationTransform.Rx + ( pow( orient[0] -  meanTransform.Rx, 2.0 ) ) / numElements; 
    deviationTransform.Ry = deviationTransform.Ry + ( pow( orient[1] -  meanTransform.Ry, 2.0 ) ) / numElements; 
    deviationTransform.Rz = deviationTransform.Rz + ( pow( orient[2] -  meanTransform.Rz, 2.0 ) ) / numElements; 

    deviationTransform.Tx = deviationTransform.Tx + ( pow( position[0] -  meanTransform.Tx, 2.0 ) ) / numElements; 
    deviationTransform.Ty = deviationTransform.Ty + ( pow( position[1] -  meanTransform.Ty, 2.0 ) ) / numElements; 
    deviationTransform.Tz = deviationTransform.Tz + ( pow( position[2] -  meanTransform.Tz, 2.0 ) ) / numElements; 
  }

  deviationTransform.Rx = sqrt(deviationTransform.Rx); 
  deviationTransform.Ry =	sqrt(deviationTransform.Ry); 
  deviationTransform.Rz =	sqrt(deviationTransform.Rz); 

  deviationTransform.Tx =	sqrt(deviationTransform.Tx); 
  deviationTransform.Ty =	sqrt(deviationTransform.Ty); 
  deviationTransform.Tz =	sqrt(deviationTransform.Tz); 


  // compute the threshold values for rotation and translation axes
  this->PositionTransformThreshold.Rx = (deviationTransform.Rx > this->MinTransformThreshold ? ThresholdMultiplier * deviationTransform.Rx : this->MinTransformThreshold); 
  this->PositionTransformThreshold.Ry = (deviationTransform.Ry > this->MinTransformThreshold ? ThresholdMultiplier * deviationTransform.Ry : this->MinTransformThreshold); 
  this->PositionTransformThreshold.Rz = (deviationTransform.Rz > this->MinTransformThreshold ? ThresholdMultiplier * deviationTransform.Rz : this->MinTransformThreshold);

  this->PositionTransformThreshold.Tx = (deviationTransform.Tx > this->MinTransformThreshold ? ThresholdMultiplier * deviationTransform.Tx : this->MinTransformThreshold); 
  this->PositionTransformThreshold.Ty = (deviationTransform.Ty > this->MinTransformThreshold ? ThresholdMultiplier * deviationTransform.Ty : this->MinTransformThreshold); 
  this->PositionTransformThreshold.Tz = (deviationTransform.Tx > this->MinTransformThreshold ? ThresholdMultiplier * deviationTransform.Tz : this->MinTransformThreshold); 

  return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
PlusStatus vtkDataCollectorSynchronizer::FindTransformMotionTimestamp( BufferItemUidType& bufferIndex, double& movedTransformTimestamp  )
{
  LOG_TRACE("vtkDataCollectorSynchronizer::FindTransformMotionTimestamp"); 
  bool diffFound = false; 
  const double localTimeOffset(0);

  while ( !diffFound && bufferIndex <= this->TrackerBuffer->GetLatestItemUidInBuffer() )
  {
    TrackerBufferItem bufferItem;
    if ( this->GetTrackerBuffer()->GetTrackerBufferItem(bufferIndex, &bufferItem, false ) != ITEM_OK )
    {
      LOG_ERROR("Failed to get tracker buffer item with UID: " << bufferIndex ); 
      bufferIndex++; 
      continue; 
    }

    if ( bufferItem.GetStatus() == TR_OK )
    {
      double timestamp = bufferItem.GetTimestamp(localTimeOffset); 
      unsigned long frameNumber = bufferItem.GetIndex();

      vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New(); 
      transform->SetMatrix( bufferItem.GetMatrix() ); 

      if ( ! this->IsTransformBelowThreshold(transform, timestamp) )
      {
        LOG_DEBUG("---> Tracker motion detected at: " << std::fixed << timestamp); 
        movedTransformTimestamp = timestamp; 
        diffFound = true; 
      }
    }

    bufferIndex++; 
  }

  if (!diffFound)
  {
    return PLUS_FAIL;
  }
  return PLUS_SUCCESS; 

}

//----------------------------------------------------------------------------
void vtkDataCollectorSynchronizer::FindStillTransform( BufferItemUidType& baseIndex, BufferItemUidType& currentIndex )
{
  LOG_TRACE("vtkDataCollectorSynchronizer::FindStillTransform"); 
  while ( currentIndex <= this->GetTrackerBuffer()->GetLatestItemUidInBuffer() && baseIndex <= this->GetTrackerBuffer()->GetLatestItemUidInBuffer() && baseIndex != currentIndex )
  {

    TrackerBufferItem baseItem; 
    if ( this->GetTrackerBuffer()->GetTrackerBufferItem(baseIndex, &baseItem, false) != ITEM_OK )
    {
      LOG_ERROR("Failed to get tracker buffer item with UID: " << baseIndex ); 
      baseIndex = baseIndex + 1; 
      currentIndex = baseIndex + this->StillFrameIndexInterval; 
      continue; 
    }

    if ( baseItem.GetStatus() != TR_OK )
    {
      // Tracker status was invalid, move to next item
      baseIndex = baseIndex + 1; 
      currentIndex = baseIndex + this->StillFrameIndexInterval; 
      continue; 
    }

    TrackerBufferItem currentItem; 
    if ( this->GetTrackerBuffer()->GetTrackerBufferItem(currentIndex, &currentItem, false) != ITEM_OK )
    {
      LOG_ERROR("Failed to get tracker buffer item with UID: " << currentIndex ); 
      baseIndex = baseIndex + 1; 
      currentIndex = baseIndex + this->StillFrameIndexInterval; 
      continue; 
    }

    if ( currentItem.GetStatus() != TR_OK )
    {
      // Tracker status was invalid, move to next item
      baseIndex = baseIndex + 1; 
      currentIndex = baseIndex + this->StillFrameIndexInterval; 
      continue; 
    }

    if ( this->GetTranslationError(baseItem.GetMatrix(), currentItem.GetMatrix() ) < this->MaxTransformDifference 
      && this->GetRotationError(baseItem.GetMatrix(), currentItem.GetMatrix()) < this->MaxTransformDifference )
    {
      // This item is below threshold, continue with the preceding item in the buffer
      currentIndex = currentIndex - 1; 
      FindStillTransform(baseIndex, currentIndex); 
    }
    else
    {
      // Item was over the threshold, move the base index and start the search from the beginning
      baseIndex = baseIndex + 1; 
      currentIndex = baseIndex + this->StillFrameIndexInterval; 
      FindStillTransform(baseIndex, currentIndex); 
    }
  }
}

//----------------------------------------------------------------------------
double vtkDataCollectorSynchronizer::GetRotationError(vtkMatrix4x4* baseTransMatrix, vtkMatrix4x4* currentTransMatrix)
{
  LOG_TRACE("vtkDataCollectorSynchronizer::GetRotationError"); 
  vtkSmartPointer<vtkMatrix4x4> diffTransMatrix = vtkSmartPointer<vtkMatrix4x4>::New(); 
  vtkSmartPointer<vtkMatrix4x4> invCurrentTransMatrix = vtkSmartPointer<vtkMatrix4x4>::New(); 

  vtkMatrix4x4::Invert(currentTransMatrix, invCurrentTransMatrix);  

  vtkMatrix4x4::Multiply4x4(baseTransMatrix, invCurrentTransMatrix, diffTransMatrix); 

  vtkSmartPointer<vtkTransform> diffTransform = vtkSmartPointer<vtkTransform>::New(); 
  diffTransform->SetMatrix(diffTransMatrix); 

  return diffTransform->GetOrientationWXYZ()[0]; 
}

//----------------------------------------------------------------------------
double vtkDataCollectorSynchronizer::GetTranslationError(vtkMatrix4x4* baseTransMatrix, vtkMatrix4x4* currentTransMatrix)
{
  LOG_TRACE("vtkDataCollectorSynchronizer::GetTranslationError"); 
  vtkSmartPointer<vtkTransform> baseTransform = vtkSmartPointer<vtkTransform>::New(); 
  baseTransform->SetMatrix(baseTransMatrix); 

  vtkSmartPointer<vtkTransform> currentTransform = vtkSmartPointer<vtkTransform>::New(); 
  currentTransform->SetMatrix(currentTransMatrix); 

  double bx = baseTransform->GetPosition()[0]; 
  double by = baseTransform->GetPosition()[1]; 
  double bz = baseTransform->GetPosition()[2]; 

  double cx = currentTransform->GetPosition()[0]; 
  double cy = currentTransform->GetPosition()[1]; 
  double cz = currentTransform->GetPosition()[2]; 

  // Euclidean distance
  double distance = sqrt( pow(bx-cx,2) + pow(by-cy,2) + pow(bz-cz,2) ); 

  return distance; 
}

//----------------------------------------------------------------------------
bool vtkDataCollectorSynchronizer::IsTransformBelowThreshold( vtkTransform* transform, double timestamp)
{
  //LOG_TRACE("vtkDataCollectorSynchronizer::IsTransformBelowThreshold"); 
  // Get the current positions and orientations 
  double currRotX = transform->GetOrientation()[0]; 
  double currRotY = transform->GetOrientation()[1]; 
  double currRotZ = transform->GetOrientation()[2]; 

  double currPosX = transform->GetPosition()[0]; 
  double currPosY = transform->GetPosition()[1]; 
  double currPosZ = transform->GetPosition()[2]; 


  const double transformDifference = abs(currPosX - PositionTransformMean.Tx) + abs(currPosY - PositionTransformMean.Ty) + abs(currPosZ - PositionTransformMean.Tz); 
  // create a new row for the sync report table 
  vtkSmartPointer<vtkVariantArray> rowSyncReportTable = vtkSmartPointer<vtkVariantArray>::New(); 
  std::ostringstream strTimestamp; 
  strTimestamp << std::fixed << timestamp; 
  rowSyncReportTable->InsertNextValue(vtkVariant(strTimestamp.str())); 

  std::ostringstream strTransformDifference; 
  strTransformDifference << std::fixed << transformDifference; 
  rowSyncReportTable->InsertNextValue(vtkVariant(strTransformDifference.str())); 

  // Insert NaN to the video timestamp and diff cols 
  rowSyncReportTable->InsertNextValue(vtkVariant("NaN")); 
  rowSyncReportTable->InsertNextValue(vtkVariant("NaN"));

  if ( this->SyncReportTable )
  {
    this->SyncReportTable->InsertNextRow(rowSyncReportTable); 
  }
  else
  {
    LOG_WARNING("Failed to add new row to sync report table - table is NULL!"); 
  }

  // Compare the current position and orientation with the threshold value
  if (abs(currRotX - this->PositionTransformMean.Rx) > this->PositionTransformThreshold.Rx 
    || 
    abs(currRotY - this->PositionTransformMean.Ry) > this->PositionTransformThreshold.Ry 
    || 
    abs(currRotZ - this->PositionTransformMean.Rz) > this->PositionTransformThreshold.Rz 
    ||
    abs(currPosX - this->PositionTransformMean.Tx) > this->PositionTransformThreshold.Tx 
    || 
    abs(currPosY - this->PositionTransformMean.Ty) > this->PositionTransformThreshold.Ty 
    || 
    abs(currPosZ - this->PositionTransformMean.Tz) > this->PositionTransformThreshold.Tz )
  {
    // larger then threshold => tracker moved
    return false;
  }

  // less then threshold => tracker NOT moved
  return true; 
}

//----------------------------------------------------------------------------
void vtkDataCollectorSynchronizer::ConvertFrameToRGB( vtkImageData* pFrame, vtkImageData* pFrameRGB, double resampleFactor )
{
  LOG_TRACE("vtkDataCollectorSynchronizer::ConvertFrameToRGB"); 

  // Make the image smaller
  vtkSmartPointer<vtkImageResample> resample = vtkSmartPointer<vtkImageResample>::New();
  resample->SetInput(pFrame); 
  resample->SetAxisMagnificationFactor(0, resampleFactor); 
  resample->SetAxisMagnificationFactor(1, resampleFactor); 
  resample->SetAxisMagnificationFactor(2, resampleFactor);

  vtkImageData* resampledImage = resample->GetOutput(); 

  if ( resampledImage->GetNumberOfScalarComponents() != 3 )
  {
    vtkSmartPointer<vtkImageExtractComponents> imageExtractor =  vtkSmartPointer<vtkImageExtractComponents>::New(); 
    imageExtractor->SetInput(resampledImage); 
    // we are using only the 0th component
    imageExtractor->SetComponents(0,0,0);
    imageExtractor->Update(); 

    pFrameRGB->DeepCopy(imageExtractor->GetOutput()); 
    pFrameRGB->Update();
  }
  else 
  {
    pFrameRGB->DeepCopy(resampledImage); 
    pFrameRGB->Update(); 
  }
}

//----------------------------------------------------------------------------
PlusStatus vtkDataCollectorSynchronizer::CopyVideoFrame( vtkImageData* frame, VideoBufferItem::ImageType::Pointer& frameInBuffer )
{
  LOG_TRACE("vtkDataCollectorSynchronizer::CopyVideoFrame"); 

  if ( frameInBuffer.IsNull() )
  {
    LOG_WARNING("vtkDataCollectorSynchronizer: Failed to copy NULL video frame!"); 
    return PLUS_FAIL; 
  }

  return UsImageConverterCommon::ConvertItkImageToVtkImage(frameInBuffer, frame); 
}

//----------------------------------------------------------------------------
void vtkDataCollectorSynchronizer::FindStillFrame( BufferItemUidType& baseIndex, BufferItemUidType& currentIndex )
{
  LOG_TRACE("vtkDataCollectorSynchronizer::FindStillFrame"); 
  const BufferItemUidType latestFrameUid = this->VideoBuffer->GetLatestItemUidInBuffer(); 
  VideoBufferItem videoItem; 
  while ( currentIndex <= latestFrameUid && baseIndex <= latestFrameUid && baseIndex != currentIndex )
  {
    if ( this->ProgressBarUpdateCallbackFunction != NULL )
    {
      const int videosyncprogress = floor(100.0*(baseIndex - this->TrackerBuffer->GetOldestItemUidInBuffer()) / (1.0 * this->GetVideoBuffer()->GetNumberOfItems())); 
      (*ProgressBarUpdateCallbackFunction)(videosyncprogress); 
    }

    if ( this->GetBaseFrame() == NULL ) 
    {
      if ( this->VideoBuffer->GetVideoBufferItem(baseIndex, &videoItem) != ITEM_OK )
      {
        LOG_WARNING("vtkDataCollectorSynchronizer: Unable to get frame from video buffer!"); 
        baseIndex = baseIndex + 1; 
        currentIndex = baseIndex + this->GetNumberOfAveragedFrames(); 
        this->SetBaseFrame(NULL); 
        continue; 
      }

      vtkImageData* baseframe = vtkImageData::New(); 
      if ( this->CopyVideoFrame(baseframe, videoItem.GetFrame()) != PLUS_SUCCESS )
      {
        LOG_WARNING("vtkDataCollectorSynchronizer: Unable to copy video frame!"); 
        if ( baseframe != NULL )
        {
          baseframe->Delete(); 
          baseframe = NULL; 
        }
        baseIndex = baseIndex + 1; 
        currentIndex = baseIndex + this->GetNumberOfAveragedFrames(); 
        this->SetBaseFrame(NULL); 
        continue; 
      }

      if ( baseframe == NULL )
      {
        LOG_WARNING("vtkDataCollectorSynchronizer: Unable to resize base frame for still frame finding if it's NULL - continue with next frame."); 
        baseIndex = baseIndex + 1; 
        currentIndex = baseIndex + this->GetNumberOfAveragedFrames(); 
        this->SetBaseFrame(NULL); 
        continue; 
      }

      vtkSmartPointer<vtkImageData> baseframeRGB = vtkSmartPointer<vtkImageData>::New(); 
      this->ConvertFrameToRGB(baseframe, baseframeRGB, 0.5); 
      baseframe->Delete(); 
      this->SetBaseFrame( baseframeRGB ); 
    }


    if ( this->VideoBuffer->GetVideoBufferItem(currentIndex, &videoItem) != ITEM_OK )
    {
      LOG_WARNING("vtkDataCollectorSynchronizer: Unable to get frame from video buffer (Uid: " << currentIndex << ")."); 
      baseIndex = baseIndex + 1; 
      currentIndex = baseIndex + this->GetNumberOfAveragedFrames(); 
      this->SetBaseFrame(NULL); 
      continue; 
    }

    vtkImageData* frame = vtkImageData::New(); 
    if ( this->CopyVideoFrame(frame, videoItem.GetFrame()) != PLUS_SUCCESS ) 
    {
      LOG_WARNING("vtkDataCollectorSynchronizer: Unable to copy video frame!"); 
      if ( frame != NULL )
      {
        frame->Delete(); 
        frame = NULL; 
      }
      baseIndex = baseIndex + 1; 
      currentIndex = baseIndex + this->GetNumberOfAveragedFrames(); 
      this->SetBaseFrame(NULL); 
      continue; 
    }

    if ( frame == NULL )
    {
      LOG_WARNING("vtkDataCollectorSynchronizer: Unable to resize actual frame for still frame finding if it's NULL - continue with next frame."); 
      baseIndex = baseIndex + 1; 
      currentIndex = baseIndex + this->GetNumberOfAveragedFrames(); 
      this->SetBaseFrame(NULL); 
      continue; 
    }

    // Convert the resampled frame to RGB 
    vtkImageData* frameRGB = vtkImageData::New(); 
    this->ConvertFrameToRGB(frame, frameRGB, 0.5); 

    // Compute frame differences
    double frameDifference = this->GetFrameDifference(frameRGB); 
    frameRGB->Delete(); 
    frame->Delete(); 

    //LOG_TRACE("FindStillFrame - baseIndex: " << std::fixed << baseIndex << "(timestamp: " << this->GetVideoBuffer()->GetTimeStamp(baseIndex) << ")  currentIndex: " << currentIndex << "   frameDifference: " << frameDifference); 
    if ( frameDifference < this->MaxFrameDifference )
    {
      currentIndex = currentIndex - 1; 

    }
    else
    {
      baseIndex = baseIndex + 1; 
      currentIndex = baseIndex + this->GetNumberOfAveragedFrames(); 
      this->SetBaseFrame(NULL); 
    }

    this->FindStillFrame(baseIndex, currentIndex); 
  }
}

//----------------------------------------------------------------------------
PlusStatus vtkDataCollectorSynchronizer::FindFrameTimestamp( BufferItemUidType& bufferIndex, double& movedFrameTimestamp, double nextMovedTimestamp )
{
  LOG_TRACE("vtkDataCollectorSynchronizer::FindFrameTimestamp"); 

  const double localTimeOffset(0); 
  VideoBufferItem videoItem; 
  if ( this->GetVideoBuffer()->GetVideoBufferItem(bufferIndex, &videoItem) == ITEM_OK )
  {
    LOG_DEBUG("Start to find next frame movement at: " << std::fixed << videoItem.GetTimestamp(localTimeOffset)); 
  }

  bool diffFound = false; 

  while ( !diffFound && bufferIndex <= this->VideoBuffer->GetLatestItemUidInBuffer() )
  {
    if ( this->ProgressBarUpdateCallbackFunction != NULL )
    {
      const int videosyncprogress = floor(100.0*(bufferIndex - this->TrackerBuffer->GetOldestItemUidInBuffer()) / (1.0 * this->GetVideoBuffer()->GetNumberOfItems())); 
      (*ProgressBarUpdateCallbackFunction)(videosyncprogress); 
    }

    if ( this->GetVideoBuffer()->GetVideoBufferItem(bufferIndex, &videoItem) != ITEM_OK )
    {	
      LOG_WARNING("vtkDataCollectorSynchronizer: Unable to get frame from frame UID: " << bufferIndex); 
      bufferIndex++; 
      continue; 
    }

    double frameTimestamp = videoItem.GetTimestamp(localTimeOffset); 

    // if the tracker moved again, the result is not reliable - return false
    if ( frameTimestamp >= nextMovedTimestamp )
    {
      LOG_DEBUG("    !!! Video motion timestamp is not reliable! We have reached the next tracker movement (" << std::fixed << nextMovedTimestamp << ")!"); 
      return PLUS_FAIL; 
    }

    unsigned long frameNumber = videoItem.GetIndex(); 

    vtkSmartPointer<vtkImageData> frame = vtkSmartPointer<vtkImageData>::New(); 
    if ( this->CopyVideoFrame(frame, videoItem.GetFrame() ) != PLUS_SUCCESS ) 
    {
      LOG_WARNING("vtkDataCollectorSynchronizer: Unable to get frame from video buffer!"); 
      bufferIndex++; 
      continue; 
    }

    if ( frame == NULL )
    {
      LOG_WARNING("vtkDataCollectorSynchronizer: Unable to resize frame for frame finding if it's NULL - continue with next frame."); 
      bufferIndex++; 
      continue; 
    }

    // Compute frame differences
    vtkSmartPointer<vtkImageData> frameRGB = vtkSmartPointer<vtkImageData>::New(); 
    this->ConvertFrameToRGB(frame, frameRGB, 0.5); 
    double frameDifference = this->GetFrameDifference(frameRGB); 

    LOG_DEBUG(std::fixed << "    FrameTimestamp: " << frameTimestamp << "  frameDifference: " << frameDifference << "  (threshold: " << this->FrameDifferenceMean + this->FrameDifferenceThreshold << ")"); 


    // create a new row for the sync report table 
    vtkSmartPointer<vtkVariantArray> rowSyncReportTable = vtkSmartPointer<vtkVariantArray>::New(); 

    // Insert NaN to the position timestamp and diff cols 
    rowSyncReportTable->InsertNextValue(vtkVariant("NaN")); 
    rowSyncReportTable->InsertNextValue(vtkVariant("NaN")); 

    std::ostringstream strTimestamp; 
    strTimestamp << std::fixed << frameTimestamp; 
    rowSyncReportTable->InsertNextValue(vtkVariant(strTimestamp.str())); 

    std::ostringstream strFrameDifference; 
    strFrameDifference << std::fixed << frameDifference; 
    rowSyncReportTable->InsertNextValue(vtkVariant(strFrameDifference.str())); 

    if ( this->SyncReportTable )
    {
      this->SyncReportTable->InsertNextRow(rowSyncReportTable); 
    }
    else
    {
      LOG_WARNING("Failed to add new row to sync report table - table is NULL!"); 
    }

    // if larger then threshold => frame moved
    if ( ! diffFound && abs(frameDifference - this->FrameDifferenceMean) >  this->FrameDifferenceThreshold )
    {
      LOG_DEBUG(std::fixed << "    ---> Video motion detected at: " << std::fixed << frameTimestamp); 
      movedFrameTimestamp = frameTimestamp; 
      diffFound = true; 
    }

    bufferIndex++; 
  }

  if (!diffFound)
  {
    LOG_ERROR("Not enough difference found in the data");
    return PLUS_FAIL;
  }
  return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
PlusStatus vtkDataCollectorSynchronizer::ComputeFrameThreshold( BufferItemUidType& bufferIndex )
{
  LOG_TRACE("vtkDataCollectorSynchronizer::ComputeFrameThreshold"); 
  // Compute frame average 
  int sizeOfAvgFrames(0); 
  VideoBufferItem videoItem; 
  const double localTimeOffset(0); 
  std::vector<double> avgFrames; 
  for ( bufferIndex; bufferIndex <= this->VideoBuffer->GetLatestItemUidInBuffer() && sizeOfAvgFrames != this->NumberOfAveragedFrames; bufferIndex++ )
  {
    if ( this->VideoBuffer->GetVideoBufferItem(bufferIndex, &videoItem) != ITEM_OK )
    {
      LOG_WARNING("vtkDataCollectorSynchronizer: Unable to get video item for frame threshold computation from frame UID: " << bufferIndex); 
      continue; 
    }

    double frameTimestamp = videoItem.GetTimestamp(localTimeOffset); 

    vtkSmartPointer<vtkImageData> frame = vtkSmartPointer<vtkImageData>::New(); 
    if ( this->CopyVideoFrame(frame, videoItem.GetFrame()) != PLUS_SUCCESS )
    {
      LOG_WARNING("vtkDataCollectorSynchronizer: Failed to copy frame from video buffer!"); 
      continue; 
    }

    if ( frame == NULL )
    {
      LOG_WARNING("vtkDataCollectorSynchronizer: Unable to resize actual frame for frame threshold computation if it's NULL - continue with next frame."); 
      continue; 
    }

    // Compute frame differences
    vtkSmartPointer<vtkImageData> frameRGB = vtkSmartPointer<vtkImageData>::New(); 
    this->ConvertFrameToRGB(frame, frameRGB, 0.5); 
    double frameDifference = this->GetFrameDifference(frameRGB); 
    avgFrames.push_back(frameDifference); 
    sizeOfAvgFrames++; 
  }

  const double numElements = avgFrames.size();

  double meanDifference(0); 
  for ( unsigned int i = 0; i < numElements; i++ )
  {
    meanDifference += avgFrames[i] / numElements; 
  }
  this->FrameDifferenceMean = meanDifference; 

  double deviationDifference = 0; 
  for ( unsigned int i = 0; i < numElements; i++ )
  {
    deviationDifference += pow(avgFrames[i] - this->FrameDifferenceMean, 2.0) / numElements; 
  }
  deviationDifference = sqrt(deviationDifference); 

  double threshold = ThresholdMultiplier * deviationDifference; 
  this->FrameDifferenceThreshold = ( threshold > this->MinFrameThreshold ? threshold : this->MinFrameThreshold ); 

  return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
double vtkDataCollectorSynchronizer::GetImageAcquisitionFrameRate(double& mean, double& deviation)
{
  LOG_TRACE("vtkDataCollectorSynchronizer::GetImageAcquisitionFrameRate"); 
  std::vector<double> frameTimestamps; 
  VideoBufferItem videoItem; 
  const double localTimeOffset(0); 

  for ( BufferItemUidType frameUid = this->GetVideoBuffer()->GetOldestItemUidInBuffer(); frameUid <= this->GetVideoBuffer()->GetLatestItemUidInBuffer(); ++frameUid )
  {
    if ( this->GetVideoBuffer()->GetVideoBufferItem(frameUid,&videoItem) != ITEM_OK )
    {
      LOG_WARNING("vtkDataCollectorSynchronizer: Unable to get video item for image acquisition computation from frame UID: " << frameUid); 
      continue; 
    }

    double timestamp = videoItem.GetTimestamp(localTimeOffset); 

    if ( timestamp > 0 ) 
    {
      frameTimestamps.push_back(timestamp); 
    }
  }

  const double numberOfFrames = ( 1.0 * frameTimestamps.size() ); 

  std::sort(frameTimestamps.begin(), frameTimestamps.end()); 

  mean = 0; 
  for (  int i = 0; i < numberOfFrames - 1; i++)
  {
    mean += abs(frameTimestamps[i] - frameTimestamps[i + 1]) / ( numberOfFrames - 1 ); 
  }

  deviation = 0; 
  for (  int i = 0; i < numberOfFrames - 1; i++)
  {
    deviation += pow( mean - abs(frameTimestamps[i] - frameTimestamps[i + 1]), 2.0 ) / ( numberOfFrames - 1 ); 
  }
  deviation = sqrt(deviation); 

  return this->GetVideoBuffer()->GetFrameRate(); 
}

//----------------------------------------------------------------------------
double vtkDataCollectorSynchronizer::GetPositionAcquisitionFrameRate(double& mean, double& deviation)
{
  LOG_TRACE("vtkDataCollectorSynchronizer::GetPositionAcquisitionFrameRate"); 
  const double localTimeOffset(0);
  std::vector<double> trackerTimestamps; 
  for ( BufferItemUidType bufferIndex = this->GetTrackerBuffer()->GetOldestItemUidInBuffer() ; bufferIndex <= this->GetTrackerBuffer()->GetLatestItemUidInBuffer(); ++bufferIndex )
  {
    TrackerBufferItem bufferItem; 
    if ( this->GetTrackerBuffer()->GetTrackerBufferItem(bufferIndex, &bufferItem, false ) != ITEM_OK )
    {
      LOG_ERROR("Failed to get tracker buffer item with UID: " << bufferIndex ); 
      continue; 
    }

    double timestamp = bufferItem.GetTimestamp(localTimeOffset); 

    if ( timestamp > 0 && bufferItem.GetStatus() == TR_OK )
    {
      trackerTimestamps.push_back(timestamp); 
    }
  }

  const double numberOfPositions = ( 1.0 * trackerTimestamps.size() ); 

  std::sort(trackerTimestamps.begin(), trackerTimestamps.end()); 

  mean = 0; 
  for (  int i = 0; i < numberOfPositions - 1; i++)
  {
    mean += abs(trackerTimestamps[i] - trackerTimestamps[i + 1]) / ( numberOfPositions - 1 ); 
  }

  deviation = 0; 
  for (  int i = 0; i < numberOfPositions - 1; i++)
  {
    deviation += pow( mean - abs(trackerTimestamps[i] - trackerTimestamps[i + 1]), 2.0 ) / ( numberOfPositions - 1 ); 
  }
  deviation = sqrt(deviation); 

  return this->GetTrackerBuffer()->GetFrameRate(); 
}

//----------------------------------------------------------------------------
double vtkDataCollectorSynchronizer::GetFrameDifference(vtkImageData* frame)
{
  LOG_TRACE("vtkDataCollectorSynchronizer::GetFrameDifference"); 
  vtkSmartPointer<vtkImageDifference> imgDiff = vtkSmartPointer<vtkImageDifference>::New(); 
  imgDiff->AveragingOn(); 
  imgDiff->AllowShiftOn(); 
  imgDiff->SetThreshold(10); 
  imgDiff->SetImage( this->GetBaseFrame() ); 
  imgDiff->SetInput( frame ); 
  imgDiff->Update(); 

  LOG_TRACE("FrameDifference: " << imgDiff->GetThresholdedError()); 
  return imgDiff->GetThresholdedError(); 
}


//----------------------------------------------------------------------------
void vtkDataCollectorSynchronizer::SaveFrameToFile(vtkImageData* frame, char* fileName)
{
  LOG_TRACE("vtkDataCollectorSynchronizer::SaveFrameToFile: " << fileName ); 
  vtkSmartPointer<vtkBMPWriter> writer = vtkSmartPointer<vtkBMPWriter>::New(); 
  writer->SetFileName(fileName); 
  writer->SetInput(frame); 
  writer->Update(); 
}

//----------------------------------------------------------------------------
PlusStatus vtkDataCollectorSynchronizer::GenerateSynchronizationReport( vtkHTMLGenerator* htmlReport, vtkGnuplotExecuter* plotter, const char* gnuplotScriptsFolder)
{
  LOG_TRACE("vtkDataCollectorSynchronizer::GenerateSynchronizationReport"); 
#ifdef PLUS_PRINT_SYNC_DEBUG_INFO 
  if ( !this->GetSynchronized() )
  {
    LOG_ERROR("Unable to generate synchronization report: synchronization not yet finished!"); 
    return PLUS_FAIL; 
  }

  if ( htmlReport == NULL || plotter == NULL )
  {
    LOG_ERROR("Caller should define HTML report generator and gnuplot plotter before report generation!"); 
    return PLUS_FAIL;
  }

  std::string plotSyncResultScript = gnuplotScriptsFolder + std::string("/PlotSyncResult.gnu"); 
  if ( !vtksys::SystemTools::FileExists( plotSyncResultScript.c_str(), true) )
  {
    LOG_ERROR("Unable to find gnuplot script at: " << plotSyncResultScript); 
    return PLUS_FAIL; 
  }

  std::string reportFile = vtksys::SystemTools::GetCurrentWorkingDirectory() + std::string("/SyncResult.txt"); 

  if ( vtkGnuplotExecuter::DumpTableToFileInGnuplotFormat( this->SyncReportTable, reportFile.c_str() ) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to write table to file in gnuplot format!"); 
    return PLUS_FAIL; 
  }

  if ( !vtksys::SystemTools::FileExists( reportFile.c_str(), true) )
  {
    LOG_ERROR("Unable to find synchronization report file at: " << reportFile); 
    return PLUS_FAIL; 
  }

  htmlReport->AddText("Video and Tracking Data Synchronization Analysis", vtkHTMLGenerator::H1); 
  double trackerMean(0), trackerDev(0); 
  double trackerFrameRate = this->GetPositionAcquisitionFrameRate(trackerMean, trackerDev); 
  std::ostringstream trackerSummary; 
  trackerSummary << "Tracker Sampling Period: " << 1000.0/trackerFrameRate << "ms <br/>" 
    << "Mean Position Acquisition Time: " << 1000*trackerMean << "ms <br/>" 
    << "Position Acquisition Deviation: " << 1000*trackerDev << "ms <br/>" ; 
  htmlReport->AddParagraph(trackerSummary.str().c_str()); 

  double videoMean(0), videoDev(0); 
  double videoFrameRate = this->GetImageAcquisitionFrameRate(videoMean, videoDev); 
  std::ostringstream videoSummary; 
  videoSummary << "Video Sampling Period: " << 1000.0/videoFrameRate << "ms <br/>" 
    << "Mean Video Acquisition Time: " << 1000*videoMean << "ms <br/>" 
    << "Video Acquisition Deviation: " << 1000*videoDev << "ms <br/>" ; 
  htmlReport->AddParagraph(videoSummary.str().c_str()); 

  plotter->ClearArguments(); 
  plotter->AddArgument("-e");
  std::ostringstream syncAnalysis; 
  syncAnalysis << "f='" << reportFile << "'; o='SyncResult';" << std::ends; 
  plotter->AddArgument(syncAnalysis.str().c_str()); 
  plotter->AddArgument(plotSyncResultScript.c_str());  
  if ( plotter->Execute() != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to run gnuplot executer!"); 
    return PLUS_FAIL; 
  }
  htmlReport->AddImage("SyncResult.jpg", "Video and Tracking Data Synchronization Analysis"); 

  htmlReport->AddHorizontalLine(); 
#endif

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkDataCollectorSynchronizer::ReadConfiguration(vtkXMLDataElement* synchronizationConfig)
{
  LOG_TRACE("vtkDataCollectorSynchronizer::ReadConfiguration"); 
  if ( synchronizationConfig == NULL )
  {
    LOG_ERROR("Unable to configure synchronizer! (XML data element is NULL)"); 
    return PLUS_FAIL; 
  }


  int synchronizationTimeLength = 0; 
  if ( synchronizationConfig->GetScalarAttribute("SynchronizationTimeLength", synchronizationTimeLength) )
  {
    this->SetSynchronizationTimeLength(synchronizationTimeLength); 
  }

  int minNumOfSyncSteps = 0; 
  if ( synchronizationConfig->GetScalarAttribute("MinNumOfSyncSteps", minNumOfSyncSteps) )
  {
    this->SetMinNumOfSyncSteps(minNumOfSyncSteps); 
  }

  int numberOfAveragedFrames = 0; 
  if ( synchronizationConfig->GetScalarAttribute("NumberOfAveragedFrames", numberOfAveragedFrames) )
  {
    this->SetNumberOfAveragedFrames(numberOfAveragedFrames); 
  }

  int numberOfAveragedTransforms = 0; 
  if ( synchronizationConfig->GetScalarAttribute("NumberOfAveragedTransforms", numberOfAveragedTransforms) )
  {
    this->SetNumberOfAveragedTransforms(numberOfAveragedTransforms); 
  }

  int thresholdMultiplier = 0; 
  if ( synchronizationConfig->GetScalarAttribute("ThresholdMultiplier", thresholdMultiplier) )
  {
    this->SetThresholdMultiplier(thresholdMultiplier); 
  }

  double minTransformThreshold = 0; 
  if ( synchronizationConfig->GetScalarAttribute("MinTransformThreshold", minTransformThreshold) )
  {
    this->SetMinTransformThreshold(minTransformThreshold); 
  }

  double maxTransformDifference = 0; 
  if ( synchronizationConfig->GetScalarAttribute("MaxTransformDifference", maxTransformDifference) )
  {
    this->SetMaxTransformDifference(maxTransformDifference); 
  }

  double minFrameThreshold = 0; 
  if ( synchronizationConfig->GetScalarAttribute("MinFrameThreshold", minFrameThreshold) )
  {
    this->SetMinFrameThreshold(minFrameThreshold); 
  }

  double maxFrameDifference = 0; 
  if ( synchronizationConfig->GetScalarAttribute("MaxFrameDifference", maxFrameDifference) )
  {
    this->SetMaxFrameDifference(maxFrameDifference); 
  }

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkDataCollectorSynchronizer::GetSyncReportTable(vtkTable* syncReportTable)
{
  if ( syncReportTable == NULL )
  {
    LOG_ERROR("Failed to get sync report - input table is NULL!"); 
    return PLUS_FAIL; 
  }

  if ( this->SyncReportTable == NULL )
  {
    LOG_ERROR("Failed to get sync report table - table is NULL!"); 
    return PLUS_FAIL; 
  }

  syncReportTable->DeepCopy(this->SyncReportTable); 

  return PLUS_SUCCESS; 
}

//-----------------------------------------------------------------------------
void vtkDataCollectorSynchronizer::InitSyncReportTable()
{
  if ( this->SyncReportTable )
  {
    // Delete table just to make sure the col's are well prepared
    this->SyncReportTable->Delete(); 
  }

  this->SyncReportTable = vtkTable::New(); 

  const char* colPositionTimestampName = "PositionTimestamp"; 
  vtkSmartPointer<vtkStringArray> colPositionTimestamp = vtkSmartPointer<vtkStringArray>::New(); 
  colPositionTimestamp->SetName(colPositionTimestampName); 
  this->SyncReportTable->AddColumn(colPositionTimestamp); 

  const char* colPositionDifferenceName = "PositionDifference"; 
  vtkSmartPointer<vtkStringArray> colPositionDifference = vtkSmartPointer<vtkStringArray>::New(); 
  colPositionDifference->SetName(colPositionDifferenceName); 
  this->SyncReportTable->AddColumn(colPositionDifference); 

  const char* colVideoTimestampName = "VideoTimestamp"; 
  vtkSmartPointer<vtkStringArray> colVideoTimestamp = vtkSmartPointer<vtkStringArray>::New(); 
  colVideoTimestamp->SetName(colVideoTimestampName); 
  this->SyncReportTable->AddColumn(colVideoTimestamp); 

  const char* colVideoDifferenceName = "VideoDifference"; 
  vtkSmartPointer<vtkStringArray> colVideoDifference = vtkSmartPointer<vtkStringArray>::New(); 
  colVideoDifference->SetName(colVideoDifferenceName); 
  this->SyncReportTable->AddColumn(colVideoDifference); 

}
