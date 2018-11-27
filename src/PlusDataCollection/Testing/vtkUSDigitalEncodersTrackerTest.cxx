/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

/*!
   \file vtkUSDigitalEncodersTrackerTest.cxx
   \brief This a simple test program to acquire tracking data.
 */

#include "PlusConfigure.h"
#include "vtkCommand.h"
#include "vtkPlusDataCollector.h"
#include "vtkMatrix4x4.h"
#include "vtkPlusChannel.h"
#include "vtkPlusDataSource.h"
#include "vtkPlusDevice.h"
#include "vtkSmartPointer.h"
#include "vtkPlusUSDigitalEncodersTracker.h"
#include "vtkXMLUtilities.h"
#include "vtksys/CommandLineArguments.hxx"
#include "vtksys/SystemTools.hxx"

int main(int argc, char** argv)
{
  bool printHelp(false);
  std::string inputConfigFileName = "USDigitalEncodersTracker.xml";
  std::string inputToolSourceId;
  double inputAcqTimeLength(60);
  std::string outputTrackerBufferSequenceFileName;
  bool renderingOff(false);

  int verboseLevel = vtkIGSIOLogger::LOG_LEVEL_UNDEFINED;

  vtksys::CommandLineArguments args;
  args.Initialize(argc, argv);

  args.AddArgument("--help", vtksys::CommandLineArguments::NO_ARGUMENT, &printHelp, "Print this help.");
  args.AddArgument("--config-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputConfigFileName, "Name of the input configuration file.");
  args.AddArgument("--tool-name", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputToolSourceId, "Will print the actual transform of this tool (names were defined in the config file, default is the first active tool)");
  args.AddArgument("--acq-time-length", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputAcqTimeLength, "Length of acquisition time in seconds (Default: 60s)");
  args.AddArgument("--output-tracker-buffer-seq-file-name", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &outputTrackerBufferSequenceFileName, "Filename of the output tracker bufffer sequence metafile (Default: TrackerBufferMetafile)");
  args.AddArgument("--rendering-off", vtksys::CommandLineArguments::NO_ARGUMENT, &renderingOff, "Run test without rendering.");
  args.AddArgument("--verbose", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &verboseLevel, "Verbose level (1=error only, 2=warning, 3=info, 4=debug, 5=trace)");

  if(!args.Parse())
  {
    std::cerr << "Problem parsing arguments" << std::endl;
    std::cout << "Help: " << args.GetHelp() << std::endl;
    exit(EXIT_FAILURE);
  }

  if(printHelp)
  {
    std::cout << args.GetHelp() << std::endl;
    exit(EXIT_SUCCESS);
  }

  vtkIGSIOLogger::Instance()->SetLogLevel(verboseLevel);

  if(inputConfigFileName.empty())
  {
    std::cerr << "--config-file is required" << std::endl;
    exit(EXIT_FAILURE);
  }

  ///// Reading configuration file

  vtkSmartPointer< vtkPlusUSDigitalEncodersTracker > usDigitalEncodersTracker =
    vtkSmartPointer< vtkPlusUSDigitalEncodersTracker >::New();

  vtkSmartPointer<vtkXMLDataElement> configRootElement = vtkSmartPointer<vtkXMLDataElement>::New();
  if(PlusXmlUtils::ReadDeviceSetConfigurationFromFile(configRootElement, inputConfigFileName.c_str()) == PLUS_FAIL)
  {
    LOG_ERROR("Unable to read configuration from file " << inputConfigFileName.c_str());
    return EXIT_FAILURE;
  }

  usDigitalEncodersTracker->ReadConfiguration(configRootElement);

  if(usDigitalEncodersTracker->Connect() != PLUS_SUCCESS)
  {
    LOG_ERROR("Unable to connect to US Digital encoders!");
    exit(EXIT_FAILURE);
  }

  usDigitalEncodersTracker->StartRecording();

  bool bTask = true;
  std::string inputMessage;
  while(bTask)
  {
    std::getline(std::cin, inputMessage);
    std::cout << " Input :: " << inputMessage << std::endl;
    if(inputMessage.find("quit") != std::string::npos)
    {
      bTask = false;
      break;
    }
  }

  usDigitalEncodersTracker->StopRecording();
  usDigitalEncodersTracker->Disconnect();

  std::cout << "Test completed successfully!" << std::endl;
  return EXIT_SUCCESS;
}