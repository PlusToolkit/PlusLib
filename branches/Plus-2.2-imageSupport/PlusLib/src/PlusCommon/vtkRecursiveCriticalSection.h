/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/ 

#ifndef __vtkRecursiveCriticalSection_h
#define __vtkRecursiveCriticalSection_h

// Get vtkCritSecType definition
#include "vtkCriticalSection.h"

/*!
  \class vtkSimpleRecursiveCriticalSection
  \brief Allows the recursive locking of variables which are accessed through different threads. Not a VTK object.
  
  \sa vtkRecursiveCriticalSection
  \ingroup PlusLibCommon
*/

// Critical Section object that is not a vtkObject.
class VTK_EXPORT vtkSimpleRecursiveCriticalSection
{
public:
  // Default cstor
  vtkSimpleRecursiveCriticalSection()
    {
    this->Init();
    }
  // Construct object locked if isLocked is different from 0
  vtkSimpleRecursiveCriticalSection(int isLocked)
    {
    this->Init();
    if(isLocked)
      {
      this->Lock();
      }
    }
  // Destructor
  virtual ~vtkSimpleRecursiveCriticalSection();

  // Default vtkObject API
  static vtkSimpleRecursiveCriticalSection *New();
  void Delete()
    {
    delete this;
    }

  void Init();

  // Description:
  // Lock the vtkRecursiveCriticalSection
  void Lock();

  // Description:
  // Unlock the vtkRecursiveCriticalSection
  void Unlock();

protected:
  vtkCritSecType   CritSec;
};


/*!
  \class vtkRecursiveCriticalSection
  \brief vtkRecursiveCriticalSection allows the recursive locking of variables which are accessed through different threads
  
  The class provides recursive locking (the same thread is allowed to lock multiple times without being blocked) consistently
  on Windows and non-Windows OS. vtkRecursiveCriticalSection is recursive on Windows and non-recursive on other operating systems.
  See a related bug report at: http://www.gccxml.org/Bug/view.php?id=9461
  
  \sa vtkSimpleRecursiveCriticalSection
  \ingroup PlusLibCommon
*/
class VTK_EXPORT vtkRecursiveCriticalSection : public vtkObject
{
public:
  static vtkRecursiveCriticalSection *New();

  vtkTypeMacro(vtkRecursiveCriticalSection,vtkObject);
  void PrintSelf(ostream& os, vtkIndent indent);
  
  // Description:
  // Lock the vtkRecursiveCriticalSection
  void Lock();

  // Description:
  // Unlock the vtkRecursiveCriticalSection
  void Unlock();

protected:
  vtkSimpleRecursiveCriticalSection SimpleRecursiveCriticalSection;
  vtkRecursiveCriticalSection() {}
  ~vtkRecursiveCriticalSection() {}

private:
  vtkRecursiveCriticalSection(const vtkRecursiveCriticalSection&);  // Not implemented.
  void operator=(const vtkRecursiveCriticalSection&);  // Not implemented.
};


inline void vtkRecursiveCriticalSection::Lock()
{
  this->SimpleRecursiveCriticalSection.Lock();
}

inline void vtkRecursiveCriticalSection::Unlock()
{
  this->SimpleRecursiveCriticalSection.Unlock();
}

#endif
