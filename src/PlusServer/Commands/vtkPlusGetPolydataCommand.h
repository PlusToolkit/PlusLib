/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkPlusGetPolydataCommand_h
#define __vtkPlusGetPolydataCommand_h

#include "vtkPlusServerExport.h"
#include "vtkPlusCommand.h"

/*!
  \class vtkPlusGetPolydataCommand
  \brief This command is used to answer the OpenIGTLink message GET_POLYDATA. GET_POLYDATA returns the requested polydata.
  \ingroup PlusLibPlusServer
 */
class vtkPlusServerExport vtkPlusGetPolydataCommand : public vtkPlusCommand
{
public:

  static vtkPlusGetPolydataCommand* New();
  vtkTypeMacro(vtkPlusGetPolydataCommand, vtkPlusCommand);
  virtual void PrintSelf(ostream& os, vtkIndent indent);
  virtual vtkPlusCommand* Clone() { return New(); }

  /*! Read command parameters from XML */
  virtual PlusStatus ReadConfiguration(vtkXMLDataElement* aConfig);

  /*! Write command parameters to XML */
  virtual PlusStatus WriteConfiguration(vtkXMLDataElement* aConfig);

  /*! Executes the command  */
  virtual PlusStatus Execute();

  /*! Get all the command names that this class can execute */
  virtual void GetCommandNames(std::list<std::string>& cmdNames);

  /*! Gets the description for the specified command name. */
  virtual std::string GetDescription(const std::string& commandName);

  void SetNameToGetPolydata();

  /*! Id of the device */
  vtkGetStdStringMacro(PolydataId);
  vtkSetStdStringMacro(PolydataId);

protected:
  /*! Prepare sending image as a response */
  PlusStatus ExecutePolydataReply(std::string& outErrorString);

  vtkPlusGetPolydataCommand();
  virtual ~vtkPlusGetPolydataCommand();

protected:
  std::string PolydataId;

private:
  vtkPlusGetPolydataCommand(const vtkPlusGetPolydataCommand&);
  void operator=(const vtkPlusGetPolydataCommand&);
};

#endif