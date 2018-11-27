/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __VTKPLUSCOMMAND_H
#define __VTKPLUSCOMMAND_H

#include "vtkPlusServerExport.h"

class vtkPlusDataCollector;
class vtkPlusCommandProcessor;
//class vtkIGSIOTransformRepository;
class vtkImageData;

#include "vtkPlusCommandResponse.h"

// igtl includes
#include "igtlMessageBase.h"

/*!
  \class vtkPlusCommand
  \brief This is an abstract superclass for commands in the OpenIGTLink network interface for Plus.

  All commands have a unique string representation to enable sending commands as string messages.
  For e.g. through OpenIGTLink.

  \ingroup PlusLibPlusServer
*/
class vtkPlusServerExport vtkPlusCommand : public vtkObject
{
public:
  static const std::string DEVICE_NAME_COMMAND;
  static const std::string DEVICE_NAME_REPLY;

  virtual vtkPlusCommand* Clone() = 0;

  virtual void PrintSelf(ostream& os, vtkIndent indent);

  /*!
    Executes the command
    \param resultString Command result in a human-readable string
  */
  virtual PlusStatus Execute() = 0;

  /*! Read command parameters from XML */
  virtual PlusStatus ReadConfiguration(vtkXMLDataElement* aConfig);

  /*! Write command parameters to XML */
  virtual PlusStatus WriteConfiguration(vtkXMLDataElement* aConfig);

  /*! Set the command processor to get access to the data collection devices and other commands */
  virtual void SetCommandProcessor(vtkPlusCommandProcessor* processor);

  /*! Set the id of the client that requested the command */
  vtkSetMacro(ClientId, int);
  vtkGetMacro(ClientId, int);

  /*!
    Gets the description for the specified command name. Command name is specified because a command object
    may be able to execute different commands.
    \param commandName Command name to provide the description for. If the pointer is NULL then all the supported commands shall be described.
  */
  virtual std::string GetDescription(const std::string& commandName) = 0;

  /*! Returns the list of command names that this command can process */
  virtual void GetCommandNames(std::list<std::string>& cmdNames) = 0;

  void SetMetaData(const igtl::MessageBase::MetaDataMap& metaData);

  vtkGetMacro(RespondWithCommandMessage, bool);
  vtkSetMacro(RespondWithCommandMessage, bool);

  vtkSetStdStringMacro(Name);
  vtkGetStdStringMacro(Name);

  vtkSetStdStringMacro(DeviceName);
  vtkGetStdStringMacro(DeviceName);

  vtkGetMacro(Id, uint32_t);
  vtkSetMacro(Id, uint32_t);

  /*!
    Get command responses from the device, append them to the provided list, and then remove them from the command.
    The ownership of the command responses are transferred to the caller, it is responsible
    for deleting them.
  */
  void PopCommandResponses(PlusCommandResponseList& responses);

  /*!
    LEGACY - for supporting receiving commands from OpenIGTLink v1/v2 clients

    Generates a command reply device name from a specified unique identifier (UID).
    The device name is "ACK_uidvalue" (if the UID is empty then the device name is "ACK").
  */
  static std::string GenerateReplyDeviceName(uint32_t uid);

  /*!
    LEGACY - for supporting receiving commands from OpenIGTLink v1/v2 clients

    Generates a command device name from a specified unique identifier (UID).
    The device name is "CMD_uidvalue" (if the UID is empty then the funtion fails).
  */
  static PlusStatus GenerateCommandDeviceName(const std::string& uid, std::string& outDeviceName);

  /*!
    LEGACY - for supporting receiving commands from OpenIGTLink v1/v2 clients

    Checks if a deviceName is a command. For example: CMD_13
  */
  static bool IsCommandDeviceName(const std::string& deviceName);

  /*!
    LEGACY - for supporting receiving commands from OpenIGTLink v1/v2 clients

    Checks if a deviceName is a reply to a command. For example: ACK_13 is a reply to CMD_13
  */
  static bool IsReplyDeviceName(const std::string& deviceName, const std::string& uid = std::string(""));

  /*!
    Gets the uid from a device name (e.g., device name is CMD_abc123, it returns abc123)
  */
  static std::string GetUidFromCommandDeviceName(const std::string& deviceName);

  /*!
    Gets the prefix from a device name (e.g., device name is CMD_abc123, it returns CMD)
  */
  static std::string GetPrefixFromCommandDeviceName(const std::string& deviceName);

protected:
  /*! Convenience function for getting a pointer to the data collector */
  virtual vtkPlusDataCollector* GetDataCollector();

  /*! Convenience function for getting a pointer to the transform repository */
  virtual vtkIGSIOTransformRepository* GetTransformRepository();

  /*! Check if the command name is in the list of command names */
  PlusStatus ValidateName();

  /*! Helper method to add a command response to the response queue */
  void QueueCommandResponse(PlusStatus status, const std::string& message, const std::string& error = "", const igtl::MessageBase::MetaDataMap* metaData = nullptr);

  vtkPlusCommand();
  virtual ~vtkPlusCommand();

  vtkPlusCommandProcessor* CommandProcessor;

  /*! Unique identifier of the Client that the response(s) will be sent to */
  int ClientId;

  /*! Device name of the received command. Reply device name is DeviceNameReply by default. */
  std::string DeviceName;

  /*! Unique identifier of the command. It can be used to match commands and replies. */
  uint32_t Id;

  /*! Should we respond using igtl::StringMessage or igtl::CommandMessage */
  bool RespondWithCommandMessage;

  /*!
    Name of the command. One command class may handle multiple commands, this Name member defines
    which of the supported command should be executed.
  */
  std::string Name;

  /*!
    Meta data passed in from the igtl message
  */
  igtl::MessageBase::MetaDataMap MetaData;

  // Contains a list of command responses that should be forwarded to the caller
  PlusCommandResponseList CommandResponseQueue;

private:
  vtkPlusCommand(const vtkPlusCommand&);
  void operator=(const vtkPlusCommand&);

};

#endif
