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
    if (config==NULL)
    {
      LOG_ERROR("Reading device set configuration file failed: invalid config input");
      return PLUS_FAIL;
    }
    if (filename==NULL)
    {
      LOG_ERROR("Reading device set configuration file failed: filename is not specified");
      return PLUS_FAIL;
    }

    std::string filePath=filename;
    if (!vtksys::SystemTools::FileExists(filePath.c_str(), true))
    {
      filePath = vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationPath(filename);
      if (!vtksys::SystemTools::FileExists(filePath.c_str(), true))
      {
        LOG_ERROR("Reading device set configuration file failed: "<<filename<<" does not exist in the current directory or in "<<vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationDirectory());
        return PLUS_FAIL;      
      }
    }

    vtkSmartPointer<vtkXMLDataElement> rootElement = vtkSmartPointer<vtkXMLDataElement>::Take(vtkXMLUtilities::ReadElementFromFile(filePath.c_str()));
    if (rootElement == NULL)
    {
      LOG_ERROR("Reading device set configuration file failed: syntax error in "<<filename);
      return PLUS_FAIL;
    }
    config->DeepCopy(rootElement);
    return PLUS_SUCCESS;
  }

  /*! Get a nested XML element with the specified name. If the element does not exist then create one. */
  static vtkXMLDataElement* GetNestedElementWithName(vtkXMLDataElement* config, const char* elementName)
  {
    if (config==NULL)
    {
      LOG_ERROR("PlusXmlUtils::GetNestedElementWithName failed: config is invalid");
      return NULL;
    }
    if (elementName==NULL)
    {
      LOG_ERROR("PlusXmlUtils::GetNestedElementWithName failed: elementName is invalid");
      return NULL;
    }    
    vtkXMLDataElement* nestedElement = config->FindNestedElementWithName(elementName); 
    if (nestedElement!=NULL)
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
      LOG_ERROR("PlusXmlUtils::GetNestedElementWithName failed: cannot add nested element with name "<<elementName);
    }
    return nestedElement;
  }
};

#define XML_FIND_NESTED_ELEMENT_OPTIONAL(destinationXmlElementVar, rootXmlElementVar, nestedXmlElementName) \
  if (rootXmlElementVar == NULL) \
  { \
    LOG_ERROR("Invalid device set configuration found while looking for optional "<<nestedXmlElementName<<" element"); \
    return PLUS_FAIL; \
  } \
  vtkXMLDataElement* destinationXmlElementVar = rootXmlElementVar->FindNestedElementWithName(nestedXmlElementName);

#define XML_FIND_NESTED_ELEMENT_REQUIRED(destinationXmlElementVar, rootXmlElementVar, nestedXmlElementName) \
  if (rootXmlElementVar == NULL) \
  { \
    LOG_ERROR("Invalid device set configuration: unable to find required "<<nestedXmlElementName<<" element"); \
    return PLUS_FAIL; \
  } \
  vtkXMLDataElement* destinationXmlElementVar = rootXmlElementVar->FindNestedElementWithName(nestedXmlElementName);  \
  if (destinationXmlElementVar == NULL)  \
  { \
    LOG_ERROR("Unable to find required "<<nestedXmlElementName<<" element in device set configuration");  \
    return PLUS_FAIL; \
  }

#define XML_FIND_NESTED_ELEMENT_CREATE_IF_MISSING(destinationXmlElementVar, rootXmlElementVar, nestedXmlElementName) \
  if (rootXmlElementVar == NULL) \
  { \
    LOG_ERROR("Invalid device set configuration: unable to find required "<<nestedXmlElementName<<" element"); \
    return PLUS_FAIL; \
  } \
  vtkXMLDataElement* destinationXmlElementVar = PlusXmlUtils::GetNestedElementWithName(rootXmlElementVar,nestedXmlElementName);  \
  if (destinationXmlElementVar == NULL)  \
  { \
    LOG_ERROR("Unable to find or create "<<nestedXmlElementName<<" element in device set configuration");  \
    return PLUS_FAIL; \
  }
  
#define XML_VERIFY_ELEMENT(xmlElementVar, expectedXmlElementName) \
  if (xmlElementVar == NULL) \
  { \
    LOG_ERROR("Missing or invalid "<<expectedXmlElementName<<" element"); \
    return PLUS_FAIL; \
  } \
  if ( xmlElementVar->GetName() == NULL || STRCASECMP(xmlElementVar->GetName(),expectedXmlElementName)!=0)  \
  { \
    LOG_ERROR("Unable to read "<<expectedXmlElementName<<" element: unexpected name: "<<(xmlElementVar->GetName()?xmlElementVar->GetName():"(unspecified)")); \
    return PLUS_FAIL; \
  }
  
