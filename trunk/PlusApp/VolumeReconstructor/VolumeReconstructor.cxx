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


// debug
#include "vtkAppendPolyData.h"
#include "vtkCubeSource.h"
#include "vtkSphereSource.h"
#include "vtkPolyData.h"
#include "vtkPolyDataWriter.h"
#include "vtkTransformPolyDataFilter.h"


typedef unsigned char PixelType;
typedef itk::Image< PixelType, 2 > ImageType;
typedef itk::Image< PixelType, 3 > ImageSequenceType;
typedef itk::ImageFileReader< ImageSequenceType > ImageSequenceReaderType;

void PrintProgressBar( int percent ); 

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
	
	
	
	// debug
	std::cout << std::endl << "image to tool (probe calibration)" << std::endl;
	reconstructor->GetImageToToolTransform()->GetMatrix()->Print( std::cout );
	std::cout << std::endl;
	vtkSmartPointer< vtkAppendPolyData > append = vtkSmartPointer< vtkAppendPolyData >::New();
	
	
	
	PixelType* imageSeqData = imageSeq->GetBufferPointer(); 
	for ( int imgNumber = 0; imgNumber < numberOfFrames; imgNumber++ )
	{
		PrintProgressBar( (100.0 * imgNumber) / numberOfFrames ); 
		unsigned char* currentFrameImageData = imageSeqData + imgNumber * frameSizeInBytes;
  
		vtkSmartPointer<vtkMatrix4x4> mToolToReference = vtkSmartPointer<vtkMatrix4x4>::New();
		if ( ! readerMetaImageSequenceIO->GetFrameTransform( imgNumber, mToolToReference ) )
		{
			LOG_ERROR("Unable to get default frame transform for frame #" << imgNumber); 
			continue; 
		}
		
		  // Add each tracked frame to reconstructor.
		
		reconstructor->AddTrackedFrame(currentFrameImageData, imageWidthInPixels, imageHeightInPixels, mToolToReference );
		
		
		//debug
		vtkSmartPointer< vtkCubeSource > source = vtkSmartPointer< vtkCubeSource >::New();
		  source->SetXLength( 640 );
		  source->SetYLength( 480 );
		vtkSmartPointer< vtkSphereSource > source1 = vtkSmartPointer< vtkSphereSource >::New();
		  source1->SetRadius( 20 );
		vtkSmartPointer< vtkTransform > tSourceToImage = vtkSmartPointer< vtkTransform >::New();
		  tSourceToImage->Translate( 320, 240, 0 );
		vtkSmartPointer< vtkMatrix4x4 > mImageToReference = vtkSmartPointer< vtkMatrix4x4 >::New();
		  mImageToReference->DeepCopy( reconstructor->GetImageToReferenceTransform( imgNumber )->GetMatrix() );
		vtkSmartPointer< vtkTransform > tImageToReference = vtkSmartPointer< vtkTransform >::New();
		  tImageToReference->PostMultiply();
		  tImageToReference->SetMatrix( mImageToReference );
		  tImageToReference->Update();
		vtkSmartPointer< vtkTransform > tSourceToReference = vtkSmartPointer< vtkTransform >::New();
		  tSourceToReference->PostMultiply();
		  tSourceToReference->Identity();
		  tSourceToReference->Concatenate( tSourceToImage );
		  tSourceToReference->Concatenate( tImageToReference );
		  tSourceToReference->Update();
		// std::cout << std::endl << "image to reference " << imgNumber << std::endl;
		// tImageToReference->GetMatrix()->Print( std::cout );
		// std::cout << std::endl;
		vtkSmartPointer< vtkTransformPolyDataFilter > transformFilter = vtkSmartPointer< vtkTransformPolyDataFilter >::New();
		  transformFilter->SetTransform( tSourceToReference );
		  transformFilter->SetInputConnection( source->GetOutputPort() );
		  transformFilter->Update();
		vtkSmartPointer< vtkTransformPolyDataFilter > transformFilter1 = vtkSmartPointer< vtkTransformPolyDataFilter >::New();
		  transformFilter1->SetTransform( tImageToReference );
		  transformFilter1->SetInputConnection( source1->GetOutputPort() );
		  transformFilter1->Update();
		append->AddInput( transformFilter->GetOutput() );
		append->AddInput( transformFilter1->GetOutput() );
		append->Update();
		
	}
	
	
	// debug
	vtkSmartPointer< vtkCubeSource > oSource = vtkSmartPointer< vtkCubeSource >::New();
	  oSource->SetXLength( 10 );
	append->AddInput( oSource->GetOutput() );
	append->Update();
	std::cout << std::endl;
	double* outextmin = reconstructor->GetVolumeExtentMin();
	double* outextmax = reconstructor->GetVolumeExtentMax();
	for ( int i = 0; i < 3; ++ i )
	  {
	  std::cout << outextmin[ i ] << " -- " << outextmax[ i ] << std::endl;
	  }
	vtkSmartPointer< vtkPolyDataWriter > pw = vtkSmartPointer< vtkPolyDataWriter >::New();
	  pw->SetFileName( "C:/us/aa.vtk" );
	  pw->SetInput( append->GetOutput() );
	  pw->Update();
	
	
	
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



