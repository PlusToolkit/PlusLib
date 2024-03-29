/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

/*!
  \file vtkStylusCalibrationTest.cxx
  \brief This test runs a stylus (pivot) calibration on a recorded data set
  and compares the results to a baseline
*/

#include "PlusConfigure.h"
#include "PlusMath.h"
#include "igsioTrackedFrame.h"
#include "vtkPlusDataCollector.h"
#include "vtkMatrix4x4.h"
#include "vtkMinimalStandardRandomSequence.h"
#include "vtkPlusPivotCalibrationAlgo.h"
#include "vtkPlusChannel.h"
#include "vtkPlusDevice.h"
#include "vtkSmartPointer.h"
#include "vtkIGSIOTrackedFrameList.h"
#include "vtkTransform.h"
#include "vtkIGSIOTransformRepository.h"
#include "vtkXMLDataElement.h"
#include "vtkXMLUtilities.h"
#include "vtksys/CommandLineArguments.hxx"
#include "vtksys/SystemTools.hxx"
#include <iostream>
#include <stdlib.h>

///////////////////////////////////////////////////////////////////
const double TRANSLATION_ERROR_THRESHOLD = 0.5; // error threshold is 0.5mm
const double ROTATION_ERROR_THRESHOLD = 0.5; // error threshold is 0.5deg

int CompareCalibrationResultsWithBaseline(const char* baselineFileName, const char* currentResultFileName, const char* stylusCoordinateFrame, const char* stylusTipCoordinateFrame);

