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
  struct EncodingParameters
  {
    /*! Optional string indicating the image encoding using FourCC value is empty by default
    If the string is empty, then images will be sent using igtl::ImageMessage using a raw RGB format
    If the string is not empty, then it will be compressed and sent as an igtl::VideoMessage using the encoding specified by the FourCC value
    */
    std::string FourCC;
    bool        Lossless;
    int         MinKeyframeDistance;
    int         MaxKeyframeDistance; // TODO: Currently non functional
    int         Speed;
    std::string RateControl;
    std::string DeadlineMode;
    int         TargetBitrate;
    EncodingParameters()
      : Lossless(true)
      , MinKeyframeDistance(50)
      , MaxKeyframeDistance(50)
      , Speed(0)
      , RateControl("")
      , DeadlineMode("")
      , TargetBitrate(-1)
    {
    }
  };

  /*! Helper struct for storing image stream and embedded transform frame names
  IGTL image message device name: [Name]_[EmbeddedTransformToFrame]
  */
  struct ImageStream
  {
    /*! Name of the image stream and the IGTL image message embedded transform "From" frame */
    std::string Name;
    /*! Name of the IGTL image message embedded transform "To" frame */
    std::string EmbeddedTransformToFrame;
    /*! Parameters for how to encode video for compressed streams*/
    EncodingParameters EncodeVideoParameters;
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
  std::vector<igsioTransformName> TransformNames;

  /*! String field names to send with IGT STRING message */
  std::vector<std::string> StringNames;

  /*! Transform names to send with IGT image message */
  std::vector<ImageStream> ImageStreams;

protected:
  int     ClientHeaderVersion;
  bool    TDATARequested;
  double  LastTDATASentTimeStamp;
  int     TDATAResolution;
};

#endif