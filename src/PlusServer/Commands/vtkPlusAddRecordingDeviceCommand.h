/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkPlusAddRecordingDeviceCommand_h
#define __vtkPlusAddRecordingDeviceCommand_h

#include "vtkPlusServerExport.h"

#include "vtkPlusCommand.h"

/*!
  \class vtkPlusAddRecordingDeviceCommand
  \brief This command adds a virtual capture device with the attached arguments as input channels
  \ingroup PlusLibPlusServer
 */
class vtkPlusServerExport vtkPlusAddRecordingDeviceCommand : public vtkPlusCommand
{
public:
  static vtkPlusAddRecordingDeviceCommand* New();
  vtkTypeMacro(vtkPlusAddRecordingDeviceCommand, vtkPlusCommand);
  virtual vtkPlusCommand* Clone() { return New(); }

  /*! Executes the command  */
  virtual PlusStatus Execute();

  /*! Get all the command names that this class can execute */
  virtual void GetCommandNames(std::list<std::string>& cmdNames);

  /*! Gets the description for the specified command name. */
  virtual std::string GetDescription(const std::string& commandName);

  void SetNameToAddRecordingDevice();

protected:
  vtkPlusAddRecordingDeviceCommand();
  virtual ~vtkPlusAddRecordingDeviceCommand();

private:
  vtkPlusAddRecordingDeviceCommand(const vtkPlusAddRecordingDeviceCommand&);
  void operator=(const vtkPlusAddRecordingDeviceCommand&);
};

#endif
