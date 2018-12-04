/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

// Local includes
#include "PlusConfigure.h"
#include "igsioCommon.h"
#include "PlusMath.h"
#include "igsioTrackedFrame.h"
#include "igsioVideoFrame.h"
#include "vtkPlusSequenceIO.h"
#include "vtkIGSIOTrackedFrameList.h"
#include "vtkPlusUsScanConvertCurvilinear.h"
#include "vtkPlusUsScanConvertLinear.h"

// VTK includes
#include <vtkImageData.h>
#include <vtkLineSource.h>
#include <vtkSmartPointer.h>
#include <vtkXMLUtilities.h>
#include <vtksys/CommandLineArguments.hxx>

namespace
{
  const float DRAWING_COLOR = 255;
}

//----------------------------------------------------------------------------
int main(int argc, char** argv)
{
  // Setup for command line arguments
  bool printHelp(false);
  std::string inputImgSeqFileName;
  std::string outputImgSeqFileName;
  std::string inputConfigFileName;
  int verboseLevel = vtkIGSIOLogger::LOG_LEVEL_UNDEFINED;

  vtksys::CommandLineArguments args;
  args.Initialize(argc, argv);

  args.AddArgument("--source-seq-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputImgSeqFileName, "The ultrasound sequence to draw the scanlines on.");
  args.AddArgument("--output-seq-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &outputImgSeqFileName, "The output ultrasound sequence with scanlines overlaid on the images.");
  args.AddArgument("--config-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputConfigFileName, "The ultrasound sequence config file.");
  args.AddArgument("--help", vtksys::CommandLineArguments::NO_ARGUMENT, &printHelp, "Print this help.");
  args.AddArgument("--verbose", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &verboseLevel, "Verbose level (1=error only, 2=warning, 3=info, 4=debug, 5=trace)");

  // Fail if arguments can't be parsed
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

  vtkIGSIOLogger::Instance()->SetLogLevel(verboseLevel);

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
  vtkSmartPointer<vtkIGSIOTrackedFrameList> trackedFrameList = vtkSmartPointer<vtkIGSIOTrackedFrameList>::New();
  if (vtkPlusSequenceIO::Read(inputImgSeqFileName, trackedFrameList) != PLUS_SUCCESS)
  {
    LOG_ERROR("Unable to load input image sequence.");
    exit(EXIT_FAILURE);
  }

  // For reading the configuration file
  vtkSmartPointer<vtkXMLDataElement> configRootElement = vtkSmartPointer<vtkXMLDataElement>::New();
  if (PlusXmlUtils::ReadDeviceSetConfigurationFromFile(configRootElement, inputConfigFileName.c_str()) == PLUS_FAIL)
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
  vtkXMLDataElement* usSimulatorAlgoElement = configRootElement->LookupElementWithName("vtkPlusUsSimulatorAlgo");
  if (usSimulatorAlgoElement != NULL)
  {
    // Get US simulator attributes
    usSimulatorAlgoElement->GetScalarAttribute("NumberOfScanlines", numOfScanlines);
  }
  else
  {
    LOG_INFO("vtkPlusUsSimulatorAlgo element not found in input configuration file. Using default NumberOfScanlines (" << numOfScanlines << ")");
  }

  // Call scanline generator with appropriate scanconvert
  const char* transducerGeometry = scanConversionElement->GetAttribute("TransducerGeometry");
  if (transducerGeometry == NULL)
  {
    LOG_ERROR("Scan converter TransducerGeometry is undefined");
    return EXIT_FAILURE;
  }
  vtkSmartPointer<vtkPlusUsScanConvert> scanConverter;
  if (STRCASECMP(transducerGeometry, "CURVILINEAR") == 0)
  {
    scanConverter = vtkSmartPointer<vtkPlusUsScanConvert>::Take(vtkPlusUsScanConvertCurvilinear::New());
  }
  else if (STRCASECMP(transducerGeometry, "LINEAR") == 0)
  {
    scanConverter = vtkSmartPointer<vtkPlusUsScanConvert>::Take(vtkPlusUsScanConvertLinear::New());
  }
  else
  {
    LOG_ERROR("Invalid scan converter TransducerGeometry: " << transducerGeometry);
    return EXIT_FAILURE;
  }

  scanConverter->ReadConfiguration(scanConversionElement);

  const int numOfSamplesPerScanline = 100; // number of dots drawn per scanline
  int rfImageExtent[6] = {0, numOfSamplesPerScanline - 1, 0, numOfScanlines - 1, 0, 0};
  scanConverter->SetInputImageExtent(rfImageExtent);

  igsioCommon::PixelLineList lines;
  for (int scanLine = 0; scanLine < rfImageExtent[3] - rfImageExtent[2] + 1; scanLine++)
  {
    double start[4] = { 0 };
    double end[4] = { 0 };
    scanConverter->GetScanLineEndPoints(scanLine, start, end);
    igsioCommon::PixelPoint startPoint = { static_cast<int>(std::round(start[0])), static_cast<int>(std::round(start[1])), static_cast<int>(std::round(start[2])) };
    igsioCommon::PixelPoint endPoint = { static_cast<int>(std::round(end[0])), static_cast<int>(std::round(end[1])), static_cast<int>(std::round(end[2])) };
    lines.push_back(igsioCommon::PixelLine(startPoint, endPoint));
  }

  if (!lines.empty())
  {
    igsioCommon::DrawScanLines(rfImageExtent, 255, lines, trackedFrameList);
  }

  // Write the new TrackedFrameList to metafile
  LOG_INFO("Writing new sequence to file...");
  if (outputImgSeqFileName.empty())
  {
    int extensionDot = inputImgSeqFileName.find_last_of(".");
    if (extensionDot != std::string::npos)
    {
      inputImgSeqFileName = inputImgSeqFileName.substr(0, extensionDot);
    }
    outputImgSeqFileName = inputImgSeqFileName + "-Scanlines.nrrd";
  }
  if (vtkPlusSequenceIO::Write(outputImgSeqFileName, trackedFrameList) != PLUS_SUCCESS)
  {
    return EXIT_FAILURE;
  }
  LOG_INFO("Writing to " << outputImgSeqFileName << " complete.");

  return EXIT_SUCCESS;
}