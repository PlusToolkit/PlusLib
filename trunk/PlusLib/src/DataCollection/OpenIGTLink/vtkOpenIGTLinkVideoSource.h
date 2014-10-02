/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkOpenIGTLinkVideoSource_h
#define __vtkOpenIGTLinkVideoSource_h

#include "PlusConfigure.h"
#include "vtkDataCollectionExport.h"

#include "vtkOpenIGTLinkDevice.h"

/*!
  \class vtkOpenIGTLinkVideoSource 
  \brief VTK interface for video input from OpenIGTLink image message

  vtkOpenIGTLinkVideoSource is a class for providing video input interfaces between VTK and OpenIGTLink ready video device. 

  \ingroup PlusLibDataCollection
*/ 
class vtkDataCollectionExport vtkOpenIGTLinkVideoSource : public vtkOpenIGTLinkDevice
{
public:

  static vtkOpenIGTLinkVideoSource *New();
  vtkTypeMacro(vtkOpenIGTLinkVideoSource,vtkOpenIGTLinkDevice);
  virtual void PrintSelf(ostream& os, vtkIndent indent);   

  /*! Get an update from the tracking system and push the new transforms to the tools. This function is called by the tracker thread.*/
  PlusStatus InternalUpdate();

  /*! Read configuration from xml data */
  virtual PlusStatus ReadConfiguration( vtkXMLDataElement* config ); 
  
  /*! Write configuration to xml data */
  virtual PlusStatus WriteConfiguration(vtkXMLDataElement* config);  

  virtual bool IsTracker() const { return false; }

  virtual PlusStatus SetImageMessageEmbeddedTransformName(const char* nameString);

  /*! Verify the device is correctly configured */
  virtual PlusStatus NotifyConfigured();

protected:
  vtkOpenIGTLinkVideoSource();
  virtual ~vtkOpenIGTLinkVideoSource();
    
  /*! Name of the transform that is supplied with the IMAGE OpenIGTLink message */ 
  PlusTransformName ImageMessageEmbeddedTransformName;
private:

  vtkOpenIGTLinkVideoSource(const vtkOpenIGTLinkVideoSource&);  // Not implemented.
  void operator=(const vtkOpenIGTLinkVideoSource&);  // Not implemented.
};

#endif
