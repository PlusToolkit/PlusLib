/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

// Local includes
#include "PlusConfigure.h"
#include "ICCapturingListener.h"
#include "vtkPlusChannel.h"
#include "vtkPlusDataSource.h"
#include "vtkPlusICCapturingSource.h"

// VTK includes
#include <vtkImageData.h>
#include <vtkInformation.h>
#include <vtkInformationVector.h>
#include <vtkMultiThreader.h>
#include <vtkObjectFactory.h>
#include <vtkStreamingDemandDrivenPipeline.h>
#include <vtksys/SystemTools.hxx>

// ICC includes
#include <tisudshl.h>

// OS includes
#include <ctype.h>

// STL includes
#include <vector>
#include <string>

//----------------------------------------------------------------------------

vtkPlusICCapturingSource* vtkPlusICCapturingSource::Instance = 0;
vtkPlusICCapturingSourceCleanup vtkPlusICCapturingSource::Cleanup;

//----------------------------------------------------------------------------
vtkPlusICCapturingSourceCleanup::vtkPlusICCapturingSourceCleanup()
{
}

//----------------------------------------------------------------------------
vtkPlusICCapturingSourceCleanup::~vtkPlusICCapturingSourceCleanup()
{
  // Destroy any remaining output window.
  vtkPlusICCapturingSource::SetInstance(NULL);
}

//----------------------------------------------------------------------------
vtkPlusICCapturingSource::vtkPlusICCapturingSource()
{
  this->ICBufferSize = 50;

  this->DeviceName = NULL;
  this->VideoNorm = NULL;
  this->VideoFormat = NULL;
  this->FrameSize[0] = 640;
  this->FrameSize[1] = 480;
  this->FrameSize[2] = 1;
  this->InputChannel = NULL;

  this->FrameGrabber = NULL;
  this->FrameGrabberListener = NULL;

  this->Modified();

  this->RequireImageOrientationInConfiguration = true;

  // No need for StartThreadForInternalUpdates, as we are notified about each new frame through a callback function
}

//----------------------------------------------------------------------------
vtkPlusICCapturingSource::~vtkPlusICCapturingSource()
{
  this->Disconnect();

  if (this->FrameGrabber != NULL)
  {
    delete this->FrameGrabber;
    this->FrameGrabber = NULL;
  }

  if (this->FrameGrabberListener != NULL)
  {
    delete this->FrameGrabberListener;
    this->FrameGrabberListener = NULL;
  }

  SetDeviceName(NULL);
  SetVideoNorm(NULL);
  SetVideoFormat(NULL);
}

//----------------------------------------------------------------------------
// Up the reference count so it behaves like New
vtkPlusICCapturingSource* vtkPlusICCapturingSource::New()
{
  vtkPlusICCapturingSource* ret = vtkPlusICCapturingSource::GetInstance();
  ret->Register(NULL);
  return ret;
}

//----------------------------------------------------------------------------
// Return the single instance of the vtkOutputWindow
vtkPlusICCapturingSource* vtkPlusICCapturingSource::GetInstance()
{
  if (!vtkPlusICCapturingSource::Instance)
  {
    // Try the factory first
    vtkPlusICCapturingSource::Instance = (vtkPlusICCapturingSource*)vtkObjectFactory::CreateInstance("vtkPlusICCapturingSource");
    if (!vtkPlusICCapturingSource::Instance)
    {
      vtkPlusICCapturingSource::Instance = new vtkPlusICCapturingSource();
    }
    if (!vtkPlusICCapturingSource::Instance)
    {
      int error = 0;
    }
  }
  // return the instance
  return vtkPlusICCapturingSource::Instance;
}

//----------------------------------------------------------------------------
void vtkPlusICCapturingSource::SetInstance(vtkPlusICCapturingSource* instance)
{
  if (vtkPlusICCapturingSource::Instance == instance)
  {
    return;
  }
  // preferably this will be NULL
  if (vtkPlusICCapturingSource::Instance)
  {
    vtkPlusICCapturingSource::Instance->Delete();;
  }
  vtkPlusICCapturingSource::Instance = instance;
  if (!instance)
  {
    return;
  }
  // user will call ->Delete() after setting instance
  instance->Register(NULL);
}

