/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkPlusOptiTrack_h
#define __vtkPlusOptiTrack_h

#include "vtkPlusDataCollectionExport.h"
#include "vtkPlusDevice.h"

/*!
\class vtkPlusOptiTrack
\brief Interface to the OptiTrack trackers
This class talks with a OptiTrack Tracker over the NatNet SDK.
Requires PLUS_USE_OPTITRACK option in CMake.
\ingroup PlusLibDataCollection
*/
class vtkPlusDataCollectionExport vtkPlusOptiTrack : public vtkPlusDevice
{
public:
  static vtkPlusOptiTrack *New();
  vtkTypeMacro( vtkPlusOptiTrack,vtkPlusDevice );
  void PrintSelf( ostream& os, vtkIndent indent );

  /* Device is a hardware tracker. */
  virtual bool IsTracker() const { return true; }
  virtual bool IsVirtual() const { return false; }

  /*! Read configuration from xml data */
  virtual PlusStatus ReadConfiguration(vtkXMLDataElement* config); 

  /*! Write configuration to xml data */
  virtual PlusStatus WriteConfiguration(vtkXMLDataElement* config);

  /*! Connect to device */
  PlusStatus InternalConnect();

  /*! Disconnect from device */
  virtual PlusStatus InternalDisconnect();

  /*! Probe to see if the tracking system is present. */
  PlusStatus Probe();

  /*!  */
  PlusStatus InternalUpdate();

protected:
  vtkPlusOptiTrack();
  ~vtkPlusOptiTrack();

private: // Functions
  vtkPlusOptiTrack(const vtkPlusOptiTrack&);
  void operator=(const vtkPlusOptiTrack&); 

  /*! Start the tracking system. */
  PlusStatus InternalStartRecording();

  /*! Stop the tracking system and bring it back to its initial state. */
  PlusStatus InternalStopRecording();

  class vtkInternal;
  vtkInternal* Internal;
};

#endif
