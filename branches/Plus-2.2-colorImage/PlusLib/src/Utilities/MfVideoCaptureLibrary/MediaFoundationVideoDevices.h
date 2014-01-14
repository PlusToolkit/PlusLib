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

#ifndef __MediaFoundationVideoDevices_h
#define __MediaFoundationVideoDevices_h

#include <basetsd.h>
#include <vector>

struct IMFAttributes;
class MediaFoundationVideoDevice;

namespace MfVideoCapture
{
  /// Class for managing of list of video devices
  class MediaFoundationVideoDevices
  {
  public:
    long InitDevices(IMFAttributes *pAttributes);
    static MediaFoundationVideoDevices& GetInstance();
    MediaFoundationVideoDevice *GetDevice(unsigned int i);
    int GetCount();
    void ClearDevices();

  private:
    MediaFoundationVideoDevices(void);
    ~MediaFoundationVideoDevices(void);

    unsigned int count;
    std::vector<MediaFoundationVideoDevice *> Devices;
  };

}

#endif