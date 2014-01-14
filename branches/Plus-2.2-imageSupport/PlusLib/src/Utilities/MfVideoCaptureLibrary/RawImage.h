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

#ifndef __RawImage_h
#define __RawImage_h

typedef unsigned char BYTE;

namespace MfVideoCapture
{
  class RawImage
  {
  public:
    // Function of creation of the instance of the class
    static long CreateInstance(RawImage **ppRImage,unsigned int size);
    void setCopy(const BYTE * pSampleBuffer);
    void fastCopy(const BYTE * pSampleBuffer);
    unsigned char * getpPixels();
    bool isNew();
    unsigned int getSize();

  private:
    bool ri_new;
    unsigned int ri_size;
    unsigned char *ri_pixels;
    RawImage(unsigned int size);
    ~RawImage(void);
  };

}

#endif