/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkICCapturingSource_h
#define __vtkICCapturingSource_h

#include "vtkDataCollectionExport.h"

#include "vtkPlusDevice.h"
class ICCapturingListener; 

class vtkDataCollectionExport vtkICCapturingSource;

/*!
\class vtkICCapturingSourceCleanup 
\brief Class that cleans up (deletes singleton instance of) vtkICCapturingSource when destroyed
\ingroup PlusLibDataCollection
*/
class vtkDataCollectionExport vtkICCapturingSourceCleanup
{
public:
  vtkICCapturingSourceCleanup();
  ~vtkICCapturingSourceCleanup();
};

/*!
\class vtkICCapturingSource 
\brief Class for providing video input interfaces between VTK and ICCapturing frame grabber device
\ingroup PlusLibDataCollection
*/
class vtkDataCollectionExport vtkICCapturingSource : public vtkPlusDevice
{
public:
  vtkTypeMacro(vtkICCapturingSource,vtkPlusDevice);
  void PrintSelf(ostream& os, vtkIndent indent);   
  /*! This is a singleton pattern New.  There will only be ONE
  reference to a vtkOutputWindow object per process.  Clients that
  call this must call Delete on the object so that the reference
  counting will work.   The single instance will be unreferenced when
  the program exits
  */
  static vtkICCapturingSource* New();
  /*! Return the singleton instance with no reference counting. */
  static vtkICCapturingSource* GetInstance();

  /*! Supply a user defined output window. Call ->Delete() on the supplied instance after setting it. */
  static void SetInstance(vtkICCapturingSource *instance);

  //BTX
  /*! Use this as a way of memory management when the
  program exits the SmartPointer will be deleted which
  will delete the Instance singleton
  */
  static vtkICCapturingSourceCleanup Cleanup;
  //ETX

  /*! Hardware device SDK version. */
  virtual std::string GetSdkVersion();
  /*! Read configuration from xml data */  virtual PlusStatus ReadConfiguration(vtkXMLDataElement* config); 
  /*! Write configuration to xml data */
  virtual PlusStatus WriteConfiguration(vtkXMLDataElement* config);

  /*! Set the IC capturing device name (e.g. "DFG/USB2-lt") */
  vtkSetStringMacro(DeviceName); 
  /*! Get the IC capturing device name (e.g. "DFG/USB2-lt") */
  vtkGetStringMacro(DeviceName); 

  /*! Set the IC capturing device video norm (e.g. "PAL_B", "NTSC_M") */
  vtkSetStringMacro(VideoNorm); 
  /*! Get the IC capturing device video norm (e.g. "PAL_B", "NTSC_M") */
  vtkGetStringMacro(VideoNorm); 

  /*! Set the IC capturing device video format (e.g. "Y800 (640x480)" ) */
  vtkSetStringMacro(VideoFormat); 
  /*! Get the IC capturing device video format (e.g. "Y800 (640x480)" ) */
  vtkGetStringMacro(VideoFormat); 

  /*! Set the IC capturing device input channel (e.g. "01 Video: SVideo" ) */
  vtkSetStringMacro(InputChannel); 
  /*! Get the IC capturing device input channel (e.g. "01 Video: SVideo" ) */
  vtkGetStringMacro(InputChannel); 

  /*! Set the IC capturing device buffer size ( Default: 50 frame ) */
  vtkSetMacro(ICBufferSize, int); 
  /*! Get the IC capturing device buffer size ( Default: 50 frame ) */
  vtkGetMacro(ICBufferSize, int);

  /*! Set the frame size that will be requested when setting the video format. Acquired image frames may be smaller if clipping is applied in the video source. */
  vtkSetVector2Macro(FrameSize,int);
  /*! Get the frame size requested from the framegrabber. */
  vtkGetVector2Macro(FrameSize,int);

  /*! Verify the device is correctly configured */
  virtual PlusStatus NotifyConfigured();

  virtual bool IsTracker() const { return false; }

  void GetListOfCaptureDevices(std::vector< std::string > &deviceNames);

  void GetListOfCaptureVideoNorms(std::vector< std::string > &videoNorms, const std::string& deviceName);

  /*! Get list of available video modes (video format & frame size) */
  void GetListOfCaptureVideoModes(std::vector< std::string > &videoModes, const std::string& deviceName, const std::string& videoNorm);

  /*! Print the name of all supported video modes (video format & frame size) for available capture devices and video norms */
  void LogListOfCaptureDevices();
  
protected:
  /*! Constructor */
  vtkICCapturingSource();
  /*! Destructor */
  ~vtkICCapturingSource();

  /*! Device-specific connect */
  virtual PlusStatus InternalConnect();

  /*! Device-specific disconnect */
  virtual PlusStatus InternalDisconnect();

  /*! Device-specific recording start */
  virtual PlusStatus InternalStartRecording();

  /*! Device-specific recording stop */
  virtual PlusStatus InternalStopRecording();

  /*! Adds a frame to the frame buffer. Called whenever the driver notified a new frame acquisition. */
  PlusStatus AddFrameToBuffer(unsigned char * data, unsigned long size, unsigned long frameNumber);

  /*! Parse DShowLib video format string (format + frame size) and if successful set VideoFormat and FrameSize */
  void ParseDShowLibVideoFormatString(const char* videoFormatFrameSizeString);

  /*! Constructs a DShowLib video format string (format + frame size) from VideoFormat and FrameSize */
  std::string GetDShowLibVideoFormatString();

  /*! Frame grabber device - DShowLib::Grabber type */
  void* FrameGrabber;
  ICCapturingListener* FrameGrabberListener; 

  /*! Device name (e.g. DFG/USB2-lt) */
  char* DeviceName;

  /*! Video norm (e.g. PAL_B or NTSC_M) */
  char* VideoNorm;

  /*! Video format (e.g. Y800) */
  char* VideoFormat;

  /*! Input channel name */
  char* InputChannel; 

  /*! IC buffer size */
  int ICBufferSize; 

  /*! Frame size of the captured image */
  int FrameSize[2];

private:

  static vtkICCapturingSource* Instance;
  static bool vtkICCapturingSourceNewFrameCallback(unsigned char * data, unsigned long size, unsigned long frameNumber);
  vtkICCapturingSource(const vtkICCapturingSource&);  // Not implemented.
  void operator=(const vtkICCapturingSource&);  // Not implemented.
};

#endif
