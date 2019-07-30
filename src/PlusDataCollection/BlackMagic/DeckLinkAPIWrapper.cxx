/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

// Local includes
#include "DeckLinkAPIWrapper.h"

// IGSIO includes
#include <igsioCommon.h>

//----------------------------------------------------------------------------
IDeckLinkIterator* BMD_PUBLIC DeckLinkAPIWrapper::CreateDeckLinkIterator()
{
#if WIN32
  IDeckLinkIterator* pointer;
  if (CoCreateInstance(CLSID_CDeckLinkIterator, NULL, CLSCTX_ALL, IID_IDeckLinkIterator, (void**)&pointer) != S_OK)
  {
    return nullptr;
  }
  else
  {
    return pointer;
  }
#else
  return ::CreateDeckLinkIteratorInstance();
#endif
}

//----------------------------------------------------------------------------
IDeckLinkDiscovery* BMD_PUBLIC DeckLinkAPIWrapper::CreateDeckLinkDiscovery()
{
#if WIN32
  IDeckLinkDiscovery* pointer;
  if (CoCreateInstance(CLSID_CDeckLinkDiscovery, NULL, CLSCTX_ALL, IID_IDeckLinkDiscovery, (void**)&pointer) != S_OK)
  {
    return nullptr;
  }
  else
  {
    return pointer;
  }
#else
  return ::CreateDeckLinkDiscoveryInstance();
#endif
}

//----------------------------------------------------------------------------
IDeckLinkAPIInformation* BMD_PUBLIC DeckLinkAPIWrapper::CreateDeckLinkAPIInformation()
{
#if WIN32
  IDeckLinkAPIInformation* pointer;
  if (CoCreateInstance(CLSID_CDeckLinkAPIInformation, NULL, CLSCTX_ALL, IID_IDeckLinkAPIInformation, (void**)&pointer) != S_OK)
  {
    return nullptr;
  }
  else
  {
    return pointer;
  }
#else
  return ::CreateDeckLinkAPIInformationInstance();
#endif
}

//----------------------------------------------------------------------------
IDeckLinkGLScreenPreviewHelper* BMD_PUBLIC DeckLinkAPIWrapper::CreateOpenGLScreenPreviewHelper()
{
#if WIN32
  IDeckLinkGLScreenPreviewHelper* pointer;
  if (CoCreateInstance(CLSID_CDeckLinkGLScreenPreviewHelper, NULL, CLSCTX_ALL, IID_IDeckLinkGLScreenPreviewHelper, (void**)&pointer) != S_OK)
  {
    return nullptr;
  }
  else
  {
    return pointer;
  }
#else
  return ::CreateOpenGLScreenPreviewHelper();
#endif
}

//----------------------------------------------------------------------------
IDeckLinkVideoConversion* BMD_PUBLIC DeckLinkAPIWrapper::CreateVideoConversion()
{
#if WIN32
  IDeckLinkVideoConversion* pointer;
  if (CoCreateInstance(CLSID_CDeckLinkVideoConversion, NULL, CLSCTX_ALL, IID_IDeckLinkVideoConversion, (void**)&pointer) != S_OK)
  {
    return nullptr;
  }
  else
  {
    return pointer;
  }
#else
  return ::CreateVideoConversionInstance();
#endif
}

//----------------------------------------------------------------------------
IDeckLinkVideoFrameAncillaryPackets* BMD_PUBLIC DeckLinkAPIWrapper::CreateVideoFrameAncillaryPackets()
{
#if WIN32
  IDeckLinkVideoFrameAncillaryPackets* pointer;
  if (CoCreateInstance(CLSID_CDeckLinkVideoFrameAncillaryPackets, NULL, CLSCTX_ALL, IID_IDeckLinkVideoFrameAncillaryPackets, (void**)&pointer) != S_OK)
  {
    return nullptr;
  }
  else
  {
    return pointer;
  }
#else
  return ::CreateVideoFrameAncillaryPacketsInstance();
#endif
}

