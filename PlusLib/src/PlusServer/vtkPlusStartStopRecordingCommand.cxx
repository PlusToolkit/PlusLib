/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

#include "PlusConfigure.h"

#include "vtkPlusCommandProcessor.h"
#include "vtkPlusStartStopRecordingCommand.h"
#include "vtkVirtualDiscCapture.h"
#include "vtkDataCollector.h"

vtkStandardNewMacro( vtkPlusStartStopRecordingCommand );

static const char START_CMD[]="StartRecording";
static const char SUSPEND_CMD[]="SuspendRecording";
static const char RESUME_CMD[]="ResumeRecording";
static const char STOP_CMD[]="StopRecording";

//----------------------------------------------------------------------------
vtkPlusStartStopRecordingCommand::vtkPlusStartStopRecordingCommand()
: OutputFilename(NULL)
, CaptureDeviceId(NULL)
{
}

//----------------------------------------------------------------------------
vtkPlusStartStopRecordingCommand::~vtkPlusStartStopRecordingCommand()
{
  SetOutputFilename(NULL);
  SetCaptureDeviceId(NULL);
}

//----------------------------------------------------------------------------
void vtkPlusStartStopRecordingCommand::SetNameToStart() { SetName(START_CMD); }
void vtkPlusStartStopRecordingCommand::SetNameToSuspend() { SetName(SUSPEND_CMD); }
void vtkPlusStartStopRecordingCommand::SetNameToResume() { SetName(RESUME_CMD); }
void vtkPlusStartStopRecordingCommand::SetNameToStop() { SetName(STOP_CMD); }

//----------------------------------------------------------------------------
void vtkPlusStartStopRecordingCommand::GetCommandNames(std::list<std::string> &cmdNames)
{ 
  cmdNames.clear(); 
  cmdNames.push_back(START_CMD);
  cmdNames.push_back(SUSPEND_CMD);
  cmdNames.push_back(RESUME_CMD);
  cmdNames.push_back(STOP_CMD);  
}

//----------------------------------------------------------------------------
std::string vtkPlusStartStopRecordingCommand::GetDescription(const char* commandName)
{ 
  std::string desc;
  if (commandName==NULL || STRCASECMP(commandName, START_CMD))
  {
    desc+=START_CMD;
    desc+=": Start collecting data into file with a VirtualStreamCapture device. CaptureDeviceId: ID of the capture device, if not specified then the first VirtualStreamCapture device will be started (optional)";
  }
  if (commandName==NULL || STRCASECMP(commandName, SUSPEND_CMD))
  {
    desc+=SUSPEND_CMD;
    desc+=": Suspend data collection. Attributes: CaptureDeviceId: (optional)";
  }
  if (commandName==NULL || STRCASECMP(commandName, RESUME_CMD))
  {
    desc+=RESUME_CMD;
    desc+=": Resume suspended data collection. Attributes: CaptureDeviceId (optional)";
  }
  if (commandName==NULL || STRCASECMP(commandName, STOP_CMD))
  {
    desc+=STOP_CMD;
    desc+=": Stop collecting data into file with a VirtualStreamCapture device. Attributes: OutputFilename: name of the output file (optional if base file name is specified in config file). CaptureDeviceId (optional)";
  }
  return desc;
}

