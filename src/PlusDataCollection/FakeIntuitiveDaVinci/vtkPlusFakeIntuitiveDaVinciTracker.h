/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkPlusFakeIntuitiveDaVinciTracker_h
#define __vtkPlusFakeIntuitiveDaVinciTracker_h

// Local includes
#include "vtkPlusDataCollectionExport.h"
#include "vtkPlusDevice.h"

// VTK includes
#include <vtkObjectFactory.h>

// Intuitive includes
#include "FakeIntuitiveDaVinci.h"

class vtkMatrix4x4;
class FakeIntuitiveDaVinci;
class vtkPlusFakeIntuitiveDaVinciTracker;

/* This class talks with the da Vinci Surgical System. */
class vtkPlusDataCollectionExport vtkPlusFakeIntuitiveDaVinciTracker : public vtkPlusDevice
{
public:
  static vtkPlusFakeIntuitiveDaVinciTracker* New();
  vtkTypeMacro(vtkPlusFakeIntuitiveDaVinciTracker, vtkPlusDevice);
  virtual void PrintSelf(ostream& os, vtkIndent indent) VTK_OVERRIDE;

  virtual bool IsTracker() const { return true; }

  /*! Probe to see if the tracking system is present. */
  virtual PlusStatus Probe();

  /*! Read da Vinci configuration and update the tracker settings accordingly */
  virtual PlusStatus ReadConfiguration(vtkXMLDataElement* rootConfigElement);

  /*! Write current da Vinci configuration settings to XML */
  virtual PlusStatus WriteConfiguration(vtkXMLDataElement* rootConfigElement);

  FakeIntuitiveDaVinci* GetDaVinci() const;

protected:
  vtkPlusFakeIntuitiveDaVinciTracker();
  ~vtkPlusFakeIntuitiveDaVinciTracker();

  /*! Connect to the tracker hardware */
  virtual PlusStatus InternalConnect();

  /*! Disconnect from the tracker hardware */
  virtual PlusStatus InternalDisconnect();

  /*!  Start the tracking system. */
  virtual PlusStatus InternalStartRecording();

  /*! Stop the tracking system and bring it back to its initial state. */
  virtual PlusStatus InternalStopRecording();

  /*! Update method */
  PlusStatus InternalUpdate();

protected:
  /*! Pointer to the MicronTrackerInterface class instance */
  FakeIntuitiveDaVinci*   DaVinci;

  /*! Index of the last frame number. This is used for providing a frame number when the tracker doesn't return any transform */
  unsigned long       LastFrameNumber;
  unsigned long       FrameNumber;

private:
  vtkPlusFakeIntuitiveDaVinciTracker(const vtkPlusFakeIntuitiveDaVinciTracker&);
  void operator=(const vtkPlusFakeIntuitiveDaVinciTracker&);

  void setVtkMatrixFromISITransform(vtkMatrix4x4& vtkMatrix, ISI_TRANSFORM* isiMatrix);

  vtkPlusDataSource* PSM1Tip;
  vtkPlusDataSource* PSM2Tip;
};

#endif
