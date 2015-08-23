/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"
#include "itk_zlib.h"
#include "itksys/SystemTools.hxx"
#include "vtkNrrdReader.h"
#include "vtkNrrdSequenceIO.h"
#include <iomanip>
#include <iostream>
#include <sys/stat.h>

#ifdef _WIN32
#define FSEEK _fseeki64
#define FTELL _ftelli64
#else
#define FSEEK fseek
#define FTELL ftell
#endif

#include "TrackedFrame.h"
#include "vtkObjectFactory.h"
#include "vtkTrackedFrameList.h"
#include "vtksys/SystemTools.hxx"  

static const int MAX_LINE_LENGTH=1000;

static const char* SEQUENCE_FIELD_US_IMG_ORIENT = "ultrasound image orientation";  
static const char* SEQUENCE_FIELD_US_IMG_TYPE = "ultrasound image type";  
static const char* SEQUENCE_FIELD_ELEMENT_DATA_FILE = "data file"; 
static const char* SEQUENCE_FIELD_KINDS = "kinds";

static std::string SEQUENCE_FIELD_FRAME_FIELD_PREFIX = "sequence frame"; 
static std::string SEQUENCE_FIELD_IMG_STATUS = "image status"; 

vtkStandardNewMacro(vtkNrrdSequenceIO); 
vtkCxxSetObjectMacro(vtkNrrdSequenceIO, TrackedFrameList, vtkTrackedFrameList);

namespace
{
  #include "itkzlib/zutil.h"

#define ALLOC(size) malloc(size)
#define TRYFREE(p) {if (p) free(p);}

#ifndef Z_BUFSIZE
#  ifdef MAXSEG_64K
#    define Z_BUFSIZE 4096 /* minimize memory usage for 16-bit DOS */
#  else
#    define Z_BUFSIZE 16384
#  endif
#endif

  /* gzip flag byte */
#define ASCII_FLAG   0x01 /* bit 0 set: file probably ascii text */
#define HEAD_CRC     0x02 /* bit 1 set: header CRC present */
#define EXTRA_FIELD  0x04 /* bit 2 set: extra field present */
#define ORIG_NAME    0x08 /* bit 3 set: original file name present */
#define COMMENT      0x10 /* bit 4 set: file comment present */
#define RESERVED     0xE0 /* bits 5..7: reserved */

  static int const gz_magic[2] = {0x1f, 0x8b}; /* gzip magic header */

  typedef struct gz_stream {
    z_stream stream;
    int      z_err;   /* error code for last stream operation */
    int      z_eof;   /* set if end of input file */
    FILE     *file;   /* .gz file */
    Byte     *inbuf;  /* input buffer */
    Byte     *outbuf; /* output buffer */
    uLong    crc;     /* crc32 of uncompressed data */
    char     *msg;    /* error message */
    char     *path;   /* path name for debugging only */
    int      transparent; /* 1 if input file is not a .gz file */
    char     mode;    /* 'w' or 'r' */
    z_off_t  start;   /* start of compressed data in file (header skipped) */
    z_off_t  in;      /* bytes into deflate or inflate */
    z_off_t  out;     /* bytes out of deflate or inflate */
    int      back;    /* one character push-back */
    int      last;    /* true if push-back is last character */
  } gz_stream;

  /* ===========================================================================
  Opens a gzip (.gz) file for reading or writing. The mode parameter
  is as in fopen ("rb" or "wb"). The file is given either by file descriptor
  or path name (if fd == -1).
  gz_open returns NULL if the file could not be opened or if there was
  insufficient memory to allocate the (de)compression state; errno
  can be checked to distinguish the two cases (if errno is zero, the
  zlib error is Z_MEM_ERROR).
  offset is an optional parameter defining the offset of compressed data
  within the file. Good for files that have a header before gzipped data
  */
  /* ===========================================================================
  * Cleanup then free the given gz_stream. Return a zlib error code.
  Try freeing in the reverse order of allocations.
  */
  static int destroy (gz_stream* s)
  {
    int err = Z_OK;

    if (!s) return Z_STREAM_ERROR;

    TRYFREE(s->msg);

    if (s->stream.state != NULL) {
      if (s->mode == 'w') {
#ifdef NO_GZCOMPRESS
        err = Z_STREAM_ERROR;
#else
        err = deflateEnd(&(s->stream));
#endif
      } else if (s->mode == 'r') {
        err = inflateEnd(&(s->stream));
      }
    }
    if (s->file != NULL && fclose(s->file)) {
#ifdef ESPIPE
      if (errno != ESPIPE) /* fclose is broken for pipes in HP/UX */
#endif
        err = Z_ERRNO;
    }
    if (s->z_err < 0) err = s->z_err;

    TRYFREE(s->inbuf);
    TRYFREE(s->outbuf);
    TRYFREE(s->path);
    TRYFREE(s);
    return err;
  }

  /* ===========================================================================
  Read a byte from a gz_stream; update next_in and avail_in. Return EOF
  for end of file.
  IN assertion: the stream s has been sucessfully opened for reading.
  */
  static int get_byte(gz_stream * s)
  {
    if (s->z_eof) return EOF;
    if (s->stream.avail_in == 0) {
      errno = 0;
      s->stream.avail_in = (uInt)fread(s->inbuf, 1, Z_BUFSIZE, s->file);
      if (s->stream.avail_in == 0) {
        s->z_eof = 1;
        if (ferror(s->file)) s->z_err = Z_ERRNO;
        return EOF;
      }
      s->stream.next_in = s->inbuf;
    }
    s->stream.avail_in--;
    return *(s->stream.next_in)++;
  }
  /* ===========================================================================
  Check the gzip header of a gz_stream opened for reading. Set the stream
  mode to transparent if the gzip magic header is not present; set s->err
  to Z_DATA_ERROR if the magic header is present but the rest of the header
  is incorrect.
  IN assertion: the stream s has already been created sucessfully;
  s->stream.avail_in is zero for the first time, but may be non-zero
  for concatenated .gz files.
  */
  static void check_header(gz_stream *s)
  {
    int method; /* method byte */
    int flags;  /* flags byte */
    uInt len;
    int c;

    /* Assure two bytes in the buffer so we can peek ahead -- handle case
    where first byte of header is at the end of the buffer after the last
    gzip segment */
    len = s->stream.avail_in;
    if (len < 2) {
      if (len) s->inbuf[0] = s->stream.next_in[0];
      errno = 0;
      len = (uInt)fread(s->inbuf + len, 1, Z_BUFSIZE >> len, s->file);
      if (len == 0 && ferror(s->file)) s->z_err = Z_ERRNO;
      s->stream.avail_in += len;
      s->stream.next_in = s->inbuf;
      if (s->stream.avail_in < 2) {
        s->transparent = s->stream.avail_in;
        return;
      }
    }

    /* Peek ahead to check the gzip magic header */
    if (s->stream.next_in[0] != gz_magic[0] ||
      s->stream.next_in[1] != gz_magic[1]) {
        s->transparent = 1;
        return;
    }
    s->stream.avail_in -= 2;
    s->stream.next_in += 2;

    /* Check the rest of the gzip header */
    method = get_byte(s);
    flags = get_byte(s);
    if (method != Z_DEFLATED || (flags & RESERVED) != 0) {
      s->z_err = Z_DATA_ERROR;
      return;
    }

    /* Discard time, xflags and OS code: */
    for (len = 0; len < 6; len++) (void)get_byte(s);

    if ((flags & EXTRA_FIELD) != 0) { /* skip the extra field */
      len  =  (uInt)get_byte(s);
      len += ((uInt)get_byte(s))<<8;
      /* len is garbage if EOF but the loop below will quit anyway */
      while (len-- != 0 && get_byte(s) != EOF) ;
    }
    if ((flags & ORIG_NAME) != 0) { /* skip the original file name */
      while ((c = get_byte(s)) != 0 && c != EOF) ;
    }
    if ((flags & COMMENT) != 0) {   /* skip the .gz file comment */
      while ((c = get_byte(s)) != 0 && c != EOF) ;
    }
    if ((flags & HEAD_CRC) != 0) {  /* skip the header crc */
      for (len = 0; len < 2; len++) (void)get_byte(s);
    }
    s->z_err = s->z_eof ? Z_DATA_ERROR : Z_OK;
  }

