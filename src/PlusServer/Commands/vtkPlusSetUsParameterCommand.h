/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkPlusSetUsParameterCommand_h
#define __vtkPlusSetUsParameterCommand_h

#include "vtkPlusServerExport.h"

#include "vtkPlusCommand.h"

class vtkPlusUsDevice;

/*!
  \class vtkPlusSetUsParameterCommand
  \brief This command requests ultrasound parameter change in the client
  \ingroup PlusLibPlusServer
 */
class vtkPlusServerExport vtkPlusSetUsParameterCommand : public vtkPlusCommand
{
public:

  static vtkPlusSetUsParameterCommand* New();
  vtkTypeMacro(vtkPlusSetUsParameterCommand, vtkPlusCommand);
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

  /*! Id of the ultrasound device to change the parameters of at the next Execute */
  vtkGetStdStringMacro(UsDeviceId);
  vtkSetStdStringMacro(UsDeviceId);

  void SetNameToSetUsParameter();

protected:
  vtkPlusUsDevice* GetUsDevice();

  vtkPlusSetUsParameterCommand();
  virtual ~vtkPlusSetUsParameterCommand();

protected:
  std::string UsDeviceId;

  /*!
     List of requested parameter changes.
     Key is the parameter name (e.g. DepthMm), value is the parameter value.
     The Execute function traverses this map and requests the parameter changes from the device.
  */
  std::map<std::string, std::string> RequestedParameterChanges;

  vtkPlusSetUsParameterCommand(const vtkPlusSetUsParameterCommand&);
  void operator=(const vtkPlusSetUsParameterCommand&);
};

#endif
