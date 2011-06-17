#include "PlusConfigure.h"
#include "vtkTrackedFrameList.h" 

#include <math.h>
#include "vtkObjectFactory.h"
#include "vtksys/SystemTools.hxx"

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkMetaImageSequenceIO.h"
#include "itkFixedArray.h"
#include "itkFlipImageFilter.h"


//----------------------------------------------------------------------------
// ************************* TrackedFrame ************************************
//----------------------------------------------------------------------------
TrackedFrame::TrackedFrame()
{
	this->Status = 0; 
	this->Timestamp = 0; 
	this->ImageData = NULL; 
	this->UltrasoundImageOrientation = US_IMG_ORIENT_XX; 
	this->FrameSize[0] = 0; 
	this->FrameSize[1] = 0; 
	this->FrameSize[2] = 0; 
}

//----------------------------------------------------------------------------
TrackedFrame::~TrackedFrame()
{
	if ( this->ImageData != NULL )
	{
		this->ImageData->UnRegister(); 
	}
}

//----------------------------------------------------------------------------
TrackedFrame::TrackedFrame(const TrackedFrame& frame)
{
	this->DefaultFrameTransformName = frame.DefaultFrameTransformName; 
	this->UltrasoundImageOrientation = frame.UltrasoundImageOrientation; 
	this->CustomFrameFieldList = frame.CustomFrameFieldList; 
	this->CustomFieldList = frame.CustomFieldList; 
	this->ImageData = frame.ImageData; 
	this->Timestamp = frame.Timestamp;
	this->Status = frame.Status; 
	
	if ( this->ImageData != NULL )
	{
		this->ImageData->Register(); 
	}
}

//----------------------------------------------------------------------------
int* TrackedFrame::GetFrameSize()
{
	if ( this->ImageData != NULL )
	{
		const int w = this->ImageData->GetLargestPossibleRegion().GetSize()[0]; 
		const int h = this->ImageData->GetLargestPossibleRegion().GetSize()[1]; 
		this->FrameSize[0] = w; 
		this->FrameSize[1] = h; 
		this->FrameSize[2] = 1; 
	}

	return this->FrameSize; 
}


//----------------------------------------------------------------------------
void TrackedFrame::SetCustomFrameField( std::string name, std::string value )
{
	CustomFrameFieldPair pair(name, value); 
	this->CustomFrameFieldList.push_back(pair); 
}

//----------------------------------------------------------------------------
const char* TrackedFrame::GetCustomFrameField( std::string name )
{
	std::vector<CustomFrameFieldPair>::iterator customFrameValue; 
	for ( customFrameValue = this->CustomFrameFieldList.begin(); customFrameValue != this->CustomFrameFieldList.end(); customFrameValue++ )
	{
		if ( STRCASECMP(customFrameValue->first.c_str(), name.c_str()) == 0 ) 
		{ 
			return customFrameValue->second.c_str(); 
		}
	}

	return NULL; 
}

//----------------------------------------------------------------------------
void TrackedFrame::SetCustomField( std::string name, std::string value )
{
	CustomFieldPair pair(name, value); 
	this->CustomFieldList.push_back(pair); 
}

//----------------------------------------------------------------------------
const char* TrackedFrame::GetCustomField( std::string name )
{
	std::vector<CustomFieldPair>::iterator customValue; 
	for ( customValue = this->CustomFieldList.begin(); customValue != this->CustomFieldList.end(); customValue++ )
	{
		if ( STRCASECMP(customValue->first.c_str(), name.c_str()) == 0 )
		{ 
			return customValue->second.c_str(); 
		}
	}

	return NULL; 
}


//----------------------------------------------------------------------------
bool TrackedFrame::GetDefaultFrameTransform(double transform[16]) 
{
	// Find default frame transform 
	std::vector<CustomFrameFieldPair>::iterator defaultFrameTransform; 
	for ( defaultFrameTransform = this->CustomFrameFieldList.begin(); defaultFrameTransform != this->CustomFrameFieldList.end(); defaultFrameTransform++ )
	{
		if ( STRCASECMP(defaultFrameTransform->first.c_str(), this->DefaultFrameTransformName.c_str()) == 0) 
		{
			std::istringstream transformFieldValue(defaultFrameTransform->second); 
			double item; 
			int i = 0; 
			while ( transformFieldValue >> item )
			{
				if ( i < 16 )
					transform[i++] = item; 
			}
			return true;
		}
	}

	LOG_ERROR("Unable to find default transform in sequence metafile!"); 
	return false; 
}