//----------------------------------------------------------------------------
BMDPixelFormat DeckLinkAPIWrapper::PixelFormatFromString(const std::string& _arg)
{
  if (igsioCommon::IsEqualInsensitive(_arg, "8BitYUV"))
  {
    return bmdFormat8BitYUV;
  }
  else if (igsioCommon::IsEqualInsensitive(_arg, "10BitYUV"))
  {
    return bmdFormat10BitYUV;
  }
  else if (igsioCommon::IsEqualInsensitive(_arg, "8BitARGB"))
  {
    return bmdFormat8BitARGB;
  }
  else if (igsioCommon::IsEqualInsensitive(_arg, "8BitBGRA"))
  {
    return bmdFormat8BitBGRA;
  }
  else if (igsioCommon::IsEqualInsensitive(_arg, "10BitRGB"))
  {
    return bmdFormat10BitRGB;
  }
  else if (igsioCommon::IsEqualInsensitive(_arg, "12BitRGB"))
  {
    return bmdFormat12BitRGB;
  }
  else if (igsioCommon::IsEqualInsensitive(_arg, "12BitRGBLE"))
  {
    return bmdFormat12BitRGBLE;
  }
  else if (igsioCommon::IsEqualInsensitive(_arg, "10BitRGBXLE"))
  {
    return bmdFormat10BitRGBXLE;
  }
  else if (igsioCommon::IsEqualInsensitive(_arg, "10BitRGBX"))
  {
    return bmdFormat10BitRGBX;
  }
  else if (igsioCommon::IsEqualInsensitive(_arg, "H265"))
  {
    return bmdFormatH265;
  }
  else if (igsioCommon::IsEqualInsensitive(_arg, "DNxHR"))
  {
    return bmdFormatDNxHR;
  }
  else if (igsioCommon::IsEqualInsensitive(_arg, "12BitRAWGRBG"))
  {
    return bmdFormat12BitRAWGRBG;
  }
  else if (igsioCommon::IsEqualInsensitive(_arg, "12BitRAWJPEG"))
  {
    return bmdFormat12BitRAWJPEG;
  }
  else
  {
    return bmdFormatUnspecified;
  }
}

//----------------------------------------------------------------------------
BMDVideoConnection DeckLinkAPIWrapper::VideoConnectionFromString(const std::string& _arg)
{
  if (igsioCommon::IsEqualInsensitive(_arg, "SDI"))
  {
    return bmdVideoConnectionSDI;
  }
  else if (igsioCommon::IsEqualInsensitive(_arg, "HDMI"))
  {
    return bmdVideoConnectionHDMI;
  }
  else if (igsioCommon::IsEqualInsensitive(_arg, "OpticalSDI"))
  {
    return bmdVideoConnectionOpticalSDI;
  }
  else if (igsioCommon::IsEqualInsensitive(_arg, "Component"))
  {
    return bmdVideoConnectionComponent;
  }
  else if (igsioCommon::IsEqualInsensitive(_arg, "Composite"))
  {
    return bmdVideoConnectionComposite;
  }
  else if (igsioCommon::IsEqualInsensitive(_arg, "SVideo"))
  {
    return bmdVideoConnectionSVideo;
  }
  else
  {
    return bmdVideoConnectionUnspecified;
  }
}

