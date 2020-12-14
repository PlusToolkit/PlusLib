/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"
#include "vtkPlusDataSource.h"
#include "vtkPlusAndorVideoSource.h"
#include "ATMCD32D.h"
#include "igtlOSUtil.h" // for Sleep
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"


vtkStandardNewMacro(vtkPlusAndorVideoSource);

// put these here so there is no public dependence on OpenCV
cv::Mat cvCameraIntrinsics;
cv::Mat cvDistanceCoefficients;
cv::Mat cvBadPixelImage;
cv::Mat cvFlatCorrection;
cv::Mat cvBiasDarkCorrection;
using CellIndices = std::vector<uint>;
std::map<int, CellIndices> cellsToCorrect;

// ----------------------------------------------------------------------------
void vtkPlusAndorVideoSource::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);

  os << indent << "Shutter: " << Shutter << std::endl;
  os << indent << "ExposureTime: " << ExposureTime << std::endl;
  os << indent << "Binning: " << HorizontalBins << " " << VerticalBins << std::endl;
  os << indent << "HSSpeed: " << HSSpeed[0] << HSSpeed[1] << std::endl;
  os << indent << "VSSpeed: " << VSSpeed << std::endl;
  os << indent << "OutputSpacing: " << OutputSpacing[0] << " " << OutputSpacing[1] << std::endl;
  os << indent << "PreAmpGain: " << PreAmpGain << std::endl;
  os << indent << "AcquisitionMode: " << m_AcquisitionMode << std::endl;
  os << indent << "ReadMode: " << m_ReadMode << std::endl;
  os << indent << "TriggerMode: " << m_TriggerMode << std::endl;
  os << indent << "RequireCoolTemp: " << RequireCoolTemp << std::endl;
  os << indent << "CoolerMode: " << CoolerMode << std::endl;
  os << indent << "CoolTemperature: " << CoolTemperature << std::endl;
  os << indent << "SafeTemperature: " << SafeTemperature << std::endl;
  os << indent << "CurrentTemperature: " << CurrentTemperature << std::endl;
  os << indent << "CameraIntrinsics: " << cvCameraIntrinsics << std::endl;
  os << indent << "DistanceCoefficients: " << cvDistanceCoefficients << std::endl;
  os << indent << "UseFrameCorrections: " << UseFrameCorrections << std::endl;
  os << indent << "FlatCorrection: " << flatCorrection << std::endl;
  os << indent << "BiasDarkCorrection: " << biasDarkCorrection << std::endl;
  os << indent << "BadPixelCorrection: " << badPixelCorrection << std::endl;
}

// ----------------------------------------------------------------------------
PlusStatus vtkPlusAndorVideoSource::ReadConfiguration(vtkXMLDataElement* rootConfigElement)
{
  LOG_TRACE("vtkPlusAndorVideoSource::ReadConfiguration");
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_READING(deviceConfig, rootConfigElement);

  long totalCameras = 0;
  // It is possible to call GetAvailableCameras before any of the cameras are initialized.
  unsigned availableCamerasResult = checkStatus(GetAvailableCameras(&totalCameras), "GetAvailableCameras");
  if(availableCamerasResult == DRV_SUCCESS)
  {
    if(totalCameras == 0)
    {
      LOG_ERROR("Unable to find any Andor cameras devices installed.");
    }
  }

  // Must initialize the system before setting parameters
  unsigned initializeResult = checkStatus(Initialize(""), "Initialize");
  if(initializeResult != DRV_SUCCESS)
  {
    return PLUS_FAIL;
  }

  const char * shutterString = deviceConfig->GetAttribute("Shutter");
  if(shutterString)
  {
    Shutter = static_cast<vtkPlusAndorVideoSource::ShutterMode>(std::atoi(shutterString));
  }
  else
  {
    Shutter = vtkPlusAndorVideoSource::ShutterMode::FullyAuto;
  }

  const char * acquisitionModeString = deviceConfig->GetAttribute("AcquisitionMode");
  if(acquisitionModeString)
  {
    m_AcquisitionMode = static_cast<vtkPlusAndorVideoSource::AcquisitionMode>(std::atoi(acquisitionModeString));
  }
  else
  {
    m_AcquisitionMode = vtkPlusAndorVideoSource::AcquisitionMode::SingleScan;
  }

  const char * readModeString = deviceConfig->GetAttribute("ReadMode");
  if(readModeString)
  {
    m_ReadMode = static_cast<vtkPlusAndorVideoSource::ReadMode>(std::atoi(readModeString));
  }
  else
  {
    m_ReadMode = vtkPlusAndorVideoSource::ReadMode::Image;
  }

  const char * triggerModeString = deviceConfig->GetAttribute("TriggerMode");
  if(triggerModeString)
  {
    m_TriggerMode = static_cast<vtkPlusAndorVideoSource::TriggerMode>(std::atoi(triggerModeString));
  }
  else
  {
    m_TriggerMode = vtkPlusAndorVideoSource::TriggerMode::Internal;
  }

  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(float, ExposureTime, deviceConfig);
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(int, PreAmpGain, deviceConfig);
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(int, CoolerMode, deviceConfig);
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(int, CoolTemperature, deviceConfig);
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(int, SafeTemperature, deviceConfig);
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(int, VSSpeed, deviceConfig);
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(int, HorizontalBins, deviceConfig);
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(int, VerticalBins, deviceConfig);

  XML_READ_BOOL_ATTRIBUTE_OPTIONAL(InitializeCoolerState, deviceConfig);
  XML_READ_BOOL_ATTRIBUTE_OPTIONAL(RequireCoolTemp, deviceConfig);
  XML_READ_BOOL_ATTRIBUTE_OPTIONAL(UseFrameCorrections, deviceConfig);

  deviceConfig->GetVectorAttribute("HSSpeed", 2, HSSpeed);
  deviceConfig->GetVectorAttribute("OutputSpacing", 3, OutputSpacing);
  deviceConfig->GetVectorAttribute("CameraIntrinsics", 9, cameraIntrinsics);
  deviceConfig->GetVectorAttribute("DistanceCoefficients", 4, distanceCoefficients);
  badPixelCorrection = deviceConfig->GetAttribute("BadPixelCorrection");
  flatCorrection = deviceConfig->GetAttribute("FlatCorrection");
  biasDarkCorrection = deviceConfig->GetAttribute("BiasDarkCorrection");

  cvCameraIntrinsics = cv::Mat(3, 3, CV_64FC1, cameraIntrinsics);
  cvDistanceCoefficients = cv::Mat(1, 4, CV_64FC1, distanceCoefficients);
  this->SetBadPixelCorrectionImage(badPixelCorrection); // load the image
  this->SetFlatCorrectionImage(flatCorrection); // load and normalize if needed
  this->SetBiasDarkCorrectionImage(biasDarkCorrection); // load the image

  return PLUS_SUCCESS;
}

