/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkPlusGetFrameRateCommand_h
#define __vtkPlusGetFrameRateCommand_h

#include "vtkPlusServerExport.h"

#include "vtkPlusCommand.h"

class vtkMatrix4x4;

/*!
  \class vtkPlusGetFrameRateCommand
  \brief This command returns the current frame rate from a vtkPlusChannel's DataSource.
  \ingroup PlusLibPlusServer

  This command is used for communicating with a vtkPlusChannel.
 */
class vtkPlusServerExport vtkPlusGetFrameRateCommand : public vtkPlusCommand
{
public:

  static vtkPlusGetFrameRateCommand* New();
  vtkTypeMacro(vtkPlusGetFrameRateCommand, vtkPlusCommand);
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

  /*! Id of the channel to pull frame rate information from. */
  virtual std::string GetChannelId() const;
  virtual void SetChannelId(const std::string& channelId);

  /*!
    If true then the command waits for a response and returns with the received frame rate in the command response.
  */
  vtkSetMacro(ResponseExpected, bool);
  vtkGetMacro(ResponseExpected, bool);
  vtkBooleanMacro(ResponseExpected, bool);

  void SetNameToGetFrameRate();

protected:
  vtkPlusGetFrameRateCommand();
  virtual ~vtkPlusGetFrameRateCommand();

private:
  std::string ChannelId;
  bool ResponseExpected;

  vtkPlusGetFrameRateCommand(const vtkPlusGetFrameRateCommand&);
  void operator=(const vtkPlusGetFrameRateCommand&);
};


#endif