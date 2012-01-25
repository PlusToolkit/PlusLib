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


//----------------------------------------------------------------------------

PlusStatus LineDetection(PlusVideoFrame* videoFrame, std::vector<double> &linePositionMetric);
double linearInterpolation(double interpVal, std::vector<double> &origVec, std::vector<double> &originalTimestamp,
                           std::vector<int> &straddleIndices);
void interpolateHelper(std::vector<double> &origVec, std::vector<double> &interpolatedVector,
          std::vector<double> &intepolatedTimestamp, std::vector<double> &originalTimestamp);
void NormalizeMetric(std::vector<double> &translationModulusVector);
double CalculateTimeOffset(std::vector<double> &translationModulusVector,
                         std::vector<double> &trackerTimestampVector,
                         std::vector<double> &imageModulusVector,
                         std::vector<double> &imageTimestampVector,
                         std::vector<double> &interpolatedTrackerModulusVector,
                         std::vector<double> &interpolatedTrackerTimestampVector,
                         std::vector<double> &interpolatedImageModulusVector,
                         std::vector<double> &interpolatedImageTimestampVector);
void interpolate(std::vector<double> &translationModulusVector,
                 std::vector<double> &trackerTimestampVector,
                 std::vector<double> &imageModulusVector,
                 std::vector<double> &imageTimestampVector,
                 std::vector<double> &interpolatedTrackerModulusVector,
                 std::vector<double> &interpolatedTrackerTimestampVector,
                 std::vector<double> &interpolatedImageModulusVector,
                 std::vector<double> &interpolatedImageTimestampVector);
int binarySearch(double key, std::vector<double> &originalTimestamp, int low, int high);
double computeCorrelation(std::vector<double> &trackerMetric, std::vector<double> &imageMetric, int indexOffset);
void xcorr(std::vector<double> &trackerMetric, std::vector<double> &imageMetric, std::vector<double> &corrValues);
void plot(std::vector<double> &x1, std::vector<double> &y1, std::vector<double> &x2, std::vector<double> &y2);

