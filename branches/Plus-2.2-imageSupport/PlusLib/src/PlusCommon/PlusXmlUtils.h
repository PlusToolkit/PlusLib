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

namespace PlusXmlUtils
{
  /*! Get a nested XML element with the specified name. If the element does not exist then create one. */
  vtkXMLDataElement* GetNestedElementWithName(vtkXMLDataElement* config, const char* elementName)
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
}


#endif //__PlusXmlUtils_h
