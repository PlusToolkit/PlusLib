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

/*!
  \class vtkSavedDataVideoSource 
  \brief Class for providing VTK video input interface from sequence metafile
  \ingroup PlusLibImageAcquisition
*/
class VTK_EXPORT vtkSavedDataVideoSource : public vtkPlusVideoSource
{
public:
	vtkTypeRevisionMacro(vtkSavedDataVideoSource,vtkPlusVideoSource);
	void PrintSelf(ostream& os, vtkIndent indent);   
	static vtkSavedDataVideoSource* New();

  /*! Read configuration from xml data */
	virtual PlusStatus ReadConfiguration(vtkXMLDataElement* config); 
  /*! Write configuration to xml data */
	virtual PlusStatus WriteConfiguration(vtkXMLDataElement* config);

  /*! Set SequenceMetafile name with path with tracking buffer data  */
	vtkSetStringMacro(SequenceMetafile);
  /*! Get SequenceMetafile name with path with tracking buffer data  */
	vtkGetStringMacro(SequenceMetafile);

  /*! Set loop start time /sa LoopStartTime */
	vtkSetMacro(LoopStartTime, double); 
  /*! Get loop start time /sa LoopStartTime */
	vtkGetMacro(LoopStartTime, double); 

  /*! Set loop time /sa LoopTime */
	vtkSetMacro(LoopTime, double); 
  /*! Get loop time /sa LoopTime */
	vtkGetMacro(LoopTime, double); 

	/*! Set flag to to enable saved dataset reply /sa ReplayEnabled */
	vtkGetMacro(ReplayEnabled, bool);
	/*! Get flag to to enable saved dataset reply /sa ReplayEnabled */
	vtkSetMacro(ReplayEnabled, bool);
	vtkBooleanMacro(ReplayEnabled, bool);

  /*! Get local video buffer */
  vtkGetObjectMacro(LocalVideoBuffer, vtkVideoBuffer); 
	
protected:
	/*! Constructor */
	vtkSavedDataVideoSource();
	/*! Destructor */
	virtual ~vtkSavedDataVideoSource();

  /*! Connect to device */
	virtual PlusStatus InternalConnect();

  /*! Disconnect from device */
	virtual PlusStatus InternalDisconnect();

  /*! The internal function which actually does the grab.  */
	PlusStatus InternalGrab();

protected:
	/*! Byte alignment of each row in the framebuffer */
	int FrameBufferRowAlignment;

  /*! Name of input sequence metafile */
	char* SequenceMetafile;

	/*! Flag to to enable saved dataset reply. If it's enabled, the video source will continuously play saved data */
	bool ReplayEnabled; 

  /*! Loop start time
      ItemTimestamp = loopStartTime + (actualTimestamp - startTimestamp) % loopTime 
  */
  double LoopStartTime; 

  /*! Loop time
      ItemTimestamp = loopStartTime + (actualTimestamp - startTimestamp) % loopTime 
  */
  double LoopTime; 

  /*! Local viceo buffer */
	vtkVideoBuffer* LocalVideoBuffer; 

private:
	static vtkSavedDataVideoSource* Instance;
	vtkSavedDataVideoSource(const vtkSavedDataVideoSource&);  // Not implemented.
	void operator=(const vtkSavedDataVideoSource&);  // Not implemented.
};

#endif

