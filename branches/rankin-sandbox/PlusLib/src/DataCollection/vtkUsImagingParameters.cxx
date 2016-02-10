/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"
#include "vtkUsImagingParameters.h"

#include <iterator>
#include <string>

//----------------------------------------------------------------------------

vtkStandardNewMacro(vtkUsImagingParameters);

//----------------------------------------------------------------------------

const char* vtkUsImagingParameters::KEY_CONTRAST = "Contrast";
const char* vtkUsImagingParameters::KEY_DEPTH = "DepthMm";
const char* vtkUsImagingParameters::KEY_DYNRANGE = "DynRangeDb";
const char* vtkUsImagingParameters::KEY_FREQUENCY = "FrequencyMhz";
const char* vtkUsImagingParameters::KEY_GAIN = "GainPercent";
const char* vtkUsImagingParameters::KEY_TGC = "TimeGainCompensation";
const char* vtkUsImagingParameters::KEY_INTENSITY = "Intensity";
const char* vtkUsImagingParameters::KEY_SECTOR = "SectorPercent";
const char* vtkUsImagingParameters::KEY_ZOOM = "ZoomFactor";
const char* vtkUsImagingParameters::KEY_SOUNDVELOCITY = "SoundVelocity";
const char* vtkUsImagingParameters::KEY_VOLTAGE = "Voltage";
const char* vtkUsImagingParameters::KEY_IMAGESIZE = "ImageSize";

//----------------------------------------------------------------------------

vtkUsImagingParameters::vtkUsImagingParameters()
  : vtkObject()
{
  this->ParameterValues[KEY_FREQUENCY]="-1";
  this->ParameterValues[KEY_DEPTH]="-1";
  this->ParameterValues[KEY_SECTOR]="-1";
  this->ParameterValues[KEY_GAIN]="-1 -1 -1";
  this->ParameterValues[KEY_INTENSITY]="-1";
  this->ParameterValues[KEY_CONTRAST]="-1";
  this->ParameterValues[KEY_DYNRANGE]="-1";
  this->ParameterValues[KEY_ZOOM]="-1";
  this->ParameterValues[KEY_SOUNDVELOCITY]="1540";
  this->ParameterValues[KEY_VOLTAGE]="-1";
  this->ParameterValues[KEY_IMAGESIZE]="-1 -1 -1";

  this->ParameterSet[KEY_FREQUENCY]=false;
  this->ParameterSet[KEY_DEPTH]=false;
  this->ParameterSet[KEY_SECTOR]=false;
  this->ParameterSet[KEY_GAIN]=false;
  this->ParameterSet[KEY_INTENSITY]=false;
  this->ParameterSet[KEY_CONTRAST]=false;
  this->ParameterSet[KEY_DYNRANGE]=false;
  this->ParameterSet[KEY_ZOOM]=false;
  this->ParameterSet[KEY_SOUNDVELOCITY]=false;
  this->ParameterSet[KEY_VOLTAGE]=false;
  this->ParameterSet[KEY_IMAGESIZE]=false;
}

//----------------------------------------------------------------------------

vtkUsImagingParameters::~vtkUsImagingParameters()
{
  LOG_TRACE("vtkUsImagingParameters::~vtkUsImagingParameters()");
}

//----------------------------------------------------------------------------
PlusStatus vtkUsImagingParameters::SetFrequencyMhz(double aFrequencyMhz)
{
  return this->SetValue<double>(KEY_FREQUENCY, aFrequencyMhz);
}

//----------------------------------------------------------------------------
PlusStatus vtkUsImagingParameters::GetFrequencyMhz(double& aFrequencyMhz) const
{
  return this->GetValue<double>(KEY_FREQUENCY, aFrequencyMhz);
}

//----------------------------------------------------------------------------
double vtkUsImagingParameters::GetFrequencyMhz() const
{
  double aValue;
  this->GetValue<double>(KEY_FREQUENCY, aValue);
  return aValue;
}

//----------------------------------------------------------------------------
PlusStatus vtkUsImagingParameters::SetDepthMm(double aDepthMm)
{
  return this->SetValue<double>(KEY_DEPTH, aDepthMm);
}

//----------------------------------------------------------------------------
PlusStatus vtkUsImagingParameters::GetDepthMm(double& aDepthMm) const
{
  return this->GetValue<double>(KEY_DEPTH, aDepthMm);
}

