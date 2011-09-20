
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
::PushMessage( igtl::MessageBase* message )
{
  this->Mutex->Lock();
  this->DataBuffer.push_back( message );
  this->Mutex->Unlock();
}



igtl::MessageBase*
vtkIGTLMessageQueue
::PullMessage()
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
