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

#include "vtkTrackedFrameList.h"
#include "TrackedFrame.h"


// May be used later--currently not used
struct TimestampedValueType
{
  std::vector<double> value;
  std::vector<double> timestamp;
};

/*!
  \class TemporalCalibration
  \brief Computes the lag--or time offset--of the US probe's tracker stream relativce to the US video stream.

  The inputted data--video and tracker--is assumed to be collected by a US probe imaging a planar object; furthermore,
  it is assumed that the probe is undergoing uni-dirctional periodic motion in the direction perpendicular to the
  plane's face (E.g. moving the probe in a repeating up-and-down fashion while imaging the bottom of a water bath).
  The inputted data is assumed to contain at least ?five? full periods (although the algorithm may work for fewer periods
  it has not been tested under these conditions. 

  \ingroup PlusLibCalibrationAlgorithm
*/

class TemporalCalibration
{
public:

  TemporalCalibration();

  /*! Sets sampling resolution [s]. Default is 0.001 seconds. TODO: Only allow up-sampling */  
  void setSamplingResolutionSec(double samplingResolutionSec); 

  /*! Sets the tracker frames; frames are assumed to be the raw-buffer tracker frames  */  
  void SetTrackerFrames(const vtkSmartPointer<vtkTrackedFrameList> trackerFrames);

  /*! Sets the US video frames; frames are assumed to be the raw-buffer video frames */  
  void SetUSVideoFrames(const vtkSmartPointer<vtkTrackedFrameList> USVideoFrames);

  /*! Sets the name of the transform to be used for tracking data. Default is "ProbeToReference" */  
  void SetTransformName(std::string transformName);

  /*! Sets the maximum allowable lag--or time offset--between the tracker and US video streams. Default is 2 seconds */  
  void SetMaximumVideoTrackerLagSec(double maxLagSec);

  /*! Returns the time [s] by which the tracker stream lags the video stream. 
  If the lag < 0, the tracker stream leads the video stream */  
  double GetTrackerLagSec();

  /*! TODO: Finish error checking et cetera */  
  PlusStatus Update(); 

private:
  bool m_TrackerLagUpToDate; // Stores whether the user has called Update(); will not return tracker lag until set to "true"
  bool m_NeverUpdated; // Has the user ever succsfully called Update();
  double m_SamplingResolutionSec; //  Resolution used for re-sampling [s] TODO: Add comment about upsampling
  vtkSmartPointer<vtkTrackedFrameList> m_TrackerFrames; 
  vtkSmartPointer<vtkTrackedFrameList> m_USVideoFrames; 
  std::vector<double> m_VideoPositionMetric; //  Position metric values for the video stream (i.e. detect. line positions)
  std::vector<double> m_VideoTimestamps; 
  std::vector<double> m_TrackerPositionMetric; 
  std::vector<double> m_TrackerTimestamps; 
  std::vector<double> m_ResampledTrackerPositionMetric; // Resampled tracker metric used for correlation
  std::vector<double> m_ResampledTrackerTimestamps; // Resampled tracker time stamps used for correlation
  std::vector<double> m_ResampledVideoPositionMetric; // Resampled video metric used for correlation
  std::vector<double> m_ResampledVideoTimestamps; // Resampled video time stamps used for correlation
  std::vector<double> m_CorrValues; // TODO: use TimestampedValueType for this
  double m_TrackerLagSec; // Time [s] that tracker lags video. If lag < 0, the tracker leads the video
  double m_MaxTrackerLagSec; // Maximum allowed tracker lag--if lag is greater, will exit computation
  std::string m_TransformName;

  
  void ResamplePositionMetrics();
  void InterpolatePositionMetric(std::vector<double> &originalTimestamps,
                                 std::vector<double> &resampledTimestamps,
                                 std::vector<double> &originalMetric,
                                 std::vector<double> &resampledPositionMetric);
  double LinearInterpolation(double resampledTimeValue, std::vector<double> &originalTimestamps, 
                             std::vector<double> &originalMetric, int lowerStraddleIndex, int upperStraddleIndex);
  void GetStraddleIndices(std::vector<double> &originalTimestamps, std::vector<double> &resampledTimestamps, 
                          std::vector<int> &lowerStraddleIndices, std::vector<int> &upperStraddleIndices);
  int FindLowerStraddleIndex(std::vector<double> &originalTimestamps, double resampledTimestamp,int currLowerStraddleIndex);
  int FindUpperStraddleIndex(std::vector<double> &originalTimestamps, double resampledTimestamp,int currLowerStraddleIndex);
  int FindFirstLowerStraddleIndex(std::vector<double> &originalTimestamps, double resampledTimestamp);
  int FindFirstUpperStraddleIndex(std::vector<double> &originalTimestamps, double resampledTimestamp);                       
  void ComputeTrackerLagSec();
  PlusStatus NormalizeMetric(std::vector<double> &metric);
  PlusStatus ComputeVideoPositionMetric();
  PlusStatus ComputeTrackerPositionMetric();
  void ComputeCrossCorrelationBetweenVideoAndTrackerMetrics();
  double ComputeCorrelationSumForGivenLagIndex(std::vector<double> &m_TrackerPositionMetric,
                                               std::vector<double> &m_VideoPositionMetric, int indexOffset);

  /* TODO: Switching to VTK table data structure */
  vtkSmartPointer<vtkTable> m_TrackerTable;
  vtkSmartPointer<vtkTable> m_VideoTable;
  vtkSmartPointer<vtkTable> m_TrackerTimestampedMetric;
  void NormalizeTableColumn(vtkSmartPointer<vtkTable> table, int column);

};


#endif // _TEMPORAL_CALIBRATION_H
