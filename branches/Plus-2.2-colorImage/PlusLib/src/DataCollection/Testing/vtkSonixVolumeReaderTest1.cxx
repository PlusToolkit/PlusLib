/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"
#include "vtksys/CommandLineArguments.hxx"
#include "vtksys/SystemTools.hxx"
#include "vtkSonixVolumeReader.h"
#include "vtkImageDifference.h"
#include "vtkSmartPointer.h"
#include "vtkImageExtractComponents.h"
#include "PlusVideoFrame.h"
#include "vtkImageData.h"
#include "vtkTrackedFrameList.h" 
#include "TrackedFrame.h" 
#include <stdlib.h>
#include <iostream>

int main (int argc, char* argv[])
{ 
  bool printHelp(false); 
  std::string inputFileName;
  std::string inputBaselineName;
  int inputFrameNumber(-1); 
  std::string outputFileName; 

  vtksys::CommandLineArguments args;
  args.Initialize(argc, argv);

  int verboseLevel = vtkPlusLogger::LOG_LEVEL_ERROR;

  args.AddArgument("--help", vtksys::CommandLineArguments::NO_ARGUMENT, &printHelp, "Print this help.");  
  args.AddArgument("--volume-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputFileName, "The file name of the Sonix volume." );
  args.AddArgument("--output-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &outputFileName, "Sequence meta file name to save (save only if defined)" );
  args.AddArgument("--frame-number", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputFrameNumber, "The frame number to compare with baseline." );
  args.AddArgument("--baseline", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputBaselineName, "The file name of the baseline image." );
  args.AddArgument("--verbose", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &verboseLevel, "Verbose level (Default: 1; 1=error only, 2=warning, 3=info, 4=debug)");  

  if ( !args.Parse() )
  {
    std::cerr << "Problem parsing arguments" << std::endl;
    std::cout << "\n\nvtkICCapturingSourceTest1 help:" << args.GetHelp() << std::endl;
    exit(EXIT_FAILURE);
  }

  vtkPlusLogger::Instance()->SetLogLevel(verboseLevel);

  if ( printHelp ) 
  {
    std::cout << "\n\nvtkICCapturingSourceTest1 help:" << args.GetHelp() << std::endl;
    exit(EXIT_SUCCESS); 

  }

  if ( inputFileName.empty() )
  {
    LOG_ERROR("The input-file-name  parameter is required!");
    exit(EXIT_FAILURE); 
  }


  vtkSmartPointer<vtkTrackedFrameList> sonixVolumeData = vtkSmartPointer<vtkTrackedFrameList>::New(); 

  if ( vtkSonixVolumeReader::GenerateTrackedFrameFromSonixVolume(inputFileName.c_str(), sonixVolumeData ) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to generate tracked frame from sonix volume: " << inputFileName ); 
    exit(EXIT_FAILURE); 
  }

  if ( !outputFileName.empty() )
  {
    std::string path = vtkPlusConfig::GetInstance()->GetOutputPath(outputFileName); 
    LOG_INFO("Save tracked frames to " << path); 
    if ( sonixVolumeData->SaveToSequenceMetafile(path.c_str(), false /*no compression*/) != PLUS_SUCCESS )
    {
      LOG_ERROR("Failed to save sonix volume to " << path); 
    }
  }
  
  vtkSmartPointer<vtkImageDifference> imgDiff = vtkSmartPointer<vtkImageDifference>::New(); 
  
  PlusVideoFrame baselineVideoFrame; 
  if ( PlusVideoFrame::ReadImageFromFile(baselineVideoFrame, inputBaselineName.c_str()) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to read baseline image from file: " << inputBaselineName ); 
    exit(EXIT_FAILURE); 
  }

  vtkSmartPointer<vtkImageExtractComponents> imageExtractorBase =  vtkSmartPointer<vtkImageExtractComponents>::New(); 
  imageExtractorBase->SetInput(baselineVideoFrame.GetVtkImage()); 
  imageExtractorBase->SetComponents(0,0,0); // we are using only the 0th component
  imageExtractorBase->Update(); 

  vtkSmartPointer<vtkImageData> baselineRGB = vtkSmartPointer<vtkImageData>::New(); 
  baselineRGB->DeepCopy(imageExtractorBase->GetOutput()); 
  baselineRGB->Update();

  if ( sonixVolumeData->GetNumberOfTrackedFrames() < inputFrameNumber )
  {
    LOG_ERROR("Unable to get tracked frame from list, frame number (" << inputFrameNumber 
      << ") is larger than tracked frame list size (" << sonixVolumeData->GetNumberOfTrackedFrames() << ")!"); 
    exit(EXIT_FAILURE); 
  }

  PlusVideoFrame* videoFrame = sonixVolumeData->GetTrackedFrame(inputFrameNumber)->GetImageData(); 

  if ( !videoFrame->IsImageValid() )
  {
    LOG_ERROR("Video frame is not valid!"); 
    exit(EXIT_FAILURE); 
  }


  vtkSmartPointer<vtkImageExtractComponents> imageExtractorInput =  vtkSmartPointer<vtkImageExtractComponents>::New(); 
  imageExtractorInput->SetInput(videoFrame->GetVtkImage() ); 
  imageExtractorInput->SetComponents(0,0,0); // we are using only the 0th component
  imageExtractorInput->Update(); 
  vtkSmartPointer<vtkImageData> frameRGB = vtkSmartPointer<vtkImageData>::New(); 
  frameRGB->DeepCopy(imageExtractorInput->GetOutput()); 
  frameRGB->Update(); 

  imgDiff->SetImage( baselineRGB ); 
  imgDiff->SetInput( frameRGB ); 
  imgDiff->Update(); 

  double error = imgDiff->GetError(); 

  if ( error > 0 )
  {
    std::cout << "Error = " << error << std::endl; 
    return EXIT_FAILURE; 
  }

  return EXIT_SUCCESS; 
}