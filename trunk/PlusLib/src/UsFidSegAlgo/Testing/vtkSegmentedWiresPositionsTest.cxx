#include "PlusConfigure.h"

#include "vtkSmartPointer.h"
#include "vtkCommand.h"
#include "vtksys/CommandLineArguments.hxx" 
#include "vtkMatrix4x4.h"
#include "vtkTransform.h"
#include "vtkMath.h"

#include "UltraSoundFiducialSegmentation.h"

#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkMetaImageSequenceIO.h"

#include <stdlib.h>
#include <iostream>

typedef unsigned char PixelType;
typedef itk::Image< PixelType, 2 > ImageType;
typedef itk::Image< PixelType, 3 > ImageSequenceType;
typedef itk::ImageFileReader< ImageSequenceType > ImageSequenceReaderType;

void PrintProgressBar( int percent ); 

///////////////////////////////////////////////////////////////////

int main (int argc, char* argv[])
{ 
	std::string inputTestDataPath;
	std::string outputWirePositionFile("./SegmentedWirePositions.txt");
	int inputImageType(1); 

	std::string inputBaselineFileName;
	double inputTranslationErrorThreshold(0); 
	double inputRotationErrorThreshold(0); 

	int verboseLevel=PlusLogger::LOG_LEVEL_INFO;

	vtksys::CommandLineArguments cmdargs;
	cmdargs.Initialize(argc, argv);

	cmdargs.AddArgument("--input-test-data-path", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputTestDataPath, "Test data path");
	cmdargs.AddArgument("--input-image-type", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputImageType, "Image type (1=SonixVideo, 2=FrameGrabber - Default: SonixVideo");	
	cmdargs.AddArgument("--output-wire-position-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &outputWirePositionFile, "Result wire position file name (Default: ./SegmentedWirePositions.txt)");

	cmdargs.AddArgument("--input-baseline-file-name", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputBaselineFileName, "Name of file storing baseline calibration results");
	cmdargs.AddArgument("--translation-error-threshold", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputTranslationErrorThreshold, "Translation error threshold in mm.");	
	cmdargs.AddArgument("--rotation-error-threshold", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputRotationErrorThreshold, "Rotation error threshold in degrees.");	
	cmdargs.AddArgument("--verbose", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &verboseLevel, "Verbose level (1=error only, 2=warning, 3=info, 4=debug)");	

	if ( !cmdargs.Parse() )
	{
		std::cerr << "Problem parsing arguments" << std::endl;
		std::cout << "Help: " << cmdargs.GetHelp() << std::endl;
		exit(EXIT_FAILURE);
	}

	if ( inputTestDataPath.empty() ) 
	{
		std::cerr << "input-test-data-path argument required" << std::endl << std::endl;
		std::cout << "Help: " << cmdargs.GetHelp() << std::endl;
		exit(EXIT_FAILURE);

	}

	PlusLogger::Instance()->SetLogLevel(verboseLevel);

	VTK_LOG_TO_CONSOLE_ON; 

	SegmentationParameters SegParameters; 
	int SearchRegionXMin(0), SearchRegionXSize(0), SearchRegionYMin(0), SearchRegionYSize(0); 

	switch ( inputImageType )
	{
	case 1: // SonixVideo frames
		SearchRegionXMin = 30; 
		SearchRegionXSize = 565; 
		SearchRegionYMin = 50; 
		SearchRegionYSize = 370;  
		SegParameters.SetScalingEstimation(0.19);


		break; 
	case 2: // FrameGrabber frames
		SearchRegionXMin = 100; 
		SearchRegionXSize = 360; 
		SearchRegionYMin = 65; 
		SearchRegionYSize = 280;  
		SegParameters.SetScalingEstimation(0.26);  
		break; 
	}

	SegParameters.SetMorphologicalOpeningBarSizeMm(2.0);  
	SegParameters.SetMorphologicalOpeningCircleRadiusMm(0.55);  
	SegParameters.SetThresholdImageTop(10); 
	SegParameters.SetThresholdImageBottom(10); 
	SegParameters.SetMaxLineLenMm(42); 
	SegParameters.SetMinLineLenMm(38); 
	SegParameters.SetMaxLinePairDistMm(22); 
	SegParameters.SetMinLinePairDistMm(18); 
	SegParameters.SetFindLines3PtDist(5.3); 
	SegParameters.SetMaxLineErrorMm(2.0); 
	SegParameters.SetMaxAngleDiff(0.191986); 
	SegParameters.SetMinTheta(0.349065); 
	SegParameters.SetMaxTheta(2.792526); 
	SegParameters.SetMaxUangleDiff(0.174532); 
	SegParameters.SetMaxUsideLineDiff(30); 
	SegParameters.SetMinUsideLineLength(320); 
	SegParameters.SetMaxUsideLineLength(350); 
	SegParameters.SetUseOriginalImageIntensityForDotIntensityScore(0); 
	SegParameters.SetFiducialGeometry(SegmentationParameters::CALIBRATION_PHANTOM_6_POINT); 

	SegParameters.UpdateParameters(); 

	LOG_INFO( "Reading sequence meta file...");  
	itk::MetaImageSequenceIO::Pointer readerMetaImageSequenceIO = itk::MetaImageSequenceIO::New(); 
	ImageSequenceReaderType::Pointer reader = ImageSequenceReaderType::New(); 

	// Set the image IO 
	reader->SetImageIO(readerMetaImageSequenceIO); 
	reader->SetFileName(inputTestDataPath.c_str());

	try
	{
		reader->Update(); 
	}
	catch (itk::ExceptionObject & err) 
	{		
		LOG_ERROR(" Sequence image reader couldn't update: " <<  err); 
		exit(EXIT_FAILURE);
	}	

	ImageSequenceType::Pointer imageSeq = reader->GetOutput();

	const unsigned long ImageWidthInPixels = imageSeq->GetLargestPossibleRegion().GetSize()[0]; 
	const unsigned long ImageHeightInPixels = imageSeq->GetLargestPossibleRegion().GetSize()[1]; 
	const unsigned long numberOfFrames = imageSeq->GetLargestPossibleRegion().GetSize()[2];	

	unsigned int frameSizeInBytes=ImageWidthInPixels*ImageHeightInPixels*sizeof(PixelType);

	PixelType* imageSeqData = imageSeq->GetBufferPointer(); // pointer to the image pixel buffer

	std::ofstream positionInfo;
	positionInfo.open (outputWirePositionFile.c_str(), ios::out );

	LOG_INFO( "Segmenting frames..."); 

	for ( int imgNumber = 0; imgNumber < numberOfFrames; imgNumber++ )
	{
		PrintProgressBar( (100.0 * imgNumber) / numberOfFrames ); 

		PixelType *currentFrameImageData= imageSeqData + imgNumber * frameSizeInBytes;

		ImageType::Pointer frame = ImageType::New();
		ImageType::SizeType size = {ImageWidthInPixels, ImageHeightInPixels };
		ImageType::IndexType start = {0,0};
		ImageType::RegionType region;
		region.SetSize(size);
		region.SetIndex(start);
		frame->SetRegions(region);
        try 
        {
            frame->Allocate();
        }
        catch (itk::ExceptionObject & err)
        {
            LOG_ERROR("Failed to allocate memory: " << err ); 
            continue; 
        }

		memcpy(frame->GetBufferPointer() , currentFrameImageData, frameSizeInBytes);

		vtkSmartPointer<vtkMatrix4x4> transformMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
		if ( !readerMetaImageSequenceIO->GetFrameTransform(imgNumber, transformMatrix) )
		{
			LOG_ERROR("Unable to get default frame transform for frame #" << imgNumber); 
			continue; 
		}

		KPhantomSeg segmenter(ImageWidthInPixels, ImageHeightInPixels, SearchRegionXMin, SearchRegionYMin, SearchRegionXSize, SearchRegionYSize); 
		segmenter.SetSegParams(SegParameters); 

		try
		{
			// Send the image to the Segmentation component to segment
			segmenter.segment( frame->GetBufferPointer(), SegParameters);	}
		catch(...)
		{
			LOG_ERROR("SegmentImage: The segmentation has failed for due to UNKNOWN exception thrown, the image was ignored!!!"); 
			continue; 
		}

		SegmentationResults segResults;
		segmenter.GetSegmentationResults(segResults); 

		if ( segResults.GetDotsFound() )
		{
			vtkSmartPointer<vtkTransform> frameTransform = vtkSmartPointer<vtkTransform>::New(); 
			frameTransform->SetMatrix(transformMatrix); 

			double posZ = frameTransform->GetPosition()[2]; 
			double rotZ = frameTransform->GetOrientation()[2]; 

			int dataType = -1; 
			positionInfo << dataType << "\t\t" << posZ << "\t" << rotZ << "\t\t"; 

			for (int i=0; i<segResults.GetFoundDotsCoordinateValue().size(); i++)
			{
				positionInfo << segResults.GetFoundDotsCoordinateValue()[i][0] << "\t" << segResults.GetFoundDotsCoordinateValue()[i][1] << "\t\t"; 
			}

			positionInfo << std::endl; 

		}
	}
	
	positionInfo.close(); 
	PrintProgressBar(100); 
	std::cout << std::endl; 

	std::cout << "Exit success!!!" << std::endl; 
	return EXIT_SUCCESS; 
}

void PrintProgressBar( int percent )
{
	std::string bar;

	for(int i = 0; i < 50; i++)
	{
		if( i < (percent/2))
		{
			bar.replace(i,1,"=");
		}
		else if( i == (percent/2))
		{
			bar.replace(i,1,">");
		}
		else
		{
			bar.replace(i,1," ");
		}
	}

	std::cout<< "\r" "[" << bar << "] ";
	std::cout.width( 3 );
	std::cout<< percent << "%     " << std::flush;
}