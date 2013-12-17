/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

/*=========================================================================
The following copyright notice is applicable to parts of this file:
Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
All rights reserved.
See Copyright.txt or http://www.kitware.com/Copyright.htm for details.
Authors include: Adam Rankin
=========================================================================*/ 

#include "PixelCodec.h"
#include "PlusConfigure.h"
#include "vtkMmfVideoSource.h"
#include "vtkObjectFactory.h"
#include "vtkPlusBuffer.h"
#include "vtkPlusChannel.h"
#include "vtkPlusDataSource.h"

#include "MediaFoundationVideoCaptureApi.h"
#include "FormatReader.h"

// Media foundation includes - require Microsoft Windows SDK 7.1 or later.
// Download from: http://www.microsoft.com/en-us/download/details.aspx?id=8279
#include <Mfapi.h>

// Windows includes
#include <shlwapi.h>

//----------------------------------------------------------------------------

namespace
{
  const int DEFAULT_X_RESOLUTION = 640;
  const int DEFAULT_Y_RESOLUTION = 480;

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

vtkCxxRevisionMacro(vtkMmfVideoSource, "$Revision: 1.0 $");
vtkStandardNewMacro(vtkMmfVideoSource);

//----------------------------------------------------------------------------
vtkMmfVideoSource::vtkMmfVideoSource()
: FrameIndex(0)
, CaptureSource(NULL)
, CaptureSourceReader(NULL)
, Mutex(vtkSmartPointer<vtkRecursiveCriticalSection>::New())
{
  this->RequireAcquisitionRateInDeviceSetConfiguration = false;
  this->RequireAveragedItemsForFilteringInDeviceSetConfiguration = false;
  this->RequireFrameBufferSizeInDeviceSetConfiguration = true;
  this->RequireImageOrientationInConfiguration = true;
  this->RequireLocalTimeOffsetSecInDeviceSetConfiguration = false;
  this->RequireRfElementInDeviceSetConfiguration = false;
  this->RequireUsImageOrientationInDeviceSetConfiguration = true;

  this->AcquisitionRate = vtkPlusDevice::VIRTUAL_DEVICE_FRAME_RATE;
}

//----------------------------------------------------------------------------
vtkMmfVideoSource::~vtkMmfVideoSource()
{ 

}


//----------------------------------------------------------------------------
void vtkMmfVideoSource::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);

  os << indent << "FrameIndex: " << (this->FrameIndex ? "On\n" : "Off\n");
}