//----------------------------------------------------------------------------
std::string vtkPlusICCapturingSource::GetSdkVersion()
{
  std::ostringstream version;
  version << "The Imaging Source UDSHL-" << UDSHL_LIB_VERSION_MAJOR << "." << UDSHL_LIB_VERSION_MINOR;
  return version.str();
}

//----------------------------------------------------------------------------
void vtkPlusICCapturingSource::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

//----------------------------------------------------------------------------
// the callback function used when there is a new frame of data received
bool vtkPlusICCapturingSource::vtkPlusICCapturingSourceNewFrameCallback(unsigned char* data, unsigned long size, unsigned long frameNumber)
{
  if (data == NULL || size == 0)
  {
    LOG_ERROR("No actual frame data received from the framegrabber");
    return false;
  }

  vtkPlusICCapturingSource::GetInstance()->AddFrameToBuffer(data, size, frameNumber);

  return true;
}

//----------------------------------------------------------------------------
// copy the Device Independent Bitmap from the VFW framebuffer into the
// vtkVideoSource framebuffer (don't do the unpacking yet)
PlusStatus vtkPlusICCapturingSource::AddFrameToBuffer(unsigned char* dataPtr, unsigned long size, unsigned long frameNumber)
{
  if (!this->Recording)
  {
    // drop the frame, we are not recording data now
    return PLUS_SUCCESS;
  }

  vtkPlusDataSource* aSource = NULL;
  if (this->GetFirstActiveOutputVideoSource(aSource) != PLUS_SUCCESS)
  {
    LOG_ERROR("Unable to retrieve the video source in the ICCapturing device.");
    return PLUS_FAIL;
  }

  this->FrameNumber = frameNumber;

  DShowLib::Grabber* grabber = static_cast<DShowLib::Grabber*>(this->FrameGrabber);
  if (grabber == nullptr || grabber->getAcqSizeMaxX() < 0 || grabber->getAcqSizeMaxY() < 0)
  {
    LOG_ERROR("Negative frame size sent from ICC device. Cannot continue.");
    return PLUS_FAIL;
  }

  FrameSizeType frameSize = {static_cast<unsigned int>(grabber->getAcqSizeMaxX()), static_cast<unsigned int>(grabber->getAcqSizeMaxY()), 1};

  int frameBufferBitsPerPixel = grabber->getVideoFormat().getBitsPerPixel();
  if (frameBufferBitsPerPixel != 8)
  {
    LOG_ERROR("vtkPlusICCapturingSource::AddFrameToBuffer: only 8-bit acquisition is supported, current frameBufferBitsPerPixel=" << frameBufferBitsPerPixel);
    return PLUS_FAIL;
  }

  PlusStatus status = aSource->AddItem(dataPtr, aSource->GetInputImageOrientation(), frameSize, VTK_UNSIGNED_CHAR, 1, US_IMG_BRIGHTNESS, 0, this->FrameNumber);
  this->Modified();

  return status;
}


