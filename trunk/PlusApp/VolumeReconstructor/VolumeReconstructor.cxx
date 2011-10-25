/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"

#include "vtksys/CommandLineArguments.hxx" 
#include "vtkXMLUtilities.h"
#include "vtkSmartPointer.h"
#include "vtkImageData.h"
#include "vtkDataSetWriter.h"

#include "vtkVolumeReconstructor.h"
#include "vtkTrackedFrameList.h"

int main (int argc, char* argv[])
{ 
	bool printHelp(false); 
	std::string inputImgSeqFileName;
	std::string inputConfigFileName;
	std::string outputVolumeFileName;

	int verboseLevel=vtkPlusLogger::LOG_LEVEL_INFO;

	vtksys::CommandLineArguments cmdargs;
	cmdargs.Initialize(argc, argv);

	cmdargs.AddArgument("--help", vtksys::CommandLineArguments::NO_ARGUMENT, &printHelp, "Print this help.");	
	cmdargs.AddArgument("--input-img-seq-file-name", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputImgSeqFileName, "Path to the input image sequence meta file.");
	cmdargs.AddArgument("--input-config-file-name", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputConfigFileName, "Path to the configuration file.");
	cmdargs.AddArgument("--output-volume-file-name", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &outputVolumeFileName, "The file name of the output volume (.vtk extension).");
	cmdargs.AddArgument("--verbose", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &verboseLevel, "Verbose level (1=error only, 2=warning, 3=info, 4=debug)");	

	if ( !cmdargs.Parse() )
	{
		std::cerr << "Problem parsing arguments" << std::endl;
		std::cout << "Volume reconstructor help: " << cmdargs.GetHelp() << std::endl;
		exit(EXIT_FAILURE);
	}

	if ( printHelp ) 
	{
		std::cout << "Volume reconstructor help: " << cmdargs.GetHelp() << std::endl;
		exit(EXIT_SUCCESS); 

	}

	if ( inputImgSeqFileName.empty() || inputConfigFileName.empty() || outputVolumeFileName.empty() ) 
	{
		LOG_ERROR( "input-img-seq-file-name, input-config-file-name and output-volume-file-name arguments are required\n");
		std::cout << "Volume reconstructor help: " << cmdargs.GetHelp() << std::endl;
		exit(EXIT_FAILURE);
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

  LOG_INFO("Reading image sequence...");
  vtkSmartPointer<vtkTrackedFrameList> trackedFrameList = vtkSmartPointer<vtkTrackedFrameList>::New(); 
  trackedFrameList->ReadFromSequenceMetafile(inputImgSeqFileName.c_str()); 
  
  LOG_INFO("Reconstruct volume...");
  reconstructor->SetOutputExtentFromFrameList(trackedFrameList);
  const int numberOfFrames = trackedFrameList->GetNumberOfTrackedFrames(); 
  std::string defaultFrameTransformName=trackedFrameList->GetDefaultFrameTransformName();
  for ( int frameIndex = 0; frameIndex < numberOfFrames; ++frameIndex )
  {
    LOG_DEBUG("Frame: "<<frameIndex);
    vtkPlusLogger::PrintProgressbar( (100.0 * frameIndex) / numberOfFrames ); 
    TrackedFrame* frame = trackedFrameList->GetTrackedFrame( frameIndex );
    reconstructor->AddTrackedFrame(frame, defaultFrameTransformName.c_str());
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

  return EXIT_SUCCESS; 
}
