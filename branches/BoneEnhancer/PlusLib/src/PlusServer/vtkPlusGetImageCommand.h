/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/ 

#ifndef __vtkPlusGetImageCommand_h
#define __vtkPlusGetImageCommand_h

#include "vtkPlusServerExport.h"

#include "vtkPlusCommand.h"

/*!
  \class vtkPlusGetImageCommand 
  \brief This command is used to answer the OpenIGTLink messages "GET_IMGMETA" and "GET_IMAGE". "GET_IMGMETA" returns all the image information from the devices that are connected at the time given. 
  \      "GET_IMAGE" returns the requested volume and the ijkToRasTransform which belongs to the volume. The Ras coordinate system is the coordinate system defined in Plus as: "Ras".
  \ The image id is empty when the command is GET_IMGMETA, which means the data will be acquired from all of the connected devices.
  \ It is the id of the image selected on slicer for the command GET_IMAGE
  \ingroup PlusLibPlusServer
 */ 
class vtkPlusServerExport vtkPlusGetImageCommand : public vtkPlusCommand
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

  void SetNameToGetImageMeta();
  void SetNameToGetImage();

  /*! Id of the device */
  vtkGetStringMacro(ImageId);
  vtkSetStringMacro(ImageId);

protected:

  /*! Prepare sending image as a response */
  PlusStatus ExecuteImageReply();

  /*! Send the image meta datasets from all the connectede devices to slicer through openigtlink */
  PlusStatus ExecuteImageMetaReply();

  vtkPlusGetImageCommand();
  virtual ~vtkPlusGetImageCommand(); 

  /*! Return ImageMetaDatasetsCount as string*/
  std::string GetImageMetaDatasetsCountAsString();

  /*!  How many image meta datasets are in total in the connected devices */
  int ImageMetaDatasetsCount;

private:

  char* ImageId;

  vtkPlusGetImageCommand( const vtkPlusGetImageCommand& );
  void operator=( const vtkPlusGetImageCommand& );
  
};

#endif
