/*=Plus=header=begin======================================================
  Progra  : Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkPlusSpinnakerVideoSource_h
#define __vtkPlusSpinnakerVideoSource_h

#include "vtkPlusDataCollectionExport.h"
#include "vtkPlusDevice.h"

/*!
  \class vtkPlusSpinnakerVideoSource
  \brief Interface class to Spinnaker API compatible Point Grey Cameras
  \ingroup PlusLibDataCollection
*/
class vtkPlusDataCollectionExport vtkPlusSpinnakerVideoSource : public vtkPlusDevice
{
public:

  static vtkPlusSpinnakerVideoSource *New();
  
  vtkTypeMacro(vtkPlusSpinnakerVideoSource, vtkPlusDevice);
  virtual void PrintSelf(ostream& os, vtkIndent indent);

  /*! Read configuration from xml data */
  virtual PlusStatus ReadConfiguration(vtkXMLDataElement* config);
  /*! Write configuration to xml data */
  PlusStatus WriteConfiguration(vtkXMLDataElement* config);

  /*!
  Record incoming data at the specified acquisition rate.  The recording
  continues indefinitely until StopRecording() is called.
  */
  PlusStatus InternalStartRecording();

  /*! Stop recording */
  PlusStatus InternalStopRecording();

  /*! Is this device a tracker */
  bool IsTracker() const { return false; }
  bool IsVirtual() const { return false; }
  
  /*! Get an update from the tracking system and push the new transforms to the tools. This function is called by the tracker thread.*/
  virtual PlusStatus InternalUpdate();

  virtual PlusStatus InternalConnect();
  virtual PlusStatus InternalDisconnect();

  /*! Verify the device is correctly configured */
  virtual PlusStatus NotifyConfigured();

protected:
  vtkPlusSpinnakerVideoSource();
  ~vtkPlusSpinnakerVideoSource();

private:
  vtkPlusSpinnakerVideoSource(const vtkPlusSpinnakerVideoSource&);
  void operator=(const vtkPlusSpinnakerVideoSource&);

  class vtkInternal;
  vtkInternal* Internal;
  
  unsigned long FrameNumber;
};

#endif