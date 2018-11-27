/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

/*=========================================================================
The following copyright notice is applicable to parts of this file:
Copyright (c) 2014, Robarts Research Institute, The University of Western Ontario, London, Ontario, Canada
All rights reserved.
Authors include:
* Elvis Chen (Robarts Research Institute and The University of Western Ontario)
* Adam Rankin (Robarts Research Institute and The University of Western Ontario)
=========================================================================*/

// Plus includes
#include "PlusConfigure.h"
#include "vtkObjectFactory.h"
#include "vtkPlusPhilips3DProbeVideoSource.h"
#include "vtkPlusDataSource.h"

// STL includes
#include <future>

// Philips API includes
#include "StreamMgr.h"
#include "vtkPlusIEEListener.h"

// System includes
#ifdef _WIN32
  #include <inaddr.h>
  #include <WS2tcpip.h>
#else
  #include <arpa/inet.h>
#endif

//----------------------------------------------------------------------------

vtkPlusPhilips3DProbeVideoSource* vtkPlusPhilips3DProbeVideoSource::ActiveDevice = NULL;

//----------------------------------------------------------------------------

namespace
{
  vtkImageData* streamedImageData = NULL;
  static double LastValidTimestamp(0);
  static double LastRetryTime(0);

  const double TIMEOUT = 1.0; // 1 seconds;
  const double RETRY_TIMER = 0.5; // 1/2 second
}

//----------------------------------------------------------------------------
bool vtkPlusPhilips3DProbeVideoSource::StreamCallback(_int64 id, SClient3DArray* ed, SClient3DArray* cd)
{
  if (vtkPlusPhilips3DProbeVideoSource::ActiveDevice == NULL)
  {
    LOG_ERROR("No Philips device has been created, but the callback was still called.");
    return false;
  }

  int dimensions[3] = {ed->width_padded, ed->height_padded, ed->depth_padded};

  /*
  * This is way smaller than what Qlab reports. Perhaps the streaming volume
  * is way smaller than the recorded one:
  *
  * 112 x 48 x 112
  */
  if (streamedImageData == NULL)
  {
    // let the calibration matrix handle the spacing and orientation of the volume in 3-space
    double spacing[3] = {1.0, 1.0, 1.0};
    double origin[3] = {0.0, 0.0, 0.0};

    streamedImageData = vtkImageData::New();
    streamedImageData->SetSpacing(spacing);
    streamedImageData->SetExtent(0, dimensions[0] - 1, 0, dimensions[1] - 1, 0, dimensions[2] - 1);
    streamedImageData->SetOrigin(origin);
#if VTK_MAJOR_VERSION > 5
    streamedImageData->AllocateScalars(VTK_UNSIGNED_CHAR, 1);
#else
    streamedImageData->SetScalarTypeToUnsignedChar();
    streamedImageData->SetNumberOfScalarComponents(1);
    streamedImageData->AllocateScalars();
#endif

    vtkPlusDataSource* videoSource(NULL);
    vtkPlusPhilips3DProbeVideoSource::ActiveDevice->GetFirstVideoSource(videoSource);
    if (dimensions[0] < 0 || dimensions[1] < 0 || dimensions[2] < 0)
    {
      LOG_ERROR("Negative dimensions received from Philips ultrasound device.");
      return false;
    }
    videoSource->SetInputFrameSize(static_cast<unsigned int>(dimensions[0]), static_cast<unsigned int>(dimensions[1]), static_cast<unsigned int>(dimensions[2]));
    videoSource->SetPixelType(VTK_UNSIGNED_CHAR);
    videoSource->SetNumberOfScalarComponents(1);
  }
  else
  {
    int streamedDimensions[3] = {0, 0, 0};
    streamedImageData->GetDimensions(streamedDimensions);

    if (dimensions[0] != streamedDimensions[0] || dimensions[1] != streamedDimensions[1] || dimensions[2] != streamedDimensions[2])
    {
      LOG_ERROR("Dimensions of new frame do not match dimensions of previous frames. Cannot add frame to buffer.");
      return false;
    }
  }

  size_t size = dimensions[0] * dimensions[1] * dimensions[2];
  unsigned char* src = ed->pData;
  unsigned char* dst = (unsigned char*)streamedImageData->GetScalarPointer();

  memcpy((void*)dst, (void*)src, size);

  vtkPlusPhilips3DProbeVideoSource::ActiveDevice->CallbackAddFrame(streamedImageData);

  LastValidTimestamp = vtkIGSIOAccurateTimer::GetSystemTime();

  return true;
}

//----------------------------------------------------------------------------

vtkStandardNewMacro(vtkPlusPhilips3DProbeVideoSource);

