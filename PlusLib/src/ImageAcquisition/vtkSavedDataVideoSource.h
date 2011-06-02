#ifndef __vtkSavedDataVideoSource_h
#define __vtkSavedDataVideoSource_h

#include "vtkVideoSource2.h"

class vtkVideoBuffer2; 

class VTK_EXPORT vtkSavedDataVideoSource;

class VTK_EXPORT vtkSavedDataVideoSourceCleanup2
{
public:
	vtkSavedDataVideoSourceCleanup2();
	~vtkSavedDataVideoSourceCleanup2();
};

class VTK_EXPORT vtkSavedDataVideoSource : public vtkVideoSource2
{
public:
	vtkTypeRevisionMacro(vtkSavedDataVideoSource,vtkVideoSource2);
	void PrintSelf(ostream& os, vtkIndent indent);   
	static vtkSavedDataVideoSource* New();
	static vtkSavedDataVideoSource* GetInstance();
	static void SetInstance(vtkSavedDataVideoSource *instance);

	//BTX
	// use this as a way of memory management when the
	// program exits the SmartPointer will be deleted which
	// will delete the Instance singleton
	static vtkSavedDataVideoSourceCleanup2 Cleanup;
	//ETX

	// Description:
	// Read/write main configuration from/to xml data
	virtual void ReadConfiguration(vtkXMLDataElement* config); 
	virtual void WriteConfiguration(vtkXMLDataElement* config);

	// Description:
	// Connect to device
	virtual int Connect();

	// Description:
	// Disconnect from device
	virtual void Disconnect();

	// Description:
	// Record incoming video at the specified FrameRate.  The recording
	// continues indefinitely until Stop() is called. 
	virtual void Record();

	// Description:
	// Play through the 'tape' sequentially at the specified frame rate.
	// If you have just finished Recoding, you should call Rewind() first.
	virtual void Play();

	// Description:
	// Stop recording or playing.
	virtual void Stop();

	// Description:
	// Grab a single video frame.
	void Grab();

	// Description:
	// Initialize the driver (this is called automatically when the
	// first grab is done).
	void Initialize();

	// Description:
	// The internal function which actually does the grab. 
	void InternalGrab();

	// Description:
	// Free the driver (this is called automatically inside the
	// destructor).
	void ReleaseSystemResources();

	// Description:
	// Set/get SequenceMetafile name with path with tracking buffer data 
	vtkSetStringMacro(SequenceMetafile);
	vtkGetStringMacro(SequenceMetafile);

	// Description:
	// Set/get tracking start timestamp 
	vtkSetMacro(StartTimestamp, double); 
	vtkGetMacro(StartTimestamp, double); 

	//! Description 
	// Flag to to enable saved dataset reply
	// If it's enabled, the video source will continuously play saved data
	vtkGetMacro(ReplayEnabled, bool);
	vtkSetMacro(ReplayEnabled, bool);
	vtkBooleanMacro(ReplayEnabled, bool);
	
protected:
	vtkSavedDataVideoSource();
	virtual ~vtkSavedDataVideoSource();

	// Description:
	// For internal use only
	void LocalInternalGrab(unsigned char * data, int type, int sz, bool cine, int frmnum);

	// byte alignment of each row in the framebuffer
	int FrameBufferRowAlignment;

	char* SequenceMetafile; 
	double StartTimestamp; 
	bool ReplayEnabled; 

	vtkVideoBuffer2* LocalVideoBuffer; 

private:
	static vtkSavedDataVideoSource* Instance;
	vtkSavedDataVideoSource(const vtkSavedDataVideoSource&);  // Not implemented.
	void operator=(const vtkSavedDataVideoSource&);  // Not implemented.
};

#endif

