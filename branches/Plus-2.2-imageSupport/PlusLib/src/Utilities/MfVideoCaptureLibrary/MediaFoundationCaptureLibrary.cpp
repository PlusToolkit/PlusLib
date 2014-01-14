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
#include "MediaFoundationVideoDevice.h"
#include "MediaFoundationVideoDevices.h"
#include "MfVideoCaptureLoggerMacros.h"
#include <mfapi.h>
#include <mfidl.h>
#include <mfplay.h>

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
  //----------------------------------------------------------------------------

  MediaFoundationCaptureLibrary::MediaFoundationCaptureLibrary(void)
  {
    HRESULT hr = MFStartup(MF_VERSION);

    if(!SUCCEEDED(hr))
    {
      LOG_ERROR("MEDIA FOUNDATION: Unable to start up the media foundation framework.");
    }
  }

  //----------------------------------------------------------------------------

  MediaFoundationCaptureLibrary::~MediaFoundationCaptureLibrary(void)
  {
    HRESULT hr = MFShutdown();  

    if(!SUCCEEDED(hr))
    {
      LOG_ERROR("MEDIA FOUNDATION: Resources cannot be released.");
    }
  }

  //----------------------------------------------------------------------------

  bool MediaFoundationCaptureLibrary::BuildListOfDevices()
  {	
    HRESULT hr = S_OK;

    IMFAttributes *pAttributes = NULL;

    CoInitialize(NULL);

    hr = MFCreateAttributes(&pAttributes, 1);

    if (SUCCEEDED(hr))
    {
      hr = pAttributes->SetGUID(
        MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE,
        MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_GUID
        );
    }	

    if (SUCCEEDED(hr))
    {
      MediaFoundationVideoDevices *vDs = &MediaFoundationVideoDevices::GetInstance();

      hr = vDs->InitDevices(pAttributes);
    }	
    else
    {
      LOG_ERROR("MEDIA FOUNDATION: The access to the video cameras denied.");

    }

    SafeRelease(&pAttributes);

    return (SUCCEEDED(hr));
  }

  //----------------------------------------------------------------------------

  MediaFoundationCaptureLibrary& MediaFoundationCaptureLibrary::GetInstance() 
  {
    static MediaFoundationCaptureLibrary instance;

    return instance;
  }

}