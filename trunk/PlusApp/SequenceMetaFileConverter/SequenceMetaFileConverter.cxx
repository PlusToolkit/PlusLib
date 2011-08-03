#include "PlusConfigure.h"
#include <iostream>

#include "vtksys/CommandLineArguments.hxx" 
#include "vtksys/SystemTools.hxx" 
#include "vtkDirectory.h"
#include "vtkMatrix4x4.h"
#include "vtkTransform.h"
#include "vtkSmartPointer.h"
#include "vtkMath.h"
#include "vtkTrackedFrameList.h"

#include "itkMetaImageSequenceIO.h"
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkRGBPixel.h"
#include "itkComposeRGBImageFilter.h"
#include "itkRGBToLuminanceImageFilter.h"
#include "itkFlipImageFilter.h"

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

void SaveImages( vtkTrackedFrameList* trackedFrameList, SAVING_METHOD savingMethod, int numberOfImagesWritten ); 
void SaveImageToMetaFile( TrackedFrame* trackedFrame, std::string metaFileName, bool useCompression ); 
void SaveImageToBitmap( const ImageType::Pointer& image, std::string bitmapFileName, int savingMethod ); 

void SaveTransformToFile(TrackedFrame* trackedFrame, std::string imageFileName, std::string toolToReferenceTransformName, std::string referenceToTrackerTransformName); 

// convert internal MF oriented image into the desired image orientation 
PlusStatus GetOrientedImage( const ImageType::Pointer& inMFOrientedImage, US_IMAGE_ORIENTATION desiredUsImageOrientation, ImageType::Pointer& outOrientedImage ); 

void ReadTransformFile( const std::string TransformFileNameWithPath, double* transformUSProbe2StepperFrame ); 
void ReadDRBTransformFile( const std::string TransformFileNameWithPath, TrackedFrame* trackedFrame); 

