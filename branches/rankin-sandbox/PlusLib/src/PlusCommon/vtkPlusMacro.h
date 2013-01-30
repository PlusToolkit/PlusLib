/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkPlusMacro_h
#define __vtkPlusMacro_h

//
// Const version of Get pointer to object.  Creates member Get"name" (e.g., GetPoints()).
// This macro should be used in the header file.
//
#define vtkGetObjectConstMacro(name,type)                               \
  virtual type *Get##name () const                                      \
  {                                                                     \
  vtkDebugConstMacro(<< this->GetClassName() << " (" << this            \
  << "): returning " #name " address " << this->name );                 \
  return this->name;                                                    \
  }

//
// This macro is used for  debug statements in instance methods
// vtkDebugMacro(<< "this is debug info" << this->SomeVariable);
//
#define vtkDebugConstMacro(x) \
  vtkDebugWithObjectConstMacro(this,x)

#ifdef NDEBUG
# define vtkDebugWithObjectConstMacro(self, x)
#else
# define vtkDebugWithObjectConstMacro(self, x)                                      \
  {                                                                                 \
  if (vtkObject::GetGlobalWarningDisplay())                                         \
    {                                                                               \
    vtkOStreamWrapper::EndlType endl;                                               \
    vtkOStreamWrapper::UseEndl(endl);                                               \
    vtkOStrStreamWrapper vtkmsg;                                                    \
    vtkmsg << "Debug: In " __FILE__ ", line " << __LINE__ << "\n"                   \
    << self->GetClassName() << " (" << self << "): " x  << "\n\n";                  \
    vtkOutputWindowDisplayDebugText(vtkmsg.str());                                  \
    vtkmsg.rdbuf()->freeze(0);                                                      \
    }                                                                               \
  }
#endif
#endif //__vtkPlusMacro_h
