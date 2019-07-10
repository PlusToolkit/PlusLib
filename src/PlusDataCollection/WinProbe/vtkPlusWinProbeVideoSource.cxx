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
  igsioFieldMapType::iterator it;
  for(it = m_CustomFields.begin(); it != m_CustomFields.end(); ++it)
  {
    os << indent2 << it->first << ": " << it->second.second << std::endl;
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
  XML_READ_BOOL_ATTRIBUTE_OPTIONAL(BHarmonicEnabled, deviceConfig);
  XML_READ_BOOL_ATTRIBUTE_OPTIONAL(MRevolvingEnabled, deviceConfig);
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(float, TransmitFrequencyMHz, deviceConfig);
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(float, ScanDepthMm, deviceConfig);
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(float, SpatialCompoundAngle, deviceConfig);
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(int, SpatialCompoundCount, deviceConfig);
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(int, MPRFrequency, deviceConfig);
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(int, MLineIndex, deviceConfig);
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(int, MWidth, deviceConfig);
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(int32_t, MWidthLines, deviceConfig);
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(int, MAcousticLineCount, deviceConfig);
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(int, MDepth, deviceConfig);
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(unsigned long, Voltage, deviceConfig); //implicit type conversion
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(unsigned long, MinValue, deviceConfig); //implicit type conversion
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(unsigned long, MaxValue, deviceConfig); //implicit type conversion
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(unsigned long, LogLinearKnee, deviceConfig); //implicit type conversion
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(unsigned long, LogMax, deviceConfig); //implicit type conversion
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(unsigned long, SSDecimation, deviceConfig); //implicit type conversion
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(double, FirstGainValue, deviceConfig);
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(int, BFrameRateLimit, deviceConfig);


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
  deviceConfig->SetAttribute("HarmonicEnabled", this->GetBHarmonicEnabled() ? "TRUE" : "FALSE");
  deviceConfig->SetAttribute("MRevolvingEnabled", this->GetMRevolvingEnabled() ? "TRUE" : "FALSE");
  deviceConfig->SetFloatAttribute("TransmitFrequencyMHz", this->GetTransmitFrequencyMHz());
  deviceConfig->SetFloatAttribute("ScanDepthMm", this->GetScanDepthMm());
  deviceConfig->SetFloatAttribute("SpatialCompoundAngle", this->GetSpatialCompoundAngle());
  deviceConfig->SetIntAttribute("SpatialCompoundCount", this->GetSpatialCompoundCount());
  deviceConfig->SetIntAttribute("MPRFrequency", this->GetMPRFrequency());
  deviceConfig->SetIntAttribute("MLineIndex", this->GetMLineIndex());
  deviceConfig->SetIntAttribute("MWidth", this->MSecondsFromWidth(this->m_MWidth));
  deviceConfig->SetIntAttribute("MWidthLines", this->m_MWidth);
  deviceConfig->SetIntAttribute("MAcousticLineCount", this->GetMAcousticLineCount());
  deviceConfig->SetIntAttribute("MDepth", this->GetMDepth());
  deviceConfig->SetUnsignedLongAttribute("Voltage", this->GetVoltage());
  deviceConfig->SetUnsignedLongAttribute("MinValue", this->GetMinValue());
  deviceConfig->SetUnsignedLongAttribute("MaxValue", this->GetMaxValue());
  deviceConfig->SetUnsignedLongAttribute("LogLinearKnee", this->GetLogLinearKnee());
  deviceConfig->SetUnsignedLongAttribute("LogMax", this->GetLogMax());
  deviceConfig->SetUnsignedLongAttribute("SSDecimation", this->GetSSDecimation());
  deviceConfig->SetAttribute("Mode", ModeToString(this->m_Mode).c_str());
  deviceConfig->SetDoubleAttribute("FirstGainValue", this->GetFirstGainValue());
  deviceConfig->SetIntAttribute("BFrameRateLimit", this->GetBFrameRateLimit());

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
  int32_t mlineWidth = pow(2, round(std::log(value * m_MPRF) / std::log(2)));
  return mlineWidth;
}

