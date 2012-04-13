/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/ 

#include "PlusConfigure.h"
#include "vtksys/CommandLineArguments.hxx"
#include <iomanip>
#include <iostream>
#include "vtkXMLUtilities.h"


#include "vtkSmartPointer.h"
#include "vtkMatrix4x4.h"
#include "vtkMetaImageSequenceIO.h"
#include "vtkTrackedFrameList.h"
#include "vtkTransformRepository.h"
#include "TrackedFrame.h"
#include <vtkSTLReader.h>
#include "vtkPolyData.h"
#include "vtkTransform.h"
#include "vtkUsSimulatorAlgo.h"
#include "vtkImageData.h" 
#include "vtkMetaImageWriter.h"
#include "vtkPointData.h"
#include "vtkAppendPolyData.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkCubeSource.h"
#include "vtkJPEGWriter.h"
#include "vtkMetaImageWriter.h"
#include "vtkXMLImageDataWriter.h"
//display
#include "vtkImageActor.h"
#include "vtkActor.h"
#include "vtkRenderWindow.h"
#include "vtkRenderer.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkPolyDataMapper.h"
#include "vtkProperty.h"
#include "vtkInteractorStyleImage.h"

void CreateSliceModels(vtkTrackedFrameList *trackedFrameList, vtkTransformRepository *transformRepository, PlusTransformName &imageToReferenceTransformName, vtkPolyData *outputPolyData)
{
  // Prepare the output polydata.
  
  vtkSmartPointer< vtkAppendPolyData > appender = vtkSmartPointer< vtkAppendPolyData >::New();

  // Loop over each tracked image slice.
  for ( int frameIndex = 0; frameIndex < trackedFrameList->GetNumberOfTrackedFrames(); ++ frameIndex )
  {
    TrackedFrame* frame = trackedFrameList->GetTrackedFrame( frameIndex );

    // Update transform repository 
    if ( transformRepository->SetTransforms(*frame) != PLUS_SUCCESS )
    {
      LOG_ERROR("Failed to set repository transforms from tracked frame!"); 
      continue; 
    }

    vtkSmartPointer<vtkMatrix4x4> tUserDefinedMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
    if ( transformRepository->GetTransform(imageToReferenceTransformName, tUserDefinedMatrix) != PLUS_SUCCESS )
    {
      std::string strTransformName; 
      imageToReferenceTransformName.GetTransformName(strTransformName); 
      LOG_ERROR("Failed to get transform from repository: " << strTransformName ); 
      continue; 
    }

    vtkSmartPointer< vtkTransform > tUserDefinedTransform = vtkSmartPointer< vtkTransform >::New();
    tUserDefinedTransform->SetMatrix( tUserDefinedMatrix );    

    int* frameSize = frame->GetFrameSize();

    vtkSmartPointer< vtkTransform > tCubeToImage = vtkSmartPointer< vtkTransform >::New();
    tCubeToImage->Scale( frameSize[ 0 ], frameSize[ 1 ], 1 );
    tCubeToImage->Translate( 0.5, 0.5, 0.5 );  // Moving the corner to the origin.

    vtkSmartPointer< vtkTransform > tCubeToTracker = vtkSmartPointer< vtkTransform >::New();
    tCubeToTracker->Identity();
    tCubeToTracker->Concatenate( tUserDefinedTransform );
    tCubeToTracker->Concatenate( tCubeToImage );

    vtkSmartPointer< vtkTransformPolyDataFilter > CubeToTracker = vtkSmartPointer< vtkTransformPolyDataFilter >::New();
    CubeToTracker->SetTransform( tCubeToTracker );
    vtkSmartPointer< vtkCubeSource > source = vtkSmartPointer< vtkCubeSource >::New();
    CubeToTracker->SetInput( source->GetOutput() );
    CubeToTracker->Update();

    appender->AddInputConnection( CubeToTracker->GetOutputPort() );

  }  

  appender->Update();
  outputPolyData->DeepCopy(appender->GetOutput());

}



