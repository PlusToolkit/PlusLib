/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkPlusVersionCommand_h
#define __vtkPlusVersionCommand_h

#include "vtkPlusServerExport.h"

#include "vtkPlusCommand.h"

/*!
  \class vtkPlusVersionCommand
  \brief This command sends and receives version information to/from a client
  \ingroup PlusLibPlusServer
 */
class vtkPlusServerExport vtkPlusVersionCommand : public vtkPlusCommand
{
public:

  static vtkPlusVersionCommand* New();
  vtkTypeMacro(vtkPlusVersionCommand, vtkPlusCommand);
  virtual vtkPlusCommand* Clone() { return New(); }

  /*! Executes the command  */
  virtual PlusStatus Execute();

  /*! Get all the command names that this class can execute */
  virtual void GetCommandNames(std::list<std::string>& cmdNames);

  /*! Gets the description for the specified command name. */
  virtual std::string GetDescription(const std::string& commandName);

  void SetNameToVersion();

protected:
  vtkPlusVersionCommand();
  virtual ~vtkPlusVersionCommand();

private:
  vtkPlusVersionCommand(const vtkPlusVersionCommand&);
  void operator=(const vtkPlusVersionCommand&);
};


#endif