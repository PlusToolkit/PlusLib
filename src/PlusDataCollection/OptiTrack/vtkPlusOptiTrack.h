/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkPlusOptiTrack_h
#define __vtkPlusOptiTrack_h

#include "vtkPlusDataCollectionExport.h"
#include "vtkPlusDevice.h"
#include "NatNetTypes.h"

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
  Should not be called, update is done using InternalCallback
  */
  PlusStatus InternalUpdate();

  /*! 
  Receive updated tracking information from the server and push the new transforms to the tools
  */
  PlusStatus InternalCallback(sFrameOfMocapData* data);
  
  /*! Read configuration from xml data */
  virtual PlusStatus ReadConfiguration(vtkXMLDataElement* config); 

  /*! Write configuration to xml data */
  virtual PlusStatus WriteConfiguration(vtkXMLDataElement* config);

protected:
  vtkPlusOptiTrack();
  ~vtkPlusOptiTrack();
  
private:  // Functions.
  /*! Index of the last frame number. This is used for providing a frame number when the tracker doesn't return any transform */
  void MatchTrackedTools();

  vtkPlusOptiTrack( const vtkPlusOptiTrack& );
  void operator=( const vtkPlusOptiTrack& ); 

private:  // Variables.  
  /*! Index of the last frame number. This is used for providing a frame number when the tracker doesn't return any transform */
  unsigned long LastFrameNumber;
  /*! IP of the computer running the client. Defaults to "127.0.0.1" */
  std::string IPClient = "127.0.0.1";
  vtkGetMacro(IPClient, std::string);
  vtkSetMacro(IPClient, std::string);
  /*! IP of the computer running the server. Defaults to "127.0.0.1" */
  std::string IPServer = "127.0.0.1";
  vtkGetMacro(IPServer, std::string);
  vtkSetMacro(IPServer, std::string);

  /*! Scale factor for converting camera units to Mm */
  float UnitsToMm;
  /*! List of tracked tools defined by the config file */
  std::set<std::string> TrackedTools;
  /*! Map from the numerical id of tool rigidbody to tool name */
  std::map<int, std::string> TrackedToolMap;

};

#endif
