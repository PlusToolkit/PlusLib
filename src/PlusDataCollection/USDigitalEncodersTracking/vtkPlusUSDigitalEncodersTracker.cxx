/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"

#include "vtkMatrix4x4.h"
#include "vtkObjectFactory.h"
#include "vtkPlusDataSource.h"
#include "vtkTransform.h"
#include "vtkXMLDataElement.h"
#include "vtksys/SystemTools.hxx"
#include "vtkMath.h"
#include "vtkVector.h"

#include "SEIDrv32.h"
#include <sstream>

#include "vtkPlusUSDigitalEncodersTracker.h"

vtkStandardNewMacro(vtkPlusUSDigitalEncodersTracker);

class vtkPlusUSDigitalEncodersTracker::vtkPlusUSDigitalEncoderInfo
{
public:
  long Model = 0;
  long Version = 0;
  long Addr = 0;
  long Addr2 = 0;

  // Supporting Modes:
  /*
    The mode is changed temporarily and will be effective until the encoder is
    reset, powered down, or another mode change command is received. It is
    not stored in the EEPROM. Mode byte as follows:

  |7|  6 |5|  4 |  3 |  2  | 1 | 0 |
  |0|/256|0|incr|size|multi|stb|rev|

    Reverse: rev = 1, the position increases counter clockwise.
          rev = 0, the position increases clockwise.
    Strobe:  stb = 1, the encoder operates in strobe mode: it waits for a strobe
                  request before reading the position; this mode is used to
                  synchronize multiple encoders. After entering this mode, wait
                  at least 2 msec before sending the first strobe command.
          stb = 0, the encoder operates in asynchronous
                  mode: it reads the position within 2 milliseconds and sends
                  the most current position when requested. The data can be
                  from 0 to 2 milliseconds old.
    Multi:   multi = 1, multi-turn mode: a 32 bit counter keeps track of the
                    position (it increases or decreases over multiple turns, i.e. 3 1/
                    2 turns at a resolution of 100 would be 350). This counter is
                    cleared at reset.
          multi = 0, single-turn mode: position is between zero and the max
                    resolution, according to the shaft angle.
                    Note: in older versions (V1.X), this bit indicated a fast mode
                    (3msec update rate) with a 9 bit accuracy.
                    Also, any other command besides position inquires can corrupt
                    the multi-turn position.
    Size: only effective in single-turn mode:
          size = 1: the encoder always sends the position in 2 bytes, even
                    if the resolution is 256 decimal or less.
          size = 0: the position is sent as 1 byte if the resolution is up to
                    256 decimal, or as 2 bytes if above 256 decimal.
                    In multi-turn mode, the position is always 4 bytes and this bit is
                    ignored.
    Incr: only effective in multi-turn mode:
          incr = 1: the encoder sends the position change since the last
                    request, as a 32 bit signed number.
          incr = 0: the encoder sends the position as a 32 bit signed
                    number.
    /256: only available for analog version,
       only effective in multi-turn mode:
          /256 = 1: the encoder position is divided by 256.
          /256 = 0: the encoder position is normal.
  */
  long Mode = 4;// Defaul mode : Strobe (OFF), MultiTurn(On), Size(OFF), Incr (OFF), /256 (OFF)
  long Resolution = 3600;
  bool Connected = false;
  int Motion = 0; // 0 : Linear motion , 1: Rotation
  double PulseSpacing = 0.0;
  double PulseSpacing2 = 0.0;
  vtkSmartPointer<vtkTransform> LocalTransform = vtkSmartPointer<vtkTransform>::New();
  vtkVector3d LocalAxis;
  vtkVector3d LocalAxis2;
public:
  vtkSmartPointer<vtkMatrix4x4> ToolToEncoderMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
  vtkSmartPointer<vtkMatrix4x4> EncoderToReference = vtkSmartPointer<vtkMatrix4x4>::New();
  igsioTransformName TransformName;
  std::string PortName;
};

