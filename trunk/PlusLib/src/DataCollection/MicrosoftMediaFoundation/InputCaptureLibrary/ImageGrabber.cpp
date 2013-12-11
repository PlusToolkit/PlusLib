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

#include "FormatReader.h"
#include "ImageGrabber.h"
#include "PlusCommon.h"
#include "RawImage.h"
#include <Shlwapi.h>
#include <mfapi.h>
#include <mfidl.h>
#include <mfobjects.h>
#include <new>
#include <stdio.h>
#include <windows.h>

//----------------------------------------------------------------------------

#define CHECK_HR(x) if (FAILED(x)) { goto done; }

//----------------------------------------------------------------------------

namespace
{
  template <class T> void SafeRelease(T **ppT)
  {
    if (*ppT)
    {
      (*ppT)->Release();
      *ppT = NULL;
    }
  }
}

//----------------------------------------------------------------------------

namespace MfVideoCapture
{
  //----------------------------------------------------------------------------

  ImageGrabber::ImageGrabber(unsigned int deviceID): m_cRef(1), ig_DeviceID(deviceID), ig_pSource(NULL), ig_pSession(NULL), ig_pTopology(NULL), ig_RIE(true), ig_Close(false)
  {
  }

  //----------------------------------------------------------------------------

  ImageGrabber::~ImageGrabber(void)
  {
    if (ig_pSession)
    {
      ig_pSession->Shutdown();
    }

    LOG_DEBUG("IMAGEGRABBER VIDEODEVICE " << ig_DeviceID << ": Destroying instance of the ImageGrabber class.");
  }

  //----------------------------------------------------------------------------

  HRESULT ImageGrabber::initImageGrabber(IMFMediaSource *pSource, GUID VideoFormat)
  {
    IMFActivate *pSinkActivate = NULL;
    IMFMediaType *pType = NULL;

    IMFPresentationDescriptor *pPD = NULL;
    IMFStreamDescriptor *pSD = NULL;
    IMFMediaTypeHandler *pHandler = NULL;
    IMFMediaType *pCurrentType = NULL;

    HRESULT hr = S_OK;
    MediaType MT;

    // Clean up.
    if (ig_pSession)
    {
      ig_pSession->Shutdown();
    }

    SafeRelease(&ig_pSession);
    SafeRelease(&ig_pTopology);

    ig_pSource = pSource;

    hr = pSource->CreatePresentationDescriptor(&pPD);
    if (FAILED(hr))
    {
      goto err;
    }

    BOOL fSelected;
    hr = pPD->GetStreamDescriptorByIndex(0, &fSelected, &pSD);
    if (FAILED(hr))
    {
      goto err;
    }

    hr = pSD->GetMediaTypeHandler(&pHandler);
    if (FAILED(hr))
    {
      goto err;
    }

    DWORD cTypes = 0;
    hr = pHandler->GetMediaTypeCount(&cTypes);
    if (FAILED(hr))
    {
      goto err;
    }

    if(cTypes > 0)
    {
      hr = pHandler->GetCurrentMediaType(&pCurrentType);

      if (FAILED(hr))
      {
        goto err;
      }

      MT = FormatReader::Read(pCurrentType);
    }

err:
    SafeRelease(&pPD);
    SafeRelease(&pSD);
    SafeRelease(&pHandler);
    SafeRelease(&pCurrentType);

    unsigned int sizeRawImage = 0;

    if(VideoFormat == MFVideoFormat_RGB24)
    {
      sizeRawImage = MT.MF_MT_FRAME_SIZE * 3;
    }
    else
      if(VideoFormat == MFVideoFormat_RGB32)
      {
        sizeRawImage = MT.MF_MT_FRAME_SIZE * 4;
      }

      CHECK_HR(hr = RawImage::CreateInstance(&ig_RIFirst, sizeRawImage));

      CHECK_HR(hr = RawImage::CreateInstance(&ig_RISecond, sizeRawImage));

      ig_RIOut = ig_RISecond;


      // Configure the media type that the Sample Grabber will receive.
      // Setting the major and subtype is usually enough for the topology loader
      // to resolve the topology.

      CHECK_HR(hr = MFCreateMediaType(&pType));
      CHECK_HR(hr = pType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video));
      CHECK_HR(hr = pType->SetGUID(MF_MT_SUBTYPE, VideoFormat));

