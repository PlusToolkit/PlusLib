/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"

#include "vtksys/CommandLineArguments.hxx"

#include "TrackedFrame.h"

#include "vtkImageData.h"
#include "vtkMatrix4x4.h"
#include "vtkTrackedFrameList.h"
#include "vtkTransformRepository.h"
#include "vtkVolumeReconstructor.h"
#include "vtkXMLUtilities.h"

int main (int argc, char* argv[])
{ 
  bool printHelp(false); 
  // Parse command line arguments.

  std::string inputImgSeqFileName;
  std::string inputConfigFileName;
  std::string outputVolumeFileName;
  std::string outputVolumeAlphaFileNameDeprecated;
  std::string outputVolumeAccumulationFileName;
  std::string outputFrameFileName; 
  std::string inputImageToReferenceTransformName; 
  
  // Deprecated arguments (2013-07-29, #800)
  std::string inputImageToReferenceTransformNameDeprecated; 
  std::string inputImgSeqFileNameDeprecated;

  int verboseLevel=vtkPlusLogger::LOG_LEVEL_UNDEFINED;

  vtksys::CommandLineArguments cmdargs;
  cmdargs.Initialize(argc, argv);

  cmdargs.AddArgument("--image-to-reference-transform", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputImageToReferenceTransformName, "Name of the transform to define the image slice pose relative to the reference coordinate system (e.g., ImageToReference). Note that this parameter is optional, if it is defined then it overrides the ImageCoordinateFrame and ReferenceCoordinateFrame attribute values in the configuration file.");
  cmdargs.AddArgument("--source-seq-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputImgSeqFileName, "Input sequence metafile filename (.mha)" );
  cmdargs.AddArgument("--config-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputConfigFileName, "Input configuration file name (.xml)" );
  cmdargs.AddArgument("--output-volume-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &outputVolumeFileName, "Output file name of the reconstructed volume (must have .mha or .mhd extension)" );
  cmdargs.AddArgument("--output-volume-accumulation-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &outputVolumeAccumulationFileName, "Output file name of the accumulation of the reconstructed volume (.mha)" );
  cmdargs.AddArgument("--output-frame-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &outputFrameFileName, "A filename that will be used for storing the tracked image frames. Each frame will be exported individually, with the proper position and orientation in the reference coordinate system");
  cmdargs.AddArgument("--help", vtksys::CommandLineArguments::NO_ARGUMENT, &printHelp, "Print this help.");
  cmdargs.AddArgument("--verbose", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &verboseLevel, "Verbose level (1=error only, 2=warning, 3=info, 4=debug, 5=trace)");  

  // Deprecated arguments (2013-07-29, #800)
  cmdargs.AddArgument("--transform", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputImageToReferenceTransformNameDeprecated, "Image to reference transform name used for the reconstruction. DEPRECATED, use --image-to-reference-transform argument instead");
  cmdargs.AddArgument("--img-seq-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputImgSeqFileNameDeprecated, "Input sequence metafile filename (.mha). DEPRECATED: use --source-seq-file argument instead" );
  // Deprecated argument (2014-08-15, #923)
  cmdargs.AddArgument("--output-volume-alpha-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &outputVolumeAlphaFileNameDeprecated, "Output file name of the alpha channel of the reconstructed volume (.mha). DEPRECATED: use --output-volume-accumulation-file argument instead" );

  if ( !cmdargs.Parse())
  {
    std::cerr << "Problem parsing arguments" << std::endl;
    std::cout << "Help: " << cmdargs.GetHelp() << std::endl;
    exit(EXIT_FAILURE);
  }

  if ( printHelp ) 
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
      inputImageToReferenceTransformName=inputImageToReferenceTransformNameDeprecated;
    }
  }
  if (!inputImgSeqFileNameDeprecated.empty())
  {
    LOG_WARNING("The --img-seq-file argument is deprecated. Use --source-seq-file instead.");
    if (inputImgSeqFileName.empty())
    {
      inputImgSeqFileName=inputImgSeqFileNameDeprecated;
    }
  }
  // Deprecated argument (2014-08-15, #923)
  if (!outputVolumeAlphaFileNameDeprecated.empty())
  {
    LOG_WARNING("The --output-volume-alpha-file argument is deprecated. Use --output-volume-accumulation-file instead.");
    if (outputVolumeAccumulationFileName.empty())
    {
      outputVolumeAccumulationFileName=outputVolumeAlphaFileNameDeprecated;
    }
  }

  if ( inputConfigFileName.empty() )
  {
    std::cout << "ERROR: Input config file name is missing!" << std::endl;
    std::cout << "Help: " << cmdargs.GetHelp() << std::endl;
    exit( EXIT_FAILURE );
  }

  vtkSmartPointer<vtkVolumeReconstructor> reconstructor = vtkSmartPointer<vtkVolumeReconstructor>::New(); 

  LOG_INFO( "Reading configuration file:" << inputConfigFileName );
  vtkSmartPointer<vtkXMLDataElement> configRootElement = vtkSmartPointer<vtkXMLDataElement>::New();
  if (PlusXmlUtils::ReadDeviceSetConfigurationFromFile(configRootElement, inputConfigFileName.c_str())==PLUS_FAIL)
  {  
    LOG_ERROR("Unable to read configuration from file " << inputConfigFileName.c_str()); 
    return EXIT_FAILURE;
  }
    
  if ( reconstructor->ReadConfiguration(configRootElement) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to read configuration from "<<inputConfigFileName.c_str()); 
    return EXIT_FAILURE; 
  }

  vtkSmartPointer<vtkTransformRepository> transformRepository = vtkSmartPointer<vtkTransformRepository>::New();
  if (configRootElement->FindNestedElementWithName("CoordinateDefinitions")!=NULL)
  {
    if ( transformRepository->ReadConfiguration(configRootElement) != PLUS_SUCCESS )
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
  LOG_INFO("Reading image sequence " << inputImgSeqFileName );
  vtkSmartPointer<vtkTrackedFrameList> trackedFrameList = vtkSmartPointer<vtkTrackedFrameList>::New(); 
  trackedFrameList->ReadFromSequenceMetafile(inputImgSeqFileName.c_str()); 

  // Reconstruct volume 
  PlusTransformName imageToReferenceTransformName;
  if (!inputImageToReferenceTransformName.empty())
  {
    // image to reference transform is specified at the command-line
    if ( imageToReferenceTransformName.SetTransformName(inputImageToReferenceTransformName.c_str()) != PLUS_SUCCESS )
    { 
      LOG_ERROR("Invalid image to reference transform name: " << inputImageToReferenceTransformName ); 
      return EXIT_FAILURE; 
    }
    reconstructor->SetImageCoordinateFrame(imageToReferenceTransformName.From().c_str());
    reconstructor->SetReferenceCoordinateFrame(imageToReferenceTransformName.To().c_str());
  }
  
  LOG_INFO("Set volume output extent...");
  std::string errorDetail;
  if ( reconstructor->SetOutputExtentFromFrameList(trackedFrameList, transformRepository, errorDetail) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to set output extent of volume!"); 
    return EXIT_FAILURE; 
  }

  LOG_INFO("Reconstruct volume...");
  const int numberOfFrames = trackedFrameList->GetNumberOfTrackedFrames(); 
  int numberOfFramesAddedToVolume=0; 

  for ( int frameIndex = 0; frameIndex < numberOfFrames; frameIndex+=reconstructor->GetSkipInterval() )
  {
    LOG_DEBUG("Frame: "<<frameIndex);
    vtkPlusLogger::PrintProgressbar( (100.0 * frameIndex) / numberOfFrames ); 

    TrackedFrame* frame = trackedFrameList->GetTrackedFrame( frameIndex );

    if ( transformRepository->SetTransforms(*frame) != PLUS_SUCCESS )
    {
      LOG_ERROR("Failed to update transform repository with frame #" << frameIndex ); 
      continue; 
    }

    // Insert slice for reconstruction
    bool insertedIntoVolume=false;
    if ( reconstructor->AddTrackedFrame(frame, transformRepository, &insertedIntoVolume ) != PLUS_SUCCESS )
    {
      LOG_ERROR("Failed to add tracked frame to volume with frame #" << frameIndex); 
      continue; 
    }

    if ( insertedIntoVolume )
    {
      numberOfFramesAddedToVolume++; 
    }

    // Write an ITK image with the image pose in the reference coordinate system
    if (!outputFrameFileName.empty())
    {       
      vtkSmartPointer<vtkMatrix4x4> imageToReferenceTransformMatrix=vtkSmartPointer<vtkMatrix4x4>::New();
      if ( transformRepository->GetTransform(imageToReferenceTransformName, imageToReferenceTransformMatrix) != PLUS_SUCCESS )
      {
        std::string strImageToReferenceTransformName; 
        imageToReferenceTransformName.GetTransformName(strImageToReferenceTransformName); 
        LOG_ERROR("Failed to get transform '"<<strImageToReferenceTransformName<<"' from transform repository!"); 
        continue; 
      }

      // Print the image to reference transform
      std::ostringstream os; 
      imageToReferenceTransformMatrix->Print( os );
      LOG_TRACE("Image to reference transform: \n" << os.str());  
      
      // Insert frame index before the file extension (image.mha => image001.mha)
      std::ostringstream ss;
      size_t found;
      found=outputFrameFileName.find_last_of(".");
      ss << outputFrameFileName.substr(0,found);
      ss.width(3);
      ss.fill('0');
      ss << frameIndex;
      ss << outputFrameFileName.substr(found);

      frame->WriteToFile(ss.str(), imageToReferenceTransformMatrix);
    }
  }

  vtkPlusLogger::PrintProgressbar( 100 ); 

  trackedFrameList->Clear(); 

  LOG_INFO("Number of frames added to the volume: " << numberOfFramesAddedToVolume << " out of " << numberOfFrames ); 

  LOG_INFO("Saving volume to file...");
  reconstructor->SaveReconstructedVolumeToMetafile(outputVolumeFileName.c_str());
  
  if (!outputVolumeAccumulationFileName.empty())
  {
    reconstructor->SaveReconstructedVolumeToMetafile(outputVolumeAccumulationFileName.c_str(), true);
  }

  return EXIT_SUCCESS; 
}
