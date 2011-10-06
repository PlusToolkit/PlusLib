#include "PlusConfigure.h"

#include <iomanip>
#include <iostream>
//#include <errno.h>

#include "vtkMetaImageSequenceIO.h"
#include "itkMetaImageIO.h" // needed only temporarily, until becoming completely indpependent from metaimageio

#include "vtksys/SystemTools.hxx"  
#include "vtkObjectFactory.h"
#include "vtkTrackedFrameList.h"

static const int MAX_LINE_LENGTH=1000;

static const char* SEQMETA_FIELD_US_IMG_ORIENT = "UltrasoundImageOrientation"; 
static const char* SEQMETA_FIELD_DEFAULT_FRAME_TRANSFORM = "DefaultFrameTransformName"; 
static const char* SEQMETA_FIELD_ELEMENT_DATA_FILE = "ElementDataFile"; 
static const char* SEQMETA_FIELD_VALUE_ELEMENT_DATA_FILE_LOCAL = "LOCAL"; 

static std::string SEQMETA_FIELD_FRAME_FIELD_PREFIX = "Seq_Frame"; 

// Quick and robust string to int conversion
PlusStatus stringToInt(const std::string& str, int &result)
{
  const char * c_str = str.c_str();
  char * pEnd=NULL;
  result = strtol(c_str, &pEnd, 10);
  if (pEnd != c_str+str.length()) 
  {
    return PLUS_FAIL;
  }
  return PLUS_SUCCESS;
}

// Quick and robust string to int conversion
PlusStatus stringToInt(const char* strPtr, int &result)
{
  if (strPtr==NULL)
  {
    return PLUS_FAIL;
  }
  std::string str=strPtr;
  return stringToInt(str, result);
}

// Trim whitespace characters from the left and right
void trim(std::string &str)
{
  str.erase(str.find_last_not_of(" \t\r\n")+1);
  str.erase(0,str.find_first_not_of(" \t\r\n"));
}

vtkCxxRevisionMacro(vtkMetaImageSequenceIO, "$Revision: 1.0 $");
vtkStandardNewMacro(vtkMetaImageSequenceIO); 
vtkCxxSetObjectMacro(vtkMetaImageSequenceIO, TrackedFrameList, vtkTrackedFrameList);

//----------------------------------------------------------------------------
vtkMetaImageSequenceIO::vtkMetaImageSequenceIO()
{ 
  this->FileName=NULL;
  this->PixelDataFileName=NULL;
  this->PixelDataFileOffset=0;
  this->TrackedFrameList=vtkTrackedFrameList::New();
  this->UseCompression=false;
  this->FileType=itk::ImageIOBase::Binary;
  this->PixelType=itk::ImageIOBase::UNKNOWNCOMPONENTTYPE;
  this->NumberOfComponents=1;
  this->NumberOfDimensions=3;
  this->Dimensions[0]=
    this->Dimensions[1]=
    this->Dimensions[2]=0;

  this->ImageOrientationInFile=US_IMG_ORIENT_MF; 
} 

