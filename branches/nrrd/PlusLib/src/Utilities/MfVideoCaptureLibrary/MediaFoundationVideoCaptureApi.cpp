/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.

This file is subject to the Code Project Open Source License.
See http://www.codeproject.com/info/cpol10.aspx

Original work by Evgeny Pereguda
http://www.codeproject.com/Members/Evgeny-Pereguda

Original "videoInput" library at
http://www.codeproject.com/Articles/559437/Capturing-video-from-web-camera-on-Windows-7-and-8

The "videoInput" library has been adapted to fit within a namespace.

=========================================================Plus=header=end*/

#include "MediaFoundationCaptureLibrary.h"
#include "MediaFoundationVideoCaptureApi.h"
#include "MediaFoundationVideoDevice.h"
#include "MediaFoundationVideoDevices.h"
#include "MfVideoCaptureLoggerMacros.h"

//----------------------------------------------------------------------------
namespace
{
  template <class T> void SafeRelease(T *ppT)
  {
    if (ppT)
    {
      delete (ppT);
      ppT = NULL;
    }
  }
}

//----------------------------------------------------------------------------
#define GET_VIDEO_DEVICE_RETURN_IF_FAILED(videoDevice, deviceId, methodName, returnValueOnFail) \
  if(!this->AccessToDevices) \
  { \
    LOG_ERROR(methodName "failed: devices are not accessible"); \
    return returnValueOnFail; \
  } \
  MediaFoundationVideoDevice *videoDevice = MediaFoundationVideoDevices::GetInstance().GetDevice(deviceID); \
  if (videoDevice==NULL) \
  { \
    LOG_ERROR(methodName "failed: failed to get video device "<<deviceID); \
    return returnValueOnFail; \
  } \
  if(videoDevice==NULL) \
  { \
    LOG_ERROR(methodName "failed: video device is invalid "<<deviceID); \
    return returnValueOnFail; \
  }


//----------------------------------------------------------------------------
namespace MfVideoCapture
{
  //----------------------------------------------------------------------------
  Parameter::Parameter()
  {
    CurrentValue = 0;
    Min = 0;
    Max = 0;
    Step = 0;
    Default = 0; 
    Flag = 0;
  }

  //----------------------------------------------------------------------------
  MediaType::MediaType()
  {
    Clear();
  }

  //----------------------------------------------------------------------------
  MediaType::~MediaType()
  {
  }

  //----------------------------------------------------------------------------
  void MediaType::Clear()
  {
    MF_MT_FRAME_SIZE = 0;
    height = 0;
    width = 0;
    MF_MT_YUV_MATRIX = 0;
    MF_MT_VIDEO_LIGHTING = 0;
    MF_MT_DEFAULT_STRIDE = 0;
    MF_MT_VIDEO_CHROMA_SITING = 0;
    MF_MT_FIXED_SIZE_SAMPLES = 0;
    MF_MT_VIDEO_NOMINAL_RANGE = 0;
    MF_MT_FRAME_RATE = 0;
    MF_MT_FRAME_RATE_low = 0;
    MF_MT_PIXEL_ASPECT_RATIO = 0;
    MF_MT_PIXEL_ASPECT_RATIO_low = 0;
    MF_MT_ALL_SAMPLES_INDEPENDENT = 0;
    MF_MT_FRAME_RATE_RANGE_MIN = 0;
    MF_MT_FRAME_RATE_RANGE_MIN_low = 0;
    MF_MT_SAMPLE_SIZE = 0;
    MF_MT_VIDEO_PRIMARIES = 0;
    MF_MT_INTERLACE_MODE = 0;
    MF_MT_FRAME_RATE_RANGE_MAX = 0;
    MF_MT_FRAME_RATE_RANGE_MAX_low = 0;
    memset(&MF_MT_MAJOR_TYPE, 0, sizeof(GUID));
    memset(&MF_MT_AM_FORMAT_TYPE, 0, sizeof(GUID));
    memset(&MF_MT_SUBTYPE, 0, sizeof(GUID));
  }

