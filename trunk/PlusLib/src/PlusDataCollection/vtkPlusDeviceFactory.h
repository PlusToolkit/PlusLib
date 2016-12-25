/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/ 

#ifndef __vtkPlusDeviceFactory_h
#define __vtkPlusDeviceFactory_h

#include "vtkPlusDataCollectionExport.h"

#include "vtkObject.h"
#include "PlusCommon.h"

class vtkPlusDevice;

/*!
  \class vtkPlusDeviceFactory 
  \brief Factory class of supported devices

  This class is a factory class of supported trackers and video sources to localize the object creation code.

  \ingroup PlusLibPlusCommon
*/
class vtkPlusDataCollectionExport vtkPlusDeviceFactory : public vtkObject
{
public:
  static vtkPlusDeviceFactory *New();
  vtkTypeMacro(vtkPlusDeviceFactory, vtkObject);
  virtual void PrintSelf(ostream& os, vtkIndent indent) VTK_OVERRIDE;

  /*! Print all supported devices with SDK version number */
  virtual void PrintAvailableDevices(ostream& os, vtkIndent indent);

  /*! Create a new device instance from device type, delete previous device if's not NULL */ 
  PlusStatus CreateInstance(const char* aDeviceType, vtkPlusDevice* &aDevice, const std::string &aDeviceId);

  /*! Retrieve a C++ class name given a factory name */
  virtual PlusStatus GetDeviceClassName(const std::string& deviceTypeName, std::string& classNameOut) const;
  
  /*! Retrieve a factory device type given a C++ class name */
  virtual PlusStatus GetDeviceTypeName(const std::string& deviceClassName, std::string& typeNameOut) const;

protected:
  vtkPlusDeviceFactory(void);
  virtual ~vtkPlusDeviceFactory(void);

  /*! Function pointer for storing New() static methods of vtkPlusDevice classes */ 
  typedef vtkPlusDevice* (*PointerToDevice)(); 
  /*! Map tracker types and the New() static methods of vtkPlusDevice classes */ 
  std::map<std::string,PointerToDevice> DeviceTypes;
  /*! Lookup map to translate factory names to C++ class names */
  std::map<std::string, std::string> DeviceTypeClassNames;

private:
  vtkPlusDeviceFactory(const vtkPlusDeviceFactory&);
  void operator=(const vtkPlusDeviceFactory&);
};

#endif