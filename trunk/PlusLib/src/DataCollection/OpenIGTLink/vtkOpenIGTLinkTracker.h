/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkOpenIGTLinkTracker_h
#define __vtkOpenIGTLinkTracker_h

#include "PlusConfigure.h"
#include "vtkDataCollectionExport.h"

#include "vtkOpenIGTLinkDevice.h"
#include "igtlClientSocket.h"
#include "igtlMessageBase.h"

/*!
\class vtkOpenIGTLinkTracker 
\brief OpenIGTLink tracker client  

\ingroup PlusLibDataCollection
*/
class vtkDataCollectionExport vtkOpenIGTLinkTracker : public vtkOpenIGTLinkDevice
{
public:

  static vtkOpenIGTLinkTracker *New();
  vtkTypeMacro( vtkOpenIGTLinkTracker,vtkOpenIGTLinkDevice );
  virtual void PrintSelf( ostream& os, vtkIndent indent );

  /*! Disconnect from device */
  virtual PlusStatus InternalDisconnect();

  /*! Get an update from the tracking system and push the new transforms to the tools. This function is called by the tracker thread.*/
  PlusStatus InternalUpdate();

  /*! Read configuration from xml data */
  virtual PlusStatus ReadConfiguration( vtkXMLDataElement* config ); 
  
  /*! Write configuration to xml data */
  virtual PlusStatus WriteConfiguration(vtkXMLDataElement* config);  

  virtual bool IsTracker() const { return true; }

  /*! 
    Set the internal tracker coordinate system name that is send to the tracker
    when tracking start is requested using an STT_TDATA message.
  */ 
  vtkSetStringMacro(TrackerInternalCoordinateSystemName); 
  /*! Get the internal tracker coordinate system name */ 
  vtkGetStringMacro(TrackerInternalCoordinateSystemName); 

protected:
  vtkOpenIGTLinkTracker();
  virtual ~vtkOpenIGTLinkTracker();

  virtual PlusStatus SendRequestedMessageTypes();

  /*! Process a TRANSFORM or POSITION message (add the received transform to the buffer) */
  PlusStatus ProcessTransformMessage(igtl::MessageHeader::Pointer headerMsg);

  /*! Process a TDATA message (add all the received transforms to the buffers) */
  PlusStatus ProcessTDataMessage(igtl::MessageHeader::Pointer headerMsg);

  /*!
    Store the latest transforms again in the buffers with the provided timestamp.
    If no transforms are defined then identity transform will be stored.
    If there is a transform defined already with the same timestamp then it will not be overwritten.
  */
  PlusStatus StoreMostRecentTransformValues(double unfilteredTimestamp);

  vtkSetMacro(UseLastTransformsOnReceiveTimeout, bool);

  /*!
    Internal tracker coordinate system name that is send to the tracker when tracking start is requested
    using an STT_TDATA message.
  */
  char* TrackerInternalCoordinateSystemName;

  /*! Use the last known transform value if not received a new value. Useful for servers that only notify about changes in the transforms. */
  bool UseLastTransformsOnReceiveTimeout;

private:  
  
  vtkOpenIGTLinkTracker( const vtkOpenIGTLinkTracker& );
  void operator=( const vtkOpenIGTLinkTracker& );    
};

#endif