  //----------------------------------------------------------------------------
  MediaFoundationVideoCaptureApi::MediaFoundationVideoCaptureApi(void): AccessToDevices(false)
  {
    LOG_DEBUG("MediaFoundationVideoCaptureApi init start");
    UpdateListOfDevices();
    if(!this->AccessToDevices)
    {
      LOG_ERROR("MediaFoundationVideoCaptureApi failed: no compatible video devices found");
    }
  }

  //----------------------------------------------------------------------------
  void MediaFoundationVideoCaptureApi::UpdateListOfDevices()
  {
    this->AccessToDevices = MediaFoundationCaptureLibrary::GetInstance().BuildListOfDevices();
    if(!this->AccessToDevices)
    {
      LOG_ERROR("MediaFoundationVideoCaptureApi::UpdateListOfDevices failed: no compatible video devices found");
    }
  }

  //----------------------------------------------------------------------------
  MediaFoundationVideoCaptureApi::~MediaFoundationVideoCaptureApi(void)
  {
    LOG_DEBUG("MediaFoundationVideoCaptureApi closed");
  }

  //----------------------------------------------------------------------------
  IMFMediaSource *MediaFoundationVideoCaptureApi::GetMediaSource(unsigned int deviceID)
  {
    GET_VIDEO_DEVICE_RETURN_IF_FAILED(videoDevice, deviceId, "MediaFoundationVideoCaptureApi::GetMediaSource", NULL);
    IMFMediaSource *out = videoDevice->GetMediaSource();
    if(out==NULL)
    {
      LOG_ERROR("VideoDevice " << deviceID << ": There is not any suitable IMFMediaSource interface.");
    }
    return out;
  }

  //----------------------------------------------------------------------------
  bool MediaFoundationVideoCaptureApi::SetupDevice(unsigned int deviceID, unsigned int streamIndex, unsigned int formatIndex)
  {
    GET_VIDEO_DEVICE_RETURN_IF_FAILED(videoDevice, deviceId, "MediaFoundationVideoCaptureApi::SetupDevice", NULL);
    bool out = videoDevice->SetupDevice(streamIndex, formatIndex);
    if(!out)
    {
      LOG_ERROR("VideoDevice " << deviceID << ": This device cannot be started.");
    }
    return out;
  }

  //----------------------------------------------------------------------------
  bool MediaFoundationVideoCaptureApi::SetupDevice(unsigned int deviceID, unsigned int streamIndex, unsigned int w, unsigned int h, unsigned int idealFramerate, GUID subtype)
  {
    GET_VIDEO_DEVICE_RETURN_IF_FAILED(videoDevice, deviceId, "MediaFoundationVideoCaptureApi::SetupDevice", NULL);
    bool out = videoDevice->SetupDevice(streamIndex, w, h, idealFramerate, subtype);
    if(!out)
    {
      LOG_ERROR("VideoDevice " << deviceID << ": This device cannot be started.");
    }
    return out;
  }
  
  //----------------------------------------------------------------------------
  MediaType MediaFoundationVideoCaptureApi::GetFormat(unsigned int deviceID, unsigned int streamIndex, unsigned int formatIndex)
  {
    GET_VIDEO_DEVICE_RETURN_IF_FAILED(videoDevice, deviceId, "MediaFoundationVideoCaptureApi::GetFormat", MediaType());
    return videoDevice->GetFormat(streamIndex, formatIndex);
  }

    //----------------------------------------------------------------------------
  int MediaFoundationVideoCaptureApi::GetNumberOfStreams(unsigned int deviceID)
  {
    GET_VIDEO_DEVICE_RETURN_IF_FAILED(videoDevice, deviceId, "MediaFoundationVideoCaptureApi::GetNumberOfStreams", 0);
    return videoDevice->GetNumberOfStreams();
  }

