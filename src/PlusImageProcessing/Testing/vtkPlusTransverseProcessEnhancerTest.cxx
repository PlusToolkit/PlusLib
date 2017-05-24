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
  std::string inputConfigFileName;
  std::string outputConfigFileName;
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
  args.AddArgument("--input-config-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputConfigFileName, "The filename for input config file.");
  args.AddArgument("--output-config-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &outputConfigFileName, "Optional filename for output config file. Creates new config file with paramaters used during this test");
  args.AddArgument("--output-seq-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &outputFileName, "The filename to write the processed sequence to.");
  args.AddArgument("--output-lines-image-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &linesImageFileName, "Optional output files for subsampled lines input image");
  args.AddArgument("--output-intermediate-image-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &intermediateImageFileName, "Optional output file for intermediate data");
  args.AddArgument("--output-shadow-image-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &shadowImageFileName, "Optional output file for shadow image");
  args.AddArgument("--output-processedlines-image-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &processedLinesImageFileName, "Optional output files for processed subsampled image");
  args.AddArgument("--verbose", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &verboseLevel, "Verbose level (1=error only, 2=warning, 3=info, 4=debug, 5=trace)");

  if (!args.Parse())
  {
    LOG_ERROR("Problem parsing arguments");
    LOG_ERROR("Help: " << args.GetHelp());
    exit(EXIT_FAILURE);
  }

  vtkPlusLogger::Instance()->SetLogLevel(verboseLevel);

  //check for various non-optional command line arguments
  int commandCheckStatus = NULL;

  if (inputFileName.empty())
  {
    LOG_ERROR("the argument --input-seq-file is required");
    commandCheckStatus = EXIT_FAILURE;
  }
  if (inputConfigFileName.empty())
  {
    LOG_ERROR("the argument --input-config-file is required");
    commandCheckStatus = EXIT_FAILURE;
  }
  if (outputFileName.empty())
  {
    LOG_ERROR("the argument --output-seq-file is required");
    commandCheckStatus = EXIT_FAILURE;
  }

  if (commandCheckStatus == EXIT_FAILURE)
  {
    return EXIT_FAILURE;
  }

  vtkSmartPointer<vtkPlusTransverseProcessEnhancer> enhancer = vtkSmartPointer<vtkPlusTransverseProcessEnhancer>::New();

  // Read input sequence
  vtkSmartPointer<vtkPlusTrackedFrameList> trackedFrameList = vtkSmartPointer<vtkPlusTrackedFrameList>::New();
  if (trackedFrameList->ReadFromSequenceMetafile(inputFileName) == PLUS_FAIL)
  {
    return EXIT_FAILURE;
  }
  vtkSmartPointer<vtkImageCast> castToUchar = vtkSmartPointer<vtkImageCast>::New();
  castToUchar->SetOutputScalarTypeToUnsignedChar();

  int numberOfFrames = trackedFrameList->GetNumberOfTrackedFrames();
  LOG_INFO("Number of frames in input: " << numberOfFrames);

  //Check methods of vtkPlusTransverseProcessEnhancer for failure

  enhancer->SetInputFrames(trackedFrameList);


  //test the abillity to read to the config file
  LOG_INFO("Reading Config file.");
  vtkSmartPointer<vtkXMLDataElement> configRootElement = vtkSmartPointer<vtkXMLDataElement>::New();
  if (PlusXmlUtils::ReadDeviceSetConfigurationFromFile(configRootElement, inputConfigFileName.c_str()) == PLUS_FAIL)
  {
    LOG_ERROR("Unable to read configuration from file " << inputConfigFileName);
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
    LOG_ERROR("Unable to read configuration from file " << inputConfigFileName);
    return EXIT_FAILURE;
  }
  LOG_INFO("Reading config file finished.");

  //Process the frames for the input file
  LOG_INFO("attempting to Process Frames.");
  if (enhancer->Update() == PLUS_FAIL)
  {
    LOG_ERROR("Could not Process Frames.");
    return EXIT_FAILURE;
  }
  LOG_INFO("Processed Frames terminated successfully.");

  //save various outputs file names
  enhancer->SetLinesImageFileName(linesImageFileName);
  enhancer->SetIntermediateImageFileName(intermediateImageFileName);
  enhancer->SetShadowImageFileName(shadowImageFileName);
  enhancer->SetProcessedLinesImageFileName(processedLinesImageFileName);

  enhancer->GetOutputFrames()->SaveToSequenceMetafile(outputFileName);

  //test the abillity to Write to the config file
  if (!outputConfigFileName.empty())
  {
    //Start the xml tree that will be written to
    vtkSmartPointer<vtkXMLDataElement> processorWriteData = vtkSmartPointer<vtkXMLDataElement>::New();
    processorWriteData->SetName("Processor");

    //Write to the config file
    LOG_DEBUG("Writing to config file...");
    if (enhancer->WriteConfiguration(processorWriteData) == PLUS_FAIL)
    {
      LOG_ERROR("Unable to write to config file.");
      return EXIT_FAILURE;
    }
    if (PlusCommon::XML::PrintXML(outputConfigFileName, processorWriteData) == PLUS_FAIL)
    {
      LOG_ERROR("An error occured when trying to save to the config file " << outputConfigFileName);
      return EXIT_FAILURE;
    }
    LOG_DEBUG("Writing to config file finished.");
  }

  LOG_INFO("Completed Test Successfully.");
  return EXIT_SUCCESS;
}