//-------------------------------------------------------------------------
vtkPlusUSDigitalEncodersTracker::vtkPlusUSDigitalEncodersTracker()
{
  this->SetDeviceId("TrackerDevice");

  // No callback function provided by the device, so the data capture thread
  // will be used to poll the hardware and add new items to the buffer
  this->StartThreadForInternalUpdates = true;
  this->AcquisitionRate = 50; //increase this default (vtkPlusDevice has 30)
}

//-------------------------------------------------------------------------
vtkPlusUSDigitalEncodersTracker::~vtkPlusUSDigitalEncodersTracker()
{
  if(this->Recording)
  {
    this->StopRecording();
  }
}

//-------------------------------------------------------------------------
void vtkPlusUSDigitalEncodersTracker::PrintSelf(ostream& os, vtkIndent indent)
{
  Superclass::PrintSelf(os, indent);
}

//-------------------------------------------------------------------------
PlusStatus vtkPlusUSDigitalEncodersTracker::InternalConnect()
{
  LOG_TRACE("vtkPlusUSDigitalEncodersTracker::Connect");

  // SEI Initialization.
  // Start the SEI Server Program, and look for devices on the SEI bus with
  // the zero COM port to look on all com ports, and the AUTOASSIGN means
  // that if there are address conflicts on the SEI bus, the device
  // addresses will automatically be reassigned so there are no conflicts
  long EncoderStatus = ::InitializeSEI(0, REINITIALIZE | AUTOASSIGN | NORESET);
  if(EncoderStatus != 0)
  {
    LOG_ERROR("Failed to initialize SEI!");
    return PLUS_FAIL;
  }
  this->IdAddress.clear();

  long numberofConnectedEncoders = ::GetNumberOfDevices();
  EncoderInfoMapType::iterator encoderInfoPos;
  for(long deviceID = 0; deviceID < numberofConnectedEncoders; ++deviceID)
  {
    // Support multiple US digital A2 encoders.
    long model = 0;
    long serialNumber = 0;
    long version = 0;
    long address = 0;
    long retVal;

    if(::GetDeviceInfo(deviceID, &model, &serialNumber, &version, &address) != 0)
    {
      LOG_ERROR("Failed to get SEI device info for device number: " << deviceID);
      return PLUS_FAIL;
    }

    encoderInfoPos = this->EncoderMap.find(deviceID);

    if(encoderInfoPos == this->EncoderMap.end())
    {
      LOG_WARNING("Unregistered encoder is detected");
    }
    else
    {
      encoderInfoPos->second->Connected = true;
      encoderInfoPos->second->Model = model;
      encoderInfoPos->second->Version = version;
      retVal = ::A2SetMode(address, encoderInfoPos->second->Mode);
      if(retVal != 0)
      {
        LOG_ERROR("Failed to set SEI device mode for device SN: " << serialNumber << ", deviceID: " << deviceID);
        return PLUS_FAIL;
      }

      //check whether we need to reset the encoder
      long pos;
      retVal = ::A2GetPosition(address, &pos);
      if(retVal != 0)
      {
        retVal = ::A2SetPosition(address, 0); // reset the value of the encoder
        if(retVal != 0)
        {
          LOG_ERROR("Failed to set initial position for SEI device SN: " << serialNumber << ", deviceID: " << deviceID);
          return PLUS_FAIL;
        }
        else
        {
          LOG_INFO("The encoder's position had to be reset to zero. SEI device SN: " << serialNumber << ", deviceID: " << deviceID);
        }
      }
      if(deviceID != address)  //update address in encoderInfo
      {
        if(encoderInfoPos->second->Addr == deviceID)
        {
          encoderInfoPos->second->Addr = address;
        }
        else if(encoderInfoPos->second->Addr2 == deviceID)
        {
          encoderInfoPos->second->Addr2 = address;
        }
        else
        {
          LOG_WARNING("Could not establish configuration address to deviceID correspondence");
        }
      }
      this->IdAddress[deviceID] = address;
    }
  }

  return PLUS_SUCCESS;
}

