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
Authors include: Adam Rankin, Francisco Marcano (University of La Laguna)
=========================================================================*/

// Local includes
#include "PlusConfigure.h"
#include "PixelCodec.h"
#include "vtkPlusChannel.h"
#include "vtkPlusDataSource.h"
#include "vtkPlusDAQMMFCombinedVideo.h"

// VTK includes
#include <vtkObjectFactory.h>

// VideoCapture API includes
#include <MediaFoundationVideoCaptureApi.h>
#include <FormatReader.h>

// Media foundation includes - require Microsoft Windows SDK 7.1 or later.
// Download from: http://www.microsoft.com/en-us/download/details.aspx?id=8279
#include <Mfapi.h>
#include <Mferror.h>
#include <Mfidl.h>
#include <Mfreadwrite.h>

// Windows includes
#include <lmerr.h>
#include <shlwapi.h>
#include <tchar.h>
#include <windows.h>

// DAQ System
#include "usb3_frm13_import.h"

//----------------------------------------------------------------------------

namespace
{
  const unsigned int DEFAULT_DEVICE_ID = 0;
  const FrameSizeType DEFAULT_FRAME_SIZE = { 640, 480, 1 };
  const double DEFAULT_ACQUISITION_RATE = 30;
  const std::wstring DEFAULT_PIXEL_TYPE_NAME = L"YUY2";
  const GUID DEFAULT_PIXEL_TYPE = MFVideoFormat_YUY2; // see http://msdn.microsoft.com/en-us/library/windows/desktop/aa370819(v=vs.85).aspx
  const std::wstring MF_VIDEO_FORMAT_PREFIX = L"MFVideoFormat_";
  const int ERRMSGBUFFERSIZE = 1024;
  template <class T> void SafeRelease(T** ppT)
  {
    if (*ppT)
    {
      (*ppT)->Release();
      *ppT = NULL;
    }
  }
}

//----------------------------------------------------------------------------
class DAQMmfVideoSourceReader : public IMFSourceReaderCallback
{
public:

  DAQMmfVideoSourceReader(vtkPlusDAQMMFCombinedVideo* plusDevice)
    : CaptureSource(NULL)
    , CaptureSourceReader(NULL)
    , RefCount(0)
    , PlusDevice(plusDevice)
  {
  };

  //------- IMFSourceReaderCallback functions ----------------------
  STDMETHODIMP QueryInterface(REFIID iid, void** ppv);
  STDMETHOD_(ULONG, AddRef)();
  STDMETHOD_(ULONG, Release)();
  STDMETHODIMP OnEvent(DWORD, IMFMediaEvent*);
  STDMETHODIMP OnFlush(DWORD);
  STDMETHODIMP OnReadSample(HRESULT hrStatus, DWORD dwStreamIndex, DWORD dwStreamFlags, LONGLONG llTimestamp, IMFSample* pSample);

  IMFMediaSource* CaptureSource;
  IMFSourceReader* CaptureSourceReader;

  long RefCount;
  vtkPlusDAQMMFCombinedVideo* PlusDevice;
};

//----------------------------------------------------------------------------
STDMETHODIMP DAQMmfVideoSourceReader::QueryInterface(REFIID iid, void** ppv)
{
  static const QITAB qit[] =
  {
    QITABENT(DAQMmfVideoSourceReader, IMFSourceReaderCallback),
    { 0 },
  };
  return QISearch(this, qit, iid, ppv);
}

//----------------------------------------------------------------------------
STDMETHODIMP_(ULONG) DAQMmfVideoSourceReader::AddRef()
{
  LONG uCount = InterlockedIncrement(&RefCount);
  if (uCount == 1)
  {
    this->PlusDevice->Register(NULL);
  }
  return uCount;
}

//----------------------------------------------------------------------------
STDMETHODIMP_(ULONG) DAQMmfVideoSourceReader::Release()
{
  ULONG uCount = InterlockedDecrement(&RefCount);
  if (uCount == 0)
  {
    LOG_DEBUG("vtkPlusDAQMMFCombinedVideo::Release - unregister");
    this->PlusDevice->UnRegister(NULL);
  }
  return uCount;
}

//----------------------------------------------------------------------------
STDMETHODIMP DAQMmfVideoSourceReader::OnEvent(DWORD, IMFMediaEvent*)
{
  return S_OK;
}

//----------------------------------------------------------------------------
STDMETHODIMP DAQMmfVideoSourceReader::OnFlush(DWORD)
{
  return S_OK;
}

