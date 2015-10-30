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
#include "MediaFoundationVideoDevice.h"
#include "MfVideoCaptureLoggerMacros.h"
#include <Strmif.h>
#include <mfapi.h>
#include <mfidl.h>
#include <mfobjects.h>
#include <string.h>

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
  MediaFoundationVideoDevice::MediaFoundationVideoDevice(void)
    : IsSetup(false)
    , LockOut(OpenLock)
    , FriendlyName(NULL)
    , Width(0)
    , Height(0)
    , FrameRate(0)
    , Source(NULL)
    , StopEventCallbackFunc(NULL)
    , DeviceIndex(-1)
    , UserData(NULL)
  {  

  }

  //----------------------------------------------------------------------------
  void MediaFoundationVideoDevice::SetParameters(CaptureDeviceParameters newParameters)
  {
    if(!this->IsSetup)
    {
      LOG_ERROR("MediaFoundationVideoDevice::SetParameters failed: device is not set up");
      return;
    }
    if(this->Source==NULL)
    {
      LOG_ERROR("MediaFoundationVideoDevice::SetParameters failed: invalid source");
      return;
    }

    IAMVideoProcAmp *pProcAmp = NULL;
    HRESULT hr = this->Source->QueryInterface(IID_PPV_ARGS(&pProcAmp));
    if (SUCCEEDED(hr))
    {
      for(unsigned int i = 0; i < CaptureDeviceParameters::NUMBER_OF_VIDEO_PROC_PARAMETERS; i++)
      {
        if(this->PreviousParameters.VideoProcParameters[i].CurrentValue != newParameters.VideoProcParameters[i].CurrentValue || this->PreviousParameters.VideoProcParameters[i].Flag != newParameters.VideoProcParameters[i].Flag)
        {
          hr = pProcAmp->Set(VideoProcAmp_Brightness + i, newParameters.VideoProcParameters[i].CurrentValue, newParameters.VideoProcParameters[i].Flag);
        }
      }
      pProcAmp->Release();
    }

    IAMCameraControl *pProcControl = NULL;
    hr = this->Source->QueryInterface(IID_PPV_ARGS(&pProcControl));
    if (SUCCEEDED(hr))
    {
      for(unsigned int i = 0; i < CaptureDeviceParameters::NUMBER_OF_CAMERA_CONTROL_PARAMETERS; i++)
      {
        if(this->PreviousParameters.CameraControlParameters[i].CurrentValue != newParameters.CameraControlParameters[i].CurrentValue || this->PreviousParameters.CameraControlParameters[i].Flag != newParameters.CameraControlParameters[i].Flag)
        {
          hr = pProcControl->Set(CameraControl_Pan+i, newParameters.CameraControlParameters[i].CurrentValue, newParameters.CameraControlParameters[i].Flag);          
        }
      }
      pProcControl->Release();
    }

    this->PreviousParameters = newParameters;
  }

  //----------------------------------------------------------------------------

  CaptureDeviceParameters MediaFoundationVideoDevice::GetParameters()
  {
    CaptureDeviceParameters out;

    if(!this->IsSetup)
    {
      LOG_ERROR("MediaFoundationVideoDevice::SetParameters failed: device is not set up");
      return out;
    }
    if(this->Source==NULL)
    {
      LOG_ERROR("MediaFoundationVideoDevice::SetParameters failed: invalid source");
      return out;
    }
    IAMVideoProcAmp *pProcAmp = NULL;
    HRESULT hr = this->Source->QueryInterface(IID_PPV_ARGS(&pProcAmp));
    if (SUCCEEDED(hr))
    {
      for(unsigned int i = 0; i < CaptureDeviceParameters::NUMBER_OF_VIDEO_PROC_PARAMETERS; i++)
      {
        Parameter temp;
        hr = pProcAmp->GetRange(VideoProcAmp_Brightness+i, &temp.Min, &temp.Max, &temp.Step, &temp.Default, &temp.Flag);
        if (SUCCEEDED(hr))
        {
          temp.CurrentValue = temp.Default;
          out.VideoProcParameters[i] = temp;
        }
      }
      pProcAmp->Release();
    }

    IAMCameraControl *pProcControl = NULL;
    hr = this->Source->QueryInterface(IID_PPV_ARGS(&pProcControl));

    if (SUCCEEDED(hr))
    {
      for(unsigned int i = 0; i < CaptureDeviceParameters::NUMBER_OF_CAMERA_CONTROL_PARAMETERS; i++)
      {
        Parameter temp;
        hr = pProcControl->GetRange(CameraControl_Pan+i, &temp.Min, &temp.Max, &temp.Step, &temp.Default, &temp.Flag);
        if (SUCCEEDED(hr))
        {
          temp.CurrentValue = temp.Default;
          out.CameraControlParameters[i] = temp;
        }
      }
      pProcControl->Release();
    }
    return out;
  }

  //----------------------------------------------------------------------------
  long MediaFoundationVideoDevice::ResetDevice(IMFActivate *pActivate)
  {
    HRESULT hr = -1;

    this->CurrentFormats.clear();

    if(this->FriendlyName)
    {
      CoTaskMemFree(this->FriendlyName);
    }
    this->FriendlyName = NULL;

    if(pActivate)
    {    
      hr = pActivate->GetAllocatedString(MF_DEVSOURCE_ATTRIBUTE_FRIENDLY_NAME, &this->FriendlyName, NULL);

      IMFMediaSource *pSource = NULL;
      hr = pActivate->ActivateObject(__uuidof(IMFMediaSource),(void**)&pSource);
      EnumerateCaptureFormats(pSource);

      SafeRelease(&pSource);

      if(FAILED(hr))  
      {      
        if(this->FriendlyName)
        {
          CoTaskMemFree(this->FriendlyName);
        }
        this->FriendlyName = NULL;
        LOG_ERROR("VIDEODEVICE " << this->DeviceIndex << ": IMFMediaSource interface cannot be created.");
      }
    }

    return hr;
  }

  //----------------------------------------------------------------------------
  long MediaFoundationVideoDevice::ReadDeviceInfo(IMFActivate *pActivate, unsigned int num)
  {
    this->DeviceIndex = num;
    HRESULT hr = ResetDevice(pActivate);
    return hr;
  }

  //----------------------------------------------------------------------------
  long MediaFoundationVideoDevice::CheckDevice(IMFAttributes *pAttributes, IMFActivate **pDevice)
  {
    HRESULT hr = S_OK;
    *pDevice = NULL;

    IMFActivate **ppDevices = NULL;
    UINT32 count=0;
    if (SUCCEEDED(hr))
    {
      hr = MFEnumDeviceSources(pAttributes, &ppDevices, &count);
      if (SUCCEEDED(hr))
      {
        if (count <= 0)
        {
          LOG_ERROR("MediaFoundationVideoDevice::CheckDevice failed: No devices available");
          hr = S_FALSE;
        }
        else if (this->DeviceIndex >= count)
        {
          LOG_ERROR("MediaFoundationVideoDevice::CheckDevice failed: Device index " << this->DeviceIndex << " is of current device is out of the range of availalbe devices (0.."<<count-1<<")");
          hr = S_FALSE;
        }
      }
      else
      {
        LOG_ERROR("MediaFoundationVideoDevice::CheckDevice failed: List of DeviceSources cannot be enumerated.");
      }
    }

    if (SUCCEEDED(hr))
    {
      wchar_t *newFriendlyName = NULL;
      hr = ppDevices[this->DeviceIndex]->GetAllocatedString(MF_DEVSOURCE_ATTRIBUTE_FRIENDLY_NAME, &newFriendlyName, NULL);
      if (SUCCEEDED(hr))
      {
        if(wcscmp(newFriendlyName, this->FriendlyName) != 0)
        {
          LOG_ERROR("MediaFoundationVideoDevice::CheckDevice failed: Device index " << this->DeviceIndex << " name is not found");
          hr = S_FALSE;
        }
      }
      else
      {
        LOG_ERROR("MediaFoundationVideoDevice::CheckDevice failed: Device index " << this->DeviceIndex << " name of device cannot be retrieved");
      }
    }

    if (SUCCEEDED(hr))
    {
      *pDevice = ppDevices[this->DeviceIndex];
      (*pDevice)->AddRef();
    }

    for(UINT32 i = 0; i < count; i++)
    {
      SafeRelease(&ppDevices[i]);
    }
    SafeRelease(ppDevices);
    return hr;
  }

  //----------------------------------------------------------------------------
  long MediaFoundationVideoDevice::InitDevice()
  {
    HRESULT hr = S_OK;
    IMFAttributes *pAttributes = NULL;
    IMFActivate * vd_pActivate = NULL;
    CoInitialize(NULL);

    if (SUCCEEDED(hr))
    {
      hr = MFCreateAttributes(&pAttributes, 1);
    }
    if (SUCCEEDED(hr))
    {
      hr = pAttributes->SetGUID(MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE,MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_GUID);
      if (!SUCCEEDED(hr))
      {
        LOG_ERROR("MediaFoundationVideoDevice::InitDevice failed: device  " << this->DeviceIndex << ": The attribute of the capture device cannot be retrieved");
      }
    }
    if (SUCCEEDED(hr))
    {
      hr = CheckDevice(pAttributes, &vd_pActivate);
      if (SUCCEEDED(hr) && vd_pActivate)
      {
        SafeRelease(&this->Source);
        hr = vd_pActivate->ActivateObject(__uuidof(IMFMediaSource), (void**)&this->Source);
        SafeRelease(&vd_pActivate);
      }
      else
      {
        LOG_ERROR("MediaFoundationVideoDevice::InitDevice failed: device  " << this->DeviceIndex << ": Cannot activate device");
      }
    }
    SafeRelease(&pAttributes);
    return hr;
  }
  //----------------------------------------------------------------------------
  int MediaFoundationVideoDevice::GetNumberOfStreams() const
  {
    return this->CurrentFormats.size();
  }

  //----------------------------------------------------------------------------
  MediaType MediaFoundationVideoDevice::GetFormat(unsigned int streamIndex, unsigned int formatIndex) const
  {
    if(streamIndex >= this->CurrentFormats.size())
    {
      return MediaType();
    }
    if (formatIndex >= this->CurrentFormats[streamIndex].size())
    {
      return MediaType();
    }
    return this->CurrentFormats[streamIndex][formatIndex];
  }

  //----------------------------------------------------------------------------
  unsigned int MediaFoundationVideoDevice::GetNumberOfFormats(unsigned int streamIndex) const
  {
    if(streamIndex >= this->CurrentFormats.size())
    {
      return 0;
    }
    return this->CurrentFormats[streamIndex].size();
  }

  //----------------------------------------------------------------------------
  void MediaFoundationVideoDevice::SetEmergencyStopEvent(void *userData, void(*func)(int, void *))
  {
    this->StopEventCallbackFunc = func;
    this->UserData = userData;
  }

  //----------------------------------------------------------------------------
  void MediaFoundationVideoDevice::CloseDevice()
  {    
    if(!this->IsSetup)
    {
      return;
    }
    this->IsSetup = false;
    Stop();
    SafeRelease(&this->Source);
    this->LockOut = OpenLock;
    LOG_DEBUG("MediaFoundationVideoDevice::CloseDevice: Device " << this->DeviceIndex << " is stopped");
  }

  //----------------------------------------------------------------------------
  bool MediaFoundationVideoDevice::Start()
  {
    if (!this->IsSetup)
    {
      LOG_ERROR("MediaFoundationVideoDevice::Start failed: not set up");
      return false;
    }
    if (this->Source == NULL)
    {
      LOG_ERROR("MediaFoundationVideoDevice::Start failed: invalid source");
      return false;
    }

    IMFPresentationDescriptor *pPD = NULL;
    HRESULT hr = this->Source->CreatePresentationDescriptor(&pPD);

    if (SUCCEEDED(hr))
    {
      this->Source->Start(pPD, NULL, NULL);
    }

    SafeRelease(&pPD);
    return SUCCEEDED(hr);
  }

  //----------------------------------------------------------------------------
  bool MediaFoundationVideoDevice::Stop()
  {
    if (!this->IsSetup)
    {
      LOG_ERROR("MediaFoundationVideoDevice::Stop failed: not set up");
      return false;
    }
    if (this->Source == NULL)
    {
      LOG_ERROR("MediaFoundationVideoDevice::Stop failed: invalid source");
      return false;
    }
    this->Source->Stop();
    return true;
  }

  //----------------------------------------------------------------------------
  unsigned int MediaFoundationVideoDevice::GetWidth() const
  {
    if (!this->IsSetup)
    {
      LOG_ERROR("MediaFoundationVideoDevice::GetWidth failed: not set up");
      return 0;
    }
    return this->Width;
  }

  //----------------------------------------------------------------------------
  unsigned int MediaFoundationVideoDevice::GetHeight() const
  {
    if (!this->IsSetup)
    {
      LOG_ERROR("MediaFoundationVideoDevice::GetHeight failed: not set up");
      return 0;
    }
    return this->Height;
  }

  //----------------------------------------------------------------------------
  unsigned int MediaFoundationVideoDevice::GetFrameRate() const
  {
    if (!this->IsSetup)
    {
      LOG_ERROR("MediaFoundationVideoDevice::GetFrameRate failed: not set up");
      return 0;
    }
    return this->FrameRate;
  }

  //----------------------------------------------------------------------------
  IMFMediaSource *MediaFoundationVideoDevice::GetMediaSource()
  {
    if(this->LockOut != OpenLock)
    {
      LOG_ERROR("MediaFoundationVideoDevice::GetMediaSource failed: locked");
      return NULL;
    }
    this->LockOut = MediaSourceLock;      
    return this->Source;
  }

  //----------------------------------------------------------------------------
  bool MediaFoundationVideoDevice::FindType(unsigned int& foundFormatIndex, unsigned int streamIndex, unsigned int w, unsigned int h, unsigned int frameRate, GUID subtype)
  {
    if (streamIndex >= this->CurrentFormats.size())
    {
      LOG_ERROR("MediaFoundationVideoDevice::FindType failed: stream "<<streamIndex<<" is not available (number of available streams: "<<this->CurrentFormats.size()<<")");
      return false;
    }
    if (frameRate==0)
    {
      // get the maximum available frame rate
      frameRate=10000;
    }

    std::wstring subtypeName = FormatReader::StringFromGUID(subtype);

    // find the smallest frame rate that is at least as high as the requested (otherwise get the highest available frame rate)
    int formatIndex=0;
    int bestFrameRateDifferenceComparedToRequested = -1-frameRate;
    int bestFormatIndex=-1;
    for (std::vector<MediaType>::iterator typeIt=this->CurrentFormats[streamIndex].begin(); typeIt!=this->CurrentFormats[streamIndex].end(); ++typeIt)
    {
      // Note: we do not check the stream type here. It may be possible to match the requested type via a SourceReader later.
      if (typeIt->width==w && typeIt->height==h)
      {
        int frameRateDifferenceComparedToRequested = (int)typeIt->MF_MT_FRAME_RATE-(int)frameRate;
        if (frameRateDifferenceComparedToRequested==0)
        {
          // the frame rate is matched exactly, we cannot get better than this
          foundFormatIndex = formatIndex;
          return true;
        }
        else if (frameRateDifferenceComparedToRequested>0)
        {
          // frame rate is higher than requested, if this one is closer, then use this
          if (frameRateDifferenceComparedToRequested<bestFrameRateDifferenceComparedToRequested || bestFrameRateDifferenceComparedToRequested<0)
          {
            bestFormatIndex = formatIndex;
            bestFrameRateDifferenceComparedToRequested = frameRateDifferenceComparedToRequested;
          }
        }
        else
        {
          // lower frame rate than needed, choose the highest frame rate
          if (bestFrameRateDifferenceComparedToRequested<0 && frameRateDifferenceComparedToRequested>bestFrameRateDifferenceComparedToRequested)
          {
            bestFormatIndex = formatIndex;
            bestFrameRateDifferenceComparedToRequested = frameRateDifferenceComparedToRequested;
          }
        }
      }
      formatIndex++;
    }
    if (bestFormatIndex<0)
    {
      foundFormatIndex=0;
      return false;
    }
    foundFormatIndex = bestFormatIndex;
    return true;
  }

  //----------------------------------------------------------------------------
  long MediaFoundationVideoDevice::SetDeviceFormat(IMFMediaSource *pSource, DWORD streamIndex, DWORD dwFormatIndex)
  {
    IMFPresentationDescriptor *pPD = NULL;
    IMFStreamDescriptor *pSD = NULL;
    IMFMediaTypeHandler *pHandler = NULL;
    IMFMediaType *pType = NULL;

    HRESULT hr = pSource->CreatePresentationDescriptor(&pPD);
    if (FAILED(hr))
    {
      goto done;
    }

    BOOL fSelected=false;
    hr = pPD->GetStreamDescriptorByIndex(streamIndex, &fSelected, &pSD);
    if (FAILED(hr))
    {
      goto done;
    }
    if (!fSelected)
    {
      hr = pPD->SelectStream(streamIndex);
      if (FAILED(hr))
      {
        LOG_ERROR("Failed to select stream "<<streamIndex);
        goto done;
      }
    }

    hr = pSD->GetMediaTypeHandler(&pHandler);
    if (FAILED(hr))
    {
      goto done;
    }

    hr = pHandler->GetMediaTypeByIndex(dwFormatIndex, &pType);
    if (FAILED(hr))
    {
      goto done;
    }

    hr = pHandler->SetCurrentMediaType(pType);

done:
    SafeRelease(&pPD);
    SafeRelease(&pSD);
    SafeRelease(&pHandler);
    SafeRelease(&pType);
    return hr;
  }

  //----------------------------------------------------------------------------
  bool MediaFoundationVideoDevice::IsDeviceSetup() const
  {
    return this->IsSetup;
  }

  //----------------------------------------------------------------------------
  bool MediaFoundationVideoDevice::IsDeviceMediaSource() const
  {
    return (this->LockOut == MediaSourceLock);
  }

  //----------------------------------------------------------------------------
  bool MediaFoundationVideoDevice::IsDeviceRawDataSource() const
  {
    return (this->LockOut == RawDataLock);
  }

  //----------------------------------------------------------------------------
  bool MediaFoundationVideoDevice::SetupDevice(unsigned int streamIndex, unsigned int formatIndex)
  {  
    if(this->IsSetup)
    {
      LOG_ERROR("MediaFoundationVideoDevice::SetupDevice failed: device " << this->DeviceIndex << " is already set up");
      return false;
    }

    HRESULT hr = InitDevice();
    if(FAILED(hr))
    {
      LOG_ERROR("MediaFoundationVideoDevice::SetupDevice failed: device " << this->DeviceIndex << " interface IMFMediaSource cannot be retrieved");
      return false;
    }

    this->Width = this->CurrentFormats[streamIndex][formatIndex].width; 
    this->Height = this->CurrentFormats[streamIndex][formatIndex].height;
    this->FrameRate = this->CurrentFormats[streamIndex][formatIndex].MF_MT_FRAME_RATE;
    hr = SetDeviceFormat(this->Source, (DWORD)streamIndex, (DWORD) formatIndex);
    this->IsSetup = (SUCCEEDED(hr));
    if(this->IsSetup)
    {
      LOG_DEBUG("MediaFoundationVideoDevice::SetupDevice: device " << this->DeviceIndex << " device is setup");
    }
    this->PreviousParameters = GetParameters();
    this->ActiveType = formatIndex;
    return this->IsSetup;
  }

  //----------------------------------------------------------------------------
  bool MediaFoundationVideoDevice::SetupDevice(unsigned int streamIndex, unsigned int w, unsigned int h, unsigned int idealFramerate, GUID subtype)
  {
    unsigned int formatIndex = 0;
    if(!FindType(formatIndex, streamIndex, w, h, idealFramerate, subtype))
    {
      LOG_ERROR("MediaFoundationVideoDevice::SetupDevice failed: device " << this->DeviceIndex << ": Cannot find the requested type");
      return false;
    }
    return SetupDevice(streamIndex, formatIndex);
  }

  //----------------------------------------------------------------------------
  wchar_t *MediaFoundationVideoDevice::GetName()
  {
    return this->FriendlyName;
  }

  //----------------------------------------------------------------------------
  MediaFoundationVideoDevice::~MediaFoundationVideoDevice(void)
  {    
    CloseDevice();
    SafeRelease(&this->Source);
    if(this->FriendlyName)
    {
      CoTaskMemFree(this->FriendlyName);
    }
    this->FriendlyName = NULL;
  }

  //----------------------------------------------------------------------------
  long MediaFoundationVideoDevice::EnumerateCaptureFormats(IMFMediaSource *pSource)
  {
    IMFPresentationDescriptor *pPD = NULL;
    IMFStreamDescriptor *pSD = NULL;
    IMFMediaTypeHandler *pHandler = NULL;

    HRESULT hr=-1;
    if (pSource==NULL)
    {
      goto done;
    }

    hr = pSource->CreatePresentationDescriptor(&pPD);
    if (FAILED(hr))
    {
      goto done;
    }

    DWORD descriptorCount = 0;
    hr = pPD->GetStreamDescriptorCount(&descriptorCount);
    if (FAILED(hr))
    {
      goto done;
    }

    for (DWORD streamIndex = 0; streamIndex<descriptorCount; streamIndex++)
    {
      std::vector<MediaType> formats;

      BOOL fSelected = false;
      hr = pPD->GetStreamDescriptorByIndex(streamIndex, &fSelected, &pSD);
      if (FAILED(hr))
      {
        goto done;
      }
      DWORD streamId=0;
      hr = pSD->GetStreamIdentifier(&streamId);
      if (FAILED(hr))
      {
        goto done;
      }
      hr = pSD->GetMediaTypeHandler(&pHandler);
      if (FAILED(hr))
      {
        goto done;
      }
      DWORD cTypes = 0;
      hr = pHandler->GetMediaTypeCount(&cTypes);
      if (FAILED(hr))
      {
        continue;
      }
      for (DWORD i = 0; i < cTypes; i++)
      {
        IMFMediaType *pType = NULL;
        hr = pHandler->GetMediaTypeByIndex(i, &pType);
        if (SUCCEEDED(hr))
        {
          MediaType mt = FormatReader::Read(pType);\
          mt.StreamId = streamId;
          formats.push_back(mt);
        }
        SafeRelease(&pType);
      }
      this->CurrentFormats.push_back(formats);
    }

done:
    SafeRelease(&pPD);
    SafeRelease(&pSD);
    SafeRelease(&pHandler);
    return hr;
  }

  //----------------------------------------------------------------------------
  unsigned int MediaFoundationVideoDevice::GetActiveType() const
  {
    return this->ActiveType;
  }
}