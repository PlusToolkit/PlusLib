/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkPlusBkProFocusCameraLinkVideoSource_h
#define __vtkPlusBkProFocusCameraLinkVideoSource_h

// PLUS Includes
#include "PlusConfigure.h"
#include "vtkPlusDataCollectionExport.h"

#include "vtkPlusDevice.h"

class PlusBkProFocusCameraLinkReceiver;

/*!
\class vtkPlusBkProFocusCameraLinkVideoSource
\brief Class for acquiring ultrasound images from BK ProFocus scanners

Requires the PLUS_USE_BKPROFOCUS_VIDEO and PLUS_USE_BKPROFOCUS_CAMERALINK options in CMake.

Need to install the DALSA Sapera package, otherwise the applications will not start because of missing the SapClassBasic72.dll.
This dll can only be used with 64-bit Plus build on 64-bit OS and 32-bit Plus build on a 32-bit OS.
Therefore on a 64-bit Windows system Plus shall be built in 64-bit mode.

Requires GrabbieLib (SDK provided by BK).

\ingroup PlusLibDataCollection
*/
class vtkPlusDataCollectionExport vtkPlusBkProFocusCameraLinkVideoSource : public vtkPlusDevice
{
public:
  enum ImagingModeType
  {
    BMode,
    RfMode
  };

  enum ScanPlaneType
  {
    Transverse,
    Sagittal
  };

  vtkTypeMacro(vtkPlusBkProFocusCameraLinkVideoSource, vtkPlusDevice);
  virtual void PrintSelf(ostream& os, vtkIndent indent) VTK_OVERRIDE;

  static vtkPlusBkProFocusCameraLinkVideoSource* New();

  virtual bool IsTracker() const { return false; }

  /*! Set the name of the BK ini file that stores connection and acquisition settings */
  vtkSetStringMacro(IniFileName);

  /*! Show Sapera grabbing window while connected. For debug purposes only. The state must not be changed while the video source is connected. */
  vtkSetMacro(ShowSaperaWindow, bool);

  /*! Show live BMode image while connected. For debug purposes only. The state must not be changed while the video source is connected. */
  vtkSetMacro(ShowBModeWindow, bool);

  PlusStatus GetFullIniFilePath(std::string& fullPath);

  void SetImagingMode(ImagingModeType imagingMode);

  /*! Verify the device is correctly configured */
  virtual PlusStatus NotifyConfigured();

protected:
  /*! Constructor */
  vtkPlusBkProFocusCameraLinkVideoSource();
  /*! Destructor */
  virtual ~vtkPlusBkProFocusCameraLinkVideoSource();

  /*! Connect to device */
  virtual PlusStatus InternalConnect();

  /*! Disconnect from device */
  virtual PlusStatus InternalDisconnect();

  /*! Read main configuration from/to xml data */
  virtual PlusStatus ReadConfiguration(vtkXMLDataElement* config);
  /*! Write main configuration from/to xml data */
  virtual PlusStatus WriteConfiguration(vtkXMLDataElement* config);

  void NewFrameCallback(void* pixelDataPtr, const FrameSizeType& frameSizeInPix, igsioCommon::VTKScalarPixelType pixelType, US_IMAGE_TYPE imageType);
  friend PlusBkProFocusCameraLinkReceiver;

  /*! Called by BK to log information messages */
  static void LogInfoMessageCallback(char* msg);

  /*! Called by BK to log debug messages */
  static void LogDebugMessageCallback(char* msg);

  /*!
    Record incoming video.  The recording
    continues indefinitely until StopRecording() is called.
  */
  virtual PlusStatus InternalStartRecording();

  /*! Stop recording or playing */
  virtual PlusStatus InternalStopRecording();

  static void EventCallback(void*, char*, size_t);

  vtkPlusChannel* FindChannelByPlane();

  /*! BK ini file storing the connection and acquisition settings */
  char* IniFileName;

  bool ShowSaperaWindow;
  bool ShowBModeWindow;

  ImagingModeType ImagingMode;

  std::map<vtkPlusChannel*, bool> ChannelConfiguredMap;

private:
  // For internal storage of additional variables (to minimize the number of included headers)
  class vtkInternal;
  vtkInternal* Internal;

  static bool vtkPlusBkProFocusCameraLinkVideoSourceNewFrameCallback(void* data, int type, int sz, bool cine, int frmnum);
  vtkPlusBkProFocusCameraLinkVideoSource(const vtkPlusBkProFocusCameraLinkVideoSource&);  // Not implemented.
  void operator=(const vtkPlusBkProFocusCameraLinkVideoSource&);  // Not implemented.
};

#endif