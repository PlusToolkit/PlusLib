#include "PlusConfigure.h"
#include "vtksys/CommandLineArguments.hxx"
#include <iomanip>
#include <sstream>

#include "vtkSmartPointer.h"
#include "vtkMatrix4x4.h"
#include "vtkTransform.h"
#include "vtkDirectory.h"
#include "vtkStringArray.h"

#include "itkMetaImageSequenceIO.h"
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkRGBPixel.h"
#include "itkComposeRGBImageFilter.h"

///////////////////////////////////////////////////////////////////
// Image type definition

typedef unsigned char          PixelType; // define type for pixel representation
typedef itk::RGBPixel< unsigned char >    RGBPixelType;
const unsigned int             imageDimension = 2; 
const unsigned int             imageSequenceDimension = 3; 

typedef itk::Image< PixelType, imageDimension > ImageType;
typedef itk::Image< RGBPixelType, imageDimension > RGBImageType;
typedef itk::Image< PixelType, 3 > ImageType3D;
typedef itk::Image< PixelType, imageSequenceDimension > ImageSequenceType;

typedef itk::ImageFileReader< ImageSequenceType > ImageSequenceReaderType;
typedef itk::ImageFileWriter< RGBImageType > ImageWriterType;
typedef itk::ImageFileWriter< ImageType3D > ImageWriterType3D;

///////////////////////////////////////////////////////////////////

enum SAVING_METHOD
{
	METAFILE=1, 
	BMP=2, 
	PNG=3, 
	JPG=4
}; 

void PrintProgressBar( int percent ); 

