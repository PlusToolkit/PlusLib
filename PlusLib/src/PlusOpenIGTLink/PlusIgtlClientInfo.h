/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/ 

#ifndef __PlusIgtlClientInfo_h
#define __PlusIgtlClientInfo_h

#include "PlusConfigure.h"
#include "igtlClientSocket.h"
#include "vtkXMLUtilities.h" 

#include <string>
#include <vector>
#include <list>

/*!
  \class PlusIgtlClientInfo 
  \brief This class provides client information for vtkPlusOpenIGTLinkServer

  \ingroup PlusLibOpenIGTLink
*/
class VTK_EXPORT PlusIgtlClientInfo
{
public:

  /*! Helper struct for storing image stream and coordinate frame names */ 
  struct ImageStream
  {
    /*! Name of the image stream */ 
    std::string Name; 
    /*! Name of the coordinate frame */ 
    std::string CoordinateFrame; 
  }; 

  /*! Deserialize client info data from string xml data */ 
  PlusStatus SetClientInfoFromXmlData( const char* strXmlData ); 
  
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

  /*! Transform names to send with igt image message */ 
  std::vector<ImageStream> ImageStreams; 

  /*! Assignment operator */
  PlusIgtlClientInfo& operator=(PlusIgtlClientInfo const&clientInfo); 

  /*! Equality operator */
  bool operator==(const PlusIgtlClientInfo& in) const
  {
    return (in.ClientSocket == this->ClientSocket ); 
  }

  /*! Constructor */ 
  PlusIgtlClientInfo();
  /*! Copy constructor */ 
  PlusIgtlClientInfo(const PlusIgtlClientInfo& clientInfo); 
  /*! Destructor */ 
  ~PlusIgtlClientInfo(); 
}; 

#endif