/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

#include "PlusConfigure.h"

#include "igtlPlusClientInfoMessage.h"

#include "igtlutil/igtl_header.h"
#include "igtlutil/igtl_util.h"


namespace igtl 
{

//----------------------------------------------------------------------------
PlusClientInfoMessage::PlusClientInfoMessage() : StringMessage()
{
  this->m_DefaultBodyType = "CLIENTINFO";
}

//----------------------------------------------------------------------------
PlusClientInfoMessage::~PlusClientInfoMessage()
{
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
