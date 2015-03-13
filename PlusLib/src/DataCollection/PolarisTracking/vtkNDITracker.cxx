/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

/*=========================================================================
The following copyright notice is applicable to parts of this file:

Copyright (c) 2000-2005 Atamai, Inc.

Use, modification and redistribution of the software, in source or
binary forms, are permitted provided that the following terms and
conditions are met:

1) Redistribution of the source code, in verbatim or modified
form, must retain the above copyright notice, this license,
the following disclaimer, and any notices that refer to this
license and/or the following disclaimer.  

2) Redistribution in binary form must include the above copyright
notice, a copy of this license and the following disclaimer
in the documentation or with other materials provided with the
distribution.

3) Modified copies of the source code must be clearly marked as such,
and must not be misrepresented as verbatim copies of the source code.

THE COPYRIGHT HOLDERS AND/OR OTHER PARTIES PROVIDE THE SOFTWARE "AS IS"
WITHOUT EXPRESSED OR IMPLIED WARRANTY INCLUDING, BUT NOT LIMITED TO,
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
PURPOSE.  IN NO EVENT SHALL ANY COPYRIGHT HOLDER OR OTHER PARTY WHO MAY
ODIFY AND/OR REDISTRIBUTE THE SOFTWARE UNDER THE TERMS OF THIS LICENSE
BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, LOSS OF DATA OR DATA BECOMING INACCURATE
OR LOSS OF PROFIT OR BUSINESS INTERRUPTION) ARISING IN ANY WAY OUT OF
THE USE OR INABILITY TO USE THE SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGES.

=========================================================================*/

#include "PlusConfigure.h"
#include "ndicapi.h"
#include "ndicapi_math.h"
#include "vtkCharArray.h"
#include "vtkMath.h"
#include "vtkMatrix4x4.h"
#include "vtkNDITracker.h"
#include "vtkObjectFactory.h"
#include "vtkRecursiveCriticalSection.h"
#include "vtkSocketCommunicator.h"
#include "vtkTimerLog.h"
#include "vtkPlusDataSource.h"
#include "vtkTransform.h"
#include <ctype.h>
#include <float.h>
#include <limits.h>
#include <math.h>
#include <string.h>

vtkStandardNewMacro(vtkNDITracker);

const int VIRTUAL_SROM_SIZE=1024;

//----------------------------------------------------------------------------
vtkNDITracker::vtkNDITracker()
{
  this->Device = 0;
  this->Version = NULL;
  this->IsDeviceTracking = 0;
  this->SerialPort = -1; // default is to probe
  this->BaudRate = 9600;
  this->MeasurementVolumeNumber = 0; // keep default volume

  this->LastFrameNumber=0;

  memset(this->CommandReply,0,VTK_NDI_REPLY_LEN);

  // PortName for data source is not required if RomFile is specified, so we don't need to enable this->RequirePortNameInDeviceSetConfiguration
  
  // No callback function provided by the device, so the data capture thread will be used to poll the hardware and add new items to the buffer
  this->StartThreadForInternalUpdates=true;
  this->AcquisitionRate=50;
}

//----------------------------------------------------------------------------
vtkNDITracker::~vtkNDITracker() 
{
  if (this->Recording)
  {
    this->StopRecording();
  }  
  for (NdiToolDescriptorsType::iterator toolDescriptorIt=this->NdiToolDescriptors.begin(); toolDescriptorIt!=this->NdiToolDescriptors.end(); ++toolDescriptorIt)
  {
    delete [] toolDescriptorIt->second.VirtualSROM;
    toolDescriptorIt->second.VirtualSROM = NULL;
  }
  this->SetVersion(NULL);
}

//----------------------------------------------------------------------------
void vtkNDITracker::PrintSelf(ostream& os, vtkIndent indent)
{
  Superclass::PrintSelf(os,indent);
}

//----------------------------------------------------------------------------
std::string vtkNDITracker::GetSdkVersion()
{
  std::ostringstream version; 
  version << "NDICAPI-" << NDICAPI_MAJOR_VERSION << "." << NDICAPI_MINOR_VERSION; 
  return version.str(); 
}

//----------------------------------------------------------------------------
PlusStatus vtkNDITracker::Probe()
{
  if (this->IsDeviceTracking)
  {
    return PLUS_SUCCESS;
  }
  int errnum = NDI_OPEN_ERROR;
  char *devicename = NULL;  
  if (this->SerialPort > 0)
  {
    devicename = ndiDeviceName(this->SerialPort-1);
    if (devicename)
    {
      errnum = ndiProbe(devicename);
    }
  }
  else
  {
    // if SerialPort is set to -1, then probe the first N serial ports
    const int MAX_SERIAL_PORT_NUMBER=20; // the serial port is almost surely less than this number
    for (int i = 0; i < MAX_SERIAL_PORT_NUMBER; i++)
    {
      devicename = ndiDeviceName(i);
      if (devicename)
      {
        errnum = ndiProbe(devicename);
        if (errnum == NDI_OKAY)
        {
          this->SerialPort = i+1;
          break;
        }
      }
    }
  }

  // if probe was okay, then send VER:0 to identify device
  if (errnum != NDI_OKAY)
  {
    return PLUS_FAIL;
  }

  this->Device = ndiOpen(devicename);
  if (this->Device)
  {
    this->SetVersion(ndiVER(this->Device,0));
    ndiClose(this->Device);
    this->Device = 0;
  }
  return PLUS_SUCCESS;
} 

