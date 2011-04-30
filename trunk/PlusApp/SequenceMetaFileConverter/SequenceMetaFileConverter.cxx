#include "PlusConfigure.h"
#include <iostream>

#include "vtksys/CommandLineArguments.hxx" 
#include "vtksys/SystemTools.hxx" 
#include "vtkDirectory.h"
#include "vtkMatrix4x4.h"
#include "vtkTransform.h"
#include "vtkSmartPointer.h"
#include "vtkMath.h"

#include "itkMetaImageSequenceIO.h"
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkRGBPixel.h"
#include "itkComposeRGBImageFilter.h"
#include "itkRGBToLuminanceImageFilter.h"

// VXL/VNL Includes
#include "vnl/vnl_matrix.h"
#include "vnl/vnl_vector.h"

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

typedef itk::ImageFileReader< ImageType > ImageReaderType;
typedef itk::ImageFileReader< RGBImageType > RGBImageReaderType;
typedef itk::ImageFileReader< ImageSequenceType > ImageSequenceReaderType;

typedef itk::ImageFileWriter< ImageType > ImageWriterType;
typedef itk::ImageFileWriter< RGBImageType > RGBImageWriterType;
typedef itk::ImageFileWriter< ImageType3D > ImageWriterType3D;
typedef itk::ImageFileWriter< ImageSequenceType > ImageSequenceWriterType;

typedef itk::RGBToLuminanceImageFilter<RGBImageType, ImageType> RGBToGrayscaleFilterType; 

// <CustomFrameFieldName, CustomFrameFieldValue>
typedef std::pair<std::string, std::string> CustomFrameFieldPair; 
// <CustomFieldName, CustomFieldValue>
typedef std::pair<std::string, std::string> CustomFieldPair; 

///////////////////////////////////////////////////////////////////



struct TrackedFrame
{
	std::string DefaultFrameTransformName; 
	std::vector<CustomFrameFieldPair> CustomFrameFieldList; 
	std::vector<CustomFieldPair> CustomFieldList; 
	ImageType* ImageData;
};

typedef std::vector<TrackedFrame> TrackedFrameList;

enum SAVING_METHOD
{
	METAFILE=1, 
	SEQUENCE_METAFILE=2,
	BMP24=3, 
	BMP8=4, 
	PNG=5, 
	JPG=6
}; 

enum CONVERT_METHOD
{
	FROM_SEQUENCE_METAFILE=1, 
	FROM_METAFILE=2,
	FROM_BMP24=3, 
	FROM_OLD_SEQUENCE_METAFILE=4
}; 


void ConvertFromBitmap(SAVING_METHOD savingMethod); 
void ConvertFromSequenceMetafile(std::vector<std::string> inputImageSequenceFileNames, SAVING_METHOD savingMethod ); 
void ConvertFromOldSequenceMetafile(std::vector<std::string> inputImageSequenceFileNames, SAVING_METHOD savingMethod); 

void SaveImages( TrackedFrameList &trackedFrameList, SAVING_METHOD savingMethod, int numberOfImagesWritten ); 
void SaveImageToMetaFile( TrackedFrame trackedFrame, std::string metaFileName, bool useCompression ); 
void SaveImageToSequenceMetaFile( TrackedFrameList &trackedFrameList, std::string sequenceDataFileName, bool useCompression ); 
void SaveImageToBitmap( ImageType* image, std::string bitmapFileName, int savingMethod ); 

void SaveTransformToFile(TrackedFrame trackedFrame, std::string imageFileName); 

void ReadTransformFile( const std::string TransformFileNameWithPath, double* transformUSProbe2StepperFrame ); 
void ReadDRBTransformFile( const std::string TransformFileNameWithPath, std::vector<CustomFrameFieldPair> &customFrameFieldList, std::string &defaultFrameTransformName ); 

void PrintProgressBar( int percent ); 

std::string inputDataDir;
std::string inputBitmapPrefix("CapturedImageID_NO_"); 
std::string inputBitmapSuffix(""); 
std::string inputTransformSuffix(".transforms");
std::string outputSequenceFileName("SeqMetafile");
std::string outputFolder("./");
bool inputUseCompression(false); 