  static gzFile gz_open_offset (const char* path, const char* mode, int fd, z_off_t offset=0)
  {
    int err;
    int level = Z_DEFAULT_COMPRESSION; /* compression level */
    int strategy = Z_DEFAULT_STRATEGY; /* compression strategy */
    char *p = (char*)mode;
    gz_stream *s;
    char fmode[80]; /* copy of mode, without the compression level */
    char *m = fmode;

    if (!path || !mode) return Z_NULL;

    s = (gz_stream *)ALLOC(sizeof(gz_stream));
    if (!s) return Z_NULL;

    s->stream.zalloc = (alloc_func)0;
    s->stream.zfree = (free_func)0;
    s->stream.opaque = (voidpf)0;
    s->stream.next_in = s->inbuf = Z_NULL;
    s->stream.next_out = s->outbuf = Z_NULL;
    s->stream.avail_in = s->stream.avail_out = 0;
    s->file = NULL;
    s->z_err = Z_OK;
    s->z_eof = 0;
    s->in = offset;
    s->out = 0;
    s->back = EOF;
    s->crc = crc32(0L, Z_NULL, 0);
    s->msg = NULL;
    s->transparent = 0;

    s->path = (char*)ALLOC(strlen(path)+1);
    if (s->path == NULL) {
      return destroy(s), (gzFile)Z_NULL;
    }
    strcpy(s->path, path); /* do this early for debugging */

    s->mode = '\0';
    do {
      if (*p == 'r') s->mode = 'r';
      if (*p == 'w' || *p == 'a') s->mode = 'w';
      if (*p >= '0' && *p <= '9') {
        level = *p - '0';
      } else if (*p == 'f') {
        strategy = Z_FILTERED;
      } else if (*p == 'h') {
        strategy = Z_HUFFMAN_ONLY;
      } else if (*p == 'R') {
        strategy = Z_RLE;
      } else {
        *m++ = *p; /* copy the mode */
      }
    } while (*p++ && m != fmode + sizeof(fmode));
    if (s->mode == '\0') return destroy(s), (gzFile)Z_NULL;

    if (s->mode == 'w') {
#ifdef NO_GZCOMPRESS
      err = Z_STREAM_ERROR;
#else
      err = deflateInit2(&(s->stream), level,
        Z_DEFLATED, -MAX_WBITS, DEF_MEM_LEVEL, strategy);
      /* windowBits is passed < 0 to suppress zlib header */

      s->stream.next_out = s->outbuf = (Byte*)ALLOC(Z_BUFSIZE);
#endif
      if (err != Z_OK || s->outbuf == Z_NULL) {
        return destroy(s), (gzFile)Z_NULL;
      }
    } else {
      s->stream.next_in  = s->inbuf = (Byte*)ALLOC(Z_BUFSIZE);

      err = inflateInit2(&(s->stream), -MAX_WBITS);
      /* windowBits is passed < 0 to tell that there is no zlib header.
      * Note that in this case inflate *requires* an extra "dummy" byte
      * after the compressed stream in order to complete decompression and
      * return Z_STREAM_END. Here the gzip CRC32 ensures that 4 bytes are
      * present after the compressed stream.
      */
      if (err != Z_OK || s->inbuf == Z_NULL) {
        return destroy(s), (gzFile)Z_NULL;
      }
    }
    s->stream.avail_out = Z_BUFSIZE;

    errno = 0;
    s->file = fd < 0 ? F_OPEN(path, fmode) : (FILE*)fdopen(fd, fmode);

    if (s->file == NULL) {
      return destroy(s), (gzFile)Z_NULL;
    }
    if (s->mode == 'w') {
      /* Write a very simple .gz header:
      */
      fprintf(s->file, "%c%c%c%c%c%c%c%c%c%c", gz_magic[0], gz_magic[1],
        Z_DEFLATED, 0 /*flags*/, 0,0,0,0 /*time*/, 0 /*xflags*/, OS_CODE);
      s->start = 10L;
      /* We use 10L instead of ftell(s->file) to because ftell causes an
      * fflush on some systems. This version of the library doesn't use
      * start anyway in write mode, so this initialization is not
      * necessary.
      */
    } else {
      fseek(s->file, offset, SEEK_CUR);
      check_header(s); /* skip the .gz header */
      s->start = ftell(s->file) - s->stream.avail_in;
    }

    return (gzFile)s;
  }
}

//----------------------------------------------------------------------------
vtkNrrdSequenceIO::vtkNrrdSequenceIO()
  : TrackedFrameList(vtkTrackedFrameList::New())
  , UseCompression(false)
  , EnableImageDataWrite(true)
  , Encoding(NRRD_ENCODING_RAW)
  , PixelType(VTK_VOID)
  , NumberOfScalarComponents(1)
  , NumberOfDimensions(4)
  , CurrentFrameOffset(0)
  , TotalBytesWritten(0)
  , ImageOrientationInFile(US_IMG_ORIENT_XX)
  , ImageOrientationInMemory(US_IMG_ORIENT_XX)
  , ImageType(US_IMG_TYPE_XX)
  , PixelDataFileOffset(0)
  , PixelDataFileName("") //empty string denotes local storage
{ 
  this->Dimensions[0]=1;
  this->Dimensions[1]=1;
  this->Dimensions[2]=1;
  this->Dimensions[3]=1;
} 

//----------------------------------------------------------------------------
vtkNrrdSequenceIO::~vtkNrrdSequenceIO()
{
  SetTrackedFrameList(NULL);
}

