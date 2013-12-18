/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

/*=========================================================================
The following copyright notice is applicable to parts of this file:
Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
All rights reserved.
See Copyright.txt or http://www.kitware.com/Copyright.htm for details.
Authors include: Adam Rankin
=========================================================================*/ 

#include "PixelCodec.h"
#include "PlusConfigure.h"
#include "vtkMmfVideoSource.h"
#include "vtkObjectFactory.h"
#include "vtkPlusBuffer.h"
#include "vtkPlusChannel.h"
#include "vtkPlusDataSource.h"

#include "MediaFoundationVideoCaptureApi.h"
#include "FormatReader.h"

// Media foundation includes - require Microsoft Windows SDK 7.1 or later.
// Download from: http://www.microsoft.com/en-us/download/details.aspx?id=8279
#include <Mfapi.h>

// Windows includes
#include <shlwapi.h>

//----------------------------------------------------------------------------

namespace
{
  const unsigned int DEFAULT_DEVICE_ID = 0;
  const int DEFAULT_FRAME_SIZE[2] = {640,480};
  const double DEFAULT_ACQUISITION_RATE=30;
  const std::string DEFAULT_PIXEL_TYPE_NAME="YUY2";
  const GUID DEFAULT_PIXEL_TYPE=MFVideoFormat_YUY2; // see http://msdn.microsoft.com/en-us/library/windows/desktop/aa370819(v=vs.85).aspx
  const std::string MF_VIDEO_FORMAT_PREFIX="MFVideoFormat_";  
  template <class T> void SafeRelease(T **ppT)
  {
    if (*ppT)
    {
      (*ppT)->Release();
      *ppT = NULL;
    }
  }
}

//----------------------------------------------------------------------------

vtkCxxRevisionMacro(vtkMmfVideoSource, "$Revision: 1.0 $");
vtkStandardNewMacro(vtkMmfVideoSource);

//----------------------------------------------------------------------------
vtkMmfVideoSource::vtkMmfVideoSource()
: FrameIndex(0)
, CaptureSource(NULL)
, CaptureSourceReader(NULL)
, Mutex(vtkSmartPointer<vtkRecursiveCriticalSection>::New())
{
  this->RequireAcquisitionRateInDeviceSetConfiguration = false;
  this->RequireAveragedItemsForFilteringInDeviceSetConfiguration = false;
  this->RequireFrameBufferSizeInDeviceSetConfiguration = true;
  this->RequireImageOrientationInConfiguration = true;
  this->RequireLocalTimeOffsetSecInDeviceSetConfiguration = false;
  this->RequireRfElementInDeviceSetConfiguration = false;
  this->RequireUsImageOrientationInDeviceSetConfiguration = true;

  this->AcquisitionRate = vtkPlusDevice::VIRTUAL_DEVICE_FRAME_RATE;

  this->RequestedVideoFormat.DeviceId=DEFAULT_DEVICE_ID;
  this->RequestedVideoFormat.FrameSize[0]=DEFAULT_FRAME_SIZE[0];
  this->RequestedVideoFormat.FrameSize[1]=DEFAULT_FRAME_SIZE[1];
  this->RequestedVideoFormat.PixelFormatName=DEFAULT_PIXEL_TYPE_NAME;

  this->ActiveVideoFormat.DeviceId=DEFAULT_DEVICE_ID;
  this->ActiveVideoFormat.FrameSize[0]=0;
  this->ActiveVideoFormat.FrameSize[1]=0;
}

//----------------------------------------------------------------------------
vtkMmfVideoSource::~vtkMmfVideoSource()
{ 

}


//----------------------------------------------------------------------------
void vtkMmfVideoSource::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);

  os << indent << "FrameIndex: " << (this->FrameIndex ? "On\n" : "Off\n");
}