//-------------------------------------------------------------------------------
int main (int argc, char* argv[])
{ 
	bool printHelp(false); 

	std::string inputConvertMethod("FROM_BMP24"); 
	CONVERT_METHOD convertMethod(FROM_BMP24); 

	std::string inputSavingMethod("SEQUENCE_METAFILE"); 
	SAVING_METHOD savingMethod(SEQUENCE_METAFILE);

	std::vector<std::string> inputImageSequenceFileNames;

	std::string verboseLevel("INFO");

	vtksys::CommandLineArguments cmdargs;
	cmdargs.Initialize(argc, argv);

	cmdargs.AddArgument("--help", vtksys::CommandLineArguments::NO_ARGUMENT, &printHelp, "Print this help.");	
	cmdargs.AddArgument("--use-compression", vtksys::CommandLineArguments::NO_ARGUMENT, &inputUseCompression, "Compress metafile and sequence metafile images.");	
	
	cmdargs.AddArgument("--saving-method", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputSavingMethod, "Saving method ( Default: SEQUENCE_METAFILE; METAFILE, SEQUENCE_METAFILE, BMP24, BMP8, PNG, JPG)" );
	cmdargs.AddArgument("--convert-method", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputConvertMethod, "Convert method ( Default: FROM_BMP24; FROM_BMP24, FROM_METAFILE, FROM_OLD_SEQUENCE_METAFILE, FROM_SEQUENCE_METAFILE)" );

	cmdargs.AddArgument("--input-data-dir", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputDataDir, "Input data directory (default: ./)");
	cmdargs.AddArgument("--input-img-seq-file-names", vtksys::CommandLineArguments::MULTI_ARGUMENT, &inputImageSequenceFileNames, "Filenames of meta image sequences (e.g. sequence_1.mhd sequence_2.mhd).");
	cmdargs.AddArgument("--input-bitmap-prefix", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputBitmapPrefix, "Prefix of bitmap images (default: CapturedImageID_NO_).");
	cmdargs.AddArgument("--input-bitmap-suffix", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputBitmapSuffix, "Suffix of bitmap images.");
	cmdargs.AddArgument("--input-transform-suffix", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputTransformSuffix, "Suffix of transform files (default: .transforms).");

	cmdargs.AddArgument("--output-folder", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &outputFolder, "Output folder name (Default: ./Output).");
	cmdargs.AddArgument("--output-sequence-file-name", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &outputSequenceFileName, "Output sequence file name. (Default: SeqMetafile)");

	cmdargs.AddArgument("--verbose", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &verboseLevel, "Verbose level (Default: INFO; ERROR, WARNING, INFO, DEBUG)");	

	if ( STRCASECMP("ERROR", verboseLevel.c_str())==0 )
	{
		PlusLogger::Instance()->SetLogLevel(PlusLogger::LOG_LEVEL_ERROR);
	}
	else if ( STRCASECMP("WARNING", verboseLevel.c_str())==0 )
	{
		PlusLogger::Instance()->SetLogLevel(PlusLogger::LOG_LEVEL_WARNING);
	}
	else if ( STRCASECMP("INFO", verboseLevel.c_str())==0 )
	{
		PlusLogger::Instance()->SetLogLevel(PlusLogger::LOG_LEVEL_INFO);
	}
	else if ( STRCASECMP("DEBUG", verboseLevel.c_str())==0 )
	{
		PlusLogger::Instance()->SetLogLevel(PlusLogger::LOG_LEVEL_DEBUG);
	}


	if ( !cmdargs.Parse() )
	{
		LOG_ERROR( "Problem parsing arguments\n");
		std::cout << "Help: " << cmdargs.GetHelp() << std::endl;
		exit(EXIT_FAILURE);
	}

	if ( printHelp ) 
	{
		std::cout << "MetaSequenceFileConverter help: " << cmdargs.GetHelp() << std::endl;
		exit(EXIT_SUCCESS); 

	}

	////////////////////////////////////////////////////

	VTK_LOG_TO_CONSOLE_ON; 

	vtkSmartPointer<vtkDirectory> dir = vtkSmartPointer<vtkDirectory>::New(); 
	if ( dir->Open(outputFolder.c_str()) == 0 ) 
	{	
		dir->MakeDirectory(outputFolder.c_str()); 
	}


	if ( STRCASECMP("METAFILE", inputSavingMethod.c_str())==0 )
	{
		savingMethod = METAFILE; 
	}
	else if ( STRCASECMP("SEQUENCE_METAFILE", inputSavingMethod.c_str())==0 )
	{
		savingMethod = SEQUENCE_METAFILE; 
	}
	else if ( STRCASECMP("BMP24", inputSavingMethod.c_str())==0 )
	{
		savingMethod = BMP24; 
	}
	else if ( STRCASECMP("BMP8", inputSavingMethod.c_str())==0 )
	{
		savingMethod = BMP8; 
	}
	else if ( STRCASECMP("PNG", inputSavingMethod.c_str())==0 )
	{
		savingMethod = PNG; 
	}
	else if ( STRCASECMP("JPG", inputSavingMethod.c_str())==0 )
	{
		savingMethod = JPG; 
	}
	else 
	{
		LOG_ERROR("Unable to recognize saving method: " << inputSavingMethod)
		exit(EXIT_FAILURE); 
	}

	if ( STRCASECMP("FROM_BMP24", inputConvertMethod.c_str())==0 )
	{
		convertMethod = FROM_BMP24; 
	}
	else if ( STRCASECMP("FROM_METAFILE", inputConvertMethod.c_str())==0 )
	{
		convertMethod = FROM_METAFILE; 
	}
	else if ( STRCASECMP("FROM_OLD_SEQUENCE_METAFILE", inputConvertMethod.c_str())==0 )
	{
		convertMethod = FROM_OLD_SEQUENCE_METAFILE; 
	}
	else if ( STRCASECMP("FROM_SEQUENCE_METAFILE", inputConvertMethod.c_str())==0 )
	{
		convertMethod = FROM_SEQUENCE_METAFILE; 
	}
	else 
	{
		LOG_ERROR("Unable to recognize convert method: " << inputConvertMethod)
		exit(EXIT_FAILURE); 
	}


	switch (convertMethod)
	{
	case FROM_SEQUENCE_METAFILE: 
		{
			ConvertFromSequenceMetafile(inputImageSequenceFileNames, savingMethod); 
		}
		break; 
	case FROM_METAFILE: 
		{
			//ConvertFromMetafile(); 
		}
		break; 
	case FROM_BMP24: 
		{
			ConvertFromBitmap(savingMethod); 
		}
		break; 
	case FROM_OLD_SEQUENCE_METAFILE: 
		{
			ConvertFromOldSequenceMetafile(inputImageSequenceFileNames, savingMethod); 
		}
		break; 
	}

	VTK_LOG_TO_CONSOLE_OFF; 

	return EXIT_SUCCESS; 
}


