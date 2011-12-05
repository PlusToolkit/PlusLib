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

#ifndef __vtkPOLARISTracker_h
#define __vtkPOLARISTracker_h

#include "vtkTracker.h"
#include "polaris.h"

// the number of tools the polaris can handle
#define VTK_POLARIS_NTOOLS 12
#define VTK_POLARIS_REPLY_LEN 512

/*!
  \class vtkPOLARISTracker
  \brief Interface class for Northern Digital's POLARIS tracking devices

  The vtkPOLARISTracker class provides an  interface to the POLARIS
  (Northern Digital Inc., Waterloo, Canada) optical tracking system.

  The vtkNDITracker class provides an  interface to the AURORA and POLARIS
  (Northern Digital Inc., Waterloo, Canada) using the new "combined API" and
  should also support all newer NDI tracking devices.  Any POLARIS systems
  purchased before 2002 will not support the combined API, and should be
  used with vtkPOLARISTracker instead.

  This class refers to ports 1,2,3,A,B,C as ports 0,1,2,3,4,5

  \sa vtkNDITracker
  \ingroup PlusLibTracking
*/
class VTK_EXPORT vtkPOLARISTracker : public vtkTracker
{
public:

  static vtkPOLARISTracker *New();
  vtkTypeMacro(vtkPOLARISTracker,vtkTracker);
  void PrintSelf(ostream& os, vtkIndent indent);
 
  /*!
    Probe to see if the tracking system is present on the
    specified serial port.  If the SerialPort is set to -1,
    then all serial ports will be checked.
  */
  PlusStatus Probe();

  /*!
    Send a command to the POLARIS in the format INIT: or VER:0 (the
    command should include a colon).  Commands can only be done after
    either Probe() or StartTracking() has been called.
    The text reply from the POLARIS is returned, without the CRC or
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
  /*! Get which serial port to use, 1 through 4 */
  vtkGetMacro(SerialPort, int);

  /*! Set which serial device to use. This overrides the SerialPort number. */
  vtkSetStringMacro(SerialDevice);
  /*! Get which serial device to use */
  vtkGetStringMacro(SerialDevice);

  /*! Set the desired baud rate. Default: 115200. If CRC errors are common, reduce to 57600. */
  vtkSetMacro(BaudRate, int);
  /*! Get the desired baud rate */
  vtkGetMacro(BaudRate, int);

  /*!
    Enable a passive tool by uploading a virtual SROM for that
    tool, where 'tool' is a number between 0 and 5.
  */
  PlusStatus LoadVirtualSROM(int tool, const char *filename);
  /*! Disable a passive tool, where 'tool' is a number between 0 and 5 */
  PlusStatus ClearVirtualSROM(int tool);

  /*!
    Get an update from the tracking system and push the new transforms
    to the tools.  This should only be used within vtkTracker.cxx.
  */
  PlusStatus InternalUpdate();

protected:
  vtkPOLARISTracker();
  ~vtkPOLARISTracker();

  /*! Set the version information */
  vtkSetStringMacro(Version);

  /*!
    Start the tracking system.  The tracking system is brought from
    its ground state into full tracking mode.  The POLARIS will
    only be reset if communication cannot be established without
    a reset.
  */
  PlusStatus InternalStartTracking();

  /*!
    Stop the tracking system and bring it back to its ground state:
    Initialized, not tracking, at 9600 Baud.
  */
  PlusStatus InternalStopTracking();

  /*! Cause the POLARIS to beep the specified number of times */
  PlusStatus InternalBeep(int n);

  /*! Set the specified tool LED to the specified state */
  PlusStatus InternalSetToolLED(int tool, int led, int state);

  /*!
    This is a low-level method for loading a virtual SROM.
    You must halt the tracking thread and take the POLARIS
    out of tracking mode before you use it.
  */
  PlusStatus InternalLoadVirtualSROM(int tool, const unsigned char data[1024]);
  /*!
    This is a low-level method for loading a virtual SROM.
    You must halt the tracking thread and take the POLARIS
    out of tracking mode before you use it.
  */
  PlusStatus InternalClearVirtualSROM(int tool);

  /*!
    Methods for detecting which ports have tools in them, and
    auto-enabling those tools.
  */
  PlusStatus EnableToolPorts();
  /*!
    Methods for detecting which ports have tools in them, and
    auto-enabling those tools.
  */
  PlusStatus DisableToolPorts();

  /*! Requested frequency of position updates in Hz (1/sec) */
  double UpdateNominalFrequency;

  /*! Index of the last frame number. This is used for providing a frame number when the tracker doesn't return any transform */
  double LastFrameNumber;

  /*! Deprecated. Computing transforms relative to a reference tracker are now handled at the data collection level. */
  int ReferenceTool;

  polaris *Polaris;
  char *Version;
  char *SerialDevice;

  vtkMatrix4x4 *SendMatrix;
  int SerialPort; 
  int BaudRate;
  int IsPOLARISTracking;

  int PortEnabled[VTK_POLARIS_NTOOLS];
  unsigned char *VirtualSROM[VTK_POLARIS_NTOOLS];

  char CommandReply[VTK_POLARIS_REPLY_LEN];

private:
  vtkPOLARISTracker(const vtkPOLARISTracker&);
  void operator=(const vtkPOLARISTracker&);  
};

#endif
