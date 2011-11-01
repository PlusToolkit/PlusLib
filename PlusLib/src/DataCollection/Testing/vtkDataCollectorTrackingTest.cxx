/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

/*!
  \file This a simple test program to acquire a tracking data and write to a metafile
*/ 

#include "PlusConfigure.h"
#include "vtksys/CommandLineArguments.hxx"
#include "vtkSmartPointer.h"
#include "vtkDataCollector.h"
#include "vtkTrackerBuffer.h"
#include "vtkXMLUtilities.h"
#include "vtkTimerLog.h"
#include "vtksys/SystemTools.hxx"
#include "vtkTracker.h"
#include "vtkTrackerTool.h"

int main(int argc, char **argv)
{
	std::string inputConfigFileName;
  std::string inputToolName; 
	double inputAcqTimeLength(60);
	std::string outputTrackerBufferSequenceFileName("TrackerBufferMetafile"); 
	std::string outputFolder("./");

	int verboseLevel=vtkPlusLogger::LOG_LEVEL_DEFAULT;

	vtksys::CommandLineArguments args;
	args.Initialize(argc, argv);

	args.AddArgument("--input-config-file-name", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputConfigFileName, "Name of the input configuration file.");
	args.AddArgument("--input-tool-name", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputToolName, "Will print the actual transform of this tool (names were defined in the config file, default is the first active tool)");	
  args.AddArgument("--input-acq-time-length", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputAcqTimeLength, "Length of acquisition time in seconds (Default: 60s)");	
	args.AddArgument("--output-tracker-buffer-seq-file-name", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &outputTrackerBufferSequenceFileName, "Filename of the output tracker bufffer sequence metafile (Default: TrackerBufferMetafile)");
	args.AddArgument("--output-folder", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &outputFolder, "Output folder (Default: ./)");
	args.AddArgument("--verbose", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &verboseLevel, "Verbose level (1=error only, 2=warning, 3=info, 4=debug, 5=trace)");	

	vtkPlusLogger::Instance()->SetLogLevel(verboseLevel);

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

  vtkSmartPointer<vtkXMLDataElement> configRootElement = vtkSmartPointer<vtkXMLDataElement>::Take(
    vtkXMLUtilities::ReadElementFromFile(inputConfigFileName.c_str()));
  if (configRootElement == NULL)
  {	
    std::cerr << "Unable to read configuration from file " << inputConfigFileName.c_str()<< std::endl;
		exit(EXIT_FAILURE);
  }

	vtkSmartPointer<vtkDataCollector> dataCollector = vtkSmartPointer<vtkDataCollector>::New(); 
  dataCollector->ReadConfiguration( configRootElement );
	dataCollector->Connect(); 
	dataCollector->Start();

	const double acqStartTime = vtkTimerLog::GetUniversalTime(); 

  if ( !dataCollector->GetTrackingEnabled() )
  {
    LOG_ERROR("Tracking is not enabled!"); 
    return EXIT_FAILURE; 
  }

  int portNumber = dataCollector->GetTracker()->GetToolPortByName(inputToolName.c_str());
  if ( portNumber < 0 )
  {
    if ( dataCollector->GetTracker()->GetFirstActiveTool(portNumber) != PLUS_SUCCESS )
    {
      LOG_ERROR("There is no active tool!"); 
      return EXIT_FAILURE; 
    }
  }

  vtkTrackerTool* tool = dataCollector->GetTracker()->GetTool(portNumber); 

  if ( tool == NULL )
  {
    LOG_ERROR("Tool is not exist anymore!"); 
    return EXIT_FAILURE;
  }

  TrackerBufferItem bufferItem; 
  vtkSmartPointer<vtkMatrix4x4> matrix = vtkSmartPointer<vtkMatrix4x4>::New(); 

	while ( acqStartTime + inputAcqTimeLength > vtkTimerLog::GetUniversalTime() )
	{
    
    tool->GetBuffer()->GetLatestTrackerBufferItem(&bufferItem); 
    if ( bufferItem.GetMatrix(matrix) != PLUS_SUCCESS )
    {
      LOG_ERROR("Failed to get matrix from buffer item!"); 
      continue; 
    }

    std::string transformParameters = PlusMath::GetTransformParametersString(matrix); 
    std::string status = vtkTracker::ConvertTrackerStatusToString(bufferItem.GetStatus()); 

    std::ostringstream message;
    message << "Tool name: " << tool->GetToolName() << "Transform:  "; 
    for ( int r = 0; r < 4; r++ )
    {
      for ( int c = 0; c < 4; c++ ) 
      {
        message << "  " << std::fixed << std::setprecision(5) << std::setw(8) << std::setfill(' ') << matrix->GetElement(r,c); 
      }
      message << "    "; 
    }
    message << "  Status: " << status; 


    LOG_INFO( message.str() ); 
		vtksys::SystemTools::Delay(200); 
	}


	LOG_INFO("Copy tracker..."); 
	vtkTracker *tracker = vtkTracker::New(); 
	dataCollector->CopyTracker(tracker); 

	LOG_INFO("Write tracker to " << outputTrackerBufferSequenceFileName);
	dataCollector->WriteTrackerToMetafile( tracker, outputFolder.c_str(), outputTrackerBufferSequenceFileName.c_str(), true); 

	tracker->Delete(); 

	std::cout << "Test completed successfully!" << std::endl;
	return EXIT_SUCCESS; 

}

