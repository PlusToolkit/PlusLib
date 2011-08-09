#ifndef __vtkURFSavedDataVideoSource_h
#define __vtkURFSavedDataVideoSource_h

#include "vtkVideoSource2.h"

class vtkVideoBuffer; 

class VTK_EXPORT vtkURFSavedDataVideoSource;

class VTK_EXPORT vtkURFSavedDataVideoSourceCleanup2
{
public:
	vtkURFSavedDataVideoSourceCleanup2();
	~vtkURFSavedDataVideoSourceCleanup2();
};

class VTK_EXPORT vtkURFSavedDataVideoSource : public vtkVideoSource2
{
public:
	vtkTypeRevisionMacro(vtkURFSavedDataVideoSource,vtkVideoSource2);
	void PrintSelf(ostream& os, vtkIndent indent);   
	static vtkURFSavedDataVideoSource* New();
	static vtkURFSavedDataVideoSource* GetInstance();
	static void SetInstance(vtkURFSavedDataVideoSource *instance);

	//BTX
	// use this as a way of memory management when the
	// program exits the SmartPointer will be deleted which
	// will delete the Instance singleton
	static vtkURFSavedDataVideoSourceCleanup2 Cleanup;
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
	vtkURFSavedDataVideoSource();
	virtual ~vtkURFSavedDataVideoSource();

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
	static vtkURFSavedDataVideoSource* Instance;
	vtkURFSavedDataVideoSource(const vtkURFSavedDataVideoSource&);  // Not implemented.
	void operator=(const vtkURFSavedDataVideoSource&);  // Not implemented.
};

#endif

