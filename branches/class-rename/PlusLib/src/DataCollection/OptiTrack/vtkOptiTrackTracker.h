/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

/*========================================================================
Date: Ag 2015
Authors include:
- Eugenio Marinetto [*][ç] emarinetto@hggm.es
- Laura Sanz [*] lsanz@hggm.es
- David Garcia [*] dgnati@hggm.es
- Mikael Brudfors [*] brudfors@hggm.es
- Javier Pascau [*][ç] jpascau@hggm.es
[*] Laboratorio de Imagen Medica, Hospital Gregorio Maranon - http://image.hggm.es/
[ç] Departamento de Bioingeniería e Ingeniería Aeroespacial. Universidad Carlos III de Madrid
==========================================================================*/

#ifndef __vtkOptiTrackTracker_h
#define __vtkOptiTrackTracker_h

#include "vtkDataCollectionExport.h"

#include "vtkPlusDevice.h"

// BiiGOptitrackControl github.com/nenetto/NPTrackingTools
#include "OptitrackTracker.h"
#include "OptitrackTool.h"
#include <sstream>

/*!
\class vtkOptimetConoProbeTracker 
\brief Interface for the OptiTrack Tracker
This class talks with a OptiTrack Tracker over the NPTrackingtools library.
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

  /*! Get the transformation for a connected tool given the tool name = PortName (PLUS) */
  void GetTransformMatrix(std::string toolName, vtkMatrix4x4* transformMatrix);

protected:

  vtkOptiTrackTracker();
  ~vtkOptiTrackTracker();
  
private:  // Functions.

  vtkOptiTrackTracker( const vtkOptiTrackTracker& );
  void operator=( const vtkOptiTrackTracker& ); 

private:  // Variables.  

  /*! Object that represents the Tracker */
  Optitrack::OptitrackTracker::Pointer OptiTrackTracker;

  /*! List of Configuration Files for Tools */
  std::vector<std::string> OptiTrackToolsConfFiles;

  /*! Cameras Parameter 1 */
  int Exposition;

  /*! Cameras Parameter 2 */
  int Threshold;

  /*! Cameras Parameter 3 */
  int Illumination;

  /*! Calibration File for Optitrack System */
  std::string CalibrationFile;

  /*! Index of the last frame number. This is used for providing a frame number when the tracker doesn't return any transform */
  unsigned long LastFrameNumber;

};

#endif