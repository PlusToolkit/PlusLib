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
#include "vtkPlusRequestIdsCommand.h"
#include "vtkPlusSaveConfigCommand.h"
#include "vtkPlusStartStopRecordingCommand.h"
#include "vtkPlusUpdateTransformCommand.h"
#include "vtkPlusStealthLinkCommand.h"
#include "vtksys/CommandLineArguments.hxx"
#include "vtkXMLUtilities.h"

#include "igtlTransformMessage.h"

// For catching Ctrl-C
#include <csignal>
#include <cstdlib>
#include <cstdio>

// Normally a client should generate unique command IDs for each executed command
// for sake of simplicity, in this sample app we don't generate new IDs, just use
// this single hardcoded value.
static const char* COMMAND_ID="101";

// Forward declare signal handler
void SignalInterruptHandler(int s);
static bool StopClient=false;

// A customized vtkPlusOpenIGTLinkClient that can display the received transformation matrices
class vtkPlusOpenIGTLinkClientWithTransformLogging : public vtkPlusOpenIGTLinkClient
{
public:

  static vtkPlusOpenIGTLinkClientWithTransformLogging *New();
  vtkTypeRevisionMacro( vtkPlusOpenIGTLinkClientWithTransformLogging, vtkPlusOpenIGTLinkClient );

  bool OnMessageReceived(igtl::MessageHeader::Pointer messageHeader)
  {
    bool messageBodyReceived=false;
    if (strcmp(messageHeader->GetDeviceType(), "TRANSFORM") != 0)
    {
      // not a transform message
      return messageBodyReceived;
    }

    igtl::TransformMessage::Pointer transformMsg = igtl::TransformMessage::New(); 
    transformMsg->SetMessageHeader(messageHeader); 
    transformMsg->AllocatePack();    
    SocketReceive(transformMsg->GetPackBodyPointer(), transformMsg->GetPackBodySize());
    messageBodyReceived=true;

    int c = transformMsg->Unpack(1);
    if ( !(c & igtl::MessageHeader::UNPACK_BODY)) 
    {
      LOG_ERROR("Failed to receive TRANSFORM reply (invalid body)");
      return messageBodyReceived;
    }

    //store the transform data into a matrix
    igtl::Matrix4x4 mx;
    transformMsg->GetMatrix(mx);
    LOG_INFO("Matrix for "<<transformMsg->GetDeviceName()<<" TRANSFORM received: ");
    igtl::PrintMatrix(mx);
    
    return messageBodyReceived;
  }

protected:
  vtkPlusOpenIGTLinkClientWithTransformLogging() {};
  virtual ~vtkPlusOpenIGTLinkClientWithTransformLogging() {};
private:
  vtkPlusOpenIGTLinkClientWithTransformLogging( const vtkPlusOpenIGTLinkClientWithTransformLogging& );
  void operator=( const vtkPlusOpenIGTLinkClientWithTransformLogging& );
};

vtkStandardNewMacro( vtkPlusOpenIGTLinkClientWithTransformLogging ); 
vtkCxxRevisionMacro( vtkPlusOpenIGTLinkClientWithTransformLogging, "$Revision: 1.0 $" );

// Utility functions for sending commands

//----------------------------------------------------------------------------
void ExecuteStartAcquisition(vtkPlusOpenIGTLinkClient* client, const std::string &deviceId)
{
  vtkSmartPointer<vtkPlusStartStopRecordingCommand> cmd=vtkSmartPointer<vtkPlusStartStopRecordingCommand>::New();    
  cmd->SetNameToStart();
  if ( !deviceId.empty() )
  {
    cmd->SetCaptureDeviceId(deviceId.c_str());
  }
  client->SendCommand(cmd);
}

//----------------------------------------------------------------------------
void ExecuteStopAcquisition(vtkPlusOpenIGTLinkClient* client, const std::string &deviceId, std::string outputFilename)
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

