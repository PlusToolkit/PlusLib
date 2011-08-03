/*=========================================================================

Module:    $RCSfile: vtkDataSender.cxx,v $
Author:  Jonathan Boisvert, Queens School Of Computing
Authors: Jan Gumprecht, Haiying Liu, Nobuhiko Hata, Harvard Medical School
Authors: Danielle Pace,
         Robarts Research Institute and The University of Western Ontario

Copyright (c) 2008, Queen's University, Kingston, Ontario, Canada
All rights reserved.
Copyright (c) 2008, Brigham and Women's Hospital, Boston, MA

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

#include <iostream>
#include <sstream>
#include <limits>
#include <string>
#include <assert.h>

#undef REMOVE_ALPHA_CHANNEL

#include "vtkDataSetWriter.h"
#include "vtkImageCast.h"
#include "vtkImageExtractComponents.h"
#include "vtkImageData.h"
#include "vtkJPEGWriter.h"
#include "vtkMatrix4x4.h"
#include "vtkMultiThreader.h"
#include "vtkTimerLog.h"
#include "vtkMutexLock.h"
#include "vtkObjectFactory.h"
#include "vtkDataSender.h"
#include "vtkTransform.h"

#include "igtlImageMessage.h"
#include "igtlMath.h"
#include "igtlOSUtil.h"
#include "igtlClientSocket.h"

using namespace std;

vtkCxxRevisionMacro(vtkDataSender, "$Revision$");
vtkStandardNewMacro(vtkDataSender);

//----------------------------------------------------------------------------
vtkDataSender::vtkDataSender()
{
  this->ServerPort = 18944;
  this->OIGTLServer = NULL;
  this->SetOIGTLServer("localhost");

  this->Socket = NULL;
  this->Socket = igtl::ClientSocket::New();

  this->SendPeriod = 1.0;

  this->SendDataBufferSize = 10;
  this->SendDataBufferIndex = -1;

  this->Connected = false;
  this->Sending = false;
  this->StartUpTime = vtkTimerLog::GetUniversalTime();
  this->NumberSent = 0;

  this->LastFrameRateUpdate = 0;
  this->UpdateCounter = 0;

  this->PlayerThreader = vtkMultiThreader::New();;
  this->PlayerThreadId = -1;
  this->IndexLockedByDataSender = -1;
  this->IndexLockedByDataProcessor = -1;
  this->IndexLock = vtkMutexLock::New();

  this->Verbose = false;
  this->LogStream.ostream::rdbuf(cerr.rdbuf());

  FrameProperties.Set = false;
  FrameProperties.SubVolumeOffset[0] = 0;
  FrameProperties.SubVolumeOffset[1] = 0;
  FrameProperties.SubVolumeOffset[2] = 0;
  
  this->Statistics.fpsCounter = 0;
  this->Statistics.meanFPS = 0;
  this->Statistics.maxFPS = -1;
  this->Statistics.minFPS = 30;
  this->Statistics.volumeCounter = 0;
}

//----------------------------------------------------------------------------
vtkDataSender::~vtkDataSender()
{
  this->SetOIGTLServer(NULL);

  if (this->Sending)
    {
    this->StopSending();
    }
  if (this->Connected)
    {
    this->CloseServerConnection();
    }

  this->PlayerThreader->Delete();

  //Delete all buffer objects
  this->DeleteAllData();
  this->IndexLock->Delete(); 
}

//----------------------------------------------------------------------------
void vtkDataSender::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}

//----------------------------------------------------------------------------
int vtkDataSender::ConnectToServer()
{
  // Opening an OpenIGTLink socket
  int e = this->Socket->ConnectToServer(this->GetOIGTLServer(), this->ServerPort);

  if(e != 0)
    {
    cout << "ERROR: Data sender - Failed to connect the OpenIGTLink socket to the server ("<< OIGTLServer <<":"<<  ServerPort << ") Error code : " << e << endl;
    return -1;
    }
  else if(Verbose)
    {
    cout << "Data sender - Successful connected to the OpenIGTLink server ("<< OIGTLServer <<":"<<  ServerPort << ")" << endl;
    }

  this->Connected = true;

  return 0;
}

//----------------------------------------------------------------------------
int vtkDataSender::CloseServerConnection()
{

  this->Socket->CloseSocket();

  if(Verbose && this->Connected)
    {
    cout << "Connection to the OpenIGTLink server ("<< OIGTLServer <<":"<<  ServerPort << ") closed" << endl;
    }

  this->Connected = false;

  return 0;
}

/******************************************************************************
 *  static inline void vtkSleep(double duration)
 *
 *  Platform-independent sleep function
 *  Set the current thread to sleep for a certain amount of time
 *
 *  @Param: double duration - Time to sleep in ms
 *
 * ****************************************************************************/
