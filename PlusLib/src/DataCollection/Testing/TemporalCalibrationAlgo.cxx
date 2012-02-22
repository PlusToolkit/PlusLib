#include "vtkTransformRepository.h"
#include "TemporalCalibrationAlgo.h"
#include "vtkMath.h"

// Line detection parameters
const int HOUGH_DISC_RADIUS = 10;
const int NUMBER_OF_LINES_TO_DETECT = 1;
const float HOUGH_ANGLE_RESOLUTION = 100;

// Default algorithm parameters
const double MINIMUM_SIGNAL_PEAK_TO_PEAK = 0.01; // If either tracker metric "swings" less than this, abort
const double METRIC_SIGNAL_EPSILON = 0.0001; // Temporal resolution below which two time values are considered identical
const double DEFAULT_SAMPLING_RESOLUTION_SEC = 0.001; 
const double DEFAULT_TRACKER_LAG_SEC = 0.001;
const double DEFAULT_MAX_TRACKER_LAG_SEC = 2;
const std::string DEFAULT_TRANSFORM_NAME = "ProbeToReference";
const int LOWER_STRADDLE_INDEX = 0;
const int UPPER_STRADDLE_INDEX = 1;


TemporalCalibration::TemporalCalibration() : m_SamplingResolutionSec(DEFAULT_SAMPLING_RESOLUTION_SEC),
                                             m_TrackerLagSec(DEFAULT_TRACKER_LAG_SEC),
                                             m_MaxTrackerLagSec(DEFAULT_MAX_TRACKER_LAG_SEC),
                                             m_TransformName(DEFAULT_TRANSFORM_NAME)
{
  
  /* TODO: Switching to VTK table data structure */
  m_TrackerTable = vtkSmartPointer<vtkTable>::New();
  m_VideoTable = vtkSmartPointer<vtkTable>::New();
  m_TrackerTimestampedMetric = vtkSmartPointer<vtkTable>::New();
  
}

