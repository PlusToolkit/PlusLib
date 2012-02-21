#include "vtkTransformRepository.h"
#include "TemporalCalibrationAlgo.h"
#include "vtkMath.h"

// Line detection parameters

const int HOUGH_DISC_RADIUS = 10;
const float HOUGH_ANGLE_RESOLUTION = 100;

///////////////


TemporalCalibration::TemporalCalibration() : m_SamplingResolutionSec(0.001),
                                             m_TrackerLag(0),
                                             m_MaxTrackerLagSec(2.0),
                                             m_TransformName("ProbeToReference")
{
  m_TrackerFrames = vtkSmartPointer<vtkTrackedFrameList>::New();
  m_USVideoFrames = vtkSmartPointer<vtkTrackedFrameList>::New();
  m_TrackerTable = vtkSmartPointer<vtkTable>::New();
  m_VideoTable = vtkSmartPointer<vtkTable>::New();

  //  For eventual replacement
  m_TrackerTimestampedMetric = vtkSmartPointer<vtkTable>::New();
  
}

void TemporalCalibration::SetTrackerFrames(vtkSmartPointer<vtkTrackedFrameList> trackerFrames)
{
  m_TrackerFrames = trackerFrames;
}

void TemporalCalibration::SetUSVideoFrames(vtkSmartPointer<vtkTrackedFrameList> USVideoFrames)
{
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


double TemporalCalibration::getTimeOffset()
{
  return m_TrackerLag;
}


PlusStatus TemporalCalibration::CalculateTrackerMetric()
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
      // there is no available transform for this frame, just skip that frame
      continue;
    }  
    //  Get the Euclidean probe-to-reference distance = (tx^2 + ty^2 + tz^2)^(1/2)   
    for(int i = 0; i < 3; ++i)
    {
      trackerTranslationModulus += probeToReferenceTransform->GetElement(i, 3) * 
                                   probeToReferenceTransform->GetElement(i, 3);
    }    
    trackerTranslationModulus = std::sqrt(trackerTranslationModulus);
    m_TrackerMetric.push_back(trackerTranslationModulus);
    m_TrackerTimestamps.push_back(trackedFrame->GetTimestamp());

  }

  vtkSmartPointer<vtkDoubleArray> trackerTimestampsArray = vtkSmartPointer<vtkDoubleArray>::New();
  trackerTimestampsArray->SetName("Tracker Timestamps [s]"); 

  vtkSmartPointer<vtkDoubleArray> trackerPositionMetricArray = vtkSmartPointer<vtkDoubleArray>::New();
  trackerPositionMetricArray->SetName("Tracker Position Metric"); 

  m_TrackerTimestampedMetric->AddColumn(trackerTimestampsArray);
  m_TrackerTimestampedMetric->AddColumn(trackerPositionMetricArray);

  m_TrackerTimestampedMetric->SetNumberOfRows(m_TrackerMetric.size());

  for(int i = 0; i < m_TrackerTimestampedMetric->GetNumberOfRows(); ++i)
  {
    m_TrackerTimestampedMetric->SetValue(i, 0, m_TrackerTimestamps.at(i));
    m_TrackerTimestampedMetric->SetValue(i, 1, m_TrackerMetric.at(i));
  }

  std::cout << m_TrackerTimestampedMetric->GetValue(0,0) << std::endl;

  //  Normalize the tracker metric.
  NormalizeMetric(m_TrackerMetric);
  NormalizeTableColumn( m_TrackerTimestampedMetric, 1);
  
  return PLUS_SUCCESS;

}


