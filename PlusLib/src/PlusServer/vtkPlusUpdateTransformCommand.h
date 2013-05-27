/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/ 

#ifndef __vtkPlusUpdateTransformCommand_h
#define __vtkPlusUpdateTransformCommand_h

#include "vtkPlusCommand.h"

class vtkMatrix4x4;

/*!
  \class vtkPlusUpdateTransformCommand 
  \brief This command updates the value of a transformation in the transform repository
  \ingroup PlusLibPlusServer
 */ 
class VTK_EXPORT vtkPlusUpdateTransformCommand : public vtkPlusCommand
{
public:

  static vtkPlusUpdateTransformCommand *New();
  vtkTypeMacro(vtkPlusUpdateTransformCommand, vtkPlusCommand);
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
  
  vtkGetStringMacro(TransformName);
  vtkSetStringMacro(TransformName);

  vtkGetObjectMacro(TransformValue, vtkMatrix4x4);
  vtkSetObjectMacro(TransformValue, vtkMatrix4x4);

  vtkGetStringMacro(TransformDate);
  vtkSetStringMacro(TransformDate);

  vtkGetMacro(TransformError, double);
  void SetTransformError(double _arg){ this->TransformError = _arg; hasError = true; }

  vtkGetMacro(TransformPersistent, bool);
  void SetTransformPersistent(bool _arg){ this->TransformPersistent = _arg; hasPersistent = true; }

  void SetNameToUpdateTransform();

protected:

  vtkPlusUpdateTransformCommand();
  virtual ~vtkPlusUpdateTransformCommand();
  
private:
  char* TransformName;
  vtkMatrix4x4* TransformValue;
  bool hasMatrix;
  bool TransformPersistent;
  bool hasPersistent;
  double TransformError;
  bool hasError;
  char* TransformDate;
  bool hasDate;

  vtkPlusUpdateTransformCommand( const vtkPlusUpdateTransformCommand& );
  void operator=( const vtkPlusUpdateTransformCommand& );
};


#endif