//----------------------------------------------------------------------------
bool TrackedFrame::GetCustomFrameTransform(const char* frameTransformName, double transform[16]) 
{
	if (frameTransformName == NULL )
	{
		LOG_ERROR("Unable to get custom frame transform: frame transform name is NULL!"); 
		return false; 
	}

	// Find default frame transform 
	std::vector<CustomFrameFieldPair>::iterator customFrameTransform; 
	for ( customFrameTransform = this->CustomFrameFieldList.begin(); customFrameTransform != this->CustomFrameFieldList.end(); customFrameTransform++ )
	{
		if ( STRCASECMP(customFrameTransform->first.c_str(), frameTransformName) == 0)
		{
			std::istringstream transformFieldValue(customFrameTransform->second); 
			double item; 
			int i = 0; 
			while ( transformFieldValue >> item )
			{
				if ( i < 16 )
					transform[i++] = item; 
			}
			return true;
		}
	}

	LOG_ERROR("Unable to find custom transform (" << frameTransformName << ") in sequence metafile!"); 
	return false; 
}


//----------------------------------------------------------------------------
void TrackedFrame::SetCustomFrameTransform(std::string frameTransformName, double transform[16]) 
{
	std::ostringstream strTransform; 
	for ( int i = 0; i < 16; ++i )
	{
		strTransform << transform[ i ] << " ";
	}

	CustomFrameFieldPair pair(frameTransformName, strTransform.str()); 
	this->CustomFrameFieldList.push_back(pair); 
}

//----------------------------------------------------------------------------
void TrackedFrame::SetCustomFrameTransform(std::string frameTransformName, vtkMatrix4x4* transform) 
{
	double dTransform[ 16 ];
	vtkMatrix4x4::DeepCopy( dTransform, transform );
	this->SetCustomFrameTransform(frameTransformName, dTransform); 
}

//----------------------------------------------------------------------------
TrackedFrame::US_IMAGE_ORIENTATION TrackedFrame::GetUsImageOrientationFromString( const char* usImgOrientation )
{
	US_IMAGE_ORIENTATION imageorientation = US_IMG_ORIENT_XX; 
	if ( usImgOrientation == NULL )
	{
		return imageorientation; 
	}
	else if ( STRCASECMP(usImgOrientation, "UF" ) == 0 )
	{
		imageorientation = US_IMG_ORIENT_UF; 
	}
	else if ( STRCASECMP(usImgOrientation, "UN" ) == 0 )
	{
		imageorientation = US_IMG_ORIENT_UN; 
	}
	else if ( STRCASECMP(usImgOrientation, "MF" ) == 0 )
	{
		imageorientation = US_IMG_ORIENT_MF; 
	}
	else if ( STRCASECMP(usImgOrientation, "MN" ) == 0 )
	{
		imageorientation = US_IMG_ORIENT_MN; 
	}

	return imageorientation; 
}

//----------------------------------------------------------------------------
const char* TrackedFrame::GetUltrasoundImageOrientation()
{
	std::string usImgOrientation; 
	switch(this->UltrasoundImageOrientation)
	{
	case US_IMG_ORIENT_UF: 
		usImgOrientation = "UF"; 
		break; 
	case US_IMG_ORIENT_UN: 
		usImgOrientation = "UN"; 
		break; 
	case US_IMG_ORIENT_MF:
		usImgOrientation = "MF"; 
		break; 
	case US_IMG_ORIENT_MN: 
		usImgOrientation = "MN"; 
		break; 
	case US_IMG_ORIENT_XX: 
		usImgOrientation = "XX"; 
		break; 
	}	 

	return usImgOrientation.c_str(); 
}	

//----------------------------------------------------------------------------
void TrackedFrame::SetUltrasoundImageOrientation(const char* usImgOrientation)
{
	this->UltrasoundImageOrientation = this->GetUsImageOrientationFromString(usImgOrientation); 
}	

