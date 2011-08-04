
#include <iostream>

#include "vtkAppendPolyData.h"
#include "vtkCubeSource.h"
#include "vtkMatrix4x4.h"
#include "vtkPolyData.h"
#include "vtkPolyDataWriter.h"
#include "vtkSmartPointer.h"
#include "vtkTransform.h"
#include "vtkTransformPolyDataFilter.h"

#include "vtkTrackedFrameList.h"
#include "vtkVolumeReconstructor.h"



/**
 * This program creates a vtkPolyData model that represents tracked ultrasound
 * image slices in their tracked positions.
 * It can be used to debug geometry problems in volume reconstruction.
 *
 * Input:  .mha format tracked ultrasound recorded by Plus (e.g. TrackedUltrasoundCapturin app).
 *         .xml format config file used for volume reconstrucion (for calibration matrix).
 * Output: .vtk format vtkPolyData.
 * 
 */
int main( int argc, char** argv )
{
  
    // Check command line arguments.
  
  if ( argc != 4 )
    {
    std::cout << "Usage: " << argv[ 0 ] << " input.mha output.vtk config.xml" << std::endl;
    return 1;
    }
  
  const char* InputFileName = argv[ 1 ];
  const char* OutputFileName = argv[ 2 ];
  const char* ConfigFileName = argv[ 3 ];
  
  
    // Read input tracked ultrasound data.
  
  std::cout << "Reading input... " << std::endl;
  
  vtkSmartPointer< vtkTrackedFrameList > trackedFrameList = vtkSmartPointer< vtkTrackedFrameList >::New(); 
	trackedFrameList->ReadFromSequenceMetafile( InputFileName );
	
	std::cout << "Reading input done." << std::endl;
  
  std::cout << "Number of frames: " << trackedFrameList->GetNumberOfTrackedFrames() << std::endl;
  
  int* FrameSize = trackedFrameList->GetFrameSize();
  
  std::cout << std::endl;
  
  
    // Read volume reconstruction config file.
  
  std::cout << "Reading config file..." << std::endl;
  
  vtkSmartPointer< vtkVolumeReconstructor > reconstructor = vtkSmartPointer< vtkVolumeReconstructor >::New(); 
  reconstructor->ReadConfiguration( ConfigFileName );
  
  std::cout << "Reading config file done." << std::endl;
  
  const vtkMatrix4x4* mImageToTool = reconstructor->GetImageToToolMatrix();
  
  if ( mImageToTool == NULL )
    {
    std::cout << "ERROR: ImageToTool calibration matrix not defined. Cannot continue." << std::endl;
    return 1;
    }
  
  std::cout << "ImageToTool calibration matrix:" << std::endl;
  const_cast< vtkMatrix4x4* >( mImageToTool )->Print( std::cout );
  
  vtkSmartPointer< vtkMatrix4x4 > mImageToTool2 = vtkSmartPointer< vtkMatrix4x4 >::New();
  mImageToTool2->DeepCopy( const_cast< vtkMatrix4x4* >( mImageToTool ) );
  
  vtkSmartPointer< vtkTransform > tImageToTool = vtkSmartPointer< vtkTransform >::New();
  tImageToTool->SetMatrix( mImageToTool2 );
  tImageToTool->Update();
  
  std::cout << std::endl;
  
  
    // Prepare the output polydata.
  
  vtkSmartPointer< vtkPolyData > outputPolyData = vtkSmartPointer< vtkPolyData >::New();
  
  vtkSmartPointer< vtkAppendPolyData > appender = vtkSmartPointer< vtkAppendPolyData >::New();
  appender->SetInput( outputPolyData );
  
  
    // Loop over each tracked image slice.
  
  for ( int frameIndex = 0; frameIndex < trackedFrameList->GetNumberOfTrackedFrames(); ++ frameIndex )
    {
    TrackedFrame* frame = trackedFrameList->GetTrackedFrame( frameIndex );
    double defaultTransform[ 16 ];
    frame->GetDefaultFrameTransform( defaultTransform);
    vtkSmartPointer< vtkMatrix4x4 > mToolToTracker = vtkSmartPointer< vtkMatrix4x4 >::New();
    mToolToTracker->DeepCopy( defaultTransform );
    
    
    //debug
    std::cerr << "ToolToReference:" << std::endl;
    for ( int r = 0; r < 4; ++ r )
      {
      for ( int c = 0; c < 4; ++ c )
        {
        std::cerr << mToolToTracker->GetElement( r, c ) << "   ";
        }
      std::cerr << std::endl;
      }
    std::cerr << std::endl;
    
    
    vtkSmartPointer< vtkTransform > tToolToTracker = vtkSmartPointer< vtkTransform >::New();
    tToolToTracker->SetMatrix( mToolToTracker );
    tToolToTracker->Update();
    
    vtkSmartPointer< vtkCubeSource > source = vtkSmartPointer< vtkCubeSource >::New();
    
    vtkSmartPointer< vtkTransform > tCubeToImage = vtkSmartPointer< vtkTransform >::New();
    tCubeToImage->Scale( FrameSize[ 0 ], FrameSize[ 1 ], 1 );
    tCubeToImage->Translate( 0.5, 0.5, 0.5 );  // Moving the corner to the origin.
    
    vtkSmartPointer< vtkTransform > tCubeToTracker = vtkSmartPointer< vtkTransform >::New();
    tCubeToTracker->Identity();
    tCubeToTracker->Concatenate( tToolToTracker );
    tCubeToTracker->Concatenate( tImageToTool );
    tCubeToTracker->Concatenate( tCubeToImage );
    
    vtkSmartPointer< vtkTransformPolyDataFilter > CubeToTracker = vtkSmartPointer< vtkTransformPolyDataFilter >::New();
    CubeToTracker->SetTransform( tCubeToTracker );
    CubeToTracker->SetInput( source->GetOutput() );
    CubeToTracker->Update();
    
    appender->AddInputConnection( CubeToTracker->GetOutputPort() );
    }
  
  
    // Write output.
  
  std::cout << "Writing output..." << std::endl;
  
  vtkSmartPointer< vtkPolyDataWriter > writer = vtkSmartPointer< vtkPolyDataWriter >::New();
  writer->SetFileName( OutputFileName );
  writer->SetInput( appender->GetOutput() );
  writer->Update();
  
  std::cout << "Writing output done." << std::endl;
  
  
  return 0;
}