int vtkPlusWinProbeVideoSource::MSecondsFromWidth(int32_t value)
{
  int mwidthSeconds = floor(value / m_MPRF);
  return mwidthSeconds;
}

// ----------------------------------------------------------------------------
vtkPlusWinProbeVideoSource* thisPtr = nullptr;

//-----------------------------------------------------------------------------
// This callback function is invoked after each frame is ready
int __stdcall frameCallback(int length, char* data, char* hHeader, char* hGeometry, char* hModeFrameHeader)
{
  thisPtr->FrameCallback(length, data, hHeader, hGeometry);
  return length;
}

//-----------------------------------------------------------------------------
void vtkPlusWinProbeVideoSource::ReconstructFrame(char* data, std::vector<uint8_t>& buffer, const FrameSizeType& frameSize)
{
  uint16_t* frame = reinterpret_cast<uint16_t*>(data + 16);
  assert(buffer.size() == frameSize[0] * frameSize[1]);
  const float logFactor = m_OutputKnee / std::log(1 + m_Knee);

  #pragma omp parallel for
  for(unsigned t = 0; t < frameSize[0]; t++)
  {
    for(unsigned s = 0; s < frameSize[1]; s++)
    {
      uint16_t val = frame[t * frameSize[1] + s];
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
      buffer[s * frameSize[0] + t] = static_cast<uint8_t>(cVal);
    }
  }
}

void vtkPlusWinProbeVideoSource::FlipTexture(char* data, const FrameSizeType& frameSize, int rowPitch)
{
  #pragma omp parallel for
  for(unsigned t = 0; t < frameSize[0]; t++)
  {
    for(unsigned s = 0; s < frameSize[1]; s++)
    {
      m_PrimaryBuffer[s * frameSize[0] + t] = data[t * rowPitch + s];
    }
  }
}