int main(int argc, char* argv[])
{
  std::string inputConfigFileName;
  std::string inputBaselineFileName;

  int numberOfPointsToAcquire = 100;
  int verboseLevel = vtkPlusLogger::LOG_LEVEL_UNDEFINED;
  double outlierGenerationProbability = 0.0;

  vtksys::CommandLineArguments cmdargs;
  cmdargs.Initialize(argc, argv);

  cmdargs.AddArgument("--config-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputConfigFileName, "Configuration file name");
  cmdargs.AddArgument("--baseline-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputBaselineFileName, "Name of file storing baseline calibration results");
  cmdargs.AddArgument("--number-of-points-to-acquire", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &numberOfPointsToAcquire, "Number of acquired points during the pivot calibration (default: 100)");
  cmdargs.AddArgument("--outlier-generation-probability", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &outlierGenerationProbability, "Probability for a point being an outlier. If this number is larger than 0 then some valid measurement points are replaced by randomly generated samples to test the robustness of the algorithm. (range: 0.0-1.0; default: 0.0)");
  cmdargs.AddArgument("--verbose", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &verboseLevel, "Verbose level (1=error only, 2=warning, 3=info, 4=debug, 5=trace)");

  if (!cmdargs.Parse())
  {
    std::cerr << "Problem parsing arguments" << std::endl;
    std::cout << "Help: " << cmdargs.GetHelp() << std::endl;
    exit(EXIT_FAILURE);
  }

  vtkPlusLogger::Instance()->SetLogLevel(verboseLevel);

  std::string programPath("./"), errorMsg;
  if (!vtksys::SystemTools::FindProgramPath(argv[0], programPath, errorMsg))
  {
    LOG_ERROR(errorMsg);
  }
  programPath = vtksys::SystemTools::GetParentDirectory(programPath.c_str());

  LOG_INFO("Initialize");

  // Read configuration
  vtkSmartPointer<vtkXMLDataElement> configRootElement = vtkSmartPointer<vtkXMLDataElement>::New();
  if (PlusXmlUtils::ReadDeviceSetConfigurationFromFile(configRootElement, inputConfigFileName.c_str()) == PLUS_FAIL)
  {
    LOG_ERROR("Unable to read configuration from file " << inputConfigFileName.c_str());
    return EXIT_FAILURE;
  }

  vtkPlusConfig::GetInstance()->SetDeviceSetConfigurationData(configRootElement);

  // Initialize data collection
  vtkSmartPointer<vtkPlusDataCollector> dataCollector = vtkSmartPointer<vtkPlusDataCollector>::New();
  if (dataCollector->ReadConfiguration(configRootElement) != PLUS_SUCCESS)
  {
    LOG_ERROR("Unable to parse configuration from file " << inputConfigFileName.c_str());
    exit(EXIT_FAILURE);
  }

  if (dataCollector->Connect() != PLUS_SUCCESS)
  {
    LOG_ERROR("Unable to initialize data collection!");
    exit(EXIT_FAILURE);
  }
  if (dataCollector->Start() != PLUS_SUCCESS)
  {
    LOG_ERROR("Unable to start data collection!");
    exit(EXIT_FAILURE);
  }
  vtkPlusChannel* aChannel(NULL);
  vtkPlusDevice* aDevice(NULL);
  if (dataCollector->GetDevice(aDevice, std::string("TrackerDevice")) != PLUS_SUCCESS)
  {
    LOG_ERROR("Unable to locate device by ID: \'TrackerDevice\'");
    exit(EXIT_FAILURE);
  }
  if (aDevice->GetOutputChannelByName(aChannel, "TrackerStream") != PLUS_SUCCESS)
  {
    LOG_ERROR("Unable to locate channel by ID: \'TrackerStream\'");
    exit(EXIT_FAILURE);
  }
  if (aChannel->GetTrackingDataAvailable() == false)
  {
    LOG_ERROR("Channel \'" << aChannel->GetChannelId() << "\' is not tracking!");
    exit(EXIT_FAILURE);
  }

  // Initialize stylus calibration
  vtkSmartPointer<vtkPlusPivotCalibrationAlgo> pivotCalibration = vtkSmartPointer<vtkPlusPivotCalibrationAlgo>::New();
  if (pivotCalibration == NULL)
  {
    LOG_ERROR("Unable to instantiate pivot calibration algorithm class!");
    exit(EXIT_FAILURE);
  }

  if (pivotCalibration->ReadConfiguration(configRootElement) != PLUS_SUCCESS)
  {
    LOG_ERROR("Unable to read pivot calibration configuration!");
    exit(EXIT_FAILURE);
  }

  // Create and initialize transform repository
  igsioTrackedFrame trackedFrame;
  aChannel->GetTrackedFrame(trackedFrame);

  vtkSmartPointer<vtkIGSIOTransformRepository> transformRepository = vtkSmartPointer<vtkIGSIOTransformRepository>::New();
  transformRepository->SetTransforms(trackedFrame);

  // Check stylus tool
  igsioTransformName stylusToReferenceTransformName(pivotCalibration->GetObjectMarkerCoordinateFrame(), pivotCalibration->GetReferenceCoordinateFrame());

  vtkSmartPointer<vtkMinimalStandardRandomSequence> random = vtkSmartPointer<vtkMinimalStandardRandomSequence>::New();
  random->SetSeed(183495439); // Just some random number was chosen as seed
  int numberOfOutliers = 0;

  // Acquire positions for pivot calibration
  for (int i = 0; i < numberOfPointsToAcquire; ++i)
  {
    vtksys::SystemTools::Delay(50);
    vtkPlusLogger::PrintProgressbar((100.0 * i) / numberOfPointsToAcquire);

    vtkSmartPointer<vtkMatrix4x4> stylusToReferenceMatrix = vtkSmartPointer<vtkMatrix4x4>::New();


    igsioTrackedFrame trackedFrame;
    if (aChannel->GetTrackedFrame(trackedFrame) != PLUS_SUCCESS)
    {
      LOG_ERROR("Failed to get tracked frame!");
      exit(EXIT_FAILURE);
    }

    if (transformRepository->SetTransforms(trackedFrame) != PLUS_SUCCESS)
    {
      LOG_ERROR("Failed to update transforms in repository with tracked frame!");
      exit(EXIT_FAILURE);
    }

    ToolStatus status(TOOL_INVALID);
    if (transformRepository->GetTransform(stylusToReferenceTransformName, stylusToReferenceMatrix, &status) != PLUS_SUCCESS || status != TOOL_OK)
    {
      LOG_ERROR("No valid transform found between stylus to reference!");
      exit(EXIT_FAILURE);
    }

    // Generate an outlier point to see if the calibration algorithm is robust enough
    random->Next();
    if (random->GetValue() < outlierGenerationProbability)
    {
      numberOfOutliers++;
      random->Next();
      double rotX = random->GetRangeValue(-160, 160);
      random->Next();
      double rotY = random->GetRangeValue(-160, 160);
      random->Next();
      double rotZ = random->GetRangeValue(-160, 160);
      double translation[3] = {0};
      random->Next();
      translation[0] = random->GetRangeValue(-1000, 1000);
      random->Next();
      translation[1] = random->GetRangeValue(-1000, 1000);
      random->Next();
      translation[2] = random->GetRangeValue(-1000, 1000);
      // Random matrix that perturbs the actual perfect transform
      vtkSmartPointer<vtkTransform> randomStylusToReferenceTransform = vtkSmartPointer<vtkTransform>::New();
      randomStylusToReferenceTransform->Identity();
      randomStylusToReferenceTransform->Translate(translation);
      randomStylusToReferenceTransform->RotateX(rotX);
      randomStylusToReferenceTransform->RotateY(rotY);
      randomStylusToReferenceTransform->RotateZ(rotZ);
      stylusToReferenceMatrix->DeepCopy(randomStylusToReferenceTransform->GetMatrix());
    }

    pivotCalibration->InsertNextCalibrationPoint(stylusToReferenceMatrix);
  }
  vtkPlusLogger::PrintProgressbar(100.0);

  if (numberOfOutliers > 0)
  {
    LOG_INFO("Number of generated outlier samples: " << numberOfOutliers);
  }

  if (pivotCalibration->DoPivotCalibration(transformRepository) != PLUS_SUCCESS)
  {
    LOG_ERROR("Calibration error!");
    exit(EXIT_FAILURE);
  }

  LOG_INFO("Number of detected outliers: " << pivotCalibration->GetNumberOfDetectedOutliers());
  LOG_INFO("Mean calibration error: " << pivotCalibration->GetPivotCalibrationErrorMm() << " mm");

  // Save result
  if (transformRepository->WriteConfiguration(configRootElement) != PLUS_SUCCESS)
  {
    LOG_ERROR("Failed to write pivot calibration result to configuration element!");
    exit(EXIT_FAILURE);
  }

  std::string calibrationResultFileName = "StylusCalibrationTest.xml";
  LOG_INFO("Writing calibration result (" << pivotCalibration->GetObjectPivotPointCoordinateFrame() << " to " << pivotCalibration->GetObjectMarkerCoordinateFrame() << " transform) to " << calibrationResultFileName);
  vtksys::SystemTools::RemoveFile(calibrationResultFileName.c_str());
  igsioCommon::XML::PrintXML(calibrationResultFileName.c_str(), configRootElement);

  if (!inputBaselineFileName.empty())
  {
    // Compare to baseline
    std::string objectMarkerCoordinateFrame = pivotCalibration->GetObjectMarkerCoordinateFrame();
    std::string objectPivotCoordinateFrame = pivotCalibration->GetObjectPivotPointCoordinateFrame();
    if (CompareCalibrationResultsWithBaseline(inputBaselineFileName.c_str(), calibrationResultFileName.c_str(), objectMarkerCoordinateFrame.c_str(), objectPivotCoordinateFrame.c_str()) != 0)
    {
      LOG_ERROR("Comparison of calibration data to baseline failed");
      std::cout << "Exit failure!!!" << std::endl;
      return EXIT_FAILURE;
    }
  }
  else
  {
    LOG_DEBUG("Baseline file is not specified. Computed results are not compared to baseline results.");
  }

  std::cout << "Exit success!!!" << std::endl;
  return EXIT_SUCCESS;
}

//-----------------------------------------------------------------------------

// return the number of differences
int CompareCalibrationResultsWithBaseline(const char* baselineFileName, const char* currentResultFileName, const char* stylusCoordinateFrame, const char* stylusTipCoordinateFrame)
{
  int numberOfFailures = 0;

  // Load current stylus calibration
  vtkSmartPointer<vtkXMLDataElement> currentRootElem = vtkSmartPointer<vtkXMLDataElement>::Take(vtkXMLUtilities::ReadElementFromFile(currentResultFileName));
  if (currentRootElem == NULL)
  {
    LOG_ERROR("Unable to read the current configuration file: " << currentResultFileName);
    return ++numberOfFailures;
  }

  vtkSmartPointer<vtkMatrix4x4> stylusTipToStylusTransformCurrent = vtkSmartPointer<vtkMatrix4x4>::New();
  double currentError(0);
  if (vtkPlusConfig::GetInstance()->ReadTransformToCoordinateDefinition(currentRootElem, stylusTipCoordinateFrame, stylusCoordinateFrame, stylusTipToStylusTransformCurrent, &currentError) != PLUS_SUCCESS)
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

  vtkSmartPointer<vtkMatrix4x4> stylusTipToStylusTransformBaseline = vtkSmartPointer<vtkMatrix4x4>::New();
  double baselineError(0);
  if (vtkPlusConfig::GetInstance()->ReadTransformToCoordinateDefinition(baselineRootElem, stylusTipCoordinateFrame, stylusCoordinateFrame, stylusTipToStylusTransformBaseline, &baselineError) != PLUS_SUCCESS)
  {
    LOG_ERROR("Failed to read current stylus calibration result from configuration file!");
    return ++numberOfFailures;
  }

  // Compare the transforms
  double posDiff = igsioMath::GetPositionDifference(stylusTipToStylusTransformCurrent, stylusTipToStylusTransformBaseline);
  double rotDiff = igsioMath::GetOrientationDifference(stylusTipToStylusTransformCurrent, stylusTipToStylusTransformBaseline);

  std::ostringstream currentTransform;
  stylusTipToStylusTransformCurrent->PrintSelf(currentTransform, vtkIndent(0));
  std::ostringstream baselineTransform;
  stylusTipToStylusTransformBaseline->PrintSelf(baselineTransform, vtkIndent(0));

  if (posDiff > TRANSLATION_ERROR_THRESHOLD)
  {
    LOG_ERROR("Transform mismatch: translation difference is " << posDiff << ", maximum allowed is " << TRANSLATION_ERROR_THRESHOLD);
    LOG_INFO("Current StylusTipToStylus transform: " << currentTransform.str());
    LOG_INFO("Baseline StylusTipToStylus transform: " << baselineTransform.str());
    numberOfFailures++;
  }

  if (rotDiff > ROTATION_ERROR_THRESHOLD)
  {
    LOG_ERROR("Transform mismatch: rotation difference is " << rotDiff << ", maximum allowed is " << TRANSLATION_ERROR_THRESHOLD);
    LOG_INFO("Current transform: " << currentTransform.str());
    LOG_INFO("Baseline transform: " << baselineTransform.str());
    numberOfFailures++;
  }

  return numberOfFailures;
}
