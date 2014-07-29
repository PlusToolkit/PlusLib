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

#include "itkLineIterator.h"
#include "itkVTKImageExport.h"
#include "itkImageToVTKImageFilter.h"


typedef itk::Image<unsigned char, 2> ImageType;
typedef unsigned char PixelType;
typedef itk::ImageToVTKImageFilter<ImageType> ITKtoVTKFilter;

template <typename T>
int GenerateScanLines(vtkSmartPointer<T> scanConverter,
                      vtkXMLDataElement* scanConversionElement,
                      vtkSmartPointer<vtkTrackedFrameList> adjustedFrameList,
                      int numOfSamplesPerScanline,int numOfScanlines,
                      std::string ultrasoundImageFile)
{
  vtkSmartPointer<vtkImageData> scanLines =
    vtkSmartPointer<vtkImageData>::New();
  // Set the transducer width/depth
  scanConverter->ReadConfiguration(scanConversionElement);

  scanLines->SetExtent(0,numOfSamplesPerScanline-1,0,numOfScanlines-1,0,0);
#if (VTK_MAJOR_VERSION < 6)
  scanLines->SetScalarTypeToUnsignedChar();
  scanLines->SetNumberOfScalarComponents(1); 
  scanLines->AllocateScalars();
#else
  scanLines->AllocateScalars(VTK_UNSIGNED_CHAR, 1);
#endif
  
  scanConverter->SetInputImageExtent(scanLines->GetExtent());

  // Start/end points of scanline
  double start[4] = {-1,-1,-1,-1};
  double end[4] = {-1,-1,-1,-1};

  // Probe line
  vtkSmartPointer<vtkLineSource> probeLine =
    vtkSmartPointer<vtkLineSource>::New();

  // Read the image sequence
  vtkSmartPointer<vtkTrackedFrameList> trackedFrameList =
    vtkSmartPointer<vtkTrackedFrameList>::New();
  trackedFrameList->ReadFromSequenceMetafile(ultrasoundImageFile.c_str());

  // Get number of frames in sequence
  int numberOfFrames = trackedFrameList->GetNumberOfTrackedFrames();
  std::cout << "Number of tracked frames: " << numberOfFrames << std::endl;

  // To hold each individual frames vtkImageData
  vtkSmartPointer<vtkImageData> imageData =
    vtkSmartPointer<vtkImageData>::New();

  // For converting itkImage to vtkImageData
  ITKtoVTKFilter::Pointer ITKtoVTK = ITKtoVTKFilter::New();

  PixelType pixel = 255; // Value to set scanlines to

  // Iterate over each frame
  std::cout << "Processing frames..." << std::endl;
  for (int index = 0; index < numberOfFrames; index++)
  {
    // Convert the current tracked frame into an itkImage
    TrackedFrame* frame = trackedFrameList->GetTrackedFrame(index);
    imageData = frame->GetImageData()->GetImage();
    ImageType::Pointer itkImageData = ImageType::New();
    PlusVideoFrame::DeepCopyVtkImageToItkImage<unsigned char>(imageData, itkImageData);

    // Iterate over each scanline, getting the start/end points of
    // the line and using lineiterator to set each pixel in the line
    for (int scanLine = 0; scanLine < numOfScanlines; scanLine++)
    {
      //std::cout << "Processing scanline #" << scanLine << "..." << std::endl;
      scanConverter->GetScanLineEndPoints(scanLine,start,end);
      ImageType::IndexType startIndex = {start[0], start[1]};
      ImageType::IndexType endIndex = {end[0], end[1]};
      //std::cout << "Start index: [" << start[0] << " " << start[1] << "]" << std::endl;
      //std::cout << "End index: [" << end[0] << " " << end[1] << "]" << std::endl;
      itk::LineIterator<ImageType> iterator(itkImageData,startIndex,endIndex);
      //ImageType::IndexType currentIndex = iterator.GetIndex();
      // Set the pixel values in the line
      while (!iterator.IsAtEnd())
      {
        iterator.Set(pixel);
        ++iterator;
      }
    }

    // Convert from itkImage to vtkImageData
    ITKtoVTK->SetInput(itkImageData);
    ITKtoVTK->Update();
    vtkSmartPointer<vtkImageData> convertedImage = ITKtoVTK->GetOutput();

    // Create new PlusVideoFrame
    PlusVideoFrame currentVideoFrame = PlusVideoFrame();
    PlusStatus copyResult = currentVideoFrame.DeepCopyFrom(convertedImage);
    if (copyResult == 0)
    {
      std::cerr << "Error deep copying itkImage to PlusVideoFrame" << std::endl;
      exit(EXIT_FAILURE);
    }

    // Create new TrackedFrame
    TrackedFrame currentTrackedFrame = TrackedFrame();
    currentTrackedFrame.SetImageData(currentVideoFrame);

    // Add TrackedFrame to new TrackedFrameList
    PlusStatus addFrameResult =
      adjustedFrameList->AddTrackedFrame(&currentTrackedFrame);
    if (addFrameResult == 0)
    {
      std::cerr << "Error adding TrackedFrame to TrackedFrameList" << std::endl;
      exit(EXIT_FAILURE);
    }
  }
  std::cout << "Done processing frames" << std::endl;
  return 1;
}

