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

#ifndef __ImageGrabberThread_h
#define __ImageGrabberThread_h

#include <windows.h>

DWORD WINAPI MainThreadFunction( LPVOID lpParam );

struct IMFMediaSource;

typedef void(*emergencyStopEventCallback)(int, void *);

namespace MfVideoCapture
{
  class ImageGrabber;

  /// Class for controlling of thread of the grabbing raw data from video device
  class ImageGrabberThread
  {
    friend DWORD WINAPI MainThreadFunction( LPVOID lpParam );

  public:
    ~ImageGrabberThread(void);

    static HRESULT CreateInstance(ImageGrabberThread **ppIGT, IMFMediaSource *pSource, unsigned int deviceID);
    void Start();
    void Stop();
    void SetEmergencyStopEvent(void *userData, void(*func)(int, void *));
    ImageGrabber *GetImageGrabber();

  protected:
    virtual void Run();

  private:
    ImageGrabberThread(IMFMediaSource *pSource, unsigned int deviceID);
    HANDLE ThreadHandle;
    DWORD   igt_ThreadIdArray;
    ImageGrabber* Grabber;
    emergencyStopEventCallback StopEventCallbackFunc;
    void *UserData;
    bool ShouldStop;
    unsigned int DeviceId;
  };

}

#endif