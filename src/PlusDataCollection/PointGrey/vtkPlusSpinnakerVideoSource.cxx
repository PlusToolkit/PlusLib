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
#include <Spinnaker.h>
#include <SpinGenApi/SpinnakerGenApi.h>

// STL includes
#include <map>
#include <sstream>

//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkPlusSpinnakerVideoSource);

namespace
{
  typedef vtkPlusSpinnakerVideoSource psvs;
  const unsigned int              DEFAULT_CAMERA_NUM(0);
  const psvs::PIXEL_ENCODING      DEFAULT_PIXEL_ENCODING(psvs::RGB24);
  const FrameSizeType             DEFAULT_FRAME_SIZE = { 640, 480, 1 };
  const unsigned int              DEFAULT_FRAME_RATE(30);
  const psvs::EXPOSURE_MODE       DEFAULT_EXPOSURE_MODE(psvs::EXPOSURE_AUTO_CONTINUOUS);
  const float                     FLAG_EXPOSURE_MICROSEC(-1);
  const psvs::GAIN_MODE           DEFAULT_GAIN_MODE(psvs::GAIN_AUTO_CONTINUOUS);
  const float                     FLAG_GAIN_DB(-1);
  const psvs::WHITE_BALANCE_MODE  DEFAULT_WHITE_BALANCE_MODE(psvs::WB_AUTO_CONTINUOUS);
  const float                     FLAG_WHITE_BALANCE(-1);

  // xml attribute values
  std::string EXPOSURE_TIMED_STRING = "TIMED";
  std::string EXPOSURE_AUTO_ONCE_STRING = "AUTO_ONCE";
  std::string EXPOSURE_AUTO_CONTINUOUS_STRING = "AUTO_CONTINUOUS";
  std::string GAIN_MANUAL_STRING = "MANUAL";
  std::string GAIN_AUTO_ONCE_STRING = "AUTO_ONCE";
  std::string GAIN_AUTO_CONTINUOUS_STRING = "AUTO_CONTINUOUS";
  std::string WB_MANUAL_STRING = "MANUAL";
  std::string WB_AUTO_ONCE_STRING = "AUTO_ONCE";
  std::string WB_AUTO_CONTINUOUS_STRING = "AUTO_CONTINUOUS";
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

  // list of connected cameras
  Spinnaker::CameraList CameraList;

  // camera pointer
  Spinnaker::CameraPtr CameraPtr;
};

