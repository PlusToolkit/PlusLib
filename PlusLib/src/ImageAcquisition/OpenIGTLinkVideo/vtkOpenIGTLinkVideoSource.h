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
\class vtkOpenIGTLinkVideoSourceCleanup 
\brief Class that cleans up (deletes singleton instance of) vtkOpenIGTLinkVideoSource when destroyed
\ingroup PlusLibImageAcquisition
*/
class VTK_EXPORT vtkOpenIGTLinkVideoSourceCleanup
{
public:
  vtkOpenIGTLinkVideoSourceCleanup();
  ~vtkOpenIGTLinkVideoSourceCleanup();
};

/*!
  \class vtkOpenIGTLinkVideoSource 
  \brief VTK interface for video input from OpenIGTLink image message

  vtkOpenIGTLinkVideoSource is a class for providing video input interfaces between VTK and OpenIGTLink ready video device. 

  \ingroup PlusLibImageAcquisition
*/ 
class VTK_EXPORT vtkOpenIGTLinkVideoSource : public vtkPlusVideoSource
{
public:
  vtkTypeRevisionMacro(vtkOpenIGTLinkVideoSource,vtkPlusVideoSource);
  void PrintSelf(ostream& os, vtkIndent indent);   

  /*! Hardware device SDK version. */
  virtual std::string GetSdkVersion(); 

  /*!
    This is a singleton pattern New.  There will only be ONE
    reference to a vtkOutputWindow object per process.  Clients that
    call this must call Delete on the object so that the reference
    counting will work.   The single instance will be unreferenced when
    the program exits.
  */
  static vtkOpenIGTLinkVideoSource* New();
  
  /*! Return the singleton instance with no reference counting. */
  static vtkOpenIGTLinkVideoSource* GetInstance();

  /*!
    Supply a user defined output window. Call ->Delete() on the supplied
    instance after setting it.
  */
  static void SetInstance(vtkOpenIGTLinkVideoSource *instance);

  //BTX
  /*!
    Use this as a way of memory management when the
    program exits the SmartPointer will be deleted which
    will delete the Instance singleton
  */
  static vtkOpenIGTLinkVideoSourceCleanup Cleanup;
  //ETX

  /*! Read/write main configuration from/to xml data */
  virtual PlusStatus ReadConfiguration(vtkXMLDataElement* config); 

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

  /*! OpenIGTLink server address */ 
  char* ServerAddress; 

  /*! OpenIGTLink server port */ 
  int ServerPort; 

  /*! OpenIGTLink client socket */ 
  igtl::ClientSocket::Pointer ClientSocket;
    
private:
 
  static vtkOpenIGTLinkVideoSource* Instance;
  vtkOpenIGTLinkVideoSource(const vtkOpenIGTLinkVideoSource&);  // Not implemented.
  void operator=(const vtkOpenIGTLinkVideoSource&);  // Not implemented.
};

#endif
