/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"
#include "TrackedFrame.h"
#include "vtkObjectFactory.h"
#include "vtkDoubleArray.h"
#include "vtkLineSegmentationAlgo.h"
#include "vtkMath.h"
#include "vtkPrincipalMotionDetectionAlgo.h"
#include "vtkTemporalCalibrationAlgo.h"
#include "vtkTrackedFrameList.h"
#include <algorithm>
#include <fstream>
#include <iostream>

//-----------------------------------------------------------------------------

vtkCxxRevisionMacro(vtkTemporalCalibrationAlgo, "$Revision: 1.0 $");
vtkStandardNewMacro(vtkTemporalCalibrationAlgo);

//-----------------------------------------------------------------------------
// Default algorithm parameters
namespace
{
  // If the tracker metric "swings" less than this, abort
  const double MINIMUM_TRACKER_SIGNAL_PEAK_TO_PEAK_MM = 8.0; 
  // If the video metric "swings" less than this, abort
  const double MINIMUM_VIDEO_SIGNAL_PEAK_TO_PEAK_PIXEL = 30.0; 
  // Temporal resolution below which two time values are considered identical
  const double TIMESTAMP_EPSILON_SEC = 0.0001; 
  // The maximum resolution that the user can request
  const double MINIMUM_SAMPLING_RESOLUTION_SEC = 0.00001; 
  const double DEFAULT_SAMPLING_RESOLUTION_SEC = 0.001; 
  const double DEFAULT_MAX_TRACKER_LAG_SEC = 0.5;

  enum SignalAlignmentMetricType
  {
    SSD,
    CORRELATION,
    SAD,
    SIGNAL_METRIC_TYPE_COUNT
  };
  const double SIGNAL_ALIGNMENT_METRIC_THRESHOLD[SIGNAL_METRIC_TYPE_COUNT] = 
  {
    -2^500, 
    -2^500,
    2^500 
  };
  SignalAlignmentMetricType SIGNAL_ALIGNMENT_METRIC = SSD;

  enum MetricNormalizationType
  {
    STD,
    AMPLITUDE
  };
  MetricNormalizationType METRIC_NORMALIZATION = STD;
}

//-----------------------------------------------------------------------------
vtkTemporalCalibrationAlgo::vtkTemporalCalibrationAlgo() : 
  SamplingResolutionSec(DEFAULT_SAMPLING_RESOLUTION_SEC),
  MaxMovingLagSec(DEFAULT_MAX_TRACKER_LAG_SEC),
  NeverUpdated(true),
  SaveIntermediateImages(false),
  MovingLagSec(0.0),
  CalibrationError(0.0),
  BestCorrelationNormalizationFactor(0.0),
  FixedSignalValuesNormalizationFactor(0.0),
  BestCorrelationLagIndex(-1)
{
  FixedSignal.frameList=NULL;
  MovingSignal.frameList=NULL;
  LineSegmentationClipRectangleOrigin[0] = 0;
  LineSegmentationClipRectangleOrigin[1] = 0;
  LineSegmentationClipRectangleSize[0] = 0;
  LineSegmentationClipRectangleSize[1] = 0;
}

//-----------------------------------------------------------------------------
vtkTemporalCalibrationAlgo::~vtkTemporalCalibrationAlgo()
{
  if (FixedSignal.frameList != NULL)
  {
    FixedSignal.frameList->UnRegister(NULL);
    FixedSignal.frameList = NULL;
  }
  if (MovingSignal.frameList != NULL)
  {
    MovingSignal.frameList->UnRegister(NULL);
    MovingSignal.frameList = NULL;
  }
}

