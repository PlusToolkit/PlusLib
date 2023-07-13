/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.

Developed by ULL & IACTEC-IAC group
=========================================================Plus=header=end*/

#ifndef __vtkPlusDAQMMFCombinedVideo_h
#define __vtkPlusDAQMMFCombinedVideo_h

#include "vtkPlusDataCollectionExport.h"
#include "vtkPlusDevice.h"

/*!
\class vtkPlusDAQMMFCombinedVideo
\brief Class for interfacing an Infrared capture device and recording frames into a Plus buffer

\ingroup PlusLibDataCollection
*/

class vtkPlusDataCollectionExport vtkPlusDAQMMFCombinedVideo : public vtkPlusDevice
{
public:
  static vtkPlusDAQMMFCombinedVideo* New();
  vtkTypeMacro(vtkPlusDAQMMFCombinedVideo, vtkPlusDevice);
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
  vtkPlusDAQMMFCombinedVideo();
  ~vtkPlusDAQMMFCombinedVideo();

  virtual PlusStatus InternalConnect() VTK_OVERRIDE;
  virtual PlusStatus InternalDisconnect() VTK_OVERRIDE;

protected:
  typedef unsigned short CAMERADATATYPE_t;
  enum COLORDEPTH_E  {COLORDEPTH_8 = 8,COLORDEPTH_16 = 16,COLORDEPTH_32 = 32,COLORDEPTH_64 = 64} colorDepth;
  enum DATAMODE_E  { DATAMODE_8 = 0,DATAMODE_16 = 1,DATAMODE_32 = 2,DATAMODE_64 = 3 } m_dataMode;
  enum CAMERAMODE_E  { CAMERAMODE_SCAN, CAMERAMODE_LINE } cameraMode;
  bool deviceRunning;
  CAMERADATATYPE_t* pImgBuf;
  unsigned char * pImgBufAux;
  DWORD m_width;
  DWORD m_height;
  DWORD m_nwidth;
  DWORD m_nheight;
  DWORD m_maxBuffSize;
  DWORD m_dwCharCount;
  int m_nbytesMode;
  double m_currentTime = UNDEFINED_TIMESTAMP;
};

#endif // __vtkPlusDAQMMFCombinedVideo_h
