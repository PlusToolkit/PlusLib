/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

#include "PlusConfigure.h"
#include "vtkImageData.h"
#include "vtkMatrix4x4.h"
#include "vtkObjectFactory.h"
#include "vtkPlusCommand.h"
#include "vtkPlusCommandProcessor.h"
#include "vtkPlusReconstructVolumeCommand.h"
#include "vtkPlusRequestChannelIDsCommand.h"
#include "vtkPlusRequestDeviceIDsCommand.h"
#include "vtkPlusStartStopRecordingCommand.h"
#include "vtkRecursiveCriticalSection.h"
#include "vtkXMLUtilities.h"

vtkStandardNewMacro( vtkPlusCommandProcessor );

//----------------------------------------------------------------------------
vtkPlusCommandProcessor::vtkPlusCommandProcessor()
: Threader(vtkSmartPointer<vtkMultiThreader>::New())
, Mutex(vtkSmartPointer<vtkRecursiveCriticalSection>::New())
, CommandExecutionThreadId(-1)
, CommandExecutionActive(std::make_pair(false,false))
, PlusServer(NULL)
{
  // Register default commands
  {
    vtkPlusCommand* cmd=vtkPlusStartStopRecordingCommand::New();
    RegisterPlusCommand(cmd);
    cmd->Delete();
  }
  {
    vtkPlusCommand* cmd=vtkPlusReconstructVolumeCommand::New();
    RegisterPlusCommand(cmd);
    cmd->Delete();
  }
  {
    vtkPlusCommand* cmd=vtkPlusRequestDeviceIDsCommand::New();
    RegisterPlusCommand(cmd);
    cmd->Delete();
  }
  {
    vtkPlusCommand* cmd=vtkPlusRequestChannelIDsCommand::New();
    RegisterPlusCommand(cmd);
    cmd->Delete();
  }
}

//----------------------------------------------------------------------------
vtkPlusCommandProcessor::~vtkPlusCommandProcessor()
{
  for (std::map<std::string,vtkPlusCommand*>::iterator it=this->RegisteredCommands.begin(); it!=this->RegisteredCommands.end(); ++it)
  {
    (it->second)->UnRegister(this); 
    (it->second)=NULL; 
  } 
  this->RegisteredCommands.clear();

  SetPlusServer(NULL);
}

