/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#ifndef _TEMPORAL_CALIBRATION_H
#define _TEMPORAL_CALIBRATION_H

#include "PlusConfigure.h"
#include "vtkObject.h"
#include <deque>
#include <iostream>
#include <time.h>
#include <vtkPiecewiseFunction.h>
#include <vtkTable.h>
class TrackedFrame;
class vtkTrackedFrameList;

/*!
  \class vtkTemporalCalibrationAlgo
  \brief Computes the time lag of the US probe's tracker stream relative to the US video stream.

  The inputted data--video and tracker--is assumed to be collected by a US probe imaging a planar object. Furthermore,
  it is assumed that the probe is undergoing uni-dirctional periodic motion in the direction perpendicular to the
  plane's face (e.g., moving the probe in a repeating up-and-down fashion while imaging the bottom of a water bath).
  The inputted data is assumed to contain at least five full periods (although the algorithm may work for fewer periods
  it has not been tested under these conditions). 

  The time offset is determined by computing the time offset where the correlation between the fixed signal (extracted from
  the video) and the moving signal (extracted from the tracker) is maximized. For the correlation computation the moving
  signal is linearly interpolated at the time positions where the fixed signal is known.

  The fixed and moving signal is cropped to the common time range. The moving signal is further cropped to the common range
  with "max tracker lag" margin.
  
  <PRE>
        -------------------------------   fixed (video)
            -----------------------       moving (tracking)

        |                             |
        ^                             ^
                 common range          

        |   |                     |   |
        ^   ^                     ^   ^
    max tracker lag           max tracker lag
  </PRE>

  \ingroup PlusLibCalibrationAlgorithm
*/


