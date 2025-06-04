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

// IGSIO includes
#include <vtkIGSIOAccurateTimer.h>
#include <igsioMath.h>

// VTK includes
#include <vtk_zlib.h>
#include <vtkImageData.h>
#include <vtkImageImport.h>
#include <vtkMatrix4x4.h>
#include <vtkObjectFactory.h>
#include <vtkSmartPointer.h>
#include <vtkTransform.h>
#include <vtkXMLUtilities.h>

// std includes
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <atomic>
#include <thread>
#include <vector>
#include <fstream>

// vtkxio includes
#include "MadgwickAhrsAlgo.h"
#include "MahonyAhrsAlgo.h"

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
#include <cast.h>

#define BLOCKINGCALL    nullptr
#define DEFAULT_FRAME_WIDTH 640
#define DEFAULT_FRAME_HEIGHT 480
#define DEFAULT_PATH_TO_SEC_KEY "/tmp/"

//-------------------------------------------------------------------------------------------------
// vtkInternal
//-------------------------------------------------------------------------------------------------

class vtkPlusClarius::vtkInternal
{
public:
  vtkInternal(vtkPlusClarius* external);

  virtual ~vtkInternal() = default;
  friend class vtkPlusClarius;

protected:

  //////////////////////
  // Clarius callback functions

  static void ErrorFn(const char* err);
  static void FreezeFn(int val);
  static void ProgressFn(int progress);
  static void ButtonFn(CusButton button, int clicks);

  /*!
  Image callback functions
  */
  static void NewProcessedImageFn(const void* newImage, const CusProcessedImageInfo* nfo, int npos, const CusPosInfo* pos);
  static void NewRawImageFn(const void* newImage, const CusRawImageInfo* nfo, int npos, const CusPosInfo* pos);

  /*!
  Callback function used when connecting
  Input value is the udpPort.
  */
  static void ConnectReturnFn(int udpPort, int imuPort, int swRevMatch);

  /*!
  Callback function for raw data request
  */
  static void RawDataRequestFn(int rawDataSize, const char* extension);

  /*!
  Callback function for raw data read
  */
  static void RawDataWriteFn(int rawDataSize);

  /*!
  Re-allocate memory to store raw ultrasound data
  */
  void AllocateRawData(int size);

  /*!
  Receive previously requested data
  */
  PlusStatus ReceiveRawData(int dataSize);

  /*!
  Write IMU data to csv
  */
  PlusStatus WritePosesToCsv(const CusProcessedImageInfo* nfo, int npos, const CusPosInfo* pos, int frameNum, double systemTime, double convertedTime);

  /*!
  Add position info to buffer
  */
  PlusStatus ProcessPositionInfo(int nfo, const CusPosInfo* posInfo);

  /*!
  Convert timestamps
  */
  static double NanoSecondsToSeconds(long long int ns);
  static long long int SecondsToNanoSeconds(double s);

protected:
  vtkPlusClarius* External;

  vtkPlusDataSource* AccelerometerTool;
  vtkPlusDataSource* GyroscopeTool;
  vtkPlusDataSource* MagnetometerTool;
  vtkPlusDataSource* TiltSensorTool;
  vtkPlusDataSource* FilteredTiltSensorTool;
  vtkPlusDataSource* OrientationSensorTool;

  vtkNew<vtkMatrix4x4> LastAccelerometerToTrackerTransform;
  vtkNew<vtkMatrix4x4> LastGyroscopeToTrackerTransform;
  vtkNew<vtkMatrix4x4> LastMagnetometerToTrackerTransform;
  vtkNew<vtkMatrix4x4> LastTiltSensorToTrackerTransform;
  vtkNew<vtkMatrix4x4> LastFilteredTiltSensorToTrackerTransform;
  vtkNew<vtkMatrix4x4> LastOrientationSensorToTrackerTransform;

  AhrsAlgo* FilteredTiltSensorAhrsAlgo;
  AhrsAlgo* AhrsAlgo;

  /*!
  If AhrsUseMagnetometer enabled (a ..._MARG algorithm is chosen) then heading will be estimated using magnetometer data.
  Otherwise (when a ..._IMU algorithm is chosen) only the gyroscope data will be used for getting the heading information.
  IMU may be more noisy, but not sensitive to magnetic field distortions.
  */
  bool AhrsUseMagnetometer;

  /*! last AHRS update time (in system time) */
  double AhrsLastUpdateTime;
  double FilteredTiltSensorAhrsLastUpdateTime;

  std::ofstream RawImuDataStream;
  bool IsReceivingRawData;

  int RawDataSize;
  void* RawDataPointer;

  std::string PathToSecKey; // path to security key, required by the clarius api

  double SystemStartTimestampSeconds;
  double ClariusStartTimestampSeconds;
  double ClariusLastTimestamp;
  int FrameNumber;

  int UdpPort;

  bool Initialized;
};

