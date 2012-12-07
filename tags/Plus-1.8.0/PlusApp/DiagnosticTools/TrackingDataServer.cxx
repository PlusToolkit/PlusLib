/*=========================================================================

  Program:   Open IGT Link -- Example for Tracking Data Server
  Module:    $RCSfile: $
  Language:  C++
  Date:      $Date: $
  Version:   $Revision: $

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/


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


void* ThreadFunction(void* ptr);
int   SendTrackingData(igtl::Socket::Pointer& socket, igtl::TrackingDataMessage::Pointer& trackingMsg);
void  GetRandomTestMatrix(igtl::Matrix4x4& matrix, float phi, float theta);

typedef struct {
  int   nloop;
  igtl::MutexLock::Pointer glock;
  igtl::Socket::Pointer socket;
  int   interval;
  int   stop;
} ThreadData;


int main(int argc, char* argv[])
{

  //------------------------------------------------------------
  // Parse Arguments

  if (argc != 2) // check number of arguments
    {
    // If not correct, print usage
    std::cerr << "Usage: " << argv[0] << " <port>"    << std::endl;
    std::cerr << "    <port>     : Port # (18944 in Slicer default)"   << std::endl;
    exit(0);
    }

  int    port     = atoi(argv[1]);

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

  while (1)
    {
    //------------------------------------------------------------
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
      //------------------------------------------------------------
      // loop
      for (;;)
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
    
  //------------------------------------------------------------
  // Close connection (The example code never reaches to this section ...)
  serverSocket->CloseSocket();

}


void* ThreadFunction(void* ptr)
{
  //------------------------------------------------------------
  // Get thread information
  igtl::MultiThreader::ThreadInfo* info = 
    static_cast<igtl::MultiThreader::ThreadInfo*>(ptr);

  //int id      = info->ThreadID;
  //int nThread = info->NumberOfThreads;
  ThreadData* td = static_cast<ThreadData*>(info->UserData);

  //------------------------------------------------------------
  // Get user data
  igtl::MutexLock::Pointer glock = td->glock;
  long interval = td->interval;
  std::cerr << "Interval = " << interval << " (ms)" << std::endl;
  //long interval = 1000;
  //long interval = (id + 1) * 100; // (ms)

  igtl::Socket::Pointer& socket = td->socket;

  //------------------------------------------------------------
  // Allocate TrackingData Message Class
  //
  // NOTE: TrackingDataElement class instances are allocated
  //       before the loop starts to avoid reallocation
  //       in each image transfer.

  igtl::TrackingDataMessage::Pointer trackingMsg;
  trackingMsg = igtl::TrackingDataMessage::New();
  trackingMsg->SetDeviceName("Tracker");

  igtl::TrackingDataElement::Pointer trackElement0;
  trackElement0 = igtl::TrackingDataElement::New();
  trackElement0->SetName("Probe");
  trackElement0->SetType(igtl::TrackingDataElement::TYPE_6D);

  igtl::TrackingDataElement::Pointer trackElement1;
  trackElement1 = igtl::TrackingDataElement::New();
  trackElement1->SetName("Reference");
  trackElement1->SetType(igtl::TrackingDataElement::TYPE_6D);

  igtl::TrackingDataElement::Pointer trackElement2;
  trackElement2 = igtl::TrackingDataElement::New();
  trackElement2->SetName("Stylus");
  trackElement2->SetType(igtl::TrackingDataElement::TYPE_6D);

  trackingMsg->AddTrackingDataElement(trackElement0);
  trackingMsg->AddTrackingDataElement(trackElement1);
  trackingMsg->AddTrackingDataElement(trackElement2);

  //------------------------------------------------------------
  // Loop
  while (!td->stop)
    {
    glock->Lock();
    SendTrackingData(socket, trackingMsg);
    glock->Unlock();
    igtl::Sleep(interval);
    }

  //glock->Lock();
  //std::cerr << "Thread #" << id << ": end." << std::endl;
  //glock->Unlock();
  
  return NULL;
}


int SendTrackingData(igtl::Socket::Pointer& socket, igtl::TrackingDataMessage::Pointer& trackingMsg)
{

  static float phi0   = 0.0;
  static float theta0 = 0.0;
  static float phi1   = 1.1;
  static float theta1 = 1.1;
  static float phi2   = 2.5;
  static float theta2 = 2.5;

  igtl::Matrix4x4 matrix;
  igtl::TrackingDataElement::Pointer ptr;

  // Channel 0
  trackingMsg->GetTrackingDataElement(0, ptr);
  GetRandomTestMatrix(matrix, phi0, theta0);
  ptr->SetMatrix(matrix);
  
  // Channel 1
  trackingMsg->GetTrackingDataElement(1, ptr);
  GetRandomTestMatrix(matrix, phi1, theta1);
  ptr->SetMatrix(matrix);
  
  // Channel 2
  trackingMsg->GetTrackingDataElement(2, ptr);
  GetRandomTestMatrix(matrix, phi2, theta2);
  ptr->SetMatrix(matrix);

  trackingMsg->Pack();
  socket->Send(trackingMsg->GetPackPointer(), trackingMsg->GetPackSize());
  
  phi0 += 0.1;
  phi1 += 0.2;
  phi2 += 0.3;
  theta0 += 0.2;
  theta1 += 0.1;
  theta2 += 0.05;

  return 0;
}



//------------------------------------------------------------
// Function to generate random matrix.
void GetRandomTestMatrix(igtl::Matrix4x4& matrix, float phi, float theta)
{
  float position[3];
  float orientation[4];

  // random position
  position[0] = 50.0 * cos(phi);
  position[1] = 50.0 * sin(phi);
  position[2] = 50.0 * cos(phi);
  phi = phi + 0.2;

  // random orientation
  orientation[0]=0.0;
  orientation[1]=0.6666666666*cos(theta);
  orientation[2]=0.577350269189626;
  orientation[3]=0.6666666666*sin(theta);
  theta = theta + 0.1;

  //igtl::Matrix4x4 matrix;
  igtl::QuaternionToMatrix(orientation, matrix);

  matrix[0][3] = position[0];
  matrix[1][3] = position[1];
  matrix[2][3] = position[2];
  
  igtl::PrintMatrix(matrix);
}


