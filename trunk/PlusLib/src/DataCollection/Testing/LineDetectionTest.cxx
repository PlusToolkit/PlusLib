#include "PlusConfigure.h"
#include "vtksys/CommandLineArguments.hxx"
#include "vtkTrackedFrameList.h"
#include "TrackedFrame.h"
#include "PlusVideoFrame.h"
#include <iostream>
#include <itkHoughTransform2DLinesImageFilter.h>
#include <itkImageFileWriter.h>
#include <itkImageDuplicator.h>
#include <itkOtsuThresholdImageFilter.h>
#include <itkBinaryThresholdImageFilter.h>
#include <itkRescaleIntensityImageFilter.h>
#include "vtkMatrix4x4.h"
#include <ctype.h>
#include <time.h>


#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderWindow.h>
#include <vtkSmartPointer.h>
#include <vtkChartXY.h>
#include <vtkTable.h>
#include <vtkPlot.h>
#include <vtkFloatArray.h>
#include <vtkContextView.h>
#include <vtkContextScene.h>
#include <vtkPen.h>
#include <vtkDoubleArray.h>
#include <vtkWindowToImageFilter.h>
#include <vtkPNGWriter.h>


//----------------------------------------------------------------------------

PlusStatus LineDetection(PlusVideoFrame* videoFrame, std::vector<double> &videoMetric);
double linearInterpolation(double interpolatedTimestamp, std::vector<double> &originalMetric, std::vector<double> &originalTimestamps,
                           std::vector<int> &straddleIndices, double samplingResolutionSec);
void interpolateHelper(std::vector<double> &originalMetric, std::vector<double> &interpolatedVector,
          std::vector<double> &intepolatedTimestamp, std::vector<double> &originalTimestamp, double samplingResolutionSec);
void NormalizeMetric(std::vector<double> &trackerMetric);
double CalculateTimeOffset(std::vector<double> &trackerMetric,
                         std::vector<double> &trackerTimestamps,
                         std::vector<double> &imageModulusVector,
                         std::vector<double> &imageTimestampVector,
                         std::vector<double> &interpolatedTrackerModulusVector,
                         std::vector<double> &resampledTrackerTimestamps,
                         std::vector<double> &resampledVideoMetric,
                         std::vector<double> &resampledVideoTimestamps,
                         double samplingResolutionSec);
void interpolate(std::vector<double> &trackerMetric,
                 std::vector<double> &trackerTimestamps,
                 std::vector<double> &imageModulusVector,
                 std::vector<double> &imageTimestampVector,
                 std::vector<double> &interpolatedTrackerModulusVector,
                 std::vector<double> &resampledTrackerTimestamps,
                 std::vector<double> &resampledVideoMetric,
                 std::vector<double> &resampledVideoTimestamps,
                 double samplingResolutionSec);
int binarySearch(double key, std::vector<double> &originalTimestamp, int low, int high);
double computeCorrelation(std::vector<double> &trackerMetric, std::vector<double> &imageMetric, int indexOffset);
void xcorr(std::vector<double> &trackerMetric, std::vector<double> &imageMetric, std::vector<double> &corrValues);
void plot(std::vector<double> &resampledTrackerTimestamps, std::vector<double> &resampledTrackerMetric, std::vector<double> &resampledVideoTimestamps, std::vector<double> &resampledVideoMetric);