static inline void vtkSleep(double duration)
{
  duration = duration; // avoid warnings
  // sleep according to OS preference
#ifdef _WIN32
  Sleep((int)(1000*duration));
#elif defined(__FreeBSD__) || defined(__linux__) || defined(sgi) || defined(__APPLE__)
  struct timespec sleep_time, dummy;
  sleep_time.tv_sec = (int)duration;
  sleep_time.tv_nsec = (int)(1000000000*(duration-sleep_time.tv_sec));
  nanosleep(&sleep_time,&dummy);
#endif
}

/******************************************************************************
 *  static int vtkThreadSleep(vtkMultiThreader::ThreadInfo *data, double time, bool checkActiveFlagImmediately)
 *
 *  Sleep until the specified absolute time has arrived.
 *  You must pass a handle to the current thread.
 *  If '0' is returned, then the thread was aborted before or during the wait. *
 *
 *  @Author:Jan Gumprecht
 *  @Date:  22.December 2008
 *
 *  @Param: vtkMultiThreader::ThreadInfo *data
 *  @Param: double time - Time until which to sleep
 *  @Param: bool checkActiveFlagImmediately - avoid waiting if enabled
 *
 * ****************************************************************************/
static int vtkThreadSleep(vtkMultiThreader::ThreadInfo *data, double time, bool checkActiveFlagImmediately)
{
  if(checkActiveFlagImmediately)
    {
    // check to see if we are being told to quit
      data->ActiveFlagLock->Lock();
      int activeFlag = *(data->ActiveFlag);
      data->ActiveFlagLock->Unlock();

      if (activeFlag == 0)
        {//We are told to quit
        return 0;
        }

      //Go on processing
      return 1;
    }

  // loop either until the time has arrived or until the thread is ended
  for (int i = 0;; i++)
    {
      double remaining = time - vtkTimerLog::GetUniversalTime();

      // check to see if we have reached the specified time
      if (remaining <= 0)
        {
        return 1;
      }
      // check the ActiveFlag at least every 0.1 seconds
      if (remaining > 0.1)
        {
        remaining = 0.1;
        }

      // check to see if we are being told to quit
      data->ActiveFlagLock->Lock();
      int activeFlag = *(data->ActiveFlag);
      data->ActiveFlagLock->Unlock();

      if (activeFlag == 0)
        {
        break;
        }

      vtkSleep(remaining);
    }

  return 0;
}

/******************************************************************************
 *  static void *vtkDataSenderThread(vtkMultiThreader::ThreadInfo *data)
 *
 *  This function runs in an alternate thread to asyncronously send data
 *
 *  @Author:Jan Gumprecht
 *  @Date:  20.Januar 2009
 *
 *  @Param: vtkMultiThreader::ThreadInfo *data
 *
 * ****************************************************************************/
