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
#include "vtkPlusReconstructVolumeCommand.h"
#include "vtkPlusRequestChannelIDsCommand.h"
#include "vtkPlusSaveConfigCommand.h"
#include "vtkPlusStartStopRecordingCommand.h"
#include "vtkPlusUpdateTransformCommand.h"
#include "vtksys/CommandLineArguments.hxx"
#include "vtkXMLUtilities.h"

// Normally a client should generate unique command IDs for each executed command
// for sake of simplicity, in this sample app we don't generate new IDs, just use
// this single hardcoded value.
static const char* COMMAND_ID="101";

int main( int argc, char** argv )
{
  // Check command line arguments.
  std::string serverHost="127.0.0.1";
  int serverPort = 18944;
  std::string command;
  std::string deviceId;
  std::string channelId;
  std::string inputFilename="PlusServerRecording.mha";
  std::string outputFilename;
  std::string outputImageName;
  std::string transformName;
  std::string transformError;
  std::string transformDate;
  std::string transformPersistent;
  std::string transformValue;
  int verboseLevel = vtkPlusLogger::LOG_LEVEL_UNDEFINED;

  vtksys::CommandLineArguments args;
  args.Initialize( argc, argv );

  args.AddArgument( "--host", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &serverHost, "Host name of the OpenIGTLink server (default: 127.0.0.1)" );
  args.AddArgument( "--port", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &serverPort, "Port address of the OpenIGTLink server (default: 18944)" );
  args.AddArgument( "--command", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &command, 
    "Command name to be executed on the server (START_ACQUISITION, STOP_ACQUISITION, SUSPEND_ACQUISITION, RESUME_ACQUISITION, \
    RECONSTRUCT, START_RECONSTRUCTION, SUSPEND_RECONSTRUCTION, RESUME_RECONSTRUCTION, STOP_RECONSTRUCTION, GET_RECONSTRUCTION_SNAPSHOT, GET_CHANNEL_IDS)" );
  args.AddArgument( "--device", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &deviceId, "ID of the controlled device (optional, default: first VirtualStreamCapture device)" );
  args.AddArgument( "--channel", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &channelId, "ID of the channel to use" );
  args.AddArgument( "--input-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputFilename, "File name of the input, used for RECONSTRUCT command" );
  args.AddArgument( "--output-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &outputFilename, "File name of the output, used for START command (optional, default: 'PlusServerRecording.mha' for acquisition, no output for volume reconstruction)" );
  args.AddArgument( "--output-image-name", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &outputImageName, "OpenIGTLink device name of the reconstructed file (optional, default: image is not sent)" );
  args.AddArgument( "--transform-name", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &transformName, "The name of the transform to update. Form=[From]To[To]Transform" );
  args.AddArgument( "--transform-date", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &transformDate, "The date of the transform to update." );
  args.AddArgument( "--transform-error", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &transformError, "The error of the transform to update." );
  args.AddArgument( "--transform-persistent", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &transformPersistent, "The persistence of the transform to update." );
  args.AddArgument( "--transform-value", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &transformValue, "The actual transformation matrix to update." );
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
    if (outputFilename.empty())
    {
      outputFilename="PlusServerRecording.mha";
    }
    cmd->SetOutputFilename(outputFilename.c_str());
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
      if (!channelId.empty())
      {
        cmd->SetChannelId(channelId.c_str());
      }
      else
      {
        LOG_ERROR("--channel is not specified");
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
  else if (STRCASECMP(command.c_str(), "GET_CHANNEL_IDS")==0)
  {
    vtkSmartPointer<vtkPlusRequestChannelIDsCommand> cmd=vtkSmartPointer<vtkPlusRequestChannelIDsCommand>::New();
    cmd->SetNameToRequestChannelIDs();
    cmd->SetId(COMMAND_ID);
    client->SendCommand(cmd);
  }
  else if (STRCASECMP(command.c_str(), "UPDATE_TRANSFORM")==0)
  {
    vtkSmartPointer<vtkPlusUpdateTransformCommand> cmd = vtkSmartPointer<vtkPlusUpdateTransformCommand>::New();
    cmd->SetNameToUpdateTransform();
    cmd->SetId(COMMAND_ID);
    cmd->SetTransformName(transformName.c_str());
    double value;
    std::stringstream ss(transformError);
    ss >> value;
    cmd->SetTransformError(value);
    cmd->SetTransformDate(transformDate.c_str());
    cmd->SetTransformPersistent(transformPersistent.compare("TRUE") == 0 );
    std::vector<std::string> elems;
    vtkMatrix4x4* transformValueMatrix = vtkMatrix4x4::New();
    PlusCommon::SplitStringIntoTokens(transformValue, ' ', elems);
    if( elems.size() != 16 )
    {
      LOG_ERROR("Invalid formatting of matrix string.");
      exit(EXIT_FAILURE);
    }
    for (int i = 0; i < 4; i++)
    {
      for (int j = 0; j < 4; j++)
      {
        std::stringstream stream(elems[i*4 + j]);
        stream >> value;
        transformValueMatrix->SetElement(i, j, value);
      }
    }
    cmd->SetTransformValue(transformValueMatrix);
    transformValueMatrix->Delete();
    client->SendCommand(cmd);
  }
  else if (STRCASECMP(command.c_str(), "SAVE_CONFIG")==0)
  {
    vtkSmartPointer<vtkPlusSaveConfigCommand> cmd = vtkSmartPointer<vtkPlusSaveConfigCommand>::New();
    cmd->SetNameToSaveConfig();
    cmd->SetId(COMMAND_ID);
    cmd->SetFilename(outputFilename.c_str());
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
    vtkSmartPointer<vtkXMLDataElement> replyElement = vtkSmartPointer<vtkXMLDataElement>::Take( vtkXMLUtilities::ReadElementFromString(reply.c_str()) );
    if (replyElement == NULL)
    {	
      LOG_ERROR("Unable to parse reply"); 
    }
    else
    {
      if (replyElement->GetAttribute("Status")==NULL)
      {
        LOG_ERROR("Status: <missing>");
      }
      else
      {
        PlusStatus status=PLUS_FAIL;
        if (STRCASECMP(replyElement->GetAttribute("Status"),"SUCCESS")==0)
        {
          status=PLUS_SUCCESS;
        }
        else if (STRCASECMP(replyElement->GetAttribute("Status"),"FAIL")==0)
        {
          status=PLUS_FAIL;
        }
        else
        {
          LOG_ERROR("Invalid status: "<<replyElement->GetAttribute("Status"));
        }
        LOG_INFO("Status: "<<(status==PLUS_SUCCESS?"SUCCESS":"FAIL"));
      }
      LOG_INFO("Message: "<<(replyElement->GetAttribute("Message")?replyElement->GetAttribute("Message"):"<none>"));
    }
  }

  client->Disconnect();

  return EXIT_SUCCESS;
}
