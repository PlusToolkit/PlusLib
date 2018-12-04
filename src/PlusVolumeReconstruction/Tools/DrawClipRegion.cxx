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
#include "vtkPlusVolumeReconstructor.h"

// VTK includes
#include <vtkImageData.h>
#include <vtkMath.h>
#include <vtkSmartPointer.h>
#include <vtkXMLUtilities.h>
#include <vtksys/CommandLineArguments.hxx>

namespace
{
  static const float DRAWING_COLOR = 255;
}

//----------------------------------------------------------------------------
void DrawSector(vtkImageData* imageData, int* imageExtent, double* origin, double* fanAnglesDeg, double radius, int numberOfPoints)
{
  double startAngleRad = vtkMath::RadiansFromDegrees(fanAnglesDeg[0]);
  double deltaAngleRad = vtkMath::RadiansFromDegrees((fanAnglesDeg[1] - fanAnglesDeg[0]) / (numberOfPoints - 1));
  for (int pointIndex = 0; pointIndex < numberOfPoints; ++pointIndex)
  {
    double angleRad = startAngleRad + pointIndex * deltaAngleRad;
    int pixelCoordX = origin[0] + radius * sin(angleRad);
    int pixelCoordY = origin[1] + radius * cos(angleRad);
    if (pixelCoordX < imageExtent[0] ||  pixelCoordX > imageExtent[1] || pixelCoordY < imageExtent[2] ||  pixelCoordY > imageExtent[3])
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
  double fanAnglesRad[2] = {vtkMath::RadiansFromDegrees(fanAnglesDeg[0]), vtkMath::RadiansFromDegrees(fanAnglesDeg[1])};
  int* extent = imageData->GetExtent();
  // origin to start radius (to highlight the origin in the image)
  if (drawOrigin)
  {
    int numberOfPointsForDrawing = startRadius / 10; // 1/10 point/pixel: dotted line
    double rounded = std::round(fanOrigin[1]);
    unsigned int val = static_cast<unsigned int>(rounded);
    std::array<int, 3> startPoint = { static_cast<int>(std::round(fanOrigin[0])),
                                      static_cast<int>(std::round(fanOrigin[1])),
                                      0
                                    };
    std::array<int, 3> endPointLeft = { static_cast<int>(std::round(fanOrigin[0] + startRadius * sin(fanAnglesRad[0]))),
                                        static_cast<int>(std::round(fanOrigin[1] + startRadius * cos(fanAnglesRad[0]))),
                                        0
                                      };
    std::array<int, 3> endPointRight = { static_cast<int>(std::round(fanOrigin[0] + startRadius * sin(fanAnglesRad[1]))),
                                         static_cast<int>(std::round(fanOrigin[1] + startRadius * cos(fanAnglesRad[1]))),
                                         0
                                       };
    igsioCommon::DrawLine(*imageData, DRAWING_COLOR, igsioCommon::LINE_STYLE_DOTS, startPoint, endPointLeft, numberOfPointsForDrawing);
    igsioCommon::DrawLine(*imageData, DRAWING_COLOR, igsioCommon::LINE_STYLE_DOTS, startPoint, endPointRight, numberOfPointsForDrawing);
  }
  // side lines from start to stop radius
  {
    int numberOfPointsForDrawing = (stopRadius - startRadius) / pointSpacing;
    {
      std::array<int, 3> startPoint = { static_cast<int>(std::round(fanOrigin[0] + startRadius * sin(fanAnglesRad[0]))),
                                        static_cast<int>(std::round(fanOrigin[1] + startRadius * cos(fanAnglesRad[0]))),
                                        0
                                      };
      std::array<int, 3> endPoint = { static_cast<int>(std::round(fanOrigin[0] + stopRadius * sin(fanAnglesRad[0]))),
                                      static_cast<int>(std::round(fanOrigin[1] + stopRadius * cos(fanAnglesRad[0]))),
                                      0
                                    };
      igsioCommon::DrawLine(*imageData, DRAWING_COLOR, igsioCommon::LINE_STYLE_DOTS, startPoint, endPoint, numberOfPointsForDrawing);
    }

    {
      std::array<int, 3> startPoint = { static_cast<int>(std::round(fanOrigin[0] + startRadius * sin(fanAnglesRad[1]))),
                                        static_cast<int>(std::round(fanOrigin[1] + startRadius * cos(fanAnglesRad[1]))),
                                        0
                                      };
      std::array<int, 3> endPoint = { static_cast<int>(std::round(fanOrigin[0] + stopRadius * sin(fanAnglesRad[1]))),
                                      static_cast<int>(std::round(fanOrigin[1] + stopRadius * cos(fanAnglesRad[1]))),
                                      0
                                    };
      igsioCommon::DrawLine(*imageData, DRAWING_COLOR, igsioCommon::LINE_STYLE_DOTS, startPoint, endPoint, numberOfPointsForDrawing);
    }
  }
  // circle sectors at start and stop radius
  {
    int numberOfPointsForDrawing = startRadius * (fanAnglesRad[1] - fanAnglesRad[0]) / pointSpacing; // sector length
    DrawSector(imageData, extent, fanOrigin, fanAnglesDeg, startRadius, numberOfPointsForDrawing);
    numberOfPointsForDrawing = stopRadius * (fanAnglesRad[1] - fanAnglesRad[0]) / pointSpacing; // sector length
    DrawSector(imageData, extent, fanOrigin, fanAnglesDeg, stopRadius, numberOfPointsForDrawing);
  }
}

//----------------------------------------------------------------------------
void DrawClipRectangle(vtkImageData* imageData, vtkPlusVolumeReconstructor* reconstructor)
{
  int* clipRectangleOrigin = reconstructor->GetClipRectangleOrigin();
  int* clipRectangleSize = reconstructor->GetClipRectangleSize();
  int* extent = imageData->GetExtent();
  int numberOfPoints = 200; // number of drawn points per line

  // Horizontal lines
  {
    std::array<int, 3> startPoint = { static_cast<int>(std::round(clipRectangleOrigin[0])),
                                      static_cast<int>(std::round(clipRectangleOrigin[1])),
                                      0
                                    };
    std::array<int, 3> endPoint = { static_cast<int>(std::round(clipRectangleOrigin[0] + clipRectangleSize[0] - 1)),
                                    static_cast<int>(std::round(clipRectangleOrigin[1])),
                                    0
                                  };
    igsioCommon::DrawLine(*imageData, DRAWING_COLOR, igsioCommon::LINE_STYLE_DOTS, startPoint, endPoint, numberOfPoints);
  }
  {
    std::array<int, 3> startPoint = { static_cast<int>(std::round(clipRectangleOrigin[0])),
                                      static_cast<int>(std::round(clipRectangleOrigin[1] + clipRectangleSize[1] - 1)),
                                      0
                                    };
    std::array<int, 3> endPoint = { static_cast<int>(std::round(clipRectangleOrigin[0] + clipRectangleSize[0] - 1)),
                                    static_cast<int>(std::round(clipRectangleOrigin[1] + clipRectangleSize[1] - 1)),
                                    0
                                  };
    igsioCommon::DrawLine(*imageData, DRAWING_COLOR, igsioCommon::LINE_STYLE_DOTS, startPoint, endPoint, numberOfPoints);
  }

  // Vertical lines
  {
    std::array<int, 3> startPoint = { static_cast<int>(std::round(clipRectangleOrigin[0])),
                                      static_cast<int>(std::round(clipRectangleOrigin[1])),
                                      0
                                    };
    std::array<int, 3> endPoint = { static_cast<int>(std::round(clipRectangleOrigin[0])),
                                    static_cast<int>(std::round(clipRectangleOrigin[1] + clipRectangleSize[1] - 1)),
                                    0
                                  };
    igsioCommon::DrawLine(*imageData, DRAWING_COLOR, igsioCommon::LINE_STYLE_DOTS, startPoint, endPoint, numberOfPoints);
  }
  {
    std::array<int, 3> startPoint = { static_cast<int>(std::round(clipRectangleOrigin[0] + clipRectangleSize[0] - 1)),
                                      static_cast<int>(std::round(clipRectangleOrigin[1])),
                                      0
                                    };
    std::array<int, 3> endPoint = { static_cast<int>(std::round(clipRectangleOrigin[0] + clipRectangleSize[0] - 1)),
                                    static_cast<int>(std::round(clipRectangleOrigin[1] + clipRectangleSize[1] - 1)),
                                    0
                                  };
    igsioCommon::DrawLine(*imageData, DRAWING_COLOR, igsioCommon::LINE_STYLE_DOTS, startPoint, endPoint, numberOfPoints);
  }
}

//----------------------------------------------------------------------------
void DrawClipFan(vtkImageData* imageData, vtkPlusVolumeReconstructor* reconstructor)
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
  int verboseLevel = vtkIGSIOLogger::LOG_LEVEL_UNDEFINED;

  vtksys::CommandLineArguments args;
  args.Initialize(argc, argv);

  args.AddArgument("--source-seq-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputImgSeqFileName, "Input ultrasound image sequence.");
  args.AddArgument("--output-seq-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &outputImgSeqFileName, "Output ultrasound sequence, with clipping rectangle and fan overlaid.");
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
    LOG_ERROR("Unable to load input sequences file.");
    exit(EXIT_FAILURE);
  }

  // For reading the configuration file
  vtkSmartPointer<vtkXMLDataElement> configRootElement = vtkSmartPointer<vtkXMLDataElement>::New();
  if (PlusXmlUtils::ReadDeviceSetConfigurationFromFile(configRootElement, inputConfigFileName.c_str()) == PLUS_FAIL)
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
  vtkSmartPointer<vtkPlusVolumeReconstructor> reconstructor = vtkSmartPointer<vtkPlusVolumeReconstructor>::New();
  if (reconstructor->ReadConfiguration(volumeReconstructionElement->GetParent()) == PLUS_FAIL)
  {
    LOG_ERROR("Failed to parse VolumeReconstruction element in input configuration file");
    return EXIT_FAILURE;
  }

  // Draw
  int numberOfFrames = trackedFrameList->GetNumberOfTrackedFrames();
  LOG_INFO("Processing " << numberOfFrames << " frames...");
  for (int frameIndex = 0; frameIndex < numberOfFrames; frameIndex++)
  {
    igsioTrackedFrame* frame = trackedFrameList->GetTrackedFrame(frameIndex);
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
      inputImgSeqFileName = inputImgSeqFileName.substr(0, extensionDot);
    }
    outputImgSeqFileName = inputImgSeqFileName + "-Scanlines.nrrd";
  }
  if (vtkPlusSequenceIO::Write(outputImgSeqFileName, trackedFrameList) != PLUS_SUCCESS)
  {
    //Error has already been logged
    return EXIT_FAILURE;
  }
  LOG_INFO("Writing to " << outputImgSeqFileName << " complete.");

  return EXIT_SUCCESS;
}
