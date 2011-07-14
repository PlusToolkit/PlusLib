#include "PlusConfigure.h"
#include "vtksys/CommandLineArguments.hxx"
#include <iomanip>

#include "vtkSmartPointer.h"
#include "vtkMatrix4x4.h"
#include "vtkTrackerBuffer.h"
#include "vtkVideoBuffer.h"
#include "vtkDataCollectorSynchronizer.h"
#include "vtkHTMLGenerator.h"
#include "vtkGnuplotExecuter.h"
#include "vtksys/SystemTools.hxx"
#include "vtkTrackedFrameList.h"


#include "itkMetaImageSequenceIO.h"
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

int main(int argc, char **argv)
{
	int numberOfErrors(0); 
	std::string inputVideoBufferSequenceFileName;
	std::string inputTrackerBufferSequenceFileName;
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
    bool generateReport(false); 

	VTK_LOG_TO_CONSOLE_ON; 
	
	int verboseLevel=PlusLogger::LOG_LEVEL_INFO;

	vtksys::CommandLineArguments args;
	args.Initialize(argc, argv);

	args.AddArgument("--input-video-buffer-seq-file-name", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputVideoBufferSequenceFileName, "Filename of the input video buffer sequence metafile.");
	args.AddArgument("--input-tracker-buffer-seq-file-name", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputTrackerBufferSequenceFileName, "Filename of the input tracker bufffer sequence metafile.");
	args.AddArgument("--input-number-of-averaged-frames", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &numberOfAveragedFrames, "Number of averaged frames (Default: 15)");
	args.AddArgument("--input-number-of-averaged-transforms", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &numberOfAveragedTransforms, "Number of averaged transforms (Default: 20)");
	args.AddArgument("--input-threshold-multiplier", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &thresholdMultiplier, "Threshold multiplier (Default: 5)");
	args.AddArgument("--input-min-transform-threshold", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &minTransformThreshold, "Minimum transform threshold (Default: 3.0)");
	args.AddArgument("--input-min-frame-threshold", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &minFrameThreshold, "Minimum frame threshold (Default: 10.0)");
	args.AddArgument("--input-max-frame-difference", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &maxFrameDifference, "Maximum frame difference (Default: 5.0)");
	args.AddArgument("--input-max-transform-difference", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &maxTransformDifference, "Maximum transform difference (Default: 5.0)");
    args.AddArgument("--generate-report", vtksys::CommandLineArguments::NO_ARGUMENT, &generateReport, "Generate HTML report (it assumes ../gnuplot/gnuplot.exe and ../scripts/ are valid)");

	args.AddArgument("--verbose", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &verboseLevel, "Verbose level (1=error only, 2=warning, 3=info, 4=debug)");	

	if ( !args.Parse() )
	{
		std::cerr << "Problem parsing arguments" << std::endl;
		std::cout << "Help: " << args.GetHelp() << std::endl;
		exit(EXIT_FAILURE);
	}


	if (inputVideoBufferSequenceFileName.empty() || inputTrackerBufferSequenceFileName.empty() )
	{
		std::cerr << "input-video-buffer-seq-file-name and input-tracker-buffer-seq-file-name arguments are required!" << std::endl;
		std::cout << "\nHelp: " << args.GetHelp() << std::endl;
		exit(EXIT_FAILURE);
	}

	///////////////

	PlusLogger::Instance()->SetLogLevel(verboseLevel);
	PlusLogger::Instance()->SetDisplayLogLevel(verboseLevel);

	std::string programPath("./"), errorMsg; 
	if ( !vtksys::SystemTools::FindProgramPath(argv[0], programPath, errorMsg) )
	{
		LOG_ERROR(errorMsg); 
	}
	programPath = vtksys::SystemTools::GetParentDirectory(programPath.c_str()); 


	// Read tracker buffer 
	LOG_INFO("Reading tracker buffer meta file..."); 
	vtkSmartPointer<vtkTrackedFrameList> trackerFrameList = vtkSmartPointer<vtkTrackedFrameList>::New(); 
	trackerFrameList->ReadFromSequenceMetafile(inputTrackerBufferSequenceFileName.c_str()); 
	
	LOG_INFO("Copy buffer to tracker buffer..."); 
	int numberOfFrames = trackerFrameList->GetNumberOfTrackedFrames();
	vtkSmartPointer<vtkTrackerBuffer> trackerBuffer = vtkSmartPointer<vtkTrackerBuffer>::New(); 
	trackerBuffer->SetBufferSize(numberOfFrames + 1); 

	for ( int frameNumber = 0; frameNumber < numberOfFrames; frameNumber++ )
	{
		PlusLogger::PrintProgressbar( (100.0 * frameNumber) / numberOfFrames ); 
		const char* strTimestamp = trackerFrameList->GetTrackedFrame(frameNumber)->GetCustomFrameField("Timestamp"); 
		double timestamp(0); 
		if ( strTimestamp != NULL )
		{
			timestamp = atof(strTimestamp); 
		}
		else
		{
			LOG_WARNING("Unable to read Timestamp field of frame #" << frameNumber); 
			numberOfErrors++; 
			continue; 
		}

		const char* strUnfilteredTimestamp = trackerFrameList->GetTrackedFrame(frameNumber)->GetCustomFrameField("UnfilteredTimestamp"); 
		double unfilteredtimestamp(0); 
		if ( strUnfilteredTimestamp != NULL )
		{
			unfilteredtimestamp = atof(strUnfilteredTimestamp); 
		}
		else
		{
			LOG_WARNING("Unable to read UnfilteredTimestamp field of frame #" << frameNumber); 
			numberOfErrors++; 
			continue; 
		}

		const char* cFlag = trackerFrameList->GetTrackedFrame(frameNumber)->GetCustomFrameField("Status"); 

		TrackerStatus status = TR_OK;
		if ( cFlag != NULL )
		{
			std::string strFlag(cFlag); 
			if ( strFlag.find("OK") != std::string::npos )
			{
				status = TR_OK;
			}
			else if ( strFlag.find("TR_MISSING") != std::string::npos )
			{
				status = TR_MISSING;
			}
			else if ( strFlag.find("TR_OUT_OF_VIEW") != std::string::npos )
			{
				status = TR_OUT_OF_VIEW;
			}
			else if ( strFlag.find("TR_OUT_OF_VOLUME") != std::string::npos )
			{
				status = TR_OUT_OF_VOLUME;
			}
			else if ( strFlag.find("TR_REQ_TIMEOUT") != std::string::npos )
			{
				status = TR_REQ_TIMEOUT;
			}
		}
		else
		{
			LOG_WARNING("Unable to read Status field of frame #" << frameNumber); 
			numberOfErrors++; 
			continue; 
		}

		const char* strFrameNumber = trackerFrameList->GetTrackedFrame(frameNumber)->GetCustomFrameField("FrameNumber"); 
		unsigned long frmnum(0); 
		if ( strFrameNumber != NULL )
		{
			frmnum = atol(strFrameNumber);
		}
		else
		{
			LOG_WARNING("Unable to read FrameNumber field of frame #" << frameNumber); 
			numberOfErrors++; 
			continue; 
		}

		
		double defaultTransform[16]; 
		if ( !trackerFrameList->GetTrackedFrame(frameNumber)->GetDefaultFrameTransform(defaultTransform) )
		{
			LOG_ERROR("Unable to get default frame transform for frame #" << frameNumber); 
			numberOfErrors++; 
			continue; 
		}
			
		vtkSmartPointer<vtkMatrix4x4> defaultTransformMatrix = vtkSmartPointer<vtkMatrix4x4>::New(); 
		defaultTransformMatrix->DeepCopy(defaultTransform); 

		trackerBuffer->AddTimeStampedItem(defaultTransformMatrix, status, frmnum, unfilteredtimestamp); 
	}
	
	PlusLogger::PrintProgressbar( 100 ); 
	std::cout << std::endl; 

	// Read video buffer 
	LOG_INFO("Reading video buffer meta file..."); 
	vtkSmartPointer<vtkTrackedFrameList> videoFrameList = vtkSmartPointer<vtkTrackedFrameList>::New(); 
	videoFrameList->ReadFromSequenceMetafile(inputVideoBufferSequenceFileName.c_str()); 
	
	const int numberOfVideoFrames = videoFrameList->GetNumberOfTrackedFrames(); 

	if ( numberOfVideoFrames == 0 )
	{
		LOG_ERROR("There are no video frames in the buffer!"); 
		return EXIT_FAILURE; 
	}

	
	const unsigned long imageWidthInPixels = videoFrameList->GetTrackedFrame(0)->ImageData->GetLargestPossibleRegion().GetSize()[0]; 
	const unsigned long imageHeightInPixels = videoFrameList->GetTrackedFrame(0)->ImageData->GetLargestPossibleRegion().GetSize()[1]; 
	unsigned int frameSizeInBytes = imageWidthInPixels * imageHeightInPixels * sizeof(TrackedFrame::PixelType);
    const int numberOfBitsPerPixel = videoFrameList->GetTrackedFrame(0)->ImageData->GetNumberOfComponentsPerPixel() * sizeof(TrackedFrame::PixelType)*8; 

	vtkSmartPointer<vtkVideoBuffer> videoBuffer = vtkSmartPointer<vtkVideoBuffer>::New(); 
	videoBuffer->SetFrameSize(imageWidthInPixels, imageHeightInPixels); 
    videoBuffer->SetNumberOfBitsPerPixel(numberOfBitsPerPixel); 

    if ( videoBuffer->SetBufferSize(numberOfVideoFrames) != PLUS_SUCCESS )
    {
        LOG_ERROR("Failed to set video buffer size!"); 
        numberOfErrors++; 
    }

	videoBuffer->UpdateBufferFrameFormats(); 
	
	LOG_INFO("Copy buffer to video buffer..."); 
	for ( int frameNumber = 0; frameNumber < numberOfVideoFrames; frameNumber++ )
	{
		PlusLogger::PrintProgressbar( (100.0 * frameNumber) / numberOfVideoFrames ); 
		const char* strTimestamp = videoFrameList->GetTrackedFrame(frameNumber)->GetCustomFrameField("Timestamp"); 
		double timestamp(0); 
		if ( strTimestamp != NULL )
		{
			timestamp = atof(strTimestamp); 
		}
		else
		{
			LOG_WARNING("Unable to read Timestamp field of frame #" << frameNumber); 
			numberOfErrors++; 
			continue; 
		}

		const char* strUnfilteredTimestamp = videoFrameList->GetTrackedFrame(frameNumber)->GetCustomFrameField("UnfilteredTimestamp"); 
		double unfilteredtimestamp(0); 
		if ( strUnfilteredTimestamp != NULL )
		{
			unfilteredtimestamp = atof(strUnfilteredTimestamp); 
		}
		else
		{
			LOG_WARNING("Unable to read UnfilteredTimestamp field of frame #" << frameNumber); 
			numberOfErrors++; 
			continue; 
		}

		const char* strFrameNumber = videoFrameList->GetTrackedFrame(frameNumber)->GetCustomFrameField("FrameNumber"); 
		unsigned long frmnum(0); 
		if ( strFrameNumber != NULL )
		{
			frmnum = atol(strFrameNumber);
		}
		else
		{
			LOG_WARNING("Unable to read FrameNumber field of frame #" << frameNumber ); 
			numberOfErrors++; 
			continue; 
		}

		TrackedFrame::PixelType *deviceDataPtr = videoFrameList->GetTrackedFrame(frameNumber)->ImageData->GetBufferPointer(); 
		const int frameSize[3] = {videoFrameList->GetTrackedFrame(frameNumber)->ImageData->GetLargestPossibleRegion().GetSize()[0], videoFrameList->GetTrackedFrame(frameNumber)->ImageData->GetLargestPossibleRegion().GetSize()[1], 1}; 
		const int numberOfBitsPerPixel = videoFrameList->GetTrackedFrame(frameNumber)->ImageData->GetNumberOfComponentsPerPixel() * sizeof(TrackedFrame::PixelType) * 8; 

    // Images in the tracked frame list always stored in MF orientation 
		if ( videoBuffer->AddTimeStampedItem(deviceDataPtr, US_IMG_ORIENT_MF, frameSize, numberOfBitsPerPixel, 0, unfilteredtimestamp, frmnum) != PLUS_SUCCESS )
		{
			LOG_WARNING("Failed to add video frame to buffer from sequence metafile with frame #" << frameNumber ); 
		}
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
    if ( generateReport )
    {
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
    }
	//************************************************************************************

	if ( numberOfErrors != 0 )
	{
		return EXIT_FAILURE; 
	}
		
	std::cout << "Test completed successfully!" << std::endl;
	return EXIT_SUCCESS; 

 }
