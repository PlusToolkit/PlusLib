/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"

// Local includes
#include "vtkPlusDeckLinkVideoSource.h"
#include "vtkIGSIOAccurateTimer.h"

// VTK includes
#include <vtkObject.h>

// System includes
#include <string>

// DeckLink SDK includes
#include "DeckLinkAPIWrapper.h"

//----------------------------------------------------------------------------
// vtkPlusDeckLinkVideoSource::vtkInternal
//----------------------------------------------------------------------------

class vtkPlusDeckLinkVideoSource::vtkInternal : public vtkObject
{
public:
  static vtkPlusDeckLinkVideoSource::vtkInternal* New(vtkPlusDeckLinkVideoSource*);
  vtkTypeMacro(vtkInternal, vtkObject);

public:
  vtkPlusDeckLinkVideoSource* External;

  vtkInternal(vtkPlusDeckLinkVideoSource* external)
    : External(external) {}

  virtual ~vtkInternal() {}

  int DeviceIndex = -1;
  std::string DeviceName = "";
  FrameSizeType RequestedFrameSize = {1920, 1080, 1};
  BMDPixelFormat RequestedPixelFormat = bmdFormatUnspecified;
  BMDVideoConnection RequestedVideoConnection = bmdVideoConnectionUnspecified;
  BMDDisplayMode RequestedDisplayMode = bmdModeUnknown;

private:
  static vtkPlusDeckLinkVideoSource::vtkInternal* New();
  vtkInternal()
    : External(nullptr)
  {}
};

//----------------------------------------------------------------------------
// vtkPlusDeckLinkVideoSource
//----------------------------------------------------------------------------

vtkStandardNewMacro(vtkPlusDeckLinkVideoSource);
vtkStandardNewMacro(vtkPlusDeckLinkVideoSource::vtkInternal);

//----------------------------------------------------------------------------
vtkPlusDeckLinkVideoSource::vtkPlusDeckLinkVideoSource::vtkInternal* vtkPlusDeckLinkVideoSource::vtkInternal::New(vtkPlusDeckLinkVideoSource* _arg)
{
  vtkPlusDeckLinkVideoSource::vtkInternal* result = new vtkPlusDeckLinkVideoSource::vtkInternal();
  result->InitializeObjectBase();
  result->External = _arg;
  return result;
}

//----------------------------------------------------------------------------
vtkPlusDeckLinkVideoSource::vtkPlusDeckLinkVideoSource()
  : vtkPlusDevice()
  , Internal(vtkInternal::New(this))
{
  LOG_TRACE("vtkPlusDeckLinkVideoSource::vtkPlusDeckLinkVideoSource()");

  this->FrameNumber = 0;
  this->StartThreadForInternalUpdates = true;
}

//----------------------------------------------------------------------------
vtkPlusDeckLinkVideoSource::~vtkPlusDeckLinkVideoSource()
{
  LOG_TRACE("vtkPlusDeckLinkVideoSource::~vtkPlusDeckLinkVideoSource()");

  this->Internal->Delete();
  this->Internal = nullptr;
}

//----------------------------------------------------------------------------
void vtkPlusDeckLinkVideoSource::PrintSelf(ostream& os, vtkIndent indent)
{
  LOG_TRACE("vtkPlusDeckLinkVideoSource::PrintSelf(ostream& os, vtkIndent indent)");
  Superclass::PrintSelf(os, indent);
}

//----------------------------------------------------------------------------
bool vtkPlusDeckLinkVideoSource::IsTracker() const
{
  return false;
}

