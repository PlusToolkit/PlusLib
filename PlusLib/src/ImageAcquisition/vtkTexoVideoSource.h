#ifndef __vtkTexoVideoSource_h
#define __vtkTexoVideoSource_h

#include "vtkVideoSource2.h"

class vtkVideoBuffer; 

class VTK_EXPORT vtkTexoVideoSource;

class VTK_EXPORT vtkTexoVideoSourceCleanup2
{
public:
	vtkTexoVideoSourceCleanup2();
	~vtkTexoVideoSourceCleanup2();
};

class VTK_EXPORT vtkTexoVideoSource : public vtkVideoSource2
{
public:
	vtkTypeRevisionMacro(vtkTexoVideoSource,vtkVideoSource2);
	void PrintSelf(ostream& os, vtkIndent indent);   
	static vtkTexoVideoSource* New();
	static vtkTexoVideoSource* GetInstance();
	static void SetInstance(vtkTexoVideoSource *instance);

	//BTX
	// use this as a way of memory management when the
	// program exits the SmartPointer will be deleted which
	// will delete the Instance singleton
	static vtkTexoVideoSourceCleanup2 Cleanup;
	//ETX

	// Description:
	// Read/write main configuration from/to xml data
	virtual PlusStatus ReadConfiguration(vtkXMLDataElement* config); 
	virtual PlusStatus WriteConfiguration(vtkXMLDataElement* config);

	// Description:
	// Connect to device
	virtual PlusStatus Connect();

	// Description:
	// Disconnect from device
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
	// The internal function which actually does the grab. 
	PlusStatus InternalGrab();

	// Description:
	// Free the driver (this is called automatically inside the
	// destructor).
	void ReleaseSystemResources();

	// Description:
	// Set/get SequenceMetafile name with path with tracking buffer data 
	vtkSetStringMacro(SequenceMetafile);
	vtkGetStringMacro(SequenceMetafile);

  // Description: 
  // Set/get loop start time 
  // itemTimestamp = loopStartTime + (actualTimestamp - startTimestamp) % loopTime 
	vtkSetMacro(LoopStartTime, double); 
	vtkGetMacro(LoopStartTime, double); 

  // Description: 
  // Set/get loop time 
  // itemTimestamp = loopStartTime + (actualTimestamp - startTimestamp) % loopTime 
	vtkSetMacro(LoopTime, double); 
	vtkGetMacro(LoopTime, double); 

	//! Description 
	// Flag to to enable saved dataset reply
	// If it's enabled, the video source will continuously play saved data
	vtkGetMacro(ReplayEnabled, bool);
	vtkSetMacro(ReplayEnabled, bool);
	vtkBooleanMacro(ReplayEnabled, bool);

  //! Description 
  // Get local video buffer 
  vtkGetObjectMacro(LocalVideoBuffer, vtkVideoBuffer); 
	
protected:
	vtkTexoVideoSource();
	virtual ~vtkTexoVideoSource();

	// Description:
	// For internal use only
	PlusStatus LocalInternalGrab(unsigned char * data, int type, int sz, bool cine, int frmnum);

	// byte alignment of each row in the framebuffer
	int FrameBufferRowAlignment;

	char* SequenceMetafile; 
	bool ReplayEnabled; 

  double LoopStartTime; 
  double LoopTime; 

	vtkVideoBuffer* LocalVideoBuffer; 

private:
	static vtkTexoVideoSource* Instance;
	vtkTexoVideoSource(const vtkTexoVideoSource&);  // Not implemented.
	void operator=(const vtkTexoVideoSource&);  // Not implemented.
};

#endif

