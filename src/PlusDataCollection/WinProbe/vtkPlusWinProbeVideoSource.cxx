/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"
#include "vtkPlusWinProbeVideoSource.h"
#include "vtkPlusChannel.h"
#include "vtkPlusDataSource.h"
#include "vtkPlusUSImagingParameters.h"

#include "WinProbe.h"

#include <algorithm>
#include <PlusMath.h>

//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkPlusWinProbeVideoSource);

// ----------------------------------------------------------------------------
void vtkPlusWinProbeVideoSource::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
  os << indent << "MinValue: " << this->m_MinValue << std::endl;
  os << indent << "MaxValue: " << this->m_MaxValue << std::endl;
  os << indent << "LogLinearKnee: " << this->m_Knee << std::endl;
  os << indent << "LogMax: " << static_cast<unsigned>(this->m_OutputKnee) << std::endl;
  os << indent << "TransducerID: " << this->m_TransducerID << std::endl;
  os << indent << "Frozen: " << this->IsFrozen() << std::endl;
  os << indent << "Voltage: " << static_cast<unsigned>(this->GetVoltage()) << std::endl;
  os << indent << "Frequency: " << this->GetTransmitFrequencyMHz() << std::endl;
  os << indent << "Depth: " << this->GetScanDepthMm() << std::endl;
  for(int i = 0; i < 8; i++)
  {
    os << indent << "TGC" << i << ": " << m_TimeGainCompensation[i] << std::endl;
  }
  for(int i = 0; i < 4; i++)
  {
    os << indent << "FocalPointDepth" << i << ": " << m_FocalPointDepth[i] << std::endl;
  }

  os << indent << "CustomFields: " << std::endl;
  vtkIndent indent2 = indent.GetNextIndent();
  igsioTrackedFrame::FieldMapType::iterator it;
  for(it = m_CustomFields.begin(); it != m_CustomFields.end(); ++it)
  {
    os << indent2 << it->first << ": " << it->second << std::endl;
  }
}

// ----------------------------------------------------------------------------
PlusStatus vtkPlusWinProbeVideoSource::ReadConfiguration(vtkXMLDataElement* rootConfigElement)
{
  LOG_TRACE("vtkPlusWinProbeVideoSource::ReadConfiguration");
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_READING(deviceConfig, rootConfigElement);

  XML_READ_STRING_ATTRIBUTE_REQUIRED(TransducerID, deviceConfig);
  XML_READ_BOOL_ATTRIBUTE_OPTIONAL(UseDeviceFrameReconstruction, deviceConfig);
  XML_READ_BOOL_ATTRIBUTE_OPTIONAL(SpatialCompoundEnabled, deviceConfig);
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(float, TransmitFrequencyMHz, deviceConfig);
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(float, ScanDepthMm, deviceConfig);
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(float, SpatialCompoundAngle, deviceConfig);
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(int, SpatialCompoundCount, deviceConfig);
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(unsigned long, Voltage, deviceConfig); //implicit type conversion
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(unsigned long, MinValue, deviceConfig); //implicit type conversion
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(unsigned long, MaxValue, deviceConfig); //implicit type conversion
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(unsigned long, LogLinearKnee, deviceConfig); //implicit type conversion
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(unsigned long, LogMax, deviceConfig); //implicit type conversion

  deviceConfig->GetVectorAttribute("TimeGainCompensation", 8, m_TimeGainCompensation);
  deviceConfig->GetVectorAttribute("FocalPointDepth", 4, m_FocalPointDepth);

  return PLUS_SUCCESS;
}

