/*=========================================================================

Module:  $RCSfile: vtkInstrumentTracker.h $
Authors: Jan Gumprecht, Haiying Liu, Nobuhiko Hata, Harvard Medical School
Authors: Danielle Pace,
         Robarts Research Institute and The University of Western Ontario

Copyright (c) 2009, Brigham and Women's Hospital, Boston, MA
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:

 * Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.

 * Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in
   the documentation and/or other materials provided with the
   distribution.

 * Neither the name of Harvard Medical School nor the names of any
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

// .NAME vtkInstrumentTracker - send image to Slicer3
// .SECTION Description
// vtkInstrumentTracker is responsible for the creation and configuration
// of socket connection with Slicer3, and as requested, sending images via OpenIGTLink.
// .SECTION Usage
// See 4DUltrasound.cxx


#ifndef VTKINSTRUMENTTRACKER_H_
#define VTKINSTRUMENTTRACKER_H_

#include "vtkPlusWin32Header.h"

#include "vtkObject.h"

#include "igtlClientSocket.h"
#include "igtlTransformMessage.h"

class vtkTaggedImageFilter;
class vtkMultiThreader;
class vtkMatrix4x4;
class vtkTrackerTool;
class vtkUltrasoundCalibFileReader;
class vtkTracker;

class vtkInstrumentTracker : public vtkObject
{
public:
  static vtkInstrumentTracker *New();
  vtkTypeRevisionMacro(vtkInstrumentTracker, vtkObject);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Tool to poll from
  vtkSetMacro(TrackerTool, vtkTrackerTool*);
  vtkGetMacro(TrackerTool, vtkTrackerTool*);

  // Description:
  // OpenIGTLink server IP
  vtkSetStringMacro(OIGTLServer);
  vtkGetStringMacro(OIGTLServer);

  // Description:
  // OpenIGTLink server port
  vtkSetMacro(ServerPort, int);
  vtkGetMacro(ServerPort, int);

  // Description:
  // Rate for sending transform matricies over OpenIGTLink
  vtkSetMacro(TrackingRate, double);
  vtkGetMacro(TrackingRate, double);

  // Description:
  // Are we successfully tracking?
  vtkGetMacro(Tracking, bool);

  // Description:
  // Timestamp when we started the data sender
  vtkSetMacro(StartUpTime, double);
  vtkGetMacro(StartUpTime, double);

  // Description:
  // Get the number of seconds we've been operating for
  double GetUpTime();

  // Description:
  // File used for logging
  void SetLogStream(ofstream &LogStream);
  ofstream& GetLogStream();

  // Description:
  // For verbose output
  vtkSetMacro(Verbose, bool);
  vtkGetMacro(Verbose, bool);

  // Description:
  // Open connection to the server
  int ConnectToServer();
  
  // Description:
  // Starting sending data to the server
  int StartTracking(vtkTracker* tracker, int port);

  // Description:
  // Stop sending data to the server
  int StopTracking();

  // Description:
  // Close the connection to the server
  int CloseServerConnection();

  //----------------------------
  // The following need to be public for the instrument tracker thread
  // Please don't call them directly!

  // Description:
  // Sends the (already prepared) OpenIGTLink message
  int SendMessage(igtl::TransformMessage::Pointer& message);

protected:
  vtkInstrumentTracker();
  ~vtkInstrumentTracker();

  // OpenIGTLink connection
  int ServerPort;
  char *OIGTLServer;
  igtl::ClientSocket::Pointer Socket;

  // rate for data transfer
  double TrackingRate;

  // the tracker tool to poll from
  vtkTrackerTool *TrackerTool;

  // current status
  bool Connected;
  bool Tracking;
  double StartUpTime;

  //Multithreader to run a thread of collecting and sending data
  vtkMultiThreader *PlayerThreader;
  int PlayerThreadId;

  // logging
  bool Verbose;
  ofstream LogStream;

};

#endif /* VTKINSTRUMENTTRACKER_H_ */
