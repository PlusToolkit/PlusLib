/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#include "vtkTransformRepository.h"
#include "TemporalCalibrationAlgo.h"
#include "vtkMath.h"

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
const double IMAGE_DOWSAMPLING_FACTOR = 2.0; 

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
    charImageType::Pointer downsampledImage;

    if(localImage.IsNull())
    {
      // Dropped frame
      continue;
    }

    // Typedef's for pixel, image, reader and writer types
    typedef unsigned char T_InputPixel;
    typedef unsigned char T_OutputPixel;
   
   
    typedef itk::Image<T_InputPixel, 2> T_Image;
   
   
    // Typedefs for the different (numerous!) elements of the "resampling"
   
    // Identity transform.
    // We don't want any transform on our image except rescaling which is not
    // specified by a transform but by the input/output spacing as we will see
    // later.
    // So no transform will be specified.
    typedef itk::IdentityTransform<double, 2> T_Transform;
   
    // If ITK resampler determines there is something to interpolate which is
    // usually the case when upscaling (!) then we must specify the interpolation
    // algorithm. In our case, we want bicubic interpolation. One way to implement
    // it is with a third order b-spline. So the type is specified here and the
    // order will be specified with a method call later on.
    typedef itk::BSplineInterpolateImageFunction<T_Image, double, double> T_Interpolator;
   
    // The resampler type itself.
    typedef itk::ResampleImageFilter<T_Image, T_Image> T_ResampleFilter;
   
   

    // Prepare the resampler.
   
    // Instantiate the transform and specify it should be the id transform.
    T_Transform::Pointer _pTransform = T_Transform::New();
    _pTransform->SetIdentity();
   
    // Instantiate the b-spline interpolator and set it as the third order
    // for bicubic.
    T_Interpolator::Pointer _pInterpolator = T_Interpolator::New();
    _pInterpolator->SetSplineOrder(3);
   
    // Instantiate the resampler. Wire in the transform and the interpolator.
    T_ResampleFilter::Pointer _pResizeFilter = T_ResampleFilter::New();
    _pResizeFilter->SetTransform(_pTransform);
    _pResizeFilter->SetInterpolator(_pInterpolator);
   
    // Set the output origin. You may shift the original image "inside" the
    // new image size by specifying something else than 0.0, 0.0 here.
   
    const double vfOutputOrigin[2]  = { 0.0, 0.0 };
    _pResizeFilter->SetOutputOrigin(vfOutputOrigin);
   
    //     Compute and set the output spacing
    //     Compute the output spacing from input spacing and old and new sizes.
    //     
    //     The computation must be so that the following holds:
    //     
    //     new width         old x spacing
    //     ----------   =   ---------------
    //     old width         new x spacing
    //    
    //    
    //     new height         old y spacing
    //    ------------  =   ---------------
    //     old height         new y spacing
    //
    //     So either we specify new height and width and compute new spacings (as
    //     we do here) or we specify new spacing and compute new height and width
    //     and computations that follows need to be modified a little (as it is
    //     done at step 2 there:
    //       http://itk.org/Wiki/ITK/Examples/DICOM/ResampleDICOM)
    //
    
   
    // Fetch original image size.
    const T_Image::RegionType& inputRegion = localImage->GetLargestPossibleRegion();
    const T_Image::SizeType& vnInputSize = inputRegion.GetSize();
    unsigned int nOldWidth = vnInputSize[0];
    unsigned int nOldHeight = vnInputSize[1];
    unsigned int nNewWidth = static_cast<unsigned int>(nOldWidth / IMAGE_DOWSAMPLING_FACTOR);
    unsigned int nNewHeight = static_cast<unsigned int>(nOldHeight / IMAGE_DOWSAMPLING_FACTOR);

    // Fetch original image spacing.
    const T_Image::SpacingType& vfInputSpacing = localImage->GetSpacing();
                                              
    double vfOutputSpacing[2];
    vfOutputSpacing[0] = vfInputSpacing[0] * (double) nOldWidth / (double) nNewWidth;
    vfOutputSpacing[1] = vfInputSpacing[1] * (double) nOldHeight / (double) nNewHeight;
   
    // Set the output spacing. If you comment out the following line, the original
    // image will be simply put in the upper left corner of the new image without
    // any scaling.
    _pResizeFilter->SetOutputSpacing(vfOutputSpacing);
   
    // Set the output size as specified on the command line.
   
    itk::Size<2> vnOutputSize = { {nNewWidth, nNewHeight} };
    _pResizeFilter->SetSize(vnOutputSize);
   
    // Specify the input.
   
    _pResizeFilter->SetInput(localImage);
   
    // Write the result
    localImage = _pResizeFilter->GetOutput();

    
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
    houghTransform->Simplify();
    houghTransform->Update();

    // Get the lines of the image into the line-list holder
    itk::HoughTransform2DLinesImageFilter<charPixelType, charPixelType>::LinesListType lines = houghTransform->GetLines();

    // Iterate through the list of lines and we draw them.
    typedef HoughTransformFilterType::LinesListType::const_iterator LineIterator;
    typedef HoughTransformFilterType::LineType::PointListType PointListType;

    itk::Size<imageDimension> size = localImage->GetLargestPossibleRegion().GetSize();
    int halfwayPoint = static_cast<int> ((float) size[0]) / 2;
    
    LineIterator itLines = lines.begin();
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
