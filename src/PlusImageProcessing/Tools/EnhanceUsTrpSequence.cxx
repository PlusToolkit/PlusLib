#include "PlusConfigure.h"
#include "igsioTrackedFrame.h"
#include "vtkImageCast.h"
#include "vtkImageData.h"
#include "vtkSmartPointer.h"
#include "vtkPlusTransverseProcessEnhancer.h"
#include <vtkPlusSequenceIO.h>

#include <vtkIGSIOTrackedFrameList.h>
#include <vtkIGSIOMetaImageSequenceIO.h>

#include "vtksys/CommandLineArguments.hxx"

#include "string"

int main(int argc, char **argv)
{
  bool printHelp = false;
  vtksys::CommandLineArguments args;
  
  std::string inputFileName;
  std::string outputFileName;
  std::string configFileName;
  bool saveIntermediateResults = false;
  int verboseLevel=vtkPlusLogger::LOG_LEVEL_UNDEFINED;

  args.Initialize(argc, argv);
  args.AddArgument("--help", vtksys::CommandLineArguments::NO_ARGUMENT, &printHelp, "Print this help");
  args.AddArgument("--input-seq-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputFileName, "The filename for the input ultrasound sequence to process.");
  args.AddArgument("--config-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &configFileName, "The filename for input config file.");
  args.AddArgument("--output-seq-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &outputFileName, "The filename to write the processed sequence to.");
  args.AddArgument("--save-intermediate-images", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &saveIntermediateResults, "If intermediate images should be saved to output files");
  args.AddArgument("--verbose", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &verboseLevel, "Verbose level (1=error only, 2=warning, 3=info, 4=debug, 5=trace)");

  if (!args.Parse())
  {
    LOG_ERROR("Error parsing arguments");
    LOG_ERROR("Help: " << args.GetHelp());
    //std::cerr << "Error parsing arguments" << std::endl;
    //std::cout << "Help: " << args.GetHelp() << std::endl;
    exit(EXIT_FAILURE);
  }

  if (printHelp)
  {
    std::cout << args.GetHelp() << std::endl;
    return EXIT_SUCCESS;
  }

  vtkPlusLogger::Instance()->SetLogLevel(verboseLevel);

  if (inputFileName.empty())
  {
    //std::cerr << "--input-seq-file not found!" << std::endl;
    LOG_ERROR("--input-seq-file not found!");
    return EXIT_FAILURE;
  }
  
  if (configFileName.empty())
  {
    //std::cerr << "--config-file not found!" << std::endl;
    LOG_ERROR("--config-file not found!");
    return EXIT_FAILURE;
  }
  
  if (outputFileName.empty())
  {
    //std::cerr << "--output-seq-file not found!" << std::endl;
    LOG_ERROR("--output-seq-file not found!");
    return EXIT_FAILURE;
  }

  // Read config file

  LOG_DEBUG("Reading config file...")
  vtkSmartPointer<vtkXMLDataElement> configRootElement = vtkSmartPointer<vtkXMLDataElement>::New();
  if (PlusXmlUtils::ReadDeviceSetConfigurationFromFile(configRootElement, configFileName.c_str())==PLUS_FAIL)
  {  
    LOG_ERROR("Unable to read configuration from file " << configFileName.c_str()); 
    return EXIT_FAILURE;
  }
  LOG_DEBUG("Reading config file finished.");

  vtkXMLDataElement* dataCollectionElement = configRootElement->FindNestedElementWithName("DataCollection");
  if (dataCollectionElement == NULL)
  {
    LOG_ERROR("Cannot find device set in XML tree element: " << ((*configRootElement).GetName()));
    return PLUS_FAIL;
  }
  
  vtkXMLDataElement* boneEnhancerElement = dataCollectionElement->FindNestedElementWithNameAndAttribute("Device", "Type", "ImageProcessor");
  if (boneEnhancerElement == NULL)
  {
    LOG_ERROR("Cannot find BoneEnhancer element in XML tree element: " << ((*dataCollectionElement).GetName()));
    return PLUS_FAIL;
  }

  vtkXMLDataElement* processorElement = boneEnhancerElement->FindNestedElementWithNameAndAttribute("Processor", "Type", "vtkPlusTransverseProcessEnhancer");
  if (processorElement == NULL)
  {
    LOG_ERROR("Cannot find image Processor vtkPlusTransverseProcessEnhancer config in XML element: " << ((*boneEnhancerElement).GetName()));
    return PLUS_FAIL;
  }

  // Read the input sequence.

  vtkSmartPointer<vtkIGSIOTrackedFrameList> trackedFrameList = vtkSmartPointer<vtkIGSIOTrackedFrameList>::New();
  vtkPlusSequenceIO::Read(inputFileName.c_str(), trackedFrameList);

  vtkSmartPointer<vtkImageCast> castToUchar = vtkSmartPointer<vtkImageCast>::New();
  castToUchar->SetOutputScalarTypeToUnsignedChar();


  int numberOfFrames = trackedFrameList->GetNumberOfTrackedFrames();
  LOG_INFO("Number of frames in input: " << numberOfFrames);

  // Bone filter.
  
  vtkSmartPointer<vtkPlusTransverseProcessEnhancer> boneFilter = vtkSmartPointer<vtkPlusTransverseProcessEnhancer>::New();
  
  boneFilter->SetInputFrames(trackedFrameList);
  boneFilter->ReadConfiguration(processorElement);

  PlusStatus filterStatus = boneFilter->Update();
  if (filterStatus != PlusStatus::PLUS_SUCCESS)
  {
    LOG_ERROR("Failed processing frames");
    return EXIT_FAILURE;
  }

  LOG_INFO("Writing output to file");

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

    // Saves the intermediate results that were recorded during the call to boneFilter->Update()
    boneFilter->SetIntermediateImageFileName(
      outputFileName.substr(0, startOutputFileNameIndex) + inputFileName.substr(startInputFileNameIndex, inputFileName.find(".") - startInputFileNameIndex) );
    boneFilter->SaveAllIntermediateResultsToFile();
  }

  if (vtkPlusSequenceIO::Write(outputFileName.c_str(), boneFilter->GetOutputFrames())== PLUS_FAIL)
  {
    LOG_ERROR("Could not save output sequence to the file: " << outputFileName);
    return EXIT_FAILURE;
  }  
  return EXIT_SUCCESS;
}