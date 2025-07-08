/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkPlusParameters_h
#define __vtkPlusParameters_h

#include "vtkPlusDataCollectionExport.h"

#include <string>
#include <map>

/*!
\class vtkPlusParameters
\brief This class is used as the basis to store a configuration of generic parameters for any device.
This class exists mainly for two reasons:
* Provide a standard interface for accessing parameters
* Enable standardized API for operating on parameters
\ingroup PlusLibDataCollection
*/

class vtkPlusDataCollectionExport vtkPlusParameters : public vtkObject
{
protected:
  class ParameterInfo
  {
  public:
    ParameterInfo() : Value(""), Set(false), Pending(false) {};
    ParameterInfo(std::string defaultValue) : Value(defaultValue), Set(false), Pending(false) {};

    /// Serialized parameter value
    std::string Value;
    /// Flag indicating whether the invalid default has been changed to meaningful value
    /// (Note: sound velocity has a meaningful default)
    bool Set;
    /// Flag indicating whether the parameter is changed but has not been set to device
    bool Pending;
  };
  typedef std::map<std::string, ParameterInfo> ParameterMap;
  typedef ParameterMap::iterator ParameterMapIterator;
  typedef ParameterMap::const_iterator ParameterMapConstIterator;

  /*! Return an iterator to the beginning of the parameter space */
  ParameterMapConstIterator begin() const;
  /*! Return an iterator to the end of the parameter space */
  ParameterMapConstIterator end() const;

  public:
    static const char* XML_ELEMENT_TAG;
    static const char* PARAMETER_TAG;
    static const char* NAME_TAG;
    static const char* VALUE_TAG;

public:
  static vtkPlusParameters* New();
  vtkTypeMacro(vtkPlusParameters, vtkObject);

  /*!
  Read main configuration from/to XML data
  Assumes that the data element passed is the device element, not the root!
  \param deviceConfig the XML element of the device
  */
  virtual PlusStatus ReadConfiguration(vtkXMLDataElement* deviceConfig);

  /*!
  Write main configuration from/to XML data
  Assumes that the data element passed is the device element, not the root!
  \param deviceConfig the XML element of the device
  */
  virtual PlusStatus WriteConfiguration(vtkXMLDataElement* deviceConfig);

  /*!
  Copy the values from another imaging parameters
  */
  virtual PlusStatus DeepCopy(const vtkPlusParameters& otherParameters);

  /*!
  Request a stored value by key name
  \param paramName the key value to retrieve
  \param outputValue the output variable to write to
  */
  template<typename T> PlusStatus GetValue(const std::string& paramName, T& outputValue) const
  {
    ParameterMapConstIterator keyIt = this->Parameters.find(paramName);
    if (keyIt != this->Parameters.end() && keyIt->second.Set == false)
    {
      return PLUS_FAIL;
    }
    else if (keyIt == this->Parameters.end())
    {
      return PLUS_FAIL;
    }

    std::stringstream ss;
    ParameterMapConstIterator it = this->Parameters.find(paramName);
    ss.str(it->second.Value);
    ss >> outputValue;
    return PLUS_SUCCESS;
  }

  /*!
  Set a stored value by key name
  Defined in the header to make it available externally
  \param paramName the key value to retrieve
  \param aValue the value to write
  */
  template<typename T> PlusStatus SetValue(const std::string& paramName, T aValue)
  {
    std::stringstream ss;
    ss << aValue;
    if (this->Parameters[paramName].Value != ss.str())
    {
      this->Parameters[paramName].Pending = true;
    }
    this->Parameters[paramName].Value = ss.str();
    this->Parameters[paramName].Set = true;
    return PLUS_SUCCESS;
  };
  /*!
  Request the set status of a parameter (whether it is not the default value)
  \param paramName the key value to retrieve
  */
  bool IsSet(const std::string& paramName) const;
  /*!
  Request the pending status of a parameter
  \param paramName the key value to retrieve
  */
  bool IsPending(const std::string& paramName) const;
  /*!
  Set the pending status of a parameter
  */
  PlusStatus SetPending(const std::string& paramName, bool pending);

  /*! Print the list of supported parameters. For diagnostic purposes only. */
  virtual void PrintSelf(ostream& os, vtkIndent indent) VTK_OVERRIDE;

protected:
  vtkPlusParameters();
  virtual ~vtkPlusParameters();

  virtual const char* GetXMLElementName() { return vtkPlusParameters::XML_ELEMENT_TAG; };

  ParameterMap Parameters;
};

#endif