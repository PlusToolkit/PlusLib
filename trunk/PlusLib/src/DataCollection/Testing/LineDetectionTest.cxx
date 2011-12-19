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
#include "itkSobelEdgeDetectionImageFilter.h"
#include "itkCastImageFilter.h"
#include <itkBasicDilateImageFilter.h>
#include "itkCannyEdgeDetectionImageFilter.h"
#include "itkRescaleIntensityImageFilter.h"
#include <itkBinaryThinningImageFilter.h>


#include "itkImage.h"

//----------------------------------------------------------------------------
PlusStatus LineDetection( PlusVideoFrame* videoFrame ); 

int main(int argc, char **argv)
{

	bool printHelp(false);

  int verboseLevel = vtkPlusLogger::LOG_LEVEL_DEFAULT;
  std::string inputSequenceMetafile; 


	vtksys::CommandLineArguments args;
	args.Initialize(argc, argv);

  args.AddArgument("--help", vtksys::CommandLineArguments::NO_ARGUMENT, &printHelp, "Print this help.");	
  args.AddArgument("--input-sequence-metafile", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputSequenceMetafile, "Input sequence metafile name with path");	
	args.AddArgument("--verbose", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &verboseLevel, "Verbose level (1=error only, 2=warning, 3=info, 4=debug, 5=trace)");	

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
		if ( LineDetection( trackedFrameList->GetTrackedFrame(frame)->GetImageData() ) != PLUS_SUCCESS )
		{
			LOG_ERROR("Line detection failed for frame#" << frame ); 
			return EXIT_FAILURE; 
		}
	}


	return EXIT_SUCCESS; 
}



