/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkICCapturingSource_h
#define __vtkICCapturingSource_h

#include "vtkPlusVideoSource.h"
class ICCapturingListener; 

class VTK_EXPORT vtkICCapturingSource;

/*!
  \class vtkICCapturingSourceCleanup 
  \brief Class that cleans up (deletes singleton instance of) vtkICCapturingSource when destroyed
  \ingroup PlusLibImageAcquisition
*/
class VTK_EXPORT vtkICCapturingSourceCleanup
{
public:
	vtkICCapturingSourceCleanup();
	~vtkICCapturingSourceCleanup();
};

/*!
  \class vtkICCapturingSource 
  \brief Class for providing video input interfaces between VTK and ICCapturing frame grabber device
  \ingroup PlusLibImageAcquisition
*/
class VTK_EXPORT vtkICCapturingSource : public vtkPlusVideoSource
{
public:
	vtkTypeRevisionMacro(vtkICCapturingSource,vtkPlusVideoSource);
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
  virtual std::string GetSDKVersion();
  /*! Read configuration from xml data */	virtual PlusStatus ReadConfiguration(vtkXMLDataElement* config); 
  /*! Write configuration to xml data */
	virtual PlusStatus WriteConfiguration(vtkXMLDataElement* config);

  /*! Set the IC capturing device license key /sa LicenceKey */
	vtkSetStringMacro(LicenceKey); 
  /*! Get the IC capturing device license key /sa LicenceKey */
	vtkGetStringMacro(LicenceKey); 

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

  /*! Frame grabber device - DShowLib::Grabber type */
	void* FrameGrabber;
	ICCapturingListener* FrameGrabberListener; 

  /*! /brief License key
	 If you have a trial version, the license key is 0 without quotation marks
	 Example: if(!DShowLib::InitLibrary( 0 ))
	 If you have a licensed version (standard or professional), the license
	 key is a string in quotation marks. The license key has to be identical to 
	 the license key entered during the IC Imaging Control setup.
	 Example: if( !DShowLib::InitLibrary( "XXXXXXX" ))
  */
	char* LicenceKey; 

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

private:

	static vtkICCapturingSource* Instance;
	static bool vtkICCapturingSourceNewFrameCallback(unsigned char * data, unsigned long size, unsigned long frameNumber);
	vtkICCapturingSource(const vtkICCapturingSource&);  // Not implemented.
	void operator=(const vtkICCapturingSource&);  // Not implemented.
};

#endif