//----------------------------------------------------------------------------
TrackedFrame::ImageType* TrackedFrame::GetOrientedImage( const char* usImageOrientation )
{
	return this->GetOrientedImage(this->GetUsImageOrientationFromString(usImageOrientation) ); 
}

//----------------------------------------------------------------------------
TrackedFrame::ImageType* TrackedFrame::GetOrientedImage( US_IMAGE_ORIENTATION usImageOrientation )
{
    if ( this->ImageData == NULL )
    {
        LOG_DEBUG("Image data was NULL, so oriented image will be NULL also!"); 
        return NULL; 
    }

	ImageType::Pointer flipedImage; 
	if ( this->UltrasoundImageOrientation == US_IMG_ORIENT_XX || usImageOrientation == US_IMG_ORIENT_XX )
	{
		LOG_DEBUG("GetOrientedImage: No ultrasound image orientation specified, return identical copy!"); 
		flipedImage = this->ImageData; 
		flipedImage->Register(); 
		return flipedImage; 
	}

	if ( this->UltrasoundImageOrientation == usImageOrientation )
	{
		flipedImage = this->ImageData; 
		flipedImage->Register(); 
		return flipedImage; 
	}

	typedef itk::FlipImageFilter <ImageType> FlipImageFilterType;
	FlipImageFilterType::Pointer flipFilter = FlipImageFilterType::New ();
	flipFilter->SetInput(this->ImageData);
	flipFilter->FlipAboutOriginOff(); 

	itk::FixedArray<bool, 2> flipAxes;
	switch( this->UltrasoundImageOrientation )
	{
	case US_IMG_ORIENT_UF: 
		{
			if ( usImageOrientation == US_IMG_ORIENT_UF ) 
			{
				flipAxes[0] = false;
				flipAxes[1] = false;
			}
			else if ( usImageOrientation == US_IMG_ORIENT_UN ) 
			{
				flipAxes[0] = false;
				flipAxes[1] = true;
			}
			else if ( usImageOrientation == US_IMG_ORIENT_MF ) 
			{
				flipAxes[0] = true;
				flipAxes[1] = false;
			}
			else if ( usImageOrientation == US_IMG_ORIENT_MN ) 
			{
				flipAxes[0] = true;
				flipAxes[1] = true;
			}
		}
		break; 
	case US_IMG_ORIENT_UN: 
		{
			if ( usImageOrientation == US_IMG_ORIENT_UF ) 
			{
				flipAxes[0] = false;
				flipAxes[1] = true;
			}
			else if ( usImageOrientation == US_IMG_ORIENT_UN ) 
			{
				flipAxes[0] = false;
				flipAxes[1] = false;
			}
			else if ( usImageOrientation == US_IMG_ORIENT_MF ) 
			{
				flipAxes[0] = true;
				flipAxes[1] = true;
			}
			else if ( usImageOrientation == US_IMG_ORIENT_MN ) 
			{
				flipAxes[0] = true;
				flipAxes[1] = false;
			}
		}
		break; 
	case US_IMG_ORIENT_MF: 
		{
			if ( usImageOrientation == US_IMG_ORIENT_UF ) 
			{
				flipAxes[0] = true;
				flipAxes[1] = false;
			}
			else if ( usImageOrientation == US_IMG_ORIENT_UN ) 
			{
				flipAxes[0] = true;
				flipAxes[1] = true;
			}
			else if ( usImageOrientation == US_IMG_ORIENT_MF ) 
			{
				flipAxes[0] = false;
				flipAxes[1] = false;
			}
			else if ( usImageOrientation == US_IMG_ORIENT_MN ) 
			{
				flipAxes[0] = false;
				flipAxes[1] = true;
			}
		}
		break; 
	case US_IMG_ORIENT_MN: 
		{
			if ( usImageOrientation == US_IMG_ORIENT_UF ) 
			{
				flipAxes[0] = true;
				flipAxes[1] = true;
			}
			else if ( usImageOrientation == US_IMG_ORIENT_UN ) 
			{
				flipAxes[0] = true;
				flipAxes[1] = false;
			}
			else if ( usImageOrientation == US_IMG_ORIENT_MF ) 
			{
				flipAxes[0] = false;
				flipAxes[1] = true;
			}
			else if ( usImageOrientation == US_IMG_ORIENT_MN ) 
			{
				flipAxes[0] = false;
				flipAxes[1] = false;
			}
		}
		break; 
	}

	flipFilter->SetFlipAxes(flipAxes);
	flipFilter->Update();

	flipedImage = flipFilter->GetOutput(); 
	flipedImage->Register();

	return flipedImage; 
}

