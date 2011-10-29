/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkSavedDataVideoSource_h
#define __vtkSavedDataVideoSource_h

#include "vtkPlusVideoSource.h"

class vtkVideoBuffer; 

class VTK_EXPORT vtkSavedDataVideoSource;

class VTK_EXPORT vtkSavedDataVideoSourceCleanup2
{
public:
	vtkSavedDataVideoSourceCleanup2();
	~vtkSavedDataVideoSourceCleanup2();
};

class VTK_EXPORT vtkSavedDataVideoSource : public vtkPlusVideoSource
{
public:
	vtkTypeRevisionMacro(vtkSavedDataVideoSource,vtkPlusVideoSource);
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
	virtual PlusStatus ReadConfiguration(vtkXMLDataElement* config); 
	virtual PlusStatus WriteConfiguration(vtkXMLDataElement* config);

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
	vtkSavedDataVideoSource();
	virtual ~vtkSavedDataVideoSource();

	// Description:
	// Connect to device
	virtual PlusStatus InternalConnect();

	// Description:
	// Disconnect from device
	virtual PlusStatus InternalDisconnect();

	// Description:
	// The internal function which actually does the grab. 
	PlusStatus InternalGrab();

	// Description:
	// For internal use only
	PlusStatus AddFrameToBuffer(unsigned char * data, int type, int sz, bool cine, int frmnum);

	// byte alignment of each row in the framebuffer
	int FrameBufferRowAlignment;

	char* SequenceMetafile; 
	bool ReplayEnabled; 

  double LoopStartTime; 
  double LoopTime; 

	vtkVideoBuffer* LocalVideoBuffer; 

private:
	static vtkSavedDataVideoSource* Instance;
	vtkSavedDataVideoSource(const vtkSavedDataVideoSource&);  // Not implemented.
	void operator=(const vtkSavedDataVideoSource&);  // Not implemented.
};

#endif

