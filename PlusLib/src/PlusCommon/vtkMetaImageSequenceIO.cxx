/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"
#include "itk_zlib.h"
#include "itksys/SystemTools.hxx"
#include "vtkMetaImageSequenceIO.h"
#include <iomanip>
#include <iostream>

//#include <stdio.h>
  
#ifdef _WIN32
  #define FSEEK _fseeki64
  #define FTELL _ftelli64
#else
  #define FSEEK fseek
  #define FTELL ftell
#endif

    

// Size of MetaIO fields, in bytes (adopted from metaTypes.h)
enum
{
  MET_NONE, MET_ASCII_CHAR, MET_CHAR, MET_UCHAR, MET_SHORT,
  MET_USHORT, MET_INT, MET_UINT, MET_LONG, MET_ULONG,
  MET_LONG_LONG, MET_ULONG_LONG, MET_FLOAT, MET_DOUBLE, MET_STRING, 
  MET_CHAR_ARRAY, MET_UCHAR_ARRAY, MET_SHORT_ARRAY, MET_USHORT_ARRAY, MET_INT_ARRAY, 
  MET_UINT_ARRAY, MET_LONG_ARRAY, MET_ULONG_ARRAY, MET_LONG_LONG_ARRAY, MET_ULONG_LONG_ARRAY,
  MET_FLOAT_ARRAY, MET_DOUBLE_ARRAY, MET_FLOAT_MATRIX, MET_OTHER,
  // insert values before this line
  MET_NUM_VALUE_TYPES
};
static const unsigned char MET_ValueTypeSize[MET_NUM_VALUE_TYPES] = 
{
   0, 1, 1, 1, 2,
   2, 4, 4, 4, 4,
   8, 8, 4, 8, 1,
   1, 1, 2, 2, 4,
   4, 4, 4, 8, 8,
   4, 8, 4, 0 
};


#include "vtksys/SystemTools.hxx"  
#include "vtkObjectFactory.h"
#include "vtkTrackedFrameList.h"
#include "TrackedFrame.h"

static const int MAX_LINE_LENGTH=1000;


static const char* SEQMETA_FIELD_US_IMG_ORIENT = "UltrasoundImageOrientation";  
static const char* SEQMETA_FIELD_US_IMG_TYPE = "UltrasoundImageType";  
static const char* SEQMETA_FIELD_ELEMENT_DATA_FILE = "ElementDataFile"; 
static const char* SEQMETA_FIELD_VALUE_ELEMENT_DATA_FILE_LOCAL = "LOCAL"; 

static std::string SEQMETA_FIELD_FRAME_FIELD_PREFIX = "Seq_Frame"; 
static std::string SEQMETA_FIELD_IMG_STATUS = "ImageStatus"; 

vtkCxxRevisionMacro(vtkMetaImageSequenceIO, "$Revision: 1.0 $");
vtkStandardNewMacro(vtkMetaImageSequenceIO); 
vtkCxxSetObjectMacro(vtkMetaImageSequenceIO, TrackedFrameList, vtkTrackedFrameList);

//----------------------------------------------------------------------------
vtkMetaImageSequenceIO::vtkMetaImageSequenceIO()
: TrackedFrameList(vtkTrackedFrameList::New())
, FileName(NULL)
, TempHeaderFileName(NULL)
, TempImageFileName(NULL)
, UseCompression(false)
, FileType(itk::ImageIOBase::Binary)
, PixelType(itk::ImageIOBase::UNKNOWNCOMPONENTTYPE)
, NumberOfComponents(1)
, NumberOfDimensions(3)
, m_CurrentFrameOffset(0)
, m_TotalBytesWritten(0)
, ImageOrientationInFile(US_IMG_ORIENT_XX)
, ImageOrientationInMemory(US_IMG_ORIENT_XX)
, ImageType(US_IMG_TYPE_XX)
, PixelDataFileOffset(0)
, PixelDataFileName(NULL)
{ 
  this->Dimensions[0]=0;
  this->Dimensions[1]=0;
  this->Dimensions[2]=0;
} 

//----------------------------------------------------------------------------
vtkMetaImageSequenceIO::~vtkMetaImageSequenceIO()
{
  SetTrackedFrameList(NULL);
  SetFileName(NULL);
  SetTempHeaderFileName(NULL);
  SetTempImageFileName(NULL);
  SetPixelDataFileName(NULL);
}

//----------------------------------------------------------------------------
PlusStatus vtkMetaImageSequenceIO::DeleteCustomFrameString(int frameNumber, const char* fieldName)
{
  TrackedFrame* trackedFrame=this->TrackedFrameList->GetTrackedFrame(frameNumber);
  if (trackedFrame==NULL)
  {
    LOG_ERROR("Cannot access frame "<<frameNumber);
    return PLUS_FAIL;
  }
  
  return trackedFrame->DeleteCustomFrameField(fieldName); 
}

