#include "TemporalCalibration.h"

TemporalCalibration::TemporalCalibration(std::string inputTrackerSequenceMetafile, 
                                         std::string inputUSImageSequenceMetafile,
                                         std::string outputFilepath, 
                                         double samplingResolutionSec) :
                                         inputTrackerSequenceMetafile_(inputTrackerSequenceMetafile),
                                         inputUSImageSequenceMetafile_(inputUSImageSequenceMetafile),
                                         outputFilepath_(outputFilepath),
                                         samplingResolutionSec_(samplingResolutionSec)
{
  trackerFrames_ = vtkSmartPointer<vtkTrackedFrameList>::New();
  USVideoFrames_ = vtkSmartPointer<vtkTrackedFrameList>::New();
  trackerLag_ = 0;
  readFiles();
}

double TemporalCalibration::getSamplingResolution()
{
  return samplingResolutionSec_;
}

void TemporalCalibration::setSamplingResolution(double samplingResolutionSec)
{
  samplingResolutionSec_  = samplingResolutionSec;
}

std::string TemporalCalibration::getInputTrackerSequenceMetafile()
{
  return inputTrackerSequenceMetafile_;
}


std::string TemporalCalibration::getInputUSImageSequenceMetafile()
{
  return inputUSImageSequenceMetafile_;
}

double TemporalCalibration::getTimeOffset()
{
  return trackerLag_;
}

std::string TemporalCalibration::getOutputFilepath()
{
  return outputFilepath_;
}

PlusStatus TemporalCalibration::CalculateTrackerMetric()
{
  // Declare homogeneous probe-to-tracker transformation matrix, and corresponding frame transform name.
  vtkSmartPointer<vtkMatrix4x4> probeToTrackerTransform = vtkSmartPointer<vtkMatrix4x4>::New(); 
  vtkSmartPointer<vtkMatrix4x4> referenceToTrackerTransform = vtkSmartPointer<vtkMatrix4x4>::New(); 
  vtkSmartPointer<vtkMatrix4x4> trackerToReferenceTransform = vtkSmartPointer<vtkMatrix4x4>::New();
  vtkSmartPointer<vtkMatrix4x4> probeToReferenceTransform = vtkSmartPointer<vtkMatrix4x4>::New();

  PlusTransformName transformName;
  transformName.SetTransformName("ProbeToTrackerTransform");
  PlusTransformName referenceToTrackerTransformName;
  referenceToTrackerTransformName.SetTransformName("ReferenceToTrackerTransform");

  //  For each tracker position in the recorded tracker sequence, get its translation from reference.
  for ( int frame = 0; frame < trackerFrames_->GetNumberOfTrackedFrames(); ++frame )
  {
    double trackerTranslationModulus=0; //  Euclidean translation of tracker from reference.
    TrackedFrame *trackedFrame = trackerFrames_->GetTrackedFrame(frame);
    trackerTimestamps_.push_back(trackedFrame->GetTimestamp());
    trackedFrame->GetCustomFrameTransform(transformName, probeToTrackerTransform);
    trackedFrame->GetCustomFrameTransform(referenceToTrackerTransformName, referenceToTrackerTransform);
    referenceToTrackerTransform->Invert(referenceToTrackerTransform,trackerToReferenceTransform);
    vtkMatrix4x4::Multiply4x4(trackerToReferenceTransform, probeToTrackerTransform, probeToReferenceTransform);
  
    //  Get the Euclidean distance of tracker from reference = (tx^2 + ty^2 + tz^2)^(1/2)
    for(int i = 0; i < 3; ++i)
    {
      trackerTranslationModulus += probeToReferenceTransform->GetElement(i, 3) * 
                                   probeToReferenceTransform->GetElement(i, 3);
    }
    
    trackerTranslationModulus = std::sqrt(trackerTranslationModulus);
    trackerMetric_.push_back(trackerTranslationModulus);
  }

  //  Normalize the tracker metric.
  NormalizeMetric(trackerMetric_);
  
  return PLUS_SUCCESS;

}


