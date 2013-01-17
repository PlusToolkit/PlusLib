/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/ 

#ifndef __vtkPlusCommandProcessor_h
#define __vtkPlusCommandProcessor_h

#include <deque>
#include <string>

#include "vtkObject.h"
#include "vtkMultiThreader.h"

#include "vtkDataCollector.h"

class vtkPlusCommand;

struct PlusCommandReply
{
  unsigned int ClientId;
  std::string ReplyString;
};

typedef std::list<PlusCommandReply> PlusCommandReplyList;

/*!
  \class vtkPlusCommandProcessor 
  \brief Creates a PlusCommand from a string.
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
  virtual PlusStatus QueueCommand(unsigned int clientId, const std::string &commandString); 

  /*! Return the queued command replies and removes the items from the queue (so that each item is returned only once). Can be called from any thread. */
  virtual PlusStatus GetCommandReplies(PlusCommandReplyList &replies);

  /*! Adds a reply to the queue for sending to a client. Can be called from any thread.  */
  virtual void QueueReply(int clientId, PlusStatus replyStatus, const std::string& replyString);

  vtkGetObjectMacro(DataCollector, vtkDataCollector);
  vtkSetObjectMacro(DataCollector, vtkDataCollector); 
    
protected:
  vtkPlusCommand* CreatePlusCommand(const std::string &commandStr);

  /*! Thread for client connection handling */ 
  static void* CommandExecutionThread( vtkMultiThreader::ThreadInfo* data );

  vtkPlusCommandProcessor();
  virtual ~vtkPlusCommandProcessor();

private:

  vtkDataCollector *DataCollector;

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
  std::deque<vtkPlusCommand*> ActiveCommands;
  PlusCommandReplyList CommandReplies;

  vtkPlusCommandProcessor(const vtkPlusCommandProcessor&);  // Not implemented.
  void operator=(const vtkPlusCommandProcessor&);  // Not implemented.
};

#endif
