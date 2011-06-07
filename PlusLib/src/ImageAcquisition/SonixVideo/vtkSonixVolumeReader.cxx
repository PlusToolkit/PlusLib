#include "vtkSonixVolumeReader.h"
#include "vtkObjectFactory.h"
#include "vtkSmartPointer.h"
#include "vtkImageFlip.h"
#include "vtkImageData.h"
#include "vtkTIFFWriter.h"


#include <iostream>
#include <sstream>

vtkCxxRevisionMacro(vtkSonixVolumeReader, "$Revision: 1.0 $");

vtkStandardNewMacro(vtkSonixVolumeReader);

//----------------------------------------------------------------------------
vtkSonixVolumeReader::vtkSonixVolumeReader()
{

}


//----------------------------------------------------------------------------
vtkSonixVolumeReader::~vtkSonixVolumeReader()
{
	if (!this->ImageDataVector.empty())
	{
		while (!this->ImageDataVector.empty())
		{
			this->ImageDataVector.back()->UnRegister(NULL); 
			this->ImageDataVector.pop_back();
		}
	}

}


//----------------------------------------------------------------------------
void vtkSonixVolumeReader::PrintSelf(ostream& os, vtkIndent indent)
{
	//this->Superclass::PrintSelf(os,indent);

	os << indent << "\nVolume specific parameters:\n"; 
	os << indent << "===========================\n"; 
	
	os << indent << "FileName: " << this->FileName << "\n";

	switch(this->DataType)
	{
	case udtBPre:   
		os << indent << "DataType: udtBPre - pre-scan data\n"; break;
	case udtBPost:  
		os << indent << "DataType: udtBPost - post-scan data (8-bit)\n"; break;
	case udtBPost32:
		os << indent << "DataType: udtBPost32 - post-scan data (32-bit)\n"; break;
	default:        
		os << indent << "DataType: unhandled\n"; break;
	}

	os << indent << "NumberOfFrames: " << this->NumberOfFrames << "\n";
	os << indent << "FrameWidth: " << this->FrameWidth << "\n";
	os << indent << "FrameHeight: " << this->FrameHeight << "\n";
	os << indent << "DataSampleSize: " << this->DataSampleSize << "\n";
}


//----------------------------------------------------------------------------
int vtkSonixVolumeReader::ReadVolume(const char* filename)
{
	this->FileName = filename; 

	return this->ReadVolume(); 
}