class vtkTemporalCalibrationAlgo : public vtkObject
{
public:
  vtkTypeRevisionMacro(vtkTemporalCalibrationAlgo,vtkObject);
  static vtkTemporalCalibrationAlgo *New();

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
    TEMPORAL_CALIBRATION_ERROR_NOT_ENOUGH_FIXED_FRAMES,
    TEMPORAL_CALIBRATION_ERROR_NO_FRAMES_IN_ULTRASOUND_DATA,
    TEMPORAL_CALIBRATION_ERROR_SAMPLING_RESOLUTION_TOO_SMALL,
    TEMPORAL_CALIBRATION_ERROR_FAILED_COMPUTE_FIXED,
    TEMPORAL_CALIBRATION_ERROR_FAILED_COMPUTE_MOVING,
    TEMPORAL_CALIBRATION_ERROR_NO_COMMON_TIME_RANGE,
  };

  enum FRAME_TYPE {
    FRAME_TYPE_NONE,
    FRAME_TYPE_TRACKER, // The tracked frame list contains tracker data
    FRAME_TYPE_VIDEO    // The tracked frame list contains US video data of a plane 
                        // (e.g., bottom of water tank)
  };

  struct SignalType
  {
    vtkTrackedFrameList* frameList;
    FRAME_TYPE frameType;
    std::string probeToReferenceTransformName;
    /*! Signal metric values that is computed from the frameList */
    std::deque<double> signalValues;     
    /*! Signal timestamps corresponding to the metric values in the frameList */
    std::deque<double> signalTimestamps; 
    /*! Normalized signal metric values that is computed from the frameList (recomputed for each offset) */
    std::deque<double> normalizedSignalValues; 
    /*! Normalized signal timestamps that is computed from the frameList (recomputed for each offset) */
    std::deque<double> normalizedSignalTimestamps; 
    /*! Start of the time range that contains the frames that should be used for signal generation */
    double signalTimeRangeMin;
    /*! End of the time range that contains the frames that should be used for signal generation */
    double signalTimeRangeMax;
  };

  PlusStatus ReadConfiguration(vtkXMLDataElement* aConfig);

  /*! Sets sampling resolution [s]. Default is 0.001 seconds. */  
  void SetSamplingResolutionSec(double samplingResolutionSec); 

  /*! Sets the list of frames and the type of the data set (tracking data, video data, ...) that will be used to compute the "fixed" position signal (usually the video data) */  
  void SetFixedFrames(vtkTrackedFrameList* frameList, FRAME_TYPE frameType); 

  /*! Sets ProbeToReferenceTransform name (in the format of "CoordinateSystem1ToCoordinateSystem2") for the moving signal. Only used if the fixed signal type is TRACKER_FRAME. */  
  void SetFixedProbeToReferenceTransformName(const std::string &probeToReferenceTransformName); 

  /*! 
    Sets the list of frames and the type of the data set (tracking data, video data, ...) that will be used to compute the "moving" position signal
    (that is interpolated during the signal alignment process at each position of the fixed signal values). The moving signalat the usually the tracking data,
    because it is more dense than the video data, so the interpolation at the less dense video timepoints is more accurate.
  */  
  void SetMovingFrames(vtkTrackedFrameList* frameList, FRAME_TYPE frameType); 

  /*! Sets ProbeToReferenceTransform name (in the format of "CoordinateSystem1ToCoordinateSystem2") for the moving signal. Only used if the moving signal type is TRACKER_FRAME. */  
  void SetMovingProbeToReferenceTransformName(const std::string &probeToReferenceTransformName); 

  /*! Sets the maximum allowable time lag between the corresponding tracker and video frames. Default is 2 seconds */  
  void SetMaximumMovingLagSec(double maxLagSec);

  /*! Enable/disable saving of intermediate images for debugging. Need to call before SetVideoFrames. */
  void SetSaveIntermediateImages(bool saveIntermediateImages);

  void SetIntermediateFilesOutputDirectory(const std::string &outputDirectory);

  void SetVideoClipRectangle( int* clipRectOriginIntVec, int* clipRectSizeIntVec );

  /*! Compute the tracker lag */  
  PlusStatus Update(TEMPORAL_CALIBRATION_ERROR &error); 

  /*!
    Returns the computed time [s] by which the tracker stream lags the video stream. 
    If the lag < 0, the tracker stream leads the video stream.
    The computed lag corresponds to the time offset that minimizes the difference between the position
    metric that is computed from the tracker and the video: min( sum(|VideoPositionMetric(t)-TrackerPositionMetric(t+lag)|) )
  */  
  PlusStatus GetMovingLagSec(double &lag);
  
  /*!
    Returns the calibration error. If the error is large then the computed tracker lag is not reliable.
    TODO: determine typical acceptable ranges
  */
  PlusStatus GetCalibrationError(double &error);

  PlusStatus GetUncalibratedMovingPositionSignal(vtkTable* unCalibratedMovingPositionSignal);
  PlusStatus GetCalibratedMovingPositionSignal(vtkTable* calibratedMovingPositionSignal);
  PlusStatus GetFixedPositionSignal(vtkTable* fixedPositionSignal);
  PlusStatus GetCorrelationSignal(vtkTable* correlationSignal);
  PlusStatus GetCorrelationSignalFine(vtkTable* correlationSignal);

  PlusStatus GetBestCorrelation(double &videoCorrelation);
  PlusStatus GetMaxCalibrationError(double &maxCalibrationError);