/*
#define XML_GET_ATTRIBUTE_REQUIRED(destinationXmlElementVar, xmlElementVar, attributeName)  \
  const char* destinationXmlElementVar = xmlElementVar->GetAttribute(attributeName);  \
  if (destinationXmlElementVar == NULL)  \
  { \
    LOG_ERROR("Unable to find required "<<attributeName<<" attribute in "<<(xmlElementVar->GetName()?xmlElementVar->GetName():"(undefined)")<<" element in device set configuration");  \
    return PLUS_FAIL; \
  }
*/

// Read a string attribute and save it to a class member variable. If not found return with fail.
#define XML_READ_STRING_ATTRIBUTE_REQUIRED(memberVar, xmlElementVar)  \
  { \
    const char* attributeName = #memberVar; \
    const char* destinationXmlElementVar = xmlElementVar->GetAttribute(attributeName);  \
    if (destinationXmlElementVar == NULL)  \
    { \
      LOG_ERROR("Unable to find required "<<attributeName<<" attribute in "<<(xmlElementVar->GetName()?xmlElementVar->GetName():"(undefined)")<<" element in device set configuration");  \
      return PLUS_FAIL; \
    } \
    this->Set##memberVar(destinationXmlElementVar);  \
  }

// Read a numeric attribute and save it to a class member variable. If not found then no change.
#define XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(memberVarType, memberVar, xmlElementVar)  \
  { \
    const char* attributeName = #memberVar; \
    memberVarType tmpValue = 0; \
    if ( xmlElementVar->GetScalarAttribute(attributeName, tmpValue) )  \
    { \
      this->Set##memberVar(tmpValue); \
    } \
  }

// Read a numeric attribute and save it to a class member variable. If not found then log warning and make no change.
#define XML_READ_SCALAR_ATTRIBUTE_WARNING(memberVarType, memberVar, xmlElementVar)  \
  { \
    const char* attributeName = #memberVar; \
    memberVarType tmpValue = 0; \
    if ( xmlElementVar->GetScalarAttribute(attributeName, tmpValue) )  \
    { \
      this->Set##memberVar(tmpValue); \
    } \
    else \
    { \
      LOG_WARNING("Unable to find expected "<<attributeName<<" attribute in "<<(xmlElementVar->GetName()?xmlElementVar->GetName():"(undefined)")<<" element in device set configuration");  \
    } \
  }

// Read a numeric attribute and save it to a class member variable. If not found then return with error.
#define XML_READ_SCALAR_ATTRIBUTE_REQUIRED(memberVarType, memberVar, xmlElementVar)  \
  { \
    const char* attributeName = #memberVar; \
    memberVarType tmpValue = 0; \
    if ( xmlElementVar->GetScalarAttribute(attributeName, tmpValue) )  \
    { \
      this->Set##memberVar(tmpValue); \
    } \
    else  \
    { \
      LOG_ERROR("Unable to find required "<<attributeName<<" attribute in "<<(xmlElementVar->GetName()?xmlElementVar->GetName():"(undefined)")<<" element in device set configuration");  \
      return PLUS_FAIL; \
    } \
  }

// Read a vector of numeric attributes and save it to a class member variable. If not found then no change.
#define XML_READ_VECTOR_ATTRIBUTE_OPTIONAL(memberVarType, vectorSize, memberVar, xmlElementVar)  \
  { \
    const char* attributeName = #memberVar; \
    memberVarType tmpValue[vectorSize] = {0}; \
    if ( xmlElementVar->GetVectorAttribute(attributeName, vectorSize, tmpValue) )  \
    { \
      this->Set##memberVar(tmpValue); \
    } \
  }

// Read a vector of numeric attributes and save it to a class member variable. If not found then no change.
// If number of parameters in the attribute is not exactly the same as expected then return with error.
#define XML_READ_VECTOR_ATTRIBUTE_EXACT_OPTIONAL(memberVarType, vectorSize, memberVar, xmlElementVar)  \
  { \
    const char* attributeName = #memberVar; \
    memberVarType tmpValue[vectorSize+1] = {0}; /* try to read one more value to detect if more values are specified */ \
    if ( xmlElementVar->GetAttribute(attributeName) ) \
    { \
      if ( xmlElementVar->GetVectorAttribute(attributeName, vectorSize+1, tmpValue) == vectorSize)  \
      { \
        this->Set##memberVar(tmpValue); \
      } \
      else \
      { \
        LOG_ERROR("Unable to parse "<<attributeName<<" attribute in "<<(xmlElementVar->GetName()?xmlElementVar->GetName():"(undefined)") \
        <<" element in device set configuration. Expected exactly "<<vectorSize<<" values separated by spaces, instead got this: "<< \
        xmlElementVar->GetAttribute(attributeName));  \
        return PLUS_FAIL; \
      } \
    } \
  }

