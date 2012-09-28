/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/ 

#ifndef __vtkPlusCommandCollection_h
#define __vtkPlusCommandCollection_h

#include "vtkCollection.h"

class vtkPlusCommand;

/*!
  \class vtkPlusCommandCollection 
  \brief Thread-safe container that stores multiple PlusCommand objects.
  \ingroup PlusLibDataCollection
*/
class
VTK_EXPORT
vtkPlusCommandCollection
: public vtkCollection
{
public:
  vtkTypeMacro(vtkPlusCommandCollection,vtkCollection);
  static vtkPlusCommandCollection* New();
  virtual void PrintSelf(ostream& os, vtkIndent indent);

  void AddItem( vtkPlusCommand* );

  vtkPlusCommand* GetNextItem();
   
  /*!
    Reentrant safe way to get an object in a collection. Just pass the
    same cookie back and forth.
  */    
  vtkPlusCommand* GetNextPlusCommand( vtkCollectionSimpleIterator &cookie );

protected:
  vtkPlusCommandCollection() {};
  ~vtkPlusCommandCollection() {};
  

private:
  /*! hide the standard AddItem from the user and the compiler. */
  void AddItem( vtkObject* o ) { this->vtkCollection::AddItem( o ); };

private:
  vtkPlusCommandCollection(const vtkPlusCommandCollection&);  // Not implemented.
  void operator=(const vtkPlusCommandCollection&);  // Not implemented.
};

#endif
