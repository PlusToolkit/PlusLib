/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"

#include "PlusTrackedFrame.h"
#include "PlusVideoFrame.h"
#include "vtkImageData.h"
#include "vtkMatrix4x4.h"
#include "vtkObjectFactory.h"
#include "vtkPlusIgtlMessageCommon.h"
#include "vtkPlusIgtlMessageFactory.h"
#include "vtkPlusTrackedFrameList.h"
#include "vtkPlusTransformRepository.h"
#include "vtksys/SystemTools.hxx"
#include <typeinfo>

//----------------------------------------------------------------------------
// IGT message types
#include "igtlCommandMessage.h"
#include "igtlImageMessage.h"
#include "igtlPlusClientInfoMessage.h"
#include "igtlPlusTrackedFrameMessage.h"
#include "igtlPlusUsMessage.h"
#include "igtlPositionMessage.h"
#include "igtlStatusMessage.h"
#include "igtlTrackingDataMessage.h"
#include "igtlTransformMessage.h"

#if defined(OpenIGTLink_ENABLE_VIDEOSTREAMING)
  #include "igtlVideoMessage.h"
  #include "igtl_video.h"
  #include "igtlI420Encoder.h"
  #if defined(OpenIGTLink_USE_VP9)
    #include "igtlVP9Encoder.h"
  #endif
  #if defined(OpenIGTLink_USE_H264)
    #include "igtlH264Encoder.h"
  #endif
#endif

//----------------------------------------------------------------------------

vtkStandardNewMacro(vtkPlusIgtlMessageFactory);

//----------------------------------------------------------------------------
vtkPlusIgtlMessageFactory::vtkPlusIgtlMessageFactory()
  : IgtlFactory(igtl::MessageFactory::New())
{
  this->IgtlFactory->AddMessageType("CLIENTINFO", (PointerToMessageBaseNew)&igtl::PlusClientInfoMessage::New);
  this->IgtlFactory->AddMessageType("TRACKEDFRAME", (PointerToMessageBaseNew)&igtl::PlusTrackedFrameMessage::New);
  this->IgtlFactory->AddMessageType("USMESSAGE", (PointerToMessageBaseNew)&igtl::PlusUsMessage::New);
}

//----------------------------------------------------------------------------
vtkPlusIgtlMessageFactory::~vtkPlusIgtlMessageFactory()
{

}

//----------------------------------------------------------------------------
void vtkPlusIgtlMessageFactory::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
  this->PrintAvailableMessageTypes(os, indent);
}

//----------------------------------------------------------------------------
vtkPlusIgtlMessageFactory::PointerToMessageBaseNew vtkPlusIgtlMessageFactory::GetMessageTypeNewPointer(const std::string& messageTypeName)
{
  return this->IgtlFactory->GetMessageTypeNewPointer(messageTypeName);
}

//----------------------------------------------------------------------------
void vtkPlusIgtlMessageFactory::PrintAvailableMessageTypes(ostream& os, vtkIndent indent)
{
  os << indent << "Supported OpenIGTLink message types: " << std::endl;
  std::vector<std::string> types;
  this->IgtlFactory->GetAvailableMessageTypes(types);
  for (std::vector<std::string>::iterator it = types.begin(); it != types.end(); ++it)
  {
    os << indent.GetNextIndent() << "- " << *it << std::endl;
  }
}

//----------------------------------------------------------------------------
igtl::MessageHeader::Pointer vtkPlusIgtlMessageFactory::CreateHeaderMessage(int headerVersion) const
{
  return this->IgtlFactory->CreateHeaderMessage(headerVersion);
}

//----------------------------------------------------------------------------
igtl::MessageBase::Pointer vtkPlusIgtlMessageFactory::CreateReceiveMessage(const igtl::MessageHeader::Pointer aIgtlMessageHdr) const
{
  if (aIgtlMessageHdr.IsNull())
  {
    LOG_ERROR("Null header sent to factory. Unable to produce a message.");
    return NULL;
  }

  igtl::MessageBase::Pointer aMessageBase;
  try
  {
    aMessageBase = this->IgtlFactory->CreateReceiveMessage(aIgtlMessageHdr);
  }
  catch (std::invalid_argument& e)
  {
    LOG_ERROR("Unable to create message: " << e.what());
    return NULL;
  }

  if (aMessageBase.IsNull())
  {
    LOG_ERROR("IGTL factory unable to produce message of type:" << aIgtlMessageHdr->GetMessageType());
    return NULL;
  }

  return aMessageBase;
}

