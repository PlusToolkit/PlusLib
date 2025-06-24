/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"
#include "igsioTrackedFrame.h"
#include "vtkImageData.h"
#include "vtkMatrix4x4.h"
#include "vtkIGSIOSequenceIO.h"
#include "vtkIGSIOTrackedFrameList.h"
#include "vtkIGSIOTransformRepository.h"
#include "vtkPlusVolumeReconstructor.h"
#include "vtkXMLUtilities.h"
#include "vtksys/CommandLineArguments.hxx"

int main(int argc, char* argv[])
{
  bool printHelp(false);
  // Parse command line arguments.

  std::string inputImgSeqFileName;
  std::string inputConfigFileName;
  std::string outputVolumeFileName;
  std::string outputVolumeAlphaFileNameDeprecated;
  std::string outputVolumeAccumulationFileName;
  std::string outputFrameFileName;
  std::string importanceMaskFileName;
  std::string inputImageToReferenceTransformName;

  // Deprecated arguments (2013-07-29, #800)
  std::string inputImageToReferenceTransformNameDeprecated;
  std::string inputImgSeqFileNameDeprecated;

  int verboseLevel = vtkPlusLogger::LOG_LEVEL_UNDEFINED;

  bool disableCompression = false;
  bool disableAutoExtent = false;

  std::vector<std::string> customHeaderFieldsToSave;
  std::vector<std::string> customHeaderValuesToSave;

  vtksys::CommandLineArguments cmdargs;
  cmdargs.Initialize(argc, argv);

  cmdargs.AddArgument("--image-to-reference-transform", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputImageToReferenceTransformName, "Name of the transform to define the image slice pose relative to the reference coordinate system (e.g., ImageToReference). Note that this parameter is optional, if it is defined then it overrides the ImageCoordinateFrame and ReferenceCoordinateFrame attribute values in the configuration file.");
  cmdargs.AddArgument("--source-seq-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputImgSeqFileName, "Input sequence file filename (.mha/.nrrd)");
  cmdargs.AddArgument("--config-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputConfigFileName, "Input configuration file name (.xml)");
  cmdargs.AddArgument("--output-volume-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &outputVolumeFileName, "Output file name of the reconstructed volume (must have .mha, .mhd, .nrrd or .nhdr extension)");
  cmdargs.AddArgument("--output-volume-accumulation-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &outputVolumeAccumulationFileName, "Output file name of the accumulation of the reconstructed volume (.mha/.nrrd)");
  cmdargs.AddArgument("--output-frame-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &outputFrameFileName, "A filename that will be used for storing the tracked image frames. Each frame will be exported individually, with the proper position and orientation in the reference coordinate system");
  cmdargs.AddArgument("--help", vtksys::CommandLineArguments::NO_ARGUMENT, &printHelp, "Print this help.");
  cmdargs.AddArgument("--disable-compression", vtksys::CommandLineArguments::NO_ARGUMENT, &disableCompression, "Do not compress output image files.");
  cmdargs.AddArgument("--disable-auto-extent", vtksys::CommandLineArguments::NO_ARGUMENT, &disableAutoExtent, "Do not compute extent from the input sequence file but use OutputOrigin and OutputExtent specified in the configuration file.");
  cmdargs.AddArgument("--save-custom-headers", vtksys::CommandLineArguments::MULTI_ARGUMENT, &customHeaderFieldsToSave, "List of custom header fields to pass into the output file.");
  cmdargs.AddArgument("--verbose", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &verboseLevel, "Verbose level (1=error only, 2=warning, 3=info, 4=debug, 5=trace)");
  cmdargs.AddArgument("--importance-mask-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &importanceMaskFileName, "The file to use as the importance mask.");

  // Deprecated arguments (2013-07-29, #800)
  cmdargs.AddArgument("--transform", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputImageToReferenceTransformNameDeprecated, "Image to reference transform name used for the reconstruction. DEPRECATED, use --image-to-reference-transform argument instead");
  cmdargs.AddArgument("--img-seq-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputImgSeqFileNameDeprecated, "Input sequence file filename (.mha/.nrrd). DEPRECATED: use --source-seq-file argument instead");
  // Deprecated argument (2014-08-15, #923)
  cmdargs.AddArgument("--output-volume-alpha-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &outputVolumeAlphaFileNameDeprecated, "Output file name of the alpha channel of the reconstructed volume (.mha/.nrrd). DEPRECATED: use --output-volume-accumulation-file argument instead");

  if (!cmdargs.Parse())
  {
    std::cerr << "Problem parsing arguments" << std::endl;
    std::cout << "Help: " << cmdargs.GetHelp() << std::endl;
    exit(EXIT_FAILURE);
  }

  if (printHelp)
  {
    std::cout << cmdargs.GetHelp() << std::endl;
    exit(EXIT_SUCCESS);
  }

  // Set the log level
  vtkPlusLogger::Instance()->SetLogLevel(verboseLevel);

  // Deprecated arguments (2013-07-29, #800)
  if (!inputImageToReferenceTransformNameDeprecated.empty())
  {
    LOG_WARNING("The --transform argument is deprecated. Use --image-to-reference-transform instead.");
    if (inputImageToReferenceTransformName.empty())
    {
      inputImageToReferenceTransformName = inputImageToReferenceTransformNameDeprecated;
    }
  }
  if (!inputImgSeqFileNameDeprecated.empty())
  {
    LOG_WARNING("The --img-seq-file argument is deprecated. Use --source-seq-file instead.");
    if (inputImgSeqFileName.empty())
    {
      inputImgSeqFileName = inputImgSeqFileNameDeprecated;
    }
  }
  // Deprecated argument (2014-08-15, #923)
  if (!outputVolumeAlphaFileNameDeprecated.empty())
  {
    LOG_WARNING("The --output-volume-alpha-file argument is deprecated. Use --output-volume-accumulation-file instead.");
    if (outputVolumeAccumulationFileName.empty())
    {
      outputVolumeAccumulationFileName = outputVolumeAlphaFileNameDeprecated;
    }
  }

  if (inputConfigFileName.empty())
  {
    std::cout << "ERROR: Input config file name is missing!" << std::endl;
    std::cout << "Help: " << cmdargs.GetHelp() << std::endl;
    exit(EXIT_FAILURE);
  }

  vtkSmartPointer<vtkPlusVolumeReconstructor> reconstructor = vtkSmartPointer<vtkPlusVolumeReconstructor>::New();

  LOG_INFO("Reading configuration file:" << inputConfigFileName);
  vtkSmartPointer<vtkXMLDataElement> configRootElement = vtkSmartPointer<vtkXMLDataElement>::New();
  if (PlusXmlUtils::ReadDeviceSetConfigurationFromFile(configRootElement, inputConfigFileName.c_str()) == PLUS_FAIL)
  {
    LOG_ERROR("Unable to read configuration from file " << inputConfigFileName.c_str());
    return EXIT_FAILURE;
  }

  if (reconstructor->ReadConfiguration(configRootElement) != PLUS_SUCCESS)
  {
    LOG_ERROR("Failed to read configuration from " << inputConfigFileName.c_str());
    return EXIT_FAILURE;
  }

  if (!importanceMaskFileName.empty())
  {
    reconstructor->SetImportanceMaskFilename(importanceMaskFileName);
  }

  vtkSmartPointer<vtkIGSIOTransformRepository> transformRepository = vtkSmartPointer<vtkIGSIOTransformRepository>::New();
  if (configRootElement->FindNestedElementWithName("CoordinateDefinitions") != NULL)
  {
    if (transformRepository->ReadConfiguration(configRootElement) != PLUS_SUCCESS)
    {
      LOG_ERROR("Failed to read transforms from CoordinateDefinitions");
      return EXIT_FAILURE;
    }
  }
  else
  {
    LOG_DEBUG("No transforms were found in CoordinateDefinitions. Only the transforms defined in the input image will be available.");
  }

  // Print calibration transform
  std::ostringstream osTransformRepo;
  transformRepository->Print(osTransformRepo);
  LOG_DEBUG("Transform repository: \n" << osTransformRepo.str());

  // Read image sequence
  LOG_INFO("Reading image sequence " << inputImgSeqFileName);
  vtkSmartPointer<vtkIGSIOTrackedFrameList> trackedFrameList = vtkSmartPointer<vtkIGSIOTrackedFrameList>::New();
  if (vtkIGSIOSequenceIO::Read(inputImgSeqFileName, trackedFrameList) != PLUS_SUCCESS)
  {
    LOG_ERROR("Unable to load input sequences file.");
    exit(EXIT_FAILURE);
  }

  // Reconstruct volume
  igsioTransformName imageToReferenceTransformName;
  if (!inputImageToReferenceTransformName.empty())
  {
    // image to reference transform is specified at the command-line
    if (imageToReferenceTransformName.SetTransformName(inputImageToReferenceTransformName.c_str()) != PLUS_SUCCESS)
    {
      LOG_ERROR("Invalid image to reference transform name: " << inputImageToReferenceTransformName);
      return EXIT_FAILURE;
    }
    reconstructor->SetImageCoordinateFrame(imageToReferenceTransformName.From());
    reconstructor->SetReferenceCoordinateFrame(imageToReferenceTransformName.To());
  }

  LOG_INFO("Set volume output extent...");
  std::string errorDetail;
  if (!disableAutoExtent)
  {
    if (reconstructor->SetOutputExtentFromFrameList(trackedFrameList, transformRepository, errorDetail) != PLUS_SUCCESS)
    {
      LOG_ERROR("Failed to set output extent of volume!");
      return EXIT_FAILURE;
    }
  }
  else
  {
    LOG_INFO("Automatic extent computation disabled. Using OutputOrigin and OutputExtent specified in the configuration file.");

    // When automatic extent computation is disabled, the reconstructor does not examine the first frame, therefore
    // the output scalar type remains the default (VTK_DOUBLE). That default
    // may differ from the actual scalar type of the incoming images, which results in the runtime error:
    // OptimizedInsertSlice: input ScalarType (10) must match out ScalarType (11)
    // The following explicit update prevents the mismatch.
    if (trackedFrameList->GetNumberOfTrackedFrames() > 0)
    {
      vtkImageData* firstImage = trackedFrameList->GetTrackedFrame(0)->GetImageData()->GetImage();
      if (firstImage != nullptr)
      {
        reconstructor->SetOutputScalarType(firstImage->GetScalarType());
      }
    }
  }

  LOG_INFO("Reconstruct volume...");
  const int numberOfFrames = trackedFrameList->GetNumberOfTrackedFrames();
  int numberOfFramesAddedToVolume = 0;

  for (int frameIndex = 0; frameIndex < numberOfFrames; frameIndex += reconstructor->GetSkipInterval())
  {
    LOG_DEBUG("Frame: " << frameIndex);
    vtkPlusLogger::PrintProgressbar((100.0 * frameIndex) / numberOfFrames);

    igsioTrackedFrame* frame = trackedFrameList->GetTrackedFrame(frameIndex);

    if (transformRepository->SetTransforms(*frame) != PLUS_SUCCESS)
    {
      LOG_ERROR("Failed to update transform repository with frame #" << frameIndex);
      continue;
    }

    // Insert slice for reconstruction
    bool insertedIntoVolume = false;
    bool isFirst = frameIndex == 0;
    bool isLast = frameIndex + reconstructor->GetSkipInterval() >= numberOfFrames;
    if (reconstructor->AddTrackedFrame(frame, transformRepository, isFirst, isLast, &insertedIntoVolume) != PLUS_SUCCESS)
    {
      LOG_ERROR("Failed to add tracked frame to volume with frame #" << frameIndex);
      continue;
    }

    if (insertedIntoVolume)
    {
      numberOfFramesAddedToVolume++;
    }

    // Write an ITK image with the image pose in the reference coordinate system
    if (!outputFrameFileName.empty())
    {
      vtkSmartPointer<vtkMatrix4x4> imageToReferenceTransformMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
      if (transformRepository->GetTransform(imageToReferenceTransformName, imageToReferenceTransformMatrix) != PLUS_SUCCESS)
      {
        std::string strImageToReferenceTransformName;
        imageToReferenceTransformName.GetTransformName(strImageToReferenceTransformName);
        LOG_ERROR("Failed to get transform '" << strImageToReferenceTransformName << "' from transform repository!");
        continue;
      }

      // Print the image to reference transform
      std::ostringstream os;
      imageToReferenceTransformMatrix->Print(os);
      LOG_TRACE("Image to reference transform: \n" << os.str());

      // Insert frame index before the file extension (image.mha => image001.mha)
      std::ostringstream ss;
      size_t found;
      found = outputFrameFileName.find_last_of(".");
      ss << outputFrameFileName.substr(0, found);
      ss.width(3);
      ss.fill('0');
      ss << frameIndex;
      ss << outputFrameFileName.substr(found);

      PlusCommon::WriteToFile(frame, ss.str(), imageToReferenceTransformMatrix);
    }
  }

  vtkPlusLogger::PrintProgressbar(100);

  if (!customHeaderFieldsToSave.empty())
  {
    std::string fieldValue;
    for (unsigned int i = 0; i < customHeaderFieldsToSave.size(); ++i)
    {
      fieldValue = trackedFrameList->GetCustomString(customHeaderFieldsToSave[i]);
      customHeaderValuesToSave.push_back(fieldValue);
    }
  }

  trackedFrameList->Clear();

  LOG_INFO("Number of frames added to the volume: " << numberOfFramesAddedToVolume << " out of " << numberOfFrames);

  LOG_INFO("Saving volume to file...");
  if (!customHeaderValuesToSave.empty())
  {
    reconstructor->SaveReconstructedVolumeToFile(outputVolumeFileName, false, !disableCompression, &customHeaderFieldsToSave, &customHeaderValuesToSave);
  }
  else{
    reconstructor->SaveReconstructedVolumeToFile(outputVolumeFileName, false, !disableCompression, nullptr, nullptr);
  }

  if (!outputVolumeAccumulationFileName.empty())
  {
    reconstructor->SaveReconstructedVolumeToFile(outputVolumeAccumulationFileName, true, !disableCompression, &customHeaderFieldsToSave, &customHeaderValuesToSave);
  }

  return EXIT_SUCCESS;
}