//----------------------------------------------------------------------------
vtkMetaImageSequenceIO::~vtkMetaImageSequenceIO()
{
  SetTrackedFrameList(NULL);
  SetFileName(NULL);
  SetPixelDataFileName(NULL);
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

PlusStatus vtkMetaImageSequenceIO::ReadImageHeader()
{
  FILE *stream=NULL;
  // open in binary mode because we determine the start of the image buffer also during this read
  if ( fopen_s( &stream, this->FileName, "rb" ) != 0 )
  {
    LOG_ERROR("The file "<<this->FileName<<" could not be opened for reading");
    return PLUS_FAIL;
  }

  char line[MAX_LINE_LENGTH+1]={0};
  char *fgetsResult=(char*)1;
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
    trim(name);
    trim(value);

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
          this->PixelDataFileOffset=_ftelli64(stream);
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
      if (stringToInt(frameNumberStr,frameNumber)!=PLUS_SUCCESS)
      {
        LOG_WARNING("Parsing line failed, cannot get file number from frame field ("<<lineStr<<")");
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

  if(STRCASECMP(this->TrackedFrameList->GetCustomString("BinaryData"),"true")==0)
  {
    this->FileType=itk::ImageIOBase::Binary;
  }
  else
  {
    this->FileType=itk::ImageIOBase::ASCII;
  }

  if(STRCASECMP(this->TrackedFrameList->GetCustomString("CompressedData"),"true")==0)
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
    stringToInt(this->TrackedFrameList->GetCustomString("ElementNumberOfChannels"), numberOfComponents);
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
  if (stringToInt(this->TrackedFrameList->GetCustomString("NDims"), nDims)==PLUS_SUCCESS)
  {
    if (nDims!=2 && nDims!=3)
    {
      LOG_ERROR("Invalid dimension (shall be 2 or 3): "<<nDims);
      return PLUS_FAIL;
    }
  }
  this->NumberOfDimensions=nDims;  

  this->ImageOrientationInFile = UsImageConverterCommon::GetUsImageOrientationFromString(GetCustomString(SEQMETA_FIELD_US_IMG_ORIENT)); 

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

  if ( fopen_s( &stream, GetPixelDataFilePath().c_str(), "rb" ) != 0 )
  {
    LOG_ERROR("The file "<<GetPixelDataFilePath()<<" could not be opened for reading");
    return PLUS_FAIL;
  }

  int frameCount=this->Dimensions[2];
  int frameSizeInBytes=this->Dimensions[0]*this->Dimensions[1]*UsImageConverterCommon::GetNumberOfBytesPerPixel(this->PixelType);
  
  std::vector<unsigned char> allFramesPixelBuffer;
  if (this->UseCompression)
  {    
    int allFramesPixelBufferSize=frameCount*frameSizeInBytes;
    allFramesPixelBuffer.resize(allFramesPixelBufferSize);

    int allFramesCompressedPixelBufferSize=0;
    stringToInt(this->TrackedFrameList->GetCustomString("CompressedDataSize"), allFramesCompressedPixelBufferSize);
    std::vector<unsigned char> allFramesCompressedPixelBuffer;
    allFramesCompressedPixelBuffer.resize(allFramesCompressedPixelBufferSize);

    _fseeki64(stream, this->PixelDataFileOffset, SEEK_SET);    
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
    if (trackedFrame->GetImageData()->AllocateFrame(this->Dimensions, this->PixelType)!=PLUS_SUCCESS)
    {
      LOG_ERROR("Cannot allocate memory for frame "<<frameNumber);
      numberOfErrors++;
      continue;
    }    
    if (!this->UseCompression)
    {
      FilePositionOffsetType offset=PixelDataFileOffset+frameNumber*frameSizeInBytes;
      _fseeki64(stream, offset, SEEK_SET);
      if (fread(&(pixelBuffer[0]), 1, frameSizeInBytes, stream)!=frameSizeInBytes)
      {
        LOG_ERROR("Could not read "<<frameSizeInBytes<<" bytes from "<<GetPixelDataFilePath());
        numberOfErrors++;
      }
      if ( UsImageConverterCommon::GetMFOrientedImage(&(pixelBuffer[0]), this->ImageOrientationInFile, this->Dimensions, this->PixelType, *trackedFrame->GetImageData()) != PLUS_SUCCESS )
      {
        LOG_ERROR("Failed to get MF oriented image from sequence metafile (frame number: " << frameNumber << ")!"); 
        numberOfErrors++;
        continue; 
      }
    }
    else
    {
      if ( UsImageConverterCommon::GetMFOrientedImage(&(allFramesPixelBuffer[0])+frameNumber*frameSizeInBytes, this->ImageOrientationInFile, this->Dimensions, this->PixelType, *trackedFrame->GetImageData()) != PLUS_SUCCESS )
      {
        LOG_ERROR("Failed to get MF oriented image from sequence metafile (frame number: " << frameNumber << ")!"); 
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
  if (WriteImageHeader()!=PLUS_SUCCESS)
  {
    return PLUS_FAIL;
  }
  if (WriteImagePixels()!=PLUS_SUCCESS)
  {
    return PLUS_FAIL;
  }

  // Update fields that are known only at the end of the processing
  if (GetUseCompression())
  {
    if (UpdateFieldInImageHeader("CompressedDataSize")!=PLUS_SUCCESS)
    {
      return PLUS_FAIL;
    }
  }

  return PLUS_SUCCESS;
}


void vtkMetaImageSequenceIO::CreateTrackedFrameIfNonExisting(int frameNumber)
{
  if (frameNumber<this->TrackedFrameList->GetNumberOfTrackedFrames())
  {
    // frame is already created
    return;
  }
  TrackedFrame emptyFrame;
  for (int i=this->TrackedFrameList->GetNumberOfTrackedFrames(); i<frameNumber+1; i++)
  {
    this->TrackedFrameList->AddTrackedFrame(&emptyFrame);
  }
}

bool vtkMetaImageSequenceIO::CanReadFile(const char*)
{
  FILE *stream=NULL;
  // open in binary mode because we determine the start of the image buffer also during this read
  if ( fopen_s( &stream, this->FileName, "rb" ) != 0 )
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
  trim(name);
  trim(value);

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

/** Writes the spacing and dimentions of the image.
* Assumes SetFileName has been called with a valid file name. */
PlusStatus vtkMetaImageSequenceIO::WriteImageHeader()
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
  if (this->TrackedFrameList->GetNumberOfTrackedFrames()>0)
  {
    // Check frame sizes 
    int* firstFrameSize = this->TrackedFrameList->GetTrackedFrame(0)->GetFrameSize(); 
    for (int frameNumber=1; frameNumber<this->TrackedFrameList->GetNumberOfTrackedFrames(); frameNumber++)
    {
      int * currFrameSize = this->TrackedFrameList->GetTrackedFrame(frameNumber)->GetFrameSize(); 
      if ( firstFrameSize[0] != currFrameSize[0] 
      || firstFrameSize[1] != currFrameSize[1] )
      {
        LOG_ERROR("Frame size mismatch: expected size (" << firstFrameSize[0] << "x" << firstFrameSize[1] 
        << ") differ from actual size (" << currFrameSize[0] << "x" << currFrameSize[1] << ") for frame #" << frameNumber); 
        return PLUS_FAIL; 
      }
      
    }
    frameSize[0]=firstFrameSize[0]; 
    frameSize[1]=firstFrameSize[1]; 
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
  std::string pixelTypeStr;
  vtkMetaImageSequenceIO::ConvertItkPixelTypeToMetaElementType(this->PixelType, pixelTypeStr);
  SetCustomString("ElementType", pixelTypeStr.c_str());  // pixel type (a.k.a component type) is stored in the ElementType element

  // Orientation
  std::string orientationStr=UsImageConverterCommon::GetStringFromUsImageOrientation(this->ImageOrientationInFile);
  SetCustomString("UltrasoundImageOrientation", orientationStr.c_str());

  // Add fields with default values if they are not present already
  if (GetCustomString("TransformMatrix")==NULL) { SetCustomString("TransformMatrix", "1 0 0 0 1 0 0 0 1"); }
  if (GetCustomString("Offset")==NULL) { SetCustomString("Offset", "0 0 0"); }
  if (GetCustomString("CenterOfRotation")==NULL) { SetCustomString("CenterOfRotation", "0 0 0"); }
  if (GetCustomString("ElementSpacing")==NULL) { SetCustomString("ElementSpacing", "1 1 1"); }
  if (GetCustomString("DefaultFrameTransformName")==NULL) { SetCustomString("DefaultFrameTransformName", "Unknown"); }
  if (GetCustomString("AnatomicalOrientation")==NULL) { SetCustomString("AnatomicalOrientation", "RAI"); }

  std::string fileExt=vtksys::SystemTools::GetFilenameExtension(this->FileName);
  if (STRCASECMP(fileExt.c_str(),".mha")==0)
  {
    SetPixelDataFileName("LOCAL");
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
  if ( fopen_s( &stream, this->FileName, "wb" ) != 0 )
  {
    LOG_ERROR("The file "<<this->FileName<<" could not be opened for writing");
    return PLUS_FAIL;
  }

  // The header shall start with these two fields
  fputs("ObjectType = Image\n", stream);
  fputs("NDims = 3\n", stream);

  std::vector<std::string> fieldNames;
  this->TrackedFrameList->GetCustomFieldNameList(fieldNames);
  for (std::vector<std::string>::iterator it=fieldNames.begin(); it != fieldNames.end(); it++) 
  {
    if (it->compare("ObjectType")==0) continue; // this must be the first element
    if (it->compare("NDims")==0) continue; // this must be the second element
    if (it->compare("ElementDataFile")==0) continue; // this must be the last element
    std::string field=(*it)+" = "+GetCustomString(it->c_str())+"\n";
    fputs(field.c_str(), stream);
  }

  // Write frame fields (Seq_Frame0000_... = ...)
  for (int frameNumber=0; frameNumber<this->TrackedFrameList->GetNumberOfTrackedFrames(); frameNumber++)
  {
    TrackedFrame* trackedFrame=this->TrackedFrameList->GetTrackedFrame(frameNumber);

    std::vector<std::string> fieldNames;
    trackedFrame->GetCustomFrameFieldNameList(fieldNames);
    for (std::vector<std::string>::iterator it=fieldNames.begin(); it != fieldNames.end(); it++) 
    {
      std::ostringstream frameIndexStr; 
      frameIndexStr << std::setfill('0') << std::setw(4) << frameNumber; 
      std::string field="Seq_Frame" + frameIndexStr.str() + "_" + (*it) + " = " + trackedFrame->GetCustomFrameField(it->c_str()) + "\n";
      fputs(field.c_str(), stream);
    }
  }
 
  fputs("ElementDataFile = ", stream);
  fputs(this->PixelDataFileName, stream);
  fputs("\n", stream);

  fclose(stream);

  return PLUS_SUCCESS;
}

PlusStatus vtkMetaImageSequenceIO::WriteImagePixels()
{

  if (this->ImageOrientationInFile!=US_IMG_ORIENT_MF)
  {
    LOG_ERROR("Saving of images is supported only in the MF orientation");
    return PLUS_FAIL;
  }

  FILE *stream=NULL;
  
  // Append image data
  if ( fopen_s( &stream, GetPixelDataFilePath().c_str(), "ab+" ) != 0 )
  {
    LOG_ERROR("The file "<<this->FileName<<" could not be opened for writing");
    return PLUS_FAIL;
  }

  PlusStatus result=PLUS_SUCCESS;
  if (!GetUseCompression())
  {
    // not compressed
    for (int frameNumber=0; frameNumber<this->TrackedFrameList->GetNumberOfTrackedFrames(); frameNumber++)
    {
      TrackedFrame* trackedFrame=this->TrackedFrameList->GetTrackedFrame(frameNumber);
      fwrite(trackedFrame->GetImageData()->GetBufferPointer(), 1, trackedFrame->GetImageData()->GetFrameSizeInBytes(), stream);
    }
  }
  else
  {
    // compressed
    int compressedDataSize=0;
    result=WriteCompressedImagePixelsToFile(stream, compressedDataSize);
    std::ostringstream compressedDataSizeStr; 
    compressedDataSizeStr << compressedDataSize; 
    SetCustomString("CompressedDataSize", compressedDataSizeStr.str().c_str());
  }

  fclose(stream);

  return result;
}

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

  for (int frameNumber=0; frameNumber<this->TrackedFrameList->GetNumberOfTrackedFrames(); frameNumber++)
  {
    TrackedFrame* trackedFrame=this->TrackedFrameList->GetTrackedFrame(frameNumber);
    if (trackedFrame==NULL)
    {
      LOG_ERROR("Cannot access frame "<<frameNumber<<" while trying to writing compress data into file");
      deflateEnd(&strm);
      return PLUS_FAIL;
    }
    strm.next_in=(Bytef*)trackedFrame->GetImageData()->GetBufferPointer();
    strm.avail_in=trackedFrame->GetImageData()->GetFrameSizeInBytes();

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

      int numberOfBytesReadyForWriting = outputBufferSize - strm.avail_out;
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


const char* vtkMetaImageSequenceIO::GetDefaultFrameTransformName()
{
  return this->TrackedFrameList->GetCustomString(SEQMETA_FIELD_DEFAULT_FRAME_TRANSFORM);
}

void vtkMetaImageSequenceIO::SetDefaultFrameTransformName(const std::string& strTransform)
{
  SetCustomString(SEQMETA_FIELD_DEFAULT_FRAME_TRANSFORM, strTransform.c_str());  
}

TrackedFrame* vtkMetaImageSequenceIO::GetTrackedFrame(int frameNumber)
{
  TrackedFrame* trackedFrame=this->TrackedFrameList->GetTrackedFrame(frameNumber);
  return trackedFrame;
}

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
  for (int i=0; i<sizeof(metaElementTypes); i++)
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

PlusStatus vtkMetaImageSequenceIO::UpdateFieldInImageHeader(const char* fieldName)
{
  FILE *stream=NULL;
  // open in read+write binary mode
  if ( fopen_s( &stream, this->FileName, "r+b" ) != 0 )
  {
    LOG_ERROR("The file "<<this->FileName<<" could not be opened for reading and writing");
    return PLUS_FAIL;
  }

  fseek(stream, 0, SEEK_SET);

  char line[MAX_LINE_LENGTH+1]={0};
  char *fgetsResult=(char*)1;
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
    trim(name);

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