//----------------------------------------------------------------------------
igtl::MessageBase::Pointer vtkPlusIgtlMessageFactory::CreateSendMessage(const std::string& messageType, int headerVersion) const
{
  igtl::MessageBase::Pointer aMessageBase;
  try
  {
    aMessageBase = this->IgtlFactory->CreateSendMessage(messageType, headerVersion);
  }
  catch (std::invalid_argument& e)
  {
    LOG_ERROR("Unable to create message: " << e.what());
    return NULL;
  }
  return aMessageBase;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusIgtlMessageFactory::PackMessages(int clientId, const PlusIgtlClientInfo& clientInfo, std::vector<igtl::MessageBase::Pointer>& igtlMessages, PlusTrackedFrame& trackedFrame,
    bool packValidTransformsOnly, vtkPlusTransformRepository* transformRepository/*=NULL*/)
{
  int numberOfErrors(0);
  igtlMessages.clear();

  if (transformRepository != NULL)
  {
    transformRepository->SetTransforms(trackedFrame);
  }

  for (std::vector<std::string>::const_iterator messageTypeIterator = clientInfo.IgtlMessageTypes.begin(); messageTypeIterator != clientInfo.IgtlMessageTypes.end(); ++ messageTypeIterator)
  {
    std::string messageType = (*messageTypeIterator);
    igtl::MessageBase::Pointer igtlMessage;
    try
    {
      igtlMessage = this->IgtlFactory->CreateSendMessage(messageType, clientInfo.GetClientHeaderVersion());
    }
    catch (std::invalid_argument& e)
    {
      LOG_ERROR("Unable to create message: " << e.what());
      continue;
    }

    if (igtlMessage.IsNull())
    {
      LOG_ERROR("Failed to pack IGT messages - unable to create instance from message type: " << messageType);
      numberOfErrors++;
      continue;
    }

    // Image message
    if (typeid(*igtlMessage) == typeid(igtl::ImageMessage))
    {
      for (std::vector<PlusIgtlClientInfo::ImageStream>::const_iterator imageStreamIterator = clientInfo.ImageStreams.begin(); imageStreamIterator != clientInfo.ImageStreams.end(); ++imageStreamIterator)
      {
        PlusIgtlClientInfo::ImageStream imageStream = (*imageStreamIterator);

        // Set transform name to [Name]To[CoordinateFrame]
        PlusTransformName imageTransformName = PlusTransformName(imageStream.Name, imageStream.EmbeddedTransformToFrame);

        vtkSmartPointer<vtkMatrix4x4> matrix = vtkSmartPointer<vtkMatrix4x4>::New();
        bool isValid;
        if (transformRepository->GetTransform(imageTransformName, matrix.Get(), &isValid) != PLUS_SUCCESS)
        {
          LOG_WARNING("Failed to create " << messageType << " message: cannot get image transform");
          numberOfErrors++;
          continue;
        }

        std::string deviceName = imageTransformName.From() + std::string("_") + imageTransformName.To();

        if (imageStream.EncodingType.empty())
        {
          igtl::ImageMessage::Pointer imageMessage = dynamic_cast<igtl::ImageMessage*>(igtlMessage->Clone().GetPointer());
          if (trackedFrame.IsFrameFieldDefined(PlusTrackedFrame::FIELD_FRIENDLY_DEVICE_NAME))
          {
            // Allow overriding of device name with something human readable
            // The transform name is passed in the metadata
            deviceName = trackedFrame.GetFrameField(PlusTrackedFrame::FIELD_FRIENDLY_DEVICE_NAME);
          }
          imageMessage->SetDeviceName(deviceName.c_str());

          // Send PlusTrackedFrame::CustomFrameFields as meta data in the image message.
          std::vector<std::string> frameFields;
          trackedFrame.GetFrameFieldNameList(frameFields);
          for (std::vector<std::string>::const_iterator stringNameIterator = frameFields.begin(); stringNameIterator != frameFields.end(); ++stringNameIterator)
          {
            if (trackedFrame.GetFrameField(*stringNameIterator) == NULL)
            {
              // No value is available, do not send anything
              LOG_WARNING("No metadata value for: " << *stringNameIterator)
              continue;
            }
            imageMessage->SetMetaDataElement(*stringNameIterator, IANA_TYPE_US_ASCII, trackedFrame.GetFrameField(*stringNameIterator));
          }

          if (vtkPlusIgtlMessageCommon::PackImageMessage(imageMessage, trackedFrame, *matrix) != PLUS_SUCCESS)
          {
            LOG_ERROR("Failed to create " << messageType << " message - unable to pack image message");
            numberOfErrors++;
            continue;
          }
          igtlMessages.push_back(imageMessage.GetPointer());
        }
        else
        {

#if defined(OpenIGTLink_ENABLE_VIDEOSTREAMING)
          igtl::SmartPointer<igtl::GenericEncoder> encoder = NULL;
          ClientEncoderKeyType clientEncoderKey;
          clientEncoderKey.ClientId = clientId;
          clientEncoderKey.ImageName = imageStream.Name;

          VideoEncoderMapType::iterator encoderIt = this->IgtlVideoEncoders.find(clientEncoderKey);
          if (encoderIt != this->IgtlVideoEncoders.end())
          {
            encoder = encoderIt->second;
          }
          else if (imageStream.EncodingType == IGTL_VIDEO_CODEC_NAME_I420)
          {
            encoder = new igtl::I420Encoder();
            this->IgtlVideoEncoders.insert(std::make_pair(clientEncoderKey, encoder));
          }
#if defined(OpenIGTLink_USE_VP9)
          else if (imageStream.EncodingType == IGTL_VIDEO_CODEC_NAME_VP9)
          {
            encoder = new igtl::VP9Encoder();
            this->IgtlVideoEncoders.insert(std::make_pair(clientEncoderKey, encoder));
          }
#endif
#if defined(OpenIGTLink_USE_H264)
          else if (imageStream.EncodingType == IGTL_VIDEO_CODEC_NAME_H264)
          {
            encoder = new igtl::H264Encoder();
            this->IgtlVideoEncoders.insert(std::make_pair(clientEncoderKey, encoder));
          }
#endif
          else
          {
            LOG_ERROR("Could not create encoder for image stream " << imageStream.Name << " of type " << imageStream.EncodingType);
            continue;
          }

          if (!encoder->GetInitializationStatus())
          {
            FrameSizeType frameSize = trackedFrame.GetFrameSize();
            encoder->SetPicWidthAndHeight(frameSize[0], frameSize[1]);
            encoder->SetLosslessLink(false);
            encoder->InitializeEncoder();
          }

          igtl::VideoMessage::Pointer videoMessage = igtl::VideoMessage::New();
          videoMessage->SetDeviceName(deviceName.c_str());
          if (vtkPlusIgtlMessageCommon::PackVideoMessage(videoMessage, trackedFrame, encoder, *matrix) != PLUS_SUCCESS)
          {
            LOG_ERROR("Failed to create " << "VIDEO" << " message - unable to pack image message");
            numberOfErrors++;
            continue;
          }
          videoMessage->SetDeviceName(deviceName.c_str());
          igtlMessages.push_back(videoMessage.GetPointer());
#else
          LOG_ERROR("Plus is not currently compiled with video streaming support!");
#endif
        }
      }
    }
    // Transform message
    else if (typeid(*igtlMessage) == typeid(igtl::TransformMessage))
    {
      for (std::vector<PlusTransformName>::const_iterator transformNameIterator = clientInfo.TransformNames.begin(); transformNameIterator != clientInfo.TransformNames.end(); ++transformNameIterator)
      {
        PlusTransformName transformName = (*transformNameIterator);
        bool isValid = false;
        transformRepository->GetTransformValid(transformName, isValid);

        if (!isValid && packValidTransformsOnly)
        {
          LOG_TRACE("Attempted to send invalid transform over IGT Link when server has prevented sending.");
          continue;
        }

        igtl::Matrix4x4 igtlMatrix;
        vtkPlusIgtlMessageCommon::GetIgtlMatrix(igtlMatrix, transformRepository, transformName);

        igtl::TransformMessage::Pointer transformMessage = dynamic_cast<igtl::TransformMessage*>(igtlMessage->Clone().GetPointer());
        vtkPlusIgtlMessageCommon::PackTransformMessage(transformMessage, transformName, igtlMatrix, isValid, trackedFrame.GetTimestamp());
        igtlMessages.push_back(transformMessage.GetPointer());
      }
    }
    // Tracking data message
    else if (typeid(*igtlMessage) == typeid(igtl::TrackingDataMessage))
    {
      if (clientInfo.GetTDATARequested() && clientInfo.GetLastTDATASentTimeStamp() + clientInfo.GetTDATAResolution() < trackedFrame.GetTimestamp())
      {
        std::map<std::string, vtkSmartPointer<vtkMatrix4x4> > transforms;
        for (std::vector<PlusTransformName>::const_iterator transformNameIterator = clientInfo.TransformNames.begin(); transformNameIterator != clientInfo.TransformNames.end(); ++transformNameIterator)
        {
          PlusTransformName transformName = (*transformNameIterator);

          bool isValid = false;
          vtkSmartPointer<vtkMatrix4x4> mat = vtkSmartPointer<vtkMatrix4x4>::New();
          transformRepository->GetTransform(transformName, mat, &isValid);

          if (!isValid && packValidTransformsOnly)
          {
            LOG_TRACE("Attempted to send invalid transform over IGT Link when server has prevented sending.");
            continue;
          }

          std::string transformNameStr;
          transformName.GetTransformName(transformNameStr);

          transforms[transformNameStr] = mat;
        }

        igtl::TrackingDataMessage::Pointer trackingDataMessage = dynamic_cast<igtl::TrackingDataMessage*>(igtlMessage->Clone().GetPointer());
        vtkPlusIgtlMessageCommon::PackTrackingDataMessage(trackingDataMessage, transforms, trackedFrame.GetTimestamp());
        igtlMessages.push_back(trackingDataMessage.GetPointer());
      }
    }
    // Position message
    else if (typeid(*igtlMessage) == typeid(igtl::PositionMessage))
    {
      for (std::vector<PlusTransformName>::const_iterator transformNameIterator = clientInfo.TransformNames.begin(); transformNameIterator != clientInfo.TransformNames.end(); ++transformNameIterator)
      {
        /*
          Advantage of using position message type:
          Although equivalent position and orientation can be described with the TRANSFORM data type,
          the POSITION data type has the advantage of smaller data size (19%). It is therefore more suitable for
          pushing high frame-rate data from tracking devices.
        */
        PlusTransformName transformName = (*transformNameIterator);
        igtl::Matrix4x4 igtlMatrix;
        vtkPlusIgtlMessageCommon::GetIgtlMatrix(igtlMatrix, transformRepository, transformName);

        float position[3] = {igtlMatrix[0][3], igtlMatrix[1][3], igtlMatrix[2][3]};
        float quaternion[4] = {0, 0, 0, 1};
        igtl::MatrixToQuaternion(igtlMatrix, quaternion);

        igtl::PositionMessage::Pointer positionMessage = dynamic_cast<igtl::PositionMessage*>(igtlMessage->Clone().GetPointer());
        vtkPlusIgtlMessageCommon::PackPositionMessage(positionMessage, transformName, position, quaternion, trackedFrame.GetTimestamp());
        igtlMessages.push_back(positionMessage.GetPointer());
      }
    }
    // TRACKEDFRAME message
    else if (typeid(*igtlMessage) == typeid(igtl::PlusTrackedFrameMessage))
    {
      igtl::PlusTrackedFrameMessage::Pointer trackedFrameMessage = dynamic_cast<igtl::PlusTrackedFrameMessage*>(igtlMessage->Clone().GetPointer());

      for (auto nameIter = clientInfo.TransformNames.begin(); nameIter != clientInfo.TransformNames.end(); ++nameIter)
      {
        bool isValid(false);
        vtkSmartPointer<vtkMatrix4x4> matrix(vtkSmartPointer<vtkMatrix4x4>::New());
        transformRepository->GetTransform(*nameIter, matrix, &isValid);
        trackedFrame.SetFrameTransform(*nameIter, matrix);
        trackedFrame.SetFrameTransformStatus(*nameIter, isValid ? FIELD_OK : FIELD_INVALID);
      }

      vtkSmartPointer<vtkMatrix4x4> imageMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
      imageMatrix->Identity();
      if (!clientInfo.ImageStreams.empty())
      {
        bool isValid;
        if (transformRepository->GetTransform(PlusTransformName(clientInfo.ImageStreams[0].Name, clientInfo.ImageStreams[0].EmbeddedTransformToFrame), imageMatrix, &isValid) != PLUS_SUCCESS)
        {
          LOG_ERROR("Unable to retrieve embedded image transform: " << clientInfo.ImageStreams[0].Name << "To" << clientInfo.ImageStreams[0].EmbeddedTransformToFrame << ".");
          continue;
        }
      }
      if (vtkPlusIgtlMessageCommon::PackTrackedFrameMessage(trackedFrameMessage, trackedFrame, imageMatrix, clientInfo.TransformNames) != PLUS_SUCCESS)
      {
        LOG_ERROR("Failed to pack IGT messages - unable to pack tracked frame message");
        numberOfErrors++;
        continue;
      }
      igtlMessages.push_back(trackedFrameMessage.GetPointer());
    }
    // USMESSAGE message
    else if (typeid(*igtlMessage) == typeid(igtl::PlusUsMessage))
    {
      igtl::PlusUsMessage::Pointer usMessage = dynamic_cast<igtl::PlusUsMessage*>(igtlMessage->Clone().GetPointer());
      if (vtkPlusIgtlMessageCommon::PackUsMessage(usMessage, trackedFrame) != PLUS_SUCCESS)
      {
        LOG_ERROR("Failed to pack IGT messages - unable to pack US message");
        numberOfErrors++;
        continue;
      }
      igtlMessages.push_back(usMessage.GetPointer());
    }
    // String message
    else if (typeid(*igtlMessage) == typeid(igtl::StringMessage))
    {
      for (std::vector< std::string >::const_iterator stringNameIterator = clientInfo.StringNames.begin(); stringNameIterator != clientInfo.StringNames.end(); ++stringNameIterator)
      {
        const char* stringName = stringNameIterator->c_str();
        const char* stringValue = trackedFrame.GetFrameField(stringName);
        if (stringValue == NULL)
        {
          // no value is available, do not send anything
          continue;
        }
        igtl::StringMessage::Pointer stringMessage = dynamic_cast<igtl::StringMessage*>(igtlMessage->Clone().GetPointer());
        vtkPlusIgtlMessageCommon::PackStringMessage(stringMessage, stringName, stringValue, trackedFrame.GetTimestamp());
        igtlMessages.push_back(stringMessage.GetPointer());
      }
    }
    else if (typeid(*igtlMessage) == typeid(igtl::CommandMessage))
    {
      // Is there any use case for the server sending commands to the client?
      igtl::CommandMessage::Pointer commandMessage = dynamic_cast<igtl::CommandMessage*>(igtlMessage->Clone().GetPointer());
      //vtkPlusIgtlMessageCommon::PackCommandMessage( commandMessage );
      igtlMessages.push_back(commandMessage.GetPointer());
    }
    else
    {
      LOG_WARNING("This message type (" << messageType << ") is not supported!");
    }
  }

  return (numberOfErrors == 0 ? PLUS_SUCCESS : PLUS_FAIL);
}

#if defined(OpenIGTLink_ENABLE_VIDEOSTREAMING)
//----------------------------------------------------------------------------
void vtkPlusIgtlMessageFactory::RemoveClientEncoders(int clientId)
{
  VideoEncoderMapType currentIgtlVideoEncoders = this->IgtlVideoEncoders;
  for (VideoEncoderMapType::iterator encoderIt = currentIgtlVideoEncoders.begin(); encoderIt != currentIgtlVideoEncoders.end(); ++encoderIt)
  {
    if (encoderIt->first.ClientId != clientId)
    {
      continue;
    }
    this->IgtlVideoEncoders.erase(encoderIt->first);
  }
}
#endif
