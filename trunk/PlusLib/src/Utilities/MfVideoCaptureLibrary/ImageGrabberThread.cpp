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
#include "ImageGrabber.h"
#include "ImageGrabberThread.h"
#include <mfapi.h>
#include <new>

namespace MfVideoCapture
{

  DWORD WINAPI MainThreadFunction( LPVOID lpParam )
  {
    ImageGrabberThread *pIGT = (ImageGrabberThread *)lpParam;
    pIGT->Run();
    return 0; 
  }

  HRESULT ImageGrabberThread::CreateInstance(ImageGrabberThread **ppIGT, IMFMediaSource *pSource, unsigned int deviceID)
  {
    *ppIGT = new (std::nothrow) ImageGrabberThread(pSource, deviceID);

    if (ppIGT == NULL)
    {
      LOG_ERROR("IMAGEGRABBERTHREAD VIDEODEVICE " << deviceID << ": Memory cannot be allocated.");

      return E_OUTOFMEMORY;
    }
    else
      LOG_ERROR("IMAGEGRABBERTHREAD VIDEODEVICE " << deviceID << ": Creating of the instance of ImageGrabberThread.");

    return S_OK;
  }

  ImageGrabberThread::ImageGrabberThread(IMFMediaSource *pSource, unsigned int deviceID): ThreadHandle(NULL), ShouldStop(false)
  {
    HRESULT hr = ImageGrabber::CreateInstance(&Grabber, deviceID);
    DeviceId = deviceID;

    if(SUCCEEDED(hr))
    {
      hr = Grabber->initImageGrabber(pSource, MFVideoFormat_RGB24);

      if(!SUCCEEDED(hr))
      {
        LOG_ERROR("IMAGEGRABBERTHREAD VIDEODEVICE " << DeviceId << ": There is a problem with initialization of the instance of the ImageGrabber class.");
      }
      else
      {
        LOG_ERROR("IMAGEGRABBERTHREAD VIDEODEVICE " << DeviceId << ": Initialization of instance of the ImageGrabber class.");
      }
    }
    else
    {
      LOG_ERROR("IMAGEGRABBERTHREAD VIDEODEVICE " << DeviceId << ": There is a problem with creation of the instance of the ImageGrabber class.");
    }
  }

  void ImageGrabberThread::SetEmergencyStopEvent(void *userData, void(*func)(int, void *))
  {
    if(func)
    {
      StopEventCallbackFunc = func;
      UserData = userData;
    }
  }

  ImageGrabberThread::~ImageGrabberThread(void)
  {
    LOG_DEBUG("IMAGEGRABBERTHREAD VIDEODEVICE " << DeviceId << ": Destroing ImageGrabberThread.");
    delete Grabber;
  }

  void ImageGrabberThread::Stop()
  {
    ShouldStop = true;

    if(Grabber)
    {
      Grabber->stopGrabbing();
    }
  }

  void ImageGrabberThread::Start()
  {
    ThreadHandle = CreateThread( 
      NULL,                   // default security attributes
      0,                      // use default stack size  
      MainThreadFunction,       // thread function name
      this,          // argument to thread function 
      0,                      // use default creation flags 
      &igt_ThreadIdArray);   // returns the thread identifier 
  }

  void ImageGrabberThread::Run()
  {
    if(Grabber)
    {
      LOG_DEBUG("IMAGEGRABBERTHREAD VIDEODEVICE " << DeviceId << ": Thread for grabbing images is started.");

      HRESULT hr = Grabber->startGrabbing();

      if(!SUCCEEDED(hr))		
      {
        LOG_ERROR("IMAGEGRABBERTHREAD VIDEODEVICE " << DeviceId << ": There is a problem with starting the process of grabbing.");
      }

    }
    else
    {
      LOG_DEBUG("IMAGEGRABBERTHREAD VIDEODEVICE " << DeviceId << ": The thread is finished without execution of grabbing.");
    }

    if(!ShouldStop)
    {
      LOG_INFO("IMAGEGRABBERTHREAD VIDEODEVICE " << DeviceId << ": Emergency Stop thread.");

      if(StopEventCallbackFunc)
      {
        StopEventCallbackFunc(DeviceId, UserData);
      }
    }
    else
    {
      LOG_DEBUG("IMAGEGRABBERTHREAD VIDEODEVICE " << DeviceId << ": Finish thread");
    }
  }

  ImageGrabber *ImageGrabberThread::GetImageGrabber()
  {
    return Grabber;
  }

}