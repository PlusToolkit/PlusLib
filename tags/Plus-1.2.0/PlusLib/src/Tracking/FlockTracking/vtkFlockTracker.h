/*=========================================================================

  Program:   AtamaiTracking for VTK
  Module:    $RCSfile: vtkFlockTracker.h,v $
  Creator:   David Gobbi <dgobbi@atamai.com>
  Language:  C++
  Author:    $Author: dgobbi $
  Date:      $Date: 2002/11/04 02:09:39 $
  Version:   $Revision: 1.1 $

==========================================================================

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
MODIFY AND/OR REDISTRIBUTE THE SOFTWARE UNDER THE TERMS OF THIS LICENSE
BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, LOSS OF DATA OR DATA BECOMING INACCURATE
OR LOSS OF PROFIT OR BUSINESS INTERRUPTION) ARISING IN ANY WAY OUT OF
THE USE OR INABILITY TO USE THE SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGES.

=========================================================================*/
// .NAME vtkFlockTracker - VTK interface to the Flock of Birds
// .SECTION Description
// The vtkFlockTracker is a VTK interface to the the Flock of Birds magnetic
// position-sensor device (Ascension Technology Corporation).
// It requires the Flock C API which is available
// from David Gobbi at the Robarts Research Institute (dgobbi@irus.rri.on.ca).
// If you are running an operating system other that Linux or Win32, you
// might need a special serial cable that has the RTS line cut in order
// to use the flock of birds.
// .SECTION see also
// vtkTrackerTool vtkPOLARISTracker


#ifndef __vtkFlockTracker_h
#define __vtkFlockTracker_h

#include "vtkTracker.h"
#include "vtkTransform.h"

struct fbird;

class VTK_EXPORT vtkFlockTracker : public vtkTracker
{
public:

  static vtkFlockTracker *New();
  vtkTypeMacro(vtkFlockTracker,vtkTracker);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Probe to see if the tracking system is present on the 
  // serial port specified by SetSerialPort.
  int Probe();

  // Description:
  // Set which serial port to use, 1 through 4.  Default: 1.
  vtkSetMacro(SerialPort, int);
  vtkGetMacro(SerialPort, int);

  // Description:
  // Set the desired baud rate.  This baud rate must match the rate
  // specified by the DIP switches on the Bird units. Default: 115200. 
  vtkSetMacro(BaudRate, int);
  vtkGetMacro(BaudRate, int);

  // Description:
  // Get an update from the tracking system and push the new transforms
  // to the tools.  This should only be called from the superclass,
  // i.e. within vtkTracker.cxx.
  void InternalUpdate();

protected:
  vtkFlockTracker();
  ~vtkFlockTracker();

  // Description:
  // Start the tracking system.  The tracking system is brought from
  // its ground state into full tracking mode.  Under Win32 and Linux
  // this will also clear all errors - under other operating systems,
  // you might have to use the 'FLY/STANDBY' switch to clear errors.
  int InternalStartTracking();

  // Description:
  // Stop the tracking system and bring it back to its ground state.
  int InternalStopTracking();

  fbird *Flock;
  int NumberOfBirds;

  vtkMatrix4x4 *SendMatrix;
  int SerialPort;
  int BaudRate;
  int Mode;

private:
  vtkFlockTracker(const vtkFlockTracker&);
  void operator=(const vtkFlockTracker&);  
};

#endif





