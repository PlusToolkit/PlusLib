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

  //----------------------------------------------------------------------------
  // Define command strings
const char* vtkPlusWinProbeVideoSource::SET_FREEZE                   = "SetFrozen";
const char* vtkPlusWinProbeVideoSource::GET_FREEZE                   = "GetFrozen";
const char* vtkPlusWinProbeVideoSource::SET_TGC                      = "SetTGC";
const char* vtkPlusWinProbeVideoSource::GET_TGC                      = "GetTGC";
const char* vtkPlusWinProbeVideoSource::SET_ALL_FOCAL_DEPTHS         = "SetAllFocalDepths";
const char* vtkPlusWinProbeVideoSource::SET_FOCAL_DEPTH              = "SetFocalDepth";
const char* vtkPlusWinProbeVideoSource::GET_FOCAL_DEPTH              = "GetFocalDepth";
const char* vtkPlusWinProbeVideoSource::SET_ALL_ARFI_FOCAL_DEPTHS    = "SetAllARFIFocalDepths";
const char* vtkPlusWinProbeVideoSource::SET_ARFI_FOCAL_DEPTH         = "SetARFIFocalDepth";
const char* vtkPlusWinProbeVideoSource::GET_ARFI_FOCAL_DEPTH         = "GetARFIFocalDepth";
const char* vtkPlusWinProbeVideoSource::SET_B_MULTIFOCAL_ZONE_COUNT  = "SetBMultifocalZoneCount";
const char* vtkPlusWinProbeVideoSource::GET_B_MULTIFOCAL_ZONE_COUNT  = "GetBMultifocalZoneCount";
const char* vtkPlusWinProbeVideoSource::SET_FIRST_GAIN_VALUE         = "SetFirstGainValue";
const char* vtkPlusWinProbeVideoSource::GET_FIRST_GAIN_VALUE         = "GetFirstGainValue";
const char* vtkPlusWinProbeVideoSource::SET_TGC_OVERALL_GAIN         = "SetTGCOverallGain";
const char* vtkPlusWinProbeVideoSource::GET_TGC_OVERALL_GAIN         = "GetTGCOverallGain";
const char* vtkPlusWinProbeVideoSource::SET_SPATIAL_COMPOUND_ENABLED = "SetSpatialCompoundEnabled";
const char* vtkPlusWinProbeVideoSource::GET_SPATIAL_COMPOUND_ENABLED = "GetSpatialCompoundEnabled";
const char* vtkPlusWinProbeVideoSource::GET_SPATIAL_COMPOUND_ANGLE   = "GetSpatialCompoundAngle";
const char* vtkPlusWinProbeVideoSource::SET_SPATIAL_COMPOUND_COUNT   = "SetSpatialCompoundCount";
const char* vtkPlusWinProbeVideoSource::GET_SPATIAL_COMPOUND_COUNT   = "GetSpatialCompoundCount";
const char* vtkPlusWinProbeVideoSource::SET_MMODE_ENABLED            = "SetMModeEnabled";
const char* vtkPlusWinProbeVideoSource::GET_MMODE_ENABLED            = "GetMModeEnabled";
const char* vtkPlusWinProbeVideoSource::SET_M_REVOLVING_ENABLED      = "SetMRevolvingEnabled";
const char* vtkPlusWinProbeVideoSource::GET_M_REVOLVING_ENABLED      = "GetMRevolvingEnabled";
const char* vtkPlusWinProbeVideoSource::SET_RF_MODE_ENABLED          = "SetRfModeEnabled";
const char* vtkPlusWinProbeVideoSource::GET_RF_MODE_ENABLED          = "GetRfModeEnabled";
const char* vtkPlusWinProbeVideoSource::SET_MPR_FREQUENCY            = "SetMPRFrequency";
const char* vtkPlusWinProbeVideoSource::GET_MPR_FREQUENCY            = "GetMPRFrequency";
const char* vtkPlusWinProbeVideoSource::SET_M_LINE_INDEX             = "SetMLineIndex";
const char* vtkPlusWinProbeVideoSource::GET_M_LINE_INDEX             = "GetMLineIndex";
const char* vtkPlusWinProbeVideoSource::SET_M_LINE_COUNT             = "SetMLineCount";
const char* vtkPlusWinProbeVideoSource::GET_M_LINE_COUNT             = "GetMLineCount";
const char* vtkPlusWinProbeVideoSource::SET_M_WIDTH                  = "SetMWidth";
const char* vtkPlusWinProbeVideoSource::GET_M_WIDTH                  = "GetMWidth";
const char* vtkPlusWinProbeVideoSource::SET_M_DEPTH                  = "SetMDepth";
const char* vtkPlusWinProbeVideoSource::GET_M_DEPTH                  = "GetMDepth";
const char* vtkPlusWinProbeVideoSource::SET_DECIMATION               = "SetDecimation";
const char* vtkPlusWinProbeVideoSource::GET_DECIMATION               = "GetDecimation";
const char* vtkPlusWinProbeVideoSource::GET_B_PRF                    = "GetBPRF";
const char* vtkPlusWinProbeVideoSource::SET_B_FRAME_RATE_LIMIT       = "SetBFrameRateLimit";
const char* vtkPlusWinProbeVideoSource::GET_B_FRAME_RATE_LIMIT       = "GetBFrameRateLimit";
const char* vtkPlusWinProbeVideoSource::SET_B_HARMONIC_ENABLED       = "SetBHarmonicEnabled";
const char* vtkPlusWinProbeVideoSource::GET_B_HARMONIC_ENABLED       = "GetBHarmonicEnabled";
const char* vtkPlusWinProbeVideoSource::SET_B_BUBBLE_CONTRAST_ENABLED = "SetBBubbleContrastEnabled";
const char* vtkPlusWinProbeVideoSource::GET_B_BUBBLE_CONTRAST_ENABLED = "GetBBubbleContrastEnabled";
const char* vtkPlusWinProbeVideoSource::SET_B_AMPLITUDE_MODULATION_ENABLED = "SetBAmplitudeModulationEnabled";
const char* vtkPlusWinProbeVideoSource::GET_B_AMPLITUDE_MODULATION_ENABLED = "GetBAmplitudeModulationEnabled";
const char* vtkPlusWinProbeVideoSource::SET_B_TRANSMIT_LOCKED        = "SetBTransmitLocked";
const char* vtkPlusWinProbeVideoSource::GET_B_TRANSMIT_LOCKED        = "GetBTransmitLocked";
const char* vtkPlusWinProbeVideoSource::SET_B_TRANSMIT_CURRENT       = "SetBTransmitCurrent";
const char* vtkPlusWinProbeVideoSource::GET_B_TRANSMIT_CURRENT       = "GetBTransmitCurrent";
const char* vtkPlusWinProbeVideoSource::SET_B_TRANSMIT_CYCLE_COUNT   = "SetBTransmitCycleCount";
const char* vtkPlusWinProbeVideoSource::GET_B_TRANSMIT_CYCLE_COUNT   = "GetBTransmitCycleCount";
const char* vtkPlusWinProbeVideoSource::SET_B_TRANSMIT_FNUMBER       = "SetBTransmitFNumber";
const char* vtkPlusWinProbeVideoSource::GET_B_TRANSMIT_FNUMBER       = "GetBTransmitFNumber";
const char* vtkPlusWinProbeVideoSource::SET_B_APODIZATION_FNUMBER        = "SetBApodizationFNumber";
const char* vtkPlusWinProbeVideoSource::GET_B_APODIZATION_FNUMBER        = "GetBApodizationFNumber";
const char* vtkPlusWinProbeVideoSource::SET_B_BUBBLE_DESTRUCTION_ENABLED = "SetBBubbleDestructionEnabled";
const char* vtkPlusWinProbeVideoSource::GET_B_BUBBLE_DESTRUCTION_ENABLED = "GetBBubbleDestructionEnabled";
const char* vtkPlusWinProbeVideoSource::SET_B_BUBBLE_DESTRUCTION_CYCLE_COUNT = "SetBBubbleDestructionCycleCount";
const char* vtkPlusWinProbeVideoSource::GET_B_BUBBLE_DESTRUCTION_CYCLE_COUNT = "GetBBubbleDestructionCycleCount";
const char* vtkPlusWinProbeVideoSource::SET_B_TX_FILTER_COEFFICIENT_SET = "SetBTXFilterCoefficientSet";
const char* vtkPlusWinProbeVideoSource::GET_B_TX_FILTER_COEFFICIENT_SET = "GetBTXFilterCoefficientSet";
const char* vtkPlusWinProbeVideoSource::GET_TRANSDUCER_INTERNAL_ID   = "GetTransducerInternalID";
const char* vtkPlusWinProbeVideoSource::SET_ARFI_ENABLED             = "SetARFIEnabled";
const char* vtkPlusWinProbeVideoSource::GET_ARFI_ENABLED             = "GetARFIEnabled";
const char* vtkPlusWinProbeVideoSource::SET_ARFI_START_SAMPLE        = "SetARFIStartSample";
const char* vtkPlusWinProbeVideoSource::GET_ARFI_START_SAMPLE        = "GetARFIStartSample";
const char* vtkPlusWinProbeVideoSource::SET_ARFI_STOP_SAMPLE         = "SetARFIStopSample";
const char* vtkPlusWinProbeVideoSource::GET_ARFI_STOP_SAMPLE         = "GetARFIStopSample";
const char* vtkPlusWinProbeVideoSource::SET_ARFI_PRE_PUSH_LINE_REPEAT_COUNT  = "SetARFIPrePushLineRepeatCount";
const char* vtkPlusWinProbeVideoSource::GET_ARFI_PRE_PUSH_LINE_REPEAT_COUNT  = "GetARFIPrePushLineRepeatCount";
const char* vtkPlusWinProbeVideoSource::SET_ARFI_POST_PUSH_LINE_REPEAT_COUNT = "SetARFIPostPushLineRepeatCount";
const char* vtkPlusWinProbeVideoSource::GET_ARFI_POST_PUSH_LINE_REPEAT_COUNT = "GetARFIPostPushLineRepeatCount";
const char* vtkPlusWinProbeVideoSource::GET_ARFI_INTER_SET_DELAY     = "GetARFIInterSetDelay";
const char* vtkPlusWinProbeVideoSource::SET_ARFI_INTER_SET_DELAY     = "SetARFIInterSetDelay";
const char* vtkPlusWinProbeVideoSource::GET_ARFI_INTER_PUSH_DELAY    = "GetARFIInterPushDelay";
const char* vtkPlusWinProbeVideoSource::SET_ARFI_INTER_PUSH_DELAY    = "SetARFIInterPushDelay";
const char* vtkPlusWinProbeVideoSource::SET_ARFI_LINE_TIMER          = "SetARFILineTimer";
const char* vtkPlusWinProbeVideoSource::GET_ARFI_LINE_TIMER          = "GetARFILineTimer";
const char* vtkPlusWinProbeVideoSource::SET_ARFI_TX_CYCLE_COUNT      = "SetARFITxCycleCount";
const char* vtkPlusWinProbeVideoSource::GET_ARFI_TX_CYCLE_COUNT      = "GetARFITxCycleCount";
const char* vtkPlusWinProbeVideoSource::SET_ARFI_TX_CYCLE_WIDTH      = "SetARFITxCycleWidth";
const char* vtkPlusWinProbeVideoSource::GET_ARFI_TX_CYCLE_WIDTH      = "GetARFITxCycleWidth";
const char* vtkPlusWinProbeVideoSource::SET_ARFI_TX_TX_CYCLE_COUNT   = "SetARFITxTxCycleCount";
const char* vtkPlusWinProbeVideoSource::GET_ARFI_TX_TX_CYCLE_COUNT   = "GetARFITxTxCycleCount";
const char* vtkPlusWinProbeVideoSource::SET_ARFI_TX_TX_CYCLE_WIDTH   = "SetARFITxTxCycleWidth";
const char* vtkPlusWinProbeVideoSource::GET_ARFI_TX_TX_CYCLE_WIDTH   = "GetARFITxTxCycleWidth";
const char* vtkPlusWinProbeVideoSource::SET_ARFI_PUSH_CONFIG         = "SetARFIPushConfigurationString";
const char* vtkPlusWinProbeVideoSource::GET_ARFI_PUSH_CONFIG         = "GetARFIPushConfigurationString";
const char* vtkPlusWinProbeVideoSource::GET_FPGA_REV_DATE_STRING     = "GetFPGARevDateString";
const char* vtkPlusWinProbeVideoSource::GET_X8BF_ENABLED             = "IsX8BFEnabled";
const char* vtkPlusWinProbeVideoSource::UV_SEND_COMMAND              = "UVSendCommand";
const char* vtkPlusWinProbeVideoSource::IS_SCANNING                  = "IsScanning";

