/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/ 

/*=========================================================================
  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.
=========================================================================*/

#include "PlusConfigure.h"

#include "vtkRecursiveCriticalSection.h"
#include "vtkObjectFactory.h"

vtkStandardNewMacro(vtkRecursiveCriticalSection);

// New for the SimpleRecursiveCriticalSection
vtkSimpleRecursiveCriticalSection *vtkSimpleRecursiveCriticalSection::New()
{
  return new vtkSimpleRecursiveCriticalSection;
}

void vtkSimpleRecursiveCriticalSection::Init()
{
#ifdef VTK_USE_SPROC
  init_lock( &this->CritSec );
#endif
  
#ifdef VTK_USE_WIN32_THREADS
  //this->MutexLock = CreateMutex( NULL, FALSE, NULL ); 
  InitializeCriticalSection(&this->CritSec);
#endif

#ifdef VTK_USE_PTHREADS
  pthread_mutexattr_t recursiveAttr;
  pthread_mutexattr_init(&recursiveAttr);
  pthread_mutexattr_settype(&recursiveAttr, PTHREAD_MUTEX_RECURSIVE);
  pthread_mutex_init(&(this->CritSec), &recursiveAttr);  
  pthread_mutexattr_destroy(&recursiveAttr); 
#endif
}


// Destruct the vtkMutexVariable
vtkSimpleRecursiveCriticalSection::~vtkSimpleRecursiveCriticalSection()
{
#ifdef VTK_USE_WIN32_THREADS
  //CloseHandle(this->MutexLock);
  DeleteCriticalSection(&this->CritSec);
#endif

#ifdef VTK_USE_PTHREADS
  pthread_mutex_destroy( &this->CritSec);
#endif
}

// Lock the vtkRecursiveCriticalSection
void vtkSimpleRecursiveCriticalSection::Lock()
{
#ifdef VTK_USE_SPROC
  spin_lock( &this->CritSec );
#endif

#ifdef VTK_USE_WIN32_THREADS
  //WaitForSingleObject( this->MutexLock, INFINITE );
  EnterCriticalSection(&this->CritSec);
#endif

#ifdef VTK_USE_PTHREADS
  pthread_mutex_lock( &this->CritSec);
#endif
}

// Unlock the vtkRecursiveCriticalSection
void vtkSimpleRecursiveCriticalSection::Unlock()
{
#ifdef VTK_USE_SPROC
  release_lock( &this->CritSec );
#endif

#ifdef VTK_USE_WIN32_THREADS
  //ReleaseMutex( this->MutexLock );
  LeaveCriticalSection(&this->CritSec);
#endif

#ifdef VTK_USE_PTHREADS
  pthread_mutex_unlock( &this->CritSec);
#endif
}

void vtkRecursiveCriticalSection::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