  //----------------------------------------------------------------------------
  bool MediaFoundationVideoCaptureApi::IsDeviceSetup(unsigned int deviceID)
  {
    GET_VIDEO_DEVICE_RETURN_IF_FAILED(videoDevice, deviceId, "MediaFoundationVideoCaptureApi::IsDeviceSetup", false);
    return videoDevice->IsDeviceSetup();
  }

  //----------------------------------------------------------------------------
  bool MediaFoundationVideoCaptureApi::IsDeviceMediaSource(unsigned int deviceID)
  {
    GET_VIDEO_DEVICE_RETURN_IF_FAILED(videoDevice, deviceId, "MediaFoundationVideoCaptureApi::IsDeviceMediaSource", false);
    return videoDevice->IsDeviceMediaSource();
  }

  //----------------------------------------------------------------------------
  bool MediaFoundationVideoCaptureApi::IsDeviceRawDataSource(unsigned int deviceID)
  {
    GET_VIDEO_DEVICE_RETURN_IF_FAILED(videoDevice, deviceId, "MediaFoundationVideoCaptureApi::IsDeviceRawDataSource", false);
    return videoDevice->IsDeviceRawDataSource();
  }

  //----------------------------------------------------------------------------
  unsigned int MediaFoundationVideoCaptureApi::GetNumberOfFormats(unsigned int deviceID, unsigned int streamIndex)
  {
    GET_VIDEO_DEVICE_RETURN_IF_FAILED(videoDevice, deviceId, "MediaFoundationVideoCaptureApi::GetFormatCount", false);
    return videoDevice->GetNumberOfFormats(streamIndex);
  }

  //----------------------------------------------------------------------------
  void MediaFoundationVideoCaptureApi::CloseAllDevices()
  {
    unsigned int numberOfDevices = MediaFoundationVideoDevices::GetInstance().GetCount();
    for(unsigned int i = 0; i < numberOfDevices; i++)
    {
      CloseDevice(i);
    }
  }

  //----------------------------------------------------------------------------
  bool MediaFoundationVideoCaptureApi::SetParameters(unsigned int deviceID, CaptureDeviceParameters parameters)
  {
    GET_VIDEO_DEVICE_RETURN_IF_FAILED(videoDevice, deviceId, "MediaFoundationVideoCaptureApi::SetParameters", false);
    videoDevice->SetParameters(parameters);
    return true;
  }

  //----------------------------------------------------------------------------
  CaptureDeviceParameters MediaFoundationVideoCaptureApi::GetParameters(unsigned int deviceID)
  {
    GET_VIDEO_DEVICE_RETURN_IF_FAILED(videoDevice, deviceId, "MediaFoundationVideoCaptureApi::GetParameters", CaptureDeviceParameters());
    return videoDevice->GetParameters();
  }

  //----------------------------------------------------------------------------
  bool MediaFoundationVideoCaptureApi::CloseDevice(unsigned int deviceID)
  {
    GET_VIDEO_DEVICE_RETURN_IF_FAILED(videoDevice, deviceId, "MediaFoundationVideoCaptureApi::CloseDevice", false);
    videoDevice->CloseDevice();
    return true;
  }

  //----------------------------------------------------------------------------
  unsigned int MediaFoundationVideoCaptureApi::GetWidth(unsigned int deviceID)
  {
    GET_VIDEO_DEVICE_RETURN_IF_FAILED(videoDevice, deviceId, "MediaFoundationVideoCaptureApi::GetWidth", 0);
    return videoDevice->GetWidth();
  }

  //----------------------------------------------------------------------------
  unsigned int MediaFoundationVideoCaptureApi::GetHeight(unsigned int deviceID)
  {
    GET_VIDEO_DEVICE_RETURN_IF_FAILED(videoDevice, deviceId, "MediaFoundationVideoCaptureApi::GetHeight", 0);
    return videoDevice->GetHeight();
  }

