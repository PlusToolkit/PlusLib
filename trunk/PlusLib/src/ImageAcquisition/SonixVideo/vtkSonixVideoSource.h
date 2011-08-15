/*=========================================================================

Module:    $RCSfile: vtkSonixVideoSource.h,v $
Author:  Siddharth Vikal, Queens School Of Computing

Copyright (c) 2008, Queen's University, Kingston, Ontario, Canada
All rights reserved.

Author: Danielle Pace
        Robarts Research Institute and The University of Western Ontario


Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:

 * Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.

 * Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in
   the documentation and/or other materials provided with the
   distribution.

 * Neither the name of Queen's University nor the names of any
   contributors may be used to endorse or promote products derived
   from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.

=========================================================================*/
  
// .NAME vtkSonixVideoSource - VTK interface for video input from Ultrasonix machine
// .SECTION Description
// vtkSonixVideoSource is a class for providing video input interfaces between VTK and Ultrasonix machine.
// The goal is to provide the ability to be able to do acquisition
// in various imaging modes, buffer the image/volume series being acquired
// and stream the frames to output. 
// Note that the data coming out of the Sonix rp through ulterius is always RGB
// This class talks to Ultrasonix's Ulterius SDK for executing the tasks 
// .SECTION Caveats
// You must call the ReleaseSystemResources() method before the application
// exits.  Otherwise the application might hang while trying to exit.
// .SECTION Usage
//  sonixGrabber->SetSonixIP("130.15.7.212");
//  sonixGrabber->SetImagingMode(0);
//  sonixGrabber->SetAcquisitionDataType(0x00000004);
//  sonixGrabber->Record();  
//  imageviewer->SetInput(sonixGrabber->GetOutput());
//  See SonixVideoSourceTest.cxx for more details
// .SECTION See Also
// vtkWin32VideoSource vtkMILVideoSource

#ifndef __vtkSonixVideoSource_h
#define __vtkSonixVideoSource_h

#include "PlusConfigure.h"
#include "vtkVideoSource2.h"
#include "ulterius.h"
#include "ulterius_def.h"

//BTX

class uDataDesc;
class ulterius;

class VTK_EXPORT vtkSonixVideoSource;

class VTK_EXPORT vtkSonixVideoSourceCleanup
{
public:
  vtkSonixVideoSourceCleanup();
  ~vtkSonixVideoSourceCleanup();
};
//ETX

class VTK_EXPORT vtkSonixVideoSource : public vtkVideoSource2
{
public:
  //static vtkSonixVideoSource *New();
  vtkTypeRevisionMacro(vtkSonixVideoSource,vtkVideoSource2);
  void PrintSelf(ostream& os, vtkIndent indent);   
  // Description:
  // This is a singleton pattern New.  There will only be ONE
  // reference to a vtkOutputWindow object per process.  Clients that
  // call this must call Delete on the object so that the reference
  // counting will work.   The single instance will be unreferenced when
  // the program exits.
  static vtkSonixVideoSource* New();
  // Description:
  // Return the singleton instance with no reference counting.
  static vtkSonixVideoSource* GetInstance();

  // Description:
  // Supply a user defined output window. Call ->Delete() on the supplied
  // instance after setting it.
  static void SetInstance(vtkSonixVideoSource *instance);
  //BTX
  // use this as a way of memory management when the
  // program exits the SmartPointer will be deleted which
  // will delete the Instance singleton
  static vtkSonixVideoSourceCleanup Cleanup;
  //ETX

  // Description:
  // Read/write main configuration from/to xml data
  virtual PlusStatus ReadConfiguration(vtkXMLDataElement* config); 
  virtual PlusStatus WriteConfiguration(vtkXMLDataElement* config);

  // Description:
  // Connect to device
  virtual PlusStatus Connect();

  // Description:
  // Disconnect from device
  virtual PlusStatus Disconnect();

  // Description:
  // Record incoming video at the specified FrameRate.  The recording
  // continues indefinitely until StopRecording() is called. 
  virtual PlusStatus StartRecording();

  // Description:
  // Stop recording or playing.
  virtual PlusStatus StopRecording();

  // Description:
  // Grab a single video frame.
  PlusStatus Grab();

  // Description:
  // Request a particular mode of imaging (e.g. B-mode (0), M-mode(1), Color-doppler(2), pulsed-doppler(3); default: B-mode).
  void SetImagingMode(int mode){ImagingMode = mode;};
  void GetImagingMode(int & mode){mode = ImagingMode;};

  // Description:
  // Get/Set the IP address of the Ultrasonix host machine
  vtkSetStringMacro(SonixIP); 
  vtkGetStringMacro(SonixIP); 