std::string inputDataDir;
std::string inputBitmapPrefix("CapturedImageID_NO_"); 
std::string inputBitmapSuffix(""); 
std::string inputTransformSuffix(".transforms");
std::string outputSequenceFileName("SeqMetafile");
std::string outputFolder("./");
std::string inputUsImageOrientation("XX"); 
std::string outputUsImageOrientation("XX");
bool inputUseCompression(false); 
bool inputNoImageData(false); 
std::string inputToolToReferenceName, inputReferenceToTrackerName; 


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
	
	cmdargs.AddArgument("--saving-method", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputSavingMethod, "Saving method ( Default: SEQUENCE_METAFILE; METAFILE, SEQUENCE_METAFILE, BMP24, BMP8, PNG, JPG)" );
	cmdargs.AddArgument("--convert-method", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputConvertMethod, "Convert method ( Default: FROM_BMP24; FROM_BMP24, FROM_METAFILE, FROM_OLD_SEQUENCE_METAFILE, FROM_SEQUENCE_METAFILE)" );
	cmdargs.AddArgument("--output-us-img-orientation", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &outputUsImageOrientation, "Output ultrasound image orientation ( Default: XX; UF, UN, MF, MN, XX)" );

	// convert from BMP24 arguments
	cmdargs.AddArgument("--input-data-dir", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputDataDir, "Input data directory for image files with transforms (default: ./)");

	// convert from SEQUENCE_METAFILE arguments
	cmdargs.AddArgument("--input-img-seq-file-names", vtksys::CommandLineArguments::MULTI_ARGUMENT, &inputImageSequenceFileNames, "Filenames of meta image sequences (e.g. sequence_1.mhd sequence_2.mhd).");
	
	// convert from FROM_BMP24 and saving to BMP24, BMP8, PNG, JPG arguments
	cmdargs.AddArgument("--input-bitmap-prefix", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputBitmapPrefix, "Prefix of bitmap images (default: CapturedImageID_NO_).");
	cmdargs.AddArgument("--input-bitmap-suffix", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputBitmapSuffix, "Suffix of bitmap images.");
	cmdargs.AddArgument("--input-transform-suffix", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputTransformSuffix, "Suffix of transform files (default: .transforms).");
	cmdargs.AddArgument("--input-us-img-orientation", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputUsImageOrientation, "Input ultrasound image orientation. NOTE: SEQUENCE_METAFILE has it's own image orientation flag ( Default: XX; UF, UN, MF, MN, XX)" );
	
	// Saving to BMP24, BMP8, PNG, JPG arguments
	cmdargs.AddArgument("--tool-to-reference-name", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputToolToReferenceName, "Tool to reference transform name in sequence metafile (e.g. ToolToReference)");
	cmdargs.AddArgument("--reference-to-tracker-name", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputReferenceToTrackerName, "Reference to tracker transform name in sequence metafile (e.g. ReferenceToTracker)");

	// Saving to SEQUENCE_METAFILE arguments
	cmdargs.AddArgument("--output-sequence-file-name", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &outputSequenceFileName, "Output sequence file name of saving method SEQUENCE_METAFILE. (Default: SeqMetafile)");
	cmdargs.AddArgument("--use-compression", vtksys::CommandLineArguments::NO_ARGUMENT, &inputUseCompression, "Compress metafile and sequence metafile images.");	
    cmdargs.AddArgument("--no-image-data", vtksys::CommandLineArguments::NO_ARGUMENT, &inputNoImageData, "Save sequence metafile without image data.");	
    

	cmdargs.AddArgument("--output-folder", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &outputFolder, "Path to the output folder where to save the converted files (Default: ./Output).");
	cmdargs.AddArgument("--verbose", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &verboseLevel, "Verbose level (Default: INFO; ERROR, WARNING, INFO, DEBUG)");	

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

    if ( STRCASECMP("ERROR", verboseLevel.c_str())==0 )
	{
		PlusLogger::Instance()->SetLogLevel(PlusLogger::LOG_LEVEL_ERROR);
        PlusLogger::Instance()->SetDisplayLogLevel(PlusLogger::LOG_LEVEL_ERROR);
	}
	else if ( STRCASECMP("WARNING", verboseLevel.c_str())==0 )
	{
		PlusLogger::Instance()->SetLogLevel(PlusLogger::LOG_LEVEL_WARNING);
        PlusLogger::Instance()->SetDisplayLogLevel(PlusLogger::LOG_LEVEL_WARNING);
	}
	else if ( STRCASECMP("INFO", verboseLevel.c_str())==0 )
	{
		PlusLogger::Instance()->SetLogLevel(PlusLogger::LOG_LEVEL_INFO);
        PlusLogger::Instance()->SetDisplayLogLevel(PlusLogger::LOG_LEVEL_INFO);
	}
	else if ( STRCASECMP("DEBUG", verboseLevel.c_str())==0 )
	{
		PlusLogger::Instance()->SetLogLevel(PlusLogger::LOG_LEVEL_DEBUG);
        PlusLogger::Instance()->SetDisplayLogLevel(PlusLogger::LOG_LEVEL_DEBUG);
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


	if ( convertMethod == FROM_BMP24 )
	{
		if ( inputDataDir.empty() )
		{
			LOG_ERROR("Need to set input-data-dir argument to convert from BMP24"); 
			exit(EXIT_FAILURE); 
		}

        if ( inputUsImageOrientation == "XX" )
        {
            LOG_ERROR("Need to set input-us-img-orientation argument to convert from BMP24"); 
            exit(EXIT_FAILURE);
        }
	}

	if ( convertMethod == FROM_SEQUENCE_METAFILE )
	{
		if ( inputImageSequenceFileNames.size() == 0 )
		{
			LOG_ERROR("Need to set input-img-seq-file-names argument to convert from SEQUENCE_METAFILE"); 
			exit(EXIT_FAILURE); 
		}
	}
	

	if ( savingMethod == BMP24 || savingMethod == BMP8 || savingMethod == PNG || savingMethod == JPG )
	{
		if ( inputToolToReferenceName.empty() )
		{
			LOG_ERROR("Need to set tool-to-reference-name argument for to BMP24, BMP8, PNG, JPG"); 
			exit(EXIT_FAILURE); 
		}

		if ( inputReferenceToTrackerName.empty() )
		{
			LOG_ERROR("Need to set reference-to-tracker-name argument for to BMP24, BMP8, PNG, JPG"); 
			exit(EXIT_FAILURE); 
		}

	}

	vtkSmartPointer<vtkDirectory> dir = vtkSmartPointer<vtkDirectory>::New(); 
	if ( dir->Open(outputFolder.c_str()) == 0 ) 
	{	
		dir->MakeDirectory(outputFolder.c_str()); 
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
	vtkSmartPointer<vtkTrackedFrameList> trackedFrameContainer = vtkSmartPointer<vtkTrackedFrameList>::New(); 
	int numberOfImagesWritten(0); 

	for ( int i = 0; i < inputImageSequenceFileNames.size(); i++) 
	{
			trackedFrameContainer->ReadFromSequenceMetafile(inputImageSequenceFileNames[i].c_str()); 
			SaveImages(trackedFrameContainer, savingMethod, numberOfImagesWritten++); 
	}
}

//-------------------------------------------------------------------------------
void ConvertFromOldSequenceMetafile(std::vector<std::string> inputImageSequenceFileNames, SAVING_METHOD savingMethod)
{
	LOG_INFO("Converting old sequence metafile images..."); 
	vtkSmartPointer<vtkTrackedFrameList> trackedFrameContainer = vtkSmartPointer<vtkTrackedFrameList>::New(); 
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
            if ( inputUsImageOrientation == "XX" )
            {
                LOG_ERROR("Failed to convert frame from old sequence metafile without proper image orientation! Please set the --input-us-img-orientation partameter!"); 
                exit(EXIT_FAILURE);
            }

			PlusLogger::PrintProgressbar( (100.0 * imgNumber) / numberOfFrames ); 

			TrackedFrame::ImageType::Pointer frame = TrackedFrame::ImageType::New(); 
			TrackedFrame::ImageType::SizeType size = {imageWidthInPixels, imageHeightInPixels};
			TrackedFrame::ImageType::IndexType start = {0,0};
			TrackedFrame::ImageType::RegionType region;
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

			// Copy offset and transform value to the default transform matrix
			std::string defaultFrameTransformName = "ToolToTrackerTransform"; 
			double defaultTransformMatrix[16] = {
				transform[0], transform[1], transform[2], offset[0], 
				transform[3], transform[4], transform[5], offset[1], 
				transform[6], transform[7], transform[8], offset[2], 
				0, 0, 0, 1
			}; 

            ImageType::Pointer mfOrientedImage = ImageType::New(); 
            US_IMAGE_ORIENTATION imgOrientation = UsImageConverterCommon::GetUsImageOrientationFromString(inputUsImageOrientation.c_str()); 
            if ( UsImageConverterCommon::GetMFOrientedImage(frame, imgOrientation, mfOrientedImage) != PLUS_SUCCESS )
            {
                LOG_ERROR("Failed to get MF oriented image from " << inputUsImageOrientation << " orientation!"); 
                exit(EXIT_FAILURE);
            }

			// Create tracked frame struct
			TrackedFrame trackedFrame;
			trackedFrame.ImageData = mfOrientedImage;
			trackedFrame.SetCustomFrameTransform(defaultFrameTransformName, defaultTransformMatrix); 
			trackedFrame.DefaultFrameTransformName = defaultFrameTransformName; 

			trackedFrameContainer->AddTrackedFrame(&trackedFrame);
		}

		PlusLogger::PrintProgressbar(100); 

		SaveImages(trackedFrameContainer, savingMethod, ++numberOfImagesWritten); 
	}
}

