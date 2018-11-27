/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/ 

#ifndef __vtkPlusIGTLMessageQueue_h
#define __vtkPlusIGTLMessageQueue_h

#include "vtkPlusOpenIGTLinkExport.h"

#include "vtkObject.h"

#include <deque>

#include "igtlMessageBase.h"

class vtkIGSIORecursiveCriticalSection;

/*!
  \class vtkPlusIGTLMessageQueue 
  \brief Message queue to store OpenIGTLink messages.
  \ingroup PlusLibOpenIGTLink
*/
class vtkPlusOpenIGTLinkExport vtkPlusIGTLMessageQueue
: public vtkObject
{
public:
  static vtkPlusIGTLMessageQueue *New();
  vtkTypeMacro( vtkPlusIGTLMessageQueue,vtkObject );
  virtual void PrintSelf(ostream& os, vtkIndent indent) VTK_OVERRIDE;

  void PushMessage( igtl::MessageBase* message );
  igtl::MessageBase* PullMessage();
  
  int GetSize();
  
protected:
  
  vtkPlusIGTLMessageQueue();
  virtual ~vtkPlusIGTLMessageQueue();
  
  
protected:

  vtkIGSIORecursiveCriticalSection* Mutex;
  
  std::deque< igtl::MessageBase* > DataBuffer;
  
};


#endif
