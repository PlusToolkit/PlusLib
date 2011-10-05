/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkSonixPortaVideoSource.h,v $

  Copyright (c) Elvis Chen, Queen's University,
                chene@cs.queensu.ca
                Sept 2008.
  All rights reserved.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkSonixPortaVideoSource - video input interface for SonixRP
// .SECTION Description
// vtkSonixPortaVideoSource is a subclass of vtkVideoSource that
// implementes Porta API for importing video directly from the Cine
// buffer of a Sonix RP Ultrasound machine.
// .SECTION Caveats
// You must call the ReleaseSystemResources() method before the application
// exits.  Otherwise the application might hang while trying to exit.
//
// the 3D/4D probe should always be connected to port-0
// .SECTION Usage
// sonixGrabber = vtkSonixPortaVideoSource::New();
// sonixGrabber->SetPortaSettingPath( "../../dat/" );
// sonixGrabber->SetPortaFirmwarePath( "../../fw/" );
// sonixGrabber->SetPortaLUTPath( "C:/LUTs/" );
// sonixGrabber->SetPortaCineSize( 256*1024*1024 );
// /* not needed, defaults 256MB */
// sonixGrabber->SetBLineDensity( 128 );
// /* line density effect the frame rate */
// sonixGrabber->Record();
// imageviewer->SetInput( sonixGrabber->GetOutput() ); 
// .SECTION See Also
// vtkWin32VideoSource vtkMILVideoSource



#ifndef __VTKSONIXPORTAVIDEOSOURCE_H__
#define __VTKSONIXPORTAVIDEOSOURCE_H__

#include "PlusConfigure.h"
#include "vtkPlusVideoSource.h"

// porta includes
#include <porta_std_includes.h>
#include <porta.h>



class VTK_EXPORT vtkSonixPortaVideoSource;

class VTK_EXPORT vtkSonixPortaVideoSourceCleanup 
{
public:
  vtkSonixPortaVideoSourceCleanup();
  ~vtkSonixPortaVideoSourceCleanup();
};

class VTK_EXPORT vtkSonixPortaVideoSource : public vtkPlusVideoSource {

  vtkTypeRevisionMacro( vtkSonixPortaVideoSource, vtkPlusVideoSource );
  void PrintSelf( ostream &os, vtkIndent indent );

  // Description:
  // This is a singleton pattern New.  There will only be ONE
  // reference to a vtkOutputWindow object per process.  Clients that
  // call this must call Delete on the object so that the reference
  // counting will work.  The single instance will be unreferenced
  // when the program exits.
  static vtkSonixPortaVideoSource *New();

  // Description:
  // return the singleton instance with no reference counting
  static vtkSonixPortaVideoSource *GetInstance();

  // Description:
  // Supply a user defined output window.  Call ->Delete() on the supplied
  // instance after setting it.
  static void SetInstance( vtkSonixPortaVideoSource *instance );
  //BTX

  // use this as a way of memory management when the
  // program exits the SmartPointer will be deleted which
  // will delete the Instance singleton
  static vtkSonixPortaVideoSourceCleanup Cleanup;
  //ETX

  // Description:
  // Read/write main configuration from/to xml data
  virtual PlusStatus ReadConfiguration(vtkXMLDataElement* config); 
  virtual PlusStatus WriteConfiguration(vtkXMLDataElement* config);

  /* List of parameters which can be set or read in B-mode, other mode parameters not currently implemented*/  
  
  // Description:
  // Get/Set the probe name
  vtkSetStringMacro(PortaProbeName); 
  vtkGetStringMacro(PortaProbeName); 

  // Description:
  // Get/Set the probe name
  vtkSetStringMacro(PortaSettingPath); 
  vtkGetStringMacro(PortaSettingPath); 

  // Description:
  // Get/Set the firmware path name
  vtkSetStringMacro(PortaFirmwarePath); 
  vtkGetStringMacro(PortaFirmwarePath); 

  // Description:
  // Get/Set the look-up table path name
  vtkSetStringMacro(PortaLUTPath); 
  vtkGetStringMacro(PortaLUTPath); 

  // Description:
  // Get/Set the look-up table path name
  vtkSetStringMacro(PortaLicensePath); 
  vtkGetStringMacro(PortaLicensePath); 

