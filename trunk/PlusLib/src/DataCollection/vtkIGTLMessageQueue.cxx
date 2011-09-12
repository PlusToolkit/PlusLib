
#include "vtkIGTLMessageQueue.h"


#include "vtkMutexLock.h"
#include "vtkObjectFactory.h"
#include <vtksys/SystemTools.hxx>

#include <string>

#include "igtlMessageBase.h"


vtkStandardNewMacro( vtkIGTLMessageQueue );
vtkCxxRevisionMacro( vtkIGTLMessageQueue, "$Revision: 1 $" );



void
vtkIGTLMessageQueue
::PrintSelf(ostream& os, vtkIndent indent)
{
  
}



void
vtkIGTLMessageQueue
::PushMessage( igtl::MessageBase::Pointer message )
{
  this->Mutex->Lock();
  this->DataBuffer.push_back( message );
  this->Mutex->Unlock();
}



igtl::MessageBase::Pointer
vtkIGTLMessageQueue
::PullMessage()
{
  this->Mutex->Lock();
  igtl::MessageBase::Pointer ret = this->DataBuffer.front();
  this->DataBuffer.pop_front();
  this->Mutex->Unlock();
  
  return ret;
}



vtkIGTLMessageQueue
::vtkIGTLMessageQueue()
{
  this->Mutex = vtkMutexLock::New();
}



vtkIGTLMessageQueue
::~vtkIGTLMessageQueue()
{
  this->Mutex->Delete();
}
