/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"
#include "itksys/SystemTools.hxx"
#include "vtkMetaImageSequenceIO.h"
#include <iomanip>
#include <iostream>


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
static const char* SEQMETA_FIELD_DIMSIZE = "DimSize";
static const char* SEQMETA_FIELD_COMPRESSED_DATA_SIZE = "CompressedDataSize";

static std::string SEQMETA_FIELD_FRAME_FIELD_PREFIX = "Seq_Frame"; 
static std::string SEQMETA_FIELD_IMG_STATUS = "ImageStatus"; 

vtkStandardNewMacro(vtkMetaImageSequenceIO);

//----------------------------------------------------------------------------
vtkMetaImageSequenceIO::vtkMetaImageSequenceIO()
  : vtkSequenceIOBase()
  , IsPixelDataBinary(true)
  , Output2DDataWithZDimensionIncluded(false)
{
} 

//----------------------------------------------------------------------------
vtkMetaImageSequenceIO::~vtkMetaImageSequenceIO()
{
}

//----------------------------------------------------------------------------
void vtkMetaImageSequenceIO::PrintSelf(ostream& os, vtkIndent indent)
{
  Superclass::PrintSelf(os, indent);

  // TODO : anything specific to print
}

//----------------------------------------------------------------------------
PlusStatus vtkMetaImageSequenceIO::ReadImageHeader()
{
  FILE *stream=NULL;
  // open in binary mode because we determine the start of the image buffer also during this read
  if ( FileOpen(&stream, this->FileName.c_str(), "rb" ) != PLUS_SUCCESS )
  {
    LOG_ERROR("The file " << this->FileName << " could not be opened for reading");
    return PLUS_FAIL;
  }

  char line[MAX_LINE_LENGTH+1]={0};
  while (fgets( line, MAX_LINE_LENGTH, stream ))
  {

    std::string lineStr=line;

    // Split line into name and value
    size_t equalSignFound;
    equalSignFound = lineStr.find_first_of("=");
    if (equalSignFound==std::string::npos)
    {
      LOG_WARNING("Parsing line failed, equal sign is missing (" << lineStr << ")");
      continue;
    }
    std::string name = lineStr.substr(0,equalSignFound);
    std::string value = lineStr.substr(equalSignFound+1);

    // trim spaces from the left and right
    PlusCommon::Trim(name);
    PlusCommon::Trim(value);

    if (name.compare(0,SEQMETA_FIELD_FRAME_FIELD_PREFIX.size(),SEQMETA_FIELD_FRAME_FIELD_PREFIX)!=0)
    {
      // field
      SetCustomString(name.c_str(), value.c_str());

      // Arrived to ElementDataFile, this is the last element
      if (name.compare(SEQMETA_FIELD_ELEMENT_DATA_FILE) == 0)
      {
        if (value.compare(SEQMETA_FIELD_VALUE_ELEMENT_DATA_FILE_LOCAL) == 0)
        {
          // pixel data stored locally
          this->PixelDataFileOffset=FTELL(stream);
        }
        else
        {
          // pixel data stored in separate file
          this->PixelDataFileName=value;
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
      underscoreFound = name.find_first_of("_");
      if (underscoreFound == std::string::npos)
      {
        LOG_WARNING("Parsing line failed, underscore is missing from frame field name ("<<lineStr<<")");
        continue;
      }
      std::string frameNumberStr = name.substr(0, underscoreFound); // 0000
      std::string frameFieldName = name.substr(underscoreFound+1); // CustomTransform

      int frameNumber=0;
      if (PlusCommon::StringToInt(frameNumberStr.c_str(), frameNumber)!=PLUS_SUCCESS)
      {
        LOG_WARNING("Parsing line failed, cannot get frame number from frame field (" << lineStr << ")");
        continue;
      }
      SetCustomFrameString(frameNumber, frameFieldName.c_str(), value.c_str());

      if (ferror(stream))
      {
        LOG_ERROR("Error reading the file " << this->FileName);
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
      this->IsPixelDataBinary=true;
    }
    else
    {
      this->IsPixelDataBinary=false;
    }
  }
  else
  {
    LOG_WARNING("BinaryData field has not been found in "<<this->FileName<<". Assume binary data.");
    this->IsPixelDataBinary=true;
  }
  if (!this->IsPixelDataBinary)
  {
    LOG_ERROR("Failed to read "<<this->FileName<<". Only binary pixel data (BinaryData=true) reading is supported.");
    return PLUS_FAIL;
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

  int numberOfScalarComponents=1;  
  if (this->TrackedFrameList->GetCustomString("ElementNumberOfChannels")!=NULL)
  {
    // this field is optional
    PlusCommon::StringToInt(this->TrackedFrameList->GetCustomString("ElementNumberOfChannels"), this->NumberOfScalarComponents);
  }

  std::string elementTypeStr=this->TrackedFrameList->GetCustomString("ElementType");
  if (ConvertMetaElementTypeToVtkPixelType(elementTypeStr.c_str(), this->PixelType)!=PLUS_SUCCESS)
  {
    LOG_ERROR("Unknown component type: "<<elementTypeStr);
    return PLUS_FAIL;
  }

  int nDims=3;
  if (PlusCommon::StringToInt(this->TrackedFrameList->GetCustomString("NDims"), nDims)==PLUS_SUCCESS)
  {
    if (nDims!=2 && nDims!=3 && nDims!=4)
    {
      LOG_ERROR("Invalid dimension (shall be 2 or 3 or 4): "<<nDims);
      return PLUS_FAIL;
    }
  }
  this->NumberOfDimensions=nDims;  

  std::string imgOrientStr = std::string(GetCustomString(SEQMETA_FIELD_US_IMG_ORIENT));
  this->ImageOrientationInFile = PlusVideoFrame::GetUsImageOrientationFromString( imgOrientStr.c_str() ); 

  const char* imgTypeStr=GetCustomString(SEQMETA_FIELD_US_IMG_TYPE);
  if (imgTypeStr==NULL)
  {
    // if the image type is not defined then assume that it is B-mode image
    this->ImageType=US_IMG_BRIGHTNESS;
  }
  else
  {
    this->ImageType = PlusVideoFrame::GetUsImageTypeFromString(imgTypeStr);
  }

  std::istringstream issDimSize(this->TrackedFrameList->GetCustomString(SEQMETA_FIELD_DIMSIZE)); // DimSize = 640 480 567, DimSize = 640 480 40 567
  int dimSize(0);
  for(int i=0; i < this->NumberOfDimensions - 1; i++) // do not iterate over last dimension, it is time!
  {
    issDimSize >> dimSize;
    this->Dimensions[i]=dimSize;
  }
  if( this->Dimensions[0] > 0 && this->Dimensions[1] > 0 && this->Dimensions[2] <= 0 )
  {
    // If the dimensions came from the file in the form X Y Nfr
    // then we would have Dimensions = {X, Y, 0, Nfr} which would break all the things
    // so set the 0 to 1
    this->Dimensions[2] = 1;
  }

  // The last dimension depends on the image orientation from the file...
  // If the orientation specifies that the data is 3D (3-letter acronym)
  // then pretend as if there is a hidden 1 in 3-dim files
  // So 
  // NDims=3
  // DimSize=630 480 567
  // is treated as
  // NDims=4
  // DimSize=630 480 1 567
  // ...
  // NDims=4
  // DimSize=630 480 567 35 is unaffected
  issDimSize >> dimSize;
  if( nDims == 3 && imgOrientStr.length() == 3 )
  {
    this->Dimensions[2] = 1;
    this->Dimensions[3] = dimSize;
  }
  else
  {
    this->Dimensions[3] = dimSize;
  }

  // If no specific image orientation is requested then determine it automatically from the image type
  // B-mode: MF
  // RF-mode: FM
  if (this->ImageOrientationInMemory==US_IMG_ORIENT_XX)
  {
    switch (this->ImageType)
    {
    case US_IMG_BRIGHTNESS:
    case US_IMG_RGB_COLOR:
      this->SetImageOrientationInMemory(US_IMG_ORIENT_MF);
      break;
    case US_IMG_RF_I_LINE_Q_LINE:
    case US_IMG_RF_IQ_LINE:
    case US_IMG_RF_REAL:
      this->SetImageOrientationInMemory(US_IMG_ORIENT_FM);
      break;
    default:
      if (this->Dimensions[0]==0 && this->Dimensions[1]==0 && this->Dimensions[2]==1)
      {
        LOG_DEBUG("Only tracking data is available in the metafile");
      }
      else
      {
        LOG_WARNING("Cannot determine image orientation automatically, unknown image type " << 
          (imgTypeStr ? imgTypeStr : "(undefined)") << ", use the same orientation in memory as in the file");
      }
      this->SetImageOrientationInMemory(this->ImageOrientationInFile);
    }
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
// Read the spacing and dimensions of the image.
PlusStatus vtkMetaImageSequenceIO::ReadImagePixels()
{ 
  int frameCount=this->Dimensions[3];
  unsigned int frameSizeInBytes=0;
  if (this->Dimensions[0]>0 && this->Dimensions[1]>0 && this->Dimensions[2]>0)
  {
    frameSizeInBytes=this->Dimensions[0]*this->Dimensions[1]*this->Dimensions[2]*PlusVideoFrame::GetNumberOfBytesPerScalar(this->PixelType)*this->NumberOfScalarComponents;
  }

  if (frameSizeInBytes==0)
  {
    LOG_DEBUG("No image data in the metafile");
    return PLUS_SUCCESS;
  }

  int numberOfErrors=0;

  FILE *stream=NULL;

  if ( FileOpen( &stream, GetPixelDataFilePath().c_str(), "rb" ) != PLUS_SUCCESS )
  {
    LOG_ERROR("The file "<<GetPixelDataFilePath()<<" could not be opened for reading");
    return PLUS_FAIL;
  }

  std::vector<unsigned char> allFramesPixelBuffer;
  if (this->UseCompression)
  {    
    unsigned int allFramesPixelBufferSize=frameCount*frameSizeInBytes;

    try
    {
      allFramesPixelBuffer.resize(allFramesPixelBufferSize);
    }
    catch(std::bad_alloc& e)
    {
      cerr << e.what() << endl;
      LOG_ERROR("vtkMetaImageSequenceIO::ReadImagePixels failed due to out of memory. Try to reduce image buffer sizes or use a 64-bit build of Plus.");
      return PLUS_FAIL;
    }

    unsigned int allFramesCompressedPixelBufferSize=0;
    PlusCommon::StringToInt(this->TrackedFrameList->GetCustomString(SEQMETA_FIELD_COMPRESSED_DATA_SIZE), allFramesCompressedPixelBufferSize);
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

    if (trackedFrame->GetImageData()->AllocateFrame(this->Dimensions, this->PixelType, this->NumberOfScalarComponents)!=PLUS_SUCCESS)
    {
      LOG_ERROR("Cannot allocate memory for frame "<<frameNumber);
      numberOfErrors++;
      continue;
    }

    int clipRectOrigin[3]={PlusCommon::NO_CLIP, PlusCommon::NO_CLIP, PlusCommon::NO_CLIP};
    int clipRectSize[3]={PlusCommon::NO_CLIP, PlusCommon::NO_CLIP, PlusCommon::NO_CLIP};

    PlusVideoFrame::FlipInfoType flipInfo;
    if ( PlusVideoFrame::GetFlipAxes(this->ImageOrientationInFile, this->ImageType, this->ImageOrientationInMemory, flipInfo) != PLUS_SUCCESS)
    {
      LOG_ERROR("Failed to convert image data to the requested orientation, from " << PlusVideoFrame::GetStringFromUsImageOrientation(this->ImageOrientationInFile) << 
        " to " << PlusVideoFrame::GetStringFromUsImageOrientation(this->ImageOrientationInMemory));
      return PLUS_FAIL;
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
      if ( PlusVideoFrame::GetOrientedClippedImage(&(pixelBuffer[0]), flipInfo, this->ImageType, this->PixelType, this->NumberOfScalarComponents, this->Dimensions, *trackedFrame->GetImageData(), clipRectOrigin, clipRectSize) != PLUS_SUCCESS )
      {
        LOG_ERROR("Failed to get oriented image from sequence metafile (frame number: " << frameNumber << ")!"); 
        numberOfErrors++;
        continue; 
      }
    }
    else
    {
      if ( PlusVideoFrame::GetOrientedClippedImage(&(allFramesPixelBuffer[0])+frameNumber*frameSizeInBytes, flipInfo, this->ImageType, this->PixelType, this->NumberOfScalarComponents, this->Dimensions, *trackedFrame->GetImageData(), clipRectOrigin, clipRectSize) != PLUS_SUCCESS )
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
PlusStatus vtkMetaImageSequenceIO::PrepareImageFile()
{
  if( this->GetUseCompression() )
  {
    // use the default memory allocation routines
    this->CompressionStream.zalloc = Z_NULL;
    this->CompressionStream.zfree = Z_NULL;
    this->CompressionStream.opaque = Z_NULL;
    int ret=deflateInit(&this->CompressionStream, Z_DEFAULT_COMPRESSION);
    if (ret!=Z_OK)
    {
      LOG_ERROR("Image compression initialization failed (errorCode="<<ret<<")");
      return PLUS_FAIL;
    }
  }
  if( FileOpen(&this->OutputImageFileHandle, this->TempImageFileName.c_str(), "ab+") != PLUS_SUCCESS )
  {
    LOG_ERROR("Unable to open output stream for writing.");
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
bool vtkMetaImageSequenceIO::CanReadFile(const std::string& filename)
{
  FILE *stream=NULL;
  // open in binary mode because we determine the start of the image buffer also during this read
  if ( vtkMetaImageSequenceIO::FileOpen( &stream, filename.c_str(), "rb" ) != PLUS_SUCCESS )
  {
    LOG_DEBUG("The file "<<filename<<" could not be opened for reading");
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
  std::string value=lineStr.substr(equalSignFound+1);

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
bool vtkMetaImageSequenceIO::CanWriteFile(const std::string& filename)
{
  if( vtksys::SystemTools::GetFilenameExtension(filename).compare(".mha") == 0 ||
    vtksys::SystemTools::GetFilenameExtension(filename).compare(".mhd") == 0)
  {
    return true;
  }

  return false;
}

//----------------------------------------------------------------------------
/** Writes the spacing and dimensions of the image.
* Assumes SetFileName has been called with a valid file name. */
PlusStatus vtkMetaImageSequenceIO::OpenImageHeader()
{
  if( this->TrackedFrameList->GetNumberOfTrackedFrames() == 0 )
  {
    LOG_ERROR("No frames in frame list, unable to query a frame for meta information.");
    return PLUS_FAIL;
  }

  // First, is this 2D or 3D?
  bool isData3D = (this->TrackedFrameList->GetTrackedFrame(0)->GetFrameSize()[2] > 1);

  // Override fields
  const char* nDims("3");
  if( isData3D || (!isData3D && this->Output2DDataWithZDimensionIncluded) )
  {
    nDims = "4";
  }
  SetCustomString("NDims", nDims);
  SetCustomString("BinaryData", "True");
  SetCustomString("BinaryDataByteOrderMSB", "False");

  // CompressedData
  if (GetUseCompression())
  {
    SetCustomString("CompressedData", "True");
    SetCustomString(SEQMETA_FIELD_COMPRESSED_DATA_SIZE, "0                "); // add spaces so that later the field can be updated with larger values
  }
  else
  {
    SetCustomString("CompressedData", "False");
    SetCustomString(SEQMETA_FIELD_COMPRESSED_DATA_SIZE, NULL);
  }

  int frameSize[3] = {0,0,0};
  if( this->EnableImageDataWrite )
  {
    this->GetMaximumImageDimensions(frameSize); 
  }
  else
  {
    frameSize[0] = 1;
    frameSize[1] = 1;
    frameSize[2] = 1;
  }

  // Set the dimensions of the data to be written
  this->Dimensions[0]=frameSize[0];
  this->Dimensions[1]=frameSize[1];
  this->Dimensions[2]=frameSize[2];
  this->Dimensions[3]=this->TrackedFrameList->GetNumberOfTrackedFrames();

  if( this->EnableImageDataWrite )
  {
    // Make sure the frame size is the same for each valid image 
    // If it's needed, we can use the largest frame size for each frame and copy the image data row by row 
    // but then, we need to save the original frame size for each frame and crop the image when we read it 
    for (unsigned int frameNumber=0; frameNumber<this->TrackedFrameList->GetNumberOfTrackedFrames(); frameNumber++)
    {
      int * currFrameSize = this->TrackedFrameList->GetTrackedFrame(frameNumber)->GetFrameSize(); 
      if ( this->TrackedFrameList->GetTrackedFrame(frameNumber)->GetImageData()->IsImageValid() 
        && ( frameSize[0] != currFrameSize[0] || frameSize[1] != currFrameSize[1] || frameSize[2] != currFrameSize[2])  )
      {
        LOG_ERROR("Frame size mismatch: expected size (" << frameSize[0] << "x" << frameSize[1] << "x" << frameSize[2]
        << ") differ from actual size (" << currFrameSize[0] << "x" << currFrameSize[1] << "x" << currFrameSize[2] << ") for frame #" << frameNumber); 
        return PLUS_FAIL; 
      }
    }
  }

  // Update NDims and Dims fields in header
  this->OverwriteNumberOfFramesInHeader(this->TrackedFrameList->GetNumberOfTrackedFrames(), true);

  // PixelType
  if (this->TrackedFrameList->IsContainingValidImageData())
  {
    this->PixelType=this->TrackedFrameList->GetPixelType();
    if ( this->PixelType == VTK_VOID )
    {
      // If the pixel type was not defined, define it to UCHAR
      this->PixelType = VTK_UNSIGNED_CHAR; 
    }
  }
  std::string pixelTypeStr;
  vtkMetaImageSequenceIO::ConvertVtkPixelTypeToMetaElementType(this->PixelType, pixelTypeStr);
  SetCustomString("ElementType", pixelTypeStr.c_str());  // pixel type (a.k.a component type) is stored in the ElementType element

  // ElementNumberOfChannels
  if (this->EnableImageDataWrite)
  {
    if( this->TrackedFrameList->IsContainingValidImageData() )
    {
      this->NumberOfScalarComponents=this->TrackedFrameList->GetNumberOfScalarComponents();
    }
    std::ostringstream ss;
    ss << this->NumberOfScalarComponents;
    SetCustomString("ElementNumberOfChannels", ss.str().c_str());
  }

  SetCustomString(SEQMETA_FIELD_US_IMG_ORIENT, PlusVideoFrame::GetStringFromUsImageOrientation(US_IMG_ORIENT_MF));
  // Image orientation
  if( this->EnableImageDataWrite )
  {
    std::string orientationStr=PlusVideoFrame::GetStringFromUsImageOrientation(this->ImageOrientationInFile);
    SetCustomString(SEQMETA_FIELD_US_IMG_ORIENT, orientationStr.c_str());
  }

  // Image type
  if( this->EnableImageDataWrite )
  {
    std::string typeStr=PlusVideoFrame::GetStringFromUsImageType(this->ImageType);
    SetCustomString(SEQMETA_FIELD_US_IMG_TYPE, typeStr.c_str());
  }

  // Add fields with default values if they are not present already
  if (GetCustomString("TransformMatrix")==NULL) { SetCustomString("TransformMatrix", "1 0 0 0 1 0 0 0 1"); }
  if (GetCustomString("Offset")==NULL) { SetCustomString("Offset", "0 0 0"); }
  if (GetCustomString("CenterOfRotation")==NULL) { SetCustomString("CenterOfRotation", "0 0 0"); }
  // TODO : does element spacing need to be 4d for ... 3d+t?
  if (GetCustomString("ElementSpacing")==NULL) { SetCustomString("ElementSpacing", "1 1 1"); }
  if (GetCustomString("AnatomicalOrientation")==NULL) { SetCustomString("AnatomicalOrientation", "RAI"); }

  FILE *stream=NULL;
  // open in binary mode because we determine the start of the image buffer also during this read
  if ( FileOpen( &stream, this->TempHeaderFileName.c_str(), "wb" ) != PLUS_SUCCESS )
  {
    LOG_ERROR("The file " << this->TempHeaderFileName << " could not be opened for writing");
    return PLUS_FAIL;
  }

  // The header shall start with these two fields
  const char* objType = "ObjectType = Image\n";
  fputs(objType, stream);
  this->TotalBytesWritten += strlen(objType);

  std::stringstream nDimsFieldStream;
  nDimsFieldStream << "NDims = ";
  if( isData3D || !isData3D && this->Output2DDataWithZDimensionIncluded )
  {
    nDimsFieldStream << this->NumberOfDimensions;
  }
  else
  {
    nDimsFieldStream << this->NumberOfDimensions - 1;
  }
  nDimsFieldStream << "\n";
  fputs(nDimsFieldStream.str().c_str(), stream);
  this->TotalBytesWritten += strlen(nDims);

  std::vector<std::string> fieldNames;
  this->TrackedFrameList->GetCustomFieldNameList(fieldNames);
  for (std::vector<std::string>::iterator it=fieldNames.begin(); it != fieldNames.end(); it++) 
  {
    if (it->compare("ObjectType")==0) continue; // this must be the first element
    if (it->compare("NDims")==0) continue; // this must be the second element
    if (it->compare("ElementDataFile")==0) continue; // this must be the last element
    std::string field=(*it)+" = "+GetCustomString(it->c_str())+"\n";
    fputs(field.c_str(), stream);
    this->TotalBytesWritten += field.length();
  }

  fclose(stream);

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkMetaImageSequenceIO::AppendImagesToHeader()
{
  FILE *stream=NULL;
  // open in binary mode because we determine the start of the image buffer also during this read
  if ( FileOpen( &stream, this->TempHeaderFileName.c_str(), "ab+" ) != PLUS_SUCCESS )
  {
    LOG_ERROR("The file " << this->TempHeaderFileName << " could not be opened for writing");
    return PLUS_FAIL;
  }

  // Write frame fields (Seq_Frame0000_... = ...)
  for (unsigned int frameNumber = CurrentFrameOffset; frameNumber < this->TrackedFrameList->GetNumberOfTrackedFrames() + CurrentFrameOffset; frameNumber++)
  {
    LOG_DEBUG("Writing frame "<<frameNumber);
    unsigned int adjustedFrameNumber = frameNumber - CurrentFrameOffset;
    TrackedFrame* trackedFrame=this->TrackedFrameList->GetTrackedFrame(adjustedFrameNumber);

    std::ostringstream frameIndexStr; 
    frameIndexStr << std::setfill('0') << std::setw(4) << frameNumber; 

    std::vector<std::string> fieldNames;
    trackedFrame->GetCustomFrameFieldNameList(fieldNames);

    for (std::vector<std::string>::iterator it=fieldNames.begin(); it != fieldNames.end(); it++) 
    {
      std::string field=SEQMETA_FIELD_FRAME_FIELD_PREFIX + frameIndexStr.str() + "_" + (*it) + " = " + trackedFrame->GetCustomFrameField(it->c_str()) + "\n";
      fputs(field.c_str(), stream);
      TotalBytesWritten += field.length();
    }
    //Only write this field if the image is saved. If only the tracking pose is kept do not save this field to the header
    if(this->EnableImageDataWrite)
    {
      // Add image status field 
      std::string imageStatus("OK"); 
      if ( !trackedFrame->GetImageData()->IsImageValid() )
      {
        imageStatus="INVALID"; 
      }
      std::string imgStatusField=SEQMETA_FIELD_FRAME_FIELD_PREFIX + frameIndexStr.str() + "_" + SEQMETA_FIELD_IMG_STATUS + " = " + imageStatus + "\n";
      fputs(imgStatusField.c_str(), stream);
      TotalBytesWritten += imgStatusField.length();
    }
  }

  fclose(stream);

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkMetaImageSequenceIO::FinalizeHeader()
{
  FILE* stream = NULL;
  // open in binary mode because we determine the start of the image buffer also during this read
  if ( FileOpen( &stream, this->TempHeaderFileName.c_str(), "ab+" ) != PLUS_SUCCESS )
  {
    LOG_ERROR("The file " << this->TempHeaderFileName << " could not be opened for writing");
    return PLUS_FAIL;
  }

  std::string dataFileStr;
  if (this->PixelDataFileName.empty())
  {
    dataFileStr = std::string(SEQMETA_FIELD_VALUE_ELEMENT_DATA_FILE_LOCAL);
  }
  else
  {
    dataFileStr = this->PixelDataFileName;
  }

  std::string elem = "ElementDataFile = "+dataFileStr+"\n";
  fputs(elem.c_str(), stream);
  TotalBytesWritten += elem.size();

  fclose(stream);

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkMetaImageSequenceIO::WriteCompressedImagePixelsToFile(int &compressedDataSize)
{
  LOG_DEBUG("Writing compressed pixel data into file started");

  compressedDataSize=0;
  unsigned char outputBuffer[Z_BUFSIZE];

  // Create a blank frame if we have to write an invalid frame to metafile 
  PlusVideoFrame blankFrame; 
  if ( blankFrame.AllocateFrame(this->Dimensions, this->PixelType, this->NumberOfScalarComponents) != PLUS_SUCCESS)
  {
    LOG_ERROR("Failed to allocate space for blank image."); 
    return PLUS_FAIL; 
  }
  blankFrame.FillBlank(); 

  for (unsigned int frameNumber=0; frameNumber<this->TrackedFrameList->GetNumberOfTrackedFrames(); frameNumber++)
  {
    TrackedFrame* trackedFrame(NULL);

    if( this->EnableImageDataWrite )
    {
      trackedFrame = this->TrackedFrameList->GetTrackedFrame(frameNumber);
      if (trackedFrame==NULL)
      {
        LOG_ERROR("Cannot access frame "<<frameNumber<<" while trying to writing compress data into file");
        deflateEnd(&this->CompressionStream);
        return PLUS_FAIL;
      }
    }

    PlusVideoFrame* videoFrame = &blankFrame;
    if( this->EnableImageDataWrite )
    {
      if ( trackedFrame->GetImageData()->IsImageValid() ) 
      {
        videoFrame = trackedFrame->GetImageData(); 
      }
    }

    this->CompressionStream.next_in = (Bytef*)videoFrame->GetScalarPointer();
    this->CompressionStream.avail_in = videoFrame->GetFrameSizeInBytes();

    // Note: it's possible to request to consume all inputs and delete all history after each frame writing to allow random access
    int flush = (frameNumber<this->TrackedFrameList->GetNumberOfTrackedFrames()-1) ? Z_NO_FLUSH : Z_FINISH;

    // run deflate() on input until output buffer not full, finish
    // compression if all of source has been read in
    do
    {
      this->CompressionStream.avail_out = Z_BUFSIZE;
      this->CompressionStream.next_out = outputBuffer;

      int ret = deflate(&this->CompressionStream, flush);    /* no bad return value */
      if (ret == Z_STREAM_ERROR)
      {
        // state clobbered
        LOG_ERROR("Zlib state became invalid during the compression process (errorCode="<<ret<<")");
        deflateEnd(&this->CompressionStream); // clean up
        return PLUS_FAIL;
      }

      size_t numberOfBytesReadyForWriting = Z_BUFSIZE - this->CompressionStream.avail_out;
      if (fwrite(outputBuffer, 1, numberOfBytesReadyForWriting, this->OutputImageFileHandle) != numberOfBytesReadyForWriting || ferror(this->OutputImageFileHandle))
      {
        LOG_ERROR("Error writing compressed data into file");
        deflateEnd(&this->CompressionStream); // clean up
        return PLUS_FAIL;
      }
      compressedDataSize+=numberOfBytesReadyForWriting;

    } while (this->CompressionStream.avail_out == 0);

    if (this->CompressionStream.avail_in != 0)
    {
      // state clobbered (by now all input should have been consumed)
      LOG_ERROR("Zlib state became invalid during the compression process");
      deflateEnd(&this->CompressionStream); // clean up
      return PLUS_FAIL;
    }
  }

  LOG_DEBUG("Writing compressed pixel data into file completed");

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkMetaImageSequenceIO::ConvertMetaElementTypeToVtkPixelType(const std::string &elementTypeStr, PlusCommon::VTKScalarPixelType &vtkPixelType)
{
  if (elementTypeStr.compare("MET_OTHER")==0
    || elementTypeStr.compare("MET_NONE")==0
    || elementTypeStr.empty())
  {
    vtkPixelType=VTK_VOID;
  }
  else if (elementTypeStr.compare("MET_CHAR")==0) { vtkPixelType = VTK_CHAR; }
  else if (elementTypeStr.compare("MET_ASCII_CHAR")==0) { vtkPixelType = VTK_CHAR; }
  else if (elementTypeStr.compare("MET_UCHAR")==0) { vtkPixelType = VTK_UNSIGNED_CHAR; }
  else if (elementTypeStr.compare("MET_SHORT")==0) { vtkPixelType = VTK_SHORT; }
  else if (elementTypeStr.compare("MET_USHORT")==0) { vtkPixelType = VTK_UNSIGNED_SHORT; }
  else if (elementTypeStr.compare("MET_INT")==0) { vtkPixelType = VTK_INT; }
  else if (elementTypeStr.compare("MET_UINT")==0)
  {
    if(sizeof(unsigned int) == MET_ValueTypeSize[MET_UINT])
    {
      vtkPixelType=VTK_UNSIGNED_INT;
    }
    else if(sizeof(unsigned long) == MET_ValueTypeSize[MET_UINT])
    {
      vtkPixelType=VTK_UNSIGNED_LONG;
    }
  }
  else if (elementTypeStr.compare("MET_LONG")==0)
  {
    if(sizeof(unsigned int) == MET_ValueTypeSize[MET_LONG])
    {
      vtkPixelType=VTK_LONG;
    }
    else if(sizeof(unsigned long) == MET_ValueTypeSize[MET_UINT])
    {
      vtkPixelType=VTK_INT;
    }
  }
  else if (elementTypeStr.compare("MET_ULONG")==0)
  {
    if(sizeof(unsigned long) == MET_ValueTypeSize[MET_ULONG])
    {
      vtkPixelType=VTK_UNSIGNED_LONG;
    }
    else if(sizeof(unsigned int) == MET_ValueTypeSize[MET_ULONG])
    {
      vtkPixelType=VTK_UNSIGNED_INT;
    }
  }
  else if (elementTypeStr.compare("MET_LONG_LONG")==0)
  {
    if(sizeof(long) == MET_ValueTypeSize[MET_LONG_LONG])
    {
      vtkPixelType=VTK_LONG;
    }
    else if(sizeof(int) == MET_ValueTypeSize[MET_LONG_LONG])
    {
      vtkPixelType=VTK_INT;
    }
    else 
    {
      vtkPixelType=VTK_VOID;
    }
  }
  else if (elementTypeStr.compare("MET_ULONG_LONG")==0)
  {
    if(sizeof(unsigned long) == MET_ValueTypeSize[MET_ULONG_LONG])
    {
      vtkPixelType=VTK_UNSIGNED_LONG;
    }
    else if(sizeof(unsigned int) == MET_ValueTypeSize[MET_ULONG_LONG])
    {
      vtkPixelType=VTK_UNSIGNED_INT;
    }
    else 
    {
      vtkPixelType=VTK_VOID;
    }
  }   
  else if (elementTypeStr.compare("MET_FLOAT")==0)
  {
    if(sizeof(float) == MET_ValueTypeSize[MET_FLOAT])
    {
      vtkPixelType=VTK_FLOAT;
    }
    else if(sizeof(double) == MET_ValueTypeSize[MET_FLOAT])
    {
      vtkPixelType=VTK_DOUBLE;
    }
  }  
  else if (elementTypeStr.compare("MET_DOUBLE")==0)
  {
    vtkPixelType=VTK_DOUBLE;
    if(sizeof(double) == MET_ValueTypeSize[MET_DOUBLE])
    {
      vtkPixelType=VTK_DOUBLE;
    }
    else if(sizeof(float) == MET_ValueTypeSize[MET_DOUBLE])
    {
      vtkPixelType=VTK_FLOAT;
    }
  }
  else
  {
    LOG_ERROR("Unknown component type: "<<elementTypeStr);
    vtkPixelType=VTK_VOID;
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkMetaImageSequenceIO::ConvertVtkPixelTypeToMetaElementType(PlusCommon::VTKScalarPixelType vtkPixelType, std::string &elementTypeStr)
{
  if (vtkPixelType==VTK_VOID)
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

  PlusCommon::VTKScalarPixelType testedPixelType=VTK_VOID;
  for (unsigned int i=0; i<sizeof(metaElementTypes); i++)
  {    
    if (ConvertMetaElementTypeToVtkPixelType(metaElementTypes[i], testedPixelType)!=PLUS_SUCCESS)
    {
      continue;
    }
    if (testedPixelType==vtkPixelType)
    {
      elementTypeStr=metaElementTypes[i];
      return PLUS_SUCCESS;
    }
  }
  elementTypeStr="MET_OTHER";
  return PLUS_FAIL;
}

//----------------------------------------------------------------------------
PlusStatus vtkMetaImageSequenceIO::UpdateFieldInImageHeader(const char* fieldName)
{
  if (this->TempHeaderFileName.empty())
  {
    LOG_ERROR("Cannot update file header, filename is invalid");
    return PLUS_FAIL;
  }
  FILE *stream=NULL;
  // open in read+write binary mode
  if ( FileOpen( &stream, this->TempHeaderFileName.c_str(), "r+b" ) != PLUS_SUCCESS )
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
      // need to add padding whitespace characters to fully replace the old line
      // the -1 is to account for the difference in \n in lineStr vs no \n in newLineStr (yet)
      int paddingCharactersNeeded=lineStr.size()-newLineStr.str().size()-1; 
      if (paddingCharactersNeeded<0)
      {
        LOG_ERROR("Cannot update line in image header (the new string '"<<newLineStr.str()<<"' is longer than the current string '"<<lineStr<<"')");
        fclose( stream );
        return PLUS_FAIL;
      }
      for (int i=0; i<paddingCharactersNeeded; i++)
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
const char* vtkMetaImageSequenceIO::GetDimensionSizeString()
{
  return SEQMETA_FIELD_DIMSIZE;
}

//----------------------------------------------------------------------------
PlusStatus vtkMetaImageSequenceIO::Close()
{
  // Update fields that are known only at the end of the processing
  if ( this->GetUseCompression() )
  {
    std::stringstream ss;
    ss << this->CompressedBytesWritten;
    this->SetCustomString(SEQMETA_FIELD_COMPRESSED_DATA_SIZE, ss.str().c_str());
    if (UpdateFieldInImageHeader(SEQMETA_FIELD_COMPRESSED_DATA_SIZE)!=PLUS_SUCCESS)
    {
      return PLUS_FAIL;
    }
    deflateEnd(&this->CompressionStream); // clean up
  }

  fclose(this->OutputImageFileHandle);

  return Superclass::Close();
}

//----------------------------------------------------------------------------
PlusStatus vtkMetaImageSequenceIO::SetFileName( const std::string& aFilename )
{
  this->FileName.clear();
  this->PixelDataFileName.clear();

  if( aFilename.empty() )
  {
    LOG_ERROR("Invalid metaimage file name");
  }

  this->FileName = aFilename;
  // Trim whitespace and " characters from the beginning and end of the filename
  this->FileName.erase(this->FileName.find_last_not_of(" \"\t\r\n")+1);
  this->FileName.erase(0,this->FileName.find_first_not_of(" \"\t\r\n"));

  // Set pixel data filename at the same time
  std::string fileExt = vtksys::SystemTools::GetFilenameLastExtension(this->FileName);
  if (STRCASECMP(fileExt.c_str(),".mha")==0)
  {
    this->PixelDataFileName="";
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

    this->PixelDataFileName=pixFileName;
  }
  else
  {
    LOG_WARNING("Writing sequence metafile with '" << fileExt << "' extension is not supported. Using mha extension instead.");
    this->FileName+=".mha";
    this->PixelDataFileName="";
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkMetaImageSequenceIO::OverwriteNumberOfFramesInHeader(int numberOfFrames, bool addPadding)
{
  bool isData3D = this->Dimensions[2] > 1;

  std::stringstream dimSizeStr;
  this->Dimensions[3]=numberOfFrames;
  dimSizeStr << this->Dimensions[0] << " " << this->Dimensions[1] << " ";
  if( isData3D || (!isData3D && this->Output2DDataWithZDimensionIncluded) )
  {
    dimSizeStr << this->Dimensions[2] << " ";
  }
  dimSizeStr << this->Dimensions[3];
  if( addPadding )
  {
    dimSizeStr << "                              ";  // add spaces so that later the field can be updated with larger values
  }
  this->SetCustomString(SEQMETA_FIELD_DIMSIZE, dimSizeStr.str().c_str());

  return PLUS_SUCCESS;
}