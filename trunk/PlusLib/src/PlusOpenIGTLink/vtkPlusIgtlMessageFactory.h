/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/ 

#ifndef __vtkPlusIgtlMessageFactory_h
#define __vtkPlusIgtlMessageFactory_h

#include "PlusConfigure.h"
#include "vtkObject.h" 
#include "igtlMessageBase.h"
#include "igtlSocket.h"

class vtkXMLDataElement; 
class TrackedFrame; 
class vtkTransformRepository; 


/*!
  \class vtkPlusIgtlMessageFactory 
  \brief Factory class of supported OpenIGTLink message types

  This class is a factory class of supported OpenIGTLink message types to localize the message creation code.

  \ingroup PlusLibOpenIGTLink
*/ 
class VTK_EXPORT vtkPlusIgtlMessageFactory: public vtkObject
{
public:
  
  static vtkPlusIgtlMessageFactory *New();
  vtkTypeRevisionMacro(vtkPlusIgtlMessageFactory,vtkObject);
  virtual void PrintSelf(ostream& os, vtkIndent indent);

  /*! Print all supported OpenIGTLink message types */
  virtual void PrintAvailableMessageTypes(ostream& os, vtkIndent indent);

  /*! Create a new igtl::MessageBase instance from message type, delete previous igtl::MessageBase if's not NULL */ 
  PlusStatus CreateInstance(const char* aIgtlMessageType, igtl::MessageBase::Pointer& aMessageBase);

  /*! 
  Generate and pack IGTL messages from tracked frame 
  \param igtlMessageTypes List of message types to generate for a client 
  \param igtMessages Output list for the generated IGTL messages
  \param trackedFrame Input tracked frame data used for IGTL message generation 
  \param transformNames List of transform names to send 
  \param imageTransformName Image transform name used in the IGTL image message 
  \param transformRepository Transform repository used for computing the selected transforms 
  */ 
  PlusStatus PackMessages(const std::vector<std::string>& igtlMessageTypes, std::vector<igtl::MessageBase::Pointer>& igtMessages, TrackedFrame& trackedFrame, 
    std::vector<PlusTransformName>& transformNames, PlusTransformName& imageTransformName, vtkTransformRepository* transformRepository=NULL); 

protected:
  vtkPlusIgtlMessageFactory();
  virtual ~vtkPlusIgtlMessageFactory();

  /*! Function pointer for storing New() static methods of igtl::MessageBase classes */ 
  typedef igtl::MessageBase::Pointer (*PointerToMessageBase)(); 
  /*! Map igt message types and the New() static methods of igtl::MessageBase classes */ 
  std::map<std::string,PointerToMessageBase> IgtlMessageTypes; 

private:
  vtkPlusIgtlMessageFactory(const vtkPlusIgtlMessageFactory&);
  void operator=(const vtkPlusIgtlMessageFactory&);

}; 

#endif 