//-----------------------------------------------------------------------------
PlusStatus vtkTemporalCalibrationAlgo::Update(TEMPORAL_CALIBRATION_ERROR &error)
{  
  if(ComputeMovingSignalLagSec(error) != PLUS_SUCCESS)
  {
    return PLUS_FAIL;
  }
  error = TEMPORAL_CALIBRATION_ERROR_NONE;
  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
void vtkTemporalCalibrationAlgo::SetSaveIntermediateImages(bool saveIntermediateImages)
{
  SaveIntermediateImages = saveIntermediateImages;
}

//-----------------------------------------------------------------------------
void vtkTemporalCalibrationAlgo::SetFixedFrames(vtkTrackedFrameList* frameList, FRAME_TYPE frameType)
{
  if (frameList != FixedSignal.frameList)
  {
    if (FixedSignal.frameList != NULL)
    {
      FixedSignal.frameList->UnRegister(NULL);
      FixedSignal.frameList = NULL;
    }
    FixedSignal.frameList = frameList;
    FixedSignal.frameList->Register(NULL);
  }
  FixedSignal.frameType=frameType;
}

//-----------------------------------------------------------------------------
void vtkTemporalCalibrationAlgo::SetFixedProbeToReferenceTransformName(const std::string &probeToReferenceTransformName)
{
  FixedSignal.probeToReferenceTransformName=probeToReferenceTransformName;
}

//-----------------------------------------------------------------------------
void vtkTemporalCalibrationAlgo::SetMovingFrames(vtkTrackedFrameList* frameList, FRAME_TYPE frameType)
{
  if (frameList != MovingSignal.frameList)
  {
    if (MovingSignal.frameList != NULL)
    {
      MovingSignal.frameList->UnRegister(NULL);
      MovingSignal.frameList = NULL;
    }
    MovingSignal.frameList = frameList;
    MovingSignal.frameList->Register(NULL);
  }
  MovingSignal.frameType=frameType;
}

//-----------------------------------------------------------------------------
void vtkTemporalCalibrationAlgo::SetMovingProbeToReferenceTransformName(const std::string &probeToReferenceTransformName)
{
  MovingSignal.probeToReferenceTransformName = probeToReferenceTransformName;
}

//-----------------------------------------------------------------------------
void vtkTemporalCalibrationAlgo::SetSamplingResolutionSec(double samplingResolutionSec)
{
  if(samplingResolutionSec < MINIMUM_SAMPLING_RESOLUTION_SEC)
  {
    LOG_ERROR("Specified resampling resolution (" << samplingResolutionSec << " seconds) is too small. Sampling resolution must be greater than: " << MINIMUM_SAMPLING_RESOLUTION_SEC << " seconds");
    return;
  }
  SamplingResolutionSec = samplingResolutionSec;
}

//-----------------------------------------------------------------------------
void vtkTemporalCalibrationAlgo::SetMaximumMovingLagSec(double maxLagSec)
{
  MaxMovingLagSec = maxLagSec;
}

//-----------------------------------------------------------------------------
void vtkTemporalCalibrationAlgo::SetIntermediateFilesOutputDirectory(const std::string &outputDirectory)
{
  IntermediateFilesOutputDirectory = outputDirectory;
}

//-----------------------------------------------------------------------------
PlusStatus vtkTemporalCalibrationAlgo::GetMovingLagSec(double &lag)
{
  if (NeverUpdated)
  {
    LOG_ERROR("You must first call the \"Update()\" to compute the tracker lag.");
    return PLUS_FAIL;
  }
  lag = MovingLagSec;
  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkTemporalCalibrationAlgo::GetBestCorrelation( double &videoCorrelation )
{
  if (NeverUpdated)
  {
    LOG_ERROR("You must first call the \"Update()\" to compute the best correlation metric.");
    return PLUS_FAIL;
  }
  videoCorrelation = BestCorrelationValue;
  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkTemporalCalibrationAlgo::GetCalibrationError(double &error)
{
  if(NeverUpdated)
  {
    LOG_ERROR("You must first call the \"Update()\" to compute the calibration error.");
    return PLUS_FAIL;
  }
  error = CalibrationError;
  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkTemporalCalibrationAlgo::GetMaxCalibrationError(double &maxCalibrationError)
{
  if(NeverUpdated)
  {
    LOG_ERROR("You must first call the \"Update()\" to compute the calibration error.");
    return PLUS_FAIL;
  }
  maxCalibrationError = MaxCalibrationError;
  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkTemporalCalibrationAlgo::GetUncalibratedMovingPositionSignal(vtkTable* unCalibratedMovingPositionSignal)
{
  ConstructTableSignal(MovingSignal.normalizedSignalTimestamps, MovingSignal.normalizedSignalValues, unCalibratedMovingPositionSignal, 0); 
  if(unCalibratedMovingPositionSignal->GetNumberOfColumns() != 2)
  {
    LOG_ERROR("Error in constructing the vtk tables that are to hold moving signal. Table has " << 
               unCalibratedMovingPositionSignal->GetNumberOfColumns() << " columns, but should have two columns");
    return PLUS_FAIL;
  }
  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkTemporalCalibrationAlgo::GetCalibratedMovingPositionSignal(vtkTable* calibratedMovingPositionSignal)
{
  ConstructTableSignal(MovingSignal.normalizedSignalTimestamps, MovingSignal.normalizedSignalValues, calibratedMovingPositionSignal, -MovingLagSec); 
  if(calibratedMovingPositionSignal->GetNumberOfColumns() != 2)
  {
    LOG_ERROR("Error in constructing the vtk tables that are to hold moving signal. Table has " << 
               calibratedMovingPositionSignal->GetNumberOfColumns() << " columns, but should have two columns");
    return PLUS_FAIL;
  }
  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkTemporalCalibrationAlgo::GetFixedPositionSignal(vtkTable *FixedPositionSignal)
{
  ConstructTableSignal(FixedSignal.signalTimestamps, FixedSignal.signalValues, FixedPositionSignal, 0); 
  if(FixedPositionSignal->GetNumberOfColumns() != 2)
  {
    LOG_ERROR("Error in constructing the vtk tables that are to hold fixed signal. Table has " << 
               FixedPositionSignal->GetNumberOfColumns() << " columns, but should have two columns");
    return PLUS_FAIL;
  }
  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkTemporalCalibrationAlgo::GetCorrelationSignal(vtkTable* correlationSignal)
{
  ConstructTableSignal(CorrelationTimeOffsets, CorrelationValues, correlationSignal, 0); 
  if(correlationSignal->GetNumberOfColumns() != 2)
  {
    LOG_ERROR("Error in constructing the vtk tables that are to hold correlated signal. Table has " << 
               correlationSignal->GetNumberOfColumns() << " columns, but should have two columns");
    return PLUS_FAIL;
  }
  correlationSignal->GetColumn(0)->SetName("Time Offset [s]");
  correlationSignal->GetColumn(1)->SetName("Computed Correlation");
  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkTemporalCalibrationAlgo::GetCorrelationSignalFine(vtkTable* correlationSignal)
{
  ConstructTableSignal(CorrelationTimeOffsetsFine, CorrelationValuesFine, correlationSignal, 0); 
  if(correlationSignal->GetNumberOfColumns() != 2)
  {
    LOG_ERROR("Error in constructing the vtk tables that are to hold fine correlated signal. Table has " << 
               correlationSignal->GetNumberOfColumns() << " columns, but should have two columns");
    return PLUS_FAIL;
  }
  correlationSignal->GetColumn(0)->SetName("Time Offset [s]");
  correlationSignal->GetColumn(1)->SetName("Computed Correlation (with fine step size)");
  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkTemporalCalibrationAlgo::GetSignalRange(const std::deque<double> &signal, int startIndex, int stopIndex,  double &minValue, double &maxValue)
{
  if (signal.empty())
  {
    LOG_ERROR("Cannot get signal range, the signal is empty");
    return PLUS_FAIL;
  }
  //  Calculate maximum and minimum metric values
  maxValue = *(std::max_element(signal.begin() + startIndex, signal.begin() + stopIndex));
  minValue = *(std::min_element(signal.begin() + startIndex, signal.begin() + stopIndex));

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkTemporalCalibrationAlgo::NormalizeMetricValues(std::deque<double> &signal, double &normalizationFactor, int startIndex/*=0*/, int stopIndex/*=-1*/)
{
  if (signal.size() == 0)
  {
    LOG_ERROR("NormalizeMetricValues failed because the metric vector is empty");
    return PLUS_FAIL;
  }

  if (stopIndex<0)
  {
    stopIndex=signal.size()-1;
  }

  // Calculate the signal mean
  double mu = 0;
  double numOfSamples=0;
  for(int i = startIndex; i <= stopIndex; ++i)
  {
    mu += signal.at(i);
  }
  mu /= (stopIndex-startIndex+1);

  // Calculate the signal "amplitude" and use its inverse as normalizationFactor
  normalizationFactor = 1.0;
  switch (METRIC_NORMALIZATION)
  {
    case AMPLITUDE:
    {
      // Divide by the maximum signal amplitude
      double minValue=0;
      double maxValue=0;
      GetSignalRange(signal,startIndex, stopIndex, minValue, maxValue);
      double maxPeakToPeak=fabs(maxValue-minValue);
      if (maxPeakToPeak<1e-10)
      {
        LOG_ERROR("Cannot normalize data, peak to peak difference is too small");
      }
      else
      {
        normalizationFactor = 1.0/maxPeakToPeak;
      }
      break;
    }
    case STD:
    {
      // Calculate standard deviation
      double stdev = 0;      
      for(int i = startIndex; i <= stopIndex; ++i)
      {
        stdev += (signal.at(i) - mu)*(signal.at(i) - mu);
      }
      stdev = std::sqrt(stdev);
      stdev /= std::sqrt( static_cast<double>(stopIndex-startIndex+1) - 1);

      if (stdev<1e-10)
      {
        LOG_ERROR("Cannot normalize data, stdev is too small");
      }
      else
      {        
        normalizationFactor = 1.0/stdev;
      }
      break;
    }
  }

  // Normalize the signal values
  for(int i = 0; i < signal.size(); ++i)
  {
    signal.at(i) = (signal.at(i)-mu)*normalizationFactor;
  }

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkTemporalCalibrationAlgo::NormalizeMetricValues(std::deque<double> &signal, double &normalizationFactor, double startTime, double stopTime, const std::deque<double> &timestamps)
{
  if (timestamps.size() == 0)
  {
    LOG_ERROR("NormalizeMetricValues failed because the metric vector is empty");
    return PLUS_FAIL;
  }

  int startIndex=0;
  for(int i = 0; i < timestamps.size(); ++i)
  {
    double t=timestamps.at(i);
    if (t>=startTime)
    {
      startIndex=i;
      break;
    }
  }

  int stopIndex=timestamps.size()-1;
  for(int i = timestamps.size()-1; i>=0; --i)
  {
    double t=timestamps.at(i);
    if (t<=stopTime)
    {
      stopIndex=i;
      break;
    }
  }

  return NormalizeMetricValues(signal, normalizationFactor, startIndex, stopIndex);
}


//-----------------------------------------------------------------------------
PlusStatus vtkTemporalCalibrationAlgo::ResampleSignalLinearly(const std::deque<double>& templateSignalTimestamps,
  const vtkSmartPointer<vtkPiecewiseFunction>& signalFunction, std::deque<double>& resampledSignalValues)
{
  resampledSignalValues.resize(templateSignalTimestamps.size());
  for(int i = 0; i < templateSignalTimestamps.size(); ++i)
  {
    resampledSignalValues[i]=signalFunction->GetValue(templateSignalTimestamps[i]);
  }
  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
void vtkTemporalCalibrationAlgo::ComputeCorrelationBetweenFixedAndMovingSignal(double minTrackerLagSec, double maxTrackerLagSec, double stepSizeSec, double &bestCorrelationValue, double &bestCorrelationTimeOffset, double &bestCorrelationNormalizationFactor, std::deque<double> &corrTimeOffsets, std::deque<double> &corrValues)
{
  // We will let the tracker metric be the "sliding" metric and let the video metric be the "fixed" metric. Since we are assuming a maximum offset between the two streams.
  // NormalizeMetricValues(m_FixedSignal.signalValues, m_FixedSignalValuesNormalizationFactor);

  //  Constuct piecewise function for tracker signal
  vtkSmartPointer<vtkPiecewiseFunction> trackerPositionPiecewiseSignal = vtkSmartPointer<vtkPiecewiseFunction>::New();
  double midpoint = 0.5;
  double sharpness = 0;
  for(int i = 0; i < MovingSignal.signalTimestamps.size(); ++i)
  {
    trackerPositionPiecewiseSignal->AddPoint(MovingSignal.signalTimestamps.at(i), MovingSignal.signalValues.at(i), midpoint, sharpness);
  }

  // Compute alignment metric for each offset
  std::deque<double> normalizationFactors;
  if (stepSizeSec<TIMESTAMP_EPSILON_SEC)
  {
    LOG_ERROR("Sampling resolution is too small: "<<stepSizeSec<<" sec");
    return;
  }
  std::deque<double> slidingSignalTimestamps(FixedSignal.signalTimestamps.size()); 
  std::deque<double> resampledTrackerPositionMetric;
  for(double offsetValueSec = minTrackerLagSec; offsetValueSec <= maxTrackerLagSec; offsetValueSec+=stepSizeSec)
  {
    //LOG_DEBUG("offsetValueSec = " << offsetValueSec);
    corrTimeOffsets.push_back(offsetValueSec);
    for(int i = 0; i < slidingSignalTimestamps.size(); ++i)
    {
      slidingSignalTimestamps.at(i) =  FixedSignal.signalTimestamps.at(i)+offsetValueSec;
    }

    NormalizeMetricValues(FixedSignal.signalValues, FixedSignalValuesNormalizationFactor, slidingSignalTimestamps.front(), slidingSignalTimestamps.back(), FixedSignal.signalTimestamps);

    ResampleSignalLinearly(slidingSignalTimestamps,trackerPositionPiecewiseSignal,resampledTrackerPositionMetric);
    double normalizationFactor=1.0;
    NormalizeMetricValues(resampledTrackerPositionMetric, normalizationFactor);
    normalizationFactors.push_back(normalizationFactor);

    corrValues.push_back(ComputeAlignmentMetric(FixedSignal.signalValues, resampledTrackerPositionMetric));
    
  }

  // Find the time offset that has the best alignment metric value
  bestCorrelationValue = corrValues.at(0);
  bestCorrelationTimeOffset = corrTimeOffsets.at(0);
  bestCorrelationNormalizationFactor = normalizationFactors.at(0);
  for(int i = 1; i < corrValues.size(); ++i)
  {
    if(corrValues.at(i) > bestCorrelationValue)
    {
      bestCorrelationValue = corrValues.at(i);
      bestCorrelationTimeOffset = corrTimeOffsets.at(i);
      bestCorrelationNormalizationFactor = normalizationFactors.at(i);
    }
  }
  LOG_DEBUG("bestCorrelationValue="<<bestCorrelationValue);
  LOG_DEBUG("bestCorrelationTimeOffset="<<bestCorrelationTimeOffset);
  LOG_DEBUG("bestCorrelationNormalizationFactor="<<bestCorrelationNormalizationFactor);
  LOG_DEBUG("numberOfSamples="<<corrValues.size());
}

double vtkTemporalCalibrationAlgo::ComputeAlignmentMetric(const std::deque<double> &signalA, const std::deque<double> &signalB)
{
  if (signalA.size() != signalB.size() )
  { 
    LOG_ERROR("Cannot compute alignment metric: input signals size mismatch");
    return 0;
  }
  switch (SIGNAL_ALIGNMENT_METRIC)
  {
  case SSD:
    {
      // Use sum of squared differences as signal alignment metric
      double ssdSum = 0;
      for(long int i = 0; i < signalA.size(); ++i)
      {
        double diff = signalA.at(i) - signalB.at(i ); //SSD
        ssdSum -= diff*diff;
      }
      return ssdSum;
    }
  case CORRELATION:
    {
      // Use correlation as signal alignment metric
      double xCorrSum = 0;
      for(long int i = 0; i < signalA.size(); ++i)
      {
        xCorrSum += signalA.at(i) * signalB.at(i); // XCORR
      }
      return xCorrSum;
    }
  case SAD:
    {
      // Use sum of absolute differences as signal alignment metric
      double sadSum = 0;
      for(long int i = 0; i < signalA.size(); ++i)
      {
        sadSum -= fabs(signalA.at(i) - signalB.at(i)); //SAD
      }
      return sadSum;
    }
  default:
    LOG_ERROR("Unknown metric: "<<SIGNAL_ALIGNMENT_METRIC);
  }
  return 0;
}


//-----------------------------------------------------------------------------
PlusStatus vtkTemporalCalibrationAlgo::ComputePositionSignalValues(SignalType &signal)
{
  switch (signal.frameType)
  {
  case FRAME_TYPE_TRACKER:
    {
      vtkSmartPointer<vtkPrincipalMotionDetectionAlgo> trackerDataMetricExtractor = vtkSmartPointer<vtkPrincipalMotionDetectionAlgo>::New();

      trackerDataMetricExtractor->SetTrackerFrames(signal.frameList);
      trackerDataMetricExtractor->SetSignalTimeRange(signal.signalTimeRangeMin, signal.signalTimeRangeMax);
      trackerDataMetricExtractor->SetProbeToReferenceTransformName(signal.probeToReferenceTransformName);      

      if (trackerDataMetricExtractor->Update() != PLUS_SUCCESS)
      {
        LOG_ERROR("Failed to get line positions from video frames");
        return PLUS_FAIL;
      }
      trackerDataMetricExtractor->GetDetectedTimestamps(signal.signalTimestamps);
      trackerDataMetricExtractor->GetDetectedPositions(signal.signalValues);

      // If the metric values do not "swing" sufficiently, the signal is considered constant--i.e. infinite period--and will
      // not work for our purposes
      double minValue = 0;
      double maxValue = 0;
      GetSignalRange(signal.signalValues, 0, signal.signalValues.size() -1, minValue, maxValue);
      double maxPeakToPeak = std::abs(maxValue - minValue);
      if(maxPeakToPeak < MINIMUM_TRACKER_SIGNAL_PEAK_TO_PEAK_MM)
      {
        LOG_ERROR("Detected metric values do not vary sufficiently (i.e. tracking signal is constant). Actual peak-to-peak variation: "<<maxPeakToPeak<<", expected minimum: "<<MINIMUM_TRACKER_SIGNAL_PEAK_TO_PEAK_MM);
        return PLUS_FAIL;
      }
      return PLUS_SUCCESS;
    }
  case FRAME_TYPE_VIDEO:
    {
      vtkSmartPointer<vtkLineSegmentationAlgo> lineSegmenter = vtkSmartPointer<vtkLineSegmentationAlgo>::New();
      lineSegmenter->SetTrackedFrameList(signal.frameList);
      lineSegmenter->SetClipRectangle(LineSegmentationClipRectangleOrigin, LineSegmentationClipRectangleSize);
      lineSegmenter->SetSignalTimeRange(signal.signalTimeRangeMin, signal.signalTimeRangeMax);
      lineSegmenter->SetSaveIntermediateImages(SaveIntermediateImages);
      lineSegmenter->SetIntermediateFilesOutputDirectory(IntermediateFilesOutputDirectory);
      if ( lineSegmenter->Update() != PLUS_SUCCESS )
      {
        LOG_ERROR("Failed to get line positions from video frames");
        return PLUS_FAIL;
      }
      lineSegmenter->GetDetectedTimestamps(signal.signalTimestamps);
      lineSegmenter->GetDetectedPositions(signal.signalValues);

      // If the metric values do not "swing" sufficiently, the signal is considered constant--i.e. infinite period--and will
      // not work for our purposes
      double minValue = 0;
      double maxValue = 0;
      this->GetSignalRange(signal.signalValues, 0, signal.signalValues.size() - 1, minValue, maxValue);
      double maxPeakToPeak = std::abs(maxValue - minValue);
      if(maxPeakToPeak < MINIMUM_VIDEO_SIGNAL_PEAK_TO_PEAK_PIXEL)
      {
        LOG_ERROR("Detected metric values do not vary sufficiently (i.e. video signal is constant)");
        return PLUS_FAIL;
      }
      return PLUS_SUCCESS;
    }
  default:
    LOG_ERROR("Compute position signal value failed. Unknown frame type: " << signal.frameType);
    return PLUS_FAIL;
  }
}

//-----------------------------------------------------------------------------
PlusStatus vtkTemporalCalibrationAlgo::ComputeCommonTimeRange()
{
  if (FixedSignal.frameList->GetNumberOfTrackedFrames()<1)
  {
    LOG_ERROR("Fixed signal frame list are empty");
    return PLUS_FAIL;
  }
  if (MovingSignal.frameList->GetNumberOfTrackedFrames()<1)
  {
    LOG_ERROR("Moving signal frame list are empty");
    return PLUS_FAIL;
  }

  double fixedTimestampMin = FixedSignal.frameList->GetTrackedFrame(0)->GetTimestamp();
  double fixedTimestampMax = FixedSignal.frameList->GetTrackedFrame(FixedSignal.frameList->GetNumberOfTrackedFrames()-1)->GetTimestamp();;
  double movingTimestampMin = MovingSignal.frameList->GetTrackedFrame(0)->GetTimestamp();
  double movingTimestampMax = MovingSignal.frameList->GetTrackedFrame(MovingSignal.frameList->GetNumberOfTrackedFrames()-1)->GetTimestamp();;
  
  double commonRangeMin = std::max(fixedTimestampMin, movingTimestampMin); 
  double commonRangeMax = std::min(fixedTimestampMax, movingTimestampMax);
  if (commonRangeMin + MaxMovingLagSec >= commonRangeMax - MaxMovingLagSec)
  {
    LOG_ERROR("Insufficient overlap between fixed and moving frames timestamps to compute time offset"); 
    return PLUS_FAIL;
  }
  
  FixedSignal.signalTimeRangeMin = commonRangeMin;
  FixedSignal.signalTimeRangeMax = commonRangeMax;
  MovingSignal.signalTimeRangeMin = commonRangeMin + MaxMovingLagSec;
  MovingSignal.signalTimeRangeMax = commonRangeMax - MaxMovingLagSec;
  
  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkTemporalCalibrationAlgo::ComputeMovingSignalLagSec(TEMPORAL_CALIBRATION_ERROR& error)
{
  // Need to determine the common signal range before extracting signals from the frames,
  // because normalization, PCA, etc. must be performed only by taking into account
  // the frames in the common range.
  if( ComputeCommonTimeRange() != PLUS_SUCCESS )
  {
    error = TEMPORAL_CALIBRATION_ERROR_NO_COMMON_TIME_RANGE;
    return PLUS_FAIL;
  }

  // Compute the position signal values from the input frames
  if ( ComputePositionSignalValues(FixedSignal) != PLUS_SUCCESS)
  {
    error = TEMPORAL_CALIBRATION_ERROR_FAILED_COMPUTE_FIXED;
    LOG_ERROR("Failed to compute position signal from fixed frames");
    return PLUS_FAIL;
  }
  if ( ComputePositionSignalValues(MovingSignal) != PLUS_SUCCESS)
  {
    error = TEMPORAL_CALIBRATION_ERROR_FAILED_COMPUTE_MOVING;
    LOG_ERROR("Failed to compute position signal from moving frames");
    return PLUS_FAIL;
  }

  // Compute approx image image frame period. We will use this frame period as a step size in the coarse optimum search phase.
  double fixedTimestampMin = FixedSignal.signalTimestamps.at(0);
  double fixedTimestampMax = FixedSignal.signalTimestamps.at(FixedSignal.signalTimestamps.size() - 1);  
  if (FixedSignal.signalTimestamps.size() < 2)
  {
    error = TEMPORAL_CALIBRATION_ERROR_NOT_ENOUGH_FIXED_FRAMES;
    LOG_ERROR("Not enough fixed frames are available");
    return PLUS_FAIL;
  }
  double imageFramePeriodSec = (fixedTimestampMax-fixedTimestampMin) / (FixedSignal.signalTimestamps.size()-1);

  double searchRangeFineStep = imageFramePeriodSec*3;

  //  Compute cross correlation with sign convention #1 
  LOG_DEBUG("ComputeCorrelationBetweenFixedAndMovingSignal(sign convention #1)");
  double bestCorrelationValue=0;
  double bestCorrelationTimeOffset=0;
  double bestCorrelationNormalizationFactor=1.0;
  std::deque<double> corrTimeOffsets;
  std::deque<double> corrValues;
  ComputeCorrelationBetweenFixedAndMovingSignal(-MaxMovingLagSec,MaxMovingLagSec,imageFramePeriodSec,bestCorrelationValue,bestCorrelationTimeOffset,bestCorrelationNormalizationFactor, corrTimeOffsets, corrValues);
  std::deque<double> corrTimeOffsetsFine;
  std::deque<double> corrValuesFine;
  ComputeCorrelationBetweenFixedAndMovingSignal(bestCorrelationTimeOffset-searchRangeFineStep,bestCorrelationTimeOffset+searchRangeFineStep,SamplingResolutionSec,bestCorrelationValue,bestCorrelationTimeOffset,bestCorrelationNormalizationFactor, corrTimeOffsetsFine, corrValuesFine);
  LOG_DEBUG("Time offset with sign convention #1: " << bestCorrelationTimeOffset);
  
  //  Compute cross correlation with sign convention #2
  LOG_DEBUG("ComputeCorrelationBetweenFixedAndMovingSignal(sign convention #2)");
  // Mirror tracker metric signal about x-axis 
  for(long int i = 0; i < MovingSignal.signalValues.size(); ++i)
  {
    MovingSignal.signalValues.at(i) *= -1;
  }
  double bestCorrelationValueInvertedTracker(0);
  double bestCorrelationTimeOffsetInvertedTracker(0);
  double bestCorrelationNormalizationFactorInvertedTracker(1.0);
  std::deque<double> corrTimeOffsetsInvertedTracker;
  std::deque<double> corrValuesInvertedTracker;
  ComputeCorrelationBetweenFixedAndMovingSignal(
    -MaxMovingLagSec,
    MaxMovingLagSec,
    imageFramePeriodSec,
    bestCorrelationValueInvertedTracker,
    bestCorrelationTimeOffsetInvertedTracker,
    bestCorrelationNormalizationFactorInvertedTracker, 
    corrTimeOffsetsInvertedTracker, 
    corrValuesInvertedTracker
    );
  std::deque<double> corrTimeOffsetsInvertedTrackerFine;
  std::deque<double> corrValuesInvertedTrackerFine;
  ComputeCorrelationBetweenFixedAndMovingSignal(
    bestCorrelationTimeOffsetInvertedTracker-searchRangeFineStep,
    bestCorrelationTimeOffsetInvertedTracker+searchRangeFineStep,
    SamplingResolutionSec,bestCorrelationValueInvertedTracker,
    bestCorrelationTimeOffsetInvertedTracker,
    bestCorrelationNormalizationFactorInvertedTracker, 
    corrTimeOffsetsInvertedTrackerFine, 
    corrValuesInvertedTrackerFine
    );  
  LOG_DEBUG("Time offset with sign convention #2: " << bestCorrelationTimeOffsetInvertedTracker);
  
  // Adopt the smallest tracker lag
  if(std::abs(bestCorrelationTimeOffset) < std::abs(bestCorrelationTimeOffsetInvertedTracker))
  {
    MovingLagSec = bestCorrelationTimeOffset;
    BestCorrelationTimeOffset = bestCorrelationTimeOffset;
    BestCorrelationValue = bestCorrelationValue;
    BestCorrelationNormalizationFactor = bestCorrelationNormalizationFactor;
    CorrelationTimeOffsets=corrTimeOffsets;
    CorrelationValues=corrValues;
    CorrelationTimeOffsetsFine=corrTimeOffsetsFine;
    CorrelationValuesFine=corrValuesFine;

    // Flip tracker metric signal back to correspond to sign convention #1 
    for(long int i = 0; i < MovingSignal.signalValues.size(); ++i)
    {
      MovingSignal.signalValues.at(i) *= -1;
    }
  }
  else
  {
    MovingLagSec = bestCorrelationTimeOffsetInvertedTracker;
    BestCorrelationTimeOffset = bestCorrelationTimeOffsetInvertedTracker;
    BestCorrelationValue = bestCorrelationValueInvertedTracker;
    BestCorrelationNormalizationFactor = bestCorrelationNormalizationFactorInvertedTracker;
    CorrelationTimeOffsets=corrTimeOffsetsInvertedTracker;
    CorrelationValues=corrValuesInvertedTracker;
    CorrelationTimeOffsetsFine=corrTimeOffsetsInvertedTrackerFine;
    CorrelationValuesFine=corrValuesInvertedTrackerFine;
  }
     
  //NormalizeMetricValues(m_FixedSignal.signalValues, m_FixedSignalValuesNormalizationFactor, m_MovingSignal.signalTimestamps.front()-m_TrackerLagSec, m_MovingSignal.signalTimestamps.back()-m_TrackerLagSec, m_FixedSignal.signalTimestamps);

  // Normalize the tracker metric based on the best index offset (only considering the overlap "window"
  for(int i = 0; i < MovingSignal.signalTimestamps.size(); ++i)
  {
    if(MovingSignal.signalTimestamps.at(i) > FixedSignal.signalTimestamps.at(0) + MovingLagSec && MovingSignal.signalTimestamps.at(i) < FixedSignal.signalTimestamps.at(FixedSignal.signalTimestamps.size() -1) + MovingLagSec)
    {
      MovingSignal.normalizedSignalValues.push_back(MovingSignal.signalValues.at(i));
      MovingSignal.normalizedSignalTimestamps.push_back(MovingSignal.signalTimestamps.at(i));
    }
  }

  // Get a normalized tracker position metric that can be displayed
   double unusedNormFactor=1.0;
   NormalizeMetricValues(MovingSignal.normalizedSignalValues, unusedNormFactor);

  CalibrationError=sqrt(-BestCorrelationValue) / BestCorrelationNormalizationFactor; // RMSE in mm

  LOG_DEBUG("Moving signal lags fixed signal by: " << MovingLagSec << " [s]");


  // Get maximum calibration error

  //  Get the timestamps of the sliding signal (i.e. cropped video signal) shifted by the best-found offset
  std::deque<double> shiftedSlidingSignalTimestamps;
  for(int i = 0; i < FixedSignal.signalTimestamps.size(); ++i)
  {
    shiftedSlidingSignalTimestamps.push_back(FixedSignal.signalTimestamps.at(i) + MovingLagSec); // TODO: check this
  }

  //  Get the values of the tracker metric at the offset sliding signal values

  //  Constuct piecewise function for tracker signal
  vtkSmartPointer<vtkPiecewiseFunction> trackerPositionPiecewiseSignal = vtkSmartPointer<vtkPiecewiseFunction>::New();
  double midpoint = 0.5;
  double sharpness = 0;
  for(int i = 0; i < MovingSignal.normalizedSignalTimestamps.size(); ++i)
  {
    trackerPositionPiecewiseSignal->AddPoint(MovingSignal.normalizedSignalTimestamps.at(i), MovingSignal.normalizedSignalValues.at(i), midpoint, sharpness);
  }

  std::deque<double> resampledNormalizedTrackerPositionMetric;
  ResampleSignalLinearly(shiftedSlidingSignalTimestamps,trackerPositionPiecewiseSignal,resampledNormalizedTrackerPositionMetric);

  for(long int i = 0; i < resampledNormalizedTrackerPositionMetric.size(); ++i)
  {
    double diff = resampledNormalizedTrackerPositionMetric.at(i) - FixedSignal.signalValues.at(i); //SSD
    CalibrationErrorVector.push_back(diff*diff); 
  }

  MaxCalibrationError = 0;
  for(long int i = 0; i < CalibrationErrorVector.size(); ++i)
  {
    if(CalibrationErrorVector.at(i) > MaxCalibrationError)
    {
      MaxCalibrationError = CalibrationErrorVector.at(i);
    }
  }

  MaxCalibrationError = std::sqrt(MaxCalibrationError)/BestCorrelationNormalizationFactor;

  NeverUpdated = false;

  if( BestCorrelationValue <= SIGNAL_ALIGNMENT_METRIC_THRESHOLD[SIGNAL_ALIGNMENT_METRIC] )
  {
    error = TEMPORAL_CALIBRATION_ERROR_RESULT_ABOVE_THRESHOLD;
    LOG_ERROR("Calculated correlation exceeds threshold value. This may be an indicator of a poor calibration.");
    return PLUS_FAIL;
  }

  LOG_DEBUG("Temporal calibration BestCorrelationValue = "<<BestCorrelationValue<<" (threshold="<<SIGNAL_ALIGNMENT_METRIC_THRESHOLD[SIGNAL_ALIGNMENT_METRIC]<<")");
  LOG_DEBUG("MaxCalibrationError="<<MaxCalibrationError);
  LOG_DEBUG("CalibrationError="<<CalibrationError);
  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkTemporalCalibrationAlgo::ConstructTableSignal(std::deque<double> &x, std::deque<double> &y, vtkTable* table,
                                               double timeCorrection)
{
  // Clear table
  while (table->GetNumberOfColumns() > 0)
  {
    table->RemoveColumn(0);
  }

  //  Create array corresponding to the time values of the tracker plot
  vtkSmartPointer<vtkDoubleArray> arrX = vtkSmartPointer<vtkDoubleArray>::New();
  table->AddColumn(arrX);
 
  //  Create array corresponding to the metric values of the tracker plot
  vtkSmartPointer<vtkDoubleArray> arrY = vtkSmartPointer<vtkDoubleArray>::New();
  table->AddColumn(arrY);
 
  // Set the tracker data
  table->SetNumberOfRows(x.size());
  for (int i = 0; i < x.size(); ++i)
  {
    table->SetValue(i, 0, x.at(i) + timeCorrection );
    table->SetValue(i, 1, y.at(i) );
  }

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkTemporalCalibrationAlgo::ReadConfiguration( vtkXMLDataElement* aConfig )
{
  if( aConfig == NULL )
  {
    LOG_ERROR("Null configuration sent to vtkTemporalCalibrationAlgo::ReadConfiguration.");
    return PLUS_FAIL;
  }

  vtkXMLDataElement* calibrationParameters = aConfig->FindNestedElementWithName("vtkTemporalCalibrationAlgo");
  if (calibrationParameters != NULL)
  {
    int clipOrigin[2] = {0};
    int clipSize[2] = {0};
    if ( calibrationParameters->GetVectorAttribute("ClipRectangleOrigin", 2, clipOrigin) && 
      calibrationParameters->GetVectorAttribute("ClipRectangleSize", 2, clipSize) )
    {
      LineSegmentationClipRectangleOrigin[0] = clipOrigin[0];
      LineSegmentationClipRectangleOrigin[1] = clipOrigin[1];
      LineSegmentationClipRectangleSize[0] = clipSize[0];
      LineSegmentationClipRectangleSize[1] = clipSize[1];
    }
    else
    {
      LOG_WARNING("Cannot find ClipRectangleOrigin or ClipRectangleSize attributes in the \'vtkTemporalCalibrationAlgo\' configuration.");
    }
  }

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
void vtkTemporalCalibrationAlgo::SetVideoClipRectangle( int* clipRectOriginIntVec, int* clipRectSizeIntVec )
{
  LineSegmentationClipRectangleOrigin[0] = clipRectOriginIntVec[0];
  LineSegmentationClipRectangleOrigin[1] = clipRectOriginIntVec[1];
  LineSegmentationClipRectangleSize[0] = clipRectSizeIntVec[0];
  LineSegmentationClipRectangleSize[1] = clipRectSizeIntVec[1];
}