PlusStatus TemporalCalibration::CalculateVideoMetric()
{

  //  For each video frame, detect line and extract mindpoint and slope parameters
  for(int frameNumber = 0; frameNumber < m_USVideoFrames->GetNumberOfTrackedFrames(); ++frameNumber)
  {
    LOG_DEBUG("Calculate video metric for frame "<<frameNumber);
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
      // dropped frame
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
    int houghNumberOfLinesToDetect = 1;
    houghTransform->SetInput(localImage);
    houghTransform->SetThreshold(houghLineDetectionThreshold);
    houghTransform->SetNumberOfLines(houghNumberOfLinesToDetect);
    houghTransform->SetDiscRadius(HOUGH_DISC_RADIUS);
    houghTransform->SetAngleResolution(HOUGH_ANGLE_RESOLUTION);
    houghTransform->Update();

    LOG_DEBUG("Angle resolution: pi/" << 2 * houghTransform->GetAngleResolution());
    LOG_DEBUG("Disc radius: " << houghTransform->GetDiscRadius());
    LOG_DEBUG("Variance of Gaussian filter: " << houghTransform->GetVariance());

    // Get the lines of the image into the line-list holder
    itk::HoughTransform2DLinesImageFilter<charPixelType, charPixelType>::LinesListType lines = houghTransform->GetLines();

    LOG_DEBUG("Found " << lines.size() << " line(s).");

    // Iterate through the list of lines and we draw them.
    typedef HoughTransformFilterType::LinesListType::const_iterator LineIterator;
    typedef HoughTransformFilterType::LineType::PointListType PointListType;
    LineIterator itLines = lines.begin();

    //std::ofstream InterceptFile;
    //InterceptFile.open ("InterceptFile.txt", std::ios::app);
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
      m_VideoMetric.push_back(m*halfwayPoint + b);

      itLines++;
    }

  }// end frameNum loop
  
  //  Normalize the video metric
  NormalizeMetric(m_VideoMetric);

  return PLUS_SUCCESS;

} //  End LineDetection


void TemporalCalibration::NormalizeMetric(std::vector<double> &metric)
{
  //  Get the mean of the Euclidean distances
  double mu = 0;
  for(int i = 0; i < metric.size(); ++i)
  {
    mu += metric.at(i);
  }

  mu /= metric.size();

  //  Normalize each measurement, s, as s' = (s-mu)/sigma
  for(int i = 0; i < metric.size(); ++i)
  {
    metric.at(i) -= mu;
  }

  //  Get the maximum and minimum signal values
  double maxVal = metric.at(1);
  double minVal = metric.at(1);

  for(int i = 1; i < metric.size(); ++i)
  {
    if(metric.at(i) > maxVal)
    {
      maxVal = metric.at(i);
    }
    else if(metric.at(i) < minVal)
    {
       minVal = metric.at(i);
    }
  }

  // normalize signal
  double normFactor = std::abs(maxVal) + std::abs(minVal);

  for(int i = 0; i < metric.size(); ++i)
  {
    metric.at(i) /= normFactor; 
  }

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
  double timeEpsilon = 0.0001; //  Resolution under which two time values are considered identical
  
  //  We assume that we are upsampling (i.e. that the rate of resampling is less than the original sampling rate).
  if(interpolatedTimestamp > originalTimestamps.at(straddleIndices.at(highIndex)))
  {
      straddleIndices.at(highIndex) = straddleIndices.at(highIndex) + 1;
      straddleIndices.at(lowIndex) = straddleIndices.at(lowIndex) + 1;
  }

  //  If the time index between the two straddling indices is very small, avoid interpolation (to
  //  prevent divding by a really small number).
  if(std::abs(originalTimestamps.at(straddleIndices.at(highIndex)) - originalTimestamps.at(straddleIndices.at(lowIndex))) < timeEpsilon)
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
  interpolatedVector.reserve(interpolatedTimestamps.size()); //  pre-allocate
  for(int i = 0; i < interpolatedTimestamps.size(); ++i)
  {
    interpolatedVector.push_back(linearInterpolation(interpolatedTimestamps.at(i), 
      originalMetric, originalTimestamps, straddleIndices, samplingResolutionSec));
  }

} // End interpolateHelper()


double TemporalCalibration::computeCorrelation(std::vector<double> &metricA, std::vector<double> &metricB, int indexOffset)
{

  double overlapSum = 0;
  for(long int i = 0; i < metricA.size(); ++i)
  {
    overlapSum += metricA.at(i) * metricB.at(i + indexOffset);
  }

  return overlapSum;

} // End computeCorrelation()

void TemporalCalibration::xcorr()
{
  int n = 0;
  while(n + (m_ResampledTrackerMetric.size() - 1) < m_ResampledVideoMetric.size())
  {
    m_CorrValues.push_back(computeCorrelation(m_ResampledTrackerMetric, m_ResampledVideoMetric, n));
    ++n;
  }

} // End xcorr()

