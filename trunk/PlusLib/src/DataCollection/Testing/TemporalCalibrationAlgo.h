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

struct TimestampedValueType
{
  std::vector<double> value;
  std::vector<double> timestamp;
};

/*!
  \class TemporalCalibration
  \brief Computes the lag--or time offset--of the US probe's tracker stream relativce to the US video stream.
  \ingroup PlusLibCalibrationAlgorithm
*/

class TemporalCalibration
{
public:
  TemporalCalibration();

  /*! Sets sampling resolution [s] */  
  void setSamplingResolutionSec(double samplingResolutionSec); 

  /*! Sets the tracker frames containing the position and orientation (pose) of the US probe */  
  void SetTrackerFrames(const vtkSmartPointer<vtkTrackedFrameList> trackerFrames);

  /*! Sets the US video frames */  
  void SetUSVideoFrames(const vtkSmartPointer<vtkTrackedFrameList> USVideoFrames);

  /*! Sets the name of the transform (e.g. "ProbeToReference") to be used for tracking data*/  
  void SetTransformName(std::string transformName);

  /*! Sets the maximum allowable lag--or time offset--between the tracker and US video streams */  
  void SetMaximumVideoTrackerLagSec(double maxLagSec);

  /*! Returns the time [s] by which the tracker stream lags the video stream. 
  If the lag < 0, the tracker stream leads the video stream */  
  double GetTrackerLagSec();

  /*! Calculates the time [s] by which the tracker stream lags the video stream. 
  If the lag < 0, the tracker stream leads the video stream */  
  double CalculateTrackerLagSec();



  PlusStatus Update(); 

  /*! INCOMPLETE */  
  void getPlotTables(vtkTable *trackerTableBefore, vtkTable *videoTableBefore, 
    vtkTable *trackerTableAfter, vtkTable *videoTableAfter);


private:
  double m_SamplingResolutionSec; //  Resolution used for re-sampling [s] TODO: Add comment about upsampling
  vtkSmartPointer<vtkTrackedFrameList> m_TrackerFrames; 
  vtkSmartPointer<vtkTrackedFrameList> m_USVideoFrames; 
  std::vector<double> m_VideoPositionMetric; //  Position metric values for the video stream (i.e. detect. line positions)
  std::vector<double> m_VideoTimestamps; 
  std::vector<double> m_TrackerPositionMetric; 
  std::vector<double> m_TrackerTimestamps; 
  std::vector<double> m_ResampledTrackerPositionMetric;
  std::vector<double> m_ResampledTrackerTimestamps;
  std::vector<double> m_ResampledVideoPositionMetric;
  std::vector<double> m_ResampledVideoTimestamps;
  std::vector<double> m_CorrValues; // TODO: use TimestampedValueType for this
  double m_TrackerLagSec; // Time [s] that tracker lags video. If lag < 0, the tracker leads the video
  double m_MaxTrackerLagSec; // Maximum allowed tracker lag--if lag is greater, will exit computation
  std::string m_TransformName;

  /* IN PROGRESS--Switching to VTK table data structure */
  vtkSmartPointer<vtkTable> m_TrackerTable;
  vtkSmartPointer<vtkTable> m_VideoTable;
  vtkSmartPointer<vtkTable> m_TrackerTimestampedMetric;
  void createPlotTables(std::vector<double> &resampledTrackerTimestamps, 
  std::vector<double> &resampledTrackerMetric, 
  std::vector<double> &resampledVideoTimestamps, 
  std::vector<double> &resampledVideoMetric);
  void NormalizeTableColumn(vtkSmartPointer<vtkTable> table, int column);

  
  void NormalizeMetric(std::vector<double> &metric);
  PlusStatus CalculateVideoMetric();
  PlusStatus CalculateTrackerPositionMetric();
  void interpolateHelper(const std::vector<double> &originalMetric, std::vector<double> &interpolatedVector,
                         std::vector<double> &interpolatedTimestamps, const std::vector<double> &originalTimestamps, 
                         double samplingResolutionSec);
  double linearInterpolation(double interpolatedTimestamp, const std::vector<double> &originalMetric, 
                             const std::vector<double> &originalTimestamps, std::vector<int> &straddleIndices, 
                             double samplingResolutionSec);
  void interpolate();
  void xcorr();
  double computeCorrelation(std::vector<double> &m_TrackerPositionMetric, std::vector<double> &m_VideoPositionMetric, int indexOffset);
};


#endif // _TEMPORAL_CALIBRATION_H
