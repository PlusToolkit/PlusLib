/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#ifndef _TEMPORAL_CALIBRATION_H
#define _TEMPORAL_CALIBRATION_H

#include "PlusConfigure.h"

#include <iostream>
#include <time.h>

#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderWindow.h>
#include <vtkSmartPointer.h>
#include <vtkChartXY.h>
#include <vtkTable.h>
#include <vtkPlot.h>
#include <vtkIntArray.h>
#include <vtkDoubleArray.h>
#include <vtkContextView.h>
#include <vtkContextScene.h>
#include <vtkPen.h>
#include <vtkDoubleArray.h>
#include <vtkWindowToImageFilter.h>
#include <vtkMatrix4x4.h>
#include <vtksys/CommandLineArguments.hxx>

#include <itkHoughTransform2DLinesImageFilter.h>
#include <itkImageFileWriter.h>
#include <itkImageDuplicator.h>
#include <itkOtsuThresholdImageFilter.h>
#include <itkBinaryThresholdImageFilter.h>
#include <itkRescaleIntensityImageFilter.h>

#include "itkImageRegionIterator.h"
#include "itkLineIterator.h"

//#include "itkNearestNeighborInterpolateImageFunction.h"
#include "itkResampleImageFilter.h"
//#include "itkBinaryThinningImageFilter.h"
//#include "itkRescaleIntensityImageFilter.h"

#include "vtkTrackedFrameList.h"
#include "TrackedFrame.h"

#include <itkCastImageFilter.h>

/*!
  \class TemporalCalibration
  \brief Computes the time lag of the US probe's tracker stream relative to the US video stream.

  The inputted data--video and tracker--is assumed to be collected by a US probe imaging a planar object. Furthermore,
  it is assumed that the probe is undergoing uni-dirctional periodic motion in the direction perpendicular to the
  plane's face (E.g. moving the probe in a repeating up-and-down fashion while imaging the bottom of a water bath).
  The inputted data is assumed to contain at least five full periods (although the algorithm may work for fewer periods
  it has not been tested under these conditions). 

  \ingroup PlusLibCalibrationAlgorithm
*/


class TemporalCalibration
{



public:

  TemporalCalibration();

  /*! Sets sampling resolution [s]. Default is 0.001 seconds. */  
  void setSamplingResolutionSec(double samplingResolutionSec); 

  /*! Sets the tracker frames; frames are assumed to be the raw (not interpolated) tracker frames  */  
  void SetTrackerFrames(const vtkSmartPointer<vtkTrackedFrameList> trackerFrames);

  /*! Sets the US video frames; frames are assumed to be the video frames */  
  void SetVideoFrames(const vtkSmartPointer<vtkTrackedFrameList> videoFrames);

  /*! Sets the name of the transform to be used for tracking data. Default is "ProbeToReference" */  
  void SetProbeToReferenceTransformName(const std::string& probeToReferenceTransformName);

  /*! Sets the maximum allowable time lag between the corresponding tracker and video frames. Default is 2 seconds */  
  void SetMaximumVideoTrackerLagSec(double maxLagSec);

  /*! Compute the tracker lag */  
  PlusStatus Update(); 

  /*!
    Returns the computed time [s] by which the tracker stream lags the video stream. 
    If the lag < 0, the tracker stream leads the video stream.
    The computed lag corresponds to the time offset that minimizes the difference between the position
    metric that is computed from the tracker and the video: min( sum(|VideoPositionMetric(t)-TrackerPositionMetric(t+lag)|) )
  */  
  PlusStatus GetTrackerLagSec(double &lag);

  vtkSmartPointer<vtkTable> GetVideoPositionSignal();
  vtkSmartPointer<vtkTable> GetUncalibratedTrackerPositionSignal();
  vtkSmartPointer<vtkTable> GetCalibratedTrackerPositionSignal();


private:
  /*! Stores whether the user has called Update(); will not return tracker lag until set to "true" */
  bool m_TrackerLagUpToDate; 
 
  /*! Has the user ever succsfully called Update() */
  bool m_NeverUpdated;
  
  /*! Resolution used for re-sampling [s] TODO: Add comment about upsampling */
  double m_SamplingResolutionSec;
  vtkSmartPointer<vtkTrackedFrameList> m_TrackerFrames; 
  vtkSmartPointer<vtkTrackedFrameList> m_VideoFrames; 
  
