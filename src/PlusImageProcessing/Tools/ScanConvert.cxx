#include "PlusConfigure.h"
#include "igsioTrackedFrame.h"
#include "vtkSmartPointer.h"
#include "vtksys/CommandLineArguments.hxx"
#include "vtkIGSIOTrackedFrameList.h"
#include "vtkPlusUsScanConvert.h"
#include "vtkPlusUsScanConvertCurvilinear.h"
#include "vtkPlusUsScanConvertLinear.h"
#include "vtkPlusSequenceIO.h"

int main(int argc, char **argv)
{
  bool printHelp = false;
  vtksys::CommandLineArguments args;
  
  std::string inputFileName;
  std::string outputFileName;
  std::string configFileName;
  int verboseLevel=vtkPlusLogger::LOG_LEVEL_UNDEFINED;

  args.Initialize(argc, argv);
  args.AddArgument("--help", vtksys::CommandLineArguments::NO_ARGUMENT, &printHelp, "Print this help");
  args.AddArgument("--input-seq-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputFileName, "The filename for the input ultrasound sequence to process.");
  args.AddArgument("--config-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &configFileName, "The filename for input config file.");
  args.AddArgument("--output-seq-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &outputFileName, "The filename to write the processed sequence to.");
  args.AddArgument("--verbose", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &verboseLevel, "Verbose level (1=error only, 2=warning, 3=info, 4=debug, 5=trace)");

  if (!args.Parse())
  {
    std::cerr << "Error parsing arguments" << std::endl;
    std::cout << "Help: " << args.GetHelp() << std::endl;
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
    std::cerr << "--input-seq-file not found!" << std::endl;
    return EXIT_FAILURE;
  }
  
  if (configFileName.empty())
  {
    std::cerr << "--config-file not found!" << std::endl;
    return EXIT_FAILURE;
  }
  
  if (outputFileName.empty())
  {
    std::cerr << "--output-seq-file not found!" << std::endl;
    return EXIT_FAILURE;
  }

  // Read config file.

  vtkSmartPointer<vtkXMLDataElement> configRootElement = vtkSmartPointer<vtkXMLDataElement>::New();
  if (PlusXmlUtils::ReadDeviceSetConfigurationFromFile(configRootElement, configFileName.c_str())==PLUS_FAIL)
  {  
    LOG_ERROR("Unable to read configuration from file " << configFileName.c_str()); 
    return EXIT_FAILURE;
  }

  vtkXMLDataElement* scanConversionElement = configRootElement->FindNestedElementWithName("ScanConversion");
  if (scanConversionElement == NULL)
  {
    LOG_ERROR("Cannot find ScanConversion element in XML tree!");
    return PLUS_FAIL;
  }

  const char* transducerGeometry = scanConversionElement->GetAttribute("TransducerGeometry");
  if (transducerGeometry == NULL)
  {
    LOG_ERROR("Scan converter TransducerGeometry is undefined!");
    return PLUS_FAIL;
  }

  // Create scan converter.

  vtkSmartPointer<vtkPlusUsScanConvert> scanConverter;
  if (STRCASECMP(transducerGeometry, "CURVILINEAR")==0)
  {
    scanConverter = vtkSmartPointer<vtkPlusUsScanConvert>::Take(vtkPlusUsScanConvertCurvilinear::New());
  }
  else if (STRCASECMP(transducerGeometry, "LINEAR")==0)
  {
    scanConverter = vtkSmartPointer<vtkPlusUsScanConvert>::Take(vtkPlusUsScanConvertLinear::New());
  }
  else
  {
    LOG_ERROR("Invalid scan converter TransducerGeometry: " << transducerGeometry);
    return PLUS_FAIL;
  }
  scanConverter->ReadConfiguration(scanConversionElement);
  
  // Read input image.

  vtkSmartPointer<vtkIGSIOTrackedFrameList> inputFrameList = vtkSmartPointer<vtkIGSIOTrackedFrameList>::New();
  vtkPlusSequenceIO::Read(inputFileName.c_str(), inputFrameList);
  int numberOfFrames = inputFrameList->GetNumberOfTrackedFrames();
  
  // Create output frame list.

  vtkSmartPointer<vtkIGSIOTrackedFrameList> outputFrameList = vtkSmartPointer<vtkIGSIOTrackedFrameList>::New();

  // Iterate thought every frame.

  for (int frameIndex = 0; frameIndex < numberOfFrames; frameIndex ++ )
  {
    igsioTrackedFrame* inputFrame = inputFrameList->GetTrackedFrame(frameIndex);
    
    scanConverter->SetInputData( inputFrame->GetImageData()->GetImage() );
    scanConverter->Update();
    
    // Allocate and store output image.

    outputFrameList->AddTrackedFrame(inputFrame);
    igsioTrackedFrame* outputFrame = outputFrameList->GetTrackedFrame(outputFrameList->GetNumberOfTrackedFrames()-1);
    outputFrame->GetImageData()->DeepCopyFrom(scanConverter->GetOutput());
  }

  std::cout << "Writing output to file. Setting log level to error only, regardless of user specified verbose level." << std::endl;
  vtkPlusLogger::Instance()->SetLogLevel(1);
  
  vtkPlusSequenceIO::Write(outputFileName.c_str(), outputFrameList);

  return EXIT_SUCCESS;
}



