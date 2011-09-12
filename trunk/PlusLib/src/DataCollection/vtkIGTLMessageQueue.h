
#ifndef __vtkIGTLMessageQueue_h
#define __vtkIGTLMessageQueue_h

#include "vtkObject.h"

#include <deque>
#include <string>

#include "igtlMessageBase.h"


class vtkMutexLock;


class
VTK_EXPORT 
vtkIGTLMessageQueue
: public vtkObject
{
public:

  static vtkIGTLMessageQueue *New();
  vtkTypeRevisionMacro( vtkIGTLMessageQueue,vtkObject );

  void PrintSelf(ostream& os, vtkIndent indent);

  void PushMessage( igtl::MessageBase::Pointer message );
  igtl::MessageBase::Pointer PullMessage();
  
  
protected:
  
  vtkIGTLMessageQueue();
  virtual ~vtkIGTLMessageQueue();
  
  
protected:

  vtkMutexLock* Mutex;
  
  std::deque< igtl::MessageBase::Pointer > DataBuffer;
  
};


#endif
