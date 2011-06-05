#ifndef __vtkDataCollectorSynchronizer_h
#define __vtkDataCollectorSynchronizer_h

#include "vtkObject.h"
#include "vtkTransform.h"
#include "vtkMatrix4x4.h"
#include "vtkImageData.h"
#include "vtkVideoBuffer2.h"
#include "vtkTrackerBuffer.h"
#include "vtkXMLDataElement.h"
#include <vector>

class vtkHTMLGenerator; 
class vtkGnuplotExecuter; 

class VTK_EXPORT vtkDataCollectorSynchronizer : public vtkObject
{
public:
	static vtkDataCollectorSynchronizer *New();
	vtkTypeRevisionMacro(vtkDataCollectorSynchronizer, vtkObject);
	virtual void PrintSelf(ostream& os, vtkIndent indent);

	// Description:
	// Read/write main configuration from/to xml data
	virtual void ReadConfiguration(vtkXMLDataElement* synchronizationConfig); 

	// Description:
	// Start the synchronization 
	virtual void Synchronize(); 

	// Description:
	// Compute video offset mean, stdev, min, max after synchronization 
	virtual void GetOffsetStatistics(double &meanVideoOffset, double &stdevVideoOffset, double &minVideoOffset, double &maxVideoOffset); 

	// Description:
	// Add generated html report from synchronization to the existing html report
	// htmlReport and plotter arguments has to be defined by the caller function
	// Solution should build with PLUS_PRINT_SYNC_DEBUG_INFO to generate this report
	virtual void GenerateSynchronizationReport( vtkHTMLGenerator* htmlReport, vtkGnuplotExecuter* plotter, const char* gnuplotScriptsFolder); 
	
	// Description:	
	// Set/get video buffer 
	vtkSetObjectMacro(VideoBuffer, vtkVideoBuffer2); 
	vtkGetObjectMacro(VideoBuffer, vtkVideoBuffer2); 

	// Description:	
	// Set/get tracker buffer 
	vtkSetObjectMacro(TrackerBuffer, vtkTrackerBuffer); 
	vtkGetObjectMacro(TrackerBuffer, vtkTrackerBuffer); 

	// Description:	
	// Set/get base frame to compare 
	vtkSetObjectMacro(BaseFrame, vtkImageData); 
	vtkGetObjectMacro(BaseFrame, vtkImageData); 

	// Description:	
	// Set/get video offset 
	vtkSetMacro(VideoOffset, double); 
	vtkGetMacro(VideoOffset, double); 
	
	// Description:	
	// Set/get tracker offset
	vtkSetMacro(TrackerOffset, double);		
	vtkGetMacro(TrackerOffset, double); 
	
	// Description:	
	// Set/get synchronization start time 
	vtkSetMacro(SyncStartTime, double); 
	vtkGetMacro(SyncStartTime, double); 

	// Description:	
	// Set/get the minimum number of synchronization movement 
	// needs to perform to get reliable result 
	vtkSetMacro(MinNumOfSyncSteps, int); 
	vtkGetMacro(MinNumOfSyncSteps, int); 

	// Description:	
	// Set/get syncronization finished flag 
	vtkSetMacro(Synchronized, bool); 
	vtkGetMacro(Synchronized, bool);
	vtkBooleanMacro(Synchronized, bool); 
	
	// Description:	
	// Set/get the threshold multiplier value
	// threshold =  ThresholdMultiplier * deviation 
	vtkSetMacro(ThresholdMultiplier, int); 
	vtkGetMacro(ThresholdMultiplier, int); 

	// Description:	
	// Set/get the number of averaged frames needed to compute the frame threshold
	vtkSetMacro(NumberOfAveragedFrames, int); 
	vtkGetMacro(NumberOfAveragedFrames, int);
	
	// Description:	
	// Set/get the number of averaged transforms needed to compute the transform threshold
	vtkSetMacro(NumberOfAveragedTransforms, int); 
	vtkGetMacro(NumberOfAveragedTransforms, int);

	// Description:	
	// Set/get the minimum frame threshold
	vtkSetMacro(MinFrameThreshold, double); 
	vtkGetMacro(MinFrameThreshold, double);

	// Description:	
	// Set/get the maximum frame difference for still frame finding
	vtkSetMacro(MaxFrameDifference, double); 
	vtkGetMacro(MaxFrameDifference, double);
		