//----------------------------------------------------------------------------
void ExecuteSuspendAcquisition(vtkPlusOpenIGTLinkClient* client, const std::string &deviceId)
{
  vtkSmartPointer<vtkPlusStartStopRecordingCommand> cmd=vtkSmartPointer<vtkPlusStartStopRecordingCommand>::New();
  cmd->SetNameToSuspend();
  if ( !deviceId.empty() )
  {
    cmd->SetCaptureDeviceId(deviceId.c_str());
  }
  client->SendCommand(cmd);
}

//----------------------------------------------------------------------------
void ExecuteResumeAcquisition(vtkPlusOpenIGTLinkClient* client, const std::string &deviceId)
{
  vtkSmartPointer<vtkPlusStartStopRecordingCommand> cmd=vtkSmartPointer<vtkPlusStartStopRecordingCommand>::New();
  cmd->SetNameToResume();
  if ( !deviceId.empty() )
  {
    cmd->SetCaptureDeviceId(deviceId.c_str());
  }
  client->SendCommand(cmd);
}

//----------------------------------------------------------------------------
void ExecuteReconstructFromFile(vtkPlusOpenIGTLinkClient* client, const std::string &deviceId, const std::string &inputFilename, const std::string &outputFilename, const std::string &outputImageName)
{
  vtkSmartPointer<vtkPlusReconstructVolumeCommand> cmd=vtkSmartPointer<vtkPlusReconstructVolumeCommand>::New();
  cmd->SetNameToReconstruct();
  if (!deviceId.empty())
  {
    cmd->SetVolumeReconstructorDeviceId(deviceId.c_str());
  }
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

//----------------------------------------------------------------------------
void ExecuteStartReconstruction(vtkPlusOpenIGTLinkClient* client, const std::string &deviceId)
{
  vtkSmartPointer<vtkPlusReconstructVolumeCommand> cmd=vtkSmartPointer<vtkPlusReconstructVolumeCommand>::New();
  cmd->SetNameToStart();
  if (!deviceId.empty())
  {
    cmd->SetVolumeReconstructorDeviceId(deviceId.c_str());
  }
  client->SendCommand(cmd);
}

//----------------------------------------------------------------------------
void ExecuteSuspendReconstruction(vtkPlusOpenIGTLinkClient* client, const std::string &deviceId)
{
  vtkSmartPointer<vtkPlusReconstructVolumeCommand> cmd=vtkSmartPointer<vtkPlusReconstructVolumeCommand>::New();
  cmd->SetNameToSuspend();
  if (!deviceId.empty())
  {
    cmd->SetVolumeReconstructorDeviceId(deviceId.c_str());
  }
  client->SendCommand(cmd);
}

//----------------------------------------------------------------------------
void ExecuteResumeReconstruction(vtkPlusOpenIGTLinkClient* client, const std::string &deviceId)
{
  vtkSmartPointer<vtkPlusReconstructVolumeCommand> cmd=vtkSmartPointer<vtkPlusReconstructVolumeCommand>::New();
  cmd->SetNameToResume();
  if (!deviceId.empty())
  {
    cmd->SetVolumeReconstructorDeviceId(deviceId.c_str());
  }
  client->SendCommand(cmd);
}

//----------------------------------------------------------------------------
void ExecuteGetSnapshotReconstruction(vtkPlusOpenIGTLinkClient* client, const std::string &deviceId, const std::string &outputFilename, const std::string &outputImageName)
{
  vtkSmartPointer<vtkPlusReconstructVolumeCommand> cmd=vtkSmartPointer<vtkPlusReconstructVolumeCommand>::New();
  cmd->SetNameToGetSnapshot();
  if (!deviceId.empty())
  {
    cmd->SetVolumeReconstructorDeviceId(deviceId.c_str());
  }
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

//----------------------------------------------------------------------------
void ExecuteStopReconstruction(vtkPlusOpenIGTLinkClient* client, const std::string &deviceId, const std::string &outputFilename, const std::string &outputImageName)
{
  vtkSmartPointer<vtkPlusReconstructVolumeCommand> cmd=vtkSmartPointer<vtkPlusReconstructVolumeCommand>::New();
  cmd->SetNameToStop();
  if (!deviceId.empty())
  {
    cmd->SetVolumeReconstructorDeviceId(deviceId.c_str());
  }
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
void ExecuteGetExamData(vtkPlusOpenIGTLinkClient* client,const std::string &deviceId, const std::string &dicomOutputDirectory)
{
	vtkSmartPointer<vtkPlusStealthLinkCommand> cmd=vtkSmartPointer<vtkPlusStealthLinkCommand>::New();
	cmd->SetNameToGetExam();
	if (!deviceId.empty())
	{
		cmd->SetStealthLinkDeviceId(deviceId.c_str());
	}
	if (!dicomOutputDirectory.empty())
	{
		cmd->SetDicomImagesOutputDirectory(dicomOutputDirectory.c_str());
	}
	client->SendCommand(cmd);
}
//----------------------------------------------------------------------------
void ExecuteGetChannelIds(vtkPlusOpenIGTLinkClient* client)
{
  vtkSmartPointer<vtkPlusRequestIdsCommand> cmd=vtkSmartPointer<vtkPlusRequestIdsCommand>::New();
  cmd->SetNameToRequestChannelIds();
  cmd->SetId(COMMAND_ID);
  client->SendCommand(cmd);
}

//----------------------------------------------------------------------------
void ExecuteGetDeviceIds(vtkPlusOpenIGTLinkClient* client, const std::string &deviceType)
{
  vtkSmartPointer<vtkPlusRequestIdsCommand> cmd=vtkSmartPointer<vtkPlusRequestIdsCommand>::New();
  cmd->SetNameToRequestDeviceIds();
  cmd->SetDeviceType(deviceType.c_str());
  cmd->SetId(COMMAND_ID);
  client->SendCommand(cmd);
}

//----------------------------------------------------------------------------
void ExecuteUpdateTransform(vtkPlusOpenIGTLinkClient* client, const std::string &transformName, const std::string &transformValue, const std::string &transformError, const std::string &transformDate, const std::string &transformPersistent)
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

//----------------------------------------------------------------------------
void ExecuteSaveConfig(vtkPlusOpenIGTLinkClient* client, const std::string &outputFilename)
{
  vtkSmartPointer<vtkPlusSaveConfigCommand> cmd = vtkSmartPointer<vtkPlusSaveConfigCommand>::New();
  cmd->SetNameToSaveConfig();
  cmd->SetId(COMMAND_ID);
  cmd->SetFilename(outputFilename.c_str());
  client->SendCommand(cmd);
}

//----------------------------------------------------------------------------
PlusStatus PrintReply(vtkPlusOpenIGTLinkClient* client)
{
  std::string reply;
  const double replyTimeoutSec=20;
  if (client->ReceiveReply(reply, replyTimeoutSec)!=PLUS_SUCCESS)
  {
    LOG_ERROR("Failed to receive reply to the command");
    return PLUS_FAIL;
  }
  LOG_INFO("Reply: "<<reply);
  vtkSmartPointer<vtkXMLDataElement> replyElement = vtkSmartPointer<vtkXMLDataElement>::Take( vtkXMLUtilities::ReadElementFromString(reply.c_str()) );
  if (replyElement == NULL)
  {	
    LOG_ERROR("Unable to parse reply"); 
    return PLUS_FAIL;
  }
  if (replyElement->GetAttribute("Status")==NULL)
  {
    LOG_ERROR("Status: <missing>");
    return PLUS_FAIL;
  }
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
  LOG_INFO("Message: "<<(replyElement->GetAttribute("Message")?replyElement->GetAttribute("Message"):"<none>"));
  return status;
}

//----------------------------------------------------------------------------
int main( int argc, char** argv )
{
  // Check command line arguments.
  std::string serverHost="127.0.0.1";
  int serverPort = 18944;
  std::string command;
  std::string deviceId;
  std::string inputFilename="PlusServerRecording.mha";
  std::string outputFilename;
  std::string outputImageName;
  std::string transformName;
  std::string transformError;
  std::string transformDate;
  std::string transformPersistent;
  std::string transformValue;
  std::string dicomOutputDirectory;
  int verboseLevel = vtkPlusLogger::LOG_LEVEL_UNDEFINED;
  bool keepConnected=false;

  vtksys::CommandLineArguments args;
  args.Initialize( argc, argv );

  args.AddArgument( "--host", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &serverHost, "Host name of the OpenIGTLink server (default: 127.0.0.1)" );
  args.AddArgument( "--port", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &serverPort, "Port address of the OpenIGTLink server (default: 18944)" );
  args.AddArgument( "--command", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &command, 
    "Command name to be executed on the server (START_ACQUISITION, STOP_ACQUISITION, SUSPEND_ACQUISITION, RESUME_ACQUISITION, \
    RECONSTRUCT, START_RECONSTRUCTION, SUSPEND_RECONSTRUCTION, RESUME_RECONSTRUCTION, STOP_RECONSTRUCTION, GET_RECONSTRUCTION_SNAPSHOT, GET_CHANNEL_IDS, GET_DEVICE_IDS, GET_EXAM_DATA)" );
  args.AddArgument( "--device", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &deviceId, "ID of the controlled device (optional, default: first VirtualStreamCapture or VirtualVolumeReconstructor device). In case of GET_DEVICE_IDS it is not an ID but a device type." );
  args.AddArgument( "--input-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputFilename, "File name of the input, used for RECONSTRUCT command" );
  args.AddArgument( "--output-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &outputFilename, "File name of the output, used for START command (optional, default: 'PlusServerRecording.mha' for acquisition, no output for volume reconstruction)" );
  args.AddArgument( "--output-image-name", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &outputImageName, "OpenIGTLink device name of the reconstructed file (optional, default: image is not sent)" );
  args.AddArgument( "--transform-name", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &transformName, "The name of the transform to update. Form=[From]To[To]Transform" );
  args.AddArgument( "--transform-date", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &transformDate, "The date of the transform to update." );
  args.AddArgument( "--transform-error", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &transformError, "The error of the transform to update." );
  args.AddArgument( "--transform-persistent", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &transformPersistent, "The persistence of the transform to update." );
  args.AddArgument( "--transform-value", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &transformValue, "The actual transformation matrix to update." );
  args.AddArgument( "--keep-connected", vtksys::CommandLineArguments::NO_ARGUMENT, &keepConnected, "Keep the connection to the server after command completion (exits on CTRL-C).");  
  args.AddArgument( "--verbose", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &verboseLevel, "Verbose level (1=error only, 2=warning, 3=info, 4=debug, 5=trace)" );
  args.AddArgument( "--dicom-directory", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &dicomOutputDirectory, "The folder directory for the dicom images acquired from the StealthLink Server");
   
  if ( !args.Parse() )
  {
    std::cerr << "Problem parsing arguments." << std::endl;
    std::cout << "Help: " << args.GetHelp() << std::endl;
    exit(EXIT_FAILURE); 
  }
  
  if ( command.empty() && !keepConnected)
  {
    LOG_ERROR("The program has nothing to do, as neither --command nor --keep-connected is specifed"); 
    std::cout << "Help: " << args.GetHelp() << std::endl;
    exit(EXIT_FAILURE); 
  }

  vtkPlusLogger::Instance()->SetLogLevel( verboseLevel );
 
  // We use vtkPlusOpenIGTLinkClientWithTransformLogging instead of vtkPlusOpenIGTLinkClient to log the received transforms
  vtkSmartPointer<vtkPlusOpenIGTLinkClientWithTransformLogging> client = vtkSmartPointer<vtkPlusOpenIGTLinkClientWithTransformLogging>::New();

  // Connect to server
  client->SetServerHost(serverHost.c_str());
  client->SetServerPort(serverPort);
  if (client->Connect()==PLUS_FAIL)
  {
    LOG_ERROR("Failed to connect to server at "<<serverHost<<":"<<serverPort);
    exit(EXIT_FAILURE);
  }    

  if ( !command.empty() )
  {
    // Execute command
    if (STRCASECMP(command.c_str(),"START_ACQUISITION")==0) { ExecuteStartAcquisition(client, deviceId); }
    else if (STRCASECMP(command.c_str(),"STOP_ACQUISITION")==0) { ExecuteStopAcquisition(client, deviceId, outputFilename); }
    else if (STRCASECMP(command.c_str(),"SUSPEND_ACQUISITION")==0) { ExecuteSuspendAcquisition(client, deviceId); }
    else if (STRCASECMP(command.c_str(),"RESUME_ACQUISITION")==0) { ExecuteResumeAcquisition(client, deviceId); }
    else if (STRCASECMP(command.c_str(),"START_RECONSTRUCTION")==0) { ExecuteStartReconstruction(client, deviceId); }
    else if (STRCASECMP(command.c_str(),"SUSPEND_RECONSTRUCTION")==0) { ExecuteSuspendReconstruction(client, deviceId); }
    else if (STRCASECMP(command.c_str(),"RESUME_RECONSTRUCTION")==0) { ExecuteResumeReconstruction(client, deviceId); }
    else if (STRCASECMP(command.c_str(),"GET_RECONSTRUCTION_SNAPSHOT")==0) { ExecuteGetSnapshotReconstruction(client, deviceId, outputFilename, outputImageName); }
    else if (STRCASECMP(command.c_str(),"STOP_RECONSTRUCTION")==0) { ExecuteStopReconstruction(client, deviceId, outputFilename, outputImageName); }
    else if (STRCASECMP(command.c_str(),"RECONSTRUCT")==0) { ExecuteReconstructFromFile(client, deviceId, inputFilename, outputFilename, outputImageName); }
    else if (STRCASECMP(command.c_str(),"GET_CHANNEL_IDS")==0) { ExecuteGetChannelIds(client); }
    else if (STRCASECMP(command.c_str(),"GET_DEVICE_IDS")==0) { ExecuteGetDeviceIds(client, deviceId /* actually a device type */); }
    else if (STRCASECMP(command.c_str(), "UPDATE_TRANSFORM")==0) { ExecuteUpdateTransform(client, transformName, transformValue, transformError, transformDate, transformPersistent); }
    else if (STRCASECMP(command.c_str(), "SAVE_CONFIG")==0) { ExecuteSaveConfig(client, outputFilename); }
	else if (STRCASECMP(command.c_str(), "GET_EXAM_DATA")==0) { ExecuteGetExamData(client, deviceId,dicomOutputDirectory); }
    else
    {
      LOG_ERROR("Unknown command: "<<command);
      client->Disconnect();
      exit(EXIT_FAILURE);
    }
    PlusStatus status=PrintReply(client);
    if (!keepConnected)
    {
      // we don't need to remain connected, just exit now
      client->Disconnect();
      return status;
    }
  }

  std::cout << "Press Ctrl-C to quit:" << std::endl;

  // Set up signal catching
  signal(SIGINT, SignalInterruptHandler);

  // Run client until requested 
  const double commandQueuePollIntervalSec=0.010;
  while (!StopClient)
  {
    // the customized client logs the transformation matrices in the data receiver thread
#ifdef _WIN32
    Sleep(commandQueuePollIntervalSec*1000);
#else
    usleep(commandQueuePollIntervalSec * 1000000);
#endif
  }

  client->Disconnect();
  return EXIT_SUCCESS;
}

// -------------------------------------------------
void SignalInterruptHandler(int s)
{
  StopClient = true;
}
