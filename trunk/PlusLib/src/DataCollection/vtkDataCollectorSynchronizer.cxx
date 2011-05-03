#include "PlusConfigure.h"
#include "vtkDataCollectorSynchronizer.h"
#include "vtkObjectFactory.h"
#include "vtkImageDifference.h"
#include "vtkImageExtractComponents.h"
#include "vtkSmartPointer.h"
#include "vtkImageImport.h"
#include "vtkBMPWriter.h"
#include "vtkVideoFrame2.h"
#include "vtkImageResample.h"
#include "vtksys/SystemTools.hxx"
#include "vtkGnuplotExecuter.h"
#include "vtkHTMLGenerator.h"

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
	this->MinNumOfSyncSteps = 5; 
	this->SynchronizationTimeLength = 10; 
	
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

#ifdef PLUS_PRINT_SYNC_DEBUG_INFO 
	this->DebugInfoStream.open ("SyncResult.txt", ios::out);
#endif

}

//----------------------------------------------------------------------------
vtkDataCollectorSynchronizer::~vtkDataCollectorSynchronizer()
{
	this->SetBaseFrame(NULL); 

	this->SetTrackerBuffer(NULL); 
	this->SetVideoBuffer(NULL); 

#ifdef PLUS_PRINT_SYNC_DEBUG_INFO
	this->DebugInfoStream.close(); 
#endif

}

//----------------------------------------------------------------------------
void vtkDataCollectorSynchronizer::PrintSelf(ostream& os, vtkIndent indent)
{
	this->Superclass::PrintSelf(os,indent);
} 