PlusStatus TemporalCalibration::Update()
{
  // Check if video frames have been assigned
  if(m_USVideoFrames == NULL)
  {
    LOG_ERROR("US video data is not assigned...Exiting");
    return PLUS_FAIL;
  }

  //  Check if tracker frames have been assigned
  if(m_TrackerFrames == NULL)
  {
    LOG_ERROR("US video data is not assigned...Exiting");
    return PLUS_FAIL;
  }

  //  Make sure video frame list is not empty
  if(m_USVideoFrames->GetNumberOfTrackedFrames() == 0)
  {
    LOG_ERROR("US video data contains no frames...Exiting");
    return PLUS_FAIL;
  }

  //  Make sure video frame list is not empty
  if(m_TrackerFrames->GetNumberOfTrackedFrames() == 0)
  {
    LOG_ERROR("US tracker data contains no frames...Exiting");
    return PLUS_FAIL;
  }

   //  Check that all the image data is valid
  int totalNumberOfInvalidVideoFrames = 0;
  int greatestNumberOfConsecutiveInvalidVideoFrames = 0;
  int currentNumberOfConsecutiveInvalidVideoFrames = 0;
  for(int i = 0 ; i < m_USVideoFrames->GetNumberOfTrackedFrames(); ++i)
  {
    if(!m_USVideoFrames->GetTrackedFrame(i)->GetImageData()->IsImageValid())
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

  /*  TODO: Validate the resampling frequency */
  CalculateTrackerLagSec();

  return PLUS_SUCCESS;
  /* TODO: Maybe output an warning message. */
  
  
}

void TemporalCalibration::SetTrackerFrames(const vtkSmartPointer<vtkTrackedFrameList> trackerFrames)
{
  // m_TrackerFrames = trackerFrames->NewInstance();
  // TODO: Do I worry about aliasing here?
  m_TrackerFrames = trackerFrames;
}

void TemporalCalibration::SetUSVideoFrames(const vtkSmartPointer<vtkTrackedFrameList> USVideoFrames)
{
   // m_USVideoFrames = USVideoFrames->NewInstance();
    m_USVideoFrames = USVideoFrames;
}

void TemporalCalibration::setSamplingResolutionSec(double samplingResolutionSec)
{
  m_SamplingResolutionSec = samplingResolutionSec;
}

void TemporalCalibration::SetTransformName(std::string transformName)
{
  m_TransformName = transformName;
}

void TemporalCalibration::SetMaximumVideoTrackerLagSec(double maxLagSec)
{
  m_MaxTrackerLagSec = maxLagSec;
}


double TemporalCalibration::GetTrackerLagSec()
{
  return m_TrackerLagSec;
}


PlusStatus TemporalCalibration::ComputeTrackerPositionMetric()
{

  vtkSmartPointer<vtkTransformRepository> transformRepository = vtkSmartPointer<vtkTransformRepository>::New();
  PlusTransformName transformName;
  transformName.SetTransformName(m_TransformName.c_str());
  
  //  For each tracker position in the recorded tracker sequence, get its translation from reference.
  for ( int frame = 0; frame < m_TrackerFrames->GetNumberOfTrackedFrames(); ++frame )
  {
    double trackerTranslationModulus=0; //  Euclidean translation of tracker from reference.
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
    for(int i = 0; i < 3; ++i)
    {
      trackerTranslationModulus += probeToReferenceTransform->GetElement(i, 3) * 
                                   probeToReferenceTransform->GetElement(i, 3);
    }    
    trackerTranslationModulus = std::sqrt(trackerTranslationModulus);
    m_TrackerPositionMetric.push_back(trackerTranslationModulus);
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


PlusStatus TemporalCalibration::ComputeVideoPositionMetric()
{

  //  For each video frame, detect line and extract mindpoint and slope parameters
  for(int frameNumber = 0; frameNumber < m_USVideoFrames->GetNumberOfTrackedFrames(); ++frameNumber)
  {
    LOG_DEBUG("Calculating video position metric for frame " << frameNumber);

    // Define image types and image dimension
    const int imageDimension(2);
    typedef unsigned char charPixelType; // The natural type of the input image
    typedef float floatPixelType; //  The type of pixel used for the Hough accumulator
    typedef itk::Image<charPixelType,imageDimension> charImageType;
    
    //  Get timestamp for image frame
    m_VideoTimestamps.push_back(m_USVideoFrames->GetTrackedFrame(frameNumber)->GetTimestamp());

    // Get curent image
    charImageType::Pointer localImage = m_USVideoFrames->GetTrackedFrame(frameNumber)->GetImageData()->GetImage<charPixelType>();
    
    if(localImage.IsNull())
    {
      // Dropped frame
      continue;
    }

    // Create the hough transform line detection filter
    typedef itk::HoughTransform2DLinesImageFilter<charPixelType, floatPixelType> HoughTransformFilterType;
    HoughTransformFilterType::Pointer houghTransform = HoughTransformFilterType::New();

    // Get threshold value via Otsu's metod
    typedef itk::OtsuThresholdImageFilter <charImageType, charImageType> otsuThresholdImageFilterType;
    otsuThresholdImageFilterType::Pointer otsuFilter = otsuThresholdImageFilterType::New();
    otsuFilter->SetInput(localImage);
    otsuFilter->Update();

    // Set parameters of the Hough transform filter
    double houghLineDetectionThreshold = otsuFilter->GetThreshold(); // set Hough threshold to Otsu threshold.
    houghTransform->SetInput(localImage);
    houghTransform->SetThreshold(houghLineDetectionThreshold);
    houghTransform->SetNumberOfLines(NUMBER_OF_LINES_TO_DETECT);
    houghTransform->SetDiscRadius(HOUGH_DISC_RADIUS);
    houghTransform->SetAngleResolution(HOUGH_ANGLE_RESOLUTION);
    houghTransform->Update();

    // Get the lines of the image into the line-list holder
    itk::HoughTransform2DLinesImageFilter<charPixelType, charPixelType>::LinesListType lines = houghTransform->GetLines();

    // Iterate through the list of lines and we draw them.
    typedef HoughTransformFilterType::LinesListType::const_iterator LineIterator;
    typedef HoughTransformFilterType::LineType::PointListType PointListType;
    LineIterator itLines = lines.begin();

    itk::Size<imageDimension> size = localImage->GetLargestPossibleRegion().GetSize();
    int halfwayPoint = static_cast<int> ((float) size[0]) / 2;

    while( itLines != lines.end() )
    {
      // Get the list of points which consists of two points to represent a
      // straight line.  Then, from these two points, we compute a fixed point
      // $u$ and a unit vector $\vec{v}$ to parameterize the line.
      PointListType pointsList = (*itLines)->GetPoints();
      PointListType::const_iterator itPoints = pointsList.begin();
     
      double u[2];
      u[0] = (*itPoints).GetPosition()[0];
      u[1] = (*itPoints).GetPosition()[1];
      itPoints++;

      double v[2];
      v[0] = u[0]-(*itPoints).GetPosition()[0];
      v[1] = u[1]-(*itPoints).GetPosition()[1];

      double m = v[1] / v[0];
      double b = u[1] - m*u[0];
      m_VideoPositionMetric.push_back(m*halfwayPoint + b);

      itLines++;
    }

  }// end frameNum loop
  
  //  Normalize the video metric
  NormalizeMetric(m_VideoPositionMetric);

  return PLUS_SUCCESS;

} //  End LineDetection


PlusStatus TemporalCalibration::NormalizeMetric(std::vector<double> &metric)
{
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
    LOG_ERROR("Detected metric values do not vary sufficiently--i.e. signal is constant...Exiting");
    return PLUS_FAIL;
  }

  for(int i = 0; i < metric.size(); ++i)
  {
    metric.at(i) /= maxPeakToPeak; 
  }

  return PLUS_SUCCESS;
}// End NormalizeMetric()


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

double TemporalCalibration::linearInterpolation(double interpolatedTimestamp, const std::vector<double> &originalMetric, 
                           const std::vector<double> &originalTimestamps, std::vector<int> &straddleIndices, double samplingResolutionSec)
{
  const int lowIndex = 0; //  Position of low index in "straddleIndices"
  const int highIndex = 1; // Position of high index in "straddleIndices"
  
  //  We assume that we are upsampling (i.e. that the resampled period is less than the original sampling period).
  if(interpolatedTimestamp > originalTimestamps.at(straddleIndices.at(highIndex)))
  {
      straddleIndices.at(highIndex) = straddleIndices.at(highIndex) + 1;
      straddleIndices.at(lowIndex) = straddleIndices.at(lowIndex) + 1;
  }

  //  If the time index between the two straddling indices is very small, avoid interpolation (to
  //  prevent divding by a really small number).
  if(std::abs(originalTimestamps.at(straddleIndices.at(highIndex)) - originalTimestamps.at(straddleIndices.at(lowIndex))) < METRIC_SIGNAL_EPSILON)
    return originalMetric.at(straddleIndices.at(highIndex));
  
  // Peform linear interpolation
  double m = (originalMetric.at(straddleIndices.at(highIndex)) - originalMetric.at(straddleIndices.at(lowIndex))) / 
             (originalTimestamps.at(straddleIndices.at(highIndex)) - originalTimestamps.at(straddleIndices.at(lowIndex)));

  return originalMetric.at(straddleIndices.at(lowIndex)) + (interpolatedTimestamp - originalTimestamps.at(straddleIndices.at(lowIndex))) * m;

} // End linearInterpolation()

void TemporalCalibration::interpolateHelper(const std::vector<double> &originalMetric, std::vector<double> &interpolatedVector,
          std::vector<double> &interpolatedTimestamps, const std::vector<double> &originalTimestamps, double samplingResolutionSec)
{

  //  For the first interpolated timestamp value, find the index of the closest element in the
  //  original timestamp array.
  int closestIndex = 0;
  double closestVal = std::abs(originalTimestamps.at(closestIndex) - interpolatedTimestamps.at(0));
  
  for(int i = 1; i < originalTimestamps.size(); ++i)
  {
    if(std::abs(originalTimestamps.at(i) - interpolatedTimestamps.at(0)) < closestVal)
    {
      closestIndex = i;
      closestVal = std::abs(originalTimestamps.at(i) - interpolatedTimestamps.at(0));
    }
  }
  
  //  Assign two indices that "straddle" the first interpolated timestamp value in the original timstamp sequence
  int indexHigh = 0;
  int indexLow = 0;

  if(originalTimestamps.at(closestIndex) > interpolatedTimestamps.at(0))
  {
   indexHigh = closestIndex;
   indexLow = closestIndex - 1;
  }
  else
  {
   indexHigh = closestIndex + 1;
   indexLow = closestIndex;
  }

  std::vector<int> straddleIndices; 
  straddleIndices.push_back(indexLow);
  straddleIndices.push_back(indexHigh);

  //  For each interpolated timestamp, find the corresponding metric value
  for(int i = 0; i < interpolatedTimestamps.size(); ++i)
  {
    interpolatedVector.push_back(linearInterpolation(interpolatedTimestamps.at(i), 
      originalMetric, originalTimestamps, straddleIndices, samplingResolutionSec));
  }

} // End interpolateHelper()

void TemporalCalibration::ResamplePositionMetrics()
{
  //  Find the time-range that is common to both tracker and image signals
  double translationTimestampMin = m_TrackerTimestamps.at(0);
  double translationTimestampMax = m_TrackerTimestamps.at(m_TrackerTimestamps.size() - 1);

  double imageTimestampMin = m_VideoTimestamps.at(0);
  double imageTimestampMax = m_VideoTimestamps.at(m_VideoTimestamps.size() - 1);

  double commonRangeMin = std::max(imageTimestampMin, translationTimestampMin); 
  double commonRangeMax = std::min(imageTimestampMax, translationTimestampMax);

  if (commonRangeMin + m_MaxTrackerLagSec >= commonRangeMax - m_MaxTrackerLagSec)
  {
    LOG_ERROR("Insufficient overlap between tracking data and image data to compute time offset...Exiting"); 
    exit(EXIT_FAILURE);
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
  InterpolatePositionMetric(m_VideoTimestamps, m_ResampledVideoTimestamps, m_VideoPositionMetric, m_ResampledVideoPositionMetric);
  InterpolatePositionMetric(m_TrackerTimestamps, m_ResampledTrackerTimestamps, m_TrackerPositionMetric, m_ResampledTrackerPositionMetric);
                                                                                                                                                                              
}

void TemporalCalibration::InterpolatePositionMetric(std::vector<double> &originalTimestamps,
                                                    std::vector<double> &resampledTimestamps,
                                                    std::vector<double> &originalMetric,
                                                    std::vector<double> &resampledPositionMetric)
                                                                 
{
  std::vector<int> lowerStraddleIndices;
  std::vector<int> upperStraddleIndices;
  GetStraddleIndices(originalTimestamps, resampledTimestamps, lowerStraddleIndices, upperStraddleIndices);

  for(long int resampledTimeValueIndex = 0; resampledTimeValueIndex < resampledTimestamps.size(); ++resampledTimeValueIndex)
  {
    resampledPositionMetric.push_back(linearInterpolationMod(resampledTimestamps.at(resampledTimeValueIndex), originalTimestamps, 
                                                             originalMetric, 
                                                             lowerStraddleIndices.at(resampledTimeValueIndex), 
                                                             upperStraddleIndices.at(resampledTimeValueIndex)));
  }                                            
}

double TemporalCalibration::linearInterpolationMod(double resampledTimeValue, std::vector<double> &originalTimestamps, 
                                                   std::vector<double> &originalMetric, int lowerStraddleIndex, int upperStraddleIndex)
                                                    
{
  //  If the time index between the two straddling indices is very small, avoid interpolation (to
  //  prevent divding by a really small number).
  if(std::abs(originalTimestamps.at(upperStraddleIndex) - originalTimestamps.at(lowerStraddleIndex)) < METRIC_SIGNAL_EPSILON)
    return originalMetric.at(upperStraddleIndex);
  
  // Peform linear interpolation
  double m = (originalMetric.at(upperStraddleIndex) - originalMetric.at(lowerStraddleIndex)) / 
             (originalTimestamps.at(upperStraddleIndex) - originalTimestamps.at(lowerStraddleIndex));

  return originalMetric.at(lowerStraddleIndex) + (resampledTimeValue - originalTimestamps.at(lowerStraddleIndex)) * m;

} // End linearInterpolation()

void TemporalCalibration::GetStraddleIndices(std::vector<double> &originalTimestamps, std::vector<double> &resampledTimestamps, 
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
    currLowerStraddleIndex = FindLowerStraddleIndex(originalTimestamps, resampledTimestamps.at(timestampIndex), currLowerStraddleIndex);
    lowerStraddleIndices.push_back(currLowerStraddleIndex);

    currUpperStraddleIndex = FindUpperStraddleIndex(originalTimestamps, resampledTimestamps.at(timestampIndex), currUpperStraddleIndex);
    upperStraddleIndices.push_back(currUpperStraddleIndex);
  }

}
int TemporalCalibration::FindFirstLowerStraddleIndex(std::vector<double> &originalTimestamps, double resampledTimestamp)                                                    
{

    //  No original timestamps lie below the desired resampled timestamp--cannot interpolate
    if(originalTimestamps.at(0) > resampledTimestamp)
    {
      LOG_ERROR("TODO");
    }

    int currIndex = 0;
    while(currIndex < originalTimestamps.size()  && originalTimestamps.at(currIndex) <= resampledTimestamp)
    {
      ++currIndex;
    }
    --currIndex;

    return currIndex;
}

int TemporalCalibration::FindFirstUpperStraddleIndex(std::vector<double> &originalTimestamps, double resampledTimestamp)
{
    //  No original timestamps lie past the desired resampled timestamp--cannot interpolate
    if(originalTimestamps.at(originalTimestamps.size() - 1) < resampledTimestamp)
    {
      LOG_ERROR("TODO");
    }

    int currIndex = originalTimestamps.size() - 1;
    while(currIndex >= 0 && originalTimestamps.at(currIndex) >= resampledTimestamp)
    {
      --currIndex;
    }

    ++currIndex;

    return currIndex;
}


int TemporalCalibration::FindLowerStraddleIndex(std::vector<double> &originalTimestamps, double resampledTimestamp,
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

int TemporalCalibration::FindUpperStraddleIndex(std::vector<double> &originalTimestamps, double resampledTimestamp,
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


void TemporalCalibration::CalculateCrossCorrelationBetweenVideoAndTrackerMetrics()
{
  int trackerLagIndex = 0;
  while(trackerLagIndex + (m_ResampledTrackerPositionMetric.size() - 1) < m_ResampledVideoPositionMetric.size())
  {
    m_CorrValues.push_back(ComputeCorrelationSumForGivenLagIndex(m_ResampledTrackerPositionMetric, m_ResampledVideoPositionMetric, trackerLagIndex));
    ++trackerLagIndex;
  }

} // End CalculateCrossCorrelationBetweenVideoAndTrackerMetrics()

double TemporalCalibration::ComputeCorrelationSumForGivenLagIndex(std::vector<double> &metricA, std::vector<double> &metricB, int indexOffset)
{

  double multipliedSignalSum = 0;
  for(long int i = 0; i < metricA.size(); ++i)
  {
    multipliedSignalSum += metricA.at(i) * metricB.at(i + indexOffset);
  }

  return multipliedSignalSum;

} // End ComputeCorrelationSumForGivenLagIndex()

void TemporalCalibration::CalculateTrackerLagSec()
{

  //  Calculate the (normalized) metrics for the video and tracker data streams
  ComputeTrackerPositionMetric();
  ComputeVideoPositionMetric();
  
  //  Resample the image and tracker metrics; this prepares the two signals for cross correlation
  //interpolate();
  ResamplePositionMetrics();

  //  Perform cross correlation
  CalculateCrossCorrelationBetweenVideoAndTrackerMetrics();
  
  //  Find the index offset corresponding to the maximum correlation sum
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

  //  Compute the time that the tracker data lags the video data using the maximum index
  m_TrackerLagSec = m_MaxTrackerLagSec - (maxCorrIndex)*m_SamplingResolutionSec;

  LOG_DEBUG("Tracker stream lags image stream by: " << m_TrackerLagSec << " [s]");


}// End CalculateTrackerLagSec()

void TemporalCalibration::interpolate()
{

  //  Find the time-range that is common to both tracker and image signals
  double translationTimestampMin = m_TrackerTimestamps.at(1);
  double translationTimestampMax = m_TrackerTimestamps.at(m_TrackerTimestamps.size() - 1);

  double imageTimestampMin = m_VideoTimestamps.at(1);
  double imageTimestampMax = m_VideoTimestamps.at(m_VideoTimestamps.size() - 1);

  double commonRangeMin = std::max(imageTimestampMin, translationTimestampMin); 
  double commonRangeMax = std::min(imageTimestampMax, translationTimestampMax);

  if (commonRangeMin + m_MaxTrackerLagSec >= commonRangeMax - m_MaxTrackerLagSec)
  {
    std::cerr << "Insufficient overlap between tracking data and image data to compute time offset...Exiting" << std::endl;
    exit(EXIT_FAILURE);
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

  //  Get resampled metrics for video and tracker sequences
  interpolateHelper(m_VideoPositionMetric,m_ResampledVideoPositionMetric, m_ResampledVideoTimestamps, m_VideoTimestamps, m_SamplingResolutionSec);
  interpolateHelper(m_TrackerPositionMetric, m_ResampledTrackerPositionMetric,m_ResampledTrackerTimestamps, m_TrackerTimestamps, m_SamplingResolutionSec);

}// End interpolate()

