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
  long Encoder_Model = 0;
  long Encoder_Version = 0;
  long Encoder_Addr = 0;
  long Encoder_Addr2 = 0;

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
  long Encoder_Mode = 4;// Defaul mode : Strobe (OFF), MultiTurn(On), Size(OFF), Incr (OFF), /256 (OFF)
  long Encoder_Resolution = 3600;
  bool Encoder_Connected = false;
  int Encoder_Motion = 0; // 0 : Linear motion , 1: Rotation
  double Encoder_PulseSpacing = 0.0;
  double Encoder_PulseSpacing2 = 0.0;
  vtkSmartPointer<vtkTransform> Encoder_LocalTransform = vtkSmartPointer<vtkTransform>::New();
  vtkVector3d Encoder_LocalAxis;
  vtkVector3d Encoder_LocalAxis2;
public:
  vtkSmartPointer<vtkMatrix4x4> Encoder_PreTMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
  vtkSmartPointer<vtkMatrix4x4> Encoder_TransformationMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
  PlusTransformName Encoder_TransformName;
  std::string Encoder_ToolId;
  bool Encoder_Persistent = true;
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
  if (this->Recording)
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
  // Initialization.
  long EncoderStatus = ::InitializeSEI(0, REINITIALIZE | AUTOASSIGN | NORESET);
  if (EncoderStatus != 0)
  {
    LOG_ERROR("Failed to initialize SEI!");
    return PLUS_FAIL;
  }

  long numberofConnectedEncoders = ::GetNumberOfDevices();
  EncoderInfoMapType::iterator encoderInfoPos;
  for (long deviceID = 0; deviceID < numberofConnectedEncoders; ++deviceID)
  {
    // Support multiple US digital A2 encoders.
    long model = 0;
    long serialNumber = 0;
    long version = 0;
    long address = 0;
    long retVal;

    if (::GetDeviceInfo(deviceID, &model, &serialNumber, &version, &address) != 0)
    {
      LOG_ERROR("Failed to get SEI device info for device number: " << deviceID);
      return PLUS_FAIL;
    }

    encoderInfoPos = this->EncoderMap.find(address);

    if (encoderInfoPos == this->EncoderMap.end())
    {
      LOG_WARNING("Unregistered encoder is detected");
    }
    else
    {
      if (encoderInfoPos->second->Encoder_Addr == address)
      {
        encoderInfoPos->second->Encoder_Connected = true;
        encoderInfoPos->second->Encoder_Model = model;
        encoderInfoPos->second->Encoder_Version = version;
        encoderInfoPos->second->Encoder_Addr = address;
        retVal = ::A2SetMode(encoderInfoPos->second->Encoder_Addr, encoderInfoPos->second->Encoder_Mode);
        if (retVal != 0)
        {
          LOG_ERROR("Failed to set SEI device mode for device SN: " << serialNumber << ", address: " << address);
          return PLUS_FAIL;
        }
        retVal = ::A2SetPosition(encoderInfoPos->second->Encoder_Addr, 0); // Initialize the value of the first encoder
        if (retVal != 0)
        {
          LOG_ERROR("Failed to set initial position for SEI device SN: " << serialNumber << ", address: " << address);
          return PLUS_FAIL;
        }
      }
      else //coreXY second encoder
      {
        encoderInfoPos->second->Encoder_Connected = true;
        encoderInfoPos->second->Encoder_Model = model;
        encoderInfoPos->second->Encoder_Version = version;
        encoderInfoPos->second->Encoder_Addr2 = address;
        retVal = ::A2SetMode(encoderInfoPos->second->Encoder_Addr2, encoderInfoPos->second->Encoder_Mode);
        if (retVal != 0)
        {
          LOG_ERROR("Failed to set SEI device mode for device SN: " << serialNumber << ", address: " << address);
          return PLUS_FAIL;
        }
        retVal = ::A2SetPosition(encoderInfoPos->second->Encoder_Addr2, 0); // Initialize the value of the second encoder
        if (retVal != 0)
        {
          LOG_ERROR("Failed to set initial position for SEI device SN: " << serialNumber << ", address: " << address);
          return PLUS_FAIL;
        }
      }
    }
  }

  // Remove unconnected encoders info from the encoder info list.
  encoderInfoPos = this->EncoderMap.begin();
  while (encoderInfoPos != this->EncoderMap.end())
  {
    if (!encoderInfoPos->second->Encoder_Connected)
    {
      encoderInfoPos = EncoderMap.erase(encoderInfoPos);
    }
    else
    {
      ++encoderInfoPos;
    }
  }

  return PLUS_SUCCESS;
}