//----------------------------------------------------------------------------
vtkPlusSpinnakerVideoSource::vtkPlusSpinnakerVideoSource()
  : Internal(new vtkInternal(this)),
  FrameNumber(0),
  CameraNumber(DEFAULT_CAMERA_NUM),
  PixelEncoding(DEFAULT_PIXEL_ENCODING),
  FrameRate(DEFAULT_FRAME_RATE),
  ExposureMode(DEFAULT_EXPOSURE_MODE),
  ExposureMicroSec(FLAG_EXPOSURE_MICROSEC),
  GainMode(DEFAULT_GAIN_MODE),
  GainDB(FLAG_GAIN_DB),
  WhiteBalanceMode(DEFAULT_WHITE_BALANCE_MODE),
  WhiteBalanceRed(FLAG_WHITE_BALANCE),
  WhiteBalanceBlue(FLAG_WHITE_BALANCE)
{
  LOG_TRACE("vtkPlusSpinnakerVideoSource::vtkPlusSpinnakerVideoSource()");
  this->RequireImageOrientationInConfiguration = true;
  this->StartThreadForInternalUpdates = true;
  this->InternalUpdateRate = 3 * DEFAULT_FRAME_RATE;
  this->AcquisitionRate = DEFAULT_FRAME_RATE;
  this->FrameSize[0] = DEFAULT_FRAME_SIZE[0];
  this->FrameSize[1] = DEFAULT_FRAME_SIZE[1];
  this->FrameSize[2] = 1;
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
  ExposureModeToString[EXPOSURE_TIMED] = EXPOSURE_TIMED_STRING;
  ExposureModeToString[EXPOSURE_AUTO_ONCE] = EXPOSURE_AUTO_ONCE_STRING;
  ExposureModeToString[EXPOSURE_AUTO_CONTINUOUS] = EXPOSURE_AUTO_CONTINUOUS_STRING;

  std::map<GAIN_MODE, std::string> GainModeToString;
  GainModeToString[GAIN_MANUAL] = GAIN_MANUAL_STRING;
  GainModeToString[GAIN_AUTO_ONCE] = GAIN_AUTO_ONCE_STRING;
  GainModeToString[GAIN_AUTO_CONTINUOUS] = GAIN_AUTO_CONTINUOUS_STRING;

  std::map<WHITE_BALANCE_MODE, std::string> WhiteBalanceModeToString;
  WhiteBalanceModeToString[WB_MANUAL] = WB_MANUAL_STRING;
  WhiteBalanceModeToString[WB_AUTO_ONCE] = WB_AUTO_ONCE_STRING;
  WhiteBalanceModeToString[WB_AUTO_CONTINUOUS] = WB_AUTO_CONTINUOUS_STRING;

  // print device parameters
  os << indent << "CameraNumber: " << this->CameraNumber << std::endl;
  os << indent << "VideoFormat:" << this->PixelEncoding << std::endl;
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
    os << indent << "WhiteBalance(red):" << this->WhiteBalanceRed << std::endl;
    os << indent << "WhiteBalance(blue):" << this->WhiteBalanceBlue << std::endl;
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
    LOG_WARNING("Requested invalid gain of " << gainDb << " (dB). Using continuous auto-gain.");
    this->GainMode = GAIN_AUTO_CONTINUOUS;
    return PLUS_FAIL;
  }
  this->GainDB = gainDb;
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusSpinnakerVideoSource::SetWhiteBalanceMode(WHITE_BALANCE_MODE wbMode)
{
  this->WhiteBalanceMode = wbMode;
  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
// verify that if any manual settings are enabled then their corresponding values have also been set
PlusStatus vtkPlusSpinnakerVideoSource::CheckCameraParameterValidity()
{
  PlusStatus retVal = PLUS_SUCCESS;

  // exposure
  if (this->ExposureMode == EXPOSURE_TIMED && this->ExposureMicroSec == FLAG_EXPOSURE_MICROSEC)
  {
    LOG_ERROR("Failed to configure SpinnakerVideoSource: ExposureMode is set to \"TIMED\", but no ExposureMicroSec attribute is provided.");
    retVal = PLUS_FAIL;
  }

  // gain
  if (this->GainMode == GAIN_MANUAL && this->GainDB == FLAG_GAIN_DB)
  {
    LOG_ERROR("Failed to configure SpinnakerVideoSource: GainMode is set to \"MANUAL\", but no GainDB attribute is provided.");
    retVal = PLUS_FAIL;
  }

  // white balance
  if (this->WhiteBalanceMode == WB_MANUAL && this->WhiteBalanceRed == FLAG_WHITE_BALANCE)
  {
    LOG_ERROR("Failed to configure SpinnakerVideoSource: WhiteBalanceMode is set to \"MANUAL\", but no WhiteBalanceRed attribute is provided.");
    retVal = PLUS_FAIL;
  }
  if (this->WhiteBalanceMode == WB_MANUAL && this->WhiteBalanceBlue == FLAG_WHITE_BALANCE)
  {
    LOG_ERROR("Failed to configure SpinnakerVideoSource: WhiteBalanceMode is set to \"MANUAL\", but no WhiteBalanceBlue attribute is provided.");
    retVal = PLUS_FAIL;
  }

  return retVal;
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusSpinnakerVideoSource::ReadConfiguration(vtkXMLDataElement* rootConfigElement)
{
  LOG_TRACE("vtkPlusSpinnakerVideoSource::ReadConfiguration(vtkXMLDataElement* rootConfigElement)");

  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_READING(deviceConfig, rootConfigElement);

  // camera params
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(int, CameraNumber, deviceConfig);

  XML_READ_ENUM2_ATTRIBUTE_OPTIONAL(PixelEncoding, deviceConfig,
    "RGB24", RGB24,
    "MONO8", MONO8);

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
    EXPOSURE_TIMED_STRING, EXPOSURE_TIMED,
    EXPOSURE_AUTO_ONCE_STRING, EXPOSURE_AUTO_ONCE,
    EXPOSURE_AUTO_CONTINUOUS_STRING, EXPOSURE_AUTO_CONTINUOUS);
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(float, ExposureMicroSec, deviceConfig);

  // gain
  XML_READ_ENUM3_ATTRIBUTE_OPTIONAL(GainMode, deviceConfig,
    GAIN_MANUAL_STRING, GAIN_MANUAL,
    GAIN_AUTO_ONCE_STRING, GAIN_AUTO_ONCE,
    GAIN_AUTO_CONTINUOUS_STRING, GAIN_AUTO_CONTINUOUS);
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(float, GainDB, deviceConfig);

  // white balance
  XML_READ_ENUM3_ATTRIBUTE_OPTIONAL(WhiteBalanceMode, deviceConfig,
    WB_MANUAL_STRING, WB_MANUAL,
    WB_AUTO_ONCE_STRING, WB_AUTO_ONCE,
    WB_AUTO_CONTINUOUS_STRING, WB_AUTO_CONTINUOUS);
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(float, WhiteBalanceRed, deviceConfig);
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(float, WhiteBalanceBlue, deviceConfig);

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
  
  return this->CheckCameraParameterValidity();
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
  
  // initialize system pointer
  this->Internal->SystemPtr = Spinnaker::System::GetInstance();

  // Retrieve list of cameras from the system
  this->Internal->CameraList = this->Internal->SystemPtr->GetCameras();

  // check CameraNumber parameter is valid
  const int numConnectedCams = this->Internal->CameraList.GetSize();
  if (numConnectedCams == 0)
  {
    this->Internal->CameraList.Clear();
    LOG_ERROR("No Spinnaker compatible camera detected.");
    return PLUS_FAIL;
  }
  else if (this->CameraNumber < 0 || this->CameraNumber > numConnectedCams)
  {
    LOG_ERROR("Invalid camera number (" << this->CameraNumber <<
      "in config. Please verify your camera is connected and that your config contains the correct CameraNumber value.");
    return PLUS_FAIL;
  }

  // get camera pointer
  this->Internal->CameraPtr = this->Internal->CameraList.GetByIndex(this->CameraNumber);

  // initialize camera
  try
  {
    this->Internal->CameraPtr->Init();
  }
  catch (Spinnaker::Exception e)
  {
    LOG_ERROR("SpinnakerVideoSource: Error initializing camera. Exception text : " << e.what());
    return PLUS_FAIL;
  }

  // configure the camera with parameters specified in the config
  try
  {
    // get camera nodemap pointer
    Spinnaker::GenApi::INodeMap& nodeMap = this->Internal->CameraPtr->GetNodeMap();

    // map PIXEL_ENCODING to SpinnakerAPI pixel format
    std::map<PIXEL_ENCODING, std::string> PixelEncodingToString;
    PixelEncodingToString[RGB24] = "BayerRG8";
    PixelEncodingToString[MONO8] = "Mono8";

    // set video encoding
    Spinnaker::GenApi::CEnumerationPtr pfPtr = nodeMap.GetNode("PixelFormat");
    if (Spinnaker::GenApi::IsAvailable(pfPtr) && Spinnaker::GenApi::IsWritable(pfPtr))
    {
      // Retrieve the desired entry node from the enumeration node
      std::string spinnakerPixelFormat = PixelEncodingToString.find(this->PixelEncoding)->second;
      Spinnaker::GenApi::CEnumEntryPtr pfEnumPtr = pfPtr->GetEntryByName(spinnakerPixelFormat.c_str());
      if (Spinnaker::GenApi::IsAvailable(pfEnumPtr) && Spinnaker::GenApi::IsReadable(pfEnumPtr))
      {
        // Retrieve the integer value from the entry node
        int64_t pixelFormat = pfEnumPtr->GetValue();
        // Set integer as new value for enumeration node
        pfPtr->SetIntValue(pixelFormat);
      }
      else
      {
        LOG_ERROR("Requested VideoFormat of \"" << this->PixelEncoding << "\" is not available...");
        return PLUS_FAIL;
      }
    }
    else
    {
      LOG_ERROR("Requested VideoFormat of \"" << this->PixelEncoding << "\" is not available...");
      return PLUS_FAIL;
    }

    // set frame width 
    Spinnaker::GenApi::CIntegerPtr WidthPtr = nodeMap.GetNode("Width");
    if (Spinnaker::GenApi::IsAvailable(WidthPtr) && Spinnaker::GenApi::IsWritable(WidthPtr))
    {
      WidthPtr->SetValue(this->FrameSize[0]);
      if (this->FrameSize[0] != WidthPtr->GetValue())
      {
        LOG_WARNING("Failed to set image width to requested value of " << this->FrameSize[0] <<
          "(px). Setting width to default value of " << DEFAULT_FRAME_SIZE[0] << "(px).");
        WidthPtr->SetValue(DEFAULT_FRAME_SIZE[0]);
        this->FrameSize[0] = DEFAULT_FRAME_SIZE[0];
      }
    }
    else
    {
      LOG_ERROR("Unable to set image width.");
      return PLUS_FAIL;
    }

    // set frame height
    Spinnaker::GenApi::CIntegerPtr HeightPtr = nodeMap.GetNode("Height");
    if (Spinnaker::GenApi::IsAvailable(HeightPtr) && Spinnaker::GenApi::IsWritable(HeightPtr))
    {
      HeightPtr->SetValue(this->FrameSize[1]);
      if (this->FrameSize[1] != HeightPtr->GetValue())
      {
        LOG_WARNING("Failed to set image height to requested value of " << this->FrameSize[1] <<
          "(px). Setting width to default value of " << DEFAULT_FRAME_SIZE[1] << "(px).");
        WidthPtr->SetValue(DEFAULT_FRAME_SIZE[1]);
        this->FrameSize[1] = DEFAULT_FRAME_SIZE[1];
      }
    }
    else
    {
      LOG_ERROR("Unable to set image height.");
      return PLUS_FAIL;
    }

    // set acquisition mode to continuous
    this->Internal->CameraPtr->AcquisitionMode.SetValue(Spinnaker::AcquisitionMode_Continuous);

    // enable manual frame rate control
    Spinnaker::GenApi::CBooleanPtr FrameRateEnablePtr = nodeMap.GetNode("AcquisitionFrameRateEnable");
    if (Spinnaker::GenApi::IsAvailable(FrameRateEnablePtr) && Spinnaker::GenApi::IsWritable(FrameRateEnablePtr))
    {
      FrameRateEnablePtr->SetValue(true);
    }
    else
    {
      LOG_ERROR("Unable to enable manual frame rate control.");
      return PLUS_FAIL;
    }

    // set frame rate
    Spinnaker::GenApi::CFloatPtr FrameRatePtr = nodeMap.GetNode("AcquisitionFrameRate");
    if (Spinnaker::GenApi::IsAvailable(FrameRatePtr) && Spinnaker::GenApi::IsWritable(FrameRatePtr))
    {
      FrameRatePtr->SetValue(this->FrameRate);
      if (this->FrameRate != FrameRatePtr->GetValue())
      {
        LOG_WARNING("Failed to set frame rate to requested value of " << this->FrameRate <<
          "(frames / sec). FrameRate set to default value of " << FrameRatePtr->GetValue() << " (frames / sec).");
      }
    }
    else
    {
      LOG_ERROR("Unable to set frame rate.");
      return PLUS_FAIL;
    }

    // set exposure mode && exposure time (if manual exposure control enabled)
    if (this->ExposureMode == EXPOSURE_TIMED)
    {
      this->Internal->CameraPtr->ExposureAuto.SetValue(Spinnaker::ExposureAuto_Off);
      this->Internal->CameraPtr->ExposureMode.SetValue(Spinnaker::ExposureMode_Timed);
      this->Internal->CameraPtr->ExposureTime.SetValue(this->ExposureMicroSec);
    }
    else if (this->ExposureMode == EXPOSURE_AUTO_ONCE)
    {
      this->Internal->CameraPtr->ExposureAuto.SetValue(Spinnaker::ExposureAuto_Once);
    }
    else if (this->ExposureMode == EXPOSURE_AUTO_CONTINUOUS)
    {
      this->Internal->CameraPtr->ExposureAuto.SetValue(Spinnaker::ExposureAuto_Continuous);
    }

    // set gain mode && gain (if manual gain control enabled)
    if (this->GainMode == GAIN_MANUAL)
    {
      this->Internal->CameraPtr->GainAuto.SetValue(Spinnaker::GainAuto_Off);
      this->Internal->CameraPtr->Gain.SetValue(this->GainDB);
    }
    else if (this->GainMode == GAIN_AUTO_ONCE)
    {
      this->Internal->CameraPtr->GainAuto.SetValue(Spinnaker::GainAuto_Once);
    }
    else if (this->GainMode == GAIN_AUTO_CONTINUOUS)
    {
      this->Internal->CameraPtr->GainAuto.SetValue(Spinnaker::GainAuto_Continuous);
    }

    // set white balance mode && wb (if manual wb control enabled)
    if (this->WhiteBalanceMode == WB_MANUAL)
    {
      this->Internal->CameraPtr->BalanceWhiteAuto.SetValue(Spinnaker::BalanceWhiteAutoEnums::BalanceWhiteAuto_Off);
      this->Internal->CameraPtr->BalanceRatioSelector.SetValue(Spinnaker::BalanceRatioSelectorEnums::BalanceRatioSelector_Blue);
      Spinnaker::GenApi::CFloatPtr balanceRatioPtr = nodeMap.GetNode("BalanceRatio");
      balanceRatioPtr->SetValue(this->WhiteBalanceBlue);
      this->Internal->CameraPtr->BalanceRatioSelector.SetValue(Spinnaker::BalanceRatioSelectorEnums::BalanceRatioSelector_Red);
      balanceRatioPtr->SetValue(this->WhiteBalanceRed);
    }
    else if (this->WhiteBalanceMode == WB_AUTO_ONCE)
    {
      this->Internal->CameraPtr->BalanceWhiteAuto.SetValue(Spinnaker::BalanceWhiteAutoEnums::BalanceWhiteAuto_Once);
    }
    else if (this->WhiteBalanceMode == WB_AUTO_CONTINUOUS)
    {
      this->Internal->CameraPtr->BalanceWhiteAuto.SetValue(Spinnaker::BalanceWhiteAutoEnums::BalanceWhiteAuto_Continuous);
    }
  }
  catch (Spinnaker::Exception &e)
  {
    LOG_ERROR("SpinnakerVideoSource: Error configuring camera. Exception text: " << e.what());
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusSpinnakerVideoSource::InternalDisconnect()
{
  LOG_TRACE("vtkPlusSpinnakerVideoSource::InternalDisconnect()");

  try
  {
    this->Internal->CameraPtr->DeInit();
    this->Internal->CameraPtr = NULL;
  }
  catch (Spinnaker::Exception &e)
  {
    LOG_ERROR("SpinnakerVideoSource: Failed to de-initialize camera. Exception text: " << e.what());
    return PLUS_FAIL;
  }

  try
  {
    this->Internal->CameraList.Clear();
  }
  catch (Spinnaker::Exception & e)
  {
    LOG_ERROR("SpinnakerVideoSource: Failed to clear camera list. Exception text: " << e.what());
    return PLUS_FAIL;
  }

  try
  {
    this->Internal->SystemPtr->ReleaseInstance();
  }
  catch (Spinnaker::Exception & e)
  {
    LOG_ERROR("SpinnakerVideoSource: Failed to release system pointer. Exception text: " << e.what());
    return PLUS_FAIL;
  }
   
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusSpinnakerVideoSource::InternalStartRecording()
{
  LOG_TRACE("vtkPlusSpinnakerVideoSource::InternalStartRecording()");

  // get camera nodemap pointer
  try
  {
    Spinnaker::GenApi::INodeMap& nodeMapTLDevice = this->Internal->CameraPtr->GetTLDeviceNodeMap();
    Spinnaker::GenApi::INodeMap& nodeMap = this->Internal->CameraPtr->GetNodeMap();

    // set acquisition mode to continuous

    Spinnaker::GenApi::CEnumerationPtr ptrAcquisitionMode = nodeMap.GetNode("AcquisitionMode");
    if (!IsAvailable(ptrAcquisitionMode) || !IsWritable(ptrAcquisitionMode))
    {
      LOG_ERROR("Unable to set acquisition mode to continuous (node retrieval). Aborting...");
      return PLUS_FAIL;
    }
    Spinnaker::GenApi::CEnumEntryPtr ptrAcquisitionModeContinuous = ptrAcquisitionMode->GetEntryByName("Continuous");
    if (!IsAvailable(ptrAcquisitionModeContinuous) || !IsReadable(ptrAcquisitionModeContinuous))
    {
      LOG_ERROR("Unable to set acquisition mode to continuous (entry 'continuous' retrieval). Aborting...");
      return PLUS_FAIL;
    }
    int64_t acquisitionModeContinuous = ptrAcquisitionModeContinuous->GetValue();
    ptrAcquisitionMode->SetIntValue(acquisitionModeContinuous);
    LOG_INFO("Acquisition mode set to continuous...");

    // begin acquiring images
    this->Internal->CameraPtr->BeginAcquisition();

    std::string deviceSerialNumber("");
    Spinnaker::GenApi::CStringPtr ptrStringSerial = nodeMapTLDevice.GetNode("DeviceSerialNumber");
    if (IsAvailable(ptrStringSerial) && IsReadable(ptrStringSerial))
    {
      deviceSerialNumber = ptrStringSerial->GetValue();
      LOG_INFO("Device serial number retrieved as " << deviceSerialNumber << ".");
    }
  }
  catch (Spinnaker::Exception &e)
  {
    LOG_ERROR("SpinnakerVideoSource: Failed to start image acquisition. Exception text: " << e.what());
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusSpinnakerVideoSource::InternalStopRecording()
{
  LOG_TRACE("vtkPlusSpinnakerVideoSource::InternalStopRecording()");

  try
  {
    // End acquiring images
    this->Internal->CameraPtr->EndAcquisition();
  }
  catch (Spinnaker::Exception &e)
  {
    LOG_ERROR("SpinnakerVideoSource: Failed to end recording. Exception text: " << e.what());
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusSpinnakerVideoSource::NotifyConfigured()
{
  // TODO: Implement some configuration checks here
  LOG_TRACE("vtkPlusSpinnakerVideoSource::NotifyConfigured()");

  if (this->GetNumberOfVideoSources() < 1)
  {
    LOG_ERROR("Device must have at least one video source.");
    return PLUS_FAIL;
  }
  if (this->OutputChannelCount() < 1)
  {
    LOG_ERROR("Device must have at least one output channel.");
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusSpinnakerVideoSource::InternalUpdate()
{
  LOG_TRACE("vtkPlusSpinnakerVideoSource::InternalUpdate()");

  PlusStatus retVal = PLUS_FAIL;

  try
  {
    // Retrieve next received image and ensure image completion
    Spinnaker::ImagePtr pResultImage = this->Internal->CameraPtr->GetNextImage();
    if (pResultImage->IsIncomplete())
    {
      cout << "Image incomplete with image status " << pResultImage->GetImageStatus() << "..." << endl << endl;
    }

    // Convert image from PointGrey to correct image type
    Spinnaker::ImagePtr convertedImage;
    if (this->PixelEncoding == RGB24)
    {
      convertedImage = pResultImage->Convert(Spinnaker::PixelFormat_RGB8, Spinnaker::HQ_LINEAR);
    }
    else if (this->PixelEncoding == MONO8)
    {
      convertedImage = pResultImage->Convert(Spinnaker::PixelFormat_Mono8, Spinnaker::HQ_LINEAR);
    }
    else
    {
      LOG_ERROR("Invalid image type selected.");
      return PLUS_FAIL;
    }

    // add image to PLUS buffer

    // get buffer
    vtkPlusDataSource* videoSource(NULL);
    if (this->GetFirstVideoSource(videoSource) != PLUS_SUCCESS)
    {
      LOG_WARNING("Failed to get video source in SpinnakerVideoSource");
      return PLUS_FAIL;
    }

    // initialize if buffer is empty
    if (videoSource->GetNumberOfItems() == 0)
    {
      if (this->PixelEncoding == RGB24)
      {
        videoSource->SetImageType(US_IMG_RGB_COLOR);
        videoSource->SetPixelType(VTK_UNSIGNED_CHAR);
        videoSource->SetNumberOfScalarComponents(3);
        videoSource->SetInputFrameSize(this->FrameSize);
      }
      else if (this->PixelEncoding == MONO8)
      {
        videoSource->SetImageType(US_IMG_BRIGHTNESS);
        videoSource->SetPixelType(VTK_UNSIGNED_CHAR);
        videoSource->SetNumberOfScalarComponents(1);
        videoSource->SetInputFrameSize(this->FrameSize);
      }
    }
    
    // add frame to PLUS buffer
    if (this->PixelEncoding == RGB24)
    {
      retVal = videoSource->AddItem(
        convertedImage->GetData(),
        this->FrameSize,
        convertedImage->GetImageSize(),
        US_IMG_RGB_COLOR,
        this->FrameNumber);
    }
    else if (this->PixelEncoding == MONO8)
    {
      retVal = videoSource->AddItem(
        convertedImage->GetData(),
        this->FrameSize,
        convertedImage->GetImageSize(),
        US_IMG_BRIGHTNESS,
        this->FrameNumber);
    }

    // Release image
    pResultImage->Release();
  }
  catch (Spinnaker::Exception &e)
  {
    LOG_ERROR("SpinnakerVideoSource: Failed in InternalUpdate(). Exception text: " << e.what());
    return PLUS_FAIL;
  }
  LOG_INFO(this->FrameNumber);
  this->FrameNumber++; 
  return retVal;
}