int main(int argc, char** argv)
{
  // Setup for command line arguments
  bool printHelp(false);
  std::string ultrasoundImageFile;
  std::string ultrasoundConfigFile;
  int verboseLevel = vtkPlusLogger::LOG_LEVEL_UNDEFINED;

  vtksys::CommandLineArguments args;
  args.Initialize(argc, argv);

  args.AddArgument("--ultrasound-image-file",vtksys::CommandLineArguments::EQUAL_ARGUMENT,
    &ultrasoundImageFile, "The ultrasound sequence to draw the scanlines on.");
  args.AddArgument("--ultrasound-config-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT,
    &ultrasoundConfigFile, "The ultrasound sequence config file.");
  args.AddArgument("--help", vtksys::CommandLineArguments::NO_ARGUMENT, &printHelp,
    "Print this help.");
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
  if (ultrasoundImageFile.empty())
  {
    std::cerr << "--ultrasound-image-file required" << std::endl;
    exit(EXIT_FAILURE);
  }
  // Fail if no ultrasound config file specified
  if (ultrasoundConfigFile.empty())
  {
    std::cerr << "--ultrasound-config-file required" << std::endl;
    exit(EXIT_FAILURE);
  }

  // For reading the configuration file
  vtkSmartPointer<vtkXMLDataElement> configRootElement = vtkSmartPointer<vtkXMLDataElement>::New();
  if (PlusXmlUtils::ReadDeviceSetConfigurationFromFile(configRootElement, ultrasoundConfigFile.c_str())==PLUS_FAIL)
  {  
    LOG_ERROR("Unable to read configuration from file " << ultrasoundConfigFile.c_str()); 
    return EXIT_FAILURE;
  }

  // Get the scan conversion XML Element
  vtkXMLDataElement* scanConversionElement =
    configRootElement->LookupElementWithName("ScanConversion");
  if (scanConversionElement == NULL)
  {
    std::cerr << "Unable to find ScanConversion element in XML tree." << std::endl;
    exit(EXIT_FAILURE);
  }

  // Get scan geometry
  std::string geometry = scanConversionElement->GetAttribute("TransducerGeometry");

  // Get the vtkUsSimulatorAlgo XML Element
  vtkXMLDataElement* usSimulatorAlgoElement =
    configRootElement->LookupElementWithName("vtkUsSimulatorAlgo");
  if (scanConversionElement == NULL)
  {
    std::cerr << "Unable to find vtkUsSimulatorAlgo element in the XML tree."
              << std::endl;
    exit(EXIT_FAILURE);
  }

  // Get US simulator attributes
  int numOfScanlines = -1;
  int numOfSamplesPerScanline = -1;
  usSimulatorAlgoElement->GetScalarAttribute("NumberOfScanlines",numOfScanlines);
  usSimulatorAlgoElement->GetScalarAttribute("NumberOfSamplesPerScanline",
                                             numOfSamplesPerScanline);

  // The framelist to hold the ultrasound sequence with
  // scanlines added
  vtkSmartPointer<vtkTrackedFrameList> adjustedFrameList =
    vtkSmartPointer<vtkTrackedFrameList>::New();

  // Call scanline generator with appropriate scanconvert
  if (geometry.compare("CURVILINEAR") == 0)
  {
    vtkSmartPointer<vtkUsScanConvertCurvilinear> scanConverter =
      vtkSmartPointer<vtkUsScanConvertCurvilinear>::New();
    GenerateScanLines(scanConverter, scanConversionElement, adjustedFrameList,
                      numOfSamplesPerScanline, numOfScanlines, ultrasoundImageFile);
  } else if (geometry.compare("LINEAR") == 0)
  {
    vtkSmartPointer<vtkUsScanConvertLinear> scanConverter =
      vtkSmartPointer<vtkUsScanConvertLinear>::New();
    GenerateScanLines(scanConverter, scanConversionElement, adjustedFrameList,
                      numOfSamplesPerScanline, numOfScanlines, ultrasoundImageFile);
  }

  // Write the new TrackedFrameList to metafile
  std::cout << "Writing new sequence to file..." << std::endl;
  int extensionDot = ultrasoundImageFile.find_last_of(".");
  if (extensionDot != std::string::npos)
  {
    ultrasoundImageFile = ultrasoundImageFile.substr(0,extensionDot);
  }
  adjustedFrameList->SaveToSequenceMetafile((ultrasoundImageFile + "-Scanlines.mha").c_str());
  std::cout << "Writing complete." << std::endl;

  return EXIT_SUCCESS;
}
