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

//----------------------------------------------------------------------------

PlusStatus LineDetection(PlusVideoFrame* videoFrame);

int main(int argc, char **argv)
{
  bool printHelp(false);
  int verboseLevel = vtkPlusLogger::LOG_LEVEL_DEFAULT;
  std::string inputSequenceMetafile;

  vtksys::CommandLineArguments args;
  args.Initialize(argc, argv);

  args.AddArgument("--help",vtksys::CommandLineArguments::NO_ARGUMENT, &printHelp, "Print this help.");
  args.AddArgument("--input-sequence-metafile", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputSequenceMetafile, "Input sequence metafile name with path");
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

  if ( inputSequenceMetafile.empty() )
  {
    std::cerr << "input-sequence-metafile required argument!" << std::endl;
    std::cout << "Help: " << args.GetHelp() << std::endl;
    exit(EXIT_FAILURE);
  }

  vtkPlusLogger::Instance()->SetLogLevel(verboseLevel);

  vtkSmartPointer<vtkTrackedFrameList> trackedFrameList = vtkSmartPointer<vtkTrackedFrameList>::New();
  if ( trackedFrameList->ReadFromSequenceMetafile(inputSequenceMetafile.c_str()) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to read sequence metafile: " << inputSequenceMetafile);
    return EXIT_FAILURE;
  }

  for ( int frame = 0; frame < trackedFrameList->GetNumberOfTrackedFrames(); ++frame )
  {
    if ( LineDetection(trackedFrameList->GetTrackedFrame(frame)->GetImageData()) != PLUS_SUCCESS )
    {
      LOG_ERROR("Line detection failed for frame#" << frame);
      return EXIT_FAILURE;
    }
  }

  return EXIT_SUCCESS;
}

PlusStatus LineDetection(PlusVideoFrame* videoFrame)
{
  LOG_DEBUG("Performing LineDetection...");

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
  int houghNumberOfLinesToDetect = 6;
  int houghDiscRadius = 1;
  houghTransform->SetInput(thresholdFilter->GetOutput());
  houghTransform->SetThreshold(houghLineDetectionThreshold);
  houghTransform->SetNumberOfLines(houghNumberOfLinesToDetect);
  houghTransform->SetDiscRadius(houghDiscRadius);
  houghTransform->Update();

  LOG_DEBUG("Angle resolution: pi/" << 2 * houghTransform->GetAngleResolution());
  LOG_DEBUG("Disc radius: " << houghTransform->GetDiscRadius());
  LOG_DEBUG("Variance of Gaussian filter: " << houghTransform->GetVariance());

  // Write edge image to file
  std::ostrstream edgeOutputImageFilename;
  edgeOutputImageFilename << "EdgeImage_" << std::setw(3) << std::setfill('0') << frameIndex << ".jpg" << std::ends;
  PlusVideoFrame::SaveImageToFile(thresholdFilter->GetOutput(), edgeOutputImageFilename.str());

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

    double norm = vcl_sqrt(v[0]*v[0]+v[1]*v[1]);
    v[0] /= norm;
    v[1] /= norm;

    // Draw a white pixels in the output image to represent the line.
    charImageType::IndexType localIndex;
    itk::Size<2> size = localOutputImage->GetLargestPossibleRegion().GetSize();
    float diag = vcl_sqrt((float)( size[0]*size[0] + size[1]*size[1] ));

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

  // Write the image with the detected line(s) to the output file.
  std::ostrstream outputImageFilename;
  outputImageFilename << "HoughTransformResult_" << std::setw(3) << std::setfill('0') << frameIndex << ".jpg" << std::ends;
  LOG_DEBUG("Writing detected lines to output to file: " << outputImageFilename.str());
  PlusVideoFrame::SaveImageToFile(localOutputImage, outputImageFilename.str());

  return PLUS_SUCCESS;
}
