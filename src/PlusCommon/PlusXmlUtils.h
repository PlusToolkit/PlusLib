/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __PlusXmlUtils_h
#define __PlusXmlUtils_h

#include "vtkXMLUtilities.h"

/*!
  \class PlusXmlUtils
  \brief Utility methods for XML tree manipulation
  \ingroup PlusLibCommon
*/

class PlusXmlUtils
{
public:
  /*! Attempt to read an XML file file from the current directory or the device set configuration file directory */
  static PlusStatus ReadDeviceSetConfigurationFromFile(vtkXMLDataElement* config, const char* filename)
  {
    if (config == NULL)
    {
      LOG_ERROR("Reading device set configuration file failed: invalid config input");
      return PLUS_FAIL;
    }
    if (filename == NULL)
    {
      LOG_ERROR("Reading device set configuration file failed: filename is not specified");
      return PLUS_FAIL;
    }

    std::string filePath = filename;
    if (!vtksys::SystemTools::FileExists(filePath.c_str(), true))
    {
      filePath = vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationPath(filename);
      if (!vtksys::SystemTools::FileExists(filePath.c_str(), true))
      {
        LOG_ERROR("Reading device set configuration file failed: " << filename << " does not exist in the current directory or in " << vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationDirectory());
        return PLUS_FAIL;
      }
    }

    vtkSmartPointer<vtkXMLDataElement> rootElement = vtkSmartPointer<vtkXMLDataElement>::Take(vtkXMLUtilities::ReadElementFromFile(filePath.c_str()));
    if (rootElement == NULL)
    {
      LOG_ERROR("Reading device set configuration file failed: syntax error in " << filename);
      return PLUS_FAIL;
    }
    config->DeepCopy(rootElement);
    return PLUS_SUCCESS;
  }

  /*! Get a nested XML element with the specified name. If the element does not exist then create one. */
  static vtkXMLDataElement* GetNestedElementWithName(vtkXMLDataElement* config, const char* elementName)
  {
    if (config == NULL)
    {
      LOG_ERROR("PlusXmlUtils::GetNestedElementWithName failed: config is invalid");
      return NULL;
    }
    if (elementName == NULL)
    {
      LOG_ERROR("PlusXmlUtils::GetNestedElementWithName failed: elementName is invalid");
      return NULL;
    }
    vtkXMLDataElement* nestedElement = config->FindNestedElementWithName(elementName);
    if (nestedElement != NULL)
    {
      // the element exists, return it
      return nestedElement;
    }
    vtkSmartPointer<vtkXMLDataElement> newNestedElement = vtkSmartPointer<vtkXMLDataElement>::New();
    newNestedElement->SetName(elementName);
    config->AddNestedElement(newNestedElement);
    nestedElement = config->FindNestedElementWithName(elementName);
    if (nestedElement == NULL)
    {
      LOG_ERROR("PlusXmlUtils::GetNestedElementWithName failed: cannot add nested element with name " << elementName);
    }
    return nestedElement;
  }
};

#define XML_FIND_NESTED_ELEMENT_OPTIONAL(destinationXmlElementVar, rootXmlElementVar, nestedXmlElementName) \
  if (rootXmlElementVar == NULL) \
  { \
    LOG_ERROR("Invalid device set configuration found while looking for optional " << nestedXmlElementName << " element"); \
    return PLUS_FAIL; \
  } \
  vtkXMLDataElement* destinationXmlElementVar = rootXmlElementVar->FindNestedElementWithName(nestedXmlElementName);

#define XML_FIND_NESTED_ELEMENT_REQUIRED(destinationXmlElementVar, rootXmlElementVar, nestedXmlElementName) \
  if (rootXmlElementVar == NULL) \
  { \
    LOG_ERROR("Invalid device set configuration: unable to find required " << nestedXmlElementName << " element"); \
    return PLUS_FAIL; \
  } \
  vtkXMLDataElement* destinationXmlElementVar = rootXmlElementVar->FindNestedElementWithName(nestedXmlElementName);  \
  if (destinationXmlElementVar == NULL)  \
  { \
    LOG_ERROR("Unable to find required " << nestedXmlElementName << " element in device set configuration");  \
    return PLUS_FAIL; \
  }

