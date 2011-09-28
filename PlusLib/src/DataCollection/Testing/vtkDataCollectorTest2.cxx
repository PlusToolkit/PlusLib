#include "PlusConfigure.h"
#include "vtksys/CommandLineArguments.hxx"
#include "vtkSmartPointer.h"
#include "vtkDataCollector.h"
#include "vtkTracker.h"
#include "vtkVideoBuffer.h"
#include "vtkSavedDataTracker.h"
#include "vtkSavedDataVideoSource.h"
#include "vtkXMLUtilities.h"

int main(int argc, char **argv)
{
  int numberOfFailures(0); 
  std::string inputConfigFileName;
  double inputAcqTimeLength(20);
  std::string outputFolder("./"); 
  std::string outputTrackerBufferSequenceFileName("TrackerBufferMetafile"); 
  std::string outputVideoBufferSequenceFileName("VideoBufferMetafile"); 
  std::string inputVideoBufferMetafile;
  std::string inputTrackerBufferMetafile;
  bool outputCompressed(true);

  int verboseLevel=vtkPlusLogger::LOG_LEVEL_INFO;

  vtksys::CommandLineArguments args;
  args.Initialize(argc, argv);

  args.AddArgument("--input-config-file-name", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputConfigFileName, "Name of the input configuration file.");
  args.AddArgument("--input-acq-time-length", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputAcqTimeLength, "Length of acquisition time in seconds (Default: 20s)");	
  args.AddArgument("--input-video-buffer-metafile", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputVideoBufferMetafile, "Video buffer sequence metafile.");
  args.AddArgument("--input-tracker-buffer-metafile", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputTrackerBufferMetafile, "Tracker buffer sequence metafile.");
  args.AddArgument("--output-tracker-buffer-seq-file-name", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &outputTrackerBufferSequenceFileName, "Filename of the output tracker buffer sequence metafile (Default: TrackerBufferMetafile)");
  args.AddArgument("--output-video-buffer-seq-file-name", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &outputVideoBufferSequenceFileName, "Filename of the output video buffer sequence metafile (Default: VideoBufferMetafile)");
  args.AddArgument("--output-folder", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &outputFolder, "Output folder (Default: ./)");
  args.AddArgument("--output-compressed", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &outputCompressed, "Compressed output (0=non-compressed, 1=compressed, default:compressed)");	
  args.AddArgument("--verbose", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &verboseLevel, "Verbose level (1=error only, 2=warning, 3=info, 4=debug)");	

  if ( !args.Parse() )
  {
    std::cerr << "Problem parsing arguments" << std::endl;
    std::cout << "Help: " << args.GetHelp() << std::endl;
    exit(EXIT_FAILURE);
  }

  if (inputConfigFileName.empty())
  {
    std::cerr << "input-config-file-name is required" << std::endl;
    exit(EXIT_FAILURE);
  }

  ///////////////

  vtkSmartPointer<vtkXMLDataElement> configRootElement = vtkXMLUtilities::ReadElementFromFile(inputConfigFileName.c_str());
  if (configRootElement == NULL)
  {	
    std::cerr << "Unable to read configuration from file " << inputConfigFileName.c_str() << std::endl;
    exit(EXIT_FAILURE);
  }

  vtkSmartPointer<vtkDataCollector> dataCollector = vtkSmartPointer<vtkDataCollector>::New(); 
  dataCollector->ReadConfiguration( configRootElement );

  vtkPlusLogger::Instance()->SetLogLevel(verboseLevel);
  vtkPlusLogger::Instance()->SetDisplayLogLevel(verboseLevel);

  if ( dataCollector->GetAcquisitionType() == SYNCHRO_VIDEO_SAVEDDATASET )
  {
    vtkSavedDataVideoSource* videoSource = static_cast<vtkSavedDataVideoSource*>(dataCollector->GetVideoSource()); 
    videoSource->SetSequenceMetafile(inputVideoBufferMetafile.c_str()); 
  }

  if ( dataCollector->GetTrackerType() == TRACKER_SAVEDDATASET )
  {
    vtkSavedDataTracker* tracker = static_cast<vtkSavedDataTracker*>(dataCollector->GetTracker()); 
    tracker->SetSequenceMetafile(inputTrackerBufferMetafile.c_str()); 
  }

  dataCollector->Initialize(); 
  dataCollector->Start();

  const double acqStartTime = vtkTimerLog::GetUniversalTime(); 

  while ( acqStartTime + inputAcqTimeLength > vtkTimerLog::GetUniversalTime() )
  {
    LOG_INFO( acqStartTime + inputAcqTimeLength - vtkTimerLog::GetUniversalTime() << " seconds left..." ); 
    vtksys::SystemTools::Delay(1000); 
  }

  vtkSmartPointer<vtkVideoBuffer> videobuffer = vtkSmartPointer<vtkVideoBuffer>::New(); 
  if ( dataCollector->GetVideoSource() != NULL ) 
  {
    LOG_INFO("Copy video buffer"); 
    dataCollector->CopyVideoBuffer(videobuffer); 
  }

  vtkSmartPointer<vtkTracker> tracker = vtkSmartPointer<vtkTracker>::New(); 
  if ( dataCollector->GetTracker() != NULL )
  {
    LOG_INFO("Copy tracker"); 
    dataCollector->CopyTracker(tracker); 
  }

  if ( dataCollector->GetVideoSource() != NULL ) 
  {
    LOG_INFO("Write video buffer to " << outputVideoBufferSequenceFileName);
    vtkDataCollector::WriteVideoBufferToMetafile( videobuffer, outputFolder.c_str(), outputVideoBufferSequenceFileName.c_str(), outputCompressed); 
  }

  if ( dataCollector->GetTracker() != NULL )
  {
    LOG_INFO("Write tracker buffer to " << outputTrackerBufferSequenceFileName);
    vtkDataCollector::WriteTrackerToMetafile( tracker, outputFolder.c_str(), outputTrackerBufferSequenceFileName.c_str(), outputCompressed); 
  }


  if ( dataCollector->GetVideoSource() != NULL )
  {
    std::ostringstream filepath; 
    filepath << outputFolder << "/" << outputVideoBufferSequenceFileName << ".mha"; 

    if ( vtksys::SystemTools::FileExists(filepath.str().c_str(), true) )
    {
      LOG_INFO("Remove generated video metafile!"); 
      if ( !vtksys::SystemTools::RemoveFile(filepath.str().c_str()) )
      {
        LOG_ERROR("Unable to remove generated video buffer: " << filepath.str() ); 
        numberOfFailures++; 
      }
    }
    else
    {
      LOG_ERROR("Unable to find video buffer at: " << filepath.str() ); 
      numberOfFailures++; 
    }
  }

  if ( dataCollector->GetTracker() != NULL )
  {
    std::ostringstream filepath; 
    filepath << outputFolder << "/" << outputTrackerBufferSequenceFileName << ".mha"; 

    if ( vtksys::SystemTools::FileExists(filepath.str().c_str(), true) )
    {
      LOG_INFO("Remove generated tracker metafile!"); 
      if ( !vtksys::SystemTools::RemoveFile(filepath.str().c_str()) )
      {
        LOG_ERROR("Unable to remove generated tracker buffer: " << filepath.str() ); 
        numberOfFailures++; 
      }
    }
    else
    {
      LOG_ERROR("Unable to find tracker buffer at: " << filepath.str() ); 
      numberOfFailures++; 
    }
  }

  if ( dataCollector->Stop() != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to stop data collection!"); 
    numberOfFailures++; 
  }

  if ( numberOfFailures > 0 ) 
  {
    LOG_ERROR("Number of failures: " << numberOfFailures ); 
    return EXIT_FAILURE; 
  }

  std::cout << "Test completed successfully!" << std::endl;
  return EXIT_SUCCESS; 

}