  // Description:
  // Frequency corresponds to paramID value of 414 for Uterius SDK
  vtkGetMacro(Frequency, int);
  vtkSetMacro(Frequency, int);

  // Description:
  // Get/set the depth (mm) of B-mode ultrasound; valid range: ; in increments of 
  // Depth corresponds to paramID value of 206 for Uterius SDK
  vtkGetMacro(Depth, int);
  vtkSetMacro(Depth, int);

  // Description:
  // Get/set the Gain (%) of B-mode ultrasound; valid range: 0-100 ; in increments of 
  // Gain corresponds to paramID value of 15 for Uterius SDK
  vtkGetMacro(Gain, int);
  vtkSetMacro(Gain, int);

  // Description:
  // Get/set the Zoom (%) of B-mode ultrasound; valid range: 0-100 ; in increments of 
  // Zoom corresponds to paramID value of 1176 for Uterius SDK
  vtkGetMacro(Zoom, int);
  vtkSetMacro(Zoom, int);
  
  // Description:
  // Get/set the Timeout value for network function calls.
  // (Default: -1) 
  vtkGetMacro(Timeout, int);
  vtkSetMacro(Timeout, int); 

  
  void SetImagingMode(int mode){ImagingMode = mode;};
  void GetImagingMode(int & mode){mode = ImagingMode;};
  
  // Description:
  // set the CineSize, defaults to 256MB, size in bytes
  void SetPortaCineSize( int size );

  // Description:
  // Set the start position
  void SetPortaMotorStartPosition( double v );
  
  // Description:
  // Return the start position
  double GetPortaMotorStartPosition();
  
  // Description:
  // Go to the start position
  double GoToPortaMotorStartPosition();
  
  // Description:
  // Moves the motor a certain angle from the home position
  double GoToPosition( double angle );
  
  // Description:
  // Moves the motor clockwise or counter-clockwise by one half-step.
  double StepPortaMotor( bool cw, int steps );
  
  // Description:
  // Return the current motor position
  double GetPortaMotorPosition();

  // Description:
  // Return current frame rate as reported by Porta
  int GetPortaFrameRate();

  
  // Description:
  // Get/set the steps motor moves for each frame
  vtkGetMacro(StepPerFrame, int);
  vtkSetMacro(StepPerFrame, int);

  // Description:
  // Get/set the number of frames for each sweep of the motor
  vtkGetMacro(FramePerVolume, int);
  vtkSetMacro(FramePerVolume, int);

  
protected:
  vtkSonixPortaVideoSource();
  ~vtkSonixPortaVideoSource();

  // Description:
  // Connect to device
  virtual PlusStatus InternalConnect();


  // Description:
  // Disconnect from device
  virtual PlusStatus InternalDisconnect();

  // Description:
  // Record incoming video at the specified FrameRate.  The recording
  // continues indefinitely until StopRecording() is called. 
  virtual PlusStatus InternalStartRecording();

  // Description:
  // Stop recording or playing.
  virtual PlusStatus InternalStopRecording();


  // Description:
  // pointer to the hardware
  porta Porta;

  // Description:
  // Imaging parameters
  int Depth;
  int Frequency;
  int Gain; 
  int Zoom; 
  int FrameRate;
  int ImagingMode;
  int Timeout; 
  
  // Description:
  // for internal use only
  PlusStatus AddFrameToBuffer( void *param, int id );
			  
private:
 
  // data members
  static vtkSonixPortaVideoSource *Instance;
  
  // Description:
  // call back whenever a new frame is available
  static bool vtkSonixPortaVideoSourceNewFrameCallback( void *param, int id );
  // vtkSonixVideoSource(const vtkSonixVideoSource&);  // Not implemented.
  // void operator=(const vtkSonixVideoSource&);  // Not implemented.							
  
  // Description:
  // Update porta parameters
  PlusStatus vtkSonixPortaVideoSource::UpdateSonixPortaParams();


  // Porta parameters
  // Description:
  // The starting position of the motor
  double PortaMotorStartPosition;

  // Description:
  // The current/actual position of the motor
  double PortaMotorPosition;

  // Description:
  // The B-mode image width
  int PortaBModeWidth;

