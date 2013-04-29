/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

/*!
  \file vtkFreehandCalibrationTest.cxx 
  \brief This test runs a freehand calibration on a recorded data set using
  several subsequences of frames and save the results
*/ 

#include "PlusConfigure.h"
#include "vtkPlusConfig.h"
#include "PlusMath.h"
#include "vtkProbeCalibrationAlgo.h"
#include "vtkTransformRepository.h"
#include "FidPatternRecognition.h"

#include "vtkSmartPointer.h"
#include "vtkCommand.h"
#include "vtkCallbackCommand.h"
#include "vtkXMLDataElement.h"
#include "vtkXMLUtilities.h"
#include "vtksys/CommandLineArguments.hxx" 
#include "vtksys/SystemTools.hxx"
#include "vtkMatrix4x4.h"
#include "vtkTransform.h"
#include "vtkTransformRepository.h"
#include "vtkMath.h"
#include "vtkTrackedFrameList.h"
#include "TrackedFrame.h"

#include <stdlib.h>
#include <iostream>

PlusStatus SubSequenceMetafile( vtkTrackedFrameList* aTrackedFrameList, std::vector<unsigned int> selectedFrames);
PlusStatus SetOptimizationMethod( vtkProbeCalibrationAlgo* freehandCalibration, std::string method);

enum OperationType
{
  INCREMENTAL_FRAME_DISTANCE,
  INCREMENTAL_NUMBER_OF_FRAMES,
  MOBILE_WINDOW,
  SAME_NUMBER_OF_FRAMES,
  NO_OPERATION
}; 