//-------------------------------------------------------------------------------------------------
vtkPlusClarius::vtkInternal::vtkInternal(vtkPlusClarius* ext)
  : External(ext)
  , AccelerometerTool(NULL)
  , GyroscopeTool(NULL)
  , MagnetometerTool(NULL)
  , TiltSensorTool(NULL)
  , FilteredTiltSensorTool(NULL)
  , OrientationSensorTool(NULL)
  , AhrsAlgo(new MadgwickAhrsAlgo())
  , AhrsUseMagnetometer(true)
  , AhrsLastUpdateTime(-1)
  , FilteredTiltSensorAhrsLastUpdateTime(-1)
  , FilteredTiltSensorAhrsAlgo(new MadgwickAhrsAlgo())
  , RawDataSize(0)
  , RawDataPointer(NULL)
  , IsReceivingRawData(false)
  , PathToSecKey(DEFAULT_PATH_TO_SEC_KEY)
  , SystemStartTimestampSeconds(0.0)
  , ClariusStartTimestampSeconds(0.0)
  , ClariusLastTimestamp(0.0)
  , FrameNumber(0)
  , UdpPort(-1)
  , Initialized(false)
{
}

//----------------------------------------------------------------------------
double vtkPlusClarius::vtkInternal::NanoSecondsToSeconds(long long int ns)
{
  return ns / 1000000000.0;
}


//----------------------------------------------------------------------------
long long int vtkPlusClarius::vtkInternal::SecondsToNanoSeconds(double s)
{
  return static_cast<long long int>(1000000000 * s);
}

//----------------------------------------------------------------------------
void vtkPlusClarius::vtkInternal::ConnectReturnFn(int udpPort, int imuPort, int swRevMatch)
{
  vtkPlusClarius* device = vtkPlusClarius::GetInstance();
  if (device == NULL)
  {
    LOG_ERROR("Clarius instance is NULL");
    return;
  }
  vtkInternal* self = device->Internal;

  if (swRevMatch != CUS_SUCCESS)
  {
    LOG_ERROR("...Clarius version mismatch");
    return;
  }

  self->UdpPort = udpPort;
  device->Connected = 1;
  if (self->UdpPort != -1)
  {
    LOG_DEBUG("... Clarius device connected, streaming port: " << self->UdpPort);
  }
  else
  {
    LOG_ERROR("... Clarius device connected but could not get valid udp port");
  }
}


//----------------------------------------------------------------------------
/*! callback for error messages
 * @param[in] err the error message sent from the listener module
 * */
void vtkPlusClarius::vtkInternal::ErrorFn(const char* err)
{
  LOG_ERROR("error: " << err);
}

//----------------------------------------------------------------------------
/*! callback for freeze state change
 * @param[in] val the freeze state value 1 = frozen, 0 = imaging */
void vtkPlusClarius::vtkInternal::FreezeFn(int val)
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
void vtkPlusClarius::vtkInternal::ProgressFn(int progress)
{
  LOG_DEBUG("Download: " << progress << "%");
}

//----------------------------------------------------------------------------
/*! callback for button clicks
 * @param[in] btn 0 = up, 1 = down
 * @param[in] clicks # of clicks performed*/
void vtkPlusClarius::vtkInternal::ButtonFn(CusButton btn, int clicks)
{
  LOG_DEBUG("button: " << btn << "clicks: " << clicks << "%");
}

//----------------------------------------------------------------------------
/*! callback for a new image sent from the scanner
 * @param[in] newImage a pointer to the raw image bits of
 * @param[in] nfo the image properties
 * @param[in] npos the # fo positional data points embedded with the frame
 * @param[in] pos the buffer of positional data
 * */