int main(int argc, char **argv)
{

	std::vector<std::string> inputImageSequenceFileNames;
	std::string outputFolder("./MetaSeqTestOutput");
	bool cleanFiles(false); 
	int inputSavingMethod(METAFILE);

	int verboseLevel=PlusLogger::LOG_LEVEL_INFO;

	vtksys::CommandLineArguments args;
	args.Initialize(argc, argv);

	args.AddArgument("--input-img-seq-file-names", vtksys::CommandLineArguments::MULTI_ARGUMENT, &inputImageSequenceFileNames, "Filenames of input image sequences (e.g. sequence_1.mhd sequence_2.mhd).");
	args.AddArgument("--output-folder", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &outputFolder, "Output folder name (Default: ./Output).");
	args.AddArgument("--clean-files", vtksys::CommandLineArguments::NO_ARGUMENT, &cleanFiles, "Delete output folder with generated files. ");	
	args.AddArgument("--verbose", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &verboseLevel, "Verbose level (1=error only, 2=warning, 3=info, 4=debug)");	
	args.AddArgument("--saving-method", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputSavingMethod, "Saving method ( Default: 1; 1=Metafile, 2=BMP, 3=PNG, 4=JPG )" );

	PlusLogger::Instance()->SetLogLevel(verboseLevel);

	if ( !args.Parse() )
	{
		std::cerr << "Problem parsing arguments" << std::endl;
		std::cout << "Help: " << args.GetHelp() << std::endl;
		exit(EXIT_FAILURE);
	}

	if ( inputImageSequenceFileNames.empty() )
	{
		std::cerr << "input-img-seq-file-name is required" << std::endl;
		exit(EXIT_FAILURE);
	}

	///////////////

	vtkSmartPointer<vtkDirectory> dir = vtkSmartPointer<vtkDirectory>::New(); 
	if ( dir->Open(outputFolder.c_str()) == 0 ) 
	{	
		dir->MakeDirectory(outputFolder.c_str()); 
	}

	LOG_INFO("Reading image sequences...");
	int numberOfImagesWritten = 0; 

	for ( int i = 0; i < inputImageSequenceFileNames.size(); i++) 
	{

		itk::MetaImageSequenceIO::Pointer readerMetaImageSequenceIO = itk::MetaImageSequenceIO::New(); 
		ImageSequenceReaderType::Pointer reader = ImageSequenceReaderType::New(); 

		// Set the image IO 
		reader->SetImageIO(readerMetaImageSequenceIO); 
		reader->SetFileName(inputImageSequenceFileNames[i].c_str());

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
		const unsigned long numberOfFrames = imageSeq->GetLargestPossibleRegion().GetSize()[2];	
		unsigned int frameSizeInBytes = imageWidthInPixels * imageHeightInPixels * sizeof(PixelType);

		LOG_INFO("Saving meta images to folder...");

		PixelType* imageSeqData = imageSeq->GetBufferPointer(); 
		for ( int imgNumber = 0; imgNumber < numberOfFrames; imgNumber++ )
		{
			PrintProgressBar( (100.0 * imgNumber) / numberOfFrames ); 
			PixelType* currentFrameImageData = imageSeqData + imgNumber * frameSizeInBytes;

			switch ( inputSavingMethod ) 
			{
			case BMP:
			case JPG:
			case PNG:
				{
					std::ostringstream fileName; 
					fileName << outputFolder << "/CapturedImageID_NO_" << std::setfill('0') << std::setw(4) << numberOfImagesWritten + imgNumber << "_RAS"; 
					if ( inputSavingMethod == BMP ) 
					{
						fileName << ".bmp"; 					
					}
					else if ( inputSavingMethod == JPG ) 
					{
						fileName << ".jpg"; 					
					}
					else if ( inputSavingMethod == PNG ) 
					{
						fileName << ".png"; 					
					}

					ImageType::Pointer frame = ImageType::New(); 
					ImageType::SizeType size = {imageWidthInPixels, imageHeightInPixels};
					ImageType::IndexType start = {0,0};
					ImageType::RegionType region;
					region.SetSize(size);
					region.SetIndex(start);
					frame->SetRegions(region);
					frame->Allocate();

					memcpy(frame->GetBufferPointer() , currentFrameImageData , frameSizeInBytes);

					RGBImageType::Pointer frameRGB = RGBImageType::New(); 
					RGBImageType::SizeType sizeRGB = {imageWidthInPixels, imageHeightInPixels};
					RGBImageType::IndexType startRGB = {0,0};
					RGBImageType::RegionType regionRGB;
					regionRGB.SetSize(sizeRGB);
					regionRGB.SetIndex(startRGB);
					frameRGB->SetRegions(regionRGB);
					frameRGB->Allocate();

					typedef itk::ComposeRGBImageFilter< ImageType, RGBImageType > ComposeRGBFilterType;
					ComposeRGBFilterType::Pointer composeRGB = ComposeRGBFilterType::New();
					composeRGB->SetInput1( frame );
					composeRGB->SetInput2( frame );
					composeRGB->SetInput3( frame );
					composeRGB->Update(); 

					ImageWriterType::Pointer writer = ImageWriterType::New();
					writer->SetFileName( fileName.str().c_str() ); 
					writer->SetInput( composeRGB->GetOutput() ); 

					try    
					{
						writer->Update();
					}	
					catch (itk::ExceptionObject & e)
					{
						LOG_ERROR(e.GetDescription());
					}

					vtkSmartPointer<vtkMatrix4x4> transMatrix = vtkSmartPointer<vtkMatrix4x4>::New(); 

					if ( !readerMetaImageSequenceIO->GetFrameTransform(imgNumber, transMatrix) )
					{
						LOG_ERROR("Unable to get default frame transform for frame #" << imgNumber); 
					}
					
					std::ostringstream transformFileName; 
					transformFileName << fileName.str().c_str() << ".transforms" << std::ends; 
					vtkstd::string transformData; 
					transformData+= "# ================================ #\n";
					transformData+= "# Transform Data of Captured Image #\n";
					transformData+= "# ================================ #\n";
					transformData+= "# THIS FILE CONTAINS THE REAL-TIME TRANSFORM DATA FOR THE CAPTURED IMAGE.\n";
					transformData+= "# DATA IS RECORDED IN THE FOLLOWING FORMAT:\n";
					transformData+= "# [FORMAT: Quaternions(Angle - in Degrees, Qx, Qy, Qz) Position(x, y, z)]\n";
					transformData+= "# THIS FILE IS AUTO-GENERATED BY THE PROGRAM.  DO NOT EDIT!\n";
					transformData+= "\n";
					transformData+= "# NAME OF THE CAPTURED IMAGE WITH PATH\n";
					transformData+= fileName.str();
					transformData+= "\n\n";

					FILE * transformFile = fopen ( transformFileName.str().c_str(), "w");
					if (transformFile != NULL)
					{
						double wxyz[4], xyz[3]; 
						vtkstd::ostringstream os; 

						os  << "# --------------------------------------------------------------- #\n"
							<< "# TRANSFORMATION FROM TRACKING DEVICE\n"
							<< std::endl; 

						vtkSmartPointer<vtkTransform> helperTransform = vtkSmartPointer<vtkTransform>::New(); 
						helperTransform->SetMatrix(transMatrix); 
						helperTransform->GetOrientationWXYZ(wxyz); 
						helperTransform->GetPosition(xyz); 

						//os  << "# [TRANSFORM_QUATERNIONS_USPROBE_TO_WORLD]\n"
						os	<< "# [TRANSFORM_QUATERNIONS_USPROBE_TO_STEPPER_FRAME]\n"
							<< wxyz[0] << "\t" << wxyz[1] << "\t" << wxyz[2] << "\t" << wxyz[3] << "\t"
							<<  xyz[0] << "\t" <<  xyz[1] << "\t" <<  xyz[2] << "\n" 
							<< "\n"
							//<< "# [TRANSFORM_HOMOGENEOUS4x4_USPROBE_TO_WORLD]\n"
							<< "# [TRANSFORM_HOMOGENEOUS4x4_USPROBE_TO_STEPPER_FRAME]\n"
							<< transMatrix->GetElement(0,0) << "\t" << transMatrix->GetElement(0,1) << "\t" 
							<< transMatrix->GetElement(0,2) << "\t" << transMatrix->GetElement(0,3) << "\n"
							<< transMatrix->GetElement(1,0) << "\t" << transMatrix->GetElement(1,1) << "\t" 
							<< transMatrix->GetElement(1,2) << "\t" << transMatrix->GetElement(1,3) << "\n"
							<< transMatrix->GetElement(2,0) << "\t" << transMatrix->GetElement(2,1) << "\t" 
							<< transMatrix->GetElement(2,2) << "\t" << transMatrix->GetElement(2,3) << "\n"
							<< transMatrix->GetElement(3,0) << "\t" << transMatrix->GetElement(3,1) << "\t" 
							<< transMatrix->GetElement(3,2) << "\t" << transMatrix->GetElement(3,3) << "\n"
							<< std::endl; 

						os << "# --------------------------------------------------------------- #\n\n"; 

						os << std::ends; 

						transformData.append(os.str().c_str()); 
						fprintf(transformFile, transformData.c_str()); 
						fclose(transformFile); 


					}
				}
				break; 
			case METAFILE:
				{

					ImageType3D::Pointer frame = ImageType3D::New(); 
					ImageType3D::SizeType size = {imageWidthInPixels, imageHeightInPixels, 1 };
					ImageType3D::IndexType start = {0,0,0};
					ImageType3D::RegionType region;
					region.SetSize(size);
					region.SetIndex(start);
					frame->SetRegions(region);
					frame->Allocate();

					memcpy(frame->GetBufferPointer() , currentFrameImageData , frameSizeInBytes);

					vtkSmartPointer<vtkMatrix4x4> transformMatrix = vtkSmartPointer<vtkMatrix4x4>::New(); 
					if ( !readerMetaImageSequenceIO->GetFrameTransform(imgNumber, transformMatrix) )
					{
						LOG_ERROR("Unable to get default frame transform for frame #" << imgNumber); 
					}

					double *offset = new double[3]; 
					offset[0] = transformMatrix->GetElement(0,3); 
					offset[1] = transformMatrix->GetElement(1,3); 
					offset[2] = transformMatrix->GetElement(2,3); 
					
					double *transform = new double[9]; 
					transform[0] = transformMatrix->GetElement(0,0); 
					transform[1] = transformMatrix->GetElement(0,1); 
					transform[2] = transformMatrix->GetElement(0,2); 

					transform[3] = transformMatrix->GetElement(1,0); 
					transform[4] = transformMatrix->GetElement(1,1); 
					transform[5] = transformMatrix->GetElement(1,2); 

					transform[6] = transformMatrix->GetElement(2,0); 
					transform[7] = transformMatrix->GetElement(2,1); 
					transform[8] = transformMatrix->GetElement(2,2); 
					
					itk::MetaImageIO::Pointer writerMetaImageIO = itk::MetaImageIO::New(); 
					ImageWriterType3D::Pointer writer = ImageWriterType3D::New(); 

					std::ostringstream fileName; 
					fileName << outputFolder << "/Frame" << std::setfill('0') << std::setw(4) << numberOfImagesWritten + imgNumber << ".mha" << std::ends;
					writer->SetFileName(fileName.str().c_str());
					writer->SetImageIO(writerMetaImageIO); 

					writerMetaImageIO->SetNumberOfDimensions(3); 
					writerMetaImageIO->SetDimensions(0, imageWidthInPixels); 
					writerMetaImageIO->SetDimensions(1, imageHeightInPixels); 
					writerMetaImageIO->SetDimensions(2, 0);
					writerMetaImageIO->GetMetaImagePointer()->AnatomicalOrientation("RAI"); 

					ImageType3D::PointType origin; 
					origin.Get_vnl_vector().copy_in(offset); 
					frame->SetOrigin(origin); 
					delete[] offset; 

					ImageType3D::DirectionType direction; 
					direction.GetVnlMatrix().copy_in(transform); 
					frame->SetDirection(direction); 
					delete[] transform; 

					writer->SetInput(frame); 

					try
					{
						writer->Update(); 
					}
					catch (itk::ExceptionObject & err) 
					{		
						LOG_ERROR( "Image writer couldn't update: " <<  err); 
						exit(EXIT_FAILURE);
					}
				}
			}
		}
		PrintProgressBar( 100 ); 
		std::cout << std::endl; 

		numberOfImagesWritten += numberOfFrames; 
	}


	vtkSmartPointer<vtkDirectory> outputDir = vtkSmartPointer<vtkDirectory>::New(); 
	if ( outputDir->Open(outputFolder.c_str()) != 0 ) 
	{	
		if ( cleanFiles )
		{
			outputDir->DeleteDirectory(outputFolder.c_str()); 
		}
	}
	else
	{
		LOG_ERROR( "Couldn't open folder for reading: " << outputFolder); 
		exit(EXIT_FAILURE);
	}

	LOG_INFO( "itkMetaImageSequenceIOTest2 completed successfully!" ); 
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