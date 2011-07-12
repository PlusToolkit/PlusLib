#include "PlusConfigure.h"

#include "vtkSmartPointer.h"
#include "vtkCommand.h"
#include "vtksys/CommandLineArguments.hxx" 
#include "vtkMatrix4x4.h"
#include "vtkTransform.h"
#include "vtkMath.h"

#include "UltraSoundFiducialSegmentation.h"
#include "vtkTemplateToStepperCalibrator.h"

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
	std::vector<std::string> inputImageSequenceFileNames;
	std::string outputCenterOfRotationFile("./CenterOfRotations.txt");
	int inputImageType(1); 

	std::string inputBaselineFileName;
	double inputTranslationErrorThreshold(0); 
	double inputRotationErrorThreshold(0); 

	int verboseLevel=PlusLogger::LOG_LEVEL_INFO;

	vtksys::CommandLineArguments cmdargs;
	cmdargs.Initialize(argc, argv);

	cmdargs.AddArgument("--input-img-seq-file-names", vtksys::CommandLineArguments::MULTI_ARGUMENT, &inputImageSequenceFileNames, "Filenames of meta image sequences (e.g. sequence_1.mhd sequence_2.mhd).");
	cmdargs.AddArgument("--input-image-type", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputImageType, "Image type (1=SonixVideo, 2=FrameGrabber - Default: SonixVideo");	
	cmdargs.AddArgument("--output-centerofrotation-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &outputCenterOfRotationFile, "Result center of rotation file name (Default: ./CenterOfRotations.txt)");

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

	if ( inputImageSequenceFileNames.empty() ) 
	{
		std::cerr << "input-img-seq-file-names argument required" << std::endl << std::endl;
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

	std::ofstream centerofrotationInfo;
	centerofrotationInfo.open (outputCenterOfRotationFile.c_str(), ios::out );

	for ( int i = 0; i < inputImageSequenceFileNames.size(); i++) 
	{

		LOG_INFO( "Reading sequence meta file...");  
		itk::MetaImageSequenceIO::Pointer readerMetaImageSequenceIO = itk::MetaImageSequenceIO::New(); 
		ImageSequenceReaderType::Pointer reader = ImageSequenceReaderType::New(); 

		// Set the image IO 
		reader->SetImageIO(readerMetaImageSequenceIO); 
		reader->SetFileName(inputImageSequenceFileNames[i].c_str());

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

		unsigned long ImageWidthInPixels = imageSeq->GetLargestPossibleRegion().GetSize()[0]; 
		unsigned long ImageHeightInPixels = imageSeq->GetLargestPossibleRegion().GetSize()[1]; 
		unsigned long numberOfFrames = imageSeq->GetLargestPossibleRegion().GetSize()[2];	

		unsigned int frameSizeInBytes=ImageWidthInPixels*ImageHeightInPixels*sizeof(PixelType);

		PixelType* imageSeqData = imageSeq->GetBufferPointer(); // pointer to the image pixel buffer

		LOG_INFO( "Segmenting frames..."); 

		vtkSmartPointer<vtkTemplateToStepperCalibrator> templateToStepperCalibrator = vtkSmartPointer<vtkTemplateToStepperCalibrator>::New(); 
		int numberOfSegmentedCenterOfRotationImages(0); 
		std::vector<double> probePosition; 

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
				probePosition.push_back(posZ); 

				// Top layer:		3, 2, 1 (from right to left);
				// Bottom Layer:	6, 5, 4 (from right to left);
				std::vector<HomogenousVector4x1> vectorOfWirePoints; 
				// Add Line #1 pixel coordinates to center of rotation point set 
				vectorOfWirePoints.push_back( HomogenousVector4x1( segResults.GetFoundDotsCoordinateValue()[0][0], segResults.GetFoundDotsCoordinateValue()[0][1], 0 ) ); 

				// Add Line #3 pixel coordinates to center of rotation point set 
				vectorOfWirePoints.push_back( HomogenousVector4x1( segResults.GetFoundDotsCoordinateValue()[2][0], segResults.GetFoundDotsCoordinateValue()[2][1], 0 ) ); 

				// Add Line #4 pixel coordinates to center of rotation point set 
				vectorOfWirePoints.push_back( HomogenousVector4x1( segResults.GetFoundDotsCoordinateValue()[3][0], segResults.GetFoundDotsCoordinateValue()[3][1], 0 ) ); 

				// Add Line #6 pixel coordinates to center of rotation point set 
				vectorOfWirePoints.push_back( HomogenousVector4x1( segResults.GetFoundDotsCoordinateValue()[5][0], segResults.GetFoundDotsCoordinateValue()[5][1], 0 ) ); 

				templateToStepperCalibrator->AddPointForCenterOfRotationCalculation(vectorOfWirePoints); 

				// Add Line #1 (point A) Line #3 (point B) and Line #6 (point C) pixel coordinates to phantom to probe distance point set 
				templateToStepperCalibrator->AddPointsForPhantomToProbeDistanceCalculation(
					segResults.GetFoundDotsCoordinateValue()[0][0], segResults.GetFoundDotsCoordinateValue()[0][1], 0, 
					segResults.GetFoundDotsCoordinateValue()[2][0], segResults.GetFoundDotsCoordinateValue()[2][1], 0, 
					segResults.GetFoundDotsCoordinateValue()[5][0], segResults.GetFoundDotsCoordinateValue()[5][1], 0 
					); 

				// Add distance between line #1 and #3 for scaling computation 
				// Constant Distance Measurements from iCAL phantom design in mm
				const double distanceN1ToN3inMm(40); // TODO: read it from pahantom design
				double xDistanceN1ToN3 = segResults.GetFoundDotsCoordinateValue()[0][0] - segResults.GetFoundDotsCoordinateValue()[2][0]; 
				double yDistanceN1ToN3 = segResults.GetFoundDotsCoordinateValue()[0][1] - segResults.GetFoundDotsCoordinateValue()[2][1]; 
				templateToStepperCalibrator->AddDistancesForSpacingCalculation(xDistanceN1ToN3, yDistanceN1ToN3, distanceN1ToN3inMm); 

				// Add distance between line #3 and #6 for scaling computation 
				// Constant Distance Measurements from iCAL phantom design in mm
				const double distanceN3ToN6inMm(20); // TODO: read it from pahantom design
				double xDistanceN3ToN6 = segResults.GetFoundDotsCoordinateValue()[2][0] - segResults.GetFoundDotsCoordinateValue()[5][0]; 
				double yDistanceN3ToN6 = segResults.GetFoundDotsCoordinateValue()[2][1] - segResults.GetFoundDotsCoordinateValue()[5][1]; 
				templateToStepperCalibrator->AddDistancesForSpacingCalculation(xDistanceN3ToN6, yDistanceN3ToN6, distanceN3ToN6inMm); 

				numberOfSegmentedCenterOfRotationImages++;

			}
		}

		PrintProgressBar(100); 
		std::cout << std::endl; 

		templateToStepperCalibrator->SetMinNumberOfPointsForCalibration(numberOfSegmentedCenterOfRotationImages - 1); 
		templateToStepperCalibrator->CalculateSpacing();
		templateToStepperCalibrator->CalculateCenterOfRotation(); 
		templateToStepperCalibrator->CalculatePhantomToProbeDistance(); 
		templateToStepperCalibrator->CalculateTemplateToStepperTransformation();

		double avgProbePos = 0; 
		for ( int i = 0; i < probePosition.size(); i++ )
		{
			avgProbePos += probePosition[i] / (1.0 * probePosition.size()); 
		}

		centerofrotationInfo << avgProbePos << "\t" 
			<< templateToStepperCalibrator->GetCenterOfRotation()[0] << "\t" << templateToStepperCalibrator->GetCenterOfRotation()[1] << "\t" 
			<< templateToStepperCalibrator->GetSpacing()[0] << "\t" << templateToStepperCalibrator->GetSpacing()[1] << std::endl; 
	}

	centerofrotationInfo.close(); 


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