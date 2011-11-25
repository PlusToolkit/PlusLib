/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

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
#include "vtkTimerLog.h"
#include "vtkPlusVideoSource.h"

int main(int argc, char **argv)
{
	bool printHelp(false);
	std::string inputConfigFileName;
	double inputAcqTimeLength(60);
	std::string outputFolder("./"); 
	std::string outputTrackerBufferSequenceFileName("TrackerBufferMetafile"); 
	std::string outputVideoBufferSequenceFileName("VideoBufferMetafile"); 
	
	int verboseLevel=vtkPlusLogger::LOG_LEVEL_DEFAULT;

	vtksys::CommandLineArguments args;
	args.Initialize(argc, argv);

	args.AddArgument("--help", vtksys::CommandLineArguments::NO_ARGUMENT, &printHelp, "Print this help.");	
	args.AddArgument("--input-config-file-name", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputConfigFileName, "Name of the input configuration file.");
	args.AddArgument("--input-acq-time-length", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputAcqTimeLength, "Length of acquisition time in seconds (Default: 60s)");	
	args.AddArgument("--output-tracker-buffer-seq-file-name", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &outputTrackerBufferSequenceFileName, "Filename of the output tracker buffer sequence metafile (Default: TrackerBufferMetafile)");
	args.AddArgument("--output-video-buffer-seq-file-name", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &outputVideoBufferSequenceFileName, "Filename of the output video buffer sequence metafile (Default: VideoBufferMetafile)");
	args.AddArgument("--output-folder", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &outputFolder, "Output folder (Default: ./)");
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

	if (inputConfigFileName.empty())
	{
		std::cerr << "input-config-file-name is required" << std::endl;
		exit(EXIT_FAILURE);
	}

	///////////////

  vtkPlusLogger::Instance()->SetLogLevel(verboseLevel);

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
  vtkSmartPointer<vtkXMLDataElement> configRootElement = vtkSmartPointer<vtkXMLDataElement>::Take(
    vtkXMLUtilities::ReadElementFromFile(inputConfigFileName.c_str()));  
  if ( configRootElement == NULL )
  {	
    LOG_ERROR("Unable to read configuration from file " << inputConfigFileName.c_str()); 
    exit(EXIT_FAILURE); 
  }
	dataCollector->ReadConfiguration(configRootElement);
	if ( dataCollector->Connect() != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to initialize data collector!"); 
    exit(EXIT_FAILURE); 
  }

	if ( dataCollector->Start() != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to start data collection!"); 
    exit(EXIT_FAILURE);
  }

	const double acqStartTime = vtkTimerLog::GetUniversalTime(); 

	//************************************************************************************
	// Record data
	while ( acqStartTime + inputAcqTimeLength > vtkTimerLog::GetUniversalTime() )
	{
		LOG_INFO( acqStartTime + inputAcqTimeLength - vtkTimerLog::GetUniversalTime() << " seconds left..." ); 
		vtksys::SystemTools::Delay(1000); 
	}

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
	// Print statistics
	if ( dataCollector->GetVideoSource() != NULL ) 
	{
		double realVideoFrameRate = dataCollector->GetVideoSource()->GetBuffer()->GetFrameRate();
		double idealVideoFrameRate = dataCollector->GetVideoSource()->GetBuffer()->GetFrameRate(true);
		int numOfItems = dataCollector->GetVideoSource()->GetBuffer()->GetNumberOfItems(); 
		int bufferSize = dataCollector->GetVideoSource()->GetBuffer()->GetBufferSize(); 

		LOG_INFO("Real video frame rate: " << realVideoFrameRate << "fps"); 
		LOG_INFO("Ideal video frame rate: " << idealVideoFrameRate << "fps"); 
		LOG_INFO("Number of items in the video buffer: " << numOfItems ); 
		LOG_INFO("Video buffer size: " << bufferSize); 
	}

	if ( dataCollector->GetTracker() != NULL )
	{
    for (ToolIteratorType it = dataCollector->GetTracker()->GetToolIteratorBegin(); it != dataCollector->GetTracker()->GetToolIteratorEnd(); ++it)
		{
      vtkTrackerTool* tool = it->second;

			int numOfItems = tool->GetBuffer()->GetNumberOfItems(); 
			int bufferSize = tool->GetBuffer()->GetBufferSize(); 
			double realFrameRate = tool->GetBuffer()->GetFrameRate();
			double idealFrameRate = tool->GetBuffer()->GetFrameRate(true);
			LOG_INFO("------------------ " << tool->GetToolName() << " ---------------------"); 
			LOG_INFO("Tracker tool " << tool <<  " real sampling frequency: " << realFrameRate << "fps"); 
			LOG_INFO("Tracker tool " << tool <<  " ideal sampling frequency: " << idealFrameRate << "fps"); 
			LOG_INFO("Number of items in the tool buffer: " << numOfItems ); 
			LOG_INFO("Tool buffer size: " << bufferSize); 
		}
	}


	//************************************************************************************
	// Generate html report
  LOG_INFO("Generate report ...");
  vtkSmartPointer<vtkHTMLGenerator> htmlReport = vtkSmartPointer<vtkHTMLGenerator>::New(); 
  htmlReport->SetTitle("iCAL Temporal Calibration Report"); 

  vtkSmartPointer<vtkGnuplotExecuter> plotter = vtkSmartPointer<vtkGnuplotExecuter>::New(); 
  plotter->SetHideWindow(true); 

  // Generate tracking data acq report
  if ( dataCollector->GetTracker() != NULL )
  {
    dataCollector->GetTracker()->GenerateTrackingDataAcquisitionReport(htmlReport, plotter); 
  }

  // Generate video data acq report
  if ( dataCollector->GetVideoSource() != NULL ) 
  {
    dataCollector->GetVideoSource()->GenerateVideoDataAcquisitionReport(htmlReport, plotter); 
  }

  std::string reportFileName = plotter->GetWorkingDirectory() + std::string("/iCALDataCollectionReport.html"); 
  htmlReport->SaveHtmlPage(reportFileName.c_str()); 

	//************************************************************************************
	// Dump buffers to file 
	if ( dataCollector->GetVideoSource() != NULL ) 
	{
		LOG_INFO("Write video buffer to " << outputVideoBufferSequenceFileName);
		dataCollector->GetVideoSource()->GetBuffer()->WriteToMetafile( outputFolder.c_str(), outputVideoBufferSequenceFileName.c_str(), false); 
	}

	if ( dataCollector->GetTracker() != NULL )
	{
		LOG_INFO("Write tracker buffer to " << outputTrackerBufferSequenceFileName);
		dataCollector->GetTracker()->WriteToMetafile( outputFolder.c_str(), outputTrackerBufferSequenceFileName.c_str(), false); 
	}

	dataCollector->Disconnect(); 

	return EXIT_SUCCESS; 
}