void TemporalCalibration::CalculateTimeOffset()
{

  // Calculate the (normalized) metrics for the video and tracker data streams
  CalculateTrackerMetric();
  CalculateVideoMetric();
  

  //  Resample the image and tracker signals, preparing them for cross correlation
  interpolate();

  //  Perform cross correlation
  xcorr();
  
  //  Find the index offset corresponding to maximum correlation value
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

  //  Compute the time that the tracker data lags the video data
  m_TrackerLag = m_MaxTrackerLagSec - (maxCorrIndex)*m_SamplingResolutionSec;
  LOG_DEBUG("Tracker stream lags image stream by: " << m_TrackerLag << " [s]");

}// End CalculateTimeOffset()

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
  interpolateHelper(m_VideoMetric,m_ResampledVideoMetric, m_ResampledVideoTimestamps, m_VideoTimestamps, m_SamplingResolutionSec);
  interpolateHelper(m_TrackerMetric, m_ResampledTrackerMetric,m_ResampledTrackerTimestamps, m_TrackerTimestamps, m_SamplingResolutionSec);

}// End interpolate()


void TemporalCalibration::getPlotTables(vtkTable *trackerTableBefore, vtkTable *videoTableBefore, 
                                        vtkTable *trackerTableAfter, vtkTable *videoTableAfter)
{
  createPlotTables(m_ResampledTrackerTimestamps, m_ResampledTrackerMetric,m_ResampledVideoTimestamps, m_ResampledVideoMetric);                                  
  
}

void TemporalCalibration::createPlotTables(std::vector<double> &resampledTrackerTimestamps, 
                                           std::vector<double> &resampledTrackerMetric, 
                                           std::vector<double> &resampledVideoTimestamps, 
                                           std::vector<double> &resampledVideoMetric)
{

 //  Create table
  vtkSmartPointer<vtkTable> table = vtkSmartPointer<vtkTable>::New();
  vtkSmartPointer<vtkTable> table2 = vtkSmartPointer<vtkTable>::New();

  //  Create array correpsonding to the time values of the tracker plot
  vtkSmartPointer<vtkDoubleArray> arrResampledTrackerTimestamps = vtkSmartPointer<vtkDoubleArray>::New();
  arrResampledTrackerTimestamps->SetName("Time [s]"); 
  table->AddColumn(arrResampledTrackerTimestamps);
 
  //  Create array corresponding to the metric values of the tracker plot
  vtkSmartPointer<vtkDoubleArray> arrResampledTrackerMetric = vtkSmartPointer<vtkDoubleArray>::New();
  arrResampledTrackerMetric->SetName("Tracker Metric");
  table->AddColumn(arrResampledTrackerMetric);

  //  Create array correpsonding to the time values of the video plot
  vtkSmartPointer<vtkDoubleArray> arrResampledVideoTimestamps = vtkSmartPointer<vtkDoubleArray>::New();
  arrResampledVideoTimestamps->SetName("Time [s]"); 
  table2->AddColumn(arrResampledVideoTimestamps);
 
  //  Create array corresponding to the metric values of the video plot
  vtkSmartPointer<vtkDoubleArray> arrResampledVideoMetric = vtkSmartPointer<vtkDoubleArray>::New();
  arrResampledVideoMetric->SetName("Video Metric");
  table2->AddColumn(arrResampledVideoMetric);
 
  
  // Set the tracker data
  table->SetNumberOfRows(resampledTrackerTimestamps.size());
  for (int i = 0; i < resampledTrackerTimestamps.size(); ++i)
  {
    table->SetValue(i, 0, (resampledTrackerTimestamps.at(i)));
    table->SetValue(i, 1, (resampledTrackerMetric.at(i)));
  }

  // Set the video data
  table2->SetNumberOfRows(resampledVideoTimestamps.size());
  for (int i = 0; i < resampledVideoTimestamps.size(); ++i)
  {
    table2->SetValue(i, 0, (resampledVideoTimestamps.at(i)));
    table2->SetValue(i, 1, (resampledVideoMetric.at(i)));
  }
}