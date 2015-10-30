/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

#include "vtkPlusIgtlMessageFactory.h"
#include "vtkObjectFactory.h"
#include "vtkTransformRepository.h" 
#include "vtkTrackedFrameList.h" 
#include "TrackedFrame.h"
#include "vtkMatrix4x4.h"
#include "vtkImageData.h" 
#include "vtksys/SystemTools.hxx"
#include "vtkPlusIgtlMessageCommon.h"
#include "PlusVideoFrame.h" 

//----------------------------------------------------------------------------
// IGT message types
#include "igtlImageMessage.h"
#include "igtlTransformMessage.h"
#include "igtlPositionMessage.h"
#include "igtlStatusMessage.h"
#include "igtlPlusClientInfoMessage.h"
#include "igtlPlusTrackedFrameMessage.h"
#include "igtlPlusUsMessage.h"

//----------------------------------------------------------------------------

vtkStandardNewMacro(vtkPlusIgtlMessageFactory);

//----------------------------------------------------------------------------
vtkPlusIgtlMessageFactory::vtkPlusIgtlMessageFactory()
{
  this->AddMessageType("NONE", NULL); 
  this->AddMessageType("IMAGE", (PointerToMessageBaseNew)&igtl::ImageMessage::New); 
  this->AddMessageType("TRANSFORM", (PointerToMessageBaseNew)&igtl::TransformMessage::New); 
  this->AddMessageType("POSITION", (PointerToMessageBaseNew)&igtl::PositionMessage::New); 
  this->AddMessageType("CLIENTINFO", (PointerToMessageBaseNew)&igtl::PlusClientInfoMessage::New); 
  this->AddMessageType("TRACKEDFRAME", (PointerToMessageBaseNew)&igtl::PlusTrackedFrameMessage::New); 
  this->AddMessageType("USMESSAGE", (PointerToMessageBaseNew)&igtl::PlusUsMessage::New); 
  this->AddMessageType("STATUS", (PointerToMessageBaseNew)&igtl::StatusMessage::New); 
  this->AddMessageType("STRING", (PointerToMessageBaseNew)&igtl::StringMessage::New); 
}

//----------------------------------------------------------------------------
vtkPlusIgtlMessageFactory::~vtkPlusIgtlMessageFactory()
{
}

//----------------------------------------------------------------------------
void vtkPlusIgtlMessageFactory::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
  this->PrintAvailableMessageTypes(os, indent); 
}

//----------------------------------------------------------------------------
void vtkPlusIgtlMessageFactory::AddMessageType(std::string messageTypeName, vtkPlusIgtlMessageFactory::PointerToMessageBaseNew messageTypeNewPointer )
{
  this->IgtlMessageTypes[messageTypeName]=messageTypeNewPointer; 
}

//----------------------------------------------------------------------------
vtkPlusIgtlMessageFactory::PointerToMessageBaseNew vtkPlusIgtlMessageFactory::GetMessageTypeNewPointer(std::string messageTypeName)
{
  if ( this->IgtlMessageTypes.find(messageTypeName) != this->IgtlMessageTypes.end() )
  {
    return IgtlMessageTypes[messageTypeName];
  }

  LOG_ERROR(messageTypeName << " message type is not registered to factory!"); 
  return NULL; 
}