//----------------------------------------------------------------------------
vtkPlusPhilips3DProbeVideoSource::vtkPlusPhilips3DProbeVideoSource()
  : Listener(NULL)
  , FrameNumber(0)
  , IPAddress(NULL)
  , Port(-1)
  , ForceZQuantize(false)
  , ResolutionFactor(2.5)
  , IntegerZ(true)
  , Isotropic(false)
  , QuantizeDim(true)
  , ZDecimation(2)
  , Set4PtFIR(true)
  , LatAndElevSmoothingIndex(4)
{
  this->StartThreadForInternalUpdates = true;
  this->AcquisitionRate = 10;

  // This effectively forces only one Philips 3D source at a time, but it paves the way
  // for a non-singleton architecture when the SDK supports it
  if (vtkPlusPhilips3DProbeVideoSource::ActiveDevice != NULL)
  {
    LOG_WARNING("There is already an active vtkPlusPhilips3DProbeVideoSource device. Philips API only supports one connection at a time, so the existing device is now deactivated and the newly created class is activated instead.");
  }

  vtkPlusPhilips3DProbeVideoSource::ActiveDevice = this;
}

//----------------------------------------------------------------------------
vtkPlusPhilips3DProbeVideoSource::~vtkPlusPhilips3DProbeVideoSource()
{
  if (this->Connected)
  {
    this->Disconnect();
  }

  if (this->Listener)
  {
    this->Listener->Delete();
    this->Listener = NULL;
  }

  vtkPlusPhilips3DProbeVideoSource::ActiveDevice = NULL;
}

