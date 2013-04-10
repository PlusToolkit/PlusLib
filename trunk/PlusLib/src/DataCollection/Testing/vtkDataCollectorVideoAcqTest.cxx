/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

/*!
  \file vtkDataCollectorVideoAcqTest.cxx 
  \brief This a simple test program to acquire a video sequence and write to a metafile
*/ 

#include "PlusConfigure.h"
#include "vtkDataCollector.h"
#include "vtkPlusChannel.h"
#include "vtkPlusDataSource.h"
#include "vtkPlusDevice.h"
#include "vtkPlusBuffer.h"
#include "vtkSmartPointer.h"
#include "vtkTimerLog.h"
#include "vtkXMLUtilities.h"
#include "vtksys/CommandLineArguments.hxx"
#include "vtksys/SystemTools.hxx"

int main(int argc, char **argv)
{
	std::string inputConfigFileName;
	double inputAcqTimeLength(60);
	std::string outputVideoBufferSequenceFileName("VideoBufferMetafile"); 

	int verboseLevel=vtkPlusLogger::LOG_LEVEL_UNDEFINED;

	vtksys::CommandLineArguments args;
	args.Initialize(argc, argv);

	args.AddArgument("--config-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputConfigFileName, "Name of the input configuration file.");
	args.AddArgument("--acq-time-length", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputAcqTimeLength, "Length of acquisition time in seconds (Default: 60s)");	
	args.AddArgument("--output-video-buffer-seq-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &outputVideoBufferSequenceFileName, "Filename of the output video bufffer sequence metafile (Default: VideoBufferMetafile)");
	args.AddArgument("--verbose", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &verboseLevel, "Verbose level (1=error only, 2=warning, 3=info, 4=debug, 5=trace)");		

	if ( !args.Parse() )
	{
		std::cerr << "Problem parsing arguments" << std::endl;
		std::cout << "Help: " << args.GetHelp() << std::endl;
		exit(EXIT_FAILURE);
	}
  
  vtkPlusLogger::Instance()->SetLogLevel(verboseLevel);

	if (inputConfigFileName.empty())
	{
		std::cerr << "config-file is required" << std::endl;
		exit(EXIT_FAILURE);
	}

	///////////////

  vtkSmartPointer<vtkXMLDataElement> configRootElement = vtkSmartPointer<vtkXMLDataElement>::Take(
    vtkXMLUtilities::ReadElementFromFile(inputConfigFileName.c_str()));
  if (configRootElement == NULL)
  {	
    std::cerr << "Unable to read configuration from file " << inputConfigFileName.c_str() << std::endl; 
		exit(EXIT_FAILURE);
  }

	vtkSmartPointer<vtkDataCollector> dataCollector = vtkSmartPointer<vtkDataCollector>::New(); 
  dataCollector->ReadConfiguration( configRootElement );
	dataCollector->Connect(); 
	dataCollector->Start();

	const double acqStartTime = vtkTimerLog::GetUniversalTime(); 

	while ( acqStartTime + inputAcqTimeLength > vtkTimerLog::GetUniversalTime() )
	{
		LOG_INFO( acqStartTime + inputAcqTimeLength - vtkTimerLog::GetUniversalTime() << " seconds left..." ); 
		vtksys::SystemTools::Delay(1000); 
	}


	LOG_INFO("Copy video buffer"); 
	vtkPlusBuffer* buffer = vtkPlusBuffer::New(); 
  vtkPlusDevice* device = NULL;
  if( dataCollector->GetDevice(device, "VideoDevice") != PLUS_SUCCESS )
  {
    LOG_ERROR("Unable to locate the device with Id=\"SomeVideoSource\". Check config file.");
    exit(EXIT_FAILURE);
  }
  vtkPlusChannel* aChannel(NULL);
  vtkPlusDataSource* aSource(NULL);
  if( device->GetOutputChannelByName(aChannel, "VideoStream") == PLUS_SUCCESS && aChannel->GetVideoSource(aSource) == PLUS_SUCCESS )
  {
    buffer->DeepCopy(aSource->GetBuffer());

    LOG_INFO("write video buffer to " << outputVideoBufferSequenceFileName);
    buffer->WriteToMetafile(outputVideoBufferSequenceFileName.c_str(), true); 

    buffer->Delete(); 

    std::cout << "Test completed successfully!" << std::endl;
    return EXIT_SUCCESS; 
  }
  else
  {
    LOG_ERROR("Unable to retrieve the video source.");
    exit(EXIT_FAILURE);
  }
}

