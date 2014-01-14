/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/ 

#include "vtksys/CommandLineArguments.hxx" 
#include "vtksys/SystemTools.hxx"

#include <QtGui/QApplication>
#include <QFile>
#include "SegmentationParameterDialogTest.h"

#include "vtkXMLUtilities.h"

PlusStatus VerifySavedConfigurationFile();

int main(int argc, char *argv[])
{
  // Parse and use command line arguments
  bool verifySavedConfigurationFile = false;

  if (argc > 1)
  {
	  std::string deviceSetConfigurationDirectoryPath;
	  std::string inputConfigFileName;
	  std::string imageDirectoryPath;

	  int verboseLevel = -1;

	  vtksys::CommandLineArguments cmdargs;
	  cmdargs.Initialize(argc, argv);

	  cmdargs.AddArgument("--device-set-configuration-dir", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &deviceSetConfigurationDirectoryPath, "Device set configuration directory path");
	  cmdargs.AddArgument("--config-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputConfigFileName, "Configuration file name");
	  cmdargs.AddArgument("--image-dir", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &imageDirectoryPath, "Image directory path");	
	  cmdargs.AddBooleanArgument("--verify-saved-configuration-file", &verifySavedConfigurationFile, "Verify saved configuration file if this parameter is set");	

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

  // Delete already existing result configuration file so that no overwrite confirmation is needed during the automated GUI test
  if (verifySavedConfigurationFile)
  {
    std::string resultConfigFileName = vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationPath("TEST_ConfigFileSaverDialogTest_Result.xml");
    QFile::remove(resultConfigFileName.c_str());
  }

  // Start the application
  QApplication app(argc, argv);

  SegmentationParameterDialogTest segmentationParameterDialogTest;

  // If verification is needed then set the file name to the tester
  if (verifySavedConfigurationFile)
  {
    std::string resultConfigFileName = vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationPath("TEST_ConfigFileSaverDialogTest_Result.xml");
    segmentationParameterDialogTest.SetSavedConfigurationFileVerification(resultConfigFileName);
  }

	segmentationParameterDialogTest.show();

	return app.exec();
}
