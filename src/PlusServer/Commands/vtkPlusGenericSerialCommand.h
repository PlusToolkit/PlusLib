/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkPlusGenericSerialCommand_h
#define __vtkPlusGenericSerialCommand_h

#include "vtkPlusServerExport.h"

#include "vtkPlusCommand.h"

class vtkMatrix4x4;

/*!
  \class vtkPlusGenericSerialCommand
  \brief This command is for communicating with vtkPlusGenericSerialDevices.
  \ingroup PlusLibPlusServer

  This command is used for communicating with a generic serial device.
 */
class vtkPlusServerExport vtkPlusGenericSerialCommand : public vtkPlusCommand
{
public:

  static vtkPlusGenericSerialCommand* New();
  vtkTypeMacro(vtkPlusGenericSerialCommand, vtkPlusCommand);
  virtual void PrintSelf(ostream& os, vtkIndent indent);
  virtual vtkPlusCommand* Clone() { return New(); }

  /*! Executes the command  */
  virtual PlusStatus Execute();

  /*! Read command parameters from XML */
  virtual PlusStatus ReadConfiguration(vtkXMLDataElement* aConfig);

  /*! Write command parameters to XML */
  virtual PlusStatus WriteConfiguration(vtkXMLDataElement* aConfig);

  /*! Get all the command names that this class can execute */
  virtual void GetCommandNames(std::list<std::string>& cmdNames);

  /*! Gets the description for the specified command name. */
  virtual std::string GetDescription(const std::string& commandName);

  /*! Id of the device that the text will be sent to */
  virtual std::string GetDeviceId() const;
  virtual void SetDeviceId(const std::string& deviceId);

  /*!
    If true then the command waits for a response and returns with the received text in the command response.
  */
  vtkSetMacro(ResponseExpected, bool);
  vtkGetMacro(ResponseExpected, bool);
  vtkBooleanMacro(ResponseExpected, bool);

  void SetNameToSerial();

protected:
  vtkPlusGenericSerialCommand();
  virtual ~vtkPlusGenericSerialCommand();

  std::string GetCommandName() const;
  void SetCommandName(const std::string& text);
  std::string GetCommandValue() const;
  void SetCommandValue(const std::string& text);

private:
  std::string DeviceId;
  std::string CommandName;
  std::string CommandValue;
  bool ResponseExpected;

  vtkPlusGenericSerialCommand(const vtkPlusGenericSerialCommand&);
  void operator=(const vtkPlusGenericSerialCommand&);
};


#endif