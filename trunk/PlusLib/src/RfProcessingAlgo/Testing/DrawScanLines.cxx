#include "PlusConfigure.h"
#include "vtksys/CommandLineArguments.hxx"

#include "vtkSmartPointer.h"
#include "vtkMetaImageReader.h"
#include "vtkXMLUtilities.h"
#include "vtkTrackedFrameList.h"
#include "vtkImageData.h"
#include "vtkMetaImageWriter.h"
#include "vtkUsScanConvertLinear.h"
#include "vtkUsScanConvertCurvilinear.h"
#include "vtkLineSource.h"
#include "PlusVideoFrame.h"
#include "TrackedFrame.h"

float DRAWING_COLOR = 255;

//----------------------------------------------------------------------------
void DrawLine(vtkImageData* imageData, int* imageExtent, double* start, double* end, int numberOfPoints)
{
  double directionVectorX = static_cast<double>(end[0]-start[0])/(numberOfPoints-1);
  double directionVectorY = static_cast<double>(end[1]-start[1])/(numberOfPoints-1);
  for (int pointIndex=0; pointIndex<numberOfPoints; ++pointIndex)
  {
    int pixelCoordX = start[0] + directionVectorX * pointIndex;
    int pixelCoordY = start[1] + directionVectorY * pointIndex;
    if (pixelCoordX<imageExtent[0] ||  pixelCoordX>imageExtent[1]
    || pixelCoordY<imageExtent[2] ||  pixelCoordY>imageExtent[3])
    {
      // outside of the specified extent
      continue;
    }
    imageData->SetScalarComponentFromFloat(pixelCoordX, pixelCoordY, 0, 0, DRAWING_COLOR);
  }
}

//----------------------------------------------------------------------------
void DrawScanLines(vtkUsScanConvert* scanConverter, vtkTrackedFrameList* trackedFrameList)
{
  int *rfImageExtent = scanConverter->GetInputImageExtent();
  int numOfSamplesPerScanline = rfImageExtent[1]-rfImageExtent[0]+1;
  int numOfScanlines = rfImageExtent[3]-rfImageExtent[2]+1;

  int numberOfFrames = trackedFrameList->GetNumberOfTrackedFrames();
  LOG_INFO("Processing "<<numberOfFrames<<" frames...");
  for (int frameIndex = 0; frameIndex < numberOfFrames; frameIndex++)
  {
    TrackedFrame* frame = trackedFrameList->GetTrackedFrame(frameIndex);
    vtkImageData* imageData = frame->GetImageData()->GetImage();
    int* outputExtent = imageData->GetExtent();
    for (int scanLine = 0; scanLine < numOfScanlines; scanLine++)
    {
      double start[4] = {0};
      double end[4] = {0};
      scanConverter->GetScanLineEndPoints(scanLine,start,end);
      DrawLine(imageData, outputExtent, start, end, numOfSamplesPerScanline);
    }
  }
}

