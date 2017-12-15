/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.

Developed by MACBIOIDI & IACTEC group 
=========================================================Plus=header=end*/

#ifndef __vtkInfraredSeekCam_h
#define __vtkInfraredSeekCam_h

#include "vtkPlusDataCollectionExport.h"
#include "vtkPlusDevice.h"

// Seek Cam
#include <seek.h>
#include <SeekCam.h>
/*!
\class vtkInfraredSeekCam
\brief Class for interfacing an Infrared Seek capture device and recording frames into a Plus buffer

\ingroup PlusLibDataCollection
*/

class vtkPlusDataCollectionExport vtkInfraredSeekCam : public vtkPlusDevice
{
public:
  static vtkInfraredSeekCam* New();
  vtkTypeMacro(vtkInfraredSeekCam, vtkPlusDevice);
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
  vtkInfraredSeekCam();
  ~vtkInfraredSeekCam();

  virtual PlusStatus InternalConnect() VTK_OVERRIDE;
  virtual PlusStatus InternalDisconnect() VTK_OVERRIDE;

protected:
  std::shared_ptr<LibSeek::SeekThermalPro>         Capture;
  std::shared_ptr<cv::Mat>                  Frame;
};

#endif // __vtkInfraredSeekCam_h