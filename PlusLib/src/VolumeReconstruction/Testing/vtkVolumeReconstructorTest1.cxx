#include "PlusConfigure.h"
#include "vtkVolumeReconstructor.h"
#include "vtkSmartPointer.h"
#include "vtksys/CommandLineArguments.hxx" 
#include "vtksys/SystemTools.hxx"
#include "vtkImageData.h"
#include "vtkTrackedFrameList.h"

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

int main (int argc, char* argv[])
{ 
	std::string inputImgSeqFileName;
	std::string inputConfigFileName;
	std::string outputVolumeFileName;

	int verboseLevel=PlusLogger::LOG_LEVEL_INFO;
	VTK_LOG_TO_CONSOLE_ON; 

	vtksys::CommandLineArguments cmdargs;
	cmdargs.Initialize(argc, argv);

	cmdargs.AddArgument("--input-img-seq-file-name", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputImgSeqFileName, "");
	cmdargs.AddArgument("--input-config-file-name", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputConfigFileName, "");
	cmdargs.AddArgument("--output-volume-file-name", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &outputVolumeFileName, "");
	cmdargs.AddArgument("--verbose", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &verboseLevel, "Verbose level (1=error only, 2=warning, 3=info, 4=debug)");	

	if ( !cmdargs.Parse() )
	{
		std::cerr << "Problem parsing arguments" << std::endl;
		std::cout << "Help: " << cmdargs.GetHelp() << std::endl;
		exit(EXIT_FAILURE);
	}

	// Set the log level
	PlusLogger::Instance()->SetLogLevel(verboseLevel);
	PlusLogger::Instance()->SetDisplayLogLevel(verboseLevel);


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