static void *vtkDataSenderThread(vtkMultiThreader::ThreadInfo *data)
{
  vtkDataSender *self = (vtkDataSender *)(data->UserData);

  double sendPeriod = self->GetSendPeriod();
  int currentIndex = -1;
  igtl::ImageMessage::Pointer imageMessage;
  int errors = 0;
  bool dataAvailable = false;
  double sectionTime;
  double loopTime;

  do
    {
    //New data available
    if(dataAvailable || !self->IsSendDataBufferEmpty())
      {
      loopTime = self->GetUpTime();
      currentIndex = self->GetHeadOfNewDataBuffer();

      #ifdef TIMINGSENDER
      self->GetLogStream() << self->GetUpTime() <<" |*********************************************" << endl;
      self->GetLogStream() << self->GetUpTime() <<" |S-INFO: Sender Found new Data at Index: " << currentIndex
                           << " | L:" << self->GetUpTime() - loopTime << endl
                           << "         | BufferSize: " << self->GetCurrentBufferSize() <<endl;
      #endif

      //Prepare new data for OpenIGTLink transfer
      sectionTime = self->GetUpTime();
      if (self->PrepareImageMessage(currentIndex, imageMessage) != -1)
        {
        #ifdef TIMINGSENDER
        self->GetLogStream() << self->GetUpTime() <<" |S-INFO: Message Prepared" << " | L:" << self->GetUpTime() - loopTime << "| S: " << self->GetUpTime() - sectionTime << endl;
        #endif

        //Send new data over OpenIGTLink
        sectionTime = self->GetUpTime();
        errors += self->SendMessage(imageMessage);

        #ifdef TIMINGSENDER
        self->GetLogStream() << self->GetUpTime() << " |S-INFO: Message Send"
                                                  << " | S: " << self->GetUpTime() - sectionTime
                                                  << " | L:" << self->GetUpTime() - loopTime
                                                  << " | FPS: " << 1 / (self->GetUpTime() - loopTime) << endl;
        #endif

        }
      else
      {
        #ifdef ERRORSENDER
        self->GetLogStream() << self->GetUpTime() <<" |S-ERROR: Error preparing message" << endl;
        #endif
      }

      //Delete sent data (Remove index from new data buffer + free maps from data)
      errors += self->UnlockData(currentIndex, DATASENDER);
      errors += self->TryToDeleteData(currentIndex);

      if(errors <= - ERRORTOLERANCE)
        {
        #ifdef ERRORSENDER
          self->GetLogStream() << self->GetUpTime() <<" |S-ERROR: Stopped sending too many errors occured" << endl;
        #endif
        self->StopSending();
        }
      }

    dataAvailable = !self->IsSendDataBufferEmpty();

    if(dataAvailable)
      {
      #ifdef  DEBUGSENDER
        self->GetLogStream() << self->GetUpTime() <<" |S-INFO: Data available sender won't sleep" << endl;
      #endif
      }
    else
      {
      #ifdef  DEBUGSENDER
        self->GetLogStream() << self->GetUpTime() <<" |S-INFO: NO Data available sender sleeps now" << endl;
      #endif
      }
    }
  while(vtkThreadSleep(data, vtkTimerLog::GetUniversalTime() + sendPeriod, dataAvailable));

  return NULL;
}
//----------------------------------------------------------------------------
bool vtkDataSender::IsSendDataBufferEmpty()
{
  return this->SendDataQueue.empty();
}

//----------------------------------------------------------------------------
bool vtkDataSender::IsSendDataBufferFull()
{
  if(static_cast<int>(this->SendDataQueue.size()) >= this->SendDataBufferSize)
    {
    return true;
    }
  else
    {
    return false;
    }
}

//----------------------------------------------------------------------------
int vtkDataSender::GetHeadOfNewDataBuffer()
{
  int head = this->SendDataQueue.front();
  this->SendDataQueue.pop();
  return head;
}

//----------------------------------------------------------------------------
int vtkDataSender::IncrementBufferIndex(int increment)
{
  this->SendDataBufferIndex = (this->SendDataBufferIndex + increment) % this->SendDataBufferSize;

  return this->SendDataBufferIndex;
}

