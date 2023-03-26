/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.

Developed by ULL & IACTEC-IAC group
=========================================================Plus=header=end*/

#ifndef __vtkPlusDAQUSB3FRM13BCam_h
#define __vtkPlusDAQUSB3FRM13BCam_h

#include "vtkPlusDataCollectionExport.h"
#include "vtkPlusDevice.h"



/*!
\class vtkPlusDAQUSB3FRM13BCam
\brief Class for interfacing an Infrared Seek capture device and recording frames into a Plus buffer

\ingroup PlusLibDataCollection
*/

class vtkPlusDataCollectionExport vtkPlusDAQUSB3FRM13BCam : public vtkPlusDevice
{
public:
  static vtkPlusDAQUSB3FRM13BCam* New();
  vtkTypeMacro(vtkPlusDAQUSB3FRM13BCam, vtkPlusDevice);
  virtual void PrintSelf(ostream& os, vtkIndent indent) VTK_OVERRIDE;

  /*! Read configuration from xml data */
  PlusStatus ReadConfiguration(vtkXMLDataElement* config);
  /*! Write configuration to xml data */
  PlusStatus WriteConfiguration(vtkXMLDataElement* config);


  /*! Is this device a tracker */
  bool IsTracker() const { return false; }

  /*! Get an update from the tracking system and push the new transforms to the tools. This function is called by the tracker thread.*/
  PlusStatus InternalUpdate();

  /*! Verify the device is correctly configured */
  virtual PlusStatus NotifyConfigured();

protected:
  vtkPlusDAQUSB3FRM13BCam();
  ~vtkPlusDAQUSB3FRM13BCam();

  virtual PlusStatus InternalConnect() VTK_OVERRIDE;
  virtual PlusStatus InternalDisconnect() VTK_OVERRIDE;

protected:
  enum COLORDEPTH_E  {COLORDEPTH_8 = 8,COLORDEPTH_16 = 16,COLORDEPTH_32 = 32,COLORDEPTH_64 = 64} colorDepth;
  enum DATAMODE_E  { DATAMODE_8,DATAMODE_16,DATAMODE_32,DATAMODE_64 } dataMode;
  enum CAMERAMODE_E  { CAMERAMODE_SCAN, CAMERAMODE_LINE } cameraMode;
  bool deviceRunning;
  unsigned short * pImgBuf; 
  DWORD width;
  DWORD height;
  DWORD maxBuffSize;
};

#endif // __vtkPlusDAQUSB3FRM13BCam_h
