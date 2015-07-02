/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkCapistranoVideoSource_h
#define __vtkCapistranoVideoSource_h

#include "vtkDataCollectionExport.h"

#include "vtkPlusDevice.h"

/*!
 \class vtkCapistranoVideoSource
 \brief Class for acquiring ultrasound images from Capistrano Labs USB
 ultrasound systems.

 Requires PLUS_USE_CAPISTRANO_VIDEO option in CMake.
 Requires the Capistrano cSDK2013 (SDK provided by Capistrano Labs).

 \ingroup PlusLibDataCollection.
*/
class vtkDataCollectionExport vtkCapistranoVideoSource: public vtkPlusDevice
{
public:
  static vtkCapistranoVideoSource * New();
  vtkTypeMacro(vtkCapistranoVideoSource, vtkPlusDevice);

  void PrintSelf(ostream& os, vtkIndent indent);   

  virtual bool IsTracker() const { return false; }

  /*! Read configuration from xml data */  
  virtual PlusStatus ReadConfiguration(vtkXMLDataElement* config); 
  /*! Write configuration to xml data */
  virtual PlusStatus WriteConfiguration(vtkXMLDataElement* config);    

  /*! Verify the device is correctly configured */
  virtual PlusStatus NotifyConfigured();

  virtual std::string GetSdkVersion();

protected:
  vtkCapistranoVideoSource();
  ~vtkCapistranoVideoSource();

  /*! Device-specific connect */
  virtual PlusStatus InternalConnect();

  /*! Device-specific disconnect */
  virtual PlusStatus InternalDisconnect();

private:
  vtkCapistranoVideoSource(const vtkCapistranoVideoSource &); // Not implemented
  void operator=(const vtkCapistranoVideoSource &); // Not implemented
};

#endif
