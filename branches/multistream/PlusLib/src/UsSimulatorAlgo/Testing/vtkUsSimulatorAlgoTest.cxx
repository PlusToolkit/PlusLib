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
#include "vtkTransform.h"
#include "vtkUsSimulatorAlgo.h"
#include "vtkImageData.h" 
#include "vtkMetaImageWriter.h"
#include "vtkPointData.h"
#include "vtkAppendPolyData.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkCubeSource.h"
#include "vtkMetaImageWriter.h"
#include "vtkXMLImageDataWriter.h"
#include "vtkSTLWriter.h"
#include "vtkTimerLog.h"

//display
#include "vtkImageActor.h"
#include "vtkActor.h"
#include "vtkRenderWindow.h"
#include "vtkRenderer.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkPolyDataMapper.h"
#include "vtkProperty.h"
#include "vtkInteractorStyleImage.h"

//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
  
  static double startTime = vtkTimerLog::GetUniversalTime(); 
  
  std::string inputModelFile;
  std::string inputTransformsFile;
  std::string inputConfigFile;
  std::string outputUsImageFile;
  std::string intersectionFile;
  std::string showModel; 

  int verboseLevel=vtkPlusLogger::LOG_LEVEL_UNDEFINED;

  int numberOfFailures(0); 

  vtksys::CommandLineArguments args;
  args.Initialize(argc, argv);

  args.AddArgument("--model-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputModelFile, "File name of the input model, for which ultrasound images will be generated.");
  args.AddArgument("--config-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputConfigFile, "Config file containing the image to probe and phantom to reference transformations  ");
  args.AddArgument("--transforms-seq-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputTransformsFile, "File containing coordinate frames and the associated model to image transformations"); 
  args.AddArgument("--output-us-img-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &outputUsImageFile, "File name of the generated output ultrasound image.");
  args.AddArgument("--verbose", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &verboseLevel, "Verbose level (1=error only, 2=warning, 3=info, 4=debug, 5=trace)");
  args.AddArgument("--output-model-frame-intersection-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &intersectionFile, "Name of stl file containing the visualization of the intersection between the model and the frames");
  args.AddArgument("--show-model-frames-intersection-display",vtksys::CommandLineArguments::EQUAL_ARGUMENT, &showModel,"If empty, does not display a visualization of the model and the frame"); 


  // Input arguments error checking
  if ( !args.Parse() )
  {
    std::cerr << "Problem parsing arguments" << std::endl;
    std::cout << "Help: " << args.GetHelp() << std::endl;
    exit(EXIT_FAILURE);
  }

  vtkPlusLogger::Instance()->SetLogLevel(verboseLevel);

  if (inputConfigFile.empty())
  {
    std::cerr << "--config-file required " << std::endl;
    exit(EXIT_FAILURE);
  }
  if (inputTransformsFile.empty())
  {
    std::cerr << "--transforms-seq-file required" << std::endl;
    exit(EXIT_FAILURE);
  }
  if (outputUsImageFile.empty())
  {
    std::cerr << "--output-us-img-file required" << std::endl;
    exit(EXIT_FAILURE);
  }

  // Read transformations data 
  LOG_DEBUG("Reading input meta file..."); 
  vtkSmartPointer< vtkTrackedFrameList > trackedFrameList = vtkSmartPointer< vtkTrackedFrameList >::New(); 				
  trackedFrameList->ReadFromSequenceMetafile( inputTransformsFile.c_str() );
  LOG_DEBUG("Reading input meta file completed"); 

  // Create repository for ultrasound images correlated to the iput tracked frames
  vtkSmartPointer<vtkTrackedFrameList> simulatedUltrasoundFrameList = vtkSmartPointer<vtkTrackedFrameList>::New(); 

  // Read config file
  LOG_DEBUG("Reading config file...")
  vtkSmartPointer<vtkXMLDataElement> configRead = vtkSmartPointer<vtkXMLDataElement>::Take(::vtkXMLUtilities::ReadElementFromFile(inputConfigFile.c_str())); 
  LOG_DEBUG("Reading config file finished.");

  vtkSmartPointer<vtkTransformRepository> transformRepository = vtkSmartPointer<vtkTransformRepository>::New(); 
  if ( transformRepository->ReadConfiguration(configRead) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to read transforms for transform repository!"); 
    exit(EXIT_FAILURE); 
  }

  // Create simulator
  vtkSmartPointer<vtkUsSimulatorAlgo> usSimulator = vtkSmartPointer<vtkUsSimulatorAlgo>::New(); 
  
  if ( usSimulator->ReadConfiguration(configRead) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to read US simulator configuration!"); 
    exit(EXIT_FAILURE); 
  }
 
  if(!inputModelFile.empty())
  {
    usSimulator->LoadModel(inputModelFile.c_str()); 
  }

  usSimulator->CreateStencilBackgroundImage();

  PlusTransformName imageToReferenceTransformName(usSimulator->GetImageCoordinateFrame(), usSimulator->GetReferenceCoordinateFrame());

  // Setup Renderer to visualize surface model and ultrasound planes
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
    mapper->SetInput((vtkPolyData*)usSimulator->GetInput());  
    vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
    actor->SetMapper(mapper);
    rendererPoly->AddActor(actor);
  }

  // Visualization of the image planes
  {
    vtkSmartPointer< vtkPolyData > slicesPolyData = vtkSmartPointer< vtkPolyData >::New();
    CreateSliceModels(trackedFrameList, transformRepository, imageToReferenceTransformName, slicesPolyData);

    if(!intersectionFile.empty()) 
    {
      vtkSmartPointer<vtkSTLWriter> surfaceModelWriter = vtkSmartPointer<vtkSTLWriter>::New(); 
      surfaceModelWriter->SetFileName(intersectionFile.c_str()); 
      surfaceModelWriter->SetInput(slicesPolyData);
      surfaceModelWriter->Write(); 
    }

    vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInput(slicesPolyData);  
    vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
    actor->SetMapper(mapper);
    rendererPoly->AddActor(actor);
  }

  if(!showModel.empty()) 
  { 
    renderWindowPoly->Render();
    renderWindowInteractorPoly->Start();
  }

  //for (int i = 0; i<30; i++)
  for (int i = 0; i<trackedFrameList->GetNumberOfTrackedFrames(); i++)      
  {
    LOG_DEBUG("Processing frame "<<i);
    TrackedFrame* frame = trackedFrameList->GetTrackedFrame(i);

    // Update transform repository 
    if ( transformRepository->SetTransforms(*frame) != PLUS_SUCCESS )
    {
      LOG_ERROR("Failed to set repository transforms from tracked frame!"); 
      return EXIT_FAILURE;
    }

    // We use the model coordinate system as reference coordinate system
    // TODO: Alter to get new position? Or does it do it automatically?
    vtkSmartPointer<vtkMatrix4x4> imageToReferenceMatrix = vtkSmartPointer<vtkMatrix4x4>::New();   
    if ( transformRepository->GetTransform(imageToReferenceTransformName, imageToReferenceMatrix) != PLUS_SUCCESS )
    {
      std::string strTransformName; 
      imageToReferenceTransformName.GetTransformName(strTransformName); 
      LOG_ERROR("Failed to get transform from repository: " << strTransformName ); 
      return EXIT_FAILURE;
    }

    // Prepare filter and filter input
    vtkSmartPointer<vtkMatrix4x4> modelToImageMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
    modelToImageMatrix->DeepCopy(imageToReferenceMatrix);
    modelToImageMatrix->Invert();
    usSimulator->SetModelToImageMatrix(modelToImageMatrix);
    usSimulator->Update();

    vtkImageData* simOutput = usSimulator->GetOutput();

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

    PlusVideoFrame * simulatorOutputPlusVideoFrame = new PlusVideoFrame(); 
    simulatorOutputPlusVideoFrame->DeepCopyFrom(simOutput); 

    frame->SetImageData(*simulatorOutputPlusVideoFrame); 

    vtkSmartPointer<vtkImageData> usImage = vtkSmartPointer<vtkImageData>::New(); 
    usImage->DeepCopy(simOutput);

    // Display output of filter
    vtkSmartPointer<vtkImageActor> redImageActor = vtkSmartPointer<vtkImageActor>::New();
    redImageActor->SetInput(simOutput);
    
    // Visualize
    vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();

    // Red image is displayed
    renderer->AddActor(redImageActor);
    renderer->ResetCamera();

    vtkSmartPointer<vtkRenderWindow> renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
    renderWindow->AddRenderer(renderer);
    renderer->SetBackground(0, 72, 0);

    vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
    vtkSmartPointer<vtkInteractorStyleImage> style = vtkSmartPointer<vtkInteractorStyleImage>::New();

    renderWindowInteractor->SetInteractorStyle(style);
    renderWindowInteractor->SetRenderWindow(renderWindow);
  }

  vtkSmartPointer<vtkMetaImageSequenceIO> simulatedUsSequenceFileWriter = vtkSmartPointer<vtkMetaImageSequenceIO>::New(); 
  simulatedUsSequenceFileWriter->SetFileName(outputUsImageFile.c_str()); 
  simulatedUsSequenceFileWriter->SetTrackedFrameList(trackedFrameList); 
  simulatedUsSequenceFileWriter->Write(); 

   static double endTime = vtkTimerLog::GetUniversalTime(); 

   static double timeElapsed = endTime-startTime; 

   LOG_INFO(" Time: " << timeElapsed ) ; 

	return EXIT_SUCCESS; 
}