//----------------------------------------------------------------------------
PlusStatus vtkMmfVideoSource::InternalConnect()
{
  this->ActiveVideoFormat = this->RequestedVideoFormat;

  GUID pixelFormat = DEFAULT_PIXEL_TYPE;
  if (!this->RequestedVideoFormat.PixelFormatName.empty())
  {
    std::string videoFormat = MF_VIDEO_FORMAT_PREFIX + this->RequestedVideoFormat.PixelFormatName;
    std::wstring videoFormatWStr(videoFormat.begin(), videoFormat.end());
    pixelFormat = MfVideoCapture::FormatReader::GUIDFromString(videoFormatWStr);
    if( pixelFormat == GUID_NULL)
    {
      LOG_ERROR("Cannot recognize requested pixel format: "<<this->RequestedVideoFormat.PixelFormatName<<". Defaulting to \'"<<DEFAULT_PIXEL_TYPE_NAME<<"\'.");
      pixelFormat = DEFAULT_PIXEL_TYPE;
    }
  }

  if( !MfVideoCapture::MediaFoundationVideoCaptureApi::GetInstance().SetupDevice(this->RequestedVideoFormat.DeviceId, 
    this->RequestedVideoFormat.FrameSize[0], this->RequestedVideoFormat.FrameSize[1], this->AcquisitionRate, pixelFormat) )
  {    
    LOG_WARNING("Unable to init capture device with requested details:"
      << " device ID: "<<this->RequestedVideoFormat.DeviceId<< " ("<<GetRequestedDeviceName()<<")"
      << ", "<<this->RequestedVideoFormat.FrameSize[0]<<"x"<<this->RequestedVideoFormat.FrameSize[1]
      << ", "<<this->AcquisitionRate<<"Hz, "<<this->ActiveVideoFormat.PixelFormatName);

    LogListOfCaptureVideoFormats(this->RequestedVideoFormat.DeviceId);

    if( !MfVideoCapture::MediaFoundationVideoCaptureApi::GetInstance().SetupDevice(DEFAULT_DEVICE_ID, DEFAULT_FRAME_SIZE[0], DEFAULT_FRAME_SIZE[1], DEFAULT_ACQUISITION_RATE, DEFAULT_PIXEL_TYPE) )
    {
      LOG_ERROR("Unable to initialize capture device with default details: device ID: "<<DEFAULT_DEVICE_ID<<" ("<<GetCaptureDeviceName(DEFAULT_DEVICE_ID)<<"), "<<DEFAULT_FRAME_SIZE[0]<<"x"<<DEFAULT_FRAME_SIZE[1]<<", "<<DEFAULT_ACQUISITION_RATE<<"Hz, "<<DEFAULT_PIXEL_TYPE_NAME);
      LogListOfCaptureDevices();
      LogListOfCaptureVideoFormats(DEFAULT_DEVICE_ID);
      return PLUS_FAIL;
    }
    this->ActiveVideoFormat.DeviceId=DEFAULT_DEVICE_ID;
    this->ActiveVideoFormat.FrameSize[0] = DEFAULT_FRAME_SIZE[0];
    this->ActiveVideoFormat.FrameSize[1] = DEFAULT_FRAME_SIZE[1];
    this->ActiveVideoFormat.PixelFormatName = DEFAULT_PIXEL_TYPE_NAME;

    LOG_INFO("Backing up to connecting with default capture settings:"
      << " device ID: "<<this->ActiveVideoFormat.DeviceId<< " ("<<GetActiveDeviceName()<<")"
      << ", "<<this->ActiveVideoFormat.FrameSize[0]<<"x"<<this->ActiveVideoFormat.FrameSize[1]
      << ", "<<DEFAULT_ACQUISITION_RATE<<"Hz, "<<this->ActiveVideoFormat.PixelFormatName);
  }

  this->CaptureSource = MfVideoCapture::MediaFoundationVideoCaptureApi::GetInstance().GetMediaSource(this->ActiveVideoFormat.DeviceId);

  this->FrameIndex = 0;

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkMmfVideoSource::InternalDisconnect()
{
  SafeRelease(&this->CaptureSourceReader);

  MfVideoCapture::MediaFoundationVideoCaptureApi::GetInstance().CloseAllDevices();

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkMmfVideoSource::InternalStartRecording()
{
  HRESULT hr;
  if( this->CaptureSource != NULL )
  {
    IMFAttributes* attr;
    MFCreateAttributes(&attr, 2);
    hr = attr->SetUnknown(MF_SOURCE_READER_ASYNC_CALLBACK, this);
    hr = attr->SetUINT32(MF_READWRITE_DISABLE_CONVERTERS, TRUE);

    hr = MFCreateSourceReaderFromMediaSource(this->CaptureSource, attr, &this->CaptureSourceReader);

    this->UpdateFrameSize();

    attr->Release();

    this->CaptureSourceReader->ReadSample(MF_SOURCE_READER_FIRST_VIDEO_STREAM, 0, NULL, NULL, NULL, NULL);
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkMmfVideoSource::InternalStopRecording()
{
  if( this->CaptureSource != NULL )
  {
    this->CaptureSource->Stop();
  }

  return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------

PlusStatus vtkMmfVideoSource::NotifyConfigured()
{
  if( this->OutputChannels.size() > 1 )
  {
    LOG_WARNING("vtkMmfVideoSource is expecting one output channel and there are " << this->OutputChannels.size() << " channels. First output channel will be used.");
    return PLUS_FAIL;
  }

  if( this->OutputChannels.size() == 0 )
  {
    LOG_ERROR("No output channels defined for microsoft media foundation video source. Cannot proceed." );
    this->CorrectlyConfigured = false;
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------

STDMETHODIMP vtkMmfVideoSource::QueryInterface( REFIID iid, void** ppv )
{
  static const QITAB qit[] =
  {
    QITABENT(vtkMmfVideoSource, IMFSourceReaderCallback),
    { 0 },
  };
  return QISearch(this, qit, iid, ppv);
}

//----------------------------------------------------------------------------

STDMETHODIMP_(ULONG) vtkMmfVideoSource::AddRef()
{
  return InterlockedIncrement(&RefCount);
}

//----------------------------------------------------------------------------

STDMETHODIMP_(ULONG) vtkMmfVideoSource::Release()
{
  ULONG uCount = InterlockedDecrement(&RefCount);
  if (uCount == 0)
  {
    delete this;
  }
  return uCount;
}

//----------------------------------------------------------------------------

STDMETHODIMP vtkMmfVideoSource::OnEvent( DWORD, IMFMediaEvent * )
{
  return S_OK;
}

//----------------------------------------------------------------------------

STDMETHODIMP vtkMmfVideoSource::OnFlush( DWORD )
{
  return S_OK;
}

//----------------------------------------------------------------------------

STDMETHODIMP vtkMmfVideoSource::OnReadSample( HRESULT hrStatus, DWORD dwStreamIndex, DWORD dwStreamFlags, LONGLONG llTimestamp, IMFSample *pSample )
{
  PlusLockGuard<vtkRecursiveCriticalSection> updateMutexGuardedLock(this->Mutex);

  if (!SUCCEEDED(hrStatus))
  {
    // Streaming error.
    LOG_ERROR("Source Reader error: " << std::hex << hrStatus);
    return S_FALSE;
  }

  if (pSample!=NULL)
  {

    // Get the media type from the stream.
    IMFMediaType* pType=NULL;
    this->CaptureSourceReader->GetCurrentMediaType((DWORD)MF_SOURCE_READER_FIRST_VIDEO_STREAM, &pType);
    if (pType==NULL)
    {
      LOG_ERROR("Cannot get current media type");
    }
    // Check the pixel type, as it may be different from what we requested (even if setup does not give any error).
    // Mostly happens for larger resolutions (e.g., when requesting webcam feed at 1280x720 with YUY then we get MJPG).
    // The check has to be done here, the media type is not yet available at InternalConnect time.
    GUID videoFormat=DEFAULT_PIXEL_TYPE;
    pType->GetGUID( MF_MT_SUBTYPE, &videoFormat );
    std::wstring videoFormatWStr = MfVideoCapture::FormatReader::StringFromGUID(videoFormat);
    std::string videoFormatStr(videoFormatWStr.begin(), videoFormatWStr.end());
    if (videoFormatStr.compare(0, MF_VIDEO_FORMAT_PREFIX.size(), MF_VIDEO_FORMAT_PREFIX)==0)
    {
      // found standard prefix, remove it
      videoFormatStr.erase(0,MF_VIDEO_FORMAT_PREFIX.size());
    }

    if (videoFormatStr.compare(this->ActiveVideoFormat.PixelFormatName)!=0)
    {
      LOG_ERROR("Unexpected video format: "<<videoFormatStr<<" (expected: "<<this->ActiveVideoFormat.PixelFormatName<<")"); 
      return S_FALSE;
    }

    IMFMediaBuffer* aBuffer=NULL;
    DWORD bufferCount=0;
    pSample->GetBufferCount(&bufferCount);
    if( bufferCount < 1 )
    {
      LOG_ERROR("No buffer available in the sample.");
      return S_FALSE;
    }
    pSample->GetBufferByIndex(0, &aBuffer);
    BYTE* bufferData=NULL;
    DWORD maxLength=0;
    DWORD currentLength=0;

    HRESULT hr = aBuffer->Lock(&bufferData, &maxLength, &currentLength);
    if( SUCCEEDED(hr) ) 
    {
      AddFrame(bufferData);
      aBuffer->Unlock();
    }  
    else
    {
      LOG_ERROR("Unable to lock the buffer.");
    }
    SafeRelease(&aBuffer);
  }

  if (MF_SOURCE_READERF_ENDOFSTREAM & dwStreamFlags)
  {
    // Reached the end of the stream.
    LOG_ERROR("End of stream reached. Capture device should never reach end of stream.");
    this->Disconnect();
    return S_FALSE;
    // This should never occur under normal operation.
  }

  if( CaptureSourceReader == NULL )
  {
    return S_FALSE;
  }
  this->CaptureSourceReader->ReadSample(MF_SOURCE_READER_FIRST_VIDEO_STREAM, 0, NULL, NULL, NULL, NULL);

  return S_OK;
}

//----------------------------------------------------------------------------

PlusStatus vtkMmfVideoSource::UpdateFrameSize()
{
  if( this->CaptureSourceReader != NULL )
  {
    int currentFrameSize[2] = {0,0};
    vtkPlusDataSource* videoSource(NULL);
    this->GetFirstActiveVideoSource(videoSource);
    videoSource->GetBuffer()->GetFrameSize(currentFrameSize);
    if( currentFrameSize[0] != this->ActiveVideoFormat.FrameSize[0] || currentFrameSize[1] != this->ActiveVideoFormat.FrameSize[1] )
    {
      currentFrameSize[0] = this->ActiveVideoFormat.FrameSize[0];
      currentFrameSize[1] = this->ActiveVideoFormat.FrameSize[1];
      videoSource->GetBuffer()->SetFrameSize(currentFrameSize);
      PlusCommon::ITKScalarPixelType pixelType = itk::ImageIOBase::UCHAR; // always convert output to 8-bit grayscale
      this->UncompressedVideoFrame.AllocateFrame(currentFrameSize, pixelType);
    }
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------

PlusStatus vtkMmfVideoSource::ReadConfiguration( vtkXMLDataElement* rootXmlElement )
{
  // Read superclass configuration
  Superclass::ReadConfiguration(rootXmlElement); 

  if ( rootXmlElement == NULL ) 
  {
    LOG_WARNING("Unable to find MmfVideoSource XML data element");
    return PLUS_FAIL; 
  }

  vtkXMLDataElement* deviceConfig = this->FindThisDeviceElement(rootXmlElement);
  if (deviceConfig == NULL) 
  {
    LOG_ERROR("Cannot find MmfVideoSource element in XML tree!");
    return PLUS_FAIL;
  }

  int deviceId=0;
  if (deviceConfig->GetScalarAttribute("CaptureDeviceId", deviceId))
  {
    this->RequestedVideoFormat.DeviceId=deviceId;
  }

  int requestedFrameSize[2]={DEFAULT_FRAME_SIZE[0],DEFAULT_FRAME_SIZE[1]};
  if( deviceConfig->GetVectorAttribute("FrameSize", 2, requestedFrameSize) )
  {
    this->RequestedVideoFormat.FrameSize[0] = requestedFrameSize[0];
    this->RequestedVideoFormat.FrameSize[1] = requestedFrameSize[1];
  }  

  this->RequestedVideoFormat.PixelFormatName.clear();
  if( deviceConfig->GetAttribute("VideoFormat") != NULL )
  {
    this->RequestedVideoFormat.PixelFormatName=deviceConfig->GetAttribute("VideoFormat");
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------

PlusStatus vtkMmfVideoSource::WriteConfiguration( vtkXMLDataElement* rootXmlElement )
{
  // Write superclass configuration
  Superclass::WriteConfiguration(rootXmlElement); 

  if ( rootXmlElement == NULL ) 
  {
    LOG_WARNING("Unable to find MmfVideoSource XML data element");
    return PLUS_FAIL; 
  }

  vtkXMLDataElement* deviceConfig = this->FindThisDeviceElement(rootXmlElement);
  if (deviceConfig == NULL) 
  {
    LOG_ERROR("Cannot find MmfVideoSource element in XML tree!");
    return PLUS_FAIL;
  }

  deviceConfig->SetIntAttribute("CaptureDeviceId", this->RequestedVideoFormat.DeviceId);
  deviceConfig->SetVectorAttribute("FrameSize", 2, this->RequestedVideoFormat.FrameSize);
  deviceConfig->SetAttribute("VideoFormat", this->RequestedVideoFormat.PixelFormatName.c_str());


  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
void vtkMmfVideoSource::SetRequestedVideoFormat(const std::string& pixelFormatName)
{
  this->RequestedVideoFormat.PixelFormatName = pixelFormatName;
}

//----------------------------------------------------------------------------
void vtkMmfVideoSource::SetRequestedFrameSize(int frameSize[2])
{
  this->RequestedVideoFormat.FrameSize[0] = frameSize[0];
  this->RequestedVideoFormat.FrameSize[1] = frameSize[1];
}

//----------------------------------------------------------------------------
void vtkMmfVideoSource::SetRequestedDeviceId(unsigned int deviceId)
{
  this->RequestedVideoFormat.DeviceId = deviceId;
}

//----------------------------------------------------------------------------
void vtkMmfVideoSource::GetListOfCaptureDevices(std::vector< std::string > &deviceNames)
{
  deviceNames.clear();
  std::vector< std::wstring > deviceNamesW;
  MfVideoCapture::MediaFoundationVideoCaptureApi::GetInstance().GetDeviceNames(deviceNamesW);
  for (std::vector< std::wstring > :: iterator deviceNameWIt=deviceNamesW.begin(); deviceNameWIt!=deviceNamesW.end(); ++deviceNameWIt)
  {
    std::string deviceName(deviceNameWIt->begin(), deviceNameWIt->end());
    deviceNames.push_back(deviceName);
  }
}

//----------------------------------------------------------------------------
void vtkMmfVideoSource::GetListOfCaptureVideoFormats(std::vector< std::string > &videoModes, unsigned int deviceId)
{
  videoModes.clear();      
  int numberOfVideoFormats = MfVideoCapture::MediaFoundationVideoCaptureApi::GetInstance().GetFormatCount(deviceId);
  for (DWORD i = 0; i < numberOfVideoFormats; i++)
  {
    MfVideoCapture::MediaType type = MfVideoCapture::MediaFoundationVideoCaptureApi::GetInstance().GetFormat(deviceId, i);
    std::string pixelType(type.MF_MT_SUBTYPEName.begin(), type.MF_MT_SUBTYPEName.end());
    if (pixelType.compare(0, MF_VIDEO_FORMAT_PREFIX.size(), MF_VIDEO_FORMAT_PREFIX)==0)
    {
      // found standard prefix, remove it
      pixelType.erase(0,MF_VIDEO_FORMAT_PREFIX.size());
    }
    std::ostringstream strFriendlyName; 
    strFriendlyName << "Frame size: " << type.width << "x" << type.height;
    strFriendlyName << ", video format: "<<pixelType;
    strFriendlyName << ", frame rate: "<<type.MF_MT_FRAME_RATE;
    videoModes.push_back(strFriendlyName.str());
  }
}

//----------------------------------------------------------------------------
std::string vtkMmfVideoSource::GetRequestedDeviceName()
{
  return GetCaptureDeviceName(this->RequestedVideoFormat.DeviceId);
}

//----------------------------------------------------------------------------
std::string vtkMmfVideoSource::GetActiveDeviceName()
{
  return GetCaptureDeviceName(this->ActiveVideoFormat.DeviceId);
}

//----------------------------------------------------------------------------
PlusStatus vtkMmfVideoSource::AddFrame(unsigned char* bufferData)
{
  int frameSize[2]={0,0};
  vtkPlusDataSource* videoSource(NULL);
  if( this->GetFirstActiveVideoSource(videoSource) != PLUS_SUCCESS )
  {
    return PLUS_FAIL;
  }
  videoSource->GetBuffer()->GetFrameSize(frameSize);

  PlusStatus decodingStatus=PLUS_FAIL;
  if (this->ActiveVideoFormat.PixelFormatName.compare("YUY2") == 0)
  {
    decodingStatus=PixelCodec::ConvertToGray(VTK_BI_YUY2, frameSize[0], frameSize[1], bufferData, (unsigned char*)this->UncompressedVideoFrame.GetBufferPointer());
  }
  else if (this->ActiveVideoFormat.PixelFormatName.compare("RGB24") == 0 )
  {
    decodingStatus=PixelCodec::ConvertToGray(BI_RGB, frameSize[0], frameSize[1], bufferData, (unsigned char*)this->UncompressedVideoFrame.GetBufferPointer());
  }
  else
  {
    LOG_ERROR("Unknown pixel type: "<<this->ActiveVideoFormat.PixelFormatName<<" (only YUY2 and RGB24 are supported)");
    decodingStatus=PLUS_FAIL;
  }

  if (decodingStatus != PLUS_SUCCESS)
  {
    LOG_ERROR("Error while decoding the grabbed image");
    return PLUS_FAIL;
  }

  this->FrameIndex++;
  vtkPlusDataSource* aSource(NULL);
  if( this->GetFirstActiveVideoSource(aSource) != PLUS_SUCCESS )
  {
    LOG_ERROR("Unable to retrieve the video source in the media foundation capture device.");
    return PLUS_FAIL;
  }
  PlusStatus status = aSource->GetBuffer()->AddItem(&this->UncompressedVideoFrame, this->FrameIndex); 

  this->Modified();
  return status;
}

//----------------------------------------------------------------------------
void vtkMmfVideoSource::LogListOfCaptureVideoFormats(unsigned int deviceId)
{
  LOG_INFO("Supported vide formats for device "<<deviceId<<" ("<<GetCaptureDeviceName(deviceId)<<")");
  std::vector< std::string > videoModes;
  GetListOfCaptureVideoFormats(videoModes, deviceId);
  for (std::vector< std::string > :: iterator modeIt=videoModes.begin(); modeIt!=videoModes.end(); ++modeIt)
  {
    LOG_INFO("  "<<(*modeIt));
  }
}

//----------------------------------------------------------------------------
void vtkMmfVideoSource::LogListOfCaptureDevices()
{
  LOG_INFO("Found capture devices:");
  std::vector< std::string > deviceNames;
  GetListOfCaptureDevices(deviceNames);
  int id=0;
  for (std::vector< std::string > :: iterator deviceNameIt=deviceNames.begin(); deviceNameIt!=deviceNames.end(); ++deviceNameIt, id++)
  {
    LOG_INFO("  "<<id<<": "<<(*deviceNameIt));
  }
}

//----------------------------------------------------------------------------
std::string vtkMmfVideoSource::GetCaptureDeviceName(unsigned int deviceId)
{
  std::wstring nameW=MfVideoCapture::MediaFoundationVideoCaptureApi::GetInstance().GetCaptureDeviceName(deviceId);
  std::string name(nameW.begin(), nameW.end());
  return name;
}