// ----------------------------------------------------------------------------
PlusStatus vtkPlusAndorVideoSource::WriteConfiguration(vtkXMLDataElement* rootConfigElement)
{
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_WRITING(deviceConfig, rootConfigElement);

  deviceConfig->SetIntAttribute("Shutter", this->Shutter);
  deviceConfig->SetFloatAttribute("ExposureTime", this->ExposureTime);
  deviceConfig->SetIntAttribute("PreAmpGain", this->PreAmpGain);
  deviceConfig->SetIntAttribute("AcquisitionMode", this->m_AcquisitionMode);
  deviceConfig->SetIntAttribute("ReadMode", this->m_ReadMode);
  deviceConfig->SetIntAttribute("TriggerMode", this->m_TriggerMode);
  deviceConfig->SetIntAttribute("CoolerMode", this->CoolerMode);
  deviceConfig->SetIntAttribute("CoolTemperature", this->CoolTemperature);
  deviceConfig->SetIntAttribute("SafeTemperature", this->SafeTemperature);
  deviceConfig->SetIntAttribute("VSSpeed", this->VSSpeed);
  deviceConfig->SetIntAttribute("HorizontalBins", this->HorizontalBins);
  deviceConfig->SetIntAttribute("VerticalBins", this->VerticalBins);

  deviceConfig->SetVectorAttribute("HSSpeed", 2, HSSpeed);
  deviceConfig->SetVectorAttribute("OutputSpacing", 3, OutputSpacing);
  deviceConfig->SetVectorAttribute("CameraIntrinsics", 9, cameraIntrinsics);
  deviceConfig->SetVectorAttribute("DistanceCoefficients", 4, distanceCoefficients);
  deviceConfig->SetAttribute("FlatCorrection", flatCorrection.c_str());
  deviceConfig->SetAttribute("BiasDarkCorrection", biasDarkCorrection.c_str());
  deviceConfig->SetAttribute("BadPixelCorrection", badPixelCorrection.c_str());

  XML_WRITE_BOOL_ATTRIBUTE(UseFrameCorrections, deviceConfig);

  return PLUS_SUCCESS;
}