int main (int argc, char* argv[])
{
  std::string inputCalibrationSeqMetafile;
  std::string inputValidationSeqMetafile;

  std::string inputConfigFileName;
  std::string resultConfigFileName = "";
  std::string saveResultsFilename;
  std::string strOperation;

#ifndef _WIN32
  //double inputTranslationErrorThreshold(LINUXTOLERANCE);
  //double inputRotationErrorThreshold(LINUXTOLERANCE);
#else
  //double inputTranslationErrorThreshold(0);
  //double inputRotationErrorThreshold(0);
#endif

  int verboseLevel=vtkPlusLogger::LOG_LEVEL_UNDEFINED;

  vtksys::CommandLineArguments cmdargs;
  cmdargs.Initialize(argc, argv);

  cmdargs.AddArgument("--calibration-seq-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputCalibrationSeqMetafile, "Sequence metafile name of input calibration dataset.");
  cmdargs.AddArgument("--validation-seq-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputValidationSeqMetafile, "Sequence metafile name of input validation dataset.");

  cmdargs.AddArgument("--config-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputConfigFileName, "Configuration file name prefix");
  cmdargs.AddArgument("--save-results-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &saveResultsFilename, "Save results file name");
  cmdargs.AddArgument("--operation", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &strOperation, "Type of experiment to perform");

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
    return EXIT_FAILURE;
  }
  vtkPlusConfig::GetInstance()->SetDeviceSetConfigurationData(configRootElement);  

  // Read coordinate definitions
  vtkSmartPointer<vtkTransformRepository> transformRepository = vtkSmartPointer<vtkTransformRepository>::New();
  if ( transformRepository->ReadConfiguration(configRootElement) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to read CoordinateDefinitions!"); 
    return EXIT_FAILURE;
  }

  vtkSmartPointer<vtkProbeCalibrationAlgo> freehandCalibration = vtkSmartPointer<vtkProbeCalibrationAlgo>::New();
  freehandCalibration->ReadConfiguration(configRootElement);

  FidPatternRecognition patternRecognition;
  PatternRecognitionError error;
  patternRecognition.ReadConfiguration(configRootElement);

  bool debugOutput=vtkPlusLogger::Instance()->GetLogLevel()>=vtkPlusLogger::LOG_LEVEL_TRACE; 
  patternRecognition.GetFidSegmentation()->SetDebugOutput(debugOutput);

  // Load and segment calibration image
  vtkSmartPointer<vtkTrackedFrameList> calibrationTrackedFrameList = vtkSmartPointer<vtkTrackedFrameList>::New();
  if (calibrationTrackedFrameList->ReadFromSequenceMetafile(inputCalibrationSeqMetafile.c_str()) != PLUS_SUCCESS)
  {
    LOG_ERROR("Reading calibration images from '" << inputCalibrationSeqMetafile << "' failed!"); 
    return EXIT_FAILURE;
  }

  int numberOfSuccessfullySegmentedCalibrationImages = 0;
  std::vector<unsigned int> segmentedCalibrationFramesIndices;
  if (patternRecognition.RecognizePattern(calibrationTrackedFrameList, error, &numberOfSuccessfullySegmentedCalibrationImages, &segmentedCalibrationFramesIndices) != PLUS_SUCCESS)
  {
    LOG_ERROR("Error occured during segmentation of calibration images!"); 
    return EXIT_FAILURE;
  }

  LOG_INFO("Segmentation success rate of calibration images: " << numberOfSuccessfullySegmentedCalibrationImages << " out of " << calibrationTrackedFrameList->GetNumberOfTrackedFrames());

  // Load and segment validation image
  vtkSmartPointer<vtkTrackedFrameList> validationTrackedFrameList = vtkSmartPointer<vtkTrackedFrameList>::New();
  if (validationTrackedFrameList->ReadFromSequenceMetafile(inputValidationSeqMetafile.c_str()) != PLUS_SUCCESS)
  {
    LOG_ERROR("Reading validation images from '" << inputValidationSeqMetafile << "' failed!"); 
    return EXIT_FAILURE;
  }

  int numberOfSuccessfullySegmentedValidationImages = 0;
  if (patternRecognition.RecognizePattern(validationTrackedFrameList, error, &numberOfSuccessfullySegmentedValidationImages) != PLUS_SUCCESS)
  {
    LOG_ERROR("Error occured during segmentation of validation images!"); 
    return EXIT_FAILURE;
  }

  LOG_INFO("Segmentation success rate of validation images: " << numberOfSuccessfullySegmentedValidationImages << " out of " << validationTrackedFrameList->GetNumberOfTrackedFrames());

  // Keep only the images properly segmented
  SubSequenceMetafile( calibrationTrackedFrameList, segmentedCalibrationFramesIndices);


  ofstream outputFile;
  outputFile.open(saveResultsFilename.c_str(),std::ios_base::app);


  OperationType operation;
  // Set operation
  if ( strOperation.empty() )
  {
    operation = NO_OPERATION;
    LOG_INFO("No modification operation has been specified (specify --operation parameter to change the input sequence).");
  }
  else if ( STRCASECMP(strOperation.c_str(), "INCREMENTAL_FRAME_DISTANCE" ) == 0 )
  {
    operation = INCREMENTAL_FRAME_DISTANCE; 
  }
  else if ( STRCASECMP(strOperation.c_str(), "INCREMENTAL_NUMBER_OF_FRAMES" ) == 0 )
  {
    operation = INCREMENTAL_NUMBER_OF_FRAMES; 
  }
  else if ( STRCASECMP(strOperation.c_str(), "SAME_NUMBER_OF_FRAMES" ) == 0 )
  {
    operation = SAME_NUMBER_OF_FRAMES; 
  }
  else if ( STRCASECMP(strOperation.c_str(), "MOBILE_WINDOW" ) == 0 )
  {
    operation = MOBILE_WINDOW; 
  }

  int minFrame = 0;
  int maxFrame = segmentedCalibrationFramesIndices.size();
  int numberOfConfigurations = 15;
  outputFile << "Number of configurations = "  << numberOfConfigurations << "\n";
  std::vector<int> numberOfCalibrationFrames;
  std::vector< std::vector<int> > selectedFrames;

  if (operation==INCREMENTAL_NUMBER_OF_FRAMES)
  {
    for (int i=0; i<numberOfConfigurations; i++)
    {
      numberOfCalibrationFrames.push_back(5+i*5);
      std::vector<int> frames; 
      for (int j=0; j<numberOfCalibrationFrames.at(i) ; j++)
      {
        int randomIndex = rand() % (maxFrame - minFrame) + minFrame;
        while(std::find(frames.begin(), frames.end(), randomIndex) != frames.end()){
          /* frames contains randomIndex */
          randomIndex = rand() % (maxFrame - minFrame) + minFrame;
        } 
        frames.push_back(randomIndex);
      }
      selectedFrames.push_back(frames);
    }
  }


  if (operation==SAME_NUMBER_OF_FRAMES)
  {
    int numberOfFrames = 80;
    for (int i=0; i<numberOfConfigurations; i++)
    {
      numberOfCalibrationFrames.push_back(numberOfFrames);
      std::vector<int> frames; 
      for (int j=0; j<numberOfCalibrationFrames.at(i) ; j++)
      {
        int randomIndex = rand() % (maxFrame - minFrame) + minFrame;
        while(std::find(frames.begin(), frames.end(), randomIndex) != frames.end()){
          /* frames contains randomIndex */
          randomIndex = rand() % (maxFrame - minFrame) + minFrame;
        } 
        frames.push_back(randomIndex);
      }
      selectedFrames.push_back(frames);
    }
  }

  if (operation==INCREMENTAL_FRAME_DISTANCE)
  {
    int numberOfFrames = 10;
    for (int i=0; i<numberOfConfigurations; i++)
    {
      maxFrame = (2+i)*numberOfFrames;
      std::vector<int> frames; 
      for (int j=0; j<numberOfFrames ; j++)
      {
        int randomIndex = rand() % (maxFrame - minFrame) + minFrame;
        while(std::find(frames.begin(), frames.end(), randomIndex) != frames.end()){
          /* frames contains randomIndex */
          randomIndex = rand() % (maxFrame - minFrame) + minFrame;
        } 
        frames.push_back(randomIndex);
      }
      selectedFrames.push_back(frames);
    }
  }

  if (operation==MOBILE_WINDOW)
  {
    int width = 40;
    int numberOfFrames = 2*width+1;
    for (int i=0; i<numberOfConfigurations; i++)
    {
      int center = rand() % (maxFrame - minFrame - numberOfFrames) + width ;
      std::vector<int> frames; 
      for (int j=0; j<numberOfFrames ; j++)
      {
        frames.push_back(center-width+j);
      }
      selectedFrames.push_back(frames);
    }
  }

  vtkSmartPointer<vtkTrackedFrameList> sequenceTrackedFrameList = vtkSmartPointer<vtkTrackedFrameList>::New();

  std::string methods[] = {"NO_OPTIMIZATION","7param2D","7param3D","8param2D","8param3D"};
  int numberOfMethods = sizeof(methods) / sizeof(methods[0]);
  outputFile << "Number of methods = "  << numberOfMethods << "\n";
  std::vector<double> calibError;
  std::vector<double> validError;
  vnl_matrix_fixed<double,4,4> imageToProbeTransformMatrix;
  int *frameSize = calibrationTrackedFrameList->GetTrackedFrame(0)->GetFrameSize();
  outputFile << "Frame size = "  << frameSize[0] << " " << frameSize[1] << "\n";
  for (int i=0; i<numberOfConfigurations; i++)
  { 
    bool calibrationFail = false;
    int numberOfFramesOfTheSequence = selectedFrames.at(i).size();
    outputFile << "Frames of the sequence = ";
    for (int j=0; j<numberOfFramesOfTheSequence; j++)
    {
      int indexInCalibrationSequence = selectedFrames.at(i).at(j);
      sequenceTrackedFrameList->AddTrackedFrame(calibrationTrackedFrameList->GetTrackedFrame(indexInCalibrationSequence));
      outputFile << indexInCalibrationSequence << " ";
    }
    outputFile << " \n ";

    for (int k=0;k<numberOfMethods;k++)
    {
      outputFile << "Method = " << methods[k] << " \n ";
      if (!calibrationFail)
      {
        SetOptimizationMethod(freehandCalibration,methods[k]);

        // Calibrate
        if (freehandCalibration->Calibrate( validationTrackedFrameList, sequenceTrackedFrameList, transformRepository, patternRecognition.GetFidLineFinder()->GetNWires()) != PLUS_SUCCESS)
        {
          LOG_ERROR("Calibration failed!");
          calibError.push_back(-1);
          calibError.push_back(-1);
          validError.push_back(-1);
          validError.push_back(-1);
          calibrationFail = true;
          //return EXIT_FAILURE;
        }

        freehandCalibration->GetCalibrationReport(&calibError, &validError, &imageToProbeTransformMatrix); 
        // TODO: double-check if the reported values matches the expected values

        outputFile << "Calibration error = ";
        outputFile << calibError.at(0) << " " << calibError.at(1) << " ";
        outputFile << validError.at(0) << " " << validError.at(1)  << " \n ";

        outputFile << "\n ";
        outputFile << "Image to Probe transform matrix = \n ";     
        for (int m=0; m<4;m++)
        {
          for (int n=0; n<4;n++)
          {
            outputFile << imageToProbeTransformMatrix(m,n) << " ";
          }
          outputFile << "\n ";
        }

      }

      calibError.clear();
      validError.clear();

    }

    sequenceTrackedFrameList->Clear();
  }

  outputFile.close();
  std::cout << "Exit success!!!" << std::endl; 
  return EXIT_SUCCESS;
}

//-------------------------------------------------------------------------------------------------

PlusStatus SubSequenceMetafile( vtkTrackedFrameList* aTrackedFrameList, std::vector<unsigned int> selectedFrames)
{
  LOG_INFO("Create a sub sequence using" << selectedFrames.size() << " frames" );
  std::sort(selectedFrames.begin(),selectedFrames.end());
  unsigned int FirstFrameIndex = selectedFrames.at(0);
  unsigned int LastFrameIndex = selectedFrames.at(selectedFrames.size()-1);
  if ( FirstFrameIndex < 0 || LastFrameIndex >= aTrackedFrameList->GetNumberOfTrackedFrames() || FirstFrameIndex > LastFrameIndex)
  {
    LOG_ERROR("Invalid input range: (" << FirstFrameIndex << ", " << LastFrameIndex << ")" << " Permitted range within (0, " << aTrackedFrameList->GetNumberOfTrackedFrames() - 1 << ")");
    return PLUS_FAIL;
  }

  if (LastFrameIndex != aTrackedFrameList->GetNumberOfTrackedFrames()-1)
  {
    aTrackedFrameList->RemoveTrackedFrameRange(LastFrameIndex+1, aTrackedFrameList->GetNumberOfTrackedFrames()-1);
  }

  for (int i=selectedFrames.size()-2;i>0;i--)
  {
    aTrackedFrameList->RemoveTrackedFrameRange(selectedFrames.at(i)+1, selectedFrames.at(i+1)-1);
  }

  if (FirstFrameIndex != 0)
  {
    aTrackedFrameList->RemoveTrackedFrameRange(0, FirstFrameIndex-1);
  }
  return PLUS_SUCCESS; 
}

//-------------------------------------------------------------------------------------------------
PlusStatus SetOptimizationMethod( vtkProbeCalibrationAlgo* freehandCalibration, std::string method)
{
  vtkProbeCalibrationOptimizerAlgo* optimizer = freehandCalibration->GetOptimizer();

  if ( STRCASECMP(method.c_str(), "NO_OPTIMIZATION" ) == 0 )
  {
    optimizer->SetOptimizationMethod(vtkProbeCalibrationOptimizerAlgo::MINIMIZE_NONE);
  }
  else if ( STRCASECMP(method.c_str(), "7param2D" ) == 0 )
  {
    optimizer->SetOptimizationMethod(vtkProbeCalibrationOptimizerAlgo::MINIMIZE_DISTANCE_OF_ALL_WIRES_IN_2D);
    optimizer->SetIsotropicPixelSpacing(true);
  }
  else if ( STRCASECMP(method.c_str(), "7param3D" ) == 0 )
  {
    optimizer->SetOptimizationMethod(vtkProbeCalibrationOptimizerAlgo::MINIMIZE_DISTANCE_OF_MIDDLE_WIRES_IN_3D);
    optimizer->SetIsotropicPixelSpacing(true);
  }
  else if ( STRCASECMP(method.c_str(), "8param2D" ) == 0 )
  {
    optimizer->SetOptimizationMethod(vtkProbeCalibrationOptimizerAlgo::MINIMIZE_DISTANCE_OF_ALL_WIRES_IN_2D);
    optimizer->SetIsotropicPixelSpacing(false);
  }
  else if ( STRCASECMP(method.c_str(), "8param3D" ) == 0 )
  {
    optimizer->SetOptimizationMethod(vtkProbeCalibrationOptimizerAlgo::MINIMIZE_DISTANCE_OF_MIDDLE_WIRES_IN_3D);
    optimizer->SetIsotropicPixelSpacing(false);
  }
  return PLUS_SUCCESS;
}
