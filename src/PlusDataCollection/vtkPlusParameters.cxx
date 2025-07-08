/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"
#include "vtkPlusParameters.h"

#include <iterator>

//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkPlusParameters);

//----------------------------------------------------------------------------

const char* vtkPlusParameters::XML_ELEMENT_TAG = "Parameters";
const char* vtkPlusParameters::PARAMETER_TAG = "Parameter";
const char* vtkPlusParameters::NAME_TAG = "Name";
const char* vtkPlusParameters::VALUE_TAG = "Value";

//----------------------------------------------------------------------------
vtkPlusParameters::vtkPlusParameters()
  : vtkObject()
{
}

//----------------------------------------------------------------------------
vtkPlusParameters::~vtkPlusParameters()
{
  LOG_TRACE("vtkPlusParameters::~vtkPlusParameters()");
}

// Check
//----------------------------------------------------------------------------
vtkPlusParameters::ParameterMapConstIterator vtkPlusParameters::begin() const
{
  return this->Parameters.begin();
}

//----------------------------------------------------------------------------
vtkPlusParameters::ParameterMapConstIterator vtkPlusParameters::end() const
{
  return this->Parameters.end();
}

//----------------------------------------------------------------------------
void vtkPlusParameters::PrintSelf(ostream& os, vtkIndent indent)
{
  Superclass::PrintSelf(os, indent);

  for (ParameterMap::iterator it = this->Parameters.begin(); it != this->Parameters.end(); ++it)
  {
    if (it->second.Set == true)
    {
      os << indent << it->first << ": " << it->second.Value
         << (it->second.Pending ? " (pending)" : "") << std::endl;
    }
  }
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusParameters::ReadConfiguration(vtkXMLDataElement* deviceConfig)
{
  vtkXMLDataElement* parameterList(NULL);
  for (int i = 0; i < deviceConfig->GetNumberOfNestedElements(); ++i)
  {
    vtkXMLDataElement* element = deviceConfig->GetNestedElement(i);
    if (STRCASECMP(element->GetName(), this->GetXMLElementName()) == 0)
    {
      parameterList = element;
      break;
    }
  }

  if (parameterList == NULL)
  {
    LOG_ERROR("Unable to locate " << this->GetXMLElementName() << " tag in device config.Unable to read imaging parameters.Device defaults will probably be used.");
    return PLUS_FAIL;
  }

  for (int i = 0; i < parameterList->GetNumberOfNestedElements(); ++i)
  {
    vtkXMLDataElement* element = parameterList->GetNestedElement(i);
    std::string name = element->GetAttribute(NAME_TAG) ? element->GetAttribute(NAME_TAG) : "";
    std::string value = element->GetAttribute(VALUE_TAG) ? element->GetAttribute(VALUE_TAG) : "";
    if (name.empty())
    {
      continue;
    }

    if (this->Parameters[name].Value != value)
    {
      // If the value changed, then mark it pending
      this->Parameters[name].Pending = true;
    }
    this->Parameters[name].Value = value;
    this->Parameters[name].Set = true;
  }

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusParameters::WriteConfiguration(vtkXMLDataElement* deviceConfig)
{
  /* Create a sub node, populate it with entries of the form
  <device ...>
    <Parameters>
      <Parameter name="DepthMm" value="55"/>
      <Parameter name="FrequencyMhz" value="12.5"/>
    </Parameters>
    ...
  </device>
  */

  XML_FIND_NESTED_ELEMENT_CREATE_IF_MISSING(parameterList, deviceConfig, this->GetXMLElementName());

  // Clear the list before writing new elements
  parameterList->RemoveAllNestedElements();

  for (ParameterMap::iterator it = this->Parameters.begin(); it != this->Parameters.end(); ++it)
  {
    if (it->second.Set == false)
    {
      // Don't write out parameters that are defaults
      continue;
    }

    vtkSmartPointer<vtkXMLDataElement> parameter = vtkSmartPointer<vtkXMLDataElement>::New();
    parameter->SetName(PARAMETER_TAG);
    parameter->SetAttribute(NAME_TAG, it->first.c_str());
    parameter->SetAttribute(VALUE_TAG, it->second.Value.c_str());

    parameterList->AddNestedElement(parameter);
  }

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
bool vtkPlusParameters::IsSet(const std::string& paramName) const
{
  ParameterMapConstIterator keyIt = this->Parameters.find(paramName);
  if (keyIt != this->Parameters.end())
  {
    return keyIt->second.Set;
  }

  LOG_ERROR("Invalid key request sent to vtkPlusParameters::IsSet -- " << paramName);
  return false;
}

//-----------------------------------------------------------------------------
bool vtkPlusParameters::IsPending(const std::string& paramName) const
{
  ParameterMapConstIterator keyIt = this->Parameters.find(paramName);
  if (keyIt != this->Parameters.end())
  {
    return keyIt->second.Pending;
  }

  LOG_ERROR("Invalid key request sent to vtkPlusParameters::IsPending -- " << paramName);
  return false;
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusParameters::SetPending(const std::string& paramName, bool pending)
{
  ParameterMapIterator keyIt = this->Parameters.find(paramName);
  if (keyIt != this->Parameters.end())
  {
    if (keyIt->second.Set)
    {
      keyIt->second.Pending = pending;
      return PLUS_SUCCESS;
    }
    else
    {
      LOG_ERROR("Pending status cannot be set for unset parameter -- " << paramName);
      return PLUS_FAIL;
    }
  }

  LOG_ERROR("Invalid key request sent to vtkPlusParameters::SetPending -- " << paramName);
  return PLUS_FAIL;
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusParameters::DeepCopy(const vtkPlusParameters& otherParameters)
{
  for (ParameterMapConstIterator it = otherParameters.Parameters.begin(); it != otherParameters.Parameters.end(); ++it)
  {
    if (this->Parameters[it->first].Value != it->second.Value)
    {
      // If the value changed, then mark it pending
      this->Parameters[it->first].Pending = true;
    }
    this->Parameters[it->first].Value = it->second.Value;
    this->Parameters[it->first].Set = it->second.Set;
  }

  return PLUS_SUCCESS;
}