//-------------------------------------------------------------------------
PlusStatus vtkPlusUSDigitalEncodersTracker::InternalDisconnect()
{
  LOG_TRACE("vtkPlusUSDigitalEncodersTracker::Disconnect");
  return this->StopRecording();
}

//-------------------------------------------------------------------------
PlusStatus vtkPlusUSDigitalEncodersTracker::Probe()
{
  LOG_TRACE("vtkPlusUSDigitalEncodersTracker::Probe");

  if (this->Recording)
  {
    return PLUS_SUCCESS;
  }

  if (!this->Connect())
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
  if (this->IsRecording())
  {
    return PLUS_SUCCESS;
  }

  return PLUS_SUCCESS;
}

//-------------------------------------------------------------------------
PlusStatus vtkPlusUSDigitalEncodersTracker::InternalStopRecording()
{
  LOG_TRACE("vtkPlusUSDigitalEncodersTracker::InternalStopRecording");

  if (::IsInitialized() != 1)
  {
    // Device not yet initialized
    return PLUS_SUCCESS;
  }

  if (::CloseSEI() != 0)
  {
    LOG_ERROR("Failed to close SEI!");
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
bool vtkPlusUSDigitalEncodersTracker::IsValidSEIAddress(long address)
{
  return address > INVALID_SEI_ADDRESS;
}

//-------------------------------------------------------------------------
PlusStatus vtkPlusUSDigitalEncodersTracker::InternalUpdate()
{
  LOG_TRACE("vtkPlusUSDigitalEncodersTracker::InternalUpdate");

  if (!this->Recording)
  {
    LOG_ERROR("called Update() when not tracking");
    return PLUS_FAIL;
  }

  if (this->CoreXY)
  {
    assert(EncoderMap.size() / 2 == 0);
    vtkPlusUSDigitalEncodersTracker::vtkPlusUSDigitalEncoderInfo& encoders =
      *this->EncoderMap.begin()->second;
    if (!encoders.Encoder_Connected)
    {
      LOG_ERROR("USDigital encoder(s) not connected!");
      return PLUS_FAIL;
    }

    long encoderValue;
    long errorCode;
    vtkSmartPointer<vtkTransform> tempTransform = vtkSmartPointer<vtkTransform>::New();

    // Read encoder positions and transform it into XY position in mm
    errorCode = ::A2GetPosition(encoders.Encoder_Addr, &encoderValue);
    if (errorCode)
    {
      LOG_ERROR("Unable to read position of first encoder with address: " << encoders.Encoder_Addr);
      return PLUS_FAIL;
    }
    double firstEnc = encoderValue * encoders.Encoder_PulseSpacing;

    errorCode = ::A2GetPosition(encoders.Encoder_Addr2, &encoderValue);
    if (errorCode)
    {
      LOG_ERROR("Unable to read position of second encoder with address: " << encoders.Encoder_Addr2);
      return PLUS_FAIL;
    }
    double secondEnc = encoderValue * encoders.Encoder_PulseSpacing2;

    double firstAxis = firstEnc + secondEnc;
    double secondAxis = firstEnc - secondEnc;

    //now make a transform matrix out of this translation and add it into PLUS system
    vtkVector3d localmovement = encoders.Encoder_LocalAxis;
    vtkMath::MultiplyScalar(localmovement.GetData(), firstAxis);
    tempTransform->Translate(localmovement.GetData());
    localmovement = encoders.Encoder_LocalAxis2;
    vtkMath::MultiplyScalar(localmovement.GetData(), secondAxis);
    tempTransform->Translate(localmovement.GetData());

    vtkMatrix4x4::Multiply4x4(encoders.Encoder_PreTMatrix,
                              tempTransform->GetMatrix(),
                              encoders.Encoder_TransformationMatrix);

    this->USDigitalEncoderTransformRepository->SetTransform(encoders.Encoder_TransformName,
        encoders.Encoder_TransformationMatrix);
    if (MyToolTimeStampedUpdate(encoders) == PLUS_FAIL)
    {
      LOG_ERROR("Unable to find tool with ID: " << encoders.Encoder_ToolId);
    }
  }
  else //regular combination of stages
  {
    EncoderInfoMapType::iterator encoderinfopos;
    for (encoderinfopos = this->EncoderMap.begin();
         encoderinfopos != this->EncoderMap.end();
         ++encoderinfopos)
    {
      if (encoderinfopos->second->Encoder_Connected)
      {
        long encoderValue;
        // Get current encoder values from one connected US digital encoder
        ::A2GetPosition(encoderinfopos->second->Encoder_Addr, &encoderValue);

        // Update transformation matrix of the connected US digital encoder
        vtkVector3d localmovement = encoderinfopos->second->Encoder_LocalAxis;
        vtkSmartPointer<vtkTransform> tempTransform = vtkSmartPointer<vtkTransform>::New();

        if (encoderinfopos->second->Encoder_Motion == 0)
        {
          vtkMath::MultiplyScalar(localmovement.GetData(),
                                  encoderValue * encoderinfopos->second->Encoder_PulseSpacing);

          tempTransform->Translate(localmovement.GetData());
        }
        else if (encoderinfopos->second->Encoder_Motion == 1)
        {
          // Check the unit of rotation angle .... (degree or radian)
          tempTransform->RotateWXYZ(encoderValue * encoderinfopos->second->Encoder_PulseSpacing,
                                    localmovement.GetData());
        }
        else
        {
          LOG_ERROR("Un-supported motion");
        }

        vtkMatrix4x4::Multiply4x4(encoderinfopos->second->Encoder_PreTMatrix,
                                  tempTransform->GetMatrix(),
                                  encoderinfopos->second->Encoder_TransformationMatrix);

        this->USDigitalEncoderTransformRepository->SetTransform(encoderinfopos->second->Encoder_TransformName,
            encoderinfopos->second->Encoder_TransformationMatrix);
        if (MyToolTimeStampedUpdate(*encoderinfopos->second) == PLUS_FAIL)
        {
          LOG_ERROR("Unable to find tool with ID: " << encoderinfopos->second->Encoder_ToolId);
          continue;
        }
      }
    }
  }

  for (unsigned int i = 0; i < EncoderList.size(); ++i)
  {
    if (!EncoderList[i].Encoder_Persistent)
    {
      this->USDigitalEncoderTransformRepository->GetTransform(EncoderList[i].Encoder_TransformName,
          EncoderList[i].Encoder_TransformationMatrix);
      if (MyToolTimeStampedUpdate(EncoderList[i]) == PLUS_FAIL)
      {
        LOG_ERROR("Unable to find tool for transform: " << EncoderList[i].Encoder_TransformName);
        continue;
      }
    }
  }

  return PLUS_SUCCESS;
}

//---------------------------------------------------------------------------
PlusStatus vtkPlusUSDigitalEncodersTracker::MyToolTimeStampedUpdate(vtkPlusUSDigitalEncoderInfo& encoderInfo)
{
  if (!encoderInfo.Encoder_Persistent)
  {
    vtkPlusDataSource* tool = NULL;
    if (this->GetTool(encoderInfo.Encoder_ToolId.c_str(), tool) != PLUS_SUCCESS)
    {
      LOG_ERROR("Unable to find tool with ID: " << encoderInfo.Encoder_ToolId);
      return PLUS_FAIL;
    }

    // Devices has no frame numbering, so just auto increment tool frame number
    unsigned long frameNumber = tool->GetFrameNumber() + 1;
    const double unfilteredTimestamp = vtkPlusAccurateTimer::GetSystemTime();
    this->ToolTimeStampedUpdate(tool->GetId(),
                                encoderInfo.Encoder_TransformationMatrix,
                                TOOL_OK,
                                frameNumber,
                                unfilteredTimestamp);
  }
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusUSDigitalEncodersTracker::ReadConfiguration(vtkXMLDataElement* rootConfigElement)
{
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_READING(deviceConfig, rootConfigElement);

  XML_FIND_NESTED_ELEMENT_REQUIRED(dataSourcesElement, deviceConfig, "DataSources");

  this->USDigitalEncoderTransformRepository->Clear();
  this->EncoderMap.clear();
  this->EncoderList.clear();

  for (int encoderIndex = 0; encoderIndex < dataSourcesElement->GetNumberOfNestedElements(); encoderIndex++)
  {
    vtkXMLDataElement* encoderInfoElement = dataSourcesElement->GetNestedElement(encoderIndex);
    if (STRCASECMP(encoderInfoElement->GetName(), "DataSource") != 0)
    {
      // if this is not a data source element, skip it
      continue;
    }

    vtkPlusUSDigitalEncoderInfo encoderInfo;

    const char* id = encoderInfoElement->GetAttribute("Id");
    if (id == NULL)
    {
      LOG_ERROR("Tool Id is missing!");
      continue;
    }

    vtkPlusDataSource* tool = NULL;
    if (this->GetTool(id, tool) != PLUS_SUCCESS)
    {
      LOG_ERROR("GetTool for tool with ID " << id << " failed");
      continue;
    }
    if (tool == NULL)
    {
      LOG_ERROR("Tool with ID " << id << " was not found");
      continue;
    }
    encoderInfo.Encoder_ToolId = id;


    // ---- Get PortName
    const char* portName = encoderInfoElement->GetAttribute("PortName");
    if (portName == NULL)
    {
      LOG_ERROR("Cannot set sensor-specific parameters: tool portname is undefined");
      continue;
    }

    if (STRCASECMP(portName, "coreXY") == 0)
    {
      CoreXY = true;
    }
    else
    {
      CoreXY = false;
    }

    // ---- Get a name of transformation
    const char* toAttribute = deviceConfig->GetAttribute("ToolReferenceFrame");

    if (!toAttribute)
    {
      toAttribute = deviceConfig->GetAttribute("Id");
      LOG_WARNING("Device's attribute 'ToolReferenceFrame' is missing (Device Id='" << toAttribute << "')!");
    }

    PlusTransformName transformName(id, toAttribute);
    if (!transformName.IsValid())
    {
      LOG_ERROR("Invalid transform name (From: '" << id << "'  To: '" << toAttribute << "')");
      continue;
    }

    encoderInfo.Encoder_TransformName = transformName;

    bool isPersistent = true;
    if (encoderInfoElement->GetAttribute("Persistent")) // if it exists, then it is non-persistent
    {
      if (STRCASECMP(encoderInfoElement->GetAttribute("Persistent"), "FALSE") == 0)
      {
        isPersistent = false;
      }
    }

    encoderInfo.Encoder_Persistent = isPersistent;
    if (this->USDigitalEncoderTransformRepository->IsExistingTransform(encoderInfo.Encoder_TransformName) != PLUS_SUCCESS)
    {
      this->USDigitalEncoderTransformRepository->SetTransform(encoderInfo.Encoder_TransformName,
          encoderInfo.Encoder_TransformationMatrix);
    }

    // ---- Get PreTMatrix:
    double vectorMatrix[16] = { 0 };
    if (encoderInfoElement->GetVectorAttribute("PreTMatrix", 16, vectorMatrix))
    {
      encoderInfo.Encoder_PreTMatrix->DeepCopy(vectorMatrix);
    }
    else
    {
      encoderInfo.Encoder_PreTMatrix->Identity();;
    }


    // Reading the MotionType of an US Digital Encoder
    std::string motiontype = encoderInfoElement->GetAttribute("MotionType");
    if (motiontype.empty())
    {
      LOG_ERROR("Cannot read the MotionType of an US Digital Encoder");
      continue;
    }
    std::transform(motiontype.begin(), motiontype.end(), motiontype.begin(), ::tolower);
    LOG_INFO("Motion Type :: " << motiontype);
    if (motiontype.find("linear") != std::string::npos)
    {
      encoderInfo.Encoder_Motion = 0;
    }
    else if (motiontype.find("rotation") != std::string::npos)
    {
      encoderInfo.Encoder_Motion = 1;
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
    if (pulseSpacing == NULL)
    {
      LOG_ERROR("Cannot read the PulseSpacing of an US Digital Encoder");
      continue;
    }
    encoderInfo.Encoder_PulseSpacing = atof(pulseSpacing);

    if (this->CoreXY)
    {
      // Reading the pulse spacing of an US Digital Encoder
      // Linear Motion : mm /pulses
      // Rotation      : rad/pulses
      const char* pulseSpacing = encoderInfoElement->GetAttribute("PulseSpacing2");
      if (pulseSpacing == NULL)
      {
        LOG_ERROR("Cannot read the second PulseSpacing of an US Digital Encoder in coreXY mode");
        continue;
      }
      encoderInfo.Encoder_PulseSpacing2 = atof(pulseSpacing);
    }

    if (!encoderInfoElement->GetVectorAttribute("LocalAxis", 3, encoderInfo.Encoder_LocalAxis.GetData()))
    {
      LOG_ERROR("Unable to find 'LocalAxis' attribute of an encoder in the configuration file");
      continue;
    }

    if (this->CoreXY)
    {
      if (!encoderInfoElement->GetVectorAttribute("LocalAxis2", 3, encoderInfo.Encoder_LocalAxis2.GetData()))
      {
        LOG_ERROR("Unable to find 'LocalAxis2' attribute of an encoder in the configuration file");
        continue;
      }
    }

    // Reading the mode of an US Digital Encoder
    const char* mode = encoderInfoElement->GetAttribute("Mode");
    if (mode == NULL)
    {
      LOG_ERROR("Cannot read the Mode of an US Digital Encoder");
      continue;
    }
    encoderInfo.Encoder_Mode = atol(mode);

    // Reading the resolution of an US Digital Encoder
    const char* resolution = encoderInfoElement->GetAttribute("Resolution");
    if (resolution == NULL)
    {
      LOG_ERROR("Cannot read the Resolution of an US Digital Encoder");
      continue;
    }
    encoderInfo.Encoder_Resolution = atol(resolution);

    EncoderList.push_back(encoderInfo);

    // Build the list of US Digital Encoder Info
    this->EncoderMap[encoderInfo.Encoder_Addr] = &EncoderList.back();
    if (this->CoreXY) //enter this encoderInfo twice (once for each address)
    {
      this->EncoderMap[encoderInfo.Encoder_Addr] = &EncoderList.back();
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
  if (r != 1)
  {
    // Device not yet initialized
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusUSDigitalEncodersTracker::SetUSDigitalA2EncodersStrobeMode()
{
  if (::A2SetStrobe() != 0)
  {
    LOG_ERROR("Failed to set US digital A2 Encodrs as Strobe mode.");
    return PLUS_FAIL;
  }
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusUSDigitalEncodersTracker::SetUSDigitalA2EncodersSleep()
{
  if (::A2SetSleep() != 0)
  {
    LOG_ERROR("Failed to set US digital A2 Encodrs as Sleep mode.");
    return PLUS_FAIL;
  }
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusUSDigitalEncodersTracker::SetUSDigitalA2EncodersWakeup()
{
  if (::A2SetWakeup() != 0)
  {
    LOG_ERROR("Failed to set US digital A2 Encodrs as Wakeup mode.");
    return PLUS_FAIL;
  }
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusUSDigitalEncodersTracker::SetUSDigitalA2EncoderOriginWithAddr(long address)
{
  if (::A2SetOrigin(address) != 0)
  {
    LOG_ERROR("Failed to set US digital A2 Encoder's origin point as current position.");
    return PLUS_FAIL;
  }
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusUSDigitalEncodersTracker::SetAllUSDigitalA2EncoderOrigin()
{
  EncoderInfoMapType::iterator it;
  for (it = this->EncoderMap.begin(); it != this->EncoderMap.end(); ++it)
  {
    if (this->SetUSDigitalA2EncoderOriginWithAddr(it->second->Encoder_Addr) == PLUS_FAIL)
    {
      return PLUS_FAIL;
    }

    if (this->CoreXY)
    {
      if (this->SetUSDigitalA2EncoderOriginWithAddr(it->second->Encoder_Addr2) == PLUS_FAIL)
      {
        return PLUS_FAIL;
      }
      //break; //only 2 encoders
    }
  }
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusUSDigitalEncodersTracker::SetUSDigitalA2EncoderModeWithAddr(long address, long mode)
{
  if (::A2SetMode(address, mode) != 0)
  {
    LOG_ERROR("Failed to set the mode of an US digital A2 Encodr.");
    return PLUS_FAIL;
  }
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusUSDigitalEncodersTracker::GetUSDigitalA2EncoderModeWithAddr(long address, long* mode)
{
  if (::A2GetMode(address, mode) != 0)
  {
    LOG_ERROR("Failed to get the mode of an US digital A2 Encodr.");
    return PLUS_FAIL;
  }
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusUSDigitalEncodersTracker::SetUSDigitalA2EncoderResoultionWithAddr(long address, long res)
{
  if (::A2SetResolution(address, res) != 0)
  {
    LOG_ERROR("Failed to set the resoultion of an US digital A2 Encodr.");
    return PLUS_FAIL;
  }
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusUSDigitalEncodersTracker::GetUSDigitalA2EncoderResoultionWithAddr(long address, long* res)
{
  if (::A2GetResolution(address, res) != 0)
  {
    LOG_ERROR("Failed to get the resoultion of an US digital A2 Encodr.");
    return PLUS_FAIL;
  }
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusUSDigitalEncodersTracker::SetUSDigitalA2EncoderPositionWithAddr(long address, long pos)
{
  if (::A2SetPosition(address, pos) != 0)
  {
    LOG_ERROR("Failed to set the position of an US digital A2 Encodr.");
    return PLUS_FAIL;
  }
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusUSDigitalEncodersTracker::GetUSDigitalA2EncoderPositionWithAddr(long address, long* pos)
{
  if (::A2GetPosition(address, pos) != 0)
  {
    LOG_ERROR("Failed to get the position of an US digital A2 Encodr.");
    return PLUS_FAIL;
  }
  return PLUS_SUCCESS;
}