//----------------------------------------------------------------------------
void vtkPlusIgtlMessageFactory::PrintAvailableMessageTypes(ostream& os, vtkIndent indent)
{
  os << indent << "Supported OpenIGTLink message types: " << std::endl; 
  std::map<std::string,PointerToMessageBaseNew>::iterator it; 
  for ( it = IgtlMessageTypes.begin(); it != IgtlMessageTypes.end(); ++it)
  {
    if ( it->second != NULL )
    {
      igtl::MessageBase::Pointer message = (*it->second)(); 
      os << indent.GetNextIndent() << "- " << it->first << " (class name: " << message->GetNameOfClass() << ")" << std::endl; 
    }
  }
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusIgtlMessageFactory::CreateInstance(const char* aIgtlMessageType, igtl::MessageBase::Pointer& aMessageBase)
{
  if ( aMessageBase.IsNotNull() )
  {
    aMessageBase->Delete(); 
    aMessageBase = NULL; 
  }
  
  std::string messageType; 
  if ( aIgtlMessageType == NULL ) 
  {
    LOG_WARNING("IGT message type is NULL, set to default: NONE"); 
    messageType = "NONE"; 
  }
  else
  {
    messageType = aIgtlMessageType; 
  }

  if ( IgtlMessageTypes.find( vtksys::SystemTools::UpperCase(messageType) ) != IgtlMessageTypes.end() )
  {
    if ( IgtlMessageTypes[vtksys::SystemTools::UpperCase(messageType)] != NULL )
    { // Call tracker New() function if tracker not NULL
      aMessageBase = (*IgtlMessageTypes[vtksys::SystemTools::UpperCase(messageType)])(); 
    }
  }
  else
  {
    LOG_ERROR("Unknown IGT message type: " << vtksys::SystemTools::UpperCase(messageType));
    return PLUS_FAIL; 
  }

  return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusIgtlMessageFactory::PackMessages(const PlusIgtlClientInfo& clientInfo, std::vector<igtl::MessageBase::Pointer>& igtlMessages, TrackedFrame& trackedFrame,
    bool packValidTransformsOnly, vtkTransformRepository* transformRepository/*=NULL*/)
{
  int numberOfErrors = 0; 
  igtlMessages.clear(); 

  if ( transformRepository != NULL )
  {
    transformRepository->SetTransforms(trackedFrame); 
  }

  for ( std::vector<std::string>::const_iterator messageTypeIterator = clientInfo.IgtlMessageTypes.begin(); messageTypeIterator != clientInfo.IgtlMessageTypes.end(); ++ messageTypeIterator )
  {
    std::string messageType = (*messageTypeIterator); 
    igtl::MessageBase::Pointer igtlMessage = NULL; 
    if ( this->CreateInstance(messageType.c_str(), igtlMessage) != PLUS_SUCCESS )
    {
      LOG_ERROR("Failed to pack IGT messages - unable to create instance from message type: " << messageType ); 
      numberOfErrors++; 
      continue; 
    }

    // Image message 
    if ( STRCASECMP(messageType.c_str(), "IMAGE") == 0 )
    {
      for ( std::vector<PlusIgtlClientInfo::ImageStream>::const_iterator imageStreamIterator = clientInfo.ImageStreams.begin(); imageStreamIterator != clientInfo.ImageStreams.end(); ++imageStreamIterator)
      {
        PlusIgtlClientInfo::ImageStream imageStream = (*imageStreamIterator);
        
        //Set transform name to [Name]To[CoordinateFrame]
        PlusTransformName imageTransformName = PlusTransformName(imageStream.Name, imageStream.EmbeddedTransformToFrame); 

        igtl::Matrix4x4 igtlMatrix;
        if (vtkPlusIgtlMessageCommon::GetIgtlMatrix(igtlMatrix, transformRepository, imageTransformName) != PLUS_SUCCESS)
        {
          LOG_WARNING("Failed to create IMAGE message: cannot get image transform");
          numberOfErrors++; 
          continue;
        }

        igtl::ImageMessage::Pointer imageMessage = igtl::ImageMessage::New(); 
        imageMessage->Copy( dynamic_cast<igtl::ImageMessage*>(igtlMessage.GetPointer()) );
        std::string deviceName = imageTransformName.From() + std::string("_") + imageTransformName.To(); 
        imageMessage->SetDeviceName(deviceName.c_str()); 
        if ( vtkPlusIgtlMessageCommon::PackImageMessage(imageMessage, trackedFrame, igtlMatrix) != PLUS_SUCCESS )
        {
          LOG_ERROR("Failed to create IMAGE message - unable to pack image message"); 
          numberOfErrors++; 
          continue;
        }
        igtlMessages.push_back( dynamic_cast<igtl::MessageBase*>(imageMessage.GetPointer()) ); 
      }
    }
    // Transform message 
    else if (STRCASECMP(messageType.c_str(), "TRANSFORM") == 0 )
    {
      for ( std::vector<PlusTransformName>::const_iterator transformNameIterator = clientInfo.TransformNames.begin(); transformNameIterator != clientInfo.TransformNames.end(); ++transformNameIterator)
      {
        PlusTransformName transformName = (*transformNameIterator);
        bool isValid = false;
        transformRepository->GetTransformValid(transformName, isValid);

        if( !isValid && packValidTransformsOnly )
        {
          LOG_TRACE("Attempted to send invalid transform over IGT Link when server has prevented sending.");
          continue;
        }

        igtl::Matrix4x4 igtlMatrix; 
        vtkPlusIgtlMessageCommon::GetIgtlMatrix(igtlMatrix, transformRepository, transformName);

        igtl::TransformMessage::Pointer transformMessage = igtl::TransformMessage::New(); 
        transformMessage->Copy( dynamic_cast<igtl::TransformMessage*>(igtlMessage.GetPointer()) );
        vtkPlusIgtlMessageCommon::PackTransformMessage( transformMessage, transformName, igtlMatrix, trackedFrame.GetTimestamp() );
        igtlMessages.push_back( dynamic_cast<igtl::MessageBase*>(transformMessage.GetPointer()) ); 
      }
    }
    // Position message 
    else if ( STRCASECMP(messageType.c_str(), "POSITION") == 0 )
    {
      for ( std::vector<PlusTransformName>::const_iterator transformNameIterator = clientInfo.TransformNames.begin(); transformNameIterator != clientInfo.TransformNames.end(); ++transformNameIterator)
      {
        PlusTransformName transformName = (*transformNameIterator);
        igtl::Matrix4x4 igtlMatrix; 
        vtkPlusIgtlMessageCommon::GetIgtlMatrix(igtlMatrix, transformRepository, transformName);

        float position[3]={igtlMatrix[0][3], igtlMatrix[1][3], igtlMatrix[2][3]};
        float quaternion[4]={0,0,0,1};
        igtl::MatrixToQuaternion( igtlMatrix, quaternion );

        igtl::PositionMessage::Pointer positionMessage = igtl::PositionMessage::New(); 
        positionMessage->Copy( dynamic_cast<igtl::PositionMessage*>(igtlMessage.GetPointer()) );
        vtkPlusIgtlMessageCommon::PackPositionMessage( positionMessage, transformName, position, quaternion, trackedFrame.GetTimestamp() );
        igtlMessages.push_back( dynamic_cast<igtl::MessageBase*>(positionMessage.GetPointer()) ); 
      }
    }
    // TRACKEDFRAME message
    else if ( STRCASECMP(messageType.c_str(), "TRACKEDFRAME") == 0 )
    {
      igtl::PlusTrackedFrameMessage::Pointer trackedFrameMessage = dynamic_cast<igtl::PlusTrackedFrameMessage*>(igtlMessage.GetPointer()); 
      if ( vtkPlusIgtlMessageCommon::PackTrackedFrameMessage(trackedFrameMessage, trackedFrame) != PLUS_SUCCESS )
      {
        LOG_ERROR("Failed to pack IGT messages - unable to pack tracked frame message"); 
        numberOfErrors++; 
        continue;
      }
      igtlMessages.push_back(igtlMessage); 
    }
    // USMESSAGE message
    else if ( STRCASECMP(messageType.c_str(), "USMESSAGE") == 0 )
    {
      igtl::PlusUsMessage::Pointer usMessage = dynamic_cast<igtl::PlusUsMessage*>(igtlMessage.GetPointer()); 
      if ( vtkPlusIgtlMessageCommon::PackUsMessage(usMessage, trackedFrame) != PLUS_SUCCESS )
      {
        LOG_ERROR("Failed to pack IGT messages - unable to pack US message"); 
        numberOfErrors++; 
        continue;
      }
      igtlMessages.push_back(igtlMessage); 
    }
    // String message 
    else if (STRCASECMP(messageType.c_str(), "STRING") == 0 )
    {
      for ( std::vector< std::string >::const_iterator stringNameIterator = clientInfo.StringNames.begin(); stringNameIterator != clientInfo.StringNames.end(); ++stringNameIterator)
      {
        const char* stringName = stringNameIterator->c_str();
        const char* stringValue = trackedFrame.GetCustomFrameField(stringName);
        if (stringValue==NULL)
        {
          // no value is available, do not send anything
          continue;
        }
        igtl::StringMessage::Pointer stringMessage = igtl::StringMessage::New(); 
        stringMessage->Copy( dynamic_cast<igtl::StringMessage*>(igtlMessage.GetPointer()) );
        vtkPlusIgtlMessageCommon::PackStringMessage( stringMessage, stringName, stringValue, trackedFrame.GetTimestamp() );
        igtlMessages.push_back( dynamic_cast<igtl::MessageBase*>(stringMessage.GetPointer()) ); 
      }
    }
    else
    {
      LOG_WARNING("This message type (" << messageType << ") is not supported!" ); 
    }
  }

  return ( numberOfErrors == 0 ? PLUS_SUCCESS : PLUS_FAIL );
}

