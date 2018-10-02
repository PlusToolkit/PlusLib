/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"
#include "vtkPlusDataCollector.h"
#include "vtkPlusVirtualTextRecognizer.h"
#include "vtksys/CommandLineArguments.hxx"
#include <map>

int main(int argc, char **argv)
{
  bool printHelp(false);
  std::string inputConfigFileName;
  std::string deviceId;
  std::string fieldValue;

  vtksys::CommandLineArguments args;
  args.Initialize(argc, argv);

  int verboseLevel = vtkPlusLogger::LOG_LEVEL_UNDEFINED;

  args.AddArgument("--help", vtksys::CommandLineArguments::NO_ARGUMENT, &printHelp, "Print this help.");
  args.AddArgument("--config-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputConfigFileName, "Config file to test with.");
  args.AddArgument("--device-id", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &deviceId, "Id of the text recognizer device.");
  args.AddArgument("--field-value", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &fieldValue, "Value of the first field.");
  args.AddArgument("--verbose", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &verboseLevel, "Verbose level (1=error only, 2=warning, 3=info, 4=debug, 5=trace)");

  if ( !args.Parse() )
  {
    std::cerr << "Problem parsing arguments" << std::endl;
    std::cout << "\n\nvtkPlusVirtualTextRecognizerTest help:" << args.GetHelp() << std::endl;
    return EXIT_FAILURE;
  }

  vtkPlusLogger::Instance()->SetLogLevel(verboseLevel);

  if ( printHelp )
  {
    std::cout << "\n\nvtkPlusVirtualTextRecognizerTest help:" << args.GetHelp() << std::endl;
    return EXIT_SUCCESS;
  }

  if( !vtksys::SystemTools::FileExists(inputConfigFileName))
  {
    LOG_ERROR("Invalid config file sent to test.");
    return EXIT_FAILURE;
  }

  vtkSmartPointer<vtkXMLDataElement> configRootElement = vtkSmartPointer<vtkXMLDataElement>::New();
  if (PlusXmlUtils::ReadDeviceSetConfigurationFromFile(configRootElement, inputConfigFileName.c_str())==PLUS_FAIL)
  {
    LOG_ERROR("Unable to read configuration from file " << inputConfigFileName.c_str());
    return EXIT_FAILURE;
  }

  vtkPlusConfig::GetInstance()->SetDeviceSetConfigurationData(configRootElement);

  vtkSmartPointer<vtkPlusDataCollector> dataCollector = vtkSmartPointer<vtkPlusDataCollector>::New();

  if( dataCollector->ReadConfiguration( configRootElement ) != PLUS_SUCCESS )
  {
    LOG_ERROR("Configuration incorrect for vtkPlusVirtualTextRecognizerTest.");
    return EXIT_FAILURE;
  }

  if ( dataCollector->Connect() != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to connect to devices!" );
    return EXIT_FAILURE;
  }

  if ( dataCollector->Start() != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to start data collection!" );
    return EXIT_FAILURE;
  }

  vtkPlusDevice* device(NULL);
  if( dataCollector->GetDevice(device, deviceId) != PLUS_SUCCESS )
  {
    LOG_ERROR("Unable to retrieve recognizer device by Id: " << deviceId);
    return EXIT_FAILURE;
  }

  vtkPlusVirtualTextRecognizer* textRecognizer = vtkPlusVirtualTextRecognizer::SafeDownCast(device);

  if( textRecognizer == NULL )
  {
    LOG_ERROR("Unable to retrieve recognizer device by Id: " << deviceId);
    return EXIT_FAILURE;
  }

  textRecognizer->SetMissingInputGracePeriodSec(0);
  
#ifdef _WIN32
  Sleep(500);
#else
  usleep(500000);
#endif 

  vtkPlusVirtualTextRecognizer::ChannelFieldListMap map = textRecognizer->GetRecognitionFields();
  vtkPlusVirtualTextRecognizer::FieldListIterator it = map.begin()->second.begin();
  if( (*it)->LatestParameterValue != fieldValue )
  {
    LOG_ERROR("Direct: Parameter \"" << (*it)->ParameterName << "\" value=\"" << (*it)->LatestParameterValue << "\" does not match expected value=\"" << fieldValue << "\"");
    return EXIT_FAILURE;
  }

  PlusTrackedFrame frame;
  (*device->GetOutputChannelsStart())->GetTrackedFrame(frame);

  if( frame.GetFrameField((*it)->ParameterName) == NULL || STRCASECMP(frame.GetFrameField((*it)->ParameterName), fieldValue.c_str()) != 0 )
  {
    LOG_ERROR("Tracked Frame: Parameter \"" << (*it)->ParameterName << "\" value=\"" << (*it)->LatestParameterValue << "\" does not match expected value=\"" << fieldValue << "\"");
    return EXIT_FAILURE;
  }

  LOG_INFO("Exit successfully");
  return EXIT_SUCCESS;
}