/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"
#include "vtkDataCollector.h"
#include "vtkHTMLGenerator.h"
#include "vtkPlusChannel.h"
#include "vtkPlusDataSource.h"
#include "vtkPlusDevice.h"
#include "vtkTimerLog.h"
#include "vtkXMLUtilities.h"
#include "vtksys/CommandLineArguments.hxx"
#include "vtksys/SystemTools.hxx"

int main(int argc, char **argv)
{
  bool printHelp(false);
  std::string inputConfigFileName;
  double inputAcqTimeLength(60);
  std::vector<std::string> acqChannelIds;
  std::string outputSequenceFileNamePrefix="Diag";

  int verboseLevel=vtkPlusLogger::LOG_LEVEL_UNDEFINED;

  vtksys::CommandLineArguments args;
  args.Initialize(argc, argv);

  args.AddArgument("--help", vtksys::CommandLineArguments::NO_ARGUMENT, &printHelp, "Print this help.");
  args.AddArgument("--config-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputConfigFileName, "Name of the input configuration file.");
  args.AddArgument("--acq-time-length", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputAcqTimeLength, "Length of acquisition time in seconds (Default: 60s)");
  args.AddArgument("--acq-channel-ids", vtksys::CommandLineArguments::MULTI_ARGUMENT, &acqChannelIds, "Identifiers of the output channels that are recorded. If not specified then all channels are recorded.");
  args.AddArgument("--output-seq-file-prefix", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &outputSequenceFileNamePrefix, "Filename prefix for the recorded output channels (Default: Diag)");
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

  vtkPlusLogger::Instance()->SetLogLevel(verboseLevel);

  if (inputConfigFileName.empty())
  {
    std::cerr << "--config-file is required" << std::endl;
    exit(EXIT_FAILURE);
  }

  // Find program path
  std::string programPath("./"), errorMsg;
  if ( !vtksys::SystemTools::FindProgramPath(argv[0], programPath, errorMsg) )
  {
    LOG_ERROR(errorMsg);
  }
  programPath = vtksys::SystemTools::GetParentDirectory(programPath.c_str());

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

  if ( dataCollector->Connect() != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to initialize data collector!");
    exit(EXIT_FAILURE);
  }

  std::vector< vtkPlusChannel* > acqChannels;
  if (acqChannelIds.empty())
  {
    // No channel names have been specified - use them all
    for( DeviceCollectionConstIterator deviceIt = dataCollector->GetDeviceConstIteratorBegin(); deviceIt != dataCollector->GetDeviceConstIteratorEnd(); ++deviceIt )
    {
      for( ChannelContainerConstIterator channelIt = (*deviceIt)->GetOutputChannelsStart(); channelIt != (*deviceIt)->GetOutputChannelsEnd(); ++channelIt )
      {
        if ((*channelIt)->GetOwnerDevice()==(*deviceIt))
        {
          // only add original channels (don't add mirrored output channels)
          acqChannels.push_back(*channelIt);
        }
      }
    }
  }
  else
  {
    for (std::vector<std::string>::iterator acqChannelIdIt=acqChannelIds.begin(); acqChannelIdIt!=acqChannelIds.end(); ++acqChannelIdIt)
    {
      vtkPlusChannel* channel=NULL;
      dataCollector->GetChannel(channel, *acqChannelIdIt);
      if (channel==NULL)
      {
        LOG_ERROR("Channel not found: " << (*acqChannelIdIt));
        exit(EXIT_FAILURE);
      }
      acqChannels.push_back(channel);
    }
  }

  // Enable timestamp reporting for video and all tools
  for (std::vector< vtkPlusChannel* >::iterator acqChannelIt=acqChannels.begin(); acqChannelIt!=acqChannels.end(); ++acqChannelIt)
  {
    vtkPlusDataSource* videoSource=NULL;
    if( (*acqChannelIt)->GetVideoSource(videoSource) == PLUS_SUCCESS && videoSource != NULL )
    {
      videoSource->SetTimeStampReporting(true);
    }
    for (DataSourceContainerConstIterator it = (*acqChannelIt)->GetToolsStartIterator(); it != (*acqChannelIt)->GetToolsEndIterator(); ++it)
    {
      vtkPlusDataSource* tool = it->second;
      tool->SetTimeStampReporting(true);
    }
  }

  if ( dataCollector->Start() != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to start data collection!");
    exit(EXIT_FAILURE);
  }

  const double acqStartTime = vtkTimerLog::GetUniversalTime();

  // Record data
  while ( acqStartTime + inputAcqTimeLength > vtkTimerLog::GetUniversalTime() )
  {
    LOG_INFO( acqStartTime + inputAcqTimeLength - vtkTimerLog::GetUniversalTime() << " seconds left..." );
    vtksys::SystemTools::Delay(1000);
  }

  // Stop recording
  if ( dataCollector->Stop() != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to stop data collection!");
    exit(EXIT_FAILURE);
  }

  // Print statistics

  vtkSmartPointer<vtkHTMLGenerator> htmlReport = vtkSmartPointer<vtkHTMLGenerator>::New();
  htmlReport->SetBaseFilename("DataCollectionReport");
  htmlReport->SetTitle("Data Collection Report");
  for (std::vector< vtkPlusChannel* >::iterator acqChannelIt=acqChannels.begin(); acqChannelIt!=acqChannels.end(); ++acqChannelIt)
  {
    LOG_INFO("---------------------------------");
    LOG_INFO("Device: "<<(*acqChannelIt)->GetOwnerDevice()->GetDeviceId());
    LOG_INFO("Channel: "<<(*acqChannelIt)->GetChannelId());

    // Video data
    vtkPlusDataSource* videoSource=NULL;
    if( (*acqChannelIt)->GetVideoSource(videoSource) == PLUS_SUCCESS && videoSource != NULL )
    {
      double realVideoFramePeriodStdevSec=0;
      double realVideoFrameRate = videoSource->GetFrameRate(false, &realVideoFramePeriodStdevSec);
      double idealVideoFrameRate = videoSource->GetFrameRate(true);
      int numOfItems = videoSource->GetNumberOfItems();
      int bufferSize = videoSource->GetBufferSize();

      LOG_INFO("Nominal video frame rate: " << idealVideoFrameRate << "fps");
      LOG_INFO("Actual video frame rate: " << realVideoFrameRate << "fps (frame period stdev: "<<realVideoFramePeriodStdevSec*1000.0<<"ms)");
      LOG_INFO("Number of items in the video buffer: " << numOfItems );
      LOG_INFO("Video buffer size: " << bufferSize);

      // Check if the same item index (usually "frame number") is stored in multiple items. It may mean too frequent data reading from a tracking device
      int numberOfNonUniqueFrames=0;
      int numberOfValidFrames=0;
      for ( BufferItemUidType frameUid = videoSource->GetOldestItemUidInBuffer(); frameUid <= videoSource->GetLatestItemUidInBuffer(); ++frameUid )
      {
        double time(0);
        if ( videoSource->GetTimeStamp(frameUid, time) != ITEM_OK ) { continue; }
        unsigned long framenum(0);
        if ( videoSource->GetIndex(frameUid, framenum) != ITEM_OK) { continue; }
        numberOfValidFrames++;
        if (frameUid == videoSource->GetOldestItemUidInBuffer())
        {
          // no previous frame
          continue;
        }
        double prevtime(0);
        if ( videoSource->GetTimeStamp(frameUid - 1, prevtime) != ITEM_OK ) { continue; }
        unsigned long prevframenum(0);
        if ( videoSource->GetIndex(frameUid - 1, prevframenum) != ITEM_OK) { continue; }
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

      // Dump video buffer to file
      std::string outputVideoBufferSequenceFileName = vtkPlusConfig::GetInstance()->GetOutputPath(outputSequenceFileNamePrefix
        + "-" + (*acqChannelIt)->GetChannelId() + "-" + videoSource->GetSourceId() + ".mha" );
      LOG_INFO("Write video buffer to " << outputVideoBufferSequenceFileName);
      videoSource->WriteToMetafile( outputVideoBufferSequenceFileName.c_str(), false);
    }

    // Tracker tools
    for (DataSourceContainerConstIterator it = (*acqChannelIt)->GetToolsStartIterator(); it != (*acqChannelIt)->GetToolsEndIterator(); ++it)
    {
      vtkPlusDataSource* tool = it->second;

      int numOfItems = tool->GetNumberOfItems();
      int bufferSize = tool->GetBufferSize();
      double realFramePeriodStdevSec=0;
      double realFrameRate = tool->GetFrameRate(false, &realFramePeriodStdevSec);
      double idealFrameRate = tool->GetFrameRate(true);
      LOG_INFO("------------------ " << tool->GetSourceId() << " ---------------------");
      LOG_INFO("Tracker tool " << tool->GetSourceId() <<  " actual sampling frequency: " << realFrameRate << "fps (sampling period stdev: "<<realFramePeriodStdevSec*1000.0<<"ms)");
      LOG_INFO("Tracker tool " << tool->GetSourceId() <<  " nominal sampling frequency: " << idealFrameRate << "fps");
      LOG_INFO("Number of items in the tool buffer: " << numOfItems );
      LOG_INFO("Tool buffer size: " << bufferSize);

      // Check if the same item index (usually "frame number") is stored in multiple items. It may mean too frequent data reading from a tracking device
      int numberOfNonUniqueFrames=0;
      int numberOfValidFrames=0;
      for ( BufferItemUidType frameUid = tool->GetOldestItemUidInBuffer(); frameUid <= tool->GetLatestItemUidInBuffer(); ++frameUid )
      {
        double time(0);
        if ( tool->GetTimeStamp(frameUid, time) != ITEM_OK ) { continue; }
        unsigned long framenum(0);
        if ( tool->GetIndex(frameUid, framenum) != ITEM_OK) { continue; }
        numberOfValidFrames++;
        if (frameUid == tool->GetOldestItemUidInBuffer())
        {
          // no previous frame
          continue;
        }
        double prevtime(0);
        if ( tool->GetTimeStamp(frameUid - 1, prevtime) != ITEM_OK ) { continue; }
        unsigned long prevframenum(0);
        if ( tool->GetIndex(frameUid - 1, prevframenum) != ITEM_OK) { continue; }
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
      // Dump tracker tool buffer to file
      std::string outputTrackerBufferSequenceFileName = vtkPlusConfig::GetInstance()->GetOutputPath(outputSequenceFileNamePrefix
        + "-" + (*acqChannelIt)->GetChannelId() + "-" + tool->GetSourceId() + ".mha" );
      LOG_INFO("Write tracker buffer to " << outputTrackerBufferSequenceFileName);
      tool->WriteToMetafile( outputTrackerBufferSequenceFileName.c_str(), false);
    }

    // Add info to data acq report
//    htmlReport->AddText(std::string(std::string("Channel: ")+(*acqChannelIt)->GetChannelId()).c_str(), vtkHTMLGenerator::H1);
    (*acqChannelIt)->GenerateDataAcquisitionReport(htmlReport);
  }

  htmlReport->SaveHtmlPageAutoFilename();

  dataCollector->Disconnect();

  return EXIT_SUCCESS;
}

