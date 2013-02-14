/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/ 

#ifndef __vtkPlusRequestDeviceIDsCommand_h
#define __vtkPlusRequestDeviceIDsCommand_h

#include "vtkPlusCommand.h"

/*!
  \class vtkPlusRequestDeviceIDsCommand 
  \brief This command returns the list of devices to the client
  \ingroup PlusLibDataCollection
 */ 
class VTK_EXPORT vtkPlusRequestDeviceIDsCommand : public vtkPlusCommand
{
public:

  static vtkPlusRequestDeviceIDsCommand *New();
  vtkTypeMacro(vtkPlusRequestDeviceIDsCommand, vtkPlusCommand);
  virtual void PrintSelf( ostream& os, vtkIndent indent );
  virtual vtkPlusCommand* Clone() { return New(); }

  /*! Executes the command  */
  virtual PlusStatus Execute();

  /*! Get all the command names that this class can execute */
  virtual void GetCommandNames(std::list<std::string> &cmdNames);

  /*! Gets the description for the specified command name. */
  virtual std::string GetDescription(const char* commandName);

  void SetNameToRequestDeviceIDs();

protected:

  vtkPlusRequestDeviceIDsCommand();
  virtual ~vtkPlusRequestDeviceIDsCommand();
  
private:

  vtkPlusRequestDeviceIDsCommand( const vtkPlusRequestDeviceIDsCommand& );
  void operator=( const vtkPlusRequestDeviceIDsCommand& );
  
};


#endif

