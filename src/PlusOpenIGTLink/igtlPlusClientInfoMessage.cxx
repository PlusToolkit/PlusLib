/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

// Local includes
#include "PlusConfigure.h"
#include "igtlPlusClientInfoMessage.h"
#include "vtkPlusIgtlMessageFactory.h"

// IGTL includes
#include <igtl_header.h>
#include <igtl_util.h>

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
    igtl::MessageBase::Pointer clone;
    {
      vtkSmartPointer<vtkPlusIgtlMessageFactory> factory = vtkSmartPointer<vtkPlusIgtlMessageFactory>::New();
      clone = dynamic_cast<igtl::MessageBase*>(factory->CreateSendMessage(this->GetMessageType(), this->GetHeaderVersion()).GetPointer());
    }

    igtl::PlusClientInfoMessage::Pointer msg = dynamic_cast<igtl::PlusClientInfoMessage*>(clone.GetPointer());

    int bodySize = this->m_MessageSize - IGTL_HEADER_SIZE;
    msg->InitBuffer();
    msg->CopyHeader(this);
    msg->AllocateBuffer(bodySize);
    if (bodySize > 0)
    {
      msg->CopyBody(this);
    }

#if OpenIGTLink_HEADER_VERSION >= 2
    msg->m_MetaDataHeader = this->m_MetaDataHeader;
    msg->m_MetaDataMap = this->m_MetaDataMap;
    msg->m_IsExtendedHeaderUnpacked = this->m_IsExtendedHeaderUnpacked;
#endif

    return clone;
  }

  //----------------------------------------------------------------------------
  void PlusClientInfoMessage::SetClientInfo(const PlusIgtlClientInfo& clientInfo)
  {
    this->m_ClientInfo = clientInfo;
    std::string clientInfoXmlData;
    this->m_ClientInfo.GetClientInfoInXmlData(clientInfoXmlData);
    this->SetString(clientInfoXmlData);
  }

  //----------------------------------------------------------------------------
  PlusIgtlClientInfo PlusClientInfoMessage::GetClientInfo()
  {
    if (this->m_ClientInfo.SetClientInfoFromXmlData(this->GetString()) != PLUS_SUCCESS)
    {
      LOG_ERROR("Failed to set Plus client info from received message!");
    }
    return this->m_ClientInfo;
  }
}