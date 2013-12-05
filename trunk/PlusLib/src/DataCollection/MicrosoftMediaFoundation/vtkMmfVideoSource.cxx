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
Authors include: Danielle Pace
=========================================================================*/ 

#include "PixelCodec.h"
#include "PlusConfigure.h"
#include "vtkMMFVideoSource.h"
#include "vtkObjectFactory.h"
#include "vtkPlusBuffer.h"
#include "vtkPlusChannel.h"
#include "vtkPlusDataSource.h"

// Media Foundation Includes
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
, CaptureDevices(NULL)
, CaptureAttributes(NULL)
, CaptureSource(NULL)
, CaptureSourceReader(NULL)
, DeviceCount(-1)
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
  HRESULT hr = MFStartup(MF_VERSION);
  if( FAILED(hr) )
  {
    LOG_ERROR("Unable to initialize the media founation framework. Cannot create capture device.");
    return PLUS_FAIL;
  }

  // Create an attribute store to specify the enumeration parameters.
  hr = MFCreateAttributes(&this->CaptureAttributes, 2);
  if (FAILED(hr))
  {
    return PLUS_FAIL;
  }

  // Source type: video capture devices
  hr = this->CaptureAttributes->SetGUID(
    MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE, 
    MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_GUID
    );
  if (FAILED(hr))
  {
    return PLUS_FAIL;
  }

  // Enumerate devices.
  hr = MFEnumDeviceSources(this->CaptureAttributes, &this->CaptureDevices, &DeviceCount);
  if (FAILED(hr))
  {
    return PLUS_FAIL;
  }

  if (DeviceCount == 0)
  {
    hr = E_FAIL;
    return PLUS_FAIL;
  }

  // Create the media source object.
  hr = CaptureDevices[0]->ActivateObject(IID_PPV_ARGS(&this->CaptureSource));
  if (FAILED(hr))
  {
    return PLUS_FAIL;
  }

  (*CaptureDevices)->AddRef();

  this->FrameIndex = 0;

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkMmfVideoSource::InternalDisconnect()
{
  SafeRelease(&this->CaptureSource);
  SafeRelease(&this->CaptureSourceReader);
  SafeRelease(&this->CaptureAttributes);

  for (DWORD i = 0; i < DeviceCount; i++)
  {
    SafeRelease(&this->CaptureDevices[i]);
  }
  CoTaskMemFree(CaptureDevices);

  MFShutdown();

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkMmfVideoSource::InternalStartRecording()
{
  HRESULT hr;
  if( this->CaptureSource != NULL )
  {
    hr = this->CaptureAttributes->SetUnknown(MF_SOURCE_READER_ASYNC_CALLBACK, this);
    hr = this->CaptureAttributes->SetUINT32(MF_READWRITE_DISABLE_CONVERTERS, TRUE);

    hr = MFCreateSourceReaderFromMediaSource(this->CaptureSource, this->CaptureAttributes, &this->CaptureSourceReader);

    if( FAILED(hr) )
    {
      LOG_ERROR("Unable to create media source reader from the capture source.");
      this->StopRecording();
      this->Disconnect();
      return PLUS_FAIL;
    }

    if( ConfigureDecoder() != PLUS_SUCCESS )
    {
      this->StopRecording();
      this->Disconnect();
      return PLUS_FAIL;
    }

    this->UpdateFrameSize();

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
PlusStatus vtkMmfVideoSource::VideoFormatDialog()
{
  /* Configure video format parameters via a windows provided dialog */
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkMmfVideoSource::VideoSourceDialog()
{
  /* Configure capture device choice via a windows provided dialog */
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------

PlusStatus vtkMmfVideoSource::NotifyConfigured()
{
  if( this->OutputChannels.size() > 1 )
  {
    LOG_WARNING("vtkMMFVideoSource is expecting one output channel and there are " << this->OutputChannels.size() << " channels. First output channel will be used.");
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
      pSample->GetBufferByIndex(0, &aBuffer);
      BYTE* bufferData;
      DWORD maxLength;
      DWORD currentLength;

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

PlusStatus vtkMmfVideoSource::ConfigureDecoder()
{
  IMFMediaType *pNativeType = NULL;
  IMFMediaType *pType = NULL;

  // Find the native format of the stream.
  HRESULT hr = this->CaptureSourceReader->GetNativeMediaType(MF_SOURCE_READER_FIRST_VIDEO_STREAM, 0, &pNativeType);
  if (FAILED(hr))
  {
    return PLUS_FAIL;
  }

  // Query the native type for information
  hr = pNativeType->GetUINT32(MF_MT_DEFAULT_STRIDE, &this->ImageStride);
  if( FAILED(hr) )
  {
    // Default stride not populated, try to calculate
    GUID subtype = GUID_NULL;

    // Get the subtype and the image size.
    hr = pNativeType->GetGUID(MF_MT_SUBTYPE, &subtype);
    if (FAILED(hr))
    {
      return PLUS_FAIL;
    }

    vtkPlusDataSource* videoSource(NULL);
    if( this->GetFirstActiveVideoSource(videoSource) != PLUS_SUCCESS )
    {
      SafeRelease(&pNativeType);
      return PLUS_FAIL;
    }

    LONG stride;
    hr = MFGetStrideForBitmapInfoHeader(subtype.Data1, videoSource->GetBuffer()->GetFrameSize()[0], &stride);
    if (FAILED(hr))
    {
      return PLUS_FAIL;
    }
    this->ImageStride = (UINT32)(stride);

    // Set the attribute for later reference.
    (void)pNativeType->SetUINT32(MF_MT_DEFAULT_STRIDE, UINT32(this->ImageStride));
  }

  GUID majorType, subtype;

  // Find the major type.
  hr = pNativeType->GetGUID(MF_MT_MAJOR_TYPE, &majorType);
  if (FAILED(hr))
  {
    SafeRelease(&pNativeType);
    return PLUS_FAIL;
  }

  // Define the output type.
  hr = MFCreateMediaType(&pType);
  if (FAILED(hr))
  {
    SafeRelease(&pNativeType);
    return PLUS_FAIL;
  }

  hr = pType->SetGUID(MF_MT_MAJOR_TYPE, majorType);
  if (FAILED(hr))
  {
    SafeRelease(&pNativeType);
    SafeRelease(&pType);
    return PLUS_FAIL;
  }

  bool hasCompatibleFormat(false);
  if (majorType == MFMediaType_Video)
  {
    GUID compatibleTypes[2] = { MFVideoFormat_YUY2, MFVideoFormat_RGB24 };

    for( int i = 0; i < 2; ++i )
    {
      RequestedVideoFormat.SourcePixelFormat = compatibleTypes[i];

      hr = pType->SetGUID(MF_MT_SUBTYPE, RequestedVideoFormat.SourcePixelFormat);
      if (FAILED(hr))
      {
        SafeRelease(&pNativeType);
        SafeRelease(&pType);
        return PLUS_FAIL;
      }

      // Set the uncompressed format.
      hr = this->CaptureSourceReader->SetCurrentMediaType(MF_SOURCE_READER_FIRST_VIDEO_STREAM, NULL, pType);
      if (SUCCEEDED(hr))
      {
        this->ActiveVideoFormat.SourcePixelFormat = compatibleTypes[i];
        hasCompatibleFormat = true;
        break;
      }
    }
  }
  else if (majorType == MFMediaType_Audio)
  {
    subtype = MFAudioFormat_PCM;

    hr = pType->SetGUID(MF_MT_SUBTYPE, subtype);
    if (FAILED(hr))
    {
      SafeRelease(&pNativeType);
      SafeRelease(&pType);
      return PLUS_FAIL;
    }

    // Set the uncompressed format.
    hr = this->CaptureSourceReader->SetCurrentMediaType(MF_SOURCE_READER_FIRST_AUDIO_STREAM, NULL, pType);
    if (SUCCEEDED(hr))
    { 
      hasCompatibleFormat = true;
    }
  }

  if( !hasCompatibleFormat )
  {
    LOG_ERROR("Capture device cannot capture video in RGB24 or YUY2, audio in PCM. Unable to continue.");
    return PLUS_FAIL;
  }
  
  SafeRelease(&pNativeType);
  SafeRelease(&pType);

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------

PlusStatus vtkMmfVideoSource::UpdateFrameSize()
{
  UINT32 width = 0;
  UINT32 height = 0;
  IMFMediaType* pNativeType(NULL);
  if( this->CaptureSourceReader != NULL )
  {
    HRESULT hr = this->CaptureSourceReader->GetNativeMediaType(MF_SOURCE_READER_FIRST_VIDEO_STREAM, 0, &pNativeType);
    hr = MFGetAttributeSize(pNativeType, MF_MT_FRAME_SIZE, &width, &height);
    if (FAILED(hr))
    {
      return PLUS_FAIL;
    }
    int currentFrameSize[2] = {0,0};
    vtkPlusDataSource* videoSource(NULL);
    this->GetFirstActiveVideoSource(videoSource);
    videoSource->GetBuffer()->GetFrameSize(currentFrameSize);
    if( currentFrameSize[0] != width || currentFrameSize[1] != height )
    {
      currentFrameSize[0] = width;
      currentFrameSize[1] = height;
      videoSource->GetBuffer()->SetFrameSize(currentFrameSize);
      PlusCommon::ITKScalarPixelType pixelType = itk::ImageIOBase::UCHAR; // always convert output to 8-bit grayscale
      this->UncompressedVideoFrame.AllocateFrame(currentFrameSize, pixelType);
    }

    SafeRelease(&pNativeType);
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
    LOG_WARNING("Unable to find MMFVideoSource XML data element");
    return PLUS_FAIL; 
  }

  vtkXMLDataElement* deviceConfig = this->FindThisDeviceElement(rootXmlElement);
  if (deviceConfig == NULL) 
  {
    LOG_ERROR("Cannot find MMFVideoSource element in XML tree!");
    return PLUS_FAIL;
  }

  int xRes = -1; 
  if ( !deviceConfig->GetScalarAttribute("XResolution", xRes) ) 
  {
    xRes = DEFAULT_X_RESOLUTION;
  }
  this->RequestedVideoFormat.width = xRes;

  int yRes = -1; 
  if ( !deviceConfig->GetScalarAttribute("YResolution", yRes) ) 
  {
    yRes = DEFAULT_Y_RESOLUTION;
  }
  this->RequestedVideoFormat.height = yRes;

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------

PlusStatus vtkMmfVideoSource::WriteConfiguration( vtkXMLDataElement* rootXmlElement )
{
  // Write superclass configuration
  Superclass::WriteConfiguration(rootXmlElement); 

  if ( rootXmlElement == NULL ) 
  {
    LOG_WARNING("Unable to find MMFVideoSource XML data element");
    return PLUS_FAIL; 
  }

  vtkXMLDataElement* deviceConfig = this->FindThisDeviceElement(rootXmlElement);
  if (deviceConfig == NULL) 
  {
    LOG_ERROR("Cannot find MMFVideoSource element in XML tree!");
    return PLUS_FAIL;
  }

  deviceConfig->SetIntAttribute("XResolution", this->RequestedVideoFormat.width);
  deviceConfig->SetIntAttribute("YResolution", this->RequestedVideoFormat.height);

  return PLUS_SUCCESS;
}