// ----------------------------------------------------------------------------
void vtkPlusWinProbeVideoSource::FrameCallback(int length, char* data, char* hHeader, char* hGeometry)
{
  CineModeFrameHeader* header = (CineModeFrameHeader*)hHeader;
  CFDGeometryStruct* cfdGeometry = (CFDGeometryStruct*)hGeometry;
  GeometryStruct* brfGeometry = (GeometryStruct*)hGeometry; //B-mode and RF
  MGeometryStruct* mGeometry = (MGeometryStruct*)hGeometry;
  PWGeometryStruct* pwGeometry = (PWGeometryStruct*)hGeometry;
  this->FrameNumber = header->TotalFrameCounter;
  InputSourceBindings usMode = header->InputSourceBinding;
  FrameSizeType frameSize;
  if(usMode & BFRFALineImage_RFData)
  {
    frameSize = { m_SamplesPerLine * m_SSDecimation, m_LineCount, 1 };
  }
  else
  {
    frameSize = { m_LineCount, m_SamplesPerLine, 1 };
  }

  if(usMode & CFD)
  {
    frameSize[0] = cfdGeometry->LineCount;
    frameSize[1] = cfdGeometry->SamplesPerKernel;
  }
  else if(usMode & B || usMode & BFRFALineImage_SampleData)
  {
    frameSize[0] = brfGeometry->LineCount;
    frameSize[1] = brfGeometry->SamplesPerLine;
    if(frameSize[1] != m_SamplesPerLine)
    {
      LOG_INFO("SamplesPerLine has changed from " << m_SamplesPerLine
               << " to " << frameSize[1] << ". Adjusting spacing and buffer sizes.");
      m_SamplesPerLine = frameSize[1];
      AdjustBufferSizes();
      AdjustSpacing(false);
    }
    else if(this->CurrentPixelSpacingMm[1] != m_ScanDepth / (m_SamplesPerLine - 1)) // we might need approximate equality check
    {
      LOG_INFO("Scan Depth changed. Adjusting spacing.");
      AdjustSpacing(false);
    }
  }
  else if(usMode & BFRFALineImage_RFData)
  {
    frameSize[0] = brfGeometry->SamplesPerLine * brfGeometry->Decimation;
    frameSize[1] = brfGeometry->LineCount;
    if(frameSize != m_ExtraSources[0]->GetInputFrameSize())
    {
      LOG_INFO("Rf frame size updated. Adjusting buffer size and spacing.");
      m_SamplesPerLine = brfGeometry->SamplesPerLine;
      m_LineCount = brfGeometry->LineCount;
      m_SSDecimation = brfGeometry->Decimation;
      AdjustBufferSizes();
      AdjustSpacing(true);
    }
    else if(this->CurrentPixelSpacingMm[0] = m_ScanDepth / (m_SamplesPerLine * m_SSDecimation - 1)) // we might need approximate equality check
    {
      LOG_INFO("Scan Depth changed. Adjusting spacing.");
      AdjustSpacing(true);
    }
  }
  else if(usMode & M_PostProcess)
  {
    frameSize[0] = mGeometry->LineCount;
    frameSize[1] = mGeometry->SamplesPerLine;
    if(m_ExtraSources.empty())
    {
      return; //the source is not defined, do not waste time on processing this frame
    }
    if(frameSize != m_ExtraSources[0]->GetInputFrameSize())
    {
      LOG_INFO("SamplesPerLine has changed from " << m_ExtraSources[0]->GetInputFrameSize()[1]
               << " to " << frameSize[1] << ". Adjusting buffer size.");
      m_SamplesPerLine = frameSize[1];
      AdjustBufferSizes();
      AdjustSpacing(false);
    }
    else if(this->CurrentPixelSpacingMm[1] != m_ScanDepth / (m_SamplesPerLine - 1)) // we might need approximate equality check
    {
      LOG_INFO("Scan Depth changed. Adjusting spacing.");
      AdjustSpacing(false);
    }
  }
  else if(usMode & PWD_PostProcess)
  {
    frameSize[0] = pwGeometry->NumberOfImageLines;
    frameSize[1] = pwGeometry->NumberOfImageSamples;
  }
  else
  {
    LOG_INFO("Unsupported frame type: " << std::hex << usMode);
    return;
  }
  if(header->TotalFrameCounter == 0)
  {
    first_timestamp = header->TimeStamp / 1000.0;
    LOG_DEBUG("First frame timestamp: "<< first_timestamp);
  }
  //timestamp counters are in milliseconds since last sequencer restart
  double timestamp = (header->TimeStamp / 1000.0) - first_timestamp;
  if(timestamp == 0.0) // some change is being applied, so this frame is not valid
  {
    return; // ignore this frame
  }
  if(timestamp < m_LastTimestamp)
  {
    double alternativeTime = m_TimestampOffset + m_LastTimestamp;
    m_TimestampOffset = vtkIGSIOAccurateTimer::GetSystemTime();
    LOG_INFO("Hardware timestamp counter restarted. Alternative time: " << alternativeTime);
  }
  m_LastTimestamp = timestamp;
  timestamp += m_TimestampOffset;
  LOG_DEBUG("Frame: " << FrameNumber << ". Mode: " << std::setw(4) << std::hex << usMode << ". Timestamp: " << timestamp);

  if(usMode & B && !m_PrimarySources.empty() // B-mode and primary source is defined
      || usMode & M_PostProcess && !m_ExtraSources.empty() // M-mode and extra source is defined
      || usMode & BFRFALineImage_SampleData && !m_PrimarySources.empty()  // B-mode and primary source is defined, if in RF/BRF mode
    )
  {
    assert(length == frameSize[0] * frameSize[1] * sizeof(uint16_t) + 16); //frame + header

    if(m_UseDeviceFrameReconstruction)
    {
      char* frameData = nullptr;
      int length = WPSaveImageToPointer(&frameData);
      assert(length == frameSize[0] * frameSize[1] * sizeof(uint32_t));
      auto* frameRGBA = reinterpret_cast<uint32_t*>(frameData);

      // all the color channels are the same for B-mode
      // and alpha is filled with ones (fully opaque)
      for(unsigned i = 0; i < frameSize[0] * frameSize[1]; i++)
      {
        m_PrimaryBuffer[i] = static_cast<uint8_t>(frameRGBA[i]);
      }
      WPFreePointer(frameData);
    }
    else
    {
      if(usMode & M_PostProcess)
      {
        this->ReconstructFrame(data, m_ExtraBuffer, frameSize);
        for(unsigned i = 0; i < m_ExtraSources.size(); i++)
        {
          frameSize[0] = m_MWidth;
          if(m_ExtraSources[i]->AddItem(&m_ExtraBuffer[0],
                                        US_IMG_ORIENT_MF,
                                        frameSize, VTK_UNSIGNED_CHAR,
                                        1, US_IMG_BRIGHTNESS, 0,
                                        this->FrameNumber,
                                        timestamp,
                                        timestamp, //no timestamp filtering needed
                                        &this->m_CustomFields) != PLUS_SUCCESS)
          {
            LOG_WARNING("Error adding item to extra video source " << m_ExtraSources[i]->GetSourceId());
          }
        }
      }
      else // B-mode
      {
        char* texture = nullptr;
        int slicePitch;
        int rowPitch;
        int tLength = WPDXGetFusedTexData(&texture, &slicePitch, &rowPitch);
        if (tLength != frameSize[0] * rowPitch)
        {
          LOG_ERROR("B Mode texture data does not match frame size");
        }
        if(tLength > 0)
        {
          this->FlipTexture(texture, frameSize, rowPitch);
        }
        else
        {
          this->ReconstructFrame(data, m_PrimaryBuffer, frameSize);
        }
        WPFreePointer(texture);

        for(unsigned i = 0; i < m_PrimarySources.size(); i++)
        {
          if(m_PrimarySources[i]->AddItem(&m_PrimaryBuffer[0],
                                          US_IMG_ORIENT_MF,
                                          frameSize, VTK_UNSIGNED_CHAR,
                                          1, US_IMG_BRIGHTNESS, 0,
                                          this->FrameNumber,
                                          timestamp,
                                          timestamp, //no timestamp filtering needed
                                          &this->m_CustomFields) != PLUS_SUCCESS)
          {
            LOG_WARNING("Error adding item to primary video source " << m_PrimarySources[i]->GetSourceId());
          }
        }
      } // B-mode
    } //m_UseDeviceFrameReconstruction
  }
  else if(usMode & B)  //this is B frame, but B-mode source is NOT defined
  {
    LOG_DEBUG("Frame ignored - B-mode source not defined. Got mode: " << std::hex << usMode);
    return;
  }
  else if(usMode & BFRFALineImage_RFData)
  {
    for(unsigned i = 0; i < m_ExtraSources.size(); i++)
    {
      assert(length == frameSize[0] * frameSize[1] * sizeof(int32_t));

      if(m_ExtraSources[i]->AddItem(data,
                                    US_IMG_ORIENT_FM,
                                    frameSize, VTK_INT,
                                    1, US_IMG_RF_REAL, 0,
                                    this->FrameNumber,
                                    timestamp,
                                    timestamp,
                                    &m_CustomFields) != PLUS_SUCCESS)
      {
        LOG_WARNING("Error adding item to RF video source " << m_ExtraSources[i]->GetSourceId());
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
void vtkPlusWinProbeVideoSource::AdjustBufferSizes()
{
  FrameSizeType frameSize = { m_LineCount, m_SamplesPerLine, 1 };

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
    m_PrimaryBuffer.resize(m_SamplesPerLine * m_LineCount);
  }

  for(unsigned i = 0; i < m_ExtraSources.size(); i++)
  {
    if(m_Mode == Mode::RF || m_Mode == Mode::BRF)
    {
      frameSize[0] = m_SamplesPerLine * m_SSDecimation;
      frameSize[1] = m_LineCount;
      m_ExtraSources[i]->SetPixelType(VTK_INT);
      m_ExtraSources[i]->SetImageType(US_IMG_RF_REAL);
      m_ExtraSources[i]->SetOutputImageOrientation(US_IMG_ORIENT_FM);
      m_ExtraSources[i]->SetInputImageOrientation(US_IMG_ORIENT_FM);
      m_ExtraBuffer.swap(std::vector<uint8_t>()); // deallocate the buffer
    }
    else if(m_Mode == Mode::M)
    {
      frameSize[0] = m_MWidth;
      m_ExtraSources[i]->SetPixelType(VTK_UNSIGNED_CHAR);
      m_ExtraSources[i]->SetImageType(US_IMG_BRIGHTNESS);
      m_ExtraSources[i]->SetOutputImageOrientation(US_IMG_ORIENT_MF);
      m_ExtraSources[i]->SetInputImageOrientation(US_IMG_ORIENT_MF);
      m_ExtraBuffer.resize(m_SamplesPerLine * m_MWidth);
    }

    m_ExtraSources[i]->Clear(); // clear current buffer content
    m_ExtraSources[i]->SetInputFrameSize(frameSize);
    LOG_INFO("SourceID: " << m_ExtraSources[i]->GetId() << ", "
             << "Frame size: " << frameSize[0] << "x" << frameSize[1]
             << ", pixel type: " << vtkImageScalarTypeNameMacro(m_ExtraSources[i]->GetPixelType())
             << ", buffer image orientation: "
             << igsioVideoFrame::GetStringFromUsImageOrientation(m_ExtraSources[i]->GetInputImageOrientation()));
  }
}

//----------------------------------------------------------------------------
void vtkPlusWinProbeVideoSource::AdjustSpacing(bool rf_mode)
{
  unsigned int numSpaceDimensions = 3;
  std::vector<double> spacing;
  if(rf_mode)
  {
    spacing = GetExtraSourceSpacing();
  }
  else
  {
    spacing = GetPrimarySourceSpacing();
  }

  for(unsigned int i = 0; i < numSpaceDimensions; ++i)
  {
    this->CurrentPixelSpacingMm[i] = spacing[i];
  }

  std::ostringstream spacingStream;

  for(unsigned int i = 0; i < numSpaceDimensions; ++i)
  {
    spacingStream << this->CurrentPixelSpacingMm[i];
    if(i != numSpaceDimensions - 1)
    {
      spacingStream << " ";
    }
  }
  this->m_CustomFields["ElementSpacing"].first = FRAMEFIELD_FORCE_SERVER_SEND;
  this->m_CustomFields["ElementSpacing"].second = spacingStream.str();
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

  AdjustSpacing(false);

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
  this->m_CustomFields["SamplingRate"].first = FRAMEFIELD_FORCE_SERVER_SEND;
  this->m_CustomFields["SamplingRate"].second = std::to_string(m_ADCfrequency);
  m_LineCount = GetSSElementCount();
  SetSCCompoundAngleCount(0);

  LOG_DEBUG("GetHandleBRFInternally: " << GetHandleBRFInternally());
  LOG_DEBUG("GetBFRFImageCaptureMode: " << GetBFRFImageCaptureMode());

  if(m_Mode == Mode::BRF || m_Mode == Mode::RF)
  {
    SetHandleBRFInternally(false);
    SetBFRFImageCaptureMode(2);
  }
  else
  {
    SetHandleBRFInternally(true);
    SetBFRFImageCaptureMode(0);
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

  //apply requested settings
  for(int i = 0; i < 8; i++)
  {
    SetTGC(i, m_TimeGainCompensation[i]);
    m_TimeGainCompensation[i] = GetTGC(i);
  }
  SetTGCFirstGainValue(m_FirstGainValue);
  //SetPendingTGCUpdate(true);
  for(int i = 0; i < 4; i++)
  {
    ::SetFocalPointDepth(i, m_FocalPointDepth[i]);
    m_FocalPointDepth[i] = ::GetFocalPointDepth(i);
  }

  this->Connected = true; // the setters and getters check this
  this->SetTransmitFrequencyMHz(m_Frequency);
  this->SetVoltage(m_Voltage);
  this->SetScanDepthMm(m_ScanDepth);
  // Update decimation variable on start, based on scan depth
  m_SSDecimation = ::GetSSDecimation();
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
    WPDXSetFusedTexBufferMax(1);
  }
  WPDXSetDrawTextLayer(false);
  WPDXSetDrawScalesAndBars(false);
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  if(m_Mode == Mode::M)
  {
    SetMIsEnabled(true);
    SetMIsRevolving(m_MRevolvingEnabled);
    SetMPRF(m_MPRF);
    SetMAcousticLineIndex(m_MLineIndex);
    ::SetMWidth(m_MWidth);
    ::SetMAcousticLineCount(m_MAcousticLineCount);
  }

  m_TimestampOffset = vtkIGSIOAccurateTimer::GetSystemTime();
  LOG_DEBUG("GetPendingRecreateTables: " << GetPendingRecreateTables());
  LOG_DEBUG("GetPendingRestartSequencer: " << GetPendingRestartSequencer());
  LOG_DEBUG("GetPendingRun30Frames: " << GetPendingRun30Frames());

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
PlusStatus vtkPlusWinProbeVideoSource::SetSSDecimation(uint8_t value)
{
  if(Connected)
  {
    ::SetSSDecimation(value);
    SetPendingRecreateTables(true);
    //what we requested might be only approximately satisfied
    m_SSDecimation = ::GetSSDecimation();
  }
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
uint8_t vtkPlusWinProbeVideoSource::GetSSDecimation()
{
  if(Connected)
  {
    m_SSDecimation = ::GetSSDecimation();
  }
  return m_SSDecimation;
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
    // Update decimation with scan depth
    m_SSDecimation = ::GetSSDecimation();
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
    SetPendingTGCUpdate(true);
    //what we requested might be only approximately satisfied
    m_TimeGainCompensation[index] = GetTGC(index);
  }
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
double vtkPlusWinProbeVideoSource::GetFirstGainValue()
{
  if(Connected)
  {
    m_FirstGainValue = GetTGCFirstGainValue();
  }
  return m_FirstGainValue;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusWinProbeVideoSource::SetFirstGainValue(double value)
{
  m_FirstGainValue = value;
  if(Connected)
  {
    SetTGCFirstGainValue(value);
    // SetPendingTGCUpdate(true);
    //what we requested might be only approximately satisfied
    m_FirstGainValue = GetTGCFirstGainValue();
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

void vtkPlusWinProbeVideoSource::SetBRFEnabled(bool value)
{
  if(Connected)
  {
    if(m_Mode == Mode::M)
    {
      SetMIsEnabled(false);
    }
    if(value)
    {
      SetHandleBRFInternally(false);
      SetBFRFImageCaptureMode(2);
    }
    else
    {
      SetHandleBRFInternally(true);
      SetBFRFImageCaptureMode(0);
    }
  }

  if(value)
  {
    m_Mode = Mode::BRF;
  }
  else
  {
    m_Mode = Mode::B;
  }
}

bool vtkPlusWinProbeVideoSource::GetBRFEnabled()
{
  bool brfEnabled = (m_Mode == Mode::BRF);
  if(Connected)
  {
    brfEnabled = (GetBFRFImageCaptureMode() == 2);
    if(brfEnabled)
    {
      m_Mode = Mode::BRF;
    }
  }
  return brfEnabled;
}

void vtkPlusWinProbeVideoSource::SetBHarmonicEnabled(bool value)
{
  if(Connected)
  {
    SetBIsHarmonic(value);
    SetPendingRecreateTables(true);
  }
  m_BHarmonicEnabled = GetBIsHarmonic();
}

bool vtkPlusWinProbeVideoSource::GetBHarmonicEnabled()
{
  if(Connected)
  {
    m_BHarmonicEnabled = GetBIsHarmonic();
  }
  return m_BHarmonicEnabled;
}

//----------------------------------------------------------------------------

void vtkPlusWinProbeVideoSource::SetMModeEnabled(bool value)
{
  if(Connected)
  {
    if(m_Mode == Mode::BRF)
    {
      SetBRFEnabled(false);
    }
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
  bool mmodeEnabled = (m_Mode == Mode::M);
  if(Connected)
  {
    mmodeEnabled = GetMIsEnabled();
    if(mmodeEnabled)
    {
      m_Mode = Mode::M;
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
    m_MWidth = mwidth;
  }
}

int vtkPlusWinProbeVideoSource::GetMWidth()
{
  int mwidthSeconds = 0;
  if(Connected)
  {
    m_MWidth = ::GetMWidth();
    mwidthSeconds = this->MSecondsFromWidth(m_MWidth);
  }
  return mwidthSeconds;
}

void vtkPlusWinProbeVideoSource::SetMWidthLines(int32_t value)
{
  if(Connected)
  {
    ::SetMWidth(value);
    SetPendingRecreateTables(true);
  }
  m_MWidth = value;
}

int32_t vtkPlusWinProbeVideoSource::GetMWidthLines()
{
  if(Connected)
  {
    m_MWidth = ::GetMWidth();
  }
  return m_MWidth;
}

void vtkPlusWinProbeVideoSource::SetMAcousticLineCount(int32_t value)
{
  if(Connected)
  {
    ::SetMAcousticLineCount(value);
    SetPendingRecreateTables(true);
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

int vtkPlusWinProbeVideoSource::GetTransducerInternalID()
{
  int transducer_id = ::GetTransducerInternalID();
  return transducer_id;
}
//----------------------------------------------------------------------------
std::vector<double> vtkPlusWinProbeVideoSource::GetPrimarySourceSpacing()
{
  std::vector<double> spacing = { 0.0, 0.0, 1.0};
  if(!m_PrimarySources.empty())
  {
    spacing[0] = this->GetTransducerWidthMm() / (m_LineCount - 1);
    spacing[1] = m_ScanDepth / (m_SamplesPerLine - 1);
  }
  return spacing;
}

std::vector<double> vtkPlusWinProbeVideoSource::GetExtraSourceSpacing()
{
  std::vector<double> spacing = { 0.0, 0.0, 1.0};
  if(!m_ExtraSources.empty())
  {
    if(GetBRFEnabled())
    {
      spacing[0] = m_ScanDepth / (m_SamplesPerLine * m_SSDecimation - 1);
      spacing[1] = this->GetTransducerWidthMm() / (m_LineCount - 1);
    }
    else
    {
      for(int i = 0; i < 3; i++)
      {
        spacing[i] = GetPrimarySourceSpacing()[i];
      }
    }
  }
  return spacing;
}

std::string vtkPlusWinProbeVideoSource::GetTransducerID()
{
  return this->m_TransducerID;
}

void vtkPlusWinProbeVideoSource::SetBFrameRateLimit(int32_t value)
{
  if (Connected)
  {
    ::SetBFrameRateLimit(value);
  }
  m_BFrameRateLimit = value;
}

int32_t vtkPlusWinProbeVideoSource::GetBFrameRateLimit()
{
  if (Connected)
  {
    m_BFrameRateLimit = ::GetBFrameRateLimit();
  }
  return m_BFrameRateLimit;
}