//----------------------------------------------------------------------------
int vtkDataSender::StartSending()
{
   if (!this->Connected)
    {
    if(!this->ConnectToServer())
      {
      return 0;
      }
    }

  if (this->Sending)
    {
    #ifdef ERRORSENDER
    this->LogStream <<  this->GetUpTime() << " |S-ERROR: Data collector already collects data" << endl;
    #endif
    return -1;
    }

  this->NumberSent = 0;

  //Start send thread
  this->PlayerThreadId =
            this->PlayerThreader->SpawnThread((vtkThreadFunctionType)\
            &vtkDataSenderThread, this);

  //Check if thread successfully started
  if(this->PlayerThreadId != -1)
    {
    this->Sending = true;
    if(Verbose)
      {
      cout << "Started sending data" << endl;
      }
    return 0;
    }
  else
    {
    #ifdef  ERRORSENDER
      this->LogStream <<  this->GetUpTime() << " |S-ERROR: Could not start sender thread" << endl;
        #endif
    return -1;
    }
}

//------------------------------------------------------------
int vtkDataSender::StopSending()
{
  //Stop send thread
  if (this->Sending)
    {
    this->PlayerThreader->TerminateThread(this->PlayerThreadId);
    this->PlayerThreadId = -1;
    this->Sending = false;

    if(this->Verbose)
      {
      cout << "Stopped sending" << endl;
      }
    }

  #ifdef  DEBUGSENDER
    this->LogStream <<  this->GetUpTime() << " |S-INFO: Data Sender stopped sending" << endl;
  #endif
  
  return 0;

}