protected:   
  PlusStatus ComputeMovingSignalLagSec(TEMPORAL_CALIBRATION_ERROR& error);
  PlusStatus ComputePositionSignalValues(SignalType &signal);
  PlusStatus GetSignalRange(const std::deque<double> &signal, int startIndex, int stopIndex, double &minValue, double &maxValue);
  PlusStatus VerifyTrackerInput(vtkTrackedFrameList *trackerFrames, TEMPORAL_CALIBRATION_ERROR &error);

  /*! Determine common signal time range between the fixed and moving signals  */
  PlusStatus ComputeCommonTimeRange();

  PlusStatus ComputeTrackerLagSec(TEMPORAL_CALIBRATION_ERROR &error);  
  PlusStatus NormalizeMetricValues(std::deque<double> &signal, double &normalizationFactor, int startIndex=0, int stopIndex=-1);
  PlusStatus NormalizeMetricValues(std::deque<double> &signal, double &normalizationFactor, double startTime, double stopTime, const std::deque<double> &timestamps);
  void ComputeCorrelationBetweenFixedAndMovingSignal(double minTrackerLagSec, double maxTrackerLagSec, double stepSizeSec, double &bestCorrelationValue, double &bestCorrelationTimeOffset, double &bestCorrelationNormalizationFactor, std::deque<double> &corrTimeOffsets, std::deque<double> &corrValues);

  double ComputeAlignmentMetric(const std::deque<double> &signalA, const std::deque<double> &signalB);

  PlusStatus ComputeLineParameters(std::vector<itk::Point<double,2> > &data, std::vector<double> &planeParameters);
  PlusStatus ConstructTableSignal(std::deque<double> &x, std::deque<double> &y, vtkTable* table, double timeCorrection); 

  PlusStatus ResampleSignalLinearly(const std::deque<double>& templateSignalTimestamps, const vtkSmartPointer<vtkPiecewiseFunction>& signalFunction, std::deque<double>& resampledSignalValues);

  SignalType FixedSignal;
  SignalType MovingSignal;

  /*! Stores whether the user has called Update(); will not return tracker lag until set to "true" */
  bool TrackerLagUpToDate;  
  /*! Has the user ever succsfully called Update() */
  bool NeverUpdated;

  /*! If "true" then images of intermediate steps are saved */
  bool SaveIntermediateImages;
  /*! Directory where the intermediate files are written to */
  std::string IntermediateFilesOutputDirectory;
  
  /*! Resolution used for re-sampling [s]*/
  double SamplingResolutionSec;
    
  /*! The computed signal correlation values (corresponding to the better sign convention) */
  std::deque<double> CorrelationValues;
  /*! The time-offsets used to compute the correlations */
  std::deque<double> CorrelationTimeOffsets;

  /*! The computed signal correlation values (corresponding to the better sign convention, in the second phase with fine resolution) */
  std::deque<double> CorrelationValuesFine;
  /*! The time-offsets used to compute the correlations (in the second phase with fine resolution) */
  std::deque<double> CorrelationTimeOffsetsFine;
  
  /*! The highest correlation value for the tested time-offsets */
  double BestCorrelationValue;
  /*! Given index for the calculated best fit */
  double BestCorrelationLagIndex;
  /*! Given time offset for the calculated best fit */
  double BestCorrelationTimeOffset;
  
  std::deque<double> CalibrationErrorVector;

  /*! Time [s] that tracker lags video. If lag < 0, the tracker leads the video */
  double MovingLagSec;

  /*! The residual error after temporal calibration of the video and tracker signals */
  double CalibrationError;
  double MaxCalibrationError;

  /*! Maximum allowed tracker lag--if lag is greater, will exit computation */
  double MaxMovingLagSec;  
   
  /*! Normalization factor used for the tracker metric. Used for computing calibration error. */
  double BestCorrelationNormalizationFactor;
  /*! Normalization factor used for the video metric. Used for computing calibration error. */
  double FixedSignalValuesNormalizationFactor;

  /*! Clip rectangle origin for the line segmentation (in pixels). Everything outside the rectangle is ignored. */
  int LineSegmentationClipRectangleOrigin[2];

  /*! Clip rectangle origin for the line segmentation (in pixels). Everything outside the rectangle is ignored. */
  int LineSegmentationClipRectangleSize[2]; 

private:
  vtkTemporalCalibrationAlgo();
  ~vtkTemporalCalibrationAlgo();
};


#endif // _TEMPORAL_CALIBRATION_H
