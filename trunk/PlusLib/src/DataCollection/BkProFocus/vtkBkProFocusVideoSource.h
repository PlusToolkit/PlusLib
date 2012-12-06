/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkBkProFocusVideoSource_h
#define __vtkBkProFocusVideoSource_h

// PLUS Includes
#include "PlusConfigure.h"
#include "vtkPlusDevice.h"

class PlusBkProFocusReceiver;

/*!
\class vtkBkProFocusVideoSource 
\brief Class for acquiring ultrasound images from BK ProFocus scanners
\ingroup PlusLibImageAcquisition
*/
class VTK_EXPORT vtkBkProFocusVideoSource : public vtkPlusDevice
{
public:  
  enum ImagingModeType
  {
    BMode,
    RfMode
  };

  vtkTypeRevisionMacro(vtkBkProFocusVideoSource,vtkPlusDevice);
  void PrintSelf(ostream& os, vtkIndent indent);   

  static vtkBkProFocusVideoSource* New();

  virtual bool IsTracker() const { return false; }

  /*! Set the name of the BK ini file that stores connection and acquisition settings */
  vtkSetStringMacro(IniFileName);

  /*! Show Sapera grabbing window while connected. For debug purposes only. The state must not be changed while the video source is connected. */
  vtkSetMacro(ShowSaperaWindow, bool);
  
  /*! Show live BMode image while connected. For debug purposes only. The state must not be changed while the video source is connected. */
  vtkSetMacro(ShowBModeWindow, bool);

  PlusStatus GetFullIniFilePath(std::string &fullPath);

  void SetImagingMode(ImagingModeType imagingMode);
  
protected:
  /*! Constructor */
  vtkBkProFocusVideoSource();
  /*! Destructor */
  virtual ~vtkBkProFocusVideoSource();

  /*! Connect to device */
  virtual PlusStatus InternalConnect();

  /*! Disconnect from device */
  virtual PlusStatus InternalDisconnect();

  /*! Read main configuration from/to xml data */
  virtual PlusStatus ReadConfiguration(vtkXMLDataElement* config); 
  /*! Write main configuration from/to xml data */
  virtual PlusStatus WriteConfiguration(vtkXMLDataElement* config);

  void NewFrameCallback(void* pixelDataPtr, const int frameSizeInPix[2], PlusCommon::ITKScalarPixelType pixelType, US_IMAGE_TYPE imageType);
  friend PlusBkProFocusReceiver;

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

  /*! BK ini file storing the connection and acquisition settings */
  char* IniFileName;

  bool ShowSaperaWindow;
  bool ShowBModeWindow; 

  ImagingModeType ImagingMode;
  
private:


  // For internal storage of additional variables (to minimize the number of included headers)
  class vtkInternal;
  vtkInternal* Internal;  

  static bool vtkBkProFocusVideoSourceNewFrameCallback(void * data, int type, int sz, bool cine, int frmnum);
  vtkBkProFocusVideoSource(const vtkBkProFocusVideoSource&);  // Not implemented.
  void operator=(const vtkBkProFocusVideoSource&);  // Not implemented.
};

#endif
