/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"
#include "itksys/SystemTools.hxx"
#include "vtkNrrdReader.h"
#include "vtkPlusNrrdSequenceIO.h"
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

#include "PlusTrackedFrame.h"
#include "vtkObjectFactory.h"
#include "vtkPlusTrackedFrameList.h"
#include "vtksys/SystemTools.hxx"

#if defined(_WIN32)
  #include <io.h>
#endif

namespace
{

  static const char* SEQUENCE_FIELD_ELEMENT_DATA_FILE = "data file";
  static const char* SEQUENCE_FIELD_KINDS = "kinds";
  static const char* SEQUENCE_FIELD_SIZES = "sizes";
  static const char* SEQUENCE_FIELD_SPACE_DIRECTIONS = "space directions";
  static const char* SEQUENCE_FIELD_SPACE_ORIGIN = "space origin";
  static const int MAX_LINE_LENGTH = 1000;
  static const int SEQUENCE_FIELD_PADDED_LINE_LENGTH = 40;
  static const std::string SEQUENCE_FIELD_US_IMG_ORIENT = std::string("ultrasound image orientation");
  static const std::string SEQUENCE_FIELD_US_IMG_TYPE = std::string("ultrasound image type");
  static std::string SEQUENCE_FIELD_FRAME_FIELD_PREFIX = "Seq_Frame";
  static std::string SEQUENCE_FIELD_IMG_STATUS = "Status";

}

//----------------------------------------------------------------------------

vtkStandardNewMacro(vtkPlusNrrdSequenceIO);

//----------------------------------------------------------------------------
vtkPlusNrrdSequenceIO::vtkPlusNrrdSequenceIO()
  : vtkPlusSequenceIOBase()
  , Encoding(NRRD_ENCODING_RAW)
  , CompressionStream(NULL)
{
}

//----------------------------------------------------------------------------
vtkPlusNrrdSequenceIO::~vtkPlusNrrdSequenceIO()
{
}

//----------------------------------------------------------------------------
std::string vtkPlusNrrdSequenceIO::EncodingToString(NrrdEncoding encoding)
{
  switch (encoding)
  {
    case vtkPlusNrrdSequenceIO::NRRD_ENCODING_RAW:
      return "NRRD_ENCODING_RAW";
    case vtkPlusNrrdSequenceIO::NRRD_ENCODING_TXT:
      return "NRRD_ENCODING_TXT";
    case vtkPlusNrrdSequenceIO::NRRD_ENCODING_HEX:
      return "NRRD_ENCODING_HEX";
    case vtkPlusNrrdSequenceIO::NRRD_ENCODING_GZ:
      return "NRRD_ENCODING_GZ";
    case vtkPlusNrrdSequenceIO::NRRD_ENCODING_BZ2:
      return "NRRD_ENCODING_BZ2";
  }
  return "";
}

