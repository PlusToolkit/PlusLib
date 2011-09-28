#include "PlusConfigure.h"

#include "vtksys/CommandLineArguments.hxx" 
#include "vtkSmartPointer.h"
#include "vtkImageData.h"
#include "vtkXMLUtilities.h"
#include "vtkDataSetWriter.h"

#include "vtkVolumeReconstructor.h"
#include "vtkTrackedFrameList.h"

int main (int argc, char* argv[])
{ 

  // Parse command line arguments.

  std::string inputImgSeqFileName;
  std::string inputConfigFileName;
  std::string outputVolumeFileName;
  std::string outputVolumeAlphaFileName;
  std::string outputFrameFileName; 

  int verboseLevel=vtkPlusLogger::LOG_LEVEL_INFO;

  vtksys::CommandLineArguments cmdargs;
  cmdargs.Initialize(argc, argv);

  cmdargs.AddArgument( "--input-img-seq-file-name", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputImgSeqFileName, "Input sequence metafile filename (.mha)" );
  cmdargs.AddArgument( "--input-config-file-name", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputConfigFileName, "Input configuration file name (.xml)" );
  cmdargs.AddArgument( "--output-volume-file-name", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &outputVolumeFileName, "Output file name of the reconstructed volume (.vtk)" );
  cmdargs.AddArgument( "--output-volume-alpha-file-name", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &outputVolumeAlphaFileName, "Output file name of the alpha channel of the reconstructed volume (.vtk)" );
  cmdargs.AddArgument( "--output-frame-file-name", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &outputFrameFileName, "A filename that will be used for storing the tracked image frames. Each frame will be exported individually, with the proper position and orientation in the reference coordinate system");
  cmdargs.AddArgument( "--verbose", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &verboseLevel, "Verbose level (1=error only, 2=warning, 3=info, 4=debug)" );

  if ( !cmdargs.Parse() )
  {
    std::cerr << "Problem parsing arguments" << std::endl;
    std::cout << "Help: " << cmdargs.GetHelp() << std::endl;
    exit(EXIT_FAILURE);
  }

  if ( inputConfigFileName.empty() )
  {
    std::cout << "ERROR: Input config file missing!" << std::endl;
    std::cout << "Help: " << cmdargs.GetHelp() << std::endl;
    exit( EXIT_FAILURE );
  }

  // Set the log level
  vtkPlusLogger::Instance()->SetLogLevel(verboseLevel);
  vtkPlusLogger::Instance()->SetDisplayLogLevel(verboseLevel);

  vtkSmartPointer<vtkVolumeReconstructor> reconstructor = vtkSmartPointer<vtkVolumeReconstructor>::New(); 

  LOG_INFO( "Reading configuration file:" << inputConfigFileName );
  vtkXMLDataElement *configRead = vtkXMLUtilities::ReadElementFromFile(inputConfigFileName.c_str());
  reconstructor->ReadConfiguration(configRead);
  configRead->Delete();
  configRead=NULL;

  // Print calibration transform
  std::ostringstream osTransformImageToTool; 
  reconstructor->GetImageToToolTransform()->GetMatrix()->Print( osTransformImageToTool );
  LOG_DEBUG("Image to tool (probe calibration) transform: \n" << osTransformImageToTool.str());  

  // Read image sequence
  LOG_INFO("Reading image sequence...");
  vtkSmartPointer<vtkTrackedFrameList> trackedFrameList = vtkSmartPointer<vtkTrackedFrameList>::New(); 
  trackedFrameList->ReadFromSequenceMetafile(inputImgSeqFileName.c_str()); 

  // Reconstruct volume 
  
  LOG_INFO("Reconstruct volume...");
  reconstructor->SetOutputExtentFromFrameList(trackedFrameList);
  const int numberOfFrames = trackedFrameList->GetNumberOfTrackedFrames(); 
  for ( int frameIndex = 0; frameIndex < numberOfFrames; ++frameIndex )
  {
    LOG_DEBUG("Frame: "<<frameIndex);
    vtkPlusLogger::PrintProgressbar( (100.0 * frameIndex) / numberOfFrames ); 

    TrackedFrame* frame = trackedFrameList->GetTrackedFrame( frameIndex );

    // Insert slice for reconstruction
    reconstructor->AddTrackedFrame(frame);

    // Write an ITK image with the image pose in the reference coordinate system
    if (!outputFrameFileName.empty())
    {       
      vtkSmartPointer<vtkMatrix4x4> imageToReferenceTransformMatrix=vtkSmartPointer<vtkMatrix4x4>::New();
      if ( reconstructor->GetImageToReferenceTransformMatrix(frame, imageToReferenceTransformMatrix)!=PLUS_SUCCESS )		
      {
        LOG_ERROR("Unable to get image to reference transform for frame #" << frameIndex); 
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

  LOG_INFO("Saving volume to file...");
  vtkSmartPointer<vtkImageData> reconstructedVolume=vtkSmartPointer<vtkImageData>::New();
  reconstructor->GetReconstructedVolume(reconstructedVolume);

  vtkSmartPointer<vtkDataSetWriter> writer3D = vtkSmartPointer<vtkDataSetWriter>::New();
  writer3D->SetFileTypeToBinary();
  writer3D->SetInput(reconstructedVolume);
  writer3D->SetFileName(outputVolumeFileName.c_str());
  writer3D->Update();

  if (!outputVolumeAlphaFileName.empty())
  {
    reconstructor->GetReconstructedVolumeAlpha(reconstructedVolume);
    writer3D->SetInput(reconstructedVolume);
    writer3D->SetFileName(outputVolumeAlphaFileName.c_str());
    writer3D->Update();
  }

  return EXIT_SUCCESS; 
}