//----------------------------------------------------------------------------
void vtkPlusCommandProcessor::PrintSelf( ostream& os, vtkIndent indent )
{
  this->Superclass::PrintSelf(os, indent);
  os << indent << "Available Commands : ";
  // TODO: print registered commands
  /*
  if( AvailableCommands )
  {
    AvailableCommands->PrintSelf( os, indent );
  }
  else
  {
    os << "None.";
  }
  */
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusCommandProcessor::Start()
{
  if ( this->CommandExecutionThreadId < 0 )
  {
    this->CommandExecutionActive.first = true;
    this->CommandExecutionThreadId = this->Threader->SpawnThread( (vtkThreadFunctionType)&CommandExecutionThread, this );
  }
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusCommandProcessor::Stop()
{
  // Stop the command execution thread
  if ( this->CommandExecutionThreadId >=0 )
  {
    this->CommandExecutionActive.first = false; 
    while ( this->CommandExecutionActive.second )
    {
      // Wait until the thread stops 
      vtkAccurateTimer::Delay( 0.2 ); 
    }
    this->CommandExecutionThreadId = -1; 
  }

  LOG_DEBUG("Command execution thread stopped");

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
void* vtkPlusCommandProcessor::CommandExecutionThread( vtkMultiThreader::ThreadInfo* data )
{
  vtkPlusCommandProcessor* self = (vtkPlusCommandProcessor*)( data->UserData );

  self->CommandExecutionActive.second = true;   

  // Execute commands until a stop is requested  
  while ( self->CommandExecutionActive.first )
  {
    int numberOfExecutedCommands=self->ExecuteCommands();
    if (numberOfExecutedCommands==0)
    {
      // no commands in the queue, wait a bit before checking again
      const double commandQueuePollIntervalSec=0.010;
#ifdef _WIN32
      Sleep(commandQueuePollIntervalSec*1000);
#else
      usleep(commandQueuePollIntervalSec * 1000000);
#endif
    }
  }

  // Close thread
  self->CommandExecutionThreadId = -1;
  self->CommandExecutionActive.second = false; 
  return NULL;
}

//----------------------------------------------------------------------------
int vtkPlusCommandProcessor::ExecuteCommands()
{
  vtkPlusCommand* cmd=NULL; // next command to be processed
  int numberOfExecutedCommands=0;
  {
    PlusLockGuard<vtkRecursiveCriticalSection> updateMutexGuardedLock(this->Mutex);
    if (this->ActiveCommands.empty())
    {
      return numberOfExecutedCommands;
    }
    cmd=this->ActiveCommands.front();
  }
    
  // Execute each active command once and remove completed command from the queue if completed.
  // Implemented in a while loop to not block the mutex during command execution, only during management of the queue.
  while (1)
  {    
    LOG_DEBUG("Executing command");
    cmd->Execute();
    numberOfExecutedCommands++;

    {
      // Remove the current command from the queue if completed and get next command
      PlusLockGuard<vtkRecursiveCriticalSection> updateMutexGuardedLock(this->Mutex);
      // Find the current command in the queue
      std::deque< vtkPlusCommand* >::iterator cmdIt=std::find(this->ActiveCommands.begin(), this->ActiveCommands.end(), cmd);
      if (cmdIt==this->ActiveCommands.end())
      {
        // the command has been removed, it should not happen, as commands should be removed only when they are executed in this method
        LOG_ERROR("Command that was under execution was removed from the queue");
        return numberOfExecutedCommands;
      }      
      if (cmd->IsCompleted())
      {
        // the command execution is completed, so remove it from the queue of active commands
        cmd->UnRegister(this); // delete command
        cmdIt=this->ActiveCommands.erase(cmdIt);        
      }
      else
      {
        // the command has not been completed, so keep in the queue and progress to the next command
        ++cmdIt;
      }
      if (cmdIt==this->ActiveCommands.end())
      {
        // it was the last command in the queue
        break;
      }
      cmd=*cmdIt;
    }
  }

  return numberOfExecutedCommands;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusCommandProcessor::RegisterPlusCommand(vtkPlusCommand* cmd)
{
  if (cmd==NULL)
  {
    LOG_ERROR("vtkPlusCommandProcessor::RegisterPlusCommand received an invalid command object");
    return PLUS_FAIL;
  }
  std::list<std::string> cmdNames;
  cmd->GetCommandNames(cmdNames);
  if (cmdNames.empty())
  {
    LOG_ERROR("Cannot register command: command name is empty");
    return PLUS_FAIL;
  }
  for (std::list<std::string>::iterator nameIt=cmdNames.begin(); nameIt!=cmdNames.end(); ++nameIt)
  {
    this->RegisteredCommands[*nameIt]=cmd;
    cmd->Register(this);
  }
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
vtkPlusCommand* vtkPlusCommandProcessor::CreatePlusCommand(const std::string &commandStr)
{
  vtkSmartPointer<vtkXMLDataElement> cmdElement = vtkSmartPointer<vtkXMLDataElement>::Take( vtkXMLUtilities::ReadElementFromString(commandStr.c_str()) );
  if (cmdElement.GetPointer()==NULL)
  {
    LOG_ERROR("failed to parse XML command string (received: "+commandStr+")");
    return NULL;
  }
  if (STRCASECMP(cmdElement->GetName(),"Command")!=0)
  {
    LOG_ERROR("Command element expected (received: "+commandStr+")");
    return NULL;
  }
  const char* cmdName=cmdElement->GetAttribute("Name");
  if (cmdName==NULL)
  {
    LOG_ERROR("Command element's Name attribute is missing (received: "+commandStr+")");
    return NULL;
  }
  if (this->RegisteredCommands.find(cmdName) == this->RegisteredCommands.end())
  {
    // unregistered command
    LOG_ERROR("Unknown command: "<<cmdName);
    return NULL;
  }
  vtkPlusCommand* cmd=(this->RegisteredCommands[cmdName])->Clone();
  if (cmd->ReadConfiguration(cmdElement)!=PLUS_SUCCESS)
  {
    cmd->Delete();
    cmd=NULL;
    LOG_ERROR("Failed to initialize command from string: "+commandStr);
    return NULL;
  }  
  return cmd;
}

//------------------------------------------------------------------------------
PlusStatus vtkPlusCommandProcessor::QueueCommand(unsigned int clientId, const std::string &commandString, const std::string &deviceName)
{  
  if (commandString.empty())
  {
    LOG_ERROR("Command string is undefined");
    return PLUS_FAIL;
  }
  vtkPlusCommand* cmd=CreatePlusCommand(commandString);
  if (cmd==NULL)
  {
    LOG_ERROR("Failed to create command from string: "+commandString);
    return PLUS_FAIL;
  }
  cmd->SetCommandProcessor(this);
  cmd->SetClientId(clientId);
  cmd->SetDeviceName(deviceName.c_str());
  {
    // Add command to the execution queue
    PlusLockGuard<vtkRecursiveCriticalSection> updateMutexGuardedLock(this->Mutex);
    this->ActiveCommands.push_back(cmd);
  }
  return PLUS_SUCCESS;
}

//------------------------------------------------------------------------------
void vtkPlusCommandProcessor::QueueReply(int clientId, PlusStatus replyStatus, const std::string& replyString, const std::string& replyDeviceName, const char* imageName/*=NULL*/, vtkImageData* imageData/*=NULL*/, vtkMatrix4x4* imageToReferenceTransform/*=NULL*/)
{
  PlusCommandReply reply;
  reply.ClientId=clientId;
  reply.DeviceName=replyDeviceName;
  reply.ReplyString=replyString;
  reply.Status=replyStatus;
  if (imageName!=NULL)
  {
    reply.ImageName=imageName;
  }
  if (imageData!=NULL)
  {
    reply.ImageData=imageData;
    reply.ImageData->Register(NULL);
  }
  if (imageToReferenceTransform!=NULL)
  {
    reply.ImageToReferenceTransform=imageToReferenceTransform;
    reply.ImageToReferenceTransform->Register(NULL);
  }
  {
    // Add reply to the sending queue
    PlusLockGuard<vtkRecursiveCriticalSection> updateMutexGuardedLock(this->Mutex);
    this->CommandReplies.push_back(reply);
  }
}

//------------------------------------------------------------------------------
PlusStatus vtkPlusCommandProcessor::GetCommandReplies(PlusCommandReplyList &replies)
{
  {
    // Add reply to the sending queue
    PlusLockGuard<vtkRecursiveCriticalSection> updateMutexGuardedLock(this->Mutex);
    replies=this->CommandReplies;
    this->CommandReplies.clear();
  }
  return PLUS_SUCCESS;
}

//------------------------------------------------------------------------------
bool vtkPlusCommandProcessor::IsRunning()
{
  return this->CommandExecutionActive.second;
}

//----------------------------------------------------------------------------
vtkPlusCommand* vtkPlusCommandProcessor::GetQueuedCommand(int clientId, int commandId)
{
  PlusLockGuard<vtkRecursiveCriticalSection> updateMutexGuardedLock(this->Mutex);
  // Find the command with the specified id
  for (std::deque< vtkPlusCommand* >::iterator cmdIt=this->ActiveCommands.begin(); cmdIt!=this->ActiveCommands.end(); ++cmdIt)
  {
    if ((*cmdIt)->GetId()==commandId 
      // &&(*cmdIt)->GetClientId()==clientId  TODO: temporarily clientId check is disabled to allow a client to talk to another client's command - in the long term a command should be deactivated if the requesting client is not connected anymore)
      )
    {
      // found!
      return (*cmdIt);
    }
  }
  // not found
  return NULL;
}