//-------------------------------------------------------------------------------
void ConvertFromSequenceMetafile(std::vector<std::string> inputImageSequenceFileNames, SAVING_METHOD savingMethod)
{
	LOG_INFO("Converting sequence metafile images..."); 
	TrackedFrameList trackedFrameContainer; 
	int numberOfImagesWritten(0); 

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

		PixelType* imageSeqData = imageSeq->GetBufferPointer(); 
		for ( int imgNumber = 0; imgNumber < numberOfFrames; imgNumber++ )
		{
			PrintProgressBar( (100.0 * imgNumber) / numberOfFrames ); 

			ImageType::Pointer frame = ImageType::New(); 
			ImageType::SizeType size = {imageWidthInPixels, imageHeightInPixels};
			ImageType::IndexType start = {0,0};
			ImageType::RegionType region;
			region.SetSize(size);
			region.SetIndex(start);
			frame->SetRegions(region);
			frame->Allocate();

			PixelType* currentFrameImageData = imageSeqData + imgNumber * frameSizeInBytes;

			memcpy(frame->GetBufferPointer() , currentFrameImageData , frameSizeInBytes);

			TrackedFrame trackedFrame;
			
			// Get Default transform name 
			std::string defaultFrameTransformName = readerMetaImageSequenceIO->GetDefaultFrameTransformName(); 
			trackedFrame.DefaultFrameTransformName = defaultFrameTransformName; 

			// Get custom fields 
			std::vector<std::string> customFieldNames = readerMetaImageSequenceIO->GetCustomFieldNames(); 
			for ( int i = 0; i < customFieldNames.size(); i++ )
			{
				CustomFieldPair field; 
				field.first = customFieldNames[i]; 
				field.second = readerMetaImageSequenceIO->GetCustomString(customFieldNames[i].c_str()); 
				trackedFrame.CustomFieldList.push_back(field); 
			}

			// Get custom frame fields 
			std::vector<std::string> customFrameFieldNames = readerMetaImageSequenceIO->GetCustomFrameFieldNames(); 
			for ( int i = 0; i < customFrameFieldNames.size(); i++ )
			{
				CustomFrameFieldPair field; 
				field.first = customFrameFieldNames[i]; 
				field.second = readerMetaImageSequenceIO->GetCustomFrameString(imgNumber,customFrameFieldNames[i].c_str()); 
				trackedFrame.CustomFrameFieldList.push_back(field); 
			}

			frame->Register();
			trackedFrame.ImageData = frame;
			trackedFrameContainer.push_back(trackedFrame);

			if ( savingMethod != SEQUENCE_METAFILE )
			{
				SaveImages(trackedFrameContainer, savingMethod, numberOfImagesWritten++); 

				for ( int i = 0; i < trackedFrameContainer.size(); i++)
				{
					trackedFrameContainer[i].ImageData->UnRegister(); 
				}
				trackedFrameContainer.clear(); 
			}
		}

		PrintProgressBar(100); 
		if ( savingMethod == SEQUENCE_METAFILE )
		{
			SaveImages(trackedFrameContainer, savingMethod, numberOfImagesWritten++); 

			for ( int i = 0; i < trackedFrameContainer.size(); i++)
			{
				trackedFrameContainer[i].ImageData->UnRegister(); 
			}
			trackedFrameContainer.clear(); 
		}
	}

}

//-------------------------------------------------------------------------------
void ConvertFromOldSequenceMetafile(std::vector<std::string> inputImageSequenceFileNames, SAVING_METHOD savingMethod)
{
	LOG_INFO("Converting old sequence metafile images..."); 
	TrackedFrameList trackedFrameContainer; 
	int numberOfImagesWritten(0); 

	for ( int i = 0; i < inputImageSequenceFileNames.size(); i++) 
	{
		itk::MetaImageSequenceIO::Pointer readerMetaImageSequenceIO = itk::MetaImageSequenceIO::New(); 
		ImageSequenceReaderType::Pointer reader = ImageSequenceReaderType::New();

		readerMetaImageSequenceIO->AddCustomFrameFieldNameForReading("TransformMatrix"); 
		readerMetaImageSequenceIO->AddCustomFrameFieldNameForReading("Offset"); 

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

		PixelType* imageSeqData = imageSeq->GetBufferPointer(); 
		for ( int imgNumber = 0; imgNumber < numberOfFrames; imgNumber++ )
		{
			PrintProgressBar( (100.0 * imgNumber) / numberOfFrames ); 

			ImageType::Pointer frame = ImageType::New(); 
			ImageType::SizeType size = {imageWidthInPixels, imageHeightInPixels};
			ImageType::IndexType start = {0,0};
			ImageType::RegionType region;
			region.SetSize(size);
			region.SetIndex(start);
			frame->SetRegions(region);
			frame->Allocate();

			PixelType* currentFrameImageData = imageSeqData + imgNumber * frameSizeInBytes;

			memcpy(frame->GetBufferPointer() , currentFrameImageData , frameSizeInBytes);

			// Get offset value 
			double offset[3] = {0,0,0}; 
			std::istringstream offsetFieldValue(readerMetaImageSequenceIO->GetCustomFrameString(imgNumber, "Offset")); 
			double offsetItem(0); 
			int offsetElement(0); 
			while ( offsetFieldValue >> offsetItem )
			{
				offset[offsetElement++] = offsetItem; 
			}

			// Get transform value
			double transform[9] = {0,0,0,0,0,0,0,0,0}; 
			std::istringstream transformFieldValue(readerMetaImageSequenceIO->GetCustomFrameString(imgNumber, "TransformMatrix")); 
			double transformItem(0); 
			int transformElement(0); 
			while ( transformFieldValue >> transformItem )
			{
				transform[transformElement++] = transformItem; 
			}

			// Copy transform to the custom frame list 
			std::vector<CustomFrameFieldPair> customFrameFieldList; 
			std::string defaultFrameTransformName = "ToolToTrackerTransform"; 

			std::ostringstream strTool2Tracker; 
			strTool2Tracker 
				<< transform[0]  << " " << transform[1]  << " " << transform[2]  << " " << offset[0]  << " " 
				<< transform[3]  << " " << transform[4]  << " " << transform[5]  << " " << offset[1]  << " " 
				<< transform[6]  << " " << transform[7]  << " " << transform[8]  << " " << offset[2]  << " " 
				<< 0			 << " " << 0			 << " " << 0			 << " " << 1		  << " "; 


			CustomFrameFieldPair toolToTrackerField; 
			toolToTrackerField.first = "ToolToTrackerTransform"; 
			toolToTrackerField.second = strTool2Tracker.str(); 

			customFrameFieldList.push_back(toolToTrackerField); 

			// Create tracked frame struct
			TrackedFrame trackedFrame;
			frame->Register();
			trackedFrame.ImageData = frame;
			trackedFrame.CustomFrameFieldList = customFrameFieldList; 
			trackedFrame.DefaultFrameTransformName = defaultFrameTransformName; 

			trackedFrameContainer.push_back(trackedFrame);
		}

		PrintProgressBar(100); 

		SaveImages(trackedFrameContainer, savingMethod, ++numberOfImagesWritten); 

		for ( int i = 0; i < trackedFrameContainer.size(); i++)
		{
			trackedFrameContainer[i].ImageData->UnRegister(); 
		}
		trackedFrameContainer.clear(); 

		
	}
}

