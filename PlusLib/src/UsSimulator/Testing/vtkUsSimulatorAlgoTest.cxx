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

#
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


 //Read model

 LOG_DEBUG("Reading in model stl file...");
 vtkSmartPointer<vtkSTLReader> modelReader = vtkSmartPointer<vtkSTLReader>::New();
 modelReader->SetFileName(inputModelFileName.c_str());
 modelReader->Update();
 LOG_DEBUG("Finished reading model stl file."); 

 
 // Acquire data in appropriate containers to prepare for filter
 //vtkSmartPointer<vtkPolyData> model = vtkSmartPointer<vtkPolyData>::New(); 
 vtkPolyData *model = vtkPolyData::New(); 
 
 model->DeepCopy(modelReader->GetOutput()); 
   //= modelReader->GetOutput(); 


 TrackedFrame* frame = trackedFrameList->GetTrackedFrame(0); // 0 for test TODO: repeate for all frames. 
   // Update transform repository 
  if ( transformRepository->SetTransforms(*frame) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to set repository transforms from tracked frame!"); 
    //continue;
  }

  
   PlusTransformName transformName; 
  if ( transformName.SetTransformName("PhantomToImage")!= PLUS_SUCCESS )
  {
    LOG_ERROR("Invalid transform name: " << "PhantomToImage" ); 
    return EXIT_FAILURE; 
  }
  
  
  vtkSmartPointer<vtkMatrix4x4> modelToImageMatrix = vtkSmartPointer<vtkMatrix4x4>::New();  
 
  if ( transformRepository->GetTransform(transformName, modelToImageMatrix) != PLUS_SUCCESS )
    {
      std::string strTransformName; 
      transformName.GetTransformName(strTransformName); 
      LOG_ERROR("Failed to get transform from repository: " << strTransformName ); 
      //continue; 
    }

    vtkSmartPointer< vtkTransform > modelToImageTransform = vtkSmartPointer< vtkTransform >::New();
   modelToImageTransform->SetMatrix( modelToImageMatrix );   

   vtkSmartPointer<vtkImageData> stencilBackgroundImage = vtkSmartPointer<vtkImageData>::New(); 
   stencilBackgroundImage->SetSpacing(0.18,0.18,1.0);
   stencilBackgroundImage->SetOrigin(0,0,0); 
   stencilBackgroundImage->SetExtent(0,639,0,479,0,1); 
   stencilBackgroundImage->SetScalarTypeToUnsignedShort();
   stencilBackgroundImage->SetNumberOfScalarComponents(1);

   stencilBackgroundImage->AllocateScalars(); 

 
  
   

  //prepare output of filter. 

   vtkSmartPointer<vtkImageData> usImage = vtkSmartPointer<vtkImageData>::New(); 
   


   
  vtkSmartPointer< vtkUsSimulatorAlgo >  usSimulator ; 
  usSimulator = vtkSmartPointer<vtkUsSimulatorAlgo>::New(); 

  usSimulator->SetInput(model); 
  usSimulator->SetModelToImageTransform(modelToImageTransform); 
  usSimulator->SetStencilBackgroundImage(stencilBackgroundImage); 
  usSimulator->SetOutput(usImage); 
  usSimulator->Update();

  vtkSmartPointer<vtkMetaImageWriter> usImageWriter=vtkSmartPointer<vtkMetaImageWriter>::New();
  usImageWriter->SetFileName(outputUsImageFileName.c_str());
  usImageWriter->SetInputConnection(usSimulator->GetOutputPort());
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