//----------------------------------------------------------------------------
// ************************* vtkTrackedFrameList *****************************
//----------------------------------------------------------------------------
vtkCxxRevisionMacro(vtkTrackedFrameList, "$Revision: 1.0 $");
vtkStandardNewMacro(vtkTrackedFrameList); 

//----------------------------------------------------------------------------
vtkTrackedFrameList::vtkTrackedFrameList()
{
	this->SetMaxNumOfFramesToWrite(500); 
	this->SetNumberOfUniqueFrames(5); 
	this->SetFrameSize(0,0,0); 
}

//----------------------------------------------------------------------------
vtkTrackedFrameList::~vtkTrackedFrameList()
{
	this->Clear(); 
}

//----------------------------------------------------------------------------
void vtkTrackedFrameList::Clear()
{
	for ( int i = 0; i < this->TrackedFrameList.size(); i++ )
	{
		if (this->TrackedFrameList[i] != NULL )
		{
			delete this->TrackedFrameList[i]; 
			this->TrackedFrameList[i] = NULL; 
		}
	}
	this->TrackedFrameList.clear(); 
}

//----------------------------------------------------------------------------
void vtkTrackedFrameList::PrintSelf(std::ostream &os, vtkIndent indent)
{
	this->Superclass::PrintSelf(os,indent);
}

//----------------------------------------------------------------------------
TrackedFrame* vtkTrackedFrameList::GetTrackedFrame(int frameNumber)
{
	if ( frameNumber < this->GetNumberOfTrackedFrames() )
	{
		return this->TrackedFrameList[frameNumber]; 
	}

	return NULL; 
}

//----------------------------------------------------------------------------
int vtkTrackedFrameList::AddTrackedFrame(TrackedFrame *trackedFrame)
{
	TrackedFrame* pTrackedFrame = new TrackedFrame(*trackedFrame); 
	this->TrackedFrameList.push_back(pTrackedFrame); 
	return (this->TrackedFrameList.size() - 1); 
}

//----------------------------------------------------------------------------
bool vtkTrackedFrameList::ValidateData(TrackedFrame* trackedFrame, bool validateTimestamp/*=true*/, bool validateStatus/*=true*/, 
									   bool validatePosition/*=true*/, char* frameTransformName /*= NULL*/, bool validateSpeed/*=true*/)
{
	bool validationResult(true); 

	if ( validateTimestamp )
	{
		validationResult &= this->ValidateTimestamp(trackedFrame); 
	}

	if ( validateStatus )
	{
		validationResult &= this->ValidateStatus(trackedFrame); 
	}

	if ( validatePosition )
	{
		validationResult &= this->ValidatePosition(trackedFrame, frameTransformName); 
	}

	if ( validateSpeed )
	{
		validationResult &= true; 
	}

	return validationResult; 
}

//----------------------------------------------------------------------------
bool vtkTrackedFrameList::ValidateTimestamp(TrackedFrame* trackedFrame)
{
	if ( this->TrackedFrameList.size() > 0 )
	{
		const bool isTimestampUnique = std::find_if(this->TrackedFrameList.begin(), this->TrackedFrameList.end(), TrackedFrameTimestampFinder(trackedFrame) ) == this->TrackedFrameList.end(); 

		if ( !isTimestampUnique )
		{
			LOG_DEBUG("Tracked frame timestamp validation result: we've already inserted this frame to container!"); 
			return false; 
		}
		
		return true; 
	}
	
	return true; 
}

