/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkPlusClariusCommand_h
#define __vtkPlusClariusCommand_h

#include "vtkPlusServerExport.h"

#include "vtkPlusCommand.h"
#include "vtkIGSIOTransformRepository.h"

class vtkPlusClarius;

/*!
  \class vtkPlusClariusCommand
  \brief This command reconstructs a volume from an image sequence and saves it to disk or sends it to the client in an IMAGE message.
  \ingroup PlusLibPlusServer
 */
class vtkPlusServerExport vtkPlusClariusCommand : public vtkPlusCommand
{
public:
  static vtkPlusClariusCommand* New();
  vtkTypeMacro(vtkPlusClariusCommand, vtkPlusCommand);
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

  /*!
  Set the command to get the raw data from the clarius
  See: https://support.clarius.com/hc/en-us/articles/360019787932-Raw-Data-Collection
  */
  void SetNameToSaveRawData();

  /*!
  Compress raw data using gzip if enabled
  */
  vtkGetMacro(CompressRawData, bool);
  vtkSetMacro(CompressRawData, bool);
  vtkBooleanMacro(CompressRawData, bool);

  /*! Id of the Clarius device */
  vtkGetStdStringMacro(ClariusDeviceId);
  vtkSetStdStringMacro(ClariusDeviceId);

  /*!
  Output filename of the raw Clarius data
  Should be a .tar file
  */
  vtkGetStdStringMacro(OutputFilename);
  vtkSetStdStringMacro(OutputFilename);

  /*!
  The number of seconds of raw data to retrieve
  */
  vtkGetMacro(RawDataLastNSeconds, double);
  vtkSetMacro(RawDataLastNSeconds, double);

protected:
  vtkPlusClarius* GetClariusDevice();

  vtkPlusClariusCommand();
  virtual ~vtkPlusClariusCommand();

protected:
  bool CompressRawData;
  std::string ClariusDeviceId;
  std::string OutputFilename;
  double      RawDataLastNSeconds;

private:
  vtkPlusClariusCommand(const vtkPlusClariusCommand&);
  void operator=(const vtkPlusClariusCommand&);
};

#endif
