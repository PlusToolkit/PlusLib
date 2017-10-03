/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

namespace PlusCommon
{
  namespace XML
  {
    //----------------------------------------------------------------------------
    template<typename T>
    PlusStatus SafeGetAttributeValueInsensitive(vtkXMLDataElement& element, const std::wstring& attributeName, T& value)
    {
      return PlusCommon::XML::SafeGetAttributeValueInsensitive(element, std::string(begin(attributeName), end(attributeName)), value);
    }

    //----------------------------------------------------------------------------
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

  //----------------------------------------------------------------------------
  template<typename ElemType>
  void JoinTokensIntoString(const std::vector<ElemType>& elems, std::string& output, char separator)
  {
    output = "";
    std::stringstream ss;
    typedef std::vector<ElemType> List;

    for (typename List::const_iterator it = elems.begin(); it != elems.end(); ++it)
    {
      ss << *it;
      if (it != elems.end() - 1)
      {
        ss << separator;
      }
    }

    output = ss.str();
  }
}