//-------------------------------------------------------------------------------
void ConvertFromBitmap(SAVING_METHOD savingMethod)
{
	LOG_INFO("Converting bitmap images..."); 
	TrackedFrameList trackedFrameContainer; 
	int numberOfImagesWritten(0); 
	int frameNumber(0); 
  
  LOG_INFO( "Opening directory" );
	vtkSmartPointer<vtkDirectory> dir = vtkSmartPointer<vtkDirectory>::New(); 
	dir->Open(inputDataDir.c_str()); 
	int totalNumberOfImages = dir->GetNumberOfFiles() / 2; 
  
	std::ostringstream imageFileNameWithPath; 
	imageFileNameWithPath << inputDataDir << "/" << inputBitmapPrefix << std::setfill('0') << std::setw(4) << frameNumber << inputBitmapSuffix << ".bmp"; 
  
  
    // Since US volumes are sometimes merged, file names not necessarily
    // end with consecutive numbers. All .bmp files should be checked.
  
	for ( int dirIndex = 0; dirIndex < dir->GetNumberOfFiles(); ++ dirIndex )
	{
		// Skip this file, if it's not a .bmp file.

		PrintProgressBar(frameNumber*100 / totalNumberOfImages ); 

		std::string fileName( dir->GetFile( dirIndex ) );
		std::size_t pos = 0;
		pos = fileName.rfind( ".bmp" );
		if ( pos != fileName.length() - 4 ) continue;


		imageFileNameWithPath.str( "" );
		imageFileNameWithPath << inputDataDir << "/" << fileName;

		RGBImageReaderType::Pointer reader = RGBImageReaderType::New(); 
		reader->SetFileName(imageFileNameWithPath.str().c_str());

		try
		{
			reader->Update(); 
		}
		catch (itk::ExceptionObject & err) 
		{		
			LOG_ERROR( "RGB image reader couldn't update: " <<  err); 
			exit(EXIT_FAILURE);
		}	

		RGBImageType::Pointer imageRGB = reader->GetOutput();

		RGBToGrayscaleFilterType::Pointer filter = RGBToGrayscaleFilterType::New(); 
		filter->SetInput(imageRGB); 

		try
		{
			filter->Update(); 
		}
		catch (itk::ExceptionObject & err) 
		{		
			LOG_ERROR( "RGB image converting failed: " <<  err); 
			exit(EXIT_FAILURE);
		}

		ImageType::Pointer imageData = filter->GetOutput(); 
    
    
      // Try to find the file name for the transform file.
    
		std::ostringstream transformFileNameWithPath; 
		transformFileNameWithPath << imageFileNameWithPath.str() << inputTransformSuffix;
		
		if ( ! vtksys::SystemTools::FileExists( transformFileNameWithPath.str().c_str() ) )
		{
			transformFileNameWithPath.str( "" );
			pos = fileName.find( ".bmp" );
			fileName.replace( pos, 4, ".transforms" );
			transformFileNameWithPath << inputDataDir << "/" << fileName;
		}

		
		std::vector<CustomFrameFieldPair> customFrameFieldList; 
		std::string defaultFrameTransformName; 
		ReadDRBTransformFile( transformFileNameWithPath.str(), customFrameFieldList, defaultFrameTransformName ); 

		TrackedFrame trackedFrame;
		trackedFrame.DefaultFrameTransformName = defaultFrameTransformName; 
		trackedFrame.CustomFrameFieldList = customFrameFieldList; 
		imageData->Register();
		trackedFrame.ImageData = imageData;

		trackedFrameContainer.push_back(trackedFrame);

		imageFileNameWithPath.str(""); 
		imageFileNameWithPath << inputDataDir << "/" << inputBitmapPrefix << std::setfill('0') << std::setw(4) << ++frameNumber << inputBitmapSuffix << ".bmp"; 
	}

	PrintProgressBar(100); 

	SaveImages(trackedFrameContainer, savingMethod, numberOfImagesWritten); 

	for ( int i = 0; i < trackedFrameContainer.size(); i++)
	{
		trackedFrameContainer[i].ImageData->UnRegister(); 
	}
	trackedFrameContainer.clear(); 
}