int main(int argc, char **argv)
{
std::string inputModelFileName;
std::string inputMetaFileName;
std::string inputConfigFileName;
std::string outputUsImageFileName;
 

	int verboseLevel=vtkPlusLogger::LOG_LEVEL_DEFAULT;

	int numberOfFailures(0); 
	
	vtksys::CommandLineArguments args;
	args.Initialize(argc, argv);

	args.AddArgument("--input-model-file-name", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputModelFileName, "File name of the input model, for which ultrasound images will be generated.");
	args.AddArgument("--input-configfile", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputConfigFileName, "Config file containing the image to probe and phantom to reference transformations  ");
  args.AddArgument("--input-metafile", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputMetaFileName, "File containing coordinate frames and the associated model to image transformations"); 
  args.AddArgument("--output-us-img-file-name", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &outputUsImageFileName, "File name of the generated output ultrasound image.");
	args.AddArgument("--verbose", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &verboseLevel, "Verbose level (1=error only, 2=warning, 3=info, 4=debug, 5=trace)");

  ///////////////////////////////////////
	if ( !args.Parse() )
	{
		std::cerr << "Problem parsing arguments" << std::endl;
		std::cout << "Help: " << args.GetHelp() << std::endl;
		exit(EXIT_FAILURE);
	}

	vtkPlusLogger::Instance()->SetLogLevel(verboseLevel);

	if (inputModelFileName.empty())
	{
		std::cerr << "--input-model-file-name required" << std::endl;
		exit(EXIT_FAILURE);
	}

	if (inputMetaFileName.empty())
	{
		std::cerr << "--input-metafile required" << std::endl;
		exit(EXIT_FAILURE);
	}
  if (inputConfigFileName.empty())
	{
		std::cerr << "--input-configfile required" << std::endl;
		exit(EXIT_FAILURE);
	}
  if (outputUsImageFileName.empty())
	{
		std::cerr << "--output-us-img-file-name required" << std::endl;
		exit(EXIT_FAILURE);
	}

	
 
 
  //Read coordinate reference (with transformations) data 
  

 LOG_DEBUG("Reading input meta file..."); 
 vtkSmartPointer< vtkTrackedFrameList > trackedFrameList = vtkSmartPointer< vtkTrackedFrameList >::New(); 				
 trackedFrameList->ReadFromSequenceMetafile( inputMetaFileName.c_str() );
 LOG_DEBUG("Reading input meta file completed"); 

 int testFrameIndex=30;
 trackedFrameList->RemoveTrackedFrameRange(0,testFrameIndex-1);
 trackedFrameList->RemoveTrackedFrameRange(1,trackedFrameList->GetNumberOfTrackedFrames()-1);

 // Need to log total frame number ?

// read config file
 LOG_DEBUG("Reading config file...")

 vtkSmartPointer<vtkXMLDataElement> configRead = vtkSmartPointer<vtkXMLDataElement>::Take(::vtkXMLUtilities::ReadElementFromFile(inputConfigFileName.c_str())); 
 
 vtkSmartPointer<vtkTransformRepository> transformRepository = vtkSmartPointer<vtkTransformRepository>::New(); 
  if ( transformRepository->ReadConfiguration(configRead) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to read transforms for transform repository!"); 
    return EXIT_FAILURE; 
  }
 LOG_DEBUG("Reading config file finished.");

 PlusTransformName imageToReferenceTransformName; 
 const char transformNameString[]="ImageToPhantom";
 if ( imageToReferenceTransformName.SetTransformName(transformNameString)!= PLUS_SUCCESS )
 {
   LOG_ERROR("Invalid transform name: " << transformNameString ); 
   return EXIT_FAILURE; 
 }


 //Read model

 LOG_DEBUG("Reading in model stl file...");
 vtkSmartPointer<vtkSTLReader> modelReader = vtkSmartPointer<vtkSTLReader>::New();
 modelReader->SetFileName(inputModelFileName.c_str());
 modelReader->Update();
 LOG_DEBUG("Finished reading model stl file."); 





 // Acquire data in appropriate containers to prepare for filter
 //vtkSmartPointer<vtkPolyData> model = vtkSmartPointer<vtkPolyData>::New(); 
 vtkSmartPointer<vtkPolyData> model = vtkSmartPointer<vtkPolyData>::New(); 
 
 model->DeepCopy(modelReader->GetOutput()); 
   //= modelReader->GetOutput(); 
 
 /*
   //display
   vtkSmartPointer<vtkImageActor> redImageActor = vtkSmartPointer<vtkImageActor>::New();

   redImageActor->SetInput(combineModelwithBackgroundStencil->GetOutput());

 
  // Visualize
  
  vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New(); 
  vtkSmartPointer<vtkRenderWindow> renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
  renderWindow->AddRenderer(renderer); 
  vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
  vtkSmartPointer<vtkInteractorStyleImage> style = vtkSmartPointer<vtkInteractorStyleImage>::New(); 
  renderWindowInteractor->SetInteractorStyle(style); 
  renderWindowInteractor->SetRenderWindow(renderWindow);
  renderWindowInteractor->Initialize();
  renderWindowInteractor->Start();
*/

  // Red image is displayed
/*  renderer->AddActor(redImageActor);
  renderer->SetBackground(0, 72, 0);*/
  
 
  // White image is displayed
  //renderer->AddActor(redImageActor);
  //renderer->AddActor(whiteImageActor);
  //renderer->ResetCamera();


  vtkSmartPointer<vtkRenderer> rendererPoly = vtkSmartPointer<vtkRenderer>::New();
  vtkSmartPointer<vtkRenderWindow> renderWindowPoly = vtkSmartPointer<vtkRenderWindow>::New();
  renderWindowPoly->AddRenderer(rendererPoly);
  vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractorPoly = vtkSmartPointer<vtkRenderWindowInteractor>::New();
  renderWindowInteractorPoly->SetRenderWindow(renderWindowPoly);
  {
    vtkSmartPointer<vtkInteractorStyleTrackballCamera> style = vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New(); 
    renderWindowInteractorPoly->SetInteractorStyle(style);
  }

  // Visualization of the surface model
  {
    vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInput(model);  
    vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
    actor->SetMapper(mapper);
    //actor->GetProperty()->SetPointSize(5);
    rendererPoly->AddActor(actor);
  }

  // Visualization of the image planes
  {
    vtkSmartPointer< vtkPolyData > slicesPolyData = vtkSmartPointer< vtkPolyData >::New();
    CreateSliceModels(trackedFrameList, transformRepository, imageToReferenceTransformName, slicesPolyData);
    vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInput(slicesPolyData);  
    vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
    actor->SetMapper(mapper);
    rendererPoly->AddActor(actor);
  }

  renderWindowPoly->Render();
  //renderWindowInteractorPoly->Start();

 //check polydata


 TrackedFrame* frame = trackedFrameList->GetTrackedFrame(0);
   // Update transform repository 
  if ( transformRepository->SetTransforms(*frame) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to set repository transforms from tracked frame!"); 
    //continue;
  }   
  
 
  // We use the model coordinate system as reference coordinate system
  vtkSmartPointer<vtkMatrix4x4> imageToReferenceMatrix = vtkSmartPointer<vtkMatrix4x4>::New();   
  if ( transformRepository->GetTransform(imageToReferenceTransformName, imageToReferenceMatrix) != PLUS_SUCCESS )
    {
      std::string strTransformName; 
      imageToReferenceTransformName.GetTransformName(strTransformName); 
      LOG_ERROR("Failed to get transform from repository: " << strTransformName ); 
      //continue; 
    }

  // Create test transform
  //vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();
  //transform->Translate(-370,-120,100);
  //imageToReferenceMatrix->DeepCopy( transform->GetMatrix() );

  vtkSmartPointer<vtkImageData> stencilBackgroundImage = vtkSmartPointer<vtkImageData>::New(); 
  stencilBackgroundImage->SetSpacing(1,1,1);
  stencilBackgroundImage->SetOrigin(0,0,0);
  
  int* frameSize = frame->GetFrameSize();
  stencilBackgroundImage->SetExtent(0,frameSize[0]-1,0,frameSize[1]-1,0,0);

  stencilBackgroundImage->SetScalarTypeToUnsignedChar();
  stencilBackgroundImage->SetNumberOfScalarComponents(1);
  stencilBackgroundImage->AllocateScalars(); 

  //int* dims = stencilBackgroundImage->GetDimensions();
 
  //for (int y = 0; y < dims[1]; y++)
  //  {
  //  for (int x = 0; x < dims[0]; x++)
  //    {
  //    unsigned char* pixel = static_cast<unsigned char*>(stencilBackgroundImage->GetScalarPointer(x,y,0));
  //    pixel[0] = 255;
  //    pixel[1] = 255;
  //    pixel[2] = 255;
  //    }
  //  }

  int* extent = stencilBackgroundImage->GetExtent();
  memset(stencilBackgroundImage->GetScalarPointer(), 0,
    ((extent[1]-extent[0]+1)*(extent[3]-extent[2]+1)*(extent[5]-extent[4]+1)*stencilBackgroundImage->GetScalarSize()*stencilBackgroundImage->GetNumberOfScalarComponents()));

  {
    vtkSmartPointer<vtkMetaImageWriter> writer=vtkSmartPointer<vtkMetaImageWriter>::New();
    writer->SetInput(stencilBackgroundImage);
    writer->SetFileName("c:\\Users\\lasso\\devel\\PlusExperimental-bin\\PlusLib\\data\\TestImages\\stencilBackgroundImage.mha ");
    writer->Write();
  }

  //prepare output of filter. 
  vtkSmartPointer< vtkUsSimulatorAlgo >  usSimulator ; 
  usSimulator = vtkSmartPointer<vtkUsSimulatorAlgo>::New(); 

  usSimulator->SetInput(model); 
  vtkSmartPointer<vtkMatrix4x4> modelToImageMatrix=vtkSmartPointer<vtkMatrix4x4>::New();
  modelToImageMatrix->DeepCopy(imageToReferenceMatrix);
  modelToImageMatrix->Invert();
  usSimulator->SetModelToImageMatrix(modelToImageMatrix); 
  usSimulator->SetStencilBackgroundImage(stencilBackgroundImage); 
  usSimulator->Update();

  vtkImageData* simOutput=usSimulator->GetOutputImage();

  double origin[3]={
    imageToReferenceMatrix->Element[0][3],
    imageToReferenceMatrix->Element[1][3],
    imageToReferenceMatrix->Element[2][3]};
  simOutput->SetOrigin(origin);
  double spacing[3]={
    sqrt(imageToReferenceMatrix->Element[0][0]*imageToReferenceMatrix->Element[0][0]+
      imageToReferenceMatrix->Element[1][0]*imageToReferenceMatrix->Element[1][0]+
      imageToReferenceMatrix->Element[2][0]*imageToReferenceMatrix->Element[2][0]),
    sqrt(imageToReferenceMatrix->Element[0][1]*imageToReferenceMatrix->Element[0][1]+
      imageToReferenceMatrix->Element[1][1]*imageToReferenceMatrix->Element[1][1]+
      imageToReferenceMatrix->Element[2][1]*imageToReferenceMatrix->Element[2][1]),
    1.0};
  simOutput->SetSpacing(spacing);

/*
  vtkSmartPointer<vtkJPEGWriter> writer=vtkSmartPointer<vtkJPEGWriter>::New();
  writer->SetInput(simOutput);
  writer->SetFileName("c:\\Users\\bartha\\devel\\PlusExperimental-bin\\bin\\Debug\\simoutput.jpg");
  //writer->Update();
  writer->Write();
*/

  /*
  {    
    vtkSmartPointer<vtkXMLImageDataWriter> writer=vtkSmartPointer<vtkXMLImageDataWriter>::New();
    writer->SetInput(simOutput);
    writer->SetFileName("c:\\Users\\lasso\\devel\\PlusExperimental-bin\\PlusLib\\data\\TestImages\\simoutput.vtk");
    //writer->Update();
    writer->Write();
  }
  */

  {
    vtkSmartPointer<vtkMetaImageWriter> writer=vtkSmartPointer<vtkMetaImageWriter>::New();
    writer->SetInput(simOutput);
    writer->SetFileName("c:\\Users\\lasso\\devel\\PlusExperimental-bin\\PlusLib\\data\\TestImages\\simoutput1.mha");
    writer->Write();
  }
/*
  {
    vtkSmartPointer<vtkMetaImageWriter> writer=vtkSmartPointer<vtkMetaImageWriter>::New();
    writer->SetInput(simOutput);
    writer->SetFileName("c:\\Users\\lasso\\devel\\PlusExperimental-bin\\PlusLib\\data\\TestImages\\simoutput2.mha");
    writer->Write();
  }
  */

      vtkSmartPointer<vtkImageData> usImage = vtkSmartPointer<vtkImageData>::New(); 
  usImage->DeepCopy(simOutput);

  

  //display
   vtkSmartPointer<vtkImageActor> redImageActor = vtkSmartPointer<vtkImageActor>::New();

   redImageActor->SetInput(simOutput);

 
  // Visualize
  vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
 
  // Red image is displayed
  renderer->AddActor(redImageActor);
  
 
  // White image is displayed
  //renderer->AddActor(redImageActor);
  //renderer->AddActor(whiteImageActor);
  renderer->ResetCamera();
 
  vtkSmartPointer<vtkRenderWindow> renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
  renderWindow->AddRenderer(renderer);
  renderer->SetBackground(0, 72, 0);

  vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
  vtkSmartPointer<vtkInteractorStyleImage> style = vtkSmartPointer<vtkInteractorStyleImage>::New();
 
  renderWindowInteractor->SetInteractorStyle(style);
 
  renderWindowInteractor->SetRenderWindow(renderWindow);
  renderWindowInteractor->Initialize();
  renderWindowInteractor->Start();







  vtkSmartPointer<vtkMetaImageWriter> usImageWriter=vtkSmartPointer<vtkMetaImageWriter>::New();
  usImageWriter->SetFileName(outputUsImageFileName.c_str());
 // usImageWriter->SetInputConnection(usSimulator->GetOutputPort()); 
  usImageWriter->SetInput(simOutput); 
  usImageWriter->Write();

/*
  newer version :
  vtkSmartPointer<vtkImageData> whiteImage = vtkSmartPointer<vtkImageData>::New(); ( maybe not pointer, just image data)
  whiteImage->SetSpacing(volumeSpacing); 
  whiteImage->SetOrigin(modelToImageTransform->GetPosition()); 
  
  whiteImage->SetDimensions(100,100,1); // make more robust, get rid of magic numbers. (base off of bounds) 

  whiteImage->SetScalarTypeToUnsignedChar();
  whiteImage->AllocateScalars();

    unsigned char inval = 255;
  unsigned char outval = 0;
  vtkIdType count = whiteImage->GetNumberOfPoints();
  for (vtkIdType i = 0; i < count; ++i)
  {
   whiteImage->GetPointData()->GetScalars()->SetTuple1(i, inval);
  }
  


*/ 


}