  /*! Position metric values for the video stream (i.e. detect. line positions) */
  std::vector<double> m_VideoPositionMetric; 
  std::vector<double> m_VideoTimestamps; 
  std::vector<double> m_TrackerPositionMetric; 
  std::vector<double> m_TrackerTimestamps; 
  
  /*! Resampled tracker metric used for correlation */
  std::vector<double> m_ResampledTrackerPositionMetric; // 
 
  /*! Resampled tracker time stamps used for correlation */
  std::vector<double> m_ResampledTrackerTimestamps; 
  
  /*! Resampled video metric used for correlation */
  std::vector<double> m_ResampledVideoPositionMetric; 
 
  /*! Resampled video time stamps used for correlation */
  std::vector<double> m_ResampledVideoTimestamps;
  
  std::vector<double> m_CorrValues; // TODO: use TimestampedValueType for this
  
  /*! Time [s] that tracker lags video. If lag < 0, the tracker leads the video */
  double m_TrackerLagSec;
  
  /*! Maximum allowed tracker lag--if lag is greater, will exit computation */
  double m_MaxTrackerLagSec; 

  vtkSmartPointer<vtkTable> m_VideoPositionSignal;
  vtkSmartPointer<vtkTable> m_UncalibratedTrackerPositionSignal;
  vtkSmartPointer<vtkTable> m_CalibratedTrackerPositionSignal;

  std::string m_ProbeToReferenceTransformName;



  void plotIntArray(std::vector<int> intensityValues);
  void plotDoubleArray(std::vector<double> intensityValues);
  PlusStatus FindPeakStart(std::vector<int> &intensityProfile,int MaxFromLargestArea,
                           int startOfMaxArea, double &startOfPeak);
  PlusStatus FindLargestPeak(std::vector<int> &intensityProfile,int &MaxFromLargestArea,
                                                int &MaxFromLargestAreaIndex, int &startOfMaxArea);
  PlusStatus ComputeCenterOfGravity(std::vector<int> &intensityProfile, int startOfMaxArea, 
                                                         double &centerOfGravity);
  
  PlusStatus ResamplePositionMetrics();
  void InterpolatePositionMetric(const std::vector<double> &originalTimestamps,
                                 const std::vector<double> &resampledTimestamps,
                                 const std::vector<double> &originalMetric,
                                 std::vector<double> &resampledPositionMetric);
  double LinearInterpolation(double resampledTimeValue, const std::vector<double> &originalTimestamps, 
                             const std::vector<double> &originalMetric, int lowerStraddleIndex, int upperStraddleIndex);
  void GetStraddleIndices(const std::vector<double> &originalTimestamps, const std::vector<double> &resampledTimestamps, 
                          std::vector<int> &lowerStraddleIndices, std::vector<int> &upperStraddleIndices);
  int FindSubsequentLowerStraddleIndex(const std::vector<double> &originalTimestamps, double resampledTimestamp, int currLowerStraddleIndex);
  int FindSubsequentUpperStraddleIndex(const std::vector<double> &originalTimestamps, double resampledTimestamp, int currLowerStraddleIndex);
  int FindFirstLowerStraddleIndex(const std::vector<double> &originalTimestamps, double resampledTimestamp);
  int FindFirstUpperStraddleIndex(const std::vector<double> &originalTimestamps, double resampledTimestamp);                       
  void ComputeTrackerLagSec();
  PlusStatus NormalizeMetric(std::vector<double> &metric);
  PlusStatus ComputeVideoPositionMetric();
  PlusStatus ComputeTrackerPositionMetric();
  void ComputeCrossCorrelationBetweenVideoAndTrackerMetrics();
  double ComputeCorrelationSumForGivenLagIndex(const std::vector<double> &m_TrackerPositionMetric,
                                               const std::vector<double> &m_VideoPositionMetric, int indexOffset);

  PlusStatus ComputeLineParameters(std::vector<itk::Point<double,2>> &data, std::vector<double> &planeParameters);
  void ConstructTableSignal(std::vector<double> &x, std::vector<double> &y, vtkSmartPointer<vtkTable> table, double timeCorrection); 
  /* TODO: Switching to VTK table data structure, maybe just use the vtkDoubleArray instead std::vector */
  vtkSmartPointer<vtkTable> m_TrackerTable;
  vtkSmartPointer<vtkTable> m_VideoTable;
  vtkSmartPointer<vtkTable> m_TrackerTimestampedMetric;
  void NormalizeTableColumn(vtkSmartPointer<vtkTable> table, int column);

};


#endif // _TEMPORAL_CALIBRATION_H
