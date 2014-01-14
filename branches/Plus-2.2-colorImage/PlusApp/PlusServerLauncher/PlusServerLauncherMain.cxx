/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/ 

#include "vtksys/CommandLineArguments.hxx" 
#include "vtksys/SystemTools.hxx"

#include <QtGui/QApplication>
#include "PlusServerLauncherMainWindow.h"

#include "vtkXMLUtilities.h"

int main(int argc, char *argv[])
{
  std::string deviceSetConfigurationDirectoryPath;
  std::string inputConfigFileName;
  bool autoConnect=false;
#ifdef _WIN32
  bool showConsole=false;
#endif

  if (argc > 1)
  {
    int verboseLevel = -1;

	  vtksys::CommandLineArguments cmdargs;
	  cmdargs.Initialize(argc, argv);

	  cmdargs.AddArgument("--device-set-configuration-dir", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &deviceSetConfigurationDirectoryPath, "Device set configuration directory path");
	  cmdargs.AddArgument("--config-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputConfigFileName, "Configuration file name");
    cmdargs.AddBooleanArgument("--connect", &autoConnect, "Automatically connect after the application is started");
#ifdef _WIN32
    cmdargs.AddBooleanArgument("--show-console", &showConsole, "Show console with log messages");	 
#endif
	  cmdargs.AddArgument("--verbose", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &verboseLevel, "Verbose level (1=error only, 2=warning, 3=info, 4=debug)");	

	  if ( !cmdargs.Parse() )
	  {
		  std::cerr << "Problem parsing arguments" << std::endl;
		  std::cout << "Help: " << cmdargs.GetHelp() << std::endl;
		  exit(EXIT_FAILURE);
	  }

    if (!deviceSetConfigurationDirectoryPath.empty())
    {
      vtkPlusConfig::GetInstance()->SetDeviceSetConfigurationDirectory(deviceSetConfigurationDirectoryPath.c_str());
    }
    if (!inputConfigFileName.empty())
    {
      vtkPlusConfig::GetInstance()->SetDeviceSetConfigurationFileName(inputConfigFileName.c_str());
    }

    if (verboseLevel > -1)
    {
  	  vtkPlusLogger::Instance()->SetLogLevel(verboseLevel);
    }
  }

  // Start the application
  QApplication app(argc, argv);
  
#ifdef _WIN32
  // Hide console window - we can see log messages using the status icon anyway
  if (!showConsole)
  {
    HWND hwnd = GetConsoleWindow();
    ShowWindow(hwnd, 0);
  }
#endif

  PlusServerLauncherMainWindow PlusServerLauncherMainWindow(0,0,autoConnect);
	PlusServerLauncherMainWindow.show();

	int retValue=app.exec();

  #ifdef _WIN32
  // Restore the console window
  if (!showConsole)
  {
    HWND hwnd = GetConsoleWindow();
    ShowWindow(hwnd, 1);
  }
  #endif

  return retValue;
}
