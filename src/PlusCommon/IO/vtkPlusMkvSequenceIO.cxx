/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#include "vtkPlusMkvSequenceIO.h"

// vtkVideoIO includes
#include <vtkMKVReader.h>
#include <vtkMKVWriter.h>
#include <vtkMKVUtil.h>

// OpenIGTLink includes
#include <igtlCodecCommonClasses.h>

// PlusLib includes
#include "PlusTrackedFrame.h"
#include "vtkPlusTrackedFrameList.h"

#ifdef PLUS_USE_OpenIGTLink
#include "igtlCommon.h"
#ifdef OpenIGTLink_ENABLE_VIDEOSTREAMING
#include <igtlI420Encoder.h>
#include <igtlI420Decoder.h>
#ifdef OpenIGTLink_USE_VP9
#include <igtlVP9Encoder.h>
#include <igtlVP9Decoder.h>
#endif //OpenIGTLink_USE_VP9
#include "igtlioVideoConverter.h"
#endif
#endif //PLUS_USE_OpenIGTLink

// VTK includes
#include <vtkImageMapToColors.h>
#include <vtkLookupTable.h>
#include <vtkNew.h>

//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkPlusMkvSequenceIO);

class vtkPlusMkvSequenceIO::vtkInternal
{
public:
  vtkPlusMkvSequenceIO* External;

  double InitialTimestamp;
  std::string EncodingFourCC;
  bool IsGreyScale;
  double InitialTimestampSeconds;
  std::map<std::string, uint64_t> FrameFieldTracks;

  igtl::GenericEncoder::Pointer Encoder;

  vtkSmartPointer<vtkMKVWriter> MKVWriter;
  vtkSmartPointer<vtkMKVReader> MKVReader;

  bool Initialized;

  double FrameRate;
  int VideoTrackNumber;

  std::map<std::string, int> VideoNameToTrackMap;
  vtkSmartPointer<vtkImageMapToColors> GreyScaleToRGBFilter;

  virtual bool FourCCRequiresEncoding(std::string fourCC);
  virtual igtl::GenericEncoder::Pointer GetEncoderFromFourCC(std::string fourCC);
  virtual igtl::GenericDecoder::Pointer GetDecoderFromFourCC(std::string fourCC);

  vtkSmartPointer<vtkUnsignedCharArray> EncodeFrame(vtkImageData* inputFrame, unsigned long &size, int &frameType);

  //---------------------------------------------------------------------------
  vtkInternal(vtkPlusMkvSequenceIO* external)
    : External(external)
    , MKVWriter(vtkSmartPointer<vtkMKVWriter>::New())
    , MKVReader(vtkSmartPointer<vtkMKVReader>::New())
    , Initialized(false)
    , InitialTimestamp(-1)
    , Encoder(NULL)
  {
  };

  //---------------------------------------------------------------------------
  ~vtkInternal()
  {
  }

};

//----------------------------------------------------------------------------
vtkPlusMkvSequenceIO::vtkPlusMkvSequenceIO()
  : Internal(new vtkInternal(this))
{
}

//----------------------------------------------------------------------------
vtkPlusMkvSequenceIO::~vtkPlusMkvSequenceIO()
{
  this->Close();

  delete Internal;
  this->Internal = NULL;
}

//----------------------------------------------------------------------------
void vtkPlusMkvSequenceIO::PrintSelf(ostream& os, vtkIndent indent)
{
  Superclass::PrintSelf(os, indent);
}

//----------------------------------------------------------------------------
void vtkPlusMkvSequenceIO::SetEncodingFourCC(std::string encodingFourCC)
{
  this->Internal->EncodingFourCC = encodingFourCC;
}