//----------------------------------------------------------------------------
double vtkUsImagingParameters::GetDepthMm() const
{
  double aValue;
  this->GetValue<double>(KEY_DEPTH, aValue);
  return aValue;
}

//----------------------------------------------------------------------------
PlusStatus vtkUsImagingParameters::SetGainPercent(double aGainPercent)
{
  return this->SetValue<double>(KEY_GAIN, aGainPercent);
}

//----------------------------------------------------------------------------
PlusStatus vtkUsImagingParameters::GetGainPercent(double aGainPercent) const
{
 return this->GetValue<double>(KEY_GAIN, aGainPercent);
}

//----------------------------------------------------------------------------
double vtkUsImagingParameters::GetGainPercent() const
{
  double aValue;
  this->GetValue<double>(KEY_GAIN, aValue);
  return aValue;
}

//----------------------------------------------------------------------------
PlusStatus vtkUsImagingParameters::SetTimeGainCompensation(const std::vector<double>& tgc)
{
  std::stringstream result;
  std::copy(tgc.begin(), tgc.end(), std::ostream_iterator<double>(result, " "));
  this->ParameterValues[KEY_GAIN] = result.str();
  this->ParameterSet[KEY_GAIN] = true;
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkUsImagingParameters::SetTimeGainCompensation(double* tgc, int length)
{
  std::vector<double> tgcVec(tgc, tgc+length);
  return this->SetTimeGainCompensation(tgcVec);
}

//----------------------------------------------------------------------------
PlusStatus vtkUsImagingParameters::GetTimeGainCompensation(std::vector<double>& tgc) const
{
  ParameterSetMap::const_iterator keyIt = this->ParameterSet.find(KEY_TGC);
  if( keyIt != this->ParameterSet.end() && keyIt->second == false )
  {
    return PLUS_FAIL;
  }
  else if( keyIt == this->ParameterSet.end() )
  {
    return PLUS_FAIL;
  }

  std::stringstream ss;
  ParameterNameMapConstIterator it = this->ParameterValues.find(KEY_TGC);
  ss.str(it->second);
  std::vector<double> numbers((std::istream_iterator<double>(ss)), 
    std::istream_iterator<double>());
  tgc = numbers;
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
std::vector<double> vtkUsImagingParameters::GetTimeGainCompensation() const
{
  std::vector<double> vec;
  this->GetTimeGainCompensation(vec);
  return vec;
}

//----------------------------------------------------------------------------
PlusStatus vtkUsImagingParameters::SetIntensity(double aIntensity)
{
  return this->SetValue<double>(KEY_INTENSITY, aIntensity);
}

//----------------------------------------------------------------------------
PlusStatus vtkUsImagingParameters::GetIntensity(double& aIntensity) const
{
  return this->GetValue<double>(KEY_INTENSITY, aIntensity);
}

//----------------------------------------------------------------------------
double vtkUsImagingParameters::GetIntensity() const
{
  double aValue;
  this->GetValue<double>(KEY_INTENSITY, aValue);
  return aValue;
}

//----------------------------------------------------------------------------
PlusStatus vtkUsImagingParameters::SetContrast(double aContrast)
{
  return this->SetValue<double>(KEY_CONTRAST, aContrast);
}

//----------------------------------------------------------------------------
PlusStatus vtkUsImagingParameters::GetContrast(double& aContrast) const
{
  return this->GetValue<double>(KEY_CONTRAST, aContrast);
}

//----------------------------------------------------------------------------
double vtkUsImagingParameters::GetContrast() const
{
  double aValue;
  this->GetValue<double>(KEY_CONTRAST, aValue);
  return aValue;
}

//----------------------------------------------------------------------------
PlusStatus vtkUsImagingParameters::SetDynRangeDb(double aDynRangeDb)
{
  return this->SetValue<double>(KEY_DYNRANGE, aDynRangeDb);
}

//----------------------------------------------------------------------------
PlusStatus vtkUsImagingParameters::GetDynRangeDb(double& aDynRangeDb) const
{
  return this->GetValue<double>(KEY_DYNRANGE, aDynRangeDb);
}

//----------------------------------------------------------------------------
double vtkUsImagingParameters::GetDynRangeDb() const
{
  double aValue;
  this->GetValue<double>(KEY_DYNRANGE, aValue);
  return aValue;
}

//----------------------------------------------------------------------------
PlusStatus vtkUsImagingParameters::SetZoomFactor(double aZoomFactor)
{
  return this->SetValue<double>(KEY_ZOOM, aZoomFactor);
}

//----------------------------------------------------------------------------
PlusStatus vtkUsImagingParameters::GetZoomFactor(double& aZoomFactor) const
{
  return this->GetValue<double>(KEY_ZOOM, aZoomFactor);
}

//----------------------------------------------------------------------------
double vtkUsImagingParameters::GetZoomFactor() const
{
  double aValue;
  this->GetValue<double>(KEY_ZOOM, aValue);
  return aValue;
}

//----------------------------------------------------------------------------
PlusStatus vtkUsImagingParameters::SetSectorPercent(double aSectorPercent)
{
  return this->SetValue<double>(KEY_SECTOR, aSectorPercent);
}

//----------------------------------------------------------------------------
PlusStatus vtkUsImagingParameters::GetSectorPercent(double& aSectorPercent) const
{
  return this->GetValue<double>(KEY_SECTOR, aSectorPercent);
}

//----------------------------------------------------------------------------
double vtkUsImagingParameters::GetSectorPercent() const
{
  double aValue;
  this->GetValue<double>(KEY_SECTOR, aValue);
  return aValue;
}

//----------------------------------------------------------------------------
PlusStatus vtkUsImagingParameters::SetSoundVelocity(float aSoundVelocity)
{
  return this->SetValue<float>(KEY_SOUNDVELOCITY, aSoundVelocity);
}

//----------------------------------------------------------------------------
PlusStatus vtkUsImagingParameters::GetSoundVelocity(float& aSoundVelocity) const
{
  return this->GetValue<float>(KEY_SOUNDVELOCITY, aSoundVelocity);
}

//----------------------------------------------------------------------------
float vtkUsImagingParameters::GetSoundVelocity() const
{
  float aValue;
  this->GetValue<float>(KEY_SOUNDVELOCITY, aValue);
  return aValue;
}

//----------------------------------------------------------------------------
PlusStatus vtkUsImagingParameters::SetProbeVoltage(float aVoltage)
{
  return this->SetValue<float>(KEY_VOLTAGE, aVoltage);
}

//----------------------------------------------------------------------------
PlusStatus vtkUsImagingParameters::GetProbeVoltage(float& aVoltage) const
{
  return this->GetValue<float>(KEY_VOLTAGE, aVoltage);
}

//----------------------------------------------------------------------------
float vtkUsImagingParameters::GetProbeVoltage() const
{
  float aValue;
  this->GetValue<float>(KEY_VOLTAGE, aValue);
  return aValue;
}

//----------------------------------------------------------------------------
PlusStatus vtkUsImagingParameters::SetImageSize(const std::vector<int>& imageSize)
{
  if( imageSize.size() != 2 || imageSize.size() != 3 )
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

  this->ParameterValues[KEY_IMAGESIZE] = result.str();
  this->ParameterSet[KEY_IMAGESIZE] = true;

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkUsImagingParameters::SetImageSize(int* imageSize, int length)
{
  std::vector<int> imageSizeVec(imageSize, imageSize+length);
  return this->SetImageSize(imageSizeVec);
}

//----------------------------------------------------------------------------
PlusStatus vtkUsImagingParameters::GetImageSize(std::vector<int>& imageSize) const
{
  ParameterSetMap::const_iterator keyIt = this->ParameterSet.find(KEY_IMAGESIZE);
  if( keyIt != this->ParameterSet.end() && keyIt->second == false )
  {
    return PLUS_FAIL;
  }
  else if( keyIt == this->ParameterSet.end() )
  {
    return PLUS_FAIL;
  }

  std::stringstream ss;
  ParameterNameMapConstIterator it = this->ParameterValues.find(KEY_IMAGESIZE);
  ss.str(it->second);
  std::vector<int> numbers((std::istream_iterator<int>(ss)), 
    std::istream_iterator<int>());
  imageSize = numbers;
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
std::vector<int> vtkUsImagingParameters::GetImageSize() const
{
  std::vector<int> imageSize;
  this->GetImageSize(imageSize);
  return imageSize;
}

//----------------------------------------------------------------------------
vtkUsImagingParameters::ParameterNameMapConstIterator vtkUsImagingParameters::begin() const
{
  return this->ParameterValues.begin();
}

//----------------------------------------------------------------------------
vtkUsImagingParameters::ParameterNameMapConstIterator vtkUsImagingParameters::end() const
{
  return this->ParameterValues.end();
}

//----------------------------------------------------------------------------
void vtkUsImagingParameters::PrintSelf(ostream& os, vtkIndent indent)
{
  Superclass::PrintSelf(os, indent);

  for( ParameterNameMap::iterator it = this->ParameterValues.begin(); it != this->ParameterValues.end(); ++it )
  {
    if( this->ParameterSet[it->first] == true )
    {
      os << indent << it->first << ": " << it->second << std::endl;
    }
  }
}

//-----------------------------------------------------------------------------
PlusStatus vtkUsImagingParameters::ReadConfiguration(vtkXMLDataElement* deviceConfig)
{
  vtkXMLDataElement* parameterList(NULL);
  for( int i = 0; i < deviceConfig->GetNumberOfNestedElements(); ++i )
  {
    vtkXMLDataElement* element = deviceConfig->GetNestedElement(i);
    if( STRCASECMP(element->GetName(), "UsImagingParameters") == 0 )
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
    this->ParameterValues[element->GetAttribute("name")] = element->GetAttribute("value");
    this->ParameterSet[element->GetAttribute("name")] = true;
  }

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkUsImagingParameters::WriteConfiguration(vtkXMLDataElement* deviceConfig)
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

  XML_FIND_NESTED_ELEMENT_CREATE_IF_MISSING(parameterList, deviceConfig, "UsImagingParameters");

  // Clear the list before writing new elements
  parameterList->RemoveAllNestedElements();

  for( ParameterNameMap::iterator it = this->ParameterValues.begin(); it != this->ParameterValues.end(); ++it )
  {
    if( this->ParameterSet[it->first] == false )
    {
      // Don't write out parameters that are defaults
      continue;
    }

    vtkSmartPointer<vtkXMLDataElement> parameter = vtkSmartPointer<vtkXMLDataElement>::New();
    parameter->SetName("UsParameter");
    parameter->SetAttribute("name", it->first.c_str());
    parameter->SetAttribute("value", it->second.c_str());

    parameterList->AddNestedElement(parameter);
  }

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
bool vtkUsImagingParameters::IsSet(const std::string& paramName) const
{
  ParameterSetMap::const_iterator keyIt = this->ParameterSet.find(paramName);
  if( keyIt != this->ParameterSet.end() )
  {
    return keyIt->second;
  }

  LOG_ERROR("Invalid key request sent to vtkUsImagingParameters::IsSet -- " << paramName);
  return false;
}

//-----------------------------------------------------------------------------
template <typename T>
PlusStatus vtkUsImagingParameters::GetValue(const std::string& paramName, T& outputValue) const
{
  ParameterSetMap::const_iterator keyIt = this->ParameterSet.find(paramName);
  if( keyIt != this->ParameterSet.end() && keyIt->second == false )
  {
    return PLUS_FAIL;
  }
  else if( keyIt == this->ParameterSet.end() )
  {
    return PLUS_FAIL;
  }

  std::stringstream ss;
  ParameterNameMapConstIterator it = this->ParameterValues.find(KEY_DEPTH);
  ss.str(it->second);
  ss >> outputValue;
  return PLUS_FAIL;
}

//-----------------------------------------------------------------------------
template <typename T>
PlusStatus vtkUsImagingParameters::SetValue(const std::string& paramName, T aValue)
{
  std::stringstream ss;
  ss << aValue;
  this->ParameterValues[paramName] = ss.str();
  this->ParameterSet[paramName] = true;

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkUsImagingParameters::DeepCopy(const vtkUsImagingParameters& otherParameters)
{
  for( ParameterNameMap::const_iterator it = otherParameters.ParameterValues.begin(); it != otherParameters.ParameterValues.end(); ++it )
  {
    this->ParameterValues[it->first] = it->second;
  }

  for( ParameterSetMap::const_iterator it = otherParameters.ParameterSet.begin(); it != otherParameters.ParameterSet.end(); ++it )
  {
    this->ParameterSet[it->first] = it->second;
  }

  return PLUS_SUCCESS;
}