  /* List of parameters which can be set or read in B-mode, other mode parameters not currently implemented*/

  // Description:
  // Request a particular mode of imaging (e.g. B-mode (0), M-mode(1), Color-doppler(2), pulsed-doppler(3); default: B-mode).
  // Frequency corresponds to paramID value of 414 for Uterius SDK
  vtkGetMacro(Frequency, int);
  vtkSetMacro(Frequency, int);

  // Description:
  // Get/set the depth (mm) of B-mode ultrasound; valid range: ; in increments of 
  // Depth corresponds to paramID value of 206 for Uterius SDK
  vtkGetMacro(Depth, int);
  vtkSetMacro(Depth, int);

  // Description:
  // Get/set the frame rate (fps) of B-mode ultrasound; valid range: ; in increments of 
  // FrameRate corresponds to paramID value of 584 for Uterius SDK
  vtkGetMacro(FrameRate, float);
  vtkSetMacro(FrameRate, float);

  // Description:
  // Get/set the Gain (%) of B-mode ultrasound; valid range: 0-100 ; in increments of 
  // Gain corresponds to paramID value of 15 for Uterius SDK
  vtkGetMacro(Gain, int);
  vtkSetMacro(Gain, int);

    // Description:
  // Get/set the DynRange (dB) of B-mode ultrasound; valid range: ; in increments of 
  // Gain corresponds to paramID value of 361 for Uterius SDK
  vtkGetMacro(DynRange, int);
  vtkSetMacro(DynRange, int);

    // Description:
  // Get/set the Zoom (%) of B-mode ultrasound; valid range: 0-100 ; in increments of 
  // Zoom corresponds to paramID value of 1176 for Uterius SDK
  vtkGetMacro(Zoom, int);
  vtkSetMacro(Zoom, int);

  // Description:
  // Get/set the Sector (%) of B-mode ultrasound; valid range: 0-100 ; in increments of 
  // Sector corresponds to paramID value of 1116 for Uterius SDK
  vtkGetMacro(Sector, int);
  vtkSetMacro(Sector, int);

  // Description:
  // Get/set the CompressionStatus to 0 for compression off, 1 for compression on. 
  // (Default: off)
  vtkGetMacro(CompressionStatus, int);
  vtkSetMacro(CompressionStatus, int);

  // Description:
  // Get/set the Timeout value for network function calls.
  // (Default: -1) 
  vtkGetMacro(Timeout, int);
  vtkSetMacro(Timeout, int);

  // Description:
  // Request a particular data type from sonix machine by means of a mask.
  // Range of types supported:  1) Screen (800 x 600); 2) B Pre Scan Converted; 3) B Post Scan Converted (8 bit);
  //              4) B Post Scan Converted (32 bit); 5) RF; 6) M Pre Scan Converted;
  //              7) M Post Scan Converted; 8) PW RF; 9) PW Spectrum;
  //              10)Color RF; 11) Color Post; 12) Color Sigma;
  //              13)Color Velocity; 14) Elasto + B-image (32); 15) Elasto Overlay (8 bit); 16) Elasto Pre Scan Coverted (8 bit)
  // Currently supported data masks: 1) Screen
  // The mask must be applied before any data can be acquired via realtime imaging or cine retreival
  vtkGetMacro(AcquisitionDataType, int);
  vtkSetMacro(AcquisitionDataType, int);
  

  // Description:
  // Initialize the driver (this is called automatically when the
  // first grab is done).
  PlusStatus Initialize();

  // Description:
  // Free the driver (this is called automatically inside the
  // destructor).
  void ReleaseSystemResources();

protected:
  vtkSonixVideoSource();
  virtual ~vtkSonixVideoSource();

  std::string GetLastUlteriusError();

  ulterius Ult;
  uDataDesc DataDescriptor;
  int Frequency;
  int Depth;
  int Sector; 
  int Gain; 
  int DynRange; 
  int Zoom; 
  int FrameRate;
  int AcquisitionDataType;
  int ImagingMode;
  int OutputFormat;
  int CompressionStatus; 
  int Timeout; 

  
  char *SonixIP;

  // Description:
  // For internal use only
  PlusStatus LocalInternalGrab(void * data, int type, int sz, bool cine, int frmnum);
  
private:
 
  static vtkSonixVideoSource* Instance;
  static bool vtkSonixVideoSourceNewFrameCallback(void * data, int type, int sz, bool cine, int frmnum);
  vtkSonixVideoSource(const vtkSonixVideoSource&);  // Not implemented.
  void operator=(const vtkSonixVideoSource&);  // Not implemented.
};

#endif