//----------------------------------------------------------------------------
// Send a raw command to the tracking unit.
// If communication has already been opened with the NDI,
// then lock the mutex to get exclusive access and then
// send the command.
// Otherwise, open communication with the unit, send the command,
// and close communication.
char *vtkNDITracker::Command(const char *command)
{
  this->CommandReply[0] = '\0';

  if (this->Device)
  {
    PlusLockGuard<vtkRecursiveCriticalSection> updateMutexGuardedLock(this->UpdateMutex);
    strncpy(this->CommandReply, ndiCommand(this->Device, command), VTK_NDI_REPLY_LEN-1);
    this->CommandReply[VTK_NDI_REPLY_LEN-1] = '\0';
  }
  else
  {
    char *devicename = ndiDeviceName(this->SerialPort-1);
    this->Device = ndiOpen(devicename);
    if (this->Device == 0) 
    {
      LOG_ERROR(ndiErrorString(NDI_OPEN_ERROR));
    }
    else
    {
      strncpy(this->CommandReply, ndiCommand(this->Device, command), VTK_NDI_REPLY_LEN-1);
      this->CommandReply[VTK_NDI_REPLY_LEN-1] = '\0';
      ndiClose(this->Device);
    }
    this->Device = 0;
  }

  return this->CommandReply;
}


