#include "PlusConfigure.h"
//#include "vtkVolumeReconstructor.h"
#include "vtkVolumeReconstructorFilter.h"
#include "vtkSmartPointer.h"
#include "vtksys/CommandLineArguments.hxx" 
#include "vtksys/SystemTools.hxx"
#include "vtkImageData.h"
#include "vtkXMLUtilities.h"
#include "vtkTrackedFrameList.h"

#include "vtkTracker.h"
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkMetaImageSequenceIO.h"
#include "vtkDataSetWriter.h"
#include "vtkXMLImageDataWriter.h"
#include "vtkImageExtractComponents.h"

typedef unsigned char PixelType;
typedef itk::Image< PixelType, 2 > ImageType;
typedef itk::Image< PixelType, 3 > ImageSequenceType;
typedef itk::ImageFileReader< ImageSequenceType > ImageSequenceReaderType;

//----------------------------------------------------------------------------
void AddToExtent( vtkImageData *image, vtkMatrix4x4* mImageToReference, double* extent_Ref)
{
  // Output volume is in the Reference coordinate system.

  // Prepare the four corner points of the input US image.
  int* frameExtent=image->GetExtent();
  std::vector< double* > corners_ImagePix;
  double c0[ 4 ] = { frameExtent[ 0 ], frameExtent[ 2 ], 0,  1 };
  double c1[ 4 ] = { frameExtent[ 0 ], frameExtent[ 3 ], 0,  1 };
  double c2[ 4 ] = { frameExtent[ 1 ], frameExtent[ 2 ], 0,  1 };
  double c3[ 4 ] = { frameExtent[ 1 ], frameExtent[ 3 ], 0,  1 };
  corners_ImagePix.push_back( c0 );
  corners_ImagePix.push_back( c1 );
  corners_ImagePix.push_back( c2 );
  corners_ImagePix.push_back( c3 );

  // Transform the corners to Reference. Check for MIN MAX update.

  for ( unsigned int corner = 0; corner < corners_ImagePix.size(); ++corner )
  {
    double corner_Ref[ 4 ] = { 0, 0, 0, 1 };
    mImageToReference->MultiplyPoint( corners_ImagePix[corner], corner_Ref );

    for ( int axis = 0; axis < 3; axis ++ )
    {
      if ( corner_Ref[axis] < extent_Ref[axis*2] )
      {
        // min extent along this coord axis has to be decreased
        extent_Ref[axis*2]=corner_Ref[axis];
      }
      if ( corner_Ref[axis] > extent_Ref[axis*2+1] )
      {
        // max extent along this coord axis has to be increased
        extent_Ref[axis*2+1]=corner_Ref[axis];
      }
    }
  }
} 


