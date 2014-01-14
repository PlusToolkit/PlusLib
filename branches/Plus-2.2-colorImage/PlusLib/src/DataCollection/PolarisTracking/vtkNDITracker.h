/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

/*=========================================================================
The following copyright notice is applicable to parts of this file:

Copyright (c) 2000-2005 Atamai, Inc.

Use, modification and redistribution of the software, in source or
binary forms, are permitted provided that the following terms and
conditions are met:

1) Redistribution of the source code, in verbatim or modified
   form, must retain the above copyright notice, this license,
   the following disclaimer, and any notices that refer to this
   license and/or the following disclaimer.  

2) Redistribution in binary form must include the above copyright
   notice, a copy of this license and the following disclaimer
   in the documentation or with other materials provided with the
   distribution.

3) Modified copies of the source code must be clearly marked as such,
   and must not be misrepresented as verbatim copies of the source code.

THE COPYRIGHT HOLDERS AND/OR OTHER PARTIES PROVIDE THE SOFTWARE "AS IS"
WITHOUT EXPRESSED OR IMPLIED WARRANTY INCLUDING, BUT NOT LIMITED TO,
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
PURPOSE.  IN NO EVENT SHALL ANY COPYRIGHT HOLDER OR OTHER PARTY WHO MAY
ODIFY AND/OR REDISTRIBUTE THE SOFTWARE UNDER THE TERMS OF THIS LICENSE
BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, LOSS OF DATA OR DATA BECOMING INACCURATE
OR LOSS OF PROFIT OR BUSINESS INTERRUPTION) ARISING IN ANY WAY OUT OF
THE USE OR INABILITY TO USE THE SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGES.

=========================================================================*/

#ifndef __vtkNDITracker_h
#define __vtkNDITracker_h

#include "vtkPlusDevice.h"
#include "ndicapi.h"

class vtkSocketCommunicator;

// the number of tools the polaris can handle
#define VTK_NDI_NTOOLS 12
#define VTK_NDI_REPLY_LEN 2048
#define RETURN_VALUE_LEN 9

/*!
  \class vtkNDITracker
  \brief Interface class for Northern Digital's tracking devices

  The vtkNDITracker class provides an  interface to the AURORA and POLARIS
  (Northern Digital Inc., Waterloo, Canada) using the new "combined API" and
  should also support all newer NDI tracking devices.  Any POLARIS systems
  purchased before 2002 will not support the combined API, and should be
  used with vtkPOLARISTracker instead.

  This class refers to ports 1,2,3,A,B,C as ports 0,1,2,3,4,5

  \sa vtkPOLARISTracker
  \ingroup PlusLibDataCollection
*/
class VTK_EXPORT vtkNDITracker : public vtkPlusDevice
{
public:

  static vtkNDITracker *New();
  vtkTypeMacro(vtkNDITracker,vtkPlusDevice);
  void PrintSelf(ostream& os, vtkIndent indent);
 
  virtual bool IsTracker() const { return true; }

  /*! Hardware device SDK version. */
  virtual std::string GetSdkVersion();

  /*!
    Probe to see if the tracking system is present on the
    specified serial port.  If the SerialPort is set to -1,
    then all serial ports will be checked.
  */
  PlusStatus Probe();

  /*!
    Send a command to the NDI in the format INIT: or VER:0 (the
    command should include a colon).  Commands can only be done after
    either Probe() or StartTracking() has been called.
    The text reply from the NDI is returned, without the CRC or
    final carriage return.
  */
  char *Command(const char *command);

  /*!
    Get the a string (perhaps a long one) describing the type and version
    of the device.
  */
  vtkGetStringMacro(Version);

  /*! Set which serial port to use, 1 through 4 */
  vtkSetMacro(SerialPort, int);
  vtkGetMacro(SerialPort, int);

  /*!
    Set which serial device to use.  If present, this overrides
    the SerialPort number.
  */
  vtkSetStringMacro(SerialDevice);
  vtkGetStringMacro(SerialDevice);

