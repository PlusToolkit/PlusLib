#ifndef __vtkDataCollectorSynchronizer_h
#define __vtkDataCollectorSynchronizer_h

#include "vtkObject.h"
#include "vtkTransform.h"
#include "vtkMatrix4x4.h"
#include "vtkImageData.h"
#include "vtkVideoBuffer.h"
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
	virtual PlusStatus ReadConfiguration(vtkXMLDataElement* synchronizationConfig); 

	// Description:
	// Start the synchronization 
	virtual PlusStatus Synchronize(); 

	// Description:
	// Compute video offset mean, stdev, min, max after synchronization 
	virtual void GetOffsetStatistics(double &meanVideoOffset, double &stdevVideoOffset, double &minVideoOffset, double &maxVideoOffset); 

	// Description:
	// Add generated html report from synchronization to the existing html report
	// htmlReport and plotter arguments has to be defined by the caller function
	virtual PlusStatus GenerateSynchronizationReport( vtkHTMLGenerator* htmlReport, vtkGnuplotExecuter* plotter, const char* gnuplotScriptsFolder); 
	
	// Description:	
	// Set/get video buffer 
	vtkSetObjectMacro(VideoBuffer, vtkVideoBuffer); 
	vtkGetObjectMacro(VideoBuffer, vtkVideoBuffer); 

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

  // Description:
	// Set/get startup delay in sec to give some time to the buffers for proper initialization before sync 
	vtkSetMacro(StartupDelaySec, double); 
	vtkGetMacro(StartupDelaySec, double);

  // Description: 
  // Get the table report of the timestamped buffer 
  PlusStatus GetSyncReportTable(vtkTable* syncReportTable); 

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

  //! Description 
  // Start tracker motion detection 
  virtual PlusStatus DetectTrackerMotions(std::vector<double> &movedTransformTimestamps); 

  //! Description 
  // Start video motion detection 
  virtual PlusStatus DetectVideoMotions(const std::vector<double> &movedTransformTimestamps); 

	virtual PlusStatus ComputeFrameThreshold( BufferItemUidType& bufferIndex ); 
	virtual PlusStatus FindFrameTimestamp( BufferItemUidType& bufferIndex, double& movedFrameTimestamp, double nextMovedTimestamp ); 
    virtual PlusStatus CopyVideoFrame( vtkImageData* frame, VideoBufferItem::ImageType::Pointer& frameInBuffer); 
	virtual void FindStillFrame( BufferItemUidType& baseIndex, BufferItemUidType& currentIndex ); 

	virtual PlusStatus ComputeTransformThreshold( BufferItemUidType& bufferIndex ); 
	virtual PlusStatus FindTransformMotionTimestamp( BufferItemUidType& bufferIndex, double& movedTransformTimestamp ); 
	virtual bool IsTransformBelowThreshold( vtkTransform* transform, double timestamp); 
	virtual void FindStillTransform( BufferItemUidType& baseIndex, BufferItemUidType& currentIndex ); 

	virtual void RemoveOutliers(); 

	virtual void ConvertFrameToRGB( vtkImageData* pFrame, vtkImageData* pFrameRGB, double resampleFactor ); 
	virtual double GetFrameDifference(vtkImageData* frame); 
	virtual double GetImageAcquisitionFrameRate(double& mean, double& deviation); 
	virtual double GetPositionAcquisitionFrameRate(double& mean, double& deviation); 

	virtual void SaveFrameToFile(vtkImageData* frame, char* fileName); 

  virtual void InitSyncReportTable(); 

	vtkImageData* BaseFrame; 

	double SynchronizationTimeLength;

  double StillFrameTimeInterval;
  int StillFrameIndexInterval; 

	vtkVideoBuffer* VideoBuffer; 
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
  double StartupDelaySec; 

	int MinNumOfSyncSteps; 
	
  vtkTable* SyncReportTable; 

	//! Pointer to the progress bar update callback function 
	ProgressBarUpdatePtr ProgressBarUpdateCallbackFunction; 

private:
	vtkDataCollectorSynchronizer(const vtkDataCollectorSynchronizer&);
	void operator=(const vtkDataCollectorSynchronizer&);  
}; 
#endif