PlusStatus TemporalCalibration::CalculateVideoMetric()
{

  for(int frameNumber = 0; frameNumber < USVideoFrames_->GetNumberOfTrackedFrames(); ++frameNumber)
  {
    // Define some image types and image dimension
    const int imageDimension(2);
    typedef unsigned char charPixelType; //the natural type of the input image
    typedef float floatPixelType; //the type of pixel used for the Hough accumulator
    typedef itk::Image<charPixelType,imageDimension> charImageType;
    
    //  Get timestamp for image frame
    videoTimestamps_.push_back(USVideoFrames_->GetTrackedFrame(frameNumber)->GetTimestamp());

    // Get curent image
    charImageType::Pointer localImage = USVideoFrames_->GetTrackedFrame(frameNumber)->GetImageData()->GetImage<charPixelType>();
    
    if(localImage.IsNull())
    {
      return PLUS_FAIL;
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
    int houghDiscRadius = 10;
    houghTransform->SetInput(localImage);
    houghTransform->SetThreshold(houghLineDetectionThreshold);
    houghTransform->SetNumberOfLines(houghNumberOfLinesToDetect);
    houghTransform->SetDiscRadius(houghDiscRadius);
    houghTransform->SetAngleResolution(100);
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
    itk::Size<2> size = localImage->GetLargestPossibleRegion().GetSize();
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
      videoMetric_.push_back(m*halfwayPoint + b);

      itLines++;
    }

  }// end frameNum loop
  
  //  Normalize the video metric
  NormalizeMetric(videoMetric_);

  return PLUS_SUCCESS;

} //  end LineDetection


void TemporalCalibration::NormalizeMetric(std::vector<double> &metric)
{

  /*
  switch (NormalizationMethod)
  {
  case NORMALIZE_STDEV:
    break;
  }
  */

  //  Get the mean of the Euclidean distances
  double mu = 0;
  for(int i = 0; i < metric.size(); ++i)
  {
    mu += metric.at(i);
  }

  mu /= metric.size();

  ////  Get the standard deviation
  //double sigma = 0;
  //for(int i = 0; i < metric.size(); ++i)
  //  sigma += (metric.at(i) - mu) * (metric.at(i) - mu);

  //sigma /= (metric.size() - 1); 
  //sigma = std::sqrt(sigma);

  //  Normalize each measurement, s, as s' = (s-mu)/sigma
  for(int i = 0; i < metric.size(); ++i)
  {
    metric.at(i) -= mu;
    /*metric.at(i) /= sigma;*/
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

  }// end for-loop

  // normalize signal
  double normFactor = std::abs(maxVal) + std::abs(minVal);

  for(int i = 0; i < metric.size(); ++i)
  {
    metric.at(i) /= normFactor; 
  }

}// End NormalizeMetric()

double TemporalCalibration::linearInterpolation(double interpolatedTimestamp, std::vector<double> &originalMetric, 
                           std::vector<double> &originalTimestamps, std::vector<int> &straddleIndices, double samplingResolutionSec)
{
  const int lowIndex = 0; //  position of low index in "straddleIndices"
  const int highIndex = 1; // position of high index in "straddleIndices"
  double timeEpsilon = 0.0001; //  resolution under which two time values are considered identical
  
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

void TemporalCalibration::interpolateHelper(std::vector<double> &originalMetric, std::vector<double> &interpolatedVector,
          std::vector<double> &interpolatedTimestamps, std::vector<double> &originalTimestamps, double samplingResolutionSec)
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
  int indexHigh;
  int indexLow;

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
  while(n + (resampledTrackerMetric_.size() - 1) < resampledVideoMetric_.size())
  {
    corrValues_.push_back(computeCorrelation(resampledTrackerMetric_, resampledVideoMetric_, n));
    ++n;
  }

} // end xcorr()

