/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/ 

#ifndef __vtkIGTLMessageQueue_h
#define __vtkIGTLMessageQueue_h

#include "vtkObject.h"

#include <deque>
#include <string>

#include "igtlMessageBase.h"


class vtkMutexLock;

/*!
  \class vtkIGTLMessageQueue 
  \brief Message queue to store OpenIGTLink messages.
  \ingroup PlusLibDataCollection
*/
class
VTK_EXPORT 
vtkIGTLMessageQueue
: public vtkObject
{
public:

  static vtkIGTLMessageQueue *New();
  vtkTypeRevisionMacro( vtkIGTLMessageQueue,vtkObject );

  void PrintSelf(ostream& os, vtkIndent indent);

  void PushMessage( igtl::MessageBase* message );
  igtl::MessageBase* PullMessage();
  
  int GetSize();
  
protected:
  
  vtkIGTLMessageQueue();
  virtual ~vtkIGTLMessageQueue();
  
  
protected:

  vtkMutexLock* Mutex;
  
  std::deque< igtl::MessageBase* > DataBuffer;
  
};


#endif
