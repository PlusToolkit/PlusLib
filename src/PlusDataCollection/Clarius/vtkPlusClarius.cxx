/*=Plus=header=begin======================================================
    Program: Plus
    Copyright (c) UBC Biomedical Signal and Image Computing Laboratory. All rights reserved.
    See License.txt for details.
=========================================================Plus=header=end*/

// Local includes
#include "PlusConfigure.h"
#include "PixelCodec.h"
#include "vtkPlusChannel.h"
#include "vtkPlusDataSource.h"
#include "vtkPlusUsImagingParameters.h"
#include "vtkPlusClarius.h"

// VTK includes
#include <vtk_zlib.h>
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

//----------------------------------------------------------------------------

vtkPlusClarius * vtkPlusClarius::instance;

//----------------------------------------------------------------------------
vtkPlusClarius* vtkPlusClarius::New()
{
  if (instance == NULL)
  {
    instance = new vtkPlusClarius();
  }

  return instance;
}

//----------------------------------------------------------------------------
vtkPlusClarius::vtkPlusClarius()
  : TcpPort(-1)
  , UdpPort(-1)
  , IpAddress("192.168.1.1")
  , FrameNumber(0)
  , FrameWidth(DEFAULT_FRAME_WIDTH)
  , FrameHeight(DEFAULT_FRAME_HEIGHT)
  , PathToSecKey(DEFAULT_PATH_TO_SEC_KEY)
  , ImuEnabled(false)
  , ImuOutputFileName("ClariusImuData.csv")
  , SystemStartTimestamp(0)
  , ClariusStartTimestamp(0)
  , ClariusLastTimestamp(0)
  , WriteImagesToDisk(false)
  , CompressRawData(false)
  , IsReceivingRawData(false)
  , RawDataPointer(nullptr)
{
  LOG_TRACE("vtkPlusClarius: Constructor");
  this->StartThreadForInternalUpdates = false;
  this->FrameWidth = DEFAULT_FRAME_WIDTH;
  this->FrameHeight = DEFAULT_FRAME_HEIGHT;

  instance = this;
}

