/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/ 

#include "vtkPlusCommandCollection.h"


#include "vtkObjectFactory.h"
#include "vtkPlusCommand.h"



vtkStandardNewMacro( vtkPlusCommandCollection );



void
vtkPlusCommandCollection
::AddItem( vtkPlusCommand *f ) 
{
  this->vtkCollection::AddItem( f );
}



vtkPlusCommand*
vtkPlusCommandCollection
::GetNextItem() 
{ 
  return static_cast< vtkPlusCommand* >( this->GetNextItemAsObject() );
}



vtkPlusCommand*
vtkPlusCommandCollection
::GetNextPlusCommand( vtkCollectionSimpleIterator &cookie ) 
{
  return static_cast< vtkPlusCommand* >( this->GetNextItemAsObject( cookie ) );
}



void
vtkPlusCommandCollection
::PrintSelf( ostream& os, vtkIndent indent )
{
  this->Superclass::PrintSelf( os, indent );
}