//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
STDMETHODIMP DAQMmfVideoSourceReader::OnReadSample(HRESULT hrStatus, DWORD dwStreamIndex, DWORD dwStreamFlags, LONGLONG llTimestamp, IMFSample* pSample)
{
  igsioLockGuard<vtkIGSIORecursiveCriticalSection> updateMutexGuardedLock(this->PlusDevice->Mutex);

  if (!SUCCEEDED(hrStatus))
  {
    // Streaming error
    LOG_ERROR("Source Reader error: " << std::hex << hrStatus);
    return S_FALSE;
  }

  if (!this->PlusDevice->IsRecording())
  {
    return S_OK;
  }

  if (this->CaptureSourceReader == NULL)
  {
    return S_FALSE;
  }

  if (pSample != NULL)
  {

    // Get the media type from the stream.
    IMFMediaType* pType = NULL;
    this->CaptureSourceReader->GetCurrentMediaType(this->PlusDevice->ActiveVideoFormat.StreamIndex, &pType);
    if (pType == NULL)
    {
      LOG_ERROR("Cannot get current media type");
    }

    // Check the image size, as it may be different from what we requested (even if setup does not give any error).
    // Mostly happens when the native resolution has a different aspect ration (e.g., 640x480 is requested but actually 640x360 is received).
    // The check has to be done here, the media type is not yet available at InternalConnect time.
    UINT32 actualWidth = 0;
    UINT32 actualHeight = 0;
    ::MFGetAttributeSize(pType, MF_MT_FRAME_SIZE, &actualWidth, &actualHeight);
    if (actualWidth != this->PlusDevice->ActiveVideoFormat.FrameSize[0] ||
      actualHeight != this->PlusDevice->ActiveVideoFormat.FrameSize[1])
    {
      LOG_ERROR("Unexpected frame size: " << actualWidth << "x" << actualHeight << " (expected: " << this->PlusDevice->ActiveVideoFormat.FrameSize[0] << "x" << this->PlusDevice->ActiveVideoFormat.FrameSize[1] << ")");
      return S_FALSE;
    }

    // Check the pixel type, as it may be different from what we requested (even if setup does not give any error).
    // Mostly happens for larger resolutions (e.g., when requesting webcam feed at 1280x720 with YUY then we get MJPG).
    // The check has to be done here, the media type is not yet available at InternalConnect time.
    GUID videoFormat = DEFAULT_PIXEL_TYPE;
    pType->GetGUID(MF_MT_SUBTYPE, &videoFormat);
    std::wstring videoFormatWStr = MfVideoCapture::FormatReader::StringFromGUID(videoFormat);
    if (igsioCommon::HasSubstrInsensitive(videoFormatWStr, MF_VIDEO_FORMAT_PREFIX))
    {
      // found standard prefix, remove it
      videoFormatWStr.erase(0, MF_VIDEO_FORMAT_PREFIX.size());
    }

    if (!igsioCommon::IsEqualInsensitive(videoFormatWStr, this->PlusDevice->ActiveVideoFormat.PixelFormatName))
    {
      LOG_ERROR_W("Unexpected video format: " << videoFormatWStr << " (expected: " << this->PlusDevice->ActiveVideoFormat.PixelFormatName << ")");
      return S_FALSE;
    }

    IMFMediaBuffer* aBuffer = NULL;
    DWORD bufferCount = 0;
    pSample->GetBufferCount(&bufferCount);
    if (bufferCount < 1)
    {
      LOG_ERROR("No buffer available in the sample.");
      return S_FALSE;
    }

    // We acquire DAQ Data here almost at the same time of MMF data. This frame will be processed later in DAQ_ProcessFrame
    this->PlusDevice->DAQ_GetFrame();

    pSample->GetBufferByIndex(0, &aBuffer);
    BYTE* bufferData = NULL;
    DWORD maxLength = 0;
    DWORD currentLength = 0;
    HRESULT hr = aBuffer->Lock(&bufferData, &maxLength, &currentLength);
    if (SUCCEEDED(hr))
    {
      this->PlusDevice->AddFrame(bufferData, currentLength);
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
    this->PlusDevice->Disconnect();
    return S_FALSE;
    // This should never occur under normal operation.
  }

  this->CaptureSourceReader->ReadSample(this->PlusDevice->ActiveVideoFormat.StreamIndex, 0, NULL, NULL, NULL, NULL);

  return S_OK;
}

//----------------------------------------------------------------------------

vtkStandardNewMacro(vtkPlusDAQMMFCombinedVideo);

//----------------------------------------------------------------------------
vtkPlusDAQMMFCombinedVideo::vtkPlusDAQMMFCombinedVideo()
  : FrameIndex(0)
  , Mutex(vtkSmartPointer<vtkIGSIORecursiveCriticalSection>::New())
{
  this->MmfSourceReader = new DAQMmfVideoSourceReader(this);
  this->RequireImageOrientationInConfiguration = true;

  this->AcquisitionRate = DEFAULT_ACQUISITION_RATE;

  this->RequestedVideoFormat.DeviceId = DEFAULT_DEVICE_ID;
  this->RequestedVideoFormat.StreamIndex = 0;
  this->RequestedVideoFormat.FrameSize[0] = DEFAULT_FRAME_SIZE[0];
  this->RequestedVideoFormat.FrameSize[1] = DEFAULT_FRAME_SIZE[1];
  this->RequestedVideoFormat.FrameSize[2] = DEFAULT_FRAME_SIZE[2];
  this->RequestedVideoFormat.PixelFormatName = DEFAULT_PIXEL_TYPE_NAME;

  this->ActiveVideoFormat.DeviceId = DEFAULT_DEVICE_ID;
  this->ActiveVideoFormat.FrameSize[0] = 0;
  this->ActiveVideoFormat.FrameSize[1] = 0;
  this->ActiveVideoFormat.FrameSize[2] = 0;
}

//----------------------------------------------------------------------------
vtkPlusDAQMMFCombinedVideo::~vtkPlusDAQMMFCombinedVideo()
{
  delete this->MmfSourceReader;
  this->MmfSourceReader = NULL;
}

//----------------------------------------------------------------------------
void vtkPlusDAQMMFCombinedVideo::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);

  os << indent << "FrameIndex: " << (this->FrameIndex ? "On\n" : "Off\n");
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusDAQMMFCombinedVideo::DAQ_InternalConnect()
{
  this->m_deviceRunning = false;

  if (!OpenDAQDevice())
  {
    LOG_ERROR("DAQVideoSource CameraLink: Failed to open.");
    return PLUS_FAIL;
  }

  this->cameraMode = CAMERAMODE_SCAN;
  switch (this->m_dataMode) {
  case DATAMODE_8:
    this->m_nbytesMode = 2;
    break;
  default:
    this->m_nbytesMode = 4;
  }

  if (!LVDS_Init())
  {
    LOG_ERROR("DAQVideoSource CameraLink: Failed to init.");
    return PLUS_FAIL;
  }
  LVDS_SetDataMode(this->m_dataMode);
  LVDS_CameraMode(this->cameraMode);
  LVDS_SetDeUse(true);
  LVDS_GetResolution(&(this->m_width), &(this->m_height));
  this->m_nwidth = this->m_width / 2;
  this->m_nheight = this->m_height;
  this->m_maxBuffSize = this->m_width * this->m_height;
  this->pImgBuf = new CAMERADATATYPE_t[this->m_maxBuffSize];
  this->m_dwCharCount = this->m_nbytesMode * this->m_maxBuffSize;
  this->pImgBufAux = new unsigned char[this->m_dwCharCount];

  if (!LVDS_Start())
  {
    LOG_ERROR("DAQVideoSource CameraLink: Failed to start");
    return PLUS_FAIL;
  }
  this->m_deviceRunning = true;

  return PLUS_SUCCESS;
}

