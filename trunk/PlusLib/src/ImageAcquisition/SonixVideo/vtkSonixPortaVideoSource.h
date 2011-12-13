/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

/*=========================================================================
The following copyright notice is applicable to parts of this file:
Copyright (c) Elvis Chen, 2008
All rights reserved.
See Copyright.txt or http://www.kitware.com/Copyright.htm for details.
Authors include: Elvis Chen (Queen's University)
=========================================================================*/  

#ifndef __VTKSONIXPORTAVIDEOSOURCE_H__
#define __VTKSONIXPORTAVIDEOSOURCE_H__

#include "PlusConfigure.h"
#include "vtkPlusVideoSource.h"

// porta includes
#include <porta_std_includes.h>
#include <porta_params_def.h>
#include <ImagingModes.h>
#include <porta.h>

class VTK_EXPORT vtkSonixPortaVideoSource;

/*!
\class vtkSonixPortaVideoSourceCleanup 
\brief Class that cleans up (deletes singleton instance of) vtkSonixPortaVideoSource when destroyed
\ingroup PlusLibImageAcquisition
*/
class VTK_EXPORT vtkSonixPortaVideoSourceCleanup 
{
public:
  vtkSonixPortaVideoSourceCleanup();
  ~vtkSonixPortaVideoSourceCleanup();
};

/*!
  \class vtkSonixPortaVideoSource 
  \brief Class providing VTK video input interface for SonixRP

  vtkSonixPortaVideoSource is a subclass of vtkVideoSource that
  implementes Porta API for importing video directly from the Cine
  buffer of a Sonix RP Ultrasound machine.

  You must call the ReleaseSystemResources() method before the application
  exits.  Otherwise the application might hang while trying to exit.

  the 3D/4D probe should always be connected to port-0

  Usage:
  sonixGrabber = vtkSonixPortaVideoSource::New();
  sonixGrabber->SetPortaSettingPath( "../../dat/" );
  sonixGrabber->SetPortaFirmwarePath( "../../fw/" );
  sonixGrabber->SetPortaLUTPath( "C:/LUTs/" );
  sonixGrabber->SetPortaCineSize( 256*1024*1024 );
  // not needed, defaults 256MB
  sonixGrabber->SetBLineDensity( 128 );
  // line density effect the frame rate
  sonixGrabber->Record();
  imageviewer->SetInput( sonixGrabber->GetOutput() ); 

  \ingroup PlusLibImageAcquisition
*/ 
class VTK_EXPORT vtkSonixPortaVideoSource : public vtkPlusVideoSource {

  vtkTypeRevisionMacro( vtkSonixPortaVideoSource, vtkPlusVideoSource );
  void PrintSelf( ostream &os, vtkIndent indent );

  /*! This is a singleton pattern New.  There will only be ONE
   reference to a vtkOutputWindow object per process.  Clients that
   call this must call Delete on the object so that the reference
   counting will work.  The single instance will be unreferenced
   when the program exits. */
  static vtkSonixPortaVideoSource *New();

  /*! return the singleton instance with no reference counting */
  static vtkSonixPortaVideoSource *GetInstance();

  /*! Supply a user defined output window.  Call ->Delete() on the supplied instance after setting it. */
  static void SetInstance( vtkSonixPortaVideoSource *instance );

  //BTX
  /*! use this as a way of memory management when the
   program exits the SmartPointer will be deleted which
   will delete the Instance singleton */
  static vtkSonixPortaVideoSourceCleanup Cleanup;
  //ETX

  /*! Read/write main configuration from/to xml data */
  virtual PlusStatus ReadConfiguration(vtkXMLDataElement* config); 
  virtual PlusStatus WriteConfiguration(vtkXMLDataElement* config);

  /* List of parameters which can be set or read in B-mode, other mode parameters not currently implemented*/  
  
  /*! Get/Set the probe name */
  vtkSetStringMacro(PortaProbeName); 
  vtkGetStringMacro(PortaProbeName); 

  /*! Get/Set the probe name */
  vtkSetStringMacro(PortaSettingPath); 
  vtkGetStringMacro(PortaSettingPath); 

  /*! Get/Set the firmware path name */
  vtkSetStringMacro(PortaFirmwarePath); 
  vtkGetStringMacro(PortaFirmwarePath); 

  /*! Get/Set the look-up table path name */
  vtkSetStringMacro(PortaLUTPath); 
  vtkGetStringMacro(PortaLUTPath); 

  /*! Get/Set the look-up table path name */
  vtkSetStringMacro(PortaLicensePath); 
  vtkGetStringMacro(PortaLicensePath); 

  /*! Frequency corresponds to paramID value of 414 for Uterius SDK */
  vtkGetMacro(Frequency, int);
  vtkSetMacro(Frequency, int);

  /*! Get/set the depth (mm) of B-mode ultrasound; valid range: ; in increments of  */
  // Depth corresponds to paramID value of 206 for Uterius SDK
  vtkGetMacro(Depth, int);
  vtkSetMacro(Depth, int);

