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
#include "vtkTIFFReader.h"
#include "vtkImageData.h"
#include <stdlib.h>
#include <iostream>

#include "vtkTIFFWriter.h"

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
	
	vtkSmartPointer<vtkTIFFReader> tiffReader = vtkSmartPointer<vtkTIFFReader>::New(); 
	tiffReader->SetFileName(inputBaselineName.c_str()); 
	tiffReader->Update(); 

	vtkSmartPointer<vtkImageData> baselineRGB = vtkSmartPointer<vtkImageData>::New(); 
	baselineRGB->CopyStructure(tiffReader->GetOutput()); 
	baselineRGB->SetNumberOfScalarComponents(3); 
	baselineRGB->SetScalarTypeToUnsignedChar(); 
	baselineRGB->Update(); 

	
	for ( int y = 0; y <= tiffReader->GetOutput()->GetExtent()[3]; y++)
	{
		for ( int x = 0; x <= tiffReader->GetOutput()->GetExtent()[1]; x++)
		{
			unsigned char* p24bit = static_cast<unsigned char*> ( baselineRGB->GetScalarPointer(x,y,0) ); 
			unsigned char* p8bit = static_cast<unsigned char*> ( tiffReader->GetOutput()->GetScalarPointer(x,y,0) );
			for ( int components = 0; components < baselineRGB->GetNumberOfScalarComponents(); components++)
			{
				*p24bit++ = *p8bit; 
			}
		}
	}

	vtkSmartPointer<vtkImageData> frameRGB = vtkSmartPointer<vtkImageData>::New(); 
	frameRGB->CopyStructure(imageDataVector->at(inputFrameNumber)); 
	frameRGB->SetNumberOfScalarComponents(3); 
	frameRGB->SetScalarTypeToUnsignedChar(); 
	frameRGB->Update(); 

	for ( int y = 0; y <= imageDataVector->at(inputFrameNumber)->GetExtent()[3]; y++)
	{
		for ( int x = 0; x <= imageDataVector->at(inputFrameNumber)->GetExtent()[1]; x++)
		{
			unsigned char* p24bit = static_cast<unsigned char*> ( frameRGB->GetScalarPointer(x,y,0) ); 
			unsigned char* p8bit = static_cast<unsigned char*> ( imageDataVector->at(inputFrameNumber)->GetScalarPointer(x,y,0) );
			for ( int components = 0; components < frameRGB->GetNumberOfScalarComponents(); components++)
			{
				*p24bit++ = *p8bit; 
			}
		}
	}


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