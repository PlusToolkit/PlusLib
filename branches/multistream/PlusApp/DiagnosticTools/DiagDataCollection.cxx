/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"
#include "vtksys/CommandLineArguments.hxx"

#include "vtkDataCollector.h"
#include "vtkTracker.h"
#include "vtkTrackerTool.h"
#include "vtkPlusVideoSource.h"
#include "vtkPlusDataBuffer.h"

#include "vtkHTMLGenerator.h"
#include "vtkGnuplotExecuter.h"
#include "vtksys/SystemTools.hxx"
#include "vtkXMLUtilities.h"
#include "vtkTimerLog.h"

int main(int argc, char **argv)
{
	bool printHelp(false);
	std::string inputConfigFileName;
	double inputAcqTimeLength(60);
	std::string outputFolder("./"); 
	std::string outputTrackerBufferSequenceFileName("TrackerBufferMetafile"); 
	std::string outputVideoBufferSequenceFileName("VideoBufferMetafile"); 
	
	int verboseLevel=vtkPlusLogger::LOG_LEVEL_UNDEFINED;

	vtksys::CommandLineArguments args;
	args.Initialize(argc, argv);

	args.AddArgument("--help", vtksys::CommandLineArguments::NO_ARGUMENT, &printHelp, "Print this help.");	
	args.AddArgument("--config-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputConfigFileName, "Name of the input configuration file.");
	args.AddArgument("--acq-time-length", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputAcqTimeLength, "Length of acquisition time in seconds (Default: 60s)");	
	args.AddArgument("--output-tracker-seq-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &outputTrackerBufferSequenceFileName, "Filename of the output tracker buffer sequence metafile (Default: TrackerBufferMetafile)");
	args.AddArgument("--output-video-seq-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &outputVideoBufferSequenceFileName, "Filename of the output video buffer sequence metafile (Default: VideoBufferMetafile)");
	args.AddArgument("--output-dir", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &outputFolder, "Output folder (Default: ./)");
	args.AddArgument("--verbose", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &verboseLevel, "Verbose level (1=error only, 2=warning, 3=info, 4=debug, 5=trace)");	

  if ( !args.Parse() )
	{
		std::cerr << "Problem parsing arguments" << std::endl;
		std::cout << "Help: " << args.GetHelp() << std::endl;
		exit(EXIT_FAILURE);
	}
  
  vtkPlusLogger::Instance()->SetLogLevel(verboseLevel);

	if ( printHelp ) 
	{
		std::cout << "Help: " << args.GetHelp() << std::endl;
		exit(EXIT_SUCCESS); 

	}

	if (inputConfigFileName.empty())
	{
		std::cerr << "--config-file is required" << std::endl;
		exit(EXIT_FAILURE);
	}

	///////////////  

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
  vtkSmartPointer<vtkXMLDataElement> configRootElement = vtkSmartPointer<vtkXMLDataElement>::Take(vtkXMLUtilities::ReadElementFromFile(inputConfigFileName.c_str()));
  if ( configRootElement == NULL )
  {	
    LOG_ERROR("Unable to read configuration from file " << inputConfigFileName.c_str()); 
    exit(EXIT_FAILURE); 
  }

  vtkPlusConfig::GetInstance()->SetDeviceSetConfigurationData(configRootElement);

	vtkSmartPointer<vtkDataCollector> dataCollector = vtkSmartPointer<vtkDataCollector>::New(); 

	dataCollector->ReadConfiguration(configRootElement);

  vtkDataCollector::DeviceCollection coll;
  dataCollector->GetTrackedFrameProducers(coll);
  if( coll.size() != 1 )
  {
    LOG_ERROR("Multiple tracked frame producers defined when only 1 expected. Please review configuration file.");
    exit(EXIT_FAILURE);
  }

  if ( dataCollector->Connect(std::string(coll.at(0)->GetDeviceId())) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to initialize data collector!"); 
    exit(EXIT_FAILURE); 
  }

  vtkPlusDevice* videoDevice = NULL;
  vtkPlusVideoSource* videoSource = NULL;
  dataCollector->GetDevice(videoDevice, "VideoSource");
  videoSource = dynamic_cast<vtkPlusVideoSource*>(videoDevice);

  if( videoSource == NULL )
  {
    LOG_ERROR("No device found with ID \"VideoSource\". Please define your video source device.");
    exit(EXIT_FAILURE);
  }

  vtkPlusDevice* trackerDevice = NULL;
  vtkTracker* tracker = NULL;
  dataCollector->GetDevice(trackerDevice, "Tracker");
  tracker = dynamic_cast<vtkTracker*>(trackerDevice);

  if( tracker == NULL )
  {
    LOG_ERROR("No device found with ID \"Tracker\". Please define your tracker device.");
    exit(EXIT_FAILURE);
  }
  
  // Enable timestamp reporting for video and all tools
	if (videoSource != NULL ) 
	{
		videoSource->GetBuffer()->SetTimeStampReporting(true);
	}
	if ( tracker != NULL )
	{
    for (ToolContainerConstIteratorType it = tracker->GetToolIteratorBegin(); it != tracker->GetToolIteratorEnd(); ++it)
		{
      vtkTrackerTool* tool = it->second;
		  tool->GetBuffer()->SetTimeStampReporting(true);
    }
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
	if ( videoSource != NULL ) 
	{
		LOG_INFO("Stop video recording ..."); 
		videoSource->StopRecording(); 
	}

	if ( tracker != NULL )
	{
		LOG_INFO("Stop tracking ..."); 
		tracker->StopTracking(); 
	}

	//************************************************************************************
	// Print statistics
	if ( videoSource != NULL ) 
	{    
    double realVideoFramePeriodStdevSec=0;
		double realVideoFrameRate = videoSource->GetBuffer()->GetFrameRate(false, &realVideoFramePeriodStdevSec);
		double idealVideoFrameRate = videoSource->GetBuffer()->GetFrameRate(true);
		int numOfItems = videoSource->GetBuffer()->GetNumberOfItems(); 
		int bufferSize = videoSource->GetBuffer()->GetBufferSize(); 

		LOG_INFO("Nominal video frame rate: " << idealVideoFrameRate << "fps"); 
    LOG_INFO("Actual video frame rate: " << realVideoFrameRate << "fps (frame period stdev: "<<realVideoFramePeriodStdevSec*1000.0<<"ms)"); 
    LOG_INFO("Number of items in the video buffer: " << numOfItems ); 
		LOG_INFO("Video buffer size: " << bufferSize); 

    // Check if the same item index (usually "frame number") is stored in multiple items. It may mean too frequent data reading from a tracking device
    int numberOfNonUniqueFrames=0;
    int numberOfValidFrames=0;
    for ( BufferItemUidType frameUid = videoSource->GetBuffer()->GetOldestItemUidInBuffer(); frameUid <= videoSource->GetBuffer()->GetLatestItemUidInBuffer(); ++frameUid )
    {
      double time(0); 
      if ( videoSource->GetBuffer()->GetTimeStamp(frameUid, time) != ITEM_OK ) { continue; }
      unsigned long framenum(0); 
      if ( videoSource->GetBuffer()->GetIndex(frameUid, framenum) != ITEM_OK) { continue; }
      numberOfValidFrames++;
      if (frameUid == videoSource->GetBuffer()->GetOldestItemUidInBuffer())
      { 
        // no previous frame
        continue;
      }
      double prevtime(0); 
      if ( videoSource->GetBuffer()->GetTimeStamp(frameUid - 1, prevtime) != ITEM_OK ) { continue; }		
      unsigned long prevframenum(0); 
      if ( videoSource->GetBuffer()->GetIndex(frameUid - 1, prevframenum) != ITEM_OK) { continue; }      
      if (framenum == prevframenum)
      {
        // the same frame number was set for different frame indexes; this should not happen
        LOG_DEBUG("Non-unique frame has been found with frame number "<<framenum<<" (uid: "<<frameUid-1<<", "<<frameUid<<", time: "<<prevtime<<", "<<time<<")");
        numberOfNonUniqueFrames++;
      }
    }  	
    LOG_INFO("Number of valid frames: "<<numberOfValidFrames);      
    LOG_INFO("Number of non-unique frames: "<<numberOfNonUniqueFrames);
    if (numberOfNonUniqueFrames>0)
    {
      LOG_WARNING("Non-unique frames are recorded in the buffer, probably the requested acquisition rate is too high");
    }

	}

	if ( tracker != NULL )
	{
    for (ToolContainerConstIteratorType it = tracker->GetToolIteratorBegin(); it != tracker->GetToolIteratorEnd(); ++it)
		{
      vtkTrackerTool* tool = it->second;

			int numOfItems = tool->GetBuffer()->GetNumberOfItems(); 
			int bufferSize = tool->GetBuffer()->GetBufferSize(); 
      double realFramePeriodStdevSec=0;
			double realFrameRate = tool->GetBuffer()->GetFrameRate(false, &realFramePeriodStdevSec);
			double idealFrameRate = tool->GetBuffer()->GetFrameRate(true);
			LOG_INFO("------------------ " << tool->GetToolName() << " ---------------------"); 
			LOG_INFO("Tracker tool " << tool <<  " actual sampling frequency: " << realFrameRate << "fps (sampling period stdev: "<<realFramePeriodStdevSec*1000.0<<"ms)"); 
			LOG_INFO("Tracker tool " << tool <<  " nominal sampling frequency: " << idealFrameRate << "fps"); 
			LOG_INFO("Number of items in the tool buffer: " << numOfItems ); 
			LOG_INFO("Tool buffer size: " << bufferSize); 

      // Check if the same item index (usually "frame number") is stored in multiple items. It may mean too frequent data reading from a tracking device
      int numberOfNonUniqueFrames=0;
      int numberOfValidFrames=0;
	    for ( BufferItemUidType frameUid = tool->GetBuffer()->GetOldestItemUidInBuffer(); frameUid <= tool->GetBuffer()->GetLatestItemUidInBuffer(); ++frameUid )
	    {
		    double time(0); 
		    if ( tool->GetBuffer()->GetTimeStamp(frameUid, time) != ITEM_OK ) { continue; }
		    unsigned long framenum(0); 
		    if ( tool->GetBuffer()->GetIndex(frameUid, framenum) != ITEM_OK) { continue; }
        numberOfValidFrames++;
        if (frameUid == tool->GetBuffer()->GetOldestItemUidInBuffer())
        { 
          // no previous frame
          continue;
        }
		    double prevtime(0); 
		    if ( tool->GetBuffer()->GetTimeStamp(frameUid - 1, prevtime) != ITEM_OK ) { continue; }		
		    unsigned long prevframenum(0); 
		    if ( tool->GetBuffer()->GetIndex(frameUid - 1, prevframenum) != ITEM_OK) { continue; }
		    if (framenum == prevframenum)
		    {
			    // the same frame number was set for different frame indexes; this should not happen
          LOG_DEBUG("Non-unique frame has been found with frame number "<<framenum<<" (uid: "<<frameUid-1<<", "<<frameUid<<", time: "<<prevtime<<", "<<time<<")");
          numberOfNonUniqueFrames++;
		    }
	    }  	
      LOG_INFO("Number of valid frames: "<<numberOfValidFrames);      
      LOG_INFO("Number of non-unique frames: "<<numberOfNonUniqueFrames);
      if (numberOfNonUniqueFrames>0)
      {
        LOG_WARNING("Non-unique frames are recorded in the buffer, probably the requested acquisition rate is too high");
      }
		}
	}  


	//************************************************************************************
	// Generate html report
  LOG_INFO("Generate report ...");
  vtkSmartPointer<vtkHTMLGenerator> htmlReport = vtkSmartPointer<vtkHTMLGenerator>::New(); 
  htmlReport->SetTitle("Data Collection Report"); 

  vtkSmartPointer<vtkGnuplotExecuter> plotter = vtkSmartPointer<vtkGnuplotExecuter>::New(); 
  plotter->SetHideWindow(true); 

  // Generate tracking data acq report
  if ( tracker != NULL )
  {
    tracker->GenerateTrackingDataAcquisitionReport(htmlReport, plotter); 
  }

  // Generate video data acq report
  if ( videoSource != NULL ) 
  {
    videoSource->GenerateVideoDataAcquisitionReport(htmlReport, plotter); 
  }

  std::string reportFileName = plotter->GetWorkingDirectory() + std::string("/DataCollectionReport.html"); 
  htmlReport->SaveHtmlPage(reportFileName.c_str()); 

	//************************************************************************************
	// Dump buffers to file 
	if ( videoSource != NULL ) 
	{
		LOG_INFO("Write video buffer to " << outputVideoBufferSequenceFileName);
		videoSource->GetBuffer()->WriteToMetafile( outputFolder.c_str(), outputVideoBufferSequenceFileName.c_str(), false); 
	}

	if ( tracker != NULL )
	{
		LOG_INFO("Write tracker buffer to " << outputTrackerBufferSequenceFileName);
		tracker->WriteToMetafile( outputFolder.c_str(), outputTrackerBufferSequenceFileName.c_str(), false); 
	}

	dataCollector->Disconnect(); 

	return EXIT_SUCCESS; 
}

