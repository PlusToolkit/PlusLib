/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkFakeTracker_h
#define __vtkFakeTracker_h

#include "vtkTracker.h"

/*! Fake tracker modes */
enum FakeTrackerMode
{
  FakeTrackerMode_Undefined = 0,
  FakeTrackerMode_Default,
  FakeTrackerMode_SmoothMove,
  FakeTrackerMode_PivotCalibration,
  FakeTrackerMode_RecordPhantomLandmarks,
  FakeTrackerMode_ToolState
};

class vtkTransform;

/*!
\class vtkFakeTracker 
\brief Represents a fake tracking system as a simulator 

This class represents a fake tracking system with tools that have
predetermined behaviour. This allows someonew who doesn't have access to
a tracking system to test code that relies on having one active.

\ingroup PlusLibTracking
*/
class VTK_EXPORT vtkFakeTracker : public vtkTracker
{
public:
  static vtkFakeTracker *New();
  vtkTypeMacro(vtkFakeTracker,vtkTracker);

  /*! Connect to device */
  PlusStatus Connect();

  /*! Disconnect from device */
  PlusStatus Disconnect();

  /*! Checks availibility of tracker (this always returns 1) */
  PlusStatus Probe();

  /*! Read configuration from xml data */
  PlusStatus ReadConfiguration(vtkXMLDataElement* config); 

  /*! Set fake mode (tool info for fake tools) */
  void SetMode(FakeTrackerMode);

  /*! Set counter value used for translating landmark points */
  vtkSetMacro(Counter, int);

protected:
  /*! Start the tracking system. */
  PlusStatus InternalStartTracking();

  /*! Stop the tracking system. */
  PlusStatus InternalStopTracking();

  /*! Get an update from the tracking system and push the new transforms to the tools. */
  PlusStatus InternalUpdate();

  vtkFakeTracker();
  ~vtkFakeTracker();

protected:
  /*! Internal frame number cunting */
  int Frame;

  /*! Internal transform used for simulating tool movements */
  vtkTransform *InternalTransform;

  /*! Stores the selected fake tracker mode */
  FakeTrackerMode Mode;

  /*! Constant seed used for random generator */
  int RandomSeed;

  /*! Stores counter value used for translating landmark points */
  int Counter;
};


#endif
