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

#include "RawImage.h"
#include <Mfapi.h>
#include <new>
#include <stdio.h>
#include <windows.h>

namespace MfVideoCapture
{
  RawImage::RawImage(unsigned int size): ri_new(false), ri_pixels(NULL)
  {
    ri_size = size;

    ri_pixels = new unsigned char[size];

    memset((void *)ri_pixels,0,ri_size);
  }

  bool RawImage::isNew()
  {
    return ri_new;
  }

  unsigned int RawImage::getSize()
  {
    return ri_size;
  }

  RawImage::~RawImage(void)
  {
    delete []ri_pixels;

    ri_pixels = NULL;
  }

  long RawImage::CreateInstance(RawImage **ppRImage,unsigned int size)
  {
    *ppRImage = new (std::nothrow) RawImage(size);

    if (ppRImage == NULL)
    {
      return E_OUTOFMEMORY;
    }
    return S_OK;
  }

  void RawImage::setCopy(const BYTE * pSampleBuffer)
  {
    memcpy(ri_pixels, pSampleBuffer, ri_size);

    ri_new = true;
  }

  void RawImage::fastCopy(const BYTE * pSampleBuffer)
  {
    int *bsrc = (int *)pSampleBuffer;

    int *dst = (int *)ri_pixels;

    unsigned int buffersize = ri_size/4;

    _asm
    {
      mov ESI, bsrc

        mov EDI, dst

        mov ECX, buffersize

        cld

        rep movsd
    }

    ri_new = true;

  }

  unsigned char * RawImage::getpPixels()
  {
    return ri_pixels;

    ri_new = false;
  }

}