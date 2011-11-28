/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

/**
* This program creates a vtkPolyData model that represents tracked ultrasound
* image slices in their tracked positions.
* It can be used to debug geometry problems in volume reconstruction.
* 
*/

#include "PlusConfigure.h"
#include "vtksys/CommandLineArguments.hxx"

#include <iostream>

#include "vtkAppendPolyData.h"
#include "vtkCubeSource.h"
#include "vtkMatrix4x4.h"
#include "vtkPolyData.h"
#include "vtkPolyDataWriter.h"
#include "vtkSmartPointer.h"
#include "vtkTransform.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkXMLUtilities.h"

#include "vtkTrackedFrameList.h"
#include "vtkVolumeReconstructor.h"


int main( int argc, char** argv )
{

  bool printHelp(false);
  std::string inputMetaFilename;
  std::string inputConfigFileName; 
  std::string outputModelFilename; 

  int verboseLevel = vtkPlusLogger::LOG_LEVEL_DEFAULT;

  vtksys::CommandLineArguments args;
  args.Initialize(argc, argv);

  args.AddArgument("--help", vtksys::CommandLineArguments::NO_ARGUMENT, &printHelp, "Print this help.");	
  args.AddArgument("--input-metafile", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputMetaFilename, "Tracked ultrasound recorded by Plus (e.g., by the TrackedUltrasoundCapturing application) in a sequence metafile (.mha)");
  args.AddArgument("--input-configfile", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputConfigFileName, "Config file used for volume reconstrucion. It contains the probe calibration matrix, the ImageToTool transform (.xml) ");
  args.AddArgument("--output-modelfile", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &outputModelFilename, "A 3D model file that contains rectangles corresponding to each US image slice (.vtk)");
	args.AddArgument("--verbose", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &verboseLevel, "Verbose level (1=error only, 2=warning, 3=info, 4=debug, 5=trace)");	

  if ( !args.Parse() )
  {
    std::cerr << "Problem parsing arguments" << std::endl;
    std::cout << "Help: " << args.GetHelp() << std::endl;
    exit(EXIT_FAILURE);
  }

  if ( printHelp ) 
  {
    std::cout << "Help: " << args.GetHelp() << std::endl;
    exit(EXIT_SUCCESS); 

  }

	vtkPlusLogger::Instance()->SetLogLevel(verboseLevel);

  if ( inputMetaFilename.empty() )
  {
    std::cerr << "--input-metafile argument required!" << std::endl; 
    std::cout << "Help: " << args.GetHelp() << std::endl;
    exit(EXIT_FAILURE); 
  }
  if ( inputConfigFileName.empty() )
  {
    std::cerr << "--input-configfile argument required!" << std::endl; 
    std::cout << "Help: " << args.GetHelp() << std::endl;
    exit(EXIT_FAILURE); 
  }
  if ( outputModelFilename.empty() )
  {
    std::cerr << "--output-modelfile argument required!" << std::endl; 
    std::cout << "Help: " << args.GetHelp() << std::endl;
    exit(EXIT_FAILURE); 
  }

  // Read input tracked ultrasound data.

  LOG_DEBUG("Reading input... ");
  vtkSmartPointer< vtkTrackedFrameList > trackedFrameList = vtkSmartPointer< vtkTrackedFrameList >::New(); 
  trackedFrameList->ReadFromSequenceMetafile( inputMetaFilename.c_str() );
  LOG_DEBUG("Reading input done.");

  LOG_DEBUG("Number of frames: " << trackedFrameList->GetNumberOfTrackedFrames());

  // Read volume reconstruction config file.

  LOG_DEBUG("Reading config file...");
  vtkSmartPointer< vtkVolumeReconstructor > reconstructor = vtkSmartPointer< vtkVolumeReconstructor >::New();   
  vtkSmartPointer<vtkXMLDataElement> configRead = vtkSmartPointer<vtkXMLDataElement>::Take(
    vtkXMLUtilities::ReadElementFromFile(inputConfigFileName.c_str()));
  reconstructor->ReadConfiguration(configRead);
  LOG_DEBUG("Reading config file done.");

  const vtkMatrix4x4* mImageToTool = reconstructor->GetImageToToolTransform()->GetMatrix();
  if ( mImageToTool == NULL )
  {
    LOG_ERROR("ERROR: ImageToTool calibration matrix not defined. Cannot continue.");
    return EXIT_FAILURE;
  }

  vtkSmartPointer< vtkMatrix4x4 > mImageToTool2 = vtkSmartPointer< vtkMatrix4x4 >::New();
  mImageToTool2->DeepCopy( const_cast< vtkMatrix4x4* >( mImageToTool ) );

  vtkSmartPointer< vtkTransform > tImageToTool = vtkSmartPointer< vtkTransform >::New();
  tImageToTool->SetMatrix( mImageToTool2 );
  tImageToTool->Update();

  // Prepare the output polydata.
  vtkSmartPointer< vtkPolyData > outputPolyData = vtkSmartPointer< vtkPolyData >::New();
  vtkSmartPointer< vtkAppendPolyData > appender = vtkSmartPointer< vtkAppendPolyData >::New();
  appender->SetInput( outputPolyData );

  // Loop over each tracked image slice.
  PlusTransformName defaultFrameTransformName=trackedFrameList->GetDefaultFrameTransformName();
  for ( int frameIndex = 0; frameIndex < trackedFrameList->GetNumberOfTrackedFrames(); ++ frameIndex )
  {
    TrackedFrame* frame = trackedFrameList->GetTrackedFrame( frameIndex );

    double defaultTransform[ 16 ];
    frame->GetCustomFrameTransform(defaultFrameTransformName, defaultTransform);
    vtkSmartPointer< vtkTransform > tToolToTracker = vtkSmartPointer< vtkTransform >::New();
    tToolToTracker->SetMatrix( defaultTransform );    

    int* frameSize = frame->GetFrameSize();

    vtkSmartPointer< vtkTransform > tCubeToImage = vtkSmartPointer< vtkTransform >::New();
    tCubeToImage->Scale( frameSize[ 0 ], frameSize[ 1 ], 1 );
    tCubeToImage->Translate( 0.5, 0.5, 0.5 );  // Moving the corner to the origin.

    vtkSmartPointer< vtkTransform > tCubeToTracker = vtkSmartPointer< vtkTransform >::New();
    tCubeToTracker->Identity();
    tCubeToTracker->Concatenate( tToolToTracker );
    tCubeToTracker->Concatenate( tImageToTool );
    tCubeToTracker->Concatenate( tCubeToImage );

    vtkSmartPointer< vtkTransformPolyDataFilter > CubeToTracker = vtkSmartPointer< vtkTransformPolyDataFilter >::New();
    CubeToTracker->SetTransform( tCubeToTracker );
    vtkSmartPointer< vtkCubeSource > source = vtkSmartPointer< vtkCubeSource >::New();
    CubeToTracker->SetInput( source->GetOutput() );
    CubeToTracker->Update();

    appender->AddInputConnection( CubeToTracker->GetOutputPort() );

  }  

  // Write model output.
  LOG_DEBUG("Writing output model file ("<<outputModelFilename<<")...");
  vtkSmartPointer< vtkPolyDataWriter > writer = vtkSmartPointer< vtkPolyDataWriter >::New();
  writer->SetFileName( outputModelFilename.c_str() );
  writer->SetInput( appender->GetOutput() );
  writer->Update();
  LOG_DEBUG("Writing model file done.");
  LOG_INFO("Model file created: "<<outputModelFilename);

  return EXIT_SUCCESS;
}