//----------------------------------------------------------------------------
void vtkPlusStartStopRecordingCommand::PrintSelf( ostream& os, vtkIndent indent )
{
  this->Superclass::PrintSelf( os, indent );
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusStartStopRecordingCommand::ReadConfiguration(vtkXMLDataElement* aConfig)
{  
  if (vtkPlusCommand::ReadConfiguration(aConfig) != PLUS_SUCCESS)
  {
    return PLUS_FAIL;
  }
  // Common parameters
  SetCaptureDeviceId(aConfig->GetAttribute("CaptureDeviceId"));

  // Stop parameters
  SetOutputFilename(aConfig->GetAttribute("OutputFilename"));

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusStartStopRecordingCommand::WriteConfiguration(vtkXMLDataElement* aConfig)
{  
  if (vtkPlusCommand::WriteConfiguration(aConfig)!=PLUS_SUCCESS)
  {
    return PLUS_FAIL;
  }  
  
  // Common parameters
  if (this->CaptureDeviceId!=NULL)
  {
    aConfig->SetAttribute("CaptureDeviceId",this->CaptureDeviceId);
  }
  else
  {
#if (VTK_VERSION_MAJOR < 6)
    // Workaround for RemoveAttribute bug in VTK5 (https://www.assembla.com/spaces/plus/tickets/859)
    PlusCommon::RemoveAttribute(aConfig, "CaptureDeviceId");
#else
    aConfig->RemoveAttribute("CaptureDeviceId");
#endif
  }

  // Start parameters
  if (this->OutputFilename != NULL)
  {
    aConfig->SetAttribute("OutputFilename",this->OutputFilename);
  }
  else
  {
#if (VTK_VERSION_MAJOR < 6)
    // Workaround for RemoveAttribute bug in VTK5 (https://www.assembla.com/spaces/plus/tickets/859)
    PlusCommon::RemoveAttribute(aConfig, "OutputFilename");
#else
    aConfig->RemoveAttribute("OutputFilename");
#endif
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
vtkVirtualDiscCapture* vtkPlusStartStopRecordingCommand::GetCaptureDevice(const char* captureDeviceId)
{
  vtkDataCollector* dataCollector=GetDataCollector();
  if (dataCollector==NULL)
  {
    LOG_ERROR("Data collector is invalid");    
    return NULL;
  }
  vtkVirtualDiscCapture *captureDevice=NULL;
  if (captureDeviceId!=NULL)
  {
    // Capture device ID is specified
    vtkPlusDevice* device=NULL;
    if (dataCollector->GetDevice(device, captureDeviceId)!=PLUS_SUCCESS)
    {
      LOG_ERROR("No VirtualStreamCapture has been found by the name "<<captureDeviceId);
      return NULL;
    }
    // device found
    captureDevice = vtkVirtualDiscCapture::SafeDownCast(device);
    if (captureDevice==NULL)
    {
      // wrong type
      LOG_ERROR("The specified device "<<captureDeviceId<<" is not VirtualStreamCapture");
      return NULL;
    }
  }
  else
  {
    // No capture device id is specified, auto-detect the first one and use that
    for( DeviceCollectionConstIterator it = dataCollector->GetDeviceConstIteratorBegin(); it != dataCollector->GetDeviceConstIteratorEnd(); ++it )
    {
      captureDevice = vtkVirtualDiscCapture::SafeDownCast(*it);
      if (captureDevice!=NULL)
      {      
        // found a recording device
        break;
      }
    }
    if (captureDevice==NULL)
    {
      LOG_ERROR("No VirtualStreamCapture has been found");
      return NULL;
    }
  }  
  return captureDevice;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusStartStopRecordingCommand::Execute()
{
  this->ResetResponse();

  if (this->Name==NULL)
  {
    this->ResponseMessage="Command failed, no command name specified";
    return PLUS_FAIL;
  }

  vtkVirtualDiscCapture *captureDevice=GetCaptureDevice(this->CaptureDeviceId);
  if (captureDevice==NULL)
  {            
    this->ResponseMessage=std::string("VirtualStreamCapture has not been found (")
      + (this->CaptureDeviceId ? this->CaptureDeviceId : "auto-detect") + "), "+this->Name+" failed";    
    return PLUS_FAIL;
  }    

  this->ResponseMessage=std::string("VirtualStreamCapture (")+captureDevice->GetDeviceId()+") "+this->Name;
  LOG_INFO("vtkPlusStartStopRecordingCommand::Execute: "<<this->Name);
  if (STRCASECMP(this->Name, START_CMD)==0)
  {
    if (captureDevice->OpenFile(this->OutputFilename)!=PLUS_SUCCESS)
    {
      this->ResponseMessage += std::string(" failed to open file ")+(this->OutputFilename?this->OutputFilename:"(undefined)");
      return PLUS_FAIL;
    }
    captureDevice->SetEnableCapturing(true);
    this->ResponseMessage+=" successful";
    return PLUS_SUCCESS;
  }
  else if (STRCASECMP(this->Name, SUSPEND_CMD)==0)
  {    
    captureDevice->SetEnableCapturing(false);
    this->ResponseMessage+=" successful";
    return PLUS_SUCCESS;
  }
  else if (STRCASECMP(this->Name, RESUME_CMD)==0)
  {    
    captureDevice->SetEnableCapturing(true);
    this->ResponseMessage+=" successful";
    return PLUS_SUCCESS;
  }
  else if (STRCASECMP(this->Name, STOP_CMD)==0)
  { 
    captureDevice->SetEnableCapturing(false);    
    
    // Once the file is closed, the filename is no longer valid, so we need to get the filename now
    std::string resultFilename=captureDevice->GetOutputFileName();
    // If we override the output filename then that will be the result filename
    if (this->OutputFilename!=NULL)
    {
      resultFilename=this->OutputFilename;
    }
    
    long numberOfFramesRecorded=captureDevice->GetTotalFramesRecorded();    
    if (captureDevice->CloseFile(this->OutputFilename) != PLUS_SUCCESS)
    {
      this->ResponseMessage += std::string(" failed to finalize file ")+resultFilename;
      return PLUS_FAIL;
    }
    std::ostringstream ss;
    ss << ", recording " << numberOfFramesRecorded <<" frames successful to file "<<resultFilename;
    this->ResponseMessage+=ss.str();
    return PLUS_SUCCESS;
  }

  this->ResponseMessage+=" unknown command";
  return PLUS_FAIL;
}