// Read a vector of numeric attributes and save it to a class member variable. If not found then do not change the value and log warning.
#define XML_READ_VECTOR_ATTRIBUTE_WARNING(memberVarType, vectorSize, memberVar, xmlElementVar)  \
  { \
    const char* attributeName = #memberVar; \
    memberVarType tmpValue[vectorSize] = {0}; \
    if ( xmlElementVar->GetVectorAttribute(attributeName, vectorSize, tmpValue) )  \
    { \
      this->Set##memberVar(tmpValue); \
    } \
    else \
    { \
      LOG_WARNING("Unable to find required "<<attributeName<<" attribute in "<<(xmlElementVar->GetName()?xmlElementVar->GetName():"(undefined)")<<" element in device set configuration");  \
    } \
  }

// Read a vector of numeric attributes and save it to a class member variable. If not found then return with failure.
#define XML_READ_VECTOR_ATTRIBUTE_REQUIRED(memberVarType, vectorSize, memberVar, xmlElementVar)  \
  { \
    const char* attributeName = #memberVar; \
    memberVarType tmpValue[vectorSize] = {0}; \
    if ( xmlElementVar->GetVectorAttribute(attributeName, vectorSize, tmpValue) )  \
    { \
      this->Set##memberVar(tmpValue); \
    } \
    else \
    { \
      LOG_ERROR("Unable to find required "<<attributeName<<" attribute in "<<(xmlElementVar->GetName()?xmlElementVar->GetName():"(undefined)")<<" element in device set configuration");  \
      return PLUS_FAIL; \
    } \
  }

  
// Read a string attribute (with the same name as the class member variable) and save it to a class member variable.
// If attribute not found then the member is not modified but a warning is logged.
#define XML_READ_STRING_ATTRIBUTE_WARNING(memberVar, xmlElementVar)  \
  { \
    const char* attributeName = #memberVar; \
    const char* destinationXmlElementVar = xmlElementVar->GetAttribute(attributeName);  \
    if (destinationXmlElementVar != NULL)  \
    { \
      this->Set##memberVar(destinationXmlElementVar);  \
    } \
    else \
    { \
      LOG_WARNING("Unable to find expected "<<attributeName<<" attribute in "<<(xmlElementVar->GetName()?xmlElementVar->GetName():"(undefined)")<<" element in device set configuration");  \
    } \
  }

// Read a string attribute (with the same name as the class member variable) and save it to a class member variable.
// If attribute not found then the member is not modified.
#define XML_READ_STRING_ATTRIBUTE_OPTIONAL(memberVar, xmlElementVar)  \
  { \
    const char* attributeName = #memberVar; \
    const char* destinationXmlElementVar = xmlElementVar->GetAttribute(attributeName);  \
    if (destinationXmlElementVar != NULL)  \
    { \
      this->Set##memberVar(destinationXmlElementVar);  \
    } \
  }

// Read a bool attribute (TRUE/FALSE) and save it to a class member variable. If not found do not change it.
#define XML_READ_BOOL_ATTRIBUTE_OPTIONAL(memberVar, xmlElementVar)  \
  { \
    const char* attributeName = #memberVar; \
    const char* strValue = xmlElementVar->GetAttribute(attributeName); \
    if (strValue != NULL) \
    { \
      if (STRCASECMP(strValue, "TRUE")==0)  \
      { \
        this->Set##memberVar(true); \
      } \
      else if (STRCASECMP(strValue, "FALSE")==0)  \
      { \
        this->Set##memberVar(false);  \
      } \
      else  \
      { \
        LOG_WARNING("Failed to read boolean value from "<<attributeName \
          <<" attribute of element "<<(xmlElementVar->GetName()?xmlElementVar->GetName():"(undefined)") \
          <<": expected 'TRUE' or 'FALSE', got '"<<strValue<<"'"); \
      } \
    } \
  }

#define XML_READ_ENUM1_ATTRIBUTE_OPTIONAL(memberVar, xmlElementVar, enumString1, enumValue1)  \
  { \
    const char* attributeName = #memberVar; \
    const char* strValue = xmlElementVar->GetAttribute(attributeName); \
    if (strValue != NULL) \
    { \
      if (STRCASECMP(strValue, enumString1)==0)  \
      { \
        this->Set##memberVar(enumValue1); \
      } \
      else  \
      { \
        LOG_WARNING("Failed to read enumerated value from "<<attributeName \
          <<" attribute of element "<<(xmlElementVar->GetName()?xmlElementVar->GetName():"(undefined)") \
          <<": expected '"<<enumString1<<"'"); \
      } \
    } \
  }