//-------------------------------------------------------------------------------
void ConvertFromBitmap(SAVING_METHOD savingMethod)
{
	LOG_INFO("Converting bitmap images..."); 
	vtkSmartPointer<vtkTrackedFrameList> trackedFrameContainer = vtkSmartPointer<vtkTrackedFrameList>::New(); 
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

		PlusLogger::PrintProgressbar(frameNumber*100 / totalNumberOfImages ); 

		std::string fileName( dir->GetFile( dirIndex ) );
		std::size_t pos = 0;
		pos = fileName.rfind( ".bmp" );
		if ( pos != fileName.length() - 4 ) continue;


		imageFileNameWithPath.str( "" );
		imageFileNameWithPath << inputDataDir << "/" << fileName;

        if ( inputUsImageOrientation == "XX" )
        {
            LOG_ERROR("Failed to convert frame from bitmap without proper image orientation! Please set the --input-us-img-orientation partameter (" << imageFileNameWithPath.str() << ")!"); 
            exit(EXIT_FAILURE);
        }

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



        ImageType::Pointer mfOrientedImage = ImageType::New(); 
        US_IMAGE_ORIENTATION imgOrientation = UsImageConverterCommon::GetUsImageOrientationFromString(inputUsImageOrientation.c_str()); 
        if ( UsImageConverterCommon::GetMFOrientedImage(imageData, imgOrientation, mfOrientedImage) != PLUS_SUCCESS )
        {
            LOG_ERROR("Failed to get MF oriented image from " << inputUsImageOrientation << " orientation!"); 
            exit(EXIT_FAILURE);
        }
		
		TrackedFrame trackedFrame;
		trackedFrame.ImageData = mfOrientedImage;
		ReadDRBTransformFile( transformFileNameWithPath.str(), &trackedFrame); 
		trackedFrameContainer->AddTrackedFrame(&trackedFrame);

		imageFileNameWithPath.str(""); 
		imageFileNameWithPath << inputDataDir << "/" << inputBitmapPrefix << std::setfill('0') << std::setw(4) << ++frameNumber << inputBitmapSuffix << ".bmp"; 
	}

	PlusLogger::PrintProgressbar(100); 

	SaveImages(trackedFrameContainer, savingMethod, numberOfImagesWritten); 
}

