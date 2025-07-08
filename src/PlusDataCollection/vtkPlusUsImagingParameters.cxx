/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"
#include "vtkPlusUsImagingParameters.h"

#include <iterator>

//----------------------------------------------------------------------------

vtkStandardNewMacro(vtkPlusUsImagingParameters);

//----------------------------------------------------------------------------

const char* vtkPlusUsImagingParameters::US_XML_ELEMENT_TAG   = "UsImagingParameters";
const char* vtkPlusUsImagingParameters::KEY_CONTRAST      = "Contrast";
const char* vtkPlusUsImagingParameters::KEY_DEPTH         = "DepthMm";
const char* vtkPlusUsImagingParameters::KEY_FOCUS_DEPTH   = "FocusDepthPercent";
const char* vtkPlusUsImagingParameters::KEY_DYNRANGE      = "DynRangeDb";
const char* vtkPlusUsImagingParameters::KEY_FREQUENCY     = "FrequencyMhz";
const char* vtkPlusUsImagingParameters::KEY_GAIN          = "GainPercent";
const char* vtkPlusUsImagingParameters::KEY_TGC           = "TimeGainCompensation";
const char* vtkPlusUsImagingParameters::KEY_INTENSITY     = "Intensity";
const char* vtkPlusUsImagingParameters::KEY_POWER         = "PowerDb";
const char* vtkPlusUsImagingParameters::KEY_SECTOR        = "SectorPercent";
const char* vtkPlusUsImagingParameters::KEY_ZOOM          = "ZoomFactor";
const char* vtkPlusUsImagingParameters::KEY_SOUNDVELOCITY = "SoundVelocity";
const char* vtkPlusUsImagingParameters::KEY_VOLTAGE       = "Voltage";
const char* vtkPlusUsImagingParameters::KEY_IMAGESIZE     = "ImageSize";

//----------------------------------------------------------------------------
vtkPlusUsImagingParameters::vtkPlusUsImagingParameters()
  : vtkPlusParameters()
{
  this->Parameters[KEY_FREQUENCY] = ParameterInfo("-1");
  this->Parameters[KEY_DEPTH] = ParameterInfo("-1");
  this->Parameters[KEY_FOCUS_DEPTH] = ParameterInfo("-1");
  this->Parameters[KEY_SECTOR] = ParameterInfo("-1");
  this->Parameters[KEY_GAIN] = ParameterInfo("-1");
  this->Parameters[KEY_TGC] = ParameterInfo("-1 -1 -1");
  this->Parameters[KEY_INTENSITY] = ParameterInfo("-1");
  this->Parameters[KEY_CONTRAST] = ParameterInfo("-1");
  this->Parameters[KEY_POWER] = ParameterInfo("-1");
  this->Parameters[KEY_DYNRANGE] = ParameterInfo("-1");
  this->Parameters[KEY_ZOOM] = ParameterInfo("-1");
  this->Parameters[KEY_SOUNDVELOCITY] = ParameterInfo("1540");
  this->Parameters[KEY_VOLTAGE] = ParameterInfo("-1");
  this->Parameters[KEY_IMAGESIZE] = ParameterInfo("-1 -1 -1");
}

