/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkPlusStartStopRecordingCommand_h
#define __vtkPlusStartStopRecordingCommand_h

#include "vtkPlusServerExport.h"

#include "vtkPlusCommand.h"

class vtkPlusVirtualCapture;
class vtkPlusDataCollector;

/*!
  \class vtkPlusStartStopRecordingCommand
  \brief This command starts and stops capturing with a vtkPlusVirtualCapture capture on the server side.
  \ingroup PlusLibPlusServer
 */
class vtkPlusServerExport vtkPlusStartStopRecordingCommand : public vtkPlusCommand
{
public:

  static vtkPlusStartStopRecordingCommand* New();
  vtkTypeMacro(vtkPlusStartStopRecordingCommand, vtkPlusCommand);
  virtual void PrintSelf(ostream& os, vtkIndent indent);
  virtual vtkPlusCommand* Clone();

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

  vtkGetStdStringMacro(OutputFilename);
  vtkSetStdStringMacro(OutputFilename);

  vtkGetStdStringMacro(CaptureDeviceId);
  vtkSetStdStringMacro(CaptureDeviceId);

  vtkGetStdStringMacro(ChannelId);
  vtkSetStdStringMacro(ChannelId);

  vtkGetMacro(EnableCompression, bool);
  vtkSetMacro(EnableCompression, bool);

  vtkGetStdStringMacro(CodecFourCC);
  vtkSetStdStringMacro(CodecFourCC);

  void SetNameToStart();
  void SetNameToSuspend();
  void SetNameToResume();
  void SetNameToStop();

  /*!
    Helper function to get pointer to the capture device
    \param captureDeviceId Capture device ID. If it is NULL then a pointer to the first VirtualStreamCapture device is returned.
  */
  vtkPlusVirtualCapture* GetCaptureDevice(const std::string& captureDeviceId);

  /*!
  Helper function to get pointer to the capture device
  \param captureDeviceId Capture device ID. If it is NULL then a pointer to the first VirtualStreamCapture device is returned.
  */
  vtkPlusVirtualCapture* GetOrCreateCaptureDevice(const std::string& channelId);

protected:
  vtkPlusStartStopRecordingCommand();
  virtual ~vtkPlusStartStopRecordingCommand();

private:
  bool        EnableCompression;
  std::string CodecFourCC;
  std::string OutputFilename;
  std::string CaptureDeviceId;
  std::string ChannelId;

  vtkPlusStartStopRecordingCommand(const vtkPlusStartStopRecordingCommand&);
  void operator=(const vtkPlusStartStopRecordingCommand&);

};

#endif