//----------------------------------------------------------------------------
BMDDisplayMode DeckLinkAPIWrapper::DisplayModeFromString(const std::string& _arg)
{
  if (igsioCommon::IsEqualInsensitive(_arg, "NTSC"))
  {
    return bmdModeNTSC;
  }
  else if (igsioCommon::IsEqualInsensitive(_arg, "NTSC2398"))
  {
    return bmdModeNTSC2398;
  }
  else if (igsioCommon::IsEqualInsensitive(_arg, "PAL"))
  {
    return bmdModePAL;
  }
  else if (igsioCommon::IsEqualInsensitive(_arg, "NTSCp"))
  {
    return bmdModeNTSCp;
  }
  else if (igsioCommon::IsEqualInsensitive(_arg, "PALp"))
  {
    return bmdModePALp;
  }
  else if (igsioCommon::IsEqualInsensitive(_arg, "HD1080p2398"))
  {
    return bmdModeHD1080p2398;
  }
  else if (igsioCommon::IsEqualInsensitive(_arg, "HD1080p24"))
  {
    return bmdModeHD1080p24;
  }
  else if (igsioCommon::IsEqualInsensitive(_arg, "HD1080p25"))
  {
    return bmdModeHD1080p25;
  }
  else if (igsioCommon::IsEqualInsensitive(_arg, "HD1080p2997"))
  {
    return bmdModeHD1080p2997;
  }
  else if (igsioCommon::IsEqualInsensitive(_arg, "HD1080p30"))
  {
    return bmdModeHD1080p30;
  }
  else if (igsioCommon::IsEqualInsensitive(_arg, "HD1080p4795"))
  {
    return bmdModeHD1080p4795;
  }
  else if (igsioCommon::IsEqualInsensitive(_arg, "HD1080p48"))
  {
    return bmdModeHD1080p48;
  }
  else if (igsioCommon::IsEqualInsensitive(_arg, "HD1080p50"))
  {
    return bmdModeHD1080p50;
  }
  else if (igsioCommon::IsEqualInsensitive(_arg, "HD1080p5994"))
  {
    return bmdModeHD1080p5994;
  }
  else if (igsioCommon::IsEqualInsensitive(_arg, "HD1080p6000"))
  {
    return bmdModeHD1080p6000;
  }
  else if (igsioCommon::IsEqualInsensitive(_arg, "HD1080p9590"))
  {
    return bmdModeHD1080p9590;
  }
  else if (igsioCommon::IsEqualInsensitive(_arg, "HD1080p96"))
  {
    return bmdModeHD1080p96;
  }
  else if (igsioCommon::IsEqualInsensitive(_arg, "HD1080p100"))
  {
    return bmdModeHD1080p100;
  }
  else if (igsioCommon::IsEqualInsensitive(_arg, "HD1080p11988"))
  {
    return bmdModeHD1080p11988;
  }
  else if (igsioCommon::IsEqualInsensitive(_arg, "HD1080p120"))
  {
    return bmdModeHD1080p120;
  }
  else if (igsioCommon::IsEqualInsensitive(_arg, "HD1080i50"))
  {
    return bmdModeHD1080i50;
  }
  else if (igsioCommon::IsEqualInsensitive(_arg, "HD1080i5994"))
  {
    return bmdModeHD1080i5994;
  }
  else if (igsioCommon::IsEqualInsensitive(_arg, "HD1080i6000"))
  {
    return bmdModeHD1080i6000;
  }
  else if (igsioCommon::IsEqualInsensitive(_arg, "HD720p50"))
  {
    return bmdModeHD720p50;
  }
  else if (igsioCommon::IsEqualInsensitive(_arg, "HD720p5994"))
  {
    return bmdModeHD720p5994;
  }
  else if (igsioCommon::IsEqualInsensitive(_arg, "HD720p60"))
  {
    return bmdModeHD720p60;
  }
  else if (igsioCommon::IsEqualInsensitive(_arg, "2k2398"))
  {
    return bmdMode2k2398;
  }
  else if (igsioCommon::IsEqualInsensitive(_arg, "2k24"))
  {
    return bmdMode2k24;
  }
  else if (igsioCommon::IsEqualInsensitive(_arg, "2k25"))
  {
    return bmdMode2k25;
  }
  else if (igsioCommon::IsEqualInsensitive(_arg, "2kDCI2398"))
  {
    return bmdMode2kDCI2398;
  }
  else if (igsioCommon::IsEqualInsensitive(_arg, "2kDCI24"))
  {
    return bmdMode2kDCI24;
  }
  else if (igsioCommon::IsEqualInsensitive(_arg, "2kDCI25"))
  {
    return bmdMode2kDCI25;
  }
  else if (igsioCommon::IsEqualInsensitive(_arg, "2kDCI2997"))
  {
    return bmdMode2kDCI2997;
  }
  else if (igsioCommon::IsEqualInsensitive(_arg, "2kDCI30"))
  {
    return bmdMode2kDCI30;
  }
  else if (igsioCommon::IsEqualInsensitive(_arg, "2kDCI4795"))
  {
    return bmdMode2kDCI4795;
  }
  else if (igsioCommon::IsEqualInsensitive(_arg, "2kDCI48"))
  {
    return bmdMode2kDCI48;
  }
  else if (igsioCommon::IsEqualInsensitive(_arg, "2kDCI50"))
  {
    return bmdMode2kDCI50;
  }
  else if (igsioCommon::IsEqualInsensitive(_arg, "2kDCI5994"))
  {
    return bmdMode2kDCI5994;
  }
  else if (igsioCommon::IsEqualInsensitive(_arg, "2kDCI60"))
  {
    return bmdMode2kDCI60;
  }
  else if (igsioCommon::IsEqualInsensitive(_arg, "2kDCI9590"))
  {
    return bmdMode2kDCI9590;
  }
  else if (igsioCommon::IsEqualInsensitive(_arg, "2kDCI96"))
  {
    return bmdMode2kDCI96;
  }
  else if (igsioCommon::IsEqualInsensitive(_arg, "2kDCI100"))
  {
    return bmdMode2kDCI100;
  }
  else if (igsioCommon::IsEqualInsensitive(_arg, "2kDCI11988"))
  {
    return bmdMode2kDCI11988;
  }
  else if (igsioCommon::IsEqualInsensitive(_arg, "2kDCI120"))
  {
    return bmdMode2kDCI120;
  }
  else if (igsioCommon::IsEqualInsensitive(_arg, "4K2160p2398"))
  {
    return bmdMode4K2160p2398;
  }
  else if (igsioCommon::IsEqualInsensitive(_arg, "4K2160p24"))
  {
    return bmdMode4K2160p24;
  }
  else if (igsioCommon::IsEqualInsensitive(_arg, "4K2160p25"))
  {
    return bmdMode4K2160p25;
  }
  else if (igsioCommon::IsEqualInsensitive(_arg, "4K2160p2997"))
  {
    return bmdMode4K2160p2997;
  }
  else if (igsioCommon::IsEqualInsensitive(_arg, "4K2160p30"))
  {
    return bmdMode4K2160p30;
  }
  else if (igsioCommon::IsEqualInsensitive(_arg, "4K2160p4795"))
  {
    return bmdMode4K2160p4795;
  }
  else if (igsioCommon::IsEqualInsensitive(_arg, "4K2160p48"))
  {
    return bmdMode4K2160p48;
  }
  else if (igsioCommon::IsEqualInsensitive(_arg, "4K2160p50"))
  {
    return bmdMode4K2160p50;
  }
  else if (igsioCommon::IsEqualInsensitive(_arg, "4K2160p5994"))
  {
    return bmdMode4K2160p5994;
  }
  else if (igsioCommon::IsEqualInsensitive(_arg, "4K2160p60"))
  {
    return bmdMode4K2160p60;
  }
  else if (igsioCommon::IsEqualInsensitive(_arg, "4K2160p9590"))
  {
    return bmdMode4K2160p9590;
  }
  else if (igsioCommon::IsEqualInsensitive(_arg, "4K2160p96"))
  {
    return bmdMode4K2160p96;
  }
  else if (igsioCommon::IsEqualInsensitive(_arg, "4K2160p100"))
  {
    return bmdMode4K2160p100;
  }
  else if (igsioCommon::IsEqualInsensitive(_arg, "4K2160p11988"))
  {
    return bmdMode4K2160p11988;
  }
  else if (igsioCommon::IsEqualInsensitive(_arg, "4K2160p120"))
  {
    return bmdMode4K2160p120;
  }
  else if (igsioCommon::IsEqualInsensitive(_arg, "4kDCI2398"))
  {
    return bmdMode4kDCI2398;
  }
  else if (igsioCommon::IsEqualInsensitive(_arg, "4kDCI24"))
  {
    return bmdMode4kDCI24;
  }
  else if (igsioCommon::IsEqualInsensitive(_arg, "4kDCI25"))
  {
    return bmdMode4kDCI25;
  }
  else if (igsioCommon::IsEqualInsensitive(_arg, "4kDCI2997"))
  {
    return bmdMode4kDCI2997;
  }
  else if (igsioCommon::IsEqualInsensitive(_arg, "4kDCI30"))
  {
    return bmdMode4kDCI30;
  }
  else if (igsioCommon::IsEqualInsensitive(_arg, "4kDCI4795"))
  {
    return bmdMode4kDCI4795;
  }
  else if (igsioCommon::IsEqualInsensitive(_arg, "4kDCI48"))
  {
    return bmdMode4kDCI48;
  }
  else if (igsioCommon::IsEqualInsensitive(_arg, "4kDCI50"))
  {
    return bmdMode4kDCI50;
  }
  else if (igsioCommon::IsEqualInsensitive(_arg, "4kDCI5994"))
  {
    return bmdMode4kDCI5994;
  }
  else if (igsioCommon::IsEqualInsensitive(_arg, "4kDCI60"))
  {
    return bmdMode4kDCI60;
  }
  else if (igsioCommon::IsEqualInsensitive(_arg, "4kDCI9590"))
  {
    return bmdMode4kDCI9590;
  }
  else if (igsioCommon::IsEqualInsensitive(_arg, "4kDCI96"))
  {
    return bmdMode4kDCI96;
  }
  else if (igsioCommon::IsEqualInsensitive(_arg, "4kDCI100"))
  {
    return bmdMode4kDCI100;
  }
  else if (igsioCommon::IsEqualInsensitive(_arg, "4kDCI11988"))
  {
    return bmdMode4kDCI11988;
  }
  else if (igsioCommon::IsEqualInsensitive(_arg, "4kDCI120"))
  {
    return bmdMode4kDCI120;
  }
  else if (igsioCommon::IsEqualInsensitive(_arg, "8K4320p2398"))
  {
    return bmdMode8K4320p2398;
  }
  else if (igsioCommon::IsEqualInsensitive(_arg, "8K4320p24"))
  {
    return bmdMode8K4320p24;
  }
  else if (igsioCommon::IsEqualInsensitive(_arg, "8K4320p25"))
  {
    return bmdMode8K4320p25;
  }
  else if (igsioCommon::IsEqualInsensitive(_arg, "8K4320p2997"))
  {
    return bmdMode8K4320p2997;
  }
  else if (igsioCommon::IsEqualInsensitive(_arg, "8K4320p30"))
  {
    return bmdMode8K4320p30;
  }
  else if (igsioCommon::IsEqualInsensitive(_arg, "8K4320p4795"))
  {
    return bmdMode8K4320p4795;
  }
  else if (igsioCommon::IsEqualInsensitive(_arg, "8K4320p48"))
  {
    return bmdMode8K4320p48;
  }
  else if (igsioCommon::IsEqualInsensitive(_arg, "8K4320p50"))
  {
    return bmdMode8K4320p50;
  }
  else if (igsioCommon::IsEqualInsensitive(_arg, "8K4320p5994"))
  {
    return bmdMode8K4320p5994;
  }
  else if (igsioCommon::IsEqualInsensitive(_arg, "8K4320p60"))
  {
    return bmdMode8K4320p60;
  }
  else if (igsioCommon::IsEqualInsensitive(_arg, "8kDCI2398"))
  {
    return bmdMode8kDCI2398;
  }
  else if (igsioCommon::IsEqualInsensitive(_arg, "8kDCI24"))
  {
    return bmdMode8kDCI24;
  }
  else if (igsioCommon::IsEqualInsensitive(_arg, "8kDCI25"))
  {
    return bmdMode8kDCI25;
  }
  else if (igsioCommon::IsEqualInsensitive(_arg, "8kDCI2997"))
  {
    return bmdMode8kDCI2997;
  }
  else if (igsioCommon::IsEqualInsensitive(_arg, "8kDCI30"))
  {
    return bmdMode8kDCI30;
  }
  else if (igsioCommon::IsEqualInsensitive(_arg, "8kDCI4795"))
  {
    return bmdMode8kDCI4795;
  }
  else if (igsioCommon::IsEqualInsensitive(_arg, "8kDCI48"))
  {
    return bmdMode8kDCI48;
  }
  else if (igsioCommon::IsEqualInsensitive(_arg, "8kDCI50"))
  {
    return bmdMode8kDCI50;
  }
  else if (igsioCommon::IsEqualInsensitive(_arg, "8kDCI5994"))
  {
    return bmdMode8kDCI5994;
  }
  else if (igsioCommon::IsEqualInsensitive(_arg, "8kDCI60"))
  {
    return bmdMode8kDCI60;
  }
  else if (igsioCommon::IsEqualInsensitive(_arg, "640x480p60"))
  {
    return bmdMode640x480p60;
  }
  else if (igsioCommon::IsEqualInsensitive(_arg, "800x600p60"))
  {
    return bmdMode800x600p60;
  }
  else if (igsioCommon::IsEqualInsensitive(_arg, "1440x900p50"))
  {
    return bmdMode1440x900p50;
  }
  else if (igsioCommon::IsEqualInsensitive(_arg, "1440x900p60"))
  {
    return bmdMode1440x900p60;
  }
  else if (igsioCommon::IsEqualInsensitive(_arg, "1440x1080p50"))
  {
    return bmdMode1440x1080p50;
  }
  else if (igsioCommon::IsEqualInsensitive(_arg, "1440x1080p60"))
  {
    return bmdMode1440x1080p60;
  }
  else if (igsioCommon::IsEqualInsensitive(_arg, "1600x1200p50"))
  {
    return bmdMode1600x1200p50;
  }
  else if (igsioCommon::IsEqualInsensitive(_arg, "1600x1200p60"))
  {
    return bmdMode1600x1200p60;
  }
  else if (igsioCommon::IsEqualInsensitive(_arg, "1920x1200p50"))
  {
    return bmdMode1920x1200p50;
  }
  else if (igsioCommon::IsEqualInsensitive(_arg, "1920x1200p60"))
  {
    return bmdMode1920x1200p60;
  }
  else if (igsioCommon::IsEqualInsensitive(_arg, "1920x1440p50"))
  {
    return bmdMode1920x1440p50;
  }
  else if (igsioCommon::IsEqualInsensitive(_arg, "1920x1440p60"))
  {
    return bmdMode1920x1440p60;
  }
  else if (igsioCommon::IsEqualInsensitive(_arg, "2560x1440p50"))
  {
    return bmdMode2560x1440p50;
  }
  else if (igsioCommon::IsEqualInsensitive(_arg, "2560x1440p60"))
  {
    return bmdMode2560x1440p60;
  }
  else if (igsioCommon::IsEqualInsensitive(_arg, "2560x1600p50"))
  {
    return bmdMode2560x1600p50;
  }
  else if (igsioCommon::IsEqualInsensitive(_arg, "2560x1600p60"))
  {
    return bmdMode2560x1600p60;
  }
  else if (igsioCommon::IsEqualInsensitive(_arg, "CintelRAW"))
  {
    return bmdModeCintelRAW;
  }
  else if (igsioCommon::IsEqualInsensitive(_arg, "CintelCompressedRAW"))
  {
    return bmdModeCintelCompressedRAW;
  }
  else
  {
    return bmdModeUnknown;
  }
}