// ----------------------------------------------------------------------------
PlusStatus vtkPlusWinProbeVideoSource::WriteConfiguration(vtkXMLDataElement* rootConfigElement)
{
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_WRITING(deviceConfig, rootConfigElement);

  deviceConfig->SetAttribute("TransducerID", this->m_TransducerID.c_str());
  deviceConfig->SetAttribute("UseDeviceFrameReconstruction", this->m_UseDeviceFrameReconstruction ? "TRUE" : "FALSE");
  deviceConfig->SetAttribute("SpatialCompoundEnabled", this->GetSpatialCompoundEnabled() ? "TRUE" : "FALSE");
  deviceConfig->SetFloatAttribute("TransmitFrequencyMHz", this->GetTransmitFrequencyMHz());
  deviceConfig->SetFloatAttribute("ScanDepthMm", this->GetScanDepthMm());
  deviceConfig->SetFloatAttribute("SpatialCompoundAngle", this->GetSpatialCompoundAngle());
  deviceConfig->SetIntAttribute("SpatialCompoundCount", this->GetSpatialCompoundCount());
  deviceConfig->SetUnsignedLongAttribute("Voltage", this->GetVoltage());
  deviceConfig->SetUnsignedLongAttribute("MinValue", this->GetMinValue());
  deviceConfig->SetUnsignedLongAttribute("MaxValue", this->GetMaxValue());
  deviceConfig->SetUnsignedLongAttribute("LogLinearKnee", this->GetLogLinearKnee());
  deviceConfig->SetUnsignedLongAttribute("LogMax", this->GetLogMax());

  deviceConfig->SetVectorAttribute("TimeGainCompensation", 8, m_TimeGainCompensation);
  deviceConfig->SetVectorAttribute("FocalPointDepth", 4, m_FocalPointDepth);

  return PLUS_SUCCESS;
}

// ----------------------------------------------------------------------------
vtkPlusWinProbeVideoSource* thisPtr = nullptr;

//-----------------------------------------------------------------------------
// This callback function is invoked after each frame is ready
int __stdcall frameCallback(int length, char* data, char* hHeader, char* hGeometry)
{
  thisPtr->FrameCallback(length, data, hHeader, hGeometry);
  return length;
}

//-----------------------------------------------------------------------------
void vtkPlusWinProbeVideoSource::ReconstructFrame(char* data)
{
  uint16_t* frame = reinterpret_cast<uint16_t*>(data + 16);
  assert(m_BModeBuffer.size() == m_SamplesPerLine * m_LineCount);
  const float logFactor = m_OutputKnee / std::log(1 + m_Knee);

  #pragma omp parallel for
  for(unsigned t = 0; t < m_LineCount; t++)
  {
    for(unsigned s = 0; s < m_SamplesPerLine; s++)
    {
      uint16_t val = frame[t * m_SamplesPerLine + s];
      if(val <= m_MinValue)  // subtract noise floor
      {
        val = 0;
      }
      else
      {
        val -= m_MinValue;
      }
      if(val > m_MaxValue)  //apply ceiling
      {
        val = m_MaxValue;
      }

      float cVal;
      if(val < m_Knee)
      {
        cVal = logFactor * std::log(float(1 + val));
      }
      else //linear mapping
      {
        cVal = m_OutputKnee + (val - m_Knee) * float(255 - m_OutputKnee) / (m_MaxValue - m_Knee);
      }
      m_BModeBuffer[s * m_LineCount + t] = static_cast<uint8_t>(cVal);
    }
  }
}

