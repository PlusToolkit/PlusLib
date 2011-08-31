
#ifndef __vtkPlusCommandCollection_h
#define __vtkPlusCommandCollection_h

#include "vtkCollection.h"

class vtkPlusCommand;



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
  
  //BTX
  // Description: 
  // Reentrant safe way to get an object in a collection. Just pass the
  // same cookie back and forth. 
  vtkPlusCommand* GetNextPlusCommand( vtkCollectionSimpleIterator &cookie );
  //ETX

protected:
  vtkPlusCommandCollection() {};
  ~vtkPlusCommandCollection() {};
  

private:
    // hide the standard AddItem from the user and the compiler.
  void AddItem( vtkObject* o ) { this->vtkCollection::AddItem( o ); };

private:
  vtkPlusCommandCollection(const vtkPlusCommandCollection&);  // Not implemented.
  void operator=(const vtkPlusCommandCollection&);  // Not implemented.
};

#endif