  /*! Set the desired baud rate.  Default: 9600. */
  vtkSetMacro(BaudRate, int);
  vtkGetMacro(BaudRate, int);

  /*!
    Enable a passive tool by uploading a virtual SROM for that
    tool, where 'tool' is a number between 0 and 5.
  */
  PlusStatus LoadVirtualSROM(int tool, const char *filename);
  /*! Disable a passive tool, where 'tool' is a number between 0 and 5 */
  void ClearVirtualSROM(int tool);

  /*!
    Get an update from the tracking system and push the new transforms
    to the tools.  This should only be used within vtkTracker.cxx.
  */
  PlusStatus InternalUpdate();
  
  /*! Internal use only */
  virtual PlusStatus InternalInterpretCommand( char * c);

  /*! Get the full TX reply for a tool */
  int GetFullTX(int tool, double transform[9]);
  /*! Get the full TX reply for a tool */
  double *GetFullTX(int tool) { this->ReturnValue[0] = (double) this->GetFullTX(tool, &this->ReturnValue[1]); return this->ReturnValue; };

  /*! Read NDI tracker configuration from xml data */
  virtual PlusStatus ReadConfiguration(vtkXMLDataElement* config); 

  /*! Read NDI tracker configuration from xml data */
  virtual PlusStatus WriteConfiguration(vtkXMLDataElement* config); 

protected:
  vtkNDITracker();
  ~vtkNDITracker();

  /*! Set the version information */
  vtkSetStringMacro(Version);

  /*!
    Start the tracking system.  The tracking system is brought from
    its ground state into full tracking mode.  The device will
    only be reset if communication cannot be established without
    a reset.
  */
  PlusStatus InternalStartRecording();

  /*!
    Stop the tracking system and bring it back to its ground state:
    Initialized, not tracking, at 9600 Baud.
  */
  PlusStatus InternalStopRecording();

  /*! Cause the device to beep the specified number of times */
  PlusStatus InternalBeep(int n);

  /*! Set the specified tool LED to the specified state */
  PlusStatus InternalSetToolLED(int tool, int led, int state);

  /*!
    This is a low-level method for loading a virtual SROM.
    You must halt the tracking thread and take the device
    out of tracking mode before you use it.
  */
  PlusStatus InternalLoadVirtualSROM(int tool, const unsigned char data[1024]);
  /*!
    This is a low-level method for loading a virtual SROM.
    You must halt the tracking thread and take the device
    out of tracking mode before you use it.
  */
  PlusStatus InternalClearVirtualSROM(int tool);

  /*!
    Methods for detecting which ports have tools in them, and
    auto-enabling those tools.
  */
  void EnableToolPorts();
  /*!
    Methods for detecting which ports have tools in them, and
    auto-enabling those tools.
  */
  void DisableToolPorts();

  /*! Find the tool for a specific port handle (-1 if not found) */
  int GetToolFromHandle(int handle);

  /*! Requested frequency of position updates in Hz (1/sec) */
  double UpdateNominalFrequency;

  /*! Index of the last frame number. This is used for providing a frame number when the tracker doesn't return any transform */
  unsigned long LastFrameNumber;

  ndicapi *Device;
  char *Version;
  char *SerialDevice;

  vtkMatrix4x4 *SendMatrix;
  int SerialPort; 
  int BaudRate;
  int IsDeviceTracking;

  int PortEnabled[VTK_NDI_NTOOLS];
  int PortHandle[VTK_NDI_NTOOLS];
  unsigned char *VirtualSROM[VTK_NDI_NTOOLS];

  char CommandReply[VTK_NDI_REPLY_LEN];

  double ReturnValue[RETURN_VALUE_LEN];

  int ServerMode;
  char* RemoteAddress;
  vtkSocketCommunicator* SocketCommunicator;

private:
  vtkNDITracker(const vtkNDITracker&);
  void operator=(const vtkNDITracker&);  
};

#endif
