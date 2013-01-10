/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

/*!
  \file BrainLabTrackerSim.cxx
  \brief This program simulates a BrainLab tracker with OpenIGTLink interface.

  The connection establishing procedure is not simulated accurately, as BrainLab
  requires user interaction to accept an incoming connection, while this simulator accepts
  connections automatically.

  The simulator sends information about the ReferenceStar and Ultrasound tools in the first 
  couple of seconds (to simulate the situation when a tool is not available at connection time),
  and later transforms are sent for an additional Pointer tool as well.
*/  

#include <iostream>
#include <math.h>
#include <cstdlib>
#include <cstring>

#include "igtlOSUtil.h"
#include "igtlMessageHeader.h"
#include "igtlImageMessage.h"
#include "igtlServerSocket.h"
#include "igtlTrackingDataMessage.h"
#include "igtlMultiThreader.h"


typedef struct 
{
  int   nloop;
  igtl::MutexLock::Pointer glock;
  igtl::Socket::Pointer socket;
  int   interval;
  int   stop;
} ThreadData;

//----------------------------------------------------------------------------
// Function to generate random matrix.
void GetRandomTestMatrix(igtl::Matrix4x4& matrix, float phi, float theta)
{
  float position[3];
  position[0] = 50.0 * cos(phi);
  position[1] = 50.0 * sin(phi);
  position[2] = 50.0 * cos(phi);

  float orientation[4];
  orientation[0]=0.0;
  orientation[1]=0.6666666666*cos(theta);
  orientation[2]=0.577350269189626;
  orientation[3]=0.6666666666*sin(theta);

  //igtl::Matrix4x4 matrix;
  igtl::QuaternionToMatrix(orientation, matrix);

  matrix[0][3] = position[0];
  matrix[1][3] = position[1];
  matrix[2][3] = position[2];

  //igtl::PrintMatrix(matrix);
}

//----------------------------------------------------------------------------
void SendTrackingData(igtl::Socket::Pointer& socket, igtl::TrackingDataMessage::Pointer& trackingMsg)
{
  static float phi0   = 0.0;
  static float theta0 = 0.0;
 
  // Update the tracking data with new transformation matrices
  for (int trackingDataElementIndex=0; trackingDataElementIndex<trackingMsg->GetNumberOfTrackingDataElements(); trackingDataElementIndex++) 
  {    
    igtl::TrackingDataElement::Pointer ptr=NULL;
    trackingMsg->GetTrackingDataElement(trackingDataElementIndex, ptr);
    igtl::Matrix4x4 matrix;
    GetRandomTestMatrix(matrix, phi0, theta0);
    ptr->SetMatrix(matrix);
  }

  trackingMsg->Pack();
  socket->Send(trackingMsg->GetPackPointer(), trackingMsg->GetPackSize());

  phi0 += 0.1;
  theta0 += 0.2;
}

//----------------------------------------------------------------------------
void* ThreadFunction(void* ptr)
{
  // Get thread information
  igtl::MultiThreader::ThreadInfo* info = static_cast<igtl::MultiThreader::ThreadInfo*>(ptr);
  int id      = info->ThreadID;
  std::cerr << "Thread #" << id << ": start" << std::endl;
  
  // Get user data
  ThreadData* td = static_cast<ThreadData*>(info->UserData);
  igtl::MutexLock::Pointer glock = td->glock;
  long interval = td->interval;
  std::cerr << "Interval = " << interval << " (ms)" << std::endl;

  igtl::Socket::Pointer& socket = td->socket;

  // Allocate TrackingData message class
  // Reuse them in the loop to avoid reallocation in each message transfer
  igtl::TrackingDataMessage::Pointer trackingMsg;
  trackingMsg = igtl::TrackingDataMessage::New();
  trackingMsg->SetDeviceName("Tracker");
  igtl::TrackingDataElement::Pointer trackElement0;
  trackElement0 = igtl::TrackingDataElement::New();
  trackElement0->SetName("ReferenceStar");
  trackElement0->SetType(igtl::TrackingDataElement::TYPE_TRACKER);
  igtl::TrackingDataElement::Pointer trackElement1;
  trackElement1 = igtl::TrackingDataElement::New();
  trackElement1->SetName("Ultrasound");
  trackElement1->SetType(igtl::TrackingDataElement::TYPE_6D);
  igtl::TrackingDataElement::Pointer trackElement2;
  trackElement2 = igtl::TrackingDataElement::New();
  trackElement2->SetName("Pointer");
  trackElement2->SetType(igtl::TrackingDataElement::TYPE_5D);

  // Start the transmission by sending only 2 tool data
  glock->Lock();
  std::cout << "Sending tool data: ReferenceStar, Ultrasound..." << std::endl;
  glock->Unlock();
  trackingMsg->AddTrackingDataElement(trackElement0);
  trackingMsg->AddTrackingDataElement(trackElement1);
  for (int i=0; i<500; i++)
  {
    glock->Lock();
    SendTrackingData(socket, trackingMsg);
    glock->Unlock();
    igtl::Sleep(interval);
  }

  // Send all tool data from now on (until stop tracking message is received)
  glock->Lock();
  std::cout << "Sending tool data: ReferenceStar, Ultrasound, Pointer..." << std::endl;
  glock->Unlock();
  trackingMsg->AddTrackingDataElement(trackElement2);  
  while (!td->stop)
  {
    glock->Lock();
    SendTrackingData(socket, trackingMsg);
    glock->Unlock();
    igtl::Sleep(interval);
  }

  glock->Lock();
  std::cerr << "Thread #" << id << ": end" << std::endl;
  glock->Unlock();

  return NULL;
}

