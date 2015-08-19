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
#include "vtkPhilips3DProbeVideoSource.h"
#include "vtkPlusDataSource.h"

// Philips API includes
#include "StreamMgr.h"
#include "vtkIEEListener.h"

// System includes
#ifdef _WIN32
#include <inaddr.h>
#include <WS2tcpip.h>
#else
#include <arpa/inet.h>
#endif 

//----------------------------------------------------------------------------

vtkPhilips3DProbeVideoSource* vtkPhilips3DProbeVideoSource::ActiveDevice = NULL;

//----------------------------------------------------------------------------

namespace
{
  vtkImageData* streamedImageData = NULL;
}

//----------------------------------------------------------------------------
bool vtkPhilips3DProbeVideoSource::StreamCallback(_int64 id, SClient3DArray *ed, SClient3DArray *cd)
{
  if( vtkPhilips3DProbeVideoSource::ActiveDevice == NULL )
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
  if( streamedImageData == NULL )
  {
    // let the calibration matrix handle the spacing and orientation of the volume in 3-space
    double spacing[3] = {1.0, 1.0, 1.0};
    double origin[3] = {0.0, 0.0, 0.0};

    streamedImageData = vtkImageData::New();
    streamedImageData->SetSpacing(spacing);
    streamedImageData->SetExtent(0, dimensions[0]-1, 0, dimensions[1]-1, 0, dimensions[2]-1);
    streamedImageData->SetOrigin(origin);
    streamedImageData->AllocateScalars(VTK_UNSIGNED_CHAR, 1);

    vtkPlusDataSource* videoSource(NULL);
    vtkPhilips3DProbeVideoSource::ActiveDevice->GetFirstVideoSource(videoSource);
    videoSource->SetInputFrameSize(dimensions[0], dimensions[1], dimensions[2]);
    videoSource->SetPixelType(VTK_UNSIGNED_CHAR);
    videoSource->SetNumberOfScalarComponents(1);
  }
  else
  {
    int streamedDimensions[3] = {0,0,0};
    streamedImageData->GetDimensions(streamedDimensions);

    if( dimensions[0] != streamedDimensions[0] || dimensions[1] != streamedDimensions[1] || dimensions[2] != streamedDimensions[2] )
    {
      LOG_ERROR("Dimensions of new frame do not match dimensions of previous frames. Cannot add frame to buffer.");
      return false;
    }
  }

  size_t size = dimensions[0]*dimensions[1]*dimensions[2];
  unsigned char *src = ed->pData;
  unsigned char *dst = (unsigned char*)streamedImageData->GetScalarPointer();

  memcpy((void*)dst, (void*)src, size);

  vtkPhilips3DProbeVideoSource::ActiveDevice->CallbackAddFrame(streamedImageData);

  return true;
}

//----------------------------------------------------------------------------

vtkStandardNewMacro(vtkPhilips3DProbeVideoSource);

//----------------------------------------------------------------------------
vtkPhilips3DProbeVideoSource::vtkPhilips3DProbeVideoSource()
: Listener(vtkIEEListener::New())
, FrameNumber(0)
, IPAddress("")
, Port(-1)
{
  // No callback function provided by the device, so the data capture thread will be used to poll the hardware and add new items to the buffer
  this->StartThreadForInternalUpdates = true;
  this->AcquisitionRate = 10;

  // This effectively forces only one philips 3d source at a time, but it paves the way
  // for a non-singleton architecture when the SDK supports it
  if( vtkPhilips3DProbeVideoSource::ActiveDevice != NULL )
  {
    LOG_WARNING("There is already an active vtkPhilips3DProbeVideoSource device. Philips API only supports one connection at a time, so the existing device is now deactivated and the newly created class is activated instead.");
  }

  vtkPhilips3DProbeVideoSource::ActiveDevice = this;
}

//----------------------------------------------------------------------------
vtkPhilips3DProbeVideoSource::~vtkPhilips3DProbeVideoSource()
{ 
  if ( this->Connected )
  {
    this->Disconnect();
  }

  this->Listener->Delete();

  vtkPhilips3DProbeVideoSource::ActiveDevice = NULL;
}

//----------------------------------------------------------------------------
void vtkPhilips3DProbeVideoSource::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);

  this->Listener->PrintSelf(os, indent);
}

