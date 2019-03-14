/*=Plus=header=begin======================================================
    Program: Plus
    Copyright (c) UBC Biomedical Signal and Image Computing Laboratory. All rights reserved.
    See License.txt for details.
    =========================================================Plus=header=end*/
#pragma comment(lib, "listen.lib")
// Local includes
#include "PlusConfigure.h"
#include "PixelCodec.h"
#include "vtkPlusChannel.h"
#include "vtkPlusDataSource.h"
#include "vtkPlusUsImagingParameters.h"
#include "vtkPlusClarius.h"

// VTK includes
#include <vtkImageData.h>
#include <vtkImageImport.h>
#include <vtkObjectFactory.h>
#include <vtkSmartPointer.h>
#include <vtkImageData.h>
#include <vtkXMLUtilities.h>
#include <vtkIGSIOAccurateTimer.h>

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <atomic>
#include <thread>
#include <vector>
#include <fstream>

// OpenCV includes
#include <opencv2/imgproc.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/opencv.hpp>

#include <stdio.h>
#ifdef _MSC_VER
#else
#include <unistd.h>
#endif
// Clarius API
#include "listen.h"

#define BLOCKINGCALL    nullptr
#define DEFAULT_FRAME_WIDTH 640
#define DEFAULT_FRAME_HEIGHT 480
#define BUFFER_SIZE 200
#define DEFAULT_PATH_TO_SEC_KEY "/tmp/"

vtkPlusClarius* vtkPlusClarius::instance;

vtkPlusClarius* vtkPlusClarius::New()
{
  if (instance == NULL)
  {
    instance = new vtkPlusClarius();
  }

  return instance;
}

/* constructor */
vtkPlusClarius::vtkPlusClarius()
  : TcpPort(-1)
  , UdpPort(-1)
  , IpAddress("192.168.1.1")
  , FrameNumber(0)
  , FrameWidth(DEFAULT_FRAME_WIDTH)
  , FrameHeight(DEFAULT_FRAME_HEIGHT)
  , PathToSecKey(DEFAULT_PATH_TO_SEC_KEY)
{  // callback based mechanism
  LOG_TRACE("vtkPlusClarius: Constructor")
  this->StartThreadForInternalUpdates = false;
  this->FrameWidth = DEFAULT_FRAME_WIDTH;
  this->FrameHeight = DEFAULT_FRAME_HEIGHT;
  this->RawImuDataStream.open("RawImus.txt", std::ofstream::app);
  this->RawImuDataStream << "FrameNum,InternalSystemTimestamp,SystemTimeStamp,ImageTimeStamp,ImuTimeStamp,ax,ay,az,gx,gy,gz,mx,my,mz,\n";
  this->RawImuDataStream.close();
  instance = this;
}

/* destructor */
vtkPlusClarius::~vtkPlusClarius()
{
  if (this->Recording) 
  {
    this->StopRecording();
  }

  if (this->Connected) 
  {
    clariusDisconnect(BLOCKINGCALL);
  }

  int destroyed = clariusDestroyListener();
  if (destroyed != 0)
  {
    LOG_ERROR("Error destoying the listener");
  }

  this->instance = NULL;
}

//----------------------------------------------------------------------------
vtkPlusClarius * vtkPlusClarius::GetInstance()
{
  LOG_TRACE("vtkPlusClarius: GetInstance()");
  if (instance != NULL)
  {
    return instance;
  }

  else
  {
    LOG_ERROR("Instance is null, creating new instance");
    instance = new vtkPlusClarius();
    return instance;
  }
}

//----------------------------------------------------------------------------
void vtkPlusClarius::PrintSelf(ostream& os, vtkIndent indent) 
{
  this->Superclass::PrintSelf(os, indent);
  os << indent << "ipAddress" << this->IpAddress << std::endl;
  os << indent << "tcpPort" << this->TcpPort << std::endl;
  os << indent << "UdpPort" << this->UdpPort << std::endl;
  os << indent << "FrameNumber" << this->FrameNumber << std::endl;
  os << indent << "FrameWidth" << this->FrameWidth << std::endl;
  os << indent << "FrameHeight" << this->FrameHeight << std::endl;
}