//----------------------------------------------------------------------------
PlusStatus vtkMetaImageSequenceIO::SetCustomFrameString(int frameNumber, const char* fieldName,  const char* fieldValue)
{
  if (fieldName==NULL || fieldValue==NULL)
  {
    LOG_ERROR("Invalid field name or value");
    return PLUS_FAIL;
  }
  CreateTrackedFrameIfNonExisting(frameNumber);
  TrackedFrame* trackedFrame=this->TrackedFrameList->GetTrackedFrame(frameNumber);
  if (trackedFrame==NULL)
  {
    LOG_ERROR("Cannot access frame "<<frameNumber);
    return PLUS_FAIL;
  }
  trackedFrame->SetCustomFrameField( fieldName, fieldValue );     
  return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
bool vtkMetaImageSequenceIO::SetCustomString(const char* fieldName, const char* fieldValue)
{
  if (fieldName==NULL)
  {
    LOG_ERROR("Invalid field name");
    return PLUS_FAIL;
  }
  this->TrackedFrameList->SetCustomString(fieldName, fieldValue); 
  return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
const char* vtkMetaImageSequenceIO::GetCustomString(const char* fieldName)
{
  if (fieldName==NULL)
  {
    LOG_ERROR("Invalid field name or value");
    return NULL;
  }
  return this->TrackedFrameList->GetCustomString(fieldName); 
}

//----------------------------------------------------------------------------
void vtkMetaImageSequenceIO::PrintSelf(ostream& os, vtkIndent indent)
{
  os << indent << "Metadata User Fields:" << std::endl;
  this->TrackedFrameList->PrintSelf(os, indent);
}

//----------------------------------------------------------------------------
PlusStatus vtkMetaImageSequenceIO::ReadImageHeader()
{
  FILE *stream=NULL;
  // open in binary mode because we determine the start of the image buffer also during this read
  if ( FileOpen(&stream, this->FileName, "rb" ) != PLUS_SUCCESS )
  {
    LOG_ERROR("The file "<<this->FileName<<" could not be opened for reading");
    return PLUS_FAIL;
  }

  char line[MAX_LINE_LENGTH+1]={0};
  while (fgets( line, MAX_LINE_LENGTH, stream ))
  {

    std::string lineStr=line;

    // Split line into name and value
    size_t equalSignFound;
    equalSignFound=lineStr.find_first_of("=");
    if (equalSignFound==std::string::npos)
    {
      LOG_WARNING("Parsing line failed, equal sign is missing ("<<lineStr<<")");
      continue;
    }
    std::string name=lineStr.substr(0,equalSignFound);
    std::string value=lineStr.substr(equalSignFound+1);

    // trim spaces from the left and right
    PlusCommon::Trim(name);
    PlusCommon::Trim(value);

    if (name.compare(0,SEQMETA_FIELD_FRAME_FIELD_PREFIX.size(),SEQMETA_FIELD_FRAME_FIELD_PREFIX)!=0)
    {
      // field
      SetCustomString(name.c_str(), value.c_str());

      // Arrived to ElementDataFile, this is the last element
      if (name.compare(SEQMETA_FIELD_ELEMENT_DATA_FILE)==0)
      {
        SetPixelDataFileName(value.c_str());
        if (value.compare(SEQMETA_FIELD_VALUE_ELEMENT_DATA_FILE_LOCAL)==0)
        {
          // pixel data stored locally
          this->PixelDataFileOffset=FTELL(stream);
        }
        else
        {
          // pixel data stored in separate file
          this->PixelDataFileOffset=0;
        }
        // this is the last element of the header
        break;
      }
    }
    else
    {
      // frame field
      // name: Seq_Frame0000_CustomTransform
      name.erase(0,SEQMETA_FIELD_FRAME_FIELD_PREFIX.size()); // 0000_CustomTransform

      // Split line into name and value
      size_t underscoreFound;
      underscoreFound=name.find_first_of("_");
      if (underscoreFound==std::string::npos)
      {
        LOG_WARNING("Parsing line failed, underscore is missing from frame field name ("<<lineStr<<")");
        continue;
      }
      std::string frameNumberStr=name.substr(0,underscoreFound); // 0000
      std::string frameFieldName=name.substr(underscoreFound+1); // CustomTransform

      int frameNumber=0;
      if (PlusCommon::StringToInt(frameNumberStr.c_str(),frameNumber)!=PLUS_SUCCESS)
      {
        LOG_WARNING("Parsing line failed, cannot get frame number from frame field ("<<lineStr<<")");
        continue;
      }
      SetCustomFrameString(frameNumber, frameFieldName.c_str(), value.c_str());

      if (ferror(stream))
      {
        LOG_ERROR("Error reading the file "<<this->FileName);
        break;
      }
      if (feof(stream))
      {
        break;
      }
    }
  } 

  fclose( stream );

  const char* binaryDataFieldValue=this->TrackedFrameList->GetCustomString("BinaryData");
  if (binaryDataFieldValue!=NULL)
  {
    if(STRCASECMP(binaryDataFieldValue,"true")==0)
    {
      this->FileType=itk::ImageIOBase::Binary;
    }
    else
    {
      this->FileType=itk::ImageIOBase::ASCII;
    }
  }
  else
  {
    LOG_WARNING("BinaryData field has not been found in "<<this->FileName<<". Assume binary data.");
    this->FileType=itk::ImageIOBase::Binary;
  }
  
  if(this->TrackedFrameList->GetCustomString("CompressedData")!=NULL
	  && STRCASECMP(this->TrackedFrameList->GetCustomString("CompressedData"),"true")==0)
  {
    SetUseCompression(true);
  }
  else
  {
    SetUseCompression(false);
  }

  int numberOfComponents=1;  
  if (this->TrackedFrameList->GetCustomString("ElementNumberOfChannels")!=NULL)
  {
    // this field is optional
    PlusCommon::StringToInt(this->TrackedFrameList->GetCustomString("ElementNumberOfChannels"), numberOfComponents);
    if (numberOfComponents!=1)
    {
      LOG_ERROR("Images images with ElementNumberOfChannels=1 are supported. This image has "<<numberOfComponents<<" channels.");
      return PLUS_FAIL;
    }
  }

  std::string elementTypeStr=this->TrackedFrameList->GetCustomString("ElementType");
  if (ConvertMetaElementTypeToItkPixelType(elementTypeStr.c_str(), this->PixelType)!=PLUS_SUCCESS)
  {
    LOG_ERROR("Unknown component type: "<<elementTypeStr);
    return PLUS_FAIL;
  }

  int nDims=3;
  if (PlusCommon::StringToInt(this->TrackedFrameList->GetCustomString("NDims"), nDims)==PLUS_SUCCESS)
  {
    if (nDims!=2 && nDims!=3)
    {
      LOG_ERROR("Invalid dimension (shall be 2 or 3): "<<nDims);
      return PLUS_FAIL;
    }
  }
  this->NumberOfDimensions=nDims;  

  this->ImageOrientationInFile = PlusVideoFrame::GetUsImageOrientationFromString(GetCustomString(SEQMETA_FIELD_US_IMG_ORIENT)); 

  const char* imgTypeStr=GetCustomString(SEQMETA_FIELD_US_IMG_TYPE);
  if (imgTypeStr==NULL)
  {
    // if the image type is not defined then assume that it is B-mode image
    this->ImageType=US_IMG_BRIGHTNESS;
  }
  else
  {
    this->ImageType = PlusVideoFrame::GetUsImageTypeFromString(GetCustomString(SEQMETA_FIELD_US_IMG_TYPE)); 
  }

  // If no specific image orientation is requested then determine it automatically from the image type
  // B-mode: MF
  // RF-mode: FM
  if (this->ImageOrientationInMemory==US_IMG_ORIENT_XX)
  {
    switch (this->ImageType)
    {
    case US_IMG_BRIGHTNESS:
      SetImageOrientationInMemory(US_IMG_ORIENT_MF);
      break;
    case US_IMG_RF_I_LINE_Q_LINE:
    case US_IMG_RF_IQ_LINE:
    case US_IMG_RF_REAL:
      SetImageOrientationInMemory(US_IMG_ORIENT_FM);
      break;
    default:
      LOG_WARNING("Cannot determine image orientation automatically, unknown image type "<<this->ImageType<<", use the same orientation in memory as in the file");
      SetImageOrientationInMemory(this->ImageOrientationInFile);
    }
  }

  std::istringstream issDimSize(this->TrackedFrameList->GetCustomString("DimSize")); // DimSize = 640 480 567
  for(int i=0; i<3; i++)
  {
    int dimSize=0;
    if (i<this->NumberOfDimensions)
    {
      issDimSize >> dimSize;    
    }
    this->Dimensions[i]=dimSize;
  } 

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
// Read the spacing and dimentions of the image.
PlusStatus vtkMetaImageSequenceIO::ReadImagePixels()
{ 
  int numberOfErrors=0;

  FILE *stream=NULL;

  if ( FileOpen( &stream, GetPixelDataFilePath().c_str(), "rb" ) != PLUS_SUCCESS )
  {
    LOG_ERROR("The file "<<GetPixelDataFilePath()<<" could not be opened for reading");
    return PLUS_FAIL;
  }

  int frameCount=this->Dimensions[2];
  unsigned int frameSizeInBytes=this->Dimensions[0]*this->Dimensions[1]*PlusVideoFrame::GetNumberOfBytesPerPixel(this->PixelType);
  
  std::vector<unsigned char> allFramesPixelBuffer;
  if (this->UseCompression)
  {    
    unsigned int allFramesPixelBufferSize=frameCount*frameSizeInBytes;
    allFramesPixelBuffer.resize(allFramesPixelBufferSize);

    unsigned int allFramesCompressedPixelBufferSize=0;
    PlusCommon::StringToInt(this->TrackedFrameList->GetCustomString("CompressedDataSize"), allFramesCompressedPixelBufferSize);
    std::vector<unsigned char> allFramesCompressedPixelBuffer;
    allFramesCompressedPixelBuffer.resize(allFramesCompressedPixelBufferSize);

    FSEEK(stream, this->PixelDataFileOffset, SEEK_SET);    
    if (fread(&(allFramesCompressedPixelBuffer[0]), 1, allFramesCompressedPixelBufferSize, stream)!=allFramesCompressedPixelBufferSize)
    {
      LOG_ERROR("Could not read "<<allFramesCompressedPixelBufferSize<<" bytes from "<<GetPixelDataFilePath());
      fclose( stream );
      return PLUS_FAIL;
    }

    uLongf unCompSize = allFramesPixelBufferSize;
    if (uncompress((Bytef*)&(allFramesPixelBuffer[0]), &unCompSize, (const Bytef*)&(allFramesCompressedPixelBuffer[0]), allFramesCompressedPixelBufferSize)!=Z_OK)
    {
      LOG_ERROR("Cannot uncompress the pixel data");
      fclose( stream );
      return PLUS_FAIL;
    }
    if (unCompSize!=allFramesPixelBufferSize)
    {
      LOG_ERROR("Cannot uncompress the pixel data: uncompressed data is less than expected");
      fclose( stream );
      return PLUS_FAIL;
    }
 
  }

  std::vector<unsigned char> pixelBuffer;
  pixelBuffer.resize(frameSizeInBytes);
  for (int frameNumber=0; frameNumber<frameCount; frameNumber++)
  {
    CreateTrackedFrameIfNonExisting(frameNumber);    
    TrackedFrame* trackedFrame=this->TrackedFrameList->GetTrackedFrame(frameNumber);    
    
    // Allocate frame only if it is valid 
    const char* imgStatus = trackedFrame->GetCustomFrameField(SEQMETA_FIELD_IMG_STATUS.c_str()); 
    if ( imgStatus != NULL  ) // Found the image status field 
    { 
      // Save status field 
      std::string strImgStatus(imgStatus); 

      // Delete image status field from tracked frame 
      // Image status can be determine by trackedFrame->GetImageData()->IsImageValid()
      trackedFrame->DeleteCustomFrameField(SEQMETA_FIELD_IMG_STATUS.c_str()); 

      if ( STRCASECMP(strImgStatus.c_str(), "OK") != 0 )// Image status _not_ OK 
      {
      LOG_DEBUG("Frame #" << frameNumber << " image data is invalid, no need to allocate data in the tracked frame list."); 
      continue; 
      }
    }

    trackedFrame->GetImageData()->SetImageOrientation(this->ImageOrientationInMemory);
    trackedFrame->GetImageData()->SetImageType(this->ImageType);

    if (trackedFrame->GetImageData()->AllocateFrame(this->Dimensions, this->PixelType)!=PLUS_SUCCESS)
    {
      LOG_ERROR("Cannot allocate memory for frame "<<frameNumber);
      numberOfErrors++;
      continue;
    }    
    if (!this->UseCompression)
    {
      FilePositionOffsetType offset=PixelDataFileOffset+frameNumber*frameSizeInBytes;
      FSEEK(stream, offset, SEEK_SET);
      if (fread(&(pixelBuffer[0]), 1, frameSizeInBytes, stream)!=frameSizeInBytes)
      {
        //LOG_ERROR("Could not read "<<frameSizeInBytes<<" bytes from "<<GetPixelDataFilePath());
        //numberOfErrors++;
      }
      if ( PlusVideoFrame::GetOrientedImage(&(pixelBuffer[0]), this->ImageOrientationInFile, this->Dimensions, this->PixelType, this->ImageOrientationInMemory, *trackedFrame->GetImageData()) != PLUS_SUCCESS )
      {
        LOG_ERROR("Failed to get oriented image from sequence metafile (frame number: " << frameNumber << ")!"); 
        numberOfErrors++;
        continue; 
      }
    }
    else
    {
      if ( PlusVideoFrame::GetOrientedImage(&(allFramesPixelBuffer[0])+frameNumber*frameSizeInBytes, this->ImageOrientationInFile, this->Dimensions, this->PixelType, this->ImageOrientationInMemory, *trackedFrame->GetImageData()) != PLUS_SUCCESS )
      {
        LOG_ERROR("Failed to get oriented image from sequence metafile (frame number: " << frameNumber << ")!"); 
        numberOfErrors++;
        continue; 
      }
    }
  }

  fclose( stream );

  if (numberOfErrors>0)
  {
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkMetaImageSequenceIO::Write() 
{
  if( this->PrepareHeader() != PLUS_SUCCESS )
  {
    LOG_ERROR("Unable to prepare the header.");
    return PLUS_FAIL;
  }
  if( this->AppendImagesToHeader() != PLUS_SUCCESS )
  {
    LOG_ERROR("Unable to append images to the header.");
    return PLUS_FAIL;
  }
  if( this->FinalizeHeader() != PLUS_SUCCESS )
  {
    LOG_ERROR("Unable to finalize the header.");
    return PLUS_FAIL;
  }

  if (WriteImagePixels(this->TempImageFileName) != PLUS_SUCCESS)
  {
    return PLUS_FAIL;
  }

  this->Close();

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
void vtkMetaImageSequenceIO::CreateTrackedFrameIfNonExisting(unsigned int frameNumber)
{
  if (frameNumber<this->TrackedFrameList->GetNumberOfTrackedFrames())
  {
    // frame is already created
    return;
  }
  TrackedFrame emptyFrame;
  for (unsigned int i=this->TrackedFrameList->GetNumberOfTrackedFrames(); i<frameNumber+1; i++)
  {
    this->TrackedFrameList->AddTrackedFrame(&emptyFrame, vtkTrackedFrameList::ADD_INVALID_FRAME);
  }
}

//----------------------------------------------------------------------------
bool vtkMetaImageSequenceIO::CanReadFile(const char*)
{
  FILE *stream=NULL;
  // open in binary mode because we determine the start of the image buffer also during this read
  if ( FileOpen( &stream, this->FileName, "rb" ) != PLUS_SUCCESS )
  {
    LOG_DEBUG("The file "<<this->FileName<<" could not be opened for reading");
    return false;
  }
  char line[MAX_LINE_LENGTH+1]={0};
  fgets( line, MAX_LINE_LENGTH, stream );
  fclose( stream );

  // the first line in the file should be:
  // ObjectType = Image

  std::string lineStr=line;

  // Split line into name and value
  size_t equalSignFound;
  equalSignFound=lineStr.find_first_of("=");
  if (equalSignFound==std::string::npos)
  {
    LOG_DEBUG("Parsing line failed, equal sign is missing ("<<lineStr<<")");
    return false;
  }
  std::string name=lineStr.substr(0,equalSignFound);
  std::string value=lineStr.substr(equalSignFound);

  // trim spaces from the left and right
  PlusCommon::Trim(name);
  PlusCommon::Trim(value);

  if (name.compare("ObjectType")!=0)
  {
    LOG_DEBUG("Expect ObjectType field name in the first field");
    return false;
  }
  if (value.compare("Image")!=0)
  {
    LOG_DEBUG("Expect Image value name in the first field");
    return false;
  }

  return true;
}

//----------------------------------------------------------------------------
PlusStatus vtkMetaImageSequenceIO::Read()
{
  this->TrackedFrameList->Clear();

  if (ReadImageHeader()!=PLUS_SUCCESS)
  {
    LOG_ERROR("Could not load header from file: " << this->FileName);
    return PLUS_FAIL;
  }

  if (ReadImagePixels()!=PLUS_SUCCESS)
  {
    return PLUS_FAIL;
  }
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
/** Writes the spacing and dimentions of the image.
* Assumes SetFileName has been called with a valid file name. */
PlusStatus vtkMetaImageSequenceIO::OpenImageHeader()
{
  // Override fields
  SetCustomString("NDims", "3");
  SetCustomString("BinaryData", "True");
  SetCustomString("BinaryDataByteOrderMSB", "False");

  // CompressedData
  if (GetUseCompression())
  {
    SetCustomString("CompressedData", "True");
    SetCustomString("CompressedDataSize", "0                "); // add spaces so that later the field can be updated with larger values
  }
  else
  {
    SetCustomString("CompressedData", "False");
    SetCustomString("CompressedDataSize", NULL);
  }

  int frameSize[2]={0};
  this->GetMaximumImageDimensions(frameSize); 

  // Make sure the frame size is the same for each valid image 
  // If it's needed, we can use the largest frame size for each frame and copy the image data row by row 
  // but then, we need to save the original frame size for each frame and crop the image when we read it 
  for (unsigned int frameNumber=0; frameNumber<this->TrackedFrameList->GetNumberOfTrackedFrames(); frameNumber++)
  {
    int * currFrameSize = this->TrackedFrameList->GetTrackedFrame(frameNumber)->GetFrameSize(); 
    if ( this->TrackedFrameList->GetTrackedFrame(frameNumber)->GetImageData()->IsImageValid() 
      && ( frameSize[0] != currFrameSize[0] || frameSize[1] != currFrameSize[1] )  )
    {
      LOG_ERROR("Frame size mismatch: expected size (" << frameSize[0] << "x" << frameSize[1] 
      << ") differ from actual size (" << currFrameSize[0] << "x" << currFrameSize[1] << ") for frame #" << frameNumber); 
      return PLUS_FAIL; 
    }
  }

  // DimSize
  std::ostringstream dimSizeStr; 
  this->Dimensions[0]=frameSize[0];
  this->Dimensions[1]=frameSize[1];
  this->Dimensions[2]=this->TrackedFrameList->GetNumberOfTrackedFrames();
  dimSizeStr << this->Dimensions[0] << " " << this->Dimensions[1] << " " << this->Dimensions[2];
  dimSizeStr << "                              ";  // add spaces so that later the field can be updated with larger values
  SetCustomString("DimSize", dimSizeStr.str().c_str());  

  // PixelType
  this->PixelType=this->TrackedFrameList->GetPixelType();
  if ( this->PixelType == itk::ImageIOBase::UNKNOWNCOMPONENTTYPE )
  {
    // If the pixel type was not defined, define it to UCHAR
    this->PixelType = itk::ImageIOBase::UCHAR; 
  }
  std::string pixelTypeStr;
  vtkMetaImageSequenceIO::ConvertItkPixelTypeToMetaElementType(this->PixelType, pixelTypeStr);
  SetCustomString("ElementType", pixelTypeStr.c_str());  // pixel type (a.k.a component type) is stored in the ElementType element

  // Image orientation
  std::string orientationStr=PlusVideoFrame::GetStringFromUsImageOrientation(this->ImageOrientationInFile);
  SetCustomString(SEQMETA_FIELD_US_IMG_ORIENT, orientationStr.c_str());

  // Image type
  std::string typeStr=PlusVideoFrame::GetStringFromUsImageType(this->ImageType);
  SetCustomString(SEQMETA_FIELD_US_IMG_TYPE, typeStr.c_str());

  // Add fields with default values if they are not present already
  if (GetCustomString("TransformMatrix")==NULL) { SetCustomString("TransformMatrix", "1 0 0 0 1 0 0 0 1"); }
  if (GetCustomString("Offset")==NULL) { SetCustomString("Offset", "0 0 0"); }
  if (GetCustomString("CenterOfRotation")==NULL) { SetCustomString("CenterOfRotation", "0 0 0"); }
  if (GetCustomString("ElementSpacing")==NULL) { SetCustomString("ElementSpacing", "1 1 1"); }
  if (GetCustomString("AnatomicalOrientation")==NULL) { SetCustomString("AnatomicalOrientation", "RAI"); }

  std::string fileExt=vtksys::SystemTools::GetFilenameLastExtension(this->FileName);
  if (STRCASECMP(fileExt.c_str(),".mha")==0)
  {
    SetPixelDataFileName(SEQMETA_FIELD_VALUE_ELEMENT_DATA_FILE_LOCAL);
  }
  else if (STRCASECMP(fileExt.c_str(),".mhd")==0)
  {
    std::string pixFileName=vtksys::SystemTools::GetFilenameWithoutExtension(this->FileName);
    if (this->UseCompression)
    {
      pixFileName+=".zraw";
    }
    else
    {
      pixFileName+=".raw";
    }

    SetPixelDataFileName(pixFileName.c_str());
  }
  else
  {
    LOG_ERROR("Writing sequence metafile with "<<fileExt<<" extension is not supported");
    return PLUS_FAIL;
  }

  FILE *stream=NULL;
  // open in binary mode because we determine the start of the image buffer also during this read
  if ( FileOpen( &stream, this->TempHeaderFileName, "wb" ) != PLUS_SUCCESS )
  {
    LOG_ERROR("The file " << this->TempHeaderFileName << " could not be opened for writing");
    return PLUS_FAIL;
  }

  // The header shall start with these two fields
  const char* objType = "ObjectType = Image\n";
  fputs(objType, stream);
  m_TotalBytesWritten += strlen(objType);
  const char* nDims = "NDims = 3\n";
  fputs(nDims, stream);
  m_TotalBytesWritten += strlen(nDims);

  std::vector<std::string> fieldNames;
  this->TrackedFrameList->GetCustomFieldNameList(fieldNames);
  for (std::vector<std::string>::iterator it=fieldNames.begin(); it != fieldNames.end(); it++) 
  {
    if (it->compare("ObjectType")==0) continue; // this must be the first element
    if (it->compare("NDims")==0) continue; // this must be the second element
    if (it->compare("ElementDataFile")==0) continue; // this must be the last element
    std::string field=(*it)+" = "+GetCustomString(it->c_str())+"\n";
    fputs(field.c_str(), stream);
    m_TotalBytesWritten += field.length();
  }

  fclose(stream);

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkMetaImageSequenceIO::AppendImagesToHeader()
{
  FILE *stream=NULL;
  // open in binary mode because we determine the start of the image buffer also during this read
  if ( FileOpen( &stream, this->TempHeaderFileName, "ab+" ) != PLUS_SUCCESS )
  {
    LOG_ERROR("The file " << this->TempHeaderFileName << " could not be opened for writing");
    return PLUS_FAIL;
  }

  // Write frame fields (Seq_Frame0000_... = ...)
  for (unsigned int frameNumber = m_CurrentFrameOffset; frameNumber < this->TrackedFrameList->GetNumberOfTrackedFrames() + m_CurrentFrameOffset; frameNumber++)
  {
    unsigned int adjustedFrameNumber = frameNumber - m_CurrentFrameOffset;
    TrackedFrame* trackedFrame=this->TrackedFrameList->GetTrackedFrame(adjustedFrameNumber);

    std::ostringstream frameIndexStr; 
    frameIndexStr << std::setfill('0') << std::setw(4) << frameNumber; 

    std::vector<std::string> fieldNames;
    trackedFrame->GetCustomFrameFieldNameList(fieldNames);

    for (std::vector<std::string>::iterator it=fieldNames.begin(); it != fieldNames.end(); it++) 
    {
      std::string field=SEQMETA_FIELD_FRAME_FIELD_PREFIX + frameIndexStr.str() + "_" + (*it) + " = " + trackedFrame->GetCustomFrameField(it->c_str()) + "\n";
      fputs(field.c_str(), stream);
      m_TotalBytesWritten += field.length();
    }

    // Add image status field 
    std::string imageStatus("OK"); 
    if ( !trackedFrame->GetImageData()->IsImageValid() )
    {
      imageStatus="INVALID"; 
    }
    std::string imgStatusField=SEQMETA_FIELD_FRAME_FIELD_PREFIX + frameIndexStr.str() + "_" + SEQMETA_FIELD_IMG_STATUS + " = " + imageStatus + "\n";
    fputs(imgStatusField.c_str(), stream);
    m_TotalBytesWritten += imgStatusField.length();
  }

  fclose(stream);

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkMetaImageSequenceIO::FinalizeHeader()
{
  FILE* stream = NULL;
  // open in binary mode because we determine the start of the image buffer also during this read
  if ( FileOpen( &stream, this->TempHeaderFileName, "ab+" ) != PLUS_SUCCESS )
  {
    LOG_ERROR("The file " << this->TempHeaderFileName << " could not be opened for writing");
    return PLUS_FAIL;
  }
  const char* elem = "ElementDataFile = ";
  fputs(elem, stream);
  m_TotalBytesWritten += strlen(elem);
  fputs(this->PixelDataFileName, stream);
  m_TotalBytesWritten += strlen(this->PixelDataFileName);
  fputs("\n", stream);
  m_TotalBytesWritten += 1;

  fclose(stream);

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
void vtkMetaImageSequenceIO::GetMaximumImageDimensions(int maxFrameSize[2])
{
  maxFrameSize[0]=0;
  maxFrameSize[1]=0;

  for (unsigned int frameNumber=0; frameNumber<this->TrackedFrameList->GetNumberOfTrackedFrames(); frameNumber++)
  {
    int * currFrameSize = this->TrackedFrameList->GetTrackedFrame(frameNumber)->GetFrameSize(); 
    if ( maxFrameSize[0] < currFrameSize[0] )
    {
      maxFrameSize[0] = currFrameSize[0]; 
    }

    if ( maxFrameSize[1] < currFrameSize[1] )
    {
      maxFrameSize[1] = currFrameSize[1]; 
    }
  }
}

//----------------------------------------------------------------------------
PlusStatus vtkMetaImageSequenceIO::WriteImagePixels(char * aFilename, bool forceAppend /* = false */)
{
  if (this->ImageOrientationInFile!=this->TrackedFrameList->GetImageOrientation())
  {
    // Reordering of the frames is not implemented, so return with an error
    LOG_ERROR("Saving of images is supported only in the same orientation as currently in the memory");
    return PLUS_FAIL;
  }

  FILE *stream=NULL;

  std::string fileOpenMode="wb"; // w (write, existing file is destroyed), b (binary)
  if ( forceAppend && !GetUseCompression())
  {
    // Pixel data is stored locally in the header file (MHA file), so we append the image data to an existing file
    // Or this sequence is being written to in chunks
    fileOpenMode="ab+"; // a+ (append to the end of the file), b (binary)
  }
  else if( forceAppend && GetUseCompression())
  {
    LOG_ERROR("Unable to append images when compression is used. You must write uncompressed and then post-compress.");
    return PLUS_FAIL;
  }
  if ( FileOpen( &stream, aFilename, fileOpenMode.c_str() ) != PLUS_SUCCESS )
  {
    LOG_ERROR("The file " << aFilename << " could not be opened for writing");
    return PLUS_FAIL;
  }

  if ( this->PixelType == itk::ImageIOBase::UNKNOWNCOMPONENTTYPE )
  {
    // If the pixel type was not defined, define it to UCHAR
    this->PixelType = itk::ImageIOBase::UCHAR; 
  }

  PlusStatus result = PLUS_SUCCESS;
  if (!GetUseCompression())
  {
    // Create a blank frame if we have to write an invalid frame to metafile 
    PlusVideoFrame blankFrame; 
    if ( blankFrame.AllocateFrame( this->Dimensions, this->PixelType)!=PLUS_SUCCESS)
    {
      LOG_ERROR("Failed to allocate space for blank image."); 
      return PLUS_FAIL; 
    }
    blankFrame.FillBlank(); 

    // not compressed
    for (unsigned int frameNumber=0; frameNumber<this->TrackedFrameList->GetNumberOfTrackedFrames(); frameNumber++)
    {
      TrackedFrame* trackedFrame=this->TrackedFrameList->GetTrackedFrame(frameNumber);

      PlusVideoFrame* videoFrame = &blankFrame; 
      if ( trackedFrame->GetImageData()->IsImageValid() ) 
      {
        videoFrame = trackedFrame->GetImageData(); 
      }

      fwrite(videoFrame->GetBufferPointer(), 1, videoFrame->GetFrameSizeInBytes(), stream);
      m_TotalBytesWritten += videoFrame->GetFrameSizeInBytes();
    }
  }
  else
  {
    // compressed
    int compressedDataSize=0;
    result = WriteCompressedImagePixelsToFile(stream, compressedDataSize);
    if( result == PLUS_SUCCESS )
    {
      m_TotalBytesWritten += compressedDataSize;
    }
    std::ostringstream compressedDataSizeStr; 
    compressedDataSizeStr << compressedDataSize; 
    SetCustomString("CompressedDataSize", compressedDataSizeStr.str().c_str());
  }

  fclose(stream);

  if( result == PLUS_SUCCESS )
  {
    m_CurrentFrameOffset += TrackedFrameList->GetNumberOfTrackedFrames();
  }
  return result;
}

//----------------------------------------------------------------------------
PlusStatus vtkMetaImageSequenceIO::WriteCompressedImagePixelsToFile(FILE *outputFileStream, int &compressedDataSize)
{
  LOG_DEBUG("Writing compressed pixel data into file started");

  compressedDataSize=0;

  const int outputBufferSize=16384; // can be any number, just picked a value from a zlib example
  unsigned char outputBuffer[outputBufferSize];
  
  z_stream strm; // stream describing the compression state

  // use the default memory allocation routines
  strm.zalloc = Z_NULL;
  strm.zfree = Z_NULL;
  strm.opaque = Z_NULL;
  int ret=deflateInit(&strm, Z_DEFAULT_COMPRESSION);
  if (ret!=Z_OK)
  {
    LOG_ERROR("Image compression initialization failed (errorCode="<<ret<<")");
    return PLUS_FAIL;
  }

  // Create a blank frame if we have to write an invalid frame to metafile 
  PlusVideoFrame blankFrame; 
  if ( blankFrame.AllocateFrame( this->Dimensions, this->PixelType)!=PLUS_SUCCESS)
  {
    LOG_ERROR("Failed to allocate space for blank image."); 
    return PLUS_FAIL; 
  }
  blankFrame.FillBlank(); 

  for (unsigned int frameNumber=0; frameNumber<this->TrackedFrameList->GetNumberOfTrackedFrames(); frameNumber++)
  {
    TrackedFrame* trackedFrame=this->TrackedFrameList->GetTrackedFrame(frameNumber);
    if (trackedFrame==NULL)
    {
      LOG_ERROR("Cannot access frame "<<frameNumber<<" while trying to writing compress data into file");
      deflateEnd(&strm);
      return PLUS_FAIL;
    }

    PlusVideoFrame* videoFrame = &blankFrame; 
    if ( trackedFrame->GetImageData()->IsImageValid() ) 
    {
      videoFrame = trackedFrame->GetImageData(); 
    }

    strm.next_in=(Bytef*)videoFrame->GetBufferPointer();
    strm.avail_in=videoFrame->GetFrameSizeInBytes();

    // Note: it's possible to request to consume all inputs and delete all history after each frame writing to allow random access
    int flush = (frameNumber<this->TrackedFrameList->GetNumberOfTrackedFrames()-1) ? Z_NO_FLUSH : Z_FINISH;

    // run deflate() on input until output buffer not full, finish
    // compression if all of source has been read in
    do 
    {
      strm.avail_out = outputBufferSize;
      strm.next_out = outputBuffer;

      ret = deflate(&strm, flush);    /* no bad return value */
      if (ret == Z_STREAM_ERROR)
      {
        // state clobbered
        LOG_ERROR("Zlib state became invalid during the compression process (errorCode="<<ret<<")");
        deflateEnd(&strm); // clean up
        return PLUS_FAIL;
      }

      size_t numberOfBytesReadyForWriting = outputBufferSize - strm.avail_out;
      if (fwrite(outputBuffer, 1, numberOfBytesReadyForWriting, outputFileStream) != numberOfBytesReadyForWriting || ferror(outputFileStream))
      {        
        LOG_ERROR("Error writing compressed data into file");
        deflateEnd(&strm); // clean up
        return PLUS_FAIL;
      }
      compressedDataSize+=numberOfBytesReadyForWriting;

    } while (strm.avail_out == 0);

    if (strm.avail_in != 0)
    {
      // state clobbered (by now all input should have been consumed)
      LOG_ERROR("Zlib state became invalid during the compression process");
      deflateEnd(&strm); // clean up
      return PLUS_FAIL;
    }
  }
  
  deflateEnd(&strm); // clean up

  LOG_DEBUG("Writing compressed pixel data into file completed");

  if (ret != Z_STREAM_END)
  {
    LOG_ERROR("Error occurred during compressing image data into file");
    return PLUS_FAIL;
  }
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
TrackedFrame* vtkMetaImageSequenceIO::GetTrackedFrame(int frameNumber)
{
  TrackedFrame* trackedFrame=this->TrackedFrameList->GetTrackedFrame(frameNumber);
  return trackedFrame;
}

//----------------------------------------------------------------------------
PlusStatus vtkMetaImageSequenceIO::ConvertMetaElementTypeToItkPixelType(const std::string &elementTypeStr, PlusCommon::ITKScalarPixelType &itkPixelType)
{
  if (elementTypeStr.compare("MET_OTHER")==0
    || elementTypeStr.compare("MET_NONE")==0
    || elementTypeStr.empty())
  {
    itkPixelType=itk::ImageIOBase::UNKNOWNCOMPONENTTYPE;
  }
  else if (elementTypeStr.compare("MET_CHAR")==0) { itkPixelType=itk::ImageIOBase::CHAR; }
  else if (elementTypeStr.compare("MET_ASCII_CHAR")==0) { itkPixelType=itk::ImageIOBase::CHAR; }
  else if (elementTypeStr.compare("MET_UCHAR")==0) { itkPixelType=itk::ImageIOBase::UCHAR; }
  else if (elementTypeStr.compare("MET_SHORT")==0) { itkPixelType=itk::ImageIOBase::SHORT; }
  else if (elementTypeStr.compare("MET_USHORT")==0) { itkPixelType=itk::ImageIOBase::USHORT; }
  else if (elementTypeStr.compare("MET_INT")==0) { itkPixelType=itk::ImageIOBase::INT; }
  else if (elementTypeStr.compare("MET_UINT")==0)
  {
    if(sizeof(unsigned int) == MET_ValueTypeSize[MET_UINT])
    {
      itkPixelType=itk::ImageIOBase::UINT;
    }
    else if(sizeof(unsigned long) == MET_ValueTypeSize[MET_UINT])
    {
      itkPixelType=itk::ImageIOBase::ULONG;
    }
  }
  else if (elementTypeStr.compare("MET_LONG")==0)
  {
    if(sizeof(unsigned int) == MET_ValueTypeSize[MET_LONG])
    {
      itkPixelType=itk::ImageIOBase::LONG;
    }
    else if(sizeof(unsigned long) == MET_ValueTypeSize[MET_UINT])
    {
      itkPixelType=itk::ImageIOBase::INT;
    }
  }
  else if (elementTypeStr.compare("MET_ULONG")==0)
  {
    if(sizeof(unsigned long) == MET_ValueTypeSize[MET_ULONG])
    {
      itkPixelType=itk::ImageIOBase::ULONG;
    }
    else if(sizeof(unsigned int) == MET_ValueTypeSize[MET_ULONG])
    {
      itkPixelType=itk::ImageIOBase::UINT;
    }
  }
  else if (elementTypeStr.compare("MET_LONG_LONG")==0)
  {
    if(sizeof(long) == MET_ValueTypeSize[MET_LONG_LONG])
    {
      itkPixelType=itk::ImageIOBase::LONG;
    }
    else if(sizeof(int) == MET_ValueTypeSize[MET_LONG_LONG])
    {
      itkPixelType=itk::ImageIOBase::INT;
    }
    else 
    {
      itkPixelType=itk::ImageIOBase::UNKNOWNCOMPONENTTYPE;
    }
  }
  else if (elementTypeStr.compare("MET_ULONG_LONG")==0)
  {
    if(sizeof(unsigned long) == MET_ValueTypeSize[MET_ULONG_LONG])
    {
      itkPixelType=itk::ImageIOBase::ULONG;
    }
    else if(sizeof(unsigned int) == MET_ValueTypeSize[MET_ULONG_LONG])
    {
      itkPixelType=itk::ImageIOBase::UINT;
    }
    else 
    {
      itkPixelType=itk::ImageIOBase::UNKNOWNCOMPONENTTYPE;
    }
  }   
  else if (elementTypeStr.compare("MET_FLOAT")==0)
  {
    if(sizeof(float) == MET_ValueTypeSize[MET_FLOAT])
    {
      itkPixelType=itk::ImageIOBase::FLOAT;
    }
    else if(sizeof(double) == MET_ValueTypeSize[MET_FLOAT])
    {
      itkPixelType=itk::ImageIOBase::DOUBLE;
    }
  }  
  else if (elementTypeStr.compare("MET_DOUBLE")==0)
  {
    itkPixelType=itk::ImageIOBase::DOUBLE;
    if(sizeof(double) == MET_ValueTypeSize[MET_DOUBLE])
    {
      itkPixelType=itk::ImageIOBase::DOUBLE;
    }
    else if(sizeof(float) == MET_ValueTypeSize[MET_DOUBLE])
    {
      itkPixelType=itk::ImageIOBase::FLOAT;
    }
  }
  else
  {
    LOG_ERROR("Unknown component type: "<<elementTypeStr);
    itkPixelType=itk::ImageIOBase::UNKNOWNCOMPONENTTYPE;
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkMetaImageSequenceIO::ConvertItkPixelTypeToMetaElementType(PlusCommon::ITKScalarPixelType itkPixelType, std::string &elementTypeStr)
{
  if (itkPixelType==itk::ImageIOBase::UNKNOWNCOMPONENTTYPE)
  {
    elementTypeStr="MET_OTHER";
    return PLUS_SUCCESS;
  }
  const char* metaElementTypes[]={
    "MET_CHAR",
    "MET_UCHAR",
    "MET_SHORT",
    "MET_USHORT",
    "MET_INT",
    "MET_UINT",
    "MET_LONG",
    "MET_ULONG",
    "MET_LONG_LONG",
    "MET_ULONG_LONG",
    "MET_FLOAT",
    "MET_DOUBLE",
  };
  
  PlusCommon::ITKScalarPixelType testedPixelType=itk::ImageIOBase::UNKNOWNCOMPONENTTYPE;
  for (unsigned int i=0; i<sizeof(metaElementTypes); i++)
  {    
    if (ConvertMetaElementTypeToItkPixelType(metaElementTypes[i], testedPixelType)!=PLUS_SUCCESS)
    {
      continue;
    }
    if (testedPixelType==itkPixelType)
    {
      elementTypeStr=metaElementTypes[i];
      return PLUS_SUCCESS;
    }
  }
  elementTypeStr="MET_OTHER";
  return PLUS_FAIL;
}

//----------------------------------------------------------------------------
std::string vtkMetaImageSequenceIO::GetPixelDataFilePath()
{
  if (STRCASECMP(SEQMETA_FIELD_VALUE_ELEMENT_DATA_FILE_LOCAL, this->PixelDataFileName)==0)
  {
    // LOCAL => data is stored in one file
    return this->FileName;
  }
  
  std::string dir=vtksys::SystemTools::GetFilenamePath(this->FileName);
  if (!dir.empty())
  {
    dir+="/";
  }
  std::string path=dir+this->PixelDataFileName;
  return path;
}

//----------------------------------------------------------------------------
PlusStatus vtkMetaImageSequenceIO::UpdateFieldInImageHeader(const char* fieldName)
{
  FILE *stream=NULL;
  // open in read+write binary mode
  if ( FileOpen( &stream, this->TempHeaderFileName, "r+b" ) != PLUS_SUCCESS )
  {
    LOG_ERROR("The file " << this->TempHeaderFileName << " could not be opened for reading and writing");
    return PLUS_FAIL;
  }

  fseek(stream, 0, SEEK_SET);

  char line[MAX_LINE_LENGTH+1]={0};
  while (fgets( line, MAX_LINE_LENGTH, stream ))
  {
    std::string lineStr=line;

    // Split line into name and value
    size_t equalSignFound;
    equalSignFound=lineStr.find_first_of("=");
    if (equalSignFound==std::string::npos)
    {
      LOG_WARNING("Parsing line failed, equal sign is missing ("<<lineStr<<")");
      continue;
    }
    std::string name=lineStr.substr(0,equalSignFound);
    PlusCommon::Trim(name);

    if (name.compare(fieldName)==0)
    {
      // found the field that has to be updated

      // construct a new line with the updated value
      std::ostringstream newLineStr; 
      newLineStr << name << " = " << GetCustomString(name.c_str());
      int paddingCharactersNeeded=lineStr.size()-newLineStr.str().size(); // need to add padding whitespace characters to fully replace the old line 
      if (paddingCharactersNeeded<0)
      {
        LOG_ERROR("Cannot update line in image header (the new string '"<<newLineStr<<"' is longer than the current string '"<<lineStr<<"')");
        fclose( stream );
        return PLUS_FAIL;
      }
      for (int i=0; i<paddingCharactersNeeded; i++);
      {        
        newLineStr << " ";
      }      
      // rewind to file pointer the first character of the line
      fseek(stream, -lineStr.size(), SEEK_CUR);

      // overwrite the old line
      if (fwrite(newLineStr.str().c_str(), 1, newLineStr.str().size(), stream)!=newLineStr.str().size())
      {
        LOG_ERROR("Cannot update line in image header (writing the updated line into the file failed)");
        fclose( stream );
        return PLUS_FAIL;
      }

      fclose( stream );
      return PLUS_SUCCESS;
    }

    if (ferror(stream))
    {
      LOG_ERROR("Error reading the file "<<this->FileName);
      break;
    }
    if (feof(stream))
    {
      break;
    }
  }

  fclose( stream );
  LOG_ERROR("Field "<<fieldName<<" is not found in the header file, update with new value is failed:"); 
  return PLUS_FAIL;
}

//----------------------------------------------------------------------------
PlusStatus vtkMetaImageSequenceIO::FileOpen(FILE **stream, const char* filename, const char* flags)
{
#ifdef _WIN32
  if (fopen_s(stream, filename, flags)!=0)
  {
    (*stream)=NULL;
  }
#else
  (*stream)=fopen(filename, flags);
#endif
  if ((*stream)==0)
  {
    return PLUS_FAIL;
  }
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkMetaImageSequenceIO::PrepareHeader()
{
  if (this->ImageOrientationInFile==US_IMG_ORIENT_XX)
  {
    // No specific orientation is requested, so just use the same as in the memory
    this->ImageOrientationInFile=this->TrackedFrameList->GetImageOrientation();
  }  
  if (this->ImageOrientationInFile!=this->TrackedFrameList->GetImageOrientation())
  {
    // Reordering of the frames is not implemented, so just save the images as they are in the memory
    LOG_WARNING("Saving of images is supported only in the same orientation as currently in the memory");
    this->ImageOrientationInFile=this->TrackedFrameList->GetImageOrientation();
  }

  if (this->ImageType == US_IMG_TYPE_XX)
  {
    // No specific type is requested, so just use the same as in the memory
    this->ImageType = this->TrackedFrameList->GetImageType();
  }
  if (this->ImageType!=this->TrackedFrameList->GetImageType())
  {
    // Reordering of the frames is not implemented, so just save the images as they are in the memory
    LOG_WARNING("Saving of images is supported only in the same type as currently in the memory");
    this->ImageType=this->TrackedFrameList->GetImageType();
  }

  if ( OpenImageHeader() != PLUS_SUCCESS)
  {
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkMetaImageSequenceIO::WriteImages()
{
  if (WriteImagePixels(this->TempImageFileName, false) != PLUS_SUCCESS)
  {
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkMetaImageSequenceIO::AppendImages()
{
  if( UseCompression )
  {
    LOG_ERROR("Unable to append images if compression is selected.");
    return PLUS_FAIL;
  }

  if (WriteImagePixels(this->TempImageFileName, true) != PLUS_SUCCESS)
  {
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkMetaImageSequenceIO::Close()
{
  // Update fields that are known only at the end of the processing
  if (GetUseCompression())
  {
    if (UpdateFieldInImageHeader("CompressedDataSize")!=PLUS_SUCCESS)
    {
      return PLUS_FAIL;
    }
  }

  // Move header to final file
  MoveDataInFiles(this->TempHeaderFileName, this->FileName, false);
  // Copy image to final file (or data file if .mhd)
  if( STRCASECMP(SEQMETA_FIELD_VALUE_ELEMENT_DATA_FILE_LOCAL, this->GetPixelDataFileName()) == 0 )
  {
    MoveDataInFiles(this->TempImageFileName, this->FileName, true);
  }
  else
  {
    MoveDataInFiles(this->TempImageFileName, this->GetPixelDataFileName(), false);
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
void vtkMetaImageSequenceIO::SetFileName( const char* aFilename )
{
  if( this->FileName != NULL )
  {
    delete this->FileName;
  }
  if( aFilename != NULL )
  {
    size_t n = strlen(aFilename) + 1;
    this->FileName = new char[n];
    strcpy(this->FileName, aFilename);

    if( this->TempHeaderFileName != NULL )
    {
      delete this->TempHeaderFileName;
    }
    n = strlen(aFilename) + 1 + 7;
    this->TempHeaderFileName = new char[n];
    strcpy(this->TempHeaderFileName, aFilename);
    strcat(this->TempHeaderFileName, "_header");

    if( this->TempImageFileName != NULL )
    {
      delete this->TempImageFileName;
    }
    n = strlen(aFilename) + 1 + 7;
    this->TempImageFileName = new char[n];
    strcpy(this->TempImageFileName, aFilename);
    strcat(this->TempImageFileName, "_images");
  }
}

//----------------------------------------------------------------------------
PlusStatus vtkMetaImageSequenceIO::MoveDataInFiles( const char* srcFilename, const char* destFilename, bool append )
{
  size_t len = 0 ;
  const int BUFFER_SIZE = 1024;
  char buffer[BUFFER_SIZE];
  memset(buffer, 0, BUFFER_SIZE);
  FILE* in = fopen( srcFilename, "rb" ) ;
  FILE* out = NULL;
  if( !append )
  {
    out = fopen( destFilename, "wb" ) ;
  }
  else
  {
    out = fopen( destFilename, "ab+");
  }
  if( in == NULL || out == NULL )
  {
    LOG_ERROR( "An error occurred while opening files!" ) ;
    return PLUS_FAIL;
  }
  else
  {
    while( (len = fread( buffer, 1, BUFFER_SIZE, in)) > 0 )
    {
      fwrite( buffer, 1, len, out ) ;
      memset(buffer, 0, BUFFER_SIZE);
    }
    fclose(in) ;
    fclose(out) ;
    if( !itksys::SystemTools::RemoveFile(srcFilename) )
    {
      LOG_WARNING("Unable to remove the file: " << srcFilename);
    }
  }
  return PLUS_SUCCESS;
}
