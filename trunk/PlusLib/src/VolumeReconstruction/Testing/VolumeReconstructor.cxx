/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"

#include "vtksys/CommandLineArguments.hxx" 
#include "vtkSmartPointer.h"
#include "vtkImageData.h"
#include "vtkXMLUtilities.h"
#include "vtkDataSetWriter.h"
//#include "vtkMetaImageWriter.h"

#include "vtkVolumeReconstructor.h"
#include "vtkTrackedFrameList.h"
#include "TrackedFrame.h"
#include "vtkTransformRepository.h"

int main (int argc, char* argv[])
{ 
  bool printHelp(false); 
  // Parse command line arguments.

  std::string inputImgSeqFileName;
  std::string inputConfigFileName;
  std::string outputVolumeFileName;
  std::string outputVolumeAlphaFileName;
  std::string outputFrameFileName; 
  std::string inputImageToReferenceTransformName; 

  int verboseLevel=vtkPlusLogger::LOG_LEVEL_DEFAULT;

  vtksys::CommandLineArguments cmdargs;
  cmdargs.Initialize(argc, argv);

  cmdargs.AddArgument("--input-transform-name", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputImageToReferenceTransformName, "Image to reference transform name used for the reconstruction");
  cmdargs.AddArgument("--input-img-seq-file-name", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputImgSeqFileName, "Input sequence metafile filename (.mha)" );
  cmdargs.AddArgument("--input-config-file-name", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputConfigFileName, "Input configuration file name (.xml)" );
  cmdargs.AddArgument("--output-volume-file-name", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &outputVolumeFileName, "Output file name of the reconstructed volume (.vtk)" );
  cmdargs.AddArgument("--output-volume-alpha-file-name", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &outputVolumeAlphaFileName, "Output file name of the alpha channel of the reconstructed volume (.vtk)" );
  cmdargs.AddArgument("--output-frame-file-name", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &outputFrameFileName, "A filename that will be used for storing the tracked image frames. Each frame will be exported individually, with the proper position and orientation in the reference coordinate system");
  cmdargs.AddArgument("--help", vtksys::CommandLineArguments::NO_ARGUMENT, &printHelp, "Print this help.");
  cmdargs.AddArgument("--verbose", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &verboseLevel, "Verbose level (1=error only, 2=warning, 3=info, 4=debug, 5=trace)");	

  if ( !cmdargs.Parse())
  {
    std::cerr << "Problem parsing arguments" << std::endl;
    std::cout << "Help: " << cmdargs.GetHelp() << std::endl;
    exit(EXIT_FAILURE);
  }

  if ( printHelp ) 
  {
    std::cout << "Help: " << cmdargs.GetHelp() << std::endl;
    exit(EXIT_SUCCESS); 
  }

  if ( inputConfigFileName.empty() )
  {
    std::cout << "ERROR: Input config file name is missing!" << std::endl;
    std::cout << "Help: " << cmdargs.GetHelp() << std::endl;
    exit( EXIT_FAILURE );
  }

  // Set the log level
  vtkPlusLogger::Instance()->SetLogLevel(verboseLevel);

  vtkSmartPointer<vtkVolumeReconstructor> reconstructor = vtkSmartPointer<vtkVolumeReconstructor>::New(); 

  LOG_INFO( "Reading configuration file:" << inputConfigFileName );
  vtkSmartPointer<vtkXMLDataElement> configRootElement = vtkSmartPointer<vtkXMLDataElement>::Take(
    vtkXMLUtilities::ReadElementFromFile(inputConfigFileName.c_str()));
  
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
  LOG_INFO("Reading image sequence...");
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
  }
  else
  {
    // Read image to reference transform from the XML configuration
    vtkXMLDataElement* reconConfig = configRootElement->FindNestedElementWithName("VolumeReconstruction");
    if (reconConfig == NULL)
    {
      LOG_ERROR("Image to reference transform is not specified at the command-line and could not load from the VolumeReconstruction element in the XML tree either!");
      return EXIT_FAILURE;
    }
    const char* referenceCoordinateFrameName=reconConfig->GetAttribute("ReferenceCoordinateFrame");
    if (referenceCoordinateFrameName==NULL)
    {
      LOG_ERROR("Image to reference transform is not specified at the command-line and could not find the ReferenceCoordinateFrame attribute in the VolumeReconstruction element in the XML tree either!");
      return EXIT_FAILURE;
    }
    const char* imageCoordinateFrameName=reconConfig->GetAttribute("ImageCoordinateFrame");
    if (imageCoordinateFrameName==NULL)
    {
      LOG_ERROR("Image to reference transform is not specified at the command-line and could not find the ImageCoordinateFrame attribute in the VolumeReconstruction element in the XML tree either!");
      return EXIT_FAILURE;
    }
    // image to reference transform is specified in the XML tree
    imageToReferenceTransformName=PlusTransformName(imageCoordinateFrameName,referenceCoordinateFrameName);
    if (!imageToReferenceTransformName.IsValid())
    { 
      LOG_ERROR("Cannot create a transform name from '" << imageCoordinateFrameName <<"' to '"<<referenceCoordinateFrameName<<"'" ); 
      return EXIT_FAILURE; 
    }
  }
  
  LOG_INFO("Set volume output extent...");
  if ( reconstructor->SetOutputExtentFromFrameList(trackedFrameList, transformRepository, imageToReferenceTransformName) != PLUS_SUCCESS )
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
    if ( reconstructor->AddTrackedFrame(frame, transformRepository, imageToReferenceTransformName, &insertedIntoVolume ) != PLUS_SUCCESS )
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



  LOG_INFO("Finalize reconstructed volume...");
  vtkSmartPointer<vtkImageData> reconstructedVolume=vtkSmartPointer<vtkImageData>::New();
  reconstructor->LoadReconstructedVolume();



  LOG_INFO("Saving volume to file...");
  vtkSmartPointer<vtkDataSetWriter> writer3D = vtkSmartPointer<vtkDataSetWriter>::New();
  writer3D->SetFileTypeToBinary();

  vtkSmartPointer<vtkImageData> gray=vtkSmartPointer<vtkImageData>::New();
  reconstructor->ExtractGrayLevels(gray);
  writer3D->SetInput(gray);
  writer3D->SetFileName(outputVolumeFileName.c_str());
  writer3D->Update();

  if (!outputVolumeAlphaFileName.empty())
  {
    vtkSmartPointer<vtkImageData> alpha=vtkSmartPointer<vtkImageData>::New();
    reconstructor->ExtractAlpha(alpha);
    writer3D->SetInput(alpha);
    writer3D->SetFileName(outputVolumeAlphaFileName.c_str());
    writer3D->Update();
  }

  return EXIT_SUCCESS; 
}
