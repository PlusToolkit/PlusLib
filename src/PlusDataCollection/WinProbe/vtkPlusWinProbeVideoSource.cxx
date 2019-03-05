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
#include <cmath>
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
  os << indent << "Mode: " << this->ModeToString(this->GetMode()) << std::endl;
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
  XML_READ_BOOL_ATTRIBUTE_OPTIONAL(MRevolvingEnabled, deviceConfig);
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(float, TransmitFrequencyMHz, deviceConfig);
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(float, ScanDepthMm, deviceConfig);
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(float, SpatialCompoundAngle, deviceConfig);
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(int, SpatialCompoundCount, deviceConfig);
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(int, MPRFrequency, deviceConfig);
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(int, MLineIndex, deviceConfig);
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(int, MWidth, deviceConfig);
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(int, MAcousticLineCount, deviceConfig);
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(int, MDepth, deviceConfig);
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(unsigned long, Voltage, deviceConfig); //implicit type conversion
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(unsigned long, MinValue, deviceConfig); //implicit type conversion
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(unsigned long, MaxValue, deviceConfig); //implicit type conversion
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(unsigned long, LogLinearKnee, deviceConfig); //implicit type conversion
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(unsigned long, LogMax, deviceConfig); //implicit type conversion
  const char* strMode = deviceConfig->GetAttribute("Mode");
  if(strMode)
  {
    m_Mode = this->StringToMode(strMode);
  }
  if(m_Mode == Mode::M)
  {
    const char* mwidthSeconds_string = deviceConfig->GetAttribute("MWidth");
    if(mwidthSeconds_string)
    {
      int mwidthSeconds = std::stoi(mwidthSeconds_string);
      if(mwidthSeconds > 0)
      {
        m_MWidth = this->MWidthFromSeconds(mwidthSeconds);
      }
    }
  }

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
  deviceConfig->SetAttribute("MRevolvingEnabled", this->GetMRevolvingEnabled() ? "TRUE" : "FALSE");
  deviceConfig->SetFloatAttribute("TransmitFrequencyMHz", this->GetTransmitFrequencyMHz());
  deviceConfig->SetFloatAttribute("ScanDepthMm", this->GetScanDepthMm());
  deviceConfig->SetFloatAttribute("SpatialCompoundAngle", this->GetSpatialCompoundAngle());
  deviceConfig->SetIntAttribute("SpatialCompoundCount", this->GetSpatialCompoundCount());
  deviceConfig->SetIntAttribute("MPRFrequency", this->GetMPRFrequency());
  deviceConfig->SetIntAttribute("MLineIndex", this->GetMLineIndex());
  deviceConfig->SetIntAttribute("MWidth", this->MSecondsFromWidth(this->m_MWidth));
  deviceConfig->SetIntAttribute("MAcousticLineCount", this->GetMAcousticLineCount());
  deviceConfig->SetIntAttribute("MDepth", this->GetMDepth());
  deviceConfig->SetUnsignedLongAttribute("Voltage", this->GetVoltage());
  deviceConfig->SetUnsignedLongAttribute("MinValue", this->GetMinValue());
  deviceConfig->SetUnsignedLongAttribute("MaxValue", this->GetMaxValue());
  deviceConfig->SetUnsignedLongAttribute("LogLinearKnee", this->GetLogLinearKnee());
  deviceConfig->SetUnsignedLongAttribute("LogMax", this->GetLogMax());
  deviceConfig->SetAttribute("Mode", ModeToString(this->m_Mode).c_str());

  deviceConfig->SetVectorAttribute("TimeGainCompensation", 8, m_TimeGainCompensation);
  deviceConfig->SetVectorAttribute("FocalPointDepth", 4, m_FocalPointDepth);

  return PLUS_SUCCESS;
}

// ----------------------------------------------------------------------------
vtkPlusWinProbeVideoSource::Mode vtkPlusWinProbeVideoSource::StringToMode(std::string modeString)
{
  if(modeString.empty())
  {
    LOG_WARNING("Empty mode string defaults to B mode");
    return Mode::B;
  }

  // convert to uppercase for easy comparison
  std::transform(modeString.begin(), modeString.end(), modeString.begin(), ::toupper);

  if(modeString == "B")
  { return Mode::B; }
  else if(modeString == "BRF")
  { return Mode::BRF; }
  else if(modeString == "RF")
  { return Mode::RF; }
  else if(modeString == "M")
  { return Mode::M; }
  else if(modeString == "PW")
  { return Mode::PW; }
  else if(modeString == "CFD")
  { return Mode::CFD; }
  else
  { LOG_ERROR("Unrecognized mode: " << modeString); }

  return Mode::B; // default mode
}

