/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkOptiTrackTracker_h
#define __vtkOptiTrackTracker_h

#include "vtkDataCollectionExport.h"

#include "vtkPlusDevice.h"

/*!
\class vtkOptimetConoProbeTracker 
\brief Interface for the OptiTrack Tracker

This class talks with a OptiTrack Tracker over the OptiTrack Tracking Tools SDK.

Requires PLUS_USE_OPTITRACK option in CMake.

\ingroup PlusLibDataCollection
*/
class vtkDataCollectionExport vtkOptiTrackTracker : public vtkPlusDevice
{
public:

  static vtkOptiTrackTracker *New();
  vtkTypeMacro( vtkOptiTrackTracker,vtkPlusDevice );
  void PrintSelf( ostream& os, vtkIndent indent );

  virtual bool IsTracker() const { return true; }

  /*! Connect to device */
  PlusStatus InternalConnect();

  /*! Disconnect from device */
  virtual PlusStatus InternalDisconnect();

  /*!
  Get an update from the tracking system and push the new transforms
  to the tools.  This should only be used within vtkTracker.cxx.
  This method is called by the tracker thread.
  */
  PlusStatus InternalUpdate();
  
  /*! Read configuration from xml data */
  virtual PlusStatus ReadConfiguration(vtkXMLDataElement* config); 

  /*! Write configuration to xml data */
  virtual PlusStatus WriteConfiguration(vtkXMLDataElement* config);

protected:

  vtkOptiTrackTracker();
  ~vtkOptiTrackTracker();
  
private:  // Functions.

  vtkOptiTrackTracker( const vtkOptiTrackTracker& );
  void operator=( const vtkOptiTrackTracker& ); 

private:  // Variables.  

};

#endif