//----------------------------------------------------------------------------
PlusStatus vtkPlusICCapturingSource::InternalConnect()
{
  if (!DShowLib::InitLibrary())
  {
    LOG_ERROR("The IC capturing library could not be initialized");
    return PLUS_FAIL;
  }

  // Add DShowLib::ExitLibrary to the list of functions that are called on application exit.
  // It is useful because when the application is forced to exit then the destructor may not be called.
  static bool exitFunctionAdded = false;
  if (!exitFunctionAdded)
  {
    atexit(DShowLib::ExitLibrary);
    exitFunctionAdded = true;
  }

  if (this->FrameGrabber == NULL)
  {
    this->FrameGrabber = new DShowLib::Grabber;
  }

  // Set the device name (e.g. DFG/USB2-lt)
  if (this->GetDeviceName() == NULL || !static_cast<DShowLib::Grabber*>(FrameGrabber)->openDev(this->GetDeviceName()))
  {
    LOG_ERROR("The IC capturing library could not be initialized - invalid device name: " << this->GetDeviceName());
    LogListOfCaptureDevices();
    return PLUS_FAIL;
  }

  // Set the video norm (e.g. PAL_B or NTSC_M)
  if (this->GetVideoNorm() == NULL || !static_cast<DShowLib::Grabber*>(FrameGrabber)->setVideoNorm(this->GetVideoNorm()))
  {
    LOG_ERROR("The IC capturing library could not be initialized - invalid video norm: " << this->GetVideoNorm());
    LogListOfCaptureDevices();
    return PLUS_FAIL;
  }

  // The Y800 color format is an 8 bit monochrome format.
  if (!static_cast<DShowLib::Grabber*>(FrameGrabber)->setVideoFormat(this->GetDShowLibVideoFormatString().c_str()))
  {
    LOG_ERROR("The IC capturing library could not be initialized - invalid video format: " << this->GetDShowLibVideoFormatString());
    LogListOfCaptureDevices();
    return PLUS_FAIL;
  }

  int bitsPerPixel = static_cast<DShowLib::Grabber*>(FrameGrabber)->getVideoFormat().getBitsPerPixel();
  if (bitsPerPixel != 8)
  {
    LOG_ERROR("The IC capturing library could not be initialized - invalid bits per pixel: " << bitsPerPixel);
    return PLUS_FAIL;
  }

  vtkPlusDataSource* aSource = NULL;
  if (this->GetFirstActiveOutputVideoSource(aSource) != PLUS_SUCCESS)
  {
    LOG_ERROR("Unable to retrieve the video source in the ICCapturing device.");
    return PLUS_FAIL;
  }
  aSource->SetPixelType(VTK_UNSIGNED_CHAR);

  FrameSizeType frameSize = {0, 0, 1};
  frameSize[0] = static_cast<DShowLib::Grabber*>(FrameGrabber)->getAcqSizeMaxX();
  frameSize[1] = static_cast<DShowLib::Grabber*>(FrameGrabber)->getAcqSizeMaxY();
  frameSize[2] = 1;
  aSource->SetInputFrameSize(frameSize);

  if (this->GetInputChannel() == NULL || !static_cast<DShowLib::Grabber*>(FrameGrabber)->setInputChannel(this->GetInputChannel()))
  {
    LOG_ERROR("The IC capturing library could not be initialized - invalid input channel: " << this->GetInputChannel());
    return PLUS_FAIL;
  }

  if (this->FrameGrabberListener == NULL)
  {
    this->FrameGrabberListener = new ICCapturingListener();
  }

  this->FrameGrabberListener->SetICCapturingSourceNewFrameCallback(vtkPlusICCapturingSource::vtkPlusICCapturingSourceNewFrameCallback);

  // Assign the number of buffers to the cListener object.
  this->FrameGrabberListener->setBufferSize(this->GetICBufferSize());

  // Register the FrameGrabberListener object for the frame ready
  // TODO: check if the listener should be removed (in disconnect, when reconnecting, ...)
  static_cast<DShowLib::Grabber*>(FrameGrabber)->addListener(FrameGrabberListener, DShowLib::GrabberListener::eFRAMEREADY);

  // Create a FrameTypeInfoArray data structure describing the allowed color formats.
  DShowLib::FrameTypeInfoArray acceptedTypes = DShowLib::FrameTypeInfoArray::createRGBArray();

  // Create the frame handler sink: 8 bit monochrome format.
  smart_ptr<DShowLib::FrameHandlerSink> pSink = DShowLib::FrameHandlerSink::create(DShowLib::eRGB8, 1);
  //smart_ptr<DShowLib::FrameHandlerSink> pSink = DShowLib::FrameHandlerSink::create( DShowLib::eY800, 1);

  // Disable snap mode.
  pSink->setSnapMode(false);

  // Apply the sink to the grabber.
  static_cast<DShowLib::Grabber*>(FrameGrabber)->setSinkType(pSink);

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusICCapturingSource::InternalDisconnect()
{
  LOG_DEBUG("Disconnect from IC capturing");

  static_cast<DShowLib::Grabber*>(FrameGrabber)->removeListener(FrameGrabberListener);
  delete this->FrameGrabberListener;
  this->FrameGrabberListener = NULL;

  delete FrameGrabber;
  this->FrameGrabber = NULL;

  DShowLib::ExitLibrary();

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusICCapturingSource::InternalStartRecording()
{
  if (!static_cast<DShowLib::Grabber*>(FrameGrabber)->startLive(false))
  {
    LOG_ERROR("Framegrabber startLive failed, cannot start the recording");
    return PLUS_FAIL;
  }
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusICCapturingSource::InternalStopRecording()
{
  if (!static_cast<DShowLib::Grabber*>(FrameGrabber)->stopLive())
  {
    LOG_ERROR("Framegrabber stopLive failed");
    return PLUS_FAIL;
  }
  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusICCapturingSource::ReadConfiguration(vtkXMLDataElement* rootConfigElement)
{
  LOG_TRACE("vtkPlusICCapturingSource::ReadConfiguration");

  // This is a singleton class, so some input channels, output channels, or tools might have been already
  // defined. Clean them up before creating the new ones from XML.
  for (ChannelContainerIterator it = this->OutputChannels.begin(); it != this->OutputChannels.end(); ++it)
  {
    (*it)->UnRegister(this);
  }
  this->InputChannels.clear();
  this->OutputChannels.clear();
  for (DataSourceContainerIterator it = this->Tools.begin(); it != this->Tools.end(); ++it)
  {
    it->second->UnRegister(this);
  }
  this->Tools.clear();
  for (DataSourceContainerIterator it = this->VideoSources.begin(); it != this->VideoSources.end(); ++it)
  {
    it->second->UnRegister(this);
  }
  this->VideoSources.clear();

  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_READING(deviceConfig, rootConfigElement);

  XML_READ_CSTRING_ATTRIBUTE_OPTIONAL(DeviceName, deviceConfig);
  XML_READ_CSTRING_ATTRIBUTE_OPTIONAL(VideoNorm, deviceConfig);
  XML_READ_CSTRING_ATTRIBUTE_OPTIONAL(VideoFormat, deviceConfig);
  XML_READ_STD_ARRAY_ATTRIBUTE_OPTIONAL(int, 2, FrameSize, deviceConfig);
  FrameSize[2] = 1;

  // Only for backward compatibility
  // VideoFormat used to contain both video format and frame size, so in case frame size is defined
  // then this method parses that and updates VideoFormat and FrameSize accordingly
  ParseDShowLibVideoFormatString(this->VideoFormat);

  XML_READ_CSTRING_ATTRIBUTE_OPTIONAL(InputChannel, deviceConfig);
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(int, ICBufferSize, deviceConfig);

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusICCapturingSource::WriteConfiguration(vtkXMLDataElement* rootConfigElement)
{
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_WRITING(imageAcquisitionConfig, rootConfigElement);

  imageAcquisitionConfig->SetAttribute("DeviceName", this->DeviceName);
  imageAcquisitionConfig->SetAttribute("VideoNorm", this->VideoNorm);
  imageAcquisitionConfig->SetAttribute("VideoFormat", this->VideoFormat);
  int frameSize[2];
  frameSize[0] = static_cast<int>(this->FrameSize[0]);
  frameSize[1] = static_cast<int>(this->FrameSize[1]);
  imageAcquisitionConfig->SetVectorAttribute("FrameSize", 2, frameSize);
  imageAcquisitionConfig->SetAttribute("InputChannel", this->InputChannel);
  imageAcquisitionConfig->SetIntAttribute("ICBufferSize", this->ICBufferSize);

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
void vtkPlusICCapturingSource::SetFrameSize(const FrameSizeType& frameSize)
{
  this->FrameSize = FrameSize;
}

//----------------------------------------------------------------------------
void vtkPlusICCapturingSource::SetFrameSize(unsigned int i, unsigned int j, unsigned int k)
{
  FrameSizeType t;
  t[0] = i;
  t[1] = j;
  t[2] = k;
  this->SetFrameSize(t);
}

//----------------------------------------------------------------------------
FrameSizeType vtkPlusICCapturingSource::GetFrameSize() const
{
  return this->FrameSize;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusICCapturingSource::NotifyConfigured()
{
  if (this->OutputChannels.size() > 1)
  {
    LOG_WARNING("ICCapturingSource is expecting one output channel and there are " << this->OutputChannels.size() << " channels. First output channel will be used.");
    return PLUS_FAIL;
  }

  if (this->OutputChannels.empty())
  {
    LOG_ERROR("No output channels defined for vtkPlusICCapturingSource. Cannot proceed.");
    this->SetCorrectlyConfigured(false);
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
void vtkPlusICCapturingSource::ParseDShowLibVideoFormatString(const char* videoFormatFrameSizeString)
{
  if (videoFormatFrameSizeString == NULL)
  {
    // parsing failed
    return;
  }

  // videoFormatFrameSizeString sample: "Y800 (640x480)"
  std::vector<std::string> splitVideoFormatFrameSize;
  igsioCommon::SplitStringIntoTokens(videoFormatFrameSizeString, ' ', splitVideoFormatFrameSize);
  if (splitVideoFormatFrameSize.size() != 2)
  {
    // parsing failed
    return;
  }

  // splitVideoFormatFrameSize[0] is Y800
  // splitVideoFormatFrameSize[1] is (640x480), so it has to be split and parsed further

  // parse frame size
  std::vector<std::string> splitFrameSize;
  igsioCommon::SplitStringIntoTokens(splitVideoFormatFrameSize[1], 'x', splitFrameSize);
  if (splitFrameSize.size() != 2 || splitFrameSize[0].empty() || splitFrameSize[1].empty()) // (640 480)
  {
    // parsing failed
    return;
  }
  if (splitFrameSize[0][0] != '(' || splitFrameSize[1][splitFrameSize[1].size() - 1] != ')')
  {
    // parsing failed
    return;
  }
  // Remove parentheses
  splitFrameSize[0].erase(splitFrameSize[0].begin());
  splitFrameSize[1].erase(splitFrameSize[1].end() - 1);

  // Convert to unsigned integer
  unsigned int frameSizeX = 0;
  unsigned int frameSizeY = 0;
  if (igsioCommon::StringToUInt(splitFrameSize[0].c_str(), frameSizeX) == PLUS_FAIL)
  {
    return;
  }
  if (igsioCommon::StringToUInt(splitFrameSize[1].c_str(), frameSizeY) == PLUS_FAIL)
  {
    return;
  }

  // Parsing successful, save results
  this->SetVideoFormat(splitVideoFormatFrameSize[0].c_str());
  this->SetFrameSize(frameSizeX, frameSizeY, 1);
}

//----------------------------------------------------------------------------
std::string vtkPlusICCapturingSource::GetDShowLibVideoFormatString()
{
  std::ostringstream ss;
  ss << (this->GetVideoFormat() ? this->GetVideoFormat() : "Y800");
  ss << " (" << this->FrameSize[0] << "x" << this->FrameSize[1] << ")" << std::ends;
  std::string formatString = ss.str();
  return formatString;
}

//----------------------------------------------------------------------------
void vtkPlusICCapturingSource::GetListOfCaptureDevices(std::vector< std::string >& deviceNames)
{
  deviceNames.clear();

  if (!DShowLib::InitLibrary())
  {
    LOG_ERROR("The IC capturing library could not be initialized");
    return;
  }
  DShowLib::Grabber grabber;
  DShowLib::Grabber::tVidCapDevListPtr pVidCapDevList = grabber.getAvailableVideoCaptureDevices();
  DShowLib::ExitLibrary();

  if (pVidCapDevList == 0)
  {
    // no devices are found
    return;
  }
  for (DShowLib::Grabber::tVidCapDevList::iterator it = pVidCapDevList->begin(); it != pVidCapDevList->end(); ++it)
  {
    deviceNames.push_back(it->toString());
  }
}

//----------------------------------------------------------------------------
void vtkPlusICCapturingSource::GetListOfCaptureVideoNorms(std::vector< std::string >& videoNorms, const std::string& deviceName)
{
  videoNorms.clear();

  if (!DShowLib::InitLibrary())
  {
    LOG_ERROR("The IC capturing library could not be initialized");
    return;
  }
  DShowLib::Grabber grabber;
  if (!grabber.openDev(deviceName))
  {
    LOG_ERROR("Could not connect to device: " << deviceName);
    DShowLib::ExitLibrary();
    return;
  }
  DShowLib::Grabber::tVidNrmListPtr pVidNrmList = grabber.getAvailableVideoNorms();
  DShowLib::ExitLibrary();

  if (pVidNrmList == 0)
  {
    LOG_ERROR("Error getting list of capture video norms for device: " << grabber.getLastError().toString());
    return;
  }

  for (DShowLib::Grabber::tVidNrmList::iterator it = pVidNrmList->begin(); it != pVidNrmList->end(); ++it)
  {
    videoNorms.push_back(it->toString());
  }
}

//----------------------------------------------------------------------------
void vtkPlusICCapturingSource::GetListOfCaptureVideoModes(std::vector< std::string >& videoFormats, const std::string& deviceName, const std::string& videoNorm)
{
  videoFormats.clear();

  if (!DShowLib::InitLibrary())
  {
    LOG_ERROR("The IC capturing library could not be initialized");
    return;
  }
  DShowLib::Grabber grabber;
  if (!grabber.openDev(deviceName))
  {
    LOG_ERROR("Could not connect to device: " << deviceName);
    DShowLib::ExitLibrary();
    return;
  }
  if (!grabber.setVideoNorm(videoNorm))
  {
    LOG_ERROR("Failed to set video norm: " << videoNorm);
    DShowLib::ExitLibrary();
    return;
  }
  DShowLib::Grabber::tVidFmtListPtr pVidFmtList = grabber.getAvailableVideoFormats();
  DShowLib::ExitLibrary();

  if (pVidFmtList == 0)
  {
    LOG_ERROR("Error getting list of capture video formats for device " << grabber.getLastError().toString());
    return;
  }

  for (DShowLib::Grabber::tVidFmtList::iterator it = pVidFmtList->begin(); it != pVidFmtList->end(); ++it)
  {
    videoFormats.push_back(it->toString());
  }
}

//----------------------------------------------------------------------------
void vtkPlusICCapturingSource::LogListOfCaptureDevices()
{
  LOG_INFO("Available video formats and frame sizes for all available capture devices:");
  std::vector<std::string> deviceNames;
  GetListOfCaptureDevices(deviceNames);
  for (std::vector<std::string>::iterator deviceNameIt = deviceNames.begin(); deviceNameIt != deviceNames.end(); ++deviceNameIt)
  {
    LOG_INFO(" Device name: " << (*deviceNameIt));
    std::vector<std::string> videoNorms;
    GetListOfCaptureVideoNorms(videoNorms, (*deviceNameIt));
    for (std::vector<std::string>::iterator videoNormIt = videoNorms.begin(); videoNormIt != videoNorms.end(); ++videoNormIt)
    {
      LOG_INFO("  Video norm: " << (*videoNormIt));
      std::vector<std::string> videoModes;
      GetListOfCaptureVideoModes(videoModes, (*deviceNameIt), (*videoNormIt));
      for (std::vector<std::string>::iterator videoModeIt = videoModes.begin(); videoModeIt != videoModes.end(); ++videoModeIt)
      {
        LOG_INFO("   " << (*videoModeIt));
      }
    }
  }
}
