/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkPlusSetCameraControlParameterCommand_h
#define __vtkPlusSetCameraControlParameterCommand_h

#include "vtkPlusServerExport.h"

#include "vtkPlusCommand.h"

/*!
  \class vtkPlusSetCameraControlParameterCommand
  \brief This command requests camera control parameter change in the client
  \ingroup PlusLibPlusServer
 */
class vtkPlusServerExport vtkPlusSetCameraControlParameterCommand : public vtkPlusCommand
{
public:

  static vtkPlusSetCameraControlParameterCommand* New();
  vtkTypeMacro(vtkPlusSetCameraControlParameterCommand, vtkPlusCommand);
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

  /*! Id of the camera device to change the parameters of at the next Execute */
  vtkGetStdStringMacro(CameraDeviceId);
  vtkSetStdStringMacro(CameraDeviceId);

  void SetNameToSetCameraControlParameter();

protected:
  vtkPlusDevice* GetCameraDevice();

  vtkPlusSetCameraControlParameterCommand();
  virtual ~vtkPlusSetCameraControlParameterCommand();

protected:
  std::string CameraDeviceId;

  /*!
     List of requested parameter changes.
     Key is the parameter name (e.g. DepthMm), value is the parameter value.
     The Execute function traverses this map and requests the parameter changes from the device.
  */
  std::map<std::string, std::string> RequestedParameterChanges;

  vtkPlusSetCameraControlParameterCommand(const vtkPlusSetCameraControlParameterCommand&);
  void operator=(const vtkPlusSetCameraControlParameterCommand&);
};

#endif