//------------------------------------------------------------
int vtkDataSender::PrepareImageMessage(int index,
                                       igtl::ImageMessage::Pointer& imageMessage)
{

  if(this->IsIndexAvailable(index) || index < 0 || index >= this->SendDataBufferSize)
    {
    #ifdef  ERRORSENDER
      this->LogStream <<  this->GetUpTime() << " |S-ERROR: No data to prepare at index: " << index << endl;
    #endif
    return -1;
    }

  #ifdef  DEBUGSENDER
    this->LogStream <<  this->GetUpTime() << " |S-INFO: Acquire lock for index: " << index << endl;
  #endif
  
  if(-1 == this->LockIndex(index, DATASENDER))
    {
    int i = 0;
    do
      {
      if(i++ > 100)
        {
        #ifdef  ERRORSENDER
          this->LogStream <<  this->GetUpTime() << " |S-ERROR: Cannot acquire lock for index: " << index << " | TimeWaited: " << i << " ms" <<endl;
        #endif
        }
      vtkSleep(0.01);
      }
      while(-1 == this->LockIndex(index, DATASENDER));
  }

  //Get Data
  vtkImageData * frame = this->SendDatabuffer[index].ImageData;
  vtkMatrix4x4 * matrix = this->SendDatabuffer[index].Matrix;
  int phase = this->SendDatabuffer[index].Phase;

  // Get properties for the frame
    frame->GetDimensions(FrameProperties.Size);
    FrameProperties.Spacing[0] = (double) frame->GetSpacing()[0];
    FrameProperties.Spacing[1] = (double) frame->GetSpacing()[1];
    FrameProperties.Spacing[2] = (double) frame->GetSpacing()[2];
    FrameProperties.Origin[0] = (float) frame->GetOrigin()[0];
    FrameProperties.Origin[1] = (float) frame->GetOrigin()[1];
    FrameProperties.Origin[2] = (float) frame->GetOrigin()[2];    
    FrameProperties.ScalarType = frame->GetScalarType();
    FrameProperties.SubVolumeSize[0] = FrameProperties.Size[0];
    FrameProperties.SubVolumeSize[1] = FrameProperties.Size[1];
    FrameProperties.SubVolumeSize[2] = FrameProperties.Size[2];
    FrameProperties.Set = true;

  //------------------------------------------------------------
  // Create a new image type message
  imageMessage = igtl::ImageMessage::New();

  // (origin was set in the matrix already)
  imageMessage->SetDimensions(FrameProperties.Size);
  imageMessage->SetSpacing((float) FrameProperties.Spacing[0],(float) FrameProperties.Spacing[1], (float) FrameProperties.Spacing[2]);
  imageMessage->SetScalarType(FrameProperties.ScalarType);
  char deviceName[50];
  if (phase != -1)
    {
    sprintf(deviceName, "4DUSPhase %d", phase);
    }
  else
    {
    sprintf(deviceName, "3DUS");
    }
  imageMessage->SetDeviceName(deviceName);
  imageMessage->AllocateScalars();

  // Copy image to output buffer
  // TODO assuming here that type is unsigned char...
  // Freehand code is implemented that way, but not necessarily true
  unsigned char * pImageMessage = (unsigned char*) imageMessage->GetScalarPointer();
  unsigned char * pFrame = (unsigned char*) frame->GetScalarPointer();
  
  #ifdef  DEBUGSENDER
    int counter = 0;
    double copyStart = this->GetUpTime();
  #endif

  #ifdef  DEBUGSENDER
    counter = FrameProperties.Size[0] * FrameProperties.Size[1] * FrameProperties.Size[2]; //scalarcomponents
  #endif
  memcpy(pImageMessage, pFrame, imageMessage->GetImageSize());
    
  //Copy matrix to output buffer
  igtl::Matrix4x4 igtlMatrix;
  igtlMatrix[0][0] = matrix->Element[0][0];
  igtlMatrix[0][1] = matrix->Element[0][1];
  igtlMatrix[0][2] = matrix->Element[0][2];
  igtlMatrix[0][3] = matrix->Element[0][3];
  igtlMatrix[1][0] = matrix->Element[1][0];
  igtlMatrix[1][1] = matrix->Element[1][1];
  igtlMatrix[1][2] = matrix->Element[1][2];
  igtlMatrix[1][3] = matrix->Element[1][3];
  igtlMatrix[2][0] = matrix->Element[2][0];
  igtlMatrix[2][1] = matrix->Element[2][1];
  igtlMatrix[2][2] = matrix->Element[2][2];
  igtlMatrix[2][3] = matrix->Element[2][3];
  igtlMatrix[3][0] = matrix->Element[3][0];
  igtlMatrix[3][1] = matrix->Element[3][1];
  igtlMatrix[3][2] = matrix->Element[3][2];
  igtlMatrix[3][3] = matrix->Element[3][3];

  this->ReleaseLock(DATASENDER);

  #ifdef  DEBUGSENDER
    this->LogStream <<  this->GetUpTime() << " |S-INFO: Size of image frame to send:"
                    << FrameProperties.Size[0] << " | "
                    << FrameProperties.Size[1] << " | "
                    << FrameProperties.Size[2] << endl
                    << "           | Origin: " << FrameProperties.Origin[0]<< "|" << FrameProperties.Origin[1]<< "|" << FrameProperties.Origin[2]<< endl
                    << "           | Copied Pixels: "<< counter << " | Copy time: " << this->GetUpTime() - copyStart <<endl
                    << "           | Index: " << index << " | "  <<endl;
  #endif

  imageMessage->SetMatrix(igtlMatrix);

  imageMessage->Pack();// Pack (serialize)
  
  //Statistics----------------------------------------------------------------
  this->Statistics.volumeCounter++;
      
  return 0;

}

//------------------------------------------------------------
int vtkDataSender::SendMessage(igtl::ImageMessage::Pointer& message)
  {

    //Send message over OpenIGTLink connection
    double sendTime = this->GetUpTime();
    int ret = this->Socket->Send(message->GetPackPointer(), message->GetPackSize());
    if (ret == 0)
      {
      cerr << "ERROR: Connection to OpenIGTLink Server lost while sending!" <<endl;
      this->StopSending();
      return -1;
      }
    else
      {
      #ifdef  DEBUGSENDER
        this->LogStream <<  this->GetUpTime() << " |S-INFO: Message successfully send to OpenIGTLink Server "<< endl
                        << "         | Send time: " << this->GetUpTime() - sendTime << "| "  << endl;        
      #endif
      if(Verbose)
        {
        this->UpdateFrameRate(this->GetUpTime());
        }
      }
  this->NumberSent++;
  return 0;
}

