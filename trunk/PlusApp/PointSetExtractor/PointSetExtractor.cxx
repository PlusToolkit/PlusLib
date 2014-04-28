/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

/*!
  \file PointSetExtractor.cxx
  \brief This program extract point set (e.g., tooltip of a stylus) from a sequence metafile.
  
  The program can be used to generate a point set from a sequence metafile.
  The sequence metafile can be acquired e.g., by fCal and the points can correspond to a tool
  trajectory (stylus tip, needle tip, probe, ...). The generated point set can be loaded into
  ParaView, 3D Slicer, MeshLab, etc. for visualization, surface reconstruction, or other
  processing.
*/ 

#include "PlusConfigure.h"

#include <vtksys/CommandLineArguments.hxx>

#include "vtkActor.h"
#include "vtkAppendPolyData.h"
#include "vtkCamera.h"
#include "vtkCellArray.h"
#include "vtkGlyph3D.h"
#include "vtkLineSource.h"
#include "vtkMatrix4x4.h"
#include "vtkPLYWriter.h" 
#include "vtkPoints.h"
#include "vtkPolyData.h"
#include "vtkPolyDataMapper.h"
#include "vtkPolyDataNormals.h"
#include "vtkProperty.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkSphereSource.h"
#include "vtkSTLWriter.h"
#include "vtkTriangleFilter.h"
#include "vtkTubeFilter.h"
#include "vtkXMLUtilities.h"

#include "vtkTrackedFrameList.h"
#include "TrackedFrame.h"
#include "vtkTransformRepository.h"

