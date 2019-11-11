/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __DeckLinkAPIWrapper_h__
#define __DeckLinkAPIWrapper_h__

#if WIN32
  // Windows includes
  #include <comutil.h>
#endif

// DeckLink includes
#include <DeckLinkAPI.h>

// STL includes
#include <string>

#ifndef BMD_PUBLIC
  #define BMD_PUBLIC
#endif

class DeckLinkAPIWrapper
{
public:
  static IDeckLinkIterator* BMD_PUBLIC CreateDeckLinkIterator();
  static IDeckLinkDiscovery* BMD_PUBLIC CreateDeckLinkDiscovery();
  static IDeckLinkAPIInformation* BMD_PUBLIC CreateDeckLinkAPIInformation();
  static IDeckLinkGLScreenPreviewHelper* BMD_PUBLIC CreateOpenGLScreenPreviewHelper();
  static IDeckLinkVideoConversion* BMD_PUBLIC CreateVideoConversion();
  static IDeckLinkVideoFrameAncillaryPackets* BMD_PUBLIC CreateVideoFrameAncillaryPackets();

  static BMDPixelFormat PixelFormatFromString(const std::string&);
  static BMDVideoConnection VideoConnectionFromString(const std::string&);
  static BMDDisplayMode DisplayModeFromString(const std::string&);
};

#endif