void TemporalCalibration::CalculateTimeOffset()
{

  // Calculate the (normalized) metrics for the video and tracker data streams
  CalculateVideoMetric();
  CalculateTrackerMetric();

  //  Resample the image and tracker signals, preparing them for cross correlation
  interpolate();

  //  Perform cross correlation
  xcorr();
  
  //  Find the index offset corresponding to maximum correlation value
  double maxCorrVal = corrValues_.at(0);
  int maxCorrIndex = 0;
  for(int i = 1; i < corrValues_.size(); ++i)
  {
    if(corrValues_.at(i) > maxCorrVal)
    {
      maxCorrVal = corrValues_.at(i);
      maxCorrIndex = i;
    }
  }

  //  Compute the time that the tracker data lags the video data
  double maxVideoOffset = 2; //  Maximum anticipated time offset [seconds]

  trackerLag_ = maxVideoOffset - (maxCorrIndex)*samplingResolutionSec_;
  std::cout << "Tracker stream lags image stream by: " << trackerLag_ << " [s]" << std::endl;

}// End CalculateTimeOffset()

void TemporalCalibration::interpolate()
{

  double maxVideoOffset = 2; //  Maximum anticipated time offset [seconds]

  //  Find the time-range that is common to both tracker and image signals
  double translationTimestampMin = trackerTimestamps_.at(1);
  double translationTimestampMax = trackerTimestamps_.at(trackerTimestamps_.size() - 1);

  double imageTimestampMin = videoTimestamps_.at(1);
  double imageTimestampMax = videoTimestamps_.at(videoTimestamps_.size() - 1);

  double commonRangeMin = std::max(imageTimestampMin, translationTimestampMin); 
  double commonRangeMax = std::min(imageTimestampMax, translationTimestampMax);

  if (commonRangeMin + maxVideoOffset >= commonRangeMax - maxVideoOffset)
  {
    std::cerr << "Insufficient overlap between tracking data and image data to compute time offset...Exiting" << std::endl;
    exit(EXIT_FAILURE);
  }

  //  Get resampled timestamps for the video sequence
  long int n = 0;
  while(commonRangeMin + n * samplingResolutionSec_ < commonRangeMax)
  {
    resampledVideoTimestamps_.push_back(commonRangeMin + n * samplingResolutionSec_);
    ++n;
  }

  //  Get resampled timestamps for the tracker sequence
  n = 0;
  while((commonRangeMin + maxVideoOffset) + n * samplingResolutionSec_ < commonRangeMax - maxVideoOffset)
  {
    resampledTrackerTimestamps_.push_back( (commonRangeMin + maxVideoOffset) + n * samplingResolutionSec_);
    ++n;
  }

  //  Get resampled metrics for video and tracker sequences
  interpolateHelper(videoMetric_,resampledVideoMetric_, resampledVideoTimestamps_, videoTimestamps_, samplingResolutionSec_);
  interpolateHelper(trackerMetric_, resampledTrackerMetric_,resampledTrackerTimestamps_, trackerTimestamps_, samplingResolutionSec_);

}// End interpolate()

PlusStatus TemporalCalibration::readFiles()
{

 if ( inputTrackerSequenceMetafile_.empty() )
  {
    std::cerr << "input-sequence-metafile required argument!" << std::endl;
    return PLUS_FAIL;
  }

  if ( inputUSImageSequenceMetafile_.empty() )
  {
    std::cerr << "input-US-image-sequence-metafile required argument!" << std::endl;
    return PLUS_FAIL;
  }

  if ( outputFilepath_.empty() )
  {
    std::cerr << "output-filepath required argument!" << std::endl;
    return PLUS_FAIL;
  }

  if ( trackerFrames_->ReadFromSequenceMetafile(inputTrackerSequenceMetafile_.c_str()) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to read tracked pose sequence metafile: " << inputTrackerSequenceMetafile_);
    return PLUS_FAIL;
  }

  if ( USVideoFrames_->ReadFromSequenceMetafile(inputUSImageSequenceMetafile_.c_str()) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to read US image sequence metafile: " << inputUSImageSequenceMetafile_);
    return PLUS_FAIL;
  }

  //  make sure all the image data is valid
  for(int i = 0 ; i < USVideoFrames_->GetNumberOfTrackedFrames(); ++i)
  {
    if(!USVideoFrames_->GetTrackedFrame(i)->GetImageData()->IsImageValid())
    {
      LOG_ERROR("Frame " << i << " is invalid. Exiting.");
      return PLUS_FAIL;
    }
  }

  return PLUS_SUCCESS;
}
