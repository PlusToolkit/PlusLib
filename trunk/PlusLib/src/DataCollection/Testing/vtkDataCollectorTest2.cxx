#include "PlusConfigure.h"
#include "vtksys/CommandLineArguments.hxx"
#include "vtkSmartPointer.h"
#include "vtkDataCollector.h"
#include "vtkTracker.h"
#include "vtkVideoBuffer2.h"

int main(int argc, char **argv)
{
	std::string inputConfigFileName;
	double inputAcqTimeLength(60);
	std::string outputFolder("./"); 
	std::string outputTrackerBufferSequenceFileName("TrackerBufferMetafile"); 
	std::string outputVideoBufferSequenceFileName("VideoBufferMetafile"); 
	bool outputCompressed(true);

	int verboseLevel=PlusLogger::LOG_LEVEL_INFO;

	vtksys::CommandLineArguments args;
	args.Initialize(argc, argv);

	args.AddArgument("--input-config-file-name", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputConfigFileName, "Name of the input configuration file.");
	args.AddArgument("--input-acq-time-length", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputAcqTimeLength, "Length of acquisition time in seconds (Default: 60s)");	
	args.AddArgument("--output-tracker-buffer-seq-file-name", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &outputTrackerBufferSequenceFileName, "Filename of the output tracker buffer sequence metafile (Default: TrackerBufferMetafile)");
	args.AddArgument("--output-video-buffer-seq-file-name", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &outputVideoBufferSequenceFileName, "Filename of the output video buffer sequence metafile (Default: VideoBufferMetafile)");
	args.AddArgument("--output-folder", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &outputFolder, "Output folder (Default: ./)");
	args.AddArgument("--output-compressed", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &outputCompressed, "Compressed output (0=non-compressed, 1=compressed, default:compressed)");	
	args.AddArgument("--verbose", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &verboseLevel, "Verbose level (1=error only, 2=warning, 3=info, 4=debug)");	

	PlusLogger::Instance()->SetLogLevel(verboseLevel);

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

	VTK_LOG_TO_CONSOLE_ON; 

	vtkSmartPointer<vtkDataCollector> dataCollector = vtkSmartPointer<vtkDataCollector>::New(); 
	dataCollector->ReadConfiguration(inputConfigFileName.c_str());
	dataCollector->Initialize(); 
	dataCollector->Start();

	const double acqStartTime = vtkTimerLog::GetUniversalTime(); 

	while ( acqStartTime + inputAcqTimeLength > vtkTimerLog::GetUniversalTime() )
	{
		LOG_INFO( acqStartTime + inputAcqTimeLength - vtkTimerLog::GetUniversalTime() << " seconds left..." ); 
		vtksys::SystemTools::Delay(1000); 
	}

	vtkSmartPointer<vtkVideoBuffer2> videobuffer = vtkSmartPointer<vtkVideoBuffer2>::New(); 
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
		LOG_INFO("Dump video buffer to " << outputVideoBufferSequenceFileName);
		dataCollector->DumpVideoBufferToMetafile( videobuffer, outputFolder.c_str(), outputVideoBufferSequenceFileName.c_str(), outputCompressed); 
	}

	if ( dataCollector->GetTracker() != NULL )
	{
		LOG_INFO("Dump tracker buffer to " << outputTrackerBufferSequenceFileName);
		dataCollector->DumpTrackerToMetafile( tracker, outputFolder.c_str(), outputTrackerBufferSequenceFileName.c_str(), outputCompressed); 
	}

	VTK_LOG_TO_CONSOLE_OFF; 

	std::cout << "Test completed successfully!" << std::endl;
	return EXIT_SUCCESS; 

}

