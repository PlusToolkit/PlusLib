
#ifndef __VTKPLUSSTARTDATACOLLECTIONCOMMAND_H
#define __VTKPLUSSTARTDATACOLLECTIONCOMMAND_H


#include <string>
#include <vector>

#include "vtkObject.h"

#include "vtkDataCollector.h"
#include "vtkPlusCommand.h"



/**
 * This command starts the vtkDataCollector on the server side.
 */
class
VTK_EXPORT 
vtkPlusStartDataCollectionCommand
: public vtkPlusCommand
{
public:
  
  static vtkPlusStartDataCollectionCommand *New();
  vtkTypeRevisionMacro( vtkPlusStartDataCollectionCommand, vtkObject );
  virtual void PrintSelf( ostream& os, vtkIndent indent );
  
  virtual bool Execute( std::string stringMessage );
  virtual std::string GetStringRepresentation();
  virtual void ProcessResponse( igtl::MessageBase::Pointer header,
                                igtl::ClientSocket::Pointer clientSocket );
  
protected:
  
  vtkPlusStartDataCollectionCommand();
  virtual ~vtkPlusStartDataCollectionCommand();
  
  
private:
	
  vtkPlusStartDataCollectionCommand( const vtkPlusStartDataCollectionCommand& );
  void operator=( const vtkPlusStartDataCollectionCommand& );
  
};


#endif