#define XML_FIND_NESTED_ELEMENT_CREATE_IF_MISSING(destinationXmlElementVar, rootXmlElementVar, nestedXmlElementName) \
  if (rootXmlElementVar == NULL) \
  { \
    LOG_ERROR("Invalid device set configuration: unable to find required " << nestedXmlElementName << " element"); \
    return PLUS_FAIL; \
  } \
  vtkXMLDataElement* destinationXmlElementVar = PlusXmlUtils::GetNestedElementWithName(rootXmlElementVar,nestedXmlElementName);  \
  if (destinationXmlElementVar == NULL)  \
  { \
    LOG_ERROR("Unable to find or create " << nestedXmlElementName << " element in device set configuration");  \
    return PLUS_FAIL; \
  }

#define XML_VERIFY_ELEMENT(xmlElementVar, expectedXmlElementName) \
  if (xmlElementVar == NULL) \
  { \
    LOG_ERROR("Missing or invalid " << expectedXmlElementName << " element"); \
    return PLUS_FAIL; \
  } \
  if ( xmlElementVar->GetName() == NULL || !PlusCommon::IsEqualInsensitive(xmlElementVar->GetName(),expectedXmlElementName))  \
  { \
    LOG_ERROR("Unable to read " << expectedXmlElementName << " element: unexpected name: " << (xmlElementVar->GetName() ? xmlElementVar->GetName() : "(unspecified)")); \
    return PLUS_FAIL; \
  }