//----------------------------------------------------------------------------
std::string vtkPlusMkvSequenceIO::GetEncodingFourCC()
{
  return this->Internal->EncodingFourCC;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusMkvSequenceIO::ReadImageHeader()
{
  this->Internal->MKVReader->SetFilename(this->FileName);
  if (!this->Internal->MKVReader->ReadHeader())
  {
    LOG_ERROR("Could not read mkv header!")
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
// Read the spacing and dimensions of the image.
PlusStatus vtkPlusMkvSequenceIO::ReadImagePixels()
{
  if (!this->Internal->MKVReader->ReadContents())
  {
    LOG_ERROR("Could not read mkv contents!");
    return PLUS_FAIL;
  }

  int totalFrameNumber = 0;
  vtkMKVUtil::VideoTrackMap videoTracks = this->Internal->MKVReader->GetVideoTracks();
  for (vtkMKVUtil::VideoTrackMap::iterator videoTrackIt = videoTracks.begin(); videoTrackIt != videoTracks.end(); ++videoTrackIt)
  {
    std::string encodingFourCC = videoTrackIt->second.Encoding;
    bool requiresDecoding = this->Internal->FourCCRequiresEncoding(encodingFourCC);

    igtl::GenericDecoder::Pointer decoder;
    if (requiresDecoding)
    {
      decoder = this->Internal->GetDecoderFromFourCC(encodingFourCC);
    }

    if (!decoder && requiresDecoding)
    {
      LOG_ERROR("Could not find decoder")
      continue;
    }

    vtkNew<vtkImageData> yuvImage;
    yuvImage->SetDimensions(videoTrackIt->second.Width, videoTrackIt->second.Height * 3 / 2, 1);
    yuvImage->AllocateScalars(VTK_UNSIGNED_CHAR, 1); // UNLESS GREYSCALE

    FrameSizeType frameSize = { videoTrackIt->second.Width, videoTrackIt->second.Height, 1 };
    int frameNumber = 0;
    for (vtkMKVUtil::FrameInfoList::iterator frameIt = videoTrackIt->second.Frames.begin(); frameIt != videoTrackIt->second.Frames.end(); ++frameIt)
    {
      this->CreateTrackedFrameIfNonExisting(totalFrameNumber);
      PlusTrackedFrame* trackedFrame = this->TrackedFrameList->GetTrackedFrame(totalFrameNumber);
      trackedFrame->GetImageData()->SetImageOrientation(US_IMG_ORIENT_MF); // TODO: save orientation and type
      trackedFrame->GetImageData()->SetImageType(US_IMG_RGB_COLOR);
      trackedFrame->GetImageData()->AllocateFrame(frameSize, VTK_UNSIGNED_CHAR, 3);
      trackedFrame->SetTimestamp(frameIt->TimestampSeconds);

      if (!requiresDecoding)
      {
        unsigned long size = frameIt->Data->GetSize() * frameIt->Data->GetElementComponentSize();
        memcpy(trackedFrame->GetImageData()->GetImage()->GetScalarPointer(), frameIt->Data->GetPointer(0), size);
      }
      else
      {
        igtl_uint32 dimensions[2] = { frameSize[0], frameSize[1] };
        igtl_uint64 size = frameIt->Data->GetSize() * frameIt->Data->GetElementComponentSize();
        if (!decoder->DecodeBitStreamIntoFrame((unsigned char*)frameIt->Data->GetPointer(0), (igtl_uint8*)yuvImage->GetScalarPointer(), dimensions, size))
        {
          LOG_ERROR("Could not decode frame!");
          continue;
        }
        decoder->ConvertYUVToRGB((igtl_uint8*)yuvImage->GetScalarPointer(), (igtl_uint8*)trackedFrame->GetImageData()->GetImage()->GetScalarPointer(), frameSize[1], frameSize[0]);
      }
      ++frameNumber;
      ++totalFrameNumber;
    }
  }

  vtkMKVUtil::MetadataTrackMap metadataTracks = this->Internal->MKVReader->GetMetadataTracks();
  for (vtkMKVUtil::MetadataTrackMap::iterator metadataTrackIt = metadataTracks.begin(); metadataTrackIt != metadataTracks.end(); ++metadataTrackIt)
  {
    for (vtkMKVUtil::FrameInfoList::iterator frameIt = metadataTrackIt->second.Frames.begin(); frameIt != metadataTrackIt->second.Frames.end(); ++frameIt)
    {
      for (unsigned int i = 0; i < this->TrackedFrameList->GetNumberOfTrackedFrames(); ++i)
      {
        PlusTrackedFrame* trackedFrame = this->TrackedFrameList->GetTrackedFrame(i);
        if (trackedFrame->GetTimestamp() != frameIt->TimestampSeconds)
        {
          continue;
        }
        std::string frameField = (char*)frameIt->Data->GetPointer(0);
        trackedFrame->SetFrameField(metadataTrackIt->second.Name, frameField);
      }
    }
  }
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusMkvSequenceIO::PrepareImageFile()
{
  this->WriteInitialImageHeader();
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
bool vtkPlusMkvSequenceIO::CanReadFile(const std::string& filename)
{
  if (PlusCommon::IsEqualInsensitive(vtksys::SystemTools::GetFilenameLastExtension(filename), ".mkv"))
  {
    return true;
  }

  if (PlusCommon::IsEqualInsensitive(vtksys::SystemTools::GetFilenameLastExtension(filename), ".webm"))
  {
    return true;
  }

  return false;
}

//----------------------------------------------------------------------------
bool vtkPlusMkvSequenceIO::CanWriteFile(const std::string& filename)
{
  if (PlusCommon::IsEqualInsensitive(vtksys::SystemTools::GetFilenameLastExtension(filename), ".mkv"))
  {
    return true;
  }

  return false;
}

//----------------------------------------------------------------------------
/** Assumes SetFileName has been called with a valid file name. */
PlusStatus vtkPlusMkvSequenceIO::WriteInitialImageHeader()
{
  this->Internal->MKVWriter->SetFilename(this->FileName);
  if (this->TrackedFrameList->Size() == 0)
  {
    LOG_ERROR("Could not write MKV header, no tracked frames")
    return PLUS_FAIL;
  }

  this->Internal->MKVWriter->WriteHeader();

  int* dimensions = this->TrackedFrameList->GetTrackedFrame(0)->GetImageData()->GetImage()->GetDimensions();
  this->Dimensions[0] = dimensions[0];
  this->Dimensions[1] = dimensions[1];
  this->Dimensions[2] = dimensions[2];
  this->Internal->VideoTrackNumber = this->Internal->MKVWriter->AddVideoTrack("Video", this->Internal->EncodingFourCC, dimensions[0], dimensions[1]);
  if (this->Internal->VideoTrackNumber < 1)
  {
    LOG_ERROR("Could not create video track!");
    return PLUS_FAIL;
  }

  // All tracks have to be initialized before starting
  // All desired custom frame field names must be availiable starting from frame 0
  for (unsigned int frameNumber = 0; frameNumber < this->TrackedFrameList->GetNumberOfTrackedFrames(); frameNumber++)
  {
    PlusTrackedFrame* trackedFrame(NULL);
    trackedFrame = this->TrackedFrameList->GetTrackedFrame(frameNumber);
    std::vector<std::string> fieldNames;
    trackedFrame->GetFrameFieldNameList(fieldNames);
    for (std::vector<std::string>::iterator it = fieldNames.begin(); it != fieldNames.end(); it++)
    {
      uint64_t trackNumber = this->Internal->FrameFieldTracks[*it];
      if (trackNumber <= 0)
      {
        int metaDataTrackNumber = this->Internal->MKVWriter->AddMetadataTrack((*it));
        if (metaDataTrackNumber > 0)
        {
          this->Internal->FrameFieldTracks[*it] = metaDataTrackNumber;
        }
      }
    }
  }

  if (this->Internal->FourCCRequiresEncoding(this->Internal->EncodingFourCC))
  {
    this->Internal->Encoder = this->Internal->GetEncoderFromFourCC(this->Internal->EncodingFourCC);
    if (!this->Internal->Encoder)
    {
      LOG_ERROR("Could not find encoder for type: " << this->Internal->EncodingFourCC);
      return PLUS_FAIL;
    }

    this->Internal->Encoder->SetLosslessLink(!this->UseCompression);
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusMkvSequenceIO::AppendImagesToHeader()
{
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusMkvSequenceIO::WriteImages()
{
  // Do the actual writing
  if (!this->Internal->Initialized)
  {
    if (this->WriteInitialImageHeader() != PLUS_SUCCESS)
    {
      return PLUS_FAIL;
    }
    this->Internal->Initialized = true;
  }

  bool requiresEncoding = this->Internal->FourCCRequiresEncoding(this->Internal->EncodingFourCC);

  for (unsigned int frameNumber = 0; frameNumber < this->TrackedFrameList->GetNumberOfTrackedFrames(); frameNumber++)
  {
    PlusTrackedFrame* trackedFrame(NULL);

    if (this->EnableImageDataWrite)
    {
      trackedFrame = this->TrackedFrameList->GetTrackedFrame(frameNumber);
      if (trackedFrame == NULL)
      {
        LOG_ERROR("Cannot access frame " << frameNumber << " while trying to writing compress data into file");
        continue;
      }

      if (this->Internal->InitialTimestamp == -1)
      {
        this->Internal->InitialTimestamp = trackedFrame->GetTimestamp();
      }

      double timestamp = trackedFrame->GetTimestamp() - this->Internal->InitialTimestamp;

      if (!requiresEncoding)
      {
        int* dimensions = trackedFrame->GetImageData()->GetImage()->GetDimensions();
        uint64_t size = dimensions[0]*dimensions[1]*dimensions[2]*trackedFrame->GetImageData()->GetImage()->GetScalarSize();
        this->Internal->MKVWriter->WriteEncodedVideoFrame((unsigned char*)trackedFrame->GetImageData()->GetImage()->GetScalarPointer(), size, true, this->Internal->VideoTrackNumber, timestamp);
      }
      else
      {
        unsigned long size = 0;
        int frameType = 0;
        vtkSmartPointer<vtkUnsignedCharArray> encodedFrame = this->Internal->EncodeFrame(trackedFrame->GetImageData()->GetImage(), size, frameType);
        if (!encodedFrame)
        {
          LOG_ERROR("Could not encode frame!");
          return PLUS_FAIL;
        }

        if (!this->Internal->MKVWriter->WriteEncodedVideoFrame(encodedFrame->GetPointer(0), size, frameType == FrameTypeKey, this->Internal->VideoTrackNumber, timestamp))
        {
          LOG_ERROR("Could not write frame to file: " << this->Internal->MKVWriter->GetFilename());
          return PLUS_FAIL;
        }

      }

      std::map<std::string, std::string> customFields = trackedFrame->GetCustomFields();
      for (std::map<std::string, std::string>::iterator customFieldIt = customFields.begin(); customFieldIt != customFields.end(); ++customFieldIt)
      {
        uint64_t trackID = this->Internal->FrameFieldTracks[customFieldIt->first];
        if (trackID == 0)
        {
          vtkErrorMacro("Could not find metadata track for: " << customFieldIt->first);
          continue;
        }

        this->Internal->MKVWriter->WriteMetadata(customFieldIt->second, trackID, trackedFrame->GetTimestamp() - this->Internal->InitialTimestamp);
      }
    }
  }
  return PLUS_SUCCESS;
}

//---------------------------------------------------------------------------
vtkSmartPointer<vtkUnsignedCharArray> vtkPlusMkvSequenceIO::vtkInternal::EncodeFrame(vtkImageData* inputFrame, unsigned long &size, int &frameType)
{
  igtl::VideoMessage::Pointer videoMessage = igtl::VideoMessage::New();
  igtlioVideoConverter::HeaderData headerData = igtlioVideoConverter::HeaderData();
  igtlioVideoConverter::ContentData contentData = igtlioVideoConverter::ContentData();
  contentData.videoMessage = videoMessage;
  contentData.image = inputFrame;

  if (inputFrame->GetNumberOfScalarComponents() != 3)
  {
    // TODO: handle greyscale
    if (!this->GreyScaleToRGBFilter->GetLookupTable())
    {
      vtkSmartPointer<vtkLookupTable> lookupTable = vtkSmartPointer<vtkLookupTable>::New();
      lookupTable->SetNumberOfTableValues(256);

      // Greyscale image
      if (inputFrame->GetNumberOfScalarComponents() == 1)
      {
        lookupTable->SetHueRange(0.0, 0.0);
        lookupTable->SetSaturationRange(0.0, 0.0);
        lookupTable->SetValueRange(0.0, 1.0);
        lookupTable->SetRange(0.0, 255.0);
        lookupTable->Build();
      }
      this->GreyScaleToRGBFilter->SetLookupTable(lookupTable);
    }

    this->GreyScaleToRGBFilter->SetInputData(inputFrame);
    this->GreyScaleToRGBFilter->SetOutputFormatToRGB();
    this->GreyScaleToRGBFilter->Update();
    contentData.image = this->GreyScaleToRGBFilter->GetOutput();
  }

  // TODO: encoding should be done without the use of the video message
  if (!igtlioVideoConverter::toIGTL(headerData, contentData, this->Encoder))
  {
    vtkErrorWithObjectMacro(External, "Could not create video message!");
    return false;
  }

  uint8_t* pointer = videoMessage->GetPackFragmentPointer(2);
  size = videoMessage->GetPackFragmentSize(2);

  //unsigned char* encodedFrame = new unsigned char[size];
  vtkSmartPointer<vtkUnsignedCharArray> encodedFrame = vtkSmartPointer<vtkUnsignedCharArray>::New();
  encodedFrame->Allocate(size);
  memcpy(encodedFrame->GetPointer(0), pointer, size);
  frameType = videoMessage->GetFrameType();

  return encodedFrame;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusMkvSequenceIO::Close()
{
  this->Internal->MKVWriter->Close();
  this->Internal->MKVReader->Close();
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusMkvSequenceIO::WriteCompressedImagePixelsToFile(int& compressedDataSize)
{
  return this->WriteImages();
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusMkvSequenceIO::SetFileName(const std::string& aFilename)
{
  this->FileName = aFilename;
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusMkvSequenceIO::UpdateDimensionsCustomStrings(int numberOfFrames, bool isData3D)
{
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
igtl::GenericEncoder::Pointer vtkPlusMkvSequenceIO::vtkInternal::GetEncoderFromFourCC(std::string fourCC)
{
  if (fourCC == "I420")
  {
    return igtl::I420Encoder::New();
  }
#if defined OpenIGTLink_USE_VP9
  else if (fourCC == VTKVIDEOIO_VP9_FOURCC)
  {
    return igtl::VP9Encoder::New();
  }
#endif
  return NULL;
}

//----------------------------------------------------------------------------
igtl::GenericDecoder::Pointer vtkPlusMkvSequenceIO::vtkInternal::GetDecoderFromFourCC(std::string fourCC)
{
  if (fourCC == "I420")
  {
    return igtl::I420Decoder::New();
  }
#if defined OpenIGTLink_USE_VP9
  else if (fourCC == VTKVIDEOIO_VP9_FOURCC)
  {
    return igtl::VP9Decoder::New();
  }
#endif
  return NULL;
}

//----------------------------------------------------------------------------
bool vtkPlusMkvSequenceIO::vtkInternal::FourCCRequiresEncoding(std::string fourCC)
{
  if (fourCC == "RV24")
  {
    return false;
  }
  return true;
}