// ----------------------------------------------------------------------------
PlusStatus vtkPlusAndorVideoSource::NotifyConfigured()
{
  if(this->OutputChannels.empty())
  {
    LOG_ERROR("No output channels defined for vtkPlusAndorVideoSource. Cannot proceed.");
    this->CorrectlyConfigured = false;
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}

// ----------------------------------------------------------------------------
std::string vtkPlusAndorVideoSource::GetSdkVersion()
{
  std::ostringstream versionString;

  char SDKVersion[256];
  checkStatus(GetVersionInfo(AT_SDKVersion, SDKVersion, sizeof(SDKVersion)), "GetVersionInfo");
  versionString << "Andor SDK version: "  << SDKVersion << std::ends;

  return versionString.str();
}


//----------------------------------------------------------------------------
vtkPlusAndorVideoSource::vtkPlusAndorVideoSource()
{
  this->RequirePortNameInDeviceSetConfiguration = true;

  this->StartThreadForInternalUpdates = false; // frames should not be acquired automatically
  this->AcquisitionRate = 1.0; // this controls the frequency
}

// ----------------------------------------------------------------------------
vtkPlusAndorVideoSource::~vtkPlusAndorVideoSource()
{
  if(!this->Connected)
  {
    this->Disconnect();
  }
}

// ----------------------------------------------------------------------------
PlusStatus vtkPlusAndorVideoSource::InitializeAndorCamera()
{
  long totalCameras = 0;
  // It is possible to call GetAvailableCameras before any of the cameras are initialized.
  unsigned availableCamerasResult = checkStatus(GetAvailableCameras(&totalCameras), "GetAvailableCameras");
  if(availableCamerasResult == DRV_SUCCESS)
  {
    if(totalCameras == 0)
    {
      LOG_ERROR("Unable to find any Andor cameras devices installed.");
      return PLUS_FAIL;
    }
  }

  unsigned initializeResult = checkStatus(Initialize(""), "Initialize");
  if(initializeResult != DRV_SUCCESS)
  {
    return PLUS_FAIL;
  }

  char headModel[_MAX_PATH];
  unsigned headModelResult = checkStatus(GetHeadModel(headModel), "GetHeadModel");
  if(headModelResult == DRV_SUCCESS)
  {
    LOG_INFO("Andor Camera Model: " << headModel);
  }

  int serialNumber;
  unsigned cameraSNResult = checkStatus(GetCameraSerialNumber(&serialNumber), "GetCameraSerialNumber");
  if(cameraSNResult == DRV_SUCCESS)
  {
    LOG_INFO("Andor Camera Serial Number: " << serialNumber);
  }

  // Check the safe temperature, and the maximum allowable temperature on the camera.
  // Use the min of the two as the safe temp.
  int MinTemp, MaxTemp;
  unsigned result = checkStatus(GetTemperatureRange(&MinTemp, &MaxTemp), "GetTemperatureRange");
  if(result == DRV_SUCCESS)
  {
    LOG_INFO("The temperature range for the connected Andor Camera is: " << MinTemp << " and " << MaxTemp);
  }

  if(MaxTemp < this->SafeTemperature)
  {
    this->SafeTemperature = MaxTemp;
  }
  if(this->CoolTemperature < MinTemp || this->CoolTemperature > MaxTemp)
  {
    LOG_ERROR("Requested temperature for Andor camera is out of range");
    return PLUS_FAIL;
  }

  /// Initialize custom fields of this data source
  this->CustomFields.clear();
  AdjustSpacing(this->HorizontalBins, this->VerticalBins);

  int x, y;
  checkStatus(GetDetector(&x, &y), "GetDetector");
  frameSize[0] = static_cast<unsigned>(x);
  frameSize[1] = static_cast<unsigned>(y);

  // init to binning of 1 (meaning no binning), and full sensor size
  checkStatus(SetImage(this->HorizontalBins, this->VerticalBins, 1, x, 1, y), "SetImage");

  checkStatus(PrepareAcquisition(), "PrepareAcquisition");

  return PLUS_SUCCESS;
}

// ----------------------------------------------------------------------------
void vtkPlusAndorVideoSource::InitializePort(DataSourceArray& port)
{
  for(unsigned i = 0; i < port.size(); i++)
  {
    port[i]->SetPixelType(VTK_UNSIGNED_SHORT);
    port[i]->SetImageType(US_IMG_BRIGHTNESS);
    port[i]->SetOutputImageOrientation(US_IMG_ORIENT_MF);
    port[i]->SetInputImageOrientation(US_IMG_ORIENT_MF);
    port[i]->SetInputFrameSize(frameSize);

    LOG_INFO("Andor source initialized. ID: " << port[i]->GetId());
  }
}

// ----------------------------------------------------------------------------
PlusStatus vtkPlusAndorVideoSource::InternalConnect()
{
  LOG_TRACE("vtkPlusAndorVideoSource::InternalConnect");
  if(this->InitializeAndorCamera() != PLUS_SUCCESS)
  {
    LOG_ERROR("Andor camera failed to initialize.");
    return PLUS_FAIL;
  }

  this->GetVideoSourcesByPortName("BLIRaw", BLIRaw);
  this->GetVideoSourcesByPortName("BLICorrected", BLICorrected);
  this->GetVideoSourcesByPortName("GrayRaw", GrayRaw);
  this->GetVideoSourcesByPortName("GrayCorrected", GrayCorrected);

  if(BLIRaw.size() + BLICorrected.size() + GrayRaw.size() + GrayCorrected.size() == 0)
  {
    vtkPlusDataSource* aSource = nullptr;
    if(this->GetFirstActiveOutputVideoSource(aSource) != PLUS_SUCCESS || aSource == nullptr)
    {
      LOG_ERROR("Standard data sources are not defined, and unable to retrieve the video source in the capturing device.");
      return PLUS_FAIL;
    }
    BLIRaw.push_back(aSource); // this is the default port
  }

  this->InitializePort(BLIRaw);
  this->InitializePort(BLICorrected);
  this->InitializePort(GrayRaw);
  this->InitializePort(GrayCorrected);

  return PLUS_SUCCESS;
}

// ----------------------------------------------------------------------------
PlusStatus vtkPlusAndorVideoSource::InternalDisconnect()
{
  LOG_DEBUG("Disconnecting from Andor");
  if(IsRecording())
  {
    this->InternalStopRecording();
  }

  // // From Andor Employee:
  // // Only Classic, ICCD and cameras with a fibre attached must have their cooling and warming up controlled at a particular rate.
  // // For everything else you can just call ShutDown and the camera will safely return to room temperature.
  // // Classic systems are cameras that use our original PCI controller cards eg CCI-010 or CCI-001.
  // int status;
  // checkStatus(::IsCoolerOn(&status), "IsCoolerOn");

  // if(status && this->CoolerMode == 0)
  // {
  //   LOG_INFO("CoolerMode 0 and Cooler is still ON. Turning off the cooler and waiting for warmup. Do not unplug the camera from power.");
  //   WaitForWarmup();
  // }

  checkStatus(FreeInternalMemory(), "FreeInternalMemory");

  unsigned result = checkStatus(ShutDown(), "ShutDown");
  if(result == DRV_SUCCESS)
  {
    LOG_INFO("Andor camera shut down successfully.");
  }

  return PLUS_SUCCESS;
}


// ----------------------------------------------------------------------------
PlusStatus vtkPlusAndorVideoSource::InternalStartRecording()
{
  return PLUS_SUCCESS;
}

// ----------------------------------------------------------------------------
PlusStatus vtkPlusAndorVideoSource::InternalStopRecording()
{
  return PLUS_SUCCESS;
}

// ----------------------------------------------------------------------------
void vtkPlusAndorVideoSource::AdjustBuffers(int horizontalBins, int verticalBins)
{
  int x, y;
  checkStatus(GetDetector(&x, &y), "GetDetector");  // full sensor size

  frameSize[0] = x / horizontalBins;
  frameSize[1] = y / verticalBins;

  for(unsigned i = 0; i < BLIRaw.size(); i++)
  {
    BLIRaw[i]->SetInputFrameSize(frameSize);
  }
  for(unsigned i = 0; i < BLICorrected.size(); i++)
  {
    BLICorrected[i]->SetInputFrameSize(frameSize);
  }
  for(unsigned i = 0; i < GrayRaw.size(); i++)
  {
    GrayRaw[i]->SetInputFrameSize(frameSize);
  }
  for(unsigned i = 0; i < GrayCorrected.size(); i++)
  {
    GrayCorrected[i]->SetInputFrameSize(frameSize);
  }
}

// ----------------------------------------------------------------------------
void vtkPlusAndorVideoSource::AdjustSpacing(int horizontalBins, int verticalBins)
{
  std::ostringstream spacingStream;
  spacingStream << this->OutputSpacing[0] * horizontalBins << " ";
  spacingStream << this->OutputSpacing[1] * verticalBins << " ";
  spacingStream << this->OutputSpacing[2];

  this->CustomFields["ElementSpacing"].first = FRAMEFIELD_FORCE_SERVER_SEND;
  this->CustomFields["ElementSpacing"].second = spacingStream.str();
  LOG_DEBUG("Adjusted spacing: " << spacingStream.str());
}

// ----------------------------------------------------------------------------
std::vector<double> vtkPlusAndorVideoSource::GetSpacing(int horizontalBins, int verticalBins)
{
  std::vector<double> spacing = { 0.0, 0.0, 1.0 };
  spacing[0] = this->OutputSpacing[0] * horizontalBins;
  spacing[1] = this->OutputSpacing[1] * verticalBins;
  return spacing;
}

// ----------------------------------------------------------------------------
float vtkPlusAndorVideoSource::GetCurrentTemperature()
{
  checkStatus(GetTemperatureF(&this->CurrentTemperature), "GetTemperatureF");
  return this->CurrentTemperature;
}

// ----------------------------------------------------------------------------
void vtkPlusAndorVideoSource::WaitForCooldown()
{
  if(this->RequireCoolTemp == false)
  {
    return;
  }
  int status;
  checkStatus(::IsCoolerOn(&status), "IsCoolerOn");
  if(!status)
  {
    TurnCoolerON();
  }
  while(checkStatus(GetTemperatureF(&this->CurrentTemperature), "GetTemperatureF") != DRV_TEMPERATURE_STABILIZED)
  {
    igtl::Sleep(5000); // wait a bit
  }
}

// ----------------------------------------------------------------------------
void vtkPlusAndorVideoSource::WaitForWarmup()
{
  int status;
  checkStatus(::IsCoolerOn(&status), "IsCoolerOn");
  if(status)
  {
    TurnCoolerOFF();
  }
  GetCurrentTemperature(); // updates this->CurrentTemperature
  if(this->CurrentTemperature < this->SafeTemperature)
  {
    while(this->CurrentTemperature < this->SafeTemperature)
    {
      igtl::Sleep(5000); // wait a bit
      GetCurrentTemperature(); // logs the status and temperature
    }
  }
}

// ----------------------------------------------------------------------------
PlusStatus vtkPlusAndorVideoSource::AcquireFrame(float exposure, ShutterMode shutterMode, int binning, int vsSpeed, int hsSpeed)
{
  checkStatus(::SetExposureTime(exposure), "SetExposureTime");
  checkStatus(::SetShutter(1, shutterMode, 0, 0), "SetShutter");

  int hbin = binning > 0 ? binning : this->HorizontalBins;
  int vbin = binning > 0 ? binning : this->VerticalBins;
  checkStatus(::SetImage(hbin, vbin, 1, 1024, 1, 1024), "Binning");
  AdjustBuffers(hbin, vbin);

  AdjustSpacing(hbin, vbin);

  int vsInd = vsSpeed >= 0 ? vsSpeed : this->VSSpeed;
  checkStatus(::SetVSSpeed(vsInd), "SetVSSpeed");

  int hsInd = hsSpeed >= 0 ? hsSpeed : this->HSSpeed[1];
  checkStatus(::SetHSSpeed(this->HSSpeed[0], hsInd), "SetHSSpeed");

  unsigned rawFrameSize = frameSize[0] * frameSize[1];
  rawFrame.resize(rawFrameSize, 0);

  checkStatus(StartAcquisition(), "StartAcquisition");
  unsigned result = checkStatus(WaitForAcquisition(), "WaitForAcquisition");
  if(result == DRV_NO_NEW_DATA)   // Log a more specific log message for WaitForAcquisition
  {
    LOG_ERROR("Non-Acquisition Event occurred.(e.g. CancelWait() called)");
  }
  this->currentTime = vtkIGSIOAccurateTimer::GetSystemTime();

  // iKon-M 934 has 16-bit digitization
  // https://andor.oxinst.com/assets/uploads/products/andor/documents/andor-ikon-m-934-specifications.pdf
  // so we choose 16-bit unsigned
  // GetMostRecentImage() is 32 bit signed variant
  checkStatus(GetMostRecentImage16(&rawFrame[0], rawFrameSize), "GetMostRecentImage16");

  return PLUS_SUCCESS;
}

// ----------------------------------------------------------------------------
void vtkPlusAndorVideoSource::AddFrameToDataSource(DataSourceArray& ds)
{
  for(unsigned i = 0; i < ds.size(); i++)
  {
    if(ds[i]->AddItem(&rawFrame[0],
                      US_IMG_ORIENT_MF,
                      frameSize, VTK_UNSIGNED_SHORT,
                      1, US_IMG_BRIGHTNESS, 0,
                      this->FrameNumber,
                      currentTime,
                      UNDEFINED_TIMESTAMP,
                      &this->CustomFields
                     ) != PLUS_SUCCESS)
    {
      LOG_WARNING("Error adding item to AndorCamera video source " << ds[i]->GetSourceId());
    }
    else
    {
      LOG_INFO("Success adding item to AndorCamera video source " << ds[i]->GetSourceId());
    }
  }
}

// ----------------------------------------------------------------------------
void vtkPlusAndorVideoSource::FindBadCells(int binning)
{
    std::vector<cv::Point> badIndicesXY;
    cv::findNonZero(cvBadPixelImage, badIndicesXY);

    std::map<uint, int> badPixelCount;
    for (int i = 0; i < badIndicesXY.size(); i++)
    {
      uint resolutionCellIndexX = badIndicesXY[i].x / binning;
      uint resolutionCellIndexY = badIndicesXY[i].y / binning;
      uint resolutionCellIndex = frameSize[1] * resolutionCellIndexY + resolutionCellIndexX;
      badPixelCount[resolutionCellIndex] += 1;
    }

    std::vector<uint> resolutionCellsToCorrect;
    for (auto const& bpc : badPixelCount)
    {
      if (binning * binning / bpc.second < 5) // tolerate up to 20% dead pixels
      {
        resolutionCellsToCorrect.push_back(bpc.first);
      }
    }

    cellsToCorrect[binning] = resolutionCellsToCorrect;
}

// ----------------------------------------------------------------------------
void vtkPlusAndorVideoSource::CorrectBadPixels(int binning, cv::Mat& cvIMG)
{
    if (cellsToCorrect.find(binning) == cellsToCorrect.end()) // it needs to be calculated
    {
      FindBadCells(binning);
    }
    std::vector<uint> resolutionCellsToCorrect = cellsToCorrect[binning];
    uint resolutionCellIndexX, resolutionCellIndexY;
    std::vector<uint> valuesForMedian, correctedCells;
    uint medianValue;
    int startX, startY;
    unsigned endX, endY;
    int numCellsToCorrect = resolutionCellsToCorrect.size();
    for (uint cell : resolutionCellsToCorrect)
	  {
      resolutionCellIndexX = cell - frameSize[0] * (cell / frameSize[0]);
      resolutionCellIndexY = cell / frameSize[0];
      startX = resolutionCellIndexX - 1;
      endX = resolutionCellIndexX + 1;
      startY = resolutionCellIndexY - 1;
      endY = resolutionCellIndexY + 1;
      if (startX < 0) { startX = 0; }
      if (startY < 0) { startY = 0; }
      if (endX > frameSize[0]) { endX = frameSize[0]; }
      if (endY > frameSize[0]) { endY = frameSize[0]; }

      for (uint x = startX; x <= endX; x++)
      {
        for (uint y = startY; y <= endY; y++)
        {
          if (std::find(resolutionCellsToCorrect.begin(), resolutionCellsToCorrect.end(), frameSize[0] * y + x) != resolutionCellsToCorrect.end())
          {
            if (std::find(correctedCells.begin(), correctedCells.end(), frameSize[0] * y + x) != correctedCells.end())
              {
                valuesForMedian.push_back(cvIMG.at<ushort>(y, x));
              }
          }
          else
          {
            valuesForMedian.push_back(cvIMG.at<ushort>(y, x));
          }
        }
      }

      sort(valuesForMedian.begin(), valuesForMedian.end());
      if (valuesForMedian.size() % 2 == 0)
      {
        medianValue = (valuesForMedian[valuesForMedian.size() / 2 - 1] + valuesForMedian[valuesForMedian.size() / 2]) / 2;
      }
      else
      {
        medianValue = valuesForMedian[valuesForMedian.size() / 2];
      }

      cvIMG.at<ushort>(resolutionCellIndexY, resolutionCellIndexX) = medianValue;
      correctedCells.push_back(cell);
      valuesForMedian.clear();
    }
}

// ----------------------------------------------------------------------------
void vtkPlusAndorVideoSource::ApplyFrameCorrections(int binning)
{
  cv::Mat cvIMG(frameSize[0], frameSize[1], CV_16UC1, &rawFrame[0]); // uses rawFrame as buffer
  CorrectBadPixels(binning, cvIMG);
  LOG_INFO("Applied bad pixel correction");

  cv::Mat floatImage;
  cvIMG.convertTo(floatImage, CV_32FC1);
  cv::Mat result;

  cv::subtract(floatImage, cvBiasDarkCorrection, floatImage, cv::noArray(), CV_32FC1);
  LOG_INFO("Applied constant bias+dark correction");

  // OpenCV's lens distortion correction
  cv::undistort(floatImage, result, cvCameraIntrinsics, cvDistanceCoefficients);
  LOG_INFO("Applied lens distortion correction");

  // Divide the image by the 32-bit floating point correction image
  cv::divide(result, cvFlatCorrection, result, 1, CV_32FC1);
  LOG_INFO("Applied multiplicative flat correction");
  result.convertTo(cvIMG, CV_16UC1);
}

// ----------------------------------------------------------------------------
PlusStatus vtkPlusAndorVideoSource::AcquireBLIFrame(int binning, int vsSpeed, int hsSpeed, float exposureTime)
{
  WaitForCooldown();
  AcquireFrame(exposureTime, ShutterMode::FullyAuto, binning, vsSpeed, hsSpeed);
  ++this->FrameNumber;
  AddFrameToDataSource(BLIRaw);

  if(this->UseFrameCorrections)
  {
    ApplyFrameCorrections(binning);
    AddFrameToDataSource(BLICorrected);
  }

  return PLUS_SUCCESS;
}

// ----------------------------------------------------------------------------
PlusStatus vtkPlusAndorVideoSource::AcquireGrayscaleFrame(int binning, int vsSpeed, int hsSpeed, float exposureTime)
{
  WaitForCooldown();
  AcquireFrame(exposureTime, ShutterMode::FullyAuto, binning, vsSpeed, hsSpeed);
  ++this->FrameNumber;
  AddFrameToDataSource(GrayRaw);

  if(this->UseFrameCorrections)
  {
    ApplyFrameCorrections(binning);
    AddFrameToDataSource(GrayCorrected);
  }

  return PLUS_SUCCESS;
}

// ----------------------------------------------------------------------------
PlusStatus vtkPlusAndorVideoSource::AcquireCorrectionFrame(const std::string correctionFilePath, ShutterMode shutter, int binning, int vsSpeed, int hsSpeed, float exposureTime)
{
  AcquireFrame(exposureTime, shutter, binning, vsSpeed, hsSpeed);
  ++this->FrameNumber;

  cv::Mat cvIMG(frameSize[0], frameSize[1], CV_16UC1, &rawFrame[0]); // uses rawFrame as buffer
  if(this->UseFrameCorrections)
  {
    CorrectBadPixels(binning, cvIMG);
    LOG_INFO("Applied bad pixel correction");
  }

  cv::imwrite(correctionFilePath, cvIMG);
  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusAndorVideoSource::SetBadPixelCorrectionImage(const std::string badPixelFilePath)
{
  try
  {
    cellsToCorrect.clear();
    cvBadPixelImage = cv::imread(badPixelFilePath, cv::IMREAD_GRAYSCALE);
    if (cvBadPixelImage.empty())
    {
        throw "Bad pixel image empty!";
    }
  }
  catch (...)
  {
    LOG_ERROR("Could not load bad pixel image from file: " << badPixelFilePath);
    return PLUS_FAIL;
  }
  return PLUS_SUCCESS;
}

// ----------------------------------------------------------------------------
PlusStatus vtkPlusAndorVideoSource::SetBiasDarkCorrectionImage(const std::string biasDarkFilePath)
{
  try
  {
    cvBiasDarkCorrection = cv::imread(biasDarkFilePath, cv::IMREAD_GRAYSCALE);
    if(cvBiasDarkCorrection.empty())
    {
      throw "Bias+dark correction image empty!";
    }
  }
  catch(...)
  {
    LOG_ERROR("Could not load bias+dark correction image from file: " << biasDarkFilePath);
    return PLUS_FAIL;
  }
  return PLUS_SUCCESS;
}

// ----------------------------------------------------------------------------
PlusStatus vtkPlusAndorVideoSource::SetFlatCorrectionImage(std::string flatFilePath)
{
  try
  {
    cvFlatCorrection = cv::imread(flatFilePath, cv::IMREAD_GRAYSCALE);
    if(cvFlatCorrection.empty())
    {
      throw "Flat correction image empty!";
    }
    double maxVal = 0.0;
    cv::minMaxLoc(cvFlatCorrection, nullptr, &maxVal);
    if(maxVal > 1.0)   // we need to normalize the image to [0.0, 1.0] range
    {
      cv::Mat temp;
      cvFlatCorrection.convertTo(temp, CV_32FC1, 1.0 / maxVal);
      cvFlatCorrection = temp;
    }
  }
  catch(...)
  {
    LOG_ERROR("Could not load flat correction image from file: " << flatFilePath);
    return PLUS_FAIL;
  }
  return PLUS_SUCCESS;
}

// Setup the Andor camera parameters ----------------------------------------------

// ----------------------------------------------------------------------------
PlusStatus vtkPlusAndorVideoSource::SetShutter(ShutterMode shutter)
{
  this->Shutter = shutter;
  checkStatus(::SetShutter(1, this->Shutter, 0, 0), "SetShutter");
  return PLUS_SUCCESS;
}

// ----------------------------------------------------------------------------
vtkPlusAndorVideoSource::ShutterMode vtkPlusAndorVideoSource::GetShutter()
{
  return this->Shutter;
}

// ----------------------------------------------------------------------------
PlusStatus vtkPlusAndorVideoSource::SetExposureTime(float exposureTime)
{
  this->ExposureTime = exposureTime;
  checkStatus(::SetExposureTime(this->ExposureTime), "SetExposureTime");
  return PLUS_SUCCESS;
}

// ----------------------------------------------------------------------------
float vtkPlusAndorVideoSource::GetExposureTime()
{
  return this->ExposureTime;
}

// ----------------------------------------------------------------------------
PlusStatus vtkPlusAndorVideoSource::SetHorizontalBins(int bins)
{
  int x, y;
  checkStatus(GetDetector(&x, &y), "GetDetector");  // full sensor size
  unsigned status = checkStatus(::SetImage(bins, this->VerticalBins, 1, x, 1, y), "SetImage");
  if(status != DRV_SUCCESS)
  {
    LOG_ERROR("SetImage failed while changing horizontal bins.");
    return PLUS_FAIL;
  }
  this->HorizontalBins = bins;
  return PLUS_SUCCESS;
}

// ----------------------------------------------------------------------------
PlusStatus vtkPlusAndorVideoSource::SetVerticalBins(int bins)
{
  int x, y;
  checkStatus(GetDetector(&x, &y), "GetDetector");  // full sensor size
  unsigned status = checkStatus(::SetImage(this->HorizontalBins, bins, 1, x, 1, y), "SetImage");
  if(status != DRV_SUCCESS)
  {
    LOG_ERROR("SetImage failed while changing vertical bins.");
    return PLUS_FAIL;
  }
  this->VerticalBins = bins;
  return PLUS_SUCCESS;
}

// ----------------------------------------------------------------------------
PlusStatus vtkPlusAndorVideoSource::SetHSSpeed(int type, int index)
{
  unsigned status = checkStatus(::SetHSSpeed(type, index), "SetHSSpeed");
  if(status != DRV_SUCCESS)
  {
    LOG_ERROR("SetHSSpeed command failed.");
    return PLUS_FAIL;
  }
  HSSpeed[0] = type;
  HSSpeed[1] = index;
  return PLUS_SUCCESS;
}

// ----------------------------------------------------------------------------
PlusStatus vtkPlusAndorVideoSource::SetVSSpeed(int index)
{
  unsigned status = checkStatus(::SetVSSpeed(index), "SetVSSpeed");
  if(status != DRV_SUCCESS)
  {
    LOG_ERROR("SetVSSpeed command failed.");
    return PLUS_FAIL;
  }
  this->VSSpeed = index;
  return PLUS_SUCCESS;
}

// ----------------------------------------------------------------------------
PlusStatus vtkPlusAndorVideoSource::SetPreAmpGain(int preAmpGain)
{
  this->PreAmpGain = preAmpGain;
  unsigned status = checkStatus(::SetPreAmpGain(this->PreAmpGain), "SetPreAmpGain");
  if(status == DRV_P1INVALID)
  {
    LOG_ERROR("Minimum threshold outside valid range (1-65535).");
  }
  else if(status == DRV_P2INVALID)
  {
    LOG_ERROR("Maximum threshold outside valid range.");
  }
  return PLUS_SUCCESS;
}

// ----------------------------------------------------------------------------
int vtkPlusAndorVideoSource::GetPreAmpGain()
{
  return this->PreAmpGain;
}

// ----------------------------------------------------------------------------
PlusStatus vtkPlusAndorVideoSource::SetAcquisitionMode(AcquisitionMode acquisitionMode)
{
  this->m_AcquisitionMode = acquisitionMode;
  checkStatus(::SetAcquisitionMode(this->m_AcquisitionMode), "SetAcquisitionMode");
  return PLUS_SUCCESS;
}

// ----------------------------------------------------------------------------
vtkPlusAndorVideoSource::AcquisitionMode vtkPlusAndorVideoSource::GetAcquisitionMode()
{
  return this->m_AcquisitionMode;
}

// ----------------------------------------------------------------------------
PlusStatus vtkPlusAndorVideoSource::SetReadMode(ReadMode readMode)
{
  this->m_ReadMode = readMode;
  checkStatus(::SetReadMode(this->m_ReadMode), "SetReadMode");
  return PLUS_SUCCESS;
}

// ----------------------------------------------------------------------------
vtkPlusAndorVideoSource::ReadMode vtkPlusAndorVideoSource::GetReadMode()
{
  return this->m_ReadMode;
}

// ----------------------------------------------------------------------------
PlusStatus vtkPlusAndorVideoSource::SetTriggerMode(TriggerMode triggerMode)
{
  this->m_TriggerMode = triggerMode;
  checkStatus(::SetTriggerMode(this->m_TriggerMode), "SetTriggerMode");
  return PLUS_SUCCESS;
}

// ----------------------------------------------------------------------------
vtkPlusAndorVideoSource::TriggerMode vtkPlusAndorVideoSource::GetTriggerMode()
{
  return this->m_TriggerMode;
}

// ----------------------------------------------------------------------------
PlusStatus vtkPlusAndorVideoSource::SetUseFrameCorrections(bool useFrameCorrections)
{
  this->UseFrameCorrections = useFrameCorrections;
  return PLUS_SUCCESS;
}

// ----------------------------------------------------------------------------
bool vtkPlusAndorVideoSource::GetUseFrameCorrections()
{
  return this->UseFrameCorrections;
}

// ----------------------------------------------------------------------------
PlusStatus vtkPlusAndorVideoSource::SetRequireCoolTemp(bool requireCoolTemp)
{
  this->RequireCoolTemp = requireCoolTemp;
  return PLUS_SUCCESS;
}

// ----------------------------------------------------------------------------
PlusStatus vtkPlusAndorVideoSource::SetInitializeCoolerState(bool InitializeCoolerState)
{
  if(InitializeCoolerState)
  {
    SetCoolerState(InitializeCoolerState);
  }
  return PLUS_SUCCESS;
}

// ----------------------------------------------------------------------------
PlusStatus vtkPlusAndorVideoSource::SetCoolerState(bool coolerState)
{
  int coolerStatus = IsCoolerOn();
  if(coolerState && coolerStatus == 0)  // Wanting to turn cooler on and cooler is currently off
  {
    TurnCoolerON();
  }
  else if(coolerState == false && coolerStatus == 1)  // Wanting to turn cooler off and cooler is currently on
  {
    TurnCoolerOFF();
  }

  return PLUS_SUCCESS;
}

// ----------------------------------------------------------------------------
bool vtkPlusAndorVideoSource::IsCoolerOn()
{
  int coolerStatus = 1;
  unsigned result = checkStatus(::IsCoolerOn(&coolerStatus), "IsCoolerOn");
  if(result == DRV_SUCCESS)
  {
    return bool(coolerStatus);
  }
  return -1;
}

// ----------------------------------------------------------------------------
PlusStatus vtkPlusAndorVideoSource::TurnCoolerON()
{
  unsigned result = checkStatus(CoolerON(), "CoolerON");
  if(result == DRV_SUCCESS)
  {
    LOG_INFO("Temperature controller switched ON.");
    checkStatus(SetTemperature(this->CoolTemperature), "SetTemperature");
    return PLUS_SUCCESS;
  }
  LOG_ERROR("CoolerON command failed to execute.");
  return PLUS_FAIL;
}

// ----------------------------------------------------------------------------
PlusStatus vtkPlusAndorVideoSource::TurnCoolerOFF()
{
  unsigned result = checkStatus(CoolerOFF(), "CoolerOFF");
  if(result == DRV_SUCCESS)
  {
    LOG_INFO("Temperature controller switched OFF.");
    return PLUS_SUCCESS;
  }
  LOG_ERROR("CoolerOFF command failed to execute.");
  return PLUS_FAIL;
}

// ----------------------------------------------------------------------------
PlusStatus vtkPlusAndorVideoSource::SetCoolerMode(int mode)
{
  unsigned result = checkStatus(::SetCoolerMode(mode), "SetCoolerMode");
  if(result == DRV_SUCCESS)
  {
    this->CoolerMode = mode;
    if(mode == 1)
    {
      LOG_INFO("Cooler mode set to 1. If the cooler is ON, temperature will be maintained on Shutdown, otherwise Camera will return to ambient temperature.");
    }
    else
    {
      LOG_INFO("Cooler mode set to 0. Camera will return to ambient temperature on ShutDown.");
    }
    return PLUS_SUCCESS;
  }
  LOG_ERROR("SetCoolerMode command failed to execute.");
  return PLUS_FAIL;
}

// ----------------------------------------------------------------------------
int vtkPlusAndorVideoSource::GetCoolerMode()
{
  return this->CoolerMode;
}

// ----------------------------------------------------------------------------
PlusStatus vtkPlusAndorVideoSource::SetCoolTemperature(int coolTemp)
{
  this->CoolTemperature = coolTemp;

  return PLUS_SUCCESS;
}

// ----------------------------------------------------------------------------
int vtkPlusAndorVideoSource::GetCoolTemperature()
{
  return this->CoolTemperature;
}

// ----------------------------------------------------------------------------
PlusStatus vtkPlusAndorVideoSource::SetSafeTemperature(int safeTemp)
{
  this->SafeTemperature = safeTemp;

  return PLUS_SUCCESS;
}

// ----------------------------------------------------------------------------
int vtkPlusAndorVideoSource::GetSafeTemperature()
{
  return this->SafeTemperature;
}

// ----------------------------------------------------------------------------
unsigned int vtkPlusAndorVideoSource::checkStatus(unsigned int returnStatus, std::string functionName)
{
  if(returnStatus == DRV_SUCCESS)
  {
    return returnStatus;
  }
  else if(returnStatus == DRV_NOT_INITIALIZED)
  {
    LOG_ERROR("Failed AndorSDK operation: " << functionName
              << "; Driver is not initialized.");
  }
  else if(returnStatus == DRV_ACQUIRING)
  {
    LOG_ERROR("Failed AndorSDK operation: " << functionName
              << "; Not allowed. Currently acquiring data.");
  }
  else if(returnStatus == DRV_P1INVALID)
  {
    LOG_ERROR("Failed AndorSDK operation: " << functionName
              << "; Parameter 1 not valid.");
  }
  else if(returnStatus == DRV_P2INVALID)
  {
    LOG_ERROR("Failed AndorSDK operation: " << functionName
              << "; Parameter 2 not valid.");
  }
  else if(returnStatus == DRV_P3INVALID)
  {
    LOG_ERROR("Failed AndorSDK operation: " << functionName
              << "; Parameter 3 not valid.");
  }
  else if(returnStatus == DRV_P4INVALID)
  {
    LOG_ERROR("Failed AndorSDK operation: " << functionName
              << "; Parameter 4 not valid.");
  }
  else if(returnStatus == DRV_P5INVALID)
  {
    LOG_ERROR("Failed AndorSDK operation: " << functionName
              << "; Parameter 5 not valid.");
  }
  else if(returnStatus == DRV_P6INVALID)
  {
    LOG_ERROR("Failed AndorSDK operation: " << functionName
              << "; Parameter 6 not valid.");
  }
  else if(returnStatus == DRV_P7INVALID)
  {
    LOG_ERROR("Failed AndorSDK operation: " << functionName
              << "; Parameter 7 not valid.");
  }
  else if(returnStatus == DRV_ERROR_ACK)
  {
    LOG_ERROR("Failed AndorSDK operation: " << functionName
              << "; Unable to communicate with card.");
  }
  else if(returnStatus == DRV_TEMP_OFF)
  {
    LOG_INFO("Cooler is OFF. Current temperature is " << this->CurrentTemperature << " °C");
  }
  else if(returnStatus == DRV_TEMPERATURE_STABILIZED)
  {
    LOG_INFO("Temperature has stabilized at " << this->CurrentTemperature << " °C");
  }
  else if(returnStatus == DRV_TEMPERATURE_NOT_REACHED)
  {
    LOG_INFO("Cooling down, current temperature is " << this->CurrentTemperature << " °C");
  }
  else if(returnStatus == DRV_TEMP_DRIFT)
  {
    LOG_INFO("Temperature had stabilised but has since drifted. Current temperature is " << this->CurrentTemperature << " °C");
  }
  else if(returnStatus == DRV_TEMP_NOT_STABILIZED)
  {
    LOG_INFO("Temperature reached but not stabilized. Current temperature is " << this->CurrentTemperature << " °C");
  }
  else if(returnStatus == DRV_VXDNOTINSTALLED)
  {
    LOG_ERROR("Failed AndorSDK operation: " << functionName
              << "; VxD not loaded.");
  }
  else if(returnStatus == DRV_INIERROR)
  {
    LOG_ERROR("Failed AndorSDK operation: " << functionName
              << "; Unable to load DETECTOR.INI.");
  }
  else if(returnStatus == DRV_COFERROR)
  {
    LOG_ERROR("Failed AndorSDK operation: " << functionName
              << "; Unable to load *.COF.");
  }
  else if(returnStatus == DRV_FLEXERROR)
  {
    LOG_ERROR("Failed AndorSDK operation: " << functionName
              << "; Unable to load *.RBF.");
  }
  else if(returnStatus == DRV_ERROR_FILELOAD)
  {
    LOG_ERROR("Failed AndorSDK operation: " << functionName
              << "; Unable to load *.COF or *.RBF files.");
  }
  else if(returnStatus == DRV_USBERROR)
  {
    LOG_ERROR("Failed AndorSDK operation: " << functionName
              << "; Unable to detect USB device or not USB 2.0.");
  }
  else if(returnStatus == DRV_ERROR_NOCAMERA)
  {
    LOG_ERROR("Failed AndorSDK operation: " << functionName
              << "; No camera found.");
  }
  else if(returnStatus == DRV_GENERAL_ERRORS)
  {
    LOG_ERROR("Failed AndorSDK operation: " << functionName
              << "; An error occured while obtaining the number of available cameras.");
  }
  else if(returnStatus == DRV_INVALID_MODE)
  {
    LOG_ERROR("Failed AndorSDK operation: " << functionName
              << "; Invalid mode or mode not available.");
  }
  else if(returnStatus == DRV_ERROR_PAGELOCK)
  {
    LOG_ERROR("Failed AndorSDK operation: " << functionName
              << "; Unable to allocate memory.");
  }
  else if(returnStatus == DRV_INVALID_FILTER)
  {
    LOG_ERROR("Failed AndorSDK operation: " << functionName
              << "; Filter not available for current acquisition.");
  }
  else if(returnStatus == DRV_BINNING_ERROR)
  {
    LOG_ERROR("Failed AndorSDK operation: " << functionName
              << "; Range not a multiple of horizontal binning.");
  }
  else if(returnStatus == DRV_SPOOLSETUPERROR)
  {
    LOG_ERROR("Failed AndorSDK operation: " << functionName
              << "; Error with spool settings.");
  }
  else if(returnStatus == DRV_IDLE)
  {
    LOG_ERROR("Failed AndorSDK operation: " << functionName
              << "; The system is not currently acquiring.");
  }
  else if(returnStatus == DRV_NO_NEW_DATA)
  {
    LOG_ERROR("Failed AndorSDK operation: " << functionName
              << "; There is no new data yet.");
  }
  else if(returnStatus == DRV_ERROR_CODES)
  {
    LOG_ERROR("Failed AndorSDK operation: " << functionName
              << "; Problem communicating with camera.");
  }
  else if(returnStatus == DRV_LOAD_FIRMWARE_ERROR)
  {
    LOG_ERROR("Failed AndorSDK operation: " << functionName
              << "; Error loading firmware.");
  }
  else if(returnStatus == DRV_NOT_SUPPORTED)
  {
    LOG_ERROR("Failed AndorSDK operation: " << functionName
              << "; Feature not supported.");
  }
  else if(returnStatus == DRV_RANDOM_TRACK_ERROR)
  {
    LOG_ERROR("Failed AndorSDK operation: " << functionName
              << "; Invalid combination of tracks.");
  }
  else
  {
    LOG_WARNING("Possible failed AndorSDK operation: " << functionName
                << "; Unknown return code " << returnStatus << "returned.");
  }

  return returnStatus;
}
