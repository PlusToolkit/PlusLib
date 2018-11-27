/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkPlusFakeTracker_h
#define __vtkPlusFakeTracker_h

#include "vtkPlusDataCollectionExport.h"

#include "vtkPlusDevice.h"
#include "vtkIGSIOTransformRepository.h"
#include "vtkIGSIOTrackedFrameList.h"
#include "vtkPoints.h"

/*! Fake tracker modes */
enum FakeTrackerMode
{
  FakeTrackerMode_Undefined = 0,
  FakeTrackerMode_Default,
  FakeTrackerMode_SmoothMove,
  FakeTrackerMode_SmoothTranslation,
  FakeTrackerMode_PivotCalibration,
  FakeTrackerMode_RecordPhantomLandmarks,
  FakeTrackerMode_ToolState
};

class vtkTransform;

/*!
\class vtkPlusFakeTracker 
\brief Represents a fake tracking system as a simulator 

This class represents a fake tracking system with tools that have
predetermined behavior. This allows someone who doesn't have access to
a tracking system to test code that relies on having one active.

\ingroup PlusLibDataCollection
*/
class vtkPlusDataCollectionExport vtkPlusFakeTracker : public vtkPlusDevice
{
public:
  static vtkPlusFakeTracker *New();
  vtkTypeMacro(vtkPlusFakeTracker,vtkPlusDevice);

  /*! Connect to device */
  PlusStatus InternalConnect();

  /*! Disconnect from device */
  PlusStatus InternalDisconnect();

  /*! Checks availability of tracker (this always returns 1) */
  PlusStatus Probe();

  /*! Read configuration from xml data */
  virtual PlusStatus ReadConfiguration(vtkXMLDataElement* config); 

  /*! Set fake mode (tool info for fake tools) */
  void SetMode(FakeTrackerMode);

  virtual bool IsTracker() const { return true; }

  /*! Set counter value used for translating landmark points */
  vtkSetMacro(Counter, int);

  /*! Set transform repository */
  vtkSetObjectMacro(TransformRepository, vtkIGSIOTransformRepository);
  /*! Get transform repository */
  vtkGetObjectMacro(TransformRepository, vtkIGSIOTransformRepository);

  /*! Get the phantom landmark points positions */
  vtkGetObjectMacro(PhantomLandmarks, vtkPoints);

protected:
  /*! Set the phantom landmark points positions */
  vtkSetObjectMacro(PhantomLandmarks, vtkPoints);

protected:
  /*! Start the tracking system. */
  PlusStatus InternalStartRecording();

  /*! Stop the tracking system. */
  PlusStatus InternalStopRecording();

  /*! Get an update from the tracking system and push the new transforms to the tools. */
  PlusStatus InternalUpdate();

  vtkPlusFakeTracker();
  ~vtkPlusFakeTracker();

protected:
  /*! Internal frame number counting */
  unsigned long Frame;

  /*! Internal transform used for simulating tool movements */
  vtkTransform *InternalTransform;

  /*! Stores the selected fake tracker mode */
  FakeTrackerMode Mode;

  /*! Transform repository object (some modes need some more transforms to simulate properly) */
  vtkIGSIOTransformRepository* TransformRepository;

  /*! Constant seed used for random generator */
  int RandomSeed;

  /*! Stores counter value used for translating landmark points */
  int Counter;

  /*!
    Point array holding the defined phantom landmarks from the configuration file.
    Need for setting up RecordPhantomLandmarks mode
  */
  vtkPoints* PhantomLandmarks;
};


#endif