//----------------------------------------------------------------------------
void vtkPlusNrrdSequenceIO::PrintSelf(ostream& os, vtkIndent indent)
{
  Superclass::PrintSelf(os, indent);

  os << indent << "NrrdEncoding: " << EncodingToString(this->Encoding) << std::endl;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusNrrdSequenceIO::ReadImageHeader()
{
  FILE* stream = NULL;
  // open in binary mode because we determine the start of the image buffer also during this read
  if (FileOpen(&stream, this->FileName.c_str(), "rb") != PLUS_SUCCESS)
  {
    LOG_ERROR("The file " << this->FileName << " could not be opened for reading");
    return PLUS_FAIL;
  }

  bool dataFileEntryFound(false);

  char line[MAX_LINE_LENGTH + 1] = {0};
  while (fgets(line, MAX_LINE_LENGTH, stream))
  {
    std::string lineStr = line;

    if (lineStr.compare("\n") == 0)
    {
      if (!dataFileEntryFound)
      {
        // pixel data stored locally
        // this->PixelDataFileName is already empty string unless overriden by data file: field
        this->PixelDataFileOffset = FTELL(stream);
      }
      // this is the last element of the header
      break;
    }

    size_t separatorFound;
    separatorFound = lineStr.find_first_of("#");
    if (separatorFound != std::string::npos || lineStr.find("NRRD") == 0)
    {
      // Header definition or comment found, skip
      continue;
    }

    // Split line into name and value
    separatorFound = lineStr.find_first_of(":");
    if (!separatorFound)
    {
      LOG_WARNING("Parsing line failed, colon is missing (" << lineStr << ")");
      continue;
    }
    std::string name = lineStr.substr(0, separatorFound);
    std::string value = lineStr.substr(separatorFound + 1);
    if (lineStr[separatorFound + 1] == '=')
    {
      // It is a key/value
      value = lineStr.substr(separatorFound + 2);
    }

    // trim spaces from the left and right
    PlusCommon::Trim(name);
    PlusCommon::Trim(value);

    if (!PlusCommon::HasSubstrInsensitive(name, SEQUENCE_FIELD_FRAME_FIELD_PREFIX))
    {
      // field
      SetCustomString(name.c_str(), value.c_str());

      // data file found, separate data file
      if (PlusCommon::IsEqualInsensitive(name, SEQUENCE_FIELD_ELEMENT_DATA_FILE))
      {
        dataFileEntryFound = true;
        this->PixelDataFileName = value;
        this->PixelDataFileOffset = 0;
      }
    }
    else
    {
      // frame field
      // name: Seq_Frame0000_CustomTransform
      name.erase(0, SEQUENCE_FIELD_FRAME_FIELD_PREFIX.size());   // 0000_CustomTransform

      // Split line into name and value
      size_t underscoreFound;
      underscoreFound = name.find_first_of("_");
      if (underscoreFound == std::string::npos)
      {
        LOG_WARNING("Parsing line failed, underscore is missing from frame field name (" << lineStr << ")");
        continue;
      }
      std::string frameNumberStr = name.substr(0, underscoreFound);   // 0000
      std::string frameFieldName = name.substr(underscoreFound + 1);   // CustomTransform

      int frameNumber = 0;
      if (PlusCommon::StringToInt(frameNumberStr.c_str(), frameNumber) != PLUS_SUCCESS)
      {
        LOG_WARNING("Parsing line failed, cannot get frame number from frame field (" << lineStr << ")");
        continue;
      }
      SetFrameString(frameNumber, frameFieldName.c_str(), value.c_str());

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

  fclose(stream);

  int nDims = 3;
  if (PlusCommon::StringToInt(this->TrackedFrameList->GetCustomString("dimension"), nDims) == PLUS_SUCCESS)
  {
    if (nDims != 2 && nDims != 3 && nDims != 4)
    {
      LOG_ERROR("Invalid dimension (shall be 2 or 3 or 4): " << nDims);
      return PLUS_FAIL;
    }
  }
  this->NumberOfDimensions = nDims;

  std::vector<std::string> kinds;
  if (this->TrackedFrameList->GetCustomString(SEQUENCE_FIELD_KINDS) != NULL)
  {
    PlusCommon::SplitStringIntoTokens(std::string(this->TrackedFrameList->GetCustomString(SEQUENCE_FIELD_KINDS)), ' ', kinds);
  }
  else
  {
    LOG_ERROR(SEQUENCE_FIELD_KINDS << " not found in file: " << this->FileName << ". Unable to read.");
    return PLUS_FAIL;
  }

  // sizes = 640 480 1 1, sizes = 640 480 1 567, sizes = 640 480 40 567
  std::istringstream issDimSize(this->TrackedFrameList->GetCustomString(SEQUENCE_FIELD_SIZES));
  unsigned int dimSize(0);
  unsigned int spatialDomainCount(0);
  for (unsigned int i = 0; i < kinds.size(); i++)
  {
    issDimSize >> dimSize;
    if (PlusCommon::IsEqualInsensitive(kinds[i], "domain"))
    {
      if (spatialDomainCount == 3 && dimSize > 1)  // 0-indexed, this is the 4th spatial domain
      {
        LOG_ERROR("PLUS supports up to 3 spatial domains. File: " << this->FileName << " contains more than 3.");
        return PLUS_FAIL;
      }
      this->Dimensions[spatialDomainCount] = dimSize;
      spatialDomainCount++;
    }
    else if (PlusCommon::IsEqualInsensitive(kinds[i], "time") || PlusCommon::IsEqualInsensitive(kinds[i], "list"))  // time = resampling ok, list = resampling not ok
    {
      this->Dimensions[3] = dimSize;
    }
    else if (PlusCommon::IsEqualInsensitive(kinds[i], "vector"))
    {
      this->NumberOfScalarComponents = dimSize;
    }
  }

  // Post process to handle setting 3rd dimension to 0 if there is no image data in the file
  if (this->Dimensions[0] == 0 || this->Dimensions[1] == 0)
  {
    this->Dimensions[2] = 0;
  }

  // Only check image related settings if dimensions are not 0 0 0
  if (this->Dimensions[0] != 0 && this->Dimensions[1] != 0 && this->Dimensions[2] != 0)
  {
    const char* elementType = this->TrackedFrameList->GetCustomString("type");
    if (elementType == NULL)
    {
      LOG_ERROR("Field type not found in file: " << this->FileName << ". Unable to read.");
      return PLUS_FAIL;
    }
    else if (ConvertNrrdTypeToVtkPixelType(elementType, this->PixelType) != PLUS_SUCCESS)
    {
      LOG_ERROR("Unknown component type: " << elementType);
      return PLUS_FAIL;
    }

    std::string imgOrientStr;
    if (GetCustomString(SEQUENCE_FIELD_US_IMG_ORIENT.c_str()) != NULL)
    {
      imgOrientStr = std::string(GetCustomString(SEQUENCE_FIELD_US_IMG_ORIENT.c_str()));
    }
    else
    {
      imgOrientStr = PlusVideoFrame::GetStringFromUsImageOrientation(US_IMG_ORIENT_MF);
      LOG_WARNING(SEQUENCE_FIELD_US_IMG_ORIENT << " field not found in header. Defaulting to " << imgOrientStr << ".");
    }
    this->ImageOrientationInFile = PlusVideoFrame::GetUsImageOrientationFromString(imgOrientStr.c_str());

    // TODO: handle detection of image orientation in file from space/space dimensions, space origin and space directions
    // handle only orthogonal rotations

    const char* imgTypeStr = GetCustomString(SEQUENCE_FIELD_US_IMG_TYPE.c_str());
    if (imgTypeStr == NULL)
    {
      // if the image type is not defined then assume that it is B-mode image
      this->ImageType = US_IMG_BRIGHTNESS;
      LOG_WARNING(SEQUENCE_FIELD_US_IMG_TYPE << " field not found in header. Defaulting to US_IMG_BRIGHTNESS.");
    }
    else
    {
      this->ImageType = PlusVideoFrame::GetUsImageTypeFromString(imgTypeStr);
    }

    // If no specific image orientation is requested then determine it automatically from the image type
    // B-mode: MF
    // RF-mode: FM
    if (this->ImageOrientationInMemory == US_IMG_ORIENT_XX)
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
          if (this->Dimensions[0] == 0 && this->Dimensions[1] == 0 && this->Dimensions[2] == 0)
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

    if (this->TrackedFrameList->GetCustomString("encoding") != NULL)
    {
      // set fields according to encoding
      std::string encoding = std::string(this->TrackedFrameList->GetCustomString("encoding"));
      this->Encoding = vtkPlusNrrdSequenceIO::StringToNrrdEncoding(encoding);
      if (this->Encoding >= NRRD_ENCODING_GZ)
      {
        this->UseCompression = true;
      }
      if (this->Encoding >= NRRD_ENCODING_BZ2)
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
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
// Read the spacing and dimensions of the image.
PlusStatus vtkPlusNrrdSequenceIO::ReadImagePixels()
{
  int frameCount = this->Dimensions[3];
  unsigned int frameSizeInBytes = 0;
  if (this->Dimensions[0] > 0 && this->Dimensions[1] > 0 && this->Dimensions[2] > 0)
  {
    frameSizeInBytes = this->Dimensions[0] * this->Dimensions[1] * this->Dimensions[2] * PlusVideoFrame::GetNumberOfBytesPerScalar(this->PixelType) * this->NumberOfScalarComponents;
  }

  if (frameSizeInBytes == 0)
  {
    LOG_DEBUG("No image data in the file");
    return PLUS_SUCCESS;
  }

  int numberOfErrors = 0;

  FILE* stream = NULL;
  gzFile gzStream = NULL;

  if (this->UseCompression && this->Encoding >= NRRD_ENCODING_GZ && this->Encoding < NRRD_ENCODING_BZ2)
  {
    if (FileOpen(&stream, this->GetPixelDataFilePath().c_str(), "rb") != PLUS_SUCCESS)
    {
      LOG_ERROR("The file " << this->GetPixelDataFilePath() << " could not be opened for reading");
      fclose(stream);
      return PLUS_FAIL;
    }

#if _WIN32
    int fd = _fileno(stream);
    int dupFd = _dup(fd);
    _lseek(dupFd, this->PixelDataFileOffset, SEEK_SET);
#else
    int fd = fileno(stream);
    int dupFd = dup(fd);
    lseek(dupFd, this->PixelDataFileOffset, SEEK_SET);
#endif

    gzStream = gzdopen(dupFd, "rb");

    if (gzStream == NULL)
    {
      LOG_ERROR("Unable to open gz stream.");
      fclose(stream);
      return PLUS_FAIL;
    }
  }
  else
  {
    if (FileOpen(&stream, this->GetPixelDataFilePath().c_str(), "rb") != PLUS_SUCCESS)
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
    unsigned int allFramesPixelBufferSize = frameCount * frameSizeInBytes;
    gzAllFramesPixelBuffer = new unsigned char[allFramesPixelBufferSize];

    vtkPlusNrrdSequenceIO::FilePositionOffsetType allFramesCompressedPixelBufferSize = vtkPlusNrrdSequenceIO::GetFileSize(this->GetPixelDataFilePath()) - this->PixelDataFileOffset;

    if (gzread(gzStream, (void*)gzAllFramesPixelBuffer, allFramesPixelBufferSize) != allFramesPixelBufferSize)
    {
      LOG_ERROR("Could not uncompress " << allFramesCompressedPixelBufferSize << " bytes to " << allFramesPixelBufferSize << " bytes from " << GetPixelDataFilePath());
      gzclose(gzStream);
      return PLUS_FAIL;
    }
    gzclose(gzStream);
  }

  std::vector<unsigned char> pixelBuffer;
  pixelBuffer.resize(frameSizeInBytes);
  for (int frameNumber = 0; frameNumber < frameCount; frameNumber++)
  {
    this->CreateTrackedFrameIfNonExisting(frameNumber);
    PlusTrackedFrame* trackedFrame = this->TrackedFrameList->GetTrackedFrame(frameNumber);

    // Allocate frame only if it is valid
    const char* imgStatus = trackedFrame->GetFrameField(SEQUENCE_FIELD_IMG_STATUS.c_str());
    if (imgStatus != NULL)    // Found the image status field
    {
      // Save status field
      std::string strImgStatus(imgStatus);

      // Delete image status field from tracked frame
      // Image status can be determine by trackedFrame->GetImageData()->IsImageValid()
      trackedFrame->DeleteFrameField(SEQUENCE_FIELD_IMG_STATUS.c_str());

      if (!PlusCommon::IsEqualInsensitive(strImgStatus, "OK"))     // Image status _not_ OK
      {
        LOG_DEBUG("Frame #" << frameNumber << " image data is invalid, no need to allocate data in the tracked frame list.");
        // TODO : is this right? don't we lose tool info from these dropped frames?
        continue;
      }
    }

    trackedFrame->GetImageData()->SetImageOrientation(this->ImageOrientationInMemory);
    trackedFrame->GetImageData()->SetImageType(this->ImageType);

    FrameSizeType frameSize = { this->Dimensions[0], this->Dimensions[1], this->Dimensions[2] };
    if (trackedFrame->GetImageData()->AllocateFrame(frameSize, this->PixelType, this->NumberOfScalarComponents) != PLUS_SUCCESS)
    {
      LOG_ERROR("Cannot allocate memory for frame " << frameNumber);
      numberOfErrors++;
      continue;
    }

    std::array<int, 3> clipRectOrigin = {PlusCommon::NO_CLIP, PlusCommon::NO_CLIP, PlusCommon::NO_CLIP};
    std::array<int, 3> clipRectSize = {PlusCommon::NO_CLIP, PlusCommon::NO_CLIP, PlusCommon::NO_CLIP};

    PlusVideoFrame::FlipInfoType flipInfo;
    if (PlusVideoFrame::GetFlipAxes(this->ImageOrientationInFile, this->ImageType, this->ImageOrientationInMemory, flipInfo) != PLUS_SUCCESS)
    {
      LOG_ERROR("Failed to convert image data to the requested orientation, from " << PlusVideoFrame::GetStringFromUsImageOrientation(this->ImageOrientationInFile) <<
                " to " << PlusVideoFrame::GetStringFromUsImageOrientation(this->ImageOrientationInMemory));
      return PLUS_FAIL;
    }

    if (!this->UseCompression)
    {
      FilePositionOffsetType offset = this->PixelDataFileOffset + frameNumber * frameSizeInBytes;
      FSEEK(stream, offset, SEEK_SET);
      if (fread(&(pixelBuffer[0]), 1, frameSizeInBytes, stream) != frameSizeInBytes)
      {
        //LOG_ERROR("Could not read "<<frameSizeInBytes<<" bytes from "<<GetPixelDataFilePath());
        //numberOfErrors++;
      }
      FrameSizeType frameSize = { this->Dimensions[0], this->Dimensions[1], this->Dimensions[2] };
      if (PlusVideoFrame::GetOrientedClippedImage(&(pixelBuffer[0]), flipInfo, this->ImageType, this->PixelType, this->NumberOfScalarComponents, frameSize, *trackedFrame->GetImageData(), clipRectOrigin, clipRectSize) != PLUS_SUCCESS)
      {
        LOG_ERROR("Failed to get oriented image from sequence file (frame number: " << frameNumber << ")!");
        numberOfErrors++;
        continue;
      }
    }
    else
    {
      FrameSizeType frameSize = { this->Dimensions[0], this->Dimensions[1], this->Dimensions[2] };
      if (PlusVideoFrame::GetOrientedClippedImage(gzAllFramesPixelBuffer + frameNumber * frameSizeInBytes, flipInfo, this->ImageType, this->PixelType, this->NumberOfScalarComponents, frameSize, *trackedFrame->GetImageData(), clipRectOrigin, clipRectSize) != PLUS_SUCCESS)
      {
        LOG_ERROR("Failed to get oriented image from sequence file (frame number: " << frameNumber << ")!");
        numberOfErrors++;
        continue;
      }
    }
  }

  if (!this->UseCompression)
  {
    fclose(stream);
  }

  if (numberOfErrors > 0)
  {
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusNrrdSequenceIO::PrepareImageFile()
{
  if (this->GetUseCompression())
  {
    this->CompressionStream = gzopen(this->TempImageFileName.c_str(), "ab");

    int error;
    gzerror(this->CompressionStream, &error);

    if (error != Z_OK)
    {
      LOG_ERROR("Unable to open compressed file for writing.");
      return PLUS_FAIL;
    }
  }
  else if (FileOpen(&this->OutputImageFileHandle, this->TempImageFileName.c_str(), "ab+") != PLUS_SUCCESS)
  {
    LOG_ERROR("Unable to open output stream for writing.");
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
bool vtkPlusNrrdSequenceIO::CanReadFile(const std::string& filename)
{
  vtkSmartPointer<vtkNrrdReader> reader = vtkSmartPointer<vtkNrrdReader>::New();

  return reader->CanReadFile(filename.c_str());
}

//----------------------------------------------------------------------------
bool vtkPlusNrrdSequenceIO::CanWriteFile(const std::string& filename)
{
  if (PlusCommon::IsEqualInsensitive(vtksys::SystemTools::GetFilenameLastExtension(filename), ".nrrd")  ||
      PlusCommon::IsEqualInsensitive(vtksys::SystemTools::GetFilenameLastExtension(filename), ".nhdr"))
  {
    return true;
  }

  return false;
}

//----------------------------------------------------------------------------
/** Writes the spacing and dimensions of the image.
* Assumes SetFileName has been called with a valid file name. */
PlusStatus vtkPlusNrrdSequenceIO::WriteInitialImageHeader()
{
  if (this->TrackedFrameList->GetNumberOfTrackedFrames() == 0)
  {
    LOG_ERROR("No frames in frame list, unable to query a frame for meta information.");
    return PLUS_FAIL;
  }

  // First, is this 2D or 3D?
  bool isData3D = (this->TrackedFrameList->GetTrackedFrame(0)->GetFrameSize()[2] > 1);
  bool isDataTimeSeries = this->IsDataTimeSeries; // don't compute it from the number of frames because we may still have only one frame but acquire more frames later

  if (this->TrackedFrameList->GetTrackedFrame(0)->GetNumberOfScalarComponents(this->NumberOfScalarComponents) == PLUS_FAIL)
  {
    LOG_ERROR("Unable to retrieve number of scalar components.");
    return PLUS_FAIL;
  }

  // Override fields
  this->NumberOfDimensions = isData3D ? 3 : 2;
  if (isDataTimeSeries)
  {
    this->NumberOfDimensions++;
  }
  if (this->NumberOfScalarComponents > 1)
  {
    this->NumberOfDimensions++;
  }

  SetCustomString("dimension", this->NumberOfDimensions);

  // CompressedData
  SetCustomString("encoding", GetUseCompression() ? "gz" : "raw");

  FrameSizeType frameSize = {0, 0, 0};
  if (this->EnableImageDataWrite)
  {
    frameSize = this->GetMaximumImageDimensions();
  }
  else
  {
    frameSize[0] = 1;
    frameSize[1] = 1;
    frameSize[2] = 1;
  }

  // Set the dimensions of the data to be written
  this->Dimensions[0] = frameSize[0];
  this->Dimensions[1] = frameSize[1];
  this->Dimensions[2] = frameSize[2];
  this->Dimensions[3] = this->TrackedFrameList->GetNumberOfTrackedFrames();

  if (this->EnableImageDataWrite)
  {
    // Make sure the frame size is the same for each valid image
    // If it's needed, we can use the largest frame size for each frame and copy the image data row by row
    // but then, we need to save the original frame size for each frame and crop the image when we read it
    for (unsigned int frameNumber = 0; frameNumber < this->TrackedFrameList->GetNumberOfTrackedFrames(); frameNumber++)
    {
      FrameSizeType currFrameSize = this->TrackedFrameList->GetTrackedFrame(frameNumber)->GetFrameSize();
      if (this->TrackedFrameList->GetTrackedFrame(frameNumber)->GetImageData()->IsImageValid()
          && (frameSize[0] != currFrameSize[0] || frameSize[1] != currFrameSize[1] || frameSize[2] != currFrameSize[2]))
      {
        LOG_ERROR("Frame size mismatch: expected size (" << frameSize[0] << "x" << frameSize[1] << "x" << frameSize[2]
                  << ") differ from actual size (" << currFrameSize[0] << "x" << currFrameSize[1] << "x" << currFrameSize[2] << ") for frame #" << frameNumber);
        return PLUS_FAIL;
      }
    }
  }

  // Does endian come from somewhere?
  SetCustomString("endian", "little");

  // Update sizes field in header
  this->UpdateDimensionsCustomStrings(this->TrackedFrameList->GetNumberOfTrackedFrames(), isData3D);

  // PixelType
  if (this->TrackedFrameList->IsContainingValidImageData())
  {
    this->PixelType = this->TrackedFrameList->GetPixelType();
    if (this->PixelType == VTK_VOID)
    {
      // If the pixel type was not defined, define it to UCHAR
      this->PixelType = VTK_UNSIGNED_CHAR;
    }
  }
  std::string pixelTypeStr;
  vtkPlusNrrdSequenceIO::ConvertVtkPixelTypeToNrrdType(this->PixelType, pixelTypeStr);
  SetCustomString("type", pixelTypeStr);   // pixel type (a.k.a component type) is stored in the type element

  // Add fields with default values if they are not present already

  // From format definition:
  // "This (or "space dimension") has to precede the other orientation-related fields, because it determines
  // how many components there are in the vectors of the space origin, space directions, and measurement frame fields."
  int numSpaceDimensions = isData3D ? 3 : 2;
  SetCustomString("space dimension", numSpaceDimensions);

  // Generate the origin string, such as (0,0) or (0,0,0)
  std::stringstream originStr;
  originStr << "(";
  for (int i = 0; i < numSpaceDimensions; ++i)
  {
    originStr << "0";
    if (i != numSpaceDimensions - 1)
    {
      originStr << ",";
    }
  }
  originStr << ")";

  // Generate the space direction string, such as
  // (1,0,0) (0,1,0) (0,0,1) none
  // or
  // none (1,0) (0,1)
  // or
  // (1,0,0) (0,1,0) (0,0,1) etc...
  std::stringstream spaceDirectionStr;
  if (this->NumberOfScalarComponents > 1)
  {
    spaceDirectionStr << "none ";
  }
  for (int i = 0; i < numSpaceDimensions; ++i)
  {
    spaceDirectionStr << "(";
    for (int j = 0; j < numSpaceDimensions; ++j)
    {
      if (i == j)
      {
        spaceDirectionStr << "1";
      }
      else
      {
        spaceDirectionStr << "0";
      }
      if (j != numSpaceDimensions - 1)
      {
        spaceDirectionStr << ",";
      }
    }
    spaceDirectionStr << ")";
    if (i != numSpaceDimensions - 1)
    {
      spaceDirectionStr << " ";
    }
  }
  if (isDataTimeSeries)
  {
    spaceDirectionStr << " none";
  }

  // Add fields with default values if they are not present already
  if (GetCustomString(SEQUENCE_FIELD_SPACE_DIRECTIONS) == NULL)
  {
    SetCustomString(SEQUENCE_FIELD_SPACE_DIRECTIONS, spaceDirectionStr.str());
  }
  if (GetCustomString(SEQUENCE_FIELD_SPACE_ORIGIN) == NULL)
  {
    SetCustomString(SEQUENCE_FIELD_SPACE_ORIGIN, originStr.str());
  }

  FILE* stream = NULL;
  // open in binary mode because we determine the start of the image buffer also during this read
  if (FileOpen(&stream, this->TempHeaderFileName.c_str(), "wb") != PLUS_SUCCESS)
  {
    LOG_ERROR("The file " << this->TempHeaderFileName << " could not be opened for writing");
    return PLUS_FAIL;
  }

  // The header shall start with these lines
  std::stringstream nrrdVersionStream;
  nrrdVersionStream << "NRRD0004" << std::endl << "# Complete NRRD file format specification at:" << std::endl << "# http://teem.sourceforge.net/nrrd/format.html" << std::endl;
  nrrdVersionStream << "# File generated by PLUS version " << PLUSLIB_VERSION << std::endl;
  fputs(nrrdVersionStream.str().c_str(), stream);
  this->TotalBytesWritten += strlen(nrrdVersionStream.str().c_str());

  if (!this->PixelDataFileName.empty())
  {
    SetCustomString(SEQUENCE_FIELD_ELEMENT_DATA_FILE, this->PixelDataFileName.c_str());
  }

  std::vector<std::string> fieldNames;
  this->TrackedFrameList->GetCustomFieldNameList(fieldNames);
  for (std::vector<std::string>::iterator it = fieldNames.begin(); it != fieldNames.end(); it++)
  {
    std::string field = (*it) + ": " + GetCustomString(it->c_str()) + "\n";
    fputs(field.c_str(), stream);
    this->TotalBytesWritten += field.length();
  }

  // Image orientation
  if (this->EnableImageDataWrite)
  {
    std::string orientationStr = SEQUENCE_FIELD_US_IMG_ORIENT + ":=" + PlusVideoFrame::GetStringFromUsImageOrientation(this->ImageOrientationInFile) + "\n";
    fputs(orientationStr.c_str(), stream);
    this->TotalBytesWritten += orientationStr.length();
  }

  // Image type
  if (this->EnableImageDataWrite)
  {
    std::string orientationStr = SEQUENCE_FIELD_US_IMG_TYPE + ":=" + PlusVideoFrame::GetStringFromUsImageType(this->ImageType) + "\n";
    fputs(orientationStr.c_str(), stream);
    this->TotalBytesWritten += orientationStr.length();
  }

  fclose(stream);

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusNrrdSequenceIO::AppendImagesToHeader()
{
  FILE* stream = NULL;
  // open in binary mode because we determine the start of the image buffer also during this read
  if (FileOpen(&stream, this->TempHeaderFileName.c_str(), "ab+") != PLUS_SUCCESS)
  {
    LOG_ERROR("The file " << this->TempHeaderFileName << " could not be opened for writing");
    return PLUS_FAIL;
  }

  // Write frame fields (Seq_Frame0000_... = ...)
  for (unsigned int frameNumber = CurrentFrameOffset; frameNumber < this->TrackedFrameList->GetNumberOfTrackedFrames() + CurrentFrameOffset; frameNumber++)
  {
    LOG_DEBUG("Writing frame " << frameNumber);
    unsigned int adjustedFrameNumber = frameNumber - CurrentFrameOffset;
    PlusTrackedFrame* trackedFrame = this->TrackedFrameList->GetTrackedFrame(adjustedFrameNumber);

    std::ostringstream frameIndexStr;
    frameIndexStr << std::setfill('0') << std::setw(4) << frameNumber;

    std::vector<std::string> fieldNames;
    trackedFrame->GetFrameFieldNameList(fieldNames);

    for (std::vector<std::string>::iterator it = fieldNames.begin(); it != fieldNames.end(); it++)
    {
      std::string field = SEQUENCE_FIELD_FRAME_FIELD_PREFIX + frameIndexStr.str() + "_" + (*it) + ":=" + trackedFrame->GetFrameField(it->c_str()) + "\n";
      fputs(field.c_str(), stream);
      TotalBytesWritten += field.length();
    }
    //Only write this field if the image is saved. If only the tracking pose is kept do not save this field to the header
    if (this->EnableImageDataWrite)
    {
      // Add image status field
      std::string imageStatus("OK");
      if (!trackedFrame->GetImageData()->IsImageValid())
      {
        imageStatus = "INVALID";
      }
      std::string imgStatusField = SEQUENCE_FIELD_FRAME_FIELD_PREFIX + frameIndexStr.str() + "_" + SEQUENCE_FIELD_IMG_STATUS + ":=" + imageStatus + "\n";
      fputs(imgStatusField.c_str(), stream);
      TotalBytesWritten += imgStatusField.length();
    }
  }

  fclose(stream);

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusNrrdSequenceIO::FinalizeHeader()
{
  FILE* stream = NULL;
  // open in binary mode because we determine the start of the image buffer also during this read
  if (FileOpen(&stream, this->TempHeaderFileName.c_str(), "ab+") != PLUS_SUCCESS)
  {
    LOG_ERROR("The file " << this->TempHeaderFileName << " could not be opened for writing");
    return PLUS_FAIL;
  }

  if (this->PixelDataFileName.empty())
  {
    std::string elem("\n");
    fputs(elem.c_str(), stream);
    TotalBytesWritten += elem.length();
  }

  fclose(stream);

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusNrrdSequenceIO::Close()
{
  if (this->GetUseCompression())
  {
    gzclose(this->CompressionStream);
  }
  else
  {
    fclose(this->OutputImageFileHandle);
  }

  return Superclass::Close();
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusNrrdSequenceIO::WriteCompressedImagePixelsToFile(int& compressedDataSize)
{
  LOG_DEBUG("Writing compressed pixel data into file started");

  compressedDataSize = 0;

  // Create a blank frame if we have to write an invalid frame to file
  PlusVideoFrame blankFrame;
  FrameSizeType frameSize = { this->Dimensions[0], this->Dimensions[1], this->Dimensions[2] };
  if (blankFrame.AllocateFrame(frameSize, this->PixelType, this->NumberOfScalarComponents) != PLUS_SUCCESS)
  {
    LOG_ERROR("Failed to allocate space for blank image.");
    return PLUS_FAIL;
  }
  blankFrame.FillBlank();

  for (unsigned int frameNumber = 0; frameNumber < this->TrackedFrameList->GetNumberOfTrackedFrames(); frameNumber++)
  {
    PlusTrackedFrame* trackedFrame(NULL);

    if (this->EnableImageDataWrite)
    {
      trackedFrame = this->TrackedFrameList->GetTrackedFrame(frameNumber);
      if (trackedFrame == NULL)
      {
        LOG_ERROR("Cannot access frame " << frameNumber << " while trying to writing compress data into file");
        gzclose(this->CompressionStream);
        return PLUS_FAIL;
      }
    }

    PlusVideoFrame* videoFrame = &blankFrame;
    if (this->EnableImageDataWrite)
    {
      if (trackedFrame->GetImageData()->IsImageValid())
      {
        videoFrame = trackedFrame->GetImageData();
      }
    }

    size_t numberOfBytesReadyForWriting = videoFrame->GetFrameSizeInBytes();
    if (gzwrite(this->CompressionStream, (Bytef*)videoFrame->GetScalarPointer(), numberOfBytesReadyForWriting) != numberOfBytesReadyForWriting)
    {
      LOG_ERROR("Error writing compressed data into file");
      gzclose(this->CompressionStream);
      return PLUS_FAIL;
    }
    int errnum;
    gzerror(this->CompressionStream, &errnum);
    if (errnum != Z_OK)
    {
      LOG_ERROR("Error writing compressed data into file. errnum: " << errnum);
      gzclose(this->CompressionStream);
      return PLUS_FAIL;
    }
    compressedDataSize += numberOfBytesReadyForWriting;
  }

  LOG_DEBUG("Writing compressed pixel data into file completed");

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusNrrdSequenceIO::ConvertNrrdTypeToVtkPixelType(const std::string& elementTypeStr, PlusCommon::VTKScalarPixelType& vtkPixelType)
{
  if (PlusCommon::IsEqualInsensitive(elementTypeStr, "signed char"))
  {
    vtkPixelType = VTK_CHAR;
  }
  else if (PlusCommon::IsEqualInsensitive(elementTypeStr, "int8"))
  {
    vtkPixelType = VTK_CHAR;
  }
  else if (PlusCommon::IsEqualInsensitive(elementTypeStr, "int8_t"))
  {
    vtkPixelType = VTK_CHAR;
  }
  else if (PlusCommon::IsEqualInsensitive(elementTypeStr, "uchar"))
  {
    vtkPixelType = VTK_UNSIGNED_CHAR;
  }
  else if (PlusCommon::IsEqualInsensitive(elementTypeStr, "unsigned char"))
  {
    vtkPixelType = VTK_UNSIGNED_CHAR;
  }
  else if (PlusCommon::IsEqualInsensitive(elementTypeStr, "uint8"))
  {
    vtkPixelType = VTK_UNSIGNED_CHAR;
  }
  else if (PlusCommon::IsEqualInsensitive(elementTypeStr, "uint8_t"))
  {
    vtkPixelType = VTK_UNSIGNED_CHAR;
  }
  else if (PlusCommon::IsEqualInsensitive(elementTypeStr, "short"))
  {
    vtkPixelType = VTK_SHORT;
  }
  else if (PlusCommon::IsEqualInsensitive(elementTypeStr, "short int"))
  {
    vtkPixelType = VTK_SHORT;
  }
  else if (PlusCommon::IsEqualInsensitive(elementTypeStr, "signed short"))
  {
    vtkPixelType = VTK_SHORT;
  }
  else if (PlusCommon::IsEqualInsensitive(elementTypeStr, "signed short int"))
  {
    vtkPixelType = VTK_SHORT;
  }
  else if (PlusCommon::IsEqualInsensitive(elementTypeStr, "int16"))
  {
    vtkPixelType = VTK_SHORT;
  }
  else if (PlusCommon::IsEqualInsensitive(elementTypeStr, "int16_t"))
  {
    vtkPixelType = VTK_SHORT;
  }
  else if (PlusCommon::IsEqualInsensitive(elementTypeStr, "ushort"))
  {
    vtkPixelType = VTK_UNSIGNED_SHORT;
  }
  else if (PlusCommon::IsEqualInsensitive(elementTypeStr, "unsigned short"))
  {
    vtkPixelType = VTK_UNSIGNED_SHORT;
  }
  else if (PlusCommon::IsEqualInsensitive(elementTypeStr, "unsigned short int"))
  {
    vtkPixelType = VTK_UNSIGNED_SHORT;
  }
  else if (PlusCommon::IsEqualInsensitive(elementTypeStr, "uint16"))
  {
    vtkPixelType = VTK_UNSIGNED_SHORT;
  }
  else if (PlusCommon::IsEqualInsensitive(elementTypeStr, "uint16_t"))
  {
    vtkPixelType = VTK_UNSIGNED_SHORT;
  }
  else if (PlusCommon::IsEqualInsensitive(elementTypeStr, "int"))
  {
    vtkPixelType = VTK_INT;
  }
  else if (PlusCommon::IsEqualInsensitive(elementTypeStr, "signed int"))
  {
    vtkPixelType = VTK_INT;
  }
  else if (PlusCommon::IsEqualInsensitive(elementTypeStr, "int32"))
  {
    vtkPixelType = VTK_INT;
  }
  else if (PlusCommon::IsEqualInsensitive(elementTypeStr, "int32_t"))
  {
    vtkPixelType = VTK_INT;
  }
  else if (PlusCommon::IsEqualInsensitive(elementTypeStr, "uint"))
  {
    vtkPixelType = VTK_UNSIGNED_INT;
  }
  else if (PlusCommon::IsEqualInsensitive(elementTypeStr, "unsigned int"))
  {
    vtkPixelType = VTK_UNSIGNED_INT;
  }
  else if (PlusCommon::IsEqualInsensitive(elementTypeStr, "uint32"))
  {
    vtkPixelType = VTK_UNSIGNED_INT;
  }
  else if (PlusCommon::IsEqualInsensitive(elementTypeStr, "uint32_t"))
  {
    vtkPixelType = VTK_UNSIGNED_INT;
  }
  else if (PlusCommon::IsEqualInsensitive(elementTypeStr, "longlong"))
  {
    vtkPixelType = VTK_LONG;
  }
  else if (PlusCommon::IsEqualInsensitive(elementTypeStr, "long long"))
  {
    vtkPixelType = VTK_LONG_LONG;
  }
  else if (PlusCommon::IsEqualInsensitive(elementTypeStr, "long long int"))
  {
    vtkPixelType = VTK_LONG_LONG;
  }
  else if (PlusCommon::IsEqualInsensitive(elementTypeStr, "signed long long"))
  {
    vtkPixelType = VTK_LONG_LONG;
  }
  else if (PlusCommon::IsEqualInsensitive(elementTypeStr, "signed long long int"))
  {
    vtkPixelType = VTK_LONG_LONG;
  }
  else if (PlusCommon::IsEqualInsensitive(elementTypeStr, "int64"))
  {
    vtkPixelType = VTK_LONG_LONG;
  }
  else if (PlusCommon::IsEqualInsensitive(elementTypeStr, "int64_t"))
  {
    vtkPixelType = VTK_LONG_LONG;
  }
  else if (PlusCommon::IsEqualInsensitive(elementTypeStr, "ulonglong"))
  {
    vtkPixelType = VTK_UNSIGNED_LONG_LONG;
  }
  else if (PlusCommon::IsEqualInsensitive(elementTypeStr, "unsigned long long"))
  {
    vtkPixelType = VTK_UNSIGNED_LONG_LONG;
  }
  else if (PlusCommon::IsEqualInsensitive(elementTypeStr, "unsigned long long int"))
  {
    vtkPixelType = VTK_UNSIGNED_LONG_LONG;
  }
  else if (PlusCommon::IsEqualInsensitive(elementTypeStr, "uint64"))
  {
    vtkPixelType = VTK_UNSIGNED_LONG_LONG;
  }
  else if (PlusCommon::IsEqualInsensitive(elementTypeStr, "uint64_t"))
  {
    vtkPixelType = VTK_UNSIGNED_LONG_LONG;
  }
  else if (PlusCommon::IsEqualInsensitive(elementTypeStr, "float"))
  {
    vtkPixelType = VTK_FLOAT;
  }
  else if (PlusCommon::IsEqualInsensitive(elementTypeStr, "double"))
  {
    vtkPixelType = VTK_DOUBLE;
  }
  else if (PlusCommon::IsEqualInsensitive(elementTypeStr, "none"))
  {
    vtkPixelType = VTK_VOID;
  }
  else
  {
    LOG_ERROR("Unknown Nrrd data type: " << elementTypeStr);
    vtkPixelType = VTK_VOID;
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusNrrdSequenceIO::ConvertVtkPixelTypeToNrrdType(PlusCommon::VTKScalarPixelType vtkPixelType, std::string& elementTypeStr)
{
  if (vtkPixelType == VTK_VOID)
  {
    elementTypeStr = "none";
    return PLUS_SUCCESS;
  }
  const char* ElementTypes[] =
  {
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

  PlusCommon::VTKScalarPixelType testedPixelType = VTK_VOID;
  for (unsigned int i = 0; i < sizeof(ElementTypes); i++)
  {
    if (ConvertNrrdTypeToVtkPixelType(ElementTypes[i], testedPixelType) != PLUS_SUCCESS)
    {
      continue;
    }
    if (testedPixelType == vtkPixelType)
    {
      elementTypeStr = ElementTypes[i];
      return PLUS_SUCCESS;
    }
  }
  elementTypeStr = "none";
  return PLUS_FAIL;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusNrrdSequenceIO::UpdateFieldInImageHeader(const char* fieldName)
{
  if (fieldName == NULL)
  {
    LOG_ERROR("NULL fieldname sent to vtkPlusNrrdSequenceIO::UpdateFieldInImageHeader");
    return PLUS_FAIL;
  }

  if (this->TempHeaderFileName.empty())
  {
    LOG_ERROR("Cannot update file header, filename is invalid");
    return PLUS_FAIL;
  }

#if _MSC_VER <= 1500
  std::fstream stream(this->TempHeaderFileName.c_str(), std::ios::in | std::ios::out | std::ios::binary);
#else
  std::fstream stream(this->TempHeaderFileName, std::ios::in | std::ios::out | std::ios::binary);
#endif

  if (!stream)
  {
    LOG_ERROR("The file " << this->TempHeaderFileName << " could not be opened for reading and writing");
    return PLUS_FAIL;
  }

  std::string line;
  while (std::getline(stream, line))
  {
    if (line.empty())
    {
      // reached the end of the header
      break;
    }

    // Split line into name and value
    size_t colonFound;
    colonFound = line.find_first_of(":");
    if (colonFound == std::string::npos || line[0] == '#')
    {
      LOG_WARNING("Not a field line. Skipping... (" << line << ")");
      continue;
    }
    std::string name = line.substr(0, colonFound);
    PlusCommon::Trim(name);

    bool isKeyValue(false);
    size_t equalFound;
    equalFound = line.find_first_of("=");
    if (equalFound != std::string::npos)
    {
      isKeyValue = true;
    }

    if (PlusCommon::IsEqualInsensitive(name, fieldName))
    {
      // found the field that has to be updated, grab the value
      std::string value = line.substr(colonFound + 1);
      if (line[colonFound + 1] == '=')
      {
        // It is a key/value
        value = line.substr(colonFound + 2);
      }
      PlusCommon::Trim(value);

      // construct a new line with the updated value
      std::ostringstream newLineStr;
      newLineStr << name << ":" << (isKeyValue ? "=" : " ") << GetCustomString(name.c_str());

      // need to add padding whitespace characters to fully replace the old line
      int paddingCharactersNeeded = line.length() - newLineStr.str().size();
      if (paddingCharactersNeeded < 0)
      {
        LOG_ERROR("Cannot update line in image header (the new string '" << newLineStr.str() << "' is longer than the current string '" << line << "')");
        return PLUS_FAIL;
      }
      for (int i = 0; i < paddingCharactersNeeded; i++)
      {
        newLineStr << " ";
      }

      // rewind to file pointer the first character of the line
      stream.seekp(-std::ios::off_type(line.size()) - 1, std::ios_base::cur);

      // overwrite the old line
      if (!(stream << newLineStr.str()))
      {
        LOG_ERROR("Cannot update line in image header (writing the updated line into the file failed)");
        return PLUS_FAIL;
      }

      return PLUS_SUCCESS;
    }
  }

  LOG_ERROR("Field " << fieldName << " is not found in the header file, update with new value is failed:");
  return PLUS_FAIL;
}

//----------------------------------------------------------------------------
const char* vtkPlusNrrdSequenceIO::GetDimensionSizeString()
{
  return SEQUENCE_FIELD_SIZES;
}

//----------------------------------------------------------------------------
const char* vtkPlusNrrdSequenceIO::GetDimensionKindsString()
{
  return SEQUENCE_FIELD_KINDS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusNrrdSequenceIO::SetFileName(const std::string& aFilename)
{
  this->FileName.clear();
  this->PixelDataFileName.clear();

  if (aFilename.empty())
  {
    LOG_ERROR("Invalid Nrrd file name");
  }

  this->FileName = aFilename;
  // Trim whitespace and " characters from the beginning and end of the filename
  this->FileName.erase(this->FileName.find_last_not_of(" \"\t\r\n") + 1);
  this->FileName.erase(0, this->FileName.find_first_not_of(" \"\t\r\n"));

  // Set pixel data filename at the same time
  std::string fileExt = vtksys::SystemTools::GetFilenameLastExtension(this->FileName);
  if (STRCASECMP(fileExt.c_str(), ".nrrd") == 0)
  {
    this->PixelDataFileName = std::string("");   //empty string denotes local storage
  }
  else if (STRCASECMP(fileExt.c_str(), ".nhdr") == 0)
  {
    std::string pixFileName = vtksys::SystemTools::GetFilenameWithoutExtension(this->FileName);
    if (this->UseCompression)
    {
      pixFileName += ".raw.gz";
    }
    else
    {
      pixFileName += ".raw";
    }

    this->PixelDataFileName = pixFileName;
  }
  else
  {
    LOG_WARNING("Writing sequence file with '" << fileExt << "' extension is not supported. Using nrrd extension instead.");
    this->FileName += ".nrrd";
    this->PixelDataFileName = std::string("");   //empty string denotes local storage
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusNrrdSequenceIO::UpdateDimensionsCustomStrings(int numberOfFrames, bool isData3D)
{
  if (this->EnableImageDataWrite && this->TrackedFrameList->IsContainingValidImageData())
  {
    this->NumberOfScalarComponents = this->TrackedFrameList->GetNumberOfScalarComponents();
  }

  std::stringstream sizesStr;
  std::stringstream kindStr;

  if (this->NumberOfScalarComponents > 1)
  {
    sizesStr << this->NumberOfScalarComponents << " ";
    kindStr << "vector" << " ";
  }

  int entries = (isData3D ? 3 : 2) + (numberOfFrames > 1 ? 1 : 0);

  this->Dimensions[3] = numberOfFrames;
  // Write out all but the last entry
  for (int i = 0; i < entries - 1; ++i)
  {
    kindStr << "domain" << " ";
    sizesStr << this->Dimensions[i] << " ";
  }

  // pad kind string with spaces then append last entry
  int lastDimension = (numberOfFrames > 1 ? 3 : (isData3D ? 2 : 1));
  std::string lastKind;
  if (numberOfFrames > 1)
  {
    lastKind = "list";
  }
  else
  {
    lastKind = "domain";
  }

  {
    std::stringstream ss;
    ss << this->Dimensions[lastDimension];
    // strlen(SEQUENCE_FIELD_SIZES) + 2 for "sizes: "
    int numchars = sizesStr.str().length() + ss.str().length() + strlen(SEQUENCE_FIELD_SIZES) + 2;
    for (int i = 0; i < SEQUENCE_FIELD_PADDED_LINE_LENGTH - numchars; ++i)
    {
      sizesStr << " ";
    }
    sizesStr << this->Dimensions[lastDimension];
  }

  // strlen(SEQUENCE_FIELD_SIZES) + 2 for "kinds: "
  int numchars = kindStr.str().length() + lastKind.length() + strlen(SEQUENCE_FIELD_KINDS) + 2;
  for (int i = 0; i < SEQUENCE_FIELD_PADDED_LINE_LENGTH - numchars; ++i)
  {
    kindStr << " ";
  }
  kindStr << lastKind;

  this->SetCustomString(SEQUENCE_FIELD_SIZES, sizesStr.str());
  this->SetCustomString(SEQUENCE_FIELD_KINDS, kindStr.str());

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
vtkPlusNrrdSequenceIO::FilePositionOffsetType vtkPlusNrrdSequenceIO::GetFileSize(const std::string& filename)
{
  struct stat stat_buf;
  int rc = stat(filename.c_str(), &stat_buf);
  return rc == 0 ? stat_buf.st_size : -1;
}

//----------------------------------------------------------------------------
vtkPlusNrrdSequenceIO::NrrdEncoding vtkPlusNrrdSequenceIO::StringToNrrdEncoding(const std::string& encoding)
{
  if (PlusCommon::IsEqualInsensitive(encoding, "raw"))
  {
    return NRRD_ENCODING_RAW;
  }
  else if (PlusCommon::IsEqualInsensitive(encoding, "txt"))
  {
    return NRRD_ENCODING_TXT;
  }
  else if (PlusCommon::IsEqualInsensitive(encoding, "text"))
  {
    return NRRD_ENCODING_TEXT;
  }
  else if (PlusCommon::IsEqualInsensitive(encoding, "ascii"))
  {
    return NRRD_ENCODING_ASCII;
  }
  else if (PlusCommon::IsEqualInsensitive(encoding, "hex"))
  {
    return NRRD_ENCODING_HEX;
  }
  else if (PlusCommon::IsEqualInsensitive(encoding, "gz"))
  {
    return NRRD_ENCODING_GZ;
  }
  else if (PlusCommon::IsEqualInsensitive(encoding, "gzip"))
  {
    return NRRD_ENCODING_GZIP;
  }
  else if (PlusCommon::IsEqualInsensitive(encoding, "bz2"))
  {
    return NRRD_ENCODING_BZ2;
  }
  else if (PlusCommon::IsEqualInsensitive(encoding, "bzip2"))
  {
    return NRRD_ENCODING_BZIP2;
  }
  else
  {
    return NRRD_ENCODING_RAW;
  }
}

//----------------------------------------------------------------------------
std::string vtkPlusNrrdSequenceIO::NrrdEncodingToString(NrrdEncoding encoding)
{
  if (encoding == NRRD_ENCODING_RAW)
  {
    return std::string("raw");
  }
  else if (encoding == NRRD_ENCODING_TXT)
  {
    return std::string("txt");
  }
  else if (encoding == NRRD_ENCODING_TEXT)
  {
    return std::string("text");
  }
  else if (encoding == NRRD_ENCODING_ASCII)
  {
    return std::string("ascii");
  }
  else if (encoding == NRRD_ENCODING_HEX)
  {
    return std::string("hex");
  }
  else if (encoding == NRRD_ENCODING_GZ)
  {
    return std::string("gz");
  }
  else if (encoding == NRRD_ENCODING_GZIP)
  {
    return std::string("gzip");
  }
  else if (encoding == NRRD_ENCODING_BZ2)
  {
    return std::string("bz2");
  }
  else if (encoding == NRRD_ENCODING_BZIP2)
  {
    return std::string("bzip2");
  }
  else
  {
    return "raw";
  }
}