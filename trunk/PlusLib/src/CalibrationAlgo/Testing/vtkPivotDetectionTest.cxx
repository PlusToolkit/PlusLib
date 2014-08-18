/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

/*!
\file vtkPivotDetectionTest.cxx 
\brief This test runs a stylus (pivot) detection on a recorded data set 
and compares the results to a baseline
*/ 

#include "PlusConfigure.h"
#include "PlusMath.h"
#include "TrackedFrame.h"
#include "vtkDataCollector.h"
#include "vtkMatrix4x4.h"
#include "vtkMinimalStandardRandomSequence.h"
#include "vtkPivotDetectionAlgo.h"
#include "vtkPlusChannel.h"
#include "vtkSmartPointer.h"
#include "vtkTrackedFrameList.h"
#include "vtkTransform.h"
#include "vtkTransformRepository.h"
#include "vtkXMLDataElement.h"
#include "vtkXMLUtilities.h"
#include "vtksys/CommandLineArguments.hxx" 
#include "vtksys/SystemTools.hxx"
#include <iostream>
#include <stdlib.h>

#include "vtkPhantomLandmarkRegistrationAlgo.h"

///////////////////////////////////////////////////////////////////
const double ERROR_THRESHOLD_MM=0.001;

int main (int argc, char* argv[])
{ 
  std::string inputConfigFileName;
  std::string inputBaselineFileName;

  std::string inputTrackedStylusTipSequenceMetafile;
  std::string stylusTipToStylusTransformNameStr;

  int verboseLevel=vtkPlusLogger::LOG_LEVEL_UNDEFINED;

  vtksys::CommandLineArguments cmdargs;
  cmdargs.Initialize(argc, argv);

  cmdargs.AddArgument("--config-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputConfigFileName, "Configuration file name");
  cmdargs.AddArgument("--baseline-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputBaselineFileName, "Name of file storing baseline calibration results");
  cmdargs.AddArgument("--verbose", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &verboseLevel, "Verbose level (1=error only, 2=warning, 3=info, 4=debug, 5=trace)");  

  cmdargs.AddArgument("--tracker-input-seq-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputTrackedStylusTipSequenceMetafile, "Input tracker sequence metafile name with path");
  cmdargs.AddArgument("--stylus-tip-to-stylus-transform", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &stylusTipToStylusTransformNameStr, 
    "Transform name that describes the probe pose relative to a static reference (default: StylusTipToReference)"); 

  if ( !cmdargs.Parse() )
  {
    std::cerr << "Problem parsing arguments" << std::endl;
    std::cout << "Help: " << cmdargs.GetHelp() << std::endl;
    exit(EXIT_FAILURE);
  }
  vtkPlusLogger::Instance()->SetLogLevel(verboseLevel);

  LOG_INFO("Initialize"); 
  // Read configuration
  vtkSmartPointer<vtkXMLDataElement> configRootElement = vtkSmartPointer<vtkXMLDataElement>::New();
  if (PlusXmlUtils::ReadDeviceSetConfigurationFromFile(configRootElement, inputConfigFileName.c_str())==PLUS_FAIL)
  {  
    LOG_ERROR("Unable to read configuration from file " << inputConfigFileName.c_str()); 
    return EXIT_FAILURE;
  }

  // Initialize stylus detection
  vtkSmartPointer<vtkPivotDetectionAlgo> PivotDetection = vtkSmartPointer<vtkPivotDetectionAlgo>::New();
  if (PivotDetection == NULL)
  {
    LOG_ERROR("Unable to instantiate pivot detection algorithm class!");
    exit(EXIT_FAILURE);
  }

  if (PivotDetection->ReadConfiguration(configRootElement) != PLUS_SUCCESS)
  {
    LOG_ERROR("Unable to read pivot calibration configuration!");
    exit(EXIT_FAILURE);
  }

  // Read stylus tracker data
  vtkSmartPointer<vtkTrackedFrameList> trackedStylusTipFrames = vtkSmartPointer<vtkTrackedFrameList>::New();
  if( !stylusTipToStylusTransformNameStr.empty() )
  {
    trackedStylusTipFrames->SetValidationRequirements(REQUIRE_UNIQUE_TIMESTAMP | REQUIRE_TRACKING_OK);
  }

  LOG_INFO("Read stylus tracker data from " << inputTrackedStylusTipSequenceMetafile);
  if ( trackedStylusTipFrames->ReadFromSequenceMetafile(inputTrackedStylusTipSequenceMetafile.c_str()) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to read stylus data from sequence metafile: " << inputTrackedStylusTipSequenceMetafile << ". Exiting...");
    exit(EXIT_FAILURE);
  }

  vtkSmartPointer<vtkTransformRepository> transformRepository = vtkSmartPointer<vtkTransformRepository>::New();

  // Check stylus tool
  PlusTransformName stylusToReferenceTransformName(PivotDetection->GetObjectMarkerCoordinateFrame(), PivotDetection->GetReferenceCoordinateFrame());

  vtkSmartPointer<vtkTransformRepository> transformRepositoryCalibration = vtkSmartPointer<vtkTransformRepository>::New();
  if ( transformRepositoryCalibration->ReadConfiguration(configRootElement) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to read CoordinateDefinitions!"); 
    exit(EXIT_FAILURE);
  }
  vtkSmartPointer<vtkMatrix4x4> stylusTipToStylusTransform = vtkSmartPointer<vtkMatrix4x4>::New();
  bool valid = false;
  transformRepositoryCalibration->GetTransform(stylusTipToStylusTransformNameStr, stylusTipToStylusTransform, &valid);
  double pivotFound[3] = {0,0,0};
  double pivotLandmark[3] = {0,0,0};
  if (valid)
  {
    // Acquire positions for pivot calibration
    for (int i=0; i < trackedStylusTipFrames->GetNumberOfTrackedFrames(); ++i)
    {
      //vtkPlusLogger::PrintProgressbar((100.0 * i) /  trackedStylusTipFrames->GetNumberOfTrackedFrames()); 
      vtkSmartPointer<vtkMatrix4x4> stylusToReferenceMatrix = vtkSmartPointer<vtkMatrix4x4>::New();

      if ( transformRepository->SetTransforms(*(trackedStylusTipFrames->GetTrackedFrame(i))) != PLUS_SUCCESS )
      {
        LOG_ERROR("Failed to update transforms in repository with tracked frame!");
        exit(EXIT_FAILURE);
      }

      valid = false;
      if ( (transformRepository->GetTransform(stylusToReferenceTransformName, stylusToReferenceMatrix, &valid) != PLUS_SUCCESS) || (!valid) )
      {
        LOG_ERROR("No valid transform found between stylus to reference!");
        exit(EXIT_FAILURE);
      }
      vtkSmartPointer<vtkMatrix4x4> stylusTipToReferenceTransformMatrix = vtkMatrix4x4::New();
      vtkMatrix4x4::Multiply4x4(stylusToReferenceMatrix,stylusTipToStylusTransform,stylusTipToReferenceTransformMatrix);
      PivotDetection->InsertNextStylusTipToReferenceTransform(stylusTipToReferenceTransformMatrix);
      PivotDetection->IsNewPivotPointFound(valid);
      if(valid)
      {
        PivotDetection->GetPivotPointsReference()->GetPoint(PivotDetection->GetPivotPointsReference()->GetNumberOfPoints()-1, pivotFound);
        LOG_INFO("\nPivot found (" << pivotFound[0]<<", " << pivotFound[1]<<", " << pivotFound[2]<<") at "<<trackedStylusTipFrames->GetTrackedFrame(i)->GetTimestamp()<<"[ms]");
      }
      PivotDetection->IsPivotDetectionCompleted(valid);
      if(valid)
      {
        break;
      }
    }
  }
  else
  {
    LOG_ERROR("No valid transform found between stylus to stylus tip!");
  }

  if (!inputBaselineFileName.empty())
  {
    // Load baseline stylus detection
    vtkSmartPointer<vtkXMLDataElement> baselineRootElem = vtkSmartPointer<vtkXMLDataElement>::Take(vtkXMLUtilities::ReadElementFromFile(inputBaselineFileName.c_str()));
    if (baselineRootElem == NULL) 
    {  
      LOG_ERROR("Unable to read the baseline configuration file: " << inputBaselineFileName); 
    }
    // Phantom registration to read landmarks but there could b done in a nicer way
    vtkSmartPointer<vtkPhantomLandmarkRegistrationAlgo> pivotDetectionBaselineLandmarks = vtkSmartPointer<vtkPhantomLandmarkRegistrationAlgo>::New();
    if (pivotDetectionBaselineLandmarks == NULL)
    {
      LOG_ERROR("Unable to instantiate phantom registration algorithm class!");
      exit(EXIT_FAILURE);
    }
    if (pivotDetectionBaselineLandmarks->ReadConfiguration(baselineRootElem) != PLUS_SUCCESS)
    {
      LOG_ERROR("Unable to read pivot Detection Baseline Landmarks!");
      exit(EXIT_FAILURE);
    }

    int numberOfLandmarks = pivotDetectionBaselineLandmarks->GetDefinedLandmarks()->GetNumberOfPoints();
    if (numberOfLandmarks != PivotDetection->GetExpectedPivotsNumber())
    {
      LOG_ERROR("Number of defined landmarks should be "<<PivotDetection->GetExpectedPivotsNumber()<< " instead of " << numberOfLandmarks << "!");
      exit(EXIT_FAILURE);
    }
    //if (PivotDetection->GetPivotPointsReference()->GetNumberOfPoints()==numberOfLandmarks)
    //{
    for (int id =0; id<PivotDetection->GetPivotPointsReference()->GetNumberOfPoints();id++)
    {
      // Compare to baseline
      pivotDetectionBaselineLandmarks->GetDefinedLandmarks()->GetPoint(id, pivotLandmark);
      PivotDetection->GetPivotPointsReference()->GetPoint(id, pivotFound);
      pivotLandmark[0]=pivotFound[0]-pivotLandmark[0];
      pivotLandmark[1]=pivotFound[1]-pivotLandmark[1];
      pivotLandmark[2]=pivotFound[2]-pivotLandmark[2];
      if (vtkMath::Norm(pivotLandmark)>ERROR_THRESHOLD_MM)
      {
        LOG_ERROR("Comparison of calibration data to baseline failed");
        std::cout << "Exit failure!!!" << std::endl;
        return EXIT_FAILURE;
      }
      else
      {
        LOG_INFO("\nPivot "<< id << " found (" << pivotFound[0]<<", " << pivotFound[1]<<", " << pivotFound[2]);
      }
    }
    //}
    //else
    //{
    //  LOG_ERROR("Comparison of calibration data to baseline failed, not the same number of detected landmarks");
    //  std::cout << "Exit failure!!!" << std::endl;
    //  return EXIT_FAILURE;
    //}

  }
  else
  {
    LOG_DEBUG("Baseline file is not specified. Computed results are not compared to baseline results.");
  }

  std::cout << "Exit success!!!" << std::endl; 
  return EXIT_SUCCESS; 
}


