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

#include "PlusCommon.h"
#include "MediaFoundationVideoDevice.h"
#include "MediaFoundationVideoDevices.h"
#include "RawImage.h"
#include "MediaFoundationCaptureLibrary.h"
#include "MediaFoundationVideoCaptureApi.h"

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
    Clear();
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
    LOG_INFO("***** VIDEOINPUT LIBRARY - 2013 (Author: Evgeny Pereguda) *****");

    UpdateListOfDevices();

    if(!AccessToDevices)
      LOG_ERROR("INITIALIZATION: There are not any suitable video devices.");
  }

  //----------------------------------------------------------------------------

  void MediaFoundationVideoCaptureApi::UpdateListOfDevices()
  {
    MediaFoundationCaptureLibrary *MF = &MediaFoundationCaptureLibrary::GetInstance();

    AccessToDevices = MF->BuildListOfDevices();

    if(!AccessToDevices)
    {
      LOG_ERROR("UPDATING: There is not any suitable video device.");
    }
  }

  //----------------------------------------------------------------------------

  MediaFoundationVideoCaptureApi::~MediaFoundationVideoCaptureApi(void)
  {
    LOG_INFO("***** CLOSE VIDEOINPUT LIBRARY - 2013 *****");
  }

  //----------------------------------------------------------------------------

  IMFMediaSource *MediaFoundationVideoCaptureApi::GetMediaSource(unsigned int deviceID)
  {
    if(AccessToDevices)
    {
      MediaFoundationVideoDevices *VDS = &MediaFoundationVideoDevices::GetInstance();
      MediaFoundationVideoDevice * VD = VDS->GetDevice(deviceID);

      if(VD)
      {
        IMFMediaSource *out = VD->GetMediaSource();

        if(!out)
          LOG_ERROR("VideoDevice " << deviceID << ": There is not any suitable IMFMediaSource interface.");

        return out;
      }
    }
    else
    {
      LOG_ERROR("VIDEODEVICE(s): There is not any suitable video device.");
    }

    return NULL;
  }

  //----------------------------------------------------------------------------

  bool MediaFoundationVideoCaptureApi::SetupDevice(unsigned int deviceID, unsigned int id)
  {
    if(AccessToDevices)
    {
      MediaFoundationVideoDevices *VDS = &MediaFoundationVideoDevices::GetInstance();
      MediaFoundationVideoDevice * VD = VDS->GetDevice(deviceID);

      if(VD)
      {
        bool out = VD->SetupDevice(id);

        if(!out)
          LOG_ERROR("VIDEODEVICE " << deviceID << ": This device cannot be started.");

        return out;
      }
    }
    else
    {
      LOG_ERROR("VIDEODEVICE(s): There are not any suitable video devices.");
    }

    return false;
  }

  //----------------------------------------------------------------------------

  bool MediaFoundationVideoCaptureApi::SetupDevice(unsigned int deviceID, unsigned int w, unsigned int h, unsigned int idealFramerate, GUID subtype)
  {
    if(AccessToDevices)
    {
      MediaFoundationVideoDevices *VDS = &MediaFoundationVideoDevices::GetInstance();
      MediaFoundationVideoDevice * VD = VDS->GetDevice(deviceID);

      if(VD)
      {
        bool out = VD->SetupDevice(w, h, idealFramerate, subtype);

        if(!out)
          LOG_ERROR("VIDEODEVICE " << deviceID << ": this device cannot be started.");

        return out;
      }
    }
    else
    {
      LOG_ERROR("VIDEODEVICE(s): There are not any suitable video devices.");
    }

    return false;
  }

  //----------------------------------------------------------------------------

  MediaType MediaFoundationVideoCaptureApi::GetFormat(unsigned int deviceID, unsigned int id)
  {
    if(AccessToDevices)
    {
      MediaFoundationVideoDevices *VDS = &MediaFoundationVideoDevices::GetInstance();
      MediaFoundationVideoDevice * VD = VDS->GetDevice(deviceID);

      if(VD)    
        return VD->GetFormat(id);
    }
    else
    {
      LOG_ERROR("VIDEODEVICE(s): There are not any suitable video devices.");
    }

    return MediaType();
  }

  //----------------------------------------------------------------------------

  bool MediaFoundationVideoCaptureApi::IsDeviceSetup(unsigned int deviceID)
  {
    if(AccessToDevices)
    {
      MediaFoundationVideoDevices *VDS = &MediaFoundationVideoDevices::GetInstance();
      MediaFoundationVideoDevice * VD = VDS->GetDevice(deviceID);

      if(VD)
        return VD->IsDeviceSetup();
    }
    else
    {
      LOG_ERROR("VIDEODEVICE(s): There are not any suitable video devices.");
    }

    return false;
  }

  //----------------------------------------------------------------------------

  bool MediaFoundationVideoCaptureApi::IsDeviceMediaSource(unsigned int deviceID)
  {
    if(AccessToDevices)
    {
      MediaFoundationVideoDevices *VDS = &MediaFoundationVideoDevices::GetInstance();
      MediaFoundationVideoDevice * VD = VDS->GetDevice(deviceID);

      if(VD)
        return VD->IsDeviceMediaSource();
    }
    else
    {
      LOG_ERROR("VIDEODEVICE(s): There are not any suitable video devices.");
    }

    return false;
  }

  //----------------------------------------------------------------------------

  bool MediaFoundationVideoCaptureApi::IsDeviceRawDataSource(unsigned int deviceID)
  {
    if(AccessToDevices)
    {
      MediaFoundationVideoDevices *VDS = &MediaFoundationVideoDevices::GetInstance();
      MediaFoundationVideoDevice * VD = VDS->GetDevice(deviceID);

      if(VD)
        return VD->IsDeviceRawDataSource();
    }
    else
    {
      LOG_ERROR("VIDEODEVICE(s): There are not any suitable video devices.");
    }

    return false;
  }

  //----------------------------------------------------------------------------

  bool MediaFoundationVideoCaptureApi::IsFrameNew(unsigned int deviceID)
  {  
    if(AccessToDevices)
    {
      if(!IsDeviceSetup(deviceID))
      {
        if(IsDeviceMediaSource(deviceID))
          return false;
      }

      MediaFoundationVideoDevices *VDS = &MediaFoundationVideoDevices::GetInstance();
      MediaFoundationVideoDevice * VD = VDS->GetDevice(deviceID);

      if(VD)
        return VD->IsFrameNew();
    }
    else
    {
      LOG_ERROR("VIDEODEVICE(s): There are not any suitable video devices.");
    }

    return false;
  }

  //----------------------------------------------------------------------------

  unsigned int MediaFoundationVideoCaptureApi::GetFormatCount(unsigned int deviceID)
  {
    if(AccessToDevices)
    {
      MediaFoundationVideoDevices *VDS = &MediaFoundationVideoDevices::GetInstance();
      MediaFoundationVideoDevice * VD = VDS->GetDevice(deviceID);

      if(VD)
        return VD->GetCountFormats();
    }
    else
    {
      LOG_ERROR("VIDEODEVICE(s): There are not any suitable video devices.");
    }

    return 0;
  }

  //----------------------------------------------------------------------------

  void MediaFoundationVideoCaptureApi::CloseAllDevices()
  {
    MediaFoundationVideoDevices *VDS = &MediaFoundationVideoDevices::GetInstance();

    for(int i = 0; i < VDS->GetCount(); i++)
    {
      CloseDevice(i);
    }
  }

  //----------------------------------------------------------------------------

  void MediaFoundationVideoCaptureApi::SetParameters(unsigned int deviceID, CaptureDeviceParameters parametrs)
  {
    if(AccessToDevices)
    {
      MediaFoundationVideoDevices *VDS = &MediaFoundationVideoDevices::GetInstance();

      MediaFoundationVideoDevice *VD = VDS->GetDevice(deviceID);

      if(VD)
        VD->SetParameters(parametrs);
    }
    else
    {
      LOG_ERROR("VIDEODEVICE(s): There are not any suitable video devices.");
    }
  }

  //----------------------------------------------------------------------------

  CaptureDeviceParameters MediaFoundationVideoCaptureApi::GetParameters(unsigned int deviceID)
  {
    CaptureDeviceParameters out;

    if(AccessToDevices)
    {
      MediaFoundationVideoDevices *VDS = &MediaFoundationVideoDevices::GetInstance();
      MediaFoundationVideoDevice *VD = VDS->GetDevice(deviceID);

      if(VD)
        out = VD->GetParameters();
    }
    else
    {
      LOG_ERROR("VIDEODEVICE(s): There are not any suitable video devices.");
    }

    return out;
  }

  //----------------------------------------------------------------------------

  void MediaFoundationVideoCaptureApi::CloseDevice(unsigned int deviceID)
  {
    if(AccessToDevices)
    {
      MediaFoundationVideoDevices *VDS = &MediaFoundationVideoDevices::GetInstance();
      MediaFoundationVideoDevice *VD = VDS->GetDevice(deviceID);

      if(VD)
        VD->CloseDevice();
    }
    else
    {
      LOG_ERROR("VIDEODEVICE(s): There are not any suitable video devices.");
    }
  }

  //----------------------------------------------------------------------------

  unsigned int MediaFoundationVideoCaptureApi::GetWidth(unsigned int deviceID)
  {
    if(AccessToDevices)
    {
      MediaFoundationVideoDevices *VDS = &MediaFoundationVideoDevices::GetInstance();
      MediaFoundationVideoDevice * VD = VDS->GetDevice(deviceID);

      if(VD)  
        return VD->GetWidth();
    }
    else
    {
      LOG_ERROR("VIDEODEVICE(s): There are not any suitable video devices.");
    }

    return 0;
  }

  //----------------------------------------------------------------------------

  unsigned int MediaFoundationVideoCaptureApi::GetHeight(unsigned int deviceID)
  {
    if(AccessToDevices)
    {
      MediaFoundationVideoDevices *VDS = &MediaFoundationVideoDevices::GetInstance();
      MediaFoundationVideoDevice * VD = VDS->GetDevice(deviceID);

      if(VD)
        return VD->GetHeight();
    }
    else
    {
      LOG_ERROR("VIDEODEVICE(s): There are not any suitable video devices.");
    }

    return 0;
  }

  //----------------------------------------------------------------------------

  wchar_t *MediaFoundationVideoCaptureApi::GetCaptureDeviceName(unsigned int deviceID)
  {
    if(AccessToDevices)
    {
      MediaFoundationVideoDevices *VDS = &MediaFoundationVideoDevices::GetInstance();
      MediaFoundationVideoDevice * VD = VDS->GetDevice(deviceID);

      if(VD)
        return VD->GetName();
    }
    else
    {
      LOG_ERROR("VIDEODEVICE(s): There are not any suitable video devices.");
    }

    return L"Empty";
  }

  //----------------------------------------------------------------------------

  unsigned int MediaFoundationVideoCaptureApi::ListDevices()
  {
    int out = 0;

    if(AccessToDevices)
    {
      MediaFoundationVideoDevices *VDS = &MediaFoundationVideoDevices::GetInstance();
      out = VDS->GetCount();

      LOG_INFO("VIDEOINPUT SPY MODE!");
      LOG_INFO("SETUP: Looking For Capture Devices.");

      for(int i = 0; i < out; i++)
      {
        LOG_INFO("SETUP: " << i << ", " << GetCaptureDeviceName(i) << ".");
      }

      LOG_INFO("SETUP: " << out << " Device(s) found.");

    }
    else
    {
      LOG_ERROR("VIDEODEVICE(s): There are not any suitable video devices.");
    }

    return out;
  }

  //----------------------------------------------------------------------------

  MediaFoundationVideoCaptureApi& MediaFoundationVideoCaptureApi::GetInstance() 
  {
    static MediaFoundationVideoCaptureApi instance;

    return instance;
  }

  //----------------------------------------------------------------------------

  bool MediaFoundationVideoCaptureApi::AreDevicesAccessable() const
  {
    return AccessToDevices;
  }

  //----------------------------------------------------------------------------

  void MediaFoundationVideoCaptureApi::SetEmergencyStopEvent(unsigned int deviceID, void *userData, void(*func)(int, void *))
  {
    if(AccessToDevices)
    {
      if(func)
      {
        MediaFoundationVideoDevices *VDS = &MediaFoundationVideoDevices::GetInstance();
        MediaFoundationVideoDevice * VD = VDS->GetDevice(deviceID);

        if(VD)  
          VD->SetEmergencyStopEvent(userData, func);
      }
    }
    else
    {
      LOG_ERROR("VIDEODEVICE(s): There are not any suitable video devices.");
    }
  }

  //----------------------------------------------------------------------------

  bool MediaFoundationVideoCaptureApi::GetPixels(unsigned int deviceID, unsigned char * dstBuffer, bool flipRedAndBlue, bool flipImage)
  {
    bool success = false;
    unsigned int bytes = 3;

    if(AccessToDevices)
    {
      if(IsDeviceRawDataSource(deviceID))
      {
        MediaFoundationVideoDevices *VDS = &MediaFoundationVideoDevices::GetInstance();
        RawImage *RIOut = VDS->GetDevice(deviceID)->GetRawImageOut();

        if(RIOut)
        {
          unsigned int height  = VDS->GetDevice(deviceID)->GetHeight();
          unsigned int width  = VDS->GetDevice(deviceID)->GetWidth(); 
          unsigned int size = bytes * width * height;

          if(size == RIOut->getSize())
          {                    
            ProcessPixels(RIOut->getpPixels(), dstBuffer, width, height, bytes, flipRedAndBlue, flipImage);

            success = true;
          }
          else
          {
            LOG_ERROR("MediaFoundationVideoCaptureApi::GetPixels() - bufferSizes do not match!");
          }
        }
        else
        {
          LOG_ERROR("MediaFoundationVideoCaptureApi::GetPixels() - Unable to grab frame for device " << deviceID << ".");
        }
      }
    }
    else
    {
      LOG_ERROR("VIDEODEVICE(s): There are not any suitable video devices.");
    }

    return success;
  }

  //----------------------------------------------------------------------------

  void MediaFoundationVideoCaptureApi::ProcessPixels(unsigned char * src, unsigned char * dst, unsigned int width, unsigned int height, unsigned int bpp, bool bRGB, bool bFlip)
  {
    unsigned int widthInBytes = width * bpp;
    unsigned int numBytes = widthInBytes * height;
    unsigned int numInts = numBytes >> 2;
    unsigned int widthInInts = widthInBytes >> 2;
    int *dstInt, *srcInt;

    if(!bRGB)
    {
      int x = 0;
      int y = 0;

      if(bFlip)
      {
        for(unsigned int y = 0; y < height; y++)
        {
          dstInt = (int *)(dst + (y * widthInBytes));
          srcInt = (int *)(src + ( (height -y -1) * widthInBytes));

          _asm
          {
            mov ESI, srcInt
              mov EDI, dstInt
              mov ECX, widthInInts
              cld
              rep movsd
          }
        }
      }
      else
      {    
        _asm
        {
          mov ESI, src
            mov EDI, dst
            mov ECX, numInts
            cld
            rep movsd
        }
      }
    }
    else
    {
      if(bFlip)
      {
        unsigned int x = 0;
        int y = (height - 1) * widthInBytes;
        src += y;

        for(unsigned int i = 0; i < numBytes; i+=3)
        {
          if(x >= width)
          {
            x = 0;
            src -= widthInBytes*2;
          }

          *dst = *(src+2);
          dst++;

          *dst = *(src+1);
          dst++; 

          *dst = *src;
          dst++; 

          src+=3;  
          x++;    
        }
      }
      else
      {            
        for(unsigned int i = 0; i < numBytes; i+=3)
        {
          *dst = *(src+2);
          dst++;

          *dst = *(src+1);
          dst++; 

          *dst = *src;
          dst++; 

          src+=3;      
        }
      }
    }
  }
}