//----------------------------------------------------------------------------
int vtkSonixVolumeReader::ReadVolume()
{
	if (this->FileName.empty())
	{
		LOG_ERROR("Error: cannot open volume file: " << this->FileName.c_str()); 
		return -1; 
	}

	// read data file
	FILE * fp; 
	errno_t err = fopen_s (&fp,this->FileName.c_str(), "rb");
	
	if(err != 0)
	{
		LOG_ERROR("Error opening volume file: " << this->FileName.c_str() << " Error No.: " << err); 
		return -1;
	}

	uFileHeader hdr;
	
	// read header
	fread(&hdr, sizeof(hdr), 1, fp);    

	this->DataType = hdr.type; 
	this->DataSampleSize = hdr.ss; 
	this->NumberOfFrames = hdr.frames; 

	if (this->DataType == udtBPost || this->DataType == udtBPost32)
	{
		this->FrameWidth = hdr.w; 
		this->FrameHeight = hdr.h; 
	}
	else if (this->DataType == udtBPre)
	{
		// if pre-scan data, then switch width and height, because the image
		// is not rasterized like a bitmap, but written rayline by rayline
		this->FrameWidth = hdr.h; 
		this->FrameHeight = hdr.w; 
	}
	else
	{
		LOG_ERROR("Unsupported data type: " << this->DataType); 
		return -1;

	}

	this->ImageDataVector.reserve(this->NumberOfFrames); 

	int frameSize = this->FrameWidth * this->FrameHeight * (this->DataSampleSize / 8);
	
	int nComponents = this->DataSampleSize/8; 

	// Pointer to data from file 
	unsigned char *dataFromFile;
	// Pointer to dataFromFile for seeking
	unsigned char *movingPointer; 

	dataFromFile = new unsigned char[frameSize];

	for (int i = 0; i < this->NumberOfFrames; i++)
	{
		vtkSmartPointer<vtkImageData> imageData = vtkSmartPointer<vtkImageData>::New(); 
		imageData->SetNumberOfScalarComponents(1); 
		imageData->SetDimensions(this->FrameWidth, this->FrameHeight, 1 ); 
		imageData->SetScalarTypeToUnsignedChar(); 
		imageData->Update(); 

		// pre-scan data has a 4 byte frame tag
		if(this->DataType == udtBPre)
		{
			int frhdr; 
			fread(&frhdr, 4, 1, fp);
		}

		unsigned char *imageDataPtr = static_cast<unsigned char*>(imageData->GetScalarPointer());
		
		// Read data from file 
		fread(dataFromFile, frameSize, 1, fp);
		
		//Copy data to vtkImageData 
		if (this->DataType == udtBPost32)
		{
			// Convert RGBA to 8bit Greyscale image
			movingPointer = dataFromFile; 
			for (int i = 0 ; i < frameSize; i += nComponents) 
			{
				// Get just the R channel from RGBA 
				*imageDataPtr++ = *movingPointer;
				movingPointer += nComponents; 
			}
		}
		else
		{
			// Copy the frame data form file to vtkImageDataSet
			memcpy(imageDataPtr,dataFromFile,frameSize);
		}

		imageData->UpdateData(); 
		
		// Add the vtkImageData to ImageDataVector
		vtkSmartPointer<vtkImageFlip> imageFlip = vtkSmartPointer<vtkImageFlip>::New();
		imageFlip->SetInput(imageData);
		imageFlip->SetFilteredAxes(1);
		imageFlip->Update();
		imageFlip->GetOutput()->Register(NULL);
		this->ImageDataVector.push_back(imageFlip->GetOutput()); 
	}

	fclose(fp);
	delete [] dataFromFile;

	return 1;
}


//----------------------------------------------------------------------------
vtkImageData *vtkSonixVolumeReader::GetFrame(int imageNumber/*=0*/)
{
	return this->ImageDataVector[imageNumber]; 
}


//----------------------------------------------------------------------------
vtkstd::vector<vtkImageData *>* vtkSonixVolumeReader::GetAllFrames()
{
	return &this->ImageDataVector; 
}


//----------------------------------------------------------------------------
void vtkSonixVolumeReader::WriteFrameAsTIFF(int imageNumber, const char* filePrefix, const char* directory /* = "./" */)
{

			vtkSmartPointer<vtkTIFFWriter> w = vtkSmartPointer<vtkTIFFWriter>::New();
			w->SetCompressionToNoCompression(); 
			w->SetInput(this->ImageDataVector[imageNumber]); 
			vtkstd::ostringstream os ; 
			os << directory <<filePrefix; 
			
			if (imageNumber < 10 ) 
			{
				os << "000" << imageNumber; 
			}
			else if (imageNumber < 100 ) 
			{
				os << "00" << imageNumber; 
			}
			else if (imageNumber < 1000 ) 
			{
				os << "0" << imageNumber; 
			}
			else 
			{
				os << imageNumber; 
			}

			os << ".tiff"; 
			w->SetFileName(os.str().c_str()); 
			w->Update(); 

}

//----------------------------------------------------------------------------
void vtkSonixVolumeReader::WriteAllFramesAsTIFF(const char* filePrefix, const char* directory /* = "./" */)
{
	if (this->ImageDataVector.empty())
	{
		LOG_WARNING("Warning: There are no images to save as TIFF"); 
		return; 
	}

	const int vectorSize = this->ImageDataVector.size(); 

	for (int imgnum = 0 ; imgnum < vectorSize; imgnum++) 
	{
		WriteFrameAsTIFF( imgnum, filePrefix, directory); 
	}
}	
