/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

#include "PlusConfigure.h"

#include "vtkPlusIGTLMessageQueue.h"

#include "vtkIGSIORecursiveCriticalSection.h"
#include "vtkObjectFactory.h"
#include <vtksys/SystemTools.hxx>

#include <string>

#include "igtlMessageBase.h"


vtkStandardNewMacro( vtkPlusIGTLMessageQueue );

//----------------------------------------------------------------------------
void vtkPlusIGTLMessageQueue::PrintSelf(ostream& os, vtkIndent indent)
{

}

//----------------------------------------------------------------------------
void vtkPlusIGTLMessageQueue::PushMessage( igtl::MessageBase* message )
{
  this->Mutex->Lock();
  this->DataBuffer.push_back( message );
  this->Mutex->Unlock();
}

//----------------------------------------------------------------------------
igtl::MessageBase* vtkPlusIGTLMessageQueue::PullMessage()
{
  this->Mutex->Lock();
  igtl::MessageBase* ret = NULL;
  if ( this->DataBuffer.size() > 0 )
  {
    this->DataBuffer.front();
    this->DataBuffer.pop_front();
  }
  this->Mutex->Unlock();

  return ret;
}

//----------------------------------------------------------------------------
int vtkPlusIGTLMessageQueue::GetSize()
{
  return this->DataBuffer.size();
}

//----------------------------------------------------------------------------
vtkPlusIGTLMessageQueue::vtkPlusIGTLMessageQueue()
{
  this->Mutex = vtkIGSIORecursiveCriticalSection::New();
}

//----------------------------------------------------------------------------
vtkPlusIGTLMessageQueue::~vtkPlusIGTLMessageQueue()
{
  this->Mutex->Delete();
}
