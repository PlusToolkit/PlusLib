/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/ 

/*!
  \file This test runs a stylus calibration on a recorded data set and compares the results to a baseline
*/ 

#include "PlusConfigure.h"
#include "vtkPivotCalibrationAlgo.h"
#include "vtkDataCollector.h"
#include "vtkTrackerTool.h"
#include "vtkPlusConfig.h"

#include "vtkSmartPointer.h"
#include "vtkXMLDataElement.h"
#include "vtkXMLUtilities.h"
#include "vtksys/CommandLineArguments.hxx" 
#include "vtksys/SystemTools.hxx"
#include "vtkMatrix4x4.h"
#include "vtkTransform.h"

#include <stdlib.h>
#include <iostream>

///////////////////////////////////////////////////////////////////
const double ERROR_THRESHOLD = 0.05; // error threshold is 5% 

int CompareCalibrationResultsWithBaseline(const char* baselineFileName, const char* currentResultFileName); 

int main (int argc, char* argv[])
{ 
	std::string inputConfigFileName;
	std::string inputBaselineFileName;

	int numberOfAcquiredPoints=100;
	int verboseLevel=vtkPlusLogger::LOG_LEVEL_DEFAULT;

	vtksys::CommandLineArguments cmdargs;
	cmdargs.Initialize(argc, argv);

	cmdargs.AddArgument("--input-config-file-name", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputConfigFileName, "Configuration file name");
	cmdargs.AddArgument("--input-baseline-file-name", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputBaselineFileName, "Name of file storing baseline calibration results");
	cmdargs.AddArgument("--number-of-acquired-points", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &numberOfAcquiredPoints, "Number of acquired points during the pivot calibration");
	cmdargs.AddArgument("--verbose", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &verboseLevel, "Verbose level (1=error only, 2=warning, 3=info, 4=debug, 5=trace)");	

	if ( !cmdargs.Parse() )
	{
		std::cerr << "Problem parsing arguments" << std::endl;
		std::cout << "Help: " << cmdargs.GetHelp() << std::endl;
		exit(EXIT_FAILURE);
	}

	vtkPlusLogger::Instance()->SetLogLevel(verboseLevel);

	std::string programPath("./"), errorMsg; 
	if ( !vtksys::SystemTools::FindProgramPath(argv[0], programPath, errorMsg) )
	{
		LOG_ERROR(errorMsg); 
	}
	programPath = vtksys::SystemTools::GetParentDirectory(programPath.c_str()); 

  LOG_INFO("Initialize"); 

  // Read configuration
  vtkSmartPointer<vtkXMLDataElement> configRootElement = vtkXMLUtilities::ReadElementFromFile(inputConfigFileName.c_str());
  if (configRootElement == NULL)
  {	
    LOG_ERROR("Unable to read configuration from file " << inputConfigFileName.c_str()); 
		exit(EXIT_FAILURE);
  }

  // Initialize data collection
	vtkSmartPointer<vtkDataCollector> dataCollector = vtkSmartPointer<vtkDataCollector>::New(); 
  if (dataCollector->ReadConfiguration(configRootElement) != PLUS_SUCCESS) {
    LOG_ERROR("Unable to parse configuration from file " << inputConfigFileName.c_str()); 
		exit(EXIT_FAILURE);
	}
	if (dataCollector->Connect() != PLUS_SUCCESS) {
    LOG_ERROR("Unable to initialize data collection!");
		exit(EXIT_FAILURE);
	}
	if (dataCollector->Start() != PLUS_SUCCESS) {
    LOG_ERROR("Unable to start data collection!");
		exit(EXIT_FAILURE);
	}
	if ((dataCollector->GetTracker() == NULL) || (dataCollector->GetTracker()->GetNumberOfTools() < 1)) {
    LOG_ERROR("Unable to initialize tracker!");
		exit(EXIT_FAILURE);
	}

  dataCollector->SetTrackingOnly(true);

  // Initialize stylus calibration
  vtkSmartPointer<vtkPivotCalibrationAlgo> pivotCalibration = vtkSmartPointer<vtkPivotCalibrationAlgo>::New();
	if (pivotCalibration == NULL) {
		LOG_ERROR("Unable to instantiate pivot calibration algorithm class!");
		exit(EXIT_FAILURE);
	}

	pivotCalibration->Initialize();

  // Get stylus tool number
  int stylusNumber = dataCollector->GetTracker()->GetFirstPortNumberByType(TRACKER_TOOL_STYLUS);
  if (stylusNumber == -1) {
    LOG_ERROR("Unable to find stylus in tracker!");
		exit(EXIT_FAILURE);
  }

	// Acquire positions for pivot calibration
  for (int i=0; i < numberOfAcquiredPoints; ++i) {
		vtksys::SystemTools::Delay(50);
		vtkPlusLogger::PrintProgressbar((100.0 * i) / numberOfAcquiredPoints); 

    vtkSmartPointer<vtkMatrix4x4> stylusToReferenceMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
	  TrackerStatus status = TR_MISSING;
	  double timestamp;

	  if (dataCollector->GetTracker()->GetTool(stylusNumber)->GetEnabled()) {
		  dataCollector->GetTransformWithTimestamp(stylusToReferenceMatrix, timestamp, status, stylusNumber);
	  }

    if (status == TR_OK) {
      pivotCalibration->InsertNextCalibrationPoint(stylusToReferenceMatrix);
    }
  }

  if (pivotCalibration->DoTooltipCalibration() != PLUS_SUCCESS) {
    LOG_ERROR("Calibration error!");
		exit(EXIT_FAILURE);
  }

	// Save result
	pivotCalibration->WriteConfiguration(configRootElement, TRACKER_TOOL_STYLUS);
  vtkstd::string calibrationResultFileName = "StylusCalibrationTest.xml";
	vtksys::SystemTools::RemoveFile(calibrationResultFileName.c_str());
  configRootElement->PrintXML(calibrationResultFileName.c_str());

  // Compare to baseline
	if ( CompareCalibrationResultsWithBaseline( inputBaselineFileName.c_str(), calibrationResultFileName.c_str() ) !=0 )
	{
		LOG_ERROR("Comparison of calibration data to baseline failed");
		std::cout << "Exit failure!!!" << std::endl;
		return EXIT_FAILURE;
	}

	std::cout << "Exit success!!!" << std::endl; 
	return EXIT_SUCCESS; 
}