// ----------------------------------------------------------------------------
void vtkPlusWinProbeVideoSource::FrameCallback(int length, char* data, char* hHeader, char* hGeometry)
{
  CineModeFrameHeader* header = (CineModeFrameHeader*)hHeader;
  CFDGeometryStruct* cfdGeometry = (CFDGeometryStruct*)hGeometry;
  GeometryStruct* brfGeometry = (GeometryStruct*)hGeometry; //B-mode and RF
  this->FrameNumber = header->TotalFrameCounter;
  InputSourceBindings usMode = header->InputSourceBinding;
  if(usMode & CFD)
  {
    m_LineCount = cfdGeometry->LineCount;
    m_SamplesPerLine = cfdGeometry->SamplesPerKernel;
  }
  else if(usMode & B || usMode & BFRFALineImage_RFData)
  {
    m_LineCount = brfGeometry->LineCount;
    m_SamplesPerLine = brfGeometry->SamplesPerLine;
  }
  else
  {
    LOG_INFO("Unsupported frame type: " << std::hex << usMode);
    return;
  }

  //timestamp counters are in milliseconds since last execute() call
  double timestamp = header->TimeStamp / 1000.0;
  timestamp += m_TimestampOffset;
  LOG_DEBUG("Frame: " << FrameNumber << ". Mode: " << std::setw(4) << std::hex << usMode << ". Timestamp: " << timestamp);

  if(usMode & B && !m_BSources.empty()) // B-mode flag is set, and B-mode source is defined
  {
    assert(length == m_SamplesPerLine * m_LineCount * sizeof(uint16_t) + 16); //frame + header
    FrameSizeType frameSize = { m_LineCount, m_SamplesPerLine, 1 };

    if(m_UseDeviceFrameReconstruction && usMode == B) //this only works with plain B-mode
    {
      WPNewData(length, data, hHeader, hGeometry);
      char* frameData = nullptr;
      int length = WPSaveImageToPointer(&frameData);
      assert(length == m_LineCount * m_SamplesPerLine * sizeof(uint32_t));
      auto* frameRGBA = reinterpret_cast<uint32_t*>(frameData);

      // all the color channels are the same for B-mode
      // and alpha is filled with ones (fully opaque)
      for(unsigned i = 0; i < m_LineCount * m_SamplesPerLine; i++)
      {
        m_BModeBuffer[i] = static_cast<uint8_t>(frameRGBA[i]);
      }
      WPFreePointer(frameData);
    }
    else
    {
      this->ReconstructFrame(data);
    }

    for(unsigned i = 0; i < m_BSources.size(); i++)
    {
      if(m_BSources[i]->AddItem(&m_BModeBuffer[0],
                                m_BSources[i]->GetInputImageOrientation(),
                                frameSize, VTK_UNSIGNED_CHAR,
                                1, US_IMG_BRIGHTNESS, 0,
                                this->FrameNumber,
                                timestamp,
                                timestamp, //no timestamp filtering needed
                                &this->m_CustomFields) != PLUS_SUCCESS)
      {
        LOG_WARNING("Error adding item to video source " << m_BSources[i]->GetSourceId());
      }
    }
  }
  else if(usMode & B)  //this is B frame, but B-mode source is NOT defined
  {
    LOG_DEBUG("Frame ignored - B-mode source not defined. Got mode: " << std::hex << usMode);
    return;
  }
  else if(usMode & BFRFALineImage_RFData)
  {
    assert(length == m_SamplesPerLine * brfGeometry->Decimation * m_LineCount * sizeof(int32_t)); //header and footer not appended to data
    FrameSizeType frameSize = { m_SamplesPerLine* brfGeometry->Decimation, m_LineCount, 1 };
    for(unsigned i = 0; i < m_RFSources.size(); i++)
    {
      if(m_RFSources[i]->AddItem(data,
                                 m_RFSources[i]->GetInputImageOrientation(),
                                 frameSize, VTK_INT,
                                 1, US_IMG_RF_REAL, 0,
                                 this->FrameNumber,
                                 timestamp,
                                 timestamp,
                                 &m_CustomFields) != PLUS_SUCCESS)
      {
        LOG_WARNING("Error adding item to video source " << m_RFSources[i]->GetSourceId());
      }
    }
  }
  else if(usMode & CFD)
  {
    //TODO
  }
  else
  {
    LOG_INFO("Frame ignored. Got mode: " << std::hex << usMode);
    return;
  }

  this->Modified();
}

//----------------------------------------------------------------------------
void vtkPlusWinProbeVideoSource::AdjustBufferSize()
{
  FrameSizeType frameSize = { m_LineCount, m_SamplesPerLine, 1 };

  LOG_DEBUG("Set up image buffers for WinProbe");
  for(unsigned i = 0; i < m_BSources.size(); i++)
  {
    m_BSources[i]->SetPixelType(VTK_UNSIGNED_CHAR);
    m_BSources[i]->SetImageType(US_IMG_BRIGHTNESS);
    m_BSources[i]->SetOutputImageOrientation(US_IMG_ORIENT_MF);
    m_BSources[i]->SetInputImageOrientation(US_IMG_ORIENT_MF);
    m_BSources[i]->SetInputFrameSize(frameSize);
    LOG_INFO("SourceID: " << m_BSources[i]->GetId() << ", "
             << "Frame size: " << frameSize[0] << "x" << frameSize[1]
             << ", pixel type: " << vtkImageScalarTypeNameMacro(m_BSources[i]->GetPixelType())
             << ", buffer image orientation: "
             << PlusVideoFrame::GetStringFromUsImageOrientation(m_BSources[i]->GetInputImageOrientation()));
  }

  for(unsigned i = 0; i < m_RFSources.size(); i++)
  {
    frameSize[0] = m_SamplesPerLine*::GetSSDecimation();
    frameSize[1] = m_LineCount;
    m_RFSources[i]->SetPixelType(VTK_INT);
    m_RFSources[i]->SetImageType(US_IMG_RF_REAL);
    m_RFSources[i]->SetOutputImageOrientation(US_IMG_ORIENT_FM);
    m_RFSources[i]->SetInputImageOrientation(US_IMG_ORIENT_FM);
    m_RFSources[i]->SetInputFrameSize(frameSize);
    LOG_INFO("SourceID: " << m_RFSources[i]->GetId() << ", "
             << "Frame size: " << frameSize[0] << "x" << frameSize[1]
             << ", pixel type: " << vtkImageScalarTypeNameMacro(m_RFSources[i]->GetPixelType())
             << ", buffer image orientation: "
             << PlusVideoFrame::GetStringFromUsImageOrientation(m_RFSources[i]->GetInputImageOrientation()));
  }

  m_BModeBuffer.resize(m_SamplesPerLine * m_LineCount);
}

