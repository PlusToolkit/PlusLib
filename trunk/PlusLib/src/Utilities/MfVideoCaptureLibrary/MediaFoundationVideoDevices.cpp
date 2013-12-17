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

#include "MfVideoCaptureLoggerMacros.h"
#include "MediaFoundationVideoDevice.h"
#include "MediaFoundationVideoDevices.h"
#include <Mfidl.h>
#include <Mfobjects.h>

//----------------------------------------------------------------------------

namespace
{
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

namespace MfVideoCapture
{
  MediaFoundationVideoDevices::MediaFoundationVideoDevices(void): count(0)
  {
  }

  //----------------------------------------------------------------------------

  void MediaFoundationVideoDevices::ClearDevices()
  {
    std::vector<MediaFoundationVideoDevice *>::iterator i = Devices.begin();

    for(; i != Devices.end(); ++i)
      delete (*i);

    Devices.clear();
  }

  //----------------------------------------------------------------------------

  MediaFoundationVideoDevices::~MediaFoundationVideoDevices(void)
  {  
    ClearDevices();
  }

  //----------------------------------------------------------------------------

  MediaFoundationVideoDevice * MediaFoundationVideoDevices::GetDevice(unsigned int i)
  {
    if(i >= Devices.size())
    {
      return NULL;
    }

    if(i < 0)
    {
      return NULL;
    }

    return Devices[i];
  }

  //----------------------------------------------------------------------------

  long MediaFoundationVideoDevices::InitDevices(IMFAttributes *pAttributes)
  {
    HRESULT hr = S_OK;

    IMFActivate **ppDevices = NULL;

    ClearDevices();

    hr = MFEnumDeviceSources(pAttributes, &ppDevices, &count);

    if (SUCCEEDED(hr))
    {
      if(count > 0)
      {
        for(UINT32 i = 0; i < count; i++)
        {
          MediaFoundationVideoDevice *vd = new MediaFoundationVideoDevice;
          vd->ReadDeviceInfo(ppDevices[i], i);
          Devices.push_back(vd);    

          SafeRelease(&ppDevices[i]);
        }

        SafeRelease(ppDevices);
      }
      else
        hr = -1;
    }
    else
    {
      LOG_ERROR("VIDEODEVICES: The instances of the videoDevice class cannot be created.");
    }

    return hr;
  }

  //----------------------------------------------------------------------------

  int MediaFoundationVideoDevices::GetCount()
  {
    return Devices.size();
  }

  //----------------------------------------------------------------------------

  MediaFoundationVideoDevices& MediaFoundationVideoDevices::GetInstance() 
  {
    static MediaFoundationVideoDevices instance;

    return instance;
  }

}