//----------------------------------------------------------------------------
vtkPlusClarius::~vtkPlusClarius()
{
  this->AllocateRawData(-1);

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
vtkPlusClarius* vtkPlusClarius::GetInstance()
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

//----------------------------------------------------------------------------
PlusStatus vtkPlusClarius::ReadConfiguration(vtkXMLDataElement* rootConfigElement)
{
  LOG_TRACE("vtkPlusClarius::ReadConfiguration");
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_READING(deviceConfig, rootConfigElement);

  XML_READ_STRING_ATTRIBUTE_REQUIRED(IpAddress, deviceConfig);
  XML_READ_SCALAR_ATTRIBUTE_REQUIRED(int, TcpPort, deviceConfig);
  // if not specified, the default value for FrameWidth is 640 and FrameHeight is 480 according to clarius;
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(int, FrameWidth, deviceConfig);
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(int, FrameHeight, deviceConfig);
  XML_READ_BOOL_ATTRIBUTE_OPTIONAL(ImuEnabled, deviceConfig);
  XML_READ_BOOL_ATTRIBUTE_OPTIONAL(WriteImagesToDisk, deviceConfig);
  if (this->ImuEnabled)
  {
    XML_READ_STRING_ATTRIBUTE_REQUIRED(ImuOutputFileName, deviceConfig);
  }
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusClarius::WriteConfiguration(vtkXMLDataElement* rootConfigElement)
{
  LOG_TRACE("vtkPlusClarius::WriteConfiguration");
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_WRITING(deviceConfig, rootConfigElement);

  XML_WRITE_STRING_ATTRIBUTE(IpAddress, deviceConfig);
  deviceConfig->SetIntAttribute("TcpPort", this->TcpPort);
  deviceConfig->SetIntAttribute("FrameWidth", this->FrameWidth);
  deviceConfig->SetIntAttribute("FrameHeight", this->FrameHeight);
  XML_WRITE_BOOL_ATTRIBUTE(ImuEnabled, deviceConfig);
  if (this->ImuEnabled)
  {
    XML_WRITE_STRING_ATTRIBUTE(ImuOutputFileName, deviceConfig);
  }
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
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

  std::vector<vtkPlusDataSource*> sources;
  sources = device->GetVideoSources();
  if (sources.size() > 1)
  {
    LOG_WARNING("More than one output video source found. First will be used");
  }
  if (sources.size() == 0)
  {
    LOG_ERROR("Video source required in configuration. Cannot proceed.");
    return PLUS_FAIL;
  }

  // Check if output channel has data source
  vtkPlusDataSource* aSource(NULL);
  if (device->OutputChannels[0]->GetVideoSource(aSource) != PLUS_SUCCESS)
  {
    LOG_ERROR("Unable to retrieve the video source in the vtkPlusClarius device.");
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusClarius::Probe()
{
  LOG_TRACE("vtkPlusClarius: Probe");
  if (this->UdpPort == -1)
  {
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
};

//----------------------------------------------------------------------------
std::string vtkPlusClarius::vtkPlusClarius::GetSdkVersion()
{
  std::ostringstream version;
  version << "Sdk version not available" << "\n";
  return version.str();
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusClarius::InternalConnect()
{
  LOG_DEBUG("vtkPlusClarius: InternalConnect");

  if (this->ImuEnabled)
  {
    this->RawImuDataStream.open(this->ImuOutputFileName, std::ofstream::app);
    this->RawImuDataStream << "FrameNum,SystemTimestamp,ConvertedTimestamp,ImageTimestamp,ImuTimeStamp,ax,ay,az,gx,gy,gz,mx,my,mz,\n";
    this->RawImuDataStream.close();
  }

  vtkPlusClarius* device = vtkPlusClarius::GetInstance();
  // Initialize Clarius Listener Before Connecting
  if (!device->Connected)
  {
    int argc = 1;
    char** argv = new char* [1];
    argv[0] = new char[4];
    strcpy(argv[0], "abc");
    const char* path = device->PathToSecKey.c_str();
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
    const char* ip = device->IpAddress.c_str();
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

//----------------------------------------------------------------------------
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

//----------------------------------------------------------------------------
/*! callback for error messages
 * @param[in] err the error message sent from the listener module
 * */
void vtkPlusClarius::ErrorFn(const char* err)
{
  LOG_ERROR("error: " << err);
}

//----------------------------------------------------------------------------
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

//----------------------------------------------------------------------------
/*! callback for readback progress
 * @pram[in] progress the readback process*/
void vtkPlusClarius::ProgressFn(int progress)
{
  LOG_DEBUG("Download: " << progress << "%");
}

//----------------------------------------------------------------------------
/*! callback for a new image sent from the scanner
 * @param[in] newImage a pointer to the raw image bits of
 * @param[in] nfo the image properties
 * @param[in] npos the # fo positional data points embedded with the frame
 * @param[in] pos the buffer of positional data
 * */
void vtkPlusClarius::NewImageFn(const void* newImage, const ClariusImageInfo* nfo, int npos, const ClariusPosInfo* pos)
{
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
void vtkPlusClarius::SaveDataCallback(const void* newImage, const ClariusImageInfo* nfo, int npos, const ClariusPosInfo* pos)
{
  LOG_TRACE("vtkPlusClarius::SaveDataCallback");
  vtkPlusClarius* device = vtkPlusClarius::GetInstance();
  if (device == NULL)
  {
    LOG_ERROR("Clarius instance is NULL!!!");
    return;
  }

  // Check if still connected
  if (device->Connected == 0)
  {
    LOG_ERROR("Trouble connecting to Clarius Device. IpAddress = " << device->IpAddress
      << " port = " << device->TcpPort);
    return;
  }

  if (newImage == NULL)
  {
    LOG_ERROR("No frame received by the device");
    return;
  }

  // check if there exist active data source;
  vtkPlusDataSource* aSource;
  if (device->GetFirstActiveOutputVideoSource(aSource) != PLUS_SUCCESS)
  {
    LOG_ERROR("Unable to retrieve active data source.");
    // Cannot return fail because the callback function has to return void
    return;
  }

  // Set Image Properties
  aSource->SetInputFrameSize(nfo->width, nfo->height, 1);
  int frameBufferBytesPerPixel = (nfo->bitsPerPixel / 8);
  int frameSizeInBytes = nfo->width * nfo->height * frameBufferBytesPerPixel;
  aSource->SetNumberOfScalarComponents(frameBufferBytesPerPixel);

  // need to copy newImage to new char vector vtkDataSource::AddItem() do not accept const char array
  std::vector<char> _image;
  size_t img_sz = nfo->width * nfo->height * (nfo->bitsPerPixel / 8);
  if (_image.size() < img_sz)
  {
    _image.resize(img_sz);
  }
  memcpy(_image.data(), newImage, img_sz);
  // the clarius timestamp is in nanoseconds
  device->ClariusLastTimestamp = static_cast<double>((double)nfo->tm / (double)1000000000);
  // Get system time (elapsed time since last reboot), return Internal system time in seconds
  double systemTime = vtkIGSIOAccurateTimer::GetSystemTime();
  if (device->FrameNumber == 0)
  {
    device->SystemStartTimestamp = systemTime;
    device->ClariusStartTimestamp = device->ClariusLastTimestamp;
  }

  // The timestamp that each image is tagged with is
  // (system_start_time + current_clarius_time - clarius_start_time)
  double converted_timestamp = device->SystemStartTimestamp + (device->ClariusLastTimestamp - device->ClariusStartTimestamp);
  if (npos != 0)
  {
    device->WritePosesToCsv(nfo, npos, pos, device->FrameNumber, systemTime, converted_timestamp);
  }
  if (device->WriteImagesToDisk)
  {
    // create cvimg to write to disk
    cv::Mat cvimg = cv::Mat(nfo->width, nfo->height, CV_8UC4);
    cvimg.data = cvimg.data = (unsigned char*)_image.data();
    if (cv::imwrite("Clarius_Image" + std::to_string(device->ClariusLastTimestamp) + ".bmp", cvimg) == false)
    {
      LOG_ERROR("ERROR writing clarius image" + std::to_string(device->ClariusLastTimestamp) + " to disk");
    }
  }
  aSource->AddItem(
    _image.data(), // pointer to char array
    aSource->GetInputImageOrientation(), // refer to this url: http://perk-software.cs.queensu.ca/plus/doc/nightly/dev/UltrasoundImageOrientation.html for reference;
                                         // Set to UN to keep the orientation of the image the same as on tablet
    aSource->GetInputFrameSize(),
    VTK_UNSIGNED_CHAR,
    frameBufferBytesPerPixel,
    US_IMG_BRIGHTNESS,
    0,
    device->FrameNumber,
    converted_timestamp,
    converted_timestamp);
  device->FrameNumber++;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusClarius::WritePosesToCsv(const ClariusImageInfo* nfo, int npos, const ClariusPosInfo* pos, int frameNum, double systemTime, double convertedTime)
{
  LOG_TRACE("vtkPlusClarius::WritePosesToCsv");
  if (npos != 0)
  {
    LOG_TRACE("timestamp in nanoseconds ClariusPosInfo" << pos[0].tm);
    std::string posInfo = "";
    for (auto i = 0; i < npos; i++)
    {
      posInfo += (std::to_string(frameNum) + ",");
      posInfo += (std::to_string(systemTime) + ",");
      posInfo += (std::to_string(convertedTime) + ",");
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
    this->RawImuDataStream.open(this->ImuOutputFileName, std::ofstream::app);
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
PlusStatus vtkPlusClarius::RequestLastNSecondsRawData(double lastNSeconds)
{
  if (lastNSeconds <= 0)
  {
    return this->RequestRawData(0, 0);
  }

  LOG_INFO("Requesting raw data for last " << lastNSeconds << " seconds");
  long long endTimestamp = static_cast<double>((long long)1000000000 * this->ClariusLastTimestamp);
  long long lastNNanoSeconds = static_cast<double>((long long)1000000000 * lastNSeconds);
  long long startTimestamp = std::max((long long)0, endTimestamp - lastNNanoSeconds);
  return this->RequestRawData(startTimestamp, endTimestamp);
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusClarius::RequestRawData(long long startTimestamp, long long endTimestamp)
{
  if (this->IsReceivingRawData)
  {
    LOG_ERROR("Receive data already in progress!");
    return PLUS_FAIL;
  }

  if (startTimestamp < 0 || endTimestamp < 0)
  {
    LOG_ERROR("Start and end timestamps must be > 0 nanoseconds");
  }

  if (startTimestamp == 0 && endTimestamp == 0)
  {
    LOG_INFO("Requesting all available raw data");
  }
  else
  {
    LOG_INFO("Requesting raw data between " << startTimestamp << "ns and " << endTimestamp << "ns");
  }

  this->IsReceivingRawData = true;

  ClariusReturnFn returnFunction = (ClariusReturnFn)(&vtkPlusClarius::RawDataRequestFn);
  clariusRequestRawData(startTimestamp, endTimestamp, returnFunction);
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
void vtkPlusClarius::RawDataRequestFn(int rawDataSize)
{
  vtkPlusClarius* self = vtkPlusClarius::GetInstance();

  if (rawDataSize < 0)
  {
    self->IsReceivingRawData = false;
    LOG_ERROR("Error requesting raw data!");
    return;
  }

  if (rawDataSize == 0)
  {
    self->IsReceivingRawData = false;
    LOG_TRACE("No data to read!");
    return;
  }

  self->ReceiveRawData(rawDataSize);
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusClarius::ReceiveRawData(int dataSize)
{
  LOG_INFO("Receiving " << dataSize << " bytes of raw data");

  ClariusReturnFn returnFunction = (ClariusReturnFn)(&vtkPlusClarius::RawDataWriteFn);
  this->AllocateRawData(dataSize);
  clariusReadRawData(&this->RawDataPointer, returnFunction);
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
void vtkPlusClarius::RawDataWriteFn(int retCode)
{
  vtkPlusClarius* self = vtkPlusClarius::GetInstance();
  self->IsReceivingRawData = false;

  if (retCode < 0)
  {
    LOG_ERROR("Could not read raw data!");
    return;
  }

  LOG_INFO("Raw data received successfully!");

  std::string filename = self->GetRawDataOutputFilename();
  if (filename.empty())
  {
    filename = vtkIGSIOAccurateTimer::GetDateAndTimeString() + "_ClariusData.tar";
  }

  if (self->CompressRawData && vtksys::SystemTools::GetFilenameLastExtension(filename) != ".gz")
  {
    filename = filename + ".gz";
  }

  if (!vtksys::SystemTools::FileIsFullPath(filename.c_str()))
  {

    filename = vtkPlusConfig::GetInstance()->GetOutputDirectory() + "/" + filename;
  }

  if (self->CompressRawData)
  {
    gzFile file = gzopen(filename.c_str(), "wb");
    gzwrite(file, self->RawDataPointer, self->RawDataSize);
    gzclose(file);
  }
  else
  {
    FILE* file = fopen(filename.c_str(), "wb");
    fwrite((char*)self->RawDataPointer, 1, self->RawDataSize, file);
    fclose(file);
  }

  LOG_INFO("Raw data saved as: " << filename);
}

//----------------------------------------------------------------------------
void vtkPlusClarius::AllocateRawData(int dataSize)
{
  if (this->RawDataPointer)
  {
    delete[] this->RawDataPointer;
    this->RawDataPointer = nullptr;
  }

  if (dataSize > 0)
  {
    this->RawDataPointer = new char[dataSize];
  }
  this->RawDataSize = dataSize;
}