//----------------------------------------------------------------------------
void vtkPlusWinProbeVideoSource::AdjustSpacing()
{
  this->CurrentPixelSpacingMm[0] = this->GetTransducerWidthMm() / (m_LineCount - 1);
  this->CurrentPixelSpacingMm[1] = m_ScanDepth / (m_SamplesPerLine - 1);
  this->CurrentPixelSpacingMm[2] = 1.0;

  std::ostringstream spacingStream;
  unsigned int numSpaceDimensions = 3;
  for(unsigned int i = 0; i < numSpaceDimensions; ++i)
  {
    spacingStream << this->CurrentPixelSpacingMm[i];
    if(i != numSpaceDimensions - 1)
    {
      spacingStream << " ";
    }
  }
  this->m_CustomFields["ElementSpacing"] = spacingStream.str();
  LOG_DEBUG("Adjusted spacing: " << spacingStream.str());
}

// ----------------------------------------------------------------------------
vtkPlusWinProbeVideoSource::vtkPlusWinProbeVideoSource()
{
  this->RequireImageOrientationInConfiguration = true;

  for(int i = 0; i < 8; i++)
  {
    m_TimeGainCompensation[i] = 0.0;
  }

  for(int i = 0; i < 4; i++)
  {
    m_FocalPointDepth[i] = 0.0f;
  }

  AdjustSpacing();

  Callback funcPtr = &frameCallback;
  thisPtr = this;
  WPSetCallback(funcPtr);
  WPInitialize();
}

// ----------------------------------------------------------------------------
vtkPlusWinProbeVideoSource::~vtkPlusWinProbeVideoSource()
{
  if(this->Connected)
  {
    this->Disconnect();
  }
}

// ----------------------------------------------------------------------------
PlusStatus vtkPlusWinProbeVideoSource::InternalConnect()
{
  this->GetVideoSourcesByPortName(vtkPlusDevice::RFMODE_PORT_NAME, m_RFSources);
  this->GetVideoSourcesByPortName(vtkPlusDevice::BMODE_PORT_NAME, m_BSources);
  if(m_RFSources.empty() && m_BSources.empty())
  {
    vtkPlusDataSource* aSource = NULL;
    if(this->GetFirstActiveOutputVideoSource(aSource) != PLUS_SUCCESS || aSource == NULL)
    {
      LOG_ERROR("Neither B-mode nor RF-mode data sources are defined, and unable to retrieve the video source in the capturing device.");
      return PLUS_FAIL;
    }
    m_BSources.push_back(aSource); //else consider this the only output of B-mode type
  }

  LOG_DEBUG("Connect to WinProbe");
  if(!WPConnect())
  {
    LOG_ERROR("Failed connecting to WinProbe!");
    return PLUS_FAIL;
  }
  if(!WPLoadDefault())
  {
    LOG_ERROR("Failed loading defaults!");
    return PLUS_FAIL;
  }
  LOG_DEBUG("Setting transducer ID: " << this->m_TransducerID);
  WPSetTransducerID(this->m_TransducerID.c_str());

  m_ADCfrequency = GetADCSamplingRate();
  this->m_CustomFields["SamplingRate"] = std::to_string(m_ADCfrequency);
  m_LineCount = GetSSElementCount();
  SetSCCompoundAngleCount(0);

  LOG_DEBUG("GetHandleBRFInternally: " << GetHandleBRFInternally());
  LOG_DEBUG("GetBFRFImageCaptureMode: " << GetBFRFImageCaptureMode());

  if(!m_BSources.empty())
  {
    SetHandleBRFInternally(true);
    SetBFRFImageCaptureMode(0);
  }
  if(!m_RFSources.empty()) //overwrite B-mode settings
  {
    SetHandleBRFInternally(false);
    SetBFRFImageCaptureMode(2);
  }
  //TODO handle additional modes

  LOG_DEBUG("GetHandleBRFInternally: " << GetHandleBRFInternally());
  LOG_DEBUG("GetBFRFImageCaptureMode: " << GetBFRFImageCaptureMode());
  SetPendingRecreateTables(true);

  return PLUS_SUCCESS;
}

