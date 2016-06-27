/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

#include "PlusConfigure.h"
#include "igtlPlusClientInfoMessage.h"
#include "igtlutil/igtl_header.h"
#include "igtlutil/igtl_util.h"
#include "vtkPlusIgtlMessageFactory.h"

namespace igtl 
{

//----------------------------------------------------------------------------
PlusClientInfoMessage::PlusClientInfoMessage() : StringMessage()
{
  this->m_SendMessageType = "CLIENTINFO";
}

//----------------------------------------------------------------------------
PlusClientInfoMessage::~PlusClientInfoMessage()
{
}

//----------------------------------------------------------------------------
igtl::MessageBase::Pointer PlusClientInfoMessage::Clone()
{
  igtl::PlusClientInfoMessage::Pointer clone;
  {
    vtkSmartPointer<vtkPlusIgtlMessageFactory> factory = vtkSmartPointer<vtkPlusIgtlMessageFactory>::New();
    clone = dynamic_cast<igtl::PlusClientInfoMessage*>(factory->CreateSendMessage(this->GetMessageType(), this->GetHeaderVersion()).GetPointer());
  }

  int bodySize = this->m_MessageSize - IGTL_HEADER_SIZE;
  clone->InitBuffer();
  clone->CopyHeader(this);
  clone->AllocateBuffer(bodySize);
  if (bodySize > 0)
  {
    clone->CopyBody(this);
  }

#if OpenIGTLink_HEADER_VERSION >= 2
  clone->m_MetaDataHeader = this->m_MetaDataHeader;
  clone->m_MetaDataMap = this->m_MetaDataMap;
  clone->m_IsExtendedHeaderUnpacked = this->m_IsExtendedHeaderUnpacked;
#endif

  return clone;
}

//----------------------------------------------------------------------------
void PlusClientInfoMessage::SetClientInfo( const PlusIgtlClientInfo& clientInfo ) 
{
  this->m_ClientInfo=clientInfo;
  std::string clientInfoXmlData; 
  this->m_ClientInfo.GetClientInfoInXmlData(clientInfoXmlData); 
  this->SetString(clientInfoXmlData); 
}

//----------------------------------------------------------------------------
PlusIgtlClientInfo PlusClientInfoMessage::GetClientInfo()
{
  if ( this->m_ClientInfo.SetClientInfoFromXmlData(this->GetString()) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to set Plus client info from received message!"); 
  }
  return this->m_ClientInfo;
}

}
