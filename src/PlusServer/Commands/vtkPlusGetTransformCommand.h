/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkPlusGetTransformCommand_h
#define __vtkPlusGetTransformCommand_h

#include "vtkPlusServerExport.h"
#include "vtkPlusCommand.h"

class vtkMatrix4x4;

/*!
  \class vtkPlusGetTransformCommand
  \brief This command retrieves the value of a transformation in the transform repository
  \ingroup PlusLibPlusServer
 */
class vtkPlusServerExport vtkPlusGetTransformCommand : public vtkPlusCommand
{
public:
  static vtkPlusGetTransformCommand* New();
  vtkTypeMacro(vtkPlusGetTransformCommand, vtkPlusCommand);
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

  vtkGetStdStringMacro(TransformName);
  vtkSetStdStringMacro(TransformName);

  void SetNameToGetTransform();

protected:
  vtkPlusGetTransformCommand();
  virtual ~vtkPlusGetTransformCommand();

protected:
  std::string TransformName;

private:
  vtkPlusGetTransformCommand(const vtkPlusGetTransformCommand&);
  void operator=(const vtkPlusGetTransformCommand&);
};

#endif