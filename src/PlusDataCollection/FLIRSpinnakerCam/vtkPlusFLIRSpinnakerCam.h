/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.

Developed by ULL & IACTEC-IAC group
=========================================================Plus=header=end*/

#ifndef __vtkPlusFLIRSpinnakerCam_h
#define __vtkPlusFLIRSpinnakerCam_h

#include "vtkPlusDataCollectionExport.h"
#include "vtkPlusDevice.h"
#include "Spinnaker.h"


/*!
\class vtkPlusFLIRSpinnakerCam
\brief Class for interfacing an FLIR Systems Spinnaker capture device and recording frames into a Plus buffer

\ingroup PlusLibDataCollection
*/

class vtkPlusDataCollectionExport vtkPlusFLIRSpinnakerCam : public vtkPlusDevice
{
public:
  static vtkPlusFLIRSpinnakerCam* New();
  vtkTypeMacro(vtkPlusFLIRSpinnakerCam, vtkPlusDevice);
  virtual void PrintSelf(ostream& os, vtkIndent indent) VTK_OVERRIDE;

  /*! Read configuration from xml data */
  PlusStatus ReadConfiguration(vtkXMLDataElement* config);
  /*! Write configuration to xml data */
  PlusStatus WriteConfiguration(vtkXMLDataElement* config);

  /*! Manage device frozen state */
  PlusStatus FreezeDevice(bool freeze);

  /*! Is this device a tracker */
  bool IsTracker() const { return false; }

  /*! Get an update from the tracking system and push the new transforms to the tools. This function is called by the tracker thread.*/
  PlusStatus InternalUpdate();

  /*! Verify the device is correctly configured */
  virtual PlusStatus NotifyConfigured();

protected:
  vtkPlusFLIRSpinnakerCam();
  ~vtkPlusFLIRSpinnakerCam();

  virtual PlusStatus InternalConnect() VTK_OVERRIDE;
  virtual PlusStatus InternalDisconnect() VTK_OVERRIDE;

protected:
  DWORD dwExposure;
};

#endif // __vtkPlusFLIRSpinnakerCam_h
