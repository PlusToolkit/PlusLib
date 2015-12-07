/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/ 

#ifndef __PlusIgtlClientInfo_h
#define __PlusIgtlClientInfo_h

#include "PlusConfigure.h"
#include "vtkPlusOpenIGTLinkExport.h"

#include "igtlClientSocket.h"

#include <string>
#include <vector>

/*!
  \class PlusIgtlClientInfo 
  \brief This class provides client information for vtkPlusOpenIGTLinkServer

  \ingroup PlusLibOpenIGTLink
*/
class vtkPlusOpenIGTLinkExport PlusIgtlClientInfo
{
public:

  /*! Constructor */ 
  PlusIgtlClientInfo();
  /*! Copy constructor */ 
  PlusIgtlClientInfo(const PlusIgtlClientInfo& clientInfo); 
  /*! Destructor */ 
  virtual ~PlusIgtlClientInfo(); 

  /*! Counter to generate unique client IDs. Access to the counter is not protected, therefore all clients should be created from the same thread. */
  static int ClientIdCounter;

  /*! Helper struct for storing image stream and embedded transform frame names 
  IGTL image message device name: [Name]_[EmbeddedTransformToFrame]
  */ 
  struct ImageStream
  {
    /*! Name of the image stream and the IGTL image message embedded transform "From" frame */ 
    std::string Name; 
    /*! Name of the IGTL image message embedded transform "To" frame */ 
    std::string EmbeddedTransformToFrame; 
  }; 

  /*! Deserialize client info data from string xml data */ 
  PlusStatus SetClientInfoFromXmlData( const char* strXmlData );

  /*! Deserialize client info data from xml data */ 
  PlusStatus SetClientInfoFromXmlData( vtkXMLDataElement* xmldata );
  
  /*! Serialize client info data to xml data and return in string */ 
  void GetClientInfoInXmlData( std::string& strXmlData ); 

  /*! Copy all non-pointer data */ 
  void ShallowCopy(const PlusIgtlClientInfo& clientInfo); 

  /*! IGTL client socket instance */ 
  igtl::ClientSocket::Pointer ClientSocket; 

  /*! Message types that client expects from the server */ 
  std::vector<std::string> IgtlMessageTypes; 

  /*! Transform names to send with igt transform, position message */ 
  std::vector<PlusTransformName> TransformNames;

  /*! String field names to send with igt STRING message */ 
  std::vector< std::string > StringNames;

  /*! Transform names to send with igt image message */ 
  std::vector<ImageStream> ImageStreams; 

  /*! Unique client identifier. Cannot be 0. */   
  int ClientId;

  /*! Assignment operator */
  PlusIgtlClientInfo& operator=(PlusIgtlClientInfo const&clientInfo); 

  /*! Equality operator */
  bool operator==(const PlusIgtlClientInfo& in) const
  {
    return (in.ClientSocket == this->ClientSocket ); 
  }
}; 

#endif