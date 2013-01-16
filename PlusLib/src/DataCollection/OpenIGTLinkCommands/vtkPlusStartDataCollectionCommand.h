/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/ 

#ifndef __vtkPlusStartDataCollectionCommand_h
#define __vtkPlusStartDataCollectionCommand_h


#include <string>

#include "vtkPlusCommand.h"

/*!
  \class vtkPlusStartDataCollectionCommand 
  \brief This command stops the vtkDataCollector on the server side. 
  \ingroup PlusLibDataCollection
 */ 
class VTK_EXPORT vtkPlusStartDataCollectionCommand : public vtkPlusCommand
{
public:
  static vtkPlusStartDataCollectionCommand *New();
  vtkTypeMacro(vtkPlusStartDataCollectionCommand, vtkObject);

  virtual vtkPlusCommand* Clone() { return New(); }

  virtual void PrintSelf( ostream& os, vtkIndent indent );

  /*! Executes the command  */
  virtual PlusStatus Execute();

  virtual void GetCommandNames(std::list<std::string> &cmdNames)
  { cmdNames.clear(); cmdNames.push_back("StartDataCollection"); }

  /*! 
    Gets the description for the specified command name.
    \param commandName Command name to provide the description for. If the pointer is NULL then all the supported commands shal be described.
  */
  virtual const char* GetDescription(const char* commandName)
  { return "StartDataCollection: Stops collecting data and save the result to file"; }
  
protected:
  
  vtkPlusStartDataCollectionCommand();
  virtual ~vtkPlusStartDataCollectionCommand();
  
  
private:
	
  vtkPlusStartDataCollectionCommand( const vtkPlusStartDataCollectionCommand& );
  void operator=( const vtkPlusStartDataCollectionCommand& );
  
};


#endif