void vtkPlusClarius::vtkInternal::NewProcessedImageFn(const void* newImage, const CusProcessedImageInfo* nfo, int npos, const CusPosInfo* pos)
{
  LOG_TRACE("vtkPlusClarius::NewProcessedImageFn");
  vtkPlusClarius* device = vtkPlusClarius::GetInstance();
  if (device == NULL)
  {
    LOG_ERROR("Clarius instance is NULL");
    return;
  }
  vtkInternal* self = device->Internal;


  LOG_TRACE("New image (" << newImage << "): " << nfo->width << " x " << nfo->height << " @ " << nfo->bitsPerPixel
    << "bits. @ " << nfo->micronsPerPixel << " microns per pixel. imu points: " << npos);

  // Check if still connected
  if (!device->IsConnected())
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
  vtkPlusDataSource* bModeSource;
  std::vector<vtkPlusDataSource*> bModeSources;
  device->GetVideoSourcesByPortName(vtkPlusDevice::BMODE_PORT_NAME, bModeSources);
  if (!bModeSources.empty())
  {
    bModeSource = bModeSources[0];
  }
  else
  {
    LOG_WARNING("Processed image was received, however no output B-Mode video source was found.");
    return;
  }
  // Set Image Properties
  bModeSource->SetInputFrameSize(nfo->width, nfo->height, 1);

  vtkSmartPointer<vtkImageData> rgbImage = vtkSmartPointer<vtkImageData>::New();
  rgbImage->SetExtent(0, nfo->width - 1, 0, nfo->height - 1, 0, 0);
  rgbImage->SetOrigin(0.0, 0.0, 0.0);
  rgbImage->SetSpacing(1.0, 1.0, 1.0);
  rgbImage->AllocateScalars(VTK_UNSIGNED_CHAR, 3);
  PixelCodec::BGRA32ToRGB24(nfo->width, nfo->height, (unsigned char*)newImage, (unsigned char*)rgbImage->GetScalarPointer());

  vtkSmartPointer<vtkImageData> outputImage = rgbImage;
  US_IMAGE_TYPE outputUSImageType = US_IMG_RGB_COLOR;
  bModeSource->SetNumberOfScalarComponents(3);
  bModeSource->SetPixelType(VTK_UNSIGNED_CHAR);
  if (bModeSource->GetImageType() == US_IMG_BRIGHTNESS)
  {
    bModeSource->SetNumberOfScalarComponents(1);

    vtkSmartPointer<vtkImageData> grayscaleImage = vtkSmartPointer<vtkImageData>::New();
    grayscaleImage->SetExtent(0, nfo->width - 1, 0, nfo->height - 1, 0, 1);
    grayscaleImage->SetOrigin(0.0, 0.0, 0.0);
    grayscaleImage->SetSpacing(1.0, 1.0, 1.0);
    grayscaleImage->AllocateScalars(VTK_UNSIGNED_CHAR, 1);
    outputImage = grayscaleImage;

    PixelCodec::ConvertToGray(
      PixelCodec::PixelEncoding_RGB24,
      nfo->width,
      nfo->height,
      (unsigned char*)rgbImage->GetScalarPointer(),
      (unsigned char*)grayscaleImage->GetScalarPointer());
    outputUSImageType = US_IMG_BRIGHTNESS;
  }

  // the clarius timestamp is in nanoseconds
  self->ClariusLastTimestamp = vtkInternal::NanoSecondsToSeconds(nfo->tm);
  // Get system time (elapsed time since last reboot), return Internal system time in seconds
  double systemTime = vtkIGSIOAccurateTimer::GetSystemTime();
  if (device->FrameNumber == 0)
  {
    self->SystemStartTimestampSeconds = systemTime;
    self->ClariusStartTimestampSeconds = self->ClariusLastTimestamp;
  }

  // The timestamp that each image is tagged with is
  // (system_start_time + current_clarius_time - clarius_start_time)
  double convertedTimestamp = self->SystemStartTimestampSeconds + (self->ClariusLastTimestamp - self->ClariusStartTimestampSeconds);
  if (npos != 0)
  {
    self->WritePosesToCsv(nfo, npos, pos, device->FrameNumber, systemTime, convertedTimestamp);
  }

  if (device->WriteImagesToDisk)
  {
    // create cvimg to write to disk
    cv::Mat cvimg = cv::Mat(nfo->width, nfo->height, CV_8UC4);
    cvimg.data = cvimg.data = (unsigned char*)newImage;
    if (cv::imwrite("Clarius_Image" + std::to_string(self->ClariusLastTimestamp) + ".bmp", cvimg) == false)
    {
      LOG_ERROR("ERROR writing clarius image" + std::to_string(self->ClariusLastTimestamp) + " to disk");
    }
  }

  igsioFieldMapType customField;
  customField["micronsPerPixel"] = std::make_pair(igsioFrameFieldFlags::FRAMEFIELD_FORCE_SERVER_SEND, std::to_string(nfo->micronsPerPixel));
  bModeSource->AddItem(
    outputImage->GetScalarPointer(),
    bModeSource->GetInputImageOrientation(), // refer to this url: http://perk-software.cs.queensu.ca/plus/doc/nightly/dev/UltrasoundImageOrientation.html for reference;
    // Set to UN to keep the orientation of the image the same as on tablet
    bModeSource->GetInputFrameSize(),
    bModeSource->GetPixelType(),
    bModeSource->GetNumberOfScalarComponents(),
    outputUSImageType,
    0,
    device->FrameNumber,
    convertedTimestamp,
    convertedTimestamp,
    &customField
  );

  self->ProcessPositionInfo(npos, pos);

  device->FrameNumber++;
}

//----------------------------------------------------------------------------
/*! callback for a new raw RF image sent from the scanner
 * @param[in] newImage a pointer to the raw image bits of
 * @param[in] nfo the image properties
 * @param[in] npos the # fo positional data points embedded with the frame
 * @param[in] pos the buffer of positional data
 * */