int32_t focalCountFromDepthsArray(float* depths, unsigned arraySize)
{
  std::vector<float> nonZeroes;
  for (unsigned i = 0; i < arraySize; i++)
  {
    if (depths[i] != 0)
    {
      nonZeroes.push_back(depths[i]);
    }
  }
  std::sort(nonZeroes.begin(), nonZeroes.end());

  unsigned i = 1;
  while (i < nonZeroes.size())
  {
    if (nonZeroes[i] == nonZeroes[i - 1])
    {
      nonZeroes.erase(nonZeroes.begin() + i); // remove duplicate
    }
    else // check next index
    {
      ++i;
    }
  }
  if (nonZeroes.size() == 0)
  {
    nonZeroes.push_back(depths[0]);  // Need to specify at least one focal depth even if all depths are 0 mm
  }
  int32_t count = nonZeroes.size();

  // copy sorted non-zero array back into original array
  for (i = 0; i < nonZeroes.size(); i++)
  {
    depths[i] = nonZeroes[i];
  }
  // fill rest of the array with zeroes
  for (; i < arraySize; i++)
  {
    depths[i] = 0;
  }

  return count;
}

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
  for(int i = 0; i < 6; i++)
  {
    os << indent << "ARFIFocalPointDepth" << i << ": " << m_ARFIFocalPointDepth[i] << std::endl;
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
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(double, OverallTimeGainCompensation, deviceConfig);
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

  m_BMultiTxCount = deviceConfig->GetVectorAttribute("FocalPointDepth", 4, m_FocalPointDepth);
  if (m_BMultiTxCount) // examine for duplicates
  {
    m_BMultiTxCount = focalCountFromDepthsArray(m_FocalPointDepth, m_BMultiTxCount);
  }
  deviceConfig->GetVectorAttribute("ARFIFocalPointDepth", 6, m_ARFIFocalPointDepth);
  // m_ARFIMultiTxCount = deviceConfig->GetVectorAttribute("ARFIFocalPointDepth", 6, m_ARFIFocalPointDepth);
  // if (m_ARFIMultiTxCount) // examine for duplicates
  // {
	// m_ARFIMultiTxCount = focalCountFromDepthsArray(&m_ARFIFocalPointDepth[1], m_ARFIMultiTxCount - 1); // first value is special
  // }

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
  deviceConfig->SetIntAttribute("BTransmitCurrent", this->GetBTransmitCurrent());
  deviceConfig->SetIntAttribute("BTransmitCycleCount", this->GetBTransmitCycleCount());
  deviceConfig->SetUnsignedLongAttribute("Voltage", this->GetVoltage());
  deviceConfig->SetUnsignedLongAttribute("MinValue", this->GetMinValue());
  deviceConfig->SetUnsignedLongAttribute("MaxValue", this->GetMaxValue());
  deviceConfig->SetUnsignedLongAttribute("LogLinearKnee", this->GetLogLinearKnee());
  deviceConfig->SetUnsignedLongAttribute("LogMax", this->GetLogMax());
  deviceConfig->SetUnsignedLongAttribute("SSDecimation", this->GetSSDecimation());
  deviceConfig->SetAttribute("Mode", ModeToString(this->m_Mode).c_str());
  deviceConfig->SetDoubleAttribute("FirstGainValue", this->GetFirstGainValue());
  deviceConfig->SetDoubleAttribute("OverallTimeGainCompensation", this->GetOverallTimeGainCompensation());
  deviceConfig->SetDoubleAttribute("BTransmitFNumber", this->GetBTransmitFNumber());
  deviceConfig->SetIntAttribute("BFrameRateLimit", this->GetBFrameRateLimit());

  deviceConfig->SetVectorAttribute("TimeGainCompensation", 8, m_TimeGainCompensation);
  deviceConfig->SetVectorAttribute("FocalPointDepth", 4, m_FocalPointDepth);
  deviceConfig->SetVectorAttribute("ARFIFocalPointDepth", 6, m_ARFIFocalPointDepth);

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
  else if(modeString == "ARFI")
  { return Mode::ARFI; }
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
  case Mode::ARFI:
    return "ARFI";
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
int32_t quadBFCount = 1; // number of 4x beamformers

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
  ARFIGeometryStruct* arfiGeometry = (ARFIGeometryStruct*)hGeometry;
  int callbackFrameNumber = header->TotalFrameCounter;
  InputSourceBindings usMode = header->InputSourceBinding;
  FrameSizeType frameSize = { 1, 1, 1 };

  if(header->TotalFrameCounter == 0)
  {
    first_timestamp = header->TimeStamp / 1000.0;
    m_TimestampOffset = vtkIGSIOAccurateTimer::GetSystemTime();
    LOG_DEBUG("First frame timestamp: "<< first_timestamp);
  }

  if(usMode & CFD)
  {
    frameSize[0] = cfdGeometry->LineCount;
    frameSize[1] = cfdGeometry->SamplesPerKernel;
  }
  else if(usMode & B || usMode & BFRFALineImage_SampleData)
  {
    if (m_UseDeviceFrameReconstruction)
    {
      return;
    }
    frameSize[0] = brfGeometry->LineCount;
    frameSize[1] = brfGeometry->SamplesPerLine;
    if(frameSize[1] != m_PrimaryFrameSize[1])
    {
      LOG_INFO("SamplesPerLine has changed from " << m_PrimaryFrameSize[1]
               << " to " << frameSize[1] << ". Adjusting spacing and buffer sizes.");
      m_PrimaryFrameSize[1] = frameSize[1];
      AdjustBufferSizes();
      AdjustSpacing(false);
    }
    else if(this->CurrentPixelSpacingMm[1] != m_ScanDepth / (m_PrimaryFrameSize[1] - 1)) // we might need approximate equality check
    {
      LOG_INFO("Scan Depth changed. Adjusting spacing.");
      AdjustSpacing(false);
    }
  }
  else if(usMode & ARFI)
  {
    int timestampsPerLineRepeat = (4 / quadBFCount);
    int lineRepeatCount = arfiGeometry->PrePushLineRepeatCount + arfiGeometry->PostPushLineRepeatCount;
    int timeblock = timestampsPerLineRepeat * lineRepeatCount * m_ARFIPushConfigurationCount * sizeof(int32_t);
    int arfiDataSize = (m_ARFIStopSample - m_ARFIStartSample) * arfiGeometry->LineCount * lineRepeatCount * m_ARFIPushConfigurationCount * sizeof(int32_t);
    assert(length == arfiDataSize + timeblock);
    frameSize[0] = (arfiDataSize + timeblock) / sizeof(int32_t);  // we want to include all data to be saved
    frameSize[1] = 1;
    frameSize[2] = 1;
    if(frameSize != m_ExtraFrameSize)
    {
      LOG_INFO("ARFI frame size updated. Adjusting buffer size and spacing.");
      m_ExtraFrameSize = frameSize;
      AdjustBufferSizes();
      AdjustSpacing(true);
    }
    else if(this->CurrentPixelSpacingMm[0] != m_ScanDepth / (m_ExtraFrameSize[1] * 1)) // we might need approximate equality check
    {
      LOG_INFO("Scan Depth changed. Adjusting spacing.");
      AdjustSpacing(true);
    }
  }
  else if(usMode & BFRFALineImage_RFData)
  {
    int harmonic_multiplier = 1;  // RF data contains more lines when the harmonic flags are turned on because it includes data for each transmit
    if (m_BHarmonicEnabled & (m_BBubbleContrastEnabled || m_BAmplitudeModulationEnabled))
    {
      harmonic_multiplier = 3;
    }
    else if (m_BHarmonicEnabled & !m_BBubbleContrastEnabled & !m_BAmplitudeModulationEnabled)
    {
      harmonic_multiplier = 2;
    }

    frameSize[0] = brfGeometry->SamplesPerLine * brfGeometry->Decimation;
    frameSize[1] = brfGeometry->LineCount * harmonic_multiplier;
    if(frameSize != m_ExtraSources[0]->GetInputFrameSize())
    {
      LOG_INFO("Rf frame size updated. Adjusting buffer size and spacing.");
      m_ExtraFrameSize[0] = frameSize[0];
      m_ExtraFrameSize[1] = frameSize[1];
      m_SSDecimation = brfGeometry->Decimation;
      AdjustBufferSizes();
      AdjustSpacing(true);
    }
    else if(this->CurrentPixelSpacingMm[0] != m_ScanDepth / (m_ExtraFrameSize[1] * m_SSDecimation - 1)) // we might need approximate equality check
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
      LOG_INFO("SamplesPerLine has changed from " << m_ExtraFrameSize[0] << "x" << m_ExtraFrameSize[1]
          << " to " << frameSize[0] << "x" << frameSize[1] << ". Adjusting buffer size.");
      m_ExtraFrameSize = frameSize;
      AdjustBufferSizes();
      AdjustSpacing(false);
    }
    else if(this->CurrentPixelSpacingMm[1] != m_ScanDepth / (m_ExtraFrameSize[1] - 1)) // we might need approximate equality check
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
  //timestamp counters are in milliseconds since last sequencer restart
  double timestamp = (header->TimeStamp / 1000.0) - first_timestamp;
  if(timestamp <= 0.0) // some change is being applied, so this frame is not valid
  {
    LOG_DEBUG("Timestamp is <= 0 so ignoring this frame.");
    return; // ignore this frame
  }
  timestamp += m_TimestampOffset;
  LOG_DEBUG("Frame: " << callbackFrameNumber << ". Mode: " << std::setw(4) << std::hex << usMode << ". Timestamp: " << timestamp << ". UseDeviceFrameReconstruction: " << m_UseDeviceFrameReconstruction);

  if(usMode & B && !m_PrimarySources.empty() // B-mode and primary source is defined
      || usMode & M_PostProcess && !m_ExtraSources.empty() // M-mode and extra source is defined
      || usMode & BFRFALineImage_SampleData && !m_PrimarySources.empty()  // B-mode and primary source is defined, if in RF/BRF mode
    )
  {
    assert(length == frameSize[0] * frameSize[1] * sizeof(uint16_t) + 16); //frame + header

    if(usMode & M_PostProcess)
    {
      this->ReconstructFrame(data, m_ExtraBuffer, frameSize);
      for(unsigned i = 0; i < m_ExtraSources.size(); i++)
      {
        frameSize[0] = m_ExtraFrameSize[0];
        if(m_ExtraSources[i]->AddItem(&m_ExtraBuffer[0],
                                      US_IMG_ORIENT_MF,
                                      frameSize, VTK_UNSIGNED_CHAR,
                                      1, US_IMG_BRIGHTNESS, 0,
                                      callbackFrameNumber,
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
      if (m_UseDeviceFrameReconstruction)
      {
        return;
      }
      this->ReconstructFrame(data, m_PrimaryBuffer, frameSize);

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
  }
  else if(usMode & B)  //this is B frame, but B-mode source is NOT defined
  {
    LOG_DEBUG("Frame ignored - B-mode source not defined. Got mode: " << std::hex << usMode);
    return;
  }
  else if(usMode & ARFI)
  {
    for(unsigned i = 0; i < m_ExtraSources.size(); i++)
    {
      // send the ARFI data as one long 2D frame to be processed downstream
      int32_t* tempData = reinterpret_cast<int32_t*>(data);
      // need to spoof the timestamps since the arfi data comes a few seconds after the push
      double currentTime = vtkIGSIOAccurateTimer::GetSystemTime();
      if(m_ExtraSources[i]->AddItem(tempData,
                                    US_IMG_ORIENT_FM,
                                    frameSize, VTK_INT,
                                    1, US_IMG_RF_REAL, 0,
                                    callbackFrameNumber,
                                    currentTime,
                                    currentTime,
                                    &m_CustomFields) != PLUS_SUCCESS)
      {
        LOG_WARNING("Error adding item to ARFI video source " << m_ExtraSources[i]->GetSourceId());
      }
      else
      {
        LOG_INFO("Success adding item to ARFI video source " << m_ExtraSources[i]->GetSourceId());
      }
    }
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
  FrameSizeType frameSize = m_PrimaryFrameSize;

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
             << igsioCommon::GetStringFromUsImageOrientation(m_PrimarySources[i]->GetInputImageOrientation()));
    m_PrimaryBuffer.resize(m_PrimaryFrameSize[1] * m_PrimaryFrameSize[0]);
  }

  frameSize = m_ExtraFrameSize;

  for(unsigned i = 0; i < m_ExtraSources.size(); i++)
  {
    if(m_Mode == Mode::RF || m_Mode == Mode::BRF || m_Mode == Mode::ARFI)
    {
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
      if(m_ExtraBuffer.size() != m_MWidth * m_ExtraFrameSize[1])
      {
        m_ExtraBuffer.resize(m_MWidth * m_ExtraFrameSize[1]);
        std::fill(m_ExtraBuffer.begin(), m_ExtraBuffer.end(), 0);
      }
    }

    if(m_ExtraSources[i]->GetInputFrameSize() != frameSize)
    {
      m_ExtraSources[i]->Clear(); // clear current buffer content
      m_ExtraSources[i]->SetInputFrameSize(frameSize);
      LOG_INFO("SourceID: " << m_ExtraSources[i]->GetId() << ", "
               << "Frame size: " << frameSize[0] << "x" << frameSize[1] << "x" << frameSize[2]
               << ", pixel type: " << vtkImageScalarTypeNameMacro(m_ExtraSources[i]->GetPixelType())
               << ", buffer image orientation: "
               << igsioCommon::GetStringFromUsImageOrientation(m_ExtraSources[i]->GetInputImageOrientation()));
    }
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

  if (m_UseDeviceFrameReconstruction)
  {
    // Device reconstructed frames are polled instead of sent via callback
    this->StartThreadForInternalUpdates = true;
  }
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
  WPInitialize();
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
  if(this->GetTransducerInternalID() == 15) // 15 corresponds to the "No transducer" entry in the Transducers.xml
  {
    LOG_ERROR("Transducer not connected!");
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
  if(::GetARFIIsX8BFEnabled())
  {
    quadBFCount = 2;
  }
  else
  {
    quadBFCount = 1;
  }
  m_FPGAVersion = GetFPGARevDateString();
  LOG_INFO("FPGA Version: " << m_FPGAVersion);

  m_ADCfrequency = GetADCSamplingRate();
  this->m_CustomFields["SamplingRate"].first = FRAMEFIELD_FORCE_SERVER_SEND;
  this->m_CustomFields["SamplingRate"].second = std::to_string(m_ADCfrequency);
  m_PrimaryFrameSize[0] = GetSSElementCount();
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
  if(m_Mode == Mode::RF || m_Mode == Mode::BRF || m_Mode == Mode::ARFI)
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
  if(m_Mode == Mode::ARFI)
  {
    this->SetARFIEnabled(true);
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
  this->Connected = true; // the setters and getters check this

  //apply requested settings
  for(int i = 0; i < 8; i++)
  {
    SetTGC(i, m_TimeGainCompensation[i]);
    m_TimeGainCompensation[i] = GetTGC(i);
  }
  SetTGCFirstGainValue(m_FirstGainValue);
  //SetPendingTGCUpdate(true);
  SetBMultiFocalZoneCount(m_BMultiTxCount);
  for(int i = 0; i < 4; i++)
  {
    ::SetFocalPointDepth(i, m_FocalPointDepth[i]);
    m_FocalPointDepth[i] = ::GetFocalPointDepth(i);
  }

  if (quadBFCount == 2)
  {
    for(int i = 0; i < 6; i++)
    {
      SetARFIFocalPointDepth(i, m_ARFIFocalPointDepth[i]);
      m_ARFIFocalPointDepth[i] = GetARFIFocalPointDepth(i);
    }

    SetARFITxTxCycleCount(m_ARFITxTxCycleCount);
    SetARFITxTxCycleWidth(m_ARFITxTxCycleWidth);
    SetARFITxCycleCount(m_ARFITxCycleCount);
    SetARFITxCycleWidth(m_ARFITxCycleWidth);
    SetARFIInterSetDelay(m_ARFIInterSetDelay);
    SetARFIInterPushDelay(m_ARFIInterPushDelay);
    SetARFILineTimer(m_ARFILineTimer);
    SetARFIStartSample(m_ARFIStartSample);
    SetARFIStopSample(m_ARFIStopSample);
    SetARFIPrePushLineRepeatCount(m_ARFIPrePushLineRepeatCount);
    SetARFIPostPushLineRepeatCount(m_ARFIPostPushLineRepeatCount);
    SetARFIPushConfigurationString(m_ARFIPushConfigurationString);
  }

  this->SetTransmitFrequencyMHz(m_Frequency);
  this->SetVoltage(m_Voltage);
  this->SetScanDepthMm(m_ScanDepth);
  // Update decimation variable on start, based on scan depth
  m_SSDecimation = ::GetSSDecimation();
  this->SetSpatialCompoundEnabled(m_SpatialCompoundEnabled); // also takes care of angle  and count
  this->SetBHarmonicEnabled(m_BHarmonicEnabled);
  this->SetBBubbleContrastEnabled(m_BBubbleContrastEnabled);
  this->SetBAmplitudeModulationEnabled(m_BAmplitudeModulationEnabled);
  this->SetBTransmitLocked(m_BTransmitLocked);
  this->SetBTransmitCurrent(m_BTransmitCurrent);
  this->SetBTransmitCycleCount(m_BTransmitCycleCount);
  this->SetBTransmitFNumber(m_BTransmitFNumber);
  this->SetBApodizationFNumber(m_BApodizationFNumber);
  this->SetBBubbleDestructionEnabled(m_BBubbleDestructionEnabled);
  this->SetBBubbleDestructionCycleCount(m_BBubbleDestructionCycleCount);

  //setup size for DirectX image
  LOG_DEBUG("Setting output size to " << m_PrimaryFrameSize[0] << "x" << m_PrimaryFrameSize[1]);
  char* sessionPtr = GetSessionPtr();
  bool success = WPVPSetSession(sessionPtr);
  if(!success)
  {
    LOG_WARNING("Failed setting session pointer!");
    WPDisconnect();
    return PLUS_FAIL;
  }
  WPSetSize(m_PrimaryFrameSize[0], m_PrimaryFrameSize[1]);
  SetMaxDmaTransferSize(0x100000);

  if(m_UseDeviceFrameReconstruction)
  {
    WPDXSetIsGetSpatialCompoundedTexEnabled(true);
    WPDXSetFusedTexBufferMax(16);
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
  WPDXDispose();
  WPDispose();
  return PLUS_SUCCESS;
}

// ----------------------------------------------------------------------------
PlusStatus vtkPlusWinProbeVideoSource::InternalStartRecording()
{
  this->FrameNumber = 0;
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
PlusStatus vtkPlusWinProbeVideoSource::InternalUpdate()
{
  if (!m_UseDeviceFrameReconstruction || !this->Connected)
  {
    return PLUS_SUCCESS;
  }

  // Handle manual probe disconnections
  if (GetTransducerInternalID() == 0xF)  // probe has been disconnected from the engine
  {
    if (IsScanning())
    {
      WPStopScanning();
    }
    LOG_WARNING("Probe has been disconnected from the engine!");
    return PLUS_SUCCESS;
  }
  else
  {
    if (!IsScanning())
    {
      WPExecute();
    }
  }

  if (!m_PrimarySources.empty())  // mode shouldn't matter here, we are always polling B-mode into primary
  {
    // Grab processed frame data
    char* texture = nullptr;
    int slicePitch;
    int rowPitch;
    int tLength = WPDXGetFusedTexData(&texture, &slicePitch, &rowPitch);
    if (tLength == 0)
    {
      LOG_DEBUG("B Mode buffer empty");
      return PLUS_FAIL;
    }

    // Adjust buffer sizes
    FrameSizeType frameSize = { 1, 1, 1 };
    frameSize[0] = tLength / rowPitch;
    frameSize[1] = rowPitch;
    if(frameSize[1] != m_PrimaryFrameSize[1])
    {
      LOG_INFO("SamplesPerLine has changed from " << m_PrimaryFrameSize[1]
               << " to " << frameSize[1] << ". Adjusting spacing and buffer sizes.");
      m_PrimaryFrameSize[1] = frameSize[1];
      AdjustBufferSizes();
      AdjustSpacing(false);
    }
    else if(this->CurrentPixelSpacingMm[1] != m_ScanDepth / (m_PrimaryFrameSize[1] - 1)) // we might need approximate equality check
    {
      LOG_INFO("Scan Depth changed. Adjusting spacing.");
      AdjustSpacing(false);
    }

    // True post-processing timestamp is packed into the data
    double timestamp = 0;
    unsigned int temp = 0;
    temp = (texture[0] & 0xff);
    timestamp += temp;
    temp = (texture[1] & 0xff);
    timestamp += (temp << 8);
    temp = (texture[2] & 0xff);
    timestamp += (temp << 16);
    temp = (texture[3] & 0xff);
    timestamp += (temp << 24);

    //timestamp counters are in milliseconds since last sequencer restart
    timestamp = (timestamp / 1000) - first_timestamp;
    if(timestamp <= 0.0) // some change is being applied, so this frame is not valid
    {
      m_RenderedTimestampOffset = vtkIGSIOAccurateTimer::GetSystemTime();
      LOG_DEBUG("Timestamp is <= 0 so ignoring this frame.");
      this->FrameNumber += 1;
      WPFreePointer(texture);
      return PLUS_SUCCESS; // ignore this frame
    }

    double currentTime = m_RenderedTimestampOffset + timestamp;
    this->FlipTexture(texture, frameSize, rowPitch);
    for(unsigned i = 0; i < m_PrimarySources.size(); i++)
    {
      if(m_PrimarySources[i]->AddItem(&m_PrimaryBuffer[0],
                                      US_IMG_ORIENT_MF,
                                      frameSize, VTK_UNSIGNED_CHAR,
                                      1, US_IMG_BRIGHTNESS, 0,
                                      this->FrameNumber,
                                      currentTime,
                                      currentTime, //no timestamp filtering needed
                                      &this->m_CustomFields) != PLUS_SUCCESS)
      {
        LOG_WARNING("Error adding item to primary video source " << m_PrimarySources[i]->GetSourceId());
      }
      this->FrameNumber += 1;
    }
    WPFreePointer(texture);
    this->Modified();
  }
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
bool vtkPlusWinProbeVideoSource::IsScanning()
{
  return WPGetIsScanningProperty();
}

// ----------------------------------------------------------------------------
PlusStatus vtkPlusWinProbeVideoSource::SetTransmitFrequencyMHz(float frequency)
{
  m_Frequency = frequency;
  if(Connected)
  {
    ::SetTxFreq(frequency);
    SetPendingRecreateTables(true);

    //what we requested might be only approximately satisfied
    m_Frequency = ::GetTxFreq();
  }
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
float vtkPlusWinProbeVideoSource::GetTransmitFrequencyMHz()
{
  if(Connected)
  {
    m_Frequency = ::GetTxFreq();
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
    ::SetSSDepth(depth);
    SetPendingRecreateTables(true);
    //what we requested might be only approximately satisfied
    m_ScanDepth = ::GetSSDepth();
    // Update decimation with scan depth
    m_SSDecimation = ::GetSSDecimation();
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
double vtkPlusWinProbeVideoSource::GetOverallTimeGainCompensation()
{
  if(Connected)
  {
    m_OverallTimeGainCompensation = GetTGCOverallGain();
  }
  return m_OverallTimeGainCompensation;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusWinProbeVideoSource::SetOverallTimeGainCompensation(double value)
{
  if(Connected)
  {
    SetTGCOverallGain(value);
    SetPendingTGCUpdate(true);
    //what we requested might be only approximately satisfied
    for (int tgcIndex=0; tgcIndex < 8; tgcIndex += 1)
    {
      m_TimeGainCompensation[tgcIndex] = GetTGC(tgcIndex);
    }
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
float vtkPlusWinProbeVideoSource::GetARFIFocalPointDepth(int index)
{
  assert(index >= 0 && index < 6);
  if(Connected)
  {
    m_ARFIFocalPointDepth[index] = WPGetARFIFocalDepth(index);
  }
  return m_ARFIFocalPointDepth[index];
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusWinProbeVideoSource::SetARFIFocalPointDepth(int index, float depth)
{
  assert(index >= 0 && index < 6);
  m_ARFIFocalPointDepth[index] = depth;
  if(Connected)
  {
    WPSetARFIFocalDepth(index, depth);
    SetPendingRecreateTables(true);
    //what we requested might be only approximately satisfied
    m_ARFIFocalPointDepth[index] = WPGetARFIFocalDepth(index);
  }
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
int32_t vtkPlusWinProbeVideoSource::GetBMultiFocalZoneCount()
{
  if(Connected)
  {
    m_BMultiTxCount = GetBMultiTxCount();
  }
  return m_BMultiTxCount;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusWinProbeVideoSource::SetBMultiFocalZoneCount(int32_t count)
{
  assert(count > 0 && count <= 4);
  m_BMultiTxCount = count;
  if(Connected)
  {
    SetBMultiTxCount(count);
    SetPendingRecreateTables(true);
  }
  return PLUS_SUCCESS;
}

bool vtkPlusWinProbeVideoSource::GetARFIIsX8BFEnabled()
{
  if(Connected)
  {
    quadBFCount = ::GetARFIIsX8BFEnabled() ? 2 : 1;
  }
  return quadBFCount == 2;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusWinProbeVideoSource::SetARFITxTxCycleCount(uint16_t propertyValue)
{
  assert(propertyValue > 0 && propertyValue <= 16);
  m_ARFITxTxCycleCount = propertyValue;
  if(Connected)
  {
    ::SetARFITxTxCycleCount(propertyValue);
    SetPendingRecreateTables(true);
  }
  return PLUS_SUCCESS;
}

uint16_t vtkPlusWinProbeVideoSource::GetARFITxTxCycleCount()
{
  if(Connected)
  {
    m_ARFITxTxCycleCount = ::GetARFITxTxCycleCount();
  }
  return m_ARFITxTxCycleCount;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusWinProbeVideoSource::SetARFITxTxCycleWidth(uint8_t propertyValue)
{
  assert(propertyValue > 0 && propertyValue <= 255);
  m_ARFITxTxCycleWidth = propertyValue;
  if(Connected)
  {
    ::SetARFITxTxCycleWidth(propertyValue);
    SetPendingRecreateTables(true);
  }
  return PLUS_SUCCESS;
}

uint8_t vtkPlusWinProbeVideoSource::GetARFITxTxCycleWidth()
{
  if(Connected)
  {
    m_ARFITxTxCycleWidth = ::GetARFITxTxCycleWidth();
  }
  return m_ARFITxTxCycleWidth;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusWinProbeVideoSource::SetARFITxCycleCount(uint16_t propertyValue)
{
  m_ARFITxCycleCount = propertyValue;
  if(Connected)
  {
    ::SetARFITxCycleCount(propertyValue);
    SetPendingRecreateTables(true);
  }
  return PLUS_SUCCESS;
}

uint16_t vtkPlusWinProbeVideoSource::GetARFITxCycleCount()
{
  if(Connected)
  {
    m_ARFITxCycleCount = ::GetARFITxCycleCount();
  }
  return m_ARFITxCycleCount;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusWinProbeVideoSource::SetARFITxCycleWidth(uint8_t propertyValue)
{
  m_ARFITxCycleWidth = propertyValue;
  if(Connected)
  {
    ::SetARFITxCycleWidth(propertyValue);
    SetPendingRecreateTables(true);
  }
  return PLUS_SUCCESS;
}

uint8_t vtkPlusWinProbeVideoSource::GetARFITxCycleWidth()
{
  if(Connected)
  {
    m_ARFITxCycleWidth = ::GetARFITxCycleWidth();
  }
  return m_ARFITxCycleWidth;
}

//----------------------------------------------------------------------------
void vtkPlusWinProbeVideoSource::SetSpatialCompoundEnabled(bool value)
{
  if(Connected)
  {
    SetSCIsEnabled(value);
    if(value)
    {
      SetSpatialCompoundCount(m_SpatialCompoundCount);
    }
    else
    {
      SetSpatialCompoundCount(0);
    }
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
PlusStatus vtkPlusWinProbeVideoSource::SetExtraSourceMode(Mode mode)
{
  if((GetBRFEnabled() && mode == Mode::BRF) || (GetMModeEnabled() && mode == Mode::M) || (GetARFIEnabled() && mode == Mode::ARFI))
  {
    return PLUS_SUCCESS;
  }
  SetMIsEnabled(mode == Mode::M);
  SetARFIIsEnabled(mode == Mode::ARFI);

  if(mode == Mode::BRF)
  {
    SetHandleBRFInternally(false);
    SetBFRFImageCaptureMode(2);
  }
  else
  {
    SetHandleBRFInternally(true);
    SetBFRFImageCaptureMode(0);
  }

  if(mode == Mode::M)
  {
    SetMIsRevolving(m_MRevolvingEnabled);
    SetMPRF(m_MPRF);
    SetMAcousticLineIndex(m_MLineIndex);
    ::SetMWidth(m_MWidth);
    ::SetMAcousticLineCount(m_MAcousticLineCount);
  }
  else if(mode == Mode::ARFI)
  {
    m_Mode = Mode::ARFI;
    int samplesPerLine = m_ARFIStopSample - m_ARFIStartSample;
    int lineCount = 16;
    int lineRepeatCount = m_ARFIPrePushLineRepeatCount + m_ARFIPostPushLineRepeatCount;
    unsigned arfiDataSize = samplesPerLine * lineCount * lineRepeatCount * m_ARFIPushConfigurationCount;

    int timestampsPerLineRepeat = (4 / quadBFCount);
    unsigned timeblockSize = timestampsPerLineRepeat * lineRepeatCount * m_ARFIPushConfigurationCount;
    m_ExtraFrameSize = { arfiDataSize + timeblockSize, 1, 1 };
    this->AdjustBufferSizes();
    std::vector<int32_t> zeroData(m_ExtraFrameSize[0] * m_ExtraFrameSize[1] * m_ExtraFrameSize[2], 0);
    // add a fake zero-filled frame immediately, because the first frame seems to get lost somehow
    for(unsigned i = 0; i < m_ExtraSources.size(); i++)
    {
      double timestamp = vtkIGSIOAccurateTimer::GetSystemTime();
      if(m_ExtraSources[i]->AddItem(&zeroData[0],
                                    US_IMG_ORIENT_FM,
                                    m_ExtraFrameSize, VTK_INT,
                                    1, US_IMG_RF_REAL, 0,
                                    this->FrameNumber,
                                    timestamp,
                                    timestamp,
                                    &m_CustomFields) != PLUS_SUCCESS)
      {
        LOG_WARNING("Error adding fake zeros item to ARFI video source " << m_ExtraSources[i]->GetSourceId());
      }
      else
      {
        LOG_WARNING("Success adding fake zeros item to ARFI video source ");
      }
    }
  }
  SetPendingRecreateTables(true);
  LOG_INFO("Mode changed to: " << this->ModeToString(mode));
  m_Mode = mode;
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
void vtkPlusWinProbeVideoSource::SetBRFEnabled(bool value)
{
  if(value)
  {
    SetExtraSourceMode(Mode::BRF);
  }
  else
  {
    SetExtraSourceMode(Mode::B);
  }
}

//----------------------------------------------------------------------------
void vtkPlusWinProbeVideoSource::SetMModeEnabled(bool value)
{
  if(value)
  {
    SetExtraSourceMode(Mode::M);
  }
  else
  {
    SetExtraSourceMode(Mode::B);
  }
}

//----------------------------------------------------------------------------
void vtkPlusWinProbeVideoSource::SetARFIEnabled(bool value)
{
  if(value)
  {
    SetExtraSourceMode(Mode::ARFI);
  }
  else
  {
    SetExtraSourceMode(Mode::B);
  }
}

//----------------------------------------------------------------------------
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

void vtkPlusWinProbeVideoSource::SetBBubbleContrastEnabled(bool value)
{
  if(Connected)
  {
    SetBIsBubbleContrast(value);
    SetPendingRecreateTables(true);
  }
  m_BBubbleContrastEnabled = GetBIsBubbleContrast();
}

bool vtkPlusWinProbeVideoSource::GetBBubbleContrastEnabled()
{
  if(Connected)
  {
    m_BBubbleContrastEnabled = GetBIsBubbleContrast();
  }
  return m_BBubbleContrastEnabled;
}

void vtkPlusWinProbeVideoSource::SetBAmplitudeModulationEnabled(bool value)
{
  if(Connected)
  {
    SetBIsAmplitudeModulation(value);
    SetPendingRecreateTables(true);
  }
  m_BAmplitudeModulationEnabled = GetBIsAmplitudeModulation();
}

bool vtkPlusWinProbeVideoSource::GetBAmplitudeModulationEnabled()
{
  if(Connected)
  {
    m_BAmplitudeModulationEnabled = GetBIsAmplitudeModulation();
  }
  return m_BAmplitudeModulationEnabled;
}

void vtkPlusWinProbeVideoSource::SetBTransmitLocked(bool value)
{
  if(Connected)
  {
    SetBIsTransmitLocked(value);
    SetPendingRecreateTables(true);
  }
  m_BTransmitLocked = GetBIsTransmitLocked();
}

bool vtkPlusWinProbeVideoSource::GetBTransmitLocked()
{
  if(Connected)
  {
    m_BTransmitLocked = GetBIsTransmitLocked();
  }
  return m_BTransmitLocked;
}


void vtkPlusWinProbeVideoSource::SetBTransmitCurrent(int value)
{
  if(Connected)
  {
    SetBTxCurrent(value);
    SetPendingRecreateTables(true);
  }
  m_BTransmitCurrent = GetBTxCurrent();
}

int vtkPlusWinProbeVideoSource::GetBTransmitCurrent()
{
  if(Connected)
  {
    m_BTransmitCurrent = GetBTxCurrent();
  }
  return m_BTransmitCurrent;
}

void vtkPlusWinProbeVideoSource::SetBTransmitCycleCount(uint16_t value)
{
  if(Connected)
  {
    SetTxTxCycleCount(value);
    SetPendingRecreateTables(true);
  }
  m_BTransmitCycleCount = GetBTransmitCycleCount();
}

uint16_t vtkPlusWinProbeVideoSource::GetBTransmitCycleCount()
{
  if(Connected)
  {
    m_BTransmitCycleCount = GetTxTxCycleCount();
  }
  return m_BTransmitCycleCount;
}

void vtkPlusWinProbeVideoSource::SetBTransmitFNumber(double value)
{
  if(Connected)
  {
    SetTxTxFNumber(value);
    SetPendingRecreateTables(true);
  }
  m_BTransmitFNumber = GetBTransmitFNumber();
}

double vtkPlusWinProbeVideoSource::GetBTransmitFNumber()
{
  if(Connected)
  {
    m_BTransmitFNumber = GetTxTxFNumber();
  }
  return m_BTransmitFNumber;
}

void vtkPlusWinProbeVideoSource::SetBApodizationFNumber(double value)
{
  if(Connected)
  {
    SetApodizationFNumber(value);
    SetPendingRecreateTables(true);
  }
  m_BApodizationFNumber = GetApodizationFNumber();
}

double vtkPlusWinProbeVideoSource::GetBApodizationFNumber()
{
  if(Connected)
  {
    m_BApodizationFNumber = GetApodizationFNumber();
  }
  return m_BApodizationFNumber;
}

void vtkPlusWinProbeVideoSource::SetBBubbleDestructionEnabled(bool value)
{
  if(Connected)
  {
    SetBisBubblePop(value);
  }
  m_BBubbleDestructionEnabled = GetBisBubblePop();
}

bool vtkPlusWinProbeVideoSource::GetBBubbleDestructionEnabled()
{
  if(Connected)
  {
    m_BBubbleDestructionEnabled = GetBisBubblePop();
  }
  return m_BBubbleDestructionEnabled;
}

void vtkPlusWinProbeVideoSource::SetBBubbleDestructionCycleCount(int16_t value)
{
  if(Connected)
  {
    SetTxBubblePopCycleCount(value);
    SetPendingRecreateTables(true);
  }
  m_BBubbleDestructionCycleCount = GetTxBubblePopCycleCount();
}

int16_t vtkPlusWinProbeVideoSource::GetBBubbleDestructionCycleCount()
{
  if(Connected)
  {
    m_BBubbleDestructionCycleCount = GetTxBubblePopCycleCount();
  }
  return m_BBubbleDestructionCycleCount;
}

void vtkPlusWinProbeVideoSource::SetBTXFilterCoefficientSet(int32_t value)
{
  if(Connected)
  {
    SetTxFilterCoefficientSet(value);
  }
  m_BTXFilterCoefficientSet = GetTxFilterCoefficientSet();
}

int32_t vtkPlusWinProbeVideoSource::GetBTXFilterCoefficientSet()
{
  if(Connected)
  {
    m_BTXFilterCoefficientSet = GetTxFilterCoefficientSet();
  }
  return m_BTXFilterCoefficientSet;
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
void vtkPlusWinProbeVideoSource::SetARFIStartSample(int32_t value)
{
  if(Connected)
  {
    m_ARFIStartSample = value;
    ::SetARFIStartSample(value);
    SetPendingRecreateTables(true);
  }
}

int32_t vtkPlusWinProbeVideoSource::GetARFIStartSample()
{
  if(Connected)
  {
    m_ARFIStartSample = ::GetARFIStartSample();
  }
  return m_ARFIStartSample;
}

//----------------------------------------------------------------------------
void vtkPlusWinProbeVideoSource::SetARFIStopSample(int32_t value)
{
  if(Connected)
  {
    m_ARFIStopSample = value;
    ::SetARFIStopSample(value);
    SetPendingRecreateTables(true);
  }
}

int32_t vtkPlusWinProbeVideoSource::GetARFIStopSample()
{
  if(Connected)
  {
    m_ARFIStopSample = ::GetARFIStopSample();
  }
  return m_ARFIStopSample;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusWinProbeVideoSource::SetARFILineTimer(uint16_t propertyValue)
{

  if(Connected)
  {
    if(propertyValue > 2000)
    {
      LOG_ERROR("The maximum ARFI line timer is 2000. Ignoring call to change to " << propertyValue);
      return PLUS_FAIL;
    }
    m_ARFILineTimer = propertyValue;
    ::SetARFILineTimer(propertyValue);
    SetPendingRecreateTables(true);
    return PLUS_SUCCESS;
  }
  return PLUS_FAIL;
}

uint16_t vtkPlusWinProbeVideoSource::GetARFILineTimer()
{
  if(Connected)
  {
    m_ARFILineTimer = ::GetARFILineTimer();
  }
  return m_ARFILineTimer;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusWinProbeVideoSource::SetARFIPrePushLineRepeatCount(int32_t propertyValue)
{
  if(Connected)
  {
    m_ARFIPrePushLineRepeatCount = propertyValue;
    ::SetARFIPrePushLineRepeatCount(propertyValue);
    SetPendingRecreateTables(true);
    return PLUS_SUCCESS;
  }
  return PLUS_FAIL;
}

int32_t vtkPlusWinProbeVideoSource::GetARFIPrePushLineRepeatCount()
{
  if(Connected)
  {
    m_ARFIPrePushLineRepeatCount = ::GetARFIPrePushLineRepeatCount();
  }
  return m_ARFIPrePushLineRepeatCount;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusWinProbeVideoSource::SetARFIPostPushLineRepeatCount(int32_t propertyValue)
{
  if(Connected)
  {
    m_ARFIPostPushLineRepeatCount = propertyValue;
    ::SetARFIPostPushLineRepeatCount(propertyValue);
    SetPendingRecreateTables(true);
    return PLUS_SUCCESS;
  }
  return PLUS_FAIL;
}

int32_t vtkPlusWinProbeVideoSource::GetARFIPostPushLineRepeatCount()
{
  if(Connected)
  {
    m_ARFIPostPushLineRepeatCount = ::GetARFIPostPushLineRepeatCount();
  }
  return m_ARFIPostPushLineRepeatCount;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusWinProbeVideoSource::SetARFIInterSetDelay(int32_t propertyValue)
{
  if(Connected)
  {
    if(propertyValue > 250 * 250)
    {
      LOG_ERROR("The maximum ARFI inter set delay is 250*250. Ignoring call to change to " << propertyValue);
      return PLUS_FAIL;
    }
    else if (propertyValue == 0)
    {
      LOG_WARNING("ARFI inter set delay defaulting to clinical delay mode of approximately 1 second.");
    }
    m_ARFIInterSetDelay = propertyValue;
    ::SetARFIInterSetDelay(propertyValue);  // API call includes SetPendingRecreateTables(true). Really just need SetPendingRestartSequencer(true);
    return PLUS_SUCCESS;
  }
  return PLUS_FAIL;
}

int32_t vtkPlusWinProbeVideoSource::GetARFIInterSetDelay()
{
  if(Connected)
  {
    m_ARFIInterSetDelay = ::GetARFIInterSetDelay();
  }
  return m_ARFIInterSetDelay;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusWinProbeVideoSource::SetARFIInterPushDelay(int32_t propertyValue)
{
  if(Connected)
  {
    if(propertyValue > 250)
    {
      LOG_ERROR("The maximum ARFI inter push delay is 250. Ignoring call to change to " << propertyValue);
      return PLUS_FAIL;
    }
    m_ARFIInterPushDelay = propertyValue;
    ::SetARFIInterPushDelay(propertyValue);  // API call includes SetPendingRecreateTables(true). Really just need SetPendingRestartSequencer(true);
    return PLUS_SUCCESS;
  }
  return PLUS_FAIL;
}

int32_t vtkPlusWinProbeVideoSource::GetARFIInterPushDelay()
{
  if(Connected)
  {
    m_ARFIInterPushDelay = ::GetARFIInterPushDelay();
  }
  return m_ARFIInterPushDelay;
}

//----------------------------------------------------------------------------
void vtkPlusWinProbeVideoSource::SetARFIPushConfigurationString(std::string pushConfiguration)
{
  if(Connected)
  {
    m_ARFIPushConfigurationCount = std::count(pushConfiguration.begin(), pushConfiguration.end(), ';') + 1;
    if(quadBFCount == 1)
    {
      LOG_ERROR("A X4BF does not support ARFI mode.");
    }
    WPSetARFIPushConfigurationString(pushConfiguration.c_str());
    SetPendingRecreateTables(true);
  }
}

//----------------------------------------------------------------------------
std::string vtkPlusWinProbeVideoSource::GetARFIPushConfigurationString()
{
  if(Connected)
  {
    char* temp = new char[50];
    WPGetARFIPushConfigurationString(temp);
    m_ARFIPushConfigurationString.assign(temp);
    delete[] temp;
  }
  return m_ARFIPushConfigurationString;
}

//----------------------------------------------------------------------------
std::string vtkPlusWinProbeVideoSource::GetFPGARevDateString()
{
  char* temp = new char[20];
  WPGetFPGARevDateString(temp);
  m_FPGAVersion.assign(temp);
  delete[] temp;
  return m_FPGAVersion;
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
    spacing[0] = this->GetTransducerWidthMm() / (m_PrimaryFrameSize[0] - 1);
    spacing[1] = m_ScanDepth / (m_PrimaryFrameSize[1] - 1);
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
      spacing[0] = m_ScanDepth / (m_ExtraFrameSize[0] - 1);
      spacing[1] = this->GetTransducerWidthMm() / (m_ExtraFrameSize[1] - 1);
    }
    else
    {
      spacing = GetPrimarySourceSpacing();
    }
  }
  return spacing;
}

bool vtkPlusWinProbeVideoSource::GetARFIEnabled()
{
  bool arfiEnabled = false;
  if(Connected)
  {
    arfiEnabled = (m_Mode == Mode::ARFI) && GetARFIIsEnabled();
  }
  return arfiEnabled;
}

PlusStatus vtkPlusWinProbeVideoSource::ARFIPush(uint8_t maximumVoltage /* = 50 */)
{
  if (this->Connected && m_Mode == Mode::ARFI)
  {
    for(unsigned i = 0; i < m_ExtraSources.size(); i++)
    {
      m_ExtraSources[i]->Clear();  // clear the rf buffer to ensure frames in the buffer are current
    }
    if(m_FPGAVersion != "2020-10-24 24")
    {
      LOG_ERROR("The FPGA version must be '2020-10-24 24' to ARFIPush. Current version: " << m_FPGAVersion);
      return PLUS_FAIL;
    }

    // Mitigate risk of burning out probes with high voltage pushes
    // and ensure we're using the voltage that we think we are
    uint8_t cappedVoltage = this->GetVoltage();
    if (cappedVoltage > maximumVoltage)
    {
      LOG_WARNING("Voltage was higher than 50V before sending ARFI push. Capping voltage to " << std::to_string(maximumVoltage) << "V.");
      cappedVoltage = maximumVoltage;
    }
    this->SetVoltage(cappedVoltage);
    this->ImagingParameters->SetProbeVoltage(cappedVoltage);

    ::ARFIPush();
    return PLUS_SUCCESS;
  }
  return PLUS_FAIL;
}

std::string vtkPlusWinProbeVideoSource::GetTransducerID()
{
  return this->m_TransducerID;
}

int vtkPlusWinProbeVideoSource::GetBPRF()
{
  if (Connected)
  {
    m_BPRF = ::GetBPRF();
  }
  return m_BPRF;
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

PlusStatus vtkPlusWinProbeVideoSource::SendCommand(const char * command)
{
  if (Connected)
  {
    ::SendCommand(command);
    return PLUS_SUCCESS;
  }
  return PLUS_FAIL;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusWinProbeVideoSource::InternalApplyImagingParameterChange()
{
  if (!this->Connected)
  {
    // trying to apply parameters when not connected leads to crashes
    LOG_ERROR("Cannot apply changes while not connected.")
    return PLUS_FAIL;
  }

  PlusStatus status = PLUS_SUCCESS;

  if (this->ImagingParameters->IsSet(vtkPlusUsImagingParameters::KEY_DEPTH)
    && this->ImagingParameters->IsPending(vtkPlusUsImagingParameters::KEY_DEPTH))
  {
    if (this->SetScanDepthMm(this->ImagingParameters->GetDepthMm()) == PLUS_SUCCESS)
    {
      this->ImagingParameters->SetPending(vtkPlusUsImagingParameters::KEY_DEPTH, false);
    }
    else
    {
      LOG_ERROR("Failed to set depth imaging parameter");
      status = PLUS_FAIL;
    }
  }

  if (this->ImagingParameters->IsSet(vtkPlusUsImagingParameters::KEY_VOLTAGE)
    && this->ImagingParameters->IsPending(vtkPlusUsImagingParameters::KEY_VOLTAGE))
  {
    if (this->SetVoltage(this->ImagingParameters->GetProbeVoltage()) == PLUS_SUCCESS)
    {
      this->ImagingParameters->SetPending(vtkPlusUsImagingParameters::KEY_VOLTAGE, false);
    }
    else
    {
      LOG_ERROR("Failed to set voltage parameter");
      status = PLUS_FAIL;
    }
  }

  if (this->ImagingParameters->IsSet(vtkPlusUsImagingParameters::KEY_FREQUENCY)
    && this->ImagingParameters->IsPending(vtkPlusUsImagingParameters::KEY_FREQUENCY))
  {
    if (this->SetTransmitFrequencyMHz(this->ImagingParameters->GetFrequencyMhz()) == PLUS_SUCCESS)
    {
      this->ImagingParameters->SetPending(vtkPlusUsImagingParameters::KEY_FREQUENCY, false);
    }
    else
    {
      LOG_ERROR("Failed to set frequency parameter");
      status = PLUS_FAIL;
    }
  }
  return status;
}