//----------------------------------------------------------------------------
PlusStatus vtkNrrdSequenceIO::DeleteCustomFrameString(int frameNumber, const char* fieldName)
{
  TrackedFrame* trackedFrame = this->TrackedFrameList->GetTrackedFrame(frameNumber);
  if (trackedFrame==NULL)
  {
    LOG_ERROR("Cannot access frame " << frameNumber);
    return PLUS_FAIL;
  }

  return trackedFrame->DeleteCustomFrameField(fieldName); 
}

//----------------------------------------------------------------------------
PlusStatus vtkNrrdSequenceIO::SetCustomFrameString(int frameNumber, const char* fieldName,  const char* fieldValue)
{
  if (fieldName==NULL || fieldValue==NULL)
  {
    LOG_ERROR("Invalid field name or value");
    return PLUS_FAIL;
  }
  this->CreateTrackedFrameIfNonExisting(frameNumber);
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
bool vtkNrrdSequenceIO::SetCustomString(const char* fieldName, const char* fieldValue)
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
const char* vtkNrrdSequenceIO::GetCustomString(const char* fieldName)
{
  if (fieldName==NULL)
  {
    LOG_ERROR("Invalid field name or value");
    return NULL;
  }
  return this->TrackedFrameList->GetCustomString(fieldName); 
}

//----------------------------------------------------------------------------
void vtkNrrdSequenceIO::PrintSelf(ostream& os, vtkIndent indent)
{
  os << indent << "Metadata User Fields:" << std::endl;
  this->TrackedFrameList->PrintSelf(os, indent);
}

//----------------------------------------------------------------------------
PlusStatus vtkNrrdSequenceIO::ReadImageHeader()
{
  FILE *stream=NULL;
  // open in binary mode because we determine the start of the image buffer also during this read
  if ( FileOpen(&stream, this->FileName.c_str(), "rb" ) != PLUS_SUCCESS )
  {
    LOG_ERROR("The file " << this->FileName << " could not be opened for reading");
    return PLUS_FAIL;
  }

  bool dataFileEntryFound(false);

  char line[MAX_LINE_LENGTH+1]={0};
  while (fgets( line, MAX_LINE_LENGTH, stream ))
  {
    std::string lineStr=line;

    if( lineStr.compare("\n") == 0 )
    {
      if( !dataFileEntryFound )
      {
        // pixel data stored locally
        // this->PixelDataFileName is already empty string unless overriden by data file: field
        this->PixelDataFileOffset=FTELL(stream);
      }
      // this is the last element of the header
      break;
    }

    size_t separatorFound;
    separatorFound = lineStr.find_first_of("#");
    if ( separatorFound != std::string::npos )
    {
      // Comment found, skip
      continue;
    }

    // Split line into name and value
    separatorFound = lineStr.find_first_of(":");
    if ( separatorFound == std::string::npos )
    {
      LOG_WARNING("Parsing line failed, colon is missing (" << lineStr << ")");
      continue;
    }
    std::string name = lineStr.substr(0,separatorFound);
    std::string value = lineStr.substr(separatorFound+1);
    if( lineStr[separatorFound+1] == '=')
    {
      // It is a key/value 
      value = lineStr.substr(separatorFound+2);
    }

    // trim spaces from the left and right
    PlusCommon::Trim(name);
    PlusCommon::Trim(value);

    if (name.compare(0,SEQUENCE_FIELD_FRAME_FIELD_PREFIX.size(),SEQUENCE_FIELD_FRAME_FIELD_PREFIX)!=0)
    {
      // field
      SetCustomString(name.c_str(), value.c_str());

      // data file found, seperate data file
      if (name.compare(SEQUENCE_FIELD_ELEMENT_DATA_FILE) == 0)
      {
        dataFileEntryFound = true;
        this->PixelDataFileName=value;
        this->PixelDataFileOffset=0;
      }
    }
    else
    {
      // frame field
      // name: Seq_Frame0000_CustomTransform
      name.erase(0,SEQUENCE_FIELD_FRAME_FIELD_PREFIX.size()); // 0000_CustomTransform

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

  if( this->TrackedFrameList->GetCustomString("encoding") != NULL )
  {
    // set fields according to encoding
    std::string encoding = std::string(this->TrackedFrameList->GetCustomString("encoding"));
    this->Encoding = vtkNrrdSequenceIO::StringToNrrdEncoding(encoding);
    if( this->Encoding >= NRRD_ENCODING_GZ )
    {
      this->UseCompression = true;
    }
    if( this->Encoding >= NRRD_ENCODING_BZ2 )
    {
      // TODO : enable bzip2 encoding
      LOG_ERROR("bzip2 encoding is currently not supported. Please re-encode NRRD using gzip encoding and re-run. Apologies for the inconvenience.");
      return PLUS_FAIL;
    }
  }
  else
  {
    LOG_ERROR("Field encoding not found in file: " << this->FileName << ". Unable to read.");
    return PLUS_FAIL;
  }

  const char* elementType = this->TrackedFrameList->GetCustomString("type");
  if( elementType == NULL )
  {
    LOG_ERROR("Field type not found in file: " << this->FileName << ". Unable to read.");
    return PLUS_FAIL;
  }
  else if ( ConvertNrrdTypeToVtkPixelType(elementType, this->PixelType) != PLUS_SUCCESS )
  {
    LOG_ERROR("Unknown component type: "<<elementType);
    return PLUS_FAIL;
  }

  int nDims=3;
  if ( PlusCommon::StringToInt(this->TrackedFrameList->GetCustomString("dimensions"), nDims)==PLUS_SUCCESS )
  {
    if (nDims!=2 && nDims!=3 && nDims!=4)
    {
      LOG_ERROR("Invalid dimension (shall be 2 or 3 or 4): "<<nDims);
      return PLUS_FAIL;
    }
  }
  this->NumberOfDimensions=nDims;  

  std::string imgOrientStr;
  if( GetCustomString(SEQUENCE_FIELD_US_IMG_ORIENT) != NULL )
  {
    imgOrientStr = std::string(GetCustomString(SEQUENCE_FIELD_US_IMG_ORIENT));
  }
  else
  {
    imgOrientStr = PlusVideoFrame::GetStringFromUsImageOrientation(US_IMG_ORIENT_MF);
    LOG_WARNING(SEQUENCE_FIELD_US_IMG_ORIENT << " field not found in header. Defaulting to " << imgOrientStr << ".");
  }
  this->ImageOrientationInFile = PlusVideoFrame::GetUsImageOrientationFromString( imgOrientStr.c_str() ); 

  const char* imgTypeStr=GetCustomString(SEQUENCE_FIELD_US_IMG_TYPE);
  if (imgTypeStr==NULL)
  {
    // if the image type is not defined then assume that it is B-mode image
    this->ImageType=US_IMG_BRIGHTNESS;
    LOG_WARNING(SEQUENCE_FIELD_US_IMG_TYPE << " field not found in header. Defaulting to US_IMG_BRIGHTNESS.");
  }
  else
  {
    this->ImageType = PlusVideoFrame::GetUsImageTypeFromString(imgTypeStr);
  }

  std::vector<std::string> kinds;
  if( this->TrackedFrameList->GetCustomString("kinds") != NULL )
  {
    PlusCommon::SplitStringIntoTokens(std::string(this->TrackedFrameList->GetCustomString("kinds")), ' ', kinds);
  }
  else
  {
    LOG_WARNING(SEQUENCE_FIELD_KINDS << " field not found in header. Defaulting to " << this->NumberOfDimensions-1 << " domains and 1 time.");
    for( int i = 0; i < this->NumberOfDimensions-1; ++i )
    {
      kinds.push_back(std::string("domain"));
    }
    kinds.push_back(std::string("time"));
  }

  // sizes = 640 480, sizes = 640 480 567, sizes = 640 480 40 567
  std::istringstream issDimSize(this->TrackedFrameList->GetCustomString("sizes")); 
  int dimSize(0);
  int spatialDomainCount(0);
  for(int i=0; i < kinds.size(); i++) 
  {
    issDimSize >> dimSize;
    if( kinds[i].compare("domain") == 0)
    {
      if( spatialDomainCount == 3 ) // 0-indexed, this is the 4th spatial domain
      {
        LOG_ERROR("PLUS supports up to 3 spatial domains. File: " << this->FileName << " contains more than 3.");
        return PLUS_FAIL;
      }
      this->Dimensions[spatialDomainCount]=dimSize;
      spatialDomainCount++;
    }
    else if( kinds[i].compare("time") == 0 || kinds[i].compare("list") == 0) // time = resampling ok, list = resampling not ok
    {
      this->Dimensions[3]=dimSize;
    }
    else if( kinds[i].compare("vector") == 0)
    {
      this->NumberOfScalarComponents = dimSize;
    }
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
      if (this->Dimensions[0]==0 && this->Dimensions[1]==0 && this->Dimensions[2]==0)
      {
        LOG_DEBUG("Only tracking data is available in the file");
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
PlusStatus vtkNrrdSequenceIO::ReadImagePixels()
{ 
  int frameCount=this->Dimensions[3];
  unsigned int frameSizeInBytes=0;
  if (this->Dimensions[0]>0 && this->Dimensions[1]>0 && this->Dimensions[2]>0)
  {
    frameSizeInBytes=this->Dimensions[0]*this->Dimensions[1]*this->Dimensions[2]*PlusVideoFrame::GetNumberOfBytesPerScalar(this->PixelType)*this->NumberOfScalarComponents;
  }
  else
  {
    LOG_ERROR("0 dimension found. Unable to read image pixels. Dimensions = [" << this->Dimensions[0] << ", " << this->Dimensions[1] << ", " << this->Dimensions[2] << "]");
    return PLUS_FAIL;
  }

  if (frameSizeInBytes==0)
  {
    LOG_DEBUG("No image data in the file");
    return PLUS_SUCCESS;
  }

  int numberOfErrors=0;

  FILE *stream=NULL;
  gzFile gzStream=NULL;

  if ( this->UseCompression && this->Encoding >= NRRD_ENCODING_GZ && this->Encoding < NRRD_ENCODING_BZ2)
  {
    // gzipped
    gzStream = gz_open_offset(this->GetPixelDataFilePath().c_str(), "rb", -1, this->PixelDataFileOffset);
  }
  else
  {
    if ( FileOpen( &stream, this->GetPixelDataFilePath().c_str(), "rb" ) != PLUS_SUCCESS )
    {
      LOG_ERROR("The file " << this->GetPixelDataFilePath() << " could not be opened for reading");
      return PLUS_FAIL;
    }
  }

  std::vector<unsigned char> allFramesPixelBuffer;
  unsigned char* gzAllFramesPixelBuffer;
  if (this->UseCompression && this->Encoding >= NRRD_ENCODING_GZ && this->Encoding < NRRD_ENCODING_BZ2)
  {
    //gzip uncompression
    unsigned int allFramesPixelBufferSize=frameCount*frameSizeInBytes;
    gzAllFramesPixelBuffer = new unsigned char[allFramesPixelBufferSize];

    vtkNrrdSequenceIO::FilePositionOffsetType allFramesCompressedPixelBufferSize = vtkNrrdSequenceIO::GetFileSize( this->GetPixelDataFilePath() ) - this->PixelDataFileOffset;

    //gzseek(gzStream, this->PixelDataFileOffset, SEEK_SET);
    if (gzread(gzStream, (void*)gzAllFramesPixelBuffer, allFramesPixelBufferSize) != allFramesPixelBufferSize)
    {
      LOG_ERROR("Could not read " << allFramesPixelBufferSize << " bytes from "<<GetPixelDataFilePath());
      gzclose(gzStream);
      return PLUS_FAIL;
    }
    gzclose(gzStream);
  }

  std::vector<unsigned char> pixelBuffer;
  pixelBuffer.resize(frameSizeInBytes);
  for (int frameNumber=0; frameNumber < frameCount; frameNumber++)
  {
    this->CreateTrackedFrameIfNonExisting(frameNumber);    
    TrackedFrame* trackedFrame=this->TrackedFrameList->GetTrackedFrame(frameNumber);    

    // Allocate frame only if it is valid 
    const char* imgStatus = trackedFrame->GetCustomFrameField(SEQUENCE_FIELD_IMG_STATUS.c_str()); 
    if ( imgStatus != NULL  ) // Found the image status field 
    { 
      // Save status field 
      std::string strImgStatus(imgStatus); 

      // Delete image status field from tracked frame 
      // Image status can be determine by trackedFrame->GetImageData()->IsImageValid()
      trackedFrame->DeleteCustomFrameField(SEQUENCE_FIELD_IMG_STATUS.c_str()); 

      if ( strImgStatus.compare("OK") != 0 )// Image status _not_ OK 
      {
        LOG_DEBUG("Frame #" << frameNumber << " image data is invalid, no need to allocate data in the tracked frame list."); 
        // TODO : is this right? don't we lose tool info from these dropped frames?
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

    int clipRectOrigin[3] = {PlusCommon::NO_CLIP, PlusCommon::NO_CLIP, PlusCommon::NO_CLIP};
    int clipRectSize[3] = {PlusCommon::NO_CLIP, PlusCommon::NO_CLIP, PlusCommon::NO_CLIP};

    PlusVideoFrame::FlipInfoType flipInfo;
    if ( PlusVideoFrame::GetFlipAxes(this->ImageOrientationInFile, this->ImageType, this->ImageOrientationInMemory, flipInfo) != PLUS_SUCCESS)
    {
      LOG_ERROR("Failed to convert image data to the requested orientation, from " << PlusVideoFrame::GetStringFromUsImageOrientation(this->ImageOrientationInFile) << 
        " to " << PlusVideoFrame::GetStringFromUsImageOrientation(this->ImageOrientationInMemory));
      return PLUS_FAIL;
    }

    if ( !this->UseCompression )
    {
      FilePositionOffsetType offset = this->PixelDataFileOffset+frameNumber*frameSizeInBytes;
      FSEEK(stream, offset, SEEK_SET);
      if ( fread(&(pixelBuffer[0]), 1, frameSizeInBytes, stream) != frameSizeInBytes)
      {
        //LOG_ERROR("Could not read "<<frameSizeInBytes<<" bytes from "<<GetPixelDataFilePath());
        //numberOfErrors++;
      }
      if ( PlusVideoFrame::GetOrientedClippedImage(&(pixelBuffer[0]), flipInfo, this->ImageType, this->PixelType, this->NumberOfScalarComponents, this->Dimensions, *trackedFrame->GetImageData(), clipRectOrigin, clipRectSize) != PLUS_SUCCESS )
      {
        LOG_ERROR("Failed to get oriented image from sequence file (frame number: " << frameNumber << ")!"); 
        numberOfErrors++;
        continue; 
      }
    }
    else
    {
      if ( PlusVideoFrame::GetOrientedClippedImage(gzAllFramesPixelBuffer+frameNumber*frameSizeInBytes, flipInfo, this->ImageType, this->PixelType, this->NumberOfScalarComponents, this->Dimensions, *trackedFrame->GetImageData(), clipRectOrigin, clipRectSize) != PLUS_SUCCESS )
      {
        LOG_ERROR("Failed to get oriented image from sequence file (frame number: " << frameNumber << ")!"); 
        numberOfErrors++;
        continue; 
      }
    }
  }
  if( !this->UseCompression )
  {
    fclose( stream );
  }

  if (numberOfErrors>0)
  {
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkNrrdSequenceIO::Write()
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

  if ( this->WriteImagePixels(this->TempImageFileName, false) != PLUS_SUCCESS )
  {
    return PLUS_FAIL;
  }

  this->Close();

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
void vtkNrrdSequenceIO::CreateTrackedFrameIfNonExisting(unsigned int frameNumber)
{
  if ( frameNumber<this->TrackedFrameList->GetNumberOfTrackedFrames() )
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
bool vtkNrrdSequenceIO::CanReadFile(const std::string& filename)
{
  vtkSmartPointer<vtkNrrdReader> reader = vtkSmartPointer<vtkNrrdReader>::New();

  return reader->CanReadFile(filename.c_str());
}

//----------------------------------------------------------------------------
bool vtkNrrdSequenceIO::CanWriteFile(const std::string& filename)
{
  if( vtksys::SystemTools::GetFilenameExtension(filename).compare(".nrrd") == 0 ||
    vtksys::SystemTools::GetFilenameExtension(filename).compare(".nhdr") == 0)
  {
    return true;
  }

  return false;
}

//----------------------------------------------------------------------------
PlusStatus vtkNrrdSequenceIO::Read()
{
  this->TrackedFrameList->Clear();

  if ( this->ReadImageHeader() != PLUS_SUCCESS )
  {
    LOG_ERROR("Could not load header from file: " << this->FileName);
    return PLUS_FAIL;
  }

  if ( this->ReadImagePixels() != PLUS_SUCCESS )
  {
    return PLUS_FAIL;
  }
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
/** Writes the spacing and dimensions of the image.
* Assumes SetFileName has been called with a valid file name. */
PlusStatus vtkNrrdSequenceIO::OpenImageHeader()
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
  if( isData3D )
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
    SetCustomString("CompressedDataSize", "0                "); // add spaces so that later the field can be updated with larger values
  }
  else
  {
    SetCustomString("CompressedData", "False");
    SetCustomString("CompressedDataSize", NULL);
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
  vtkNrrdSequenceIO::ConvertVtkPixelTypeToNrrdType(this->PixelType, pixelTypeStr);
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

  // Image orientation
  if( this->EnableImageDataWrite )
  {
    std::string orientationStr=PlusVideoFrame::GetStringFromUsImageOrientation(this->ImageOrientationInFile);
    SetCustomString(SEQUENCE_FIELD_US_IMG_ORIENT, orientationStr.c_str());
  }

  // Image type
  if( this->EnableImageDataWrite )
  {
    std::string typeStr=PlusVideoFrame::GetStringFromUsImageType(this->ImageType);
    SetCustomString(SEQUENCE_FIELD_US_IMG_TYPE, typeStr.c_str());
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
  if( isData3D )
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
PlusStatus vtkNrrdSequenceIO::AppendImagesToHeader()
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
      std::string field=SEQUENCE_FIELD_FRAME_FIELD_PREFIX + frameIndexStr.str() + "_" + (*it) + " = " + trackedFrame->GetCustomFrameField(it->c_str()) + "\n";
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
      std::string imgStatusField=SEQUENCE_FIELD_FRAME_FIELD_PREFIX + frameIndexStr.str() + "_" + SEQUENCE_FIELD_IMG_STATUS + " = " + imageStatus + "\n";
      fputs(imgStatusField.c_str(), stream);
      TotalBytesWritten += imgStatusField.length();
    }
  }

  fclose(stream);

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkNrrdSequenceIO::FinalizeHeader()
{
  FILE* stream = NULL;
  // open in binary mode because we determine the start of the image buffer also during this read
  if ( FileOpen( &stream, this->TempHeaderFileName.c_str(), "ab+" ) != PLUS_SUCCESS )
  {
    LOG_ERROR("The file " << this->TempHeaderFileName << " could not be opened for writing");
    return PLUS_FAIL;
  }

  std::string elem;
  // In NRRD, empty line denotes end of header
  if (this->PixelDataFileName.empty())
  {
    LOG_INFO("PixelDataFileName is empty. Using single file pixel data storage.");
    elem = "\n";
  }
  else
  {
    elem = SEQUENCE_FIELD_ELEMENT_DATA_FILE + std::string(": ") + this->PixelDataFileName + std::string("\n\n");
  }

  fputs(elem.c_str(), stream);
  TotalBytesWritten += elem.size();

  fclose(stream);

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
void vtkNrrdSequenceIO::GetMaximumImageDimensions(int maxFrameSize[3])
{
  maxFrameSize[0]=0;
  maxFrameSize[1]=0;
  maxFrameSize[2]=0;

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

    if( maxFrameSize[2] < currFrameSize[2] )
    {
      maxFrameSize[2] = currFrameSize[2];
    }
  }
}

//----------------------------------------------------------------------------
PlusStatus vtkNrrdSequenceIO::WriteImagePixels(const std::string& aFilename, bool forceAppend /* = false */)
{
  if (this->EnableImageDataWrite && this->TrackedFrameList->IsContainingValidImageData() && this->ImageOrientationInFile!=this->TrackedFrameList->GetImageOrientation())
  {
    // Reordering of the frames is not implemented, so return with an error
    LOG_ERROR("Saving of images is supported only in the same orientation as currently in the memory");
    return PLUS_FAIL;
  }

  bool imageDataAvailable = (this->Dimensions[0]>0 && this->Dimensions[1]>0 && this->Dimensions[2]>0);

  FILE *stream=NULL;

  std::string fileOpenMode="wb"; // w (write, existing file is destroyed), b (binary)
  if ( forceAppend && !GetUseCompression())
  {
    // Pixel data is stored locally in the header file (NRRD file), so we append the image data to an existing file
    // Or this sequence is being written to in chunks
    fileOpenMode="ab+"; // a+ (append to the end of the file), b (binary)
  }
  else if( forceAppend && GetUseCompression())
  {
    LOG_ERROR("Unable to append images when compression is used. You must write uncompressed and then post-compress.");
    return PLUS_FAIL;
  }
  if ( this->FileOpen( &stream, aFilename.c_str(), fileOpenMode.c_str() ) != PLUS_SUCCESS )
  {
    LOG_ERROR("The file " << aFilename << " could not be opened for writing");
    return PLUS_FAIL;
  }

  if ( this->PixelType == VTK_VOID )
  {
    // If the pixel type was not defined, define it to UCHAR
    this->PixelType = VTK_UNSIGNED_CHAR; 
  }

  PlusStatus result = PLUS_SUCCESS;
  if ( !GetUseCompression() )
  {
    if (imageDataAvailable)
    {
      // Create a blank frame if we have to write an invalid frame to file 
      PlusVideoFrame blankFrame; 
      if ( blankFrame.AllocateFrame(this->Dimensions, this->PixelType, this->NumberOfScalarComponents)!=PLUS_SUCCESS)
      {
        LOG_ERROR("Failed to allocate space for blank image."); 
        return PLUS_FAIL; 
      }
      blankFrame.FillBlank(); 

      // not compressed
      for (unsigned int frameNumber=0; frameNumber<this->TrackedFrameList->GetNumberOfTrackedFrames(); frameNumber++)
      {
        TrackedFrame* trackedFrame = this->TrackedFrameList->GetTrackedFrame(frameNumber);

        PlusVideoFrame* videoFrame = &blankFrame;
        if ( this->EnableImageDataWrite && trackedFrame->GetImageData()->IsImageValid() ) 
        {
          videoFrame = trackedFrame->GetImageData(); 
        }

        unsigned long result = fwrite(videoFrame->GetScalarPointer(), 1, videoFrame->GetFrameSizeInBytes(), stream);
        if( result != videoFrame->GetFrameSizeInBytes() )
        {
          LOG_ERROR("Unable to write entire frame to file.");
        }
        TotalBytesWritten += result;
      }
    }
  }
  else
  {
    // compressed
    int compressedDataSize=0;
    if (imageDataAvailable)
    {
      result = WriteCompressedImagePixelsToFile(stream, compressedDataSize);
      if( result == PLUS_SUCCESS )
      {
        TotalBytesWritten += compressedDataSize;
      }
    }
    std::ostringstream compressedDataSizeStr; 
    compressedDataSizeStr << compressedDataSize; 
    SetCustomString("CompressedDataSize", compressedDataSizeStr.str().c_str());
  }

  fclose(stream);

  if( result == PLUS_SUCCESS )
  {
    CurrentFrameOffset += TrackedFrameList->GetNumberOfTrackedFrames();
  }
  return result;
}

//----------------------------------------------------------------------------
PlusStatus vtkNrrdSequenceIO::WriteCompressedImagePixelsToFile(FILE *outputFileStream, int &compressedDataSize)
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

  // Create a blank frame if we have to write an invalid frame to file 
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
        deflateEnd(&strm);
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

    strm.next_in = (Bytef*)videoFrame->GetScalarPointer();
    strm.avail_in = videoFrame->GetFrameSizeInBytes();

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
TrackedFrame* vtkNrrdSequenceIO::GetTrackedFrame(int frameNumber)
{
  TrackedFrame* trackedFrame=this->TrackedFrameList->GetTrackedFrame(frameNumber);
  return trackedFrame;
}

//----------------------------------------------------------------------------
PlusStatus vtkNrrdSequenceIO::ConvertNrrdTypeToVtkPixelType(const std::string &elementTypeStr, PlusCommon::VTKScalarPixelType &vtkPixelType)
{
  if (elementTypeStr.compare("signed char")==0) { vtkPixelType = VTK_CHAR; }
  else if (elementTypeStr.compare("int8")==0) { vtkPixelType = VTK_CHAR; }
  else if (elementTypeStr.compare("int8_t")==0) { vtkPixelType = VTK_CHAR; }
  else if (elementTypeStr.compare("uchar")==0) { vtkPixelType = VTK_UNSIGNED_CHAR; }
  else if (elementTypeStr.compare("unsigned char")==0) { vtkPixelType = VTK_UNSIGNED_CHAR; }
  else if (elementTypeStr.compare("uint8")==0) { vtkPixelType = VTK_UNSIGNED_CHAR; }
  else if (elementTypeStr.compare("uint8_t")==0) { vtkPixelType = VTK_UNSIGNED_CHAR; }
  else if (elementTypeStr.compare("short")==0) { vtkPixelType = VTK_SHORT; }
  else if (elementTypeStr.compare("short int")==0) { vtkPixelType = VTK_SHORT; }
  else if (elementTypeStr.compare("signed short")==0) { vtkPixelType = VTK_SHORT; }
  else if (elementTypeStr.compare("signed short int")==0) { vtkPixelType = VTK_SHORT; }
  else if (elementTypeStr.compare("int16")==0) { vtkPixelType = VTK_SHORT; }
  else if (elementTypeStr.compare("int16_t")==0) { vtkPixelType = VTK_SHORT; }
  else if (elementTypeStr.compare("ushort")==0) { vtkPixelType = VTK_UNSIGNED_SHORT; }
  else if (elementTypeStr.compare("unsigned short")==0) { vtkPixelType = VTK_UNSIGNED_SHORT; }
  else if (elementTypeStr.compare("unsigned short int")==0) { vtkPixelType = VTK_UNSIGNED_SHORT; }
  else if (elementTypeStr.compare("uint16")==0) { vtkPixelType = VTK_UNSIGNED_SHORT; }
  else if (elementTypeStr.compare("uint16_t")==0) { vtkPixelType = VTK_UNSIGNED_SHORT; }
  else if (elementTypeStr.compare("int")==0) { vtkPixelType = VTK_INT; }
  else if (elementTypeStr.compare("signed int")==0) { vtkPixelType = VTK_INT; }
  else if (elementTypeStr.compare("int32")==0) { vtkPixelType = VTK_INT; }
  else if (elementTypeStr.compare("int32_t")==0) { vtkPixelType = VTK_INT; }
  else if (elementTypeStr.compare("uint")==0) { vtkPixelType = VTK_UNSIGNED_INT; }
  else if (elementTypeStr.compare("unsigned int")==0) { vtkPixelType = VTK_UNSIGNED_INT; }
  else if (elementTypeStr.compare("uint32")==0) { vtkPixelType = VTK_UNSIGNED_INT; }
  else if (elementTypeStr.compare("uint32_t")==0) { vtkPixelType = VTK_UNSIGNED_INT; }
  else if (elementTypeStr.compare("longlong")==0) { vtkPixelType = VTK_LONG; }
  else if (elementTypeStr.compare("long long")==0) { vtkPixelType = VTK_LONG_LONG; }
  else if (elementTypeStr.compare("long long int")==0) { vtkPixelType = VTK_LONG_LONG; }
  else if (elementTypeStr.compare("signed long long")==0) { vtkPixelType = VTK_LONG_LONG; }
  else if (elementTypeStr.compare("signed long long int")==0) { vtkPixelType = VTK_LONG_LONG; }
  else if (elementTypeStr.compare("int64")==0) { vtkPixelType = VTK_LONG_LONG; }
  else if (elementTypeStr.compare("int64_t")==0) { vtkPixelType = VTK_LONG_LONG; }
  else if (elementTypeStr.compare("ulonglong")==0) { vtkPixelType = VTK_UNSIGNED_LONG_LONG; }
  else if (elementTypeStr.compare("unsigned long long")==0) { vtkPixelType = VTK_UNSIGNED_LONG_LONG; }
  else if (elementTypeStr.compare("unsigned long long int")==0) { vtkPixelType = VTK_UNSIGNED_LONG_LONG; }
  else if (elementTypeStr.compare("uint64")==0) { vtkPixelType = VTK_UNSIGNED_LONG_LONG; }
  else if (elementTypeStr.compare("uint64_t")==0) { vtkPixelType = VTK_UNSIGNED_LONG_LONG; }
  else if (elementTypeStr.compare("float")==0) { vtkPixelType = VTK_FLOAT; }
  else if (elementTypeStr.compare("double")==0) { vtkPixelType = VTK_DOUBLE; }
  else
  {
    LOG_ERROR("Unknown Nrrd data type: " << elementTypeStr);
    vtkPixelType=VTK_VOID;
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkNrrdSequenceIO::ConvertVtkPixelTypeToNrrdType(PlusCommon::VTKScalarPixelType vtkPixelType, std::string &elementTypeStr)
{
  if (vtkPixelType==VTK_VOID)
  {
    elementTypeStr="MET_OTHER";
    return PLUS_SUCCESS;
  }
  const char* ElementTypes[]={
    "int8",
    "uint8",
    "int16",
    "uint16",
    "int32",
    "uint32",
    "int64",
    "uint64",
    "float",
    "double",
  };

  PlusCommon::VTKScalarPixelType testedPixelType=VTK_VOID;
  for (unsigned int i=0; i<sizeof(ElementTypes); i++)
  {    
    if (ConvertNrrdTypeToVtkPixelType(ElementTypes[i], testedPixelType)!=PLUS_SUCCESS)
    {
      continue;
    }
    if (testedPixelType==vtkPixelType)
    {
      elementTypeStr=ElementTypes[i];
      return PLUS_SUCCESS;
    }
  }
  elementTypeStr="MET_OTHER";
  return PLUS_FAIL;
}

//----------------------------------------------------------------------------
std::string vtkNrrdSequenceIO::GetPixelDataFilePath()
{
  if ( this->PixelDataFileName.empty() )
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
PlusStatus vtkNrrdSequenceIO::UpdateFieldInImageHeader(const char* fieldName)
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
      int paddingCharactersNeeded=lineStr.size()-newLineStr.str().size(); // need to add padding whitespace characters to fully replace the old line 
      if (paddingCharactersNeeded<0)
      {
        LOG_ERROR("Cannot update line in image header (the new string '"<<newLineStr.str()<<"' is longer than the current string '"<<lineStr<<"')");
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
PlusStatus vtkNrrdSequenceIO::FileOpen(FILE **stream, const char* filename, const char* flags)
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
PlusStatus vtkNrrdSequenceIO::PrepareHeader()
{
  if (this->EnableImageDataWrite && this->TrackedFrameList->IsContainingValidImageData())
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
  }

  if( this->TempHeaderFileName.empty())
  {
    std::string tempFilename;
    if( PlusCommon::CreateTemporaryFilename(tempFilename, vtkPlusConfig::GetInstance()->GetOutputDirectory()) != PLUS_SUCCESS )
    {
      LOG_ERROR("Unable to create temporary header file. Check write access.");
      return PLUS_FAIL;
    }
    this->TempHeaderFileName=tempFilename;
  }

  if( this->TempImageFileName.empty() )
  {
    std::string tempFilename;
    if( PlusCommon::CreateTemporaryFilename(tempFilename, vtkPlusConfig::GetInstance()->GetOutputDirectory()) != PLUS_SUCCESS )
    {
      LOG_ERROR("Unable to create temporary image file. Check write access.");
      return PLUS_FAIL;
    }
    this->TempImageFileName=tempFilename;
  }

  if ( this->OpenImageHeader() != PLUS_SUCCESS)
  {
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkNrrdSequenceIO::WriteImages()
{
  if (WriteImagePixels(this->TempImageFileName, false) != PLUS_SUCCESS)
  {
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkNrrdSequenceIO::AppendImages()
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
PlusStatus vtkNrrdSequenceIO::Close()
{
  // Update fields that are known only at the end of the processing
  if (GetUseCompression())
  {
    if (UpdateFieldInImageHeader("CompressedDataSize")!=PLUS_SUCCESS)
    {
      return PLUS_FAIL;
    }
  }

  std::string headerFullPath = vtkPlusConfig::GetInstance()->GetOutputPath(this->FileName);

  // Rename header to final filename
  RenameFile(this->TempHeaderFileName.c_str(), headerFullPath.c_str());

  if( this->PixelDataFileName.empty() )
  {
    // Append image to final file (nrrd)
    AppendFile(this->TempImageFileName, headerFullPath.c_str());
  }
  else
  {
    // Rename image to final filename (nhdr+raw)
    std::string pixFullPath = vtkPlusConfig::GetInstance()->GetOutputPath(this->PixelDataFileName);
    RenameFile(this->TempImageFileName.c_str(), pixFullPath.c_str());
  }

  this->TempHeaderFileName.clear();
  this->TempImageFileName.clear();

  CurrentFrameOffset = 0;
  TotalBytesWritten = 0;

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkNrrdSequenceIO::Discard()
{
  vtksys::SystemTools::RemoveFile(this->TempHeaderFileName.c_str());
  vtksys::SystemTools::RemoveFile(this->TempImageFileName.c_str());

  this->TempHeaderFileName.clear();
  this->TempImageFileName.clear();

  CurrentFrameOffset = 0;
  TotalBytesWritten = 0;

  return PLUS_SUCCESS;
}


//----------------------------------------------------------------------------
PlusStatus vtkNrrdSequenceIO::SetFileName( const std::string& aFilename )
{
  this->FileName.clear();
  this->PixelDataFileName.clear();

  if( aFilename.empty() )
  {
    LOG_ERROR("Invalid Nrrd file name");
  }

  this->FileName = aFilename;
  // Trim whitespace and " characters from the beginning and end of the filename
  this->FileName.erase(this->FileName.find_last_not_of(" \"\t\r\n")+1);
  this->FileName.erase(0,this->FileName.find_first_not_of(" \"\t\r\n"));

  // Set pixel data filename at the same time
  std::string fileExt = vtksys::SystemTools::GetFilenameLastExtension(this->FileName);
  if (STRCASECMP(fileExt.c_str(),".nrrd")==0)
  {
    this->PixelDataFileName=std::string(""); //empty string denotes local storage
  }
  else if (STRCASECMP(fileExt.c_str(),".nhdr")==0)
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
    LOG_WARNING("Writing sequence file with '" << fileExt << "' extension is not supported. Using nrrd extension instead.");
    this->FileName+=".nrrd";
    this->PixelDataFileName=std::string(""); //empty string denotes local storage
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkNrrdSequenceIO::AppendFile(const std::string& sourceFilename, const std::string& destFilename)
{
  FILE* in = fopen( sourceFilename.c_str(), "rb" ) ;
  FILE* out = NULL;
  out = fopen( destFilename.c_str(), "ab+");
  if( in == NULL || out == NULL )
  {
    LOG_ERROR( "An error occurred while appending data from " << sourceFilename << " to " << destFilename) ;
    return PLUS_FAIL;
  }
  else
  {
    const int BUFFER_SIZE = 32000;
    char *buffer = new char[BUFFER_SIZE];
    size_t len = 0 ;
    while( (len = fread( buffer, 1, BUFFER_SIZE, in)) > 0 )
    {
      fwrite( buffer, 1, len, out ) ;
      memset(buffer, 0, BUFFER_SIZE);
    }
    fclose(in);
    fclose(out);
    if( !itksys::SystemTools::RemoveFile(sourceFilename.c_str()) )
    {
      LOG_WARNING("Unable to remove the file " << sourceFilename << " after append is completed");
    }
    delete[] buffer;
  }
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkNrrdSequenceIO::OverwriteNumberOfFramesInHeader(int numberOfFrames, bool addPadding)
{
  bool isData3D = this->Dimensions[2] > 1;

  std::stringstream dimSizeStr;
  this->Dimensions[3]=numberOfFrames;
  dimSizeStr << this->Dimensions[0] << " " << this->Dimensions[1] << " ";
  if( isData3D )
  {
    dimSizeStr << this->Dimensions[2] << " ";
  }
  dimSizeStr << this->Dimensions[3];
  if( addPadding )
  {
    dimSizeStr << "                              ";  // add spaces so that later the field can be updated with larger values
  }
  this->SetCustomString("DimSize", dimSizeStr.str().c_str());

  return PLUS_SUCCESS;
}


//----------------------------------------------------------------------------
PlusStatus vtkNrrdSequenceIO::RenameFile(const char* oldname, const char* newname)
{
  // Adopted from CMake's cmSystemTools.cxx
  bool success = false;
#ifdef _WIN32
  // On Windows the move functions will not replace existing files. Check if the destination exists.
  if (itksys::SystemTools::FileExists( newname, true))
  {
    // The destination exists.  We have to replace it carefully.  The
    // MoveFileEx function does what we need but is not available on
    // Win9x.
    OSVERSIONINFO osv;
    DWORD attrs;

    // Make sure the destination is not read only.
    attrs = GetFileAttributes(newname);
    if(attrs & FILE_ATTRIBUTE_READONLY)
    {
      SetFileAttributes(newname, attrs & ~FILE_ATTRIBUTE_READONLY);
    }

    // Check the windows version number.
    osv.dwOSVersionInfoSize = sizeof(osv);
    GetVersionEx(&osv);
    if(osv.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS)
    {
      // This is Win9x.  There is no MoveFileEx implementation.  We
      // cannot quite rename the file atomically.  Just delete the
      // destination and then move the file.
      DeleteFile(newname);
      success = (MoveFile(oldname, newname)!=0);
    }
    else
    {
      // This is not Win9x.  Use the MoveFileEx implementation.
      success = (MoveFileEx(oldname, newname, MOVEFILE_REPLACE_EXISTING)!=0);
    }
  }
  else
  {
    // The destination does not exist.  Just move the file.
    success = (MoveFile(oldname, newname)!=0);
  }
#else
  /* On UNIX we have an OS-provided call to do this atomically.  */
  success = (rename(oldname, newname) == 0);
#endif
  return success ? PLUS_SUCCESS : PLUS_FAIL;
}

//----------------------------------------------------------------------------
vtkNrrdSequenceIO::FilePositionOffsetType vtkNrrdSequenceIO::GetFileSize(const std::string& filename)
{
  struct stat stat_buf;
  int rc = stat(filename.c_str(), &stat_buf);
  return rc == 0 ? stat_buf.st_size : -1;
}

//----------------------------------------------------------------------------
vtkNrrdSequenceIO::NrrdEncoding vtkNrrdSequenceIO::StringToNrrdEncoding(const std::string& encoding)
{
  if(encoding.compare("raw")==0){return NRRD_ENCODING_RAW;}
  else if(encoding.compare("txt")==0){return NRRD_ENCODING_TXT;}
  else if(encoding.compare("text")==0){return NRRD_ENCODING_TEXT;}
  else if(encoding.compare("ascii")==0){return NRRD_ENCODING_ASCII;}
  else if(encoding.compare("hex")==0){return NRRD_ENCODING_HEX;}
  else if(encoding.compare("gz")==0){return NRRD_ENCODING_GZ;}
  else if(encoding.compare("gzip")==0){return NRRD_ENCODING_GZIP;}
  else if(encoding.compare("bz2")==0){return NRRD_ENCODING_BZ2;}
  else if(encoding.compare("bzip2")==0){return NRRD_ENCODING_BZIP2;}
  else
  {
    return NRRD_ENCODING_RAW;
  }
}

//----------------------------------------------------------------------------
std::string vtkNrrdSequenceIO::NrrdEncodingToString(NrrdEncoding encoding)
{
  if(encoding == NRRD_ENCODING_RAW){return std::string("raw");}
  else if(encoding == NRRD_ENCODING_TXT){return std::string("txt");}
  else if(encoding == NRRD_ENCODING_TEXT){return std::string("text");}
  else if(encoding == NRRD_ENCODING_ASCII){return std::string("ascii");}
  else if(encoding == NRRD_ENCODING_HEX){return std::string("hex");}
  else if(encoding == NRRD_ENCODING_GZ){return std::string("gz");}
  else if(encoding == NRRD_ENCODING_GZIP){return std::string("gzip");}
  else if(encoding == NRRD_ENCODING_BZ2){return std::string("bz2");}
  else if(encoding == NRRD_ENCODING_BZIP2){return std::string("bzip2");}
  else
  {
    return "raw";
  }
}
