#include "PlusConfigure.h"
#include "vtkVolumeReconstructor.h"
#include "vtkSmartPointer.h"
#include "vtksys/CommandLineArguments.hxx" 
#include "vtksys/SystemTools.hxx"
#include "vtkImageData.h"

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

void PrintProgressBar( int percent ); 

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

	vtkSmartPointer<vtkVolumeReconstructor> reconstructor = vtkSmartPointer<vtkVolumeReconstructor>::New(); 

	LOG_INFO("Reading configuration file...");
	reconstructor->ReadConfiguration(inputConfigFileName.c_str()); 

	//***************************  Image sequence reading *****************************
	LOG_INFO("Reading image sequence...");
	itk::MetaImageSequenceIO::Pointer readerMetaImageSequenceIO = itk::MetaImageSequenceIO::New(); 
	ImageSequenceReaderType::Pointer reader = ImageSequenceReaderType::New(); 

	// Set the image IO 
	reader->SetImageIO(readerMetaImageSequenceIO); 
	reader->SetFileName(inputImgSeqFileName.c_str());

	try
	{
		reader->Update(); 
	}
	catch (itk::ExceptionObject & err) 
	{		
		LOG_ERROR( "Sequence image reader couldn't update: " <<  err); 
		exit(EXIT_FAILURE);
	}	

	ImageSequenceType::Pointer imageSeq = reader->GetOutput();

	const unsigned long imageWidthInPixels = imageSeq->GetLargestPossibleRegion().GetSize()[0]; 
	const unsigned long imageHeightInPixels = imageSeq->GetLargestPossibleRegion().GetSize()[1]; 
	unsigned long numberOfFrames = imageSeq->GetLargestPossibleRegion().GetSize()[2];	
	unsigned int frameSizeInBytes = imageWidthInPixels * imageHeightInPixels * sizeof(PixelType);
	
	//***************************  Volume reconstruction ***************************** 
	
	// Set the input frame parameters 
	reconstructor->SetNumberOfFrames( numberOfFrames ); 
	reconstructor->SetFrameSize( imageWidthInPixels, imageHeightInPixels, 1); 
	
	LOG_INFO("Initialize reconstructor...");
	reconstructor->Initialize(); 

	LOG_INFO("Adding images to reconstructor...");
	
	PixelType* imageSeqData = imageSeq->GetBufferPointer(); 
	for ( int imgNumber = 0; imgNumber < numberOfFrames; imgNumber++ )
	{
		PrintProgressBar( (100.0 * imgNumber) / numberOfFrames ); 
		unsigned char* currentFrameImageData = imageSeqData + imgNumber * frameSizeInBytes;

		vtkSmartPointer<vtkMatrix4x4> transformMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
		if ( !readerMetaImageSequenceIO->GetFrameTransform(imgNumber, transformMatrix) )
		{
			LOG_ERROR("Unable to get default frame transform for frame #" << imgNumber); 
			continue; 
		}
		
		// Add each tracked frame to reconstructor 
		reconstructor->AddTrackedFrame(currentFrameImageData, imageWidthInPixels, imageHeightInPixels, transformMatrix ); 
	}
	
	PrintProgressBar( 100 ); 
	std::cout << std::endl; 

	LOG_INFO("Start reconstruction...");
	reconstructor->StartReconstruction(); 

	while ( reconstructor->GetReconstructor()->ReconstructionFrameCount > 0 ) 
	{
		PrintProgressBar( ( 1 - ( 1.0 * reconstructor->GetReconstructor()->ReconstructionFrameCount /  numberOfFrames )) * 100 ); 
		vtksys::SystemTools::Delay(200); 
	}

	PrintProgressBar( 100 ); 
	std::cout << std::endl; 

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


void PrintProgressBar( int percent )
{
	std::string bar;

	for(int i = 0; i < 50; i++)
	{
		if( i < (percent/2))
		{
			bar.replace(i,1,"=");
		}
		else if( i == (percent/2))
		{
			bar.replace(i,1,">");
		}
		else
		{
			bar.replace(i,1," ");
		}
	}

	std::cout<< "\r" "[" << bar << "] ";
	std::cout.width( 3 );
	std::cout<< percent << "%     " << std::flush;
}



