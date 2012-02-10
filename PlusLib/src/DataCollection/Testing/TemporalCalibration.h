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

class TemporalCalibration
{
  public:
   TemporalCalibration(std::string inputTrackerSequenceMetafile, 
                       std::string inputUSImageSequenceMetafile,
                       std::string outputFilepath, 
                       double samplingResolutionSec); //  Constructor
    double getSamplingResolution(); //  Returns sampling resolution [s]
    void setSamplingResolution(double samplingResolutionSec); //  Sets sampling resolution [s]
    std::string getInputTrackerSequenceMetafile(); // Get tracker filename
    std::string getInputUSImageSequenceMetafile(); // Get US video sequence filename
    std::string getOutputFilepath(); // Get output filename
    void CalculateTimeOffset();
    double getTimeOffset();

    
  private:
    PlusStatus readFiles();
    std::string inputTrackerSequenceMetafile_;
    std::string inputUSImageSequenceMetafile_;
    std::string outputFilepath_;
    double samplingResolutionSec_; //  Resolution used for re-sampling [seconds]
    vtkSmartPointer<vtkTrackedFrameList> trackerFrames_; //  Pointer to the tracker (pose) frames
    vtkSmartPointer<vtkTrackedFrameList> USVideoFrames_; // Pointer to the US image frames
    std::vector<double> videoMetric_; //  Contains metric values for the video stream (i.e. detect. line values)
    std::vector<double> videoTimestamps_; // Contains timestamp data for image data stream
    std::vector<double> trackerMetric_; //  Contains the metric values for the tracker (i.e. pose) stream
    std::vector<double> trackerTimestamps_; // Contains timestamp data for tracker (i.e. pose) stream
    std::vector<double> resampledTrackerMetric_;
    std::vector<double> resampledTrackerTimestamps_;
    std::vector<double> resampledVideoMetric_;
    std::vector<double> resampledVideoTimestamps_;
    std::vector<double> corrValues_;
    double trackerLag_; 

    void NormalizeMetric(std::vector<double> &metric);
    PlusStatus CalculateVideoMetric();
    PlusStatus CalculateTrackerMetric();
      void interpolateHelper(std::vector<double> &originalMetric, std::vector<double> &interpolatedVector,
          std::vector<double> &interpolatedTimestamps, std::vector<double> &originalTimestamps, double samplingResolutionSec);
    double linearInterpolation(double interpolatedTimestamp, std::vector<double> &originalMetric, 
                           std::vector<double> &originalTimestamps, std::vector<int> &straddleIndices, double samplingResolutionSec);
   
    void interpolate();

    void xcorr();
    double computeCorrelation(std::vector<double> &trackerMetric_, std::vector<double> &videoMetric_, int indexOffset);
};


#endif // _TEMPORAL_CALIBRATION_H
