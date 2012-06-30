/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

#include "PlusConfigure.h"
#include "vtksys/CommandLineArguments.hxx"

#include <iomanip>
#include <iostream>

#include "vtkRfToBrightnessConvert.h"
#include "vtkXMLUtilities.h"
#include "vtkSmartPointer.h"
#include "vtkMetaImageSequenceIO.h"
#include "vtkTrackedFrameList.h"
#include "TrackedFrame.h"
#include "vtkTransform.h"
#include "vtkImageData.h" 


//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
  std::string inputRfFile;
  std::string inputConfigFile;
  std::string outputBrightnessFile;

  int verboseLevel=vtkPlusLogger::LOG_LEVEL_DEFAULT;

  int numberOfFailures(0); 

  vtksys::CommandLineArguments args;
  args.Initialize(argc, argv);

  args.AddArgument("--input-rf-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputRfFile, "File name of input RF image data");
  args.AddArgument("--input-config-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputConfigFile, "Config file containing processing parameters");
  args.AddArgument("--output-brightness-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &outputBrightnessFile, "File name of the generated output brightness image");
  args.AddArgument("--verbose", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &verboseLevel, "Verbose level (1=error only, 2=warning, 3=info, 4=debug, 5=trace)");


  // Input arguments error checking
  if ( !args.Parse() )
  {
    std::cerr << "Problem parsing arguments" << std::endl;
    std::cout << "Help: " << args.GetHelp() << std::endl;
    exit(EXIT_FAILURE);
  }

  vtkPlusLogger::Instance()->SetLogLevel(verboseLevel);

  if (inputConfigFile.empty())
  {
    std::cerr << "--input-config-file required " << std::endl;
    exit(EXIT_FAILURE);
  }
  if (inputRfFile.empty())
  {
    std::cerr << "--input-rf-file required" << std::endl;
    exit(EXIT_FAILURE);
  }
  if (outputBrightnessFile.empty())
  {
    std::cerr << "--output-brightness-file" << std::endl;
    exit(EXIT_FAILURE);
  }

  // Read transformations data 
  LOG_DEBUG("Reading input meta file..."); 
  // frameList it will contain initially the RF data and the image data will be replaced by the processed output
  vtkSmartPointer< vtkTrackedFrameList > frameList = vtkSmartPointer< vtkTrackedFrameList >::New();
  frameList->ReadFromSequenceMetafile( inputRfFile.c_str() );
  LOG_DEBUG("Reading input meta file completed"); 

  // Read config file
  LOG_DEBUG("Reading config file...")
  vtkSmartPointer<vtkXMLDataElement> configRead = vtkSmartPointer<vtkXMLDataElement>::Take(::vtkXMLUtilities::ReadElementFromFile(inputConfigFile.c_str())); 
  LOG_DEBUG("Reading config file finished.");

  // Create converter
  vtkSmartPointer<vtkRfToBrightnessConvert> rfToBrightnessConverter = vtkSmartPointer<vtkRfToBrightnessConvert>::New(); 
  rfToBrightnessConverter->SetNumberOfThreads(1); // just for test
  if ( rfToBrightnessConverter->ReadConfiguration(configRead) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to read conversion parameters from the configuration file"); 
    exit(EXIT_FAILURE); 
  }

  // Process the frames
  for (int i = 0; i<frameList->GetNumberOfTrackedFrames(); i++)
  {
    TrackedFrame* rfFrame = frameList->GetTrackedFrame(i);

    // Do the conversion
    rfToBrightnessConverter->SetInput(rfFrame->GetImageData()->GetVtkImage());
    rfToBrightnessConverter->Update();
    vtkImageData* brightnessImage=rfToBrightnessConverter->GetOutput();

    // Update the pixel data in the frame
    rfFrame->GetImageData()->DeepCopyFrom(brightnessImage);    
  }

  vtkSmartPointer<vtkMetaImageSequenceIO> brightnessSequenceFileWriter = vtkSmartPointer<vtkMetaImageSequenceIO>::New(); 
  brightnessSequenceFileWriter->SetFileName(outputBrightnessFile.c_str()); 
  brightnessSequenceFileWriter->SetTrackedFrameList(frameList); 
  brightnessSequenceFileWriter->Write(); 

	return EXIT_SUCCESS; 
}