//----------------------------------------------------------------------------
int main(int argc, char** argv)
{
  // Setup for command line arguments
  bool printHelp(false);
  std::string inputImgSeqFileName;
  std::string outputImgSeqFileName;
  std::string inputConfigFileName;
  int verboseLevel = vtkPlusLogger::LOG_LEVEL_UNDEFINED;

  vtksys::CommandLineArguments args;
  args.Initialize(argc, argv);

  args.AddArgument("--source-seq-file",vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputImgSeqFileName, "The ultrasound sequence to draw the scanlines on.");
  args.AddArgument("--output-seq-file",vtksys::CommandLineArguments::EQUAL_ARGUMENT, &outputImgSeqFileName, "The output ultrasound sequence with scanlines overlaid on the images.");
  args.AddArgument("--config-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputConfigFileName, "The ultrasound sequence config file.");
  args.AddArgument("--help", vtksys::CommandLineArguments::NO_ARGUMENT, &printHelp, "Print this help.");
  args.AddArgument("--verbose", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &verboseLevel, "Verbose level (1=error only, 2=warning, 3=info, 4=debug, 5=trace)");

  // Fail if arguements can't be parsed
  if (!args.Parse())
  {
    std::cerr << "Error parsing arguments." << std::endl;
    std::cout << "Help: " << args.GetHelp() << std::endl;
    exit(EXIT_FAILURE);
  }
  // Print help if requested
  if (printHelp)
  {
    std::cout << args.GetHelp() << std::endl;
    exit(EXIT_SUCCESS);
  }

  vtkPlusLogger::Instance()->SetLogLevel(verboseLevel);

  // Fail if no ultrasound image file specified
  if (inputImgSeqFileName.empty())
  {
    LOG_ERROR("--seq-file required");
    exit(EXIT_FAILURE);
  }
  // Fail if no ultrasound config file specified
  if (inputConfigFileName.empty())
  {
    LOG_ERROR("--config-file required");
    exit(EXIT_FAILURE);
  }

  // Read the image sequence
  vtkSmartPointer<vtkTrackedFrameList> trackedFrameList = vtkSmartPointer<vtkTrackedFrameList>::New();
  trackedFrameList->ReadFromSequenceMetafile(inputImgSeqFileName.c_str());

  // For reading the configuration file
  vtkSmartPointer<vtkXMLDataElement> configRootElement = vtkSmartPointer<vtkXMLDataElement>::New();
  if (PlusXmlUtils::ReadDeviceSetConfigurationFromFile(configRootElement, inputConfigFileName.c_str())==PLUS_FAIL)
  {  
    LOG_ERROR("Unable to read configuration from file " << inputConfigFileName.c_str()); 
    return EXIT_FAILURE;
  }

  vtkXMLDataElement* scanConversionElement = configRootElement->LookupElementWithName("ScanConversion");
  if (scanConversionElement == NULL)
  {
    LOG_ERROR("ScanConversion element was not found in input configuration file");
    return EXIT_FAILURE;
  }

  // Get number of scanlines from US simulator algo (if present)
  int numOfScanlines = 50;
  vtkXMLDataElement* usSimulatorAlgoElement = configRootElement->LookupElementWithName("vtkUsSimulatorAlgo");
  if (usSimulatorAlgoElement != NULL)
  {
    // Get US simulator attributes
    usSimulatorAlgoElement->GetScalarAttribute("NumberOfScanlines",numOfScanlines);
  }
  else
  {
    LOG_INFO("vtkUsSimulatorAlgo element not found in input configuration file. Using default NumberOfScanlines ("<<numOfScanlines<<")");
  }

  // Call scanline generator with appropriate scanconvert
  const char* transducerGeometry = scanConversionElement->GetAttribute("TransducerGeometry");
  if (transducerGeometry==NULL)
  {
    LOG_ERROR("Scan converter TransducerGeometry is undefined");
    return EXIT_FAILURE;
  }
  vtkSmartPointer<vtkUsScanConvert> scanConverter;
  if (STRCASECMP(transducerGeometry,"CURVILINEAR") == 0)
  {
    scanConverter = vtkSmartPointer<vtkUsScanConvert>::Take(vtkUsScanConvertCurvilinear::New());
  }
  else if (STRCASECMP(transducerGeometry, "LINEAR") == 0)
  {
    scanConverter = vtkSmartPointer<vtkUsScanConvert>::Take(vtkUsScanConvertLinear::New());
  }
  else
  {
    LOG_ERROR("Invalid scan converter TransducerGeometry: "<<transducerGeometry);
    return EXIT_FAILURE;
  }

  scanConverter->ReadConfiguration(scanConversionElement);

  const int numOfSamplesPerScanline = 100; // number of dots drawn per scanline
  int rfImageExtent[6] = {0,numOfSamplesPerScanline-1,0,numOfScanlines-1,0,0};
  scanConverter->SetInputImageExtent(rfImageExtent);
  DrawScanLines(scanConverter, trackedFrameList);

  // Write the new TrackedFrameList to metafile
  LOG_INFO("Writing new sequence to file...");
  if (outputImgSeqFileName.empty())
  {
    int extensionDot = inputImgSeqFileName.find_last_of(".");
    if (extensionDot != std::string::npos)
    {
      inputImgSeqFileName = inputImgSeqFileName.substr(0,extensionDot);
    }
    outputImgSeqFileName = inputImgSeqFileName + "-Scanlines.mha";
  }
  trackedFrameList->SaveToSequenceMetafile(outputImgSeqFileName.c_str());
  LOG_INFO("Writing to "<<outputImgSeqFileName<<" complete.");

  return EXIT_SUCCESS;
}
