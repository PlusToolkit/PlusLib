/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __PlusIgtlClientInfo_h
#define __PlusIgtlClientInfo_h

// Local includes
#include "PlusConfigure.h"
#include "vtkPlusOpenIGTLinkExport.h"

// IGTL includes
#include <igtlClientSocket.h>

// STL includes
#include <string>
#include <vector>

class vtkPlusCommandProcessor;

/*!
  \class PlusIgtlClientInfo
  \brief This class provides client information for vtkPlusOpenIGTLinkServer

  \ingroup PlusLibOpenIGTLink
*/
class vtkPlusOpenIGTLinkExport PlusIgtlClientInfo
{
public:
  /*! Helper struct for storing image stream and embedded transform frame names
  IGTL image message device name: [Name]_[EmbeddedTransformToFrame]
  */
  struct ImageStream
  {
    ImageStream()
      : Name("")
      , EmbeddedTransformToFrame("") {}

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

  /*! IGTL header version supported by the client */
  int GetClientHeaderVersion() const;
  /*! IGTL header version supported by the client */
  void SetClientHeaderVersion(int version);

  /*! Flag for whether or not the client socket blocks when sending data (wait for all data to be sent) (default no) */
  bool GetSendBlocking() const;
  /*! Flag for whether or not the client socket blocks when sending data (wait for all data to be sent) (default no) */
  void SetSendBlocking(bool val);

  /*! Minimum time between two TDATA frames. Use 0 for as fast as possible. If e.g. 50 ms is specified, the maximum update rate will be 20 Hz. */
  int GetTDATAResolution() const;
  /*! Minimum time between two TDATA frames. Use 0 for as fast as possible. If e.g. 50 ms is specified, the maximum update rate will be 20 Hz. */
  void SetTDATAResolution(int val);

  /*! Flag for start TDATA transmission request: true on STT, false on STP.
  If the start requested flag is false then don't send TDATA to the client. */
  bool GetTDATARequested() const;
  /*! Flag for start TDATA transmission request: true on STT, false on STP.
  If the start requested flag is false then don't send TDATA to the client. */
  void SetTDATARequested(bool val);

  /*! timestamp of the last sent TDATA message. */
  double GetLastTDATASentTimeStamp() const;
  /*! timestamp of the last sent TDATA message. */
  void SetLastTDATASentTimeStamp(double val);

  /*! Message types that client expects from the server */
  std::vector<std::string> IgtlMessageTypes;

  /*! Transform names to send with IGT transform, position message */
  std::vector<PlusTransformName> TransformNames;

  /*! String field names to send with IGT STRING message */
  std::vector<std::string> StringNames;

  /*! Transform names to send with IGT image message */
  std::vector<ImageStream> ImageStreams;

protected:
  int     ClientHeaderVersion;
  bool    SendBlocking;
  bool    TDATARequested;
  double  LastTDATASentTimeStamp;
  int     TDATAResolution;
};

#endif