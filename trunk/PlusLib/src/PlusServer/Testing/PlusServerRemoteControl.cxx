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
  args.AddArgument( "--command", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &command, "Command name to be executed on the server (START, STOP, SUSPEND, RESUME, RECONSTRUCT)" );
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
  if (STRCASECMP(command.c_str(),"START")==0)
  {
    vtkSmartPointer<vtkPlusStartStopRecordingCommand> cmd=vtkSmartPointer<vtkPlusStartStopRecordingCommand>::New();    
    cmd->SetCommandNameStart();
    cmd->SetOutputFilename(outputFilename.c_str());
    if ( !deviceId.empty() )
    {
      cmd->SetCaptureDeviceId(deviceId.c_str());
    }
    client->SendCommand(cmd);
  }
  else if (STRCASECMP(command.c_str(),"STOP")==0)
  {
    vtkSmartPointer<vtkPlusStartStopRecordingCommand> cmd=vtkSmartPointer<vtkPlusStartStopRecordingCommand>::New();
    cmd->SetCommandNameStop();
    if ( !deviceId.empty() )
    {
      cmd->SetCaptureDeviceId(deviceId.c_str());
    }
    client->SendCommand(cmd);
  }
  else if (STRCASECMP(command.c_str(),"SUSPEND")==0)
  {
    vtkSmartPointer<vtkPlusStartStopRecordingCommand> cmd=vtkSmartPointer<vtkPlusStartStopRecordingCommand>::New();
    cmd->SetCommandNameSuspend();
    if ( !deviceId.empty() )
    {
      cmd->SetCaptureDeviceId(deviceId.c_str());
    }
    client->SendCommand(cmd);
  }
  else if (STRCASECMP(command.c_str(),"RESUME")==0)
  {
    vtkSmartPointer<vtkPlusStartStopRecordingCommand> cmd=vtkSmartPointer<vtkPlusStartStopRecordingCommand>::New();
    cmd->SetCommandNameResume();
    if ( !deviceId.empty() )
    {
      cmd->SetCaptureDeviceId(deviceId.c_str());
    }
    client->SendCommand(cmd);
  }
  else if (STRCASECMP(command.c_str(),"RECONSTRUCT")==0)
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
