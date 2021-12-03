/*=Plus=header=begin======================================================
  Progra  : Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkPlusAzureKinect_h
#define __vtkPlusAzureKinect_h

#include "vtkPlusDataCollectionExport.h"
#include "vtkPlusDevice.h"

/*!
  \class vtkPlusAzureKinect
  \brief Interface class to Kinect Azure cameras
  \ingroup PlusLibDataCollection
*/
class vtkPlusDataCollectionExport vtkPlusAzureKinect : public vtkPlusDevice
{
public:

  static vtkPlusAzureKinect* New();

  vtkTypeMacro(vtkPlusAzureKinect, vtkPlusDevice);
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
  bool IsTracker() const
  {
    return false;
  }
  bool IsVirtual() const
  {
    return false;
  }

  /*! Get an update from the tracking system and push the new transforms to the tools. This function is called by the tracker thread.*/
  virtual PlusStatus InternalUpdate();

  virtual PlusStatus InternalConnect();
  virtual PlusStatus InternalDisconnect();

  /*! Verify the device is correctly configured */
  virtual PlusStatus NotifyConfigured();

protected:
  vtkPlusAzureKinect();
  ~vtkPlusAzureKinect();

private:
  vtkPlusAzureKinect(const vtkPlusAzureKinect&);
  void operator=(const vtkPlusAzureKinect&);

  class vtkInternal;
  vtkInternal* Internal;

  unsigned long FrameNumber;
};

#endif