//------------------------------------------------------------
int vtkDataSender::NewData(vtkImageData* frame, vtkMatrix4x4* trackerMatrix, int phase)
{

  //Check if new data buffer is full
  if(this->IsSendDataBufferFull())
    {
    #ifdef  ERRORSENDER
      this->LogStream <<  this->GetUpTime() << " |S-ERROR: Senders new data buffer is full" << endl;
    #endif
    return -1;
    }

  int index = this->IncrementBufferIndex(1);

  //Add new data to Buffer
  if(-1 == this->AddDatatoBuffer(index, frame, trackerMatrix, phase))
    {
    cout << "ERROR - adding data to the buffer when there's already data there" << endl;
    cout << "\tYou're flooding the data sender, try a longer image send period" << endl;
    this->IncrementBufferIndex(-1);
    return -1;
    }

  return index;
}

//------------------------------------------------------------
int vtkDataSender::AddDatatoBuffer(int index, vtkImageData* imageData, vtkMatrix4x4* matrix, int phase)
{
  if(!this->IsIndexAvailable(index))
    {
    #ifdef  ERRORSENDER
      this->LogStream <<  this->GetUpTime() << " |S-ERROR: Send Data Buffer already has data at index: " << index << endl;
    #endif
    return -1;
    }

  //Create and fill new send data object
  struct SenderDataStruct newSendData;
  newSendData.ImageData = imageData;
  newSendData.Matrix = matrix;
  newSendData.Phase = phase;
  newSendData.SenderLock = 1;
  newSendData.ProcessorLock = 1;

  //Add Object to buffer
  this->SendDatabuffer[index] = newSendData;
  this->SendDataQueue.push(index);//Add Index to new data buffer

  return 0;
}

//------------------------------------------------------------
void vtkDataSender::DeleteAllData()
  {
  int index;
  while(!this->IsSendDataBufferEmpty())
    {
    index = this->GetHeadOfNewDataBuffer();
    this->UnlockData(index, DATASENDER);
    this->UnlockData(index, DATAPROCESSOR);
    this->TryToDeleteData(index);
    }
  }

//------------------------------------------------------------
int vtkDataSender::TryToDeleteData(int index)
{
  if(this->IsIndexAvailable(index) || index < 0 || index >= this->SendDataBufferSize)
    {
    #ifdef  ERRORSENDER
      this->LogStream <<  this->GetUpTime() << " |S-ERROR: Not data to delete in Send Data Buffer at index: " << index << endl;
    #endif
    return -1;
    }

  if(this->SendDatabuffer[index].SenderLock <= 0 && this->SendDatabuffer[index].ProcessorLock <= 0)
    {
    this->SendDatabuffer[index].ImageData->Delete();
    this->SendDatabuffer[index].Matrix->Delete();
    if(1 != this->SendDatabuffer.erase(index))
      {
      assert(true);
      }
    #ifdef  DEBUGSENDER
      this->LogStream <<  this->GetUpTime() << " |S-INFO: Deleted data in Send Data Buffer at index: " << index << endl;
    #endif
    }
  else
    {
    #ifdef  DEBUGSENDER
      this->LogStream <<  this->GetUpTime() << " |S-WARNING: Could not delete data at index:" << index << " lock by: ";
      
      if( this->SendDatabuffer[index].SenderLock > 0)
        {
        this->LogStream << "DataSender ";
        }
      
      if(this->SendDatabuffer[index].ProcessorLock > 0)
        {
        this->LogStream << "DataProcessor";
        }
      
      this->LogStream << endl;
    #endif
    }

  return 0;
}

