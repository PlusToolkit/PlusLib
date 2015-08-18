/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

/*!
\file ModelRenderer.cxx 
\brief Renders a CAD model that can be included in the fixture catalog 

Rendering tries to emphasize edges and allow to choose an optimal size and viewing angle.

*/ 

#include "PlusConfigure.h"
#include "vtkCamera.h"
#include "vtkFeatureEdges.h"
#include "vtkInteractorStyleTrackballCamera.h" 
#include "vtkPNGWriter.h"
#include "vtkPolyDataMapper.h"
#include "vtkPolyDataNormals.h"
#include "vtkProperty.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkRenderer.h"
#include "vtkSmartPointer.h"
#include "vtkSTLReader.h"
#include "vtkToolAxesActor.h" 
#include "vtkTriangleFilter.h"
#include "vtkWindowToImageFilter.h"
#include "vtksys/CommandLineArguments.hxx"
#include "vtksys/SystemTools.hxx"

int main(int argc, char **argv)
{
  bool printHelp(false);
  std::string modelFileName;
  std::string outputImageFileName;
  std::vector<double> cameraRollPitchYawDeg;
  double cameraZoomFactor = 1.0;
  bool renderingOff(false);

  int verboseLevel=vtkPlusLogger::LOG_LEVEL_UNDEFINED;

  vtksys::CommandLineArguments args;
  args.Initialize(argc, argv);

  args.AddArgument("--help", vtksys::CommandLineArguments::NO_ARGUMENT, &printHelp, "Print this help.");  
  args.AddArgument("--model-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &modelFileName, "Name of the input STL model file");
  args.AddArgument("--output-image-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &outputImageFileName, "Name of the output PNG image file that will be created. Optional. If not specified then an interactive 3D window is displayed.");
  args.AddArgument("--camera-orientation", vtksys::CommandLineArguments::MULTI_ARGUMENT, &cameraRollPitchYawDeg, "Roll, pitch, yaw angles of the camera in degrees. Optional. Default: 0 20 20.");
  args.AddArgument("--camera-zoom", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &cameraZoomFactor, "Zoom factor of the camera. Optional. Default: 1.0.");
  args.AddArgument("--rendering-off", vtksys::CommandLineArguments::NO_ARGUMENT, &renderingOff, "Run test without rendering.");  
  args.AddArgument("--verbose", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &verboseLevel, "Verbose level (1=error only, 2=warning, 3=info, 4=debug, 5=trace)");  

  if ( !args.Parse() )
  {
    std::cerr << "Problem parsing arguments" << std::endl;
    std::cout << "Help: " << args.GetHelp() << std::endl;
    exit(EXIT_FAILURE);
  }
  
  if ( printHelp ) 
  {
    std::cout << args.GetHelp() << std::endl;
    exit(EXIT_SUCCESS); 
  }

  vtkPlusLogger::Instance()->SetLogLevel(verboseLevel);

  if (modelFileName.empty())
  {
    std::cerr << "--model-file is required" << std::endl;
    exit(EXIT_FAILURE);
  }

  if (cameraRollPitchYawDeg.size()==0)
  {
    cameraRollPitchYawDeg.push_back(0);  
    cameraRollPitchYawDeg.push_back(20);
    cameraRollPitchYawDeg.push_back(20);
  }
  else if (cameraRollPitchYawDeg.size()!=3)
  {
    LOG_ERROR("--camera-orientation requires 3 parameters");
    exit(EXIT_FAILURE);
  }

  vtkSmartPointer<vtkSTLReader> modelReader = vtkSmartPointer<vtkSTLReader>::New();
  modelReader->SetFileName(modelFileName.c_str());
  modelReader->Update();
  vtkPolyData* modelPolyData=modelReader->GetOutput();
  if (modelPolyData->GetNumberOfPoints()==0)
  {
    LOG_ERROR("Failed to load model from file: "<<modelFileName);
    return EXIT_FAILURE;
  }

  vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
  renderer->SetBackground(1,1,1);
  renderer->SetUseDepthPeeling(1);
  renderer->SetMaximumNumberOfPeels(100);
  renderer->SetOcclusionRatio(0.1);

  vtkSmartPointer<vtkRenderWindow> renWin = vtkSmartPointer<vtkRenderWindow>::New();
  if (!outputImageFileName.empty())
  {
    renWin->OffScreenRenderingOn();
  }
  renWin->SetSize(500,500);
  renWin->SetAlphaBitPlanes(1); // for depth peeling
  renWin->SetMultiSamples(0); // for depth peeling
  renWin->AddRenderer(renderer);

  //Create the interactor that handles the event loop
  vtkSmartPointer<vtkRenderWindowInteractor> iren = vtkSmartPointer<vtkRenderWindowInteractor>::New();
  iren->SetInteractorStyle(vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New()); 
  iren->SetRenderWindow(renWin);

  // Must be called after iren and renderer are linked or there will be problems
  renderer->Render();  

  // iren must be initialized so that it can handle events
  iren->Initialize();

  vtkSmartPointer<vtkPolyDataNormals> modelNormals = vtkSmartPointer<vtkPolyDataNormals>::New();
  
  modelNormals->SetInputConnection(modelReader->GetOutputPort());

  vtkSmartPointer<vtkPolyDataMapper> modelMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
  modelMapper->SetInputConnection(modelNormals->GetOutputPort());

  vtkSmartPointer<vtkActor> modelActor = vtkSmartPointer<vtkActor>::New();
  modelActor->SetMapper(modelMapper);
  modelActor->GetProperty()->SetColor(0.9, 0.9, 0.9);
  modelActor->GetProperty()->SetOpacity(0.8);
  renderer->AddActor(modelActor);

  vtkSmartPointer<vtkTriangleFilter> triangleFilter = vtkSmartPointer<vtkTriangleFilter>::New();
  triangleFilter->SetInputConnection(modelNormals->GetOutputPort());

  vtkSmartPointer<vtkFeatureEdges> edgeExtractor = vtkSmartPointer<vtkFeatureEdges>::New();
  edgeExtractor->SetInputConnection(triangleFilter->GetOutputPort());
  edgeExtractor->ColoringOff();
  edgeExtractor->BoundaryEdgesOn();
  edgeExtractor->ManifoldEdgesOn();
  edgeExtractor->NonManifoldEdgesOn();

  vtkSmartPointer<vtkPolyDataMapper> modelEdgesMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
  modelEdgesMapper->SetInputConnection(edgeExtractor->GetOutputPort());
  modelEdgesMapper->SetResolveCoincidentTopologyToPolygonOffset();
  vtkSmartPointer<vtkActor> modelEdgesActor = vtkSmartPointer<vtkActor>::New();
  modelEdgesActor->SetMapper(modelEdgesMapper);
  modelEdgesActor->GetProperty()->SetColor(0.0, 0.0, 0.0);
  renderer->AddActor(modelEdgesActor);

  // Add an origin display actor
  vtkSmartPointer<vtkToolAxesActor> originActor=vtkSmartPointer<vtkToolAxesActor>::New();
  originActor->SetName("origin");
  //renderer->AddActor(originActor);

  // Set projection to parallel to enable estimate distances
  renderer->GetActiveCamera()->ParallelProjectionOn();
  renderer->GetActiveCamera()->Roll(cameraRollPitchYawDeg[0]);
  renderer->GetActiveCamera()->Pitch(cameraRollPitchYawDeg[1]);
  renderer->GetActiveCamera()->Yaw(cameraRollPitchYawDeg[2]);
  renderer->ResetCamera();
  renderer->GetActiveCamera()->Zoom(cameraZoomFactor);

  if (outputImageFileName.empty())
  {
    iren->Start();
  }
  else
  {
    vtkSmartPointer<vtkWindowToImageFilter> windowToImageFilter = vtkSmartPointer<vtkWindowToImageFilter>::New();
    windowToImageFilter->SetInput(renWin);
    windowToImageFilter->Update();

    vtkSmartPointer<vtkPNGWriter> writer = vtkSmartPointer<vtkPNGWriter>::New();
    writer->SetFileName(outputImageFileName.c_str());
    writer->SetInputData_vtk5compatible(windowToImageFilter->GetOutput());
    writer->Write();

    LOG_INFO("Rendered image is written to "<<outputImageFileName);
  }

  return EXIT_SUCCESS; 
}