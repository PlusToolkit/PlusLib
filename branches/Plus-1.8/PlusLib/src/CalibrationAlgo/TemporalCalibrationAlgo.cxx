/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"

#include "vtkMath.h"
#include "vtkDoubleArray.h"

#include <iostream>
#include <fstream>
#include <algorithm>

#include "TemporalCalibrationAlgo.h"
#include "vtkLineSegmentationAlgo.h"
#include "vtkPrincipalMotionDetectionAlgo.h"

// Default algorithm parameters
static const double MINIMUM_TRACKER_SIGNAL_PEAK_TO_PEAK_MM = 8.0; // If the tracker metric "swings" less than this, abort
static const double MINIMUM_VIDEO_SIGNAL_PEAK_TO_PEAK_PIXEL = 30.0; // If the video metric "swings" less than this, abort
static const double TIMESTAMP_EPSILON_SEC = 0.0001; // Temporal resolution below which two time values are considered identical
static const double MINIMUM_SAMPLING_RESOLUTION_SEC = 0.00001; // The maximum resolution that the user can request
static const double DEFAULT_SAMPLING_RESOLUTION_SEC = 0.001; 
static const double DEFAULT_MAX_TRACKER_LAG_SEC = 2.0;

enum SignalAlignmentMetricType                                                    { SSD,    CORRELATION, SAD,  SIGNAL_METRIC_TYPE_COUNT };
static const double SIGNAL_ALIGNMENT_METRIC_THRESHOLD[SIGNAL_METRIC_TYPE_COUNT] = { -2^500, -2^500,      2^500 };
const SignalAlignmentMetricType SIGNAL_ALIGNMENT_METRIC = SSD;

enum MetricNormalizationType { STD, AMPLITUDE };
const MetricNormalizationType METRIC_NORMALIZATION = AMPLITUDE;

//-----------------------------------------------------------------------------
TemporalCalibration::TemporalCalibration() : 
  m_SamplingResolutionSec(DEFAULT_SAMPLING_RESOLUTION_SEC),
  m_MaxMovingLagSec(DEFAULT_MAX_TRACKER_LAG_SEC),
  m_NeverUpdated(true),
  m_SaveIntermediateImages(false),
  m_TrackerLagSec(0.0),
  m_CalibrationError(0.0),
  m_BestCorrelationNormalizationFactor(0.0),
  m_FixedSignalValuesNormalizationFactor(0.0),
  m_BestCorrelationLagIndex(-1)
{
  m_FixedSignal.frameList=NULL;
  m_MovingSignal.frameList=NULL;
  m_LineSegmentationClipRectangleOrigin[0]=0;
  m_LineSegmentationClipRectangleOrigin[1]=0;
  m_LineSegmentationClipRectangleSize[0]=0;
  m_LineSegmentationClipRectangleSize[1]=0;
}

//-----------------------------------------------------------------------------
TemporalCalibration::~TemporalCalibration()
{
  if (m_FixedSignal.frameList!=NULL)
  {
    m_FixedSignal.frameList->UnRegister(NULL);
    m_FixedSignal.frameList=NULL;
  }
  if (m_MovingSignal.frameList!=NULL)
  {
    m_MovingSignal.frameList->UnRegister(NULL);
    m_MovingSignal.frameList=NULL;
  }
}