//------------------------------------------------------------
int vtkDataSender::UnlockData(int index, int lock)
{
  if(this->IsIndexAvailable(index) || index < 0 || index >= this->SendDataBufferSize)
    {
    #ifdef  ERRORSENDER
    this->LogStream <<  this->GetUpTime() << " |S-ERROR: Send Data Buffer has no data at index: " << index << endl;
    #endif
    return -1;
    }

  if(lock == DATASENDER)
    {
    #ifdef  ERRORSENDER
    this->LogStream <<  this->GetUpTime() << " |S-INFO: Decrementing sender lock at: " << index << endl;
    #endif
    this->SendDatabuffer[index].SenderLock--;
    }
  else if(lock == DATAPROCESSOR)
    {
    #ifdef  ERRORSENDER
    this->LogStream <<  this->GetUpTime() << " |S-INFO: Decrementing processor lock at: " << index << endl;
    #endif
    this->SendDatabuffer[index].ProcessorLock--;
    }
  else
    {
    #ifdef  ERRORSENDER
      this->LogStream <<  this->GetUpTime() << " |S-ERROR: Try to release unknown lock" << endl;
    #endif

    return -1;
    }

  return 0;
}

/******************************************************************************
 * double vtkDataSender::GetUpTime()
 *
 *  Returns elapsed Time since program start
 *
 *  @Author:Jan Gumprecht
 *  @Date:  2.February 2009
 *
 * ****************************************************************************/
double vtkDataSender::GetUpTime()
{
  return vtkTimerLog::GetUniversalTime() - this->GetStartUpTime();
}

/******************************************************************************
 * bool vtkDataSender::IsIndexAvailable(int index)
 *
 *  Checks if given index is available
 *
 *  @Author:Jan Gumprecht
 *  @Date:  2.February 2009
 *
 *  @Return: True, if index is available
 *           False, if index is used
 *
 * ****************************************************************************/
bool vtkDataSender::IsIndexAvailable(int index)
{
  if(this->SendDatabuffer.find(index) == this->SendDatabuffer.end() && index < this->SendDataBufferSize && index >= 0)
    {
    return true;
    }
  else
    {
    return false;
    }
}

/******************************************************************************
 * void vtkDataSender::SetLogStream(ofstream &LogStream)
 *
 *  Redirects Logstream
 *
 *  @Author:Jan Gumprecht
 *  @Date:  4.February 2009
 *
 * ****************************************************************************/
void vtkDataSender::SetLogStream(ofstream &LogStream)
{
  this->LogStream.ostream::rdbuf(LogStream.ostream::rdbuf());
  this->LogStream.precision(6);
  this->LogStream.setf(ios::fixed,ios::floatfield);
}

/******************************************************************************
 * ofstream& vtkDataSender::GetLogStream()
 *
 *  Returns logstream
 *
 *  @Author:Jan Gumprecht
 *  @Date:  4.February 2009
 *
 *  @Return: Logstream
 *
 * ****************************************************************************/
ofstream& vtkDataSender::GetLogStream()
{
  return this->LogStream;
}

/******************************************************************************
 * void UpdateFrameRate(double sendTime)
 *
 *  Calculates and prints current frame rate to console
 *
 *  @Author:Jan Gumprecht
 *  @Date:  5.February 2009
 *
 *  @Param: double sentTime - send time of last message
 *
 * ****************************************************************************/