      // Create the sample grabber sink.
      CHECK_HR(hr = MFCreateSampleGrabberSinkActivate(pType, this, &pSinkActivate));

      // To run as fast as possible, set this attribute (requires Windows 7):
      CHECK_HR(hr = pSinkActivate->SetUINT32(MF_SAMPLEGRABBERSINK_IGNORE_CLOCK, TRUE));

      // Create the Media Session.

      CHECK_HR(hr = MFCreateMediaSession(NULL, &ig_pSession));

      // Create the topology.
      CHECK_HR(hr = CreateTopology(pSource, pSinkActivate, &ig_pTopology));

done:

      // Clean up.
      if (FAILED(hr))
      {		
        if (ig_pSession)
        {
          ig_pSession->Shutdown();
        }

        SafeRelease(&ig_pSession);
        SafeRelease(&ig_pTopology);
      }

      SafeRelease(&pSinkActivate);
      SafeRelease(&pType);

      return hr;
  }

  //----------------------------------------------------------------------------

  void ImageGrabber::stopGrabbing()
  {
    if(ig_pSession)
      ig_pSession->Stop();

LOG_DEBUG("IMAGEGRABBER VIDEODEVICE " << ig_DeviceID << ": Stopping of of grabbing of images.");
  }

  //----------------------------------------------------------------------------

  HRESULT ImageGrabber::startGrabbing(void)
  {
    IMFMediaEvent *pEvent = NULL;

    PROPVARIANT var;
    PropVariantInit(&var);

    HRESULT hr = S_OK;
    CHECK_HR(hr = ig_pSession->SetTopology(0, ig_pTopology));
    CHECK_HR(hr = ig_pSession->Start(&GUID_NULL, &var));

    LOG_DEBUG("IMAGEGRABBER VIDEODEVICE " << ig_DeviceID << ": Start Grabbing of the images.");

    while (1)
    {

      HRESULT hrStatus = S_OK;
      MediaEventType met;

      if(!ig_pSession) break;
      hr = ig_pSession->GetEvent(0, &pEvent);
      if(!SUCCEEDED(hr))
      {
        hr = S_OK;

        goto done;
      }

      hr = pEvent->GetStatus(&hrStatus);
      if(!SUCCEEDED(hr))
      {
        hr = S_OK;

        goto done;
      }

      hr = pEvent->GetType(&met);
      if(!SUCCEEDED(hr))
      {
        hr = S_OK;

        goto done;
      }

      if (met == MESessionEnded)
      {			
        LOG_DEBUG("IMAGEGRABBER VIDEODEVICE " << ig_DeviceID << ": MESessionEnded.");

        ig_pSession->Stop();

        break;
      }

      if (met == MESessionStopped)
      {
        LOG_DEBUG("IMAGEGRABBER VIDEODEVICE " << ig_DeviceID << ": MESessionStopped.");

        break;
      }

      SafeRelease(&pEvent);
    }

    LOG_DEBUG("IMAGEGRABBER VIDEODEVICE " << ig_DeviceID << ": Finish startGrabbing.");

done:
    SafeRelease(&pEvent);
    SafeRelease(&ig_pSession);
    SafeRelease(&ig_pTopology);
    return hr;
  }

  //----------------------------------------------------------------------------

  HRESULT ImageGrabber::CreateTopology(IMFMediaSource *pSource, IMFActivate *pSinkActivate, IMFTopology **ppTopo)
  {
    IMFTopology *pTopology = NULL;
    IMFPresentationDescriptor *pPD = NULL;
    IMFStreamDescriptor *pSD = NULL;
    IMFMediaTypeHandler *pHandler = NULL;
    IMFTopologyNode *pNode1 = NULL;
    IMFTopologyNode *pNode2 = NULL;

    HRESULT hr = S_OK;
    DWORD cStreams = 0;

    CHECK_HR(hr = MFCreateTopology(&pTopology));
    CHECK_HR(hr = pSource->CreatePresentationDescriptor(&pPD));
    CHECK_HR(hr = pPD->GetStreamDescriptorCount(&cStreams));

    for (DWORD i = 0; i < cStreams; i++)
    {
      // In this example, we look for audio streams and connect them to the sink.

      BOOL fSelected = FALSE;
      GUID majorType;

      CHECK_HR(hr = pPD->GetStreamDescriptorByIndex(i, &fSelected, &pSD));
      CHECK_HR(hr = pSD->GetMediaTypeHandler(&pHandler));
      CHECK_HR(hr = pHandler->GetMajorType(&majorType));

      if (majorType == MFMediaType_Video && fSelected)
      {
        CHECK_HR(hr = AddSourceNode(pTopology, pSource, pPD, pSD, &pNode1));
        CHECK_HR(hr = AddOutputNode(pTopology, pSinkActivate, 0, &pNode2));
        CHECK_HR(hr = pNode1->ConnectOutput(0, pNode2, 0));
        break;
      }
      else
      {
        CHECK_HR(hr = pPD->DeselectStream(i));
      }
      SafeRelease(&pSD);
      SafeRelease(&pHandler);
    }

    *ppTopo = pTopology;
    (*ppTopo)->AddRef();

done:
    SafeRelease(&pTopology);
    SafeRelease(&pNode1);
    SafeRelease(&pNode2);
    SafeRelease(&pPD);
    SafeRelease(&pSD);
    SafeRelease(&pHandler);
    return hr;
  }

  //----------------------------------------------------------------------------

  HRESULT ImageGrabber::AddSourceNode(
    IMFTopology *pTopology,           // Topology.
    IMFMediaSource *pSource,          // Media source.
    IMFPresentationDescriptor *pPD,   // Presentation descriptor.
    IMFStreamDescriptor *pSD,         // Stream descriptor.
    IMFTopologyNode **ppNode)         // Receives the node pointer.
  {
    IMFTopologyNode *pNode = NULL;

    HRESULT hr = S_OK;
    CHECK_HR(hr = MFCreateTopologyNode(MF_TOPOLOGY_SOURCESTREAM_NODE, &pNode));
    CHECK_HR(hr = pNode->SetUnknown(MF_TOPONODE_SOURCE, pSource));
    CHECK_HR(hr = pNode->SetUnknown(MF_TOPONODE_PRESENTATION_DESCRIPTOR, pPD));
    CHECK_HR(hr = pNode->SetUnknown(MF_TOPONODE_STREAM_DESCRIPTOR, pSD));
    CHECK_HR(hr = pTopology->AddNode(pNode));

    // Return the pointer to the caller.
    *ppNode = pNode;
    (*ppNode)->AddRef();

done:
    SafeRelease(&pNode);
    return hr;
  }

  //----------------------------------------------------------------------------

  HRESULT ImageGrabber::AddOutputNode(
    IMFTopology *pTopology,     // Topology.
    IMFActivate *pActivate,     // Media sink activation object.
    DWORD dwId,                 // Identifier of the stream sink.
    IMFTopologyNode **ppNode)   // Receives the node pointer.
  {
    IMFTopologyNode *pNode = NULL;

    HRESULT hr = S_OK;
    CHECK_HR(hr = MFCreateTopologyNode(MF_TOPOLOGY_OUTPUT_NODE, &pNode));
    CHECK_HR(hr = pNode->SetObject(pActivate));
    CHECK_HR(hr = pNode->SetUINT32(MF_TOPONODE_STREAMID, dwId));
    CHECK_HR(hr = pNode->SetUINT32(MF_TOPONODE_NOSHUTDOWN_ON_REMOVE, FALSE));
    CHECK_HR(hr = pTopology->AddNode(pNode));

    // Return the pointer to the caller.
    *ppNode = pNode;
    (*ppNode)->AddRef();

done:
    SafeRelease(&pNode);
    return hr;
  }

  //----------------------------------------------------------------------------

  HRESULT ImageGrabber::CreateInstance(ImageGrabber **ppIG, unsigned int deviceID)
  {
    *ppIG = new (std::nothrow) ImageGrabber(deviceID);

    if (ppIG == NULL)
    {
      return E_OUTOFMEMORY;
    }

    LOG_DEBUG("IMAGEGRABBER VIDEODEVICE " << deviceID << ": Creating instance of ImageGrabber.");

    return S_OK;
  }

  //----------------------------------------------------------------------------

  STDMETHODIMP ImageGrabber::QueryInterface(REFIID riid, void** ppv)
  {
    // Creation tab of shifting interfaces from start of this class
    static const QITAB qit[] = 
    {

      QITABENT(ImageGrabber, IMFSampleGrabberSinkCallback),
      QITABENT(ImageGrabber, IMFClockStateSink),
      { 0 }
    };
    return QISearch(this, qit, riid, ppv);
  }

  //----------------------------------------------------------------------------

  STDMETHODIMP_(ULONG) ImageGrabber::AddRef()
  {
    return InterlockedIncrement(&m_cRef);
  }

  //----------------------------------------------------------------------------

  STDMETHODIMP_(ULONG) ImageGrabber::Release()
  {
    ULONG cRef = InterlockedDecrement(&m_cRef);
    if (cRef == 0)
    {
      delete this;
    }
    return cRef;
  }

  //----------------------------------------------------------------------------

  STDMETHODIMP ImageGrabber::OnClockStart(MFTIME hnsSystemTime, LONGLONG llClockStartOffset)
  {
    return S_OK;
  }

  //----------------------------------------------------------------------------

  STDMETHODIMP ImageGrabber::OnClockStop(MFTIME hnsSystemTime)
  {
    return S_OK;
  }

  //----------------------------------------------------------------------------

  STDMETHODIMP ImageGrabber::OnClockPause(MFTIME hnsSystemTime)
  {
    return S_OK;
  }

  //----------------------------------------------------------------------------

  STDMETHODIMP ImageGrabber::OnClockRestart(MFTIME hnsSystemTime)
  {
    return S_OK;
  }

  //----------------------------------------------------------------------------

  STDMETHODIMP ImageGrabber::OnClockSetRate(MFTIME hnsSystemTime, float flRate)
  {
    return S_OK;
  }

  //----------------------------------------------------------------------------

  STDMETHODIMP ImageGrabber::OnSetPresentationClock(IMFPresentationClock* pClock)
  {
    return S_OK;
  }

  //----------------------------------------------------------------------------

  STDMETHODIMP ImageGrabber::OnProcessSample(REFGUID guidMajorMediaType, DWORD dwSampleFlags,
    LONGLONG llSampleTime, LONGLONG llSampleDuration, const BYTE * pSampleBuffer,
    DWORD dwSampleSize)
  {	
    if(ig_RIE)
    {
      ig_RIFirst->fastCopy(pSampleBuffer);

      ig_RIOut = ig_RIFirst;
    }
    else
    {
      ig_RISecond->fastCopy(pSampleBuffer);

      ig_RIOut = ig_RISecond;
    }

    ig_RIE = !ig_RIE;


    return S_OK;
  }

  //----------------------------------------------------------------------------

  STDMETHODIMP ImageGrabber::OnShutdown()
  {
    return S_OK;
  }

  //----------------------------------------------------------------------------

  RawImage *ImageGrabber::getRawImage()
  {
    return ig_RIOut;
  }
}