	// Description:	
	// Set/get the minimum transform threshold 
	vtkSetMacro(MinTransformThreshold, double); 
	vtkGetMacro(MinTransformThreshold, double);
	
	// Description:	
	// Set/get the maximum transform difference for still position finding
	vtkSetMacro(MaxTransformDifference, double); 
	vtkGetMacro(MaxTransformDifference, double);
	
	// Description:	
	// Get/Set the Synchronization Time Length in seconds
	vtkSetMacro(SynchronizationTimeLength, double); 
	vtkGetMacro(SynchronizationTimeLength, double);

	//! Description 
	// Callback function for progress bar refreshing
	typedef void (*ProgressBarUpdatePtr)(int percent);
    void SetProgressBarUpdateCallbackFunction(ProgressBarUpdatePtr cb) { ProgressBarUpdateCallbackFunction = cb; } 
	
protected:

	struct PositionContainer
	{
		double Rx; 
		double Ry; 
		double Rz; 
		double Tx; 
		double Ty; 
		double Tz; 
	}; 

	vtkDataCollectorSynchronizer();
	virtual ~vtkDataCollectorSynchronizer();

	virtual void ComputeFrameThreshold( vtkVideoBuffer2::FrameUidType& bufferIndex ); 
	virtual bool FindFrameTimestamp( vtkVideoBuffer2::FrameUidType& bufferIndex, double& movedFrameTimestamp, double nextMovedTimestamp ); 
	virtual bool GetFrameFromVideoBuffer( vtkImageData* frame, vtkVideoBuffer2::FrameUidType bufferIndex ); 
	virtual void FindStillFrame( vtkVideoBuffer2::FrameUidType& baseIndex, vtkVideoBuffer2::FrameUidType& currentIndex ); 

	virtual void ComputeTransformThreshold( int& bufferIndex ); 
	virtual bool FindTransformTimestamp( int& bufferIndex, double& movedTransformTimestamp ); 
	virtual bool IsTransformBelowThreshold( vtkTransform* transform, double timestamp); 
	virtual void FindStillTransform( int& baseIndex, int& currentIndex ); 
	virtual double GetRotationError(vtkMatrix4x4* baseTransMatrix, vtkMatrix4x4* currentTransMatrix); 
	virtual double GetTranslationError(vtkMatrix4x4* baseTransMatrix, vtkMatrix4x4* currentTransMatrix); 

	virtual void RemoveOutliers(); 

	virtual void ConvertFrameToRGB( vtkImageData* pFrame, vtkImageData* pFrameRGB ); 
	virtual double GetFrameDifference(vtkImageData* frame); 
	virtual double GetImageAcquisitionFrameRate(double& mean, double& deviation); 
	virtual double GetPositionAcquisitionFrameRate(double& mean, double& deviation); 

	virtual void SaveFrameToFile(vtkImageData* frame, char* fileName); 

	vtkImageData* BaseFrame; 

	double SynchronizationTimeLength;

	vtkVideoBuffer2* VideoBuffer; 
	int CurrentVideoBufferIndex; 
	double FrameDifferenceMean; 
	double FrameDifferenceThreshold; 
	int NumberOfAveragedFrames; 
	double MinFrameThreshold; 
	double MaxFrameDifference; 
	double VideoOffset; 

	vtkTrackerBuffer* TrackerBuffer; 
	int CurrentTrackerBufferIndex; 
	PositionContainer PositionTransformMean;
	PositionContainer PositionTransformThreshold;
	int NumberOfAveragedTransforms; 
	double MinTransformThreshold; 
	double MaxTransformDifference; 
	double TrackerOffset; 
	
	std::vector<double> FrameTimestamp; 
	std::vector<double> TransformTimestamp; 

	int ThresholdMultiplier; 

	bool Synchronized; 
	double SyncStartTime; 

	int MinNumOfSyncSteps; 
	
	std::ofstream DebugInfoStream;

	//! Pointer to the progress bar update callback function 
	ProgressBarUpdatePtr ProgressBarUpdateCallbackFunction; 

private:
	vtkDataCollectorSynchronizer(const vtkDataCollectorSynchronizer&);
	void operator=(const vtkDataCollectorSynchronizer&);  
}; 
#endif