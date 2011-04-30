#include "PlusConfigure.h"
#include "vtksys/CommandLineArguments.hxx"
#include <iomanip>

#include "vtkSmartPointer.h"
#include "vtkMatrix4x4.h"
#include "vtkTrackerBuffer.h"
#include "vtkVideoBuffer2.h"
#include "vtkVideoFrame2.h"
#include "vtkDataCollectorSynchronizer.h"
#include "vtkHTMLGenerator.h"
#include "vtkGnuplotExecuter.h"
#include "vtksys/SystemTools.hxx"


#include "itkMetaImageSequenceIO.h"
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

///////////////////////////////////////////////////////////////////
// Image type definition

typedef unsigned char          PixelType; // define type for pixel representation
const unsigned int             imageDimension = 2; 
const unsigned int             imageSequenceDimension = 3; 

typedef itk::Image< PixelType, imageDimension > ImageType;
typedef itk::Image< PixelType, imageSequenceDimension > ImageSequenceType;

typedef itk::ImageFileReader< ImageSequenceType > ImageSequenceReaderType;
typedef itk::ImageFileWriter< ImageSequenceType > ImageSequenceWriterType;

///////////////////////////////////////////////////////////////////


int main(int argc, char **argv)
{

	std::string inputVideoBufferSequenceFileName;
	std::string inputTrackerBufferSequenceFileName;
	std::string inputTestDataDir;
	double inputSyncStartTimestamp(0); 
	double synchronizationTimeLength(20); 
	int minNumOfSyncSteps(5); 
	int numberOfAveragedFrames(15); 
	int numberOfAveragedTransforms(20); 
	double minTransformThreshold(3.0); 
	double minFrameThreshold(10.0); 
	double maxFrameDifference(5.0); 
	double maxTransformDifference(2.0); 
	double thresholdMultiplier(5); 

	VTK_LOG_TO_CONSOLE_ON; 
	
	int verboseLevel=PlusLogger::LOG_LEVEL_INFO;

	vtksys::CommandLineArguments args;
	args.Initialize(argc, argv);

	args.AddArgument("--input-test-data-dir", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputTestDataDir, "Test data directory");
	args.AddArgument("--input-video-buffer-seq-file-name", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputVideoBufferSequenceFileName, "Filename of the input video buffer sequence metafile.");
	args.AddArgument("--input-tracker-buffer-seq-file-name", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputTrackerBufferSequenceFileName, "Filename of the input tracker bufffer sequence metafile.");
	args.AddArgument("--input-number-of-averaged-frames", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &numberOfAveragedFrames, "Number of averaged frames (Default: 15)");
	args.AddArgument("--input-number-of-averaged-transforms", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &numberOfAveragedTransforms, "Number of averaged transforms (Default: 20)");
	args.AddArgument("--input-threshold-multiplier", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &thresholdMultiplier, "Threshold multiplier (Default: 5)");
	args.AddArgument("--input-min-transform-threshold", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &minTransformThreshold, "Minimum transform threshold (Default: 3.0)");
	args.AddArgument("--input-min-frame-threshold", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &minFrameThreshold, "Minimum frame threshold (Default: 10.0)");
	args.AddArgument("--input-max-frame-difference", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &maxFrameDifference, "Maximum frame difference (Default: 5.0)");
	args.AddArgument("--input-max-transform-difference", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &maxTransformDifference, "Maximum transform difference (Default: 5.0)");

	args.AddArgument("--verbose", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &verboseLevel, "Verbose level (1=error only, 2=warning, 3=info, 4=debug)");	

	if ( !args.Parse() )
	{
		std::cerr << "Problem parsing arguments" << std::endl;
		std::cout << "Help: " << args.GetHelp() << std::endl;
		exit(EXIT_FAILURE);
	}


	if (inputVideoBufferSequenceFileName.empty() || inputTrackerBufferSequenceFileName.empty() || inputTestDataDir.empty() )
	{
		std::cerr << "input-test-data-dir, input-video-buffer-seq-file-name and input-tracker-buffer-seq-file-name arguments are required!" << std::endl;
		std::cout << "\nHelp: " << args.GetHelp() << std::endl;
		exit(EXIT_FAILURE);
	}

	///////////////

	PlusLogger::Instance()->SetLogLevel(verboseLevel);

	std::string programPath("./"), errorMsg; 
	if ( !vtksys::SystemTools::FindProgramPath(argv[0], programPath, errorMsg) )
	{
		LOG_ERROR(errorMsg); 
	}
	programPath = vtksys::SystemTools::GetParentDirectory(programPath.c_str()); 


	// Read tracker buffer 
	itk::MetaImageSequenceIO::Pointer trackerReaderIO = itk::MetaImageSequenceIO::New(); 
	ImageSequenceReaderType::Pointer trackerReader = ImageSequenceReaderType::New(); // reader object, pointed to by smart pointer

	// Set the image IO 
	trackerReader->SetImageIO(trackerReaderIO); 
				
	std::string trackerBufferPath = inputTestDataDir + "/" + inputTrackerBufferSequenceFileName;
	trackerReader->SetFileName(trackerBufferPath); 

	LOG_INFO("Reading tracker buffer meta file: " << trackerBufferPath); 

	try
	{
		trackerReader->Update(); 
	}
	catch (itk::ExceptionObject & err) 
	{		
		LOG_ERROR(" Exception! Tracker reader did not update: " <<  err); 
		return EXIT_FAILURE;
	}	
	
	LOG_INFO("Copy buffer to tracker buffer..."); 
	int numberOfFrames = trackerReaderIO->GetDimensions(2);
	vtkSmartPointer<vtkTrackerBuffer> trackerBuffer = vtkSmartPointer<vtkTrackerBuffer>::New(); 
	trackerBuffer->SetBufferSize(numberOfFrames + 1); 

	for ( int frameNumber = 0; frameNumber < numberOfFrames; frameNumber++ )
	{
		PlusLogger::PrintProgressbar( (100.0 * frameNumber) / numberOfFrames ); 
		const char* strTimestamp = trackerReaderIO->GetCustomFrameString(frameNumber, "Timestamp"); 
		double timestamp(0); 
		if ( strTimestamp != NULL )
		{
			timestamp = atof(strTimestamp); 
		}
		else
		{
			LOG_WARNING("Unable to read Timestamp field of frame #" << frameNumber); 
		}

		const char* strUnfilteredTimestamp = trackerReaderIO->GetCustomFrameString(frameNumber, "UnfilteredTimestamp"); 
		double unfilteredtimestamp(0); 
		if ( strUnfilteredTimestamp != NULL )
		{
			unfilteredtimestamp = atof(strUnfilteredTimestamp); 
		}
		else
		{
			LOG_WARNING("Unable to read UnfilteredTimestamp field of frame #" << frameNumber); 
		}

		const char* cFlag = trackerReaderIO->GetCustomFrameString(frameNumber, "Status"); 

		long flag(0);
		if ( cFlag != NULL )
		{
			std::string strFlag(cFlag); 
			if ( strFlag.find("OK") != std::string::npos )
			{
				flag = 0;
			}
			else if ( strFlag.find("TR_MISSING") != std::string::npos )
			{
				flag = flag & TR_MISSING;
			}
			else if ( strFlag.find("TR_OUT_OF_VIEW") != std::string::npos )
			{
				flag = flag & TR_OUT_OF_VIEW;
			}
			else if ( strFlag.find("TR_OUT_OF_VOLUME") != std::string::npos )
			{
				flag = flag & TR_OUT_OF_VOLUME;
			}
			else if ( strFlag.find("TR_REQ_TIMEOUT") != std::string::npos )
			{
				flag = flag & TR_REQ_TIMEOUT;
			}
		}
		else
		{
			LOG_WARNING("Unable to read Status field of frame #" << frameNumber); 
		}

		const char* strFrameNumber = trackerReaderIO->GetCustomFrameString(frameNumber, "FrameNumber"); 
		unsigned long frmnum(0); 
		if ( strFrameNumber != NULL )
		{
			frmnum = atol(strFrameNumber);
		}
		else
		{
			LOG_WARNING("Unable to read FrameNumber field of frame #" << frameNumber); 
		}

		vtkSmartPointer<vtkMatrix4x4> matrix = vtkSmartPointer<vtkMatrix4x4>::New(); 
		if ( !trackerReaderIO->GetFrameTransform(frameNumber, matrix) )
		{
			LOG_ERROR("Unable to get default frame transform for frame #" << frameNumber); 
		}

		trackerBuffer->AddItem(matrix, flag, frmnum, unfilteredtimestamp, timestamp); 
	}
	
	PlusLogger::PrintProgressbar( 100 ); 
	std::cout << std::endl; 

	LOG_INFO("Reading video buffer meta file..."); 
	
	// Read video buffer 
	itk::MetaImageSequenceIO::Pointer videoReaderIO = itk::MetaImageSequenceIO::New(); 
	ImageSequenceReaderType::Pointer videoReader = ImageSequenceReaderType::New(); // reader object, pointed to by smart pointer

	// Set the image IO 
	videoReader->SetImageIO(videoReaderIO); 
				
	std::string videoBufferPath = inputTestDataDir + "/" + inputVideoBufferSequenceFileName;
	videoReader->SetFileName(videoBufferPath); 

	try
	{
		videoReader->Update(); 
	}
	catch (itk::ExceptionObject & err) 
	{		
		LOG_ERROR(" Exception! Video reader did not update: " <<  err); 
		return EXIT_FAILURE;
	}	
	
	ImageSequenceType::Pointer imageSeq = videoReader->GetOutput();
	const unsigned long imageWidthInPixels = imageSeq->GetLargestPossibleRegion().GetSize()[0]; 
	const unsigned long imageHeightInPixels = imageSeq->GetLargestPossibleRegion().GetSize()[1]; 
	const unsigned long numberOfVideoFrames = imageSeq->GetLargestPossibleRegion().GetSize()[2];	
	unsigned int frameSizeInBytes = imageWidthInPixels * imageHeightInPixels * sizeof(PixelType);

	vtkSmartPointer<vtkVideoBuffer2> videoBuffer = vtkSmartPointer<vtkVideoBuffer2>::New(); 
	videoBuffer->GetFrameFormat()->SetFrameSize(imageWidthInPixels, imageHeightInPixels, 1); 
	videoBuffer->GetFrameFormat()->SetFrameExtent(0, imageWidthInPixels - 1, 0, imageHeightInPixels - 1, 0, 1); 
	videoBuffer->GetFrameFormat()->SetPixelFormat(VTK_LUMINANCE); 
	videoBuffer->GetFrameFormat()->SetBitsPerPixel(8); 
	videoBuffer->GetFrameFormat()->SetFrameGrabberType(FG_BASE); 
	videoBuffer->SetBufferSize(numberOfVideoFrames + 1); 
	
	LOG_INFO("Allocate space for video buffer..."); 
	for ( int i = 0; i < videoBuffer->GetBufferSize(); i++ )
	{
		videoBuffer->GetFrame(i)->Allocate(); 
	}

	LOG_INFO("Copy buffer to video buffer..."); 
	PixelType* imageSeqData = imageSeq->GetBufferPointer();
	for ( int frameNumber = 0; frameNumber < numberOfVideoFrames; frameNumber++ )
	{
		PlusLogger::PrintProgressbar( (100.0 * frameNumber) / numberOfVideoFrames ); 
		const char* strTimestamp = videoReaderIO->GetCustomFrameString(frameNumber, "Timestamp"); 
		double timestamp(0); 
		if ( strTimestamp != NULL )
		{
			timestamp = atof(strTimestamp); 
		}
		else
		{
			LOG_WARNING("Unable to read Timestamp field of frame #" << frameNumber); 
		}

		const char* strUnfilteredTimestamp = trackerReaderIO->GetCustomFrameString(frameNumber, "UnfilteredTimestamp"); 
		double unfilteredtimestamp(0); 
		if ( strUnfilteredTimestamp != NULL )
		{
			unfilteredtimestamp = atof(strUnfilteredTimestamp); 
		}
		else
		{
			LOG_WARNING("Unable to read UnfilteredTimestamp field of frame #" << frameNumber); 
		}

		const char* strFrameNumber = videoReaderIO->GetCustomFrameString(frameNumber, "FrameNumber"); 
		unsigned long frmnum(0); 
		if ( strFrameNumber != NULL )
		{
			frmnum = atol(strFrameNumber);
		}
		else
		{
			LOG_WARNING("Unable to read FrameNumber field of frame #" << frameNumber ); 
		}

		videoBuffer->Seek(1); 
		PixelType* currentFrameImageData = imageSeqData + frameNumber * frameSizeInBytes;
		unsigned char *frameBufferPtr = reinterpret_cast<unsigned char *>(videoBuffer->GetFrame(0)->GetVoidPointer(0));
		memcpy(frameBufferPtr,currentFrameImageData,frameSizeInBytes);
		videoBuffer->AddItem(videoBuffer->GetFrame(0), unfilteredtimestamp, timestamp, frmnum );
	}

	PlusLogger::PrintProgressbar( 100 ); 
	std::cout << std::endl; 



	LOG_INFO("Initialize synchronizer..."); 
	vtkSmartPointer<vtkDataCollectorSynchronizer> synchronizer = vtkSmartPointer<vtkDataCollectorSynchronizer>::New(); 
	synchronizer->SetSyncStartTime(inputSyncStartTimestamp); 
	synchronizer->SetSynchronizationTimeLength(synchronizationTimeLength); 
	synchronizer->SetMinNumOfSyncSteps(minNumOfSyncSteps);
	synchronizer->SetNumberOfAveragedFrames(numberOfAveragedFrames); 
	synchronizer->SetNumberOfAveragedTransforms(numberOfAveragedTransforms); 
	synchronizer->SetThresholdMultiplier(thresholdMultiplier); 
	synchronizer->SetMinFrameThreshold(minFrameThreshold); 
	synchronizer->SetMinTransformThreshold(minTransformThreshold); 
	synchronizer->SetMaxFrameDifference(maxFrameDifference); 
	synchronizer->SetMaxTransformDifference(maxTransformDifference); 
	synchronizer->SetTrackerBuffer(trackerBuffer); 
	synchronizer->SetVideoBuffer(videoBuffer); 
	synchronizer->SetProgressBarUpdateCallbackFunction(PlusLogger::PrintProgressbar); 

	LOG_INFO("Synchronization Start Timestamp: " << std::fixed << inputSyncStartTimestamp); 
	LOG_INFO("Synchronization Time Length: " << synchronizationTimeLength << "s"); 
	LOG_INFO("Minimum Number Of Synchronization Steps: " << minNumOfSyncSteps); 
	LOG_INFO("Number Of Averaged Frames: " << numberOfAveragedFrames); 
	LOG_INFO("Minimum Frame Threshold: " << minFrameThreshold); 
	LOG_INFO("Maximum Frame Difference: " << maxFrameDifference); 
	LOG_INFO("Number Of Averaged Transforms: " << numberOfAveragedTransforms ); 
	LOG_INFO("Minimum Transform Threshold: " << minTransformThreshold);
	LOG_INFO("Maximum Transform Difference: " << maxTransformDifference); 
	LOG_INFO("Threshold Multiplier: " << thresholdMultiplier ); 
	LOG_INFO("Tracker Buffer Size: " << trackerBuffer->GetNumberOfItems() ); 
	LOG_INFO("Tracker Frame Rate: " << trackerBuffer->GetFrameRate() ); 
	LOG_INFO("Video Buffer Size: " << videoBuffer->GetNumberOfItems() ); 
	LOG_INFO("Video Frame Rate: " << videoBuffer->GetFrameRate() ); 

	synchronizer->Synchronize(); 

	//************************************************************************************
	// Generate html report
	const std::string gnuplotPath = vtksys::SystemTools::CollapseFullPath("../gnuplot/gnuplot.exe", programPath.c_str()); 
	const std::string gnuplotScriptsFolder = vtksys::SystemTools::CollapseFullPath("../scripts/"  , programPath.c_str()); 

	vtkSmartPointer<vtkHTMLGenerator> htmlReport = vtkSmartPointer<vtkHTMLGenerator>::New(); 
	htmlReport->SetTitle("iCAL Temporal Calibration Report"); 

	vtkSmartPointer<vtkGnuplotExecuter> plotter = vtkSmartPointer<vtkGnuplotExecuter>::New(); 
	plotter->SetGnuplotCommand(gnuplotPath.c_str()); 
	plotter->SetWorkingDirectory( programPath.c_str() ); 
	plotter->SetHideWindow(true); 

	// Synchronizer Analysis
	synchronizer->GenerateSynchronizationReport(htmlReport, plotter, gnuplotScriptsFolder.c_str()); 

	htmlReport->SaveHtmlPage("iCALTemporalCalibrationReport.html"); 
	//************************************************************************************

	std::cout << "Test completed successfully!" << std::endl;
	return EXIT_SUCCESS; 

 }