//----------------------------------------------------------------------------
void vtkDataCollectorSynchronizer::Synchronize()
{
	LOG_TRACE("vtkDataCollectorSynchronizer::Synchronize"); 

	if ( this->SyncStartTime == 0 )
	{
		this->SetSyncStartTime( this->TrackerBuffer->GetTimeStamp(this->TrackerBuffer->GetNumberOfItems() - 1) ); 
		LOG_DEBUG("Sync start time: " << this->SyncStartTime ); 
	}

#ifdef PLUS_PRINT_SYNC_DEBUG_INFO
	this->DebugInfoStream << "# SynchronizationStartTime:\t" << std::fixed << this->SyncStartTime << std::endl; 
	this->DebugInfoStream << "TransformTimestamp\tTransformDifference\tFrameTimestamp\tFrameDifference" << std::endl; 
#endif

	// Clear previous results 
	this->TransformTimestamp.clear(); 
	this->FrameTimestamp.clear(); 

	int trackerBufferIndex = this->TrackerBuffer->GetIndexFromTime( this->SyncStartTime ); 
	int videoBufferIndex = this->VideoBuffer->GetIndexFromTime( this->SyncStartTime ); 
	int stillPositionIndex = videoBufferIndex; 
	int syncStep(1); 
	
	std::vector<double> stillTransformTimestamps; 
	std::vector<double> movedTransformTimestamps; 

	const double videoFrameRate = this->VideoBuffer->GetFrameRate(); 
	const double stillFrameIntervall = 2.0*this->GetNumberOfAveragedFrames() / videoFrameRate; 

	while ( trackerBufferIndex >= 0 && stillPositionIndex >= 0)
	{
		LOG_TRACE("Tracker sync step: " << syncStep ); 
		const int percent = floor(100.0*(this->TrackerBuffer->GetBufferSize() - trackerBufferIndex) / (1.0 * this->TrackerBuffer->GetBufferSize())); 
		if ( this->ProgressBarUpdateCallbackFunction != NULL )
		{
			(*ProgressBarUpdateCallbackFunction)(percent); 
		}

		stillPositionIndex = trackerBufferIndex - floor(1.0*this->GetNumberOfAveragedTransforms()/2.0); 
		this->FindStillTransform(trackerBufferIndex, stillPositionIndex ); 

		if ( stillPositionIndex >= 0 )
		{
			double stillTransformTimestamp = this->GetTrackerBuffer()->GetTimeStamp(trackerBufferIndex); 
			this->ComputeTransformThreshold( trackerBufferIndex ); 

			double movedTransformTimestamp(0); 
			if ( this->FindTransformTimestamp( trackerBufferIndex, movedTransformTimestamp ) )
			{
				if ( movedTransformTimestamps.size() == 0 )
				{
					stillTransformTimestamps.push_back(stillTransformTimestamp); 
					movedTransformTimestamps.push_back(movedTransformTimestamp); 
				}
				else if ( movedTransformTimestamps.back() + stillFrameIntervall < movedTransformTimestamp )
				{
					stillTransformTimestamps.push_back(stillTransformTimestamp); 
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

	
	LOG_DEBUG("Still Frame Search Intervall: " << 1000*stillFrameIntervall << "ms"); 

	int videoSyncStep(0); 
	for ( unsigned int i = 0; i < movedTransformTimestamps.size() && videoBufferIndex >= 0; i++ )
	{
		LOG_TRACE("Video sync step: " << videoSyncStep ); 
		LOG_DEBUG(std::fixed << "Next moved transform timestamp: " << movedTransformTimestamps[i] ); 
		// Choose an initial timestamp to search for still images 
		double stillFrameTimestamp = movedTransformTimestamps[i] - stillFrameIntervall; 
		
		if ( this->FrameTimestamp.size() > 0 && stillFrameTimestamp < this->FrameTimestamp.back() )
		{
			// still frame timestamp cannot be less than the last moved frame timestamp 
			stillFrameTimestamp = this->FrameTimestamp.back() + 0.5; 
		}

		// Find the initial frame index 
		videoBufferIndex = this->VideoBuffer->GetIndexFromTime( stillFrameTimestamp );
		int stillFrameIndex = videoBufferIndex - this->GetNumberOfAveragedFrames(); 

		// Find the timestamp of next possible movement 
		double nextMovedTimestamp = movedTransformTimestamps[i] + stillFrameIntervall; 
		/*if ( i + 1 < movedTransformTimestamps.size() )
		{
			nextMovedTimestamp = movedTransformTimestamps[i + 1] - stillFrameIntervall; 
		}*/
			
		// Find the next still frame 
		this->SetBaseFrame(NULL); 
		this->FindStillFrame(videoBufferIndex, stillFrameIndex ); 
		stillFrameTimestamp = this->VideoBuffer->GetTimeStamp(videoBufferIndex); 
		LOG_DEBUG("Still frame timestamp: " << std::fixed << stillFrameTimestamp); 

		if ( videoBufferIndex > 0 ) 
		{
			// Set the baseframe of the image compare 
			vtkSmartPointer<vtkImageData> baseframe = vtkSmartPointer<vtkImageData>::New(); 
			this->GetFrameFromVideoBuffer(baseframe, videoBufferIndex); 

			// Make the image smaller
			vtkSmartPointer<vtkImageResample> resample = vtkSmartPointer<vtkImageResample>::New();
			resample->SetInput(baseframe); 
			resample->SetAxisMagnificationFactor(0, 0.5); 
			resample->SetAxisMagnificationFactor(1, 0.5); 
			resample->SetAxisMagnificationFactor(2, 0.5);

			vtkSmartPointer<vtkImageData> baseframeRGB = vtkSmartPointer<vtkImageData>::New(); 
			this->ConvertFrameToRGB(resample->GetOutput(), baseframeRGB); 
			this->SetBaseFrame( baseframeRGB ); 

			// Compute the frame threshold
			this->ComputeFrameThreshold( videoBufferIndex ); 

			double currentFrameTimestamp = this->VideoBuffer->GetTimeStamp(videoBufferIndex); 
			if ( currentFrameTimestamp > movedTransformTimestamps[i] )
			{
				LOG_DEBUG("Start frame timestamp is already over the moved transform timestamp (difference: " << 1000*(currentFrameTimestamp - movedTransformTimestamps[i]) << "ms)"); 
				continue; 
			}

			// Find the moved image timestamp 
			double movedFrameTimestamp(0); 
			if ( this->FindFrameTimestamp( videoBufferIndex, movedFrameTimestamp, nextMovedTimestamp ) )	
			{
				// Save the frame and transform timestamp 
				this->FrameTimestamp.push_back(movedFrameTimestamp); 
				this->TransformTimestamp.push_back(movedTransformTimestamps[i]); 

				LOG_DEBUG(">>>>>> Step " << videoSyncStep << " - Frame and transform moved!" ); 
				LOG_DEBUG("Frame timestamp: " << std::fixed << movedFrameTimestamp); 
				LOG_DEBUG("Transform timestamp: " << std::fixed << movedTransformTimestamps[i]); 
				LOG_DEBUG("Transform to frame difference: " << std::fixed << 1000*(movedTransformTimestamps[i] - movedFrameTimestamp) << "ms"); 
			}
			else
			{
				// The result is not reliable
				LOG_DEBUG(">>>>>> Step " << videoSyncStep << " - Result is not reliable!"); 
			}
		}
		videoSyncStep++; 
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
void vtkDataCollectorSynchronizer::ComputeTransformThreshold( int& bufferIndex )
{
	LOG_TRACE("vtkDataCollectorSynchronizer::ComputeTransformThreshold"); 
	int sizeOfAvgPositons(0); 
	std::vector< vtkSmartPointer<vtkTransform> > avgTransforms; 
	for ( bufferIndex; bufferIndex >= 0 && sizeOfAvgPositons != this->NumberOfAveragedTransforms; bufferIndex-- )
	{
		if ( (this->GetTrackerBuffer()->GetFlags(bufferIndex) & (TR_MISSING | TR_OUT_OF_VIEW | TR_REQ_TIMEOUT))  == 0 )
		{
			vtkSmartPointer<vtkMatrix4x4> matrix = vtkSmartPointer<vtkMatrix4x4>::New(); 
			this->GetTrackerBuffer()->GetMatrix(matrix, bufferIndex); 

			vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New(); 
			transform->SetMatrix( matrix ); 
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
}

//----------------------------------------------------------------------------
bool vtkDataCollectorSynchronizer::FindTransformTimestamp( int& bufferIndex, double& movedTransformTimestamp  )
{
	LOG_TRACE("vtkDataCollectorSynchronizer::FindTransformTimestamp"); 
	bool diffFound = false; 
	
	while ( !diffFound && bufferIndex >= 0 )
	{
		if ( (this->GetTrackerBuffer()->GetFlags(bufferIndex) & (TR_MISSING | TR_OUT_OF_VIEW | TR_REQ_TIMEOUT))  == 0 )
		{
			double timestamp = this->GetTrackerBuffer()->GetTimeStamp(bufferIndex); 
			unsigned long frameNumber = this->GetTrackerBuffer()->GetFrameNumber(bufferIndex); 
			unsigned long prevFrameNumber = this->GetTrackerBuffer()->GetFrameNumber(bufferIndex + 1); 
			unsigned long nextFrameNumber = this->GetTrackerBuffer()->GetFrameNumber(bufferIndex - 1); 

			vtkSmartPointer<vtkMatrix4x4> matrix = vtkSmartPointer<vtkMatrix4x4>::New(); 
			this->GetTrackerBuffer()->GetMatrix(matrix, bufferIndex); 

			vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New(); 
			transform->SetMatrix( matrix ); 
			
			if ( ! this->IsTransformBelowThreshold(transform, timestamp) )
			{
//				if ( nextFrameNumber - frameNumber > 1 || frameNumber - prevFrameNumber > 1 ) 
//				{
//					// we have missing frames, the result is not reliable
//#ifdef PLUS_PRINT_SYNC_DEBUG_INFO
//					this->DebugInfoStream << "# Final Transform Timestamp is not reliable! We have missing frames!" <<  std::endl; 
//#endif
//					LOG_DEBUG("# Final Transform Timestamp is not reliable! We have missing frames!"); 
//					//return false; 
//				}

#ifdef PLUS_PRINT_SYNC_DEBUG_INFO
				this->DebugInfoStream << "# Final TransformTimestamp:\t" << timestamp << std::endl; 
#endif
				movedTransformTimestamp = timestamp; 
				diffFound = true; 
			}
		}

		bufferIndex--; 
	}

	return diffFound; 

}

//----------------------------------------------------------------------------
void vtkDataCollectorSynchronizer::FindStillTransform( int& baseIndex, int& currentIndex )
{
	LOG_TRACE("vtkDataCollectorSynchronizer::FindStillTransform"); 
	while ( currentIndex >= 0 && baseIndex >= 0 && baseIndex != currentIndex )
	{
		vtkSmartPointer<vtkMatrix4x4> baseMatrix = vtkSmartPointer<vtkMatrix4x4>::New(); 
		if ( (this->GetTrackerBuffer()->GetFlags(baseIndex) & (TR_MISSING | TR_OUT_OF_VIEW | TR_REQ_TIMEOUT))  == 0 )
		{
			this->GetTrackerBuffer()->GetMatrix(baseMatrix, baseIndex); 
		}

		vtkSmartPointer<vtkMatrix4x4> currentMatrix = vtkSmartPointer<vtkMatrix4x4>::New(); 
		if ( (this->GetTrackerBuffer()->GetFlags(currentIndex) & (TR_MISSING | TR_OUT_OF_VIEW | TR_REQ_TIMEOUT))  == 0 )
		{
			this->GetTrackerBuffer()->GetMatrix(currentMatrix, currentIndex); 
		}

		if ( this->GetTranslationError(baseMatrix, currentMatrix) < this->MaxTransformDifference && this->GetRotationError(baseMatrix, currentMatrix) < this->MaxTransformDifference )
		{
			currentIndex = currentIndex + 1; 
			FindStillTransform(baseIndex, currentIndex); 
		}
		else
		{
			baseIndex = baseIndex - 1; 
			currentIndex = baseIndex - floor(1.0*this->GetNumberOfAveragedTransforms()/2.0); 
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

#ifdef PLUS_PRINT_SYNC_DEBUG_INFO
	double transformDifference = abs(currPosX - PositionTransformMean.Tx) + abs(currPosY - PositionTransformMean.Ty) + abs(currPosZ - PositionTransformMean.Tz); 
	// TransformTimestamp TransformDifference FrameTimestamp FrameDifference
	if ( timestamp > 0 )
	{
		this->DebugInfoStream << timestamp << "\t" << transformDifference << "\tNaN\tNaN" << std::endl; 
	}
#endif

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
void vtkDataCollectorSynchronizer::ConvertFrameToRGB( vtkImageData* pFrame, vtkImageData* pFrameRGB )
{
	LOG_TRACE("vtkDataCollectorSynchronizer::ConvertFrameToRGB"); 
	if ( pFrame->GetNumberOfScalarComponents() != 3 )
	{
		vtkSmartPointer<vtkImageExtractComponents> imageExtractor =  vtkSmartPointer<vtkImageExtractComponents>::New(); 
		imageExtractor->SetInput(pFrame); 
		// we are using only the 0th component
		imageExtractor->SetComponents(0,0,0);
		imageExtractor->Update(); 

		pFrameRGB->DeepCopy(imageExtractor->GetOutput()); 
		pFrameRGB->Update();
	}
	else 
	{
		pFrameRGB->DeepCopy(pFrame); 
		pFrameRGB->Update(); 
	}
}

//----------------------------------------------------------------------------
void vtkDataCollectorSynchronizer::GetFrameFromVideoBuffer( vtkImageData* frame, int bufferIndex )
{
	LOG_TRACE("vtkDataCollectorSynchronizer::GetFrameFromVideoBuffer"); 
	int* extent = this->GetVideoBuffer()->GetFrame(bufferIndex)->GetFrameExtent(); 
	int pixelFormat = this->GetVideoBuffer()->GetFrame(bufferIndex)->GetPixelFormat(); 
	frame->SetExtent( extent ); 
	frame->SetScalarTypeToUnsignedChar(); 
	frame->SetNumberOfScalarComponents(1); 
	frame->AllocateScalars(); 

	this->GetVideoBuffer()->GetFrame(bufferIndex)->CopyData( frame->GetScalarPointer() , extent, extent, pixelFormat); 
}

//----------------------------------------------------------------------------
void vtkDataCollectorSynchronizer::FindStillFrame( int& baseIndex, int& currentIndex )
{
	LOG_TRACE("vtkDataCollectorSynchronizer::FindStillFrame"); 
	while ( currentIndex >= 0 && baseIndex >= 0 && baseIndex != currentIndex )
	{
		
		const int videosyncprogress = floor(100.0*(this->GetVideoBuffer()->GetBufferSize() - baseIndex) / (1.0 * this->GetVideoBuffer()->GetBufferSize())); 
		if ( this->ProgressBarUpdateCallbackFunction != NULL )
		{
			(*ProgressBarUpdateCallbackFunction)(videosyncprogress); 
		}

		if ( this->GetBaseFrame() == NULL ) 
		{
			vtkImageData* baseframe = vtkImageData::New(); 
			this->GetFrameFromVideoBuffer(baseframe, baseIndex); 
			// Make the image smaller
			vtkImageResample* resample = vtkImageResample::New();
			resample->SetInput(baseframe); 
			resample->SetAxisMagnificationFactor(0, 0.5); 
			resample->SetAxisMagnificationFactor(1, 0.5); 
			resample->SetAxisMagnificationFactor(2, 0.5); 

			vtkSmartPointer<vtkImageData> baseframeRGB = vtkSmartPointer<vtkImageData>::New(); 
			this->ConvertFrameToRGB(resample->GetOutput(), baseframeRGB); 
			
			resample->Delete(); 
			baseframe->Delete(); 
			this->SetBaseFrame( baseframeRGB ); 
		}


		vtkImageData* frame = vtkImageData::New(); 
		this->GetFrameFromVideoBuffer(frame, currentIndex);

		// Make the image smaller
		vtkImageResample* resample = vtkImageResample::New();
		resample->SetInput(frame); 
		resample->SetAxisMagnificationFactor(0, 0.5); 
		resample->SetAxisMagnificationFactor(1, 0.5); 
		resample->SetAxisMagnificationFactor(2, 0.5);
		
		// Convert the resampled frame to RGB 
		vtkImageData* frameRGB = vtkImageData::New(); 
		this->ConvertFrameToRGB(resample->GetOutput(), frameRGB); 
		
		// Compute frame differences
		double frameDifference = this->GetFrameDifference(frameRGB); 
		resample->Delete(); 
		frameRGB->Delete(); 
		frame->Delete(); 
			
		LOG_TRACE("FindStillFrame - baseIndex: " << std::fixed << baseIndex << "(timestamp: " << this->GetVideoBuffer()->GetTimeStamp(baseIndex) << ")  currentIndex: " << currentIndex << "   frameDifference: " << frameDifference); 
		if ( frameDifference < this->MaxFrameDifference )
		{
			currentIndex = currentIndex + 1; 
			
		}
		else
		{
			baseIndex = baseIndex - 1; 
			currentIndex = baseIndex - this->GetNumberOfAveragedFrames(); 
			this->SetBaseFrame(NULL); 
		}

		this->FindStillFrame(baseIndex, currentIndex); 
	}
}

//----------------------------------------------------------------------------
bool vtkDataCollectorSynchronizer::FindFrameTimestamp( int& bufferIndex, double& movedFrameTimestamp, double nextMovedTimestamp )
{
	LOG_TRACE("vtkDataCollectorSynchronizer::FindFrameTimestamp"); 

	LOG_DEBUG("****Start to find next frame movement at: " << std::fixed << this->GetVideoBuffer()->GetTimeStamp(bufferIndex) ); 
	bool diffFound = false; 
	
	while ( !diffFound && bufferIndex > 0 )
	{
		const int videosyncprogress = floor(100.0*(this->GetVideoBuffer()->GetBufferSize() - bufferIndex) / (1.0 * this->GetVideoBuffer()->GetBufferSize())); 
		if ( this->ProgressBarUpdateCallbackFunction != NULL )
		{
			(*ProgressBarUpdateCallbackFunction)(videosyncprogress); 
		}

		double frameTimestamp = this->GetVideoBuffer()->GetTimeStamp(bufferIndex); 
		
		// if the tracker moved again, the result is not reliable - return false
		if ( frameTimestamp >= nextMovedTimestamp )
		{
			LOG_DEBUG("# FinalFrameTimestamp is not reliable! We have reached the next tracker movement ("<< std::fixed << nextMovedTimestamp << ")!"); 
#ifdef PLUS_PRINT_SYNC_DEBUG_INFO
			this->DebugInfoStream << "# FinalFrameTimestamp is not reliable! We have reached the next tracker movement!" <<  std::endl; 
#endif
			return false; 
		}

		unsigned long frameNumber = this->GetVideoBuffer()->GetFrameNumber(bufferIndex); 
		unsigned long prevFrameNumber = this->GetVideoBuffer()->GetFrameNumber(bufferIndex + 1); 
		unsigned long nextFrameNumber = this->GetVideoBuffer()->GetFrameNumber(bufferIndex - 1); 

		vtkSmartPointer<vtkImageData> frame = vtkSmartPointer<vtkImageData>::New(); 
		this->GetFrameFromVideoBuffer(frame, bufferIndex); 

		// Make the image smaller
		vtkSmartPointer<vtkImageResample> resample = vtkSmartPointer<vtkImageResample>::New();
		resample->SetInput(frame); 
		resample->SetAxisMagnificationFactor(0, 0.5); 
		resample->SetAxisMagnificationFactor(1, 0.5); 
		resample->SetAxisMagnificationFactor(2, 0.5);

		// Compute frame differences
		vtkSmartPointer<vtkImageData> frameRGB = vtkSmartPointer<vtkImageData>::New(); 
		this->ConvertFrameToRGB(resample->GetOutput(), frameRGB); 
		double frameDifference = this->GetFrameDifference(frameRGB); 

		LOG_DEBUG(std::fixed << "FrameTimestamp: " << frameTimestamp << "  frameDifference: " << frameDifference << "  (threshold: " << this->FrameDifferenceMean + this->FrameDifferenceThreshold << ")"); 
#ifdef PLUS_PRINT_SYNC_DEBUG_INFO
		// TransformTimestamp TransformDifference FrameTimestamp FrameDifference
		if ( frameTimestamp > 0 )
		{
			this->DebugInfoStream << "NaN\tNaN\t" << std::fixed << frameTimestamp << "\t" << frameDifference << std::endl; 
		}
#endif		

		// if larger then threshold => frame moved
		if ( ! diffFound && abs(frameDifference - this->FrameDifferenceMean) >  this->FrameDifferenceThreshold )
		{
			
//			if ( nextFrameNumber - frameNumber > 1 || frameNumber - prevFrameNumber > 1 ) 
//			{
//				// we have missing frames, the result is not reliable
//#ifdef PLUS_PRINT_SYNC_DEBUG_INFO
//				this->DebugInfoStream << "# FinalFrameTimestamp is not reliable! We have missing frames!" <<  std::endl; 
//#endif
//				LOG_DEBUG("# Final Frame Timestamp is not reliable! We have missing frames!"); 
//				//return false; 
//			}

			LOG_DEBUG(std::fixed << "FinalFrameTimestamp: " << frameTimestamp); 
#ifdef PLUS_PRINT_SYNC_DEBUG_INFO
			this->DebugInfoStream << "# FinalFrameTimestamp:\t" << frameTimestamp << std::endl; 
#endif
			movedFrameTimestamp = frameTimestamp; 
			diffFound = true; 
		}

		bufferIndex--; 
	}

	return diffFound;
}

//----------------------------------------------------------------------------
void vtkDataCollectorSynchronizer::ComputeFrameThreshold( int& bufferIndex )
{
	LOG_TRACE("vtkDataCollectorSynchronizer::ComputeFrameThreshold"); 
	// Compute frame average 
	int sizeOfAvgFrames(0); 
	std::vector<double> avgFrames; 
	for ( bufferIndex; bufferIndex >= 0 && sizeOfAvgFrames != this->NumberOfAveragedFrames; bufferIndex-- )
	{
		double frameTimestamp = this->GetVideoBuffer()->GetTimeStamp(bufferIndex); 

		vtkSmartPointer<vtkImageData> frame = vtkSmartPointer<vtkImageData>::New(); 
		this->GetFrameFromVideoBuffer(frame, bufferIndex); 

		// Make the image smaller
		vtkSmartPointer<vtkImageResample> resample = vtkSmartPointer<vtkImageResample>::New();
		resample->SetInput(frame); 
		resample->SetAxisMagnificationFactor(0, 0.5); 
		resample->SetAxisMagnificationFactor(1, 0.5); 
		resample->SetAxisMagnificationFactor(2, 0.5);

		// Compute frame differences
		vtkSmartPointer<vtkImageData> frameRGB = vtkSmartPointer<vtkImageData>::New(); 
		this->ConvertFrameToRGB(resample->GetOutput(), frameRGB); 
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
}

//----------------------------------------------------------------------------
double vtkDataCollectorSynchronizer::GetImageAcquisitionFrameRate(double& mean, double& deviation)
{
	LOG_TRACE("vtkDataCollectorSynchronizer::GetImageAcquisitionFrameRate"); 
	std::vector<double> frameTimestamps; 
	for ( int bufferIndex = 0; bufferIndex < this->GetVideoBuffer()->GetBufferSize(); bufferIndex++ )
	{
		double timestamp = this->GetVideoBuffer()->GetTimeStamp(bufferIndex); 
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
	std::vector<double> trackerTimestamps; 
	for ( int bufferIndex = 0; bufferIndex < this->GetTrackerBuffer()->GetBufferSize(); bufferIndex++ )
	{
		double timestamp = this->GetTrackerBuffer()->GetTimeStamp(bufferIndex); 

		if ( timestamp > 0 && (this->GetTrackerBuffer()->GetFlags(bufferIndex) & (TR_MISSING | TR_OUT_OF_VIEW | TR_REQ_TIMEOUT))  == 0 )
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
void vtkDataCollectorSynchronizer::GenerateSynchronizationReport( vtkHTMLGenerator* htmlReport, vtkGnuplotExecuter* plotter, const char* gnuplotScriptsFolder)
{
	LOG_TRACE("vtkDataCollectorSynchronizer::GenerateSynchronizationReport"); 
#ifdef PLUS_PRINT_SYNC_DEBUG_INFO 
	if ( !this->GetSynchronized() )
	{
		LOG_WARNING("Unable to generate synchronization report: synchronization not yet finished!"); 
		return; 
	}

	if ( htmlReport == NULL || plotter == NULL )
	{
		LOG_ERROR("Caller should define HTML report generator and gnuplot plotter before report generation!"); 
		return; 
	}

	std::string plotSyncResultScript = gnuplotScriptsFolder + std::string("/PlotSyncResult.gnu"); 
	if ( !vtksys::SystemTools::FileExists( plotSyncResultScript.c_str(), true) )
	{
		LOG_ERROR("Unable to find gnuplot script at: " << plotSyncResultScript); 
		return; 
	}
	
	std::string reportFile = vtksys::SystemTools::GetCurrentWorkingDirectory() + std::string("/SyncResult.txt"); 
	
	if ( !vtksys::SystemTools::FileExists( reportFile.c_str(), true) )
	{
		LOG_ERROR("Unable to find synchronization report file at: " << reportFile); 
		return; 
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
	plotter->Execute(); 
	htmlReport->AddImage("SyncResult.jpg", "Video and Tracking Data Synchronization Analysis"); 
	
	htmlReport->AddHorizontalLine(); 
#endif
}