//-----------------------------------------------------------------------------
PlusStatus TemporalCalibration::Update(TEMPORAL_CALIBRATION_ERROR &error)
{  
  if(ComputeMovingSignalLagSec() != PLUS_SUCCESS)
  {
    return PLUS_FAIL;
  }
  error = TEMPORAL_CALIBRATION_ERROR_NONE;
  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
void TemporalCalibration::SetSaveIntermediateImages(bool saveIntermediateImages)
{
  m_SaveIntermediateImages = saveIntermediateImages;
}

//-----------------------------------------------------------------------------
void TemporalCalibration::SetTrackerFrames(vtkTrackedFrameList* trackerFrames, const std::string &probeToReferenceTransformName)
{
  SetMovingFrames(trackerFrames, TRACKER_FRAME);
  SetMovingProbeToReferenceTransformName(probeToReferenceTransformName);
}

//-----------------------------------------------------------------------------
void TemporalCalibration::SetVideoFrames(vtkTrackedFrameList* videoFrames)
{
  SetFixedFrames(videoFrames, PLANE_VIDEO_FRAME);
}

//-----------------------------------------------------------------------------
void TemporalCalibration::SetFixedFrames(vtkTrackedFrameList* frameList, FRAME_TYPE frameType)
{
  if (frameList!=m_FixedSignal.frameList)
  {
    if (m_FixedSignal.frameList!=NULL)
    {
      m_FixedSignal.frameList->UnRegister(NULL);
      m_FixedSignal.frameList=NULL;
    }
    m_FixedSignal.frameList=frameList;
    m_FixedSignal.frameList->Register(NULL);
  }
  m_FixedSignal.frameType=frameType;
}

//-----------------------------------------------------------------------------
void TemporalCalibration::SetFixedProbeToReferenceTransformName(const std::string &probeToReferenceTransformName)
{
  m_FixedSignal.probeToReferenceTransformName=probeToReferenceTransformName;
}

//-----------------------------------------------------------------------------
void TemporalCalibration::SetMovingFrames(vtkTrackedFrameList* frameList, FRAME_TYPE frameType)
{
  if (frameList!=m_MovingSignal.frameList)
  {
    if (m_MovingSignal.frameList!=NULL)
    {
      m_MovingSignal.frameList->UnRegister(NULL);
      m_MovingSignal.frameList=NULL;
    }
    m_MovingSignal.frameList=frameList;
    m_MovingSignal.frameList->Register(NULL);
  }
  m_MovingSignal.frameType=frameType;
}

//-----------------------------------------------------------------------------
void TemporalCalibration::SetMovingProbeToReferenceTransformName(const std::string &probeToReferenceTransformName)
{
  m_MovingSignal.probeToReferenceTransformName=probeToReferenceTransformName;
}

//-----------------------------------------------------------------------------
void TemporalCalibration::SetSamplingResolutionSec(double samplingResolutionSec)
{
  if(samplingResolutionSec < MINIMUM_SAMPLING_RESOLUTION_SEC)
  {
    LOG_ERROR("Specified resampling resolution (" << samplingResolutionSec << " seconds) is too small. Sampling resolution must be greater than: " << MINIMUM_SAMPLING_RESOLUTION_SEC << " seconds");
    return;
  }
  m_SamplingResolutionSec = samplingResolutionSec;
}

//-----------------------------------------------------------------------------
void TemporalCalibration::SetMaximumVideoTrackerLagSec(double maxLagSec)
{
  m_MaxMovingLagSec = maxLagSec;
}

//-----------------------------------------------------------------------------
void TemporalCalibration::SetIntermediateFilesOutputDirectory(const std::string &outputDirectory)
{
  m_IntermediateFilesOutputDirectory = outputDirectory;
}

//-----------------------------------------------------------------------------
PlusStatus TemporalCalibration::GetTrackerLagSec(double &lag)
{
  if (m_NeverUpdated)
  {
    LOG_ERROR("You must first call the \"Update()\" to compute the tracker lag.");
    return PLUS_FAIL;
  }
  lag = m_TrackerLagSec;
  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus TemporalCalibration::GetBestCorrelation( double &videoCorrelation )
{
  if (m_NeverUpdated)
  {
    LOG_ERROR("You must first call the \"Update()\" to compute the best correlation metric.");
    return PLUS_FAIL;
  }
	videoCorrelation = m_BestCorrelationValue;
  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus TemporalCalibration::GetCalibrationError(double &error)
{
  if(m_NeverUpdated)
  {
    LOG_ERROR("You must first call the \"Update()\" to compute the calibration error.");
    return PLUS_FAIL;
  }
  error = m_CalibrationError;
  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus TemporalCalibration::GetMaxCalibrationError(double &maxCalibrationError)
{
  if(m_NeverUpdated)
  {
    LOG_ERROR("You must first call the \"Update()\" to compute the calibration error.");
    return PLUS_FAIL;
  }
  maxCalibrationError = m_MaxCalibrationError;
  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus TemporalCalibration::GetUncalibratedTrackerPositionSignal(vtkTable* uncalibratedTrackerPositionSignal)
{
  ConstructTableSignal(m_MovingSignal.normalizedSignalTimestamps, m_MovingSignal.normalizedSignalValues, uncalibratedTrackerPositionSignal, 0); 
  if(uncalibratedTrackerPositionSignal->GetNumberOfColumns() != 2)
  {
    LOG_ERROR("Error in constructing the vtk tables that are to hold video signal. Table has " << 
               uncalibratedTrackerPositionSignal->GetNumberOfColumns() << " columns, but should have two columns");
    return PLUS_FAIL;
  }
  uncalibratedTrackerPositionSignal->GetColumn(0)->SetName("Time [s]");
  uncalibratedTrackerPositionSignal->GetColumn(1)->SetName("Uncalibrated Tracker Position Metric");
  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus TemporalCalibration::GetCalibratedTrackerPositionSignal(vtkTable* calibratedTrackerPositionSignal)
{
  ConstructTableSignal(m_MovingSignal.normalizedSignalTimestamps, m_MovingSignal.normalizedSignalValues, calibratedTrackerPositionSignal, -m_TrackerLagSec); 
  if(calibratedTrackerPositionSignal->GetNumberOfColumns() != 2)
  {
    LOG_ERROR("Error in constructing the vtk tables that are to hold video signal. Table has " << 
               calibratedTrackerPositionSignal->GetNumberOfColumns() << " columns, but should have two columns");
    return PLUS_FAIL;
  }
  calibratedTrackerPositionSignal->GetColumn(0)->SetName("Time [s]");
  calibratedTrackerPositionSignal->GetColumn(1)->SetName("Calibrated Tracker Position Metric");
  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus TemporalCalibration::GetVideoPositionSignal(vtkTable *videoPositionSignal)
{
  ConstructTableSignal(m_FixedSignal.signalTimestamps, m_FixedSignal.signalValues, videoPositionSignal, 0); 
  if(videoPositionSignal->GetNumberOfColumns() != 2)
  {
    LOG_ERROR("Error in constructing the vtk tables that are to hold uncalibrated tracker signal. Table has " << 
               videoPositionSignal->GetNumberOfColumns() << " columns, but should have two columns");
    return PLUS_FAIL;
  }
  videoPositionSignal->GetColumn(0)->SetName("Time [s]");
  videoPositionSignal->GetColumn(1)->SetName("Video Position Metric");
  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus TemporalCalibration::GetCorrelationSignal(vtkTable* correlationSignal)
{
  ConstructTableSignal(m_CorrTimeOffsets, m_CorrValues, correlationSignal, 0); 
  if(correlationSignal->GetNumberOfColumns() != 2)
  {
    LOG_ERROR("Error in constructing the vtk tables that are to hold video signal. Table has " << 
               correlationSignal->GetNumberOfColumns() << " columns, but should have two columns");
    return PLUS_FAIL;
  }
  correlationSignal->GetColumn(0)->SetName("Time Offset [s]");
  correlationSignal->GetColumn(1)->SetName("Computed Correlation");
  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus TemporalCalibration::GetCorrelationSignalFine(vtkTable* correlationSignal)
{
  ConstructTableSignal(m_CorrTimeOffsetsFine, m_CorrValuesFine, correlationSignal, 0); 
  if(correlationSignal->GetNumberOfColumns() != 2)
  {
    LOG_ERROR("Error in constructing the vtk tables that are to hold video signal. Table has " << 
               correlationSignal->GetNumberOfColumns() << " columns, but should have two columns");
    return PLUS_FAIL;
  }
  correlationSignal->GetColumn(0)->SetName("Time Offset [s]");
  correlationSignal->GetColumn(1)->SetName("Computed Correlation (with fine step size)");
  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus TemporalCalibration::GetSignalRange(const std::deque<double> &signal, int startIndex, int stopIndex,  double &minValue, double &maxValue)
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
PlusStatus TemporalCalibration::NormalizeMetricValues(std::deque<double> &signal, double &normalizationFactor, int startIndex/*=0*/, int stopIndex/*=-1*/)
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
PlusStatus TemporalCalibration::NormalizeMetricValues(std::deque<double> &signal, double &normalizationFactor, double startTime, double stopTime, const std::deque<double> &timestamps)
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
PlusStatus TemporalCalibration::ResampleSignalLinearly(const std::deque<double>& templateSignalTimestamps,
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
void TemporalCalibration::ComputeCorrelationBetweenFixedAndMovingSignal(double minTrackerLagSec, double maxTrackerLagSec, double stepSizeSec, double &bestCorrelationValue, double &bestCorrelationTimeOffset, double &bestCorrelationNormalizationFactor, std::deque<double> &corrTimeOffsets, std::deque<double> &corrValues)
{
  // We will let the tracker metric be the "sliding" metric and let the video metric be the "fixed" metric. Since we are assuming a maximum offset between the two streams.
  // NormalizeMetricValues(m_FixedSignal.signalValues, m_FixedSignalValuesNormalizationFactor);

	//	Constuct piecewise function for tracker signal
	vtkSmartPointer<vtkPiecewiseFunction> trackerPositionPiecewiseSignal = vtkSmartPointer<vtkPiecewiseFunction>::New();
	double midpoint = 0.5;
	double sharpness = 0;
	for(int i = 0; i < m_MovingSignal.signalTimestamps.size(); ++i)
	{
		trackerPositionPiecewiseSignal->AddPoint(m_MovingSignal.signalTimestamps.at(i), m_MovingSignal.signalValues.at(i), midpoint, sharpness);
	}

  // Compute alignment metric for each offset
  std::deque<double> normalizationFactors;
  if (stepSizeSec<TIMESTAMP_EPSILON_SEC)
  {
    LOG_ERROR("Sampling resolution is too small: "<<stepSizeSec<<" sec");
    return;
  }
  std::deque<double> slidingSignalTimestamps(m_FixedSignal.signalTimestamps.size()); 
  std::deque<double> resampledTrackerPositionMetric;
	for(double offsetValueSec = minTrackerLagSec; offsetValueSec <= maxTrackerLagSec; offsetValueSec+=stepSizeSec)
	{
    //LOG_DEBUG("offsetValueSec = " << offsetValueSec);
    corrTimeOffsets.push_back(offsetValueSec);
		for(int i = 0; i < slidingSignalTimestamps.size(); ++i)
		{
			slidingSignalTimestamps.at(i) =  m_FixedSignal.signalTimestamps.at(i)+offsetValueSec;
		}

    NormalizeMetricValues(m_FixedSignal.signalValues, m_FixedSignalValuesNormalizationFactor, slidingSignalTimestamps.front(), slidingSignalTimestamps.back(), m_FixedSignal.signalTimestamps);

		ResampleSignalLinearly(slidingSignalTimestamps,trackerPositionPiecewiseSignal,resampledTrackerPositionMetric);
		double normalizationFactor=1.0;
		NormalizeMetricValues(resampledTrackerPositionMetric, normalizationFactor);
		normalizationFactors.push_back(normalizationFactor);

    corrValues.push_back(ComputeAlignmentMetric(m_FixedSignal.signalValues, resampledTrackerPositionMetric));
		
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

double TemporalCalibration::ComputeAlignmentMetric(const std::deque<double> &signalA, const std::deque<double> &signalB)
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
PlusStatus TemporalCalibration::ComputePositionSignalValues(SignalType &signal)
{
  switch (signal.frameType)
  {
  case TRACKER_FRAME:
    {
      vtkSmartPointer<vtkPrincipalMotionDetectionAlgo> trackerDataMetricExtractor=vtkSmartPointer<vtkPrincipalMotionDetectionAlgo>::New();

      trackerDataMetricExtractor->SetTrackerFrames(signal.frameList);
      trackerDataMetricExtractor->SetSignalTimeRange(signal.signalTimeRangeMin, signal.signalTimeRangeMax);
      trackerDataMetricExtractor->SetProbeToReferenceTransformName(signal.probeToReferenceTransformName);      

      if (trackerDataMetricExtractor->Update()!=PLUS_SUCCESS)
      {
        LOG_ERROR("Failed to get line positions from video frames");
        return PLUS_FAIL;
      }
      trackerDataMetricExtractor->GetDetectedTimestamps(signal.signalTimestamps);
      trackerDataMetricExtractor->GetDetectedPositions(signal.signalValues);

      // If the metric values do not "swing" sufficiently, the signal is considered constant--i.e. infinite period--and will
      // not work for our purposes
      double minValue=0;
      double maxValue=0;
      GetSignalRange(signal.signalValues, 0, signal.signalValues.size() -1, minValue, maxValue);
      double maxPeakToPeak = std::abs(maxValue-minValue);
      if(maxPeakToPeak < MINIMUM_TRACKER_SIGNAL_PEAK_TO_PEAK_MM)
      {
        LOG_ERROR("Detected metric values do not vary sufficiently (i.e. tracking signal is constant). Actual peak-to-peak variation: "<<maxPeakToPeak<<", expected minimum: "<<MINIMUM_TRACKER_SIGNAL_PEAK_TO_PEAK_MM);
        return PLUS_FAIL;
      }
      return PLUS_SUCCESS;
    }
  case PLANE_VIDEO_FRAME:
    {
      vtkSmartPointer<vtkLineSegmentationAlgo> lineSegmenter=vtkSmartPointer<vtkLineSegmentationAlgo>::New();
      lineSegmenter->SetVideoFrames(signal.frameList);
      lineSegmenter->SetClipRectangle(m_LineSegmentationClipRectangleOrigin, m_LineSegmentationClipRectangleSize);
      lineSegmenter->SetSignalTimeRange(signal.signalTimeRangeMin, signal.signalTimeRangeMax);
      lineSegmenter->SetSaveIntermediateImages(m_SaveIntermediateImages);
      lineSegmenter->SetIntermediateFilesOutputDirectory(m_IntermediateFilesOutputDirectory);
      if (lineSegmenter->Update()!=PLUS_SUCCESS)
      {
        LOG_ERROR("Failed to get line positions from video frames");
        return PLUS_FAIL;
      }
      lineSegmenter->GetDetectedTimestamps(signal.signalTimestamps);
      lineSegmenter->GetDetectedPositions(signal.signalValues);

      // If the metric values do not "swing" sufficiently, the signal is considered constant--i.e. infinite period--and will
      // not work for our purposes
      double minValue=0;
      double maxValue=0;
      GetSignalRange(signal.signalValues,0, signal.signalValues.size() -1, minValue, maxValue);
      double maxPeakToPeak = std::abs(maxValue-minValue);
      if(maxPeakToPeak < MINIMUM_VIDEO_SIGNAL_PEAK_TO_PEAK_PIXEL)
      {
        LOG_ERROR("Detected metric values do not vary sufficiently (i.e. video signal is constant)");
        return PLUS_FAIL;
      }
      return PLUS_SUCCESS;
    }
  default:
    LOG_ERROR("Compute position signal value failed. Unknown frame type: "<<signal.frameType);
    return PLUS_FAIL;
  }
}

//-----------------------------------------------------------------------------
PlusStatus TemporalCalibration::ComputeCommonTimeRange()
{
  if (m_FixedSignal.frameList->GetNumberOfTrackedFrames()<1)
  {
    LOG_ERROR("Fixed signal frame list are empty");
    return PLUS_FAIL;
  }
  if (m_MovingSignal.frameList->GetNumberOfTrackedFrames()<1)
  {
    LOG_ERROR("Moving signal frame list are empty");
    return PLUS_FAIL;
  }

  double fixedTimestampMin = m_FixedSignal.frameList->GetTrackedFrame(0)->GetTimestamp();
  double fixedTimestampMax = m_FixedSignal.frameList->GetTrackedFrame(m_FixedSignal.frameList->GetNumberOfTrackedFrames()-1)->GetTimestamp();;
  double movingTimestampMin = m_MovingSignal.frameList->GetTrackedFrame(0)->GetTimestamp();
  double movingTimestampMax = m_MovingSignal.frameList->GetTrackedFrame(m_MovingSignal.frameList->GetNumberOfTrackedFrames()-1)->GetTimestamp();;
  
  double commonRangeMin = std::max(fixedTimestampMin, movingTimestampMin); 
  double commonRangeMax = std::min(fixedTimestampMax, movingTimestampMax);
  if (commonRangeMin + m_MaxMovingLagSec >= commonRangeMax - m_MaxMovingLagSec)
  {
    LOG_ERROR("Insufficient overlap between fixed and moving frames timestamps to compute time offset"); 
    return PLUS_FAIL;
  }
  
  m_FixedSignal.signalTimeRangeMin=commonRangeMin;
  m_FixedSignal.signalTimeRangeMax=commonRangeMax;
  m_MovingSignal.signalTimeRangeMin=commonRangeMin+m_MaxMovingLagSec;
  m_MovingSignal.signalTimeRangeMax=commonRangeMax-m_MaxMovingLagSec;
  
  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus TemporalCalibration::ComputeMovingSignalLagSec()
{

  // Need to determine the common signal range before extracting signals from the frames,
  // because normalization, PCA, etc. must be performed only by taking into account
  // the frames in the common range.
  if (ComputeCommonTimeRange()!=PLUS_SUCCESS)
  {
    return PLUS_FAIL;
  }

  // Compute the position signal values from the input frames
  if (ComputePositionSignalValues(m_FixedSignal)!=PLUS_SUCCESS)
  {
    LOG_ERROR("Failed to compute position signal from fixed frames");
    return PLUS_FAIL;
  }
  if (ComputePositionSignalValues(m_MovingSignal)!=PLUS_SUCCESS)
  {
    LOG_ERROR("Failed to compute position signal from moving frames");
    return PLUS_FAIL;
  }

  // Compute approximage image frame period. We will use this frame period as a step size in the coarse optimum search phase.
  double fixedTimestampMin = m_FixedSignal.signalTimestamps.at(0);
  double fixedTimestampMax = m_FixedSignal.signalTimestamps.at(m_FixedSignal.signalTimestamps.size() - 1);  
  if (m_FixedSignal.signalTimestamps.size()<2)
  {
    LOG_ERROR("Not enough video frames are available");
    return PLUS_FAIL;
  }
  double imageFramePeriodSec=(fixedTimestampMax-fixedTimestampMin)/(m_FixedSignal.signalTimestamps.size()-1);

  double searchRangeFineStep=imageFramePeriodSec*3;

  //  Compute cross correlation with sign convention #1 
  LOG_DEBUG("ComputeCorrelationBetweenFixedAndMovingSignal(sign convention #1)");
  double bestCorrelationValue=0;
  double bestCorrelationTimeOffset=0;
  double bestCorrelationNormalizationFactor=1.0;
  std::deque<double> corrTimeOffsets;
  std::deque<double> corrValues;
  ComputeCorrelationBetweenFixedAndMovingSignal(-m_MaxMovingLagSec,m_MaxMovingLagSec,imageFramePeriodSec,bestCorrelationValue,bestCorrelationTimeOffset,bestCorrelationNormalizationFactor, corrTimeOffsets, corrValues);
  std::deque<double> corrTimeOffsetsFine;
  std::deque<double> corrValuesFine;
  ComputeCorrelationBetweenFixedAndMovingSignal(bestCorrelationTimeOffset-searchRangeFineStep,bestCorrelationTimeOffset+searchRangeFineStep,m_SamplingResolutionSec,bestCorrelationValue,bestCorrelationTimeOffset,bestCorrelationNormalizationFactor, corrTimeOffsetsFine, corrValuesFine);
  LOG_DEBUG("Time offset with sign convention #1: " << bestCorrelationTimeOffset);
  
  //  Compute cross correlation with sign convention #2
  LOG_DEBUG("ComputeCorrelationBetweenFixedAndMovingSignal(sign convention #2)");
  // Mirror tracker metric signal about x-axis 
  for(long int i = 0; i < m_MovingSignal.signalValues.size(); ++i)
  {
    m_MovingSignal.signalValues.at(i) *= -1;
  }
  double bestCorrelationValueInvertedTracker=0;
  double bestCorrelationTimeOffsetInvertedTracker=0;
  double bestCorrelationNormalizationFactorInvertedTracker=1.0;
  std::deque<double> corrTimeOffsetsInvertedTracker;
  std::deque<double> corrValuesInvertedTracker;
  ComputeCorrelationBetweenFixedAndMovingSignal(-m_MaxMovingLagSec,m_MaxMovingLagSec,imageFramePeriodSec,bestCorrelationValueInvertedTracker,bestCorrelationTimeOffsetInvertedTracker,bestCorrelationNormalizationFactorInvertedTracker, corrTimeOffsetsInvertedTracker, corrValuesInvertedTracker);
  std::deque<double> corrTimeOffsetsInvertedTrackerFine;
  std::deque<double> corrValuesInvertedTrackerFine;
  ComputeCorrelationBetweenFixedAndMovingSignal(bestCorrelationTimeOffsetInvertedTracker-searchRangeFineStep,bestCorrelationTimeOffsetInvertedTracker+searchRangeFineStep,m_SamplingResolutionSec,bestCorrelationValueInvertedTracker,bestCorrelationTimeOffsetInvertedTracker,bestCorrelationNormalizationFactorInvertedTracker, corrTimeOffsetsInvertedTrackerFine, corrValuesInvertedTrackerFine);  
  LOG_DEBUG("Time offset with sign convention #2: " << bestCorrelationTimeOffsetInvertedTracker);
  
	// Adopt the smallest tracker lag
  if(std::abs(bestCorrelationTimeOffset) < std::abs(bestCorrelationTimeOffsetInvertedTracker))
  {
    m_TrackerLagSec = bestCorrelationTimeOffset;
    m_BestCorrelationTimeOffset = bestCorrelationTimeOffset;
    m_BestCorrelationValue = bestCorrelationValue;
		m_BestCorrelationNormalizationFactor = bestCorrelationNormalizationFactor;
    m_CorrTimeOffsets=corrTimeOffsets;
    m_CorrValues=corrValues;
    m_CorrTimeOffsetsFine=corrTimeOffsetsFine;
    m_CorrValuesFine=corrValuesFine;

    // Flip tracker metric signal back to correspond to sign convention #1 
    for(long int i = 0; i < m_MovingSignal.signalValues.size(); ++i)
    {
      m_MovingSignal.signalValues.at(i) *= -1;
    }
  }
  else
  {
    m_TrackerLagSec = bestCorrelationTimeOffsetInvertedTracker;
    m_BestCorrelationTimeOffset = bestCorrelationTimeOffsetInvertedTracker;
    m_BestCorrelationValue = bestCorrelationValueInvertedTracker;
		m_BestCorrelationNormalizationFactor = bestCorrelationNormalizationFactorInvertedTracker;
    m_CorrTimeOffsets=corrTimeOffsetsInvertedTracker;
    m_CorrValues=corrValuesInvertedTracker;
    m_CorrTimeOffsetsFine=corrTimeOffsetsInvertedTrackerFine;
    m_CorrValuesFine=corrValuesInvertedTrackerFine;
  }
	   
  //NormalizeMetricValues(m_FixedSignal.signalValues, m_FixedSignalValuesNormalizationFactor, m_MovingSignal.signalTimestamps.front()-m_TrackerLagSec, m_MovingSignal.signalTimestamps.back()-m_TrackerLagSec, m_FixedSignal.signalTimestamps);

  // Normalize the tracker metric based on the best index offset (only considering the overlap "window"
	for(int i = 0; i < m_MovingSignal.signalTimestamps.size(); ++i)
	{
    if(m_MovingSignal.signalTimestamps.at(i) > m_FixedSignal.signalTimestamps.at(0) + m_TrackerLagSec && m_MovingSignal.signalTimestamps.at(i) < m_FixedSignal.signalTimestamps.at(m_FixedSignal.signalTimestamps.size() -1) + m_TrackerLagSec)
		{
      m_MovingSignal.normalizedSignalValues.push_back(m_MovingSignal.signalValues.at(i));
			m_MovingSignal.normalizedSignalTimestamps.push_back(m_MovingSignal.signalTimestamps.at(i));
		}
	}

	// Get a normalized tracker position metric that can be displayed
	 double unusedNormFactor=1.0;
	 NormalizeMetricValues(m_MovingSignal.normalizedSignalValues, unusedNormFactor);

	m_CalibrationError=sqrt(-m_BestCorrelationValue)/m_BestCorrelationNormalizationFactor; // RMSE in mm

  LOG_DEBUG("Tracker stream lags image stream by: " << m_TrackerLagSec << " [s]");


  // Get maximum calibration error

	//	Get the timestamps of the sliding signal (i.e. cropped video signal) shifted by the best-found offset
	std::deque<double> shiftedSlidingSignalTimestamps;
	for(int i = 0; i < m_FixedSignal.signalTimestamps.size(); ++i)
	{
		shiftedSlidingSignalTimestamps.push_back(m_FixedSignal.signalTimestamps.at(i) + m_TrackerLagSec); // TODO: check this
	}

	//	Get the values of the tracker metric at the offset sliding signal values

	//	Constuct piecewise function for tracker signal
	vtkSmartPointer<vtkPiecewiseFunction> trackerPositionPiecewiseSignal = vtkSmartPointer<vtkPiecewiseFunction>::New();
	double midpoint = 0.5;
	double sharpness = 0;
	for(int i = 0; i < m_MovingSignal.normalizedSignalTimestamps.size(); ++i)
	{
		trackerPositionPiecewiseSignal->AddPoint(m_MovingSignal.normalizedSignalTimestamps.at(i), m_MovingSignal.normalizedSignalValues.at(i), midpoint, sharpness);
	}

	std::deque<double> resampledNormalizedTrackerPositionMetric;
	ResampleSignalLinearly(shiftedSlidingSignalTimestamps,trackerPositionPiecewiseSignal,resampledNormalizedTrackerPositionMetric);

  for(long int i = 0; i < resampledNormalizedTrackerPositionMetric.size(); ++i)
  {
    double diff = resampledNormalizedTrackerPositionMetric.at(i) - m_FixedSignal.signalValues.at(i); //SSD
    m_CalibrationErrorVector.push_back(diff*diff); 
  }

  m_MaxCalibrationError = 0;
  for(long int i = 0; i < m_CalibrationErrorVector.size(); ++i)
  {
    if(m_CalibrationErrorVector.at(i) > m_MaxCalibrationError)
    {
      m_MaxCalibrationError = m_CalibrationErrorVector.at(i);
    }
  }

  m_MaxCalibrationError = std::sqrt(m_MaxCalibrationError)/m_BestCorrelationNormalizationFactor;

  m_NeverUpdated = false;

  if( m_BestCorrelationValue <= SIGNAL_ALIGNMENT_METRIC_THRESHOLD[SIGNAL_ALIGNMENT_METRIC] )
  {
    LOG_ERROR("Calculated correlation exceeds threshold value. This may be an indicator of a poor calibration.");
    return PLUS_FAIL;
  }

  LOG_DEBUG("Temporal calibration BestCorrelationValue = "<<m_BestCorrelationValue<<" (threshold="<<SIGNAL_ALIGNMENT_METRIC_THRESHOLD[SIGNAL_ALIGNMENT_METRIC]<<")");
  LOG_DEBUG("MaxCalibrationError="<<m_MaxCalibrationError);
  LOG_DEBUG("CalibrationError="<<m_CalibrationError);
  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus TemporalCalibration::ConstructTableSignal(std::deque<double> &x, std::deque<double> &y, vtkTable* table,
                                               double timeCorrection)
{
  // Clear table
  while (table->GetNumberOfColumns() > 0)
  {
    table->RemoveColumn(0);
  }

  //  Create array correpsonding to the time values of the tracker plot
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
void TemporalCalibration::SetVideoClipRectangle(int clipRectangleOrigin[2], int clipRectangleSize[2])
{
  m_LineSegmentationClipRectangleOrigin[0]=clipRectangleOrigin[0];
  m_LineSegmentationClipRectangleOrigin[1]=clipRectangleOrigin[1];
  m_LineSegmentationClipRectangleSize[0]=clipRectangleSize[0];
  m_LineSegmentationClipRectangleSize[1]=clipRectangleSize[1];
}