//-----------------------------------------------------------------------------

// return the number of differences
int CompareCalibrationResultsWithBaseline(const char* baselineFileName, const char* currentResultFileName)
{
	int numberOfFailures=0;

	double* transformCurrent = new double[16]; 
	double* transformBaseline = new double[16];
	for (int i=0; i<16; ++i) {
		transformCurrent[i] = 0.0;
		transformBaseline[i] = 0.0;
	}

	// Load current stylus calibration
	vtkSmartPointer<vtkXMLDataElement> rootElementCurrent = vtkXMLUtilities::ReadElementFromFile(currentResultFileName);
	if (rootElementCurrent == NULL) {	
		LOG_ERROR("Unable to read the current configuration file: " << currentResultFileName); 
		return ++numberOfFailures;
	}
	vtkSmartPointer<vtkXMLDataElement> stylusDefinitionCurrent = vtkPlusConfig::LookupElementWithNameContainingChildWithNameAndAttribute(rootElementCurrent, "Tracker", "Tool", "Type", "Stylus");
	if (stylusDefinitionCurrent == NULL) {
		LOG_ERROR("No stylus definition is found in the test result XML tree!");
		return ++numberOfFailures;
	}
	vtkSmartPointer<vtkXMLDataElement> calibrationCurrent = stylusDefinitionCurrent->FindNestedElementWithName("Calibration");
	if (calibrationCurrent == NULL) {
		LOG_ERROR("No calibration section is found in stylus definition in test result!");
		return ++numberOfFailures;
	} else {
		calibrationCurrent->GetVectorAttribute("Transform", 16, transformCurrent);
	}

	// Load baseline stylus calibration
	vtkSmartPointer<vtkXMLDataElement> rootElementBaseline = vtkXMLUtilities::ReadElementFromFile(baselineFileName);
	if (rootElementBaseline == NULL) {	
		LOG_ERROR("Unable to read the baseline configuration file: " << baselineFileName); 
		return ++numberOfFailures;
	}
	vtkSmartPointer<vtkXMLDataElement> stylusDefinitionBaseline = vtkPlusConfig::LookupElementWithNameContainingChildWithNameAndAttribute(rootElementBaseline, "Tracker", "Tool", "Type", "Stylus");
	if (stylusDefinitionBaseline == NULL) {
		LOG_ERROR("No stylus definition is found in the baseline XML tree!");
		return ++numberOfFailures;
	}
	vtkSmartPointer<vtkXMLDataElement> calibrationBaseline = stylusDefinitionBaseline->FindNestedElementWithName("Calibration");
	if (calibrationBaseline == NULL) {
		LOG_ERROR("No calibration section is found in stylus definition in baseline!");
		return ++numberOfFailures;
	} else {
		calibrationBaseline->GetVectorAttribute("Transform", 16, transformBaseline);
	}

	if (numberOfFailures > 0) {
		delete[] transformCurrent;
		delete[] transformBaseline;

		return numberOfFailures;
	}

	// Compare the transforms
	for (int i=0; i<16; ++i) {
		double ratio = 1.0 * transformCurrent[i] / transformBaseline[i];
		double diff = fabs(transformCurrent[i] - transformBaseline[i]);

		if ( (ratio > 1 + ERROR_THRESHOLD || ratio < 1 - ERROR_THRESHOLD) && (diff > 10.0 * ERROR_THRESHOLD) ) // error has to be greater than 5% and also greater than 0.5mm
		{
			LOG_ERROR("Transform element (" << i << ") mismatch: current=" << transformCurrent[i]<< ", baseline=" << transformBaseline[i]);
			numberOfFailures++;
		}
	}

	delete[] transformCurrent;
	delete[] transformBaseline;

	return numberOfFailures;
}