// ----------------------------------------------------------------------------
PlusStatus vtkPlusWinProbeVideoSource::InternalDisconnect()
{
  LOG_DEBUG("Disconnecting from WinProbe");
  if(IsRecording())
  {
    this->InternalStopRecording();
  }
  WPDisconnect();
  LOG_DEBUG("Disconnect from WinProbe finished");
  return PLUS_SUCCESS;
}

// ----------------------------------------------------------------------------
PlusStatus vtkPlusWinProbeVideoSource::InternalStartRecording()
{
  //apply requested settings
  for(int i = 0; i < 8; i++)
  {
    SetTGC(i, m_TimeGainCompensation[i]);
    m_TimeGainCompensation[i] = GetTGC(i);
  }
  //SetPendingTGCUpdate(true);
  for(int i = 0; i < 4; i++)
  {
    ::SetFocalPointDepth(i, m_FocalPointDepth[i]);
    m_FocalPointDepth[i] = ::GetFocalPointDepth(i);
  }
  this->SetTransmitFrequencyMHz(m_Frequency);
  this->SetVoltage(m_Voltage);
  this->SetScanDepthMm(m_ScanDepth); //as a side-effect calls AdjustSpacing and AdjustBufferSize
  if (m_SpatialCompoundEnabled)
  {
    this->SetSpatialCompoundAngle(m_SpatialCompoundAngle);
    this->SetSpatialCompoundCount(m_SpatialCompoundCount);
  }

  //setup size for DirectX image
  LOG_DEBUG("Setting output size to " << m_LineCount << "x" << m_SamplesPerLine);
  WPSetSize(m_LineCount, m_SamplesPerLine);
  char* sessionPtr = GetSessionPtr();
  bool success = WPVPSetSession(sessionPtr);
  if(!success)
  {
    LOG_WARNING("Failed setting session pointer!");
    WPDisconnect();
    return PLUS_FAIL;
  }
  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  m_TimestampOffset = vtkPlusAccurateTimer::GetSystemTime();
  LOG_DEBUG("GetPendingRecreateTables: " << GetPendingRecreateTables());
  LOG_DEBUG("GetPendingRestartSequencer: " << GetPendingRestartSequencer());
  LOG_DEBUG("GetPendingRun30Frames: " << GetPendingRun30Frames());
  WPExecute();
  return PLUS_SUCCESS;
}

// ----------------------------------------------------------------------------
PlusStatus vtkPlusWinProbeVideoSource::InternalStopRecording()
{
  WPStopScanning();
  return PLUS_SUCCESS;
}

// ----------------------------------------------------------------------------
PlusStatus vtkPlusWinProbeVideoSource::FreezeDevice(bool freeze)
{
  if(!IsRecording() == freeze) //already in desired mode
  {
    return PLUS_SUCCESS;
  }

  if(IsRecording())
  {
    this->StopRecording();
  }
  else
  {
    this->StartRecording();
  }

  return PLUS_SUCCESS;
}

// ----------------------------------------------------------------------------
bool vtkPlusWinProbeVideoSource::IsFrozen()
{
  return !IsRecording();
}

