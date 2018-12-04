/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"
#include "vtkCallbackCommand.h"
#if WIN32
#include "vtkPlusNvidiaDVPVideoSourceWin32.h"
#elif __linux__
#include "vtkPlusNvidiaDVPVideoSourceLinux.h"
#endif
#include "vtkSmartPointer.h"
#include "vtksys/CommandLineArguments.hxx"

void PrintLogsCallback( vtkObject* obj, unsigned long eid, void* clientdata, void* calldata );

int main( int argc, char** argv )
{
  bool printHelp( false );
  bool renderingOff( false );

  vtksys::CommandLineArguments args;
  args.Initialize( argc, argv );

  int verboseLevel = vtkIGSIOLogger::LOG_LEVEL_UNDEFINED;

  args.AddArgument( "--help", vtksys::CommandLineArguments::NO_ARGUMENT, &printHelp, "Print this help." );
  args.AddArgument( "--rendering-off", vtksys::CommandLineArguments::NO_ARGUMENT, &renderingOff, "Run test without rendering." );
  args.AddArgument( "--verbose", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &verboseLevel, "Verbose level (1=error only, 2=warning, 3=info, 4=debug, 5=trace)" );

  if ( !args.Parse() )
  {
    std::cerr << "Problem parsing arguments" << std::endl;
    std::cout << "\n\nvtkPlusNvidiaDVPVideoSourceTest help:" << args.GetHelp() << std::endl;
    exit( EXIT_FAILURE );
  }

  vtkIGSIOLogger::Instance()->SetLogLevel( verboseLevel );

  if ( printHelp )
  {
    std::cout << "\n\nvtkPlusNvidiaDVPVideoSourceTest help:" << args.GetHelp() << std::endl;
    exit( EXIT_SUCCESS );

  }

  vtkSmartPointer<vtkPlusNvidiaDVPVideoSource> dvpDevice = vtkSmartPointer<vtkPlusNvidiaDVPVideoSource>::New();

  // Add an observer to warning and error events for redirecting it to the stdout
  vtkSmartPointer<vtkCallbackCommand> callbackCommand = vtkSmartPointer<vtkCallbackCommand>::New();
  callbackCommand->SetCallback( PrintLogsCallback );
  dvpDevice->AddObserver( "WarningEvent", callbackCommand );
  dvpDevice->AddObserver( "ErrorEvent", callbackCommand );

  LOG_INFO( "Initialize..." );
  dvpDevice->Connect();

  if ( dvpDevice->GetConnected() )
  {
    LOG_INFO( "Start recording..." );
    dvpDevice->StartRecording();
  }
  else
  {
    dvpDevice->Disconnect();
    LOG_ERROR( "Unable to connect to IC capture device" );
    exit( EXIT_FAILURE );
  }

  // TODO : make some graphics API specific calls to test data acquisition

  dvpDevice->StopRecording();
  dvpDevice->Disconnect();
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