//-------------------------------------------------------------------------------
void SaveImages( vtkTrackedFrameList* trackedFrameList, SAVING_METHOD savingMethod, int numberOfImagesWritten)
{
	const int numberOfFrames = trackedFrameList->GetNumberOfTrackedFrames(); 

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
					PlusLogger::PrintProgressbar(imgNumber*100 / numberOfFrames ); 
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

                // Convert the internal MF oriented image into the desired image orientation 
                ImageType::Pointer orientedImage = ImageType::New(); 
                US_IMAGE_ORIENTATION desiredOrientation = UsImageConverterCommon::GetUsImageOrientationFromString( outputUsImageOrientation.c_str() ); 
                if ( GetOrientedImage(trackedFrameList->GetTrackedFrame(imgNumber)->ImageData, desiredOrientation, orientedImage) != PLUS_SUCCESS )
                {
                    LOG_ERROR("Failed to get " << outputUsImageOrientation << " oriented image from MF orientation!"); 
                    exit(EXIT_FAILURE);
                }

				SaveImageToBitmap(orientedImage, fileName.str(), savingMethod); 
				SaveTransformToFile(trackedFrameList->GetTrackedFrame(imgNumber), fileName.str(), inputToolToReferenceName, inputReferenceToTrackerName); 
			} 

			if ( numberOfFrames > 1 )
			{
				PlusLogger::PrintProgressbar(100); 
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
					PlusLogger::PrintProgressbar(imgNumber*100 / numberOfFrames ); 
				}

				std::ostringstream fileName; 
				fileName << outputFolder << "/Frame" << std::setfill('0') << std::setw(4) << numberOfImagesWritten + imgNumber << ".mha" << std::ends;
        
				SaveImageToMetaFile(trackedFrameList->GetTrackedFrame(imgNumber), fileName.str(), inputUseCompression ); 
			}

			if ( numberOfFrames > 1 )
			{
				PlusLogger::PrintProgressbar(100); 
			}
		}
		break; 
	case SEQUENCE_METAFILE:
		{
      if ( numberOfFrames > 1 )
      {
        LOG_INFO("Saving sequence meta file..."); 
      }

      if ( inputNoImageData ) 
      {
        for ( int i = 0; i < numberOfFrames; ++i )
        {
          if ( trackedFrameList->GetTrackedFrame(i)->ImageData.IsNotNull() )
          {
            trackedFrameList->GetTrackedFrame(i)->ImageData = NULL; 
          }
        }
      }

      std::string fileName = vtksys::SystemTools::GetFilenameWithoutLastExtension(outputSequenceFileName); 
      std::string extension = vtksys::SystemTools::GetFilenameLastExtension(outputSequenceFileName); 
      
      vtkTrackedFrameList::SEQ_METAFILE_EXTENSION metaExtension = vtkTrackedFrameList::SEQ_METAFILE_MHA; 
      if ( STRCASECMP(".mha", extension.c_str() ) == 0 )
      {
        metaExtension = vtkTrackedFrameList::SEQ_METAFILE_MHA; 
      }
      else if ( STRCASECMP(".mhd", extension.c_str() ) == 0 )
      {
        metaExtension = vtkTrackedFrameList::SEQ_METAFILE_MHD; 
      }
       
      if ( trackedFrameList->SaveToSequenceMetafile(outputFolder.c_str(), fileName.c_str(), metaExtension, inputUseCompression) != PLUS_SUCCESS )
      {
        LOG_ERROR("Failed to save tracked frames to sequence metafile!"); 
        return; 
      }
    }
  }
}