int main(int argc, char **argv)
{
  time_t startTime;
  time (&startTime);


  bool printHelp(false);
  int verboseLevel = vtkPlusLogger::LOG_LEVEL_DEFAULT;
  std::string inputTrackerSequenceMetafile;
  std::string inputUSImageSequenceMetafile;

  vtksys::CommandLineArguments args;
  args.Initialize(argc, argv);

  args.AddArgument("--help",vtksys::CommandLineArguments::NO_ARGUMENT, &printHelp, "Print this help.");
  args.AddArgument("--input-US-image-sequence-metafile", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputUSImageSequenceMetafile, "Input US image sequence metafile name with path");
  args.AddArgument("--input-sequence-metafile-tracker-position", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputTrackerSequenceMetafile, "Input tracker sequence metafile name with path");
  args.AddArgument("--verbose",vtksys::CommandLineArguments::EQUAL_ARGUMENT, &verboseLevel, "Verbose level (1=error only, 2=warning, 3=info, 4=debug, 5=trace)");


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

  // Declare homogeneous probe-to-tracker transformation matrix, and corresponding frame transform name
  vtkSmartPointer<vtkMatrix4x4> probeToTrackerTransform = vtkSmartPointer<vtkMatrix4x4>::New(); 
  vtkSmartPointer<vtkMatrix4x4> referenceToTrackerTransform = vtkSmartPointer<vtkMatrix4x4>::New(); 
  vtkSmartPointer<vtkMatrix4x4> trackerToReferenceTransform = vtkSmartPointer<vtkMatrix4x4>::New();
  vtkSmartPointer<vtkMatrix4x4> probeToReferenceTransform = vtkSmartPointer<vtkMatrix4x4>::New();

  PlusTransformName transformName;
  transformName.SetTransformName("ProbeToTrackerTransform");
  PlusTransformName referenceToTrackerTransformName;
  referenceToTrackerTransformName.SetTransformName("ReferenceToTrackerTransform");


  double translationModulus;
  double tmpTranslationElement;
  std::vector<double> translationModulusVector;
  std::vector<double> trackerTimestampVector;
  TrackedFrame *trackedFrame;

  for ( int frame = 0; frame < trackedFrameList->GetNumberOfTrackedFrames(); ++frame )
  {
    translationModulus = 0;
    trackedFrame = trackedFrameList->GetTrackedFrame(frame);
    trackerTimestampVector.push_back(trackedFrame->GetTimestamp());
    trackedFrame->GetCustomFrameTransform(transformName, probeToTrackerTransform);
    trackedFrame->GetCustomFrameTransform(referenceToTrackerTransformName, referenceToTrackerTransform);
    referenceToTrackerTransform->Invert(referenceToTrackerTransform,trackerToReferenceTransform);
    vtkMatrix4x4::Multiply4x4(trackerToReferenceTransform, probeToTrackerTransform, probeToReferenceTransform);
  
    //  Get the Euclidean distance of the probe to the tracker = (tx^2 + ty^2 + tz^2)^(1/2)
    for(int i = 0; i < 3; ++i)
    {
      tmpTranslationElement = probeToReferenceTransform->GetElement(i, 3);
      translationModulus += tmpTranslationElement*tmpTranslationElement;
    }
    
    translationModulus = std::sqrt(translationModulus);
    translationModulusVector.push_back(translationModulus);
  }

  // Normalize the metric
  NormalizeMetric(translationModulusVector);

  // Write the tracker metric to file
  std::string trackerMetricFileName = "normalizedTrackerMetric_";
  trackerMetricFileName.append(numbers);
  trackerMetricFileName.append(".txt");

  std::ofstream probeToReferenceTransformNormalizedFile;
  probeToReferenceTransformNormalizedFile.open (trackerMetricFileName.c_str());
  for(int i = 0; i < translationModulusVector.size(); ++i)
    probeToReferenceTransformNormalizedFile << translationModulusVector.at(i) << "," << trackerTimestampVector.at(i) << std::endl;

  probeToReferenceTransformNormalizedFile.close();

  std::vector<double> linePositionMetric;
  std::vector<double> imageTimestampVector;
  int failedFrames = 0;
  for ( int frame = 0; frame < imageList->GetNumberOfTrackedFrames(); ++frame )
  {
    imageTimestampVector.push_back(imageList->GetTrackedFrame(frame)->GetTimestamp());
    if ( LineDetection(imageList->GetTrackedFrame(frame)->GetImageData(), linePositionMetric) != PLUS_SUCCESS )
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

  NormalizeMetric(linePositionMetric);

  //  Write image metric to file
  std::string imageMetricFileName = "normalizedImageMetric_";
  imageMetricFileName.append(numbers);
  imageMetricFileName.append(".txt");


  std::ofstream linePositionMetricFile;
  linePositionMetricFile.open(imageMetricFileName.c_str());
  for(int i = 0; i < linePositionMetric.size(); ++i)
    linePositionMetricFile << linePositionMetric.at(i) << "," << imageTimestampVector.at(i) << std::endl;

  linePositionMetricFile.close();

  std::vector<double> interpolatedTranslationModulusVector;
  std::vector<double> interpolatedTrackerTimestampVector;
  std::vector<double> interpolatedImageModulusVector;
  std::vector<double> interpolatedImageTimestampVector;

  double trackerLag; // Time by which the tracker data lags the video data [s]
  trackerLag = CalculateTimeOffset(translationModulusVector,
                      trackerTimestampVector,
                      linePositionMetric,
                      imageTimestampVector,
                      interpolatedTranslationModulusVector,
                      interpolatedTrackerTimestampVector,
                      interpolatedImageModulusVector,
                      interpolatedImageTimestampVector);
  
  /*
  // Write the intepolated tracker metric to file
  std::string interpolatedTrackerMetricFileName = "normalizedInterpolatedTrackerMetric_";
  interpolatedTrackerMetricFileName.append(numbers);
  interpolatedTrackerMetricFileName.append(".txt");

  std::ofstream interpolatedProbeToReferenceTransformNormalizedFile;
  interpolatedProbeToReferenceTransformNormalizedFile.open (interpolatedTrackerMetricFileName.c_str());
  for(int i = 0; i < interpolatedTranslationModulusVector.size(); ++i)
    interpolatedProbeToReferenceTransformNormalizedFile << interpolatedTranslationModulusVector.at(i) << "," << interpolatedTrackerTimestampVector.at(i) << std::endl;

  interpolatedProbeToReferenceTransformNormalizedFile.close();
  */

  //  stop timer
  time_t endTime;
  time(&endTime);

  std::string runTimeFileName = "RunTime_";
  runTimeFileName.append(numbers);
  runTimeFileName.append(".txt");

  std::ofstream runTimeFile;
  runTimeFile.open(runTimeFileName.c_str());
  runTimeFile << difftime(endTime, startTime) << std::endl;
  runTimeFile.close();

  std::string trackerLagFilename = "TrackerLag_";
  trackerLagFilename.append(numbers);
  trackerLagFilename.append(".txt");

  std::ofstream trackerLagFile;
  trackerLagFile.open(trackerLagFilename.c_str());
  trackerLagFile << trackerLag << std::endl;
  trackerLagFile.close();

  //  Write file indicating whether test was succesful; set to 1 (= 'Yes')
  isTestValidFile.open(isTestValidFileName.c_str());
  isTestValidFile << 1 << std::endl;
  isTestValidFile.close();

   std::vector<double> alignedTrackerTimestampVector;
   for(int i = 0; i < interpolatedTrackerTimestampVector.size(); ++i)
      alignedTrackerTimestampVector.push_back(interpolatedTrackerTimestampVector.at(i) - trackerLag);

  //  Plot metrics before and after alignment
  plot(interpolatedTrackerTimestampVector, interpolatedTranslationModulusVector, interpolatedImageTimestampVector, interpolatedImageModulusVector);
  plot(alignedTrackerTimestampVector, interpolatedTranslationModulusVector, interpolatedImageTimestampVector,  interpolatedImageModulusVector);

  return EXIT_SUCCESS;
}

PlusStatus LineDetection(PlusVideoFrame* videoFrame, std::vector<double> &linePositionMetric)
{
  LOG_DEBUG("Performing LineDetection..." << linePositionMetric.size());

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
    linePositionMetric.push_back(m*halfwayPoint + b);
  
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


void NormalizeMetric(std::vector<double> &translationModulusVector){

  //  Get the mean of the Euclidean distances
  double mu = 0;
  for(int i = 0; i < translationModulusVector.size(); ++i)
    mu += translationModulusVector.at(i);

  mu /= translationModulusVector.size();

  ////  Get the standard deviation
  //double sigma = 0;
  //for(int i = 0; i < translationModulusVector.size(); ++i)
  //  sigma += (translationModulusVector.at(i) - mu) * (translationModulusVector.at(i) - mu);

  //sigma /= (translationModulusVector.size() - 1); //with Bessel correction factor (i.e. N-1)
  //sigma = std::sqrt(sigma);

  //  Normalize each measurement, s, as s' = (s-mu)/sigma
  for(int i = 0; i < translationModulusVector.size(); ++i)
  {
    translationModulusVector.at(i) -= mu;
    /*translationModulusVector.at(i) /= sigma;*/
  }

  //  Get the maximum and minimum signal values
  double maxVal = translationModulusVector.at(1);
  double minVal = translationModulusVector.at(1);

  for(int i = 1; i < translationModulusVector.size(); ++i)
  {
    if(translationModulusVector.at(i) > maxVal)
    {
      maxVal = translationModulusVector.at(i);
    }
    else if(translationModulusVector.at(i) < minVal)
    {
       minVal = translationModulusVector.at(i);
    }

  }// end for-loop

  // normalize signal
  double normFactor = std::abs(maxVal) + std::abs(minVal);

  for(int i = 0; i < translationModulusVector.size(); ++i)
    translationModulusVector.at(i) /= normFactor; 

}// End NormalizeMetric()

double CalculateTimeOffset(std::vector<double> &translationModulusVector,
                         std::vector<double> &trackerTimestampVector,
                         std::vector<double> &imageModulusVector,
                         std::vector<double> &imageTimestampVector,
                         std::vector<double> &interpolatedTrackerModulusVector,
                         std::vector<double> &interpolatedTrackerTimestampVector,
                         std::vector<double> &interpolatedImageModulusVector,
                         std::vector<double> &interpolatedImageTimestampVector)
{
  //  Resample the image and tracker signals, preparing them for cross correlation
  interpolate(translationModulusVector,
              trackerTimestampVector,
              imageModulusVector,
              imageTimestampVector,
              interpolatedTrackerModulusVector,
              interpolatedTrackerTimestampVector,
              interpolatedImageModulusVector,
              interpolatedImageTimestampVector);

  //  Perform cross correlation
  std::vector<double> corrValues;
  xcorr(interpolatedTrackerModulusVector, interpolatedImageModulusVector, corrValues);
  
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
  double trackerLag;
  double maxVideoOffset = 2; //  seconds
  double videoOffsetResolution = 0.001; //  seconds

  trackerLag = maxVideoOffset - (maxCorrIndex)*videoOffsetResolution;
  std::cout << "Tracker stream lags image stream by: " << trackerLag << " [s]" << std::endl;

  return trackerLag; 

}// End CalculateTimeOffset()

 
void interpolate(std::vector<double> &translationModulusVector,
                 std::vector<double> &trackerTimestampVector,
                 std::vector<double> &imageModulusVector,
                 std::vector<double> &imageTimestampVector,
                 std::vector<double> &interpolatedTrackerModulusVector,
                 std::vector<double> &interpolatedTrackerTimestampVector,
                 std::vector<double> &interpolatedImageModulusVector,
                 std::vector<double> &interpolatedImageTimestampVector)
{

  double maxVideoOffset = 2; //  Maximum anticipated time offset [seconds]
  double videoOffsetResolution = 0.001; //  Resolution used for re-sampling [seconds]

  //  Find the time-range that is common to both tracker and image signals
  double translationTimestampMin = trackerTimestampVector.at(1);
  double translationTimestampMax = trackerTimestampVector.at(trackerTimestampVector.size() - 1);

  double imageTimestampMin = imageTimestampVector.at(1);
  double imageTimestampMax = imageTimestampVector.at(imageTimestampVector.size() - 1);

  double commonRangeMin = std::max(imageTimestampMin, translationTimestampMin); 
  double commonRangeMax = std::min(imageTimestampMax, translationTimestampMax);

  if (commonRangeMin + maxVideoOffset >= commonRangeMax - maxVideoOffset)
  {
    std::cerr << "Insufficient overlap between tracking data and image data to compute time offset...Exiting" << std::endl;
    exit(EXIT_FAILURE);
  }


  //  Get resampled timestamp for the image sequence
  long int n = 0;
  while(commonRangeMin + n * videoOffsetResolution < commonRangeMax)
  {
    interpolatedImageTimestampVector.push_back(commonRangeMin + n * videoOffsetResolution);
    ++n;
  }

  //  Get resampled timestamp for the tracker sequence.
  n = 0;
  while((commonRangeMin + maxVideoOffset) + n * videoOffsetResolution < commonRangeMax - maxVideoOffset)
  {
    interpolatedTrackerTimestampVector.push_back( (commonRangeMin + maxVideoOffset) + n * videoOffsetResolution);
    ++n;
  }

  //  Get resampled image and tracker signals 
  interpolateHelper(imageModulusVector,interpolatedImageModulusVector, interpolatedImageTimestampVector, imageTimestampVector);
  interpolateHelper(translationModulusVector, interpolatedTrackerModulusVector,interpolatedTrackerTimestampVector, trackerTimestampVector);

}

void interpolateHelper(std::vector<double> &origVec, std::vector<double> &interpolatedVector,
          std::vector<double> &interpolatedTimestamp, std::vector<double> &originalTimestamp)
{

  //  For the first interpolated timestamp value, find the index of the closest element in the
  //  original timestamp array.
  int closestIndex = 0;
  int closestVal = std::abs(originalTimestamp.at(closestIndex) - interpolatedTimestamp.at(0));
  
  for(int i = 1; i < originalTimestamp.size(); ++i)
  {
    if(std::abs(originalTimestamp.at(i) - interpolatedTimestamp.at(0)) < closestVal)
    {
      closestIndex = i;
      closestVal = std::abs(originalTimestamp.at(i) - interpolatedTimestamp.at(0));
    }
  }

  
  //  Assign two indices that "straddle" the first interpolated timestamp value in the original timstamp sequence
  int indexHigh;
  int indexLow;
  if(closestIndex == 0 || closestIndex == originalTimestamp.size() - 1)
  {
    indexHigh = closestIndex;
    indexLow = indexHigh;
  }
  else if(originalTimestamp.at(closestIndex) > interpolatedTimestamp.at(0))
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

  interpolatedVector.reserve(interpolatedTimestamp.size()); //  pre-allocate
  for(int i = 0; i < interpolatedTimestamp.size(); ++i)
    interpolatedVector.push_back(linearInterpolation(interpolatedTimestamp.at(i), origVec, 
                                 originalTimestamp, straddleIndices) );

} //  interpolateHelper()

double linearInterpolation(double interpVal, std::vector<double> &origVec, std::vector<double> &originalTimestamp,
                           std::vector<int> &straddleIndices)
{
  const int lowIndex = 0; //  position of low index in "straddleIndices"
  const int highIndex = 1; // position of high index in "straddleIndices"
  double timeEpsilon = 0.0000001; //  resolution under which two time values are considered identical
  
  //  We assume that we are upsampling (i.e. that the rate of resampling is less than the original sampling rate).
  if(interpVal > originalTimestamp.at(straddleIndices.at(highIndex)))
  {
    if(straddleIndices.at(highIndex) == straddleIndices.at(lowIndex))
    {
      straddleIndices.at(highIndex) = straddleIndices.at(highIndex) + 1;
    }
    else
    {
      straddleIndices.at(highIndex) = straddleIndices.at(highIndex) + 1;
      straddleIndices.at(lowIndex) = straddleIndices.at(lowIndex) + 1;
    }
  }

  //  If the time index between the two straddling indices is very small, avoid interpolation to
  //  prevent divding by a really small number.
  if(std::abs(originalTimestamp.at(straddleIndices.at(highIndex)) - originalTimestamp.at(straddleIndices.at(lowIndex))) < timeEpsilon)
    return origVec.at(straddleIndices.at(highIndex));
  
  // Peform linear interpolation
  double m = (origVec.at(straddleIndices.at(highIndex)) - origVec.at(straddleIndices.at(lowIndex))) / 
             (originalTimestamp.at(straddleIndices.at(highIndex)) - originalTimestamp.at(straddleIndices.at(lowIndex)));
  return origVec.at(straddleIndices.at(lowIndex)) + (interpVal - originalTimestamp.at(straddleIndices.at(lowIndex))) * m;

}// end linearInterpolation()
 

void xcorr(std::vector<double> &trackerMetric, std::vector<double> &imageMetric, std::vector<double> &corrValues)
{
  int n = 0;
  corrValues.reserve(imageMetric.size() - trackerMetric.size() + 1); // pre-allocate
  while(n + (trackerMetric.size() - 1) < imageMetric.size())
  {
    corrValues.push_back(computeCorrelation(trackerMetric, imageMetric, n));
    ++n;
  }

}// end xcorr()

double computeCorrelation(std::vector<double> &trackerMetric, std::vector<double> &imageMetric, int indexOffset){

  double overlapSum = 0;
  for(long int i = 0; i < trackerMetric.size(); ++i)
    overlapSum += trackerMetric.at(i) * imageMetric.at(i+indexOffset);

  return overlapSum;

}// end computeCorrelation()

void plot(std::vector<double> &x1, std::vector<double> &y1, std::vector<double> &x2, std::vector<double> &y2)
{

  // Create a table with some points in it
  vtkSmartPointer<vtkTable> table = vtkSmartPointer<vtkTable>::New();
  vtkSmartPointer<vtkTable> table2 = vtkSmartPointer<vtkTable>::New();

  vtkSmartPointer<vtkFloatArray> arrX1 = vtkSmartPointer<vtkFloatArray>::New();
  arrX1->SetName("yime [s]"); 
  table->AddColumn(arrX1);
 
  vtkSmartPointer<vtkFloatArray> arrY1 = vtkSmartPointer<vtkFloatArray>::New();
  arrY1->SetName("Tracker Metric");
  table->AddColumn(arrY1);

  vtkSmartPointer<vtkFloatArray> arrX2 = vtkSmartPointer<vtkFloatArray>::New();
  arrX2->SetName("time [s]"); 
  table2->AddColumn(arrX2);
 
  vtkSmartPointer<vtkFloatArray> arrY2 = vtkSmartPointer<vtkFloatArray>::New();
  arrY2->SetName("Video Metric");
  table2->AddColumn(arrY2);
 
 
  // Fill in the table with some example values
  table->SetNumberOfRows(x1.size());
  for (int i = 0; i < x1.size(); ++i)
  {
    table->SetValue(i, 0, static_cast<float>(x1.at(i)));
    table->SetValue(i, 1, static_cast<float>(y1.at(i)));
  }

  table2->SetNumberOfRows(x2.size());
  for (int i = 0; i < x2.size(); ++i)
  {
    table2->SetValue(i, 0, static_cast<float>(x2.at(i)));
    table2->SetValue(i, 1, static_cast<float>(y2.at(i)));
  }

  // Set up the view
  vtkSmartPointer<vtkContextView> view = vtkSmartPointer<vtkContextView>::New();
  view->GetRenderer()->SetBackground(1.0, 1.0, 1.0);
 
  // Add multiple line plots, setting the colors etc
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
 
 
  view->GetRenderWindow()->SetMultiSamples(0);
 
  // Start interactor
  view->GetInteractor()->Initialize();
  view->GetInteractor()->Start();

}