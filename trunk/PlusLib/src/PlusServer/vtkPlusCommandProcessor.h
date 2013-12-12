/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/ 

#ifndef __vtkPlusCommandProcessor_h
#define __vtkPlusCommandProcessor_h

#include "vtkMultiThreader.h"
#include "vtkObject.h"
#include "vtkPlusOpenIGTLinkServer.h"
#include <deque>
#include <string>

class vtkPlusCommand;
class vtkImageData;
class vtkMatrix4x4;

struct PlusCommandReply
{
  PlusCommandReply()
  : ClientId(0), ImageData(NULL), ImageToReferenceTransform(NULL)
  {
  }
  unsigned int ClientId;
  PlusStatus Status;
  std::string DeviceName;
  std::string CustomAttributes;
  std::string ImageName;
  vtkImageData* ImageData;
  vtkMatrix4x4* ImageToReferenceTransform;
};

typedef std::list<PlusCommandReply> PlusCommandReplyList;

/*!
  \class vtkPlusCommandProcessor 
  \brief Creates a PlusCommand from a string.
  If the commands are to be executed on the main thread then call ExecuteCommands() periodically from the main thread.
  If the commands are to be executed on a separate thread (to allow background processing, but maybe requiring more synchronization) call Start() to start an internal processing thread. 
  Probably one of the processing models would be enough, but at this point it's not clear which one is better.
  TODO: keep only one method and remove the other approach completely once the processing model decision is finalized.
  \ingroup PlusLibDataCollection
*/
class
VTK_EXPORT
vtkPlusCommandProcessor : public vtkObject
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

  /*! Return the queued command replies and removes the items from the queue (so that each item is returned only once). Can be called from any thread. */
  virtual PlusStatus GetCommandReplies(PlusCommandReplyList &replies);

  /*! Adds a reply to the queue for sending to a client. Can be called from any thread.  */
  virtual void QueueReply(int clientId, PlusStatus replyStatus, const std::string& replyString, const std::string& replyDeviceName, const char* imageName=NULL, vtkImageData* imageData=NULL, vtkMatrix4x4* imageToReferenceTransform=NULL);

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
  std::deque<vtkPlusCommand*> CommandQueue;
  PlusCommandReplyList CommandReplies;

  vtkPlusCommandProcessor(const vtkPlusCommandProcessor&);  // Not implemented.
  void operator=(const vtkPlusCommandProcessor&);  // Not implemented.
};

#endif
