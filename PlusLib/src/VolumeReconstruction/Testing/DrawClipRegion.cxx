#include "PlusConfigure.h"
#include "vtksys/CommandLineArguments.hxx"

#include "vtkSmartPointer.h"
#include "vtkMetaImageReader.h"
#include "vtkXMLUtilities.h"
#include "vtkTrackedFrameList.h"
#include "vtkImageData.h"
#include "vtkMetaImageWriter.h"
#include "vtkVolumeReconstructor.h"
#include "PlusVideoFrame.h"
#include "TrackedFrame.h"
#include "vtkMath.h"

float DRAWING_COLOR = 255;

//----------------------------------------------------------------------------
void DrawLine(vtkImageData* imageData, int* imageExtent, double startX, double startY, double endX, double endY, int numberOfPoints)
{
  double directionVectorX = static_cast<double>(endX-startX)/(numberOfPoints-1);
  double directionVectorY = static_cast<double>(endY-startY)/(numberOfPoints-1);
  for (int pointIndex=0; pointIndex<numberOfPoints; ++pointIndex)
  {
    int pixelCoordX = startX + directionVectorX * pointIndex;
    int pixelCoordY = startY + directionVectorY * pointIndex;
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
void DrawSector(vtkImageData* imageData, int* imageExtent, double* origin, double* fanAnglesDeg, double radius, int numberOfPoints)
{
  double startAngleRad = vtkMath::RadiansFromDegrees(fanAnglesDeg[0]);
  double deltaAngleRad = vtkMath::RadiansFromDegrees((fanAnglesDeg[1]-fanAnglesDeg[0])/(numberOfPoints-1));
  for (int pointIndex=0; pointIndex<numberOfPoints; ++pointIndex)
  {
    double angleRad = startAngleRad + pointIndex*deltaAngleRad;
    int pixelCoordX = origin[0] + radius*sin(angleRad);
    int pixelCoordY = origin[1] + radius*cos(angleRad);
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
void DrawFan(vtkImageData* imageData, double* fanOrigin, double startRadius, double stopRadius, double* fanAnglesDeg, int pointSpacing, bool drawOrigin)
{
  double fanAnglesRad[2]={vtkMath::RadiansFromDegrees(fanAnglesDeg[0]), vtkMath::RadiansFromDegrees(fanAnglesDeg[1])};
  int* extent = imageData->GetExtent();
  // origin to start radius (to highlight the origin in the image)
  if (drawOrigin)
  {
    int numberOfPointsForDrawing = startRadius/10; // 1/10 point/pixel: dotted line
    DrawLine(imageData, extent, fanOrigin[0], fanOrigin[1], fanOrigin[0]+startRadius*sin(fanAnglesRad[0]), fanOrigin[1]+startRadius*cos(fanAnglesRad[0]), numberOfPointsForDrawing);
    DrawLine(imageData, extent, fanOrigin[0], fanOrigin[1], fanOrigin[0]+startRadius*sin(fanAnglesRad[1]), fanOrigin[1]+startRadius*cos(fanAnglesRad[1]), numberOfPointsForDrawing);
  }
  // side lines from start to stop radius
  {
    int numberOfPointsForDrawing = (stopRadius-startRadius)/pointSpacing;
    DrawLine(imageData, extent, fanOrigin[0]+startRadius*sin(fanAnglesRad[0]), fanOrigin[1]+startRadius*cos(fanAnglesRad[0]), fanOrigin[0]+stopRadius*sin(fanAnglesRad[0]), fanOrigin[1]+stopRadius*cos(fanAnglesRad[0]), numberOfPointsForDrawing);
    DrawLine(imageData, extent, fanOrigin[0]+startRadius*sin(fanAnglesRad[1]), fanOrigin[1]+startRadius*cos(fanAnglesRad[1]), fanOrigin[0]+stopRadius*sin(fanAnglesRad[1]), fanOrigin[1]+stopRadius*cos(fanAnglesRad[1]), numberOfPointsForDrawing);
  }
  // circle sectors at start and stop radius
  {
    int numberOfPointsForDrawing = startRadius*(fanAnglesRad[1]-fanAnglesRad[0])/pointSpacing; // sector length
    DrawSector(imageData, extent, fanOrigin, fanAnglesDeg, startRadius, numberOfPointsForDrawing);
    numberOfPointsForDrawing = stopRadius*(fanAnglesRad[1]-fanAnglesRad[0])/pointSpacing; // sector length
    DrawSector(imageData, extent, fanOrigin, fanAnglesDeg, stopRadius, numberOfPointsForDrawing);
  }
}

//----------------------------------------------------------------------------
void DrawClipRectangle(vtkImageData* imageData, vtkVolumeReconstructor* reconstructor)
{
  int* clipRectangleOrigin = reconstructor->GetClipRectangleOrigin();
  int* clipRectangleSize = reconstructor->GetClipRectangleSize();
  int* extent = imageData->GetExtent();
  int numberOfPoints=200; // number of drawn points per line
  // Horizontal lines
  DrawLine(imageData, extent, clipRectangleOrigin[0],clipRectangleOrigin[1], clipRectangleOrigin[0]+clipRectangleSize[0]-1, clipRectangleOrigin[1], numberOfPoints);
  DrawLine(imageData, extent, clipRectangleOrigin[0],clipRectangleOrigin[1]+clipRectangleSize[1]-1, clipRectangleOrigin[0]+clipRectangleSize[0]-1, clipRectangleOrigin[1]+clipRectangleSize[1]-1, numberOfPoints);
  // Vertical lines
  DrawLine(imageData, extent, clipRectangleOrigin[0],clipRectangleOrigin[1], clipRectangleOrigin[0], clipRectangleOrigin[1]+clipRectangleSize[1]-1, numberOfPoints);
  DrawLine(imageData, extent, clipRectangleOrigin[0]+clipRectangleSize[0]-1,clipRectangleOrigin[1], clipRectangleOrigin[0]+clipRectangleSize[0]-1, clipRectangleOrigin[1]+clipRectangleSize[1]-1, numberOfPoints);
}

//----------------------------------------------------------------------------
void DrawClipFan(vtkImageData* imageData, vtkVolumeReconstructor* reconstructor)
{
  bool isImageEmpty = false;
  reconstructor->UpdateFanAnglesFromImage(imageData, isImageEmpty);
  if (!reconstructor->FanClippingApplied())
  {
    return;
  }
  
  double* fanOrigin = reconstructor->GetFanOrigin();
  double* maxFanAnglesDeg = reconstructor->GetFanAnglesDeg();
  double* detectedFanAnglesDeg = reconstructor->GetDetectedFanAnglesDeg();
  double fanRadiusStartPixel = reconstructor->GetFanRadiusStartPixel();
  double fanRadiusStopPixel = reconstructor->GetFanRadiusStopPixel();

  if (isImageEmpty)
  {
    // draw maximum fan angles with dotted line
    DrawFan(imageData, fanOrigin, fanRadiusStartPixel, fanRadiusStopPixel, maxFanAnglesDeg, 10, true /* draw origin*/);
  }
  else
  {
    if (reconstructor->GetEnableFanAnglesAutoDetect())
    {
      // draw maximum fan angles with dotted line
      DrawFan(imageData, fanOrigin, fanRadiusStartPixel, fanRadiusStopPixel, maxFanAnglesDeg, 10, true /* draw origin*/);
      DrawFan(imageData, fanOrigin, fanRadiusStartPixel, fanRadiusStopPixel, detectedFanAnglesDeg, 1, false /* do not draw origin*/);      
    }
    else
    {
      // no fan angle auto-detect => max angle range is used => draw maximum fan angles with solid line
      DrawFan(imageData, fanOrigin, fanRadiusStartPixel, fanRadiusStopPixel, maxFanAnglesDeg, 1, true /* draw origin*/);
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

  args.AddArgument("--source-seq-file",vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputImgSeqFileName, "Input ultrasound image sequence.");
  args.AddArgument("--output-seq-file",vtksys::CommandLineArguments::EQUAL_ARGUMENT, &outputImgSeqFileName, "Output ultrasound sequence, with clipping rectangle and fan overlaid.");
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
  vtkXMLDataElement* volumeReconstructionElement = configRootElement->LookupElementWithName("VolumeReconstruction");
  if (volumeReconstructionElement == NULL)
  {
    LOG_ERROR("VolumeReconstruction element was not found in input configuration file");
    return EXIT_FAILURE;
  }
  vtkSmartPointer<vtkVolumeReconstructor> reconstructor = vtkSmartPointer<vtkVolumeReconstructor>::New();
  if (reconstructor->ReadConfiguration(volumeReconstructionElement->GetParent())==PLUS_FAIL)
  {
    LOG_ERROR("Failed to parse VolumeReconstruction element in input configuration file");
    return EXIT_FAILURE;
  }

  // Draw
  int numberOfFrames = trackedFrameList->GetNumberOfTrackedFrames();
  LOG_INFO("Processing "<<numberOfFrames<<" frames...");
  for (int frameIndex = 0; frameIndex < numberOfFrames; frameIndex++)
  {
    TrackedFrame* frame = trackedFrameList->GetTrackedFrame(frameIndex);
    vtkImageData* imageData = frame->GetImageData()->GetImage();
    DrawClipRectangle(imageData, reconstructor);
    DrawClipFan(imageData, reconstructor);
  }

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
