/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkPlusOpenHapticsDevice_h
#define __vtkPlusOpenHapticsDevice_h

#include "vtkPlusDataCollectionExport.h"

#include "vtkPlusDevice.h"
#include <HD/hd.h>

class vtkMatrix4x4;
class vtkTransform;


/*!
  \class vtkPlusOpenHapticsDevice
  \brief Device interface for Open Haptics devices
  \ingroup PlusLibDataCollection
*/
class vtkPlusDataCollectionExport vtkPlusOpenHapticsDevice : public vtkPlusDevice
{
public:

  static vtkPlusOpenHapticsDevice* New();
  vtkTypeMacro(vtkPlusOpenHapticsDevice, vtkPlusDevice);

  /*! Hardware device SDK version. */
  virtual std::string GetSdkVersion();

  virtual bool IsTracker() const { return true; }

  /*!
    Probe to see if the device is present.
  */
  PlusStatus Probe();

  /*!
    Get an update from the device and push the new transforms
    to the tools.  This should only be used within vtkTracker.cxx.
  */
  PlusStatus InternalUpdate();


  /*! Connect to the device hardware */
  PlusStatus InternalConnect();
  /*! Disconnect from the device hardware */
  PlusStatus InternalDisconnect();

  /*! Read configuration from xml data */
  virtual PlusStatus ReadConfiguration(vtkXMLDataElement* config);

  /*! Read configuration from xml data */
  virtual PlusStatus WriteConfiguration(vtkXMLDataElement* config);

  vtkSetStdStringMacro(DeviceName);
  vtkGetStdStringMacro(DeviceName);

  PlusStatus NotifyConfigured();


protected:
  vtkPlusOpenHapticsDevice();
  ~vtkPlusOpenHapticsDevice();

  /*! Index of the last frame number. This is used for providing a frame number when the tracker doesn't return any transform */
  unsigned int LastFrameNumber;

  unsigned int FrameNumber;

  std::string DeviceName;


private:
  vtkPlusOpenHapticsDevice(const vtkPlusOpenHapticsDevice&);
  void operator=(const vtkPlusOpenHapticsDevice&);
  static HDCallbackCode HDCALLBACK positionCallback(void* pData);

  HHD DeviceHandle;     ///< device handle
  vtkSmartPointer<vtkTransform> toolTransform;
  vtkSmartPointer<vtkTransform> rotation;
  vtkSmartPointer<vtkMatrix4x4> velMatrix;
  vtkSmartPointer<vtkMatrix4x4> buttonMatrix;
  vtkSmartPointer<vtkMatrix4x4> toolMatrix;
  vtkSmartPointer<vtkMatrix4x4> rasCorrection;
};

#endif