void vtkDataSender::UpdateFrameRate(double sendTime)
{
  this->UpdateCounter++;

  if(this->LastFrameRateUpdate + 1 <= this->GetUpTime())
    {
    if(this->LastFrameRateUpdate != 0)
      {
      float fps = this->UpdateCounter / (sendTime - this->LastFrameRateUpdate);
      cout << "\b\b\b\b\b" << std::flush;
      cout << setw(5) << setprecision(2) << fps;
      cout << "\a" <<std::flush;
        
      this->UpdateCounter = 0;
      this->Statistics.fpsCounter++;
      this->Statistics.meanFPS += fps;
      if(fps > this->Statistics.maxFPS)
        {
        this->Statistics.maxFPS = fps;
        }
      if(fps < this->Statistics.minFPS)
        {
        this->Statistics.minFPS = fps;
        }
      #ifdef  DEBUGSENDER
        this->LogStream <<  this->GetUpTime() << " |S-INFO: Current frame rate " << fps << endl
                                    << "           |        Mean FPS: " << this->Statistics.meanFPS / this->Statistics.fpsCounter << " Max FPS: "<< this->Statistics.maxFPS << " Min FPS: "<< this->Statistics.minFPS << endl;
        
      #endif
      }
    else
      {
      cout << "\nFrame rate:       fps\b\b\b\b" <<std::flush;
      }
    this->LastFrameRateUpdate = sendTime;
    }
}

/******************************************************************************
 * int vtkDataSender::LockIndex(int index, int requester)
 *
 *  Lock index "index"
 *
 *  @Author:Jan Gumprecht
 *  @Date:  11.February 2009
 *
 *  @Param: int index - index to lock
 *  @Param: int requester - Thread who wants to lock the index
 * 
 *  @Param: 0 on success
 *         -1 on failure
 *
 * ****************************************************************************/
int vtkDataSender::LockIndex(int index, int requester)
{
  int retVal = 0;
  this->IndexLock->Lock();
  if(requester == DATASENDER)
    {
    if(index == this->IndexLockedByDataProcessor)
       {
       retVal = -1;
       }
    else
      {
      #ifdef  DEBUGSENDER
        this->LogStream <<  this->GetUpTime() << " |S-INFO: Sender locked index:" << index << endl;
      #endif
      this->IndexLockedByDataSender = index;    
      }
    }
  else if(requester == DATAPROCESSOR)
    {
    if(index == this->IndexLockedByDataSender)
       {
       retVal = -1;
       }
    else
      {
      #ifdef DEBUGSENDER
        this->LogStream <<  this->GetUpTime() << " |S-INFO: Processor locked index:" << index << endl;
      #endif
      this->IndexLockedByDataProcessor = index;    
      }
    }
  else
    {
    #ifdef  ERRORSENDER
      this->LogStream <<  this->GetUpTime() << " |S-ERROR: Unknown requester("<< requester <<") tries to lock index:" << index << endl;
    #endif
    retVal = -1;
    }
  this->IndexLock->Unlock();
  
  return retVal;
}

/******************************************************************************
 * int vtkDataSender::ReleaseLock(int requester)
 *
 *  Release Lock
 *
 *  @Author:Jan Gumprecht
 *  @Date:  11.February 2009
 *
 *  @Param: int index - index to lock
 *  @Param: int requester - Thread who wants to lock the index
 * 
 *  @Param: 0 on success
 *         -1 on failure
 *
 * ****************************************************************************/
int vtkDataSender::ReleaseLock(int requester)
{
  int retVal = 0;
  this->IndexLock->Lock();
  if(requester == DATASENDER)
    {
    #ifdef  DEBUGSENDER
        this->LogStream <<  this->GetUpTime() << " |S-INFO: Sender released index:" << this->IndexLockedByDataSender << endl;
    #endif
    this->IndexLockedByDataSender = -1;
    }
  else if (requester == DATAPROCESSOR)
    {
    #ifdef  DEBUGSENDER
        this->LogStream <<  this->GetUpTime() << " |S-INFO: Processor released index:" << this->IndexLockedByDataProcessor << endl;
    #endif
    this->IndexLockedByDataProcessor = -1;
    }
  else
    {
    #ifdef  ERRORSENDER
      this->LogStream <<  this->GetUpTime() << " |S-ERROR: Unknown requester("<< requester <<") tries to release a lock "<< endl;
    #endif
    retVal = -1;
    }
  this->IndexLock->Unlock();
  
  return retVal;
}