PlusStatus LineDetection( PlusVideoFrame* videoFrame )
{
	static int frameIndex=0;
	frameIndex++;

	//create the hough transform filter

	typedef unsigned char PixelType;
	
	const int Dimension = 2;
  typedef   float  AccumulatorPixelType;
  typedef itk::Image<AccumulatorPixelType, Dimension> AccumulatorImageType;
  typedef itk::HoughTransform2DLinesImageFilter<AccumulatorPixelType, AccumulatorPixelType> HoughTransformFilterType;
	HoughTransformFilterType::Pointer houghTransform = HoughTransformFilterType::New();

	LOG_DEBUG("Performing LineDetection...");	

	//create the hough transform
	typedef itk::Image<PixelType> ImageType;
	ImageType::Pointer localImage = videoFrame->GetImage<PixelType>();

  //cast filters
  typedef itk::Image<bool, 2>  BooleanImageType;
  typedef itk::Image<float, 2>  FloatImageType;
  typedef itk::CastImageFilter<ImageType, BooleanImageType> CharToBooleanImageCastFilterType;
  CharToBooleanImageCastFilterType::Pointer CharToBooleanImageCastFilter = CharToBooleanImageCastFilterType::New();
  
  typedef itk::RescaleIntensityImageFilter<ImageType, BooleanImageType>  CharToBooleanImageRescalerFilterType;
  CharToBooleanImageRescalerFilterType::Pointer CharToBooleanImageRescalerFilter = CharToBooleanImageRescalerFilterType::New(); 

  typedef itk::RescaleIntensityImageFilter<BooleanImageType, ImageType> BooleanToCharImageRescalerFilterType;
  BooleanToCharImageRescalerFilterType::Pointer BooleanToCharImageRescalerFilter = BooleanToCharImageRescalerFilterType::New();

  typedef itk::RescaleIntensityImageFilter<BooleanImageType, FloatImageType> BooleanToFloatImageRescalerFilterType;
  BooleanToFloatImageRescalerFilterType::Pointer BooleanToFloatImageRescalerFilter = BooleanToFloatImageRescalerFilterType::New();

  typedef itk::RescaleIntensityImageFilter<ImageType, FloatImageType>  CharToFloatImageRescalerFilterType;
  CharToFloatImageRescalerFilterType::Pointer CharToFloatImageRescalerFilter = CharToFloatImageRescalerFilterType::New(); 

  //get image threshold based on Otsu's metod
	typedef itk::OtsuThresholdImageFilter <ImageType, ImageType> OtsuThresholdImageFilterType;
	OtsuThresholdImageFilterType::Pointer otsuFilter = OtsuThresholdImageFilterType::New();
	otsuFilter->SetInput(localImage);
	otsuFilter->Update();

  //create a binary threshold filter, with Otsu's threshold
	typedef itk::BinaryThresholdImageFilter <ImageType, ImageType> BinaryThresholdImageFilterType;
	BinaryThresholdImageFilterType::Pointer thresholdFilter = BinaryThresholdImageFilterType::New();
	thresholdFilter->SetInput(localImage);
	thresholdFilter->SetLowerThreshold(0);
	thresholdFilter->SetUpperThreshold(otsuFilter->GetThreshold());
	thresholdFilter->SetInsideValue(0);
	thresholdFilter->SetOutsideValue(255);
  thresholdFilter->Update();

  //edge thining
  typedef itk::BinaryThinningImageFilter<BooleanImageType, BooleanImageType> BinaryThinningImageFilterType;
  BinaryThinningImageFilterType::Pointer thiningFilter = BinaryThinningImageFilterType::New();
  
  CharToBooleanImageRescalerFilter->SetInput(thresholdFilter->GetOutput());
  CharToBooleanImageRescalerFilter->SetOutputMinimum(0);
  CharToBooleanImageRescalerFilter->SetOutputMaximum(1);
  CharToBooleanImageRescalerFilter->Update();

  thiningFilter->SetInput(CharToBooleanImageRescalerFilter->GetOutput());
  thiningFilter->Update();
  
  BooleanToCharImageRescalerFilter->SetInput(thiningFilter->GetOutput());
  BooleanToCharImageRescalerFilter->SetOutputMinimum(0);
  BooleanToCharImageRescalerFilter->SetOutputMaximum(255);
  BooleanToCharImageRescalerFilter->Update();

  BooleanToFloatImageRescalerFilter->SetInput(thiningFilter->GetOutput());
  BooleanToFloatImageRescalerFilter->SetOutputMinimum(0);
  BooleanToFloatImageRescalerFilter->SetOutputMaximum(1);
  BooleanToFloatImageRescalerFilter->Update();

  CharToFloatImageRescalerFilter ->SetInput(thresholdFilter->GetOutput());
  CharToFloatImageRescalerFilter->SetOutputMinimum(0);
  CharToFloatImageRescalerFilter->SetOutputMaximum(1);
  CharToFloatImageRescalerFilter->Update();

  

  typedef itk::Image<double, 2>  DoubleImageType;
 
  typedef itk::CannyEdgeDetectionImageFilter <DoubleImageType, DoubleImageType>
  CannyEdgeDetectionImageFilterType;
 
  typedef itk::CastImageFilter<ImageType, DoubleImageType> cstFilterType;
  cstFilterType::Pointer cstFilter = cstFilterType::New();
  cstFilter->SetInput(localImage);
  cstFilter->Update();

  typedef itk::RescaleIntensityImageFilter<DoubleImageType, ImageType>  RescalerType;
  RescalerType::Pointer rescaler = RescalerType::New(); 

  CannyEdgeDetectionImageFilterType::Pointer cannyFilter = CannyEdgeDetectionImageFilterType::New();
  cannyFilter->SetInput(cstFilter->GetOutput());
  cannyFilter->Update();
  
  //perform hough transform of thresholded image
	double lineDetectionIntensityThreshold = 0;
  houghTransform->SetInput(CharToFloatImageRescalerFilter->GetOutput());
	houghTransform->SetThreshold(lineDetectionIntensityThreshold);
	houghTransform->SetNumberOfLines(6);
  //houghTransform->SetAngleResolution(10000);
  houghTransform->SetDiscRadius(1);
	houghTransform->Update();
  cout << houghTransform->GetAngleResolution() << "\n";
  cout << houghTransform->GetDiscRadius()<< "\n";
  cout << houghTransform->GetVariance()<< "\n";

	
	typedef float FloatPixelType;
	typedef itk::Image<FloatPixelType> FloatImageType;


	// Convert image to float
	typedef itk::CastImageFilter<ImageType, FloatImageType> CharToFloatImageCastFilterType;
	CharToFloatImageCastFilterType::Pointer charToFloatCastFilter = CharToFloatImageCastFilterType::New();
	charToFloatCastFilter->SetInput(thresholdFilter->GetOutput());

	// Sobel edge detection
	typedef itk::SobelEdgeDetectionImageFilter <FloatImageType,FloatImageType> SobelEdgeDetectionImageFilterType;
	SobelEdgeDetectionImageFilterType::Pointer sobelFilter = SobelEdgeDetectionImageFilterType::New();
	sobelFilter->SetInput(charToFloatCastFilter->GetOutput());

	// Write Sobel output to file	
	typedef itk::CastImageFilter<FloatImageType,ImageType> FloatToCharImageCastFilterType;
	FloatToCharImageCastFilterType::Pointer floatToCharCastFilter = FloatToCharImageCastFilterType::New();
	floatToCharCastFilter->SetInput(sobelFilter->GetOutput());

  rescaler->SetInput(cannyFilter->GetOutput());
  rescaler->SetOutputMinimum(0);
  rescaler->SetOutputMaximum(255);
  rescaler->Update();



	typedef itk::CastImageFilter<DoubleImageType,ImageType> DoubleToCharImageCastFilterType;
	DoubleToCharImageCastFilterType::Pointer doubleToCharCastFilter = DoubleToCharImageCastFilterType::New();
  doubleToCharCastFilter->SetInput(cannyFilter->GetOutput());

	std::ostrstream sobelOutputImageFilename; 
	sobelOutputImageFilename << "SobelResult_" << std::setw(3) << std::setfill('0') << frameIndex << ".jpg" << std::ends;
	//PlusVideoFrame::SaveImageToFile(floatToCharCastFilter->GetOutput(), sobelOutputImageFilename.str());
  PlusVideoFrame::SaveImageToFile(BooleanToCharImageRescalerFilter->GetOutput(), sobelOutputImageFilename.str());
  
	//get the lines of the image into the line-list holder
	itk::HoughTransform2DLinesImageFilter<PixelType, PixelType>::LinesListType lines = houghTransform->GetLines();

	LOG_DEBUG("Found " << lines.size() << " line(s).");


	typedef  unsigned char OutputPixelType;
	typedef  itk::Image< OutputPixelType, Dimension > OutputImageType;  

	typedef itk::ImageDuplicator< ImageType > DuplicatorType;
	DuplicatorType::Pointer duplicator = DuplicatorType::New();
	duplicator->SetInputImage(localImage);
	duplicator->Update();
	ImageType::Pointer localOutputImage = duplicator->GetOutput();


  /*
  typedef  unsigned char    OutputPixelType;
  typedef  itk::Image< OutputPixelType, Dimension > OutputImageType;  

  OutputImageType::Pointer  localOutputImage = OutputImageType::New();

  OutputImageType::RegionType region;
  region.SetSize(localImage->GetLargestPossibleRegion().GetSize());
  region.SetIndex(localImage->GetLargestPossibleRegion().GetIndex());
  localOutputImage->SetRegions( region );
  localOutputImage->SetOrigin(localImage->GetOrigin());
  localOutputImage->SetSpacing(localImage->GetSpacing());
  localOutputImage->Allocate();
  localOutputImage->FillBuffer(0);
*/

	//  We iterate through the list of lines and we draw them.
	typedef HoughTransformFilterType::LinesListType::const_iterator LineIterator;
	typedef HoughTransformFilterType::LineType::PointListType PointListType;
  LineIterator itLines = lines.begin();

	while( itLines != lines.end() )
	{  
		//  We get the list of points which consists of two points to represent a
		//  straight line.  Then, from these two points, we compute a fixed point
		//  $u$ and a unit vector $\vec{v}$ to parameterize the line.
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

		//  We draw a white pixels in the output image to represent the line.
		ImageType::IndexType localIndex;
		itk::Size<2> size = localOutputImage->GetLargestPossibleRegion().GetSize();
		float diag = vcl_sqrt((float)( size[0]*size[0] + size[1]*size[1] ));

		for(int i=static_cast<int>(-diag); i<static_cast<int>(diag); i++)
		{
			localIndex[0]=(long int)(u[0]+i*v[0]);
			localIndex[1]=(long int)(u[1]+i*v[1]);

			OutputImageType::RegionType outputRegion =
				localOutputImage->GetLargestPossibleRegion();

			if( outputRegion.IsInside( localIndex ) )
			{
				localOutputImage->SetPixel( localIndex, 255 );
			}
		}
		itLines++;
	}
	//  We setup a writer to write out the binary image created.
	std::ostrstream outputImageFilename; 
	outputImageFilename << "HoughTransformResult_" << std::setw(3) << std::setfill('0') << frameIndex << ".jpg" << std::ends;

	LOG_DEBUG("Write output to file: "<<outputImageFilename.str());
	PlusVideoFrame::SaveImageToFile(localOutputImage, outputImageFilename.str());

	
	typedef itk::CastImageFilter<FloatImageType,ImageType> CastFilterType;
	CastFilterType::Pointer castFilter = CastFilterType::New();
	castFilter->SetInput(sobelFilter->GetOutput());
	


	return PLUS_SUCCESS; 
}