//----------------------------------------------------------------------------
int main(int argc, char* argv[])
{  
  int port = 22222;
  
  if (argc==2)
  {
    port=atoi(argv[1]);
  }

  if (argc > 2 || port==0)
  {
    // There is a problem with the command-line artguments, print usage
    std::cerr << "Usage: " << argv[0] << " <port>"    << std::endl;
    std::cerr << "    <port>     : Server port number (default: 22222)"   << std::endl;
    exit(0);
  }

  std::cout << "Started BrainLab tracker simulator server at port " << port << std::endl;

  igtl::ServerSocket::Pointer serverSocket;
  serverSocket = igtl::ServerSocket::New();
  int r = serverSocket->CreateServer(port);
  if (r < 0)
  {
    std::cerr << "Cannot create a server socket." << std::endl;
    exit(0);
  }

  igtl::MultiThreader::Pointer threader = igtl::MultiThreader::New();
  igtl::MutexLock::Pointer glock = igtl::MutexLock::New();
  ThreadData td;

  std::cout << "Waiting for connections..." << std::endl;

  while (true)
  {
    // Waiting for Connection
    int threadID = -1;
    igtl::Socket::Pointer socket;
    socket = serverSocket->WaitForConnection(1000);

    if (socket.IsNotNull()) // if client connected
    {
      std::cerr << "A client is connected." << std::endl;

      // Create a message buffer to receive header
      igtl::MessageHeader::Pointer headerMsg;
      headerMsg = igtl::MessageHeader::New();
      while (true)
      {
        // Initialize receive buffer
        headerMsg->InitPack();

        // Receive generic header from the socket
        int rs = socket->Receive(headerMsg->GetPackPointer(), headerMsg->GetPackSize());
        if (rs == 0)
        {
          if (threadID >= 0)
          {
            td.stop = 1;
            threader->TerminateThread(threadID);
            threadID = -1;
          }
          std::cerr << "Disconnecting the client." << std::endl;
          td.socket = NULL;  // VERY IMPORTANT. Completely remove the instance.
          socket->CloseSocket();
          break;
        }
        if (rs != headerMsg->GetPackSize())
        {
          continue;
        }

        // Deserialize the header
        headerMsg->Unpack();

        // Check data type and receive data body
        if (strcmp(headerMsg->GetDeviceType(), "STT_TDATA") == 0)
        {
          std::cerr << "Received a STT_TDATA message." << std::endl;

          igtl::StartTrackingDataMessage::Pointer startTracking;
          startTracking = igtl::StartTrackingDataMessage::New();
          startTracking->SetMessageHeader(headerMsg);
          startTracking->AllocatePack();

          int r2 = socket->Receive(startTracking->GetPackBodyPointer(), startTracking->GetPackBodySize());
          int c = startTracking->Unpack(1);
          if (c & igtl::MessageHeader::UNPACK_BODY) // if CRC check is OK
          {
            td.interval = startTracking->GetResolution();
            td.glock    = glock;
            td.socket   = socket;
            td.stop     = 0;
            threadID    = threader->SpawnThread((igtl::ThreadFunctionType) &ThreadFunction, &td);
          }
        }
        else if (strcmp(headerMsg->GetDeviceType(), "STP_TDATA") == 0)
        {
          socket->Skip(headerMsg->GetBodySizeToRead(), 0);
          std::cerr << "Received a STP_TDATA message." << std::endl;
          if (threadID >= 0)
          {
            td.stop  = 1;
            threader->TerminateThread(threadID);
            threadID = -1;
            std::cerr << "Disconnecting the client." << std::endl;
            td.socket = NULL;  // VERY IMPORTANT. Completely remove the instance.
            socket->CloseSocket();
          }
          break;
        }
        else
        {
          std::cerr << "Receiving : " << headerMsg->GetDeviceType() << std::endl;
          socket->Skip(headerMsg->GetBodySizeToRead(), 0);
        }
      }
    }
  }

  // Close connection (the example code actually never reaches to this section ...)
  serverSocket->CloseSocket();
}



