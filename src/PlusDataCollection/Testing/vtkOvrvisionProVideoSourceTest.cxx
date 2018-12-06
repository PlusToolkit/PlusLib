/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

// Local includes
#include "PlusConfigure.h"
#include "vtkPlusOvrvisionProVideoSource.h"
#include "vtkPlusDataSource.h"

// VTK includes
#include <vtkCallbackCommand.h>
#include <vtkSmartPointer.h>
#include <vtksys/CommandLineArguments.hxx>

void PrintLogsCallback( vtkObject* obj, unsigned long eid, void* clientdata, void* calldata );

int main( int argc, char** argv )
{
  bool printHelp( false );
  std::string inputConfigFileName;
  bool renderingOff( false );

  vtksys::CommandLineArguments args;
  args.Initialize( argc, argv );

  int verboseLevel = vtkPlusLogger::LOG_LEVEL_UNDEFINED;

  args.AddArgument( "--help", vtksys::CommandLineArguments::NO_ARGUMENT, &printHelp, "Print this help." );
  args.AddArgument("--config-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputConfigFileName, "Config file containing the device configuration.");
  args.AddArgument( "--verbose", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &verboseLevel, "Verbose level (1=error only, 2=warning, 3=info, 4=debug, 5=trace)" );

  if ( !args.Parse() )
  {
    std::cerr << "Problem parsing arguments" << std::endl;
    std::cout << "\n\nvtkPlusOvrvisionProVideoSourceTest help:" << args.GetHelp() << std::endl;
    exit( EXIT_FAILURE );
  }

  vtkPlusLogger::Instance()->SetLogLevel( verboseLevel );

  if ( printHelp )
  {
    std::cout << "\n\nvtkPlusOvrvisionProVideoSourceTest help:" << args.GetHelp() << std::endl;
    exit( EXIT_SUCCESS );
  }

  // Read config file
  LOG_DEBUG("Reading config file...");
  vtkSmartPointer<vtkXMLDataElement> configRootElement = vtkSmartPointer<vtkXMLDataElement>::New();
  if (PlusXmlUtils::ReadDeviceSetConfigurationFromFile(configRootElement, inputConfigFileName.c_str()) == PLUS_FAIL)
  {
    LOG_ERROR("Unable to read configuration from file " << inputConfigFileName.c_str());
    return EXIT_FAILURE;
  }

  vtkSmartPointer<vtkPlusOvrvisionProVideoSource> device = vtkSmartPointer<vtkPlusOvrvisionProVideoSource>::New();
  device->SetDeviceId("OvrvisionPro");

  // Add an observer to warning and error events for redirecting it to the stdout
  vtkSmartPointer<vtkCallbackCommand> callbackCommand = vtkSmartPointer<vtkCallbackCommand>::New();
  callbackCommand->SetCallback( PrintLogsCallback );
  device->AddObserver( "WarningEvent", callbackCommand );
  device->AddObserver( "ErrorEvent", callbackCommand );

  device->ReadConfiguration(configRootElement);

  LOG_INFO( "Initialize..." );
  device->Connect();

  Sleep( 1000 ); // Wait a bit

  auto channel = *device->GetOutputChannelsStart();
  if ( channel == nullptr )
  {
    LOG_ERROR("No channel configured for device.");
    exit(EXIT_FAILURE);
  }

  igsioTrackedFrame frame;
  if (channel->GetTrackedFrame(frame) != PLUS_SUCCESS)
  {
    LOG_ERROR("Unable to retrieve frame from device.");
    exit(EXIT_FAILURE);
  }

  // TODO : some sort of image validation?

  device->Disconnect();
  LOG_INFO( "Exit successfully" );
  exit( EXIT_SUCCESS );
}

// Callback function for error and warning redirects
void PrintLogsCallback( vtkObject* obj, unsigned long eid, void* clientdata, void* calldata )
{
  if ( eid == vtkCommand::GetEventIdFromString( "WarningEvent" ) )
  {
    LOG_WARNING( ( const char* )calldata );
  }
  else if ( eid == vtkCommand::GetEventIdFromString( "ErrorEvent" ) )
  {
    LOG_ERROR( ( const char* )calldata );
  }
}
