/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#include "vtkTransformRepository.h"
#include "TemporalCalibrationAlgo.h"
#include "vtkMath.h"
#include "RANSAC.h"
#include "PlaneParametersEstimator.h"


// Line detection parameters
const int HOUGH_DISC_RADIUS = 10; // TODO: find out the unit (pixel/mm)
const int NUMBER_OF_LINES_TO_DETECT = 1;
const float HOUGH_ANGLE_RESOLUTION = 100; // TODO: find out the unit (deg/rad/other?)

// Default algorithm parameters
// TODO: have separate SIGNAL P2P for video and tracker (one in mm the other in pixels)
const double MINIMUM_SIGNAL_PEAK_TO_PEAK = 0.01; // If either tracker metric "swings" less than this, abort
const double TIMESTAMP_EPSILON_SEC = 0.0001; // Temporal resolution below which two time values are considered identical
const double MINIMUM_SAMPLING_RESOLUTION_SEC = 0.00001; // The maximum resolution that the user can request
const double DEFAULT_SAMPLING_RESOLUTION_SEC = 0.001; 
const double DEFAULT_MAX_TRACKER_LAG_SEC = 2;
const std::string DEFAULT_PROBE_TO_REFERENCE_TRANSFORM_NAME = "ProbeToReference";
const double IMAGE_DOWSAMPLING_FACTOR_X = 4; // new resolution_x = old resolution_x/ IMAGE_DOWSAMPLING_FACTOR_X
const double IMAGE_DOWSAMPLING_FACTOR_Y = 4; // new resolution_y = old resolution_y/ IMAGE_DOWSAMPLING_FACTOR_Y
//-----------------------------------------------------------------------------
TemporalCalibration::TemporalCalibration() : m_SamplingResolutionSec(DEFAULT_SAMPLING_RESOLUTION_SEC),
                                             m_MaxTrackerLagSec(DEFAULT_MAX_TRACKER_LAG_SEC),
                                             m_ProbeToReferenceTransformName(DEFAULT_PROBE_TO_REFERENCE_TRANSFORM_NAME),
                                             m_NeverUpdated(true)
{
  /* TODO: Switching to VTK table data structure */
  m_TrackerTable = vtkSmartPointer<vtkTable>::New();
  m_VideoTable = vtkSmartPointer<vtkTable>::New();
  m_TrackerTimestampedMetric = vtkSmartPointer<vtkTable>::New();
  
}

//-----------------------------------------------------------------------------
// TODO: Finish error checking et cetera
PlusStatus TemporalCalibration::Update()
{
  // Check if video frames have been assigned
  if(m_VideoFrames == NULL)
  {
    LOG_ERROR("Video data is not assigned");
    return PLUS_FAIL;
  }

  //  Check if tracker frames have been assigned
  if(m_TrackerFrames == NULL)
  {
    LOG_ERROR("US video data is not assigned");
    return PLUS_FAIL;
  }

  //  Make sure video frame list is not empty
  if(m_VideoFrames->GetNumberOfTrackedFrames() == 0)
  {
    LOG_ERROR("Video data contains no frames");
    return PLUS_FAIL;
  }

  //  Make sure video frame list is not empty
  if(m_TrackerFrames->GetNumberOfTrackedFrames() == 0)
  {
    LOG_ERROR("US tracker data contains no frames");
    return PLUS_FAIL;
  }

   //  Check that all the image data is valid
  int totalNumberOfInvalidVideoFrames = 0;
  int greatestNumberOfConsecutiveInvalidVideoFrames = 0;
  int currentNumberOfConsecutiveInvalidVideoFrames = 0;
  for(int i = 0 ; i < m_VideoFrames->GetNumberOfTrackedFrames(); ++i)
  {
    if(!m_VideoFrames->GetTrackedFrame(i)->GetImageData()->IsImageValid())
    {
      ++totalNumberOfInvalidVideoFrames;
      ++currentNumberOfConsecutiveInvalidVideoFrames;
    }
    else
    {
      if(currentNumberOfConsecutiveInvalidVideoFrames > greatestNumberOfConsecutiveInvalidVideoFrames)
      {
        greatestNumberOfConsecutiveInvalidVideoFrames = currentNumberOfConsecutiveInvalidVideoFrames ;
      } 
      currentNumberOfConsecutiveInvalidVideoFrames = 0;
    }
  }

  // TODO: print a warning if totalNumberOfInvalidVideoFrames or currentNumberOfConsecutiveInvalidVideoFrames is too high

  if(m_SamplingResolutionSec < MINIMUM_SAMPLING_RESOLUTION_SEC)
  {
    LOG_ERROR("Specified resampling resolution ("<<m_SamplingResolutionSec<<" seconds) is too small. Sampling resolution must be greater than: " << 
      MINIMUM_SAMPLING_RESOLUTION_SEC << " seconds");
    return PLUS_FAIL;
  }

  ComputeTrackerLagSec();

  m_NeverUpdated = false;

  return PLUS_SUCCESS;
  /* TODO: Maybe output an warning message. */
  
  
}

