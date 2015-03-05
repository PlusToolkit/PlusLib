/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/ 

#ifndef __vtkPlusSendTextCommand_h
#define __vtkPlusSendTextCommand_h

#include "vtkPlusServerExport.h"

#include "vtkPlusCommand.h"

class vtkMatrix4x4;

/*!
  \class vtkPlusSendTextCommand 
  \brief This command send text to a device and optionally return with a response text
  \ingroup PlusLibPlusServer
  
  This command is used for communicating with a generic serial device.
 */ 
class vtkPlusServerExport vtkPlusSendTextCommand : public vtkPlusCommand
{
public:

  static vtkPlusSendTextCommand *New();
  vtkTypeMacro(vtkPlusSendTextCommand, vtkPlusCommand);
  virtual void PrintSelf( ostream& os, vtkIndent indent );
  virtual vtkPlusCommand* Clone() { return New(); }

  /*! Executes the command  */
  virtual PlusStatus Execute();

  /*! Read command parameters from XML */
  virtual PlusStatus ReadConfiguration(vtkXMLDataElement* aConfig);

  /*! Write command parameters to XML */
  virtual PlusStatus WriteConfiguration(vtkXMLDataElement* aConfig);

  /*! Get all the command names that this class can execute */
  virtual void GetCommandNames(std::list<std::string> &cmdNames);

  /*! Gets the description for the specified command name. */
  virtual std::string GetDescription(const char* commandName);
 
  /*! Id of the device that the text will be sent to */
  vtkGetStringMacro(DeviceId);
  vtkSetStringMacro(DeviceId);
 
  /*! Text to send */
  vtkGetStringMacro(Text);
  vtkSetStringMacro(Text);

  /*! Text to send */
  vtkGetStringMacro(ResponseText);
  vtkSetStringMacro(ResponseText);

  /*!
    If true then the command waits for a response and returns with the received text in the command response.
  */
  vtkSetMacro(ResponseExpected, bool);
  vtkGetMacro(ResponseExpected, bool);
  vtkBooleanMacro(ResponseExpected, bool);

  void SetNameToSendText();

protected:
  vtkPlusSendTextCommand();
  virtual ~vtkPlusSendTextCommand();
  
private:
  char* DeviceId;
  char* Text;
  char* ResponseText;
  bool ResponseExpected;

  vtkPlusSendTextCommand( const vtkPlusSendTextCommand& );
  void operator=( const vtkPlusSendTextCommand& );
};


#endif