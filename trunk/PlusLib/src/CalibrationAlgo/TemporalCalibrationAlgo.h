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
#include "vtkPCAStatistics.h"
#include <vtkWindowToImageFilter.h>
#include <vtkRenderer.h> 
#include <vtkPNGWriter.h>
#include <vtkAxis.h>
#include <vtkDelimitedTextWriter.h>
#include <vtkPiecewiseFunction.h>

#include <itkHoughTransform2DLinesImageFilter.h>
#include <itkImageFileWriter.h>
#include <itkImageDuplicator.h>
#include <itkOtsuThresholdImageFilter.h>
#include <itkBinaryThresholdImageFilter.h>
#include <itkRescaleIntensityImageFilter.h>
#include "itkRGBPixel.h"
#include <itkImageFileWriter.h>

#include "itkImageRegionIterator.h"
#include "itkLineIterator.h"

//#include "itkNearestNeighborInterpolateImageFunction.h"
#include "itkResampleImageFilter.h"
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
  enum TEMPORAL_CALIBRATION_ERROR {
    TEMPORAL_CALIBRATION_ERROR_NONE,
    TEMPORAL_CALIBRATION_ERROR_RESULT_ABOVE_THRESHOLD,
    TEMPORAL_CALIBRATION_ERROR_INVALID_TRANSFORM_NAME,
    TEMPORAL_CALIBRATION_ERROR_NO_TIMESTAMPS,
    TEMPORAL_CALIBRATION_ERROR_UNABLE_NORMALIZE_METRIC,
    TEMPORAL_CALIBRATION_ERROR_CORRELATION_RESULT_EMPTY,
    TEMPORAL_CALIBRATION_ERROR_NO_VIDEO_DATA,
    TEMPORAL_CALIBRATION_ERROR_NOT_MF_ORIENTATION,
    TEMPORAL_CALIBRATION_ERROR_NO_FRAMES_IN_VIDEO_DATA,
    TEMPORAL_CALIBRATION_ERROR_NO_FRAMES_IN_ULTRASOUND_DATA,
    TEMPORAL_CALIBRATION_ERROR_SAMPLING_RESOLUTION_TOO_SMALL,
  };

  TemporalCalibration();

  /*! Sets sampling resolution [s]. Default is 0.001 seconds. */  
  void SetSamplingResolutionSec(double samplingResolutionSec); 

  /*! Sets the tracker frames; frames are assumed to be the raw (not interpolated) tracker frames  */  
  void SetTrackerFrames(vtkTrackedFrameList* trackerFrames);

  /*! Sets the US video frames; frames are assumed to be the video frames */  
  void SetVideoFrames(vtkTrackedFrameList* videoFrames);

  /*! Sets the name of the transform to be used for tracking data. Default is "ProbeToReference" */  
  void SetProbeToReferenceTransformName(const std::string& probeToReferenceTransformName);

  /*! Sets the maximum allowable time lag between the corresponding tracker and video frames. Default is 2 seconds */  
  void SetMaximumVideoTrackerLagSec(double maxLagSec);

  void SetSaveIntermediateImagesToOn(bool saveIntermediateImages);

  void SetIntermediateFilesOutputDirectory(std::string &outputDirectory);

  /*! Compute the tracker lag */  
  PlusStatus Update(TEMPORAL_CALIBRATION_ERROR &error); 

  /*!
    Returns the computed time [s] by which the tracker stream lags the video stream. 
    If the lag < 0, the tracker stream leads the video stream.
    The computed lag corresponds to the time offset that minimizes the difference between the position
    metric that is computed from the tracker and the video: min( sum(|VideoPositionMetric(t)-TrackerPositionMetric(t+lag)|) )
  */  
  PlusStatus GetTrackerLagSec(double &lag);
  
  /*!
    Returns the calibration error. If the error is large then the computed tracker lag is not reliable.
    TODO: determine typical acceptable ranges
  */
  PlusStatus GetCalibrationError(double &error);

  PlusStatus GetUncalibratedTrackerPositionSignal(vtkTable* unCalibratedvideoPositionSignal);
	PlusStatus GetCalibratedTrackerPositionSignal(vtkTable* calibratedvideoPositionSignal);
  PlusStatus GetVideoPositionSignal(vtkTable* TrackerPositionSignal);
  PlusStatus GetCorrelationSignal(vtkTable* correlationSignal);

  PlusStatus GetBestCorrelation(double &videoCorrelation);
  PlusStatus GetMaxCalibrationError(double &maxCalibrationError);

