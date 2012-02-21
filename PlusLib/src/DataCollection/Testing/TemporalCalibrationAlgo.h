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
  \brief Computes the time-offset between the tracker stream and the US video stream.
  \ingroup PlusLibCalibrationAlgorithm
*/

class TemporalCalibration
{
public:
  TemporalCalibration();

  /*! Returns sampling resolution [s] */ 
  void setSamplingResolutionSec(double samplingResolutionSec); //  Sets sampling resolution [s]
  void CalculateTimeOffset();
  double getTimeOffset();
  void getPlotTables(vtkTable *trackerTableBefore, vtkTable *videoTableBefore, 
    vtkTable *trackerTableAfter, vtkTable *videoTableAfter);
  void SetTrackerFrames(vtkSmartPointer<vtkTrackedFrameList> trackerFrames);
  void SetUSVideoFrames(vtkSmartPointer<vtkTrackedFrameList> USVideoFrames);
  void SetTransformName(std::string transformName);
  void SetMaximumVideoTrackerLagSec(double maxLagSec);


private:
  double m_SamplingResolutionSec; //  Resolution used for re-sampling [seconds]
  vtkSmartPointer<vtkTrackedFrameList> m_TrackerFrames; //  Pointer to the tracker (pose) frames
  vtkSmartPointer<vtkTrackedFrameList> m_USVideoFrames; // Pointer to the US image frames
  /* TimestampedValueType m_VideoPositionMetric; TODO: use this kind of format */
  std::vector<double> m_VideoMetric; //  Contains metric values for the video stream (i.e. detect. line values)
  std::vector<double> m_VideoTimestamps; // Contains timestamp data for image data stream
  std::vector<double> m_TrackerMetric; //  Contains the metric values for the tracker (i.e. pose) stream
  std::vector<double> m_TrackerTimestamps; // Contains timestamp data for tracker (i.e. pose) stream
  std::vector<double> m_ResampledTrackerMetric;
  std::vector<double> m_ResampledTrackerTimestamps;
  std::vector<double> m_ResampledVideoMetric;
  std::vector<double> m_ResampledVideoTimestamps;
  std::vector<double> m_CorrValues; // TODO: use TimestampedValueType for this
  double m_TrackerLag; // TODO: comment neg/pos; include unit in name
  double m_MaxTrackerLagSec;
  std::string m_TransformName;
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
  PlusStatus CalculateTrackerMetric();
  void interpolateHelper(const std::vector<double> &originalMetric, std::vector<double> &interpolatedVector,
    std::vector<double> &interpolatedTimestamps, const std::vector<double> &originalTimestamps, double samplingResolutionSec);
  double linearInterpolation(double interpolatedTimestamp, const std::vector<double> &originalMetric, 
    const std::vector<double> &originalTimestamps, std::vector<int> &straddleIndices, double samplingResolutionSec);
  void interpolate();
  void xcorr();
  double computeCorrelation(std::vector<double> &m_TrackerMetric, std::vector<double> &m_VideoMetric, int indexOffset);
};


#endif // _TEMPORAL_CALIBRATION_H