int main(int argc, char **argv)
{
  time_t startTime;
  time (&startTime);

  bool printHelp(false);
  bool plotResults(false);
  int verboseLevel = vtkPlusLogger::LOG_LEVEL_DEFAULT;
  std::string inputTrackerSequenceMetafile;
  std::string inputUSImageSequenceMetafile;
  double samplingResolutionSec = 0.001; //  Resolution used for re-sampling [seconds]

  vtksys::CommandLineArguments args;
  args.Initialize(argc, argv);

  args.AddArgument("--help",vtksys::CommandLineArguments::NO_ARGUMENT, &printHelp, "Print this help.");
  args.AddArgument("--input-US-image-sequence-metafile", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputUSImageSequenceMetafile, "Input US image sequence metafile name with path");
  args.AddArgument("--input-sequence-metafile-tracker-position", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputTrackerSequenceMetafile, "Input tracker sequence metafile name with path");
  args.AddArgument("--verbose",vtksys::CommandLineArguments::EQUAL_ARGUMENT, &verboseLevel, "Verbose level (1=error only, 2=warning, 3=info, 4=debug, 5=trace)");
  args.AddArgument("--plot-results",vtksys::CommandLineArguments::EQUAL_ARGUMENT, &plotResults, "Plot results (display position vs. time plots without and with temporal calibration)");
  args.AddArgument("--sampling-resolution-sec",vtksys::CommandLineArguments::EQUAL_ARGUMENT, &samplingResolutionSec, "Sampling resolution (in seconds, default is 0.001)");    

  if ( !args.Parse() )
  {
    std::cerr << "Problem parsing arguments" << std::endl;
    std::cout << "Help: " << args.GetHelp() << std::endl;
    exit(EXIT_FAILURE);
  }

   // Get the number identifier of the trial (for testing only)
  std::string numbers;
  for(int i = 0; i < inputUSImageSequenceMetafile.length(); i++)
  {
    if(isdigit(inputUSImageSequenceMetafile.at(i))){
      numbers.append(inputUSImageSequenceMetafile.substr(i,1));
    }
  }

  //  Write file indicating whether test was succesful; initially set to 0 (= 'No')
  std::string isTestValidFileName = "IsTestValid_";
  isTestValidFileName.append(numbers);
  isTestValidFileName.append(".txt");

  std::ofstream isTestValidFile;
  isTestValidFile.open(isTestValidFileName.c_str());
  isTestValidFile << 0 << std::endl;
  isTestValidFile.close();


  if ( printHelp )
  {
    std::cout << "Help: " << args.GetHelp() << std::endl;
    exit(EXIT_SUCCESS);
  }

  if ( inputTrackerSequenceMetafile.empty() )
  {
    std::cerr << "input-sequence-metafile required argument!" << std::endl;
    std::cout << "Help: " << args.GetHelp() << std::endl;
    exit(EXIT_FAILURE);
  }

  if ( inputUSImageSequenceMetafile.empty() )
  {
    std::cerr << "input-US-image-sequence-metafile required argument!" << std::endl;
    std::cout << "Help: " << args.GetHelp() << std::endl;
    exit(EXIT_FAILURE);
  }

  vtkPlusLogger::Instance()->SetLogLevel(verboseLevel);

  vtkSmartPointer<vtkTrackedFrameList> trackedFrameList = vtkSmartPointer<vtkTrackedFrameList>::New();
  vtkSmartPointer<vtkTrackedFrameList> imageList = vtkSmartPointer<vtkTrackedFrameList>::New();

  if ( trackedFrameList->ReadFromSequenceMetafile(inputTrackerSequenceMetafile.c_str()) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to read tracked pose sequence metafile: " << inputTrackerSequenceMetafile);
    return EXIT_FAILURE;
  }

  if ( imageList->ReadFromSequenceMetafile(inputUSImageSequenceMetafile.c_str()) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to read US image sequence metafile: " << inputUSImageSequenceMetafile);
    return EXIT_FAILURE;
  }

  //  make sure all the image data is valid
  for(int i = 0 ; i < imageList->GetNumberOfTrackedFrames(); ++i)
  {
    if(!imageList->GetTrackedFrame(i)->GetImageData()->IsImageValid())
    {
      LOG_ERROR("Frame " << i << " is invalid. Exiting.");
      return EXIT_FAILURE;
    }
  }

  // Declare homogeneous probe-to-tracker transformation matrix, and corresponding frame transform name.
  vtkSmartPointer<vtkMatrix4x4> probeToTrackerTransform = vtkSmartPointer<vtkMatrix4x4>::New(); 
  vtkSmartPointer<vtkMatrix4x4> referenceToTrackerTransform = vtkSmartPointer<vtkMatrix4x4>::New(); 
  vtkSmartPointer<vtkMatrix4x4> trackerToReferenceTransform = vtkSmartPointer<vtkMatrix4x4>::New();
  vtkSmartPointer<vtkMatrix4x4> probeToReferenceTransform = vtkSmartPointer<vtkMatrix4x4>::New();

  PlusTransformName transformName;
  transformName.SetTransformName("ProbeToTrackerTransform");
  PlusTransformName referenceToTrackerTransformName;
  referenceToTrackerTransformName.SetTransformName("ReferenceToTrackerTransform");


  double trackerTranslationModulus; //  Euclidean translation of tracker from reference.
  std::vector<double> trackerMetric;  //  Stores the metric values for the tracker data stream.
  std::vector<double> trackerTimestamps; // Stores timestamps for the tracker data stream.
  
  TrackedFrame *trackedFrame;

  //  For each tracker position in the recorded tracker sequence, get its translation from reference.
  for ( int frame = 0; frame < trackedFrameList->GetNumberOfTrackedFrames(); ++frame )
  {
    trackerTranslationModulus = 0;
    trackedFrame = trackedFrameList->GetTrackedFrame(frame);
    trackerTimestamps.push_back(trackedFrame->GetTimestamp());
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
    trackerMetric.push_back(trackerTranslationModulus);
  }

  //  Normalize the tracker metric.
  NormalizeMetric(trackerMetric);

  //  Write the tracker metric to file.
  std::string trackerMetricFileName = "normalizedTrackerMetric_";
  trackerMetricFileName.append(numbers);
  trackerMetricFileName.append(".txt");

  std::ofstream probeToReferenceTransformNormalizedFile;
  probeToReferenceTransformNormalizedFile.open (trackerMetricFileName.c_str());
  for(int i = 0; i < trackerMetric.size(); ++i)
    probeToReferenceTransformNormalizedFile << trackerMetric.at(i) << "," << trackerTimestamps.at(i) << std::endl;

  probeToReferenceTransformNormalizedFile.close();


  std::vector<double> videoMetric;
  std::vector<double> imageTimestampVector;
  int failedFrames = 0;
  for ( int frame = 0; frame < imageList->GetNumberOfTrackedFrames(); ++frame )
  {
    imageTimestampVector.push_back(imageList->GetTrackedFrame(frame)->GetTimestamp());
    if ( LineDetection(imageList->GetTrackedFrame(frame)->GetImageData(), videoMetric) != PLUS_SUCCESS )
    {
      LOG_ERROR("Line detection failed for frame#" << frame);
      failedFrames++;
      return EXIT_FAILURE;
      //if( (float)failedFrames / imageList->GetNumberOfTrackedFrames() > 0.1)
      //{
      //  LOG_ERROR("Line detection has failed on more than 10% of the frames. Exiting...");
      //  return EXIT_FAILURE;
      //}
    }

  }

  NormalizeMetric(videoMetric);

  //  Write image metric to file
  std::string imageMetricFileName = "normalizedImageMetric_";
  imageMetricFileName.append(numbers);
  imageMetricFileName.append(".txt");


  std::ofstream videoMetricFile;
  videoMetricFile.open(imageMetricFileName.c_str());
  for(int i = 0; i < videoMetric.size(); ++i)
    videoMetricFile << videoMetric.at(i) << "," << imageTimestampVector.at(i) << std::endl;

  videoMetricFile.close();

  std::vector<double> resampledTrackerMetric;
  std::vector<double> resampledTrackerTimestamps;
  std::vector<double> resampledVideoMetric;
  std::vector<double> resampledVideoTimestamps;

  double trackerLag; // Time by which the tracker data lags the video data [s]
  trackerLag = CalculateTimeOffset(trackerMetric,
                      trackerTimestamps,
                      videoMetric,
                      imageTimestampVector,
                      resampledTrackerMetric,
                      resampledTrackerTimestamps,
                      resampledVideoMetric,
                      resampledVideoTimestamps,
                      samplingResolutionSec);

  //  stop timer
  time_t endTime;
  time(&endTime);

  //  Write run-time to file.
  std::string runTimeFileName = "RunTime_";
  runTimeFileName.append(numbers);
  runTimeFileName.append(".txt");

  std::ofstream runTimeFile;
  runTimeFile.open(runTimeFileName.c_str());
  runTimeFile << difftime(endTime, startTime) << std::endl;
  runTimeFile.close();

  //  Write the calculated tracker lag to file.
  std::string trackerLagFilename = "TrackerLag_";
  trackerLagFilename.append(numbers);
  trackerLagFilename.append(".txt");

  std::ofstream trackerLagFile;
  trackerLagFile.open(trackerLagFilename.c_str());
  trackerLagFile << trackerLag << std::endl;
  trackerLagFile.close();

  //  Write file indicating whether test was succesful; set to 1 (= 'Yes').
  isTestValidFile.open(isTestValidFileName.c_str());
  isTestValidFile << 1 << std::endl;
  isTestValidFile.close();

  //  Create an aligned Tracker timestamp vector.
   std::vector<double> alignedTrackerTimestampVector;
   for(int i = 0; i < resampledTrackerTimestamps.size(); ++i)
      alignedTrackerTimestampVector.push_back(resampledTrackerTimestamps.at(i) - trackerLag);

   if (plotResults)
   {
     //  Plot metrics before and after alignment
     plot(resampledTrackerTimestamps, resampledTrackerMetric, resampledVideoTimestamps, resampledVideoMetric);
     plot(alignedTrackerTimestampVector, resampledTrackerMetric, resampledVideoTimestamps,  resampledVideoMetric);
   }

  return EXIT_SUCCESS;
}

