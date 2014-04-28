/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/ 

#ifndef __VTKPLUSCOMMAND_H
#define __VTKPLUSCOMMAND_H

class vtkDataCollector;
class vtkPlusCommandProcessor;
class vtkTransformRepository;
class vtkImageData;

/*!
  \class vtkPlusCommand 
  \brief This is an abstract superclass for commands in the OpenIGTLink network interface for Plus.

  All commands have a unique string representation to enable sending commands as string messages.
  For e.g. through OpenIGTLink.
  
  \ingroup PlusLibPlusServer
*/
class VTK_EXPORT vtkPlusCommand : public vtkObject
{
public:

  virtual vtkPlusCommand* Clone() = 0;

  virtual void PrintSelf( ostream& os, vtkIndent indent );
    
  /*!
    Executes the command 
    \param resultString Command result in a human-readable string
  */
  virtual PlusStatus Execute()=0;

  /*! Read command parameters from XML */
  virtual PlusStatus ReadConfiguration(vtkXMLDataElement* aConfig);

  /*! Write command parameters to XML */
  virtual PlusStatus WriteConfiguration(vtkXMLDataElement* aConfig);

  /*! Set the command processor to get access to the data collection devices and other commands */
  virtual void SetCommandProcessor( vtkPlusCommandProcessor* processor );  

  /*! Set the id of the client that requested the command */
  virtual void SetClientId( int clientId );  
  vtkGetMacro(ClientId, int);

  /*! 
    Gets the description for the specified command name. Command name is specified because a command object
    may be able to execute different commands.
    \param commandName Command name to provide the description for. If the pointer is NULL then all the supported commands shal be described.
  */
  virtual std::string GetDescription(const char* commandName)=0;

  /*! Returns the list of command names that this command can process */
  virtual void GetCommandNames(std::list<std::string> &cmdNames)=0;
  
  vtkGetStringMacro(Name);
  vtkSetStringMacro(Name);

  vtkGetStringMacro(DeviceName);
  vtkSetStringMacro(DeviceName);

  vtkGetStringMacro(Id);
  vtkSetStringMacro(Id);

  std::string GetResponseMessage() { return this->ResponseMessage; };
  vtkGetMacro(ResponseImage, vtkImageData*);
  std::string GetResponseImageDeviceName() { return this->ResponseImageDeviceName; };
  vtkGetMacro(ResponseImageToReferenceTransform, vtkMatrix4x4*);

  /*!
    Generates a command device name from a specified unique identifier (UID).
    The device name is "CMD_uidvalue" (if the UID is empty then the device name is "CMD").
  */
  static std::string GenerateCommandDeviceName(const std::string &uid);

  /*!
    Generates a command reply device name from a specified unique identifier (UID).
    The device name is "ACK_uidvalue" (if the UID is empty then the device name is "ACK").
  */
  static std::string GenerateReplyDeviceName(const std::string &uid);

  /*!
    Returns true if the device name is an acknowledgment.
    If the uid is non-empty then it returns true only if it acknowledges the command with the specified uid.
  */
  static bool IsReplyDeviceName(const std::string &deviceName, const std::string &uid);

  /*!
    Gets the uid from a device name (e.g., device name is CMD_abc123, it returns abc123)
  */
  static std::string GetUidFromCommandDeviceName(const std::string &deviceName);

  /*!
    Gets the prefix from a device name (e.g., device name is CMD_abc123, it returns CMD)
  */
  static std::string GetPrefixFromCommandDeviceName(const std::string &deviceName);

  /*! Returns the default reply device name, which conforms to the new CMD/ACQ protocol */
  std::string GetReplyDeviceName();

protected:
  /*! Convenience function for getting a pointer to the data collector */
  virtual vtkDataCollector* GetDataCollector();

  /*! Convenience function for getting a pointer to the transform repository */
  virtual vtkTransformRepository* GetTransformRepository();

  /*! Check if the command name is in the list of command names */
  PlusStatus ValidateName();

  /*! Clears all the execute response member variables */
  void ResetResponse();

  void SetResponseImage(vtkImageData *imageData);
  void SetResponseImageToReferenceTransform(vtkMatrix4x4 *matrix);
  
  vtkPlusCommand();
  virtual ~vtkPlusCommand();
    
  vtkPlusCommandProcessor* CommandProcessor;
  int ClientId;
  
  // Device name of the received command. Reply device name is DeviceNameReply by default.
  char* DeviceName;
  
  // Unique identifier of the command. It can be used to match commands and replies.
  char* Id;

  char* Name;

  // STRING message
  std::string ResponseMessage;

  // IMAGE message
  vtkImageData* ResponseImage;
  std::string ResponseImageDeviceName;
  vtkMatrix4x4* ResponseImageToReferenceTransform;

  // TRANSFORM message
  //std::string ResponseTransformDeviceName;
  //vtkMatrix4x4* ResponseTransformMatrix;
      
private:  
  vtkPlusCommand( const vtkPlusCommand& );
  void operator=( const vtkPlusCommand& );
  
};

#endif