private:
  PlusStatus filterFrames();

  double m_MaxCalibrationError;

  std::vector<double> m_CalibrationErrorVector;

  /*! Stores whether the user has called Update(); will not return tracker lag until set to "true" */
  bool m_TrackerLagUpToDate; 
 
  /*! If "true" then images of intermediate steps (i.e. scanlines used, detected lines) are saved in local directory */
  bool m_SaveIntermediateImages;

  /*! Has the user ever succsfully called Update() */
  bool m_NeverUpdated;

  /*!Directory where the intermediate files are written to!*/
  std::string m_IntermediateFilesOutputDirectory;
  
  /*! Resolution used for re-sampling [s]*/
  double m_SamplingResolutionSec;
  vtkSmartPointer<vtkTrackedFrameList> m_TrackerFrames; 
  vtkSmartPointer<vtkTrackedFrameList> m_VideoFrames; 
  
  /*! Position metric values for the video stream (i.e. detect. line positions) */
  std::vector<double> m_VideoPositionMetric; 
  std::vector<double> m_VideoTimestamps; 
  std::vector<double> m_TrackerPositionMetric; 
  std::vector<double> m_TrackerTimestamps; 
  
  /*! The time-offsets used to compute the correlations */
  std::vector<double> m_CorrIndices;

	std::vector<double> m_CorrTimeOffsets;
	
	/*! The highest correlation value for the tested time-offsets */
	double m_BestCorrelationValue;

  /*! Resampled tracker metric used for correlation */
  std::vector<double> m_ResampledTrackerPositionMetric;
 
  /*! Resampled tracker time stamps used for correlation */
  std::vector<double> m_ResampledTrackerTimestamps; 
  
  /*! Resampled video metric used for correlation */
  std::vector<double> m_ResampledVideoPositionMetric; 
 
  /*! Resampled video time stamps used for correlation */
  std::vector<double> m_ResampledVideoTimestamps;

  double m_CommonRangeMin; 
  double m_CommonRangeMax;

	std::vector<double> m_NormalizationFactors;

	std::vector<double> m_FilteredTrackerTimestamps;
	std::vector<double> m_FilteredVideoTimestamps;

	std::vector<double> m_FilteredVideoPositionMetric;
	std::vector<double> m_FilteredTrackerPositionMetric;

	std::vector<double> m_SlidingSignalTimestamps;
  std::vector<double> m_SlidingSignalMetric;

	std::vector<double> m_NormalizedTrackerPositionMetric;
	std::vector<double> m_NormalizedTrackerTimestamps;
  std::vector<double> m_CorrValues; // TODO: use TimestampedValueType for this
  
  /*! Time [s] that tracker lags video. If lag < 0, the tracker leads the video */
  double m_TrackerLagSec;

  /*! Given index for the calculated best fit */
  double m_BestCorrelationLagIndex;
	double m_BestCorrelationTimeOffset;

  /*! The residual error after temporal calibration of the video and tracker signals */
  double m_CalibrationError;

  /*! Maximum allowed tracker lag--if lag is greater, will exit computation */
  double m_MaxTrackerLagSec; 

  std::string m_ProbeToReferenceTransformName;


  void plotIntArray(std::vector<int> intensityValues);
  void plotDoubleArray(std::vector<double> intensityValues);
  PlusStatus FindPeakStart(std::vector<int> &intensityProfile,int MaxFromLargestArea,
                           int startOfMaxArea, double &startOfPeak);
  PlusStatus FindLargestPeak(std::vector<int> &intensityProfile,int &MaxFromLargestArea,
                                                int &MaxFromLargestAreaIndex, int &startOfMaxArea);
  PlusStatus ComputeCenterOfGravity(std::vector<int> &intensityProfile, int startOfMaxArea, 
                                                         double &centerOfGravity);

	PlusStatus ResampleSignalLinearly(const std::vector<double>& templateSignalTimestamps,
																											 const std::vector<double>& origSignalTimestamps,
																											 const std::vector<double>& origSignalValues,
																											 std::vector<double>& resampledSignalValues);
  
  PlusStatus FilterPositionMetrics(TEMPORAL_CALIBRATION_ERROR &error);
  PlusStatus ComputeTrackerLagSec(TEMPORAL_CALIBRATION_ERROR &error);
	PlusStatus NormalizeMetricWindow2(std::vector<double> &signal, double &normalizationFactor);
  PlusStatus ComputeVideoPositionMetric(TEMPORAL_CALIBRATION_ERROR &error);
  PlusStatus ComputeTrackerPositionMetric(TEMPORAL_CALIBRATION_ERROR &error);
	void ComputeCorrelationBetweenVideoAndTrackerMetrics2();

	double ComputeCrossCorrelationSum(const std::vector<double> &signalA, const std::vector<double> &signalB);
	double ComputeSsd(const std::vector<double> &signalA, const std::vector<double> &signalB);
	double ComputeSad(const std::vector<double> &signalA, const std::vector<double> &signalB);

  PlusStatus ComputeLineParameters(std::vector<itk::Point<double,2> > &data, std::vector<double> &planeParameters);
  PlusStatus ConstructTableSignal(std::vector<double> &x, std::vector<double> &y, vtkTable* table, double timeCorrection); 
  
  /*! TODO */  
  PlusStatus InterpolatePositionMetrics(const std::vector<double> &originalTimestamps,
                                        const std::vector<double> &originalMetricValues,
                                        const std::vector<double> &resampledTimestamps,
                                        std::vector<double> &resampledPositionMetric,
                                        double midpoint, double sharpness);

	PlusStatus InterpolatePositionMetrics2(const vtkSmartPointer<vtkPiecewiseFunction>& trackerPositionPiecewiseSignal,
																				 const std::vector<double> &templateTimestamps,
																				 std::vector<double> &resampledPositionMetric);

	PlusStatus ResampleSignalLinearly2(const std::vector<double>& templateSignalTimestamps,
																		 const vtkSmartPointer<vtkPiecewiseFunction>& trackerPositionPiecewiseSignal,
																		 std::vector<double>& resampledSignalValues);

  /* TODO: Switching to VTK table data structure, maybe just use the vtkDoubleArray instead std::vector */
  vtkSmartPointer<vtkTable> m_TrackerTable;
  vtkSmartPointer<vtkTable> m_VideoTable;
  vtkSmartPointer<vtkTable> m_TrackerTimestampedMetric;

  /*! Normalization factor used for the tracker metric. Used for computing calibration error. */
  double m_BestCorrelationNormalizationFactor;
  /*! Normalization factor used for the video metric. Used for computing calibration error. */
  double m_VideoPositionMetricNormalizationFactor;

  void ComputePrincipalAxis(std::vector<itk::Point<double, 3> > &trackerPositions, 
                                               itk::Point<double,3> &principalAxis,  int numValidFrames);

};


#endif // _TEMPORAL_CALIBRATION_H