PlusStatus LineDetection(PlusVideoFrame* videoFrame, std::vector<double> &videoMetric)
{
  LOG_DEBUG("Performing LineDetection..." << videoMetric.size());

  // Update frame counter
  static int frameIndex = -1; //because frame indices start at zero
  frameIndex++;

  // Define some image types and image dimension
  const int imageDimension(2);
  typedef unsigned char charPixelType; //the natural type of the input image
  typedef float floatPixelType; //the type of pixel used for the Hough accumulator
  typedef itk::Image<charPixelType,imageDimension> charImageType;
  
  // Get curent image
  charImageType::Pointer localImage = videoFrame->GetImage<charPixelType>();

  if(localImage.IsNull())
  {
    std::cerr << "Frame " << frameIndex << "is invalid." << std::endl;
    frameIndex--; //  ignore this frame
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

  // Create a threshold filter and use Otsu's threshold for thresholding value
  typedef itk::BinaryThresholdImageFilter<charImageType, charImageType> binaryThresholdImageFilterType;
  binaryThresholdImageFilterType::Pointer thresholdFilter = binaryThresholdImageFilterType::New();
  thresholdFilter->SetInput(localImage);
  thresholdFilter->SetLowerThreshold(0);
  thresholdFilter->SetUpperThreshold(otsuFilter->GetThreshold());
  thresholdFilter->SetInsideValue(0);
  thresholdFilter->SetOutsideValue(255);

  // Set parameters of the Hough transform filter
  double houghLineDetectionThreshold = 0; //threshold above which pixels will be considered by Hough
  int houghNumberOfLinesToDetect = 1;
  int houghDiscRadius = 10;
  houghTransform->SetInput(thresholdFilter->GetOutput());
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

  // Define output image
  typedef unsigned char outputPixelType;
  typedef itk::Image<outputPixelType, imageDimension> outputCharImageType;

  // Duplicate the original image to get a copy to draw detected lines on
  typedef itk::ImageDuplicator<charImageType> DuplicatorType;
  DuplicatorType::Pointer duplicator = DuplicatorType::New();
  duplicator->SetInputImage(localImage);
  duplicator->Update();
  charImageType::Pointer localOutputImage = duplicator->GetOutput();
  
  // Iterate through the list of lines and we draw them.
  typedef HoughTransformFilterType::LinesListType::const_iterator LineIterator;
  typedef HoughTransformFilterType::LineType::PointListType PointListType;
  LineIterator itLines = lines.begin();

  //std::ofstream InterceptFile;
  //InterceptFile.open ("InterceptFile.txt", std::ios::app);
  itk::Size<2> size = localOutputImage->GetLargestPossibleRegion().GetSize();
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
    videoMetric.push_back(m*halfwayPoint + b);
  
    double norm = vcl_sqrt(v[0]*v[0]+v[1]*v[1]);
    v[0] /= norm;
    v[1] /= norm;

    // Draw a white pixels in the output image to represent the line.
    charImageType::IndexType localIndex;
    float diag = vcl_sqrt((float)( size[0]*size[0] + size[1]*size[1] ));

    float p_1 = 0;


    for(int i = static_cast<int>(-diag); i<static_cast<int>(diag); i++)
    {
      localIndex[0]=(long int)(u[0]+i*v[0]);
      localIndex[1]=(long int)(u[1]+i*v[1]);

      outputCharImageType::RegionType outputRegion = localOutputImage->GetLargestPossibleRegion();

      if(outputRegion.IsInside(localIndex))
      {
        localOutputImage->SetPixel(localIndex, 255);
      }
    }

    itLines++;
  }

  return PLUS_SUCCESS;
}


