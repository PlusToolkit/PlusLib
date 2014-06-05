/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __PlusXmlUtils_h
#define __PlusXmlUtils_h

#include "vtkOutputWindow.h"
#include "vtkPlusLogger.h"
#include "itkImageIOBase.h"

/*!
  \class PlusXmlUtils
  \brief Utility methods for XML tree manipulation
  \ingroup PlusLibCommon
*/

class PlusXmlUtils
{
public:
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

#define DSC_FIND_NESTED_ELEMENT_OPTIONAL(destinationElementVar, rootElementVar, nestedElementName) \
  if (rootElementVar == NULL) \
  { \
    LOG_ERROR("Invalid device set configuration found while looking for optional "<<nestedElementName<<" element"); \
    return PLUS_FAIL; \
  } \
  vtkXMLDataElement* destinationElementVar = rootElementVar->FindNestedElementWithName(nestedElementName);

#define DSC_FIND_NESTED_ELEMENT_REQUIRED(destinationElementVar, rootElementVar, nestedElementName) \
  if (rootElementVar == NULL) \
  { \
    LOG_ERROR("Invalid device set configuration: unable to find required "<<nestedElementName<<" element"); \
    return PLUS_FAIL; \
  } \
  vtkXMLDataElement* destinationElementVar = rootElementVar->FindNestedElementWithName(nestedElementName);  \
  if (destinationElementVar == NULL)  \
  { \
    LOG_ERROR("Unable to find required "<<nestedElementName<<" element in device set configuration");  \
    return PLUS_FAIL; \
  }

/*
#define DSC_GET_ATTRIBUTE_REQUIRED(destinationElementVar, elementVar, attributeName)  \
  const char* destinationElementVar = elementVar->GetAttribute(attributeName);  \
  if (destinationElementVar == NULL)  \
  { \
    LOG_ERROR("Unable to find required "<<attributeName<<" attribute in "<<(elementVar->GetName()?elementVar->GetName():"(undefined)")<<" element in device set configuration");  \
    return PLUS_FAIL; \
  }
*/

// Read a string attribute and save it to a class member variable. If not found return with fail.
#define DSC_READ_STRING_ATTRIBUTE_REQUIRED(memberVar, elementVar)  \
  { \
    const char* attributeName = #memberVar; \
    const char* destinationElementVar = elementVar->GetAttribute(attributeName);  \
    if (destinationElementVar == NULL)  \
    { \
      LOG_ERROR("Unable to find required "<<attributeName<<" attribute in "<<(elementVar->GetName()?elementVar->GetName():"(undefined)")<<" element in device set configuration");  \
      return PLUS_FAIL; \
    } \
    this->Set##memberVar(destinationElementVar);  \
  }

// Read a numeric attribute and save it to a class member variable. If not found then no change.
#define DSC_READ_SCALAR_ATTRIBUTE_OPTIONAL(memberVarType, memberVar, elementVar)  \
  { \
    const char* attributeName = #memberVar; \
    memberVarType tmpValue = 0; \
    if ( elementVar->GetScalarAttribute(attributeName, tmpValue) )  \
    { \
      this->Set##memberVar(tmpValue); \
    } \
  }

// Read a numeric attribute and save it to a class member variable. If not found then return with error.
#define DSC_READ_SCALAR_ATTRIBUTE_REQUIRED(memberVarType, memberVar, elementVar)  \
  { \
    const char* attributeName = #memberVar; \
    memberVarType tmpValue = 0; \
    if ( elementVar->GetScalarAttribute(attributeName, tmpValue) )  \
    { \
      this->Set##memberVar(tmpValue); \
    } \
    else  \
    { \
      LOG_ERROR("Unable to find required "<<attributeName<<" attribute in "<<(elementVar->GetName()?elementVar->GetName():"(undefined)")<<" element in device set configuration");  \
      return PLUS_FAIL; \
    } \
  }

// Read a vector of numeric attributes and save it to a class member variable. If not found then no change.
#define DSC_READ_VECTOR_ATTRIBUTE_OPTIONAL(memberVarType, vectorSize, memberVar, elementVar)  \
  { \
    const char* attributeName = #memberVar; \
    memberVarType tmpValue[vectorSize] = {0}; \
    if ( elementVar->GetVectorAttribute(attributeName, vectorSize, tmpValue) )  \
    { \
      this->Set##memberVar(tmpValue); \
    } \
  }

// Read a string attribute (with the same name as the class member variable) and save it to a class member variable.
// If attribute not found then the member is not modified but a warning is logged.
#define DSC_READ_STRING_ATTRIBUTE_WARNING(memberVar, elementVar)  \
  { \
    const char* attributeName = #memberVar; \
    const char* destinationElementVar = elementVar->GetAttribute(attributeName);  \
    if (destinationElementVar != NULL)  \
    { \
      this->Set##memberVar(destinationElementVar);  \
    } \
    else \
    { \
      LOG_WARNING("Unable to find expected "<<attributeName<<" attribute in "<<(elementVar->GetName()?elementVar->GetName():"(undefined)")<<" element in device set configuration");  \
    } \
  }

// Read a string attribute (with the same name as the class member variable) and save it to a class member variable.
// If attribute not found then the member is not modified.
#define DSC_READ_STRING_ATTRIBUTE_OPTIONAL(memberVar, elementVar)  \
  { \
    const char* attributeName = #memberVar; \
    const char* destinationElementVar = elementVar->GetAttribute(attributeName);  \
    if (destinationElementVar != NULL)  \
    { \
      this->Set##memberVar(destinationElementVar);  \
    } \
  }

// Read a bool attribute (TRUE/FALSE) and save it to a class member variable. If not found do not change it.
#define DSC_READ_BOOL_ATTRIBUTE_OPTIONAL(memberVar, elementVar)  \
  { \
    const char* attributeName = #memberVar; \
    const char* strValue = elementVar->GetAttribute(attributeName); \
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
          <<" attribute of element "<<(elementVar->GetName()?elementVar->GetName():"(undefined)") \
          <<" expected 'TRUE' or 'FALSE', got '"<<strValue<<"'"); \
      } \
    } \
  }

#define DSC_READ_ENUM2_ATTRIBUTE_OPTIONAL(memberVar, elementVar, enumString1, enumValue1, enumString2, enumValue2)  \
  { \
    const char* attributeName = #memberVar; \
    const char* strValue = elementVar->GetAttribute(attributeName); \
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
          <<" attribute of element "<<(elementVar->GetName()?elementVar->GetName():"(undefined)") \
          <<" expected '"<<enumString1<<"' or '"<<enumString2<<"', got '"<<strValue<<"'"); \
      } \
    } \
  }

#endif //__PlusXmlUtils_h
