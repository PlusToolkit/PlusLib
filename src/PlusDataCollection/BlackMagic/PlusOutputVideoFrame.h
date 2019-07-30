/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __PlusOutputVideoFrame_h__
#define __PlusOutputVideoFrame_h__

// DeckLink includes
#if WIN32
  // Windows includes
  #include <comutil.h>
#endif
#include <DeckLinkAPI.h>

// STL includes
#include <atomic>
#include <mutex>

class PlusOutputVideoFrame : public IDeckLinkVideoFrame
{
public:
  PlusOutputVideoFrame(long width, long height, BMDPixelFormat pixelFormat, BMDFrameFlags frameFlags);
  virtual ~PlusOutputVideoFrame();

  virtual long STDMETHODCALLTYPE GetWidth();
  virtual long STDMETHODCALLTYPE GetHeight();
  virtual long STDMETHODCALLTYPE GetRowBytes();
  virtual BMDPixelFormat STDMETHODCALLTYPE GetPixelFormat();
  virtual BMDFrameFlags STDMETHODCALLTYPE GetFlags();
  virtual void STDMETHODCALLTYPE SetFlags(BMDFrameFlags flags);
  virtual HRESULT STDMETHODCALLTYPE GetBytes(void** buffer);

  virtual HRESULT STDMETHODCALLTYPE GetTimecode(BMDTimecodeFormat format, IDeckLinkTimecode** timecode) {return E_NOTIMPL;}
  virtual HRESULT STDMETHODCALLTYPE GetAncillaryData(IDeckLinkVideoFrameAncillary** ancillary) {return E_NOTIMPL;}

  // IUnknown interface
  virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, LPVOID* ppv);
  virtual ULONG STDMETHODCALLTYPE AddRef();
  virtual ULONG STDMETHODCALLTYPE Release();

protected:
  long                Width;
  long                Height;
  BMDPixelFormat      PixelFormat;
  BMDFrameFlags       FrameFlags;
  BMDTimecodeFormat   TimecodeFormat;
  void*               Pixels;
  std::mutex          Mutex;

protected:
  std::atomic<ULONG>  ReferenceCount;
};

#endif
