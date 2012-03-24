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
#include "PlusVideoFrame.h" 
#include "vtksys/SystemTools.hxx"
#include "vtkPlusIgtlMessageCommon.h"

//----------------------------------------------------------------------------
// IGT message types
#include "igtlImageMessage.h"
#include "igtlTransformMessage.h"
#include "igtlPositionMessage.h"
#include "igtlPlusClientInfoMessage.h"


//----------------------------------------------------------------------------

vtkCxxRevisionMacro(vtkPlusIgtlMessageFactory, "$Revision: 1.0 $");
vtkStandardNewMacro(vtkPlusIgtlMessageFactory);

//----------------------------------------------------------------------------
vtkPlusIgtlMessageFactory::vtkPlusIgtlMessageFactory()
{	
  IgtlMessageTypes["NONE"]=NULL; 
  IgtlMessageTypes["IMAGE"]=(PointerToMessageBase)&igtl::ImageMessage::New; 
  IgtlMessageTypes["TRANSFORM"]=(PointerToMessageBase)&igtl::TransformMessage::New; 
  IgtlMessageTypes["POSITION"]=(PointerToMessageBase)&igtl::PositionMessage::New; 
  IgtlMessageTypes["CLIENTINFO"]=(PointerToMessageBase)&igtl::PlusClientInfoMessage::New; 
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
void vtkPlusIgtlMessageFactory::PrintAvailableMessageTypes(ostream& os, vtkIndent indent)
{
  os << indent << "Supported OpenIGTLink message types: " << std::endl; 
  std::map<std::string,PointerToMessageBase>::iterator it; 
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
PlusStatus vtkPlusIgtlMessageFactory::PackMessages(const std::vector<std::string> igtlMessageTypes, std::vector<igtl::MessageBase::Pointer>& igtlMessages, TrackedFrame& trackedFrame, 
    std::vector<PlusTransformName>& transformNames, PlusTransformName& imageTransformName, vtkTransformRepository* transformRepository/*=NULL*/)
{
  int numberOfErrors = 0; 
  igtlMessages.clear(); 

  if ( transformRepository != NULL )
  {
    transformRepository->SetTransforms(trackedFrame); 
  }

  for ( std::vector<std::string>::const_iterator messageTypeIterator = igtlMessageTypes.begin(); messageTypeIterator != igtlMessageTypes.end(); ++ messageTypeIterator )
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
    if ( dynamic_cast<igtl::ImageMessage*>(igtlMessage.GetPointer()) != NULL )
    {
      igtl::Matrix4x4 igtlMatrix; 
      vtkPlusIgtlMessageCommon::GetIgtlMatrix(igtlMatrix, transformRepository, imageTransformName); 

      igtl::ImageMessage::Pointer imageMessage = dynamic_cast<igtl::ImageMessage*>(igtlMessage.GetPointer()); 
      if ( vtkPlusIgtlMessageCommon::PackImageMessage(imageMessage, trackedFrame, igtlMatrix) != PLUS_SUCCESS )
      {
        LOG_ERROR("Failed to pack IGT messages - unable to pack image message"); 
        numberOfErrors++; 
        continue;
      }
      igtlMessages.push_back(igtlMessage); 
    }
    // Transform message 
    else if ( dynamic_cast<igtl::TransformMessage*>(igtlMessage.GetPointer()) != NULL )
    {
      for ( std::vector<PlusTransformName>::iterator transformNameIterator = transformNames.begin(); transformNameIterator != transformNames.end(); ++transformNameIterator)
      {
        PlusTransformName transformName = (*transformNameIterator);
        igtl::Matrix4x4 igtlMatrix; 
        vtkPlusIgtlMessageCommon::GetIgtlMatrix(igtlMatrix, transformRepository, transformName);

        igtl::TransformMessage::Pointer transformMessage = igtl::TransformMessage::New(); 
        transformMessage->Copy( dynamic_cast<igtl::TransformMessage*>(igtlMessage.GetPointer()) );
        vtkPlusIgtlMessageCommon::PackTransformMessage( transformMessage, transformName, igtlMatrix, trackedFrame.GetTimestamp() );
        igtlMessages.push_back( dynamic_cast<igtl::MessageBase*>(transformMessage.GetPointer()) ); 
      }
    }
    // Position message 
    else if ( dynamic_cast<igtl::PositionMessage*>(igtlMessage.GetPointer()) != NULL )
    {
      for ( std::vector<PlusTransformName>::iterator transformNameIterator = transformNames.begin(); transformNameIterator != transformNames.end(); ++transformNameIterator)
      {
        PlusTransformName transformName = (*transformNameIterator);
        igtl::Matrix4x4 igtlMatrix; 
        vtkPlusIgtlMessageCommon::GetIgtlMatrix(igtlMatrix, transformRepository, transformName);

        igtl::PositionMessage::Pointer positionMessage = igtl::PositionMessage::New(); 
        positionMessage->Copy( dynamic_cast<igtl::PositionMessage*>(igtlMessage.GetPointer()) );
        vtkPlusIgtlMessageCommon::PackPositionMessage( positionMessage, transformName, igtlMatrix, trackedFrame.GetTimestamp() );
        igtlMessages.push_back( dynamic_cast<igtl::MessageBase*>(positionMessage.GetPointer()) ); 
      }
    }
    else
    {
      LOG_WARNING("This message type (" << messageType << ") is not yet implemented!" ); 
    }
  }

  return ( numberOfErrors == 0 ? PLUS_SUCCESS : PLUS_FAIL );
}

