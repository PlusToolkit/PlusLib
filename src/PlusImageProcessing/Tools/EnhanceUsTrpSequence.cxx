#include "PlusConfigure.h"
#include "PlusTrackedFrame.h"
#include "vtkImageCast.h"
#include "vtkImageData.h"
#include "vtkPlusMetaImageSequenceIO.h"
#include "vtkSmartPointer.h"
#include "vtkPlusTrackedFrameList.h"
#include "vtkPlusTransverseProcessEnhancer.h"

#include "vtksys/CommandLineArguments.hxx"

#include "string"

int main(int argc, char **argv)
{
  bool printHelp = false;
  vtksys::CommandLineArguments args;
  
  std::string inputFileName;
  std::string outputFileName;
  std::string configFileName;
  std::string linesImageFileName;
  std::string shadowImageFileName;
  std::string intermediateImageFileName;
  std::string processedLinesImageFileName;
  int verboseLevel=vtkPlusLogger::LOG_LEVEL_UNDEFINED;

  args.Initialize(argc, argv);
  args.AddArgument("--help", vtksys::CommandLineArguments::NO_ARGUMENT, &printHelp, "Print this help");
  args.AddArgument("--input-seq-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputFileName, "The filename for the input ultrasound sequence to process.");
  args.AddArgument("--config-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &configFileName, "The filename for input config file.");
  args.AddArgument("--output-seq-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &outputFileName, "The filename to write the processed sequence to.");
  args.AddArgument("--lines-image-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &linesImageFileName, "Optional output files for subsampled input image");
  args.AddArgument("--shadow-image-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &shadowImageFileName, "Optional output files for shadow images");
  args.AddArgument("--intermediate-image-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &intermediateImageFileName, "Optional output file");
  args.AddArgument("--processedlines-image-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &processedLinesImageFileName, "Optional output files for processed subsampled image");
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
    //std::cout << args.GetHelp() << std::endl;
    LOG_ERROR(args.GetHelp());
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

  vtkSmartPointer<vtkPlusTrackedFrameList> trackedFrameList = vtkSmartPointer<vtkPlusTrackedFrameList>::New();
  trackedFrameList->ReadFromSequenceMetafile(inputFileName.c_str());

  vtkSmartPointer<vtkImageCast> castToUchar = vtkSmartPointer<vtkImageCast>::New();
  castToUchar->SetOutputScalarTypeToUnsignedChar();


  int numberOfFrames = trackedFrameList->GetNumberOfTrackedFrames();
  LOG_INFO("Number of frames in input: " << numberOfFrames);
  //std::cout << "Number of frames in input: " << numberOfFrames << std::endl;

  // Bone filter.
  
  vtkSmartPointer<vtkPlusTransverseProcessEnhancer> boneFilter = vtkSmartPointer<vtkPlusTransverseProcessEnhancer>::New();
  
  boneFilter->SetInputFrames(trackedFrameList);
  boneFilter->ReadConfiguration(processorElement);

  PlusStatus filterStatus = boneFilter->Update();
  if (filterStatus != PlusStatus::PLUS_SUCCESS)
  {
    //std::cout << "Failed processing frames" << std::endl;
    LOG_ERROR("Failed processing frames");
    return EXIT_FAILURE;
  }

  LOG_INFO("Writing output to file.");
  vtkPlusLogger::Instance()->SetLogLevel(3);
  if (! linesImageFileName.empty())
  {
    boneFilter->SetLinesImageFileName( linesImageFileName );
    boneFilter->GetLinesFrameList()->SaveToSequenceMetafile(linesImageFileName.c_str());
    //boneFilter->GetLinesImageList()->SaveToSequenceMetafile(linesImageFileName.c_str());
  }

  if (!shadowImageFileName.empty())
  {
    boneFilter->SetShadowImageFileName(shadowImageFileName);
    boneFilter->GetShadowFrameList()->SaveToSequenceMetafile(shadowImageFileName.c_str());
    //boneFilter->GetShadowImageList()->SaveToSequenceMetafile(shadowImageFileName.c_str());
  }

  if (!intermediateImageFileName.empty())
  {
    boneFilter->SetIntermediateImageFileName(intermediateImageFileName);
    boneFilter->GetIntermediateFrameList()->SaveToSequenceMetafile(intermediateImageFileName.c_str());
    //boneFilter->GetIntermediateImageList()->SaveToSequenceMetafile(intermediateImageFileName.c_str());
  }

  if (! processedLinesImageFileName.empty())
  {
    boneFilter->SetProcessedLinesImageFileName( processedLinesImageFileName );
    boneFilter->GetProcessedLinesFrameList()->SaveToSequenceMetafile(processedLinesImageFileName.c_str());
    //boneFilter->GetProcessedLinesImageList()->SaveToSequenceMetafile(processedLinesImageFileName.c_str());
  }

  //std::cout << "Writing output to file." << std::endl;
  boneFilter->GetOutputFrames()->SaveToSequenceMetafile(outputFileName.c_str());
  
  return EXIT_SUCCESS;
}