// Read a string attribute and save it to a class member variable. If not found return with fail.
#define XML_READ_STRING_ATTRIBUTE_REQUIRED(memberVar, xmlElementVar)  \
  { \
    const char* destinationXmlElementVar = xmlElementVar->GetAttribute(#memberVar);  \
    if (destinationXmlElementVar == NULL)  \
    { \
      LOG_ERROR("Unable to find required " << #memberVar << " attribute in " << (xmlElementVar->GetName() ? xmlElementVar->GetName() : "(undefined)") << " element in device set configuration");  \
      return PLUS_FAIL; \
    } \
    this->Set##memberVar(std::string(destinationXmlElementVar));  \
  }

// Read a string attribute and save it to a variable.
#define XML_READ_STRING_ATTRIBUTE_NONMEMBER_REQUIRED(varName, var, xmlElementVar)  \
  { \
    const char* destinationXmlElementVar = xmlElementVar->GetAttribute(#varName);  \
    if (destinationXmlElementVar == NULL)  \
    { \
      LOG_ERROR("Unable to find required " << #varName << " attribute in " << (xmlElementVar->GetName() ? xmlElementVar->GetName() : "(undefined)") << " element in device set configuration");  \
      return PLUS_FAIL; \
    } \
    if (destinationXmlElementVar != NULL)  \
    { \
      var = std::string(destinationXmlElementVar); \
    } \
  }

// Read a string attribute and save it to a class member variable. If not found return with fail.
#define XML_READ_CSTRING_ATTRIBUTE_REQUIRED(memberVar, xmlElementVar)  \
  { \
    const char* destinationXmlElementVar = xmlElementVar->GetAttribute(#memberVar);  \
    if (destinationXmlElementVar == NULL)  \
    { \
      LOG_ERROR("Unable to find required " << #memberVar << " attribute in " << (xmlElementVar->GetName() ? xmlElementVar->GetName() : "(undefined)") << " element in device set configuration");  \
      return PLUS_FAIL; \
    } \
    this->Set##memberVar(destinationXmlElementVar);  \
  }

// Read a string attribute (with the same name as the class member variable) and save it to a class member variable.
// If attribute not found then the member is not modified but a warning is logged.
#define XML_READ_CSTRING_ATTRIBUTE_WARNING(memberVar, xmlElementVar)  \
  { \
    const char* destinationXmlElementVar = xmlElementVar->GetAttribute(#memberVar);  \
    if (destinationXmlElementVar != NULL)  \
    { \
      this->Set##memberVar(destinationXmlElementVar);  \
    } \
    else \
    { \
      LOG_WARNING("Unable to find expected " << #memberVar << " attribute in " << (xmlElementVar->GetName() ? xmlElementVar->GetName() : "(undefined)") << " element in device set configuration");  \
    } \
  }

// Read a string attribute (with the same name as the class member variable) and save it to a class member variable.
// If attribute not found then the member is not modified.
#define XML_READ_CSTRING_ATTRIBUTE_OPTIONAL(memberVar, xmlElementVar)  \
  { \
    const char* destinationXmlElementVar = xmlElementVar->GetAttribute(#memberVar);  \
    if (destinationXmlElementVar != NULL)  \
    { \
      this->Set##memberVar(destinationXmlElementVar);  \
    } \
  }

// Read a string attribute (with the same name as the class member variable) and save it to a class member variable.
// If attribute not found then the member is not modified.
#define XML_READ_STRING_ATTRIBUTE_OPTIONAL(memberVar, xmlElementVar)  \
  { \
    const char* destinationXmlElementVar = xmlElementVar->GetAttribute(#memberVar);  \
    if (destinationXmlElementVar != NULL)  \
    { \
      this->Set##memberVar(std::string(destinationXmlElementVar));  \
    } \
  }

// Read a string attribute (with the same name as the class member variable) and save it to a class member variable.
// If attribute not found then the member is not modified.
#define XML_READ_STRING_ATTRIBUTE_WARNING(memberVar, xmlElementVar)  \
  { \
    const char* destinationXmlElementVar = xmlElementVar->GetAttribute(#memberVar);  \
    if (destinationXmlElementVar != NULL)  \
    { \
      this->Set##memberVar(std::string(destinationXmlElementVar));  \
    } \
    else \
    { \
      LOG_WARNING("Unable to find expected " << #memberVar << " attribute in " << (xmlElementVar->GetName() ? xmlElementVar->GetName() : "(undefined)") << " element in device set configuration");  \
    } \
  }

// Read a string attribute and save it to a variable.
#define XML_READ_STRING_ATTRIBUTE_NONMEMBER_OPTIONAL(varName, var, xmlElementVar)  \
  { \
    const char* destinationXmlElementVar = xmlElementVar->GetAttribute(#varName);  \
    if (destinationXmlElementVar != NULL)  \
    { \
      var = std::string(destinationXmlElementVar); \
    } \
  }

// Read a numeric attribute and save it to a class member variable. If not found then no change.
#define XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(memberVarType, memberVar, xmlElementVar)  \
  { \
    memberVarType tmpValue = 0; \
    if ( xmlElementVar->GetScalarAttribute(#memberVar, tmpValue) )  \
    { \
      this->Set##memberVar(tmpValue); \
    } \
  }

// Read a numeric attribute and save it to a class member variable. If not found then log warning and make no change.
#define XML_READ_SCALAR_ATTRIBUTE_WARNING(memberVarType, memberVar, xmlElementVar)  \
  { \
    memberVarType tmpValue = 0; \
    if ( xmlElementVar->GetScalarAttribute(#memberVar, tmpValue) )  \
    { \
      this->Set##memberVar(tmpValue); \
    } \
    else \
    { \
      LOG_WARNING("Unable to find expected " << #memberVar << " attribute in " << (xmlElementVar->GetName() ? xmlElementVar->GetName() : "(undefined)") << " element in device set configuration");  \
    } \
  }

// Read a numeric attribute and save it to a class member variable. If not found then return with error.
#define XML_READ_SCALAR_ATTRIBUTE_REQUIRED(memberVarType, memberVar, xmlElementVar)  \
  { \
    memberVarType tmpValue = 0; \
    if ( xmlElementVar->GetScalarAttribute(#memberVar, tmpValue) )  \
    { \
      this->Set##memberVar(tmpValue); \
    } \
    else  \
    { \
      LOG_ERROR("Unable to find required " << #memberVar << " attribute in " << (xmlElementVar->GetName() ? xmlElementVar->GetName() : "(undefined)") << " element in device set configuration");  \
      return PLUS_FAIL; \
    } \
  }

// Read a numeric attribute and save it to a variable
#define XML_READ_SCALAR_ATTRIBUTE_NONMEMBER_OPTIONAL(varType, attributeName, var, xmlElementVar)  \
  { \
    varType tmpValue; \
    if ( xmlElementVar->GetScalarAttribute(#attributeName, tmpValue) )  \
    { \
      var = tmpValue; \
    } \
  }

// Read a numeric attribute and save it to a class member variable. If not found then return with error.
#define XML_READ_SCALAR_ATTRIBUTE_NONMEMBER_REQUIRED(varType, attributeName, var, xmlElementVar)  \
  { \
    varType tmpValue; \
    if ( xmlElementVar->GetScalarAttribute(#attributeName, tmpValue) )  \
    { \
      var = tmpValue; \
    } \
    else  \
    { \
      LOG_ERROR("Unable to find required " << #attributeName << " attribute in " << (xmlElementVar->GetName() ? xmlElementVar->GetName() : "(undefined)") << " element in device set configuration");  \
      return PLUS_FAIL; \
    } \
  }

// Read a vector of numeric attributes and save it to a class member variable. If not found then no change.
#define XML_READ_VECTOR_ATTRIBUTE_OPTIONAL(memberVarType, vectorSize, memberVar, xmlElementVar)  \
  { \
    memberVarType tmpValue[vectorSize] = {0}; \
    if ( xmlElementVar->GetVectorAttribute(#memberVar, vectorSize, tmpValue) )  \
    { \
      this->Set##memberVar(tmpValue); \
    } \
  }

// Read a vector of numeric attributes and save it to a class member variable. If not found then no change.
#define XML_READ_STD_ARRAY_ATTRIBUTE_OPTIONAL(memberVarType, vectorSize, memberVar, xmlElementVar)  \
  { \
    memberVarType tmpValue[vectorSize] = {0}; \
    if ( xmlElementVar->GetVectorAttribute(#memberVar, vectorSize, tmpValue) )  \
    { \
      for(int i = 0; i < vectorSize+1; ++i) \
      { \
        memberVar[i] = tmpValue[i]; \
      } \
    } \
  }

// Read a vector of numeric attributes and save it to a variable. If not found then no change.
#define XML_READ_STD_ARRAY_ATTRIBUTE_NONMEMBER_OPTIONAL(varType, attributeName, vectorSize, var, xmlElementVar)  \
  { \
    varType tmpValue[vectorSize] = {0}; \
    if ( xmlElementVar->GetVectorAttribute(#attributeName, vectorSize, tmpValue) )  \
    { \
      for(int i = 0; i < vectorSize; ++i) \
      { \
        var[i] = tmpValue[i]; \
      } \
    } \
  }

// Read a vector of numeric attributes and save it to a class member variable. If not found then no change.
// If number of parameters in the attribute is not exactly the same as expected then return with error.
#define XML_READ_STD_ARRAY_ATTRIBUTE_NONMEMBER_EXACT_OPTIONAL(varType, attributeName, vectorSize, var, xmlElementVar)  \
  { \
    varType tmpValue[vectorSize+1] = {0}; /* try to read one more value to detect if more values are specified */ \
    if ( xmlElementVar->GetAttribute(#attributeName) ) \
    { \
      if ( xmlElementVar->GetVectorAttribute(#attributeName, vectorSize+1, tmpValue) == vectorSize)  \
      { \
        for(int i = 0; i < vectorSize; ++i) \
        { \
          var[i] = tmpValue[i]; \
        } \
      } \
      else \
      { \
        LOG_ERROR("Unable to parse " << #attributeName << " attribute in " << (xmlElementVar->GetName() ? xmlElementVar->GetName() : "(undefined)") \
        <<" element in device set configuration. Expected exactly " << vectorSize << " values separated by spaces, instead got this: "<< \
        xmlElementVar->GetAttribute(#attributeName));  \
        return PLUS_FAIL; \
      } \
    } \
  }

// Read a vector of numeric attributes and save it to a class member variable. If not found then no change.
// If number of parameters in the attribute is not exactly the same as expected then return with error.
#define XML_READ_VECTOR_ATTRIBUTE_EXACT_OPTIONAL(memberVarType, vectorSize, memberVar, xmlElementVar)  \
  { \
    memberVarType tmpValue[vectorSize+1] = {0}; /* try to read one more value to detect if more values are specified */ \
    if ( xmlElementVar->GetAttribute(#memberVar) ) \
    { \
      if ( xmlElementVar->GetVectorAttribute(#memberVar, vectorSize+1, tmpValue) == vectorSize)  \
      { \
        this->Set##memberVar(tmpValue); \
      } \
      else \
      { \
        LOG_ERROR("Unable to parse " << #memberVar << " attribute in " << (xmlElementVar->GetName() ? xmlElementVar->GetName() : "(undefined)") \
        <<" element in device set configuration. Expected exactly " << vectorSize << " values separated by spaces, instead got this: "<< \
        xmlElementVar->GetAttribute(#memberVar));  \
        return PLUS_FAIL; \
      } \
    } \
  }

// Read a vector of numeric attributes and save it to a class member variable. If not found then no change.
// If number of parameters in the attribute is not exactly the same as expected then return with error.
#define XML_READ_STD_ARRAY_ATTRIBUTE_EXACT_OPTIONAL(memberVarType, vectorSize, memberVar, xmlElementVar)  \
  { \
    memberVarType tmpValue[vectorSize+1] = {0}; /* try to read one more value to detect if more values are specified */ \
    if ( xmlElementVar->GetAttribute(#memberVar) ) \
    { \
      if ( xmlElementVar->GetVectorAttribute(#memberVar, vectorSize+1, tmpValue) == vectorSize)  \
      { \
        for(int i = 0; i < vectorSize+1; ++i) \
        { \
          memberVar[i] = tmpValue[i]; \
        } \
      } \
      else \
      { \
        LOG_ERROR("Unable to parse " << #memberVar << " attribute in " << (xmlElementVar->GetName() ? xmlElementVar->GetName() : "(undefined)") \
        <<" element in device set configuration. Expected exactly " << vectorSize << " values separated by spaces, instead got this: "<< \
        xmlElementVar->GetAttribute(#memberVar));  \
        return PLUS_FAIL; \
      } \
    } \
  }

// Read a vector of numeric attributes and save it to a class member variable. If not found then do not change the value and log warning.
#define XML_READ_VECTOR_ATTRIBUTE_WARNING(memberVarType, vectorSize, memberVar, xmlElementVar)  \
  { \
    memberVarType tmpValue[vectorSize] = {0}; \
    if ( xmlElementVar->GetVectorAttribute(#memberVar, vectorSize, tmpValue) )  \
    { \
      this->Set##memberVar(tmpValue); \
    } \
    else \
    { \
      LOG_WARNING("Unable to find required " << #memberVar << " attribute in " << (xmlElementVar->GetName() ? xmlElementVar->GetName() : "(undefined)") << " element in device set configuration");  \
    } \
  }

// Read a vector of numeric attributes and save it to a class member variable. If not found then return with failure.
#define XML_READ_VECTOR_ATTRIBUTE_REQUIRED(memberVarType, vectorSize, memberVar, xmlElementVar)  \
  { \
    memberVarType tmpValue[vectorSize] = {0}; \
    if ( xmlElementVar->GetVectorAttribute(#memberVar, vectorSize, tmpValue) )  \
    { \
      this->Set##memberVar(tmpValue); \
    } \
    else \
    { \
      LOG_ERROR("Unable to find required " << #memberVar << " attribute in " << (xmlElementVar->GetName() ? xmlElementVar->GetName() : "(undefined)") << " element in device set configuration");  \
      return PLUS_FAIL; \
    } \
  }

// Read a vector of numeric attributes and save it to a variable
#define XML_READ_VECTOR_ATTRIBUTE_NONMEMBER_OPTIONAL(varType, vectorSize, attributeName, var, xmlElementVar)  \
  { \
    varType tmpValue[vectorSize]; \
    if ( xmlElementVar->GetVectorAttribute(#attributeName, vectorSize, tmpValue) )  \
    { \
      memcpy(var, tmpValue, sizeof(varType)*vectorSize); \
    } \
  }

// Read a vector of numeric attributes and save it to a variable
#define XML_READ_VECTOR_ATTRIBUTE_NONMEMBER_REQUIRED(varType, vectorSize, attributeName, var, xmlElementVar)  \
  { \
    varType tmpValue[vectorSize]; \
    if ( xmlElementVar->GetVectorAttribute(#attributeName, vectorSize, tmpValue) )  \
    { \
      memcpy(var, tmpValue, sizeof(varType)*vectorSize); \
    } \
    else \
    { \
      LOG_WARNING("Unable to find required " << #attributeName << " attribute in " << (xmlElementVar->GetName() ? xmlElementVar->GetName() : "(undefined)") << " element in device set configuration");  \
    } \
  }

// Read a bool attribute (TRUE/FALSE) and save it to a class member variable. If not found do not change it.
#define XML_READ_BOOL_ATTRIBUTE_OPTIONAL(memberVar, xmlElementVar)  \
  { \
    const char* strValue = xmlElementVar->GetAttribute(#memberVar); \
    if (strValue != NULL) \
    { \
      if (PlusCommon::IsEqualInsensitive(strValue, "TRUE"))  \
      { \
        this->Set##memberVar(true); \
      } \
      else if (PlusCommon::IsEqualInsensitive(strValue, "FALSE"))  \
      { \
        this->Set##memberVar(false);  \
      } \
      else  \
      { \
        LOG_WARNING("Failed to read boolean value from " << #memberVar \
          <<" attribute of element " << (xmlElementVar->GetName() ? xmlElementVar->GetName() : "(undefined)") \
          <<": expected 'TRUE' or 'FALSE', got '" << strValue << "'"); \
      } \
    } \
  }

// Read a bool attribute (TRUE/FALSE) and save it to a variable
#define XML_READ_BOOL_ATTRIBUTE_NONMEMBER_OPTIONAL(attributeName, var, xmlElementVar)  \
    { \
      const char* strValue = xmlElementVar->GetAttribute(#attributeName); \
      if (strValue != NULL) \
      { \
        if (PlusCommon::IsEqualInsensitive(strValue, "TRUE"))  \
        { \
          var = true; \
        } \
        else if (PlusCommon::IsEqualInsensitive(strValue, "FALSE"))  \
        { \
          var = false; \
        } \
        else  \
        { \
          LOG_WARNING("Failed to read boolean value from " << #attributeName \
            <<" attribute of element " << (xmlElementVar->GetName() ? xmlElementVar->GetName() : "(undefined)") \
            <<": expected 'TRUE' or 'FALSE', got '" << strValue << "'"); \
        } \
      } \
    }

// Read a bool attribute (TRUE/FALSE) and save it to a variable
#define XML_READ_BOOL_ATTRIBUTE_NONMEMBER_REQUIRED(attributeName, var, xmlElementVar)  \
    { \
      const char* strValue = xmlElementVar->GetAttribute(#attributeName); \
      if (strValue != NULL) \
      { \
        if (PlusCommon::IsEqualInsensitive(strValue, "TRUE"))  \
        { \
          var = true; \
        } \
        else if (PlusCommon::IsEqualInsensitive(strValue, "FALSE"))  \
        { \
          var = false; \
        } \
        else  \
        { \
          LOG_ERROR("Failed to read boolean value from " << #attributeName \
            <<" attribute of element " << (xmlElementVar->GetName() ? xmlElementVar->GetName() : "(undefined)") \
            <<": expected 'TRUE' or 'FALSE', got '" << strValue << "'"); \
          return PLUS_FAIL; \
        } \
      } \
      else \
      { \
        LOG_ERROR("Unable to find required " << #attributeName << " attribute in " << (xmlElementVar->GetName() ? xmlElementVar->GetName() : "(undefined)") << " element in device set configuration"); \
        return PLUS_FAIL; \
      } \
    }

#define XML_READ_ENUM1_ATTRIBUTE_OPTIONAL(memberVar, xmlElementVar, enumString1, enumValue1)  \
  { \
    const char* strValue = xmlElementVar->GetAttribute(#memberVar); \
    if (strValue != NULL) \
    { \
      if (PlusCommon::IsEqualInsensitive(strValue, enumString1))  \
      { \
        this->Set##memberVar(enumValue1); \
      } \
      else  \
      { \
        LOG_WARNING("Failed to read enumerated value from " << #memberVar \
          << " attribute of element " << (xmlElementVar->GetName() ? xmlElementVar->GetName() : "(undefined)") \
          << ": expected '" << enumString1 << "'"); \
      } \
    } \
  }

#define XML_READ_ENUM2_ATTRIBUTE_OPTIONAL(memberVar, xmlElementVar, enumString1, enumValue1, enumString2, enumValue2)  \
  { \
    const char* strValue = xmlElementVar->GetAttribute(#memberVar); \
    if (strValue != NULL) \
    { \
      if (PlusCommon::IsEqualInsensitive(strValue, enumString1))  \
      { \
        this->Set##memberVar(enumValue1); \
      } \
      else if (PlusCommon::IsEqualInsensitive(strValue, enumString2))  \
      { \
        this->Set##memberVar(enumValue2);  \
      } \
      else  \
      { \
        LOG_WARNING("Failed to read enumerated value from " << #memberVar \
          << " attribute of element " << (xmlElementVar->GetName() ? xmlElementVar->GetName() : "(undefined)") \
          << ": expected '" << enumString1 << "' or '" << enumString2 << "', got '" << strValue << "'"); \
      } \
    } \
  }

#define XML_READ_ENUM2_ATTRIBUTE_NONMEMBER_OPTIONAL(varName, var, xmlElementVar, enumString1, enumValue1, enumString2, enumValue2)  \
  { \
    const char* strValue = xmlElementVar->GetAttribute(#varName); \
    if (strValue != NULL) \
    { \
      if (PlusCommon::IsEqualInsensitive(strValue, enumString1))  \
      { \
        var = enumValue1; \
      } \
      else if (PlusCommon::IsEqualInsensitive(strValue, enumString2))  \
      { \
        var = enumValue2;  \
      } \
      else  \
      { \
        LOG_WARNING("Failed to read enumerated value from " << #varName \
          << " attribute of element " << (xmlElementVar->GetName() ? xmlElementVar->GetName() : "(undefined)") \
          << ": expected '" << enumString1 << "' or '" << enumString2 << "', got '" << strValue << "'"); \
      } \
    } \
  }

#define XML_READ_ENUM2_ATTRIBUTE_NONMEMBER_REQUIRED(varName, var, xmlElementVar, enumString1, enumValue1, enumString2, enumValue2)  \
  { \
    const char* strValue = xmlElementVar->GetAttribute(#varName); \
    if (strValue != NULL) \
    { \
      if (PlusCommon::IsEqualInsensitive(strValue, enumString1))  \
      { \
        var = enumValue1; \
      } \
      else if (PlusCommon::IsEqualInsensitive(strValue, enumString2))  \
      { \
        var = enumValue2;  \
      } \
      else  \
      { \
        LOG_ERROR("Failed to read enumerated value from " << #varName \
          << " attribute of element " << (xmlElementVar->GetName() ? xmlElementVar->GetName() : "(undefined)") \
          << ": expected '" << enumString1 << "' or '" << enumString2 << "', got '" << strValue << "'"); \
        return PLUS_FAIL; \
      } \
    } \
  }

#define XML_READ_ENUM3_ATTRIBUTE_OPTIONAL(memberVar, xmlElementVar, enumString1, enumValue1, enumString2, enumValue2, enumString3, enumValue3)  \
  { \
    const char* strValue = xmlElementVar->GetAttribute(#memberVar); \
    if (strValue != NULL) \
    { \
      if (PlusCommon::IsEqualInsensitive(strValue, enumString1))  \
      { \
        this->Set##memberVar(enumValue1); \
      } \
      else if (PlusCommon::IsEqualInsensitive(strValue, enumString2))  \
      { \
        this->Set##memberVar(enumValue2);  \
      } \
      else if (PlusCommon::IsEqualInsensitive(strValue, enumString3))  \
      { \
        this->Set##memberVar(enumValue3);  \
      } \
      else  \
      { \
        LOG_WARNING("Failed to read enumerated value from " << #memberVar \
          << " attribute of element " << (xmlElementVar->GetName() ? xmlElementVar->GetName() : "(undefined)") \
          << ": expected '" << enumString1 << "', '" << enumString2 << "', or '" << enumString3 << "', got '" << strValue << "'"); \
      } \
    } \
  }

#define XML_READ_ENUM4_ATTRIBUTE_OPTIONAL(memberVar, xmlElementVar, enumString1, enumValue1, enumString2, enumValue2, enumString3, enumValue3, enumString4, enumValue4)  \
  { \
    const char* strValue = xmlElementVar->GetAttribute(#memberVar); \
    if (strValue != NULL) \
    { \
      if (PlusCommon::IsEqualInsensitive(strValue, enumString1))  \
      { \
        this->Set##memberVar(enumValue1); \
      } \
      else if (PlusCommon::IsEqualInsensitive(strValue, enumString2))  \
      { \
        this->Set##memberVar(enumValue2);  \
      } \
      else if (PlusCommon::IsEqualInsensitive(strValue, enumString3))  \
      { \
        this->Set##memberVar(enumValue3);  \
      } \
      else if (PlusCommon::IsEqualInsensitive(strValue, enumString4))  \
      { \
        this->Set##memberVar(enumValue4);  \
      } \
      else  \
      { \
        LOG_WARNING("Failed to read enumerated value from " << #memberVar \
          << " attribute of element " << (xmlElementVar->GetName() ? xmlElementVar->GetName() : "(undefined)") \
          << ": expected '" << enumString1 << "', '" << enumString2 << "', '" << enumString3 << "', or '" << enumString4 << "', got '" << strValue << "'"); \
      } \
    } \
  }

#define XML_READ_ENUM_ATTRIBUTE_OPTIONAL(memberVar, xmlElementVar, numberToStringConverter, startValue, numberOfValues)  \
  { \
    const char* strValue = xmlElementVar->GetAttribute(#memberVar); \
    if (strValue != NULL) \
    { \
      bool strValueValid = false; \
      for (int value = startValue; value < startValue+numberOfValues; value++) \
      { \
        if (PlusCommon::IsEqualInsensitive(strValue, numberToStringConverter(value)))  \
        { \
          this->Set##memberVar(value); \
          strValueValid = true; \
          break; \
        } \
      } \
      if (!strValueValid) \
      { \
        std::ostringstream ss; \
        ss << "Failed to read enumerated value from " << #memberVar \
          << " attribute of element " << (xmlElementVar->GetName() ? xmlElementVar->GetName() : "(undefined)") \
          << ": expected "; \
        for (int value = startValue; value < startValue + numberOfValues; value++) \
        { \
          if (value > startValue) \
          { \
            ss << ", ";  \
          } \
          ss << "'" << numberToStringConverter(value) << "'";  \
        } \
        LOG_WARNING(ss.str()); \
      } \
    } \
  }

#define XML_REMOVE_ATTRIBUTE(attributeName, xmlElementVar)  xmlElementVar->RemoveAttribute(attributeName);

#define XML_WRITE_STRING_ATTRIBUTE(memberVar, xmlElementVar)  \
  xmlElementVar->SetAttribute(#memberVar, this->Get##memberVar().c_str()); \

#define XML_WRITE_STRING_ATTRIBUTE_IF_NOT_EMPTY(memberVar, xmlElementVar)  \
  if (!this->Get##memberVar().empty()) \
  { \
    xmlElementVar->SetAttribute(#memberVar, this->Get##memberVar().c_str()); \
  }

#define XML_WRITE_CSTRING_ATTRIBUTE_IF_NOT_NULL(memberVar, xmlElementVar)  \
  if (this->Get##memberVar()) \
  { \
    xmlElementVar->SetAttribute(#memberVar, this->Get##memberVar()); \
  }

#define XML_WRITE_STRING_ATTRIBUTE_REMOVE_IF_EMPTY(memberVar, xmlElementVar)  \
  if (!this->Get##memberVar().empty()) \
  { \
    xmlElementVar->SetAttribute(#memberVar, this->Get##memberVar().c_str()); \
  } \
  else \
  { \
    XML_REMOVE_ATTRIBUTE(memberVar.c_str(), xmlElementVar); \
  }

#define XML_WRITE_CSTRING_ATTRIBUTE_REMOVE_IF_NULL(memberVar, xmlElementVar)  \
  if (this->Get##memberVar()) \
  { \
    xmlElementVar->SetAttribute(#memberVar, this->Get##memberVar()); \
  } \
  else \
  { \
    XML_REMOVE_ATTRIBUTE(#memberVar, xmlElementVar); \
  }

#define XML_WRITE_BOOL_ATTRIBUTE(memberVar, xmlElementVar)  \
  { \
    if (this->Get##memberVar()) \
    { \
      xmlElementVar->SetAttribute(#memberVar, "TRUE"); \
    } \
    else \
    { \
      xmlElementVar->SetAttribute(#memberVar, "FALSE"); \
    } \
  }

#define XML_WRITE_BOOL_ATTRIBUTE_NONMEMBER(attributeName, var, xmlElementVar)  \
  { \
    if (var) \
    { \
      xmlElementVar->SetAttribute(#attributeName, "TRUE"); \
    } \
    else \
    { \
      xmlElementVar->SetAttribute(#attributeName, "FALSE"); \
    } \
  }

#define XML_READ_WARNING_DEPRECATED_CSTRING_REPLACED(memberVar, xmlElementVar, newMemberVar)  \
  { \
    const char* newAttributeName = #newMemberVar; \
    const char* strValue = xmlElementVar->GetAttribute(#memberVar); \
    if (strValue != NULL) \
    { \
      LOG_WARNING("Use of " << #memberVar << " is deprecated. Use " << #newMemberVar << " instead."); \
    } \
  }

#endif //__PlusXmlUtils_h