// ----------------------------------------------------------------------------
PlusStatus vtkPlusWinProbeVideoSource::SetTransmitFrequencyMHz(float frequency)
{
  m_Frequency = frequency;
  if(Connected)
  {
    ::SetTxTxFrequency(frequency);
    SetPendingRecreateTables(true);
    //what we requested might be only approximately satisfied
    m_Frequency = ::GetTxTxFrequency();
  }
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
float vtkPlusWinProbeVideoSource::GetTransmitFrequencyMHz()
{
  if(Connected)
  {
    m_Frequency = ::GetTxTxFrequency();
  }
  return m_Frequency;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusWinProbeVideoSource::SetVoltage(uint8_t voltage)
{
  m_Voltage = voltage;
  if(Connected)
  {
    ::SetVoltage(voltage);
    SetPendingRecreateTables(true);
    //what we requested might be only approximately satisfied
    m_Voltage = ::GetVoltage();
  }
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
uint8_t vtkPlusWinProbeVideoSource::GetVoltage()
{
  if(Connected)
  {
    m_Voltage = ::GetVoltage();
  }
  return m_Voltage;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusWinProbeVideoSource::SetScanDepthMm(float depth)
{
  m_ScanDepth = depth;
  if(Connected)
  {
    ::SetSSDepth(depth);
    SetPendingRecreateTables(true);
    //what we requested might be only approximately satisfied
    m_ScanDepth = ::GetSSDepth();
    m_SamplesPerLine = static_cast<unsigned int>(GetSSSamplesPerLine()); //this and decimation change depending on depth
    AdjustSpacing();
    AdjustBufferSize();
  }
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
float vtkPlusWinProbeVideoSource::GetScanDepthMm()
{
  if(Connected)
  {
    m_ScanDepth = ::GetSSDepth();
  }
  return m_ScanDepth;
}

//----------------------------------------------------------------------------
float vtkPlusWinProbeVideoSource::GetTransducerWidthMm()
{
  if(Connected)
  {
    m_TransducerWidth = ::GetTWidth();
  }
  return m_TransducerWidth;
}

//----------------------------------------------------------------------------
const double* vtkPlusWinProbeVideoSource::GetCurrentPixelSpacingMm()
{
  return this->CurrentPixelSpacingMm;
}

//----------------------------------------------------------------------------
double vtkPlusWinProbeVideoSource::GetTimeGainCompensation(int index)
{
  assert(index >= 0 && index < 8);
  if(Connected)
  {
    m_TimeGainCompensation[index] = GetTGC(index);
  }
  return m_TimeGainCompensation[index];
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusWinProbeVideoSource::SetTimeGainCompensation(int index, double value)
{
  assert(index >= 0 && index < 8);
  m_TimeGainCompensation[index] = value;
  if(Connected)
  {
    SetTGC(index, value);
    SetPendingRecreateTables(true);
    //SetPendingTGCUpdate(true);
    //what we requested might be only approximately satisfied
    m_TimeGainCompensation[index] = GetTGC(index);
  }
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
float vtkPlusWinProbeVideoSource::GetFocalPointDepth(int index)
{
  assert(index >= 0 && index < 4);
  if(Connected)
  {
    m_FocalPointDepth[index] = ::GetFocalPointDepth(index);
  }
  return m_TimeGainCompensation[index];
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusWinProbeVideoSource::SetFocalPointDepth(int index, float depth)
{
  assert(index >= 0 && index < 4);
  m_FocalPointDepth[index] = depth;
  if(Connected)
  {
    ::SetFocalPointDepth(index, depth);
    SetPendingRecreateTables(true);
    //what we requested might be only approximately satisfied
    m_TimeGainCompensation[index] = ::GetFocalPointDepth(index);
  }
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
void vtkPlusWinProbeVideoSource::SetSpatialCompoundEnabled(bool value)
{
  if(Connected)
  {
    SetSCIsEnabled(value);
  }
  m_SpatialCompoundEnabled = value;
}

bool vtkPlusWinProbeVideoSource::GetSpatialCompoundEnabled()
{
  if(Connected)
  {
    m_SpatialCompoundEnabled = GetSCIsEnabled();
  }
  return m_SpatialCompoundEnabled;
}

void vtkPlusWinProbeVideoSource::SetSpatialCompoundAngle(float value)
{
  m_SpatialCompoundAngle = value;
  if(Connected)
  {
    SetSCCompoundAngle(value);
    m_SpatialCompoundAngle = GetSCCompoundAngle(); //in case it was not exactly satisfied
  }
}

float vtkPlusWinProbeVideoSource::GetSpatialCompoundAngle()
{
  if(Connected)
  {
    m_SpatialCompoundAngle = GetSCCompoundAngle();
  }
  return m_SpatialCompoundAngle;
}

void vtkPlusWinProbeVideoSource::SetSpatialCompoundCount(int32_t value)
{
  if(Connected)
  {
    SetSCCompoundAngleCount(value);
  }
  m_SpatialCompoundCount = value;
}

int32_t vtkPlusWinProbeVideoSource::GetSpatialCompoundCount()
{
  if(Connected)
  {
    m_SpatialCompoundCount = GetSCCompoundAngleCount();
  }
  return m_SpatialCompoundCount;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusWinProbeVideoSource::SetTransducerID(std::string guid)
{
  this->m_TransducerID = guid;
  if(Connected)
  {
    WPSetTransducerID(guid.c_str());
    SetPendingRecreateTables(true);
  }
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
std::string vtkPlusWinProbeVideoSource::GetTransducerID()
{
  return this->m_TransducerID;
}
