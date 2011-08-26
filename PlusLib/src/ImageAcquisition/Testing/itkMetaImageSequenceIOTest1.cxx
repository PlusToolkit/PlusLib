#include "PlusConfigure.h"
#include "vtksys/CommandLineArguments.hxx"
#include <iomanip>

#include "vtkSmartPointer.h"
#include "vtkMatrix4x4.h"

#include "itkMetaImageSequenceIO.h"
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

///////////////////////////////////////////////////////////////////
// Image type definition

typedef unsigned char          PixelType; // define type for pixel representation
const unsigned int             imageDimension = 2; 
const unsigned int             imageSequenceDimension = 3; 

typedef itk::Image< PixelType, imageDimension > ImageType;
typedef itk::Image< PixelType, imageSequenceDimension > ImageSequenceType;

typedef itk::ImageFileReader< ImageSequenceType > ImageSequenceReaderType;
typedef itk::ImageFileWriter< ImageSequenceType > ImageSequenceWriterType;

///////////////////////////////////////////////////////////////////

int main(int argc, char **argv)
{

	std::string inputImageSequenceFileName;
	std::string outputImageSequenceFileName;
	std::string inputTestDataDir;

	int verboseLevel=vtkPlusLogger::LOG_LEVEL_INFO;

	int numberOfFailures(0); 
	
	vtksys::CommandLineArguments args;
	args.Initialize(argc, argv);

	args.AddArgument("--input-test-data-dir", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputTestDataDir, "Test data directory");
	args.AddArgument("--input-img-seq-file-name", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputImageSequenceFileName, "Filename of the input image sequence.");
	args.AddArgument("--output-img-seq-file-name", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &outputImageSequenceFileName, "Filename of the output image sequence.");
	args.AddArgument("--verbose", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &verboseLevel, "Verbose level (1=error only, 2=warning, 3=info, 4=debug)");	
	
	if ( !args.Parse() )
	{
		std::cerr << "Problem parsing arguments" << std::endl;
		std::cout << "Help: " << args.GetHelp() << std::endl;
		exit(EXIT_FAILURE);
	}

	vtkPlusLogger::Instance()->SetLogLevel(verboseLevel);

	if (inputImageSequenceFileName.empty())
	{
		std::cerr << "input-img-seq-file-name is required" << std::endl;
		exit(EXIT_FAILURE);
	}

	if (outputImageSequenceFileName.empty())
	{
		std::cerr << "output-img-seq-file-name is required" << std::endl;
		exit(EXIT_FAILURE);
	}

	///////////////

	
	itk::MetaImageSequenceIO::Pointer readerMetaImageSequenceIO = itk::MetaImageSequenceIO::New(); 
	ImageSequenceReaderType::Pointer reader = ImageSequenceReaderType::New(); // reader object, pointed to by smart pointer

	// Set the image IO 
	reader->SetImageIO(readerMetaImageSequenceIO); 
				
	std::string inputImageSequencePath=inputTestDataDir+"\\"+inputImageSequenceFileName;
	reader->SetFileName(inputImageSequencePath);   // set input file name, can also use variables commented out

	try
	{
		reader->Update(); 
	}
	catch (itk::ExceptionObject & err) 
	{		
		LOG_ERROR(" Exception! reader did not update: " <<  err); 
		return EXIT_FAILURE;
	}	
	
	int numberOfFrames = readerMetaImageSequenceIO->GetDimensions(2);

	LOG_INFO("Test GetCustomString method ..."); 
	const char* defaultTransformName = readerMetaImageSequenceIO->GetCustomString("DefaultFrameTransformName"); 
	if ( defaultTransformName == NULL )
	{
		LOG_ERROR("Unable to get custom string!"); 
		numberOfFailures++; 
	}

	LOG_INFO("Test GetDefaultFrameTransformName method ..."); 
	if ( strcmp(readerMetaImageSequenceIO->GetDefaultFrameTransformName(), defaultTransformName) > 0 )
	{
		LOG_ERROR("Unable to get default frame transform name!"); 
		numberOfFailures++; 
	}

	LOG_INFO("Test GetCustomTransform method ..."); 
	double tImageToTool[16]; 
	if ( !readerMetaImageSequenceIO->GetCustomTransform("ImageToToolTransform", tImageToTool) )
	{
		LOG_ERROR("Unable to get custom transform!"); 
		numberOfFailures++; 	
	}

	
	// ****************************************************************************** 

	itk::MetaImageSequenceIO::Pointer writerMetaImageSequenceIO = itk::MetaImageSequenceIO::New();
	ImageSequenceWriterType::Pointer writer = ImageSequenceWriterType::New(); // writer object, pointed to by smart pointer
	
	ImageSequenceType::Pointer image = reader->GetOutput(); 
	typedef itk::MetaDataDictionary DictionaryType; 
	DictionaryType &dictionary = image->GetMetaDataDictionary(); 

	writer->SetFileName(outputImageSequenceFileName);
	writer->SetInput(image); 
	writer->SetMetaDataDictionary(dictionary); 
	writer->UpdateOutputInformation(); 
	writer->SetImageIO(writerMetaImageSequenceIO); 
	writer->UseCompressionOn();

	LOG_INFO("Test SetDefaultFrameTransform method ..."); 
	writerMetaImageSequenceIO->SetDefaultFrameTransformName("DefaultTransform"); 

	LOG_INFO("Test SetFrameTransform method ..."); 
	// Add the transformation matrix to metafile
	for ( int i = 0 ; i < numberOfFrames; i++ ) 
	{
		vtkSmartPointer<vtkMatrix4x4> transMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
		transMatrix->SetElement(0,0,i); 
		writerMetaImageSequenceIO->SetFrameTransform(i, transMatrix); 
	}

	LOG_INFO("Test SetCustomFrameTransform method ..."); 
	for ( int i = 0 ; i < numberOfFrames; i++ ) 
	{
		vtkSmartPointer<vtkMatrix4x4> transMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
		transMatrix->SetElement(0,0,i); 
		writerMetaImageSequenceIO->SetCustomFrameTransform(i, "CustomTransform", transMatrix); 
	}
		
	LOG_INFO("Test SetCustomTransform method ..."); 
	vtkSmartPointer<vtkMatrix4x4> calibMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
	calibMatrix->Identity(); 
	writerMetaImageSequenceIO->SetCustomTransform("ImageToToolTransform", calibMatrix); 

	LOG_INFO("Test SetCustomFrameString method ..."); 
	if ( !writerMetaImageSequenceIO->SetCustomFrameString(5, writerMetaImageSequenceIO->GetDefaultFrameTransformName(), "DuplicateMatrix" ) ) 
	{
		LOG_DEBUG("Succesfuly avoid to duplicate a userfield in the writer's metadata!"); 
	}
	else
	{
		LOG_ERROR("Couldn't avoid to duplicate a userfield in the writer's metadata!"); 
		return EXIT_FAILURE; 
	}

	try
	{
		writer->Update(); 
	}
	catch (itk::ExceptionObject & err) 
	{		
		LOG_ERROR(" Exception! writer did not update: " <<  err); 
		return EXIT_FAILURE;
	}	

	LOG_INFO("Test GetFrameTransform method ..."); 
	for ( int i = 0; i < numberOfFrames; i++ )
	{
		vtkSmartPointer<vtkMatrix4x4> writerMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
		vtkSmartPointer<vtkMatrix4x4> readerMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
		
		if ( !readerMetaImageSequenceIO->GetFrameTransform(i, readerMatrix) )
		{
			LOG_ERROR("Unable to get default frame transform to frame #" << i); 
			numberOfFailures++; 
		}

		if ( !writerMetaImageSequenceIO->GetFrameTransform(i, writerMatrix) )
		{
			LOG_ERROR("Unable to get default frame transform to frame #" << i); 
			numberOfFailures++; 
		}

		for (int row = 0; row < 4; row++)
		{
			for (int col = 0; col < 4; col++)
			{
				if ( readerMatrix->GetElement(row, col) != writerMatrix->GetElement(row, col) ) 
				{
					LOG_ERROR("The input and output matrices are not the same at element: (" << row << ", " << col << "). ");
					numberOfFailures++; 
				}
			}
		}
		
	}

	if ( numberOfFailures > 0 )
	{
		LOG_ERROR("Total number of failures: " << numberOfFailures ); 
		LOG_ERROR("itkMetaImageSequenceIOTest1 failed!"); 
		return EXIT_FAILURE;
	}

	LOG_INFO("itkMetaImageSequenceIOTest1 completed successfully!"); 
	return EXIT_SUCCESS; 
 }