/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/ 

/*!
  \file This test runs a stylus calibration on a recorded data set and compares the results to a baseline
*/ 

#include "PlusConfigure.h"
#include "vtkPlusConfig.h"
#include "vtkPivotCalibrationAlgo.h"
#include "vtkDataCollector.h"
#include "vtkTrackedFrameList.h"
#include "TrackedFrame.h"
#include "vtkTransformRepository.h"

#include "vtkSmartPointer.h"
#include "vtkXMLDataElement.h"
#include "vtkXMLUtilities.h"
#include "vtksys/CommandLineArguments.hxx" 
#include "vtksys/SystemTools.hxx"
#include "vtkMatrix4x4.h"
#include "vtkTransform.h"
#include "PlusMath.h"

#include <stdlib.h>
#include <iostream>

///////////////////////////////////////////////////////////////////
const double TRANSLATION_ERROR_THRESHOLD = 0.5; // error threshold is 0.5mm
const double ROTATION_ERROR_THRESHOLD = 0.5; // error threshold is 0.5deg

int CompareCalibrationResultsWithBaseline(const char* baselineFileName, const char* currentResultFileName, const char* stylusToolName); 

int main (int argc, char* argv[])
{ 
  std::string inputConfigFileName;
  std::string inputBaselineFileName;

  int verboseLevel=vtkPlusLogger::LOG_LEVEL_DEFAULT;

  vtksys::CommandLineArguments cmdargs;
  cmdargs.Initialize(argc, argv);

  cmdargs.AddArgument("--input-config-file-name", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputConfigFileName, "Configuration file name");
  cmdargs.AddArgument("--input-baseline-file-name", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputBaselineFileName, "Name of file storing baseline calibration results");
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
  vtkSmartPointer<vtkXMLDataElement> configRootElement = vtkSmartPointer<vtkXMLDataElement>::Take(vtkXMLUtilities::ReadElementFromFile(inputConfigFileName.c_str()));
  if (configRootElement == NULL)
  {  
    LOG_ERROR("Unable to read configuration from file " << inputConfigFileName.c_str()); 
    exit(EXIT_FAILURE);
  }

  vtkPlusConfig::GetInstance()->SetDeviceSetConfigurationData(configRootElement); 

  // Read number of points to acquire and needed tool names
  vtkXMLDataElement* fCalElement = configRootElement->FindNestedElementWithName("fCal"); 
  if (fCalElement == NULL)
  {
    LOG_ERROR("Unable to find fCal element in XML tree!"); 
    exit(EXIT_FAILURE);
  }

  // Number of stylus calibraiton points to acquire
  int numberOfStylusCalibrationPointsToAcquire = 0; 
  fCalElement->GetScalarAttribute("NumberOfStylusCalibrationPointsToAcquire", numberOfStylusCalibrationPointsToAcquire);
  if (numberOfStylusCalibrationPointsToAcquire < 5)
  {
    LOG_ERROR("Invalid number of stylus calibration points to aquire!");
    exit(EXIT_FAILURE);
  }

  // Stylus and reference tool names
  vtkXMLDataElement* trackerToolNames = fCalElement->FindNestedElementWithName("TrackerToolNames"); 
  if (trackerToolNames == NULL)
  {
    LOG_ERROR("Unable to find TrackerToolNames element in XML tree!"); 
    exit(EXIT_FAILURE);
  }
  const char* stylusToolName = trackerToolNames->GetAttribute("Stylus");
  if (stylusToolName == NULL)
  {
    LOG_ERROR("Stylus tool name is not specified in the fCal section of the configuration!");
    exit(EXIT_FAILURE);
  }
  const char* referenceToolName = trackerToolNames->GetAttribute("Reference");
  if (referenceToolName == NULL)
  {
    LOG_ERROR("Reference tool name is not specified in the fCal section of the configuration!");
    exit(EXIT_FAILURE);
  }

  // Initialize data collection
  vtkSmartPointer<vtkDataCollector> dataCollector = vtkSmartPointer<vtkDataCollector>::New(); 
  if (dataCollector->ReadConfiguration(configRootElement) != PLUS_SUCCESS)
  {
    LOG_ERROR("Unable to parse configuration from file " << inputConfigFileName.c_str()); 
    exit(EXIT_FAILURE);
  }

  dataCollector->SetTrackingOnly(true);

  if (dataCollector->Connect() != PLUS_SUCCESS) {
    LOG_ERROR("Unable to initialize data collection!");
    exit(EXIT_FAILURE);
  }
  if (dataCollector->Start() != PLUS_SUCCESS) {
    LOG_ERROR("Unable to start data collection!");
    exit(EXIT_FAILURE);
  }
  if (dataCollector->GetTrackingEnabled() == false) {
    LOG_ERROR("Data collector is not tracking!");
    exit(EXIT_FAILURE);
  }

  // Initialize stylus calibration
  vtkSmartPointer<vtkPivotCalibrationAlgo> pivotCalibration = vtkSmartPointer<vtkPivotCalibrationAlgo>::New();
  if (pivotCalibration == NULL)
  {
    LOG_ERROR("Unable to instantiate pivot calibration algorithm class!");
    exit(EXIT_FAILURE);
  }

  pivotCalibration->Initialize();

  // Create and initialize transform repository
  TrackedFrame trackedFrame;
  dataCollector->GetTrackedFrame(&trackedFrame);

  vtkSmartPointer<vtkTransformRepository> transformRepository = vtkSmartPointer<vtkTransformRepository>::New();
  transformRepository->SetTransforms(trackedFrame);

  // Check stylus tool
  PlusTransformName stylusToReferenceTransformName(stylusToolName, referenceToolName);
  vtkSmartPointer<vtkMatrix4x4> stylusToReferenceTransformMAtrix = vtkSmartPointer<vtkMatrix4x4>::New();
  bool valid = false;
  transformRepository->GetTransform(stylusToReferenceTransformName, stylusToReferenceTransformMAtrix, &valid);
  if (!valid)
  {
    LOG_ERROR("No valid transform found between stylus to reference!");
    exit(EXIT_FAILURE);
  }

  // Acquire positions for pivot calibration
  for (int i=0; i < numberOfStylusCalibrationPointsToAcquire; ++i)
  {
    vtksys::SystemTools::Delay(50);
    vtkPlusLogger::PrintProgressbar((100.0 * i) / numberOfStylusCalibrationPointsToAcquire); 

    vtkSmartPointer<vtkMatrix4x4> stylusToReferenceMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
    TrackerStatus status = TR_MISSING;
    double timestamp;

    dataCollector->GetTransformWithTimestamp(stylusToReferenceMatrix, timestamp, status, stylusToReferenceTransformName);

    if (status == TR_OK)
    {
      pivotCalibration->InsertNextCalibrationPoint(stylusToReferenceMatrix);
    }
  }

  if (pivotCalibration->DoTooltipCalibration() != PLUS_SUCCESS)
  {
    LOG_ERROR("Calibration error!");
    exit(EXIT_FAILURE);
  }

  // Save result
  if ( vtkPlusConfig::WriteTransformToCoordinateDefinition("StylusTip", stylusToolName, pivotCalibration->GetTooltipToToolTransform()->GetMatrix(), 
    pivotCalibration->GetCalibrationError(), vtksys::SystemTools::GetCurrentDateTime("%Y.%m.%d %X").c_str()) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to write pivot calibration result to config file!");
    exit(EXIT_FAILURE);
  }

  vtkstd::string calibrationResultFileName = "StylusCalibrationTest.xml";
  vtksys::SystemTools::RemoveFile(calibrationResultFileName.c_str());
  configRootElement->PrintXML(calibrationResultFileName.c_str());

  // Compare to baseline
  if ( CompareCalibrationResultsWithBaseline( inputBaselineFileName.c_str(), calibrationResultFileName.c_str(), stylusToolName ) !=0 )
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
int CompareCalibrationResultsWithBaseline(const char* baselineFileName, const char* currentResultFileName, const char* stylusToolName )
{
  int numberOfFailures=0;

  // Load current stylus calibration
  vtkSmartPointer<vtkXMLDataElement> currentRootElem = vtkSmartPointer<vtkXMLDataElement>::Take(vtkXMLUtilities::ReadElementFromFile(currentResultFileName));
  if (currentRootElem == NULL) 
  {  
    LOG_ERROR("Unable to read the current configuration file: " << currentResultFileName); 
    return ++numberOfFailures;
  }
  
  vtkSmartPointer<vtkMatrix4x4> transformCurrent = vtkSmartPointer<vtkMatrix4x4>::New(); 
  double currentError(0); 
  if ( vtkPlusConfig::ReadTransformToCoordinateDefinition(currentRootElem, "StylusTip", stylusToolName, transformCurrent, &currentError) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to read current pivot calibration result from configuration file!"); 
    return ++numberOfFailures;
  }

  // Load baseline stylus calibration
  vtkSmartPointer<vtkXMLDataElement> baselineRootElem = vtkSmartPointer<vtkXMLDataElement>::Take(vtkXMLUtilities::ReadElementFromFile(baselineFileName));
  if (baselineRootElem == NULL) 
  {  
    LOG_ERROR("Unable to read the baseline configuration file: " << baselineFileName); 
    return ++numberOfFailures;
  }

  vtkSmartPointer<vtkMatrix4x4> transformBaseline = vtkSmartPointer<vtkMatrix4x4>::New(); 
  double baselineError(0); 
  if ( vtkPlusConfig::ReadTransformToCoordinateDefinition(baselineRootElem, "StylusTip", stylusToolName, transformBaseline, &baselineError) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to read current stylus calibration result from configuration file!"); 
    return ++numberOfFailures;
  }


  // Compare the transforms
  double posDiff = PlusMath::GetPositionDifference(transformCurrent, transformBaseline); 
  double rotDiff = PlusMath::GetOrientationDifference(transformCurrent, transformBaseline); 

  if ( posDiff > TRANSLATION_ERROR_THRESHOLD || rotDiff > ROTATION_ERROR_THRESHOLD )
  {
    std::ostringstream currentTransform; 
    transformCurrent->PrintSelf(currentTransform, vtkIndent(0));
    std::ostringstream baselineTransform; 
    transformBaseline->PrintSelf(baselineTransform, vtkIndent(0));

    LOG_ERROR("Transform mismatch: current=" << currentTransform.str() << ", baseline=" << baselineTransform.str() );
    numberOfFailures++;
  }

  return numberOfFailures;
}
