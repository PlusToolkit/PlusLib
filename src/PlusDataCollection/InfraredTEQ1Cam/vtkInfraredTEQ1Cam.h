/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.

Developed by MACBIOIDI-ULPGC & IACTEC-IAC group
=========================================================Plus=header=end*/

#ifndef __vtkInfraredTEQ1Cam_h
#define __vtkInfraredTEQ1Cam_h

#include "vtkPlusDataCollectionExport.h"
#include "vtkPlusDevice.h"

namespace i3
{
  class TE_B;
}

/*!
\class vtkInfraredTEQ1Cam
\brief Class for interfacing an Infrared Seek capture device and recording frames into a Plus buffer

\ingroup PlusLibDataCollection
*/

class vtkPlusDataCollectionExport vtkInfraredTEQ1Cam : public vtkPlusDevice
{
public:
  static vtkInfraredTEQ1Cam* New();
  vtkTypeMacro(vtkInfraredTEQ1Cam, vtkPlusDevice);
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
  vtkInfraredTEQ1Cam();
  ~vtkInfraredTEQ1Cam();

  virtual PlusStatus InternalConnect() VTK_OVERRIDE;
  virtual PlusStatus InternalDisconnect() VTK_OVERRIDE;

protected:
  int device;

  i3::TE_B* pTE;
  float* pImgBuf;
  int width;
  int height;
};

#endif // __vtkInfraredTEQ1Cam_h
