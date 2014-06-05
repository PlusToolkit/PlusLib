/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/ 

#ifndef __vtkPlusGetImageCommand_h
#define __vtkPlusGetImageCommand_h

#include "vtkPlusCommand.h"

/*!
  \class vtkPlusGetImageCommand 
  \brief This command is used to answer the OpenIGTLink messages "GET_IMGMETA" and "GET_IMAGE". "GET_IMGMETA" returns all the image information from the devices that are connected at the time given. 
  \      "GET_IMAGE" returns the requested volume and the ijkToRasTransform which belongs to the volume 
	\ingroup PlusLibPlusServer
 */ 
class VTK_EXPORT vtkPlusGetImageCommand : public vtkPlusCommand
{
public:
  
  static vtkPlusGetImageCommand *New();
  vtkTypeMacro(vtkPlusGetImageCommand, vtkPlusCommand);
  virtual void PrintSelf( ostream& os, vtkIndent indent );
  virtual vtkPlusCommand* Clone() { return New(); }

  /*! Executes the command  */
  virtual PlusStatus Execute();  


  /*! Get all the command names that this class can execute */
  virtual void GetCommandNames(std::list<std::string> &cmdNames);

  /*! Gets the description for the specified command name. */
  virtual std::string GetDescription(const char* commandName);

  void SetNameToGetImageMeta(); // TODO patientName, patientID , description do similiar for patientId and description
  void SetNameToGetImage();

	/*! Id of the device */
  vtkGetStringMacro(DeviceId);
  vtkSetStringMacro(DeviceId);

protected:

  /*! Saves image to disk (if requested) and prepare sending image as a response (if requested) */
  PlusStatus ProcessImageReply(vtkDataCollector*);

	PlusStatus ProcessImageMetaReply(vtkDataCollector*);

  vtkPlusGetImageCommand();
  virtual ~vtkPlusGetImageCommand();  

private:

	char* DeviceId;

  vtkPlusGetImageCommand( const vtkPlusGetImageCommand& );
  void operator=( const vtkPlusGetImageCommand& );
  
};

#endif
