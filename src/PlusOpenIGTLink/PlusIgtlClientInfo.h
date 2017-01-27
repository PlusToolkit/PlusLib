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

class vtkPlusCommandProcessor;

/*!
  \class PlusIgtlClientInfo
  \brief This class provides client information for vtkPlusOpenIGTLinkServer

  \ingroup PlusLibOpenIGTLink
*/
struct vtkPlusOpenIGTLinkExport PlusIgtlClientInfo
{
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

  PlusIgtlClientInfo();

  /*! De-serialize client info data from string xml data */
  PlusStatus SetClientInfoFromXmlData(const char* strXmlData);

  /*! De-serialize client info data from xml data */
  PlusStatus SetClientInfoFromXmlData(vtkXMLDataElement* xmldata);

  /*! Serialize client info data to xml data and return in string */
  void GetClientInfoInXmlData(std::string& strXmlData);

  virtual void PrintSelf(ostream& os, vtkIndent indent);

  int GetClientHeaderVersion() const;

  void SetClientHeaderVersion(int version);

  /*! IGTL header version supported by the client */
  int ClientHeaderVersion;

  /*! Message types that client expects from the server */
  std::vector<std::string> IgtlMessageTypes;

  /*! Transform names to send with IGT transform, position message */
  std::vector<PlusTransformName> TransformNames;

  /*! String field names to send with IGT STRING message */
  std::vector< std::string > StringNames;

  /*! Transform names to send with IGT image message */
  std::vector<ImageStream> ImageStreams;

  /*! A new TDATA is only sent if the time elapsed is at least the resolution
     value (otherwise we don't send this tracking data to the client) */
  int Resolution;

  /*! flag for start TDATA transmission request: true on STT, false on STP.
     If the start requested flag is false then don't send TDATA to the client. */
  bool TDATARequested;

  /*! timestamp of the last sent TDATA message. */
  double LastTDATASentTimeStamp;
};

#endif