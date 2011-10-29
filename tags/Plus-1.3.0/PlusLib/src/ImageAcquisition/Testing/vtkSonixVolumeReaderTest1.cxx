/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"
#include "vtksys/CommandLineArguments.hxx"
#include "vtkSonixVolumeReader.h"
#include "vtkImageDifference.h"
#include "vtkSmartPointer.h"
#include "vtkImageExtractComponents.h"
#include "PlusVideoFrame.h"
#include "vtkImageData.h"
#include <stdlib.h>
#include <iostream>

int main (int argc, char* argv[])
{ 
	bool printHelp(false); 
	std::string inputFileName;
	std::string inputBaselineName;
	int inputFrameNumber(-1); 

	vtksys::CommandLineArguments args;
	args.Initialize(argc, argv);

	int verboseLevel = vtkPlusLogger::LOG_LEVEL_ERROR;

	args.AddArgument("--help", vtksys::CommandLineArguments::NO_ARGUMENT, &printHelp, "Print this help.");	
	args.AddArgument("--input-file-name", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputFileName, "The file name of the Sonix volume." );
	args.AddArgument("--input-frame-number", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputFrameNumber, "The frame number to compare with baseline." );
	args.AddArgument("--input-baseline-name", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputBaselineName, "The file name of the baseline image." );
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

	if ( inputFileName.empty() || inputBaselineName.empty() ||  inputFrameNumber < 0 )
	{
		LOG_ERROR("The input-file-name, the input-frame-number and the input-baseline-name parameters are required!");
		exit(EXIT_FAILURE); 
	}


	vtkSonixVolumeReader * sonixVolumeReader = vtkSonixVolumeReader::New(); 
	
	if ( sonixVolumeReader->ReadVolume(inputFileName.c_str() ) < 0 ) 
	{
		LOG_ERROR("Unable to read volume"); 
		return EXIT_FAILURE; 
	}

	vtkstd::vector<vtkImageData*>* imageDataVector = sonixVolumeReader->GetAllFrames(); 

	vtkSmartPointer<vtkImageDifference> imgDiff = vtkSmartPointer<vtkImageDifference>::New(); 
	
  PlusVideoFrame baselineVideoFrame; 
  if ( PlusVideoFrame::ReadImageFromFile(baselineVideoFrame, inputBaselineName.c_str()) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to read baseline image from file: " << inputBaselineName ); 
    return EXIT_FAILURE; 
  }


  vtkSmartPointer<vtkImageExtractComponents> imageExtractorBase =  vtkSmartPointer<vtkImageExtractComponents>::New(); 
  imageExtractorBase->SetInput(baselineVideoFrame.GetVtkImage()); 
  imageExtractorBase->SetComponents(0,0,0); // we are using only the 0th component
  imageExtractorBase->Update(); 

  vtkSmartPointer<vtkImageData> baselineRGB = vtkSmartPointer<vtkImageData>::New(); 
  baselineRGB->DeepCopy(imageExtractorBase->GetOutput()); 
  baselineRGB->Update();

  vtkSmartPointer<vtkImageExtractComponents> imageExtractorInput =  vtkSmartPointer<vtkImageExtractComponents>::New(); 
  imageExtractorInput->SetInput(imageDataVector->at(inputFrameNumber)); 
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
		sonixVolumeReader->Delete(); 
		return EXIT_FAILURE; 
	}

	sonixVolumeReader->Delete(); 

	return EXIT_SUCCESS; 
}