void NormalizeMetric(std::vector<double> &metric){

  //  Get the mean of the Euclidean distances
  double mu = 0;
  for(int i = 0; i < metric.size(); ++i)
    mu += metric.at(i);

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
    metric.at(i) /= normFactor; 

}// End NormalizeMetric()

double CalculateTimeOffset(std::vector<double> &trackerMetric,
                         std::vector<double> &trackerTimestamps,
                         std::vector<double> &videoMetric,
                         std::vector<double> &videoTimestamps,
                         std::vector<double> &resampledTrackerMetric,
                         std::vector<double> &resampledTrackerTimestamps,
                         std::vector<double> &resampledVideoMetric,
                         std::vector<double> &resampledVideoTimestamps,
                         double samplingResolutionSec
                         )
{
  //  Resample the image and tracker signals, preparing them for cross correlation
  interpolate(trackerMetric,
              trackerTimestamps,
              videoMetric,
              videoTimestamps,
              resampledTrackerMetric,
              resampledTrackerTimestamps,
              resampledVideoMetric,
              resampledVideoTimestamps,
              samplingResolutionSec);

  //  Perform cross correlation
  std::vector<double> corrValues;
  xcorr(resampledTrackerMetric, resampledVideoMetric, corrValues);
  
  //  Find the index offset corresponding to maximum correlation value
  double maxCorrVal = corrValues.at(0);
  int maxCorrIndex = 0;
  for(int i = 1; i < corrValues.size(); ++i)
  {
    if(corrValues.at(i) > maxCorrVal)
    {
      maxCorrVal = corrValues.at(i);
      maxCorrIndex = i;
    }
  }

  //  Compute the time that the tracker data lags the video data
  double trackerLag;  //  Time by which the tracker stream lags the video stream [seconds]
  double maxVideoOffset = 2; //  Maximum anticipated time offset [seconds]

  trackerLag = maxVideoOffset - (maxCorrIndex)*samplingResolutionSec;
  std::cout << "Tracker stream lags image stream by: " << trackerLag << " [s]" << std::endl;

  return trackerLag; 

}// End CalculateTimeOffset()

 
void interpolate(std::vector<double> &trackerMetric,
                 std::vector<double> &trackerTimestamps,
                 std::vector<double> &videoMetric,
                 std::vector<double> &videoTimestamps,
                 std::vector<double> &resampledTrackerMetric,
                 std::vector<double> &resampledTrackerTimestamps,
                 std::vector<double> &resampledVideoMetric,
                 std::vector<double> &resampledVideoTimestamps,
                 double samplingResolutionSec)
{

  double maxVideoOffset = 2; //  Maximum anticipated time offset [seconds]

  //  Find the time-range that is common to both tracker and image signals
  double translationTimestampMin = trackerTimestamps.at(1);
  double translationTimestampMax = trackerTimestamps.at(trackerTimestamps.size() - 1);

  double imageTimestampMin = videoTimestamps.at(1);
  double imageTimestampMax = videoTimestamps.at(videoTimestamps.size() - 1);

  double commonRangeMin = std::max(imageTimestampMin, translationTimestampMin); 
  double commonRangeMax = std::min(imageTimestampMax, translationTimestampMax);

  if (commonRangeMin + maxVideoOffset >= commonRangeMax - maxVideoOffset)
  {
    std::cerr << "Insufficient overlap between tracking data and image data to compute time offset...Exiting" << std::endl;
    exit(EXIT_FAILURE);
  }

  //  Get resampled timestamps for the video sequence
  long int n = 0;
  while(commonRangeMin + n * samplingResolutionSec < commonRangeMax)
  {
    resampledVideoTimestamps.push_back(commonRangeMin + n * samplingResolutionSec);
    ++n;
  }

  //  Get resampled timestamps for the tracker sequence
  n = 0;
  while((commonRangeMin + maxVideoOffset) + n * samplingResolutionSec < commonRangeMax - maxVideoOffset)
  {
    resampledTrackerTimestamps.push_back( (commonRangeMin + maxVideoOffset) + n * samplingResolutionSec);
    ++n;
  }

  //  Get resampled metrics for video and tracker sequences
  interpolateHelper(videoMetric,resampledVideoMetric, resampledVideoTimestamps, videoTimestamps, samplingResolutionSec);
  interpolateHelper(trackerMetric, resampledTrackerMetric,resampledTrackerTimestamps, trackerTimestamps, samplingResolutionSec);

}// End interpolate()

