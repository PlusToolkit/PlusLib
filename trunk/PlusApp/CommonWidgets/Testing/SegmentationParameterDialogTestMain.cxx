/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/ 

#include "vtksys/CommandLineArguments.hxx" 
#include "vtksys/SystemTools.hxx"

#include <QtGui/QApplication>
#include "SegmentationParameterDialogTest.h"

int main(int argc, char *argv[])
{
  // Parse and use command line arguments
  if (argc > 1)
  {
	  std::string deviceSetConfigurationDirectoryPath;
	  std::string inputConfigFileName;
	  std::string imageDirectoryPath;

	  int verboseLevel = -1;

	  vtksys::CommandLineArguments cmdargs;
	  cmdargs.Initialize(argc, argv);

	  cmdargs.AddArgument("--device-set-configuration-directory-path", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &deviceSetConfigurationDirectoryPath, "Device set configuration directory path");
	  cmdargs.AddArgument("--input-config-file-name", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputConfigFileName, "Configuration file name");
	  cmdargs.AddArgument("--image-directory-path", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &imageDirectoryPath, "Image directory path");	

	  cmdargs.AddArgument("--verbose", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &verboseLevel, "Verbose level (1=error only, 2=warning, 3=info, 4=debug)");	

	  if ( !cmdargs.Parse() )
	  {
		  std::cerr << "Problem parsing arguments" << std::endl;
		  std::cout << "Help: " << cmdargs.GetHelp() << std::endl;
		  exit(EXIT_FAILURE);
	  }

    vtkPlusConfig::GetInstance()->SetDeviceSetConfigurationDirectory(deviceSetConfigurationDirectoryPath.c_str());
    vtkPlusConfig::GetInstance()->SetDeviceSetConfigurationFileName(inputConfigFileName.c_str());
    vtkPlusConfig::GetInstance()->SetImageDirectory(imageDirectoryPath.c_str());

    if (verboseLevel > -1)
    {
  	  vtkPlusLogger::Instance()->SetLogLevel(verboseLevel);
    }
  }

  // Start the application
  QApplication app(argc, argv);

	SegmentationParameterDialogTest segmentationParameterDialogTest;
	segmentationParameterDialogTest.show();

	return app.exec();
}
