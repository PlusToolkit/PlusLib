/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"
#include "vtkPlusSpinnakerVideoSource.h"

// Local includes
#include "vtkPlusChannel.h"
#include "vtkPlusDataSource.h"

// Spinnaker API includes
#include "Spinnaker.h"
#include "SpinGenApi/SpinnakerGenApi.h"

// STL includes
#include <map>
#include <sstream>

//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkPlusSpinnakerVideoSource);

namespace
{
  const unsigned int DEFAULT_DEVICE_ID = 0;
  const FrameSizeType DEFAULT_FRAME_SIZE = { 640, 480, 1 };
  const unsigned int DEFAULT_FRAME_RATE = 30;
}

//----------------------------------------------------------------------------
class vtkPlusSpinnakerVideoSource::vtkInternal
{
public:
  vtkPlusSpinnakerVideoSource * External;

  vtkInternal(vtkPlusSpinnakerVideoSource* external)
    : External(external)
  {
  }

  virtual ~vtkInternal()
  {
  }

  // Singleton reference to system object
  Spinnaker::SystemPtr SystemPtr;
};

//----------------------------------------------------------------------------
vtkPlusSpinnakerVideoSource::vtkPlusSpinnakerVideoSource()
  : Internal(new vtkInternal(this))
{
  LOG_TRACE("vtkPlusSpinnakerVideoSource::vtkPlusSpinnakerVideoSource()");
  this->RequireImageOrientationInConfiguration = true;
  this->StartThreadForInternalUpdates = true;
  this->InternalUpdateRate = 30;
  this->AcquisitionRate = 30;
}

//----------------------------------------------------------------------------
vtkPlusSpinnakerVideoSource::~vtkPlusSpinnakerVideoSource()
{
  LOG_TRACE("vtkPlusSpinnakerVideoSource::~vtkPlusSpinnakerVideoSource()");

  delete Internal;
  Internal = nullptr;
}

//----------------------------------------------------------------------------
void vtkPlusSpinnakerVideoSource::PrintConfiguration(ostream& os, vtkIndent indent)
{
  // define enum maps
  std::map<EXPOSURE_MODE, std::string> ExposureModeToString;
  ExposureModeToString[EXPOSURE_TIMED] = "TIMED";
  ExposureModeToString[EXPOSURE_AUTO_ONCE] = "AUTO_ONCE";
  ExposureModeToString[EXPOSURE_AUTO_CONTINUOUS] = "AUTO_CONTINUOUS";

  std::map<GAIN_MODE, std::string> GainModeToString;
  GainModeToString[GAIN_MANUAL] = "MANUAL";
  GainModeToString[GAIN_AUTO_ONCE] = "AUTO_ONCE";
  GainModeToString[GAIN_AUTO_CONTINUOUS] = "AUTO_CONTINUOUS";

  std::map<WHITE_BALANCE_MODE, std::string> WhiteBalanceModeToString;
  WhiteBalanceModeToString[WB_MANUAL] = "MANUAL";
  WhiteBalanceModeToString[WB_AUTO_ONCE] = "AUTO_ONCE";
  WhiteBalanceModeToString[WB_AUTO_CONTINUOUS] = "AUTO_CONTINUOUS";

  std::map<SHARPENING_MODE, std::string> SharpeningModeToString;
  SharpeningModeToString[SHARPENING_MANUAL] = "MANUAL";
  SharpeningModeToString[SHARPENING_AUTO] = "AUTO";

  // print device parameters
  os << indent << "CameraNumber: " << this->CameraNumber << std::endl;
  os << indent << "VideoFormat:" << this->VideoFormat << std::endl;
  os << indent << "FrameSize:" << "[" << this->FrameSize[0] << ", " << this->FrameSize[1] << "]" << std::endl;
  os << indent << "FrameRate:" << this->FrameRate << std::endl;
  os << indent << "ExposureMode:" << ExposureModeToString.find(this->ExposureMode)->second << std::endl;
  if (this->ExposureMode == EXPOSURE_TIMED)
  {
    os << indent << "ExposureMicroSec:" << this->ExposureMicroSec << std::endl;
  }
  os << indent << "GainMode:" << GainModeToString.find(this->GainMode)->second << std::endl;
  if (this->GainMode == GAIN_MANUAL)
  {
    os << indent << "GainDB:" << this->GainDB << std::endl;
  }
  os << indent << "WhiteBalanceMode:" << WhiteBalanceModeToString.find(this->WhiteBalanceMode)->second << std::endl;
  if (this->WhiteBalanceMode == WB_MANUAL)
  {
    os << indent << "WhiteBalance:" << this->WhiteBalance << std::endl;
  }
  os << indent << "SharpeningMode:" << SharpeningModeToString.find(this->SharpeningMode)->second << std::endl;
  if (this->SharpeningMode == SHARPENING_MANUAL)
  {
    os << indent << "Sharpening:" << this->Sharpening << std::endl;
  }
}