//----------------------------------------------------------------------------
PlusStatus vtkMmfVideoSource::InternalConnect()
{
  this->ActiveVideoFormat.Width = this->RequestedVideoFormat.Width;
  this->ActiveVideoFormat.Height = this->RequestedVideoFormat.Height;
  this->ActiveVideoFormat.PixelFormat = this->RequestedVideoFormat.PixelFormat;
  this->ActiveVideoFormat.PixelFormatName = this->RequestedVideoFormat.PixelFormatName;

  if( !MfVideoCapture::MediaFoundationVideoCaptureApi::GetInstance().SetupDevice(0, 
    this->RequestedVideoFormat.Width,
    this->RequestedVideoFormat.Height,
    this->AcquisitionRate) )
  {
    if( !MfVideoCapture::MediaFoundationVideoCaptureApi::GetInstance().SetupDevice(0, DEFAULT_X_RESOLUTION, DEFAULT_Y_RESOLUTION, 50) )
    {
      LOG_ERROR("Unable to initialize capture device with default details.");
      return PLUS_FAIL;
    }
    LOG_WARNING("Unable to init capture device with requested details. Backing up to defaults.");

    this->ActiveVideoFormat.Width = DEFAULT_X_RESOLUTION;
    this->ActiveVideoFormat.Height = DEFAULT_Y_RESOLUTION;
    this->ActiveVideoFormat.PixelFormat = MFVideoFormat_YUY2;
    this->ActiveVideoFormat.PixelFormatName = "MFVideoFormat_YUY2";
  }

  this->CaptureSource = MfVideoCapture::MediaFoundationVideoCaptureApi::GetInstance().GetMediaSource(0);

  this->FrameIndex = 0;

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkMmfVideoSource::InternalDisconnect()
{
  SafeRelease(&this->CaptureSourceReader);

  MfVideoCapture::MediaFoundationVideoCaptureApi::GetInstance().CloseAllDevices();

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkMmfVideoSource::InternalStartRecording()
{
  HRESULT hr;
  if( this->CaptureSource != NULL )
  {
    IMFAttributes* attr;
    MFCreateAttributes(&attr, 2);
    hr = attr->SetUnknown(MF_SOURCE_READER_ASYNC_CALLBACK, this);
    hr = attr->SetUINT32(MF_READWRITE_DISABLE_CONVERTERS, TRUE);

    hr = MFCreateSourceReaderFromMediaSource(this->CaptureSource, attr, &this->CaptureSourceReader);

    this->UpdateFrameSize();

    attr->Release();

    this->CaptureSourceReader->ReadSample(MF_SOURCE_READER_FIRST_VIDEO_STREAM, 0, NULL, NULL, NULL, NULL);
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkMmfVideoSource::InternalStopRecording()
{
  if( this->CaptureSource != NULL )
  {
    this->CaptureSource->Stop();
  }

  return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------

PlusStatus vtkMmfVideoSource::NotifyConfigured()
{
  if( this->OutputChannels.size() > 1 )
  {
    LOG_WARNING("vtkMmfVideoSource is expecting one output channel and there are " << this->OutputChannels.size() << " channels. First output channel will be used.");
    return PLUS_FAIL;
  }

  if( this->OutputChannels.size() == 0 )
  {
    LOG_ERROR("No output channels defined for microsoft media foundation video source. Cannot proceed." );
    this->CorrectlyConfigured = false;
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------

STDMETHODIMP vtkMmfVideoSource::QueryInterface( REFIID iid, void** ppv )
{
  static const QITAB qit[] =
  {
    QITABENT(vtkMmfVideoSource, IMFSourceReaderCallback),
    { 0 },
  };
  return QISearch(this, qit, iid, ppv);
}

//----------------------------------------------------------------------------

STDMETHODIMP_(ULONG) vtkMmfVideoSource::AddRef()
{
  return InterlockedIncrement(&RefCount);
}

//----------------------------------------------------------------------------

STDMETHODIMP_(ULONG) vtkMmfVideoSource::Release()
{
  ULONG uCount = InterlockedDecrement(&RefCount);
  if (uCount == 0)
  {
    delete this;
  }
  return uCount;
}

//----------------------------------------------------------------------------

STDMETHODIMP vtkMmfVideoSource::OnEvent( DWORD, IMFMediaEvent * )
{
  return S_OK;
}

//----------------------------------------------------------------------------

STDMETHODIMP vtkMmfVideoSource::OnFlush( DWORD )
{
  return S_OK;
}

//----------------------------------------------------------------------------

STDMETHODIMP vtkMmfVideoSource::OnReadSample( HRESULT hrStatus, DWORD dwStreamIndex, DWORD dwStreamFlags, LONGLONG llTimestamp, IMFSample *pSample )
{
  PlusLockGuard<vtkRecursiveCriticalSection> updateMutexGuardedLock(this->Mutex);

  if (SUCCEEDED(hrStatus))
  {
    if (pSample)
    {
      IMFMediaBuffer* aBuffer;
      DWORD bufferCount;
      pSample->GetBufferCount(&bufferCount);
      if( bufferCount < 1 )
      {
        LOG_ERROR("No buffer available in the sample.");
        return S_FALSE;
      }
      pSample->GetBufferByIndex(0, &aBuffer);
      BYTE* bufferData;
      DWORD maxLength;
      DWORD currentLength;

      // Get the media type from the stream.
      IMFMediaType* pType;
      this->CaptureSourceReader->GetCurrentMediaType((DWORD)MF_SOURCE_READER_FIRST_VIDEO_STREAM, 
        &pType);

      UINT32 width, height;
      MFGetAttributeSize(pType, MF_MT_FRAME_SIZE, &width, &height);

      HRESULT hr = aBuffer->Lock(&bufferData, &maxLength, &currentLength);
      if( FAILED(hr) ) 
      {
        LOG_ERROR("Unable to lock the buffer.");
      }
      else
      {
        int frameSize[2];
        vtkPlusDataSource* videoSource(NULL);
        if( this->GetFirstActiveVideoSource(videoSource) != PLUS_SUCCESS )
        {
          return S_FALSE;
        }
        videoSource->GetBuffer()->GetFrameSize(frameSize);
        if (PixelCodec::ConvertToGray(VTK_BI_YUY2, frameSize[0], frameSize[1], bufferData, (unsigned char*)this->UncompressedVideoFrame.GetBufferPointer() ) != PLUS_SUCCESS)
        {
          LOG_ERROR("Error while decoding the grabbed image");
          return PLUS_FAIL;
        }

        this->FrameIndex++;
        vtkPlusDataSource* aSource(NULL);
        if( this->GetFirstActiveVideoSource(aSource) != PLUS_SUCCESS )
        {
          LOG_ERROR("Unable to retrieve the video source in the media foundation capture device.");
          return PLUS_FAIL;
        }
        PlusStatus status = aSource->GetBuffer()->AddItem(&this->UncompressedVideoFrame, this->FrameIndex); 

        this->Modified();
      }

      aBuffer->Unlock();
      SafeRelease(&aBuffer);
    }
  }
  else
  {
    // Streaming error.
    LOG_ERROR("Source Reader error: " << std::hex << hrStatus);
  }

  if (MF_SOURCE_READERF_ENDOFSTREAM & dwStreamFlags)
  {
    // Reached the end of the stream.
    LOG_ERROR("End of stream reached. Capture device should never reach end of stream.");
    this->Disconnect();
    return S_FALSE;
    // This should never occur under normal operation.
  }

  if( CaptureSourceReader == NULL )
  {
    return S_FALSE;
  }
  this->CaptureSourceReader->ReadSample(MF_SOURCE_READER_FIRST_VIDEO_STREAM, 0, NULL, NULL, NULL, NULL);

  return S_OK;
}

//----------------------------------------------------------------------------

PlusStatus vtkMmfVideoSource::UpdateFrameSize()
{
  if( this->CaptureSourceReader != NULL )
  {
    int currentFrameSize[2] = {0,0};
    vtkPlusDataSource* videoSource(NULL);
    this->GetFirstActiveVideoSource(videoSource);
    videoSource->GetBuffer()->GetFrameSize(currentFrameSize);
    if( currentFrameSize[0] != this->ActiveVideoFormat.Width || currentFrameSize[1] != this->ActiveVideoFormat.Height )
    {
      currentFrameSize[0] = this->ActiveVideoFormat.Width;
      currentFrameSize[1] = this->ActiveVideoFormat.Height;
      videoSource->GetBuffer()->SetFrameSize(currentFrameSize);
      PlusCommon::ITKScalarPixelType pixelType = itk::ImageIOBase::UCHAR; // always convert output to 8-bit grayscale
      this->UncompressedVideoFrame.AllocateFrame(currentFrameSize, pixelType);
    }
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------

PlusStatus vtkMmfVideoSource::ReadConfiguration( vtkXMLDataElement* rootXmlElement )
{
  // Read superclass configuration
  Superclass::ReadConfiguration(rootXmlElement); 

  if ( rootXmlElement == NULL ) 
  {
    LOG_WARNING("Unable to find MmfVideoSource XML data element");
    return PLUS_FAIL; 
  }

  vtkXMLDataElement* deviceConfig = this->FindThisDeviceElement(rootXmlElement);
  if (deviceConfig == NULL) 
  {
    LOG_ERROR("Cannot find MmfVideoSource element in XML tree!");
    return PLUS_FAIL;
  }

  int xRes = DEFAULT_X_RESOLUTION; int yRes = DEFAULT_Y_RESOLUTION; 
  if( deviceConfig->GetAttribute("FrameSize") != NULL )
  {
    std::vector<std::string> frameSize;
    PlusCommon::SplitStringIntoTokens(deviceConfig->GetAttribute("FrameSize"), 'x', frameSize);
    if( frameSize.size() != 2 )
    {
      LOG_ERROR("Unable to parse FrameSize.");
    }
    else
    {
      std::stringstream xResolution(frameSize[0]);
      xResolution >> xRes;
      std::stringstream yResolution(frameSize[1]);
      yResolution >> yRes;
    }
  }
  
  this->RequestedVideoFormat.Width = xRes;
  this->RequestedVideoFormat.Height = yRes;

  if( deviceConfig->GetAttribute("VideoFormat") != NULL )
  {
    std::string pixAttrStr(deviceConfig->GetAttribute("VideoFormat"));
    std::string videoFormat = "MFVideoFormat_";
    videoFormat.append(pixAttrStr);
    std::wstring videoFormatWStr(videoFormat.begin(), videoFormat.end());
    GUID pixelFormat = MfVideoCapture::FormatReader::GUIDFromString(videoFormatWStr);
    if( pixelFormat == GUID_NULL)
    {
      LOG_ERROR("Cannot recognize requested pixel format. Defaulting to \'YUY2\'.");
      this->RequestedVideoFormat.PixelFormat = MFVideoFormat_YUY2;
      this->RequestedVideoFormat.PixelFormatName = "MFVideoFormat_YUY2";
    }
    else
    {
      this->RequestedVideoFormat.PixelFormat = pixelFormat;
      this->RequestedVideoFormat.PixelFormatName = videoFormat;
    }
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------

PlusStatus vtkMmfVideoSource::WriteConfiguration( vtkXMLDataElement* rootXmlElement )
{
  // Write superclass configuration
  Superclass::WriteConfiguration(rootXmlElement); 

  if ( rootXmlElement == NULL ) 
  {
    LOG_WARNING("Unable to find MmfVideoSource XML data element");
    return PLUS_FAIL; 
  }

  vtkXMLDataElement* deviceConfig = this->FindThisDeviceElement(rootXmlElement);
  if (deviceConfig == NULL) 
  {
    LOG_ERROR("Cannot find MmfVideoSource element in XML tree!");
    return PLUS_FAIL;
  }

  deviceConfig->SetIntAttribute("XResolution", this->RequestedVideoFormat.Width);
  deviceConfig->SetIntAttribute("YResolution", this->RequestedVideoFormat.Height);

  return PLUS_SUCCESS;
}