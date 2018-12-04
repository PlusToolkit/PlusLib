/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkPlusIgtlMessageFactory_h
#define __vtkPlusIgtlMessageFactory_h

#include "PlusConfigure.h"
#include "vtkPlusOpenIGTLinkExport.h"

// VTK includes
#include "vtkObject.h"

// OpenIGTLink includes
#include "igtlMessageBase.h"
#include "igtlMessageFactory.h"

#if defined(OpenIGTLink_ENABLE_VIDEOSTREAMING)
  #include "igtlCodecCommonClasses.h"
#endif

// PlusLib includes
#include "PlusIgtlClientInfo.h"

class vtkXMLDataElement;
//class igsioTrackedFrame; 
//class vtkIGSIOTransformRepository;

/*!
  \class vtkPlusIgtlMessageFactory
  \brief Factory class of supported OpenIGTLink message types

  This class is a factory class of supported OpenIGTLink message types to localize the message creation code.

  \ingroup PlusLibOpenIGTLink
*/
class vtkPlusOpenIGTLinkExport vtkPlusIgtlMessageFactory: public vtkObject
{
public:
  static vtkPlusIgtlMessageFactory* New();
  vtkTypeMacro(vtkPlusIgtlMessageFactory, vtkObject);
  virtual void PrintSelf(ostream& os, vtkIndent indent) VTK_OVERRIDE;

  /*! Function pointer for storing New() static methods of igtl::MessageBase classes */
  typedef igtl::MessageBase::Pointer (*PointerToMessageBaseNew)();

  /*!
  Get pointer to message type new function, or NULL if the message type not registered
  Usage: igtl::MessageBase::Pointer message = GetMessageTypeNewPointer("IMAGE")();
  */
  virtual vtkPlusIgtlMessageFactory::PointerToMessageBaseNew GetMessageTypeNewPointer(const std::string& messageTypeName);

  /*! Print all supported OpenIGTLink message types */
  virtual void PrintAvailableMessageTypes(ostream& os, vtkIndent indent);

  /// Constructs a message header.
  /// Throws invalid_argument if headerMsg is NULL.
  /// Throws invalid_argument if this->IsValid(headerMsg) returns false.
  /// Creates message, calls InitBuffer()
  igtl::MessageHeader::Pointer CreateHeaderMessage(int headerVersion) const;

  /// Constructs a message from the given populated header.
  /// Throws invalid_argument if headerMsg is NULL.
  /// Throws invalid_argument if this->IsValid(headerMsg) returns false.
  /// Creates message, sets header onto message and calls AllocateBuffer() on the message.
  igtl::MessageBase::Pointer CreateReceiveMessage(igtl::MessageHeader::Pointer headerMsg) const;

  /// Constructs an empty message from the given message type.
  /// Throws invalid_argument if messageType is empty.
  /// Creates message, sets header onto message and calls AllocateBuffer() on the message.
  igtl::MessageBase::Pointer CreateSendMessage(const std::string& messageType, int headerVersion) const;

  /*!
  Generate and pack IGTL messages from tracked frame
  \param clientId Id of the client that messages will be sent to
  \param packValidTransformsOnly Control whether or not to pack transform messages if they contain invalid transforms
  \param clientInfo Specifies list of message types and names to generate for a client.
  \param igtMessages Output list for the generated IGTL messages
  \param trackedFrame Input tracked frame data used for IGTL message generation
  \param transformRepository Transform repository used for computing the selected transforms
  */
  PlusStatus PackMessages(int clientId, const PlusIgtlClientInfo& clientInfo, std::vector<igtl::MessageBase::Pointer>& igtMessages, igsioTrackedFrame& trackedFrame,
                          bool packValidTransformsOnly, vtkIGSIOTransformRepository* transformRepository = NULL);

#if defined(OpenIGTLink_ENABLE_VIDEOSTREAMING)
  /*!
  Remove all encoders with matching clientId from this->IgtlVideoEncoders
  \param clientId Id of the client for which the video encoders will be removed
  */
  void RemoveClientEncoders(int clientId);
#endif

protected:
  vtkPlusIgtlMessageFactory();
  virtual ~vtkPlusIgtlMessageFactory();

  igtl::MessageFactory::Pointer IgtlFactory;

protected:
  int PackImageMessage(const PlusIgtlClientInfo& clientInfo, vtkIGSIOTransformRepository& transformRepository, const std::string& messageType,
                       igtl::MessageBase::Pointer igtlMessage, igsioTrackedFrame& trackedFrame, std::vector<igtl::MessageBase::Pointer>& igtlMessages, int clientId);
  int PackTransformMessage(const PlusIgtlClientInfo& clientInfo, vtkIGSIOTransformRepository& transformRepository, bool packValidTransformsOnly,
                           igtl::MessageBase::Pointer igtlMessage, igsioTrackedFrame& trackedFrame, std::vector<igtl::MessageBase::Pointer>& igtlMessages);
  int PackTrackingDataMessage(const PlusIgtlClientInfo& clientInfo, igsioTrackedFrame& trackedFrame, vtkIGSIOTransformRepository& transformRepository, bool packValidTransformsOnly,
                              igtl::MessageBase::Pointer igtlMessage, std::vector<igtl::MessageBase::Pointer>& igtlMessages);
  int PackPositionMessage(const PlusIgtlClientInfo& clientInfo, vtkIGSIOTransformRepository& transformRepository, igtl::MessageBase::Pointer igtlMessage,
                          igsioTrackedFrame& trackedFrame, std::vector<igtl::MessageBase::Pointer>& igtlMessages);
  int PackTrackedFrameMessage(igtl::MessageBase::Pointer igtlMessage, const PlusIgtlClientInfo& clientInfo, vtkIGSIOTransformRepository& transformRepository,
                              igsioTrackedFrame& trackedFrame, std::vector<igtl::MessageBase::Pointer>& igtlMessages);
  int PackUsMessage(igtl::MessageBase::Pointer igtlMessage, igsioTrackedFrame& trackedFrame, std::vector<igtl::MessageBase::Pointer>& igtlMessages);
  int PackStringMessage(const PlusIgtlClientInfo& clientInfo, igsioTrackedFrame& trackedFrame, igtl::MessageBase::Pointer igtlMessage, std::vector<igtl::MessageBase::Pointer>& igtlMessages);
  int PackCommandMessage(igtl::MessageBase::Pointer igtlMessage, std::vector<igtl::MessageBase::Pointer>& igtlMessages);


#if defined(OpenIGTLink_ENABLE_VIDEOSTREAMING)
  struct ClientEncoderKeyType
  {
    int                 ClientId;
    std::string         ImageName;
    friend bool operator<(const ClientEncoderKeyType& left, const ClientEncoderKeyType& right)
    {
      return left.ClientId < right.ClientId || left.ImageName < right.ImageName;
    }
  };

  typedef std::map<ClientEncoderKeyType, igtl::SmartPointer<igtl::GenericEncoder> > VideoEncoderMapType;
  VideoEncoderMapType IgtlVideoEncoders;
#endif

private:
  vtkPlusIgtlMessageFactory(const vtkPlusIgtlMessageFactory&);
  void operator=(const vtkPlusIgtlMessageFactory&);

};

#endif