//-------------------------------------------------------------------------------
void SaveImages( TrackedFrameList &trackedFrameList, SAVING_METHOD savingMethod, int numberOfImagesWritten)
{
	const int numberOfFrames = trackedFrameList.size(); 

	switch ( savingMethod ) 
	{
	case BMP24:
	case BMP8:
	case JPG:
	case PNG:
		{
			if ( numberOfFrames > 1 )
			{
				LOG_INFO("Saving images and transforms..."); 
			}

			for ( int imgNumber = 0; imgNumber < numberOfFrames; imgNumber++ )
			{
				if ( numberOfFrames > 1 )
				{
					PrintProgressBar(imgNumber*100 / numberOfFrames ); 
				}

				std::ostringstream fileName; 
				fileName << outputFolder << "/" << inputBitmapPrefix << std::setfill('0') << std::setw(4) << numberOfImagesWritten + imgNumber << inputBitmapSuffix; 

				if ( savingMethod == BMP8 || savingMethod == BMP24 ) 
				{
					fileName << ".bmp"; 					
				}
				else if ( savingMethod == JPG ) 
				{
					fileName << ".jpg"; 					
				}
				else if ( savingMethod == PNG ) 
				{
					fileName << ".png"; 					
				}

				SaveImageToBitmap(trackedFrameList[imgNumber].ImageData, fileName.str(), savingMethod); 
				SaveTransformToFile(trackedFrameList[imgNumber], fileName.str()); 

			} 

			if ( numberOfFrames > 1 )
			{
				PrintProgressBar(100); 
			}
		}
		break; 
	case METAFILE:
		{
			if ( numberOfFrames > 1 )
			{
				LOG_INFO("Saving metafiles..."); 
			}

			for ( int imgNumber = 0; imgNumber < numberOfFrames; imgNumber++ )
			{
				if ( numberOfFrames > 1 )
				{
					PrintProgressBar(imgNumber*100 / numberOfFrames ); 
				}

				std::ostringstream fileName; 
				fileName << outputFolder << "/Frame" << std::setfill('0') << std::setw(4) << numberOfImagesWritten + imgNumber << ".mha" << std::ends;
        
				SaveImageToMetaFile(trackedFrameList[imgNumber], fileName.str(), inputUseCompression ); 
			}

			if ( numberOfFrames > 1 )
			{
				PrintProgressBar(100); 
			}
		}
		break; 
	case SEQUENCE_METAFILE:
		{
			if ( numberOfFrames > 1 )
			{
				LOG_INFO("Saving sequence meta file..."); 
			}

			std::ostringstream sequenceDataFileName; 
			// sequenceDataFileName << outputFolder << "/" << outputSequenceFileName << std::setfill('0') << std::setw(2) << numberOfImagesWritten << ".mha" << std::ends; 
			sequenceDataFileName << outputFolder << "/" << outputSequenceFileName << std::ends; 
			SaveImageToSequenceMetaFile(trackedFrameList, sequenceDataFileName.str(), inputUseCompression); 
		}
	}

}


//-------------------------------------------------------------------------------
void SaveImageToSequenceMetaFile( TrackedFrameList &trackedFrameList, std::string sequenceDataFileName, bool useCompression ) 
{
	const int numberOfFrames = trackedFrameList.size(); 
	if ( numberOfFrames == 0 )
	{
		LOG_WARNING("There is no frame to write into sequence meta file!"); 
		return; 
	}

	const unsigned long imageWidthInPixels = trackedFrameList[0].ImageData->GetLargestPossibleRegion().GetSize()[0]; 
	const unsigned long imageHeightInPixels = trackedFrameList[0].ImageData->GetLargestPossibleRegion().GetSize()[1]; 

	ImageSequenceType::Pointer imageDataSequence = ImageSequenceType::New();
	ImageSequenceType::SizeType size = {imageWidthInPixels, imageHeightInPixels, numberOfFrames };
	ImageSequenceType::IndexType start = {0,0,0};
	ImageSequenceType::RegionType region;
	region.SetSize(size);
	region.SetIndex(start);
	imageDataSequence->SetRegions(region);
	imageDataSequence->Allocate();

	itk::MetaImageSequenceIO::Pointer writerMetaImageSequenceIO = itk::MetaImageSequenceIO::New();

	PixelType* imageData = imageDataSequence->GetBufferPointer(); // pointer to the image pixel buffer

	unsigned int frameSizeInBytes=imageWidthInPixels*imageHeightInPixels*sizeof(PixelType);
	
	for ( int i = 0 ; i < numberOfFrames; i++ ) 
	{
		if ( numberOfFrames > 1 )
		{
			PrintProgressBar(i*100 / numberOfFrames ); 
		}

		PixelType *currentFrameImageData = imageData + i * frameSizeInBytes;
		memcpy(currentFrameImageData, trackedFrameList[i].ImageData->GetBufferPointer(), frameSizeInBytes); 
		
		// Write custom fields only once 
		if ( i == 0 )
		{
			writerMetaImageSequenceIO->SetDefaultFrameTransformName( trackedFrameList[i].DefaultFrameTransformName ); 

			for( int field = 0; field < trackedFrameList[i].CustomFieldList.size(); field++ )
			{
				writerMetaImageSequenceIO->SetCustomString(trackedFrameList[i].CustomFieldList[field].first.c_str(), trackedFrameList[i].CustomFieldList[field].second.c_str() ); 
			}
		}

		for( int field = 0; field < trackedFrameList[i].CustomFrameFieldList.size(); field++ )
		{
			writerMetaImageSequenceIO->SetCustomFrameString(i, trackedFrameList[i].CustomFrameFieldList[field].first.c_str(), trackedFrameList[i].CustomFrameFieldList[field].second.c_str() ); 
		}
	}

	if ( numberOfFrames > 1 )
	{
		PrintProgressBar(100); 
	}

	ImageSequenceWriterType::Pointer writer = ImageSequenceWriterType::New(); 

	writer->SetFileName(sequenceDataFileName);
	writer->SetInput(imageDataSequence); 
	writer->SetImageIO(writerMetaImageSequenceIO); 
	writer->SetUseCompression(useCompression); 

	try
	{
		writer->Update(); 
	}
	catch (itk::ExceptionObject & err) 
	{		
		LOG_ERROR(" Unable to update sequence writer: " << err);
	}	

}


