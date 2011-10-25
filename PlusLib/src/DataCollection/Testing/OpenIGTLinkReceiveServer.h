/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

#ifndef __OpenIGTLinkReceiveServer_h
#define __OpenIGTLinkReceiveServer_h


#include "PlusConfigure.h"

#include "vtkMultiThreader.h"

#include "igtlMessageHeader.h"
#include "igtlServerSocket.h"



class OpenIGTLinkReceiveServer
{

public:
  
  OpenIGTLinkReceiveServer( int port );
  ~OpenIGTLinkReceiveServer();
  
  PlusStatus Start();
  PlusStatus Stop();
  
  bool GetActive();
  int  GetNumberOfReceivedMessages();
  
  igtl::ServerSocket::Pointer GetServerSocket();
  
  friend static void* vtkReceiverThread( vtkMultiThreader::ThreadInfo *data );
  
private:
  
  int ReceiveTransform( igtl::Socket * socket, igtl::MessageHeader * header );
  int ReceiveImage( igtl::Socket * socket, igtl::MessageHeader * header );
  
  igtl::ServerSocket::Pointer ServerSocket;
  
  vtkMultiThreader* Threader;
  
  bool IsActive;
  int  ThreadId;
  int  NumberOfReceivedMessages;

};

#endif
