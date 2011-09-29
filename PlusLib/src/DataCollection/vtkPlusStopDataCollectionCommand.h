
#ifndef __vtkPlusStopDataCollectionCommand_h
#define __vtkPlusStopDataCollectionCommand_h


#include <string>
#include <vector>

#include "vtkObject.h"

#include "vtkDataCollector.h"
#include "vtkPlusCommand.h"



/**
 * This command stops the vtkDataCollector on the server side.
 */
class
VTK_EXPORT 
vtkPlusStopDataCollectionCommand
: public vtkPlusCommand
{
public:
  
  static vtkPlusStopDataCollectionCommand *New();
  vtkTypeRevisionMacro( vtkPlusStopDataCollectionCommand, vtkObject );
  virtual void PrintSelf( ostream& os, vtkIndent indent );
  
  virtual bool CanExecute( std::string str );
  virtual bool Execute();
  virtual std::string GetStringRepresentation();
  
  
protected:
  
  vtkPlusStopDataCollectionCommand();
  virtual ~vtkPlusStopDataCollectionCommand();
  
  
private:
	
  vtkPlusStopDataCollectionCommand( const vtkPlusStopDataCollectionCommand& );
  void operator=( const vtkPlusStopDataCollectionCommand& );
  
};


#endif

