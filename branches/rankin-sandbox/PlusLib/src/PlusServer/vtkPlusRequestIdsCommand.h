/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/ 

#ifndef __vtkPlusRequestIdsCommand_h
#define __vtkPlusRequestIdsCommand_h

#include "vtkPlusCommand.h"

/*!
  \class vtkPlusRequestDeviceIDsCommand 
  \brief This command returns the list of devices to the client
  \ingroup PlusLibPlusServer
 */ 
class VTK_EXPORT vtkPlusRequestIdsCommand : public vtkPlusCommand
{
public:

  static vtkPlusRequestIdsCommand *New();
  vtkTypeMacro(vtkPlusRequestIdsCommand, vtkPlusCommand);
  virtual void PrintSelf( ostream& os, vtkIndent indent );
  virtual vtkPlusCommand* Clone() { return New(); }

  /*! Executes the command  */
  virtual PlusStatus Execute();

  /*! Get all the command names that this class can execute */
  virtual void GetCommandNames(std::list<std::string> &cmdNames);

  /*! Gets the description for the specified command name. */
  virtual std::string GetDescription(const char* commandName);

  void SetNameToRequestChannelIds();
  void SetNameToRequestDeviceIds();

  /*! Read command parameters from XML */
  virtual PlusStatus ReadConfiguration(vtkXMLDataElement* aConfig);

  /*! Write command parameters to XML */
  virtual PlusStatus WriteConfiguration(vtkXMLDataElement* aConfig);

  /*! Restrict the returned device IDs to this type */
  vtkSetStringMacro(DeviceType);
  vtkGetStringMacro(DeviceType);

protected:

  vtkPlusRequestIdsCommand();
  virtual ~vtkPlusRequestIdsCommand();

  char* DeviceType;

private:

  vtkPlusRequestIdsCommand( const vtkPlusRequestIdsCommand& );
  void operator=( const vtkPlusRequestIdsCommand& );
  
};


#endif

