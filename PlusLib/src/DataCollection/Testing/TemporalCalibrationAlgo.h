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
  \brief Calculates ultrasound image spacing from phantom definition file TODO: update this
  \ingroup PlusLibCalibrationAlgorithm
*/
class TemporalCalibration
{
public:
  TemporalCalibration(std::string inputTrackerSequenceMetafile, 
    std::string inputUSImageSequenceMetafile,
    std::string outputFilepath, 
    double samplingResolutionSec);

  /*! Returns sampling resolution [s] */
  double getSamplingResolution(); 
  void setSamplingResolution(double samplingResolutionSec); //  Sets sampling resolution [s]
  std::string getInputTrackerSequenceMetafile(); // Get tracker filename
  std::string getInputUSImageSequenceMetafile(); // Get US video sequence filename
  std::string getOutputFilepath(); // Get output filename
  void CalculateTimeOffset();
  double getTimeOffset();
  void writeVideoMetric();
  void writeTrackerMetric();
  void writeResampledVideoMetric();
  void writeResampledTrackerMetric();
  void getPlotTables(vtkTable *trackerTableBefore, vtkTable *videoTableBefore, 
    vtkTable *trackerTableAfter, vtkTable *videoTableAfter);

private:
  PlusStatus readFiles();
  std::string inputTrackerSequenceMetafile_;
  std::string inputUSImageSequenceMetafile_;
  std::string outputFilepath_;
  double samplingResolutionSec_; //  Resolution used for re-sampling [seconds]
  vtkSmartPointer<vtkTrackedFrameList> trackerFrames_; //  Pointer to the tracker (pose) frames
  vtkSmartPointer<vtkTrackedFrameList> USVideoFrames_; // Pointer to the US image frames
  /* TimestampedValueType m_VideoPositionMetric; TODO: use this kind of format */
  std::vector<double> videoMetric_; //  Contains metric values for the video stream (i.e. detect. line values)
  std::vector<double> videoTimestamps_; // Contains timestamp data for image data stream
  std::vector<double> trackerMetric_; //  Contains the metric values for the tracker (i.e. pose) stream
  std::vector<double> trackerTimestamps_; // Contains timestamp data for tracker (i.e. pose) stream
  std::vector<double> resampledTrackerMetric_;
  std::vector<double> resampledTrackerTimestamps_;
  std::vector<double> resampledVideoMetric_;
  std::vector<double> resampledVideoTimestamps_;
  std::vector<double> corrValues_; // TODO: use TimestampedValueType for this
  double trackerLag_; // TODO: comment neg/pos; include unit in name
  const double maxVideoOffset_;//  Maximum anticipated time offset [seconds]
  vtkSmartPointer<vtkTable> trackerTable_;
  vtkSmartPointer<vtkTable> videoTable_;
  void createPlotTables(std::vector<double> &resampledTrackerTimestamps, 
    std::vector<double> &resampledTrackerMetric, 
    std::vector<double> &resampledVideoTimestamps, 
    std::vector<double> &resampledVideoMetric);


  void NormalizeMetric(std::vector<double> &metric);
  PlusStatus CalculateVideoMetric();
  PlusStatus CalculateTrackerMetric();
  void interpolateHelper(const std::vector<double> &originalMetric, std::vector<double> &interpolatedVector,
    std::vector<double> &interpolatedTimestamps, const std::vector<double> &originalTimestamps, double samplingResolutionSec);
  double linearInterpolation(double interpolatedTimestamp, const std::vector<double> &originalMetric, 
    const std::vector<double> &originalTimestamps, std::vector<int> &straddleIndices, double samplingResolutionSec);
  void interpolate();
  void xcorr();
  double computeCorrelation(std::vector<double> &trackerMetric_, std::vector<double> &videoMetric_, int indexOffset);
  void writeMetric(std::string &outputFilepath, std::string headerMessage, std::vector<double> &metricArr, std::vector<double> &timestampArr);
};


#endif // _TEMPORAL_CALIBRATION_H
