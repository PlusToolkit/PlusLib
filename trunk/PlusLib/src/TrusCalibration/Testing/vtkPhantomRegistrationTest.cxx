/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

/*!
\file This test runs a phantom registration on a recorded data set and compares the results to a baseline
*/ 

#include "PlusConfigure.h"
#include "vtkPlusConfig.h"
#include "vtkPhantomRegistrationAlgo.h"
#include "vtkDataCollectorHardwareDevice.h"
#include "vtkTrackedFrameList.h"
#include "TrackedFrame.h"
#include "vtkFakeTracker.h"
#include "vtkTransformRepository.h"

#include "vtkSmartPointer.h"
#include "vtkXMLDataElement.h"
#include "vtkXMLUtilities.h"
#include "vtksys/CommandLineArguments.hxx" 
#include "vtksys/SystemTools.hxx"
#include "vtkMatrix4x4.h"
#include "vtkTransform.h"
#include "vtkMath.h"

#include <stdlib.h>
#include <iostream>

///////////////////////////////////////////////////////////////////
const double ERROR_THRESHOLD = 0.001; // error threshold  

PlusStatus CompareRegistrationResultsWithBaseline(const char* baselineFileName, const char* currentResultFileName); 

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

  LOG_INFO("Initialize"); 

  // Read configuration
  vtkSmartPointer<vtkXMLDataElement> configRootElement = vtkSmartPointer<vtkXMLDataElement>::Take(vtkXMLUtilities::ReadElementFromFile(inputConfigFileName.c_str()));
  if (configRootElement == NULL)
  {  
    LOG_ERROR("Unable to read configuration from file " << inputConfigFileName.c_str()); 
    exit(EXIT_FAILURE);
  }

  vtkPlusConfig::GetInstance()->SetDeviceSetConfigurationData(configRootElement); 

  // Stylus and reference tool names
  vtkXMLDataElement* fCalElement = configRootElement->FindNestedElementWithName("fCal"); 
  if (fCalElement == NULL)
  {
    LOG_ERROR("Unable to find fCal element in XML tree!"); 
    exit(EXIT_FAILURE);
  }

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
  vtkSmartPointer<vtkDataCollectorHardwareDevice> dataCollector = vtkSmartPointer<vtkDataCollectorHardwareDevice>::New(); 
  if (dataCollector->ReadConfiguration(configRootElement) != PLUS_SUCCESS) {
    LOG_ERROR("Unable to parse configuration from file " << inputConfigFileName.c_str()); 
    exit(EXIT_FAILURE);
  }

  dataCollector->SetTrackingOnly(true);

  if (dataCollector->Connect() != PLUS_SUCCESS) {
    LOG_ERROR("Data collector was unable to connect to devices!");
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

  // Read coordinate definitions
  vtkSmartPointer<vtkTransformRepository> transformRepository = vtkSmartPointer<vtkTransformRepository>::New();
  if ( transformRepository->ReadConfiguration(configRootElement) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to read CoordinateDefinitions!"); 
    exit(EXIT_FAILURE);
  }

  // Initialize phantom registration
  vtkSmartPointer<vtkPhantomRegistrationAlgo> phantomRegistration = vtkSmartPointer<vtkPhantomRegistrationAlgo>::New();
  if (phantomRegistration == NULL)
  {
    LOG_ERROR("Unable to instantiate phantom registration algorithm class!");
    exit(EXIT_FAILURE);
  }
  if (phantomRegistration->ReadConfiguration(configRootElement) != PLUS_SUCCESS)
  {
    LOG_ERROR("Unable to read phantom definition!");
    exit(EXIT_FAILURE);
  }

  if (phantomRegistration->GetDefinedLandmarks()->GetNumberOfPoints() != 8)
  {
    LOG_ERROR("Number of defined landmarks should be 8 instead of " << phantomRegistration->GetDefinedLandmarks()->GetNumberOfPoints() << "!");
    exit(EXIT_FAILURE);
  }

  // Acquire landmarks
  vtkFakeTracker *fakeTracker = dynamic_cast<vtkFakeTracker*>(dataCollector->GetTracker());
  if (fakeTracker == NULL) {
    LOG_ERROR("Invalid tracker object!");
    exit(EXIT_FAILURE);
  }
  fakeTracker->SetTransformRepository(transformRepository);

  TrackedFrame trackedFrame;
  PlusTransformName stylusTipToReferenceTransformName("StylusTip", referenceToolName);
  
  for (int landmarkCounter=0; landmarkCounter<8; ++landmarkCounter)
  {
    fakeTracker->SetCounter(landmarkCounter);
    vtkAccurateTimer::Delay(2.1 / fakeTracker->GetFrequency());

    vtkSmartPointer<vtkMatrix4x4> stylusTipToReferenceMatrix = vtkSmartPointer<vtkMatrix4x4>::New();

    dataCollector->GetTrackedFrame(&trackedFrame);
    transformRepository->SetTransforms(trackedFrame);

    bool valid(false); 
    if ( (transformRepository->GetTransform(stylusTipToReferenceTransformName, stylusTipToReferenceMatrix, &valid) != PLUS_SUCCESS) || (!valid) )
    {
      LOG_ERROR("No valid transform found between stylus tip to reference!");
      continue; 
    }

    // Compute point position from matrix
    double stylusTipPosition[3]={stylusTipToReferenceMatrix->GetElement(0,3), stylusTipToReferenceMatrix->GetElement(1,3), stylusTipToReferenceMatrix->GetElement(2,3) };
    
    // Add recorded point to algorithm
    phantomRegistration->GetRecordedLandmarks()->InsertPoint(landmarkCounter, stylusTipPosition);
    phantomRegistration->GetRecordedLandmarks()->Modified();

    vtkPlusLogger::PrintProgressbar((100.0 * landmarkCounter) / 8); 
  }

  if (phantomRegistration->Register() != PLUS_SUCCESS)
  {
    LOG_ERROR("Phantom registration failed!");
    exit(EXIT_FAILURE);
  }

  vtkPlusLogger::PrintProgressbar(100); 

  LOG_INFO("Registration error = " << phantomRegistration->GetRegistrationError());

  // Save result
  PlusTransformName tnPhantomToPhantomReference("Phantom", "Reference"); 
  transformRepository->SetTransform(tnPhantomToPhantomReference, phantomRegistration->GetPhantomToReferenceTransformMatrix() ); 
  transformRepository->SetTransformPersistent(tnPhantomToPhantomReference, true); 
  transformRepository->SetTransformError(tnPhantomToPhantomReference, phantomRegistration->GetRegistrationError() ); 
  transformRepository->WriteConfiguration(configRootElement); 

  vtkstd::string registrationResultFileName = "PhantomRegistrationTest.xml";
  vtksys::SystemTools::RemoveFile(registrationResultFileName.c_str());
  configRootElement->PrintXML(registrationResultFileName.c_str());

  if ( CompareRegistrationResultsWithBaseline( inputBaselineFileName.c_str(), registrationResultFileName.c_str() ) != PLUS_SUCCESS )
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
PlusStatus CompareRegistrationResultsWithBaseline(const char* baselineFileName, const char* currentResultFileName)
{
  int numberOfFailures=0;

  if ( baselineFileName == NULL )
  {
    LOG_ERROR("Unable to read the baseline configuration file - filename is NULL"); 
    return PLUS_FAIL;
  }

  if ( currentResultFileName == NULL )
  {
    LOG_ERROR("Unable to read the current configuration file - filename is NULL"); 
    return PLUS_FAIL;
  }

  PlusTransformName tnPhantomToPhantomReference("Phantom", "Reference"); 

  // Load current phantom registration
  vtkSmartPointer<vtkXMLDataElement> currentRootElem = vtkSmartPointer<vtkXMLDataElement>::Take(
    vtkXMLUtilities::ReadElementFromFile(currentResultFileName));
  if (currentRootElem == NULL) 
  {  
    LOG_ERROR("Unable to read the current configuration file: " << currentResultFileName); 
    return PLUS_FAIL;
  }

  vtkSmartPointer<vtkTransformRepository> currentTransformRepository = vtkSmartPointer<vtkTransformRepository>::New(); 
  if ( currentTransformRepository->ReadConfiguration(currentRootElem) != PLUS_SUCCESS )
  {
    LOG_ERROR("Unable to read the current CoordinateDefinitions from configuration file: " << currentResultFileName); 
    return PLUS_FAIL;
  }

  vtkSmartPointer<vtkMatrix4x4> currentMatrix = vtkSmartPointer<vtkMatrix4x4>::New(); 
  if ( currentTransformRepository->GetTransform(tnPhantomToPhantomReference, currentMatrix) != PLUS_SUCCESS )
  {
    std::string strTransformName; 
    tnPhantomToPhantomReference.GetTransformName(strTransformName);
    LOG_ERROR("Unable to get '" << strTransformName << "' coordinate definition from configuration file: " << currentResultFileName); 
    return PLUS_FAIL;
  }

  // Load baseline phantom registration
  vtkSmartPointer<vtkXMLDataElement> baselineRootElem = vtkSmartPointer<vtkXMLDataElement>::Take(
    vtkXMLUtilities::ReadElementFromFile(baselineFileName));
  if (baselineFileName == NULL) 
  {  
    LOG_ERROR("Unable to read the baseline configuration file: " << baselineFileName); 
    return PLUS_FAIL;
  }

  vtkSmartPointer<vtkTransformRepository> baselineTransformRepository = vtkSmartPointer<vtkTransformRepository>::New(); 
  if ( baselineTransformRepository->ReadConfiguration(baselineRootElem) != PLUS_SUCCESS )
  {
    LOG_ERROR("Unable to read the baseline CoordinateDefinitions from configuration file: " << baselineFileName); 
    return PLUS_FAIL;
  }

  vtkSmartPointer<vtkMatrix4x4> baselineMatrix = vtkSmartPointer<vtkMatrix4x4>::New(); 
  if ( baselineTransformRepository->GetTransform(tnPhantomToPhantomReference, baselineMatrix) != PLUS_SUCCESS )
  {
    std::string strTransformName; 
    tnPhantomToPhantomReference.GetTransformName(strTransformName);
    LOG_ERROR("Unable to get '" << strTransformName << "' coordinate definition from configuration file: " << baselineFileName); 
    return PLUS_FAIL;
  }

  // Compare the transforms
  double posDiff=PlusMath::GetPositionDifference(currentMatrix, baselineMatrix); 
  double orientDiff=PlusMath::GetOrientationDifference(currentMatrix, baselineMatrix); 

  if ( fabs(posDiff) > ERROR_THRESHOLD || fabs(orientDiff) > ERROR_THRESHOLD )
  {
    LOG_ERROR("Transform mismatch (position difference: " << posDiff << "  orientation difference: " << orientDiff);
    return PLUS_FAIL; 
  }

  return PLUS_SUCCESS; 
}
