#include "PlusConfigure.h"
#include "vtksys/CommandLineArguments.hxx"
#include "vtkDataCollector.h"
#include "vtkDataCollectorSynchronizer.h"
#include "vtkTracker.h"
#include "vtkTrackerTool.h"
#include "vtkVideoBuffer.h"
#include "vtkHTMLGenerator.h"
#include "vtkGnuplotExecuter.h"
#include "vtksys/SystemTools.hxx"
#include "vtkXMLUtilities.h"

int main(int argc, char **argv)
{
	bool printHelp(false);
	std::string inputConfigFileName;
	double inputAcqTimeLength(60);
	std::string outputFolder("./"); 
	std::string outputTrackerBufferSequenceFileName("TrackerBufferMetafile"); 
	std::string outputVideoBufferSequenceFileName("VideoBufferMetafile"); 
	int numberOfAveragedFrames(15); 
	int numberOfAveragedTransforms(20); 
	double thresholdMultiplier(5); 

	int verboseLevel=vtkPlusLogger::LOG_LEVEL_INFO;

	vtksys::CommandLineArguments args;
	args.Initialize(argc, argv);

	args.AddArgument("--help", vtksys::CommandLineArguments::NO_ARGUMENT, &printHelp, "Print this help.");	
	args.AddArgument("--input-config-file-name", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputConfigFileName, "Name of the input configuration file.");
	args.AddArgument("--input-acq-time-length", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputAcqTimeLength, "Length of acquisition time in seconds (Default: 60s)");	
	args.AddArgument("--output-tracker-buffer-seq-file-name", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &outputTrackerBufferSequenceFileName, "Filename of the output tracker buffer sequence metafile (Default: TrackerBufferMetafile)");
	args.AddArgument("--output-video-buffer-seq-file-name", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &outputVideoBufferSequenceFileName, "Filename of the output video buffer sequence metafile (Default: VideoBufferMetafile)");
	args.AddArgument("--output-folder", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &outputFolder, "Output folder (Default: ./)");
	args.AddArgument("--averaged-frames", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &numberOfAveragedFrames, "Number of averaged frames for synchronization (Default: 15)");
	args.AddArgument("--averaged-transforms", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &numberOfAveragedTransforms, "Number of averaged transforms for synchronization (Default: 20)");
	args.AddArgument("--threshold-multiplier", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &thresholdMultiplier, "Set the stdev multiplier of threshold value for synchronization (Default: 5)");

	args.AddArgument("--verbose", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &verboseLevel, "Verbose level (1=error only, 2=warning, 3=info, 4=debug)");	

	vtkPlusLogger::Instance()->SetLogLevel(verboseLevel);

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

	if (inputConfigFileName.empty())
	{
		std::cerr << "input-config-file-name is required" << std::endl;
		exit(EXIT_FAILURE);
	}

	///////////////

	VTK_LOG_TO_CONSOLE_ON; 

	//************************************************************************************
	// Find program path 
	std::string programPath("./"), errorMsg; 
	if ( !vtksys::SystemTools::FindProgramPath(argv[0], programPath, errorMsg) )
	{
		LOG_ERROR(errorMsg); 
	}
	programPath = vtksys::SystemTools::GetParentDirectory(programPath.c_str()); 

	//************************************************************************************
	// Initialize data collector
	vtkSmartPointer<vtkDataCollector> dataCollector = vtkSmartPointer<vtkDataCollector>::New(); 
  vtkSmartPointer<vtkXMLDataElement> configRootElement = vtkXMLUtilities::ReadElementFromFile(inputConfigFileName.c_str());
  if (configRootElement == NULL) {	
    LOG_ERROR("Unable to read configuration from file " << inputConfigFileName); 
		exit(EXIT_FAILURE);
  }
	dataCollector->ReadConfiguration(configRootElement);
	dataCollector->Initialize(); 
	dataCollector->Start();

	const int mainToolNumber = dataCollector->GetTracker()->GetFirstPortNumberByType(TRACKER_TOOL_PROBE);
	const double acqStartTime = vtkTimerLog::GetUniversalTime(); 

	//************************************************************************************
	// Record data
	while ( acqStartTime + inputAcqTimeLength > vtkTimerLog::GetUniversalTime() )
	{
		LOG_INFO( acqStartTime + inputAcqTimeLength - vtkTimerLog::GetUniversalTime() << " seconds left..." ); 
		vtksys::SystemTools::Delay(1000); 
	}

	
	//************************************************************************************
	// Copy buffers to local buffer
	vtkSmartPointer<vtkVideoBuffer> videobuffer = vtkSmartPointer<vtkVideoBuffer>::New(); 
	if ( dataCollector->GetVideoSource() != NULL ) 
	{
		LOG_INFO("Copy video buffer ..."); 
		dataCollector->CopyVideoBuffer(videobuffer); 
	}

	vtkSmartPointer<vtkTracker> tracker = vtkSmartPointer<vtkTracker>::New(); 
	if ( dataCollector->GetTracker() != NULL )
	{
		LOG_INFO("Copy tracker ..."); 
		dataCollector->CopyTracker(tracker); 
	}
	vtkTrackerBuffer* trackerbuffer = tracker->GetTool(mainToolNumber)->GetBuffer(); 

	//************************************************************************************
	// Stop recording
	if ( dataCollector->GetVideoSource() != NULL ) 
	{
		LOG_INFO("Stop video recording ..."); 
		dataCollector->GetVideoSource()->StopRecording(); 
	}

	if ( dataCollector->GetTracker() != NULL )
	{
		LOG_INFO("Stop tracking ..."); 
		dataCollector->GetTracker()->StopTracking(); 
	}

	//************************************************************************************
	// Run synchronizer 
	LOG_INFO("Initialize synchronizer..."); 
	vtkSmartPointer<vtkDataCollectorSynchronizer> synchronizer = vtkSmartPointer<vtkDataCollectorSynchronizer>::New(); 
	synchronizer->SetSynchronizationTimeLength(inputAcqTimeLength); 
	synchronizer->SetNumberOfAveragedFrames(numberOfAveragedFrames); 
	synchronizer->SetNumberOfAveragedTransforms(numberOfAveragedTransforms); 
	synchronizer->SetThresholdMultiplier(thresholdMultiplier); 
	synchronizer->SetTrackerBuffer(trackerbuffer); 
	synchronizer->SetVideoBuffer(videobuffer); 

	LOG_INFO("Number Of Averaged Frames: " << numberOfAveragedFrames); 
	LOG_INFO("Number Of Averaged Transforms: " << numberOfAveragedTransforms ); 
	LOG_INFO("Threshold Multiplier: " << thresholdMultiplier ); 
	LOG_INFO("Tracker Buffer Size: " << trackerbuffer->GetNumberOfItems() ); 
	LOG_INFO("Tracker Frame Rate: " << trackerbuffer->GetFrameRate() ); 
	LOG_INFO("Video Buffer Size: " << videobuffer->GetNumberOfItems() ); 
	LOG_INFO("Video Frame Rate: " << videobuffer->GetFrameRate() ); 

	synchronizer->Synchronize(); 

	//************************************************************************************
	// Generate html report
	LOG_INFO("Generate report ...");
	const std::string gnuplotPath = vtksys::SystemTools::CollapseFullPath("../gnuplot/gnuplot.exe", programPath.c_str()); 
	const std::string gnuplotScriptsFolder = vtksys::SystemTools::CollapseFullPath("../scripts/"  , programPath.c_str()); 

	vtkSmartPointer<vtkHTMLGenerator> htmlReport = vtkSmartPointer<vtkHTMLGenerator>::New(); 
	htmlReport->SetTitle("iCAL Temporal Calibration Report"); 

	vtkSmartPointer<vtkGnuplotExecuter> plotter = vtkSmartPointer<vtkGnuplotExecuter>::New(); 
	plotter->SetGnuplotCommand(gnuplotPath.c_str()); 
	plotter->SetWorkingDirectory( programPath.c_str() ); 
	plotter->SetHideWindow(true); 

	// Generate tracking data acq report
	tracker->GenerateTrackingDataAcquisitionReport(htmlReport, plotter, gnuplotScriptsFolder.c_str()); 

	// Generate video data acq report
	dataCollector->GetVideoSource()->GenerateVideoDataAcquisitionReport(htmlReport, plotter, gnuplotScriptsFolder.c_str()); 

	// Synchronizer Analysis report
	synchronizer->GenerateSynchronizationReport(htmlReport, plotter, gnuplotScriptsFolder.c_str()); 

	htmlReport->SaveHtmlPage("iCALTemporalCalibrationReport.html"); 
	//************************************************************************************

	// Dump buffers to file 
	if ( dataCollector->GetVideoSource() != NULL ) 
	{
		LOG_INFO("Write video buffer to " << outputVideoBufferSequenceFileName);
		dataCollector->WriteVideoBufferToMetafile( videobuffer, outputFolder.c_str(), outputVideoBufferSequenceFileName.c_str(), false); 
	}

	if ( dataCollector->GetTracker() != NULL )
	{
		LOG_INFO("Write tracker buffer to " << outputTrackerBufferSequenceFileName);
		dataCollector->WriteTrackerToMetafile( tracker, outputFolder.c_str(), outputTrackerBufferSequenceFileName.c_str(), false); 
	}

	VTK_LOG_TO_CONSOLE_OFF; 

	return EXIT_SUCCESS; 
}

