#ifndef __ChrDataItem_h
#define __ChrDataItem_h

#include "vtkXMLDataElement.h"
#include "ChrSerialPacket.h"

class ChrDataItem
{
public:
  ChrDataItem()
  {
    m_Address = 0;
    m_Bits = 0;
    m_StartBit = 0;
    m_DataType=CHR_INT16;
    m_ScaleFactor=1.0;

    m_Value=0.0;
  }

  PlusStatus ReadDescriptionFromXml(vtkXMLDataElement* dataItemElem)
  {
    PlusStatus status=PLUS_SUCCESS;    

    if (GetStringValueFromChildElement("Name",m_Name,dataItemElem)!=PLUS_SUCCESS)
    {
      status=PLUS_FAIL;
    }
    if (GetUnsignedCharValueFromChildElement("Address",m_Address,dataItemElem)!=PLUS_SUCCESS)
    {
      status=PLUS_FAIL;
    }
    if (GetUnsignedCharValueFromChildElement("Bits",m_Bits,dataItemElem)!=PLUS_SUCCESS)
    {
      status=PLUS_FAIL;
    }
    if (GetUnsignedCharValueFromChildElement("Start",m_StartBit,dataItemElem)!=PLUS_SUCCESS)
    {
      status=PLUS_FAIL;
    }
    if (GetDoubleValueFromChildElement("ScaleFactor",m_ScaleFactor,dataItemElem)!=PLUS_SUCCESS)
    {
      status=PLUS_FAIL;
    }
    std::string typeStr;
    if (GetStringValueFromChildElement("DataType",typeStr,dataItemElem)!=PLUS_SUCCESS)
    {
      status=PLUS_FAIL;
    }
    else
    {
      if (typeStr.compare("int16")==0)
      {
        m_DataType=CHR_INT16;
      }
      else if (typeStr.compare("uint16")==0)
      {
        m_DataType=CHR_UINT16;
      }
      else if (typeStr.compare("float")==0)
      {
        m_DataType=CHR_FLOAT;
      }
      else if (typeStr.compare("en/dis")==0)
      {
        m_DataType=CHR_EN_DIS;
      }
      else if (typeStr.compare("binary")==0)
      {
        m_DataType=CHR_BINARY;
      }
      else
      {
        LOG_ERROR("Unknown data type: "<<typeStr);
        status=PLUS_FAIL;
      }      
    }
      
    return status;
  }

  int GetByteAddressFromBitAddress(unsigned int bitAddress)
  {
    //return bitAddress/8;
    unsigned int dwordAddress=(bitAddress/32)*4;
    unsigned int remainingOffsetBit=bitAddress%32;
    if (remainingOffsetBit<8)
    {
      return dwordAddress+2;
    }
    if (remainingOffsetBit<16)
    {
      return dwordAddress+3;
    }
    if (remainingOffsetBit<24)
    {
      return dwordAddress+0;
    }
    // remainingOffsetBit<32
    return dwordAddress+1;
  }

  bool ReadValueFromPacket(ChrSerialPacket& packet)
  {
    switch (this->m_DataType)
    {
    case CHR_INT16:
      {
        int dataStartByteAddress=m_Address*4+GetByteAddressFromBitAddress(m_StartBit);
        int dataEndByteAddress=m_Address*4+GetByteAddressFromBitAddress(m_StartBit+m_Bits-1);
        int packetStartByteAddress=packet.GetAddress()*4;
        int packetEndByteAddress=packet.GetAddress()*4+packet.GetDataLength();
        if (dataStartByteAddress<packetStartByteAddress
          || dataEndByteAddress>packetEndByteAddress)
        {
          // data is not in the packet
          return false;
        }
        int intValue=int(packet.GetDataByte(dataStartByteAddress-packetStartByteAddress))*256
          +int(packet.GetDataByte(dataStartByteAddress-packetStartByteAddress+1));
        m_Value=intValue*m_ScaleFactor;
        return true;        
      }
    default:
      LOG_ERROR("Unsupported data type: "<<this->m_DataType);
      return false;
    }
  }

  double GetValue() { return m_Value; }

  enum CHR_DATA_TYPE
  {
    CHR_INT16,
    CHR_UINT16,
    CHR_FLOAT,
    CHR_EN_DIS,
    CHR_BINARY    
  };
  
private:

  PlusStatus GetUnsignedCharValueFromChildElement(const std::string &childElemName, unsigned char &childElemValue, vtkXMLDataElement* dataElem)
  {
    vtkXMLDataElement* childElem=dataElem->FindNestedElementWithName(childElemName.c_str());
    if (childElem==NULL)
    {
      LOG_ERROR("Child element name "<<childElemName<<" not found");
      return PLUS_FAIL;
    }
    if (PlusCommon::StringToInt<unsigned char>(childElem->GetCharacterData(), childElemValue)!=PLUS_SUCCESS)
    {
      LOG_ERROR("Failed to read integer value from element "<<childElemName);
      return PLUS_FAIL;
    }
    return PLUS_SUCCESS;
  }

  PlusStatus GetDoubleValueFromChildElement(const std::string &childElemName, double &childElemValue, vtkXMLDataElement* dataElem)
  {
    vtkXMLDataElement* childElem=dataElem->FindNestedElementWithName(childElemName.c_str());
    if (childElem==NULL)
    {
      LOG_ERROR("Child element name "<<childElemName<<" not found");
      return PLUS_FAIL;
    }
    if (PlusCommon::StringToDouble(childElem->GetCharacterData(), childElemValue)!=PLUS_SUCCESS)
    {
      LOG_ERROR("Failed to read double value from element "<<childElemName);
      return PLUS_FAIL;
    }
    return PLUS_SUCCESS;
  }

  PlusStatus GetStringValueFromChildElement(const std::string &childElemName, std::string &childElemValue, vtkXMLDataElement* dataElem)
  {
    vtkXMLDataElement* childElem=dataElem->FindNestedElementWithName(childElemName.c_str());
    if (childElem==NULL)
    {
      LOG_ERROR("Child element name "<<childElemName<<" not found");
      return PLUS_FAIL;
    }
    const char* s=childElem->GetCharacterData();
    if (s==NULL)
    {
      LOG_ERROR("Failed to string value from element "<<childElemName);
      return PLUS_FAIL;
    }
    childElemValue=s;
    return PLUS_SUCCESS;
  }

  std::string m_Name;
  unsigned char m_Address;
  unsigned char m_Bits;
  unsigned char m_StartBit;
  CHR_DATA_TYPE m_DataType;
  double m_ScaleFactor;

  double m_Value;
};

#endif
