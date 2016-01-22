/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/ 

#ifndef __vtkPlusCommandProcessor_h
#define __vtkPlusCommandProcessor_h

#include "vtkPlusServerExport.h"

#include "vtkMultiThreader.h"
#include "vtkObject.h"
#include "vtkPlusCommand.h"
#include "vtkPlusCommandResponse.h"
#include "vtkPlusOpenIGTLinkServer.h"
#include <string>

class vtkImageData;
class vtkMatrix4x4;

/*!
  \class vtkPlusCommandProcessor 
  \brief Creates a PlusCommand from a string.
  If the commands are to be executed on the main thread then call ExecuteCommands() periodically from the main thread.
  If the commands are to be executed on a separate thread (to allow background processing, but maybe requiring more synchronization) call Start() to start an internal processing thread. 
  Probably one of the processing models would be enough, but at this point it's not clear which one is better.
  TODO: keep only one method and remove the other approach completely once the processing model decision is finalized.
  \ingroup PlusLibPlusServer
*/
class vtkPlusServerExport vtkPlusCommandProcessor : public vtkObject
{
public:

  static vtkPlusCommandProcessor *New();
  vtkTypeMacro(vtkPlusCommandProcessor, vtkObject);
  void PrintSelf(ostream& os, vtkIndent indent);   

  /*! 
    Execute all commands in the queue from the current thread (useful if commands should be executed from the main thread) 
    \return Number of executed commands
  */
  int ExecuteCommands();

  /*! Start thread for processing the commands in the queue. Must be called from the main thread. */
  virtual PlusStatus Start();

  /*! Stop command processing. Must be called from the main thread. */
  virtual PlusStatus Stop();

  /*! Returns true if the command processing thread is running. Can be called from any thread. */
  virtual bool IsRunning();

  /*!
    Register custom command. Must be called from the main thread.
    \param cmd It should point to a valid vtkPlusCommand instance. The caller can delete the cmd object after the call.
  */
  virtual PlusStatus RegisterPlusCommand(vtkPlusCommand *cmd);

  /*! Adds a command to the queue for execution. Can be called from any thread.  */
  virtual PlusStatus QueueCommand(unsigned int clientId, const std::string &commandString, const std::string &deviceName, const std::string& uid); 

  /*! Adds a command to the queue for execution of the vtkGetImageCommand with the name GET_IMGMETA!*/
  PlusStatus QueueGetImageMetaData(unsigned int clientId, const std::string &deviceName);

  /*! Adds a command to the queue for execution of the vtkGetImageCommand with the name GET_IMAGE !*/
  PlusStatus QueueGetImage(unsigned int clientId, const std::string &deviceName);

  /*!
    Return the queued command responses and removes the items from the queue (so that each item is returned only once) and clears the response queue.
    The caller is responsible for deleting the returned response objects.
    Can be called from any thread.
  */
  virtual void PopCommandResponses(PlusCommandResponseList &responses);

  vtkGetObjectMacro(PlusServer, vtkPlusOpenIGTLinkServer);
  vtkSetObjectMacro(PlusServer, vtkPlusOpenIGTLinkServer); 

protected:
  vtkPlusCommand* CreatePlusCommand(const std::string &commandStr);

  /*! Thread for client connection handling */ 
  static void* CommandExecutionThread( vtkMultiThreader::ThreadInfo* data );

  vtkPlusCommandProcessor();
  virtual ~vtkPlusCommandProcessor();

private:

  vtkPlusOpenIGTLinkServer *PlusServer;

    /*! Multithreader instance for controlling threads */ 
  vtkSmartPointer<vtkMultiThreader> Threader;

  /*! Mutex instance for safe data access */ 
  vtkSmartPointer<vtkRecursiveCriticalSection> Mutex;

  // Active flag for threads (first: request, second: respond )
  std::pair<bool,bool> CommandExecutionActive;

  // Thread identifier
  int  CommandExecutionThreadId;

  /*! Map command names and the New() static methods of vtkPlusCommand classes */ 
  std::map<std::string,vtkPlusCommand*> RegisteredCommands; 

  /*! 
    This queue contains all the active commands. 
    After a command's execute method is called it may still remain active (remain in the queue),
    until it signals that it is completed.
  */	
  //std::list<vtkPlusCommand*> CommandQueue;
  typedef std::list< vtkSmartPointer<vtkPlusCommand> > PlusCommandList;
  PlusCommandList CommandQueue;
  PlusCommandResponseList CommandResponseQueue;

  vtkPlusCommandProcessor(const vtkPlusCommandProcessor&);  // Not implemented.
  void operator=(const vtkPlusCommandProcessor&);  // Not implemented.
};

#endif