  // Description:
  // The B-mode iamge height
  int PortaBModeHeight;

  // Description:
  // 1 if the probe has been selected, 0 otherwise
  int PortaProbeSelected;

  // Description:
  // 1 if the PortaImageMode has been set in the hardware, 0 otherwise
  int PortaModeSelected;

  // Description:
  // name of the probe
  char *PortaProbeName;

  // Description:
  // Path to the Porta Setting directory
  char *PortaSettingPath;

  // Description:
  // Path to the Porta Firmware directory
  char *PortaFirmwarePath;

  // Description:
  // Path to the Porta LUT directory
  char *PortaLUTPath;

  // Description:
  // Path to the licenses text file
  char *PortaLicensePath;

  // Description:
  // Size of the Cine buffer
  int PortaCineSize;
    
  // Description:
  // storage for sonix frame buffer
  unsigned char *ImageBuffer; 

  // Description:
  // the number of frames that will acquired in one sweep of the motor
  int FramePerVolume;
  
  // Description:
  //  The number of steps the motor will move while acuqiring a single frame
  int StepPerFrame;

  
};


#endif // of __VTKSONIXPORTAVIDEOSOURCE_H__



/*
#ifndef __VTKSONIXPORTAVIDEOSOURCE_H__
#define __VTKSONIXPORTAVIDEOSOURCE_H__

// VTK includes
#include <vtkVideoSource.h>

// porta includes
#include <porta_std_includes.h>


class VTK_EXPORT vtkSonixPortaVideoSource;

class VTK_EXPORT vtkSonixPortaVideoSourceCleanup 
{
public:
  vtkSonixPortaVideoSourceCleanup();
  ~vtkSonixPortaVideoSourceCleanup();
};

class VTK_EXPORT vtkSonixPortaVideoSource : public vtkVideoSource {

public:
  vtkTypeRevisionMacro( vtkSonixPortaVideoSource, vtkVideoSource );
  void PrintSelf( ostream &os, vtkIndent indent );

  // Description:
  // This is a singleton pattern New.  There will only be ONE
  // reference to a vtkOutputWindow object per process.  Clients that
  // call this must call Delete on the object so that the reference
  // counting will work.  The single instance will be unreferenced
  // when the program exits.
  static vtkSonixPortaVideoSource *New();
  
  // Description:
  // return the singleton instance with no reference counting
  static vtkSonixPortaVideoSource *GetInstance();
  
  // Description:
  // Supply a user defined output window.  Call ->Delete() on the supplied
  // instance after setting it.
  static void SetInstance( vtkSonixPortaVideoSource *instance );

  //BTX
  // use this as a way of memory management when the
  // program exits the SmartPointer will be deleted which
  // will delete the Instance singleton
  static vtkSonixPortaVideoSourceCleanup Cleanup;
  //ETX
  
  // Description:
  // Record incoming video at the specified FrameRate.  The recording
  // continues indefinitely until Stop() is called.
  void Record();
    
  // Description
  // Stop recording
  void Stop();

  // Description
  // Grab a single video frame
  void Grab();

  // Description:
  // Request a particular vtk output format (default: VTK_RGB)
  void SetOutputFormat( int format );
  
  // Description:
  // Release the video driver.  This method must be called before
  // application exit, or else the application might hang during
  // exit.  
  void ReleaseSystemResources();

  // Description:
  // set the path for porta setting
  void SetPortaSettingPath( char *path );
  
  // Description:
  // set the path for porta firmware
  void SetPortaFirmwarePath( char *path );
  
  // Description:
  // set the path for porta LUT
  void SetPortaLUTPath( char *path );
  
  // Description:
  // set the CineSize, defaults to 256MB, size in bytes
  void SetPortaCineSize( int size );
  
  // Description:
  // Adjusts the zoom level of the B image, and any overlay that is associated,
  // units in percentage, where 100% scales the image so that the 
  // full height exactly fits the specified display area. 
  int SetZoom( int percentage );

  // Description:
  // Adjusts the line density when the sector is at 100 %
  int SetBLineDensity( int density );

  // Description:
  // Set the imaging depth, units in mm
  int SetImageDepth( int myDepth );

  // Description:
  // Set the start position
  void SetPortaMotorStartPosition( double v );
  
  // Description:
  // Return the start position
  double GetPortaMotorStartPosition();
  
  // Description:
  // Go to the start position
  double GoToPortaMotorStartPosition();
  
  // Description:
  // Moves the motor a certain angle from the home position
  double GoToPosition( double angle );
  
  // Description:
  // Moves the motor clockwise or counter-clockwise by one half-step.
  double StepPortaMotor( bool cw, int steps );
  
  // Description:
  // Return the current motor position
  double GetPortaMotorPosition();

  // Description:
  // Return current frame rate as reported by Porta
  int GetPortaFrameRate();

  // Description:
  // Set the Frame Size in BMode
  // Return 1 if successful, 0 otherwise
  int SetBModeFrameSize( int w, int h );
  
protected:
  vtkSonixPortaVideoSource();
  ~vtkSonixPortaVideoSource();

  // Description:
  // update/upload imaging parameters to the Sonix hardware.
  // Needed if the imaging parameter is changed and the
  // user desire to see the changes reflected on the next
  // imaged frame.  Return 1 if successful, 0 otherwise
  int UpdateSonixPortaParams();
   
  // Description:
  // Set up the frame buffer format
  void DoFormatSetup(); 

  // Description:
  int RequestInformation( vtkInformation *, vtkInformationVector **,
			  vtkInformationVector * );
  
  // Description:
  // Request data method override
  int RequestData( vtkInformation*, vtkInformationVector **,
		   vtkInformationVector * );

  // Description:
  // how to unpack each line?
  void UnpackRasterLine( char *outptr, char *inptr,
			 int start, int count );

  // Description:
  // Connect to device
  virtual PlusStatus InternalConnect();

  // Description:
  // Disconnect from device
  virtual PlusStatus InternalDisconnect(); 

  // Description:
  // Imaging depth
  int Depth;
  
private:
  // private functions

  // Description:
  // call back whenever a new frame is available
  static bool vtkSonixPortaVideoSourceNewFrameCallback( void *param,
							unsigned char *data,
							int cineBlock,
							int reserved );

  // Description:
  // for internal use only
  void AddFrameToBuffer( void *param, unsigned char *data,
			  int cineBlock, int reserved );

  // Description:
  // Get/Set the probe name
  vtkSetStringMacro(PortaProbeName); 
  vtkGetStringMacro(PortaProbeName); 

  // Description:
  // Get/Set the firmware path name
  vtkSetStringMacro(PortaFirmwarePath); 
  vtkGetStringMacro(PortaFirmwarePath); 

  // Description:
  // Get/Set the look-up table path name
  vtkSetStringMacro(PortaLUTPath); 
  vtkGetStringMacro(PortaLUTPath); 

  // data members
  static vtkSonixPortaVideoSource *Instance;

  // Description:
  // pointer to the Sonix RP hardware
  porta Porta;

  // Description:
  // The starting position of the motor
  double PortaMotorStartPosition;

  // Description:
  // The current/actual position of the motor
  double PortaMotorPosition;

  // Description:
  // The B-mode image width
  int PortaBModeWidth;

  // Description:
  // The B-mode iamge height
  int PortaBModeHeight;

  // Description:
  // The M-mode image width
  int PortaMModeWidth;

  // Description:
  // The M-mode iamge height
  int PortaMModeHeight;

  // Description:
  // The image mode, B-mode only for now
  int PortaImageMode;

  // Description:
  // 1 if the probe has been selected, 0 otherwise
  int PortaProbeSelected;

  // Description:
  // 1 if the PortaImageMode has been set in the hardware, 0 otherwise
  int PortaModeSelected;

  // Description:
  // name of the probe
  char *PortaProbeName;

  // Description:
  // Path to the Porta Setting directory
  char *PortaSettingPath;

  // Description:
  // Path to the Porta Firmware directory
  char *PortaFirmwarePath;

  // Description:
  // Path to the Porta LUT directory
  char *PortaLUTPath;

  // Description:
  // Size of the Cine buffer
  int PortaCineSize;
    
  // Description:
  // storage for sonix frame buffer
  unsigned char *ImageBuffer;    
  
};


#endif // of __VTKSONIXPORTAVIDEOSOURCE_H__
*/