//-------------------------------------------------------------------------------
void SaveImageToBitmap( const ImageType::Pointer& image, std::string bitmapFileName, int savingMethod) 
{
	if ( savingMethod == BMP24 )
	{
		const unsigned long imageWidthInPixels = image->GetLargestPossibleRegion().GetSize()[0]; 
		const unsigned long imageHeightInPixels = image->GetLargestPossibleRegion().GetSize()[1]; 

		LOG_INFO("imageWidthInPixels: " << imageWidthInPixels << "    imageHeightInPixels: " << imageHeightInPixels); 

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
void SaveTransformToFile(TrackedFrame* trackedFrame, std::string imageFileName, std::string toolToReferenceTransformName, std::string referenceToTrackerTransformName)
{
	double toolToReferenceMatrix[16]; 
	if ( !trackedFrame->GetCustomFrameTransform(toolToReferenceTransformName.c_str(), toolToReferenceMatrix) )
	{
		LOG_ERROR("Unable to find tool to reference transform with name: " << toolToReferenceTransformName); 
		return; 
	}

	double referenceToTrackerMatrix[16]; 
	if ( !trackedFrame->GetCustomFrameTransform(referenceToTrackerTransformName.c_str(), referenceToTrackerMatrix) )
	{
		LOG_ERROR("Unable to find reference to tracker transform with name: " << referenceToTrackerTransformName); 
		return;
	}
	
	vtkSmartPointer<vtkTransform> tToolToReference = vtkSmartPointer<vtkTransform>::New(); 
	tToolToReference->SetMatrix(toolToReferenceMatrix); 

	vtkSmartPointer<vtkTransform> tReferenceToTracker = vtkSmartPointer<vtkTransform>::New(); 
	tReferenceToTracker->SetMatrix(referenceToTrackerMatrix); 

	vtkSmartPointer<vtkTransform> tUsProbeToTracker = vtkSmartPointer<vtkTransform>::New(); 
	tUsProbeToTracker->PostMultiply(); 
	tUsProbeToTracker->Identity(); 
	tUsProbeToTracker->Concatenate(tToolToReference); 
	tUsProbeToTracker->Concatenate(tReferenceToTracker); 
	tUsProbeToTracker->Update(); 

	double usprobeToTrackerMatrix[16]; 
	vtkMatrix4x4::DeepCopy(usprobeToTrackerMatrix, tUsProbeToTracker->GetMatrix()); 

	std::ostringstream transformFileName; 
	transformFileName << imageFileName << inputTransformSuffix << std::ends; 
	vtkstd::string transformData; 
	transformData+= "# ================================ #\n";
	transformData+= "# Transform Data of Captured Image #\n";
	transformData+= "# ================================ #\n";
	transformData+= "# THIS FILE CONTAINS THE REAL-TIME TRANSFORM DATA FOR THE CAPTURED IMAGE.\n";
	transformData+= "# DATA IS RECORDED IN THE FOLLOWING FORMAT:\n";
	transformData+= "# [FORMAT: Angle - in Degrees, Qx, Qy, Qz, Position in meter (x, y, z)]\n";
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
		vtkSmartPointer<vtkTransform> helperTransform = vtkSmartPointer<vtkTransform>::New(); 

		// # TRANSFORM: FROM THE US PROBE FRAME TO THE TRACKER FRAME
		helperTransform->SetMatrix(usprobeToTrackerMatrix); 
		helperTransform->GetOrientationWXYZ(wxyz); 
		helperTransform->GetPosition(xyz); 
		os  << "# TRANSFORM: FROM THE US PROBE FRAME TO THE TRACKER FRAME\n"
			<< wxyz[0] << "\t" << wxyz[1] << "\t" << wxyz[2] << "\t" << wxyz[3] << "\t"
			// /1000.0 is for conversion from mm to meter 
			<<  xyz[0]/1000.0 << "\t" <<  xyz[1]/1000.0 << "\t" <<  xyz[2]/1000.0 << "\n" 
			<< std::endl; 


		// # TRANSFORM: FROM THE DRB REFERENCE FRAME TO THE TRACKER FRAME
		helperTransform->SetMatrix(referenceToTrackerMatrix); 
		helperTransform->GetOrientationWXYZ(wxyz); 
		helperTransform->GetPosition(xyz); 

		os  << "# TRANSFORM: FROM THE DRB REFERENCE FRAME TO THE TRACKER FRAME\n" 
			<< wxyz[0] << "\t" << wxyz[1] << "\t" << wxyz[2] << "\t" << wxyz[3] << "\t"
			// /1000.0 is for conversion from mm to meter 
			<<  xyz[0]/1000.0 << "\t" <<  xyz[1]/1000.0 << "\t" <<  xyz[2]/1000.0 << "\n" 
			<< std::endl; 

		os << std::ends; 

		transformData.append(os.str().c_str()); 
		fprintf(transformFile, transformData.c_str()); 
		fclose(transformFile); 
	}
}


//-------------------------------------------------------------------------------
void SaveImageToMetaFile( TrackedFrame* trackedFrame, std::string metaFileName, bool useCompression)
{
	TrackedFrame::ImageType::Pointer mfOrientedImage = trackedFrame->ImageData; 

    US_IMAGE_ORIENTATION desiredOrientation = UsImageConverterCommon::GetUsImageOrientationFromString( outputUsImageOrientation.c_str() ); 
    TrackedFrame::ImageType::Pointer orientedImage = TrackedFrame::ImageType::New(); 
    if ( GetOrientedImage(mfOrientedImage, desiredOrientation, orientedImage) != PLUS_SUCCESS )
    {
        LOG_ERROR("Failed to save oriented image to metafile (" << metaFileName << ")!"); 
        return; 
    }

	const unsigned long imageWidthInPixels = orientedImage->GetLargestPossibleRegion().GetSize()[0]; 
	const unsigned long imageHeightInPixels = orientedImage->GetLargestPossibleRegion().GetSize()[1]; 
	unsigned int frameSizeInBytes = imageWidthInPixels * imageHeightInPixels * sizeof(PixelType);

	ImageType3D::Pointer frame = ImageType3D::New(); 
	ImageType3D::SizeType size = {imageWidthInPixels, imageHeightInPixels, 1 };
	ImageType3D::IndexType start = {0,0,0};
	ImageType3D::RegionType region;
	region.SetSize(size);
	region.SetIndex(start);
	frame->SetRegions(region);
	frame->Allocate();

	memcpy(frame->GetBufferPointer() , orientedImage->GetBufferPointer() , frameSizeInBytes);

	double transformMatrix[16]; 
	trackedFrame->GetDefaultFrameTransform(transformMatrix); 


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
void ReadDRBTransformFile( const std::string TransformFileNameWithPath, TrackedFrame* trackedFrame )
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

	trackedFrame->SetCustomFrameTransform("ToolToTrackerTransform", tToolToTracker->GetMatrix() ); 


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

	trackedFrame->SetCustomFrameTransform("ReferenceToTrackerTransform", tReferenceToTracker->GetMatrix() ); 


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

	trackedFrame->SetCustomFrameTransform("ToolToReferenceTransform", tToolToReference->GetMatrix() ); 

	trackedFrame->DefaultFrameTransformName = "ToolToReferenceTransform"; 
}


//----------------------------------------------------------------------------
 PlusStatus GetOrientedImage( const ImageType::Pointer& inMFOrientedImage, US_IMAGE_ORIENTATION desiredUsImageOrientation, ImageType::Pointer& outOrientedImage )
{
    if ( inMFOrientedImage.IsNull() )
    {
        LOG_WARNING("Failed to get oriented image - input image is NULL!"); 
        return PLUS_FAIL; 
    }

    if ( outOrientedImage.IsNull() )
    {
        LOG_WARNING("Failed to get oriented image - output image is NULL!"); 
        return PLUS_FAIL; 
    }

    if ( desiredUsImageOrientation == US_IMG_ORIENT_XX )
	{
		LOG_DEBUG("GetOrientedImage: No ultrasound image orientation specified, return identical copy!"); 
		outOrientedImage = inMFOrientedImage; 
		return PLUS_SUCCESS; 
	}

    if ( US_IMG_ORIENT_MF == desiredUsImageOrientation )
	{
		outOrientedImage = inMFOrientedImage; 
		return PLUS_SUCCESS;
	}

    // if the desired image orientation is not MF, then flip the image 
	typedef itk::FlipImageFilter <ImageType> FlipImageFilterType;
	FlipImageFilterType::Pointer flipFilter = FlipImageFilterType::New ();
	flipFilter->SetInput(inMFOrientedImage);
	flipFilter->FlipAboutOriginOff(); 

    itk::FixedArray<bool, 2> flipAxes;
    if ( desiredUsImageOrientation == US_IMG_ORIENT_UF ) 
    {
        flipAxes[0] = true;
        flipAxes[1] = false;
    }
    else if ( desiredUsImageOrientation == US_IMG_ORIENT_UN ) 
    {
        flipAxes[0] = true;
        flipAxes[1] = true;
    }
    else if ( desiredUsImageOrientation == US_IMG_ORIENT_MF ) 
    {
        flipAxes[0] = false;
        flipAxes[1] = false;
    }
    else if ( desiredUsImageOrientation == US_IMG_ORIENT_MN ) 
    {
        flipAxes[0] = false;
        flipAxes[1] = true;
    }

	flipFilter->SetFlipAxes(flipAxes);
	flipFilter->Update();

    outOrientedImage = flipFilter->GetOutput(); 

	return PLUS_SUCCESS; 
}