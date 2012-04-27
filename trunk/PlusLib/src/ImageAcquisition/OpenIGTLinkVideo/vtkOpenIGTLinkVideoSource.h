/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkOpenIGTLinkVideoSource_h
#define __vtkOpenIGTLinkVideoSource_h

#include "PlusConfigure.h"
#include "vtkPlusVideoSource.h"
#include "igtlClientSocket.h"

class VTK_EXPORT vtkOpenIGTLinkVideoSource;

/*!
  \class vtkOpenIGTLinkVideoSource 
  \brief VTK interface for video input from OpenIGTLink image message

  vtkOpenIGTLinkVideoSource is a class for providing video input interfaces between VTK and OpenIGTLink ready video device. 

  \ingroup PlusLibImageAcquisition
*/ 
class VTK_EXPORT vtkOpenIGTLinkVideoSource : public vtkPlusVideoSource
{
public:
  static vtkOpenIGTLinkVideoSource *New();
  vtkTypeRevisionMacro(vtkOpenIGTLinkVideoSource,vtkPlusVideoSource);
  void PrintSelf(ostream& os, vtkIndent indent);   

  /*! Hardware device SDK version. */
  virtual std::string GetSdkVersion(); 

  /*! Read/write main configuration from/to xml data */
  virtual PlusStatus ReadConfiguration(vtkXMLDataElement* config); 

  /*! Set OpenIGTLink message type */ 
  vtkSetStringMacro(MessageType); 
  /*! Get OpenIGTLink message type */ 
  vtkGetStringMacro(MessageType); 

 /*! Set OpenIGTLink server address */ 
  vtkSetStringMacro(ServerAddress); 
  /*! Get OpenIGTLink server address */ 
  vtkGetStringMacro(ServerAddress); 

  /*! Set OpenIGTLink server port */ 
  vtkSetMacro(ServerPort, int); 
  /*! Get OpenIGTLink server port */ 
  vtkGetMacro(ServerPort, int); 

protected:
  /*! Constructor */
  vtkOpenIGTLinkVideoSource();
  /*! Destructor */
  virtual ~vtkOpenIGTLinkVideoSource();

  /*! Connect to device */
  virtual PlusStatus InternalConnect();

  /*! Disconnect from device */
  virtual PlusStatus InternalDisconnect();

  /*!
    Called at the end of StartRecording to allow hardware-specific
    actions for starting the recording
  */
  virtual PlusStatus InternalStartRecording(); 

  /*! The internal function which actually does the grab.  */
	PlusStatus InternalGrab();

  /*! OpenIGTLink message type */
  char* MessageType; 

  /*! OpenIGTLink server address */ 
  char* ServerAddress; 

  /*! OpenIGTLink server port */ 
  int ServerPort; 

  /*! Number of retry attempts for message sending to and receiving from the server */ 
  int NumberOfRetryAttempts; 

  /*! OpenIGTLink client socket */ 
  igtl::ClientSocket::Pointer ClientSocket;
    
private:
  vtkOpenIGTLinkVideoSource(const vtkOpenIGTLinkVideoSource&);  // Not implemented.
  void operator=(const vtkOpenIGTLinkVideoSource&);  // Not implemented.
};

#endif
