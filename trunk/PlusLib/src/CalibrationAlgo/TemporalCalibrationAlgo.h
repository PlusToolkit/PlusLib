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

#include <vtkTable.h>
#include <vtkPiecewiseFunction.h>

#include "vtkTrackedFrameList.h"
#include "TrackedFrame.h"

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
  PlusStatus SetTrackerFrames(vtkTrackedFrameList* trackerFrames, const std::string &probeToReferenceTransformName);

  /*! Sets the US video frames; frames are assumed to be the video frames */  
  PlusStatus SetVideoFrames(vtkTrackedFrameList* videoFrames);

  /*! Sets the maximum allowable time lag between the corresponding tracker and video frames. Default is 2 seconds */  
  void SetMaximumVideoTrackerLagSec(double maxLagSec);

  /*! Enable/disable saving of intermediate images for debugging */
  void SetSaveIntermediateImages(bool saveIntermediateImages);

  void SetIntermediateFilesOutputDirectory(const std::string &outputDirectory);

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
  PlusStatus GetCorrelationSignalFine(vtkTable* correlationSignal);

  PlusStatus GetBestCorrelation(double &videoCorrelation);
  PlusStatus GetMaxCalibrationError(double &maxCalibrationError);

private:
  PlusStatus ComputeMovingSignalLagSec();
  PlusStatus ComputeCommonTimeRange();
  PlusStatus GetSignalRange(const std::deque<double> &signal, double &minValue, double &maxValue);
  PlusStatus VerifyTrackerInput(vtkTrackedFrameList *trackerFrames, TEMPORAL_CALIBRATION_ERROR &error);

  double m_MaxCalibrationError;

  std::deque<double> m_CalibrationErrorVector;

  /*! Stores whether the user has called Update(); will not return tracker lag until set to "true" */
  bool m_TrackerLagUpToDate; 
 
  /*! If "true" then images of intermediate steps are saved */
  bool m_SaveIntermediateImages;

  /*! Has the user ever succsfully called Update() */
  bool m_NeverUpdated;

  /*! Directory where the intermediate files are written to */
  std::string m_IntermediateFilesOutputDirectory;
  
  /*! Resolution used for re-sampling [s]*/
  double m_SamplingResolutionSec;
  
  /*! Fixed signal metric values. Typically the video is used as fixed signal. */
  std::deque<double> m_FixedSignalValues; 
  /*! Fixed signal timestamps. Typically the video is used as fixed signal. */
  std::deque<double> m_FixedSignalTimestamps; 

  std::deque<double> m_MovingSignalValues; 
  std::deque<double> m_MovingSignalTimestamps; 
  
  /*! The computed signal correlation values (corresponding to the better sign convention) */
  std::deque<double> m_CorrValues;

  /*! The time-offsets used to compute the correlations */
	std::deque<double> m_CorrTimeOffsets;

  /*! The computed signal correlation values (corresponding to the better sign convention, in the second phase with fine resolution) */
  std::deque<double> m_CorrValuesFine;

  /*! The time-offsets used to compute the correlations (in the second phase with fine resolution) */
	std::deque<double> m_CorrTimeOffsetsFine;
	
	/*! The highest correlation value for the tested time-offsets */
	double m_BestCorrelationValue;

  /*! Resampled tracker metric used for correlation */
  std::deque<double> m_ResampledTrackerPositionMetric;
 
  /*! Resampled tracker time stamps used for correlation */
  std::deque<double> m_ResampledTrackerTimestamps; 
  
  /*! Resampled video metric used for correlation */
  std::deque<double> m_ResampledVideoPositionMetric; 
 
  /*! Resampled video time stamps used for correlation */
  std::deque<double> m_ResampledVideoTimestamps;

  double m_CommonRangeMin; 
  double m_CommonRangeMax;

	std::deque<double> m_NormalizationFactors;
	std::deque<double> m_NormalizedTrackerPositionMetric;
	std::deque<double> m_NormalizedTrackerTimestamps;
  
  /*! Time [s] that tracker lags video. If lag < 0, the tracker leads the video */
  double m_TrackerLagSec;

  /*! Given index for the calculated best fit */
  double m_BestCorrelationLagIndex;
	double m_BestCorrelationTimeOffset;

  /*! The residual error after temporal calibration of the video and tracker signals */
  double m_CalibrationError;

  /*! Maximum allowed tracker lag--if lag is greater, will exit computation */
  double m_MaxMovingLagSec;  
   
  PlusStatus CropSignalsToCommonRange();
  PlusStatus ComputeTrackerLagSec(TEMPORAL_CALIBRATION_ERROR &error);  
  PlusStatus NormalizeMetricValues(std::deque<double> &signal, double &normalizationFactor, int startIndex=0, int stopIndex=-1);
  PlusStatus NormalizeMetricValues(std::deque<double> &signal, double &normalizationFactor, double startTime, double stopTime, const std::deque<double> &timestamps);
  PlusStatus ComputeVideoPositionMetric(TEMPORAL_CALIBRATION_ERROR &error);
  PlusStatus ComputeTrackerPositionMetric(TEMPORAL_CALIBRATION_ERROR &error);
	void ComputeCorrelationBetweenVideoAndTrackerMetrics(double minTrackerLagSec, double maxTrackerLagSec, double stepSizeSec, double &bestCorrelationValue, double &bestCorrelationTimeOffset, double &bestCorrelationNormalizationFactor, std::deque<double> &corrTimeOffsets, std::deque<double> &corrValues);

  double ComputeAlignmentMetric(const std::deque<double> &signalA, const std::deque<double> &signalB);

  PlusStatus ComputeLineParameters(std::vector<itk::Point<double,2> > &data, std::vector<double> &planeParameters);
  PlusStatus ConstructTableSignal(std::deque<double> &x, std::deque<double> &y, vtkTable* table, double timeCorrection); 

	PlusStatus ResampleSignalLinearly(const std::deque<double>& templateSignalTimestamps, const vtkSmartPointer<vtkPiecewiseFunction>& signalFunction, std::deque<double>& resampledSignalValues);

  /*! Normalization factor used for the tracker metric. Used for computing calibration error. */
  double m_BestCorrelationNormalizationFactor;
  /*! Normalization factor used for the video metric. Used for computing calibration error. */
  double m_FixedSignalValuesNormalizationFactor;

};


#endif // _TEMPORAL_CALIBRATION_H
