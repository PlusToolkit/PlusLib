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

const char* vtkPlusUsImagingParameters::XML_ELEMENT_TAG   = "UsImagingParameters";
const char* vtkPlusUsImagingParameters::KEY_CONTRAST      = "Contrast";
const char* vtkPlusUsImagingParameters::KEY_DEPTH         = "DepthMm";
const char* vtkPlusUsImagingParameters::KEY_DYNRANGE      = "DynRangeDb";
const char* vtkPlusUsImagingParameters::KEY_FREQUENCY     = "FrequencyMhz";
const char* vtkPlusUsImagingParameters::KEY_GAIN          = "GainPercent";
const char* vtkPlusUsImagingParameters::KEY_TGC           = "TimeGainCompensation";
const char* vtkPlusUsImagingParameters::KEY_INTENSITY     = "Intensity";
const char* vtkPlusUsImagingParameters::KEY_SECTOR        = "SectorPercent";
const char* vtkPlusUsImagingParameters::KEY_ZOOM          = "ZoomFactor";
const char* vtkPlusUsImagingParameters::KEY_SOUNDVELOCITY = "SoundVelocity";
const char* vtkPlusUsImagingParameters::KEY_VOLTAGE       = "Voltage";
const char* vtkPlusUsImagingParameters::KEY_IMAGESIZE     = "ImageSize";