// ----------------------------------------------------------------------------
std::string vtkPlusWinProbeVideoSource::ModeToString(vtkPlusWinProbeVideoSource::Mode mode)
{
  switch(mode)
  {
  case Mode::B:
    return "B";
    break;
  case Mode::BRF:
    return "BRF";
    break;
  case Mode::RF:
    return "RF";
    break;
  case Mode::M:
    return "M";
    break;
  case Mode::PW:
    return "PW";
    break;
  case Mode::CFD:
    return "CFD";
    break;
  default:
    LOG_ERROR("Invalid mode passed: " << int(mode));
    return "B";
    break;
  }
}

// ----------------------------------------------------------------------------
int32_t vtkPlusWinProbeVideoSource::MWidthFromSeconds(int value)
{
  int32_t mlineWidth;
  if(value < 4)
  {
    mlineWidth = 128 * pow(2, value - 1);
  }
  else if(value <= 80)
  {
    mlineWidth = 1024 * value / 10;
  }
  else
  {
    mlineWidth = 128;  //Default to 1s width
  }
  return mlineWidth;
}

int vtkPlusWinProbeVideoSource::MSecondsFromWidth(int32_t value)
{
  int mwidthSeconds;
  if(value < 1024)
  {
    mwidthSeconds = std::log(value / 128) / std::log(2) + 1;
  }
  else if(value <= 8192)
  {
    mwidthSeconds = value * 10 / 1024;
    if(mwidthSeconds == 80)
    {
      mwidthSeconds += 1;  //81 s
    }
  }
  else
  {
    mwidthSeconds = 1;  //Default to 1s width
  }
  return mwidthSeconds;
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

void vtkPlusWinProbeVideoSource::FlipTexture(char* data)
{
  #pragma omp parallel for
  for(unsigned t = 0; t < m_LineCount; t++)
  {
    for(unsigned s = 0; s < m_SamplesPerLine; s++)
    {
      m_BModeBuffer[s * m_LineCount + t] = data[t * m_SamplesPerLine + s];
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
  else if(usMode & M_PostProcess)
  {
    m_LineCount = brfGeometry->LineCount;
    m_SamplesPerLine = brfGeometry->SamplesPerLine;
  }
  else if(usMode & PWD_PostProcess)
  {
    m_LineCount = brfGeometry->ElementCount;
    m_SamplesPerLine = brfGeometry->SamplesPerLine;
  }
  else
  {
    LOG_INFO("Unsupported frame type: " << std::hex << usMode);
    return;
  }

  //timestamp counters are in milliseconds since last recreate tables call
  double timestamp = header->TimeStamp / 1000.0;
  if(timestamp == 0.0) // some change is being applied, so this frame is not valid
  {
    return; // ignore this frame
  }
  timestamp += m_TimestampOffset;
  LOG_DEBUG("Frame: " << FrameNumber << ". Mode: " << std::setw(4) << std::hex << usMode << ". Timestamp: " << timestamp);

  if(usMode & B && !m_PrimarySources.empty()) // B-mode flag is set, and B-mode source is defined
  {
    assert(length == m_SamplesPerLine * m_LineCount * sizeof(uint16_t) + 16); //frame + header
    FrameSizeType frameSize = { m_LineCount, m_SamplesPerLine, 1 };

    if(m_UseDeviceFrameReconstruction && usMode == B) //this only works with plain B-mode
    {
      //WPNewData(length, data, hHeader, hGeometry);
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
      char* texture = nullptr;
      int tLength = WPDXGetFusedTexData(&texture);
      assert(tLength == m_SamplesPerLine * m_LineCount);
      if (tLength == m_SamplesPerLine * m_LineCount)
      {
        this->FlipTexture(texture);
        WPFreePointer(texture);
      }
      else
      {
        this->ReconstructFrame(data);
      }
    }

    for(unsigned i = 0; i < m_PrimarySources.size(); i++)
    {
      if(m_PrimarySources[i]->AddItem(&m_BModeBuffer[0],
                                      m_PrimarySources[i]->GetInputImageOrientation(),
                                      frameSize, VTK_UNSIGNED_CHAR,
                                      1, US_IMG_BRIGHTNESS, 0,
                                      this->FrameNumber,
                                      timestamp,
                                      timestamp, //no timestamp filtering needed
                                      &this->m_CustomFields) != PLUS_SUCCESS)
      {
        LOG_WARNING("Error adding item to video source " << m_PrimarySources[i]->GetSourceId());
      }
    }
  }
  else if(usMode & B)  //this is B frame, but B-mode source is NOT defined
  {
    LOG_DEBUG("Frame ignored - B-mode source not defined. Got mode: " << std::hex << usMode);
    return;
  }
  else if(usMode & BFRFALineImage_RFData || usMode & M_PostProcess || usMode & PWD_PostProcess)
  {
    assert(length == m_SamplesPerLine * brfGeometry->Decimation * m_LineCount * sizeof(int32_t)); //header and footer not appended to data
    FrameSizeType frameSize = { m_SamplesPerLine* brfGeometry->Decimation, m_LineCount, 1 };
    for(unsigned i = 0; i < m_ExtraSources.size(); i++)
    {
      if(m_ExtraSources[i]->AddItem(data,
                                    m_ExtraSources[i]->GetInputImageOrientation(),
                                    frameSize, VTK_INT,
                                    1, US_IMG_RF_REAL, 0,
                                    this->FrameNumber,
                                    timestamp,
                                    timestamp,
                                    &m_CustomFields) != PLUS_SUCCESS)
      {
        LOG_WARNING("Error adding item to video source " << m_ExtraSources[i]->GetSourceId());
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
  for(unsigned i = 0; i < m_PrimarySources.size(); i++)
  {
    m_PrimarySources[i]->Clear(); // clear current buffer content
    m_PrimarySources[i]->SetPixelType(VTK_UNSIGNED_CHAR);
    m_PrimarySources[i]->SetImageType(US_IMG_BRIGHTNESS);
    m_PrimarySources[i]->SetOutputImageOrientation(US_IMG_ORIENT_MF);
    m_PrimarySources[i]->SetInputImageOrientation(US_IMG_ORIENT_MF);
    m_PrimarySources[i]->SetInputFrameSize(frameSize);
    LOG_INFO("SourceID: " << m_PrimarySources[i]->GetId() << ", "
             << "Frame size: " << frameSize[0] << "x" << frameSize[1]
             << ", pixel type: " << vtkImageScalarTypeNameMacro(m_PrimarySources[i]->GetPixelType())
             << ", buffer image orientation: "
             << igsioVideoFrame::GetStringFromUsImageOrientation(m_PrimarySources[i]->GetInputImageOrientation()));
  }

  for(unsigned i = 0; i < m_ExtraSources.size(); i++)
  {
    frameSize[0] = m_SamplesPerLine*::GetSSDecimation();
    frameSize[1] = m_LineCount;
    m_ExtraSources[i]->Clear(); // clear current buffer content
    m_ExtraSources[i]->SetPixelType(VTK_INT);
    m_ExtraSources[i]->SetImageType(US_IMG_RF_REAL);
    m_ExtraSources[i]->SetOutputImageOrientation(US_IMG_ORIENT_FM);
    m_ExtraSources[i]->SetInputImageOrientation(US_IMG_ORIENT_FM);
    m_ExtraSources[i]->SetInputFrameSize(frameSize);
    LOG_INFO("SourceID: " << m_ExtraSources[i]->GetId() << ", "
             << "Frame size: " << frameSize[0] << "x" << frameSize[1]
             << ", pixel type: " << vtkImageScalarTypeNameMacro(m_ExtraSources[i]->GetPixelType())
             << ", buffer image orientation: "
             << igsioVideoFrame::GetStringFromUsImageOrientation(m_ExtraSources[i]->GetInputImageOrientation()));
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
  this->GetVideoSourcesByPortName(vtkPlusDevice::RFMODE_PORT_NAME, m_ExtraSources);
  this->GetVideoSourcesByPortName(vtkPlusDevice::BMODE_PORT_NAME, m_PrimarySources);
  if(m_ExtraSources.empty() && m_PrimarySources.empty())
  {
    vtkPlusDataSource* aSource = NULL;
    if(this->GetFirstActiveOutputVideoSource(aSource) != PLUS_SUCCESS || aSource == NULL)
    {
      LOG_ERROR("Neither B-mode nor RF-mode data sources are defined, and unable to retrieve the video source in the capturing device.");
      return PLUS_FAIL;
    }
    m_PrimarySources.push_back(aSource); //else consider this the only output of B-mode type
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

  std::string presetPath = "Default.xml";
  LOG_DEBUG("Loading Default Presets. " << presetPath);
  if(!LoadXmlPreset(presetPath.c_str()))
  {
    LOG_ERROR("Failed loading default presets!")
    return PLUS_FAIL;
  }

  m_ADCfrequency = GetADCSamplingRate();
  this->m_CustomFields["SamplingRate"] = std::to_string(m_ADCfrequency);
  m_LineCount = GetSSElementCount();
  SetSCCompoundAngleCount(0);

  LOG_DEBUG("GetHandleBRFInternally: " << GetHandleBRFInternally());
  LOG_DEBUG("GetBFRFImageCaptureMode: " << GetBFRFImageCaptureMode());

  if(!m_PrimarySources.empty())
  {
    SetHandleBRFInternally(true);
    SetBFRFImageCaptureMode(0);
  }
  if(!m_ExtraSources.empty()) //overwrite B-mode settings
  {
    SetHandleBRFInternally(false);
    SetBFRFImageCaptureMode(2);
  }
  if(m_Mode != Mode::RF) // all modes except pure RF require primary source
  {
    if(m_PrimarySources.empty())
    {
      LOG_ERROR("Primary source is not defined!");
    }
  }
  if(m_Mode == Mode::RF || m_Mode == Mode::BRF)
  {
    if(m_ExtraSources.empty())
    {
      LOG_ERROR("RF source is not defined!");
    }
  }
  if(m_Mode == Mode::PW)
  {
    SetPWIsEnabled(true);
  }
  if(m_Mode == Mode::CFD)
  {
    SetVoltage(70);
    SetCFSamplesPerKernel(2);
    // ...
    // more setup required
    SetCFIsEnabled(true);
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
  this->SetSpatialCompoundEnabled(m_SpatialCompoundEnabled); // also takes care of angle  and count

  //setup size for DirectX image
  LOG_DEBUG("Setting output size to " << m_LineCount << "x" << m_SamplesPerLine);
  char* sessionPtr = GetSessionPtr();
  bool success = WPVPSetSession(sessionPtr);
  if(!success)
  {
    LOG_WARNING("Failed setting session pointer!");
    WPDisconnect();
    return PLUS_FAIL;
  }
  WPSetSize(m_LineCount, m_SamplesPerLine); 
  if(!m_UseDeviceFrameReconstruction)
  {
    WPDXSetIsGetSpatialCompoundedTexEnabled(true);
  }
  WPDXSetDrawTextLayer(false);
  WPDXSetDrawScalesAndBars(false);
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  if(m_Mode == Mode::M)
  {
    this->SetMModeEnabled(true);
  }

  m_TimestampOffset = vtkIGSIOAccurateTimer::GetSystemTime();
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
    m_TimestampOffset = vtkIGSIOAccurateTimer::GetSystemTime(); // recreate tables resets internal timer
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
    m_TimestampOffset = vtkIGSIOAccurateTimer::GetSystemTime(); // recreate tables resets internal timer
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
    if(Recording)
    {
      WPStopScanning();
    }
    ::SetSSDepth(depth);
    SetPendingRecreateTables(true);
    //what we requested might be only approximately satisfied
    m_ScanDepth = ::GetSSDepth();
    m_SamplesPerLine = static_cast<unsigned int>(GetSSSamplesPerLine()); //this and decimation change depending on depth
    AdjustSpacing();
    AdjustBufferSize();
    m_TimestampOffset = vtkIGSIOAccurateTimer::GetSystemTime(); // recreate tables resets internal timer
    if(Recording)
    {
      WPExecute();
    }
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
    //SetPendingRecreateTables(true);
    SetPendingTGCUpdate(true);
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
  return m_FocalPointDepth[index];
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
    m_FocalPointDepth[index] = ::GetFocalPointDepth(index);
    m_TimestampOffset = vtkIGSIOAccurateTimer::GetSystemTime(); // recreate tables resets internal timer
  }
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
void vtkPlusWinProbeVideoSource::SetSpatialCompoundEnabled(bool value)
{
  if(Connected)
  {
    SetSCIsEnabled(value);
    if(value)
    {
      SetSCCompoundAngle(m_SpatialCompoundAngle);
      SetSCCompoundAngleCount(m_SpatialCompoundCount);
    }
    else
    {
      SetSCCompoundAngleCount(0);
    }
    SetPendingRecreateTables(true);
    m_TimestampOffset = vtkIGSIOAccurateTimer::GetSystemTime(); // recreate tables resets internal timer
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
    SetPendingRecreateTables(true);
    m_SpatialCompoundAngle = GetSCCompoundAngle(); //in case it was not exactly satisfied
    m_TimestampOffset = vtkIGSIOAccurateTimer::GetSystemTime(); // recreate tables resets internal timer
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
    SetPendingRecreateTables(true);
    m_TimestampOffset = vtkIGSIOAccurateTimer::GetSystemTime(); // recreate tables resets internal timer
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

void vtkPlusWinProbeVideoSource::SetMModeEnabled(bool value)
{

  // m_MModeEnabled = value;
  if(Connected)
  {
    SetMIsEnabled(value);
    if(value)
    {
      SetMIsRevolving(m_MRevolvingEnabled);
      SetMPRF(m_MPRF);
      SetMAcousticLineIndex(m_MLineIndex);
      ::SetMWidth(m_MWidth);
      ::SetMAcousticLineCount(m_MAcousticLineCount);
    }
    SetPendingRecreateTables(true);
    m_TimestampOffset = vtkIGSIOAccurateTimer::GetSystemTime(); // recreate tables resets internal timer
    LOG_INFO("M-Mode enabled");
  }
  if(value)
  {
    m_Mode = Mode::M;
  }
  else
  {
    m_Mode = Mode::B;
  }
}

bool vtkPlusWinProbeVideoSource::GetMModeEnabled()
{
  bool mmodeEnabled;
  if(Connected)
  {
    mmodeEnabled = GetMIsEnabled();
    if(mmodeEnabled)
    {
      m_Mode = Mode::M;
    }
    else
    {
      m_Mode = Mode::B;
    }
  }
  return mmodeEnabled;
}


void vtkPlusWinProbeVideoSource::SetMRevolvingEnabled(bool value)
{
  if(Connected)
  {
    SetMIsRevolving(value);
    SetPendingRecreateTables(true);
    m_TimestampOffset = vtkIGSIOAccurateTimer::GetSystemTime(); // recreate tables resets internal timer
  }
  m_MRevolvingEnabled = value;
}

bool vtkPlusWinProbeVideoSource::GetMRevolvingEnabled()
{
  if(Connected)
  {
    m_MRevolvingEnabled = GetMIsRevolving();
  }
  return m_MRevolvingEnabled;
}

void vtkPlusWinProbeVideoSource::SetMPRFrequency(int32_t value)
{
  if(Connected)
  {
    SetMPRF(value);
    SetPendingRecreateTables(true);
    m_TimestampOffset = vtkIGSIOAccurateTimer::GetSystemTime(); // recreate tables resets internal timer
  }
  m_MPRF = value;
}

int32_t vtkPlusWinProbeVideoSource::GetMPRFrequency()
{
  if(Connected)
  {
    m_MPRF = GetMPRF();
  }
  return m_MPRF;
}

void vtkPlusWinProbeVideoSource::SetMLineIndex(int32_t value)
{
  if(Connected)
  {
    SetMAcousticLineIndex(value);
    SetPendingRecreateTables(true);
    m_TimestampOffset = vtkIGSIOAccurateTimer::GetSystemTime(); // recreate tables resets internal timer
  }
  m_MLineIndex = value;
}

int32_t vtkPlusWinProbeVideoSource::GetMLineIndex()
{
  if(Connected)
  {
    m_MLineIndex = GetMAcousticLineIndex();
  }
  return m_MLineIndex;
}

void vtkPlusWinProbeVideoSource::SetMWidth(int value)
{
  if(Connected)
  {
    int32_t mwidth = this->MWidthFromSeconds(value);
    ::SetMWidth(mwidth);
    SetPendingRecreateTables(true);
    m_TimestampOffset = vtkIGSIOAccurateTimer::GetSystemTime(); // recreate tables resets internal timer
  }
  m_MWidth = value;
}

int32_t vtkPlusWinProbeVideoSource::GetMWidth()
{
  int mwidthSeconds = 0;
  if(Connected)
  {
    m_MWidth = ::GetMWidth();
    mwidthSeconds = this->MSecondsFromWidth(m_MWidth);
  }
  return mwidthSeconds;
}

void vtkPlusWinProbeVideoSource::SetMAcousticLineCount(int32_t value)
{
  if(Connected)
  {
    ::SetMAcousticLineCount(value);
    SetPendingRecreateTables(true);
    m_TimestampOffset = vtkIGSIOAccurateTimer::GetSystemTime(); // recreate tables resets internal timer
  }
  m_MAcousticLineCount = value;
}

int32_t vtkPlusWinProbeVideoSource::GetMAcousticLineCount()
{
  if(Connected)
  {
    m_MAcousticLineCount = ::GetMAcousticLineCount();
  }
  return m_MAcousticLineCount;
}

void vtkPlusWinProbeVideoSource::SetMDepth(int32_t value)
{
  if(Connected)
  {
    ::SetMDepth(value);
    SetPendingRecreateTables(true);
    m_TimestampOffset = vtkIGSIOAccurateTimer::GetSystemTime(); // recreate tables resets internal timer
  }
  m_MDepth = value;
}

int32_t vtkPlusWinProbeVideoSource::GetMDepth()
{
  if(Connected)
  {
    m_MDepth = ::GetMDepth();
  }
  return m_MDepth;
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