void interpolateHelper(std::vector<double> &originalMetric, std::vector<double> &interpolatedVector,
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

double linearInterpolation(double interpolatedTimestamp, std::vector<double> &originalMetric, 
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
 

void xcorr(std::vector<double> &trackerMetric, std::vector<double> &videoMetric, std::vector<double> &corrValues)
{

  int n = 0;
  corrValues.reserve(videoMetric.size() - trackerMetric.size() + 1); // pre-allocate
  while(n + (trackerMetric.size() - 1) < videoMetric.size())
  {
    corrValues.push_back(computeCorrelation(trackerMetric, videoMetric, n));
    ++n;
  }

} // end xcorr()


double computeCorrelation(std::vector<double> &trackerMetric, std::vector<double> &videoMetric, int indexOffset)
{

  double overlapSum = 0;
  for(long int i = 0; i < trackerMetric.size(); ++i)
    overlapSum += trackerMetric.at(i) * videoMetric.at(i + indexOffset);

  return overlapSum;

} // End computeCorrelation()

void plot(std::vector<double> &resampledTrackerTimestamps, std::vector<double> &resampledTrackerMetric, std::vector<double> &resampledVideoTimestamps, std::vector<double> &resampledVideoMetric)
{

  //  Create table
  vtkSmartPointer<vtkTable> table = vtkSmartPointer<vtkTable>::New();
  vtkSmartPointer<vtkTable> table2 = vtkSmartPointer<vtkTable>::New();

  //  Create array correpsonding to the time values of the tracker plot
  vtkSmartPointer<vtkFloatArray> arrResampledTrackerTimestamps = vtkSmartPointer<vtkFloatArray>::New();
  arrResampledTrackerTimestamps->SetName("Time [s]"); 
  table->AddColumn(arrResampledTrackerTimestamps);
 
  //  Create array corresponding to the metric values of the tracker plot
  vtkSmartPointer<vtkFloatArray> arrResampledTrackerMetric = vtkSmartPointer<vtkFloatArray>::New();
  arrResampledTrackerMetric->SetName("Tracker Metric");
  table->AddColumn(arrResampledTrackerMetric);

  //  Create array correpsonding to the time values of the video plot
  vtkSmartPointer<vtkFloatArray> arrResampledVideoTimestamps = vtkSmartPointer<vtkFloatArray>::New();
  arrResampledVideoTimestamps->SetName("Time [s]"); 
  table2->AddColumn(arrResampledVideoTimestamps);
 
  //  Create array corresponding to the metric values of the video plot
  vtkSmartPointer<vtkFloatArray> arrResampledVideoMetric = vtkSmartPointer<vtkFloatArray>::New();
  arrResampledVideoMetric->SetName("Video Metric");
  table2->AddColumn(arrResampledVideoMetric);
 
  // Set the tracker data
  table->SetNumberOfRows(resampledTrackerTimestamps.size());
  for (int i = 0; i < resampledTrackerTimestamps.size(); ++i)
  {
    table->SetValue(i, 0, static_cast<float>(resampledTrackerTimestamps.at(i)));
    table->SetValue(i, 1, static_cast<float>(resampledTrackerMetric.at(i)));
  }

  // Set the video data
  table2->SetNumberOfRows(resampledVideoTimestamps.size());
  for (int i = 0; i < resampledVideoTimestamps.size(); ++i)
  {
    table2->SetValue(i, 0, static_cast<float>(resampledVideoTimestamps.at(i)));
    table2->SetValue(i, 1, static_cast<float>(resampledVideoMetric.at(i)));
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

  vtkPlot *line2 = chart->AddPlot(vtkChart::LINE);

  #if VTK_MAJOR_VERSION <= 5
    line2->SetInput(table2, 0, 1);
  #else
    line->SetInputData(table2, 0, 1);
  #endif

  line2->SetColor(0, 255, 0, 255);
  line2->SetWidth(1.0);
  line2->GetPen()->SetColor(255,0, 0);
  line2 = chart->AddPlot(vtkChart::LINE);

  // Start interactor
  view->GetInteractor()->Initialize();
  view->GetInteractor()->Start();

} //  End plot()