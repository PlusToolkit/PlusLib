/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/ 

#ifndef __vtkIGTLMessageQueue_h
#define __vtkIGTLMessageQueue_h

#include "vtkPlusOpenIGTLinkExport.h"

#include "vtkObject.h"

#include <deque>

#include "igtlMessageBase.h"

class vtkRecursiveCriticalSection;

/*!
  \class vtkIGTLMessageQueue 
  \brief Message queue to store OpenIGTLink messages.
  \ingroup PlusLibOpenIGTLink
*/
class vtkPlusOpenIGTLinkExport vtkIGTLMessageQueue
: public vtkObject
{
public:

  static vtkIGTLMessageQueue *New();
  vtkTypeMacro( vtkIGTLMessageQueue,vtkObject );

  void PrintSelf(ostream& os, vtkIndent indent);

  void PushMessage( igtl::MessageBase* message );
  igtl::MessageBase* PullMessage();
  
  int GetSize();
  
protected:
  
  vtkIGTLMessageQueue();
  virtual ~vtkIGTLMessageQueue();
  
  
protected:

  vtkRecursiveCriticalSection* Mutex;
  
  std::deque< igtl::MessageBase* > DataBuffer;
  
};


#endif