PlusStatus vtkPlusDAQMMFCombinedVideo::InternalConnect()
{
  igsioLockGuard<vtkIGSIORecursiveCriticalSection> updateMutexGuardedLock(this->Mutex);

  if (this->MmfSourceReader->RefCount != 0)
  {
    LOG_WARNING("There is a reference to this class from a previous connection");
  }

  this->ActiveVideoFormat = this->RequestedVideoFormat;

  GUID pixelFormat = DEFAULT_PIXEL_TYPE;
  if (!this->RequestedVideoFormat.PixelFormatName.empty())
  {
    std::wstring videoFormat = MF_VIDEO_FORMAT_PREFIX + this->RequestedVideoFormat.PixelFormatName;
    pixelFormat = MfVideoCapture::FormatReader::GUIDFromString(videoFormat);
    if (pixelFormat == GUID_NULL)
    {
      LOG_ERROR_W("Cannot recognize requested pixel format: " << this->RequestedVideoFormat.PixelFormatName << ". Defaulting to \'" << DEFAULT_PIXEL_TYPE_NAME << "\'.");
      pixelFormat = DEFAULT_PIXEL_TYPE;
    }
  }

  if (!MfVideoCapture::MediaFoundationVideoCaptureApi::GetInstance().SetupDevice(this->RequestedVideoFormat.DeviceId, this->RequestedVideoFormat.StreamIndex,
    this->RequestedVideoFormat.FrameSize[0], this->RequestedVideoFormat.FrameSize[1], this->AcquisitionRate, pixelFormat))
  {
    LOG_WARNING_W("Unable to init capture device with requested details:"
      << " device ID: " << this->RequestedVideoFormat.DeviceId << " (" << GetRequestedDeviceName() << ") stream " << this->RequestedVideoFormat.StreamIndex
      << ", " << this->RequestedVideoFormat.FrameSize[0] << "x" << this->RequestedVideoFormat.FrameSize[1]
      << ", " << this->AcquisitionRate << "Hz, " << this->ActiveVideoFormat.PixelFormatName);

    LogListOfCaptureVideoFormats(this->RequestedVideoFormat.DeviceId);

    if (!MfVideoCapture::MediaFoundationVideoCaptureApi::GetInstance().SetupDevice(DEFAULT_DEVICE_ID, 0, DEFAULT_FRAME_SIZE[0], DEFAULT_FRAME_SIZE[1], DEFAULT_ACQUISITION_RATE, DEFAULT_PIXEL_TYPE))
    {
      LOG_ERROR_W("Unable to initialize capture device with default details: device ID: " << DEFAULT_DEVICE_ID << " (" << GetCaptureDeviceName(DEFAULT_DEVICE_ID) << ") stream 0, " << DEFAULT_FRAME_SIZE[0] << "x" << DEFAULT_FRAME_SIZE[1] << ", " << DEFAULT_ACQUISITION_RATE << "Hz, " << DEFAULT_PIXEL_TYPE_NAME);
      LogListOfCaptureDevices();
      LogListOfCaptureVideoFormats(DEFAULT_DEVICE_ID);
      return PLUS_FAIL;
    }
    this->ActiveVideoFormat.DeviceId = DEFAULT_DEVICE_ID;
    this->ActiveVideoFormat.FrameSize[0] = DEFAULT_FRAME_SIZE[0];
    this->ActiveVideoFormat.FrameSize[1] = DEFAULT_FRAME_SIZE[1];
    this->ActiveVideoFormat.FrameSize[2] = DEFAULT_FRAME_SIZE[2];
    this->ActiveVideoFormat.PixelFormatName = DEFAULT_PIXEL_TYPE_NAME;

    LOG_INFO_W("Backing up to connecting with default capture settings:"
      << " device ID: " << this->ActiveVideoFormat.DeviceId << " (" << GetActiveDeviceName() << ")"
      << ", " << this->ActiveVideoFormat.FrameSize[0] << "x" << this->ActiveVideoFormat.FrameSize[1]
      << ", " << DEFAULT_ACQUISITION_RATE << "Hz, " << this->ActiveVideoFormat.PixelFormatName);
  }

  this->MmfSourceReader->CaptureSource = MfVideoCapture::MediaFoundationVideoCaptureApi::GetInstance().GetMediaSource(this->ActiveVideoFormat.DeviceId);
  if (this->MmfSourceReader->CaptureSource == NULL)
  {
    LOG_ERROR("Unable to request capture source from the media foundation library.");
    return PLUS_FAIL;
  }

  unsigned int frameRate = MfVideoCapture::MediaFoundationVideoCaptureApi::GetInstance().GetFrameRate(this->ActiveVideoFormat.DeviceId);
  LOG_DEBUG_W("vtkPlusDAQMMFCombinedVideo connected to device '" << GetActiveDeviceName() << "' at frame rate of " << frameRate << "Hz");

  this->FrameIndex = 0;

  return(this->DAQ_InternalConnect());
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusDAQMMFCombinedVideo::DAQ_InternalDisconnect()
{
  LVDS_Stop();
  CloseDAQDevice();
  this->pImgBuf = nullptr;
  return PLUS_SUCCESS;
}

PlusStatus vtkPlusDAQMMFCombinedVideo::InternalDisconnect()
{
  igsioLockGuard<vtkIGSIORecursiveCriticalSection> updateMutexGuardedLock(this->Mutex);

  MfVideoCapture::MediaFoundationVideoCaptureApi::GetInstance().CloseDevice(this->ActiveVideoFormat.DeviceId);
  this->MmfSourceReader->CaptureSource = NULL;

  return (this->InternalDisconnect());
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusDAQMMFCombinedVideo::InternalStartRecording()
{
  igsioLockGuard<vtkIGSIORecursiveCriticalSection> updateMutexGuardedLock(this->Mutex);

  HRESULT hr;
  if (this->MmfSourceReader->CaptureSource != NULL)
  {
    IMFAttributes* attr;
    MFCreateAttributes(&attr, 2);
    hr = attr->SetUnknown(MF_SOURCE_READER_ASYNC_CALLBACK, this->MmfSourceReader);
    hr = attr->SetUINT32(MF_READWRITE_DISABLE_CONVERTERS, TRUE);

    hr = MFCreateSourceReaderFromMediaSource(this->MmfSourceReader->CaptureSource, attr, &this->MmfSourceReader->CaptureSourceReader);

    if (FAILED(hr))
    {
      LOG_ERROR("Unable to create source reader from media source.");
      return PLUS_FAIL;
    }
    this->UpdateFrameSize();

    attr->Release();

    std::wstring videoFormat = MF_VIDEO_FORMAT_PREFIX + this->RequestedVideoFormat.PixelFormatName;

    IMFMediaType* pDecodeType = NULL;
    hr = MFCreateMediaType(&pDecodeType);
    hr = pDecodeType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video);
    hr = pDecodeType->SetGUID(MF_MT_SUBTYPE, MfVideoCapture::FormatReader::GUIDFromString(videoFormat));

    hr = this->MmfSourceReader->CaptureSourceReader->SetCurrentMediaType(this->ActiveVideoFormat.StreamIndex, NULL, pDecodeType);
    if (FAILED(hr))
    {
      LOG_WARNING_W("Unable to set SourceReader output to requested format: " << this->RequestedVideoFormat.PixelFormatName
        << ". Using device default.");
    }
    SafeRelease(&pDecodeType);

    MfVideoCapture::MediaFoundationVideoCaptureApi::GetInstance().StartRecording(this->ActiveVideoFormat.DeviceId);
    this->MmfSourceReader->CaptureSourceReader->ReadSample(this->ActiveVideoFormat.StreamIndex, 0, NULL, NULL, NULL, NULL);
  }
  else
  {
    LOG_ERROR("Unable to request IMFMediaSource from the media foundation capture library. Unable to continue.");
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusDAQMMFCombinedVideo::InternalStopRecording()
{
  LOG_DEBUG("vtkPlusDAQMMFCombinedVideo::InternalStopRecording");

  igsioLockGuard<vtkIGSIORecursiveCriticalSection> updateMutexGuardedLock(this->Mutex);

  MfVideoCapture::MediaFoundationVideoCaptureApi::GetInstance().StopRecording(this->ActiveVideoFormat.DeviceId);

  this->MmfSourceReader->CaptureSourceReader->Flush(this->ActiveVideoFormat.StreamIndex);
  SafeRelease(&this->MmfSourceReader->CaptureSourceReader);
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusDAQMMFCombinedVideo::NotifyConfigured()
{
  if (this->GetDataSource(this->MMFDataSourceName, this->MMFDataSource) != PLUS_SUCCESS)
  {
    LOG_ERROR("Unable to locate data source for MMF Windows Video SOurce labelled: " << this->MMFDataSourceName);
    return PLUS_FAIL;
  }

  if (this->GetDataSource(this->DAQDataSourceName, this->DAQDataSource) != PLUS_SUCCESS)
  {
    LOG_ERROR("Unable to locate data source for DAQ Cameralink source labelled: " << this->DAQDataSourceName);
    return PLUS_FAIL;
  }
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusDAQMMFCombinedVideo::UpdateFrameSize()
{
  if (this->MmfSourceReader->CaptureSourceReader != NULL)
  {
    vtkPlusDataSource* videoSource(NULL);

    videoSource = this->MMFDataSource;
    FrameSizeType currentFrameSize = videoSource->GetInputFrameSize();
    if (currentFrameSize[0] != this->ActiveVideoFormat.FrameSize[0] || currentFrameSize[1] != this->ActiveVideoFormat.FrameSize[1] || currentFrameSize[2] != 1)
    {
      currentFrameSize[0] = this->ActiveVideoFormat.FrameSize[0];
      currentFrameSize[1] = this->ActiveVideoFormat.FrameSize[1];
      currentFrameSize[2] = this->ActiveVideoFormat.FrameSize[2];
      videoSource->SetInputFrameSize(currentFrameSize);
      videoSource->SetPixelType(VTK_UNSIGNED_CHAR);
      unsigned int numberOfScalarComponents = (videoSource->GetImageType() == US_IMG_RGB_COLOR ? 3 : 1);
      videoSource->SetNumberOfScalarComponents(numberOfScalarComponents);
      this->UncompressedVideoFrame.SetImageType(videoSource->GetImageType());
      this->UncompressedVideoFrame.SetImageOrientation(videoSource->GetInputImageOrientation());
      this->UncompressedVideoFrame.AllocateFrame(currentFrameSize, VTK_UNSIGNED_CHAR, numberOfScalarComponents);
    }
  }

  return PLUS_SUCCESS;
}

PlusStatus vtkPlusDAQMMFCombinedVideo::DAQ_ReadConfiguration(vtkXMLDataElement* rootConfigElement)
{
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_READING(deviceConfig, rootConfigElement);
  this->m_dataMode = DATAMODE_8;
  const char* dataModeString = deviceConfig->GetAttribute("DataMode");
  if (dataModeString)
  {
    switch (std::atoi(dataModeString)) {
    case 8:
      this->m_dataMode = DATAMODE_8;
      break;
    case 16:
      this->m_dataMode = DATAMODE_16;
      break;
    default:
      this->m_dataMode = DATAMODE_8;
    }
  }
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusDAQMMFCombinedVideo::ReadConfiguration(vtkXMLDataElement* rootConfigElement)
{
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_READING(deviceConfig, rootConfigElement);

  XML_READ_STRING_ATTRIBUTE_REQUIRED(MMFDataSourceName, deviceConfig);
  XML_READ_STRING_ATTRIBUTE_REQUIRED(DAQDataSourceName, deviceConfig);

  int deviceId = 0;
  if (deviceConfig->GetScalarAttribute("CaptureDeviceId", deviceId))
  {
    this->RequestedVideoFormat.DeviceId = deviceId;
  }

  int streamIndex = 0;
  if (deviceConfig->GetScalarAttribute("CaptureStreamIndex", streamIndex))
  {
    this->RequestedVideoFormat.StreamIndex = (DWORD)streamIndex;
  }

  int requestedFrameSize[2] = { static_cast<int>(DEFAULT_FRAME_SIZE[0]), static_cast<int>(DEFAULT_FRAME_SIZE[1]) };
  if (deviceConfig->GetVectorAttribute("FrameSize", 2, requestedFrameSize))
  {
    if (requestedFrameSize[0] < 0 || requestedFrameSize[1] < 0)
    {
      LOG_ERROR("Negative frame size defined in config file. Cannot continue.");
      return PLUS_FAIL;
    }
    this->RequestedVideoFormat.FrameSize[0] = static_cast<unsigned int>(requestedFrameSize[0]);
    this->RequestedVideoFormat.FrameSize[1] = static_cast<unsigned int>(requestedFrameSize[1]);
    this->RequestedVideoFormat.FrameSize[2] = 1;
  }

  this->RequestedVideoFormat.PixelFormatName.clear();
  if (deviceConfig->GetAttribute("VideoFormat") != NULL)
  {
    auto attr = std::string(deviceConfig->GetAttribute("VideoFormat"));
    this->RequestedVideoFormat.PixelFormatName = std::wstring(attr.begin(), attr.end());
  }

  this->DAQ_ReadConfiguration(rootConfigElement);

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusDAQMMFCombinedVideo::WriteConfiguration(vtkXMLDataElement* rootConfigElement)
{
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_WRITING(deviceConfig, rootConfigElement);

  deviceConfig->SetIntAttribute("CaptureDeviceId", this->RequestedVideoFormat.DeviceId);
  if (this->RequestedVideoFormat.StreamIndex != 0)
  {
    deviceConfig->SetIntAttribute("CaptureStreamIndex", this->RequestedVideoFormat.StreamIndex);
  }
  else
  {
    XML_REMOVE_ATTRIBUTE("CaptureStreamIndex", deviceConfig);
  }
  int frameSize[2] = { static_cast<int>(this->RequestedVideoFormat.FrameSize[0]), static_cast<int>(this->RequestedVideoFormat.FrameSize[1]) };
  deviceConfig->SetVectorAttribute("FrameSize", 2, frameSize);
  auto attr = std::string(this->RequestedVideoFormat.PixelFormatName.begin(), this->RequestedVideoFormat.PixelFormatName.end());
  deviceConfig->SetAttribute("VideoFormat", attr.c_str());

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
void vtkPlusDAQMMFCombinedVideo::SetRequestedVideoFormat(const std::wstring& pixelFormatName)
{
  this->RequestedVideoFormat.PixelFormatName = pixelFormatName;
}

//----------------------------------------------------------------------------
void vtkPlusDAQMMFCombinedVideo::SetRequestedStreamIndex(unsigned int streamIndex)
{
  this->RequestedVideoFormat.StreamIndex = (DWORD)streamIndex;
}

//----------------------------------------------------------------------------
void vtkPlusDAQMMFCombinedVideo::SetRequestedFrameSize(const FrameSizeType& frameSize)
{
  this->RequestedVideoFormat.FrameSize = frameSize;
}

//----------------------------------------------------------------------------
void vtkPlusDAQMMFCombinedVideo::SetRequestedDeviceId(unsigned int deviceId)
{
  this->RequestedVideoFormat.DeviceId = deviceId;
}

//----------------------------------------------------------------------------
void vtkPlusDAQMMFCombinedVideo::GetListOfCaptureDevices(std::vector<std::wstring>& deviceNames)
{
  MfVideoCapture::MediaFoundationVideoCaptureApi::GetInstance().GetDeviceNames(deviceNames);
}

//----------------------------------------------------------------------------
void vtkPlusDAQMMFCombinedVideo::GetListOfCaptureVideoFormats(std::vector<std::wstring>& videoModes, unsigned int deviceId)
{
  videoModes.clear();
  unsigned int numberOfStreams = MfVideoCapture::MediaFoundationVideoCaptureApi::GetInstance().GetNumberOfStreams(deviceId);
  for (unsigned int streamIndex = 0; streamIndex < numberOfStreams; ++streamIndex)
  {
    unsigned int numberOfVideoFormats = MfVideoCapture::MediaFoundationVideoCaptureApi::GetInstance().GetNumberOfFormats(deviceId, streamIndex);
    for (unsigned int formatIndex = 0; formatIndex < numberOfVideoFormats; formatIndex++)
    {
      MfVideoCapture::MediaType type = MfVideoCapture::MediaFoundationVideoCaptureApi::GetInstance().GetFormat(deviceId, streamIndex, formatIndex);
      std::wstring pixelType(type.MF_MT_SUBTYPEName.begin(), type.MF_MT_SUBTYPEName.end());
      if (igsioCommon::HasSubstrInsensitive(pixelType, MF_VIDEO_FORMAT_PREFIX))
      {
        // found standard prefix, remove it
        pixelType.erase(0, MF_VIDEO_FORMAT_PREFIX.size());
      }
      std::wostringstream strFriendlyName;
      strFriendlyName << "Stream index " << streamIndex;
      strFriendlyName << " - Frame size: " << type.width << "x" << type.height;
      strFriendlyName << ", video format: " << pixelType;
      strFriendlyName << ", frame rate: " << type.MF_MT_FRAME_RATE;
      videoModes.push_back(strFriendlyName.str());
    }
  }
}

//----------------------------------------------------------------------------
std::wstring vtkPlusDAQMMFCombinedVideo::GetRequestedDeviceName()
{
  return GetCaptureDeviceName(this->RequestedVideoFormat.DeviceId);
}

//----------------------------------------------------------------------------
std::wstring vtkPlusDAQMMFCombinedVideo::GetActiveDeviceName()
{
  return GetCaptureDeviceName(this->ActiveVideoFormat.DeviceId);
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusDAQMMFCombinedVideo::DAQ_GetFrame()
{
  this->m_currentTime = vtkIGSIOAccurateTimer::GetSystemTime();
  if (!LVDS_GetFrame(&(this->m_dwCharCount), (unsigned char*)(this->pImgBufAux)))
  {
    LOG_DEBUG("vtkPlusDAQMMFCombinedVideo::InternalUpdate Unable to receive DAQ frame");
  }
  return PLUS_SUCCESS;
}

PlusStatus vtkPlusDAQMMFCombinedVideo::DAQ_ProcessFrame()
{
  std::ostringstream ss;
  int ix, iy;

  if (!this->m_deviceRunning)
  {
    LOG_ERROR("vtkPlusDAQMMFCombinedVideo::InternalUpdate Unable to read DAQ data");
    return PLUS_SUCCESS;
  }

  // DAQ Frame has been moved to OnReadSample to reduce the impact of frame processing between MMF and DAQ

  // Byte assignment to output image buffer
  for (ix = 0, iy = 0; ix < this->m_maxBuffSize; ix += 2, iy += this->m_nbytesMode) {
    ((unsigned char*)this->pImgBuf)[ix + 0] = this->pImgBufAux[iy + 0];
    ((unsigned char*)this->pImgBuf)[ix + 1] = this->pImgBufAux[iy + 1];
  }

  vtkPlusDataSource* aSource(nullptr);

  aSource = this->DAQDataSource;

  if (aSource->GetNumberOfItems() == 0)
  {
    // Init the buffer with the metadata from the first frame
    aSource->SetImageType(US_IMG_BRIGHTNESS);
    aSource->SetPixelType(VTK_UNSIGNED_SHORT);
    aSource->SetNumberOfScalarComponents(1);
    aSource->SetInputFrameSize(this->m_nwidth, this->m_nheight, 1);
  }

  // Add the frame to the stream buffer
  FrameSizeType frameSize = { static_cast<unsigned int>(this->m_nwidth), static_cast<unsigned int>(this->m_nheight), 1 };
  if (aSource->AddItem(this->pImgBuf, aSource->GetInputImageOrientation(), frameSize, VTK_UNSIGNED_SHORT, 1, US_IMG_BRIGHTNESS, 0, this->FrameIndex, this->m_currentTime, this->m_currentTime) == PLUS_FAIL)
  {
    return PLUS_FAIL;
  }
  return PLUS_SUCCESS;
}

PlusStatus vtkPlusDAQMMFCombinedVideo::AddFrame(unsigned char* bufferData, DWORD bufferSize)
{
  if (!this->Recording)
  {
    LOG_ERROR("vtkPlusDAQMMFCombinedVideo::AddFrame skipped, not recording anymore");
    return PLUS_SUCCESS;
  }
  this->FrameIndex++;

  PlusStatus status2 = this->DAQ_ProcessFrame();

  vtkPlusDataSource* videoSource(NULL);

  videoSource = this->MMFDataSource;
  FrameSizeType frameSize = videoSource->GetInputFrameSize();

  PlusStatus decodingStatus(PLUS_SUCCESS);
  PixelCodec::PixelEncoding encoding(PixelCodec::PixelEncoding_ERROR);
  if (igsioCommon::IsEqualInsensitive(this->ActiveVideoFormat.PixelFormatName, L"YUY2"))
  {
    if (bufferSize < frameSize[0] * frameSize[1] * 2)
    {
      LOG_ERROR("Failed to decode pixel data from YUY2 due to buffer size mismatch");
      return PLUS_FAIL;
    }
    encoding = PixelCodec::PixelEncoding_YUY2;
  }
  else if (igsioCommon::IsEqualInsensitive(this->ActiveVideoFormat.PixelFormatName, L"MJPG"))
  {
    encoding = PixelCodec::PixelEncoding_MJPG;
  }
  else if (igsioCommon::IsEqualInsensitive(this->ActiveVideoFormat.PixelFormatName, L"RGB24"))
  {
    if (bufferSize < frameSize[0] * frameSize[1] * 3)
    {
      LOG_ERROR("Failed to decode pixel data from RGB24 due to buffer size mismatch");
      return PLUS_FAIL;
    }
    encoding = PixelCodec::PixelEncoding_BGR24;
  }
  else
  {
    LOG_ERROR_W("Unknown pixel type: " << this->ActiveVideoFormat.PixelFormatName << " (only YUY2, MJPG and RGB24 are supported)");
    return PLUS_FAIL;
  }

  if (videoSource->GetImageType() == US_IMG_RGB_COLOR)
  {
    decodingStatus = PixelCodec::ConvertToBGR24(PixelCodec::ComponentOrder_RGB, encoding, frameSize[0], frameSize[1], bufferData, (unsigned char*)this->UncompressedVideoFrame.GetScalarPointer());
  }
  else
  {
    decodingStatus = PixelCodec::ConvertToGray(encoding, frameSize[0], frameSize[1], bufferData, (unsigned char*)this->UncompressedVideoFrame.GetScalarPointer());
  }

  if (decodingStatus != PLUS_SUCCESS)
  {
    LOG_ERROR("Error while decoding the grabbed image");
    return PLUS_FAIL;
  }

  vtkPlusDataSource* aSource(NULL);

  aSource = this->MMFDataSource;  

  const double maximumFrameTimeVariance = 0.2;  // to make sure we don't drop frames because of slight variance in acquisition rate, we allow up to 20% higher frame rate before we start dropping frames
  double acquisitionRate = this->GetAcquisitionRate();
  double minimumTimeBetweenBetweenRecordedFramesSec = (1.0 - maximumFrameTimeVariance) / this->GetAcquisitionRate();
  double lastFrameTimeSec = -1.0;
  double currentTime = vtkIGSIOAccurateTimer::GetSystemTime();

  StreamBufferItem latestFrame;
  if (aSource->GetNumberOfItems() > 2 && aSource->GetLatestStreamBufferItem(&latestFrame) == ITEM_OK)
  {
    lastFrameTimeSec = latestFrame.GetUnfilteredTimestamp(0.0);
    double secondsSinceLastFrame = currentTime - lastFrameTimeSec;
    if (lastFrameTimeSec > 0 && secondsSinceLastFrame < minimumTimeBetweenBetweenRecordedFramesSec)
    {
      // For some webcams, the requested acquistion rate may not be availiable (not supported, or error in configuration).
      // In this case we can artificially limit frames to the requested acquisition rate by ignoring frames.

      // The required time has not elapsed between frames.
      // Do not need to record this frame.
      return PLUS_SUCCESS;
    }
  }
  PlusStatus status = aSource->AddItem(&this->UncompressedVideoFrame, this->FrameIndex, currentTime);
  
  status = ((status == PLUS_SUCCESS) && (status2 == PLUS_SUCCESS))?PLUS_SUCCESS: PLUS_FAIL;
  
  LOG_DEBUG("FrameIndex;" << this->FrameIndex << "; currentTimeMMF; " << currentTime << "; m_currentTimeDAQ; " << this->m_currentTime);

  this->Modified();
  return status;
}

//----------------------------------------------------------------------------
void vtkPlusDAQMMFCombinedVideo::LogListOfCaptureVideoFormats(unsigned int deviceId)
{
  LOG_INFO_W("Supported video formats for Device Id " << deviceId << " (" << GetCaptureDeviceName(deviceId) << ")");
  std::vector<std::wstring> videoModes;
  GetListOfCaptureVideoFormats(videoModes, deviceId);
  for (auto modeIt = videoModes.begin(); modeIt != videoModes.end(); ++modeIt)
  {
    LOG_INFO_W("  " << (*modeIt));
  }
}

//----------------------------------------------------------------------------
void vtkPlusDAQMMFCombinedVideo::LogListOfCaptureDevices()
{
  LOG_INFO("Found capture devices:");
  std::vector<std::wstring> deviceNames;
  GetListOfCaptureDevices(deviceNames);
  int id = 0;
  for (auto deviceNameIt = deviceNames.begin(); deviceNameIt != deviceNames.end(); ++deviceNameIt, id++)
  {
    LOG_INFO_W("  " << id << ": " << (*deviceNameIt));
  }
}

//----------------------------------------------------------------------------
std::wstring vtkPlusDAQMMFCombinedVideo::GetCaptureDeviceName(unsigned int deviceId)
{
  return MfVideoCapture::MediaFoundationVideoCaptureApi::GetInstance().GetCaptureDeviceName(deviceId);
}