#include "PlusConfigure.h"
#include "vtkVolumeReconstructor.h"
#include "vtkSmartPointer.h"
#include "vtksys/CommandLineArguments.hxx" 
#include "vtksys/SystemTools.hxx"
#include "vtkImageData.h"

#include "vtkDataSetWriter.h"
#include "itkImage.h"
#include "vtkImageExtractComponents.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkMetaImageSequenceIO.h"
#include "vtkTrackerTool.h"
#include "vtkXMLImageDataWriter.h"
#include "vtkTrackedFrameList.h"

int main (int argc, char* argv[])
{ 
	VTK_LOG_TO_CONSOLE_ON;  

	bool printHelp(false); 
	std::string inputImgSeqFileName;
	std::string inputConfigFileName;
	std::string outputVolumeFileName;

	int verboseLevel=PlusLogger::LOG_LEVEL_INFO;

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
	PlusLogger::Instance()->SetLogLevel(verboseLevel);

	vtkSmartPointer<vtkVolumeReconstructor> reconstructor = vtkSmartPointer<vtkVolumeReconstructor>::New(); 

	LOG_INFO("Reading configuration file...");
	reconstructor->ReadConfiguration(inputConfigFileName.c_str()); 

	//***************************  Image sequence reading *****************************
	LOG_INFO("Reading image sequence...");
	vtkSmartPointer<vtkTrackedFrameList> trackedFrameList = vtkSmartPointer<vtkTrackedFrameList>::New(); 
	trackedFrameList->ReadFromSequenceMetafile(inputImgSeqFileName.c_str()); 

	//***************************  Volume reconstruction ***************************** 
	
	// Set the input frame parameters 
	reconstructor->SetNumberOfFrames( trackedFrameList->GetNumberOfTrackedFrames() ); 
	reconstructor->SetFrameSize( trackedFrameList->GetFrameSize() ); 
    reconstructor->SetNumberOfBitsPerPixel( trackedFrameList->GetNumberOfBitsPerPixel() ); 
	
	LOG_INFO("Initialize reconstructor...");
	reconstructor->Initialize(); 
  
	LOG_INFO("Adding images to reconstructor...");
	
	std::ostringstream osTransformImageToTool; 
	reconstructor->GetImageToToolTransform()->GetMatrix()->Print( osTransformImageToTool );
	LOG_DEBUG("Image to tool (probe calibration) transform: \n" << osTransformImageToTool.str());  
	
	
	const int numberOfFrames = trackedFrameList->GetNumberOfTrackedFrames(); 
	for ( int imgNumber = 0; imgNumber < numberOfFrames; ++imgNumber )
	{
		PlusLogger::PrintProgressbar( (100.0 * imgNumber) / numberOfFrames ); 
  
		vtkSmartPointer<vtkMatrix4x4> mToolToReference = vtkSmartPointer<vtkMatrix4x4>::New();
		double tToolToReference[16]; 
		if ( trackedFrameList->GetTrackedFrame(imgNumber)->GetDefaultFrameTransform(tToolToReference) )
		{
			mToolToReference->DeepCopy(tToolToReference); 
		}
		else
		{
			LOG_ERROR("Unable to get default frame transform for frame #" << imgNumber); 
			continue; 
		}
		
		int* frameSize = trackedFrameList->GetTrackedFrame(imgNumber)->GetFrameSize(); 

		 // Add each tracked frame to reconstructor - US image orientation always MF in tracked frame list
		reconstructor->AddTrackedFrame(trackedFrameList->GetTrackedFrame(imgNumber)->ImageData->GetBufferPointer(), "MF", frameSize[0] , frameSize[1], mToolToReference );
	}
	
	PlusLogger::PrintProgressbar( 100 ); 
	
	trackedFrameList->Clear(); 
  
	LOG_INFO("Start reconstruction...");
	reconstructor->StartReconstruction(); 

	while ( reconstructor->GetReconstructor()->ReconstructionFrameCount > 0 ) 
	{
		PlusLogger::PrintProgressbar( ( 1 - ( 1.0 * reconstructor->GetReconstructor()->ReconstructionFrameCount /  numberOfFrames )) * 100 ); 
		vtksys::SystemTools::Delay(200); 
	}

	PlusLogger::PrintProgressbar( 100 ); 

	LOG_INFO("Fill holes in output volume...");
	reconstructor->FillHoles(); 

	LOG_INFO("Saving volume to file...");
	vtkSmartPointer<vtkImageExtractComponents> extract = vtkSmartPointer<vtkImageExtractComponents>::New();
	vtkSmartPointer<vtkDataSetWriter> writer3D = vtkSmartPointer<vtkDataSetWriter>::New();
	// keep only 0th component
	extract->SetComponents(0);
	extract->SetInput(reconstructor->GetReconstructor()->GetOutputFromPort(0));

	// write out to file
	writer3D->SetFileTypeToBinary();
	writer3D->SetInput(extract->GetOutput());
	writer3D->SetFileName(outputVolumeFileName.c_str());
	writer3D->Update();
  
	VTK_LOG_TO_CONSOLE_OFF; 
	return EXIT_SUCCESS; 
}
