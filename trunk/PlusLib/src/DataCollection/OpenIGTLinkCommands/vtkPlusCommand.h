/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/ 

#ifndef __VTKPLUSCOMMAND_H
#define __VTKPLUSCOMMAND_H

class vtkPlusCommandProcessor;

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
  void SetCommandProcessor( vtkPlusCommandProcessor* processor );  

  /*! Set the command processor to get access to the data collection devices and other commands */
  void SetClientId( int clientId );  

  /*! 
    Gets the description for the specified command name. Command name is specified because a command object
    may be able to execute different commands.
    \param commandName Command name to provide the description for. If the pointer is NULL then all the supported commands shal be described.
  */
  virtual const char* GetDescription(const char* commandName)=0;

  /*! Returns the list of command names that this command can process */
  virtual void GetCommandNames(std::list<std::string> &cmdNames)=0;

  /*! Returns true if the command has been completed and no more need to call its Execute function */
  virtual bool IsCompleted();
  
protected:

  /*! Send a reply to the caller and set the status of the command completed (so it can be removed from the queue) */
  PlusStatus SetCommandCompleted(PlusStatus replyStatus, const std::string& replyString);
  
  vtkPlusCommand();
  virtual ~vtkPlusCommand();
    
  vtkPlusCommandProcessor* CommandProcessor;
  bool Completed;
  int ClientId;
      
private:	
  vtkPlusCommand( const vtkPlusCommand& );
  void operator=( const vtkPlusCommand& );
  
};

#endif

