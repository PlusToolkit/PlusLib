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

#ifndef __MediaFoundationVideoDevice_h
#define __MediaFoundationVideoDevice_h

#include "MediaFoundationVideoCaptureApi.h"
#include <map>
#include <string>
#include <vector>
#include <stdint.h>

struct IMFActivate;
struct IMFMediaSource;
struct IMFMediaType;
struct IMFAttributes;

typedef void(*emergencyStopEventCallback)(int, void *);

namespace MfVideoCapture
{

  /*!
  \class MediaFoundationVideoDevice 
  \brief Interface class to a media foundation capture device

  \ingroup PlusLibDataCollection
  */
  class MediaFoundationVideoDevice
  {
    enum TypeLock
    {
      MediaSourceLock,
      RawDataLock,
      OpenLock
    };
  public:
    MediaFoundationVideoDevice(void);
    ~MediaFoundationVideoDevice(void);

    void CloseDevice();
    CaptureDeviceParameters GetParameters();
    void SetParameters(CaptureDeviceParameters Parameters);
    void SetEmergencyStopEvent(void *userData, void(*func)(int, void *));
    long ReadDeviceInfo(IMFActivate *pActivate, unsigned int Num);
    wchar_t *GetName();
    bool Start();
    bool Stop();
    unsigned int GetNumberOfFormats(unsigned int streamIndex) const;
    unsigned int GetWidth() const;
    unsigned int GetHeight() const;
    unsigned int GetFrameRate() const;
    MediaType GetFormat(unsigned int streamIndex, unsigned int id) const;
    int GetNumberOfStreams() const;
    unsigned int GetActiveType() const;
    bool SetupDevice(unsigned int streamIndex, unsigned int w, unsigned int h, unsigned int idealFramerate = 0, GUID subtype = MFVideoFormat_YUY2);
    bool SetupDevice(unsigned int streamIndex, unsigned int formatIndex);
    bool IsDeviceSetup() const;
    bool IsDeviceMediaSource() const;
    bool IsDeviceRawDataSource() const;
    IMFMediaSource *GetMediaSource();

  protected:
    long EnumerateCaptureFormats(IMFMediaSource *pSource);
    long SetDeviceFormat(IMFMediaSource *pSource, DWORD streamIndex, DWORD dwFormatIndex);
    bool FindType(unsigned int& formatIndex, unsigned int streamIndex, unsigned int w, unsigned int h, unsigned int frameRate = 0, GUID subtype = MFVideoFormat_YUY2);
    long ResetDevice(IMFActivate *pActivate);
    long InitDevice();
    long CheckDevice(IMFAttributes *pAttributes, IMFActivate **pDevice);

    bool IsSetup;
    TypeLock LockOut;
    wchar_t *FriendlyName;
    unsigned int Width;
    unsigned int Height;
    unsigned int FrameRate;
    IMFMediaSource *Source;
    emergencyStopEventCallback StopEventCallbackFunc;
    CaptureDeviceParameters PreviousParameters;
    unsigned int DeviceIndex;
    unsigned int ActiveType;
    std::vector< std::vector<MediaType> > CurrentFormats; // list of formats for each stream (mediatype[streamIndex][formatIndex])
    void *UserData;
  };
}
#endif