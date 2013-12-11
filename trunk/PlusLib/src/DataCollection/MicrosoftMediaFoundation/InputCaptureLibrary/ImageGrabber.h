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

#ifndef __ImageGrabber_h
#define __ImageGrabber_h

#include <mfidl.h>

struct IMFMediaSource;
namespace MfVideoCapture
{
  class RawImage;

  // Class for grabbing image from video stream
  class ImageGrabber : public IMFSampleGrabberSinkCallback
  {
  public:
    ~ImageGrabber(void);

    HRESULT initImageGrabber(IMFMediaSource *pSource, GUID VideoFormat);

    HRESULT startGrabbing(void);

    void stopGrabbing();

    RawImage *getRawImage();

    // Function of creation of the instance of the class
    static HRESULT CreateInstance(ImageGrabber **ppIG,unsigned int deviceID);

  private:
    bool ig_RIE;
    bool ig_Close;
    long m_cRef;
    unsigned int ig_DeviceID;
    IMFMediaSource *ig_pSource;
    IMFMediaSession *ig_pSession;
    IMFTopology *ig_pTopology;
    RawImage *ig_RIFirst;
    RawImage *ig_RISecond;
    RawImage *ig_RIOut;

    ImageGrabber(unsigned int deviceID);

    HRESULT CreateTopology(IMFMediaSource *pSource, IMFActivate *pSinkActivate, IMFTopology **ppTopo);

    HRESULT AddSourceNode(
      IMFTopology *pTopology,           
      IMFMediaSource *pSource,          
      IMFPresentationDescriptor *pPD,   
      IMFStreamDescriptor *pSD,         
      IMFTopologyNode **ppNode);

    HRESULT AddOutputNode(
      IMFTopology *pTopology,     
      IMFActivate *pActivate,     
      DWORD dwId,                 
      IMFTopologyNode **ppNode);

    // IUnknown methods
    STDMETHODIMP QueryInterface(REFIID iid, void** ppv);
    STDMETHODIMP_(ULONG) AddRef();
    STDMETHODIMP_(ULONG) Release();

    // IMFClockStateSink methods
    STDMETHODIMP OnClockStart(MFTIME hnsSystemTime, LONGLONG llClockStartOffset);
    STDMETHODIMP OnClockStop(MFTIME hnsSystemTime);
    STDMETHODIMP OnClockPause(MFTIME hnsSystemTime);
    STDMETHODIMP OnClockRestart(MFTIME hnsSystemTime);
    STDMETHODIMP OnClockSetRate(MFTIME hnsSystemTime, float flRate);

    // IMFSampleGrabberSinkCallback methods
    STDMETHODIMP OnSetPresentationClock(IMFPresentationClock* pClock);
    STDMETHODIMP OnProcessSample(REFGUID guidMajorMediaType, DWORD dwSampleFlags,
      LONGLONG llSampleTime, LONGLONG llSampleDuration, const BYTE * pSampleBuffer,
      DWORD dwSampleSize);
    STDMETHODIMP OnShutdown();
  };

}

#endif