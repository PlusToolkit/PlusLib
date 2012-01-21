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

#include "itkContinuousIndex.h"
#include "itkLinearInterpolateImageFunction.h"

//----------------------------------------------------------------------------

PlusStatus LineDetection(PlusVideoFrame* videoFrame, std::vector<double> &linePositionMetric);
double linearInterpolation(double interpVal, std::vector<double> &origVec, std::vector<double> &originalTimestamp);
void test(std::vector<double> &origVec, std::vector<double> &interpolatedVector,
          std::vector<double> &intepolatedTimestamp, std::vector<double> &originalTimestamp);
void NormalizeMetric(std::vector<double> &translationModulusVector);
void CalculateTimeOffset(std::vector<double> &translationModulusVector,
                         std::vector<double> &trackerTimestampVector,
                         std::vector<double> &imageModulusVector,
                         std::vector<double> &imageTimestampVector,
                         std::vector<double> &interpolatedTrackerModulusVector,
                         std::vector<double> &interpolatedTrackerTimestampVector,
                         std::vector<double> &interpolatedImageModulusVector,
                         std::vector<double> &interpolatedImageTimestampVector);
double computeCorrelation(std::vector<double> &trackerMetric, std::vector<double> &imageMetric, int indexOffset);
void xcorr(std::vector<double> &trackerMetric, std::vector<double> &imageMetric, std::vector<double> &corrValues);

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

  // Get the number identifier of the trial (for testing only)
  std::string numbers;
  for(int i = 0; i < inputUSImageSequenceMetafile.length(); i++)
  {
    if(isdigit(inputUSImageSequenceMetafile.at(i))){
      numbers.append(inputUSImageSequenceMetafile.substr(i,1));
    }
  }

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
  for ( int frame = 0; frame < imageList->GetNumberOfTrackedFrames(); ++frame )
  {
    imageTimestampVector.push_back(imageList->GetTrackedFrame(frame)->GetTimestamp());
    if ( LineDetection(imageList->GetTrackedFrame(frame)->GetImageData(), linePositionMetric) != PLUS_SUCCESS )
    {
      LOG_ERROR("Line detection failed for frame#" << frame);
      return EXIT_FAILURE;
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

  CalculateTimeOffset(translationModulusVector,
                      trackerTimestampVector,
                      linePositionMetric,
                      imageTimestampVector,
                      interpolatedTranslationModulusVector,
                      interpolatedTrackerTimestampVector,
                      interpolatedImageModulusVector,
                      interpolatedImageTimestampVector);

  //  Perform cross correlation
  std::vector<double> corrValues;
  xcorr(interpolatedTranslationModulusVector, interpolatedImageModulusVector, corrValues);
  
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
  std::cout << trackerLag << std::endl;
  
  // Write the intepolated tracker metric to file
  std::string interpolatedTrackerMetricFileName = "normalizedInterpolatedTrackerMetric_";
  interpolatedTrackerMetricFileName.append(numbers);
  interpolatedTrackerMetricFileName.append(".txt");

  std::ofstream interpolatedProbeToReferenceTransformNormalizedFile;
  interpolatedProbeToReferenceTransformNormalizedFile.open (interpolatedTrackerMetricFileName.c_str());
  for(int i = 0; i < interpolatedTranslationModulusVector.size(); ++i)
    interpolatedProbeToReferenceTransformNormalizedFile << interpolatedTranslationModulusVector.at(i) << "," << interpolatedTrackerTimestampVector.at(i) << std::endl;

  interpolatedProbeToReferenceTransformNormalizedFile.close();

  //  stop timer
  time_t endTime;
  time(&endTime);

  std::string runTimeFileName = "RunTime_";
  runTimeFileName.append(numbers);
  runTimeFileName.append(".txt");

  std::ofstream runTimeFile;
  runTimeFile.open(runTimeFileName.c_str());
  runTimeFile << difftime(endTime, startTime) << std::endl;

  return EXIT_SUCCESS;
}

PlusStatus LineDetection(PlusVideoFrame* videoFrame, std::vector<double> &linePositionMetric)
{
  LOG_DEBUG("Performing LineDetection..." << linePositionMetric.size());

  // Update frame counter
  static int frameIndex(-1); //because frame indices start at zero
  frameIndex++;

  // Define some image types and image dimension
  const int imageDimension(2);
  typedef unsigned char charPixelType; //the natural type of the input image
  typedef float floatPixelType; //the type of pixel used for the Hough accumulator
  typedef itk::Image<charPixelType,imageDimension> charImageType;
  
  // Get curent image
  charImageType::Pointer localImage = videoFrame->GetImage<charPixelType>();

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
  int houghDiscRadius = 1;
  houghTransform->SetInput(thresholdFilter->GetOutput());
  houghTransform->SetThreshold(houghLineDetectionThreshold);
  houghTransform->SetNumberOfLines(houghNumberOfLinesToDetect);
  houghTransform->SetDiscRadius(houghDiscRadius);
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
    //InterceptFile << b << std::endl;
    //InterceptFile.flush();
  
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

void CalculateTimeOffset(std::vector<double> &translationModulusVector,
                         std::vector<double> &trackerTimestampVector,
                         std::vector<double> &imageModulusVector,
                         std::vector<double> &imageTimestampVector,
                         std::vector<double> &interpolatedTrackerModulusVector,
                         std::vector<double> &interpolatedTrackerTimestampVector,
                         std::vector<double> &interpolatedImageModulusVector,
                         std::vector<double> &interpolatedImageTimestampVector)
{
  double videoOffset = 0;//  seconds
  double maxVideoOffset = 2;//  seconds
  double videoOffsetResolution = 0.001;//  seconds

  double translationTimestampMin = trackerTimestampVector.at(1);
  double translationTimestampMax = trackerTimestampVector.at(1);
  for(int i = 0; i < translationModulusVector.size(); ++i)
  {
      if(trackerTimestampVector.at(i) > translationTimestampMax)
      {
        translationTimestampMax = trackerTimestampVector.at(i);
      }
      else if(trackerTimestampVector.at(i) < translationTimestampMin)
      {
        translationTimestampMin = trackerTimestampVector.at(i);
      }
  }// end for-loop

  double imageTimestampMin = imageTimestampVector.at(1);
  double imageTimestampMax = imageTimestampVector.at(1);
  for(int i = 0; i < imageModulusVector.size(); ++i)
  {
      if(imageTimestampVector.at(i) > imageTimestampMax)
      {
        imageTimestampMax = imageTimestampVector.at(i);
      }
      else if(imageTimestampVector.at(i) < imageTimestampMin)
      {
        imageTimestampMin = imageTimestampVector.at(i);
      }
  }// end for-loop

  double commonRangeMin = std::max(imageTimestampMin, translationTimestampMin ); 
  double commonRangeMax = std::min(imageTimestampMax, translationTimestampMax);

  //  Get resampled timestamps
  long int n = 0;
  while(commonRangeMin + n * videoOffsetResolution < commonRangeMax)
  {
    interpolatedImageTimestampVector.push_back(commonRangeMin + n * videoOffsetResolution);
    ++n;
  }

  n = 0;
  while((commonRangeMin + maxVideoOffset) + n * videoOffsetResolution < commonRangeMax - maxVideoOffset)
  {
    interpolatedTrackerTimestampVector.push_back((commonRangeMin + maxVideoOffset) + n * videoOffsetResolution);
    ++n;
  }

  test(imageModulusVector,interpolatedImageModulusVector, interpolatedImageTimestampVector, imageTimestampVector);
  test(translationModulusVector, interpolatedTrackerModulusVector,interpolatedTrackerTimestampVector, trackerTimestampVector);

}// End CalculateTimeOffset()


 
void test(std::vector<double> &origVec, std::vector<double> &interpolatedVector,
          std::vector<double> &interpolatedTimestamp, std::vector<double> &originalTimestamp)
{
  for(int i = 0; i < interpolatedTimestamp.size(); ++i)
  {
    interpolatedVector.push_back(linearInterpolation(interpolatedTimestamp.at(i), origVec, originalTimestamp));
  }
}

double linearInterpolation(double interpVal, std::vector<double> &origVec, std::vector<double> &originalTimestamp)
{
  double timeEpsilon = 0.0000001; //  resolution under which we consider two time values to be identical
  double minDiff = std::abs(originalTimestamp.at(0) - interpVal);
  int minIndex = 0;
  bool isEqual = false;
  int i = 1;
  while(i < originalTimestamp.size() && !isEqual)
  {
    if(std::abs(originalTimestamp.at(i) - interpVal) < timeEpsilon)
    {
      minDiff = 0;
      minIndex = i;
      isEqual = true;
    }
    else if(std::abs(originalTimestamp.at(i) - interpVal) < minDiff)
    {
      minDiff = std::abs(originalTimestamp.at(i) - interpVal);
      minIndex = i;
    }

    ++i;

  }// end for-loop

  if(isEqual)
  {
    return origVec.at(minIndex);
  }
  else if(interpVal < std::abs(originalTimestamp.at(minIndex)))
  {
    if(std::abs( originalTimestamp.at(minIndex) - originalTimestamp.at(minIndex - 1)) < timeEpsilon)
    {
      return origVec.at(minIndex);
    }
    else
    {
      double m = (origVec.at(minIndex) - origVec.at(minIndex - 1)) / (originalTimestamp.at(minIndex) - originalTimestamp.at(minIndex - 1));
      return origVec.at(minIndex - 1) + (interpVal - originalTimestamp.at(minIndex - 1))*m;
    }
  }
  else
  {
    if(std::abs( originalTimestamp.at(minIndex) - originalTimestamp.at(minIndex + 1)) < timeEpsilon )
    {
      return origVec.at(minIndex);
    }
    else
    {
      double m = (origVec.at(minIndex + 1) - origVec.at(minIndex)) / (originalTimestamp.at(minIndex + 1) - originalTimestamp.at(minIndex));
      return origVec.at(minIndex) + (interpVal - originalTimestamp.at(minIndex))*m;
    }
  }

}
 

void xcorr(std::vector<double> &trackerMetric, std::vector<double> &imageMetric, std::vector<double> &corrValues)
{

  int n = 0;
  while(n + (trackerMetric.size() - 1) < imageMetric.size())
  {
    corrValues.push_back(computeCorrelation(trackerMetric, imageMetric, n));
    ++n;
  }

}// end xcorr

double computeCorrelation(std::vector<double> &trackerMetric, std::vector<double> &imageMetric, int indexOffset){

  double overlapSum = 0;
  for(long int i = 0; i < trackerMetric.size(); ++i)
    overlapSum += trackerMetric.at(i) * imageMetric.at(i+indexOffset);

  return overlapSum;
}// end computeCorrelation