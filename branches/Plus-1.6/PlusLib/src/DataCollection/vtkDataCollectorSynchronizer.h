/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/ 

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

/*!
  \class vtkDataCollectorSynchronizer 
  \brief Determines the time lag difference between tracker and image source based on motion detection.
  This method needs a stabilizer device that keeps the tools still between the moves (eg. a stepper).
  \ingroup PlusLibDataCollection
*/
class VTK_EXPORT vtkDataCollectorSynchronizer : public vtkObject
{
public:

	static vtkDataCollectorSynchronizer *New();
	vtkTypeRevisionMacro(vtkDataCollectorSynchronizer, vtkObject);
	virtual void PrintSelf(ostream& os, vtkIndent indent);

	/*! Read/write main configuration from/to xml data */
	virtual PlusStatus ReadConfiguration(vtkXMLDataElement* rootElement); 

	/*! Start the synchronization */ 
	virtual PlusStatus Synchronize(); 

	/*! Compute video offset mean, stdev, min, max after synchronization  */
	virtual void GetOffsetStatistics(double &meanVideoOffset, double &stdevVideoOffset, double &minVideoOffset, double &maxVideoOffset); 

	/*!
    Add generated html report from synchronization to the existing html report
    htmlReport and plotter arguments has to be defined by the caller function
  */
	virtual PlusStatus GenerateSynchronizationReport( vtkHTMLGenerator* htmlReport, vtkGnuplotExecuter* plotter); 
	
	/*! Set/get video buffer  */
	vtkSetObjectMacro(VideoBuffer, vtkVideoBuffer); 
	vtkGetObjectMacro(VideoBuffer, vtkVideoBuffer); 

	/*! Set/get tracker buffer  */
	vtkSetObjectMacro(TrackerBuffer, vtkTrackerBuffer); 
	vtkGetObjectMacro(TrackerBuffer, vtkTrackerBuffer); 

	/*! Set/get base frame to compare  */
	vtkSetObjectMacro(BaseFrame, vtkImageData); 
	vtkGetObjectMacro(BaseFrame, vtkImageData); 

	/*! Set/get video offset  */
	vtkSetMacro(VideoOffset, double); 
	vtkGetMacro(VideoOffset, double); 
	
	/*! Set/get tracker offset */
	vtkSetMacro(TrackerOffset, double);		
	vtkGetMacro(TrackerOffset, double); 
	
	/*! Set/get synchronization start time  */
	vtkSetMacro(SyncStartTime, double); 
	vtkGetMacro(SyncStartTime, double); 

	/*!
    Set/get the minimum number of synchronization movement
	  needs to perform to get reliable result  
	*/
	vtkSetMacro(MinNumOfSyncSteps, int); 
	vtkGetMacro(MinNumOfSyncSteps, int); 

	/*! Set/get syncronization finished flag  */
	vtkSetMacro(Synchronized, bool); 
	vtkGetMacro(Synchronized, bool);
	vtkBooleanMacro(Synchronized, bool); 
	
	/*!
    Set/get the threshold multiplier value
	  threshold =  ThresholdMultiplier * deviation  
	*/
	vtkSetMacro(ThresholdMultiplier, int); 
	vtkGetMacro(ThresholdMultiplier, int); 

	/*! Set/get the number of averaged frames needed to compute the frame threshold */
	vtkSetMacro(NumberOfAveragedFrames, int); 
	vtkGetMacro(NumberOfAveragedFrames, int);
	
	/*! Set/get the number of averaged transforms needed to compute the transform threshold */
	vtkSetMacro(NumberOfAveragedTransforms, int); 
	vtkGetMacro(NumberOfAveragedTransforms, int);

	/*! Set/get the minimum frame threshold */
	vtkSetMacro(MinFrameThreshold, double); 
	vtkGetMacro(MinFrameThreshold, double);

	/*! Set/get the maximum frame difference for still frame finding */
	vtkSetMacro(MaxFrameDifference, double); 
	vtkGetMacro(MaxFrameDifference, double);
		
	/*! Set/get the minimum transform threshold  */
	vtkSetMacro(MinTransformThreshold, double); 
	vtkGetMacro(MinTransformThreshold, double);
	
	/*! Set/get the maximum transform difference for still position finding */
	vtkSetMacro(MaxTransformDifference, double); 
	vtkGetMacro(MaxTransformDifference, double);
	
	/*! Get/Set the Synchronization Time Length in seconds */
	vtkSetMacro(SynchronizationTimeLength, double); 
	vtkGetMacro(SynchronizationTimeLength, double);

	/*! Set/get startup delay in sec to give some time to the buffers for proper initialization before sync  */
	vtkSetMacro(StartupDelaySec, double); 
	vtkGetMacro(StartupDelaySec, double);

  /*! Get the table report of the timestamped buffer  */
  PlusStatus GetSyncReportTable(vtkTable* syncReportTable); 

	typedef void (*ProgressBarUpdatePtr)(int percent);
  /*! Callback function for progress bar refreshing */
  void SetProgressBarUpdateCallbackFunction(ProgressBarUpdatePtr cb) { ProgressBarUpdateCallbackFunction = cb; } 
	
protected:

  /*! Structure containing the orientation and position of a transform */
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

  /*! Start tracker motion detection  */
  virtual PlusStatus DetectTrackerMotions(std::vector<double> &movedTransformTimestamps); 

  /*! Start video motion detection  */
  virtual PlusStatus DetectVideoMotions(const std::vector<double> &movedTransformTimestamps); 

  /*! Computes the frame threshold */
	virtual PlusStatus ComputeFrameThreshold( BufferItemUidType& bufferIndex ); 

  /*! Find the next available changed frame in the buffer */
	virtual PlusStatus FindFrameTimestamp( BufferItemUidType& bufferIndex, double& movedFrameTimestamp, double nextMovedTimestamp ); 

  /*! Search for still frame in the buffer */
 	virtual void FindStillFrame( BufferItemUidType& baseIndex, BufferItemUidType& currentIndex ); 

  /*! Computes transformation threshold */
	virtual PlusStatus ComputeTransformThreshold( BufferItemUidType& bufferIndex ); 

  /*! Find the next transform motion in the buffer */
	virtual PlusStatus FindTransformMotionTimestamp( BufferItemUidType& bufferIndex, double& movedTransformTimestamp ); 

  /*! Check if the transform motion is below the threshold */
	virtual bool IsTransformBelowThreshold( vtkTransform* transform, double timestamp); 

  /*! Search for still transform in the buffer */
	virtual void FindStillTransform( BufferItemUidType& baseIndex, BufferItemUidType& currentIndex ); 

  /*! Remove outliers from the computation */
	virtual void RemoveOutliers(); 

  /*! Convert grayscale images to RGB */
	virtual void ConvertFrameToRGB( vtkImageData* pFrame, vtkImageData* pFrameRGB, double resampleFactor ); 

  /*! Get the difference between two images */
	virtual double GetFrameDifference(vtkImageData* frame); 

  /*! Returns the image acquisition frame rate */
	virtual double GetImageAcquisitionFrameRate(double& mean, double& deviation); 

  /*! Returns the position acquisition frame rate */
	virtual double GetPositionAcquisitionFrameRate(double& mean, double& deviation); 

  /*! Initialze the synchronization report table */
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

	/*! Pointer to the progress bar update callback function  */
	ProgressBarUpdatePtr ProgressBarUpdateCallbackFunction; 

private:
	vtkDataCollectorSynchronizer(const vtkDataCollectorSynchronizer&);
	void operator=(const vtkDataCollectorSynchronizer&);  
}; 
#endif