/*! Read configuration from xml data */
PlusStatus vtkPlusClarius::ReadConfiguration(vtkXMLDataElement* rootConfigElement) 
{
  LOG_TRACE("vtkPlusClarius::ReadConfiguration");
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_READING(deviceConfig, rootConfigElement);
  // this function calls the the ReadConfiguration function in the SuperClass (which is PlusDevice in this case)
  XML_READ_STRING_ATTRIBUTE_REQUIRED(IpAddress, deviceConfig);
  XML_READ_SCALAR_ATTRIBUTE_REQUIRED(int, TcpPort, deviceConfig);
  // Clarius uses a callback mechanism, does not require acquisition rate.
  // if not specified, the default value for FrameWidth is 640 and FrameHeight is 480 according to clarius;
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(int, FrameWidth, deviceConfig);
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(int, FrameHeight, deviceConfig);
  XML_FIND_NESTED_ELEMENT_REQUIRED(dataSourcesElement, deviceConfig, "DataSources");
  return PLUS_SUCCESS;
}

/*! Write configuration to xml data
 *  This is responsible for recording the current state of your device to XML.
 *  This can be called at any time.*/
PlusStatus vtkPlusClarius::WriteConfiguration(vtkXMLDataElement* rootConfigElement) 
{
  LOG_TRACE("vtkPlusClarius::WriteConfiguration");
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_WRITING(deviceConfig, rootConfigElement);

  deviceConfig->SetIntAttribute("TcpPort", this->TcpPort);
  // deviceConfig->SetAttribute("IpAddress", this->IpAddress);

  return PLUS_SUCCESS;
}


/*! Perform any completion tasks once configured
 * a multi-purpose function which is called after all devices have been configured,
 * all inputs and outputs have been connected between devices,
 * but before devices begin collecting data.
 * This is the last chance for your device to raise an error about improper or insufficient configuration.
 */
PlusStatus vtkPlusClarius::NotifyConfigured()
{
  LOG_TRACE("vtkPlusClarius::NotifyConfigured");
  vtkPlusClarius* device = vtkPlusClarius::GetInstance();
  if (device->OutputChannels.size() > 1)
  {
    LOG_WARNING("vtkPlusClarius is expecting one output channel and there are " <<
      this->OutputChannels.size() << " channels. First output channel will be used.");
  }

  if (device->OutputChannels.empty())
  {
    LOG_ERROR("No output channels defined for vtkPlusClarius. Cannot proceed.");
    this->CorrectlyConfigured = false;
    return PLUS_FAIL;
  }

  if (device->OutputChannels.empty())
  {
    LOG_ERROR("No output channels defined for vtkPlusClarius");
  }

  std::vector< vtkPlusDataSource*> sources;
  sources = device->GetVideoSources();
  if (sources.size() != 1)
  {
    LOG_ERROR("More than one output source found");
  }

  vtkPlusDataSource* aSource = sources[0];
  aSource->SetBufferSize(BUFFER_SIZE);
  return PLUS_SUCCESS;
}

/*!
 Probe to see to see if the device is connected to the
 computer.  This method should be overridden in subclasses.
 */
