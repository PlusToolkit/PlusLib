#include "PlusConfigure.h"
#include "vtksys/CommandLineArguments.hxx"
#include <iomanip>

#include "vtkSmartPointer.h"
#include "vtkMatrix4x4.h"
#include "vtkDirectory.h"

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
typedef itk::Image< PixelType, 3 > ImageType3D;
typedef itk::Image< PixelType, imageSequenceDimension > ImageSequenceType;

typedef itk::ImageFileWriter< ImageSequenceType > ImageSequenceWriterType;

///////////////////////////////////////////////////////////////////

void PrintProgressBar( int percent ); 

int main(int argc, char **argv)
{

	std::string outputImageSequenceFileName("GeneratedUsSeqMetafile.mhd");
	std::string outputFolder;
	
	int verboseLevel=vtkPlusLogger::LOG_LEVEL_INFO;

	vtksys::CommandLineArguments args;
	args.Initialize(argc, argv);

	args.AddArgument("--output-img-seq-file-name", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &outputImageSequenceFileName, "Filename of the input image sequence.");
	args.AddArgument("--output-folder", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &outputFolder, "Output folder name (Default: ./Output).");
	args.AddArgument("--verbose", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &verboseLevel, "Verbose level (1=error only, 2=warning, 3=info, 4=debug)");	

	vtkPlusLogger::Instance()->SetLogLevel(verboseLevel);
	
	if ( !args.Parse() )
	{
		std::cerr << "Problem parsing arguments" << std::endl;
		std::cout << "Help: " << args.GetHelp() << std::endl;
		exit(EXIT_FAILURE);
	}

	///////////////

	vtkSmartPointer<vtkDirectory> dir = vtkSmartPointer<vtkDirectory>::New(); 
	if ( ! outputFolder.empty() && dir->Open(outputFolder.c_str()) == 0 ) 
	{	
		dir->MakeDirectory(outputFolder.c_str()); 
	}
	
	LOG_INFO("Writing image sequence...");
	itk::MetaImageSequenceIO::Pointer writerMetaImageSequenceIO = itk::MetaImageSequenceIO::New(); 
	ImageSequenceWriterType::Pointer writer = ImageSequenceWriterType::New(); 

	ImageSequenceType::Pointer imageDataSequence = ImageSequenceType::New();
	ImageSequenceType::SizeType size = {400, 400, 340 };
	ImageSequenceType::IndexType start = {0,0,0};
	ImageSequenceType::RegionType region;
	region.SetSize(size);
	region.SetIndex(start);
	imageDataSequence->SetRegions(region);
	 try
    {
        imageDataSequence->Allocate();
    }
    catch (itk::ExceptionObject & err) 
    {		
        LOG_ERROR("Unable to allocate memory for image data sequence: " << err);
        return EXIT_FAILURE; 
    }

	const unsigned long ImageWidthInPixels = imageDataSequence->GetLargestPossibleRegion().GetSize()[0]; 
	const unsigned long ImageHeightInPixels = imageDataSequence->GetLargestPossibleRegion().GetSize()[1]; 
	const unsigned long numberOfFrames = imageDataSequence->GetLargestPossibleRegion().GetSize()[2];	

	PixelType* imageData = imageDataSequence->GetBufferPointer(); // pointer to the image pixel buffer
	memset(imageData, 0, ImageWidthInPixels*ImageHeightInPixels*numberOfFrames*sizeof(PixelType));

	unsigned int frameSizeInBytes=ImageWidthInPixels*ImageHeightInPixels*sizeof(PixelType);

	for ( int i = 0 ; i < numberOfFrames; i++ ) 
	{
		PrintProgressBar( 100 * i / numberOfFrames ); 
		PixelType *currentFrameImageData = imageData + i * frameSizeInBytes;

		if ( i > numberOfFrames - 40 && i < numberOfFrames - 10 ) 
		{
			for ( int y = 150 ; y < 180; y++ ) 
			{
				for ( int x = 70; x < 100; x++ )
				{
					PixelType* px = currentFrameImageData + (x + ImageWidthInPixels * y);
					*px = 255;
				}
			}
		}

		for ( int y = i; y < i+60; y++ ) 
		{
			for ( int x = i; x < i+60; x++ )
			{
				PixelType* px = currentFrameImageData + (x + ImageWidthInPixels * y);
				*px = 255;
			}
		}

		for ( int y = i; y < i+60; y++ ) 
		{
			for ( int x = ImageWidthInPixels - i - 60; x < ImageWidthInPixels - i; x++ )
			{
				PixelType* px = currentFrameImageData + (x + ImageWidthInPixels * y);
				*px = 255;
			}
		}

		for ( int y = ImageHeightInPixels - i - 60; y < ImageHeightInPixels - i; y++ ) 
		{
			for ( int x = i; x < i+60; x++ )
			{
				PixelType* px = currentFrameImageData + (x + ImageWidthInPixels * y);
				*px = 255;
			}
		}

		for ( int y = ImageHeightInPixels - i - 60; y < ImageHeightInPixels - i; y++ ) 
		{
			for ( int x = ImageWidthInPixels - i - 60; x < ImageWidthInPixels - i; x++ )
			{
				PixelType* px = currentFrameImageData + (x + ImageWidthInPixels * y);
				*px = 255;
			}
		}

		vtkSmartPointer<vtkMatrix4x4> matrix = vtkSmartPointer<vtkMatrix4x4>::New();
		matrix->SetElement(2,3, -i);
		writerMetaImageSequenceIO->SetFrameTransform(i, matrix); 
	}

	PrintProgressBar( 100 ); 
	std::cout << std::endl; 

	std::ostringstream sequenceDataFileName; 
	if ( ! outputFolder.empty() )
	{
		sequenceDataFileName << outputFolder << "/";
	}
	sequenceDataFileName << outputImageSequenceFileName << std::ends; 
	
	writer->SetFileName(sequenceDataFileName.str());
	writer->SetInput(imageDataSequence); 
	writer->SetImageIO(writerMetaImageSequenceIO); 
	writer->SetUseCompression(true); 

	try
	{
		writer->Update(); 
	}
	catch (itk::ExceptionObject & err) 
	{		
		LOG_ERROR(" Unable to update sequence writer: " << err);
        return EXIT_FAILURE; 
	}	
	
	LOG_INFO( "itkMetaImageSequenceIOTest4 completed successfully!" ); 
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