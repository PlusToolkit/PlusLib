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

	  cmdargs.AddArgument("--device-set-configuration-directory-path", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &deviceSetConfigurationDirectoryPath, "Device set configuration directory path");
	  cmdargs.AddArgument("--input-config-file-name", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputConfigFileName, "Configuration file name");
	  cmdargs.AddArgument("--image-directory-path", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &imageDirectoryPath, "Image directory path");	
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
    std::string resultConfigFileName = vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationDirectory();
    resultConfigFileName.append("/TEST_ConfigFileSaverDialogTest_Result.xml");
    QFile::remove(QString::fromStdString(resultConfigFileName));
  }

  // Start the application
  QApplication app(argc, argv);

	SegmentationParameterDialogTest segmentationParameterDialogTest;
	segmentationParameterDialogTest.show();

	int returnValue = app.exec();

  if (verifySavedConfigurationFile)
  {
    if (VerifySavedConfigurationFile() != PLUS_SUCCESS)
    {
      LOG_ERROR("Saved configuration file does not contain the expected values!");
      returnValue = 1;
    }
  }

  return returnValue;
}

//-----------------------------------------------------------------------------

PlusStatus VerifySavedConfigurationFile()
{
  // Assemble result configuration file name
  std::string resultConfigFileName = vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationDirectory();
  resultConfigFileName.append("/TEST_ConfigFileSaverDialogTest_Result.xml");

	// Load result configuration file
  vtkSmartPointer<vtkXMLDataElement> resultRootElement = vtkXMLUtilities::ReadElementFromFile(resultConfigFileName.c_str());
	if (resultRootElement == NULL) {	
		LOG_ERROR("Unable to read the result configuration file: " << resultConfigFileName); 
		return PLUS_FAIL;
	}

  // Find Device set element
	vtkSmartPointer<vtkXMLDataElement> usDataCollection = resultRootElement->FindNestedElementWithName("USDataCollection");
	if (usDataCollection == NULL) {
		LOG_ERROR("No USDataCollection element is found in the XML tree!");
		return PLUS_FAIL;
	}

	vtkSmartPointer<vtkXMLDataElement> deviceSet = usDataCollection->FindNestedElementWithName("DeviceSet");
	if (deviceSet == NULL) {
		LOG_ERROR("No DeviceSet element is found in the XML tree!");
		return PLUS_FAIL;
	}

  // Get name and description
  const char* name = deviceSet->GetAttribute("Name");
  if ((name == NULL) || (STRCASECMP(name, "") == 0)) {
    LOG_WARNING("Name attribute cannot be found in DeviceSet element!");
    return PLUS_FAIL;
  }

  const char* description = deviceSet->GetAttribute("Description");
  if ((description == NULL) || (STRCASECMP(description, "") == 0)) {
    LOG_WARNING("Description attribute cannot be found in DeviceSet element!");
    return PLUS_FAIL;
  }

  // Verify name and description
  if (STRCASECMP(name, "TEST ConfigFileSaverDialogTest Result") != 0)
  {
    LOG_ERROR("Device set name does not match the expected value!");
    return PLUS_FAIL;
  }

  if (STRCASECMP(description, "ConfigFileSaverDialogTest result with changed line length tolerance value 11.0") != 0)
  {
    LOG_ERROR("Device set description does not match the expected value!");
    return PLUS_FAIL;
  }

  // Check segmentation parameter calue change
  vtkSmartPointer<vtkXMLDataElement> usCalibration = resultRootElement->FindNestedElementWithName("USCalibration");
	if (usCalibration == NULL) {
		LOG_ERROR("No USCalibration element is found in the XML tree!");
		return PLUS_FAIL;
	}

	vtkSmartPointer<vtkXMLDataElement> calibrationController = usCalibration->FindNestedElementWithName("CalibrationController");
	if (calibrationController == NULL) {
		LOG_ERROR("No CalibrationController element is found in the XML tree!");
		return PLUS_FAIL;
	}

	vtkSmartPointer<vtkXMLDataElement> segmentationParameters = calibrationController->FindNestedElementWithName("SegmentationParameters");
	if (segmentationParameters == NULL) {
		LOG_ERROR("No SegmentationParameters element is found in the XML tree!");
		return PLUS_FAIL;
	}

	double maxLineLengthErrorPercent(0.0); 
	if ( segmentationParameters->GetScalarAttribute("MaxLineLengthErrorPercent", maxLineLengthErrorPercent) )
	{
		if (maxLineLengthErrorPercent != 11.0)
    {
      LOG_ERROR("Line length tolerance does not match the expected value!");
      return PLUS_FAIL;
    }
	} else {
    LOG_ERROR("Could not read MaxLineLengthErrorPercent from configuration");
		return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}
