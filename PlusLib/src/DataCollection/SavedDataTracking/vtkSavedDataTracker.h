/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkSavedDataTracker_h
#define __vtkSavedDataTracker_h

#include "vtkPlusDevice.h"

class vtkPlusStreamBuffer; 

/*!
\class vtkSavedDataTracker 
\brief Simulator interface for exported buffers 

Simulator reads exported tracked buffer from sequence metafile and play back it 

\ingroup PlusLibTracking
*/
class VTK_EXPORT vtkSavedDataTracker : public vtkPlusDevice
{
public:

  static vtkSavedDataTracker *New();
  vtkTypeMacro(vtkSavedDataTracker,vtkPlusDevice);
  void PrintSelf(ostream& os, vtkIndent indent);

  /*! Probe to see if the tracking system is present on the specified serial port. */
  PlusStatus Probe();

  /*! Get an update from the tracking system and push the new transforms to the tools. */
  PlusStatus InternalUpdate();

  /*! Read tracker configuration from xml data */
  virtual PlusStatus ReadConfiguration(vtkXMLDataElement* config); 

  /*! Write tracker configuration to xml data */
  virtual PlusStatus WriteConfiguration(vtkXMLDataElement* config); 

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

  /*! Flag to to enable saved dataset repeating. If it's enabled, the video source will continuously play saved data */
  vtkGetMacro(RepeatEnabled, bool);

  /*! Flag to to enable saved dataset repeating. If it's enabled, the video source will continuously play saved data */
  vtkSetMacro(RepeatEnabled, bool);

  /*! Flag to to enable saved dataset repeating. If it's enabled, the video source will continuously play saved data */
  vtkBooleanMacro(RepeatEnabled, bool);

  /*! Get local tracker buffer */
  vtkPlusStreamBuffer* GetLocalTrackerBuffer(); 

protected:
  vtkSavedDataTracker();
  ~vtkSavedDataTracker();

  void DeleteLocalTrackerBuffers(); 

  /*! Connect to device */
  virtual PlusStatus InternalConnect();

  /*! Disconnect from device */
  virtual PlusStatus InternalDisconnect();

  /*! Start the tracking system. */
  PlusStatus InternalStartRecording();

  /*! Stop the tracking system */
  PlusStatus InternalStopRecording();

  /*! Sequence metafile name */
  char* SequenceMetafile; 

  /*! Flag to enable saved dataset repeat */
  bool RepeatEnabled; 

  /*! Local buffer for each tracker tool, used for storing data read from sequence metafile */
  std::map<std::string, vtkPlusStreamBuffer*> LocalTrackerBuffers; 

  /*! Flag used for storing initialization state */
  bool Initialized;

  /*! Initila loop start timestamp */
  double LoopStartTime; 

  /*! Length of the loop time */
  double LoopTime; 

private:
  vtkSavedDataTracker(const vtkSavedDataTracker&);
  void operator=(const vtkSavedDataTracker&);  
};

#endif