//-------------------------------------------------------------------------------
void SaveImageToBitmap( ImageType* image, std::string bitmapFileName, int savingMethod) 
{
	if ( savingMethod == BMP24 )
	{
		const unsigned long imageWidthInPixels = image->GetLargestPossibleRegion().GetSize()[0]; 
		const unsigned long imageHeightInPixels = image->GetLargestPossibleRegion().GetSize()[1]; 

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
		composeRGB->SetInput1( image );
		composeRGB->SetInput2( image );
		composeRGB->SetInput3( image );
		composeRGB->Update(); 

		RGBImageWriterType::Pointer writer = RGBImageWriterType::New();
		writer->SetFileName( bitmapFileName.c_str() ); 
		writer->SetInput( composeRGB->GetOutput() ); 

		try    
		{
			writer->Update();
		}	
		catch (itk::ExceptionObject & e)
		{
			LOG_ERROR(e.GetDescription());
		}
	}
	else
	{
		ImageWriterType::Pointer writer = ImageWriterType::New();
		writer->SetFileName( bitmapFileName.c_str() ); 
		writer->SetInput( image ); 

		try    
		{
			writer->Update();
		}	
		catch (itk::ExceptionObject & e)
		{
			LOG_ERROR(e.GetDescription());
		}
	}
}