//----------------------------------------------------------------------------
PlusStatus vtkPhilips3DProbeVideoSource::InternalConnect()
{
  LOG_TRACE("vtkPhilips3DProbeVideoSource::InternalConnect");

  this->Listener->SetMachineName(this->IPAddress);
  this->Listener->SetPortNumber(this->Port);
  if( this->Listener->Connect(&vtkPhilips3DProbeVideoSource::StreamCallback) == PLUS_FAIL )
  {
    LOG_ERROR("Unable to connect to Philips device.");
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPhilips3DProbeVideoSource::InternalDisconnect()
{
  LOG_TRACE("vtkPhilips3DProbeVideoSource::InternalDisconnect");

  this->Listener->Disconnect();

  if( streamedImageData != NULL )
  {
    streamedImageData->Delete();
    streamedImageData = NULL;
  }

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkPhilips3DProbeVideoSource::ReadConfiguration(vtkXMLDataElement* rootConfigElement)
{
  LOG_TRACE("vtkPhilips3DProbeVideoSource::ReadConfiguration");
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_READING(deviceConfig, rootConfigElement);

  if( deviceConfig->GetScalarAttribute("Port", this->Port) != 1 || this->Port <= 0)
  {
    LOG_ERROR("Unable to find port in Philips ie33 configuration or invalid port selected.");
    return PLUS_FAIL;
  }

  const char* IP = deviceConfig->GetAttribute("IP");
  if( IP == NULL )
  {
    LOG_ERROR("Unable to find Philips ie33 IP in config file. Please check configuration.");
    return PLUS_FAIL;
  }
#ifdef _WIN32
  struct in_addr address;
  int result = InetPton(AF_INET, IP, &address);
#else
  struct sockaddr_in address;
  int result = inet_pton(AF_INET, IP, &(address.sin_addr));
#endif 
  if( result != 1 )
  {
    LOG_ERROR("Improperly formatted IP address. Please confirm formatting in config file.");
    return PLUS_FAIL;
  }
  this->IPAddress = std::string(IP);

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkPhilips3DProbeVideoSource::WriteConfiguration(vtkXMLDataElement* rootConfig)
{
  LOG_TRACE("vtkPhilips3DProbeVideoSource::WriteConfiguration");
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_WRITING(deviceConfig, rootConfig);

  deviceConfig->SetAttribute("IP", this->IPAddress.c_str());
  deviceConfig->SetIntAttribute("Port", this->Port);

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkPhilips3DProbeVideoSource::NotifyConfigured()
{
  if( this->OutputChannels.size() > 1 )
  {
    LOG_WARNING("vtkPhilips3DProbeVideoSource is expecting one output channel and there are " << this->OutputChannels.size() << " channels. First output channel will be used.");
    this->SetCorrectlyConfigured(false);
    return PLUS_FAIL;
  }

  if( this->OutputChannels.empty() )
  {
    LOG_ERROR("No output channels defined for vtkPhilips3DProbeVideoSource. Cannot proceed." );
    this->SetCorrectlyConfigured(false);
    return PLUS_FAIL;
  }

  vtkPlusDataSource* videoSource(NULL);
  if( this->GetFirstVideoSource(videoSource) != PLUS_SUCCESS )
  {
    LOG_ERROR("Unable to find video source. Device needs a video buffer to put new frames into.");
    this->SetCorrectlyConfigured(false);
    return PLUS_FAIL;
  }

  if( this->IPAddress.empty() || this->Port <= 0 )
  {
    this->SetCorrectlyConfigured(false);
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
bool vtkPhilips3DProbeVideoSource::IsTracker() const
{
  return false;
}

//----------------------------------------------------------------------------
void vtkPhilips3DProbeVideoSource::CallbackAddFrame(vtkImageData* imageData)
{
  vtkPlusDataSource* videoSource(NULL);
  if( this->GetFirstVideoSource(videoSource) != PLUS_SUCCESS )
  {
    LOG_ERROR("Unable to find video source. Cannot add new frame.");
    return;
  }
  if( videoSource->AddItem(imageData, videoSource->GetInputImageOrientation(), US_IMG_BRIGHTNESS, this->FrameNumber) != PLUS_SUCCESS )
  {
    LOG_ERROR("Unable to add item to buffer.");
    return;
  }
  this->FrameNumber++;
}

//----------------------------------------------------------------------------
void vtkPhilips3DProbeVideoSource::SetIPAddress(const std::string& ipAddress)
{
  this->IPAddress = ipAddress;
}

//----------------------------------------------------------------------------
std::string vtkPhilips3DProbeVideoSource::GetIPAddress() const
{
  return this->IPAddress;
}