  /*! Get/set the Gain (%) of B-mode ultrasound; valid range: 0-100 ; in increments of  */
  // Gain corresponds to paramID value of 15 for Uterius SDK
  vtkGetMacro(Gain, int);
  vtkSetMacro(Gain, int);

  /*! Get/set the Zoom (%) of B-mode ultrasound; valid range: 0-100 ; in increments of  */
  // Zoom corresponds to paramID value of 1176 for Uterius SDK
  vtkGetMacro(Zoom, int);
  vtkSetMacro(Zoom, int);
  
  /*! Get/set the Timeout value for network function calls. */
  // (Default: -1) 
  vtkGetMacro(Timeout, int);
  vtkSetMacro(Timeout, int); 

  
  void SetImagingMode(int mode){ImagingMode = mode;};
  void GetImagingMode(int & mode){mode = ImagingMode;};
  
  /*! set the CineSize, defaults to 256MB, size in bytes */
  void SetPortaCineSize( int size );

  /*! Set the start position */
  void SetPortaMotorStartPosition( double v );
  
  /*! Return the start position */
  double GetPortaMotorStartPosition();
  
  /*! Go to the start position */
  double GoToPortaMotorStartPosition();
  
  /*! Moves the motor a certain angle from the home position */
  double GoToPosition( double angle );
  
  /*! Moves the motor clockwise or counter-clockwise by one half-step. */
  double StepPortaMotor( bool cw, int steps );
  
  /*! Return the current motor position */
  double GetPortaMotorPosition();

  /*! Return current frame rate as reported by Porta */
  int GetPortaFrameRate();

  
  /*! Get/set the steps motor moves for each frame */
  vtkGetMacro(StepPerFrame, int);
  vtkSetMacro(StepPerFrame, int);

  /*! Get/set the number of frames for each sweep of the motor */
  vtkGetMacro(FramePerVolume, int);
  vtkSetMacro(FramePerVolume, int);

  
protected:
  vtkSonixPortaVideoSource();
  ~vtkSonixPortaVideoSource();

  /*! Connect to device */
  virtual PlusStatus InternalConnect();


  /*! Disconnect from device */
  virtual PlusStatus InternalDisconnect();

  /*! Record incoming video at the specified FrameRate.  The recording */
  // continues indefinitely until StopRecording() is called. 
  virtual PlusStatus InternalStartRecording();

  /*! Stop recording or playing. */
  virtual PlusStatus InternalStopRecording();


  /*! pointer to the hardware */
  porta Porta;

  /*! Imaging parameters */
  int Depth;
  int Frequency;
  int Gain; 
  int Zoom; 
  int FrameRate;
  int ImagingMode;
  int Timeout; 
  
  /*! for internal use only */
  PlusStatus AddFrameToBuffer( void *param, int id );
			  
private:
 
  // data members
  static vtkSonixPortaVideoSource *Instance;
  
  /*! call back whenever a new frame is available */
#if (PLUS_ULTRASONIX_SDK_MAJOR_VERSION < 5) || (PLUS_ULTRASONIX_SDK_MAJOR_VERSION == 5 && PLUS_ULTRASONIX_SDK_MINOR_VERSION < 7)
  static bool vtkSonixPortaVideoSourceNewFrameCallback( void *param, int id );
#else // SDK version 5.7.x or newer
  static bool vtkSonixPortaVideoSourceNewFrameCallback( void *param, int id, int header );
#endif
  
  // vtkSonixVideoSource(const vtkSonixVideoSource&);  // Not implemented.
  // void operator=(const vtkSonixVideoSource&);  // Not implemented.							
  
  /*! Update porta parameters */
  PlusStatus vtkSonixPortaVideoSource::UpdateSonixPortaParams();


  // Porta parameters
  /*! The starting position of the motor */
  double PortaMotorStartPosition;

  /*! The current/actual position of the motor */
  double PortaMotorPosition;

  /*! The B-mode image width */
  int PortaBModeWidth;

  /*! The B-mode iamge height */
  int PortaBModeHeight;

  /*! 1 if the probe has been selected, 0 otherwise */
  int PortaProbeSelected;

  /*! 1 if the PortaImageMode has been set in the hardware, 0 otherwise */
  int PortaModeSelected;

  /*! name of the probe */
  char *PortaProbeName;

  /*! Path to the Porta Setting directory */
  char *PortaSettingPath;

  /*! Path to the Porta Firmware directory */
  char *PortaFirmwarePath;

  /*! Path to the Porta LUT directory */
  char *PortaLUTPath;

  /*! Path to the licenses text file */
  char *PortaLicensePath;

  /*! Size of the Cine buffer */
  int PortaCineSize;
    
  /*! storage for sonix frame buffer */
  unsigned char *ImageBuffer; 

  /*! the number of frames that will acquired in one sweep of the motor */
  int FramePerVolume;
  
  /*!  The number of steps the motor will move while acuqiring a single frame */
  int StepPerFrame;

  
};


#endif // of __VTKSONIXPORTAVIDEOSOURCE_H__