//----------------------------------------------------------------------------
PlusStatus vtkNDITracker::InternalConnect()
{ 
  int baud = NDI_9600;
  switch (this->BaudRate)
  {
  case 9600: baud = NDI_9600; break; 
  case 14400: baud = NDI_14400; break; 
  case 19200: baud = NDI_19200; break; 
  case 38400: baud = NDI_38400; break; 
  case 57600: baud = NDI_57600; break; 
  case 115200: baud = NDI_115200; break;
  case 921600: baud = NDI_921600; break;
  case 1228739: baud = NDI_1228739; break;
  default:
    LOG_ERROR("Illegal baud rate: "<<this->BaudRate<<". Valid values: 9600, 14400, 19200, 38400, 5760, 115200, 921600, 1228739");
    return PLUS_FAIL;
  }

  char *devicename = ndiDeviceName(this->SerialPort-1);
  this->Device = ndiOpen(devicename);
  if (this->Device == 0) 
  {
    LOG_ERROR("Failed to open port: " << (devicename == NULL ? "unknown" : devicename) << " - " << ndiErrorString(NDI_OPEN_ERROR));
    return PLUS_FAIL;
  }
  // initialize Device
  bool resetOccurred=false;
  const char* initCommandReply=ndiCommand(this->Device,"INIT:");
  if (initCommandReply!=NULL && strncmp(initCommandReply,"RESET",5)==0)
  {
    // The tracker device was left in high-speed mode after exiting debugger. When the INIT was sent at 9600 baud,
    // the device reset back to default 9600 and returned status RESET.
    // Re-issue the INIT command to avoid 'command not valid in current mode' errors.
    resetOccurred=true;
  }
  int errnum = 0;
  if (ndiGetError(this->Device) || resetOccurred)
  {
    ndiRESET(this->Device);
    //ndiGetError(this->Device); // ignore the error
    ndiCommand(this->Device,"INIT:");
    errnum = ndiGetError(this->Device);
    if (errnum) 
    {
      LOG_ERROR(ndiErrorString(errnum));
      ndiClose(this->Device);
      this->Device = 0;
      return PLUS_FAIL;
    }
  }

  // set the baud rate
  // also: NOHANDSHAKE cuts down on CRC errs and timeouts
  ndiCommand(this->Device,"COMM:%d%03d%d",baud,NDI_8N1,NDI_NOHANDSHAKE);
  errnum = ndiGetError(this->Device);
  if (errnum) 
  {
    LOG_ERROR(ndiErrorString(errnum));
    ndiClose(this->Device);
    this->Device = 0;
    return PLUS_FAIL;
  }

  if (this->MeasurementVolumeNumber!=0)
  {
    const char* volumeSelectCommandReply=ndiCommand(this->Device,"VSEL:%d",this->MeasurementVolumeNumber);
    errnum = ndiGetError(this->Device);
    if (errnum) 
    {
      LOG_ERROR("Failed to set measurement volume "<<this->MeasurementVolumeNumber<<": "<<ndiErrorString(errnum));

      const unsigned char MODE_GET_VOLUMES_LIST = 0x03; // list of volumes available
      const char* volumeListCommandReply=ndiCommand(this->Device,"SFLIST:%02X",MODE_GET_VOLUMES_LIST);
      errnum = ndiGetError(this->Device);
      if (errnum || volumeListCommandReply==NULL)
      {
        LOG_ERROR("Failed to retrieve list of available volumes: "<<ndiErrorString(errnum));
      }
      else
      {
        LogVolumeList(volumeListCommandReply, 0, vtkPlusLogger::LOG_LEVEL_INFO);
      }
      ndiClose(this->Device);
      this->Device = 0;
      return PLUS_FAIL;
    }
    else
    {
      const unsigned char MODE_GET_VOLUMES_LIST = 0x03; // list of volumes available
      const char* volumeListCommandReply=ndiCommand(this->Device,"SFLIST:%02X",MODE_GET_VOLUMES_LIST);
      errnum = ndiGetError(this->Device);
      if (!errnum || volumeListCommandReply!=NULL)
      {
        LogVolumeList(volumeListCommandReply, this->MeasurementVolumeNumber, vtkPlusLogger::LOG_LEVEL_DEBUG);
      }
    }
  }

  // get information about the device
  this->SetVersion(ndiVER(this->Device,0));

  if (this->EnableToolPorts()!=PLUS_SUCCESS)
  {
    LOG_ERROR("Failed to enable tool ports");
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkNDITracker::InternalDisconnect()
{ 
  for (NdiToolDescriptorsType::iterator toolDescriptorIt=this->NdiToolDescriptors.begin(); toolDescriptorIt!=this->NdiToolDescriptors.end(); ++toolDescriptorIt)
  {
    this->ClearVirtualSromInTracker(toolDescriptorIt->second);
  }

  this->DisableToolPorts();

  // return to default comm settings
  ndiCommand(this->Device,"COMM:00000");
  int errnum = ndiGetError(this->Device);
  if (errnum) 
  {
    LOG_ERROR(ndiErrorString(errnum));
  }
  ndiClose(this->Device);
  this->Device = 0;

  return PLUS_SUCCESS;
}


//----------------------------------------------------------------------------
PlusStatus vtkNDITracker::InternalStartRecording()
{
  if (this->IsDeviceTracking)
  {
    return PLUS_SUCCESS;
  }

  ndiCommand(this->Device,"TSTART:");
  int errnum = ndiGetError(this->Device);
  if (errnum) 
  {
    LOG_ERROR("Failed TSTART: " << ndiErrorString(errnum) );
    ndiClose(this->Device);
    this->Device = 0;
    return PLUS_FAIL;
  }

  this->IsDeviceTracking = 1;

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkNDITracker::InternalStopRecording()
{
  if (this->Device == 0)
  {
    return PLUS_FAIL;
  }

  ndiCommand(this->Device,"TSTOP:");
  int errnum = ndiGetError(this->Device);
  if (errnum) 
  {
    LOG_ERROR(ndiErrorString(errnum));
  }
  this->IsDeviceTracking = 0;

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkNDITracker::InternalUpdate()
{  
  if (!this->IsDeviceTracking)
  {
    LOG_ERROR("called Update() when NDI was not tracking");
    return PLUS_FAIL;
  }

  int errnum=0;

  // get the transforms for all tools from the NDI
  ndiCommand(this->Device,"TX:0801");
  errnum = ndiGetError(this->Device);
  if (errnum)
  {
    if (errnum == NDI_BAD_CRC || errnum == NDI_TIMEOUT) // common errors
    {
      LOG_WARNING(ndiErrorString(errnum));
    }
    else
    {
      LOG_ERROR(ndiErrorString(errnum));
    }
    return PLUS_FAIL;
  }

  // default to incrementing frame count by one (in case a frame index cannot be retrieved from the tracker for a specific tool)
  this->LastFrameNumber++;
  int defaultToolFrameNumber = this->LastFrameNumber;
  const double toolTimestamp = vtkAccurateTimer::GetSystemTime(); // unfiltered timestamp
  vtkSmartPointer<vtkMatrix4x4> toolToTrackerTransform=vtkSmartPointer<vtkMatrix4x4>::New();
  for (DataSourceContainerConstIterator it = this->GetToolIteratorBegin(); it != this->GetToolIteratorEnd(); ++it)
  {
    ToolStatus toolFlags = TOOL_OK;
    toolToTrackerTransform->Identity();
    unsigned long toolFrameNumber = defaultToolFrameNumber;
    vtkPlusDataSource* trackerTool = it->second;
    std::string toolSourceId=trackerTool->GetSourceId();
    NdiToolDescriptorsType::iterator ndiToolDescriptorIt = this->NdiToolDescriptors.find(toolSourceId);
    if (ndiToolDescriptorIt==this->NdiToolDescriptors.end())
    {
      LOG_ERROR("Tool descriptor is not found for tool "<<toolSourceId);
      this->ToolTimeStampedUpdate(trackerTool->GetSourceId(), toolToTrackerTransform, toolFlags, toolFrameNumber, toolTimestamp);
      continue;
    }
    int portHandle = ndiToolDescriptorIt->second.PortHandle;
    if (portHandle <= 0)
    {
      LOG_ERROR("Port handle is invalid for tool "<<toolSourceId);
      this->ToolTimeStampedUpdate(toolSourceId.c_str(), toolToTrackerTransform, toolFlags, toolFrameNumber, toolTimestamp);
      continue;
    }

    double ndiTransform[8]={1,0,0,0,0,0,0,0};
    int ndiToolAbsent = ndiGetTXTransform(this->Device, portHandle, ndiTransform);
    int ndiPortStatus = ndiGetTXPortStatus(this->Device, portHandle);
    unsigned long ndiFrameIndex = ndiGetTXFrame(this->Device, portHandle);

    // convert status flags from NDI to Plus format
    const unsigned long ndiPortStatusValidFlags = NDI_TOOL_IN_PORT | NDI_INITIALIZED | NDI_ENABLED;
    if ((ndiPortStatus & ndiPortStatusValidFlags) != ndiPortStatusValidFlags) 
    {
      toolFlags = TOOL_MISSING;
    }
    else
    {
      if (ndiToolAbsent)
      {
        toolFlags = TOOL_OUT_OF_VIEW;
      }
      if (ndiPortStatus & NDI_OUT_OF_VOLUME)
      {
        toolFlags = TOOL_OUT_OF_VOLUME;
      }
      // TODO all these button state toolFlags are on regardless of the actual state
      //if (ndiPortStatus & NDI_SWITCH_1_ON)  { toolFlags = TOOL_SWITCH1_IS_ON; }
      //if (ndiPortStatus & NDI_SWITCH_2_ON)  { toolFlags = TOOL_SWITCH2_IS_ON; }
      //if (ndiPortStatus & NDI_SWITCH_3_ON)  { toolFlags = TOOL_SWITCH3_IS_ON; }
    }

    ndiTransformToMatrixd(ndiTransform,*toolToTrackerTransform->Element);
    toolToTrackerTransform->Transpose();

    // by default (if there is no camera frame number associated with
    // the tool transformation) the most recent timestamp is used.    
    if (!ndiToolAbsent && ndiFrameIndex)
    {
      // this will create a timestamp from the frame number      
      toolFrameNumber = ndiFrameIndex;
      if (ndiFrameIndex>this->LastFrameNumber)
      {
        this->LastFrameNumber=ndiFrameIndex;
      }
    }

    // send the matrix and status to the tool's vtkPlusDataBuffer
    this->ToolTimeStampedUpdate(toolSourceId.c_str(), toolToTrackerTransform, toolFlags, toolFrameNumber, toolTimestamp);
  }

  // Update tool connections if a wired tool is plugged in
  if (ndiGetTXSystemStatus(this->Device) & NDI_PORT_OCCUPIED)
  { 
    LOG_WARNING("A wired tool has been plugged into tracker "<<(this->GetDeviceId()?this->GetDeviceId():"(unknown NDI tracker"));
    // Make the newly connected tools available
    this->EnableToolPorts();
  }
  
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkNDITracker::ReadSromFromFile(NdiToolDescriptor& toolDescriptor, const char *filename)
{
  FILE *file = fopen(filename,"rb");
  if (file == NULL)
  {
    LOG_ERROR("couldn't find srom file " << filename);
    return PLUS_FAIL;
  }

  if (toolDescriptor.VirtualSROM == 0)
  {
    toolDescriptor.VirtualSROM = new unsigned char[VIRTUAL_SROM_SIZE];
  }

  memset(toolDescriptor.VirtualSROM,0,VIRTUAL_SROM_SIZE);
  fread(toolDescriptor.VirtualSROM,1,VIRTUAL_SROM_SIZE,file);
  fclose(file);
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkNDITracker::EnableToolPorts()
{
  PlusStatus status=PLUS_SUCCESS;

  // stop tracking
  if (this->IsDeviceTracking)
  {
    ndiCommand(this->Device,"TSTOP:");
    int errnum = ndiGetError(this->Device);
    if (errnum)
    { 
      LOG_ERROR(ndiErrorString(errnum));
      status=PLUS_FAIL;
    }    
  }

  // free ports that are waiting to be freed
  {
    ndiCommand(this->Device,"PHSR:01");
    int ntools = ndiGetPHSRNumberOfHandles(this->Device);
    for (int ndiToolIndex = 0; ndiToolIndex < ntools; ndiToolIndex++)
    {
      int portHandle = ndiGetPHSRHandle(this->Device,ndiToolIndex);
      ndiCommand(this->Device,"PHF:%02X",portHandle);
      int errnum = ndiGetError(this->Device);
      if (errnum)
      { 
        LOG_ERROR(ndiErrorString(errnum));
        status=PLUS_FAIL;
      }
    }
  }

  // Set wireless port handles and send SROM files to tracker
  // We need to do this before initializing and enabling
  // the ports waiting to be initialized.
  for (NdiToolDescriptorsType::iterator toolDescriptorIt=this->NdiToolDescriptors.begin(); toolDescriptorIt!=this->NdiToolDescriptors.end(); ++toolDescriptorIt)
  {
    if (toolDescriptorIt->second.WiredPortNumber == -1) //wireless tool
    {  
      if (this->UpdatePortHandle(toolDescriptorIt->second)!=PLUS_SUCCESS)
      {
        LOG_ERROR("Failed to determine NDI port handle for tool "<<toolDescriptorIt->first);
        return PLUS_FAIL;
      }
      if (this->SendSromToTracker(toolDescriptorIt->second)!=PLUS_SUCCESS)
      {
        LOG_ERROR("Failed send SROM to NDI tool "<<toolDescriptorIt->first);
        return PLUS_FAIL;
      }
    }
  }

  // initialize ports waiting to be initialized
  {
    int errnum=0;
    int ntools=0;
    do // repeat as necessary (in case multi-channel tools are used) 
    {
      ndiCommand(this->Device,"PHSR:02");
      ntools = ndiGetPHSRNumberOfHandles(this->Device);
      for (int ndiToolIndex = 0; ndiToolIndex < ntools; ndiToolIndex++)
      {
        int portHandle = ndiGetPHSRHandle(this->Device,ndiToolIndex);
        ndiCommand(this->Device,"PINIT:%02X",portHandle);
        errnum = ndiGetError(this->Device);
        if (errnum)
        { 
          LOG_ERROR(ndiErrorString(errnum));
          status=PLUS_FAIL;
        }
      }
    }
    while (ntools > 0 && errnum == 0);
  }

  // enable initialized tools
  {
    ndiCommand(this->Device,"PHSR:03");
    int ntools = ndiGetPHSRNumberOfHandles(this->Device);
    for (int ndiToolIndex = 0; ndiToolIndex < ntools; ndiToolIndex++)
    {
      int portHandle = ndiGetPHSRHandle(this->Device,ndiToolIndex);
      ndiCommand(this->Device,"PHINF:%02X0001",portHandle);
      char identity[34];
      ndiGetPHINFToolInfo(this->Device,identity);
      int mode='D'; // default
      if (identity[1] == 0x03) // button-box
      {
        mode = 'B';
      }
      else if (identity[1] == 0x01) // reference
      {
        mode = 'S';
      }
      // enable the tool
      ndiCommand(this->Device,"PENA:%02X%c",portHandle,mode);
      int errnum = ndiGetError(this->Device);
      if (errnum)
      {
        LOG_ERROR(ndiErrorString(errnum));
        status=PLUS_FAIL;
      }
    }
  }

  // Set wired port handles and send SROM files to tracker
  // We need to do this after enabling all the tools because tools on
  // splitters (two 5-DOF tools with one connector) only appear after the tool is enabled.
  for (NdiToolDescriptorsType::iterator toolDescriptorIt=this->NdiToolDescriptors.begin(); toolDescriptorIt!=this->NdiToolDescriptors.end(); ++toolDescriptorIt)
  {
    if (toolDescriptorIt->second.WiredPortNumber >= 0) //wired tool
    {
      if (this->UpdatePortHandle(toolDescriptorIt->second)!=PLUS_SUCCESS)
      {
        LOG_ERROR("Failed to determine NDI port handle for tool "<<toolDescriptorIt->first);
        return PLUS_FAIL;
      }
      if (this->SendSromToTracker(toolDescriptorIt->second)!=PLUS_SUCCESS)
      {
        LOG_ERROR("Failed send SROM to NDI tool "<<toolDescriptorIt->first);
        return PLUS_FAIL;
      }
    }
  }

  // Update tool info

  ndiCommand(this->Device,"PHSR:00");

  for (NdiToolDescriptorsType::iterator toolDescriptorIt=this->NdiToolDescriptors.begin(); toolDescriptorIt!=this->NdiToolDescriptors.end(); ++toolDescriptorIt)
  {
    vtkPlusDataSource* trackerTool = NULL;
    if ( this->GetTool(toolDescriptorIt->first, trackerTool) != PLUS_SUCCESS )
    {
      LOG_ERROR("Failed to get NDI tool: " << toolDescriptorIt->first );
      status=PLUS_FAIL;
      continue; 
    }

    ndiCommand(this->Device,"PHINF:%02X0025",toolDescriptorIt->second.PortHandle);
    int errnum = ndiGetError(this->Device);
    if (errnum)
    { 
      LOG_ERROR(ndiErrorString(errnum));
      status=PLUS_FAIL;
      continue;
    }

    // decompose identity string from end to front
    char identity[34];
    ndiGetPHINFToolInfo(this->Device, identity);
    identity[31] = '\0';
    std::string serialNumber(&identity[23]);
    PlusCommon::Trim(serialNumber);
    trackerTool->SetCustomProperty("SerialNumber", serialNumber);
    identity[23] = '\0';
    std::string toolRevision(&identity[20]);
    PlusCommon::Trim(toolRevision);
    trackerTool->SetCustomProperty("Revision", toolRevision);
    identity[20] = '\0';
    std::string toolManufacturer(&identity[8]);
    PlusCommon::Trim(toolManufacturer);
    trackerTool->SetCustomProperty("Manufacturer", toolManufacturer);
    identity[8] = '\0';
    trackerTool->SetCustomProperty("NdiIdentity",PlusCommon::Trim(&identity[0]));
    char partNumber[24];
    ndiGetPHINFPartNumber(this->Device, partNumber);
    partNumber[20] = '\0';
    std::string toolPartNumber(&partNumber[0]);
    PlusCommon::Trim(toolPartNumber);
    trackerTool->SetCustomProperty("PartNumber", toolPartNumber);
    int status = ndiGetPHINFPortStatus(this->Device);

    toolDescriptorIt->second.PortEnabled = ((status & NDI_ENABLED) != 0);
    if (!toolDescriptorIt->second.PortEnabled)
    {
      LOG_ERROR("Failed to enable NDI tool "<<toolDescriptorIt->first);
      status=PLUS_FAIL;
    }
  }

  // re-start the tracking
  if (this->IsDeviceTracking)
  {
    ndiCommand(this->Device,"TSTART:");
    int errnum = ndiGetError(this->Device);
    if (errnum)
    { 
      LOG_ERROR("Failed TSTART: " << ndiErrorString(errnum));
      status=PLUS_FAIL;
    }
  }
  
  return status;
}

//----------------------------------------------------------------------------
// Disable all enabled tool ports.
void vtkNDITracker::DisableToolPorts()
{
  // stop tracking
  if (this->IsDeviceTracking)
  {
    ndiCommand(this->Device,"TSTOP:");
    int errnum = ndiGetError(this->Device);
    if (errnum)
    { 
      LOG_ERROR(ndiErrorString(errnum));
    }    
  }

  // disable all enabled tools
  ndiCommand(this->Device,"PHSR:04");
  int ntools = ndiGetPHSRNumberOfHandles(this->Device);
  for (int ndiToolIndex = 0; ndiToolIndex < ntools; ndiToolIndex++)
  {
    int portHandle = ndiGetPHSRHandle(this->Device,ndiToolIndex);
    ndiCommand(this->Device,"PDIS:%02X",portHandle);
    int errnum = ndiGetError(this->Device);
    if (errnum)
    { 
      LOG_ERROR(ndiErrorString(errnum));
    }    
  }

  // disable the enabled ports
  for (NdiToolDescriptorsType::iterator toolDescriptorIt=this->NdiToolDescriptors.begin(); toolDescriptorIt!=this->NdiToolDescriptors.end(); ++toolDescriptorIt)
  {
    toolDescriptorIt->second.PortEnabled = false;
  }

  // re-start the tracking
  if (this->IsDeviceTracking)
  {
    ndiCommand(this->Device,"TSTART:");
    int errnum = ndiGetError(this->Device);
    if (errnum)
    { 
      LOG_ERROR(ndiErrorString(errnum));
    }
  }
}

//----------------------------------------------------------------------------
PlusStatus vtkNDITracker::Beep(int n)
{
  if (this->Recording)
  {
    LOG_ERROR("vtkNDITracker::Beep failed: not connected to the device");
    return PLUS_FAIL;
  }
  if (n > 9)
  {
    n = 9;
  }
  if (n < 0)
  {
    n = 0;
  }
  ndiCommand(this->Device,"BEEP:%i",n);
  int errnum = ndiGetError(this->Device);
  /*
  if (errnum && errnum != NDI_NO_TOOL)
  {
    LOG_ERROR(ndiErrorString(errnum));
    return PLUS_FAIL;
  }
  */
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus  vtkNDITracker::SetToolLED(const char* sourceId, int led, LedState state)
{
  if (!this->Recording)
  {
    LOG_ERROR("vtkNDITracker::InternalSetToolLED failed: not recording");
    return PLUS_FAIL;
  }
  NdiToolDescriptorsType::iterator ndiToolDescriptorIt = this->NdiToolDescriptors.find(sourceId);
  if (ndiToolDescriptorIt==this->NdiToolDescriptors.end())
  {
    LOG_ERROR("InternalSetToolLED failed: Tool descriptor is not found for tool "<<sourceId);
    return PLUS_FAIL;
  }
  int portHandle=ndiToolDescriptorIt->second.PortHandle;
  if (portHandle <= 0)
  {
    LOG_ERROR("vtkNDITracker::InternalSetToolLED failed: invalid port handle");
    return PLUS_FAIL;
  }

  int plstate = NDI_BLANK;
  switch (state)
  {
  case TR_LED_OFF: plstate = NDI_BLANK; break;
  case TR_LED_ON: plstate = NDI_SOLID; break;
  case TR_LED_FLASH: plstate = NDI_FLASH; break;
  default:
    LOG_ERROR("vtkNDITracker::InternalSetToolLED failed: unsupported LED state: "<<state);
    return PLUS_FAIL;
  }

  ndiCommand(this->Device, "LED:%02X%d%c", portHandle, led+1, plstate);
  int errnum = ndiGetError(this->Device);
  /*
  if (errnum && errnum != NDI_NO_TOOL)
  {
  LOG_ERROR(ndiErrorString(errnum));
  return 0;
  }
  */

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkNDITracker::UpdatePortHandle(NdiToolDescriptor& toolDescriptor)
{  
  if (toolDescriptor.WiredPortNumber>=0) // wired tool
  {
    ndiCommand(this->Device, "PHSR:00");
    int ntools = ndiGetPHSRNumberOfHandles(this->Device);
    int ndiToolIndex = 0;
    for (; ndiToolIndex < ntools; ndiToolIndex++)
    {
      if (ndiGetPHSRInformation(this->Device,ndiToolIndex) & NDI_TOOL_IN_PORT)
      {
        int portHandle = ndiGetPHSRHandle(this->Device,ndiToolIndex);
        ndiCommand(this->Device,"PHINF:%02X0021",portHandle);
        char location[14];
        ndiGetPHINFPortLocation(this->Device,location);
        int foundWiredPortNumber = (location[10]-'0')*10 + (location[11]-'0') - 1;
        int foundWiredPortChannel = (location[12]-'0')*10 + (location[13]-'0'); // this is nonzero if 5-DOF tools with splitter
        int combinedPortAndChannelNumber = foundWiredPortChannel*100 + foundWiredPortNumber;
        if (toolDescriptor.WiredPortNumber == combinedPortAndChannelNumber)
        {
          // found the portHandle
          toolDescriptor.PortHandle = portHandle;
          break;
        }
      }
    }
    if (ndiToolIndex == ntools)
    {
      LOG_ERROR("Active NDI tool not found in port " << toolDescriptor.WiredPortNumber <<". Make sure the tool is plugged in.");
      return PLUS_FAIL;
    }    
  }
  else // wireless tool
  {
    ndiCommand(this->Device, "PHRQ:*********1****");
    int portHandle = ndiGetPHRQHandle(this->Device);
    toolDescriptor.PortHandle = portHandle;
  }

  int errnum = ndiGetError(this->Device);
  if (errnum)
  {
    LOG_ERROR(ndiErrorString(errnum));
    return PLUS_FAIL;
  }
  
  return PLUS_SUCCESS;
}


//----------------------------------------------------------------------------
PlusStatus vtkNDITracker::SendSromToTracker(const NdiToolDescriptor& toolDescriptor)
{
  if (toolDescriptor.VirtualSROM == NULL)
  {
    // nothing to load
    return PLUS_SUCCESS;
  }

  PlusLockGuard<vtkRecursiveCriticalSection> updateMutexGuardedLock(this->UpdateMutex);
  const int TRANSFER_BLOCK_SIZE=64; // in bytes
  char hexbuffer[TRANSFER_BLOCK_SIZE*2];
  for (int i = 0; i < VIRTUAL_SROM_SIZE; i += TRANSFER_BLOCK_SIZE)
  {
    ndiCommand(this->Device," VER 0");
    ndiCommand(this->Device, "PVWR:%02X%04X%.128s", toolDescriptor.PortHandle, i,
      ndiHexEncode(hexbuffer, &(toolDescriptor.VirtualSROM[i]), TRANSFER_BLOCK_SIZE));
  }

  int errnum = ndiGetError(this->Device);
  if (errnum) 
  {
    LOG_ERROR("Failed to send SROM to NDI tracker");
    LOG_ERROR(ndiErrorString(errnum));
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkNDITracker::ClearVirtualSromInTracker(NdiToolDescriptor& toolDescriptor)
{
  if (toolDescriptor.VirtualSROM==NULL)
  {
    // nothing to clear
    return PLUS_SUCCESS;
  }

  ndiCommand(this->Device, "PHF:%02X", toolDescriptor.PortHandle);
  toolDescriptor.PortEnabled = false;
  toolDescriptor.PortHandle = 0;

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkNDITracker::ReadConfiguration(vtkXMLDataElement* rootConfigElement)
{
  // Clean up any previously read config data
  for (NdiToolDescriptorsType::iterator toolDescriptorIt=this->NdiToolDescriptors.begin(); toolDescriptorIt!=this->NdiToolDescriptors.end(); ++toolDescriptorIt)
  {
    delete [] toolDescriptorIt->second.VirtualSROM;
    toolDescriptorIt->second.VirtualSROM = NULL;
  }
  this->NdiToolDescriptors.clear();

  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_READING(deviceConfig, rootConfigElement);

  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(unsigned long, SerialPort, deviceConfig);
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(unsigned long, BaudRate, deviceConfig);
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(int, MeasurementVolumeNumber, deviceConfig);

  XML_FIND_NESTED_ELEMENT_REQUIRED(dataSourcesElement, deviceConfig, "DataSources");

  for ( int nestedElementIndex = 0; nestedElementIndex < dataSourcesElement->GetNumberOfNestedElements(); nestedElementIndex++ )
  {
    vtkXMLDataElement* toolDataElement = dataSourcesElement->GetNestedElement(nestedElementIndex); 
    if ( STRCASECMP(toolDataElement->GetName(), "DataSource") != 0 )
    {
      // if this is not a data source element, skip it
      continue; 
    }
    if ( toolDataElement->GetAttribute("Type") != NULL && STRCASECMP(toolDataElement->GetAttribute("Type"), "Tool") != 0 )
    {
      // if this is not a Tool element, skip it
      continue; 
    }
    const char* toolId = toolDataElement->GetAttribute("Id");
    if ( toolId==NULL)
    {
      LOG_ERROR("Failed to initialize NDI tool: DataSource Id is missing");
      continue; 
    }
    PlusTransformName toolTransformName(toolId, this->GetToolReferenceFrameName());
    std::string toolSourceId=toolTransformName.GetTransformName();
    vtkPlusDataSource* trackerTool = NULL;
    if ( this->GetTool(toolSourceId, trackerTool) != PLUS_SUCCESS || trackerTool==NULL)
    {
      LOG_ERROR("Failed to get NDI tool: " << toolSourceId);
      continue; 
    }

    int wiredPortNumber = -1;
    const char* portName = toolDataElement->GetAttribute("PortName"); 
    if ( portName != NULL ) 
    {
      wiredPortNumber = atoi(portName);
      if (wiredPortNumber < 0)
      {
        LOG_WARNING("NDI wired tool's port number has to be >=0");
        continue;
      }
    }

    const char* romFileName = toolDataElement->GetAttribute("RomFile");

    NdiToolDescriptor toolDescriptor;
    toolDescriptor.PortEnabled=false;
    toolDescriptor.PortHandle=0;
    toolDescriptor.VirtualSROM=NULL;
    toolDescriptor.WiredPortNumber=-1;

    if (romFileName)
    {
      // Passive (wireless) tool
      if (wiredPortNumber>=0)
      {
        LOG_WARNING("NDI PortName and RomFile are both specified for tool "<<toolSourceId<<". The tool is assumed to be passive (wireless) and the PortName attribute is ignored");
      }
      std::string romFilePath = vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationPath(romFileName);
      this->ReadSromFromFile(toolDescriptor, romFilePath.c_str());
    }
    else
    {
      // Active (wired) tool
      toolDescriptor.WiredPortNumber = wiredPortNumber;
    }

    this->NdiToolDescriptors[toolSourceId]=toolDescriptor;
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkNDITracker::WriteConfiguration(vtkXMLDataElement* rootConfig)
{
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_WRITING(trackerConfig, rootConfig);
  trackerConfig->SetIntAttribute("SerialPort", this->SerialPort);
  trackerConfig->SetIntAttribute("BaudRate", this->BaudRate );
  trackerConfig->SetIntAttribute("MeasurementVolumeNumber", this->MeasurementVolumeNumber );
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
void vtkNDITracker::LogVolumeList(const char* ndiVolumeListCommandReply, int selectedVolume, vtkPlusLogger::LogLevelType logLevel)
{
  unsigned long numberOfVolumes = ndiHexToUnsignedLong(ndiVolumeListCommandReply, 1);
  if (selectedVolume==0)
  {
    LOG_DYNAMIC("Number of available measurement volumes: "<<numberOfVolumes, logLevel);
  }
  for (int volIndex=0; volIndex<numberOfVolumes; volIndex++)
  {
    if (selectedVolume>0 && selectedVolume!=volIndex+1)
    {
      continue;
    }
    LOG_DYNAMIC("Measurement volume "<<volIndex+1, logLevel);
    const char* volDescriptor = ndiVolumeListCommandReply+1+volIndex*74;

    std::string shapeType;
    switch (volDescriptor[0])
    {
    case '9': shapeType="Cube volume"; break;
    case 'A': shapeType="Dome volume"; break;
    default: shapeType="unknown";
    }
    LOG_DYNAMIC(" Shape type: "<<shapeType<<" ("<<volDescriptor[0]<<")", logLevel);

    LOG_DYNAMIC(" D1 (minimum x value) = "<<ndiSignedToLong(volDescriptor+1, 7)/100, logLevel);
    LOG_DYNAMIC(" D2 (maximum x value) = "<<ndiSignedToLong(volDescriptor+8, 7)/100, logLevel);
    LOG_DYNAMIC(" D3 (minimum y value) = "<<ndiSignedToLong(volDescriptor+15, 7)/100, logLevel);
    LOG_DYNAMIC(" D4 (maximum y value) = "<<ndiSignedToLong(volDescriptor+22, 7)/100, logLevel);
    LOG_DYNAMIC(" D5 (minimum z value) = "<<ndiSignedToLong(volDescriptor+29, 7)/100, logLevel);
    LOG_DYNAMIC(" D6 (maximum z value) = "<<ndiSignedToLong(volDescriptor+36, 7)/100, logLevel);
    LOG_DYNAMIC(" D7 (reserved) = "<<ndiSignedToLong(volDescriptor+43, 7)/100, logLevel);
    LOG_DYNAMIC(" D8 (reserved) = "<<ndiSignedToLong(volDescriptor+50, 7)/100, logLevel);
    LOG_DYNAMIC(" D9 (reserved) = "<<ndiSignedToLong(volDescriptor+57, 7)/100, logLevel);
    LOG_DYNAMIC(" D10 (reserved) = "<<ndiSignedToLong(volDescriptor+64, 7)/100, logLevel);

    LOG_DYNAMIC(" Reserved: "<<volDescriptor[71], logLevel);

    std::string metalResistant;
    switch (volDescriptor[72])
    {
    case '0': metalResistant="no information"; break;
    case '1': metalResistant="metal resistant"; break;
    case '2': metalResistant="not metal resistant"; break;
    default: metalResistant="unknown";
    }
    LOG_DYNAMIC(" Metal resistant: "<<metalResistant<<" ("<<volDescriptor[72]<<")", logLevel);
  }
}
