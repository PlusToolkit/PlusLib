/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"
#include "vtkUsImagingParameters.h"

#include <string>

//----------------------------------------------------------------------------

vtkStandardNewMacro(vtkUsImagingParameters);

//----------------------------------------------------------------------------

vtkUsImagingParameters::vtkUsImagingParameters()
: vtkObject()
{
  this->ParameterValues["FrequencyMhz"]="-1";
  this->ParameterValues["DepthMm"]="-1";
  this->ParameterValues["SectorPercent"]="-1";
  this->ParameterValues["GainPercent"]="-1 -1 -1";
  this->ParameterValues["Intensity"]="-1";
  this->ParameterValues["Contrast"]="-1";
  this->ParameterValues["DynRangeDb"]="-1";
  this->ParameterValues["ZoomFactor"]="-1";
  this->ParameterValues["SoundVelocity"]="1540";

  this->ParameterValidity["FrequencyMhz"]=false;
  this->ParameterValidity["DepthMm"]=false;
  this->ParameterValidity["SectorPercent"]=false;
  this->ParameterValidity["GainPercent"]=false;
  this->ParameterValidity["Intensity"]=false;
  this->ParameterValidity["Contrast"]=false;
  this->ParameterValidity["DynRangeDb"]=false;
  this->ParameterValidity["ZoomFactor"]=false;
  this->ParameterValidity["SoundVelocity"]=false;
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
  if( this->ParameterValidity["FrequencyMhz"] == true && currentValue == aFrequencyMhz )
  {
    return PLUS_SUCCESS;
  }
  std::stringstream ss;
  ss << aFrequencyMhz;
  this->ParameterValues["FrequencyMhz"] = ss.str();

  this->ParameterValidity["FrequencyMhz"] = true;
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkUsImagingParameters::GetFrequencyMhz(double& aFrequencyMhz)
{
  if( this->ParameterValidity["FrequencyMhz"] == false )
  {
    return PLUS_FAIL;
  }

  std::stringstream ss;
  ss.str(this->ParameterValues["FrequencyMhz"]);
  ss >> aFrequencyMhz;
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
double vtkUsImagingParameters::GetFrequencyMhz()
{
  double aValue;
  std::stringstream ss;
  ss.str(this->ParameterValues["FrequencyMhz"]);
  ss >> aValue;
  return aValue;
}

//----------------------------------------------------------------------------
PlusStatus vtkUsImagingParameters::SetDepthMm(double aDepthMm)
{
  double currentValue;
  this->GetDepthMm(currentValue);
  if( this->ParameterValidity["DepthMm"] == true && currentValue == aDepthMm )
  {
    return PLUS_SUCCESS;
  }

  std::stringstream ss;
  ss << aDepthMm;
  this->ParameterValues["DepthMm"] = ss.str();

  this->ParameterValidity["DepthMm"] = true;
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkUsImagingParameters::GetDepthMm(double& aDepthMm)
{
  if( this->ParameterValidity["DepthMm"] == false )
  {
    return PLUS_FAIL;
  }

  std::stringstream ss;
  ss.str(this->ParameterValues["DepthMm"]);
  ss >> aDepthMm;
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
double vtkUsImagingParameters::GetDepthMm()
{
  double aValue;
  std::stringstream ss;
  ss.str(this->ParameterValues["DepthMm"]);
  ss >> aValue;
  return aValue;
}

//----------------------------------------------------------------------------
PlusStatus vtkUsImagingParameters::SetGainPercent(double aGainPercent[3])
{
  double currentValue[3];
  this->GetGainPercent(currentValue);
  if( this->ParameterValidity["GainPercent"] == true && currentValue[0] == aGainPercent[0] && currentValue[1] == aGainPercent[1] && currentValue[2] == aGainPercent[2] )
  {
    return PLUS_SUCCESS;
  }

  std::stringstream ss;
  ss << aGainPercent[0] << " " << aGainPercent[1] << " " << aGainPercent[2];
  this->ParameterValues["GainPercent"] = ss.str();

  this->ParameterValidity["GainPercent"] = true;
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkUsImagingParameters::GetGainPercent(double aGainPercent[3])
{
  if( this->ParameterValidity["GainPercent"] == false )
  {
    return PLUS_FAIL;
  }

  std::stringstream ss;
  ss.str(this->ParameterValues["GainPercent"]);
  ss >> aGainPercent[0] >> aGainPercent[1] >> aGainPercent[2];
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkUsImagingParameters::SetIntensity(double aIntensity)
{
  double currentValue;
  this->GetIntensity(currentValue);
  if( this->ParameterValidity["Intensity"] == true && currentValue == aIntensity )
  {
    return PLUS_SUCCESS;
  }

  std::stringstream ss;
  ss.str(this->ParameterValues["Intensity"]);
  ss >> aIntensity;

  this->ParameterValidity["Intensity"] = true;
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkUsImagingParameters::GetIntensity(double& aIntensity)
{
  if( this->ParameterValidity["Intensity"] == false )
  {
    return PLUS_FAIL;
  }

  std::stringstream ss;
  ss.str(this->ParameterValues["Intensity"]);
  ss >> aIntensity;
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
double vtkUsImagingParameters::GetIntensity()
{
  double aValue;
  std::stringstream ss;
  ss.str(this->ParameterValues["Intensity"]);
  ss >> aValue;
  return aValue;
}

//----------------------------------------------------------------------------
PlusStatus vtkUsImagingParameters::SetContrast(double aContrast)
{
  double currentValue;
  this->GetContrast(currentValue);
  if( this->ParameterValidity["Contrast"] == true && currentValue == aContrast )
  {
    return PLUS_SUCCESS;
  }

  std::stringstream ss;
  ss.str(this->ParameterValues["Contrast"]);
  ss >> aContrast;

  this->ParameterValidity["Contrast"] = true;
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkUsImagingParameters::GetContrast(double& aContrast)
{
  if( this->ParameterValidity["Contrast"] == false )
  {
    return PLUS_FAIL;
  }

  std::stringstream ss;
  ss.str(this->ParameterValues["Contrast"]);
  ss >> aContrast;
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
double vtkUsImagingParameters::GetContrast()
{
  double aValue;
  std::stringstream ss;
  ss.str(this->ParameterValues["Contrast"]);
  ss >> aValue;
  return aValue;
}

//----------------------------------------------------------------------------
PlusStatus vtkUsImagingParameters::SetDynRangeDb(double aDynRangeDb)
{
  double currentValue;
  this->GetDynRangeDb(currentValue);
  if( this->ParameterValidity["DynRangeDb"] == true && currentValue == aDynRangeDb )
  {
    return PLUS_SUCCESS;
  }

  std::stringstream ss;
  ss.str(this->ParameterValues["DynRangeDb"]);
  ss >> aDynRangeDb;

  this->ParameterValidity["DynRangeDb"] = true;
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkUsImagingParameters::GetDynRangeDb(double& aDynRangeDb)
{
  if( this->ParameterValidity["DynRangeDb"] == false )
  {
    return PLUS_FAIL;
  }

  std::stringstream ss;
  ss.str(this->ParameterValues["DynRangeDb"]);
  ss >> aDynRangeDb;
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
double vtkUsImagingParameters::GetDynRangeDb()
{
  double aValue;
  std::stringstream ss;
  ss.str(this->ParameterValues["DynRangeDb"]);
  ss >> aValue;
  return aValue;
}

//----------------------------------------------------------------------------
PlusStatus vtkUsImagingParameters::SetZoomFactor(double aZoomFactor)
{
  double currentValue;
  this->GetZoomFactor(currentValue);
  if( this->ParameterValidity["ZoomFactor"] == true && currentValue == aZoomFactor )
  {
    return PLUS_SUCCESS;
  }

  std::stringstream ss;
  ss.str(this->ParameterValues["ZoomFactor"]);
  ss >> aZoomFactor;

  this->ParameterValidity["ZoomFactor"] = true;
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkUsImagingParameters::GetZoomFactor(double& aZoomFactor)
{
  if( this->ParameterValidity["ZoomFactor"] == false )
  {
    return PLUS_FAIL;
  }

  std::stringstream ss;
  ss.str(this->ParameterValues["ZoomFactor"]);
  ss >> aZoomFactor;
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
double vtkUsImagingParameters::GetZoomFactor()
{
  double aValue;
  std::stringstream ss;
  ss.str(this->ParameterValues["ZoomFactor"]);
  ss >> aValue;
  return aValue;
}

//----------------------------------------------------------------------------
PlusStatus vtkUsImagingParameters::SetSectorPercent(double aSectorPercent)
{
  double currentValue;
  this->GetSectorPercent(currentValue);
  if( this->ParameterValidity["SectorPercent"] == true && currentValue == aSectorPercent )
  {
    return PLUS_SUCCESS;
  }

  std::stringstream ss;
  ss.str(this->ParameterValues["SectorPercent"]);
  ss >> aSectorPercent;

  this->ParameterValidity["SectorPercent"] = true;
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkUsImagingParameters::GetSectorPercent(double& aSectorPercent)
{
  if( this->ParameterValidity["SectorPercent"] == false )
  {
    return PLUS_FAIL;
  }

  std::stringstream ss;
  ss.str(this->ParameterValues["SectorPercent"]);
  ss >> aSectorPercent;
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
double vtkUsImagingParameters::GetSectorPercent()
{
  double aValue;
  std::stringstream ss;
  ss.str(this->ParameterValues["SectorPercent"]);
  ss >> aValue;
  return aValue;
}

//----------------------------------------------------------------------------
PlusStatus vtkUsImagingParameters::SetSoundVelocity(int aSoundVelocity)
{
  int currentValue;
  this->GetSoundVelocity(currentValue);
  if( this->ParameterValidity["SoundVelocity"] == true && currentValue == aSoundVelocity )
  {
    return PLUS_SUCCESS;
  }

  std::stringstream ss;
  ss.str(this->ParameterValues["SoundVelocity"]);
  ss >> aSoundVelocity;

  this->ParameterValidity["SoundVelocity"] = true;
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkUsImagingParameters::GetSoundVelocity(int& aSoundVelocity)
{
  if( this->ParameterValidity["SoundVelocity"] == false )
  {
    return PLUS_FAIL;
  }

  std::stringstream ss;
  ss.str(this->ParameterValues["SoundVelocity"]);
  ss >> aSoundVelocity;
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
int vtkUsImagingParameters::GetSoundVelocity()
{
  int aValue;
  std::stringstream ss;
  ss.str(this->ParameterValues["SoundVelocity"]);
  ss >> aValue;
  return aValue;
}

//----------------------------------------------------------------------------
void vtkUsImagingParameters::PrintSelf(ostream& os, vtkIndent indent)
{
  Superclass::PrintSelf(os, indent);

  for( ParameterNameMap::iterator it = this->ParameterValues.begin(); it != this->ParameterValues.end(); ++it )
  {
    if( this->ParameterValidity[it->first] == true )
    {
      os << indent << it->first << ": " << it->second << "\n";
    }
  }
}

//----------------------------------------------------------------------------
PlusStatus vtkUsImagingParameters::GetDisplayedFrameRate(double &aFrameRate)
{

  return PLUS_SUCCESS;
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
    LOG_ERROR("Unable to locate UsImagingParameters tag in device config. Unable to read imaging parameters.");
    return PLUS_FAIL;
  }

  for( int i = 0; i < parameterList->GetNumberOfNestedElements(); ++i )
  {
    vtkXMLDataElement* element = parameterList->GetNestedElement(i);
    this->ParameterValues[element->GetAttribute("name")] = element->GetAttribute("value");
    this->ParameterValidity[element->GetAttribute("name")] = true;
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

  vtkSmartPointer<vtkXMLDataElement> parameterList = vtkSmartPointer<vtkXMLDataElement>::New();
  parameterList->SetName("UsImagingParameters");

  for( ParameterNameMap::iterator it = this->ParameterValues.begin(); it != this->ParameterValues.end(); ++it )
  {
    if( this->ParameterValidity[it->first] == false )
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