//----------------------------------------------------------------------------
bool vtkPlusDeckLinkVideoSource::IsVirtual() const
{
  return false;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusDeckLinkVideoSource::ReadConfiguration(vtkXMLDataElement* rootConfigElement)
{
  LOG_TRACE("vtkPlusDeckLinkVideoSource::ReadConfiguration");

  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_READING(deviceConfig, rootConfigElement);

  XML_FIND_NESTED_ELEMENT_REQUIRED(dataSourcesElement, deviceConfig, "DataSources");

  int devIndex(-1);
  XML_READ_SCALAR_ATTRIBUTE_NONMEMBER_OPTIONAL(int, DeviceIndex, devIndex, deviceConfig);
  if (devIndex != -1)
  {
    this->Internal->DeviceIndex = devIndex;
  }

  std::string devName("");
  XML_READ_STRING_ATTRIBUTE_NONMEMBER_OPTIONAL(DeviceName, devName, deviceConfig);
  if (!devName.empty())
  {
    this->Internal->DeviceName = devName;
  }

  int size[3] = {-1, -1, -1};
  XML_READ_VECTOR_ATTRIBUTE_NONMEMBER_OPTIONAL(int, 2, FrameSize, size, deviceConfig);
  if (size[0] != -1 && size[1] != -1)
  {
    this->Internal->RequestedFrameSize[0] = static_cast<unsigned int>(size[0]);
    this->Internal->RequestedFrameSize[1] = static_cast<unsigned int>(size[1]);
    this->Internal->RequestedFrameSize[2] = 1;
  }

  std::string pixelFormat("");
  XML_READ_STRING_ATTRIBUTE_NONMEMBER_OPTIONAL(PixelFormat, pixelFormat, deviceConfig);
  if (!pixelFormat.empty())
  {
    this->Internal->RequestedPixelFormat = DeckLinkAPIWrapper::PixelFormatFromString(pixelFormat);
    if (this->Internal->RequestedPixelFormat == bmdFormatUnspecified)
    {
      LOG_ERROR("Unknown pixel format requested. Please see device page documentation for supported pixel formats.");
      return PLUS_FAIL;
    }
  }

  std::string videoConnection("");
  XML_READ_STRING_ATTRIBUTE_NONMEMBER_OPTIONAL(ConnectionType, videoConnection, deviceConfig);
  if (!videoConnection.empty())
  {
    this->Internal->RequestedVideoConnection = DeckLinkAPIWrapper::VideoConnectionFromString(videoConnection);
    if (this->Internal->RequestedVideoConnection == bmdVideoConnectionUnspecified)
    {
      LOG_ERROR("Unknown connection type requested. Please see device page documentation for supported connections.");
      return PLUS_FAIL;
    }
  }

  std::string displayMode("");
  XML_READ_STRING_ATTRIBUTE_NONMEMBER_OPTIONAL(DisplayMode, displayMode, deviceConfig);
  if (!displayMode.empty())
  {
    this->Internal->RequestedDisplayMode = DeckLinkAPIWrapper::DisplayModeFromString(displayMode);
    if (this->Internal->RequestedDisplayMode == bmdModeUnknown)
    {
      LOG_ERROR("Unable to recognize requested display mode. Please see device documentation for valid entries.");
      return PLUS_FAIL;
    }
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusDeckLinkVideoSource::WriteConfiguration(vtkXMLDataElement* rootConfigElement)
{
  LOG_TRACE("vtkPlusDeckLinkVideoSource::WriteConfiguration");
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_WRITING(deviceConfig, rootConfigElement);
  return PLUS_FAIL;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusDeckLinkVideoSource::InternalConnect()
{
  LOG_TRACE("vtkPlusDeckLinkVideoSource::InternalConnect");

  IDeckLink* deckLink(nullptr);
  IDeckLinkIterator* deckLinkIterator(nullptr);
  IDeckLinkInput* deckLinkInput(nullptr);
  IDeckLinkDisplayModeIterator* deckLinkDisplayModeIterator(nullptr);
  IDeckLinkDisplayMode* deckLinkDisplayMode(nullptr);

#if WIN32
  // Initialize COM on this thread
  HRESULT result = CoInitializeEx(NULL, COINIT_MULTITHREADED);
  if (FAILED(result))
  {
    LOG_ERROR("Initialization of COM failed - result = " << std::hex << std::setw(8) << std::setfill('0') << result);
    goto failure;
  }
#endif

  deckLinkIterator = DeckLinkAPIWrapper::CreateDeckLinkIterator();

  // Enumerate all cards in this system
  int count = 0;
  while (deckLinkIterator->Next(&deckLink) == S_OK)
  {
    // Query the DeckLink for its input interface
    result = deckLink->QueryInterface(IID_IDeckLinkInput, (void**)&deckLinkInput);
    if (result != S_OK)
    {
      LOG_ERROR("Could not obtain the IDeckLinkInput interface - result = " << std::hex << std::setw(8) << std::setfill('0') << result);
      goto failure;
    }

    if (deckLinkInput->GetDisplayModeIterator(&deckLinkDisplayModeIterator) != S_OK)
    {
      LOG_ERROR("Unable to iterate display modes. Cannot select input display mode.");
      return PLUS_FAIL;
    }

    while (deckLinkDisplayModeIterator->Next(&deckLinkDisplayMode))
    {
      if (this->Internal->RequestedDisplayMode != bmdModeUnknown && deckLinkDisplayMode->GetDisplayMode() == this->Internal->RequestedDisplayMode)
      {
        BOOL supported;
        if (deckLinkInput->DoesSupportVideoMode(this->Internal->RequestedVideoConnection, this->Internal->RequestedDisplayMode, this->Internal->RequestedPixelFormat, bmdSupportedVideoModeDefault, &supported) && supported)
        {
          // Found by display mode
        }
      }
      else
      {
        BMDTimeValue frameDuration;
        BMDTimeScale timeScale;
        if (deckLinkDisplayMode->GetFrameRate(&frameDuration, &timeScale) != S_OK)
        {
          LOG_WARNING("Unable to retrieve frame rate for display mode. Skipping.");
          continue;
        }

        if (deckLinkDisplayMode->GetWidth() == this->Internal->RequestedFrameSize[0] &&
            deckLinkDisplayMode->GetHeight() == this->Internal->RequestedFrameSize[1] &&
            (double)timeScale / (double)frameDuration == this->AcquisitionRate)
        {
          BOOL supported;
          if (deckLinkInput->DoesSupportVideoMode(bmdVideoConnectionUnspecified, deckLinkDisplayMode->GetDisplayMode(), bmdFormatUnspecified, bmdSupportedVideoModeDefault, &supported) && supported)
          {
            // Found by frame details
          }
        }
      }

      deckLinkDisplayMode->Release();
    }
    deckLink->Release();
    count++;
  }

failure:
  if (deckLinkDisplayModeIterator)
  {
    deckLinkDisplayModeIterator->Release();
  }
  if (deckLinkIterator)
  {
    deckLinkIterator->Release();
  }
  if (deckLinkInput)
  {
    deckLinkInput->Release();
  }
  if (deckLink)
  {
    deckLink->Release();
  }
#if WIN32
  // Uninitalize COM on this thread
  CoUninitialize();
#endif

  return PLUS_FAIL;

}

//----------------------------------------------------------------------------
PlusStatus vtkPlusDeckLinkVideoSource::InternalDisconnect()
{
  LOG_TRACE("vtkPlusDeckLinkVideoSource::InternalDisconnect");

#if WIN32
  // Uninitalize COM on this thread
  CoUninitialize();
#endif

  return PLUS_FAIL;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusDeckLinkVideoSource::InternalStartRecording()
{
  LOG_TRACE("vtkPlusDeckLinkVideoSource::InternalStartRecording");
  return PLUS_FAIL;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusDeckLinkVideoSource::InternalStopRecording()
{
  LOG_TRACE("vtkPlusDeckLinkVideoSource::InternalStopRecording");
  return PLUS_FAIL;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusDeckLinkVideoSource::Probe()
{
  LOG_TRACE("vtkPlusDeckLinkVideoSource::Probe");
  return PLUS_FAIL;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusDeckLinkVideoSource::InternalUpdate()
{
  LOG_TRACE("vtkPlusDeckLinkVideoSource::InternalUpdate");

  return PLUS_FAIL;
}