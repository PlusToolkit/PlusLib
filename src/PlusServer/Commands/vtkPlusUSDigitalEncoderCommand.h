/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkPlusUSDigitalEncodersCommand_h
#define __vtkPlusUSDigitalEncodersCommand_h

#include "vtkPlusServerExport.h"

#include "vtkPlusCommand.h"

class vtkMatrix4x4;

/*!
  \class vtkPlusUSDigitalEncoderCommand
  \brief This command controls USDigitalEncodersTracker devices.
  \ingroup PlusLibPlusServer

  This command is used for controlling a USDigitalEncodersTracker device
  to reset or retrieve the positions of its individual encoders.
 */
class vtkPlusServerExport vtkPlusUSDigitalEncoderCommand : public vtkPlusCommand
{
public:

  static vtkPlusUSDigitalEncoderCommand* New();
  vtkTypeMacro(vtkPlusUSDigitalEncoderCommand, vtkPlusCommand);
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

  /*! Id of the device that the command will be sent to */
  virtual std::string GetDeviceId() const;
  virtual void SetDeviceId(const std::string& deviceId);

  /*!
    If true then the command waits for a response and returns with the received command response.
  */
  vtkSetMacro(ResponseExpected, bool);
  vtkGetMacro(ResponseExpected, bool);
  vtkBooleanMacro(ResponseExpected, bool);

  void SetNameToEncoderCommand();

protected:
  vtkPlusUSDigitalEncoderCommand();
  virtual ~vtkPlusUSDigitalEncoderCommand();

  std::list<std::pair<std::string, std::string>> CommandList;

private:
  std::string DeviceId;
  bool ResponseExpected;

  vtkPlusUSDigitalEncoderCommand(const vtkPlusUSDigitalEncoderCommand&);
  void operator=(const vtkPlusUSDigitalEncoderCommand&);
};


#endif