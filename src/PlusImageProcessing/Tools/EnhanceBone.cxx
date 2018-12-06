/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

#include "PlusConfigure.h"
#include "igsioVideoFrame.h"
#include "igsioTrackedFrame.h"
#include "vtkPlusForoughiBoneSurfaceProbability.h"
#include "vtkImageCast.h"
#include "vtkImageData.h"
#include "vtkMetaImageReader.h"
#include "vtkMetaImageWriter.h"
#include "vtkIGSIOSequenceIO.h"
#include "vtkSmartPointer.h"
#include "vtkIGSIOTrackedFrameList.h"
#include "vtkXMLUtilities.h"
#include "vtksys/CommandLineArguments.hxx"

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

  vtkPlusLogger::Instance()->SetLogLevel(verboseLevel);

  // Fail if no ultrasound image file specified
  if (inputImgSeqFileName.empty())
  {
    LOG_ERROR("--seq-file required");
    exit(EXIT_FAILURE);
  }

  // Read the image sequence
  vtkSmartPointer<vtkIGSIOTrackedFrameList> trackedFrameList = vtkSmartPointer<vtkIGSIOTrackedFrameList>::New();
  if( vtkIGSIOSequenceIO::Read(inputImgSeqFileName, trackedFrameList) != PLUS_SUCCESS )
  {
    LOG_ERROR("Unable to read sequence file: " << inputImgSeqFileName);
    exit(EXIT_FAILURE);
  }

  vtkSmartPointer<vtkImageCast> castToDouble = vtkSmartPointer<vtkImageCast>::New();
  castToDouble->SetOutputScalarTypeToDouble();

  vtkSmartPointer<vtkPlusForoughiBoneSurfaceProbability> boneSurfaceFilter = vtkSmartPointer<vtkPlusForoughiBoneSurfaceProbability>::New();
  boneSurfaceFilter->SetInputConnection(castToDouble->GetOutputPort());
  
  vtkSmartPointer<vtkImageCast> castToUnsignedChar = vtkSmartPointer<vtkImageCast>::New();
  castToUnsignedChar->SetOutputScalarTypeToUnsignedChar();
  castToUnsignedChar->SetInputConnection(boneSurfaceFilter->GetOutputPort());

  int numberOfFrames = trackedFrameList->GetNumberOfTrackedFrames();
  LOG_INFO("Processing "<<numberOfFrames<<" frames...");
  for (int frameIndex = 0; frameIndex < numberOfFrames; frameIndex++)
  {
    TrackedFrame* frame = trackedFrameList->GetTrackedFrame(frameIndex);
    vtkImageData* imageData = frame->GetImageData()->GetImage();

    castToDouble->SetInputData(imageData);
    castToUnsignedChar->Update();

    // Write back the processed output to the input trackedframelist
    frame->GetImageData()->DeepCopyFrom(castToUnsignedChar->GetOutput());
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
    outputImgSeqFileName = inputImgSeqFileName + "-Bones.nrrd";
  }
  if( vtkIGSIOSequenceIO::Write(outputImgSeqFileName, trackedFrameList) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to save output volume to " << outputImgSeqFileName); 
    return EXIT_FAILURE;
  }
  LOG_INFO("Writing to "<<outputImgSeqFileName<<" complete.");

  return EXIT_SUCCESS;
}
