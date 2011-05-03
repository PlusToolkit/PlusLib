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
// sonixGrabber->Initialize();
// sonixGrabber->Record();
// imageviewer->SetInput( sonixGrabber->GetOutput() ); 
// .SECTION See Also
// vtkWin32VideoSource vtkMILVideoSource

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
  // Initialize the driver (this is called automatically 
  // when the first grab is done)
  void Initialize();
  
  // Description:
  // Record incoming video at the specified FrameRate.  The recording
  // continues indefinitely until Stop() is called.
  void Record();
  
  // Description:
  // Play through the 'tape' sequentially at the specified frame rate.
  // If you have just finished Recording, you should call Rewind() first.
  void Play();
  
  // Description
  // Stop recording or Playing
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
  void LocalInternalGrab( void *param, unsigned char *data,
			  int cineBlock, int reserved );

  // data members
  static vtkSonixPortaVideoSource *Instance;

  // Description:
  // pointer to the Sonix RP hardware
  porta *PtrPorta;

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
  // 0 if porta has not been initialized, 1 otherwise
  // use int since VTK doesn't quite support boolean
  int Initialized;
  
  // Description:
  // storage for sonix frame buffer
  unsigned char *ImageBuffer;
  
  // Description:
  // 0 if the PortaSettingPath is not set, 1 otherwise
  int bIsPortaSettingPathSet;
  
  // Description:
  // 0 if the PortaSettingPath is not set, 1 otherwise
  int bIsPortaFirmwarePathSet;
  
  // Description:
  // 0 if the PortaSettingPath is not set, 1 otherwise
  int bIsPortaLUTPathSet;
};


#endif // of __VTKSONIXPORTAVIDEOSOURCE_H__