  //----------------------------------------------------------------------------
  unsigned int MediaFoundationVideoCaptureApi::GetFrameRate(unsigned int deviceID)
  {
    GET_VIDEO_DEVICE_RETURN_IF_FAILED(videoDevice, deviceId, "MediaFoundationVideoCaptureApi::GetFrameRate", 0);
    return videoDevice->GetFrameRate();
  }

  //----------------------------------------------------------------------------
  wchar_t *MediaFoundationVideoCaptureApi::GetCaptureDeviceName(unsigned int deviceID)
  {
    GET_VIDEO_DEVICE_RETURN_IF_FAILED(videoDevice, deviceId, "MediaFoundationVideoCaptureApi::GetCaptureDeviceName", L"Empty");
    return videoDevice->GetName();
  }

  //----------------------------------------------------------------------------
  unsigned int MediaFoundationVideoCaptureApi::ListDevices()
  {
    if (!this->AccessToDevices)
    {
      LOG_ERROR("MediaFoundationVideoCaptureApi::ListDevices failed: no devices found");
      return 0;
    }
    unsigned int numberOfDevices = MediaFoundationVideoDevices::GetInstance().GetCount();
    for(unsigned int i = 0; i < numberOfDevices; i++)
    {
      LOG_INFO("Device " << i << ": " << GetCaptureDeviceName(i));
    }
    return numberOfDevices;
  }

  //----------------------------------------------------------------------------
  void MediaFoundationVideoCaptureApi::GetDeviceNames(std::vector< std::wstring > &deviceNames)
  {
    deviceNames.clear();
    if (!this->AccessToDevices)
    {
      LOG_ERROR("MediaFoundationVideoCaptureApi::GetDeviceNames failed: no devices found");
      return;
    }
    unsigned int numberOfDevices = MediaFoundationVideoDevices::GetInstance().GetCount();
    for(unsigned int i = 0; i < numberOfDevices; i++)
    {
      std::wstring deviceName=GetCaptureDeviceName(i);
      deviceNames.push_back(deviceName);
    }
  }

  //----------------------------------------------------------------------------
  MediaFoundationVideoCaptureApi& MediaFoundationVideoCaptureApi::GetInstance() 
  {
    static MediaFoundationVideoCaptureApi instance;
    return instance;
  }

  //----------------------------------------------------------------------------
  bool MediaFoundationVideoCaptureApi::AreDevicesAccessible() const
  {
    return AccessToDevices;
  }

  //----------------------------------------------------------------------------
  bool MediaFoundationVideoCaptureApi::SetEmergencyStopEvent(unsigned int deviceID, void *userData, void(*func)(int, void *))
  {
    GET_VIDEO_DEVICE_RETURN_IF_FAILED(videoDevice, deviceId, "MediaFoundationVideoCaptureApi::SetEmergencyStopEvent", false);
    videoDevice->SetEmergencyStopEvent(userData, func);
    return true;
  }

  //----------------------------------------------------------------------------
  unsigned int MediaFoundationVideoCaptureApi::GetDeviceActiveFormat( unsigned int deviceID )
  {
    GET_VIDEO_DEVICE_RETURN_IF_FAILED(videoDevice, deviceId, "MediaFoundationVideoCaptureApi::GetDeviceActiveFormat", -1);
    return videoDevice->GetActiveType();
  }

  //----------------------------------------------------------------------------
  bool MediaFoundationVideoCaptureApi::StartRecording( unsigned int deviceID )
  {
    GET_VIDEO_DEVICE_RETURN_IF_FAILED(videoDevice, deviceId, "MediaFoundationVideoCaptureApi::StartRecording", false);
    return videoDevice->Start();
  }

  //----------------------------------------------------------------------------
  bool MediaFoundationVideoCaptureApi::StopRecording( unsigned int deviceID )
  {
    GET_VIDEO_DEVICE_RETURN_IF_FAILED(videoDevice, deviceId, "MediaFoundationVideoCaptureApi::StopRecording", false);
    return videoDevice->Stop();
  }

}