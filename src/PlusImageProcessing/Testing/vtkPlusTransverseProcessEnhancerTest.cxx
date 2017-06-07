/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

/*!
\file vtkPlusTransverseProcessEnhancerTest.cxx
This is a program meant to test vtkPlusTransverseProcessEnhancer.cxx from the command line.
*/

#include "PlusConfigure.h"
#include "vtkPlusTransverseProcessEnhancer.h"

// VTK includes
#include <vtkSmartPointer.h>
#include <vtksys/CommandLineArguments.hxx>


#include "PlusTrackedFrame.h"
#include "vtkPlusTrackedFrameList.h"
#include "vtkImageCast.h"


//----------------------------------------------------------------------------


int main(int argc, char** argv)
{
  bool printHelp = false;
  vtksys::CommandLineArguments args;

  //Setup variables for command line
  std::string inputFileName;
  std::string configFileName;
  std::string outputFileName;
  std::string linesImageFileName;
  std::string intermediateImageFileName;
  std::string shadowImageFileName;
  std::string processedLinesImageFileName;

  int verboseLevel = vtkPlusLogger::LOG_LEVEL_UNDEFINED;

  //Get command line arguments
  args.Initialize(argc, argv);
  args.AddArgument("--help", vtksys::CommandLineArguments::NO_ARGUMENT, &printHelp, "Print this help");
  args.AddArgument("--input-seq-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputFileName, "The filename for the input ultrasound sequence to process.");
  args.AddArgument("--config-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &configFileName, "The filename for input config file.");
  args.AddArgument("--output-seq-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &outputFileName, "The filename to write the processed sequence to.");
  args.AddArgument("--lines-image-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &linesImageFileName, "Optional output files for subsampled lines input image");
  args.AddArgument("--intermediate-image-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &intermediateImageFileName, "Optional output file for intermediate data");
  args.AddArgument("--shadow-image-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &shadowImageFileName, "Optional output file for shadow image");
  args.AddArgument("--processedlines-image-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &processedLinesImageFileName, "Optional output files for processed subsampled image");
  args.AddArgument("--verbose", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &verboseLevel, "Verbose level (1=error only, 2=warning, 3=info, 4=debug, 5=trace)");

  if (!args.Parse())
  {
    std::cerr << "Problem parsing arguments" << std::endl;
    std::cout << "Help: " << args.GetHelp() << std::endl;
    exit(EXIT_FAILURE);
  }

  vtkPlusLogger::Instance()->SetLogLevel(verboseLevel);

  //check for various non-optional command line arguments
  int commandCheckStatus = NULL;

  if (inputFileName.empty())
  {
    std::cerr << "--input-seq-file not found!" << std::endl;
    commandCheckStatus = EXIT_FAILURE;
  }
  if (outputFileName.empty())
  {
    std::cerr << "--output-seq-file not found!" << std::endl;
    commandCheckStatus = EXIT_FAILURE;
  }
  if (configFileName.empty())
  {
    std::cerr << "--config-file not found!" << std::endl;
    commandCheckStatus = EXIT_FAILURE;
  }

  if (commandCheckStatus == EXIT_FAILURE)
  {
    return EXIT_FAILURE;
  }

  vtkSmartPointer<vtkPlusTransverseProcessEnhancer> enhancer = vtkSmartPointer<vtkPlusTransverseProcessEnhancer>::New();

  // Read input sequence
  vtkSmartPointer<vtkPlusTrackedFrameList> trackedFrameList = vtkSmartPointer<vtkPlusTrackedFrameList>::New();
  if (trackedFrameList->ReadFromSequenceMetafile(inputFileName.c_str()) == PLUS_FAIL)
  {
    return EXIT_FAILURE;
  }
  vtkSmartPointer<vtkImageCast> castToUchar = vtkSmartPointer<vtkImageCast>::New();
  castToUchar->SetOutputScalarTypeToUnsignedChar();

  int numberOfFrames = trackedFrameList->GetNumberOfTrackedFrames();
  std::cout << "Number of frames in input: " << numberOfFrames << std::endl;


  //Check methods of vtkPlusTransverseProcessEnhancer for failure

  enhancer->SetInputFrames(trackedFrameList);

  //test the abillity to read to the config file
  LOG_DEBUG("Reading config file...");
  std::cout << "Reading Config file." << std::endl;
  vtkSmartPointer<vtkXMLDataElement> configRootElement = vtkSmartPointer<vtkXMLDataElement>::New(); 
  if (PlusXmlUtils::ReadDeviceSetConfigurationFromFile(configRootElement, configFileName.c_str()) == PLUS_FAIL)
  {
    LOG_ERROR("Unable to read configuration from file " << configFileName.c_str());
    std::cout << "Failed to read Config file" << std::endl;

    return EXIT_FAILURE;
  }

  vtkXMLDataElement* processorElement = configRootElement->FindNestedElementWithName("ScanConversion");
  if (processorElement == NULL)
  {
    LOG_ERROR("Cannot find device set in XML tree element: " << ((*configRootElement).GetName()));
    return PLUS_FAIL;
  }

  if (enhancer->ReadConfiguration(configRootElement) == PLUS_FAIL)
  {
    LOG_ERROR("Unable to read configuration from file " << configFileName.c_str());
    std::cout << "Failed to read Config file" << std::endl;
    return EXIT_FAILURE;
  }
  LOG_DEBUG("Reading config file finished.");
  std::cout << "Done reading Config file." << std::endl;

  //Process the frames for the input file
  LOG_DEBUG("attempting to Process Frames.");
  std::cout << "Processing Frame." << std::endl;
  if (enhancer->Update() == PLUS_FAIL)
  {
    LOG_ERROR("Could not Process Frames.");
    std::cout << "Failed processing frames." << std::endl;
    return EXIT_FAILURE;
  }
  LOG_DEBUG("Processed Frames terminated successfully.");
  std::cout << "Processing Frame successfully." << std::endl;

  //write various outputs to their respective files
  if (linesImageFileName.empty() == false)
  {
    enhancer->SetLinesImageFileName(linesImageFileName);
    if (enhancer->GetLinesFrameList()->SaveToSequenceMetafile(linesImageFileName.c_str()) == PLUS_FAIL)
    {
      LOG_ERROR("An issue occured when trying to save the lines image to file: " << linesImageFileName.c_str());
      return EXIT_FAILURE;
    }
  }
  if (intermediateImageFileName.empty() == false)
  {
    enhancer->SetLinesImageFileName(intermediateImageFileName);
    if (enhancer->GetLinesFrameList()->SaveToSequenceMetafile(intermediateImageFileName.c_str()) == PLUS_FAIL)
    {
      LOG_ERROR("An issue occured when trying to save the intermediate image to file: " << intermediateImageFileName.c_str());
      return EXIT_FAILURE;
    }
  }
  if (shadowImageFileName.empty() == false)
  {
    enhancer->SetLinesImageFileName(shadowImageFileName);
    if (enhancer->GetLinesFrameList()->SaveToSequenceMetafile(shadowImageFileName.c_str()) == PLUS_FAIL)
    {
      LOG_ERROR("An issue occured when trying to save the shadow image to file: " << shadowImageFileName.c_str());
      return EXIT_FAILURE;
    }
  }
  if (processedLinesImageFileName.empty() == false)
  {
    enhancer->SetLinesImageFileName(processedLinesImageFileName);
    if (enhancer->GetLinesFrameList()->SaveToSequenceMetafile(processedLinesImageFileName.c_str()) == PLUS_FAIL)
    {
      LOG_ERROR("An issue occured when trying to save the processed lines image to file: " << processedLinesImageFileName.c_str());
      return EXIT_FAILURE;
    }
  }

  enhancer->GetOutputFrames()->SaveToSequenceMetafile(outputFileName.c_str());


  std::cout << "Completed Test Successfully." << std::endl;
  return EXIT_SUCCESS;
}