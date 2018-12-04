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
#include <vtkPlusSequenceIO.h>

// VTK includes
#include "vtkImageCast.h"
#include <vtkSmartPointer.h>
#include <vtksys/CommandLineArguments.hxx>

// IGSIO includes
#include <igsioTrackedFrame.h>
#include <vtkIGSIOTrackedFrameList.h>

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
  bool saveIntermediateResults = false;
  int verboseLevel = vtkIGSIOLogger::LOG_LEVEL_UNDEFINED;

  //Get command line arguments
  args.Initialize(argc, argv);
  args.AddArgument("--help", vtksys::CommandLineArguments::NO_ARGUMENT, &printHelp, "Print this help");
  args.AddArgument("--input-seq-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputFileName, "The filename for the input ultrasound sequence to process.");
  args.AddArgument("--input-config-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputConfigFileName, "The filename for input config file.");
  args.AddArgument("--output-config-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &outputConfigFileName, "Optional filename for output config file. Creates new config file with paramaters used during this test");
  args.AddArgument("--output-seq-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &outputFileName, "The filename to write the processed sequence to.");
  args.AddArgument("--save-intermediate-images", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &saveIntermediateResults, "If intermediate images should be saved to output files");
  args.AddArgument("--verbose", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &verboseLevel, "Verbose level (1=error only, 2=warning, 3=info, 4=debug, 5=trace)");

  if (!args.Parse())
  {
    LOG_ERROR("Problem parsing arguments");
    LOG_INFO("Help: " << args.GetHelp());
    exit(EXIT_FAILURE);
  }

  vtkIGSIOLogger::Instance()->SetLogLevel(verboseLevel);

  //check for various non-optional command line arguments
  int commandCheckStatus = EXIT_SUCCESS;

  if (inputFileName.empty())
  {
    LOG_ERROR("The argument --input-seq-file is required");
    commandCheckStatus = EXIT_FAILURE;
  }
  if (inputConfigFileName.empty())
  {
    LOG_ERROR("The argument --input-config-file is required");
    commandCheckStatus = EXIT_FAILURE;
  }
  if (outputFileName.empty())
  {
    LOG_ERROR("The argument --output-seq-file is required");
    commandCheckStatus = EXIT_FAILURE;
  }

  if (commandCheckStatus == EXIT_FAILURE)
  {
    return EXIT_FAILURE;
  }

  vtkSmartPointer<vtkPlusTransverseProcessEnhancer> enhancer = vtkSmartPointer<vtkPlusTransverseProcessEnhancer>::New();

  // Read input sequence
  vtkSmartPointer<vtkIGSIOTrackedFrameList> trackedFrameList = vtkSmartPointer<vtkIGSIOTrackedFrameList>::New();
  if (vtkPlusSequenceIO::Read(inputFileName, trackedFrameList) == PLUS_FAIL)
  {
    return EXIT_FAILURE;
  }
  vtkSmartPointer<vtkImageCast> castToUchar = vtkSmartPointer<vtkImageCast>::New();
  castToUchar->SetOutputScalarTypeToUnsignedChar();

  int numberOfFrames = trackedFrameList->GetNumberOfTrackedFrames();
  LOG_INFO("Number of frames in input: " << numberOfFrames);

  enhancer->SetInputFrames(trackedFrameList);

  LOG_INFO("Reading Config file");
  vtkSmartPointer<vtkXMLDataElement> configRootElement = vtkSmartPointer<vtkXMLDataElement>::New();
  if (PlusXmlUtils::ReadDeviceSetConfigurationFromFile(configRootElement, inputConfigFileName.c_str()) == PLUS_FAIL)
  {
    LOG_ERROR("Unable to read configuration from file " << inputConfigFileName);
    return EXIT_FAILURE;
  }

  vtkSmartPointer<vtkXMLDataElement> elementToUse;

  // Find the ScanConversion element in the XML file
  vtkSmartPointer<vtkXMLDataElement> processorElement = configRootElement->FindNestedElementWithName("ScanConversion");
  if (processorElement == NULL)
  {
    processorElement = configRootElement->LookupElementWithName("Processor");
    if (processorElement != NULL){
      vtkSmartPointer<vtkXMLDataElement> scanConversionElement = processorElement->FindNestedElementWithName("ScanConversion");
      if (scanConversionElement == NULL){
        LOG_ERROR("Cannot find device set in XML tree element: ScanConversion");
        return PLUS_FAIL;
      }
      else
      {
        elementToUse = processorElement;
      }
    }
    else
    {
      // If this cannot be found, the algorithm cannot work
      LOG_ERROR("Cannot find device set in XML tree: Processor");
      return PLUS_FAIL;
    }
  }
  else
  {
    elementToUse = configRootElement;
  }

  if (enhancer->ReadConfiguration(elementToUse) == PLUS_FAIL)
  {
    LOG_ERROR("Unable to read configuration from file " << inputConfigFileName);
    return EXIT_FAILURE;
  }
  LOG_INFO("Reading config file finished");

  // Process the frames for the input file
  enhancer->SetSaveIntermediateResults(saveIntermediateResults);
  LOG_INFO("Processing frames...");

  if (enhancer->Update() == PLUS_FAIL)
  {
    LOG_ERROR("Processing frames failed!");
    return EXIT_FAILURE;
  }

  LOG_INFO("Processing frames successful");
  if (saveIntermediateResults)
  {
    // Find out where to add the unique suffix for each intermediate image
    int startInputFileNameIndex = 0;
    if (inputFileName.find("/") != std::string::npos)
    {
      startInputFileNameIndex = inputFileName.rfind("/") + 1;
    }
    if (inputFileName.find("\\") != std::string::npos)
    {
      startInputFileNameIndex = inputFileName.rfind("\\") + 1;
    }
    int startOutputFileNameIndex = 0;
    if (outputFileName.find("/") != std::string::npos)
    {
      startOutputFileNameIndex = outputFileName.rfind("/") + 1;
    }
    if (outputFileName.find("\\") != std::string::npos)
    {
      startOutputFileNameIndex = outputFileName.rfind("\\") + 1;
    }

    // Saves the intermediate results that were recorded during the call to enhancer->Update()
    enhancer->SetIntermediateImageFileName(
      outputFileName.substr(0, startOutputFileNameIndex) + inputFileName.substr(startInputFileNameIndex, inputFileName.find(".") - startInputFileNameIndex));
    enhancer->SaveAllIntermediateResultsToFile();
  }

  if (vtkPlusSequenceIO::Write(outputFileName, enhancer->GetOutputFrames()) == PLUS_FAIL)
  {
    LOG_ERROR("Could not save output sequence to the file: " << outputFileName);
    return EXIT_FAILURE;
  }

  // Test the ability to Write to the config file
  if (!outputConfigFileName.empty())
  {
    //Start the xml tree that will be written to
    vtkSmartPointer<vtkXMLDataElement> processorWriteData = vtkSmartPointer<vtkXMLDataElement>::New();
    processorWriteData->SetName("Processor");

    //Write to the config file
    LOG_INFO("Writing to config file.");
    if (enhancer->WriteConfiguration(processorWriteData) == PLUS_FAIL)
    {
      LOG_ERROR("Unable to write to config file.");
      return EXIT_FAILURE;
    }
    if (igsioCommon::XML::PrintXML(outputConfigFileName, processorWriteData) == PLUS_FAIL)
    {
      LOG_ERROR("An error occured when trying to save to the config file " << outputConfigFileName);
      return EXIT_FAILURE;
    }
    LOG_INFO("Writing to config file finished successfully.");
  }

  LOG_INFO("Completed Test Successfully.");
  return EXIT_SUCCESS;
}
