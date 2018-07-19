/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkPlusGetUsParameterCommand_h
#define __vtkPlusGetUsParameterCommand_h

#include "vtkPlusServerExport.h"

#include "vtkPlusCommand.h"

class vtkPlusUsDevice;

/*!
  \class vtkPlusGetUsParameterCommand
  \brief This command requests ultrasound parameter change in the client
  \ingroup PlusLibPlusServer
 */
class vtkPlusServerExport vtkPlusGetUsParameterCommand : public vtkPlusCommand
{
public:

  static vtkPlusGetUsParameterCommand* New();
  vtkTypeMacro(vtkPlusGetUsParameterCommand, vtkPlusCommand);
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

  void SetNameToGetUsParameter();

protected:
  vtkPlusUsDevice* GetUsDevice();

  vtkPlusGetUsParameterCommand();
  virtual ~vtkPlusGetUsParameterCommand();

protected:
  std::string UsDeviceId;

  /*!
     List of requested parameter changes.
     Key is the parameter name (e.g. DepthMm), value is the parameter value.
     The Execute function traverses this map and requests the parameter changes from the device.
  */
  std::vector<std::string> RequestedParameters;

  vtkPlusGetUsParameterCommand(const vtkPlusGetUsParameterCommand&);
  void operator=(const vtkPlusGetUsParameterCommand&);
};

#endif
