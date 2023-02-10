/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.

Developed by ULL & IACTEC-IAC group
=========================================================Plus=header=end*/

#ifndef __vtkPlusUltravioletPCOUVCam_h
#define __vtkPlusUltravioletPCOUVCam_h

#include "vtkPlusDataCollectionExport.h"
#include "vtkPlusDevice.h"

/*!
\class vtkPlusUltravioletPCOUVCam
\brief Class for interfacing an PCO Ultraviolet capture device and recording frames into a Plus buffer

\ingroup PlusLibDataCollection
*/

class vtkPlusDataCollectionExport vtkPlusUltravioletPCOUVCam : public vtkPlusDevice
{
public:
  static vtkPlusUltravioletPCOUVCam* New();
  vtkTypeMacro(vtkPlusUltravioletPCOUVCam, vtkPlusDevice);
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
  vtkPlusUltravioletPCOUVCam();
  ~vtkPlusUltravioletPCOUVCam();

  virtual PlusStatus InternalConnect() VTK_OVERRIDE;
  virtual PlusStatus InternalDisconnect() VTK_OVERRIDE;

protected:
  int device;
  WORD *pImgBuf;
  HANDLE cam;
  HANDLE BufEvent;
  short BufNum;
  WORD XResAct, YResAct, XResMax, YResMax;
  DWORD bufsize;
  DWORD dwDelay;
  DWORD dwExposure;
  WORD wTimeBaseDelay;
  WORD wTimeBaseExposure;
};

#endif // __vtkPlusUltravioletPCOUVCam_h