//----------------------------------------------------------------------------
vtkPlusUsImagingParameters::vtkPlusUsImagingParameters()
  : vtkObject()
{
  this->Parameters[KEY_FREQUENCY] = ParameterInfo("-1");
  this->Parameters[KEY_DEPTH] = ParameterInfo("-1");
  this->Parameters[KEY_SECTOR] = ParameterInfo("-1");
  this->Parameters[KEY_GAIN] = ParameterInfo("-1");
  this->Parameters[KEY_TGC] = ParameterInfo("-1 -1 -1");
  this->Parameters[KEY_INTENSITY] = ParameterInfo("-1");
  this->Parameters[KEY_CONTRAST] = ParameterInfo("-1");
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
  this->Parameters[KEY_TGC].Value = result.str();
  this->Parameters[KEY_TGC].Set = true;
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusUsImagingParameters::SetTimeGainCompensation(double* tgc, int length)
{
  std::vector<double> tgcVec(tgc, tgc+length);
  return this->SetTimeGainCompensation(tgcVec);
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusUsImagingParameters::GetTimeGainCompensation(std::vector<double>& tgc) const
{
  ParameterMap::const_iterator keyIt = this->Parameters.find(KEY_TGC);
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

// Check
//----------------------------------------------------------------------------
vtkPlusUsImagingParameters::ParameterMapConstIterator vtkPlusUsImagingParameters::begin() const
{
  return this->Parameters.begin();
}

//----------------------------------------------------------------------------
vtkPlusUsImagingParameters::ParameterMapConstIterator vtkPlusUsImagingParameters::end() const
{
  return this->Parameters.end();
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
PlusStatus vtkPlusUsImagingParameters::SetImageSize(const std::vector<int>& imageSize)
{
  if( imageSize.size() != 2 && imageSize.size() != 3 )
  {
    LOG_ERROR("Invalid image dimensions.");
    return PLUS_FAIL;
  }
  std::stringstream result;
  std::copy(imageSize.begin(), imageSize.end(), std::ostream_iterator<double>(result, " "));
  if( imageSize.size() == 2 )
  {
    result << " " << 1;
  }

  this->Parameters[KEY_IMAGESIZE].Value = result.str();
  this->Parameters[KEY_IMAGESIZE].Set = true;

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusUsImagingParameters::SetImageSize(int* imageSize, int length)
{
  std::vector<int> imageSizeVec(imageSize, imageSize+length);
  return this->SetImageSize(imageSizeVec);
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusUsImagingParameters::SetImageSize(int x, int y, int z)
{
  std::vector<int> imageSizeVec;
  imageSizeVec.push_back(x);
  imageSizeVec.push_back(y);
  imageSizeVec.push_back(z);
  return this->SetImageSize(imageSizeVec);
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusUsImagingParameters::GetImageSize(std::vector<int>& imageSize) const
{
  ParameterMap::const_iterator keyIt = this->Parameters.find(KEY_IMAGESIZE);
  if( keyIt != this->Parameters.end() && keyIt->second.Set == false )
  {
    return PLUS_FAIL;
  }
  else if( keyIt == this->Parameters.end() )
  {
    return PLUS_FAIL;
  }

  std::stringstream ss;
  ParameterMapConstIterator it = this->Parameters.find(KEY_IMAGESIZE);
  ss.str(it->second.Value);
  std::vector<int> numbers((std::istream_iterator<int>(ss)), std::istream_iterator<int>());
  imageSize = numbers;
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
std::vector<int> vtkPlusUsImagingParameters::GetImageSize() const
{
  std::vector<int> imageSize;
  this->GetImageSize(imageSize);
  return imageSize;
}

//----------------------------------------------------------------------------
void vtkPlusUsImagingParameters::PrintSelf(ostream& os, vtkIndent indent)
{
  Superclass::PrintSelf(os, indent);

  for (ParameterMap::iterator it=this->Parameters.begin(); it!=this->Parameters.end(); ++it)
  {
    if (it->second.Set == true)
    {
      os << indent << it->first << ": " << it->second.Value
         << (it->second.Changed ? " (changed)" : "") << std::endl;
    }
  }
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusUsImagingParameters::ReadConfiguration(vtkXMLDataElement* deviceConfig)
{
  vtkXMLDataElement* parameterList(NULL);
  for( int i = 0; i < deviceConfig->GetNumberOfNestedElements(); ++i )
  {
    vtkXMLDataElement* element = deviceConfig->GetNestedElement(i);
    if( STRCASECMP(element->GetName(), XML_ELEMENT_TAG) == 0 )
    {
      parameterList = element;
      break;
    }
  }

  if( parameterList == NULL )
  {
    LOG_ERROR("Unable to locate UsImagingParameters tag in device config. Unable to read imaging parameters. Device defaults will probably be used.");
    return PLUS_FAIL;
  }

  for( int i = 0; i < parameterList->GetNumberOfNestedElements(); ++i )
  {
    vtkXMLDataElement* element = parameterList->GetNestedElement(i);
    this->Parameters[element->GetAttribute("name")] = ParameterInfo(element->GetAttribute("value"));
    this->Parameters[element->GetAttribute("name")].Set = true;
  }

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusUsImagingParameters::WriteConfiguration(vtkXMLDataElement* deviceConfig)
{
  /* Create a sub node, populate it with entries of the form
  <device ...>
    <UsImagingParameters>
      <UsParameter name="DepthMm" value="55"/>
      <UsParameter name="FreqMhz" value="12.5"/>
      <UsParameter name="SectorSizeMm" value="60"/>
    </UsImagingParameters>
    ...
  </device>
  */

  XML_FIND_NESTED_ELEMENT_CREATE_IF_MISSING(parameterList, deviceConfig, XML_ELEMENT_TAG);

  // Clear the list before writing new elements
  parameterList->RemoveAllNestedElements();

  for( ParameterMap::iterator it = this->Parameters.begin(); it != this->Parameters.end(); ++it )
  {
    if (it->second.Set == false)
    {
      // Don't write out parameters that are defaults
      continue;
    }

    vtkSmartPointer<vtkXMLDataElement> parameter = vtkSmartPointer<vtkXMLDataElement>::New();
    parameter->SetName("UsParameter");
    parameter->SetAttribute("name", it->first.c_str());
    parameter->SetAttribute("value", it->second.Value.c_str());

    parameterList->AddNestedElement(parameter);
  }

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
bool vtkPlusUsImagingParameters::IsSet(const std::string& paramName) const
{
  ParameterMap::const_iterator keyIt = this->Parameters.find(paramName);
  if( keyIt != this->Parameters.end() )
  {
    return keyIt->second.Set;
  }

  LOG_ERROR("Invalid key request sent to vtkPlusUsImagingParameters::IsSet -- " << paramName);
  return false;
}

//-----------------------------------------------------------------------------
bool vtkPlusUsImagingParameters::IsChanged(const std::string& paramName) const
{
  ParameterMap::const_iterator keyIt = this->Parameters.find(paramName);
  if (keyIt != this->Parameters.end())
  {
    return keyIt->second.Changed;
  }

  LOG_ERROR("Invalid key request sent to vtkPlusUsImagingParameters::IsChanged -- " << paramName);
  return false;
}

//-----------------------------------------------------------------------------
template <typename T>
PlusStatus vtkPlusUsImagingParameters::GetValue(const std::string& paramName, T& outputValue) const
{
  ParameterMap::const_iterator keyIt = this->Parameters.find(paramName);
  if( keyIt != this->Parameters.end() && keyIt->second.Set == false )
  {
    return PLUS_FAIL;
  }
  else if( keyIt == this->Parameters.end() )
  {
    return PLUS_FAIL;
  }

  std::stringstream ss;
  ParameterMapConstIterator it = this->Parameters.find(paramName);
  ss.str(it->second.Value);
  ss >> outputValue;
  return PLUS_FAIL;
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusUsImagingParameters::DeepCopy(const vtkPlusUsImagingParameters& otherParameters)
{
  for (ParameterMap::const_iterator it=otherParameters.Parameters.begin(); it!=otherParameters.Parameters.end(); ++it)
  {
    this->Parameters[it->first] = it->second;
  }

  return PLUS_SUCCESS;
}