int main (int argc, char* argv[])
{ 
  // Parse command line arguments.

  std::string inputImgSeqFileName;
  std::string inputConfigFileName;
  std::string outputVolumeFileName;
  std::string outputFrameFileName; 

  int verboseLevel=vtkPlusLogger::LOG_LEVEL_INFO;
  VTK_LOG_TO_CONSOLE_ON; 

  vtksys::CommandLineArguments cmdargs;
  cmdargs.Initialize(argc, argv);

  cmdargs.AddArgument( "--input-img-seq-file-name", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputImgSeqFileName, "" );
  cmdargs.AddArgument( "--input-config-file-name", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputConfigFileName, "" );
  cmdargs.AddArgument( "--output-volume-file-name", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &outputVolumeFileName, "" );
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


  //vtkSmartPointer<vtkVolumeReconstructor> reconstructor = vtkSmartPointer<vtkVolumeReconstructor>::New(); 
  vtkSmartPointer<vtkVolumeReconstructorFilter> reconstructor = vtkSmartPointer<vtkVolumeReconstructorFilter>::New(); 


  LOG_INFO( "Reading configuration file:" << inputConfigFileName );
  vtkXMLDataElement *configRead = vtkXMLUtilities::ReadElementFromFile(inputConfigFileName.c_str());
  vtkSmartPointer<vtkXMLDataElement> config=configRead;
  configRead->Delete();
  configRead=NULL;

  reconstructor->ReadConfiguration(config);


  //***************************  Image sequence reading *****************************
  LOG_INFO("Reading image sequence...");
  vtkSmartPointer<vtkTrackedFrameList> trackedFrameList = vtkSmartPointer<vtkTrackedFrameList>::New(); 
  trackedFrameList->ReadFromSequenceMetafile(inputImgSeqFileName.c_str()); 


  //***************************  Volume reconstruction ***************************** 

  LOG_INFO("Reading input files...");

  // reconstruction transforms

  vtkSmartPointer<vtkXMLDataElement> dataCollectionConfig = config->FindNestedElementWithName("USDataCollection");
  if (dataCollectionConfig == NULL)
  {
    LOG_ERROR("Cannot find USDataCollection element in XML tree!");
    return PLUS_FAIL;
  }
  vtkSmartPointer<vtkXMLDataElement> trackerDefinition = dataCollectionConfig->FindNestedElementWithName("Tracker"); 
  if ( trackerDefinition == NULL) 
  {
    LOG_ERROR("Cannot find Tracker element in XML tree!");
    return PLUS_FAIL;
  }
  std::string toolType;
  vtkTracker::ConvertToolTypeToString(TRACKER_TOOL_PROBE, toolType);
  vtkSmartPointer<vtkXMLDataElement> probeDefinition = trackerDefinition->FindNestedElementWithNameAndAttribute("Tool", "Type", toolType.c_str());
  if (probeDefinition == NULL) {
    LOG_ERROR("No probe definition is found in the XML tree!");
    return PLUS_FAIL;
  }
  vtkSmartPointer<vtkXMLDataElement> calibration = probeDefinition->FindNestedElementWithName("Calibration");
  if (calibration == NULL) {
    LOG_ERROR("No calibration section is found in probe definition!");
    return PLUS_FAIL;
  }
  double aImageToTool[16];
  if (!calibration->GetVectorAttribute("MatrixValue", 16, aImageToTool)) 
  {
    LOG_ERROR("No calibration matrix is found in probe definition!");
    return EXIT_FAILURE;
  }

  // Transformation chain:
  // ImageToReference =
  // = ReferenceFromImage
  // = ReferenceFromTool * ToolFromImage
  // = ToolToReference * ImageToTool

  // tToolToReference: different for each frame (comes from the tracker)
  vtkSmartPointer< vtkTransform > tToolToReference = vtkSmartPointer< vtkTransform >::New();

  // tImageToTool: the calibration matrix, constant throughout the reconstruction
  vtkSmartPointer< vtkTransform > tImageToTool = vtkSmartPointer< vtkTransform >::New();
  tImageToTool->SetMatrix( aImageToTool );
  //tImageToTool->Update();

  vtkSmartPointer< vtkTransform > tImageToReference = vtkSmartPointer< vtkTransform >::New();
  tImageToReference->Identity();
  tImageToReference->Concatenate( tToolToReference );
  tImageToReference->Concatenate( tImageToTool );

  const int numberOfFrames = trackedFrameList->GetNumberOfTrackedFrames(); 

  LOG_INFO("Scanning images to determine volume extents...");
  double extent_Ref[6]={VTK_DOUBLE_MAX, VTK_DOUBLE_MIN,
    VTK_DOUBLE_MAX, VTK_DOUBLE_MIN,
    VTK_DOUBLE_MAX, VTK_DOUBLE_MIN};

  for ( int frameIndex = 0; frameIndex < numberOfFrames; ++frameIndex )
  {
    TrackedFrame* frame = trackedFrameList->GetTrackedFrame( frameIndex );

    double defaultTransform[16]; 
    if ( !frame->GetDefaultFrameTransform(defaultTransform) )		
    {
      LOG_ERROR("Unable to get default frame transform for frame #" << frameIndex); 
      continue; 
    }
    tToolToReference->SetMatrix( defaultTransform );
    tImageToReference->Update();  
    vtkImageData* frameImage=trackedFrameList->GetTrackedFrame(frameIndex)->ImageData.GetVtkImageNonFlipped();

    AddToExtent(frameImage, tImageToReference->GetMatrix(), extent_Ref);
  }

  // Set the output extent from the current min and max values.

  int outputExtent[ 6 ] = { 0, 0, 0, 0, 0, 0 };
  double* outputSpacing = reconstructor->GetOutputSpacing();
  outputExtent[ 1 ] = int( ( extent_Ref[1] - extent_Ref[0] ) / outputSpacing[ 0 ] );
  outputExtent[ 3 ] = int( ( extent_Ref[3] - extent_Ref[2] ) / outputSpacing[ 1 ] );
  outputExtent[ 5 ] = int( ( extent_Ref[5] - extent_Ref[4] ) / outputSpacing[ 2 ] );

  reconstructor->SetOutputExtent( outputExtent );
  reconstructor->SetOutputOrigin( extent_Ref[0], extent_Ref[2], extent_Ref[4] ); 
  reconstructor->ResetOutput(); // :TODO: call this automatically

  LOG_INFO("Reconstruct volume...");

  std::ostringstream osTransformImageToTool; 
  tImageToTool->GetMatrix()->Print( osTransformImageToTool );
  LOG_DEBUG("Image to tool (probe calibration) transform: \n" << osTransformImageToTool.str());  

  for ( int frameIndex = 0; frameIndex < numberOfFrames; ++frameIndex )
  {
    LOG_DEBUG("Frame: "<<frameIndex);
    vtkPlusLogger::PrintProgressbar( (100.0 * frameIndex) / numberOfFrames ); 

    TrackedFrame* frame = trackedFrameList->GetTrackedFrame( frameIndex );

    double defaultTransform[16]; 
    if ( !frame->GetDefaultFrameTransform(defaultTransform) )		
    {
      LOG_ERROR("Unable to get default frame transform for frame #" << frameIndex); 
      continue; 
    }
    tToolToReference->SetMatrix( defaultTransform );
    tImageToReference->Update();
   
    {
      std::ostringstream os; 
      tImageToReference->GetMatrix()->Print( os );
      LOG_TRACE("Image to reference transform: \n" << os.str());  
    }

    // Add each tracked frame to reconstructor - US image orientation always MF in tracked frame list  
    vtkImageData* frameImage=trackedFrameList->GetTrackedFrame(frameIndex)->ImageData.GetVtkImageNonFlipped();

    if (!outputFrameFileName.empty())
    {       
      // Insert frame index before the file extension (image.mha => image001.mha)
      std::ostringstream ss;
      size_t found;
      found=outputFrameFileName.find_last_of(".");
      ss << outputFrameFileName.substr(0,found);
      ss.width(3);
      ss.fill('0');
      ss << frameIndex;
      ss << outputFrameFileName.substr(found);

      trackedFrameList->GetTrackedFrame(frameIndex)->WriteToFile(ss.str(), tImageToReference->GetMatrix());
    }

    reconstructor->InsertSlice(frameImage, tImageToReference->GetMatrix());
  }

  vtkPlusLogger::PrintProgressbar( 100 ); 

  trackedFrameList->Clear(); 

  LOG_INFO("Fill holes in output volume...");
  //reconstructor->FillHolesInOutput();  // TODO: move fillholes function into a separate class

  LOG_INFO("Saving volume to file...");
  vtkSmartPointer<vtkImageExtractComponents> extract = vtkSmartPointer<vtkImageExtractComponents>::New();
  vtkSmartPointer<vtkDataSetWriter> writer3D = vtkSmartPointer<vtkDataSetWriter>::New();
  // keep only 0th component
  extract->SetComponents(0);
  extract->SetInput(reconstructor->GetReconstructedVolume());

  // write out to file
  writer3D->SetFileTypeToBinary();
  writer3D->SetInput(extract->GetOutput());
  writer3D->SetFileName(outputVolumeFileName.c_str());
  writer3D->Update();

  // write out the image data file 
  //vtkSmartPointer<vtkXMLImageDataWriter> imgWriter3D = vtkSmartPointer<vtkXMLImageDataWriter>::New();
  //imgWriter3D->SetInputConnection( reconstructor->GetReconstructor()->GetOutputPort(0) );
  //imgWriter3D->SetFileName(outputVolumeFileName.c_str());
  ////imgWriter3D->SetNumberOfPieces(64);
  //imgWriter3D->SetDataModeToAscii(); 
  //imgWriter3D->Write();


  VTK_LOG_TO_CONSOLE_OFF; 
  return EXIT_SUCCESS; 
}