PlusStatus vtkPlusClarius::Probe() 
{
  LOG_TRACE("vtkPlusClarius: Probe");
  if (this->UdpPort == -1)
  {
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
};

/*! Hardware device SDK version. This method should be overridden in subclasses. */
std::string vtkPlusClarius::vtkPlusClarius::GetSdkVersion()
{
  std::ostringstream version;
  version << "Sdk version not available" << "\n";
  return version.str();
}

/* Device-specific connect */
PlusStatus vtkPlusClarius::InternalConnect()
{
  LOG_DEBUG("vtkPlusClarius: InternalConnect");
  vtkPlusClarius* device = vtkPlusClarius::GetInstance();
  // Initialize Clarius Listener Before Connecting
  if (!device->Connected) 
  {
    int argc = 1;
    char** argv = new char *[1];
    argv[0] = new char[4];
    strcpy(argv[0], "abc");
    const char *path = device->PathToSecKey.c_str();
    ClariusNewImageFn SaveDataCallBackPtr = static_cast<ClariusNewImageFn>(&vtkPlusClarius::SaveDataCallback);
    ClariusFreezeFn FreezeCallBackFnPtr = static_cast<ClariusFreezeFn>(&vtkPlusClarius::FreezeFn);
    ClariusProgressFn ProgressCallBackFnPtr = static_cast<ClariusProgressFn>(&vtkPlusClarius::ProgressFn);
    ClariusErrorFn ErrorCallBackFnPtr = static_cast<ClariusErrorFn>(&vtkPlusClarius::ErrorFn);
    try 
    {
      if (clariusInitListener(argc, argv, path,
        SaveDataCallBackPtr,
        FreezeCallBackFnPtr,
        ProgressCallBackFnPtr,
        ErrorCallBackFnPtr,
        BLOCKINGCALL) < 0)
      {
        return PLUS_FAIL;
      }
    }

    catch (const std::runtime_error& re)
    {
      LOG_ERROR("Runtime error: " << re.what());
      return PLUS_FAIL;
    }

    catch (const std::exception& ex)
    {
      LOG_ERROR("Error occurred: " << ex.what());
      return PLUS_FAIL;
    }

    catch (...)
    {
      LOG_ERROR("Unknown failure occured");
      return PLUS_FAIL;
    }

    // Attempt to connect;
    int isConnected = -1;
    const char *ip = device->IpAddress.c_str();
    try {
      isConnected = clariusConnect(ip, device->TcpPort, BLOCKINGCALL);
    }

    catch (const std::runtime_error& re)
    {
      LOG_ERROR("Runtime error: " << re.what());
      return PLUS_FAIL;
    }

    catch (const std::exception& ex)
    {
      LOG_ERROR("Error occurred: " << ex.what());
      return PLUS_FAIL;
    }

    catch (...) 
    {
      LOG_ERROR("Unknown failure occured");
      return PLUS_FAIL;
    }

    if (isConnected < 0)
    {
      LOG_ERROR("Could not connect to scanner at Ip = " << ip << " port number= " << device->TcpPort << " isConnected = " << isConnected);
      return PLUS_FAIL;
    }

    device->UdpPort = clariusGetUdpPort();
    if (device->UdpPort != -1) 
    {
      LOG_DEBUG("... Clarius device connected, streaming port: " << clariusGetUdpPort());
      if (clariusSetOutputSize(device->FrameWidth, device->FrameHeight) < 0) 
      {
        LOG_DEBUG("Clarius Output size can not be set, falling back to default 640*480");
        device->FrameWidth = DEFAULT_FRAME_WIDTH;
        device->FrameHeight = DEFAULT_FRAME_HEIGHT;
      }
      
      device->Connected = 1;
      return PLUS_SUCCESS;
    }

    else 
    {
      LOG_ERROR("... Clarius device connected but could not get valid udp port");
      return PLUS_FAIL;
    }

  }

  else 
  {
    LOG_DEBUG("Scanner already connected to IP address=" << device->IpAddress
      << " TCP Port Number =" << device->TcpPort << "Streaming Image at UDP Port=" << device->UdpPort);
    device->Connected = 1;
    return PLUS_SUCCESS;
  }
};

PlusStatus vtkPlusClarius::CheckOutputChannels()
{
  LOG_TRACE("vtkPlusClarius: CheckOutputChannels");
  if (instance->GetNumberOfOutputPorts() <= 0)
  {
    LOG_ERROR("instance->GetNumberOfOutpuPorts == " << instance->GetNumberOfOutputPorts());
    return PLUS_FAIL;
  }

  else 
  {
    return PLUS_SUCCESS;
  }
}

/* Device-specific on-update function */
PlusStatus vtkPlusClarius::InternalUpdate() 
{
  LOG_TRACE("vtkPlusClarius: InternalUpdate");
  LOG_ERROR("InternalUpdate() should never be called");
  return PLUS_SUCCESS;
};

/* Device-specific disconnect */
PlusStatus vtkPlusClarius::InternalDisconnect()
{
  LOG_DEBUG("vtkPlusClarius: InternalDisconnect");
  vtkPlusClarius* device = vtkPlusClarius::GetInstance();
  if (device->GetConnected()) 
  {
    if (clariusDisconnect(nullptr) < 0) 
    {
      LOG_ERROR("could not disconnect from scanner");
      return PLUS_FAIL;
    }
    else 
    {
      device->Connected = 0;
      LOG_DEBUG("Clarius device is now disconnected");
      return PLUS_SUCCESS;
    }
  }
  else 
  {
    LOG_DEBUG("...Clarius device already disconnected");
    return PLUS_SUCCESS;
  }
};

/* Record incoming data at the specified acquisition rate.  The recording
 * continues indefinitely until StopRecording() is called.
*/
PlusStatus vtkPlusClarius::InternalStartRecording() 
{
  LOG_TRACE("vtkPlusClarius: InternalStartRecording");
  // Clarius uses a callback mechanism, doesn't need this;
  return PLUS_SUCCESS;
}

/*! Stop recording */
PlusStatus vtkPlusClarius::InternalStopRecording() 
{
  LOG_TRACE("vtkPlusClarius: InternalStopRecording");
  // Clarius uses a callback mechanism, doesn't need this;
  return PLUS_SUCCESS;
}

/*! callback for error messages
 * @param[in] err the error message sent from the listener module
 * */
void vtkPlusClarius::ErrorFn(const char *err) 
{
  LOG_ERROR("error: " << err);
}

/*! callback for freeze state change
 * @param[in] val the freeze state value 1 = frozen, 0 = imaging */
void vtkPlusClarius::FreezeFn(int val) 
{
  if (val) 
  {
    LOG_INFO("Clarius Frozen");
  }

  else 
  {
    LOG_INFO("Clarius Imaging");
  }
}

/*! callback for readback progress
 * @pram[in] progress the readback process*/
void vtkPlusClarius::ProgressFn(int progress)
{
  LOG_INFO("download: " << progress);
}

/*! callback for a new image sent from the scanner
 * @param[in] newImage a pointer to the raw image bits of
 * @param[in] nfo the image properties
 * @param[in] npos the # fo positional data points embedded with the frame
 * @param[in] pos the buffer of positional data
 * */
void vtkPlusClarius::NewImageFn(const void *newImage, const ClariusImageInfo *nfo, int npos, const ClariusPosInfo* pos) {
  LOG_TRACE("new image (" << newImage << "): " << nfo->width << " x " << nfo->height << " @ " << nfo->bitsPerPixel
    << "bits. @ " << nfo->micronsPerPixel << " microns per pixel. imu points: " << npos);
  if (npos) 
  {
    for (auto i = 0; i < npos; i++)
    {
      LOG_TRACE("imu: " << i << ", time: " << pos[i].tm);
      LOG_TRACE("accel: " << pos[i].ax << "," << pos[i].ay << "," << pos[i].az);
      LOG_TRACE("gyro: " << pos[i].gx << "," << pos[i].gy << "," << pos[i].gz);
      LOG_TRACE("magnet: " << pos[i].mx << "," << pos[i].my << "," << pos[i].mz);
    }

  }
}

//----------------------------------------------------------------------------
void vtkPlusClarius::SaveDataCallback(const void *newImage, const ClariusImageInfo *nfo, int npos, const ClariusPosInfo *pos) {
  LOG_TRACE("vtkPlusClarius::SaveDataCallback");
  vtkPlusClarius* device = vtkPlusClarius::GetInstance();
  if (device == NULL)
  {
    LOG_ERROR("Clarius instance is NULL!!!");
  }

  // Check if still connected
  if (device->Connected == 0)
  {
    LOG_ERROR("Trouble connecting to Clarius Device. IpAddress = " << device->IpAddress
      << " port = " << device->TcpPort);
  }

  if (newImage == NULL) 
  {
    LOG_ERROR("no frame received by the device");
  }

  // check if device has output channels;
  if (device->OutputChannels.empty())
  {
    LOG_ERROR("No output channels defined for vtkPlusClarius");
  }

  vtkPlusChannel* outputChannel = device->OutputChannels[0];
  // Check if ouput channel has data source
  vtkPlusDataSource* aSource(NULL);
  if (outputChannel->GetVideoSource(aSource) != PLUS_SUCCESS)
  {
    LOG_ERROR("Unable to retrieve the video source in the vtkPlusClarius device.");
  }

  // check if there exist active data source;
  vtkPlusDataSource* activeDataSource;
  if (device->GetFirstActiveOutputVideoSource(activeDataSource) != PLUS_SUCCESS) 
  {
    LOG_ERROR("Unable to retrieve active data source.");
  }

  // check if the output channel's data source is the one currently active;
  if (activeDataSource != aSource) 
  {
    LOG_ERROR("active data source and output channel data source are different");
  }

  PlusCommon::VTKScalarPixelType pixelType = VTK_UNSIGNED_INT;
  US_IMAGE_TYPE imgType = US_IMG_BRIGHTNESS;
  if (aSource != nullptr) 
  {
    // Set Image Properties
    int frameSize[3] = { nfo->width, nfo->height, 1};
    aSource->SetInputFrameSize(frameSize[0], frameSize[1], frameSize[2]);
    int numberOfBytesToSkip = 0;
    int frameBufferBytesPerPixel = (nfo->bitsPerPixel / 8);
    aSource->SetNumberOfScalarComponents(frameBufferBytesPerPixel);
    if (aSource->GetNumberOfBytesPerPixel() != frameBufferBytesPerPixel) 
    {
      LOG_ERROR("aSource->GetNumberOfBytesPerPixel() != frameBufferBytesPerPixel");
    }
    /*
    LOG_TRACE("Frame size: " << frameSize[0] << "x" << frameSize[1]
      << " FrameBufferBytesPerPixel" << frameBufferBytesPerPixel
      << " NumberOfScalarComponents: " << aSource->GetNumberOfScalarComponents()
      << " Image timeStamp in nanoseconds: " << nfo->tm);
    */
    // deep copy the image to unsigned char
    std::vector<char> _image;
    size_t img_sz = nfo->width * nfo->height * (nfo->bitsPerPixel / 8);
    if (_image.size() < img_sz) 
    {
      _image.resize(img_sz);
    }

    memcpy(_image.data(), newImage, img_sz);

    // create cvimg to write to disk
    cv::Mat cvimg = cv::Mat(frameSize[0], frameSize[1], CV_8UC4);
    int frameSizeInBytes = nfo->width * nfo->height * (nfo->bitsPerPixel / 8);
    int frameNum = device->FrameNumber;
    cvimg.data = (unsigned char *)_image.data();
    double timestamp = static_cast<double>((double)nfo->tm / (double)1000000000);
    /*
    // uncomment the following to write the images to disk
    if (cv::imwrite("Clarius_cvImage" + std::to_string(timestamp) + ".bmp", cvimg) == false) 
    {
      LOG_ERROR("ERROR writing cvimg.jpg to file");
    }
    */
    // Get system time (elapsed time since last reboot), return Internal system time in seconds
    double internalSystemTime = vtkIGSIOAccurateTimer::GetInternalSystemTime();
    double systemTime = vtkIGSIOAccurateTimer::GetSystemTime();
    LOG_TRACE("timestamp: " << nfo->tm 
        << "; double timestamp " << timestamp 
        << "; System Time" << systemTime 
        << " Internal System Time " <<internalSystemTime << ":");
    if (npos != 0) 
    {
      device->WritePosesToCsv(nfo, npos, pos, device->FrameNumber, internalSystemTime, systemTime);
    }

    aSource->AddItem(
      cvimg.data, // pointer to char array
      aSource->GetInputImageOrientation(), // refer to this url: http://perk-software.cs.queensu.ca/plus/doc/nightly/dev/UltrasoundImageOrientation.html for reference;
                         // Set to UN to keep the orientation of the image the same as on tablet
      aSource->GetInputFrameSize(), // integer array with length == 3, the frame size in the x, y and z axis
      pixelType,  // set to VTK_UNSIGNED_INT because the images have 32bits per pixel
      cvimg.channels(), // bytes per pixel
      imgType, // US_IMG_BRIGHTNESS for grayscale image
      numberOfBytesToSkip, // 0
      device->FrameNumber,
      timestamp, // device timestamp in seconds -- should this be set to system timestamps?
      timestamp);
    (device->FrameNumber)++;
  }

  else 
  {
    LOG_ERROR("Image Source not found");
    return;
  }
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusClarius::WritePosesToCsv(const ClariusImageInfo *nfo, int npos, const ClariusPosInfo* pos, int frameNum, double internalSystemTime, double systemTime) {
  LOG_DEBUG("vtkPlusClarius::WritePosesToCsv(const ClariusImageInfo *nfo, int npos, const ClariusPosInfo* pos, int frameNum)");
  if (npos != 0)
  {
    LOG_TRACE("timestamp in nanoseconds ClariusPosInfo" << pos[0].tm);
    std::string posInfo = "";
    for (auto i = 0; i < npos; i++)
    {
      posInfo += (std::to_string(frameNum) + ",");
      posInfo += (std::to_string(internalSystemTime) + ",");
      posInfo += (std::to_string(systemTime) + ",");
      posInfo += (std::to_string(nfo->tm) + ",");
      posInfo += (std::to_string(pos[i].tm) + ",");
      posInfo += (std::to_string(pos[i].ax) + ",");
      posInfo += (std::to_string(pos[i].ay) + ",");
      posInfo += (std::to_string(pos[i].az) + ",");
      posInfo += (std::to_string(pos[i].gx) + ",");
      posInfo += (std::to_string(pos[i].gy) + ",");
      posInfo += (std::to_string(pos[i].gz) + ",");
      posInfo += (std::to_string(pos[i].mx) + ",");
      posInfo += (std::to_string(pos[i].my) + ",");
      posInfo += (std::to_string(pos[i].mz) + ",");
      posInfo += "\n";
    }

    // write the string to file
    this->RawImuDataStream.open("RawImus.txt", std::ofstream::app);
    if (this->RawImuDataStream.is_open() == false) 
    {
      LOG_ERROR("Error opening file for raw imu data");
      return PLUS_FAIL;
    }

    this->RawImuDataStream << posInfo;
    this->RawImuDataStream.close();
    return PLUS_SUCCESS;
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusClarius::SetFrameHeight(int FrameHeight)
{
  vtkPlusClarius* device = vtkPlusClarius::GetInstance();
  device->FrameHeight = FrameHeight;
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusClarius::SetFrameWidth(int FrameWidth)
{
  vtkPlusClarius* device = vtkPlusClarius::GetInstance();
  device->FrameWidth = FrameWidth;
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusClarius::SetIpAddress(std::string IpAddress)
{
  vtkPlusClarius* device = vtkPlusClarius::GetInstance();
  device->IpAddress = IpAddress;
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusClarius::SetTcpPort(unsigned int TcpPort)
{
  vtkPlusClarius* device = vtkPlusClarius::GetInstance();
  device->TcpPort = TcpPort;
  return PLUS_SUCCESS;
}
