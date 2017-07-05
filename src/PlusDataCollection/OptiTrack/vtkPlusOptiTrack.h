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

  /*! Get the transformation for a connected tool given the tool name = PortName (PLUS) */
  void GetTransformMatrix(std::string toolName, vtkMatrix4x4* transformMatrix);

protected:
  vtkPlusOptiTrack();
  ~vtkPlusOptiTrack();
  
private:  // Functions.

  vtkPlusOptiTrack( const vtkPlusOptiTrack& );
  void operator=( const vtkPlusOptiTrack& ); 

private:  // Variables.  
  /*! Index of the last frame number. This is used for providing a frame number when the tracker doesn't return any transform */
  unsigned long LastFrameNumber;

};

#endif