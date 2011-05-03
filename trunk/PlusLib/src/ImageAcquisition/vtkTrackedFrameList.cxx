#include "PlusConfigure.h"
#include "vtkTrackedFrameList.h" 

#include <math.h>
#include "vtkObjectFactory.h"
#include "vtksys/SystemTools.hxx"

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkMetaImageSequenceIO.h"

//----------------------------------------------------------------------------
vtkCxxRevisionMacro(vtkTrackedFrameList, "$Revision: 1.0 $");
vtkStandardNewMacro(vtkTrackedFrameList); 

//----------------------------------------------------------------------------
vtkTrackedFrameList::vtkTrackedFrameList()
{
	this->SetMaxNumOfFramesToWrite(500); 
	this->SetNumberOfUniqueFrames(5); 
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
		return std::find_if(this->TrackedFrameList.begin(), this->TrackedFrameList.end(), TrackedFrameTimestampFinder(trackedFrame) ) == this->TrackedFrameList.end(); 
	}
	
	return true; 
}

//----------------------------------------------------------------------------
bool vtkTrackedFrameList::ValidatePosition(TrackedFrame* trackedFrame, char* frameTransformName)
{
	std::vector<TrackedFrame*>::iterator searchIndex; 
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
		return false; 
	}
	
	return true; 	
}

//----------------------------------------------------------------------------
bool vtkTrackedFrameList::ValidateStatus(TrackedFrame* trackedFrame)
{
	return (trackedFrame->Status == 0); 
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
void vtkTrackedFrameList::ReadFromSequenceMetafile(const char* trackedSequenceDataFileName)
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
		vtkErrorMacro(<< " Sequence image reader couldn't update: " <<  err); 
		exit(EXIT_FAILURE);
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
}

//----------------------------------------------------------------------------
void vtkTrackedFrameList::SaveToSequenceMetafile(const char* outputFolder, const char* sequenceDataFileName, SEQ_METAFILE_EXTENSION extension /*=SEQ_METAFILE_MHA*/ , bool useCompression /*=true*/)
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
		
		const unsigned long ImageWidthInPixels  = TrackedFrameList[0]->ImageData->GetLargestPossibleRegion().GetSize()[0]; 
		const unsigned long ImageHeightInPixels = TrackedFrameList[0]->ImageData->GetLargestPossibleRegion().GetSize()[1]; 

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
			memcpy(currentFrameImageData, TrackedFrameList[trackedFrameListItem]->ImageData->GetBufferPointer(), frameSizeInBytes); 

			// Write custom fields only once 
			if ( i == 0 )
			{
				writerMetaImageSequenceIO->SetDefaultFrameTransformName( TrackedFrameList[trackedFrameListItem]->DefaultFrameTransformName ); 

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

