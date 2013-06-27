/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkBkProFocusCameraLinkVideoSource_h
#define __vtkBkProFocusCameraLinkVideoSource_h

// PLUS Includes
#include "PlusConfigure.h"
#include "vtkPlusDevice.h"

class PlusBkProFocusCameraLinkReceiver;

/*!
\class vtkBkProFocusCameraLinkVideoSource 
\brief Class for acquiring ultrasound images from BK ProFocus scanners
\ingroup PlusLibImageAcquisition
*/
class VTK_EXPORT vtkBkProFocusCameraLinkVideoSource : public vtkPlusDevice
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

  vtkTypeRevisionMacro(vtkBkProFocusCameraLinkVideoSource,vtkPlusDevice);
  void PrintSelf(ostream& os, vtkIndent indent);   

  static vtkBkProFocusCameraLinkVideoSource* New();

  virtual bool IsTracker() const { return false; }

  /*! Set the name of the BK ini file that stores connection and acquisition settings */
  vtkSetStringMacro(IniFileName);

  /*! Show Sapera grabbing window while connected. For debug purposes only. The state must not be changed while the video source is connected. */
  vtkSetMacro(ShowSaperaWindow, bool);
  
  /*! Show live BMode image while connected. For debug purposes only. The state must not be changed while the video source is connected. */
  vtkSetMacro(ShowBModeWindow, bool);

  PlusStatus GetFullIniFilePath(std::string &fullPath);

  void SetImagingMode(ImagingModeType imagingMode);

  /*! Verify the device is correctly configured */
  virtual PlusStatus NotifyConfigured();
  
protected:
  /*! Constructor */
  vtkBkProFocusCameraLinkVideoSource();
  /*! Destructor */
  virtual ~vtkBkProFocusCameraLinkVideoSource();

  /*! Connect to device */
  virtual PlusStatus InternalConnect();

  /*! Disconnect from device */
  virtual PlusStatus InternalDisconnect();

  /*! Read main configuration from/to xml data */
  virtual PlusStatus ReadConfiguration(vtkXMLDataElement* config); 
  /*! Write main configuration from/to xml data */
  virtual PlusStatus WriteConfiguration(vtkXMLDataElement* config);

  void NewFrameCallback(void* pixelDataPtr, const int frameSizeInPix[2], PlusCommon::ITKScalarPixelType pixelType, US_IMAGE_TYPE imageType);
  friend PlusBkProFocusCameraLinkReceiver;

  /*! Called by BK to log information messages */
  static void LogInfoMessageCallback(char *msg);

  /*! Called by BK to log debug messages */
  static void LogDebugMessageCallback(char *msg);

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

  static bool vtkBkProFocusCameraLinkVideoSourceNewFrameCallback(void * data, int type, int sz, bool cine, int frmnum);
  vtkBkProFocusCameraLinkVideoSource(const vtkBkProFocusCameraLinkVideoSource&);  // Not implemented.
  void operator=(const vtkBkProFocusCameraLinkVideoSource&);  // Not implemented.
};

#endif
