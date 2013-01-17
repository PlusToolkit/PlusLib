/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/ 

#ifndef __vtkPlusReconstructVolumeCommand_h
#define __vtkPlusReconstructVolumeCommand_h

#include "vtkPlusCommand.h"

/*!
  \class vtkPlusReconstructVolumeCommand 
  \brief This command stops capturing with a vtkVirtualStreamDiscCapture capture on the server side. 
  \ingroup PlusLibDataCollection
 */ 
class VTK_EXPORT vtkPlusReconstructVolumeCommand : public vtkPlusCommand
{
public:
  
  static vtkPlusReconstructVolumeCommand *New();
  vtkTypeMacro(vtkPlusReconstructVolumeCommand, vtkObject);
  virtual void PrintSelf( ostream& os, vtkIndent indent );
  virtual vtkPlusCommand* Clone() { return New(); }

  /*! Read command parameters from XML */
  virtual PlusStatus ReadConfiguration(vtkXMLDataElement* aConfig);

  /*! Write command parameters to XML */
  virtual PlusStatus WriteConfiguration(vtkXMLDataElement* aConfig);

  /*! Executes the command  */
  virtual PlusStatus Execute();

  /*! Get all the command names that this class can execute */
  virtual void GetCommandNames(std::list<std::string> &cmdNames);

  /*! Gets the description for the specified command name. */
  virtual std::string GetDescription(const char* commandName);
  
  vtkSetStringMacro(InputSeqFilename);
  vtkGetStringMacro(InputSeqFilename);

  vtkSetStringMacro(OutputVolFilename);
  vtkGetStringMacro(OutputVolFilename);

protected:
  
  vtkPlusReconstructVolumeCommand();
  virtual ~vtkPlusReconstructVolumeCommand();  
  
private:

  char* InputSeqFilename;
  char* OutputVolFilename;

  vtkPlusReconstructVolumeCommand( const vtkPlusReconstructVolumeCommand& );
  void operator=( const vtkPlusReconstructVolumeCommand& );
  
};

#endif