//-------------------------------------------------------------------------
PlusStatus vtkPlusUSDigitalEncodersTracker::InternalDisconnect()
{
  LOG_TRACE("vtkPlusUSDigitalEncodersTracker::Disconnect");
  this->StopRecording();

  if(::IsInitialized() != 1)
  {
    // Device not yet initialized
    return PLUS_SUCCESS;
  }

  if(::CloseSEI() != 0)
  {
    LOG_ERROR("Failed to close SEI!");
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}

//-------------------------------------------------------------------------
PlusStatus vtkPlusUSDigitalEncodersTracker::Probe()
{
  LOG_TRACE("vtkPlusUSDigitalEncodersTracker::Probe");

  if(this->Recording)
  {
    return PLUS_SUCCESS;
  }

  if(!this->Connect())
  {
    LOG_ERROR("Unable to connect to USDigital Encoders");
    return PLUS_FAIL;
  }

  this->Disconnect();

  return PLUS_SUCCESS;
}

//-------------------------------------------------------------------------
PlusStatus vtkPlusUSDigitalEncodersTracker::InternalStartRecording()
{
  LOG_TRACE("vtkPlusUSDigitalEncodersTracker::InternalStartRecording");
  if(this->IsRecording())
  {
    return PLUS_SUCCESS;
  }

  return PLUS_SUCCESS;
}

//-------------------------------------------------------------------------
PlusStatus vtkPlusUSDigitalEncodersTracker::InternalStopRecording()
{
  LOG_TRACE("vtkPlusUSDigitalEncodersTracker::InternalStopRecording");
  return PLUS_SUCCESS;
}

//-------------------------------------------------------------------------
PlusStatus vtkPlusUSDigitalEncodersTracker::InternalUpdate()
{
  LOG_TRACE("vtkPlusUSDigitalEncodersTracker::InternalUpdate");

  if(!this->Recording)
  {
    LOG_ERROR("called Update() when not tracking");
    return PLUS_FAIL;
  }

  for(auto it = EncoderList.begin(); it != EncoderList.end(); ++it)
  {
    RETURN_WITH_FAIL_IF(!it->Connected, "USDigital encoder(s) not connected!");

    long encoderValue;
    long errorCode;
    vtkSmartPointer<vtkTransform> tempTransform = vtkSmartPointer<vtkTransform>::New();

    // Read encoder positions and transform it into XY position in mm
    errorCode = ::A2GetPosition(it->Addr, &encoderValue);
    RETURN_WITH_FAIL_IF(errorCode, "Unable to read position of first encoder with address: " << it->Addr);
    vtkVector3d localmovement = it->LocalAxis;

    if(it->Addr2 != 0)  //coreXY
    {
      double firstEnc = encoderValue * it->PulseSpacing;

      errorCode = ::A2GetPosition(it->Addr2, &encoderValue);
      RETURN_WITH_FAIL_IF(errorCode, "Unable to read position of second encoder with address: " << it->Addr2);

      double secondEnc = encoderValue * it->PulseSpacing2;

      double firstAxis = firstEnc + secondEnc;
      double secondAxis = firstEnc - secondEnc;

      //now make a transform matrix out of this translation and add it into PLUS system
      vtkMath::MultiplyScalar(localmovement.GetData(), firstAxis);
      tempTransform->Translate(localmovement.GetData());
      localmovement = it->LocalAxis2;
      vtkMath::MultiplyScalar(localmovement.GetData(), secondAxis);
      tempTransform->Translate(localmovement.GetData());
    }
    else //single encoder
    {
      // Update transformation matrix of the connected US digital encoder
      if(it->Motion == 0)
      {
        vtkMath::MultiplyScalar(localmovement.GetData(), encoderValue * it->PulseSpacing);
        tempTransform->Translate(localmovement.GetData());
      }
      else if(it->Motion == 1)
      {
        // Check the unit of rotation angle .... (degree or radian)
        tempTransform->RotateWXYZ(encoderValue * it->PulseSpacing, localmovement.GetData());
      }
      else
      {
        LOG_ERROR("Un-supported motion type");
      }
    }

    vtkMatrix4x4::Multiply4x4(it->ToolToEncoderMatrix, tempTransform->GetMatrix(), it->EncoderToReference);
    this->TransformRepository->SetTransform(it->TransformName, it->EncoderToReference);

    vtkPlusDataSource* tool = NULL;
    if(this->GetToolByPortName(it->PortName.c_str(), tool) != PLUS_SUCCESS)
    {
      LOG_ERROR("Unable to find tool with port name: " << it->PortName);
    }

    // Device has no frame numbering, so just auto increment tool frame number
    unsigned long frameNumber = tool->GetFrameNumber() + 1;
    const double unfilteredTimestamp = vtkIGSIOAccurateTimer::GetSystemTime();
    this->ToolTimeStampedUpdate(tool->GetId(),
                                it->EncoderToReference, TOOL_OK, frameNumber, unfilteredTimestamp);
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusUSDigitalEncodersTracker::ReadConfiguration(vtkXMLDataElement* rootConfigElement)
{
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_READING(deviceConfig, rootConfigElement);

  XML_FIND_NESTED_ELEMENT_REQUIRED(dataSourcesElement, deviceConfig, "DataSources");

  this->TransformRepository->Clear();
  this->EncoderMap.clear();
  this->EncoderList.clear();
  long deviceID = 0;

  for(int encoderIndex = 0; encoderIndex < dataSourcesElement->GetNumberOfNestedElements(); encoderIndex++)
  {
    vtkXMLDataElement* encoderInfoElement = dataSourcesElement->GetNestedElement(encoderIndex);
    if(STRCASECMP(encoderInfoElement->GetName(), "DataSource") != 0)
    {
      // if this is not a data source element, skip it
      continue;
    }

    vtkPlusUSDigitalEncoderInfo encoderInfo;

    const char* portName = encoderInfoElement->GetAttribute("PortName");
    if(portName == NULL)
    {
      LOG_ERROR("Tool portname is not specified");
      continue;
    }

    const char* id = encoderInfoElement->GetAttribute("Id");
    if(id == NULL)
    {
      LOG_ERROR("Tool Id is missing!");
      continue;
    }

    vtkPlusDataSource* tool = NULL;
    if(this->GetToolByPortName(portName, tool) != PLUS_SUCCESS)
    {
      LOG_ERROR("GetTool for port " << portName << " failed");
      continue;
    }
    if(tool == NULL)
    {
      LOG_ERROR("Tool with ID " << id << " was not found");
      continue;
    }
    encoderInfo.PortName = portName;


    // ---- Get a name of transformation
    const char* toAttribute = encoderInfoElement->GetAttribute("ToolReferenceFrame");
    if(!toAttribute)
    {
      toAttribute = deviceConfig->GetAttribute("ToolReferenceFrame");
      if(!toAttribute)
      {
        toAttribute = deviceConfig->GetAttribute("Id");
        LOG_WARNING("Device's attribute 'ToolReferenceFrame' is missing (Device Id='" << toAttribute << "')!");
      }
    }

    igsioTransformName transformName(id, toAttribute);
    if(!transformName.IsValid())
    {
      LOG_ERROR("Invalid transform name (From: '" << id << "'  To: '" << toAttribute << "')");
      continue;
    }

    encoderInfo.TransformName = transformName;

    if(this->TransformRepository->IsExistingTransform(encoderInfo.TransformName) != PLUS_SUCCESS)
    {
      this->TransformRepository->SetTransform(encoderInfo.TransformName,
                                              encoderInfo.EncoderToReference);
    }

    double vectorMatrix[16] = { 0 };
    if(encoderInfoElement->GetVectorAttribute("ToolToEncoderMatrix", 16, vectorMatrix))
    {
      encoderInfo.ToolToEncoderMatrix->DeepCopy(vectorMatrix);
    }
    else
    {
      encoderInfo.ToolToEncoderMatrix->Identity();
    }

    // Reading the MotionType of an US Digital Encoder
    std::string motiontype = encoderInfoElement->GetAttribute("MotionType");
    if(motiontype.empty())
    {
      LOG_ERROR("Cannot read the MotionType of an US Digital Encoder");
      continue;
    }
    std::transform(motiontype.begin(), motiontype.end(), motiontype.begin(), ::tolower);
    LOG_INFO("Motion Type :: " << motiontype);
    if(motiontype.find("linear") != std::string::npos)
    {
      encoderInfo.Motion = 0;
    }
    else if(motiontype.find("rotation") != std::string::npos)
    {
      encoderInfo.Motion = 1;
    }
    else
    {
      LOG_ERROR("Motion type of a US Digital Encoder unrecognized!");
      continue;
    }

    // Reading the pulse spacing of an US Digital Encoder
    // Linear Motion : mm /pulses
    // Rotation      : rad/pulses
    const char* pulseSpacing = encoderInfoElement->GetAttribute("PulseSpacing");
    if(pulseSpacing == NULL)
    {
      LOG_ERROR("Cannot read the PulseSpacing of an US Digital Encoder");
      continue;
    }
    encoderInfo.PulseSpacing = atof(pulseSpacing);

    const char* pulseSpacing2 = encoderInfoElement->GetAttribute("PulseSpacing2");
    bool coreXY = (pulseSpacing2 != NULL);
    if(coreXY)
    {
      encoderInfo.PulseSpacing2 = atof(pulseSpacing2);
    }
    else
    {
      LOG_INFO("PulseSpacing2 not found - this is not a coreXY configuration. Tool Id: " << id);
    }

    if(!encoderInfoElement->GetVectorAttribute("LocalAxis", 3, encoderInfo.LocalAxis.GetData()))
    {
      LOG_ERROR("Unable to find 'LocalAxis' attribute of an encoder in the configuration file");
      continue;
    }

    if(coreXY)
    {
      if(!encoderInfoElement->GetVectorAttribute("LocalAxis2", 3, encoderInfo.LocalAxis2.GetData()))
      {
        LOG_ERROR("Unable to find 'LocalAxis2' attribute in the configuration file");
        continue;
      }
    }

    // Reading the mode of a US Digital Encoder
    const char* mode = encoderInfoElement->GetAttribute("Mode");
    if(mode == NULL)
    {
      LOG_ERROR("Cannot read the Mode of an US Digital Encoder");
      continue;
    }
    encoderInfo.Mode = atol(mode);

    // Reading the resolution of an US Digital Encoder
    const char* resolution = encoderInfoElement->GetAttribute("Resolution");
    if(resolution == NULL)
    {
      LOG_ERROR("Cannot read the Resolution of an US Digital Encoder");
      continue;
    }
    encoderInfo.Resolution = atol(resolution);

    encoderInfo.Addr = deviceID++;
    if(coreXY)
    {
      encoderInfo.Addr2 = deviceID++;
    }
    EncoderList.push_back(encoderInfo);

    this->EncoderMap[encoderInfo.Addr] = &EncoderList.back();
    if(coreXY)  //enter this encoderInfo twice (once for each deviceID)
    {
      this->EncoderMap[encoderInfo.Addr2] = &EncoderList.back();
    }
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusUSDigitalEncodersTracker::WriteConfiguration(vtkXMLDataElement* rootConfigElement)
{
  LOG_WARNING("Writing configuration for USDigitalEncoders is not supported");
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusUSDigitalEncodersTracker::IsStepperAlive()
{
  long r = ::IsInitialized();
  RETURN_WITH_FAIL_IF(r != 1, "Device not yet initialized!");
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusUSDigitalEncodersTracker::SetUSDigitalA2EncodersStrobeMode()
{
  RETURN_WITH_FAIL_IF(::A2SetStrobe() != 0, "Failed to set US digital A2 Encoders as Strobe mode.");
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusUSDigitalEncodersTracker::SetUSDigitalA2EncodersSleep()
{
  RETURN_WITH_FAIL_IF(::A2SetSleep() != 0, "Failed to set US digital A2 Encoders as Sleep mode.");
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusUSDigitalEncodersTracker::SetUSDigitalA2EncodersWakeup()
{
  RETURN_WITH_FAIL_IF(::A2SetWakeup() != 0, "Failed to set US digital A2 Encoders as Wakeup mode.");
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusUSDigitalEncodersTracker::SetUSDigitalA2EncoderOriginWithID(long id)
{
  IDtoAddressType::iterator enc = this->IdAddress.find(id);
  RETURN_WITH_FAIL_IF(enc == this->IdAddress.end(), "Non-existent device ID: " << id);
  RETURN_WITH_FAIL_IF(::A2SetOrigin(enc->second) != 0,
                      "Failed to set US digital A2 Encoder's origin point as current position.");
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusUSDigitalEncodersTracker::SetAllUSDigitalA2EncoderOrigin()
{
  EncoderListType::iterator it;
  for(it = this->EncoderList.begin(); it != this->EncoderList.end(); ++it)
  {
    if(::A2SetOrigin(it->Addr) != 0)
    {
      return PLUS_FAIL;
    }

    if(it->Addr2 != 0)  //coreXY
    {
      if(::A2SetOrigin(it->Addr2) != 0)
      {
        return PLUS_FAIL;
      }
    }
  }
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusUSDigitalEncodersTracker::SetUSDigitalA2EncoderModeWithID(long id, long mode)
{
  IDtoAddressType::iterator enc = this->IdAddress.find(id);
  RETURN_WITH_FAIL_IF(enc == this->IdAddress.end(), "Non-existent device ID: " << id);
  RETURN_WITH_FAIL_IF(::A2SetMode(enc->second, mode) != 0,
                      "Failed to set the mode of an US digital A2 Encoder.");
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusUSDigitalEncodersTracker::GetUSDigitalA2EncoderModeWithID(long id, long* mode)
{
  IDtoAddressType::iterator enc = this->IdAddress.find(id);
  RETURN_WITH_FAIL_IF(enc == this->IdAddress.end(), "Non-existent device ID: " << id);
  RETURN_WITH_FAIL_IF(::A2GetMode(enc->second, mode) != 0,
                      "Failed to get the mode of an US digital A2 Encoder.");
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusUSDigitalEncodersTracker::SetUSDigitalA2EncoderResoultionWithID(long id, long res)
{
  IDtoAddressType::iterator enc = this->IdAddress.find(id);
  RETURN_WITH_FAIL_IF(enc == this->IdAddress.end(), "Non-existent device ID: " << id);

  RETURN_WITH_FAIL_IF(::A2SetResolution(enc->second, res) != 0,
                      "Failed to set the resoultion of an US digital A2 Encoder.");
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusUSDigitalEncodersTracker::GetUSDigitalA2EncoderResoultionWithID(long id, long* res)
{
  IDtoAddressType::iterator enc = this->IdAddress.find(id);
  RETURN_WITH_FAIL_IF(enc == this->IdAddress.end(), "Non-existent device ID: " << id);

  RETURN_WITH_FAIL_IF(::A2GetResolution(enc->second, res) != 0,
                      "Failed to get the resoultion of an US digital A2 Encoder.");
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusUSDigitalEncodersTracker::SetUSDigitalA2EncoderPositionWithID(long id, long pos)
{
  IDtoAddressType::iterator enc = this->IdAddress.find(id);
  RETURN_WITH_FAIL_IF(enc == this->IdAddress.end(), "Non-existent device ID: " << id);

  RETURN_WITH_FAIL_IF(::A2SetPosition(enc->second, pos) != 0,
                      "Failed to set the position of an US digital A2 Encoder.");
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusUSDigitalEncodersTracker::GetUSDigitalA2EncoderPositionWithID(long id, long* pos)
{
  IDtoAddressType::iterator enc = this->IdAddress.find(id);
  RETURN_WITH_FAIL_IF(enc == this->IdAddress.end(), "Non-existent device ID: " << id);

  RETURN_WITH_FAIL_IF(::A2GetPosition(enc->second, pos) != 0,
                      "Failed to get the position of an US digital A2 Encoder.");
  return PLUS_SUCCESS;
}
