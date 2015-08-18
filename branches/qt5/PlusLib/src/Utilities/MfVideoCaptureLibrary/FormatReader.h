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

#ifndef __FormatReader_h
#define __FormatReader_h

#include <guiddef.h>
#include "MediaFoundationVideoCaptureApi.h"

struct IMFMediaType;

/*!
\class FormatReader 
\brief Class for parsing info from IMFMediaType into the local format

\ingroup PlusLibDataCollection
*/
namespace MfVideoCapture
{
  class FormatReader
  {
  public:
    static MediaType Read(IMFMediaType *pType);
    static std::wstring StringFromGUID(const GUID& aGuid);
    static GUID GUIDFromString(const std::wstring& aString);
  private:
    FormatReader(void);
    ~FormatReader(void);
  };
}
#endif