/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/ 

#ifndef __vtkPlusDeviceFactory_h
#define __vtkPlusDeviceFactory_h

#include "vtkObject.h"
#include "PlusCommon.h"

class vtkPlusDevice;

/*!
  \class vtkPlusDeviceFactory 
  \brief Factory class of supported devices

  This class is a factory class of supported trackers and video sources to localize the object creation code.

  \ingroup PlusLibPlusCommon
*/
class VTK_EXPORT vtkPlusDeviceFactory : public vtkObject
{
public:
  static vtkPlusDeviceFactory *New();
  vtkTypeRevisionMacro(vtkPlusDeviceFactory, vtkObject);
  virtual void PrintSelf(ostream& os, vtkIndent indent);

  /*! Print all supported devices with SDK version number */
  virtual void PrintAvailableDevices(ostream& os, vtkIndent indent);

  /*! Create a new device instance from device type, delete previous device if's not NULL */ 
  PlusStatus CreateInstance(const char* aDeviceType, vtkPlusDevice* &aDevice, const std::string &aDeviceId);

protected:
  vtkPlusDeviceFactory(void);
  virtual ~vtkPlusDeviceFactory(void);

  /*! Function pointer for storing New() static methods of vtkPlusDevice classes */ 
  typedef vtkPlusDevice* (*PointerToDevice)(); 
  /*! Map tracker types and the New() static methods of vtkPlusDevice classes */ 
  std::map<std::string,PointerToDevice> DeviceTypes; 

private:
  vtkPlusDeviceFactory(const vtkPlusDeviceFactory&);
  void operator=(const vtkPlusDeviceFactory&);
};

#endif