//----------------------------------------------------------------------------
vtkPlusUsImagingParameters::~vtkPlusUsImagingParameters()
{
  LOG_TRACE("vtkPlusUsImagingParameters::~vtkPlusUsImagingParameters()");
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusUsImagingParameters::SetFrequencyMhz(double aFrequencyMhz)
{
  return this->SetValue<double>(KEY_FREQUENCY, aFrequencyMhz);
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusUsImagingParameters::GetFrequencyMhz(double& aFrequencyMhz) const
{
  return this->GetValue<double>(KEY_FREQUENCY, aFrequencyMhz);
}

//----------------------------------------------------------------------------
double vtkPlusUsImagingParameters::GetFrequencyMhz() const
{
  double aValue;
  this->GetValue<double>(KEY_FREQUENCY, aValue);
  return aValue;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusUsImagingParameters::SetDepthMm(double aDepthMm)
{
  return this->SetValue<double>(KEY_DEPTH, aDepthMm);
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusUsImagingParameters::GetDepthMm(double& aDepthMm) const
{
  return this->GetValue<double>(KEY_DEPTH, aDepthMm);
}

//----------------------------------------------------------------------------
double vtkPlusUsImagingParameters::GetDepthMm() const
{
  double aValue;
  this->GetValue<double>(KEY_DEPTH, aValue);
  return aValue;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusUsImagingParameters::SetFocusDepthPercent(double aFocusDepthPercent)
{
  return this->SetValue<double>(KEY_FOCUS_DEPTH, aFocusDepthPercent);
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusUsImagingParameters::GetFocusDepthPercent(double& aFocusDepthPercent) const
{
  return this->GetValue<double>(KEY_FOCUS_DEPTH, aFocusDepthPercent);
}

//----------------------------------------------------------------------------
double vtkPlusUsImagingParameters::GetFocusDepthPercent() const
{
  double aValue;
  this->GetValue<double>(KEY_FOCUS_DEPTH, aValue);
  return aValue;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusUsImagingParameters::SetGainPercent(double aGainPercent)
{
  return this->SetValue<double>(KEY_GAIN, aGainPercent);
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusUsImagingParameters::GetGainPercent(double aGainPercent) const
{
  return this->GetValue<double>(KEY_GAIN, aGainPercent);
}

//----------------------------------------------------------------------------
double vtkPlusUsImagingParameters::GetGainPercent() const
{
  double aValue;
  this->GetValue<double>(KEY_GAIN, aValue);
  return aValue;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusUsImagingParameters::SetTimeGainCompensation(const std::vector<double>& tgc)
{
  std::stringstream result;
  std::copy(tgc.begin(), tgc.end(), std::ostream_iterator<double>(result, " "));

  if (this->Parameters[KEY_TGC].Value != result.str())
  {
    // If the value changed, then mark it pending
    this->Parameters[KEY_TGC].Pending = true;
  }
  this->Parameters[KEY_TGC].Value = result.str();
  this->Parameters[KEY_TGC].Set = true;
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusUsImagingParameters::SetTimeGainCompensation(double* tgc, int length)
{
  std::vector<double> tgcVec(tgc, tgc + length);
  return this->SetTimeGainCompensation(tgcVec);
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusUsImagingParameters::GetTimeGainCompensation(std::vector<double>& tgc) const
{
  ParameterMapConstIterator keyIt = this->Parameters.find(KEY_TGC);
  if (keyIt != this->Parameters.end() && keyIt->second.Set == false)
  {
    return PLUS_FAIL;
  }
  else if (keyIt == this->Parameters.end())
  {
    return PLUS_FAIL;
  }

  std::stringstream ss;
  ParameterMapConstIterator it = this->Parameters.find(KEY_TGC);
  ss.str(it->second.Value);
  std::vector<double> numbers((std::istream_iterator<double>(ss)), std::istream_iterator<double>());
  tgc = numbers;
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
std::vector<double> vtkPlusUsImagingParameters::GetTimeGainCompensation() const
{
  std::vector<double> vec;
  this->GetTimeGainCompensation(vec);
  return vec;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusUsImagingParameters::SetIntensity(double aIntensity)
{
  return this->SetValue<double>(KEY_INTENSITY, aIntensity);
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusUsImagingParameters::GetIntensity(double& aIntensity) const
{
  return this->GetValue<double>(KEY_INTENSITY, aIntensity);
}

//----------------------------------------------------------------------------
double vtkPlusUsImagingParameters::GetIntensity() const
{
  double aValue;
  this->GetValue<double>(KEY_INTENSITY, aValue);
  return aValue;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusUsImagingParameters::SetContrast(double aContrast)
{
  return this->SetValue<double>(KEY_CONTRAST, aContrast);
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusUsImagingParameters::GetContrast(double& aContrast) const
{
  return this->GetValue<double>(KEY_CONTRAST, aContrast);
}

//----------------------------------------------------------------------------
double vtkPlusUsImagingParameters::GetContrast() const
{
  double aValue;
  this->GetValue<double>(KEY_CONTRAST, aValue);
  return aValue;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusUsImagingParameters::SetPowerDb(double aPower)
{
  return this->SetValue<double>(KEY_POWER, aPower);
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusUsImagingParameters::GetPowerDb(double& aPower) const
{
  return this->GetValue<double>(KEY_POWER, aPower);
}

//----------------------------------------------------------------------------
double vtkPlusUsImagingParameters::GetPowerDb() const
{
  double aValue;
  this->GetValue<double>(KEY_POWER, aValue);
  return aValue;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusUsImagingParameters::SetDynRangeDb(double aDynRangeDb)
{
  return this->SetValue<double>(KEY_DYNRANGE, aDynRangeDb);
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusUsImagingParameters::GetDynRangeDb(double& aDynRangeDb) const
{
  return this->GetValue<double>(KEY_DYNRANGE, aDynRangeDb);
}

//----------------------------------------------------------------------------
double vtkPlusUsImagingParameters::GetDynRangeDb() const
{
  double aValue;
  this->GetValue<double>(KEY_DYNRANGE, aValue);
  return aValue;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusUsImagingParameters::SetZoomFactor(double aZoomFactor)
{
  return this->SetValue<double>(KEY_ZOOM, aZoomFactor);
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusUsImagingParameters::GetZoomFactor(double& aZoomFactor) const
{
  return this->GetValue<double>(KEY_ZOOM, aZoomFactor);
}

//----------------------------------------------------------------------------
double vtkPlusUsImagingParameters::GetZoomFactor() const
{
  double aValue;
  this->GetValue<double>(KEY_ZOOM, aValue);
  return aValue;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusUsImagingParameters::SetSectorPercent(double aSectorPercent)
{
  return this->SetValue<double>(KEY_SECTOR, aSectorPercent);
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusUsImagingParameters::GetSectorPercent(double& aSectorPercent) const
{
  return this->GetValue<double>(KEY_SECTOR, aSectorPercent);
}

//----------------------------------------------------------------------------
double vtkPlusUsImagingParameters::GetSectorPercent() const
{
  double aValue;
  this->GetValue<double>(KEY_SECTOR, aValue);
  return aValue;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusUsImagingParameters::SetSoundVelocity(float aSoundVelocity)
{
  return this->SetValue<float>(KEY_SOUNDVELOCITY, aSoundVelocity);
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusUsImagingParameters::GetSoundVelocity(float& aSoundVelocity) const
{
  return this->GetValue<float>(KEY_SOUNDVELOCITY, aSoundVelocity);
}

//----------------------------------------------------------------------------
float vtkPlusUsImagingParameters::GetSoundVelocity() const
{
  float aValue;
  this->GetValue<float>(KEY_SOUNDVELOCITY, aValue);
  return aValue;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusUsImagingParameters::SetProbeVoltage(float aVoltage)
{
  return this->SetValue<float>(KEY_VOLTAGE, aVoltage);
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusUsImagingParameters::GetProbeVoltage(float& aVoltage) const
{
  return this->GetValue<float>(KEY_VOLTAGE, aVoltage);
}

//----------------------------------------------------------------------------
float vtkPlusUsImagingParameters::GetProbeVoltage() const
{
  float aValue;
  this->GetValue<float>(KEY_VOLTAGE, aValue);
  return aValue;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusUsImagingParameters::SetImageSize(const FrameSizeType& imageSize)
{
  std::stringstream result;
  std::copy(imageSize.begin(), imageSize.end(), std::ostream_iterator<double>(result, " "));

  if (this->Parameters[KEY_IMAGESIZE].Value != result.str())
  {
    // If the value changed, then mark it pending
    this->Parameters[KEY_IMAGESIZE].Pending = true;
  }
  this->Parameters[KEY_IMAGESIZE].Value = result.str();
  this->Parameters[KEY_IMAGESIZE].Set = true;

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusUsImagingParameters::SetImageSize(unsigned int x, unsigned int y, unsigned int z)
{
  FrameSizeType imageSize = { x, y, z };
  return this->SetImageSize(imageSize);
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusUsImagingParameters::GetImageSize(FrameSizeType& imageSize) const
{
  ParameterMapConstIterator keyIt = this->Parameters.find(KEY_IMAGESIZE);
  if (keyIt != this->Parameters.end() && keyIt->second.Set == false)
  {
    return PLUS_FAIL;
  }
  else if (keyIt == this->Parameters.end())
  {
    return PLUS_FAIL;
  }

  std::stringstream ss;
  ParameterMapConstIterator it = this->Parameters.find(KEY_IMAGESIZE);
  ss.str(it->second.Value);
  std::vector<unsigned int> numbers((std::istream_iterator<unsigned int>(ss)), std::istream_iterator<unsigned int>());

  imageSize[0] = numbers[0];
  if (numbers.size() > 1)
  {
    imageSize[1] = numbers[1];
  }
  if (numbers.size() > 2)
  {
    imageSize[2] = numbers[2];
  }
  else
  {
    imageSize[2] = 1;
  }
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
FrameSizeType vtkPlusUsImagingParameters::GetImageSize() const
{
  FrameSizeType imageSize;
  this->GetImageSize(imageSize);
  return imageSize;
}
