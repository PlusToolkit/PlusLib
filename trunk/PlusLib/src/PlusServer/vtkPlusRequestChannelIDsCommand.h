/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/ 

#ifndef __vtkPlusRequestChannelIDsCommand_h
#define __vtkPlusRequestChannelIDsCommand_h

#include "vtkPlusCommand.h"

/*!
  \class vtkPlusRequestDeviceIDsCommand 
  \brief This command returns the list of devices to the client
  \ingroup PlusLibDataCollection
 */ 
class VTK_EXPORT vtkPlusRequestChannelIDsCommand : public vtkPlusCommand
{
public:

  static vtkPlusRequestChannelIDsCommand *New();
  vtkTypeMacro(vtkPlusRequestChannelIDsCommand, vtkPlusCommand);
  virtual void PrintSelf( ostream& os, vtkIndent indent );
  virtual vtkPlusCommand* Clone() { return New(); }

  /*! Executes the command  */
  virtual PlusStatus Execute();

  /*! Get all the command names that this class can execute */
  virtual void GetCommandNames(std::list<std::string> &cmdNames);

  /*! Gets the description for the specified command name. */
  virtual std::string GetDescription(const char* commandName);

  void SetNameToRequestChannelIDs();

  /*! Read command parameters from XML */
  virtual PlusStatus ReadConfiguration(vtkXMLDataElement* aConfig);

  /*! Write command parameters to XML */
  virtual PlusStatus WriteConfiguration(vtkXMLDataElement* aConfig);

  vtkGetStringMacro(DeviceId);
  vtkSetStringMacro(DeviceId);

protected:

  vtkPlusRequestChannelIDsCommand();
  virtual ~vtkPlusRequestChannelIDsCommand();
  
  char* DeviceId;

private:

  vtkPlusRequestChannelIDsCommand( const vtkPlusRequestChannelIDsCommand& );
  void operator=( const vtkPlusRequestChannelIDsCommand& );
  
};


#endif