#define XML_READ_ENUM2_ATTRIBUTE_OPTIONAL(memberVar, xmlElementVar, enumString1, enumValue1, enumString2, enumValue2)  \
  { \
    const char* attributeName = #memberVar; \
    const char* strValue = xmlElementVar->GetAttribute(attributeName); \
    if (strValue != NULL) \
    { \
      if (STRCASECMP(strValue, enumString1)==0)  \
      { \
        this->Set##memberVar(enumValue1); \
      } \
      else if (STRCASECMP(strValue, enumString2)==0)  \
      { \
        this->Set##memberVar(enumValue2);  \
      } \
      else  \
      { \
        LOG_WARNING("Failed to read enumerated value from "<<attributeName \
          <<" attribute of element "<<(xmlElementVar->GetName()?xmlElementVar->GetName():"(undefined)") \
          <<": expected '"<<enumString1<<"' or '"<<enumString2<<"', got '"<<strValue<<"'"); \
      } \
    } \
  }

#define XML_READ_ENUM3_ATTRIBUTE_OPTIONAL(memberVar, xmlElementVar, enumString1, enumValue1, enumString2, enumValue2, enumString3, enumValue3)  \
  { \
    const char* attributeName = #memberVar; \
    const char* strValue = xmlElementVar->GetAttribute(attributeName); \
    if (strValue != NULL) \
    { \
      if (STRCASECMP(strValue, enumString1)==0)  \
      { \
        this->Set##memberVar(enumValue1); \
      } \
      else if (STRCASECMP(strValue, enumString2)==0)  \
      { \
        this->Set##memberVar(enumValue2);  \
      } \
      else if (STRCASECMP(strValue, enumString3)==0)  \
      { \
        this->Set##memberVar(enumValue3);  \
      } \
      else  \
      { \
        LOG_WARNING("Failed to read enumerated value from "<<attributeName \
          <<" attribute of element "<<(xmlElementVar->GetName()?xmlElementVar->GetName():"(undefined)") \
          <<": expected '"<<enumString1<<"', '"<<enumString2<<"', or '"<<enumString3<<"', got '"<<strValue<<"'"); \
      } \
    } \
  }

#if (VTK_MAJOR_VERSION < 6)
    // Workaround for RemoveAttribute bug in VTK5 (https://www.assembla.com/spaces/plus/tickets/859)
  #define XML_REMOVE_ATTRIBUTE(xmlElementVar, attributeName)  PlusCommon::RemoveAttribute(xmlElementVar, attributeName);
#else
  #define XML_REMOVE_ATTRIBUTE(xmlElementVar, attributeName)  xmlElementVar->RemoveAttribute(attributeName);
#endif

#define XML_WRITE_STRING_ATTRIBUTE_IF_NOT_NULL(memberVar, xmlElementVar)  \
  if (this->Get##memberVar()) \
  { \
    const char* attributeName = #memberVar; \
    xmlElementVar->SetAttribute(attributeName, this->Get##memberVar()); \
  }

#define XML_WRITE_STRING_ATTRIBUTE_REMOVE_IF_NULL(memberVar, xmlElementVar)  \
  if (this->Get##memberVar()) \
  { \
    const char* attributeName = #memberVar; \
    xmlElementVar->SetAttribute(attributeName, this->Get##memberVar()); \
  } \
  else \
  { \
    XML_REMOVE_ATTRIBUTE(xmlElementVar, #memberVar); \
  }

  
#define XML_WRITE_BOOL_ATTRIBUTE(memberVar, xmlElementVar)  \
  { \
  const char* attributeName = #memberVar; \
    if (this->Get##memberVar()) \
    { \
      xmlElementVar->SetAttribute(attributeName, "TRUE"); \
    } \
    else \
    { \
      xmlElementVar->SetAttribute(attributeName, "FALSE"); \
    } \
  }

#define XML_READ_WARNING_DEPRECATED_STRING_REPLACED(memberVar, xmlElementVar, newMemberVar)  \
  { \
    const char* attributeName = #memberVar; \
    const char* newAttributeName = #newMemberVar; \
    const char* strValue = xmlElementVar->GetAttribute(attributeName); \
    if (strValue != NULL) \
    { \
      LOG_WARNING("Use of "<<attributeName \
        <<" is deprecated. Use "<<newAttributeName<<" instead."); \
    } \
  }
  
#endif //__PlusXmlUtils_h
