/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

/*!
\file PlusServerRemoteControl.cxx 
\brief Client to remote control PlusServer through OpenIGTLink
*/ 

#include "PlusConfigure.h"
#include "vtkPlusOpenIGTLinkClient.h"
#include "vtksys/CommandLineArguments.hxx"

#include "vtkPlusStartStopRecordingCommand.h"
#include "vtkPlusReconstructVolumeCommand.h"
#include "vtkPlusRequestDeviceIDsCommand.h"
#include "vtkPlusRequestChannelIDsCommand.h"

// Normally a client should generate unique command IDs for each executed command
// for sake of simplicity, in this sample app we don't generate new IDs, just use
// this single hardcoded value.
static const int COMMAND_ID=101;

int main( int argc, char** argv )
{
  // Check command line arguments.
  std::string serverHost="127.0.0.1";
  int serverPort = 18944;
  std::string command;
  std::string deviceId;
  std::string inputFilename="PlusServerRecording.mha";
  std::string outputFilename="PlusServerRecording.mha";
  std::string outputImageName;
  int verboseLevel = vtkPlusLogger::LOG_LEVEL_UNDEFINED;

  const int numOfTestClientsToConnect = 5; // only if testing is enabled S

  vtksys::CommandLineArguments args;
  args.Initialize( argc, argv );

  args.AddArgument( "--host", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &serverHost, "Host name of the OpenIGTLink server (default: 127.0.0.1)" );
  args.AddArgument( "--port", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &serverPort, "Port address of the OpenIGTLink server (default: 18944)" );
  args.AddArgument( "--command", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &command, 
    "Command name to be executed on the server (START_ACQUISITION, STOP_ACQUISITION, SUSPEND_ACQUISITION, RESUME_ACQUISITION, \
    RECONSTRUCT, START_RECONSTRUCTION, SUSPEND_RECONSTRUCTION, RESUME_RECONSTRUCTION, STOP_RECONSTRUCTION, GET_RECONSTRUCTION_SNAPSHOT, GET_DEVICE_IDS, GET_CHANNEL_IDS)" );
  args.AddArgument( "--device", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &deviceId, "ID of the controlled device (optional, default: first VirtualStreamCapture device)" );
  args.AddArgument( "--input-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputFilename, "File name of the input, used for RECONSTRUCT command" );
  args.AddArgument( "--output-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &outputFilename, "File name of the output, used for START command (optional, default: PlusServerRecording.mha)" );
  args.AddArgument( "--output-image-name", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &outputImageName, "OpenIGTLink device name of the reconstructed file (optional, default: image is not sent)" );
  args.AddArgument( "--verbose", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &verboseLevel, "Verbose level (1=error only, 2=warning, 3=info, 4=debug, 5=trace)" );

  if ( !args.Parse() )
  {
    std::cerr << "Problem parsing arguments." << std::endl;
    std::cout << "Help: " << args.GetHelp() << std::endl;
    exit(EXIT_FAILURE); 
  }
  
  vtkPlusLogger::Instance()->SetLogLevel( verboseLevel );

  if ( command.empty() )
  {
    LOG_ERROR("--command argument is required!"); 
    std::cout << "Help: " << args.GetHelp() << std::endl;
    exit(EXIT_FAILURE); 
  }
  
  vtkSmartPointer<vtkPlusOpenIGTLinkClient> client = vtkSmartPointer<vtkPlusOpenIGTLinkClient>::New();

  // Connect to server
  client->SetServerHost(serverHost.c_str());
  client->SetServerPort(serverPort);
  if (client->Connect()==PLUS_FAIL)
  {
    LOG_ERROR("Failed to connect to server at "<<serverHost<<":"<<serverPort);
    exit(EXIT_FAILURE);
  }    

  // Execute command
  if (STRCASECMP(command.c_str(),"START_ACQUISITION")==0)
  {
    vtkSmartPointer<vtkPlusStartStopRecordingCommand> cmd=vtkSmartPointer<vtkPlusStartStopRecordingCommand>::New();    
    cmd->SetNameToStart();
    cmd->SetOutputFilename(outputFilename.c_str());
    if ( !deviceId.empty() )
    {
      cmd->SetCaptureDeviceId(deviceId.c_str());
    }
    client->SendCommand(cmd);
  }
  else if (STRCASECMP(command.c_str(),"STOP_ACQUISITION")==0)
  {
    vtkSmartPointer<vtkPlusStartStopRecordingCommand> cmd=vtkSmartPointer<vtkPlusStartStopRecordingCommand>::New();
    cmd->SetNameToStop();
    if ( !deviceId.empty() )
    {
      cmd->SetCaptureDeviceId(deviceId.c_str());
    }
    client->SendCommand(cmd);
  }
  else if (STRCASECMP(command.c_str(),"SUSPEND_ACQUISITION")==0)
  {
    vtkSmartPointer<vtkPlusStartStopRecordingCommand> cmd=vtkSmartPointer<vtkPlusStartStopRecordingCommand>::New();
    cmd->SetNameToSuspend();
    if ( !deviceId.empty() )
    {
      cmd->SetCaptureDeviceId(deviceId.c_str());
    }
    client->SendCommand(cmd);
  }
  else if (STRCASECMP(command.c_str(),"RESUME_ACQUISITION")==0)
  {
    vtkSmartPointer<vtkPlusStartStopRecordingCommand> cmd=vtkSmartPointer<vtkPlusStartStopRecordingCommand>::New();
    cmd->SetNameToResume();
    if ( !deviceId.empty() )
    {
      cmd->SetCaptureDeviceId(deviceId.c_str());
    }
    client->SendCommand(cmd);
  }
  else if (STRCASECMP(command.c_str(),"RECONSTRUCT")==0 || STRCASECMP(command.c_str(),"START_RECONSTRUCTION")==0)
  {
    vtkSmartPointer<vtkPlusReconstructVolumeCommand> cmd=vtkSmartPointer<vtkPlusReconstructVolumeCommand>::New();
    cmd->SetInputSeqFilename(inputFilename.c_str());
    if (!outputFilename.empty())
    {
      cmd->SetOutputVolFilename(outputFilename.c_str());
    }
    if (!outputImageName.empty())
    {
      cmd->SetOutputVolDeviceName(outputImageName.c_str());
    }
    if (STRCASECMP(command.c_str(),"RECONSTRUCT")==0)
    {
      cmd->SetNameToReconstruct();
    }
    else
    {
      if (!deviceId.empty())
      {
        cmd->SetTrackedVideoDeviceId(deviceId.c_str());
      }
      else
      {
        LOG_ERROR("--device is not specified");
        exit(EXIT_FAILURE);
      }
      cmd->SetNameToStart();
      cmd->SetId(COMMAND_ID);
    }
    client->SendCommand(cmd);
  }
  else if (STRCASECMP(command.c_str(),"STOP_RECONSTRUCTION")==0)
  {
    vtkSmartPointer<vtkPlusReconstructVolumeCommand> cmd=vtkSmartPointer<vtkPlusReconstructVolumeCommand>::New();
    cmd->SetNameToStop();
    cmd->SetReferencedCommandId(COMMAND_ID);
    client->SendCommand(cmd);
  }
  else if (STRCASECMP(command.c_str(),"SUSPEND_RECONSTRUCTION")==0)
  {
    vtkSmartPointer<vtkPlusReconstructVolumeCommand> cmd=vtkSmartPointer<vtkPlusReconstructVolumeCommand>::New();
    cmd->SetNameToSuspend();
    cmd->SetReferencedCommandId(COMMAND_ID);
    client->SendCommand(cmd);
  }
  else if (STRCASECMP(command.c_str(),"RESUME_RECONSTRUCTION")==0)
  {
    vtkSmartPointer<vtkPlusReconstructVolumeCommand> cmd=vtkSmartPointer<vtkPlusReconstructVolumeCommand>::New();
    cmd->SetNameToResume();
    cmd->SetReferencedCommandId(COMMAND_ID);
    client->SendCommand(cmd);
  }
  else if (STRCASECMP(command.c_str(),"GET_RECONSTRUCTION_SNAPSHOT")==0)
  {
    vtkSmartPointer<vtkPlusReconstructVolumeCommand> cmd=vtkSmartPointer<vtkPlusReconstructVolumeCommand>::New();
    cmd->SetNameToGetSnapshot();
    cmd->SetReferencedCommandId(COMMAND_ID);
    client->SendCommand(cmd);
  }
  else if (STRCASECMP(command.c_str(), "GET_DEVICE_IDS")==0)
  {
    vtkSmartPointer<vtkPlusRequestDeviceIDsCommand> cmd=vtkSmartPointer<vtkPlusRequestDeviceIDsCommand>::New();
    cmd->SetNameToRequestDeviceIDs();
    cmd->SetId(COMMAND_ID);
    client->SendCommand(cmd);
  }
  else if (STRCASECMP(command.c_str(), "GET_CHANNEL_IDS")==0)
  {
    vtkSmartPointer<vtkPlusRequestChannelIDsCommand> cmd=vtkSmartPointer<vtkPlusRequestChannelIDsCommand>::New();
    cmd->SetNameToRequestChannelIDs();
    cmd->SetId(COMMAND_ID);
    if ( !deviceId.empty() )
    {
      cmd->SetDeviceId(deviceId.c_str());
    }
    client->SendCommand(cmd);
  }
  else
  {
    LOG_ERROR("Unknown command: "<<command);
    client->Disconnect();
    exit(EXIT_FAILURE);
  }
  
  // Get reply
  std::string reply;
  const double replyTimeoutSec=10;
  if (client->ReceiveReply(reply, replyTimeoutSec)!=PLUS_SUCCESS)
  {
    LOG_ERROR("Failed to receive reply to the command");
  }
  else
  {
    LOG_INFO("Reply: "<<reply);
  }

  client->Disconnect();

  return EXIT_SUCCESS;
}
