/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkSavedDataTracker_h
#define __vtkSavedDataTracker_h

#include "vtkTracker.h"

class vtkTrackerBuffer; 

/*!
\class vtkSavedDataTracker 
\brief Simulator interface for exported buffers 

Simulator reads exported tracked buffer from sequence metafile and play back it 

\ingroup PlusLibTracking
*/
class VTK_EXPORT vtkSavedDataTracker : public vtkTracker
{
public:

	static vtkSavedDataTracker *New();
	vtkTypeMacro(vtkSavedDataTracker,vtkTracker);
	void PrintSelf(ostream& os, vtkIndent indent);

	/*! Connect to device */
	PlusStatus Connect();

	/*! Disconnect from device */
	virtual PlusStatus Disconnect();

	/*! Probe to see if the tracking system is present on the specified serial port. */
	PlusStatus Probe();

	/*! Get an update from the tracking system and push the new transforms to the tools. */
	PlusStatus InternalUpdate();

	/*! Read tracker configuration from xml data */
	PlusStatus ReadConfiguration(vtkXMLDataElement* config); 

  /*! Write tracker configuration to xml data */
	PlusStatus WriteConfiguration(vtkXMLDataElement* config); 

	/*! Set SequenceMetafile name with path with tracking buffer data */
	vtkSetStringMacro(SequenceMetafile);

  /*! Get SequenceMetafile name with path with tracking buffer data */
	vtkGetStringMacro(SequenceMetafile);

  /*! Set loop start time: itemTimestamp = loopStartTime + (actualTimestamp - startTimestamp) % loopTime */
	vtkSetMacro(LoopStartTime, double); 
	
  /*! Get loop start time: itemTimestamp = loopStartTime + (actualTimestamp - startTimestamp) % loopTime */
  vtkGetMacro(LoopStartTime, double); 

  /*! Set loop time: itemTimestamp = loopStartTime + (actualTimestamp - startTimestamp) % loopTime */
	vtkSetMacro(LoopTime, double); 

  /*! Get loop time: itemTimestamp = loopStartTime + (actualTimestamp - startTimestamp) % loopTime */
	vtkGetMacro(LoopTime, double); 

	/*! Flag to to enable saved dataset reply. If it's enabled, the video source will continuously play saved data */
	vtkGetMacro(ReplayEnabled, bool);
  
  /*! Flag to to enable saved dataset reply. If it's enabled, the video source will continuously play saved data */
	vtkSetMacro(ReplayEnabled, bool);

  /*! Flag to to enable saved dataset reply. If it's enabled, the video source will continuously play saved data */
	vtkBooleanMacro(ReplayEnabled, bool);

  /*! Get local tracker buffer */
  vtkTrackerBuffer* GetLocalTrackerBuffer(); 

protected:
	vtkSavedDataTracker();
	~vtkSavedDataTracker();

  void DeleteLocalTrackerBuffers(); 

	/*! Start the tracking system. */
	PlusStatus InternalStartTracking();

	/*! Stop the tracking system */
	PlusStatus InternalStopTracking();

  /*! Sequence metafile name */
	char* SequenceMetafile; 

  /*! Flag to enable saved dataset replay */
	bool ReplayEnabled; 

  /*! Local buffer for each tracker tool, used for storing data read from sequence metafile */
  std::map<std::string, vtkTrackerBuffer*> LocalTrackerBuffers; 
	
  /*! Flag used for storing initialization state */
	bool Initialized;

  /*! Frame number counter */
  long FrameNumber; 

  /*! Initila loop start timestamp */
  double LoopStartTime; 

  /*! Length of the loop time */
  double LoopTime; 
		
private:
	vtkSavedDataTracker(const vtkSavedDataTracker&);
	void operator=(const vtkSavedDataTracker&);  
};

#endif