//-------------------------------------------------------------------------------
void SaveTransformToFile(TrackedFrame trackedFrame, std::string imageFileName)
{
	// Find default frame transform 
    std::vector<CustomFrameFieldPair>::iterator defaultFrameTransform; 
	for ( defaultFrameTransform = trackedFrame.CustomFrameFieldList.begin(); defaultFrameTransform != trackedFrame.CustomFrameFieldList.end(); defaultFrameTransform++ )
	{
		if ( defaultFrameTransform->first.find(trackedFrame.DefaultFrameTransformName) != std::string::npos )
		{
			break; 
		}
	}
	
	if ( defaultFrameTransform == trackedFrame.CustomFrameFieldList.end() )
	{
		LOG_ERROR("Unable to find default transform in sequence metafile!"); 
		return; 
	}

	std::istringstream transformFieldValue(defaultFrameTransform->second); 

	double item, transform[16]; 
	int i = 0; 
	while ( transformFieldValue >> item )
	{
		transform[i++] = item; 
	}

	vtkSmartPointer<vtkMatrix4x4> transMatrix = vtkSmartPointer<vtkMatrix4x4>::New(); 
	transMatrix->DeepCopy(transform); 

	std::ostringstream transformFileName; 
	transformFileName << imageFileName << inputTransformSuffix << std::ends; 
	vtkstd::string transformData; 
	transformData+= "# ================================ #\n";
	transformData+= "# Transform Data of Captured Image #\n";
	transformData+= "# ================================ #\n";
	transformData+= "# THIS FILE CONTAINS THE REAL-TIME TRANSFORM DATA FOR THE CAPTURED IMAGE.\n";
	transformData+= "# DATA IS RECORDED IN THE FOLLOWING FORMAT:\n";
	transformData+= "# [FORMAT: Angle - in Degrees, Qx, Qy, Qz, Position(x, y, z)]\n";
	transformData+= "# THIS FILE IS AUTO-GENERATED BY THE PROGRAM.  DO NOT EDIT!\n";
	transformData+= "\n";
	transformData+= "# NAME OF THE CAPTURED IMAGE WITH PATH\n";
	transformData+= imageFileName;
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


//-------------------------------------------------------------------------------
void SaveImageToMetaFile( TrackedFrame trackedFrame, std::string metaFileName, bool useCompression)
{
	ImageType* image = trackedFrame.ImageData; 

	const unsigned long imageWidthInPixels = image->GetLargestPossibleRegion().GetSize()[0]; 
	const unsigned long imageHeightInPixels = image->GetLargestPossibleRegion().GetSize()[1]; 
	unsigned int frameSizeInBytes = imageWidthInPixels * imageHeightInPixels * sizeof(PixelType);

	ImageType3D::Pointer frame = ImageType3D::New(); 
	ImageType3D::SizeType size = {imageWidthInPixels, imageHeightInPixels, 1 };
	ImageType3D::IndexType start = {0,0,0};
	ImageType3D::RegionType region;
	region.SetSize(size);
	region.SetIndex(start);
	frame->SetRegions(region);
	frame->Allocate();

	memcpy(frame->GetBufferPointer() , image->GetBufferPointer() , frameSizeInBytes);

	// Find default frame transform 
    std::vector<CustomFrameFieldPair>::iterator defaultFrameTransform; 
	for ( defaultFrameTransform = trackedFrame.CustomFrameFieldList.begin(); defaultFrameTransform != trackedFrame.CustomFrameFieldList.end(); defaultFrameTransform++ )
	{
		if ( defaultFrameTransform->first.find(trackedFrame.DefaultFrameTransformName) != std::string::npos )
		{
			break; 
		}
	}
	
	if ( defaultFrameTransform == trackedFrame.CustomFrameFieldList.end() )
	{
		LOG_ERROR("Unable to find default transform in sequence metafile!"); 
		return; 
	}

	std::istringstream transformFieldValue(defaultFrameTransform->second); 

	double item, transformMatrix[16]; 
	int i = 0; 
	while ( transformFieldValue >> item )
	{
		transformMatrix[i++] = item; 
	}

	double *offset = new double[3]; 
	double *transform = new double[9]; 

	transform[0] = transformMatrix[0]; 
	transform[1] = transformMatrix[1];
	transform[2] = transformMatrix[2];
	offset[0] = transformMatrix[3];

	transform[3] = transformMatrix[4]; 
	transform[4] = transformMatrix[5];
	transform[5] = transformMatrix[6];
	offset[1] = transformMatrix[7];

	transform[6] = transformMatrix[8]; 
	transform[7] = transformMatrix[9];
	transform[8] = transformMatrix[10];
	offset[2] = transformMatrix[11];

	itk::MetaImageIO::Pointer writerMetaImageIO = itk::MetaImageIO::New(); 
	ImageWriterType3D::Pointer writer = ImageWriterType3D::New(); 

	writer->SetFileName(metaFileName.c_str());
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
	writer->SetUseCompression(useCompression); 

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


//-------------------------------------------------------------------------------
void ReadTransformFile( const std::string TransformFileNameWithPath, double* transformUSProbe2StepperFrame )
{
	try
	{
		std::ifstream TransformsFile( TransformFileNameWithPath.c_str(), ios::in );
		if( !TransformsFile.is_open() )
		{
			std::cerr << "\n\n" << __FILE__ << "," << __LINE__ << "\n"
				<< ">>>>>>>> Failed to open the position/transform file: "
				<< TransformFileNameWithPath << "!!!  This file should be associated with and "
				<< "located at the same directory as that of the corresponding image file.  Throw up ...\n";

			throw;
		}

		std::string SectionName("");
		std::string ThisConfiguration("");

		// Start from the beginning of the file
		TransformsFile.seekg( 0, ios::beg );

		ThisConfiguration = "TRANSFORM_HOMOGENEOUS4x4_USPROBE_TO_STEPPER_FRAME]";
		while ( TransformsFile.eof() != true && SectionName != ThisConfiguration )
		{
			TransformsFile.ignore(1024, '[');
			TransformsFile >> SectionName;
		}
		if(  SectionName != ThisConfiguration )
		{	// If the designated configuration is not found, throw up error
			std::cerr << "\n\n" << __FILE__ << "," << __LINE__ << "\n"
				<< ">>>>>>>> CANNOT find the input section named: [" << ThisConfiguration 
				<< " in the transform file!!!  Throw up ...\n";
			TransformsFile.close();
			throw;
		}
		TransformsFile.ignore(1024, '\n');
		vnl_matrix<double> matrix4x4(4,4); 
		TransformsFile >> matrix4x4;

		matrix4x4.copy_out(transformUSProbe2StepperFrame);

		// Close the file for reading
		TransformsFile.close();

	}
	catch(...)
	{
		std::cerr << "\n\n" << __FILE__ << "," << __LINE__ << "\n"
			<< ">>>>>>>> Failed to read in the transform file!!!"
			<< "  Throw up ...\n";

		throw;	
	}
}


//-------------------------------------------------------------------------------
void ReadDRBTransformFile( const std::string TransformFileNameWithPath, std::vector<CustomFrameFieldPair> &customFrameFieldList, std::string &defaultFrameTransformName )
{
	std::ifstream TransformsFile( TransformFileNameWithPath.c_str(), ios::in );
	if( !TransformsFile.is_open() )
	{
		LOG_ERROR ("Failed to open the position/transform file: " << TransformFileNameWithPath ); 
		exit(EXIT_FAILURE); 
	}


	//********** Read the probe-to-tracker transform parameters from file *******
	
	// Note: In old transform file, the first four rotations parameters are
	// called quaternions, although actually they are
	// [ rotation angle, axis x, axis y, axis z ] parameters.
	
	std::string SectionName("");
	std::string ThisConfiguration("");

	// Start from the beginning of the file
	TransformsFile.seekg( 0, ios::beg );

	// # TRANSFORM: FROM THE US PROBE FRAME TO THE TRACKER FRAME
	ThisConfiguration = "TRANSFORM:";
	while ( TransformsFile.eof() != true && SectionName != ThisConfiguration )
	{
		TransformsFile.ignore(1024, '#');
		TransformsFile >> SectionName;
	}
	if(  SectionName != ThisConfiguration )
	{	// If the designated configuration is not found, throw error
		LOG_ERROR ( "CANNOT find the input section named: [" << ThisConfiguration << " in the transform file!!! ");
		TransformsFile.close();
		exit(EXIT_FAILURE); 
	}

	TransformsFile.ignore(1024, '\n');
	vnl_vector<double> pToolToTracker(7); 
	TransformsFile >> pToolToTracker;
  
  
  
	// # TRANSFORM: FROM THE DRB REFERENCE FRAME TO THE TRACKER FRAME
	SectionName= ""; 
	ThisConfiguration = "TRANSFORM:";
	while ( TransformsFile.eof() != true && SectionName != ThisConfiguration )
	{
		TransformsFile.ignore(1024, '#');
		TransformsFile >> SectionName;
	}
	if(  SectionName != ThisConfiguration )
	{	// If the designated configuration is not found, throw up error
		LOG_ERROR ( "CANNOT find the input section named: [" << ThisConfiguration << " in the transform file!!! ");
		TransformsFile.close();
		exit(EXIT_FAILURE); 
	}
	TransformsFile.ignore(1024, '\n');
	vnl_vector<double> pReferenceToTracker( 7 );
	TransformsFile >> pReferenceToTracker;

	// Close the file for reading
	TransformsFile.close();
  
  
	//***********************************************************************
  
	// Convert transform parameters to 4x4 matrix.
	// Note: first four parameters are not really quaternions!
	
	// FROM THE US PROBE FRAME TO THE TRACKER FRAME
	
	vtkSmartPointer< vtkTransform > tToolToTracker = vtkSmartPointer< vtkTransform >::New();
    tToolToTracker->PostMultiply(); // Transform order as written.
    tToolToTracker->Identity();
    tToolToTracker->RotateWXYZ( pToolToTracker[ 0 ], pToolToTracker[ 1 ],
                                pToolToTracker[ 2 ], pToolToTracker[ 3 ] );
      
      // Convert from meters to millimeters.
      // .transforms files use meters. Sequence metafiles uses millimeters.
    
    tToolToTracker->Translate( pToolToTracker[ 4 ] * 1000.0, pToolToTracker[ 5 ] * 1000.0, pToolToTracker[ 6 ] * 1000.0 );
    tToolToTracker->Update();
  
  
  double dToolToTracker[ 16 ]; 
  vtkMatrix4x4::DeepCopy( dToolToTracker, tToolToTracker->GetMatrix() ); 
  
  std::ostringstream strToolToTracker;
  for ( int i = 0; i < 16; ++ i ) strToolToTracker << dToolToTracker[ i ] << " ";
	
  
  CustomFrameFieldPair toolToTrackerField; 
    toolToTrackerField.first  = "ToolToTrackerTransform"; 
    toolToTrackerField.second = strToolToTracker.str(); 
  
  customFrameFieldList.push_back( toolToTrackerField ); 
	
	
	  // FROM THE DRB REFERENCE FRAME TO THE TRACKER FRAME
	
	vtkSmartPointer< vtkTransform > tReferenceToTracker = vtkSmartPointer< vtkTransform >::New();
	  tReferenceToTracker->PostMultiply();
	  tReferenceToTracker->Identity();
	
	  // All zeros indicate (in some files) that the transform is not given.
	
	if ( pReferenceToTracker[ 0 ] != 0.0 || pReferenceToTracker[ 1 ] != 0 ||
	     pReferenceToTracker[ 2 ] != 0.0 || pReferenceToTracker[ 3 ] != 0 )
	  {
	  tReferenceToTracker->RotateWXYZ( pReferenceToTracker[ 0 ], pReferenceToTracker[ 1 ],
	                                   pReferenceToTracker[ 2 ], pReferenceToTracker[ 3 ] );
	    
      // Convert from meters to millimeters.
      // .transforms files use meters. Sequence metafiles uses millimeters.
    
    tReferenceToTracker->Translate( pReferenceToTracker[ 4 ] * 1000.0,
	                                  pReferenceToTracker[ 5 ] * 1000.0,
	                                  pReferenceToTracker[ 6 ] * 1000.0 );
	  }
	
	tReferenceToTracker->Update();
  
  
  std::ostringstream strReferenceToTracker; 
  double dReferenceToTracker[ 16 ];
  vtkMatrix4x4::DeepCopy( dReferenceToTracker, tReferenceToTracker->GetMatrix() );
  for ( int i = 0; i < 16; ++ i ) strReferenceToTracker << dReferenceToTracker[ i ] << " ";
  
  
  CustomFrameFieldPair referenceToTrackerField; 
    referenceToTrackerField.first  = "ReferenceToTrackerTransform"; 
    referenceToTrackerField.second = strReferenceToTracker.str(); 
  
  customFrameFieldList.push_back( referenceToTrackerField );
  
  
	  // tToolToReference = inv( tReferenceToTracker ) * tProbeToTracker
	  // This is matrix multiplication. In post-multiply mode, the * order is reversed.
	
	vtkSmartPointer< vtkTransform > tTrackerToReference = vtkSmartPointer< vtkTransform >::New();
	  tTrackerToReference->PostMultiply();
	  tTrackerToReference->SetInput( tReferenceToTracker );
	  tTrackerToReference->Inverse();
	  tTrackerToReference->Update();
	
	vtkSmartPointer< vtkTransform > tToolToReference = vtkSmartPointer< vtkTransform >::New(); 
	  tToolToReference->PostMultiply();
	  tToolToReference->Identity();
	  tToolToReference->Concatenate( tToolToTracker );
	  tToolToReference->Concatenate( tTrackerToReference);
	  tToolToReference->Update();
	
	
	// Copy transform to the custom frame field list
  
  double dToolToReference[ 16 ]; 
  vtkMatrix4x4::DeepCopy( dToolToReference, tToolToReference->GetMatrix() ); 
  
  std::ostringstream strToolToReference; 
  for ( int i = 0; i < 16; ++ i ) strToolToReference << dToolToReference[ i ] << " ";
  
  CustomFrameFieldPair toolToReferenceField; 
    toolToReferenceField.first = "ToolToReferenceTransform"; 
    toolToReferenceField.second = strToolToReference.str(); 
  
  customFrameFieldList.push_back( toolToReferenceField );
  
  defaultFrameTransformName = "ToolToReferenceTransform"; 
}


//-------------------------------------------------------------------------------
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

	if ( percent == 100)
	{
		std::cout << std::endl << std::endl;
	}
}