//----------------------------------------------------------------------------
bool vtkTrackedFrameList::ValidatePosition(TrackedFrame* trackedFrame, char* frameTransformName)
{
	TrackedFrameListType::iterator searchIndex; 
	const int containerSize = this->TrackedFrameList.size(); 
	if (containerSize < this->NumberOfUniqueFrames )
	{
		searchIndex = this->TrackedFrameList.begin(); 
	}
	else
	{
		searchIndex =this->TrackedFrameList.end() - this->NumberOfUniqueFrames; 
	}

	if (std::find_if(searchIndex, this->TrackedFrameList.end(), TrackedFramePositionFinder(trackedFrame, frameTransformName) ) != this->TrackedFrameList.end() )
	{
		// We've already inserted this frame 
		LOG_DEBUG("Tracked frame position validation result: we've already inserted this frame to container!"); 
		return false; 
	}
	
	return true; 	
}

//----------------------------------------------------------------------------
bool vtkTrackedFrameList::ValidateStatus(TrackedFrame* trackedFrame)
{
	const bool isStatusValid = (trackedFrame->Status == 0); 
	if ( !isStatusValid )
	{
		LOG_DEBUG("Tracked frame status validation result: tracked frame status invalid!"); 
	}

	return isStatusValid; 
}

//----------------------------------------------------------------------------
int* vtkTrackedFrameList::GetFrameSize()
{
	if ( this->GetNumberOfTrackedFrames() > 0 )
	{
		this->SetFrameSize(this->GetTrackedFrame(0)->GetFrameSize()); 
	}
	else
	{
		LOG_WARNING("Unable to get frame size: there is no frame in the tracked frame list!"); 
	}

	return this->FrameSize; 
}

//----------------------------------------------------------------------------
std::string vtkTrackedFrameList::GetDefaultFrameTransformName()
{
	std::string defaultFrameTransformName; 
	if ( !TrackedFrameList.empty() )
	{
		defaultFrameTransformName = this->GetTrackedFrame(0)->DefaultFrameTransformName; 
	}

	return defaultFrameTransformName; 
}