int main(int argc, char **argv)
{

  // Parse command-line arguments
  ///////////////

	bool printHelp=false;
	std::string inputConfigFileName;
  std::string inputSequenceFileName; 
	std::string outputPointsFileName;
  std::string outputSurfaceFileName;
  bool display=false;
  bool addTube=false;
  bool addSpheres=false;
  double radius=1;
	
  std::string stylusName("Stylus");
  std::string referenceName("Reference");

	int verboseLevel=vtkPlusLogger::LOG_LEVEL_UNDEFINED;

	vtksys::CommandLineArguments args;
	args.Initialize(argc, argv);

	args.AddArgument("--help", vtksys::CommandLineArguments::NO_ARGUMENT, &printHelp, "Print this help.");	
	args.AddArgument("--config-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputConfigFileName, "Name of the input configuration file");
	args.AddArgument("--source-seq-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputSequenceFileName, "Name of the input sequence metafile that contains the tracking data");	
	args.AddArgument("--stylus-name", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &stylusName, "Name of the stylus tool (Default: Stylus)");
  args.AddArgument("--reference-name", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &referenceName, "Name of the reference tool (Default: Reference)");
  args.AddArgument("--output-pointset-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &outputPointsFileName, "Filename of the output pointset file in PLY format (optional)");
  args.AddArgument("--display", vtksys::CommandLineArguments::NO_ARGUMENT, &display, "Show the points on the screen (optional)");	
	args.AddArgument("--verbose", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &verboseLevel, "Verbose level (1=error only, 2=warning, 3=info, 4=debug, 5=trace)");	
  args.AddArgument("--output-surface-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &outputSurfaceFileName, "Filename of the output sruface file in STL format (required if spheres or tube added)");
  args.AddArgument("--add-spheres", vtksys::CommandLineArguments::NO_ARGUMENT, &addSpheres, "Add a sphere at each point position (optional)");	
  args.AddArgument("--add-tube", vtksys::CommandLineArguments::NO_ARGUMENT, &addTube, "Add a tube connecting the point positions (optional)");	
  args.AddArgument("--radius", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &radius, "Radius of the tube or speheres (default: 5)");  

	if ( !args.Parse() )
	{
		std::cerr << "Problem parsing arguments" << std::endl;
		std::cout << "Help: " << args.GetHelp() << std::endl;
		exit(EXIT_FAILURE);
	}
	vtkPlusLogger::Instance()->SetLogLevel(verboseLevel);
	if ( printHelp ) 
	{
		std::cout << "Help: " << args.GetHelp() << std::endl;
		exit(EXIT_SUCCESS); 

	}
	if (inputConfigFileName.empty())
	{
		std::cerr << "--config-file is required" << std::endl;
		exit(EXIT_FAILURE);
	}
  if (inputSequenceFileName.empty())
	{
		std::cerr << "input-seq-file-name is required" << std::endl;
		exit(EXIT_FAILURE);
	}

  // Read the file and do the conversion
	/////////////// 

  LOG_INFO("Read input file...");
  vtkSmartPointer<vtkTrackedFrameList> trackedFrameList = vtkSmartPointer<vtkTrackedFrameList>::New();
  if ( trackedFrameList->ReadFromSequenceMetafile(inputSequenceFileName.c_str()) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to read tracked pose sequence metafile: " << inputSequenceFileName);
    return EXIT_FAILURE;
  }

  vtkSmartPointer<vtkTransformRepository> transformRepository = vtkSmartPointer<vtkTransformRepository>::New();

  // Read config file
  if (!inputConfigFileName.empty())
  {
    LOG_DEBUG("Reading config file...")
    vtkSmartPointer<vtkXMLDataElement> configRead = vtkSmartPointer<vtkXMLDataElement>::Take(::vtkXMLUtilities::ReadElementFromFile(inputConfigFileName.c_str())); 
    transformRepository->ReadConfiguration(configRead);
    LOG_DEBUG("Reading config file finished.");
  }

  PlusTransformName stylusToReferenceTransformName(stylusName, referenceName);
  if (!stylusToReferenceTransformName.IsValid())
  {
    LOG_ERROR("The tool names ("<<stylusName<<", "<<referenceName<<") are invalid");
    return EXIT_FAILURE;
  }
  
  //  Get stylustip positions in the reference coordinate frame
  LOG_INFO("Extract points...");
  vtkSmartPointer<vtkPoints> surfacePoints = vtkSmartPointer<vtkPoints>::New();
  double stylusTipPositionInReferenceFrame[4]={0,0,0,1};  
  for ( int frame = 0; frame < trackedFrameList->GetNumberOfTrackedFrames(); ++frame )
  {
    TrackedFrame *trackedFrame = trackedFrameList->GetTrackedFrame(frame);
    transformRepository->SetTransforms(*trackedFrame);
    vtkSmartPointer<vtkMatrix4x4> stylusToReferenceTransform = vtkSmartPointer<vtkMatrix4x4>::New();
    bool valid = false;
    transformRepository->GetTransform(stylusToReferenceTransformName, stylusToReferenceTransform, &valid);
    if (!valid)
    {
      // There is no available transform for this frame; skip that frame
      continue;
    }  
    stylusTipPositionInReferenceFrame[0]=stylusToReferenceTransform->Element[0][3];
    stylusTipPositionInReferenceFrame[1]=stylusToReferenceTransform->Element[1][3];
    stylusTipPositionInReferenceFrame[2]=stylusToReferenceTransform->Element[2][3];
      
    LOG_DEBUG("Stylus tip position: "
      <<stylusTipPositionInReferenceFrame[0]<<",   "
      <<stylusTipPositionInReferenceFrame[1]<<",   "
      <<stylusTipPositionInReferenceFrame[2]);
    surfacePoints->InsertNextPoint(stylusTipPositionInReferenceFrame);
  }
  int numberOfPoints = surfacePoints->GetNumberOfPoints();
  LOG_INFO("Number of points: "<<numberOfPoints);

  // Create a polydata, with a vertex at each point
	vtkSmartPointer<vtkCellArray> polyDataCells = vtkSmartPointer<vtkCellArray>::New();	
	for (vtkIdType ptIndex=0; ptIndex<numberOfPoints; ptIndex++)
  {
    polyDataCells->InsertNextCell(vtkIdType(1),&ptIndex);
  }
  vtkSmartPointer<vtkPolyData> pointsPolyData = vtkSmartPointer<vtkPolyData>::New();
	pointsPolyData->SetPoints(surfacePoints);
  pointsPolyData->SetVerts(polyDataCells);

  vtkSmartPointer<vtkAppendPolyData> polyDataAppend = vtkSmartPointer<vtkAppendPolyData>::New();

#if (VTK_VERSION_MAJOR < 6)
  polyDataAppend->AddInput(pointsPolyData);
#else
  polyDataAppend->AddInputData(pointsPolyData);
#endif
  
  if (addSpheres)
  {
    vtkSmartPointer<vtkSphereSource> sphere=vtkSmartPointer<vtkSphereSource>::New();
    sphere->SetThetaResolution(16);
    sphere->SetPhiResolution(16);
    sphere->SetRadius(radius);
    sphere->Update();
    vtkSmartPointer<vtkGlyph3D> glyph=vtkSmartPointer<vtkGlyph3D>::New();
    glyph->SetInputData_vtk5compatible(pointsPolyData);
#if (VTK_VERSION_MAJOR < 6)
    glyph->SetSource(sphere->GetOutput());
#else
    glyph->SetSourceData(sphere->GetOutput());
#endif    
    polyDataAppend->AddInputConnection(glyph->GetOutputPort());
  }

  if (addTube)
  {
    vtkSmartPointer<vtkLineSource> line=vtkSmartPointer<vtkLineSource>::New();
    line->SetPoints(surfacePoints);
    vtkSmartPointer<vtkTubeFilter> tube=vtkSmartPointer<vtkTubeFilter>::New();
    tube->SetInputConnection(line->GetOutputPort());
    tube->SetNumberOfSides(32);
    tube->SetRadius(radius);
    tube->CappingOn();
    vtkSmartPointer<vtkTriangleFilter> triangulator=vtkSmartPointer<vtkTriangleFilter>::New();
    triangulator->SetInputConnection(tube->GetOutputPort())    ;
    polyDataAppend->AddInputConnection(triangulator->GetOutputPort());
  }
  
  polyDataAppend->Update();

  // Write to output file
  if (!outputPointsFileName.empty())
  {
    LOG_INFO("Write points to "<<outputPointsFileName);   
    vtkSmartPointer<vtkPLYWriter> polyWriter=vtkSmartPointer<vtkPLYWriter>::New();
    polyWriter->SetInputData_vtk5compatible(pointsPolyData);
    polyWriter->SetFileName(outputPointsFileName.c_str());
    polyWriter->Update();
  }
  if (!outputSurfaceFileName.empty())
  {
    LOG_INFO("Write surface to "<<outputSurfaceFileName);   
    vtkSmartPointer<vtkSTLWriter> polyWriter=vtkSmartPointer<vtkSTLWriter>::New();
    polyWriter->SetInputData_vtk5compatible(polyDataAppend->GetOutput());
    polyWriter->SetFileName(outputSurfaceFileName.c_str());
    polyWriter->Update();
  }

  // Display points in 3D renderer
  if (display)
  {
    LOG_INFO("Visualize...");

    vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
    vtkSmartPointer<vtkRenderWindow> renWin = vtkSmartPointer<vtkRenderWindow>::New();
    renWin->AddRenderer(renderer);

    vtkSmartPointer<vtkPolyDataNormals> polyDataWithNormals=vtkSmartPointer<vtkPolyDataNormals>::New();
    polyDataWithNormals->SetInputConnection(polyDataAppend->GetOutputPort());

    vtkSmartPointer<vtkPolyDataMapper> pointsMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    pointsMapper->SetInputConnection(polyDataWithNormals->GetOutputPort());
    vtkSmartPointer<vtkActor> pointsActor = vtkSmartPointer<vtkActor>::New();
    pointsActor->SetMapper(pointsMapper);
    renderer->AddActor(pointsActor);

    vtkSmartPointer<vtkRenderWindowInteractor> iren = vtkSmartPointer<vtkRenderWindowInteractor>::New();
    iren->SetRenderWindow(renWin);
    renWin->Render();
    iren->Start();
  }

	return EXIT_SUCCESS; 
}