void vtkPlusClarius::vtkInternal::NewRawImageFn(const void* newImage, const CusRawImageInfo* nfo, int npos, const CusPosInfo* pos)
{
  LOG_TRACE("vtkPlusClarius::NewRawImageFn");
  vtkPlusClarius* device = vtkPlusClarius::GetInstance();
  if (device == NULL)
  {
    LOG_ERROR("Clarius instance is NULL");
    return;
  }
  vtkInternal* self = device->Internal;

  if (!nfo->rf)
  {
    LOG_TRACE("New raw image is envelope, not Rf.");
    return;
  }

  LOG_TRACE("New raw image (" << newImage << "): " << nfo->lines << " lines using " << nfo->samples << " samples, @ " << nfo->bitsPerSample << " bits."
    << nfo->axialSize << " axial microns per sample, " << nfo->lateralSize << " lateral microns per line.");

  // Check if still connected
  if (!device->IsConnected())
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

  vtkPlusDataSource* rfModeSource = nullptr;
  std::vector<vtkPlusDataSource*> rfModeSources;
  device->GetVideoSourcesByPortName(vtkPlusDevice::RFMODE_PORT_NAME, rfModeSources);
  if (!rfModeSources.empty())
  {
    rfModeSource = rfModeSources[0];
  }
  else
  {
    LOG_WARNING("Raw image was received, however no output RF video source was found.");
    return;
  }

  // Set Image Properties
  int pixelType = VTK_UNSIGNED_CHAR;
  int frameBufferBytesPerSample = (nfo->bitsPerSample / 8);

  switch (frameBufferBytesPerSample)
  {
  case VTK_SIZEOF_LONG_LONG:
    pixelType = VTK_LONG_LONG;
    break;
  case VTK_SIZEOF_INT:
    pixelType = VTK_INT;
    break;
  case VTK_SIZEOF_SHORT:
    pixelType = VTK_SHORT;
    break;
  case VTK_SIZEOF_CHAR:
  default:
    pixelType = VTK_CHAR;
    break;
  }
  rfModeSource->SetInputFrameSize(nfo->samples, nfo->lines, 1);
  rfModeSource->SetPixelType(pixelType);
  rfModeSource->SetImageType(US_IMG_RF_REAL);
  rfModeSource->SetOutputImageOrientation(US_IMG_ORIENT_MF);
  rfModeSource->SetNumberOfScalarComponents(1);

  int frameSizeInBytes = nfo->lines * nfo->samples * frameBufferBytesPerSample;

  // the clarius timestamp is in nanoseconds
  self->ClariusLastTimestamp = vtkInternal::NanoSecondsToSeconds(nfo->tm);
  // Get system time (elapsed time since last reboot), return Internal system time in seconds
  double systemTime = vtkIGSIOAccurateTimer::GetSystemTime();
  if (self->FrameNumber == 0)
  {
    self->SystemStartTimestampSeconds = systemTime;
    self->ClariusStartTimestampSeconds = self->ClariusLastTimestamp;
  }

  double convertedTimestamp = self->SystemStartTimestampSeconds + (self->ClariusLastTimestamp - self->ClariusStartTimestampSeconds);
  rfModeSource->AddItem(
    (void*)newImage, // pointer to char array
    rfModeSource->GetInputImageOrientation(), // refer to this url: http://perk-software.cs.queensu.ca/plus/doc/nightly/dev/UltrasoundImageOrientation.html for reference;
    // Set to UN to keep the orientation of the image the same as on tablet
    rfModeSource->GetInputFrameSize(),
    pixelType,
    rfModeSource->GetNumberOfScalarComponents(),
    rfModeSource->GetImageType(),
    0,
    device->FrameNumber,
    convertedTimestamp,
    convertedTimestamp);

  self->ProcessPositionInfo(npos, pos);

  device->FrameNumber++;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusClarius::vtkInternal::ProcessPositionInfo(int npos, const CusPosInfo* pos)
{
  vtkPlusClarius* device = vtkPlusClarius::GetInstance();
  if (!device)
  {
    LOG_ERROR("Invalid device");
    return PLUS_FAIL;
  }
  vtkInternal* self = device->Internal;

  // Iterate through all of the incoming position info.
  for (int i = 0; i < npos; i++)
  {
    const CusPosInfo currentPos = pos[i];

    // the clarius timestamp is in nanoseconds
    // The timestamp that each pose is tagged with is
    // (system_start_time + current_clarius_time - clarius_start_time)
    double poseTimeStampSeconds = vtkInternal::NanoSecondsToSeconds(currentPos.tm);
    poseTimeStampSeconds = self->SystemStartTimestampSeconds + (poseTimeStampSeconds - self->ClariusStartTimestampSeconds);

    double acceleration[3] = { currentPos.ax , currentPos.ay , currentPos.az };
    if (self->AccelerometerTool != NULL)
    {
      self->LastAccelerometerToTrackerTransform->Identity();
      self->LastAccelerometerToTrackerTransform->SetElement(0, 3, acceleration[0]);
      self->LastAccelerometerToTrackerTransform->SetElement(1, 3, acceleration[1]);
      self->LastAccelerometerToTrackerTransform->SetElement(2, 3, acceleration[2]);
      device->ToolTimeStampedUpdateWithoutFiltering(self->AccelerometerTool->GetId(), self->LastAccelerometerToTrackerTransform, TOOL_OK, poseTimeStampSeconds, poseTimeStampSeconds);
    }

    double angularRate[3] = { currentPos.gx , currentPos.gy , currentPos.gz };
    if (self->GyroscopeTool != NULL)
    {
      self->LastGyroscopeToTrackerTransform->Identity();
      self->LastGyroscopeToTrackerTransform->SetElement(0, 3, angularRate[0]);
      self->LastGyroscopeToTrackerTransform->SetElement(1, 3, angularRate[1]);
      self->LastGyroscopeToTrackerTransform->SetElement(2, 3, angularRate[2]);
      device->ToolTimeStampedUpdateWithoutFiltering(self->GyroscopeTool->GetId(), self->LastGyroscopeToTrackerTransform, TOOL_OK, poseTimeStampSeconds, poseTimeStampSeconds);
    }

    double magneticField[3] = { currentPos.mx , currentPos.my , currentPos.mz };
    if (self->MagnetometerTool != NULL)
    {
      // magnetometer data is valid
      self->LastMagnetometerToTrackerTransform->Identity();
      self->LastMagnetometerToTrackerTransform->SetElement(0, 3, magneticField[0]);
      self->LastMagnetometerToTrackerTransform->SetElement(1, 3, magneticField[1]);
      self->LastMagnetometerToTrackerTransform->SetElement(2, 3, magneticField[2]);
      device->ToolTimeStampedUpdateWithoutFiltering(self->MagnetometerTool->GetId(), self->LastMagnetometerToTrackerTransform, TOOL_OK, poseTimeStampSeconds, poseTimeStampSeconds);
    }

    double orientationQuat[4] = { currentPos.qw, currentPos.qx, currentPos.qy, currentPos.qz };
    if (self->OrientationSensorTool != NULL)
    {
      double orientationMatrix[3][3] = {
        {0.0, 0.0, 0.0},
        {0.0, 0.0, 0.0},
        {0.0, 0.0, 0.0}
      };
      vtkMath::QuaternionToMatrix3x3(orientationQuat, orientationMatrix);
      for (int c = 0; c < 3; c++)
      {
        for (int r = 0; r < 3; r++)
        {
          self->LastOrientationSensorToTrackerTransform->SetElement(r, c, orientationMatrix[r][c]);
        }
      }
      device->ToolTimeStampedUpdateWithoutFiltering(self->OrientationSensorTool->GetId(), self->LastOrientationSensorToTrackerTransform, TOOL_OK, poseTimeStampSeconds, poseTimeStampSeconds);
    }

    if (self->TiltSensorTool != NULL)
    {
      double downVector_Sensor[4] = { acceleration[0], acceleration[1], acceleration[2], 0 }; // provided by the sensor
      vtkMath::Normalize(downVector_Sensor);

      igsioMath::ConstrainRotationToTwoAxes(downVector_Sensor, device->TiltSensorWestAxisIndex, self->LastTiltSensorToTrackerTransform);

      device->ToolTimeStampedUpdateWithoutFiltering(self->TiltSensorTool->GetId(), self->LastTiltSensorToTrackerTransform, TOOL_OK, poseTimeStampSeconds, poseTimeStampSeconds);
    }

    if (self->FilteredTiltSensorTool != NULL)
    {
      self->FilteredTiltSensorAhrsAlgo->UpdateIMUWithTimestamp(
        vtkMath::RadiansFromDegrees(angularRate[0]), vtkMath::RadiansFromDegrees(angularRate[1]), vtkMath::RadiansFromDegrees(angularRate[2]),
        acceleration[0], acceleration[1], acceleration[2], poseTimeStampSeconds);

      double rotQuat[4] = { 0.0, 0.0, 0.0, 0.0 };
      self->AhrsAlgo->GetOrientation(rotQuat[0], rotQuat[1], rotQuat[2], rotQuat[3]);

      double rotMatrix[3][3] = {
        {0.0, 0.0, 0.0},
        {0.0, 0.0, 0.0},
        {0.0, 0.0, 0.0}
      };
      vtkMath::QuaternionToMatrix3x3(rotQuat, rotMatrix);

      double filteredDownVector_Sensor[4] = { rotMatrix[2][0], rotMatrix[2][1], rotMatrix[2][2], 0.0 };
      vtkMath::Normalize(filteredDownVector_Sensor);

      igsioMath::ConstrainRotationToTwoAxes(filteredDownVector_Sensor, device->FilteredTiltSensorWestAxisIndex, self->LastFilteredTiltSensorToTrackerTransform);

      device->ToolTimeStampedUpdateWithoutFiltering(self->FilteredTiltSensorTool->GetId(), self->LastFilteredTiltSensorToTrackerTransform, TOOL_OK, poseTimeStampSeconds, poseTimeStampSeconds);

      // write back the results to the FilteredTiltSensor_AHRS algorithm
      for (int c = 0; c < 3; c++)
      {
        for (int r = 0; r < 3; r++)
        {
          rotMatrix[r][c] = self->LastFilteredTiltSensorToTrackerTransform->GetElement(r, c);
        }
      }
      double filteredTiltSensorRotQuat[4] = { 0 };
      vtkMath::Matrix3x3ToQuaternion(rotMatrix, filteredTiltSensorRotQuat);
      self->FilteredTiltSensorAhrsAlgo->SetOrientation(filteredTiltSensorRotQuat[0], filteredTiltSensorRotQuat[1], filteredTiltSensorRotQuat[2], filteredTiltSensorRotQuat[3]);
    }
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusClarius::vtkInternal::WritePosesToCsv(const CusProcessedImageInfo* nfo, int npos, const CusPosInfo* pos, int frameNum, double systemTime, double convertedTime)
{
  LOG_TRACE("vtkPlusClarius::WritePosesToCsv");
  if (npos != 0)
  {
    LOG_TRACE("timestamp in nanoseconds CusPosInfo" << pos[0].tm);
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
    this->RawImuDataStream.open(this->External->ImuOutputFileName, std::ofstream::app);
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
void vtkPlusClarius::vtkInternal::RawDataRequestFn(int rawDataSize, const char* extension)
{
  vtkPlusClarius* device = vtkPlusClarius::GetInstance();
  if (!device)
  {
    LOG_ERROR("Invalid device");
    return;
  }
  vtkInternal* self = device->Internal;

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
PlusStatus vtkPlusClarius::vtkInternal::ReceiveRawData(int dataSize)
{
  LOG_INFO("Receiving " << dataSize << " bytes of raw data");

  CusReturnFn returnFunction = (CusReturnFn)(&vtkInternal::RawDataWriteFn);
  this->AllocateRawData(dataSize);
  cusCastReadRawData(&this->RawDataPointer, returnFunction);
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
void vtkPlusClarius::vtkInternal::RawDataWriteFn(int retCode)
{
  vtkPlusClarius* device = vtkPlusClarius::GetInstance();
  if (!device)
  {
    LOG_ERROR("Invalid device");
    return;
  }
  vtkInternal* self = device->Internal;

  self->IsReceivingRawData = false;

  if (retCode < 0)
  {
    LOG_ERROR("Could not read raw data!");
    return;
  }

  LOG_INFO("Raw data received successfully!");

  std::string filename = device->GetRawDataOutputFilename();
  if (filename.empty())
  {
    filename = vtkIGSIOAccurateTimer::GetDateAndTimeString() + "_ClariusData.tar";
  }

  if (device->CompressRawData && vtksys::SystemTools::GetFilenameLastExtension(filename) != ".gz")
  {
    filename = filename + ".gz";
  }

  if (!vtksys::SystemTools::FileIsFullPath(filename.c_str()))
  {

    filename = vtkPlusConfig::GetInstance()->GetOutputDirectory() + "/" + filename;
  }

  if (device->CompressRawData)
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
void vtkPlusClarius::vtkInternal::AllocateRawData(int dataSize)
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


//----------------------------------------------------------------------------
vtkPlusClarius* vtkPlusClarius::Instance;

//----------------------------------------------------------------------------
vtkPlusClarius* vtkPlusClarius::New()
{
  if (vtkPlusClarius::Instance == NULL)
  {
    vtkPlusClarius::Instance = new vtkPlusClarius();
  }
  return vtkPlusClarius::Instance;
}

//----------------------------------------------------------------------------
vtkPlusClarius::vtkPlusClarius()
  : Internal(new vtkInternal(this))
  , TcpPort(-1)
  , IpAddress("192.168.1.1")
  , FrameWidth(DEFAULT_FRAME_WIDTH)
  , FrameHeight(DEFAULT_FRAME_HEIGHT)
  , ImuEnabled(false)
  , ImuOutputFileName("ClariusImuData.csv")
  , WriteImagesToDisk(false)
  , CompressRawData(false)
  , AhrsAlgorithmGain{ 1.5, 0.0 }
  , FilteredTiltSensorAhrsAlgorithmGain{ 1.5, 0.0 }
  , FilteredTiltSensorWestAxisIndex(1)
  , TiltSensorWestAxisIndex(1)
{
  LOG_TRACE("vtkPlusClarius: Constructor");
  this->StartThreadForInternalUpdates = true;
  this->RequirePortNameInDeviceSetConfiguration = true;
}

//----------------------------------------------------------------------------
vtkPlusClarius::~vtkPlusClarius()
{
  this->Internal->AllocateRawData(-1);

  if (this->Recording)
  {
    this->StopRecording();
  }

  if (this->Connected)
  {
    cusCastDisconnect(BLOCKINGCALL);
  }

  if (this->Internal->Initialized)
  {
    int destroyed = cusCastDestroy();
    if (destroyed != 0)
    {
      LOG_ERROR("Error destoying the listener");
    }
  }

  if (this->Internal)
  {
    delete this->Internal;
  }

  this->Instance = NULL;
}

//----------------------------------------------------------------------------
vtkPlusClarius* vtkPlusClarius::GetInstance()
{
  LOG_TRACE("vtkPlusClarius: GetInstance()");
  if (Instance != NULL)
  {
    return Instance;
  }

  else
  {
    LOG_ERROR("Instance is null, creating new instance");
    Instance = new vtkPlusClarius();
    return Instance;
  }
}

//----------------------------------------------------------------------------
void vtkPlusClarius::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
  os << indent << "ipAddress" << this->IpAddress << std::endl;
  os << indent << "tcpPort" << this->TcpPort << std::endl;
  os << indent << "UdpPort" << this->Internal->UdpPort << std::endl;
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

  int tiltSensorWestAxisIndex = 0;
  if (deviceConfig->GetScalarAttribute("TiltSensorWestAxisIndex", tiltSensorWestAxisIndex))
  {
    if (tiltSensorWestAxisIndex < 0 || tiltSensorWestAxisIndex > 2)
    {
      LOG_ERROR("TiltSensorWestAxisIndex is invalid. Specified value: " << tiltSensorWestAxisIndex << ". Valid values: 0, 1, 2. Keep using the default value: "
        << this->TiltSensorWestAxisIndex);
    }
    else
    {
      this->TiltSensorWestAxisIndex = tiltSensorWestAxisIndex;
    }
  }

  int FilteredTiltSensorWestAxisIndex = 0;
  if (deviceConfig->GetScalarAttribute("FilteredTiltSensorWestAxisIndex", FilteredTiltSensorWestAxisIndex))
  {
    if (FilteredTiltSensorWestAxisIndex < 0 || FilteredTiltSensorWestAxisIndex > 2)
    {
      LOG_ERROR("FilteredTiltSensorWestAxisIndex is invalid. Specified value: " << FilteredTiltSensorWestAxisIndex << ". Valid values: 0, 1, 2. Keep using the default value: "
        << this->FilteredTiltSensorWestAxisIndex);
    }
    else
    {
      this->FilteredTiltSensorWestAxisIndex = FilteredTiltSensorWestAxisIndex;
    }
  }

  XML_READ_VECTOR_ATTRIBUTE_OPTIONAL(double, 2, AhrsAlgorithmGain, deviceConfig);
  XML_READ_VECTOR_ATTRIBUTE_OPTIONAL(double, 2, FilteredTiltSensorAhrsAlgorithmGain, deviceConfig);

  const char* ahrsAlgoName = deviceConfig->GetAttribute("AhrsAlgorithm");
  if (ahrsAlgoName != NULL)
  {
    if (STRCASECMP("MADGWICK_MARG", ahrsAlgoName) == 0 || STRCASECMP("MADGWICK_IMU", ahrsAlgoName) == 0)
    {
      if (dynamic_cast<MadgwickAhrsAlgo*>(this->Internal->AhrsAlgo) == 0)
      {
        // not the requested type
        // delete the old algo and create a new one with the correct type
        delete this->Internal->AhrsAlgo;
        this->Internal->AhrsAlgo = new MadgwickAhrsAlgo;
      }
      if (STRCASECMP("MADGWICK_MARG", ahrsAlgoName) == 0)
      {
        this->Internal->AhrsUseMagnetometer = true;
      }
      else
      {
        this->Internal->AhrsUseMagnetometer = false;
      }
    }
    else if (STRCASECMP("MAHONY_MARG", ahrsAlgoName) == 0 || STRCASECMP("MAHONY_IMU", ahrsAlgoName) == 0)
    {
      if (dynamic_cast<MahonyAhrsAlgo*>(this->Internal->AhrsAlgo) == 0)
      {
        // not the requested type
        // delete the old algo and create a new one with the correct type
        delete this->Internal->AhrsAlgo;
        this->Internal->AhrsAlgo = new MahonyAhrsAlgo;
      }
      if (STRCASECMP("MAHONY_MARG", ahrsAlgoName) == 0)
      {
        this->Internal->AhrsUseMagnetometer = true;
      }
      else
      {
        this->Internal->AhrsUseMagnetometer = false;
      }
    }
    else
    {
      LOG_ERROR("Unable to recognize AHRS algorithm type: " << ahrsAlgoName << ". Supported types: MADGWICK_MARG, MAHONY_MARG, MADGWICK_IMU, MAHONY_IMU");
      return PLUS_FAIL;
    }
  }
  const char* FilteredTiltSensorAhrsAlgoName = deviceConfig->GetAttribute("FilteredTiltSensorAhrsAlgorithm");
  if (FilteredTiltSensorAhrsAlgoName != NULL)
  {
    if (STRCASECMP("MADGWICK_IMU", FilteredTiltSensorAhrsAlgoName) == 0)
    {
      if (dynamic_cast<MadgwickAhrsAlgo*>(this->Internal->FilteredTiltSensorAhrsAlgo) == 0)
      {
        // not the requested type
        // delete the old algo and create a new one with the correct type
        delete this->Internal->FilteredTiltSensorAhrsAlgo;
        this->Internal->FilteredTiltSensorAhrsAlgo = new MadgwickAhrsAlgo;
      }
    }
    else if (STRCASECMP("MAHONY_IMU", FilteredTiltSensorAhrsAlgoName) == 0)
    {
      if (dynamic_cast<MahonyAhrsAlgo*>(this->Internal->FilteredTiltSensorAhrsAlgo) == 0)
      {
        // not the requested type
        // delete the old algo and create a new one with the correct type
        delete this->Internal->FilteredTiltSensorAhrsAlgo;
        this->Internal->FilteredTiltSensorAhrsAlgo = new MahonyAhrsAlgo;
      }
    }
    else
    {
      LOG_ERROR("Unable to recognize AHRS algorithm type for Filtered Tilt: " << FilteredTiltSensorAhrsAlgoName << ". Supported types: MADGWICK_IMU, MAHONY_IMU");
      return PLUS_FAIL;
    }
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
  if (this->Internal->UdpPort == -1)
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
    this->Internal->RawImuDataStream.open(this->ImuOutputFileName, std::ofstream::app);
    this->Internal->RawImuDataStream << "FrameNum,SystemTimestamp,ConvertedTimestamp,ImageTimestamp,ImuTimeStamp,ax,ay,az,gx,gy,gz,mx,my,mz,\n";

    this->Internal->RawImuDataStream.close();
  }

  this->Internal->AccelerometerTool = NULL;
  this->GetToolByPortName("Accelerometer", this->Internal->AccelerometerTool);

  this->Internal->GyroscopeTool = NULL;
  this->GetToolByPortName("Gyroscope", this->Internal->GyroscopeTool);

  this->Internal->MagnetometerTool = NULL;
  this->GetToolByPortName("Magnetometer", this->Internal->MagnetometerTool);

  this->Internal->TiltSensorTool = NULL;
  this->GetToolByPortName("TiltSensor", this->Internal->TiltSensorTool);

  this->Internal->FilteredTiltSensorTool = NULL;
  this->GetToolByPortName("FilteredTiltSensor", this->Internal->FilteredTiltSensorTool);

  this->Internal->OrientationSensorTool = NULL;
  this->GetToolByPortName("OrientationSensor", this->Internal->OrientationSensorTool);

  // Initialize Clarius Listener Before Connecting
  if (!this->IsConnected())
  {
    // Callbacks
    CusNewProcessedImageFn processedImageCallbackPtr = static_cast<CusNewProcessedImageFn>(&vtkInternal::NewProcessedImageFn);
    CusNewRawImageFn rawDataCallBackPtr = static_cast<CusNewRawImageFn>(&vtkInternal::NewRawImageFn);
    CusFreezeFn freezeCallBackFnPtr = static_cast<CusFreezeFn>(&vtkInternal::FreezeFn);
    CusButtonFn buttonCallBackFnPtr = static_cast<CusButtonFn>(&vtkInternal::ButtonFn);
    CusProgressFn progressCallBackFnPtr = static_cast<CusProgressFn>(&vtkInternal::ProgressFn);
    CusErrorFn errorCallBackFnPtr = static_cast<CusErrorFn>(&vtkInternal::ErrorFn);

    // No B-mode data sources. Disable B mode callback.
    std::vector<vtkPlusDataSource*> bModeSources;
    this->GetVideoSourcesByPortName(vtkPlusDevice::BMODE_PORT_NAME, bModeSources);
    if (bModeSources.empty())
    {
      processedImageCallbackPtr = nullptr;
    }

    // No RF-mode data sources. Disable RF mode callback.
    std::vector<vtkPlusDataSource*> rfModeSources;
    this->GetVideoSourcesByPortName(vtkPlusDevice::RFMODE_PORT_NAME, rfModeSources);
    if (rfModeSources.empty())
    {
      rawDataCallBackPtr = nullptr;
    }

    try
    {
      CusInitParams initParams;
      initParams.args.argc = 0;
      initParams.args.argv = nullptr;
      initParams.storeDir = this->Internal->PathToSecKey.c_str();
      initParams.newProcessedImageFn = processedImageCallbackPtr;
      initParams.newRawImageFn = rawDataCallBackPtr;
      initParams.newSpectralImageFn = nullptr;
      initParams.newImuDataFn = nullptr;
      initParams.freezeFn = freezeCallBackFnPtr;
      initParams.buttonFn = buttonCallBackFnPtr;
      initParams.progressFn = progressCallBackFnPtr;
      initParams.errorFn = errorCallBackFnPtr;
      initParams.width = this->FrameWidth;
      initParams.height = this->FrameHeight;
      if (cusCastInit(&initParams) < 0)
      {
        return PLUS_FAIL;
      }
      this->Internal->Initialized = true;
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
    const char* ip = this->IpAddress.c_str();
    try
    {
      CusConnectFn returnFunction = (CusConnectFn)(&vtkInternal::ConnectReturnFn);
      cusCastConnect(ip, this->TcpPort, "research", returnFunction);

      // Wait for the udp port to be determined.
      int maxConnectionAttempts = 20;
      for (int i = 0; i < 20; ++i)
      {
        // Attempt to get the udp port.
        const double connectionDelaySeconds = 1.0;
        vtkIGSIOAccurateTimer::DelayWithEventProcessing(connectionDelaySeconds);

        if (this->Internal->UdpPort != -1)
        {
          vtkIGSIOAccurateTimer::DelayWithEventProcessing(connectionDelaySeconds);
          break;
        }
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

    if (this->Internal->UdpPort != -1)
    {
      if (cusCastSetOutputSize(this->FrameWidth, this->FrameHeight) < 0)
      {
        LOG_DEBUG("Clarius Output size can not be set, falling back to default 640*480");
        this->FrameWidth = DEFAULT_FRAME_WIDTH;
        this->FrameHeight = DEFAULT_FRAME_HEIGHT;
      }
    }
    else
    {
      this->Connected = 1;
      this->InternalDisconnect();
      this->Connected = 0;
      return PLUS_FAIL;
    }

    if (!this->IsConnected())
    {
      LOG_ERROR("Could not connect to scanner at ip: " << ip << " port number: " << this->TcpPort);
      return PLUS_FAIL;
    }
  }
  else
  {
    LOG_DEBUG("Scanner already connected to IP address=" << this->IpAddress
      << " TCP Port Number =" << this->TcpPort << "Streaming Image at UDP Port=" << this->Internal->UdpPort);
    this->Connected = 1;
    return PLUS_SUCCESS;
  }
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusClarius::InternalDisconnect()
{
  LOG_DEBUG("vtkPlusClarius: InternalDisconnect");
  vtkPlusClarius* device = vtkPlusClarius::GetInstance();
  if (device->GetConnected())
  {
    if (cusCastDisconnect(nullptr) < 0)
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
PlusStatus vtkPlusClarius::RequestLastNSecondsRawData(double lastNSeconds)
{
  if (lastNSeconds <= 0)
  {
    return this->RequestRawData(0, 0);
  }

  LOG_INFO("Requesting raw data for last " << lastNSeconds << " seconds");
  long long int endTimestamp = vtkInternal::SecondsToNanoSeconds(this->Internal->ClariusLastTimestamp);
  long long int lastNNanoSeconds = vtkInternal::SecondsToNanoSeconds(lastNSeconds);
  long long int startTimestamp = std::max((long long)0, endTimestamp - lastNNanoSeconds);
  return this->RequestRawData(startTimestamp, endTimestamp);
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusClarius::RequestRawData(long long int startTimestamp, long long int endTimestamp)
{
  if (this->Internal->IsReceivingRawData)
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

  this->Internal->IsReceivingRawData = true;

  CusRawRequestFn returnFunction = (CusRawRequestFn)(&vtkInternal::RawDataRequestFn);
  cusCastRequestRawData(startTimestamp, endTimestamp, 0, returnFunction);
  return PLUS_SUCCESS;
}
