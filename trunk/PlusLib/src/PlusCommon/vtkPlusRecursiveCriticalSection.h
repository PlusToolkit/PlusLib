/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkPlusRecursiveCriticalSection_h
#define __vtkPlusRecursiveCriticalSection_h

#include "vtkPlusCommonExport.h"

// Get vtkCritSecType definition
#include "vtkCriticalSection.h"

/*!
  \class vtkPlusSimpleRecursiveCriticalSection
  \brief Allows the recursive locking of variables which are accessed through different threads. Not a VTK object.

  \sa vtkPlusRecursiveCriticalSection
  \ingroup PlusLibCommon
*/

// Critical Section object that is not a vtkObject.
class vtkPlusCommonExport vtkPlusSimpleRecursiveCriticalSection
{
public:
  // Default cstor
  vtkPlusSimpleRecursiveCriticalSection()
  {
    this->Init();
  }
  // Construct object locked if isLocked is different from 0
  vtkPlusSimpleRecursiveCriticalSection(int isLocked)
  {
    this->Init();
    if (isLocked)
    {
      this->Lock();
    }
  }
  // Destructor
  virtual ~vtkPlusSimpleRecursiveCriticalSection();

  // Default vtkObject API
  static vtkPlusSimpleRecursiveCriticalSection* New();
  void Delete()
  {
    delete this;
  }

  void Init();

  // Description:
  // Lock the vtkPlusRecursiveCriticalSection
  void Lock();

  // Description:
  // Unlock the vtkPlusRecursiveCriticalSection
  void Unlock();

protected:
  vtkCritSecType   CritSec;
};


/*!
  \class vtkPlusRecursiveCriticalSection
  \brief vtkPlusRecursiveCriticalSection allows the recursive locking of variables which are accessed through different threads

  The class provides recursive locking (the same thread is allowed to lock multiple times without being blocked) consistently
  on Windows and non-Windows OS. vtkPlusRecursiveCriticalSection is recursive on Windows and non-recursive on other operating systems.
  See a related bug report at: http://www.gccxml.org/Bug/view.php?id=9461

  \sa vtkPlusSimpleRecursiveCriticalSection
  \ingroup PlusLibCommon
*/
class vtkPlusCommonExport vtkPlusRecursiveCriticalSection : public vtkObject
{
public:
  static vtkPlusRecursiveCriticalSection* New();
  vtkTypeMacro(vtkPlusRecursiveCriticalSection, vtkObject);
  virtual void PrintSelf(ostream& os, vtkIndent indent) VTK_OVERRIDE;

  // Description:
  // Lock the vtkPlusRecursiveCriticalSection
  void Lock();

  // Description:
  // Unlock the vtkPlusRecursiveCriticalSection
  void Unlock();

protected:
  vtkPlusSimpleRecursiveCriticalSection SimpleRecursiveCriticalSection;
  vtkPlusRecursiveCriticalSection() {}
  ~vtkPlusRecursiveCriticalSection() {}

private:
  vtkPlusRecursiveCriticalSection(const vtkPlusRecursiveCriticalSection&);  // Not implemented.
  void operator=(const vtkPlusRecursiveCriticalSection&);  // Not implemented.
};


inline void vtkPlusRecursiveCriticalSection::Lock()
{
  this->SimpleRecursiveCriticalSection.Lock();
}

inline void vtkPlusRecursiveCriticalSection::Unlock()
{
  this->SimpleRecursiveCriticalSection.Unlock();
}

#endif
