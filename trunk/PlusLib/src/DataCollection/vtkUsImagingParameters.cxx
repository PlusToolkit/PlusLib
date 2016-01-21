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

  this->ParameterSet[KEY_FREQUENCY]=false;
  this->ParameterSet[KEY_DEPTH]=false;
  this->ParameterSet[KEY_SECTOR]=false;
  this->ParameterSet[KEY_GAIN]=false;
  this->ParameterSet[KEY_INTENSITY]=false;
  this->ParameterSet[KEY_CONTRAST]=false;
  this->ParameterSet[KEY_DYNRANGE]=false;
  this->ParameterSet[KEY_ZOOM]=false;
  this->ParameterSet[KEY_SOUNDVELOCITY]=false;
}

//----------------------------------------------------------------------------

vtkUsImagingParameters::~vtkUsImagingParameters()
{
  LOG_TRACE("vtkUsImagingParameters::~vtkUsImagingParameters()");
}

//----------------------------------------------------------------------------
PlusStatus vtkUsImagingParameters::SetFrequencyMhz(double aFrequencyMhz)
{
  double currentValue;
  this->GetFrequencyMhz(currentValue);
  if( this->ParameterSet[KEY_FREQUENCY] == true && currentValue == aFrequencyMhz )
  {
    return PLUS_SUCCESS;
  }
  std::stringstream ss;
  ss << aFrequencyMhz;
  this->ParameterValues[KEY_FREQUENCY] = ss.str();

  this->ParameterSet[KEY_FREQUENCY] = true;
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkUsImagingParameters::GetFrequencyMhz(double& aFrequencyMhz)
{
  if( this->ParameterSet[KEY_FREQUENCY] == false )
  {
    return PLUS_FAIL;
  }

  std::stringstream ss;
  ss.str(this->ParameterValues[KEY_FREQUENCY]);
  ss >> aFrequencyMhz;
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
double vtkUsImagingParameters::GetFrequencyMhz()
{
  double aValue;
  std::stringstream ss;
  ss.str(this->ParameterValues[KEY_FREQUENCY]);
  ss >> aValue;
  return aValue;
}

//----------------------------------------------------------------------------
PlusStatus vtkUsImagingParameters::SetDepthMm(double aDepthMm)
{
  double currentValue;
  this->GetDepthMm(currentValue);
  if( this->ParameterSet[KEY_DEPTH] == true && currentValue == aDepthMm )
  {
    return PLUS_SUCCESS;
  }

  std::stringstream ss;
  ss << aDepthMm;
  this->ParameterValues[KEY_DEPTH] = ss.str();

  this->ParameterSet[KEY_DEPTH] = true;
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkUsImagingParameters::GetDepthMm(double& aDepthMm)
{
  if( this->ParameterSet[KEY_DEPTH] == false )
  {
    return PLUS_FAIL;
  }

  std::stringstream ss;
  ss.str(this->ParameterValues[KEY_DEPTH]);
  ss >> aDepthMm;
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
double vtkUsImagingParameters::GetDepthMm()
{
  double aValue;
  std::stringstream ss;
  ss.str(this->ParameterValues[KEY_DEPTH]);
  ss >> aValue;
  return aValue;
}

//----------------------------------------------------------------------------
PlusStatus vtkUsImagingParameters::SetGainPercent(double aGainPercent)
{
  double currentValue = 0;
  this->GetGainPercent(currentValue);
  if( this->ParameterSet[KEY_GAIN] == true && currentValue == aGainPercent )
  {
    return PLUS_SUCCESS;
  }

  std::stringstream ss;
  ss << aGainPercent;
  this->ParameterValues[KEY_GAIN] = ss.str();

  this->ParameterSet[KEY_GAIN] = true;
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkUsImagingParameters::GetGainPercent(double aGainPercent)
{
  if( this->ParameterSet[KEY_GAIN] == false )
  {
    return PLUS_FAIL;
  }

  double aValue;
  std::stringstream ss;
  ss.str(this->ParameterValues[KEY_GAIN]);
  ss >> aValue;
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
double vtkUsImagingParameters::GetGainPercent()
{
  double aValue;
  std::stringstream ss;
  ss.str(this->ParameterValues[KEY_GAIN]);
  ss >> aValue;
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
PlusStatus vtkUsImagingParameters::GetTimeGainCompensation(std::vector<double>& tgc)
{
  if( this->ParameterSet[KEY_TGC] == false )
  {
    return PLUS_FAIL;
  }

  std::stringstream ss;
  ss.str(this->ParameterValues[KEY_TGC]);
  std::vector<double> numbers((std::istream_iterator<double>(ss)), 
    std::istream_iterator<double>());
  tgc = numbers;
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
std::vector<double> vtkUsImagingParameters::GetTimeGainCompensation()
{
  std::stringstream ss;
  ss.str(this->ParameterValues[KEY_GAIN]);
  std::vector<double> numbers((std::istream_iterator<double>(ss)), std::istream_iterator<double>());
  return numbers;
}

//----------------------------------------------------------------------------
PlusStatus vtkUsImagingParameters::SetIntensity(double aIntensity)
{
  double currentValue;
  this->GetIntensity(currentValue);
  if( this->ParameterSet[KEY_INTENSITY] == true && currentValue == aIntensity )
  {
    return PLUS_SUCCESS;
  }

  std::stringstream ss;
  ss.str(this->ParameterValues[KEY_INTENSITY]);
  ss >> aIntensity;

  this->ParameterSet[KEY_INTENSITY] = true;
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkUsImagingParameters::GetIntensity(double& aIntensity)
{
  if( this->ParameterSet[KEY_INTENSITY] == false )
  {
    return PLUS_FAIL;
  }

  std::stringstream ss;
  ss.str(this->ParameterValues[KEY_INTENSITY]);
  ss >> aIntensity;
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
double vtkUsImagingParameters::GetIntensity()
{
  double aValue;
  std::stringstream ss;
  ss.str(this->ParameterValues[KEY_INTENSITY]);
  ss >> aValue;
  return aValue;
}

//----------------------------------------------------------------------------
PlusStatus vtkUsImagingParameters::SetContrast(double aContrast)
{
  double currentValue;
  this->GetContrast(currentValue);
  if( this->ParameterSet[KEY_CONTRAST] == true && currentValue == aContrast )
  {
    return PLUS_SUCCESS;
  }

  std::stringstream ss;
  ss.str(this->ParameterValues[KEY_CONTRAST]);
  ss >> aContrast;

  this->ParameterSet[KEY_CONTRAST] = true;
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkUsImagingParameters::GetContrast(double& aContrast)
{
  if( this->ParameterSet[KEY_CONTRAST] == false )
  {
    return PLUS_FAIL;
  }

  std::stringstream ss;
  ss.str(this->ParameterValues[KEY_CONTRAST]);
  ss >> aContrast;
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
double vtkUsImagingParameters::GetContrast()
{
  double aValue;
  std::stringstream ss;
  ss.str(this->ParameterValues[KEY_CONTRAST]);
  ss >> aValue;
  return aValue;
}

//----------------------------------------------------------------------------
PlusStatus vtkUsImagingParameters::SetDynRangeDb(double aDynRangeDb)
{
  double currentValue;
  this->GetDynRangeDb(currentValue);
  if( this->ParameterSet[KEY_DYNRANGE] == true && currentValue == aDynRangeDb )
  {
    return PLUS_SUCCESS;
  }

  std::stringstream ss;
  ss.str(this->ParameterValues[KEY_DYNRANGE]);
  ss >> aDynRangeDb;

  this->ParameterSet[KEY_DYNRANGE] = true;
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkUsImagingParameters::GetDynRangeDb(double& aDynRangeDb)
{
  if( this->ParameterSet[KEY_DYNRANGE] == false )
  {
    return PLUS_FAIL;
  }

  std::stringstream ss;
  ss.str(this->ParameterValues[KEY_DYNRANGE]);
  ss >> aDynRangeDb;
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
double vtkUsImagingParameters::GetDynRangeDb()
{
  double aValue;
  std::stringstream ss;
  ss.str(this->ParameterValues[KEY_DYNRANGE]);
  ss >> aValue;
  return aValue;
}

//----------------------------------------------------------------------------
PlusStatus vtkUsImagingParameters::SetZoomFactor(double aZoomFactor)
{
  double currentValue;
  this->GetZoomFactor(currentValue);
  if( this->ParameterSet[KEY_ZOOM] == true && currentValue == aZoomFactor )
  {
    return PLUS_SUCCESS;
  }

  std::stringstream ss;
  ss.str(this->ParameterValues[KEY_ZOOM]);
  ss >> aZoomFactor;

  this->ParameterSet[KEY_ZOOM] = true;
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkUsImagingParameters::GetZoomFactor(double& aZoomFactor)
{
  if( this->ParameterSet[KEY_ZOOM] == false )
  {
    return PLUS_FAIL;
  }

  std::stringstream ss;
  ss.str(this->ParameterValues[KEY_ZOOM]);
  ss >> aZoomFactor;
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
double vtkUsImagingParameters::GetZoomFactor()
{
  double aValue;
  std::stringstream ss;
  ss.str(this->ParameterValues[KEY_ZOOM]);
  ss >> aValue;
  return aValue;
}

//----------------------------------------------------------------------------
PlusStatus vtkUsImagingParameters::SetSectorPercent(double aSectorPercent)
{
  double currentValue;
  this->GetSectorPercent(currentValue);
  if( this->ParameterSet[KEY_SECTOR] == true && currentValue == aSectorPercent )
  {
    return PLUS_SUCCESS;
  }

  std::stringstream ss;
  ss.str(this->ParameterValues[KEY_SECTOR]);
  ss >> aSectorPercent;

  this->ParameterSet[KEY_SECTOR] = true;
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkUsImagingParameters::GetSectorPercent(double& aSectorPercent)
{
  if( this->ParameterSet[KEY_SECTOR] == false )
  {
    return PLUS_FAIL;
  }

  std::stringstream ss;
  ss.str(this->ParameterValues[KEY_SECTOR]);
  ss >> aSectorPercent;
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
double vtkUsImagingParameters::GetSectorPercent()
{
  double aValue;
  std::stringstream ss;
  ss.str(this->ParameterValues[KEY_SECTOR]);
  ss >> aValue;
  return aValue;
}

//----------------------------------------------------------------------------
PlusStatus vtkUsImagingParameters::SetSoundVelocity(float aSoundVelocity)
{
  float currentValue;
  this->GetSoundVelocity(currentValue);
  if( this->ParameterSet[KEY_SOUNDVELOCITY] == true && currentValue == aSoundVelocity )
  {
    return PLUS_SUCCESS;
  }

  std::stringstream ss;
  ss.str(this->ParameterValues[KEY_SOUNDVELOCITY]);
  ss >> aSoundVelocity;

  this->ParameterSet[KEY_SOUNDVELOCITY] = true;
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkUsImagingParameters::GetSoundVelocity(float& aSoundVelocity)
{
  if( this->ParameterSet[KEY_SOUNDVELOCITY] == false )
  {
    return PLUS_FAIL;
  }

  std::stringstream ss;
  ss.str(this->ParameterValues[KEY_SOUNDVELOCITY]);
  ss >> aSoundVelocity;
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
float vtkUsImagingParameters::GetSoundVelocity()
{
  float aValue;
  std::stringstream ss;
  ss.str(this->ParameterValues[KEY_SOUNDVELOCITY]);
  ss >> aValue;
  return aValue;
}

//----------------------------------------------------------------------------
void vtkUsImagingParameters::PrintSelf(ostream& os, vtkIndent indent)
{
  Superclass::PrintSelf(os, indent);

  for( ParameterNameMap::iterator it = this->ParameterValues.begin(); it != this->ParameterValues.end(); ++it )
  {
    if( this->ParameterSet[it->first] == true )
    {
      os << indent << it->first << ": " << it->second << "\n";
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
      <UsParameter name=KEY_DEPTH value="55"/>
      <UsParameter name="FreqMhz" value="12.5"/>
      <UsParameter name="SectorSizeMm" value="60"/>
    </UsImagingParameters>
  ...
  </device>
  */

  vtkSmartPointer<vtkXMLDataElement> parameterList = vtkSmartPointer<vtkXMLDataElement>::New();
  parameterList->SetName("UsImagingParameters");

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
bool vtkUsImagingParameters::IsSet(const char* paramName)
{
  if( paramName == NULL )
  {
    LOG_ERROR("NULL param name sent as parameter. Cannot perform lookup.");
    return false;
  }

  if( this->ParameterSet.find(paramName) != this->ParameterSet.end() )
  {
    return this->ParameterSet[paramName];
  }

  LOG_ERROR("Invalid key request sent to vtkUsImagingParameters::IsSet -- " << paramName);
  return false;
}

//-----------------------------------------------------------------------------
PlusStatus vtkUsImagingParameters::GetValue(const char* paramName, double& outputValue)
{
  if( paramName == NULL )
  {
    LOG_ERROR("NULL param name sent as parameter. Cannot perform lookup.");
    return PLUS_FAIL;
  }

  if( this->ParameterValues.find(paramName) != this->ParameterValues.end() && this->ParameterSet[paramName] == true)
  {
    std::stringstream ss;
    ss.str(this->ParameterValues[paramName]);
    ss >> outputValue;
    return PLUS_SUCCESS;
  }

  return PLUS_FAIL;
}

//-----------------------------------------------------------------------------
PlusStatus vtkUsImagingParameters::SetValue(const char* paramName, double aValue)
{
  if( this->ParameterValues.find(paramName) != this->ParameterValues.end() )
  {
    std::stringstream ss;
    ss << aValue;
    this->ParameterValues[paramName] = ss.str();
    this->ParameterSet[paramName] = true;

    return PLUS_SUCCESS;
  }

  return PLUS_FAIL;
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