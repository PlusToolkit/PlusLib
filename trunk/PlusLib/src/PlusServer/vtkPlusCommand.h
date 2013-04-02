/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/ 

#ifndef __VTKPLUSCOMMAND_H
#define __VTKPLUSCOMMAND_H

class vtkPlusCommandProcessor;
class vtkDataCollector;

/*!
  \class vtkPlusCommand 
  \brief This is an abstract superclass for commands in the OpenIGTLink network interface for Plus.

  All commands have a unique string representation to enable sending commands as string messages.
  For e.g. through OpenIGTLink.
  
  \ingroup PlusLibDataCollection
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

  /*! Set the command processor to get access to the data collection devices and other commands */
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

  /*! Returns true if the command has been completed and no more need to call its Execute function */
  virtual bool IsCompleted();
  
  vtkGetStringMacro(Name);
  vtkSetStringMacro(Name);

  vtkGetStringMacro(DeviceName);
  vtkSetStringMacro(DeviceName);

  vtkGetMacro(Id, int);
  vtkSetMacro(Id, int);

  /*! Returns the default reply device name, which is the same as receiver device name with the "Reply" appended to the end */
  static std::string GetDefaultReplyDeviceName(const std::string& aDeviceName);

protected:

  /*! Send a reply to the caller and set the status of the command completed (so it can be removed from the queue) */
  PlusStatus SetCommandCompleted(PlusStatus replyStatus, const std::string& replyString, const std::string& replyDeviceName);

  /*! Sends a reply to the default reply device with the default reply device name */
  PlusStatus SetCommandCompleted(PlusStatus replyStatus, const std::string& replyString);

  /*! Convenience function for getting a pointer to the data collector */
  virtual vtkDataCollector* GetDataCollector();

  /*! Check if the command name is in the list of command names */
  PlusStatus ValidateName();
  
  vtkPlusCommand();
  virtual ~vtkPlusCommand();
    
  vtkPlusCommandProcessor* CommandProcessor;
  bool Completed;
  int ClientId;
  
  // Device name of the received command. Reply device name is DeviceNameReply by default.
  char* DeviceName;
  
  // Unique identifier of the command. It can be used to match commands and replies.
  int Id;

  char* Name;
      
private:	
  vtkPlusCommand( const vtkPlusCommand& );
  void operator=( const vtkPlusCommand& );
  
};

#endif
