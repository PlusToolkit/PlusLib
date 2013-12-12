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
#include "vtkPlusRequestIdsCommand.h"
#include "vtkPlusSaveConfigCommand.h"
#include "vtkPlusStartStopRecordingCommand.h"
#include "vtkPlusUpdateTransformCommand.h"
#include "vtkRecursiveCriticalSection.h"
#include "vtkXMLUtilities.h"

vtkStandardNewMacro( vtkPlusCommandProcessor );

//----------------------------------------------------------------------------
vtkPlusCommandProcessor::vtkPlusCommandProcessor()
: PlusServer(NULL)
, Threader(vtkSmartPointer<vtkMultiThreader>::New())
, Mutex(vtkSmartPointer<vtkRecursiveCriticalSection>::New())
, CommandExecutionActive(std::make_pair(false,false))
, CommandExecutionThreadId(-1)
{
  // Register default commands
  {
    vtkPlusCommand* cmd = vtkPlusStartStopRecordingCommand::New();
    RegisterPlusCommand(cmd);
    cmd->Delete();
  }
  {
    vtkPlusCommand* cmd = vtkPlusReconstructVolumeCommand::New();
    RegisterPlusCommand(cmd);
    cmd->Delete();
  }
  {
    vtkPlusCommand* cmd = vtkPlusRequestIdsCommand::New();
    RegisterPlusCommand(cmd);
    cmd->Delete();
  }
  {
    vtkPlusCommand* cmd = vtkPlusUpdateTransformCommand::New();
    RegisterPlusCommand(cmd);
    cmd->Delete();
  }
  {
    vtkPlusCommand* cmd = vtkPlusSaveConfigCommand::New();
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
    self->ExecuteCommands();
    // no commands in the queue, wait a bit before checking again
    const double commandQueuePollIntervalSec=0.010;
#ifdef _WIN32
    Sleep(commandQueuePollIntervalSec*1000);
#else
    usleep(commandQueuePollIntervalSec * 1000000);
#endif
  }

  // Close thread
  self->CommandExecutionThreadId = -1;
  self->CommandExecutionActive.second = false; 
  return NULL;
}

//----------------------------------------------------------------------------
int vtkPlusCommandProcessor::ExecuteCommands()
{   
  // Implemented in a while loop to not block the mutex during command execution, only during management of the queue.
  int numberOfExecutedCommands=0;
  while (1)
  {
    vtkPlusCommand* cmd=NULL; // next command to be processed  
    {
      PlusLockGuard<vtkRecursiveCriticalSection> updateMutexGuardedLock(this->Mutex);
      if (this->CommandQueue.empty())
      {
        return numberOfExecutedCommands;
      }
      cmd=this->CommandQueue.front();
      this->CommandQueue.pop_front();
    }

    LOG_DEBUG("Executing command");
    std::string messageToSend;
    vtkImageData* imageToSend=NULL;
    PlusStatus status=cmd->Execute();
    if (status==PLUS_SUCCESS)
    {
      if (cmd->GetResponseImage()!=NULL)
      {
        if (cmd->GetResponseImageDeviceName().empty() || cmd->GetResponseImageToReferenceTransform()==NULL)
        {
          LOG_WARNING("Response image device name or ImageToReferenceTransform is undefined");
        }
        // send message+image as a response
        QueueReply(cmd->GetClientId(), PLUS_SUCCESS, cmd->GetResponseMessage(), cmd->GetReplyDeviceName(),
          cmd->GetResponseImageDeviceName().c_str(), cmd->GetResponseImage(), cmd->GetResponseImageToReferenceTransform());
      }
      else
      {
        // send only message as a response
        QueueReply(cmd->GetClientId(), PLUS_SUCCESS, cmd->GetResponseMessage(), cmd->GetReplyDeviceName());
      }
    }
    else
    {
      // send error message as response
      LOG_ERROR(cmd->GetResponseMessage());
      QueueReply(cmd->GetClientId(), status, cmd->GetResponseMessage(), cmd->GetReplyDeviceName());
    }

    // the command execution is completed, so remove it from the queue of active commands
    cmd->UnRegister(this); // delete command
    numberOfExecutedCommands++;
  }

  // we never actually reach this point
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
  vtkPlusCommand* cmd = (this->RegisteredCommands[cmdName])->Clone();
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
PlusStatus vtkPlusCommandProcessor::QueueCommand(unsigned int clientId, const std::string &commandString, const std::string &deviceName, const std::string& uid)
{  
  if (commandString.empty())
  {
    LOG_ERROR("Command string is undefined");
    return PLUS_FAIL;
  }
  vtkPlusCommand* cmd = CreatePlusCommand(commandString);
  if (cmd == NULL)
  {
    std::string reply("Failed to create command from string: ");
    reply += commandString;
    std::string replyDeviceName=vtkPlusCommand::GenerateReplyDeviceName(uid);
    this->QueueReply( clientId, PLUS_FAIL, reply, replyDeviceName );
    LOG_ERROR(reply);
    return PLUS_FAIL;
  }
  cmd->SetCommandProcessor(this);
  cmd->SetClientId(clientId);
  cmd->SetDeviceName(deviceName.c_str());
  cmd->SetId(uid.c_str());

  {
    // Add command to the execution queue
    PlusLockGuard<vtkRecursiveCriticalSection> updateMutexGuardedLock(this->Mutex);
    this->CommandQueue.push_back(cmd);
  }
  return PLUS_SUCCESS;
}

//------------------------------------------------------------------------------
void vtkPlusCommandProcessor::QueueReply(int clientId, PlusStatus replyStatus, const std::string& replyString, const std::string& replyDeviceName, const char* imageName/*=NULL*/, vtkImageData* imageData/*=NULL*/, vtkMatrix4x4* imageToReferenceTransform/*=NULL*/)
{
  PlusCommandReply reply;
  reply.ClientId=clientId;
  reply.DeviceName=replyDeviceName;
  reply.CustomAttributes="Message=\""+replyString+"\"";
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