//----------------------------------------------------------------------------
void vtkPlusSpinnakerVideoSource::PrintSelf(ostream& os, vtkIndent indent)
{
  LOG_TRACE("vtkPlusSpinnakerVideoSource::PrintSelf(ostream& os, vtkIndent indent)");

  this->Superclass::PrintSelf(os, indent);
  this->PrintConfiguration(os, indent);
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusSpinnakerVideoSource::SetFrameRate(int frameRate)
{
  if (frameRate < 1)
  {
    LOG_WARNING("Requested frame rate of " << frameRate << " (fps) is too low. Using default of " << DEFAULT_FRAME_RATE << ".");
    this->FrameRate = DEFAULT_FRAME_RATE;
    return PLUS_FAIL;
  }
  this->AcquisitionRate = frameRate;
  this->InternalUpdateRate = frameRate;
  this->FrameRate = frameRate;
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusSpinnakerVideoSource::SetExposureMode(EXPOSURE_MODE expMode)
{
  this->ExposureMode = expMode;
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusSpinnakerVideoSource::SetExposureMicroSec(int exposureMicroSec)
{
  if (exposureMicroSec < 11 || exposureMicroSec > 30000000)
  {
    LOG_WARNING("Requested invalid exposure time of " << exposureMicroSec << " (us). Using continuous auto-exposure.");
    this->ExposureMode = EXPOSURE_AUTO_CONTINUOUS;
    return PLUS_FAIL;
  }
  this->ExposureMicroSec = exposureMicroSec;
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusSpinnakerVideoSource::SetGainMode(GAIN_MODE gainMode)
{
  this->GainMode = gainMode;
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusSpinnakerVideoSource::SetGainDB(int gainDb)
{
  if (gainDb < 0 || gainDb > 18)
  {
    LOG_WARNING("Requested invalid fain of " << gainDb << " (dB). Using continuous auto-gain.");
    this->GainMode = GAIN_AUTO_CONTINUOUS;
    return PLUS_FAIL;
  }
  this->GainDB = GainDB;
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusSpinnakerVideoSource::SetWhiteBalanceMode(WHITE_BALANCE_MODE wbMode)
{
  this->WhiteBalanceMode = wbMode;
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusSpinnakerVideoSource::SetSharpeningMode(SHARPENING_MODE sharpMode)
{
  this->SharpeningMode = sharpMode;
  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusSpinnakerVideoSource::ReadConfiguration(vtkXMLDataElement* rootConfigElement)
{
  LOG_TRACE("vtkPlusSpinnakerVideoSource::ReadConfiguration(vtkXMLDataElement* rootConfigElement)");

  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_READING(deviceConfig, rootConfigElement);

  // camera params
  XML_READ_SCALAR_ATTRIBUTE_REQUIRED(int, CameraNumber, deviceConfig);
  XML_READ_STRING_ATTRIBUTE_OPTIONAL(VideoFormat, deviceConfig);

  // frame size
  int requestedFrameSize[2] = { static_cast<int>(DEFAULT_FRAME_SIZE[0]), static_cast<int>(DEFAULT_FRAME_SIZE[1]) };
  if (deviceConfig->GetVectorAttribute("FrameSize", 2, requestedFrameSize))
  {
    if (requestedFrameSize[0] < 0 || requestedFrameSize[1] < 0)
    {
      LOG_ERROR("Negative frame size defined in config file. Please define a positive frame size.");
      return PLUS_FAIL;
    }
    this->FrameSize[0] = static_cast<unsigned int>(requestedFrameSize[0]);
    this->FrameSize[1] = static_cast<unsigned int>(requestedFrameSize[1]);
    this->FrameSize[2] = 1;
  }

  // frame rate
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(int, FrameRate, deviceConfig);

  // exposure
  XML_READ_ENUM3_ATTRIBUTE_OPTIONAL(ExposureMode, deviceConfig,
    "TIMED", EXPOSURE_TIMED,
    "AUTO_ONCE", EXPOSURE_AUTO_ONCE,
    "AUTO_CONTINUOUS", EXPOSURE_AUTO_CONTINUOUS);
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(float, ExposureMicroSec, deviceConfig);

  // gain
  XML_READ_ENUM3_ATTRIBUTE_OPTIONAL(GainMode, deviceConfig,
    "MANUAL", GAIN_MANUAL,
    "AUTO_ONCE", GAIN_AUTO_ONCE,
    "AUTO_CONTINUOUS", GAIN_AUTO_CONTINUOUS);
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(float, GainDB, deviceConfig);

  // white balance
  XML_READ_ENUM3_ATTRIBUTE_OPTIONAL(WhiteBalanceMode, deviceConfig,
    "MANUAL", WB_MANUAL,
    "AUTO_ONCE", WB_AUTO_ONCE,
    "AUTO_CONTINUOUS", WB_AUTO_CONTINUOUS);
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(float, WhiteBalance, deviceConfig);

  // sharpening
  XML_READ_ENUM2_ATTRIBUTE_OPTIONAL(SharpeningMode, deviceConfig,
    "MANUAL", SHARPENING_MANUAL,
    "AUTO", SHARPENING_AUTO);
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(float, Sharpening, deviceConfig);

  XML_FIND_NESTED_ELEMENT_REQUIRED(dataSourcesElement, deviceConfig, "DataSources");
  for (int nestedElementIndex = 0; nestedElementIndex < dataSourcesElement->GetNumberOfNestedElements(); nestedElementIndex++)
  {
    vtkXMLDataElement* dataElement = dataSourcesElement->GetNestedElement(nestedElementIndex);
    if (STRCASECMP(dataElement->GetName(), "DataSource") != 0)
    {
      // if this is not a data source element, skip it
      continue;
    }

    if (dataElement->GetAttribute("Type") != NULL && STRCASECMP(dataElement->GetAttribute("Type"), "Video") == 0)
    {
      // this is a video element
      // get video ID
      const char* toolId = dataElement->GetAttribute("Id");
      if (toolId == NULL)
      {
        // tool doesn't have ID needed to generate transform
        LOG_ERROR("Failed to initialize Spinnaker API VideoSource: Id is missing");
        continue;
      }
    }
  }

  // print configuration
  std::ostringstream str;
  this->PrintConfiguration(str, vtkIndent(0));
  LOG_INFO(std::endl << "SpinnakerVideoSource configuration:" << std::endl << str.str());
  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusSpinnakerVideoSource::WriteConfiguration(vtkXMLDataElement* rootConfigElement)
{
  LOG_TRACE("vtkPlusSpinnakerVideoSource::WriteConfiguration(vtkXMLDataElement* rootConfigElement)");

  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_WRITING(deviceConfig, rootConfigElement);

  return PLUS_FAIL;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusSpinnakerVideoSource::InternalConnect()
{
  LOG_TRACE("vtkPlusSpinnakerVideoSource::InternalConnect()");
  
  // initiailize system pointer
  this->Internal->SystemPtr = Spinnaker::System::GetInstance();

  // Retrieve list of cameras from the system
  Spinnaker::CameraList camList = this->Internal->SystemPtr->GetCameras();
  
  if (camList.GetSize() == 0)
  {
    camList.Clear();
    LOG_ERROR("No Spinnaker compatible camera detected.");
    return PLUS_FAIL;
  }

  return PLUS_FAIL;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusSpinnakerVideoSource::InternalDisconnect()
{
  LOG_TRACE("vtkPlusSpinnakerVideoSource::InternalDisconnect()");

  this->Internal->SystemPtr->ReleaseInstance();
  return PLUS_FAIL;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusSpinnakerVideoSource::InternalStartRecording()
{
  LOG_TRACE("vtkPlusSpinnakerVideoSource::InternalStartRecording()");
  return PLUS_FAIL;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusSpinnakerVideoSource::InternalStopRecording()
{
  LOG_TRACE("vtkPlusSpinnakerVideoSource::InternalStopRecording()");
  return PLUS_FAIL;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusSpinnakerVideoSource::NotifyConfigured()
{
  // TODO: Implement some configuration checks here
  LOG_TRACE("vtkPlusSpinnakerVideoSource::NotifyConfigured()");
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusSpinnakerVideoSource::InternalUpdate()
{
  LOG_TRACE("vtkPlusSpinnakerVideoSource::InternalUpdate()");

  this->FrameNumber++; 
  return PLUS_FAIL;
}