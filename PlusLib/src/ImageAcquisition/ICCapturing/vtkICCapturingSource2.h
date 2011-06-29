#ifndef __vtkICCapturingSource2_h
#define __vtkICCapturingSource2_h

#include "vtkVideoSource2.h"
class ICCapturingListener; 

class VTK_EXPORT vtkICCapturingSource2;

class VTK_EXPORT vtkICCapturingSourceCleanup2
{
public:
	vtkICCapturingSourceCleanup2();
	~vtkICCapturingSourceCleanup2();
};


class VTK_EXPORT vtkICCapturingSource2 : public vtkVideoSource2
{
public:
	vtkTypeRevisionMacro(vtkICCapturingSource2,vtkVideoSource2);
	void PrintSelf(ostream& os, vtkIndent indent);   
	// Description:
	// This is a singleton pattern New.  There will only be ONE
	// reference to a vtkOutputWindow object per process.  Clients that
	// call this must call Delete on the object so that the reference
	// counting will work.   The single instance will be unreferenced when
	// the program exits.
	static vtkICCapturingSource2* New();
	// Description:
	// Return the singleton instance with no reference counting.
	static vtkICCapturingSource2* GetInstance();

	// Description:
	// Supply a user defined output window. Call ->Delete() on the supplied
	// instance after setting it.
	static void SetInstance(vtkICCapturingSource2 *instance);
	//BTX
	// use this as a way of memory management when the
	// program exits the SmartPointer will be deleted which
	// will delete the Instance singleton
	static vtkICCapturingSourceCleanup2 Cleanup;
	//ETX

	// Description:
	// Read/write main configuration from/to xml data
	virtual PlusStatus ReadConfiguration(vtkXMLDataElement* config); 
	virtual PlusStatus WriteConfiguration(vtkXMLDataElement* config);

	// Description:
	// Connect to device
	// Should be overridden to connect to the hardware 
	virtual PlusStatus Connect();

	// Description:
	// Disconnect from device
	// Should be overridden to disconnect from the hardware 
	virtual PlusStatus Disconnect();

	// Description:
	// Record incoming video at the specified FrameRate.  The recording
	// continues indefinitely until StopRecording() is called. 
	virtual PlusStatus StartRecording();

	// Description:
	// Stop recording or playing.
	virtual PlusStatus StopRecording();

	// Description:
	// Grab a single video frame.
	PlusStatus Grab();

	// Description:
	// Initialize the driver (this is called automatically when the
	// first grab is done).
	PlusStatus Initialize();

	// Description:
	// Free the driver (this is called automatically inside the
	// destructor).
	void ReleaseSystemResources();

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
	vtkICCapturingSource2();
	~vtkICCapturingSource2();

	//int RequestInformation(vtkInformation *, vtkInformationVector **, vtkInformationVector *);

	// Description:
	// For internal use only
	PlusStatus LocalInternalGrab(unsigned char * data, unsigned long size, unsigned long frameNumber);

	// byte alignment of each row in the framebuffer
	int FrameBufferRowAlignment;

	void* FrameGrabber;
	ICCapturingListener* FrameGrabberListener; 

	char* LicenceKey; 
	char* DeviceName; 
	char* VideoNorm; 
	char* VideoFormat; 
	char* InputChannel; 

	int ICBufferSize; 

private:

	static vtkICCapturingSource2* Instance;
	static bool vtkICCapturingSource2NewFrameCallback(unsigned char * data, unsigned long size, unsigned long frameNumber);
	vtkICCapturingSource2(const vtkICCapturingSource2&);  // Not implemented.
	void operator=(const vtkICCapturingSource2&);  // Not implemented.
};

#endif





