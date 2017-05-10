/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

namespace PlusCommon
{
  namespace XML
  {
    template<typename T>
    PlusStatus SafeGetAttributeValueInsensitive(vtkXMLDataElement& element, const std::wstring& attributeName, T& value)
    {
      return PlusCommon::XML::SafeGetAttributeValueInsensitive(element, std::string(begin(attributeName), end(attributeName)), value);
    }

    template<typename T>
    PlusStatus SafeGetAttributeValueInsensitive(vtkXMLDataElement& element, const std::string& attributeName, T& value)
    {
      if (element.GetAttribute(attributeName.c_str()) == NULL)
      {
        return PLUS_FAIL;
      }
      element.GetScalarAttribute(attributeName.c_str(), value);
      return PLUS_SUCCESS;
    }
  }
}