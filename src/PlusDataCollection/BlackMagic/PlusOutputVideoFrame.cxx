/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

// Local includes
#include "PlusOutputVideoFrame.h"

//----------------------------------------------------------------------------
PlusOutputVideoFrame::PlusOutputVideoFrame(long width, long height, BMDPixelFormat pixelFormat, BMDFrameFlags frameFlags)
  : Width(width)
  , Height(height)
  , PixelFormat(pixelFormat)
  , FrameFlags(frameFlags)
{
  this->Pixels = new unsigned char[this->Height * this->GetRowBytes()];
}

//----------------------------------------------------------------------------
PlusOutputVideoFrame::~PlusOutputVideoFrame()
{
  delete [] this->Pixels;
  this->Pixels = nullptr;
}

//----------------------------------------------------------------------------
long STDMETHODCALLTYPE PlusOutputVideoFrame::GetWidth()
{
  return this->Width;
}

//----------------------------------------------------------------------------
long STDMETHODCALLTYPE PlusOutputVideoFrame::GetHeight()
{
  return this->Height;
}

//----------------------------------------------------------------------------
long STDMETHODCALLTYPE PlusOutputVideoFrame::GetRowBytes()
{
  switch (this->PixelFormat)
  {
    case bmdFormat8BitYUV:
      {
        return sizeof(char) * 2 * this->Width;
      }
    case bmdFormat10BitYUV:
      {
        // TBImplemented
        break;
      }
    case bmdFormat8BitARGB:
      {
        return sizeof(char) * 4 * this->Width;
      }
    case bmdFormat8BitBGRA:
      {
        return sizeof(char) * 4 * this->Width;
      }
    case bmdFormat10BitRGB:
      {
        // TBImplemented
        break;
      }
    case bmdFormat12BitRGB:
      {
        // TBImplemented
        break;
      }
    case bmdFormat12BitRGBLE:
      {
        // TBImplemented
        break;
      }
    case bmdFormat10BitRGBXLE:
      {
        // TBImplemented
        break;
      }
    case bmdFormat10BitRGBX:
      {
        // TBImplemented
        break;
      }
    case bmdFormatH265:
      {
        // TBImplemented
        break;
      }
    case bmdFormatDNxHR:
      {
        // TBImplemented
        break;
      }
    case bmdFormat12BitRAWGRBG:
      {
        // TBImplemented
        break;
      }
    case bmdFormat12BitRAWJPEG:
      {
        // TBImplemented
        break;
      }
  }

  return -1;
}

//----------------------------------------------------------------------------
BMDPixelFormat STDMETHODCALLTYPE PlusOutputVideoFrame::GetPixelFormat()
{
  return this->PixelFormat;
}

//----------------------------------------------------------------------------
BMDFrameFlags STDMETHODCALLTYPE PlusOutputVideoFrame::GetFlags()
{
  return this->FrameFlags;
}

//----------------------------------------------------------------------------
void STDMETHODCALLTYPE PlusOutputVideoFrame::SetFlags(BMDFrameFlags flags)
{
  this->FrameFlags = flags;
}

//----------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE PlusOutputVideoFrame::GetBytes(void** buffer)
{
  this->Mutex.lock();
  *buffer = Pixels;
  this->Mutex.unlock();

  return S_OK;
}

//----------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE PlusOutputVideoFrame::QueryInterface(REFIID iid, LPVOID* ppv)
{
  HRESULT result = E_NOINTERFACE;

  if (ppv == NULL)
  {
    return E_INVALIDARG;
  }

  // Initialize the return result
  *ppv = NULL;

  // Obtain the IUnknown interface and compare it the provided REFIID
  if (iid == IID_IUnknown)
  {
    *ppv = this;
    AddRef();
    result = S_OK;
  }
  else if (iid == IID_IDeckLinkVideoFrame)
  {
    *ppv = (IDeckLinkVideoFrame*)this;
    AddRef();
    result = S_OK;
  }

  return result;
}

//----------------------------------------------------------------------------
ULONG STDMETHODCALLTYPE PlusOutputVideoFrame::AddRef()
{
  return ++ReferenceCount;
}

//----------------------------------------------------------------------------
ULONG STDMETHODCALLTYPE PlusOutputVideoFrame::Release()
{
  ULONG newRefValue;

  ReferenceCount--;
  newRefValue = ReferenceCount;
  if (newRefValue == 0)
  {
    delete this;
    return 0;
  }

  return newRefValue;
}