//----------------------------------------------------------------------------
PlusStatus vtkTrackedFrameList::ReadFromSequenceMetafile(const char* trackedSequenceDataFileName)
{
	typedef itk::Image< TrackedFrame::PixelType, 3 > ImageSequenceType;
	typedef itk::ImageFileReader< ImageSequenceType > ImageSequenceReaderType;
	itk::MetaImageSequenceIO::Pointer readerMetaImageSequenceIO = itk::MetaImageSequenceIO::New(); 
	ImageSequenceReaderType::Pointer reader = ImageSequenceReaderType::New(); 

	// Set the image IO 
	reader->SetImageIO(readerMetaImageSequenceIO); 
	reader->SetFileName(trackedSequenceDataFileName);

	try
	{
		reader->Update(); 
	}
	catch (itk::ExceptionObject & err) 
	{		
		LOG_ERROR(" Sequence image reader couldn't update: " <<  err); 
		return PLUS_FAIL;
	}	

	ImageSequenceType::Pointer imageSeq = reader->GetOutput();

	const unsigned long ImageWidthInPixels = imageSeq->GetLargestPossibleRegion().GetSize()[0]; 
	const unsigned long ImageHeightInPixels = imageSeq->GetLargestPossibleRegion().GetSize()[1]; 
	const unsigned long numberOfFrames = imageSeq->GetLargestPossibleRegion().GetSize()[2];	

	unsigned int frameSizeInBytes=ImageWidthInPixels*ImageHeightInPixels*sizeof(TrackedFrame::PixelType);

	TrackedFrame::PixelType* imageSeqData = imageSeq->GetBufferPointer(); // pointer to the image pixel buffer

	for ( int imgNumber = 0; imgNumber < numberOfFrames; imgNumber++ )
	{

		TrackedFrame::PixelType *currentFrameImageData= imageSeqData + imgNumber * frameSizeInBytes;

		TrackedFrame::ImageType::Pointer frame = TrackedFrame::ImageType::New();
		TrackedFrame::ImageType::SizeType size = {ImageWidthInPixels, ImageHeightInPixels };
		TrackedFrame::ImageType::IndexType start = {0,0};
		TrackedFrame::ImageType::RegionType region;
		region.SetSize(size);
		region.SetIndex(start);
		frame->SetRegions(region);
		frame->Allocate();

		memcpy(frame->GetBufferPointer() , currentFrameImageData, frameSizeInBytes);

		TrackedFrame trackedFrame; 

		// Get Default transform name 
		std::string defaultFrameTransformName = readerMetaImageSequenceIO->GetDefaultFrameTransformName(); 
		trackedFrame.DefaultFrameTransformName = defaultFrameTransformName; 

		// Get ultrasound image orientation 
		trackedFrame.SetUltrasoundImageOrientation(readerMetaImageSequenceIO->GetUltrasoundImageOrientation()); 

		// Get custom fields 
		std::vector<std::string> customFieldNames = readerMetaImageSequenceIO->GetCustomFieldNames(); 
		for ( int i = 0; i < customFieldNames.size(); i++ )
		{
			TrackedFrame::CustomFieldPair field; 
			field.first = customFieldNames[i]; 
			field.second = readerMetaImageSequenceIO->GetCustomString(customFieldNames[i].c_str()); 
			trackedFrame.CustomFieldList.push_back(field); 
		}

		// Get custom frame fields 
		std::vector<std::string> customFrameFieldNames = readerMetaImageSequenceIO->GetCustomFrameFieldNames(); 
		for ( int i = 0; i < customFrameFieldNames.size(); i++ )
		{
			TrackedFrame::CustomFrameFieldPair field; 
			field.first = customFrameFieldNames[i]; 
			field.second = readerMetaImageSequenceIO->GetCustomFrameString(imgNumber,customFrameFieldNames[i].c_str()); 
			trackedFrame.CustomFrameFieldList.push_back(field); 
		}
		
		trackedFrame.ImageData = frame;
		trackedFrame.ImageData->Register(); 
		
		this->AddTrackedFrame(&trackedFrame); 
	}

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
void vtkTrackedFrameList::SaveToSequenceMetafile(const char* outputFolder, const char* sequenceDataFileName, SEQ_METAFILE_EXTENSION extension /*=SEQ_METAFILE_MHA*/ , bool useCompression /*=true*/, const char* usImageOrientation /*=NULL*/)
{
	LOG_TRACE("vtkTrackedFrameList::SaveToSequenceMetafile - outputFolder: " << outputFolder << "  sequenceDataFileName: " << sequenceDataFileName); 

	if ( TrackedFrameList.empty() )
	{
		LOG_ERROR("Unable to save tracked frame list to sequence metafile - tracked frame list empty!"); 
		return; 
	}

	if ( !vtksys::SystemTools::FileExists(outputFolder, false) )
	{
		vtksys::SystemTools::MakeDirectory(outputFolder); 
		LOG_DEBUG("Created new folder: " << outputFolder ); 
	}

	const int numberOfFilesToWrite = ceil( (1.0 * TrackedFrameList.size()) / (1.0 * this->MaxNumOfFramesToWrite) ); 

	for ( int fileNumber = 1; fileNumber <= numberOfFilesToWrite; fileNumber++ )
	{

		typedef itk::Image< TrackedFrame::PixelType, 3 > ImageSequenceType;
		typedef itk::ImageFileWriter< ImageSequenceType > ImageSequenceWriterType;

		ImageSequenceType::Pointer imageDataSequence = ImageSequenceType::New();

		int numberOfFrames(0); 
		if ( TrackedFrameList.size() - (fileNumber - 1) * this->MaxNumOfFramesToWrite > this->MaxNumOfFramesToWrite )
		{
			numberOfFrames = this->MaxNumOfFramesToWrite; 
		}
		else
		{
			numberOfFrames = TrackedFrameList.size() - (fileNumber - 1) * this->MaxNumOfFramesToWrite; 
		}
		
		unsigned long ImageWidthInPixels(1), ImageHeightInPixels(1); 
        if ( TrackedFrameList[0]->ImageData != NULL )
        {
            ImageWidthInPixels = TrackedFrameList[0]->ImageData->GetLargestPossibleRegion().GetSize()[0]; 
		    ImageHeightInPixels = TrackedFrameList[0]->ImageData->GetLargestPossibleRegion().GetSize()[1]; 
        }

		ImageSequenceType::SizeType size = {ImageWidthInPixels, ImageHeightInPixels, numberOfFrames };
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
			LOG_ERROR("Unable to allocate memory for image sequence : " << err);
			return; 
		}	

		itk::MetaImageSequenceIO::Pointer writerMetaImageSequenceIO = itk::MetaImageSequenceIO::New();

		TrackedFrame::PixelType* imageData = imageDataSequence->GetBufferPointer(); // pointer to the image pixel buffer

		

		unsigned int frameSizeInBytes=ImageWidthInPixels*ImageHeightInPixels*sizeof(TrackedFrame::PixelType);

		for ( int i = 0 ; i < numberOfFrames; i++ ) 
		{
			int trackedFrameListItem = (fileNumber - 1) * this->MaxNumOfFramesToWrite + i; 
			TrackedFrame::PixelType *currentFrameImageData = imageData + i * frameSizeInBytes;
			
			TrackedFrame::ImageType* orientedImage = TrackedFrameList[trackedFrameListItem]->GetOrientedImage(usImageOrientation); 
            if ( orientedImage != NULL )
            {
			    memcpy(currentFrameImageData, orientedImage->GetBufferPointer(), frameSizeInBytes); 
			    orientedImage->UnRegister(); 
            }
            else
            {
               memset(currentFrameImageData, NULL, frameSizeInBytes); 
            }

			// Write custom fields only once 
			if ( i == 0 )
			{
				// Set default frame transform name
				writerMetaImageSequenceIO->SetDefaultFrameTransformName( TrackedFrameList[trackedFrameListItem]->DefaultFrameTransformName ); 
				
				// Set ultrasound image orientation
				if ( TrackedFrameList[trackedFrameListItem]->UltrasoundImageOrientation != TrackedFrame::US_IMG_ORIENT_XX )
				{
					// Set to the converted orientation 
					writerMetaImageSequenceIO->SetUltrasoundImageOrientation( usImageOrientation ); 
				}
				else
				{
					// Cannot convert from undefined image orientation, so the sequence should be undefined, too.
					writerMetaImageSequenceIO->SetUltrasoundImageOrientation( NULL ); 
				}

				for( int field = 0; field < TrackedFrameList[trackedFrameListItem]->CustomFieldList.size(); field++ )
				{
					writerMetaImageSequenceIO->SetCustomString(TrackedFrameList[trackedFrameListItem]->CustomFieldList[field].first.c_str(), TrackedFrameList[trackedFrameListItem]->CustomFieldList[field].second.c_str() ); 
				}
			}

			for( int field = 0; field < TrackedFrameList[trackedFrameListItem]->CustomFrameFieldList.size(); field++ )
			{
				writerMetaImageSequenceIO->SetCustomFrameString(i, TrackedFrameList[trackedFrameListItem]->CustomFrameFieldList[field].first.c_str(), TrackedFrameList[trackedFrameListItem]->CustomFrameFieldList[field].second.c_str() ); 
			}
		}



		ImageSequenceWriterType::Pointer writer = ImageSequenceWriterType::New(); 

		std::ostringstream sequenceDataFilePath; 

		switch (extension)
		{
		case SEQ_METAFILE_MHA:
			if ( numberOfFilesToWrite == 1 )
			{
				sequenceDataFilePath << outputFolder << "/" << sequenceDataFileName << ".mha"; 
			}
			else
			{
				sequenceDataFilePath << outputFolder << "/" << sequenceDataFileName << "_" << std::setfill('0') << std::setw(2) << fileNumber << ".mha"; 
			}
			break; 
		case SEQ_METAFILE_MHD:
			if ( numberOfFilesToWrite == 1 )
			{
				sequenceDataFilePath << outputFolder << "/" << sequenceDataFileName << ".mhd"; 
			}
			else
			{
				sequenceDataFilePath << outputFolder << "/" << sequenceDataFileName << "_" << std::setfill('0') << std::setw(2) << fileNumber << ".mhd"; 
			}
			break; 
		}



		writer->SetFileName(sequenceDataFilePath.str().c_str());
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
}

