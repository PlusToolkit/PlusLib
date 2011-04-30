/*=========================================================================

Module:  $RCSfile: vtkDataSender.h $
Authors: Jan Gumprecht, Haiying Liu, Nobuhiko Hata, Harvard Medical School
Authors: Danielle Pace,
         Robarts Research Institute and The University of Western Ontario

Copyright (c) 2008, Brigham and Women's Hospital, Boston, MA
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

// .NAME vtkDataSender - send image to Slicer3
// .SECTION Description
// vtkDataSender is responsible for the creation and configuration
// of socket connection with Slicer3, and as requested, sending images via OpenIGTLink.
// .SECTION Usage
// See SynchroGrab.cxx

#ifndef __vtkDataSender_h
#define __vtkDataSender_h

#include <queue>
#include <map>

#include "vtkPlusWin32Header.h"

#include "vtkObject.h"
#include "vtkImageData.h"
#include "igtlImageMessage.h"
#include "igtlClientSocket.h"

class vtkMultiThreader;
class vtkMatrix4x4;
class vtkMutexLock;
class igtlImageMessage;
class igtlOSUtil;

struct FrameProperties {
        bool Set;
        int ScalarType;
        int Size[3];
        double Spacing[3];
        int SubVolumeSize[3];
        int SubVolumeOffset[3];
        float Origin[3];
};

struct SenderDataStruct{
  vtkImageData* ImageData;
  vtkMatrix4x4* Matrix;
  int Phase;
  int SenderLock;
  int ProcessorLock;
};

struct StatisticsStruct{
  int fpsCounter;
  double meanFPS;
  double maxFPS;
  double minFPS;
  int volumeCounter;
  //int meanVolumeSize[3];
  //int maxVolumeSize[3];
  //int minVolumeSize[3];
};

class vtkDataSender : public vtkObject
{
public:
  static vtkDataSender *New();
  vtkTypeRevisionMacro(vtkDataSender, vtkObject);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // OpenIGTLink server IP
  vtkSetStringMacro(OIGTLServer);
  vtkGetStringMacro(OIGTLServer);

  // DescriptionL
  // OpenIGTLink server port
  vtkSetMacro(ServerPort, int);
  vtkGetMacro(ServerPort, int);

  // Description:
  // Time between sending subsequent image messages
  vtkSetMacro(SendPeriod, double);
  vtkGetMacro(SendPeriod, double);

  // Description:
  // Size of the buffer storing images to be sent over OpenIGTLink connection
  vtkGetMacro(SendDataBufferSize, int);

  // Description:
  // Have we successfully connected to the server?
  vtkGetMacro(Connected, bool);

  // Description:
  // Have we started sending image data?
  vtkGetMacro(Sending, bool);

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
  // Start sending data to the server
  int StartSending();

  // Description:
  // Stop sending data to the server
  int StopSending();

  // Description:
  // Close the connection to the server
  int CloseServerConnection();

  // Description:
  // Current number of items within the buffer
  int GetCurrentBufferSize(){return SendDataQueue.size();}

  // Description:
  // Get the index at the front of the queue
  int GetHeadOfNewDataBuffer();

  // Description:
  // Is the buffer empty?
  bool IsSendDataBufferEmpty();

  // Description:
  // Is the buffer full?
  bool IsSendDataBufferFull();

  // Description:
  // Get the number of images that we've sent since we started
  vtkGetMacro(NumberSent, int);

  // Description:
  // Add new data to the buffer
  int NewData(vtkImageData* frame, vtkMatrix4x4* trackerMatrix, int phase);

  //----------------------------
  // The following need to be public for the data sender thread
  // Please don't call them directly!

  // Description:
  // Sets up the OpenIGTLink message for the data stored at the specified index
  // in the buffer
  int PrepareImageMessage(int index, igtl::ImageMessage::Pointer& imageMessage);

  // Description:
  // Actually sends the OpenIGTLink message
  int SendMessage(igtl::ImageMessage::Pointer& message);

  // Description:
  // Removes the data from the buffer at the specified index, if it is not locked
  int TryToDeleteData(int index);

  // Description:
  // Removes all of the data from the buffer
  void DeleteAllData();

  // Description:
  // Lock/Unlock for synchronization
  int LockIndex(int index, int requester);
  int UnlockData(int index, int lock);
  int ReleaseLock(int requester);

protected:
  vtkDataSender();
  ~vtkDataSender();

  // OpenIGTLink connection
  int ServerPort;
  char *OIGTLServer;
  igtl::ClientSocket::Pointer Socket;

  // rate for data transfer
  double SendPeriod;

  // buffer containing images to be sent
  std::queue<int> SendDataQueue; // Stores index of incoming objects
  int SendDataBufferSize; // Maximum number of items that can be stored at the same time
  int SendDataBufferIndex; // Object which is currently/ was last processed
  std::map<int, struct SenderDataStruct> SendDatabuffer;

  // current status
  bool Sending;
  bool Connected;
  double StartUpTime;
  double LastFrameRateUpdate;
  int UpdateCounter;
  int NumberSent;

  //Multithreader to run a thread of collecting and sending data
  vtkMultiThreader *PlayerThreader;
  int PlayerThreadId;
  vtkMutexLock *IndexLock;
  int IndexLockedByDataSender;
  int IndexLockedByDataProcessor;

  // structs
  struct FrameProperties FrameProperties;
  struct StatisticsStruct Statistics;

  // logging
  bool Verbose;
  ofstream LogStream;

  // Description:
  // Add the image data to the buffer at the specified index
  int AddDatatoBuffer(int index, vtkImageData* imageData, vtkMatrix4x4* matrix, int phase);

  // Description:
  // Check to see if the buffer already has data at the specified index
  bool IsIndexAvailable(int index);

  // Description:
  // Calculates and prints current frame rate to console
  void UpdateFrameRate(double sendTime);

  // Description:
  // Increment current index of the buffer
  int IncrementBufferIndex(int increment);

 private:
  vtkDataSender(const vtkDataSender&);  // Not implemented.
  void operator=(const vtkDataSender&);  // Not implemented.
};
#endif