//-----------------------------------------------------------------------------
void TemporalCalibration::SetTrackerFrames(const vtkSmartPointer<vtkTrackedFrameList> trackerFrames)
{
  m_TrackerFrames = trackerFrames;
}

//-----------------------------------------------------------------------------
void TemporalCalibration::SetVideoFrames(const vtkSmartPointer<vtkTrackedFrameList> videoFrames)
{
    m_VideoFrames = videoFrames;
}

//-----------------------------------------------------------------------------
void TemporalCalibration::setSamplingResolutionSec(double samplingResolutionSec)
{
  m_SamplingResolutionSec = samplingResolutionSec;
}

//-----------------------------------------------------------------------------
void TemporalCalibration::SetProbeToReferenceTransformName(const std::string& transformName)
{
  m_ProbeToReferenceTransformName = transformName;
}

//-----------------------------------------------------------------------------
void TemporalCalibration::SetMaximumVideoTrackerLagSec(double maxLagSec)
{
  m_MaxTrackerLagSec = maxLagSec;
}

//-----------------------------------------------------------------------------
PlusStatus TemporalCalibration::GetTrackerLagSec(double &lag)
{
  if(m_NeverUpdated)
  {
    LOG_ERROR("You must call the Update() to compute the tracker lag.");
    return PLUS_FAIL;
  }
  lag=m_TrackerLagSec;
  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus TemporalCalibration::ComputeTrackerPositionMetric()
{
  vtkSmartPointer<vtkTransformRepository> transformRepository = vtkSmartPointer<vtkTransformRepository>::New();
  PlusTransformName transformName;
  if (transformName.SetTransformName(m_ProbeToReferenceTransformName.c_str())!=PLUS_SUCCESS)
  {
    LOG_ERROR("Cannot compute tracker position metric, transform name is invalid ("<<m_ProbeToReferenceTransformName<<")");
    return PLUS_FAIL;
  }
  
  //  For each tracker position in the recorded tracker sequence, get its translation from reference.
  for ( int frame = 0; frame < m_TrackerFrames->GetNumberOfTrackedFrames(); ++frame )
  {
    TrackedFrame *trackedFrame = m_TrackerFrames->GetTrackedFrame(frame);
    transformRepository->SetTransforms(*trackedFrame);
    vtkSmartPointer<vtkMatrix4x4> probeToReferenceTransform = vtkSmartPointer<vtkMatrix4x4>::New();
    bool valid = false;
    transformRepository->GetTransform(transformName, probeToReferenceTransform, &valid);
    if (!valid)
    {
      // There is no available transform for this frame; skip that frame
      continue;
    }  
    //  Get the Euclidean probe-to-reference distance = (tx^2 + ty^2 + tz^2)^(1/2)   
    // TODO: compute the distance from the mean position instead of the reference coordinate frame origin
    double trackerTranslationDistance=0; //  Euclidean translation of tracker from reference.
    for(int i = 0; i < 3; ++i)
    {
      trackerTranslationDistance += probeToReferenceTransform->GetElement(i, 3) * 
                                   probeToReferenceTransform->GetElement(i, 3);
    }    
    trackerTranslationDistance = std::sqrt(trackerTranslationDistance);
    m_TrackerPositionMetric.push_back(trackerTranslationDistance);
    m_TrackerTimestamps.push_back(trackedFrame->GetTimestamp());
  }

  NormalizeMetric(m_TrackerPositionMetric);

  /* TODO: Switching to VTK table data structure */
  /*
  vtkSmartPointer<vtkDoubleArray> trackerTimestampsArray = vtkSmartPointer<vtkDoubleArray>::New();
  trackerTimestampsArray->SetName("Tracker Timestamps [s]"); 
  
  vtkSmartPointer<vtkDoubleArray> trackerPositionMetricArray = vtkSmartPointer<vtkDoubleArray>::New();
  trackerPositionMetricArray->SetName("Tracker Position Metric"); 
  
  m_TrackerTimestampedMetric->AddColumn(trackerTimestampsArray);
  m_TrackerTimestampedMetric->AddColumn(trackerPositionMetricArray);

  m_TrackerTimestampedMetric->SetNumberOfRows(m_TrackerPositionMetric.size());

  for(int i = 0; i < m_TrackerTimestampedMetric->GetNumberOfRows(); ++i)
  {
    m_TrackerTimestampedMetric->SetValue(i, 0, m_TrackerTimestamps.at(i));
    m_TrackerTimestampedMetric->SetValue(i, 1, m_TrackerPositionMetric.at(i));
  }

  NormalizeTableColumn( m_TrackerTimestampedMetric, 1);
  */
  return PLUS_SUCCESS;

}

//-----------------------------------------------------------------------------
PlusStatus TemporalCalibration::ComputeVideoPositionMetric()
{

  //  For each video frame, detect line and extract mindpoint and slope parameters
  for(int frameNumber = 0; frameNumber < m_VideoFrames->GetNumberOfTrackedFrames(); ++frameNumber)
  {
    LOG_DEBUG("Calculating video position metric for frame " << frameNumber);

    // Define image types and image dimension
    const int imageDimension=2;
    typedef unsigned char charPixelType; // The natural type of the input image
    typedef float floatPixelType; //  The type of pixel used for the Hough accumulator
    typedef itk::Image<charPixelType,imageDimension> charImageType;
    
    
    //  Get timestamp for image frame
    m_VideoTimestamps.push_back(m_VideoFrames->GetTrackedFrame(frameNumber)->GetTimestamp());

    // Get curent image
    charImageType::Pointer localImage = m_VideoFrames->GetTrackedFrame(frameNumber)->GetImageData()->GetImage<charPixelType>();

    if(localImage.IsNull())
    {
      // Dropped frame
      continue;
    }

    charImageType::RegionType region = localImage->GetLargestPossibleRegion();
    
    charImageType::IndexType startPixel;
    startPixel[0] = static_cast<int>(region.GetSize()[0] / 2.0);
    startPixel[1] = 0;

    charImageType::IndexType endPixel;
    endPixel[0] = startPixel[0];
    endPixel[1] = region.GetSize()[1] - 1;
   
    std::vector<int> intensityProfile; // Holds intensity profile of the line
    itk::LineIterator<charImageType> it(localImage, startPixel, endPixel);
    it.GoToBegin();
    while (!it.IsAtEnd())
    {
      intensityProfile.push_back((int)it.Get());
      it.Set(255);
      ++it;
    }
    
   bool writeSampleLineImageToFile = vtkPlusLogger::Instance()->GetLogLevel()>=vtkPlusLogger::LOG_LEVEL_TRACE;
   if(writeSampleLineImageToFile)
   {
    // Write image showing the sampling line to file
    std::ostrstream downsampledVideoFrameFilename;
    downsampledVideoFrameFilename << "lineImage" << std::setw(3) << std::setfill('0') << frameNumber << ".bmp" << std::ends;
    PlusVideoFrame::SaveImageToFile(localImage , downsampledVideoFrameFilename.str());
   }

   bool plotIntensityProfile = vtkPlusLogger::Instance()->GetLogLevel()>=vtkPlusLogger::LOG_LEVEL_TRACE;
   if(plotIntensityProfile)
   {
    // Plot the intensity profile
    plotIntArray(intensityProfile);
   }

    // Find the max intensity value from the peak with the largest area
    int MaxFromLargestArea = -1;
    int MaxFromLargestAreaIndex = -1;
    int startOfMaxArea = -1;
    FindLargestPeak(intensityProfile, MaxFromLargestArea, MaxFromLargestAreaIndex,startOfMaxArea);
    
    LOG_TRACE("Max intensity value: " << MaxFromLargestArea);
    LOG_TRACE("Max intensity index: " << MaxFromLargestAreaIndex);

    /* Using 50% max-peak starting point*/
    double centerOfGravity = -1;
    ComputeCenterOfGravity(intensityProfile, startOfMaxArea, centerOfGravity);
    m_VideoPositionMetric.push_back(centerOfGravity);
    LOG_TRACE("Center of gravity of max-peak: " << centerOfGravity);

    /* Using 50% max-peak starting point*/
    //int startOfPeak = -1;
    //FindPeakStart(intensityProfile,MaxFromLargestArea, startOfMaxArea, startOfPeak);
    //m_VideoPositionMetric.push_back(startOfPeak);
    //LOG_TRACE("Fifty-percent peak start: " << startOfPeak);

                                            
  }// end frameNum loop
  
   bool plotVideoMetric = vtkPlusLogger::Instance()->GetLogLevel()>=vtkPlusLogger::LOG_LEVEL_TRACE;
   if(plotVideoMetric)
   {
    plotDoubleArray(m_VideoPositionMetric);
   }


  //  Normalize the video metric
  NormalizeMetric(m_VideoPositionMetric);

  
  return PLUS_SUCCESS;

} //  End LineDetection

//-----------------------------------------------------------------------------

PlusStatus TemporalCalibration::FindPeakStart(std::vector<int> &intensityProfile,int MaxFromLargestArea,
                                              int startOfMaxArea, int &startOfPeak)

{
  // Start of peak is defined as the location at which it reaches 50% of its maximum value.

  double startPeakValue = MaxFromLargestArea * 0.50;

   int pixelIndex = startOfMaxArea;

  while( intensityProfile.at(pixelIndex) <= startPeakValue)
  {
    ++pixelIndex;
  }
  
  startOfPeak = --pixelIndex;

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

PlusStatus TemporalCalibration::FindLargestPeak(std::vector<int> &intensityProfile,int &MaxFromLargestArea,
                                                int &MaxFromLargestAreaIndex, int &startOfMaxArea)
{
  int currentLargestArea = 0;
  int currentArea = 0;
  int currentMaxFromLargestArea = 0;
  int currentMaxFromLargestAreaIndex = 0;
  int currentMax = 0;
  int currentMaxIndex = 0;
  bool underPeak = false;
  int currentStartOfMaxArea = 0;
  int currentStart = 0;

  for(int pixelLoc = 0; pixelLoc < intensityProfile.size(); ++pixelLoc)
  {
    if(intensityProfile.at(pixelLoc) > 0 && !underPeak)
    {
      underPeak = true;
      currentMax = intensityProfile.at(pixelLoc);
      currentMaxIndex = pixelLoc;
      currentArea = intensityProfile.at(pixelLoc);
      currentStart = pixelLoc;
    }

    if(intensityProfile.at(pixelLoc) > 0 && underPeak)
    {
      currentArea += intensityProfile.at(pixelLoc);
      
      if(intensityProfile.at(pixelLoc) > currentMax)
      {
        currentMax = intensityProfile.at(pixelLoc);
        currentMaxIndex = pixelLoc;
      }
    }

    if(intensityProfile.at(pixelLoc) == 0 && underPeak)
    {
      underPeak = false;
      if(currentArea > currentLargestArea)
      {
        currentLargestArea = currentArea;
        currentMaxFromLargestArea = currentMax;
        currentMaxFromLargestAreaIndex = currentMaxIndex;
        currentStartOfMaxArea = currentStart;
      }
    }
  } //end loop through intensity profile

  MaxFromLargestArea = currentMaxFromLargestArea;
  MaxFromLargestAreaIndex = currentMaxFromLargestAreaIndex;
  startOfMaxArea = currentStartOfMaxArea;

  return PLUS_SUCCESS;
}
//-----------------------------------------------------------------------------

PlusStatus TemporalCalibration::ComputeCenterOfGravity(std::vector<int> &intensityProfile, int startOfMaxArea, 
                                                       double &centerOfGravity)
{
 int pixelLoc = startOfMaxArea;
 int pointsInPeak = 0;
 double intensitySum = 0;
 while(intensityProfile.at(pixelLoc) != 0)
 {
  intensitySum += pixelLoc * intensityProfile.at(pixelLoc);
  pointsInPeak += intensityProfile.at(pixelLoc);
  ++pixelLoc;
 }

 if(pointsInPeak == 0)
 {
   // no peak!
   return PLUS_FAIL;
 }

 centerOfGravity = intensitySum / pointsInPeak;

 return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus TemporalCalibration::NormalizeMetric(std::vector<double> &metric)
{
  if (metric.size()==0)
  {
    LOG_ERROR("NormalizeMetric failed, the metric vector is empty");
    return PLUS_FAIL;
  }

  //  Calculate the metric mean
  double mu = 0;
  for(int i = 0; i < metric.size(); ++i)
  {
    mu += metric.at(i);
  }
  mu /= metric.size();

  //  Subtract the metric mean from each metric value as: s' = s - mu
  for(int i = 0; i < metric.size(); ++i)
  {
    metric.at(i) -= mu;
  }

  //  Calculate maximum and minimum metric values
  double maxMetricValue = metric.at(0);
  double minMetricValue = metric.at(0);

  for(int i = 1; i < metric.size(); ++i)
  {
    if(metric.at(i) > maxMetricValue)
    {
      maxMetricValue = metric.at(i);
    }
    else if(metric.at(i) < minMetricValue)
    {
       minMetricValue = metric.at(i);
    }
  }

  // Normalize the signal between [-0.5, +0.5]--i.e. maximum of unity amplitude--by dividing by max peak-to-peak
  double maxPeakToPeak = std::abs(maxMetricValue) + std::abs(minMetricValue);

  // If the metric values do not "swing" sufficiently, the signal is considered constant--i.e. infinite period--and will
  // not work for our purposes
  if(maxPeakToPeak < MINIMUM_SIGNAL_PEAK_TO_PEAK)
  {
    LOG_ERROR("Detected metric values do not vary sufficiently--i.e. signal is constant");
    return PLUS_FAIL;
  }

  for(int i = 0; i < metric.size(); ++i)
  {
    metric.at(i) /= maxPeakToPeak; 
  }

  return PLUS_SUCCESS;
}// End NormalizeMetric()


//-----------------------------------------------------------------------------
void TemporalCalibration::NormalizeTableColumn(vtkSmartPointer<vtkTable> table, int column)
{
  //  Get the mean of the Euclidean distances
  double mu = 0;
  for(int i = 0; i < table->GetNumberOfRows(); ++i)
  {
    mu += table->GetValue(i,column).ToDouble();
  }

  mu /= table->GetNumberOfRows();

  //  Normalize each measurement, s, as s' = (s-mu)/sigma
  for(int i = 0; i < table->GetNumberOfRows(); ++i)
  {
    table->SetValue(i,column, table->GetValue(i,column).ToDouble() - mu);
  }

  //  Get the maximum and minimum signal values
  double maxVal = table->GetValue(0,column).ToDouble();
  double minVal = table->GetValue(0,column).ToDouble();

  for(int i = 1; i < table->GetNumberOfRows(); ++i)
  {
    if(table->GetValue(i,column) > maxVal)
    {
      maxVal = table->GetValue(i,column).ToDouble();
    }
    else if(table->GetValue(i,column) < minVal)
    {
       minVal = table->GetValue(i,column).ToDouble();
    }
  }

  // normalize signal
  double normFactor = std::abs(maxVal) + std::abs(minVal);

  for(int i = 0; i < table->GetNumberOfRows(); ++i)
  {
    table->SetValue(i,column, table->GetValue(i,column).ToDouble() / normFactor);
  }

}// End NormalizeMetric()


//-----------------------------------------------------------------------------
PlusStatus TemporalCalibration::ResamplePositionMetrics()
{
  if (m_TrackerTimestamps.size()==0)
  {
    LOG_ERROR("ResamplePositionMetrics failed, the TrackerTimestamps vector is empty");
    return PLUS_FAIL;
  }

  //  Find the time-range that is common to both tracker and image signals
  double translationTimestampMin = m_TrackerTimestamps.at(0);
  double translationTimestampMax = m_TrackerTimestamps.at(m_TrackerTimestamps.size() - 1);

  double imageTimestampMin = m_VideoTimestamps.at(0);
  double imageTimestampMax = m_VideoTimestamps.at(m_VideoTimestamps.size() - 1);

  double commonRangeMin = std::max(imageTimestampMin, translationTimestampMin); 
  double commonRangeMax = std::min(imageTimestampMax, translationTimestampMax);

  if (commonRangeMin + m_MaxTrackerLagSec >= commonRangeMax - m_MaxTrackerLagSec)
  {
    LOG_ERROR("Insufficient overlap between tracking data and image data to compute time offset"); 
    return PLUS_FAIL;
  }

  //  Get resampled timestamps for the video sequence
  long int n = 0;  
  while(commonRangeMin + n * m_SamplingResolutionSec < commonRangeMax)
  {
    m_ResampledVideoTimestamps.push_back(commonRangeMin + n * m_SamplingResolutionSec);
    ++n;
  }

  //  Get resampled timestamps for the tracker sequence
  n = 0;
  while((commonRangeMin + m_MaxTrackerLagSec) + n * m_SamplingResolutionSec < commonRangeMax - m_MaxTrackerLagSec)
  {
    m_ResampledTrackerTimestamps.push_back( (commonRangeMin + m_MaxTrackerLagSec) + n * m_SamplingResolutionSec);
    ++n;
  }
  
  //  Get resampled position metric for the US video data
  LOG_DEBUG("InterpolatePositionMetric for video data");
  InterpolatePositionMetric(m_VideoTimestamps, m_ResampledVideoTimestamps, m_VideoPositionMetric, m_ResampledVideoPositionMetric);
  LOG_DEBUG("InterpolatePositionMetric for tracker data");
  InterpolatePositionMetric(m_TrackerTimestamps, m_ResampledTrackerTimestamps, m_TrackerPositionMetric, m_ResampledTrackerPositionMetric);
            
  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
void TemporalCalibration::InterpolatePositionMetric(const std::vector<double> &originalTimestamps,
                                                    const std::vector<double> &resampledTimestamps,
                                                    const std::vector<double> &originalMetric,
                                                    std::vector<double> &resampledPositionMetric)
                                                                 
{
  std::vector<int> lowerStraddleIndices;
  std::vector<int> upperStraddleIndices;
  GetStraddleIndices(originalTimestamps, resampledTimestamps, lowerStraddleIndices, upperStraddleIndices);

  for(long int resampledTimeValueIndex = 0; resampledTimeValueIndex < resampledTimestamps.size(); ++resampledTimeValueIndex)
  {
    resampledPositionMetric.push_back(LinearInterpolation(resampledTimestamps.at(resampledTimeValueIndex), originalTimestamps, 
                                                             originalMetric, 
                                                             lowerStraddleIndices.at(resampledTimeValueIndex), 
                                                             upperStraddleIndices.at(resampledTimeValueIndex)));
  }                                            
}

//-----------------------------------------------------------------------------
double TemporalCalibration::LinearInterpolation(double resampledTimeValue, const std::vector<double> &originalTimestamps, 
                                                   const std::vector<double> &originalMetric, int lowerStraddleIndex, int upperStraddleIndex)
                                                    
{
  //  If the time index between the two straddling indices is very small, avoid interpolation (to
  //  prevent divding by a really small number).
  if(std::abs(originalTimestamps.at(upperStraddleIndex) - originalTimestamps.at(lowerStraddleIndex)) < TIMESTAMP_EPSILON_SEC)
    return originalMetric.at(upperStraddleIndex);
  
  // Peform linear interpolation
  double m = (originalMetric.at(upperStraddleIndex) - originalMetric.at(lowerStraddleIndex)) / 
             (originalTimestamps.at(upperStraddleIndex) - originalTimestamps.at(lowerStraddleIndex));

  return originalMetric.at(lowerStraddleIndex) + (resampledTimeValue - originalTimestamps.at(lowerStraddleIndex)) * m;

} // End linearInterpolation()

//-----------------------------------------------------------------------------
void TemporalCalibration::GetStraddleIndices(const std::vector<double> &originalTimestamps, const std::vector<double> &resampledTimestamps, 
                                             std::vector<int> &lowerStraddleIndices, std::vector<int> &upperStraddleIndices)
{
  //  Find the straddle indices; that is, the indices of the original timestamp series that "straddle", or lie on either side of,
  //  the first resampled timestamp value.
  int currLowerStraddleIndex = FindFirstLowerStraddleIndex(originalTimestamps, resampledTimestamps.at(0) );
  int currUpperStraddleIndex = FindFirstUpperStraddleIndex(originalTimestamps, resampledTimestamps.at(0) );

  lowerStraddleIndices.push_back(currLowerStraddleIndex);
  upperStraddleIndices.push_back(currUpperStraddleIndex);

  //  For each subsequent resampled timestamp value, find the straddle indices
  for(long int timestampIndex = 0; timestampIndex < resampledTimestamps.size(); ++timestampIndex)
  {
    currLowerStraddleIndex = FindSubsequentLowerStraddleIndex(originalTimestamps, resampledTimestamps.at(timestampIndex), currLowerStraddleIndex);
    lowerStraddleIndices.push_back(currLowerStraddleIndex);

    currUpperStraddleIndex = FindSubsequentUpperStraddleIndex(originalTimestamps, resampledTimestamps.at(timestampIndex), currUpperStraddleIndex);
    upperStraddleIndices.push_back(currUpperStraddleIndex);
  }

}

//-----------------------------------------------------------------------------
int TemporalCalibration::FindFirstLowerStraddleIndex(const std::vector<double> &originalTimestamps, double resampledTimestamp)                                                    
{
  if(originalTimestamps.size()==0)
  {
    LOG_WARNING("FindFirstLowerStraddleIndex failed, timestamps vector is empty");
    return 0;
  }

  //  No original timestamps lie below the desired resampled timestamp--cannot interpolate
  if(originalTimestamps.at(0) > resampledTimestamp)
  {
    LOG_WARNING("FindFirstLowerStraddleIndex failed to find a value within the valid time range, cannot interpolate");
    return 0;
  }

  int currIndex = 0;
  while(currIndex < originalTimestamps.size()  && originalTimestamps.at(currIndex) <= resampledTimestamp)
  {
    ++currIndex;
  }
  --currIndex;

  return currIndex;
}

//-----------------------------------------------------------------------------
int TemporalCalibration::FindFirstUpperStraddleIndex(const std::vector<double> &originalTimestamps, double resampledTimestamp)
{
  if(originalTimestamps.size()==0)
  {
    LOG_WARNING("FindFirstUpperStraddleIndex failed, timestamps vector is empty");
    return 0;
  }
  //  No original timestamps lie past the desired resampled timestamp--cannot interpolate
  if(originalTimestamps.at(originalTimestamps.size() - 1) < resampledTimestamp)
  {
    LOG_WARNING("FindFirstUpperStraddleIndex failed to find a value within the valid time range, cannot interpolate");
    return originalTimestamps.size() - 1;
  }

  int currIndex = originalTimestamps.size() - 1;
  while(currIndex >= 0 && originalTimestamps.at(currIndex) >= resampledTimestamp)
  {
    --currIndex;
  }
  ++currIndex;

  return currIndex;
}

//-----------------------------------------------------------------------------
int TemporalCalibration::FindSubsequentLowerStraddleIndex(const std::vector<double> &originalTimestamps, double resampledTimestamp,
                                                       int currLowerStraddleIndex)
{  
    int currIndex = currLowerStraddleIndex;
    while(currIndex < originalTimestamps.size() && originalTimestamps.at(currIndex) <= resampledTimestamp)
    {
     ++currIndex;
    }
    --currIndex;
    return currIndex;
}

//-----------------------------------------------------------------------------
int TemporalCalibration::FindSubsequentUpperStraddleIndex(const std::vector<double> &originalTimestamps, double resampledTimestamp,
                                                       int currUpperStraddleIndex)
{
    if(originalTimestamps.at(currUpperStraddleIndex) >= resampledTimestamp)
    {
      return currUpperStraddleIndex;
    }
    int currIndex = currUpperStraddleIndex;
    while(currIndex < originalTimestamps.size()  && originalTimestamps.at(currIndex) < resampledTimestamp)
    {
      ++currIndex;
    }
    return currIndex;
}

//-----------------------------------------------------------------------------
void TemporalCalibration::ComputeCrossCorrelationBetweenVideoAndTrackerMetrics()
{
  int trackerLagIndex = 0;
  while(trackerLagIndex + (m_ResampledTrackerPositionMetric.size() - 1) < m_ResampledVideoPositionMetric.size())
  {
    m_CorrValues.push_back(ComputeCorrelationSumForGivenLagIndex(m_ResampledTrackerPositionMetric, m_ResampledVideoPositionMetric, trackerLagIndex));
    ++trackerLagIndex;
  }
}

//-----------------------------------------------------------------------------
double TemporalCalibration::ComputeCorrelationSumForGivenLagIndex(const std::vector<double> &metricA, const std::vector<double> &metricB, int indexOffset)
{
  double multipliedSignalSum = 0;
  for(long int i = 0; i < metricA.size(); ++i)
  {
    multipliedSignalSum += metricA.at(i) * metricB.at(i + indexOffset);
  }
  return multipliedSignalSum;
}

//-----------------------------------------------------------------------------
void TemporalCalibration::ComputeTrackerLagSec()
{
  // Calculate the (normalized) metrics for the video and tracker data streams
  ComputeTrackerPositionMetric();
  ComputeVideoPositionMetric();
  
  // Resample the image and tracker metrics; this prepares the two signals for cross correlation  
  ResamplePositionMetrics();

  //  Compute cross correlation
  LOG_DEBUG("ComputeCrossCorrelationBetweenVideoAndTrackerMetrics");
  ComputeCrossCorrelationBetweenVideoAndTrackerMetrics();
  
  // Find the index offset corresponding to the maximum correlation sum
  double maxCorrVal = m_CorrValues.at(0);
  int maxCorrIndex = 0;
  for(int i = 1; i < m_CorrValues.size(); ++i)
  {
    if(m_CorrValues.at(i) > maxCorrVal)
    {
      maxCorrVal = m_CorrValues.at(i);
      maxCorrIndex = i;
    }
  }

  // Compute the time that the tracker data lags the video data using the maximum index
  m_TrackerLagSec = m_MaxTrackerLagSec - (maxCorrIndex)*m_SamplingResolutionSec;

  LOG_DEBUG("Tracker stream lags image stream by: " << m_TrackerLagSec << " [s]");

}

//-----------------------------------------------------------------------------
void TemporalCalibration::plotIntArray(std::vector<int> intensityValues)
{

  //  Create table
  vtkSmartPointer<vtkTable> table = vtkSmartPointer<vtkTable>::New();

  //  Create array correpsonding to the time values of the tracker plot
  vtkSmartPointer<vtkIntArray> arrPixelPositions = vtkSmartPointer<vtkIntArray>::New();
  arrPixelPositions->SetName("Pixel Positions"); 
  table->AddColumn(arrPixelPositions);
 
  //  Create array corresponding to the metric values of the tracker plot
  vtkSmartPointer<vtkIntArray> arrIntensityProfile = vtkSmartPointer<vtkIntArray>::New();
  arrIntensityProfile->SetName("Intensity Profile");
  table->AddColumn(arrIntensityProfile);
 
  // Set the tracker data
  table->SetNumberOfRows(intensityValues.size());
  for (int i = 0; i < intensityValues.size(); ++i)
  {
    table->SetValue(i, 0, i);
    table->SetValue(i, 1, (intensityValues.at(i)));
  }

  // Set up the view
  vtkSmartPointer<vtkContextView> view = vtkSmartPointer<vtkContextView>::New();
  view->GetRenderer()->SetBackground(1.0, 1.0, 1.0);
 
  // Add the two line plots
  vtkSmartPointer<vtkChartXY> chart =  vtkSmartPointer<vtkChartXY>::New();
  view->GetScene()->AddItem(chart);
  vtkPlot *line = chart->AddPlot(vtkChart::LINE);

  #if VTK_MAJOR_VERSION <= 5
    line->SetInput(table, 0, 1);
  #else
    line->SetInputData(table, 0, 1);
  #endif

  line->SetColor(0, 255, 0, 255);
  line->SetWidth(1.0);
  line = chart->AddPlot(vtkChart::LINE);

  // Start interactor
  view->GetInteractor()->Initialize();
  view->GetInteractor()->Start();

} //  End plot()

//-----------------------------------------------------------------------------
void TemporalCalibration::plotDoubleArray(std::vector<double> intensityValues)
{

  //  Create table
  vtkSmartPointer<vtkTable> table = vtkSmartPointer<vtkTable>::New();

  //  Create array correpsonding to the time values of the tracker plot
  vtkSmartPointer<vtkDoubleArray> arrPixelPositions = vtkSmartPointer<vtkDoubleArray>::New();
  arrPixelPositions->SetName("Pixel Positions"); 
  table->AddColumn(arrPixelPositions);
 
  //  Create array corresponding to the metric values of the tracker plot
  vtkSmartPointer<vtkDoubleArray> arrIntensityProfile = vtkSmartPointer<vtkDoubleArray>::New();
  arrIntensityProfile->SetName("Intensity Profile");
  table->AddColumn(arrIntensityProfile);
 
  // Set the tracker data
  table->SetNumberOfRows(intensityValues.size());
  for (int i = 0; i < intensityValues.size(); ++i)
  {
    table->SetValue(i, 0, i);
    table->SetValue(i, 1, (intensityValues.at(i)));
  }

  // Set up the view
  vtkSmartPointer<vtkContextView> view = vtkSmartPointer<vtkContextView>::New();
  view->GetRenderer()->SetBackground(1.0, 1.0, 1.0);
 
  // Add the two line plots
  vtkSmartPointer<vtkChartXY> chart =  vtkSmartPointer<vtkChartXY>::New();
  view->GetScene()->AddItem(chart);
  vtkPlot *line = chart->AddPlot(vtkChart::LINE);

  #if VTK_MAJOR_VERSION <= 5
    line->SetInput(table, 0, 1);
  #else
    line->SetInputData(table, 0, 1);
  #endif

  line->SetColor(0, 255, 0, 255);
  line->SetWidth(1.0);
  line = chart->AddPlot(vtkChart::LINE);

  // Start interactor
  view->GetInteractor()->Initialize();
  view->GetInteractor()->Start();

} //  End plot()


//-----------------------------------------------------------------------------
PlusStatus TemporalCalibration::ComputeLineParameters()
{
  const unsigned int DIMENSION = 2;

  typedef itk::PlaneParametersEstimator<DIMENSION> PlaneEstimatorType;
  typedef itk::RANSAC<itk::Point<double, DIMENSION>, double> RANSACType;

  std::vector< itk::Point<double,DIMENSION> > data;
  std::vector<double> truePlaneParameters, planeParameters; 
  double outlierDistance = 20.0;
  unsigned int i;  
  double dotProduct;

  std::cout<<"Known (hyper)plane parameters [n,a]\n\t [ ";
  for( i=0; i<(2*DIMENSION-1); i++ )
    std::cout<<truePlaneParameters[i]<<", ";
  std::cout<<truePlaneParameters[i]<<"]\n\n";

  //create and initialize the parameter estimator
  double maximalDistanceFromPlane = 0.5;
  PlaneEstimatorType::Pointer planeEstimator = PlaneEstimatorType::New();
  planeEstimator->SetDelta( maximalDistanceFromPlane );
  planeEstimator->LeastSquaresEstimate( data, planeParameters );
  if( planeParameters.empty() )
    std::cout<<"Least squares estimate failed, degenerate configuration?\n";
  else
  {
    std::cout<<"Least squares hyper(plane) parameters: [n,a]\n\t [ ";
    for( i=0; i<(2*DIMENSION-1); i++ )
      std::cout<<planeParameters[i]<<", ";
    std::cout<<planeParameters[i]<<"]\n\n";
    //cos(theta), theta is the angle between the two unit normals
    dotProduct = 0.0;
    for( i=0; i<DIMENSION; i++ )
      dotProduct+= planeParameters[i]*truePlaneParameters[i];
    std::cout<<"\tDot product of real and computed normals[+-1=correct]: ";
    std::cout<<dotProduct<<"\n";
    //distance between known hyper(plane) and estimated point on plane
    dotProduct = 0.0;
    for( i=0; i<DIMENSION; i++ )
      dotProduct+= (planeParameters[DIMENSION+i] - 
      truePlaneParameters[DIMENSION+i])*truePlaneParameters[i];
    std::cout<<"\tCheck if computed point is on known plane [0=correct]: ";
    std::cout<<dotProduct<<"\n\n";
  }

  //create and initialize the RANSAC algorithm
  double desiredProbabilityForNoOutliers = 0.999;
  double percentageOfDataUsed;
  RANSACType::Pointer ransacEstimator = RANSACType::New();
  ransacEstimator->SetData( data );
  ransacEstimator->SetParametersEstimator( planeEstimator.GetPointer() );
  percentageOfDataUsed = 
    ransacEstimator->Compute( planeParameters, desiredProbabilityForNoOutliers );
  if( planeParameters.empty() )
    std::cout<<"RANSAC estimate failed, degenerate configuration?\n";
  else
  {
    std::cout<<"RANSAC hyper(plane) parameters: [n,a]\n\t [ ";
    for( i=0; i<(2*DIMENSION-1); i++ )
      std::cout<<planeParameters[i]<<", ";
    std::cout<<planeParameters[i]<<"]\n\n";
    //cos(theta), theta is the angle between the two unit normals
    dotProduct = 0.0;
    for( i=0; i<DIMENSION; i++ )
      dotProduct+= planeParameters[i]*truePlaneParameters[i];
    std::cout<<"\tDot product of real and computed normals[+-1=correct]: ";
    std::cout<<dotProduct<<"\n";
    //distance between known hyper(plane) and estimated point on plane
    dotProduct = 0.0;
    for( i=0; i<DIMENSION; i++ )
      dotProduct+= ( planeParameters[DIMENSION+i] - 
      truePlaneParameters[DIMENSION+i] )*truePlaneParameters[i];
    std::cout<<"\tCheck if computed point is on known plane [0=correct]: ";
    std::cout<<dotProduct<<"\n\n";
    std::cout<<"\tPercentage of points which were used for final estimate: ";
    std::cout<<percentageOfDataUsed<<"\n\n";

  }
  return PLUS_SUCCESS;
}
