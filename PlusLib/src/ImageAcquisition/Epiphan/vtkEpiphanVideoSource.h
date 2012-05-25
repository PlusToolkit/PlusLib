/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkEpiphanVideoSource_h
#define __vtkEpiphanVideoSource_h

#include "vtkPlusVideoSource.h"
#include "vtkMultiThreader.h"
#include "vtkTimerLog.h"
#include "epiphan/frmgrab.h"

/* exit codes */
#define V2U_GRABFRAME_STATUS_OK       0  /* successful completion */
#define V2U_GRABFRAME_STATUS_NODEV    1  /* VGA2USB device not found */
#define V2U_GRABFRAME_STATUS_VMERR    2  /* Video mode detection failure */
#define V2U_GRABFRAME_STATUS_NOSIGNAL 3  /* No signal detected */
#define V2U_GRABFRAME_STATUS_GRABERR  4  /* Capture error */
#define V2U_GRABFRAME_STATUS_IOERR    5  /* File save error */
#define V2U_GRABFRAME_STATUS_CMDLINE  6  /* Command line syntax error */


class VTK_EXPORT vtkEpiphanVideoSource;

/*!
  \class vtkICCapturingSourceCleanup 
  \brief Class that cleans up (deletes singleton instance of) vtkICCapturingSource when destroyed
  \ingroup PlusLibImageAcquisition
*/
/*
class VTK_EXPORT vtkEpiphanVideoSourceCleanup
{
public:
	vtkEpiphanVideoSourceCleanup();
	~vtkEpiphanVideoSourceCleanup();
};
*/

/*!
  \class vtkICCapturingSource 
  \brief Class for providing video input interfaces between VTK and ICCapturing frame grabber device
  \ingroup PlusLibImageAcquisition
*/
class VTK_EXPORT vtkEpiphanVideoSource : public vtkPlusVideoSource
{
public:
	static vtkEpiphanVideoSource *New();
	vtkTypeRevisionMacro(vtkEpiphanVideoSource,vtkPlusVideoSource);
	void PrintSelf(ostream& os, vtkIndent indent);   
  /*! This is a singleton pattern New.  There will only be ONE
	 reference to a vtkOutputWindow object per process.  Clients that
	 call this must call Delete on the object so that the reference
	 counting will work.   The single instance will be unreferenced when
	 the program exits
  */
  /*! Return the singleton instance with no reference counting. */
	//static vtkEpiphanVideoSource* GetInstance();

  /*! Supply a user defined output window. Call ->Delete() on the supplied instance after setting it. */
	//static void SetInstance(vtkEpiphanVideoSource *instance);

	//BTX
	/*! Use this as a way of memory management when the
	 program exits the SmartPointer will be deleted which
	 will delete the Instance singleton
  */
	//static vtkEpiphanVideoSourceCleanup Cleanup;
	//ETX

  /*! Hardware device SDK version. */
  virtual std::string GetSdkVersion();
  /*! Read configuration from xml data */	
  virtual PlusStatus ReadConfiguration(vtkXMLDataElement* config); 
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

	V2U_TIME v2u_computeTime();

protected:
  /*! Constructor */
	vtkEpiphanVideoSource();
  /*! Destructor */
	~vtkEpiphanVideoSource();

  /*! Device-specific connect */
	virtual PlusStatus InternalConnect();

  /*! Device-specific disconnect */
	virtual PlusStatus InternalDisconnect();

  /*! Device-specific recording start */
	virtual PlusStatus InternalStartRecording();

  /*! Device-specific recording stop */
//	virtual PlusStatus InternalStopRecording();

  /*! The internal function which actually does the grab.  */
	PlusStatus InternalGrab();

  /*! Adds a frame to the frame buffer. Called whenever the driver notified a new frame acquisition. */
	//PlusStatus AddFrameToBuffer(unsigned char * data, unsigned long size, unsigned long frameNumber);

  /*! Frame grabber device - DShowLib::Grabber type */
	void* FrameGrabber;
	
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

	// crop rectangle for the grabber
	V2URect * cropRect;

	 // serial number of the frame grabber
	 char serialNumber[15];

	// pointer to the grabber
	FrmGrabber* fg;

	// status of the frame grabber
	int status;

	int FrameSize[2];

private:

	//static vtkEpiphanVideoSource* Instance;
	//static bool vtkEpiphanVideoSourceNewFrameCallback(unsigned char * data, unsigned long size, unsigned long frameNumber);
	vtkEpiphanVideoSource(const vtkEpiphanVideoSource&);  // Not implemented.
	void operator=(const vtkEpiphanVideoSource&);  // Not implemented.
};

#endif