//----------------------------------------------------------------------------
void vtkPlusPhilips3DProbeVideoSource::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);

  if (this->Listener)
  {
    this->Listener->PrintSelf(os, indent);
  }
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusPhilips3DProbeVideoSource::InternalConnect()
{
  LOG_TRACE("vtkPlusPhilips3DProbeVideoSource::InternalConnect");

  this->Listener = vtkPlusIEEListener::New(this->ForceZQuantize, this->ResolutionFactor, this->IntegerZ, this->Isotropic, this->QuantizeDim, this->ZDecimation, this->Set4PtFIR, this->LatAndElevSmoothingIndex);
  this->Listener->SetMachineName(this->IPAddress);
  this->Listener->SetPortNumber(this->Port);
  if (this->Listener->Connect(&vtkPlusPhilips3DProbeVideoSource::StreamCallback) == PLUS_FAIL)
  {
    LOG_ERROR("Unable to connect to Philips device.");
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusPhilips3DProbeVideoSource::InternalDisconnect()
{
  LOG_TRACE("vtkPlusPhilips3DProbeVideoSource::InternalDisconnect");

  this->Listener->Disconnect();

  if (streamedImageData != NULL)
  {
    streamedImageData->Delete();
    streamedImageData = NULL;
  }

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusPhilips3DProbeVideoSource::InternalUpdate()
{
  if (this->Listener->IsConnected())
  {
    // Listener thinks it's connected, let's check for timeouts
    if (vtkIGSIOAccurateTimer::GetSystemTime() - LastValidTimestamp >= TIMEOUT)
    {
      LOG_INFO("Philips iE33: 3D mode timeout disconnected. Did you switch to another mode?");
      // Don't call a full disconnect because that stops the InternalUpdate loop
      // Only disconnect the listener and periodically try again
      this->Listener->Disconnect();
      LastRetryTime = vtkIGSIOAccurateTimer::GetSystemTime();
    }
  }
  else if (vtkIGSIOAccurateTimer::GetSystemTime() - LastRetryTime >= RETRY_TIMER)
  {
    LOG_INFO("Philips iE33: Retrying connection to 3D mode.");
    // Retry connection if it's time
    LastRetryTime = vtkIGSIOAccurateTimer::GetSystemTime();
    if (this->Listener->Connect(&vtkPlusPhilips3DProbeVideoSource::StreamCallback, vtkIGSIOLogger::LOG_LEVEL_WARNING))
    {
      LOG_INFO("Philips iE33: Connection successfully re-established.");
      LastValidTimestamp = vtkIGSIOAccurateTimer::GetSystemTime();
    }
  }

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusPhilips3DProbeVideoSource::ReadConfiguration(vtkXMLDataElement* rootConfigElement)
{
  LOG_TRACE("vtkPlusPhilips3DProbeVideoSource::ReadConfiguration");
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_READING(deviceConfig, rootConfigElement);

  XML_READ_SCALAR_ATTRIBUTE_REQUIRED(int, Port, deviceConfig);
  XML_READ_CSTRING_ATTRIBUTE_REQUIRED(IPAddress, deviceConfig);
#ifdef _WIN32
  struct in_addr address;
  int result = InetPton(AF_INET, this->IPAddress, &address);
#else
  struct sockaddr_in address;
  int result = inet_pton(AF_INET, this->IPAddress, &(address.sin_addr));
#endif
  if (result != 1)
  {
    LOG_ERROR("Improperly formatted IPAddress. Please confirm formatting in config file.");
    return PLUS_FAIL;
  }

  XML_READ_BOOL_ATTRIBUTE_OPTIONAL(ForceZQuantize, deviceConfig);
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(double, ResolutionFactor, deviceConfig);
  XML_READ_BOOL_ATTRIBUTE_OPTIONAL(IntegerZ, deviceConfig);
  XML_READ_BOOL_ATTRIBUTE_OPTIONAL(Isotropic, deviceConfig);
  XML_READ_BOOL_ATTRIBUTE_OPTIONAL(QuantizeDim, deviceConfig);
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(int, ZDecimation, deviceConfig);
  XML_READ_BOOL_ATTRIBUTE_OPTIONAL(Set4PtFIR, deviceConfig);
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(int, LatAndElevSmoothingIndex, deviceConfig);

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusPhilips3DProbeVideoSource::WriteConfiguration(vtkXMLDataElement* rootConfig)
{
  LOG_TRACE("vtkPlusPhilips3DProbeVideoSource::WriteConfiguration");
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_WRITING(deviceConfig, rootConfig);

  XML_WRITE_CSTRING_ATTRIBUTE_IF_NOT_NULL(IPAddress, deviceConfig);
  deviceConfig->SetIntAttribute("Port", this->Port);

  deviceConfig->SetDoubleAttribute("ResolutionFactor", this->ResolutionFactor);
  deviceConfig->SetIntAttribute("ZDecimation", this->ZDecimation);
  deviceConfig->SetIntAttribute("LatAndElevSmoothingIndex", this->LatAndElevSmoothingIndex);
  XML_WRITE_BOOL_ATTRIBUTE(ForceZQuantize, deviceConfig);
  XML_WRITE_BOOL_ATTRIBUTE(IntegerZ, deviceConfig);
  XML_WRITE_BOOL_ATTRIBUTE(Isotropic, deviceConfig);
  XML_WRITE_BOOL_ATTRIBUTE(QuantizeDim, deviceConfig);
  XML_WRITE_BOOL_ATTRIBUTE(Set4PtFIR, deviceConfig);
  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusPhilips3DProbeVideoSource::NotifyConfigured()
{
  if (this->OutputChannels.size() > 1)
  {
    LOG_WARNING("vtkPlusPhilips3DProbeVideoSource is expecting one output channel and there are " << this->OutputChannels.size() << " channels. First output channel will be used.");
    this->SetCorrectlyConfigured(false);
    return PLUS_FAIL;
  }

  if (this->OutputChannels.empty())
  {
    LOG_ERROR("No output channels defined for vtkPlusPhilips3DProbeVideoSource. Cannot proceed.");
    this->SetCorrectlyConfigured(false);
    return PLUS_FAIL;
  }

  vtkPlusDataSource* videoSource(NULL);
  if (this->GetFirstVideoSource(videoSource) != PLUS_SUCCESS)
  {
    LOG_ERROR("Unable to find video source. Device needs a video buffer to put new frames into.");
    this->SetCorrectlyConfigured(false);
    return PLUS_FAIL;
  }

  if (STRCASECMP(this->IPAddress, "") == 0 || this->Port <= 0)
  {
    this->SetCorrectlyConfigured(false);
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
bool vtkPlusPhilips3DProbeVideoSource::IsTracker() const
{
  return false;
}

//----------------------------------------------------------------------------
void vtkPlusPhilips3DProbeVideoSource::CallbackAddFrame(vtkImageData* imageData)
{
  vtkPlusDataSource* videoSource(NULL);
  if (this->GetFirstVideoSource(videoSource) != PLUS_SUCCESS)
  {
    LOG_ERROR("Unable to find video source. Cannot add new frame.");
    return;
  }

  std::future<void> addTask = std::async(std::launch::async, [ = ]()
  {
    if (videoSource->AddItem(imageData, videoSource->GetInputImageOrientation(), US_IMG_BRIGHTNESS, this->FrameNumber) != PLUS_SUCCESS)
    {
      LOG_ERROR("Unable to add item to buffer.");
      return;
    }
  });
  std::future<uint8_t> maxPixelTask = std::async(std::launch::async, [ = ]()
  {
    int extent[6];
    imageData->GetExtent(extent);
    uint8_t* pixelPtr = (uint8_t*)imageData->GetScalarPointer();
    auto numComponents = imageData->GetNumberOfScalarComponents();

    uint8_t maxPixelValue(0);
    for (int z = 0; z < extent[5] - extent[4]; ++z)
    {
      for (int y = 0; y < extent[3] - extent[2]; ++y)
      {
        for (int x = 0; x < extent[1] - extent[0]; ++x)
        {
          if (*pixelPtr > maxPixelValue)
          {
            maxPixelValue = *pixelPtr;
          }
          pixelPtr += numComponents;
        }
      }
    }

    return maxPixelValue;
  });

  addTask.wait();
  unsigned int maxPixelValue = static_cast<unsigned int>(maxPixelTask.get());

  std::stringstream ss;
  ss << maxPixelValue;
  videoSource->ModifyBufferItemFrameField(videoSource->GetLatestItemUidInBuffer(), "MaximumPixelValue", ss.str());

  this->FrameNumber++;
}