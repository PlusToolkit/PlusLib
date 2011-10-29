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

class VTK_EXPORT vtkICCapturingSourceCleanup
{
public:
	vtkICCapturingSourceCleanup();
	~vtkICCapturingSourceCleanup();
};


class VTK_EXPORT vtkICCapturingSource : public vtkPlusVideoSource
{
public:
	vtkTypeRevisionMacro(vtkICCapturingSource,vtkPlusVideoSource);
	void PrintSelf(ostream& os, vtkIndent indent);   
	// Description:
	// This is a singleton pattern New.  There will only be ONE
	// reference to a vtkOutputWindow object per process.  Clients that
	// call this must call Delete on the object so that the reference
	// counting will work.   The single instance will be unreferenced when
	// the program exits.
	static vtkICCapturingSource* New();
	// Description:
	// Return the singleton instance with no reference counting.
	static vtkICCapturingSource* GetInstance();

	// Description:
	// Supply a user defined output window. Call ->Delete() on the supplied
	// instance after setting it.
	static void SetInstance(vtkICCapturingSource *instance);
	//BTX
	// use this as a way of memory management when the
	// program exits the SmartPointer will be deleted which
	// will delete the Instance singleton
	static vtkICCapturingSourceCleanup Cleanup;
	//ETX

	// Description:
	// Read/write main configuration from/to xml data
	virtual PlusStatus ReadConfiguration(vtkXMLDataElement* config); 
	virtual PlusStatus WriteConfiguration(vtkXMLDataElement* config);

	// Description:
	// Set/Get the IC capturing device license key
	// If you have a trial version, the license key is 0 without quotation marks
	// Example: if(!DShowLib::InitLibrary( 0 ))
	// If you have a licensed version (standard or professional), the license
	// key is a string in quotation marks. The license key has to be identical to 
	// the license key entered during the IC Imaging Control setup.
	// Example: if( !DShowLib::InitLibrary( "XXXXXXX" ))
	vtkSetStringMacro(LicenceKey); 
	vtkGetStringMacro(LicenceKey); 

	// Description:
	// Set/Get the IC capturing device name (e.g. "DFG/USB2-lt")
	vtkSetStringMacro(DeviceName); 
	vtkGetStringMacro(DeviceName); 

	// Description:
	// Set/Get the IC capturing device video norm (e.g. "PAL_B", "NTSC_M")
	vtkSetStringMacro(VideoNorm); 
	vtkGetStringMacro(VideoNorm); 

	// Description:
	// Set/Get the IC capturing device video format (e.g. "Y800 (640x480)" )
	vtkSetStringMacro(VideoFormat); 
	vtkGetStringMacro(VideoFormat); 

	// Description:
	// Set/Get the IC capturing device input channel (e.g. "01 Video: SVideo" )
	vtkSetStringMacro(InputChannel); 
	vtkGetStringMacro(InputChannel); 

	// Description:
	// Set/Get the IC capturing device buffer size ( Default: 50 frame )
	vtkSetMacro(ICBufferSize, int); 
	vtkGetMacro(ICBufferSize, int); 

protected:
	vtkICCapturingSource();
	~vtkICCapturingSource();

	// Description:
	// Device-specific connect
	virtual PlusStatus InternalConnect();

	// Description:
  // Device-specific disconnect
	virtual PlusStatus InternalDisconnect();

	// Description:
	// Device-specific recording start
	virtual PlusStatus InternalStartRecording();

	// Description:
	// Device-specific recording stop
	virtual PlusStatus InternalStopRecording();

	// Description:
	// Adds a frame to the frame buffer. Called whenever the driver notified a new frame acquisition.
	PlusStatus AddFrameToBuffer(unsigned char * data, unsigned long size, unsigned long frameNumber);

	void* FrameGrabber;
	ICCapturingListener* FrameGrabberListener; 

	char* LicenceKey; 
	char* DeviceName; 
	char* VideoNorm; 
	char* VideoFormat; 
	char* InputChannel; 

	int ICBufferSize; 

private:

	static vtkICCapturingSource* Instance;
	static bool vtkICCapturingSourceNewFrameCallback(unsigned char * data, unsigned long size, unsigned long frameNumber);
	vtkICCapturingSource(const vtkICCapturingSource&);  // Not implemented.
	void operator=(const vtkICCapturingSource&);  // Not implemented.
};

#endif





