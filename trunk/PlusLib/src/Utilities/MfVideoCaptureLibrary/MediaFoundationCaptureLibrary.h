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

#ifndef __VideoInputLibrary_h
#define __VideoInputLibrary_h

struct IMFAttributes;

namespace MfVideoCapture
{
  // Class for creating of Media Foundation context
  class MediaFoundationCaptureLibrary
  {
  public:
    static MediaFoundationCaptureLibrary& GetInstance();
    bool BuildListOfDevices();

  private: 
    MediaFoundationCaptureLibrary(void);
    virtual ~MediaFoundationCaptureLibrary(void);
  };
}

#endif