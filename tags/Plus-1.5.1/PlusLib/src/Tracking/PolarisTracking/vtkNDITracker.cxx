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

#include <limits.h>
#include <float.h>
#include <math.h>
#include <ctype.h>
#include "ndicapi.h"
#include "ndicapi_math.h"
#include "vtkMath.h"
#include "vtkTimerLog.h"
#include "vtkMatrix4x4.h"
#include "vtkTransform.h"
#include "vtkCriticalSection.h"
#include "vtkNDITracker.h"
#include "vtkTrackerTool.h"
#include "vtkObjectFactory.h"
#include "vtkSocketCommunicator.h"
#include <string.h>
#include "vtkCharArray.h"

vtkStandardNewMacro(vtkNDITracker);

//----------------------------------------------------------------------------
vtkNDITracker::vtkNDITracker()
{
  this->Device = 0;
  this->Version = NULL;
  this->CommandReply[0] = '\0';
  this->SendMatrix = vtkMatrix4x4::New();
  this->IsDeviceTracking = 0;
  this->SerialPort = -1; // default is to probe
  this->SerialDevice = 0;
  this->BaudRate = 9600;

  for (int i = 0; i < VTK_NDI_NTOOLS; i++)
  {
    this->PortHandle[i] = 0;
    this->PortEnabled[i] = 0;
    this->VirtualSROM[i] = 0;
  }

  this->UpdateNominalFrequency=60.0;
  this->LastFrameNumber=0;

  this->ServerMode=0;
  this->RemoteAddress=NULL;
  this->SocketCommunicator=vtkSocketCommunicator::New();

  this->ReferenceTool=0;
}

//----------------------------------------------------------------------------
vtkNDITracker::~vtkNDITracker() 
{
  if (this->Tracking)
  {
    this->StopTracking();
  }
  if (this->SendMatrix!=NULL)
  {
    this->SendMatrix->Delete();
    this->SendMatrix=NULL;
  }
  for (int i = 0; i < VTK_NDI_NTOOLS; i++)
  {
    if (this->VirtualSROM[i] != 0)
    {
      delete [] this->VirtualSROM[i];
      this->VirtualSROM[i]=NULL;
    }
  }
  if (this->Version)
  {
    delete [] this->Version;
    this->Version=NULL;
  }
  if (this->SocketCommunicator!=NULL)
  {
    this->SocketCommunicator->Delete();
    this->SocketCommunicator=NULL;
  }
}

//----------------------------------------------------------------------------
void vtkNDITracker::PrintSelf(ostream& os, vtkIndent indent)
{
  vtkTracker::PrintSelf(os,indent);

  os << indent << "SendMatrix: " << this->SendMatrix << "\n";
  this->SendMatrix->PrintSelf(os,indent.GetNextIndent());
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
  int errnum = NDI_OPEN_ERROR;;
  char *devicename = this->SerialDevice;

  if (this->IsDeviceTracking)
  {
    return PLUS_SUCCESS;
  }
  //client
  if(!this->ServerMode && this->RemoteAddress)
  {
    int success=0;
    char *msg = "Probe";
    int len = 6;

    if(this->SocketCommunicator->GetIsConnected()>0)
    {
      if(this->SocketCommunicator->Send(&len, 1, 1, 11))
      {
        if(this->SocketCommunicator->Send(msg, len, 1, 22))
        { 
          //wait to receive the information whether the Probe() was
          //successful on the server;
          if(!this->SocketCommunicator->Receive(&success, 1,1, 11))
          {  
            LOG_ERROR("Could not receive the success information from server Probe()\n");
          }
        }
      }
      else
      {
        LOG_ERROR("Could not send message Probe");
      }
    }
    return (success)?PLUS_SUCCESS:PLUS_FAIL;
  }

  // server & normal
  // if SerialPort is set to -1, then probe all serial ports
  if ((this->SerialDevice == 0 || this->SerialDevice[0] == '\0') &&
    this->SerialPort < 0)
  {
    for (int i = 0; i < 8; i++)
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
  else // otherwise probe the specified serial port only
  {
    if (devicename == 0 ||  devicename[0] == '\0')
    {
      devicename = ndiDeviceName(this->SerialPort-1);
    }
    if (devicename)
    {
      errnum = ndiProbe(devicename);
    }
  }

  // if probe was okay, then send VER:0 to identify device
  if (errnum == NDI_OKAY)
  {
    this->Device = ndiOpen(devicename);
    if (this->Device)
    {
      this->SetVersion(ndiVER(this->Device,0));

      ndiClose(this->Device);
      this->Device = 0;
    }
    int success[1] = {1};
    // server
    if(this->ServerMode && this->SocketCommunicator->GetIsConnected()>0)
    {
      if(!this->SocketCommunicator->Send(success, 1, 1, 11))
      {
        LOG_ERROR("Could not send the success information.\n");
      }
    }
    return PLUS_SUCCESS;
  }
  int success=0;
  // server
  if(this->ServerMode && this->SocketCommunicator->GetIsConnected()>0)
  {
    if(!this->SocketCommunicator->Send(&success, 1, 1, 11))
    {
      LOG_ERROR("Could not send the success information.\n");
    }
  }
  return PLUS_FAIL;
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
    this->RequestUpdateMutex->Lock();
    this->UpdateMutex->Lock();
    this->RequestUpdateMutex->Unlock();
    strncpy(this->CommandReply, ndiCommand(this->Device, command), 
      VTK_NDI_REPLY_LEN-1);
    this->CommandReply[VTK_NDI_REPLY_LEN-1] = '\0';
    this->UpdateMutex->Unlock();
  }
  else
  {
    char *devicename = this->SerialDevice;
    if (devicename == 0 || devicename[0] == '\0')
    {
      devicename = ndiDeviceName(this->SerialPort-1);
    }
    this->Device = ndiOpen(devicename);
    if (this->Device == 0) 
    {
      LOG_ERROR(ndiErrorString(NDI_OPEN_ERROR));
    }
    else
    {
      strncpy(this->CommandReply, ndiCommand(this->Device, command), 
        VTK_NDI_REPLY_LEN-1);
      this->CommandReply[VTK_NDI_REPLY_LEN-1] = '\0';
      ndiClose(this->Device);
    }
    this->Device = 0;
  }

  return this->CommandReply;
}

//----------------------------------------------------------------------------
PlusStatus vtkNDITracker::InternalStartTracking()
{
  int errnum, tool;
  int baud;

  if (this->IsDeviceTracking)
  {
    return PLUS_SUCCESS;
  }

  switch (this->BaudRate)
  {
  case 9600: baud = NDI_9600; break; 
  case 14400: baud = NDI_14400; break; 
  case 19200: baud = NDI_19200; break; 
  case 38400: baud = NDI_38400; break; 
  case 57600: baud = NDI_57600; break; 
  case 115200: baud = NDI_115200; break;
  default:
    LOG_ERROR("Illegal baud rate");
    return PLUS_FAIL;
  }

  char *devicename = this->SerialDevice;
  if (devicename == 0 || devicename[0] == '\0')
  {
    devicename = ndiDeviceName(this->SerialPort-1);
  }
  this->Device = ndiOpen(devicename);
  if (this->Device == 0) 
  {
    LOG_ERROR(ndiErrorString(NDI_OPEN_ERROR));
    return PLUS_FAIL;
  }
  // initialize Device
  ndiCommand(this->Device,"INIT:");
  if (ndiGetError(this->Device))
  {
    ndiRESET(this->Device);
    errnum = ndiGetError(this->Device);
    if (errnum) 
    {
      LOG_ERROR(ndiErrorString(errnum));
      ndiClose(this->Device);
      this->Device = 0;
      return PLUS_FAIL;
    }
    ndiCommand(this->Device,"INIT:");
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

  // get information about the device
  this->SetVersion(ndiVER(this->Device,0));

  for (tool = 0; tool < VTK_NDI_NTOOLS; tool++)
  {
    this->PortHandle[tool] = 0;
    if (this->VirtualSROM[tool])
    {
      this->InternalLoadVirtualSROM(tool,this->VirtualSROM[tool]);
    }
  }
  this->EnableToolPorts();

  ndiCommand(this->Device,"TSTART:");

  errnum = ndiGetError(this->Device);
  if (errnum) 
  {
    LOG_ERROR(ndiErrorString(errnum));
    ndiClose(this->Device);
    this->Device = 0;
    return PLUS_FAIL;
  }

  this->IsDeviceTracking = 1;

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkNDITracker::InternalStopTracking()
{
  if (this->Device == 0)
  {
    return PLUS_FAIL;
  }

  int errnum, tool;

  ndiCommand(this->Device,"TSTOP:");
  errnum = ndiGetError(this->Device);
  if (errnum) 
  {
    LOG_ERROR(ndiErrorString(errnum));
  }
  this->IsDeviceTracking = 0;

  for (tool = 0; tool < VTK_NDI_NTOOLS; tool++)
  {
    if (this->VirtualSROM[tool])
    {
      this->InternalClearVirtualSROM(tool);
    }
  }

  this->DisableToolPorts();

  // return to default comm settings
  ndiCommand(this->Device,"COMM:00000");
  errnum = ndiGetError(this->Device);
  if (errnum) 
  {
    LOG_ERROR(ndiErrorString(errnum));
  }
  ndiClose(this->Device);
  this->Device = 0;

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
// Important notes on the data collection rate of the Polaris:
//
// The camera frame rate is 60Hz, and therefore the maximum data
// collection rate is also 60Hz.  The maximum data transfer rate
// to the computer is also 60Hz.
//
// Depending on the number of enabled tools, the data collection
// rate might be reduced.  Each of the active tools requires one
// camera frame, and all the passive tools (if any are enabled)
// collectively require one camera frame.
//
// Therefore if there are two enabled active tools, the data rate
// is reduced to 30Hz.  Ditto for an active tool and a passive tool.
// If all tools are passive, the data rate is 60Hz.  With 3 active
// tools and one or more passive tools, the data rate is 15Hz.
// With 3 active tools, or 2 active and one or more passive tools,
// the data rate is 20Hz.
//
// The data transfer rate to the computer is independent of the data
// collection rate, and there might be duplicated records.  The
// data tranfer rate is limited by the speed of the serial port
// and by the number of characters sent per data record.  If tools
// are marked as 'missing' then the number of characters that
// are sent will be reduced.

PlusStatus vtkNDITracker::InternalUpdate()
{
  int errnum, tool, ph;
  int status[VTK_NDI_NTOOLS];
  int absent[VTK_NDI_NTOOLS];
  unsigned long frame[VTK_NDI_NTOOLS];
  double transform[VTK_NDI_NTOOLS][8];
  double *referenceTransform = 0;
  long flags;
  const unsigned long mflags = NDI_TOOL_IN_PORT | NDI_INITIALIZED | NDI_ENABLED;

  if (!this->IsDeviceTracking)
  {
    LOG_ERROR("called Update() when NDI was not tracking");
    return PLUS_FAIL;
  }

  // initialize transformations to identity
  for (tool = 0; tool < VTK_NDI_NTOOLS; tool++)
  {
    transform[tool][0] = 1.0;
    transform[tool][1] = transform[tool][2] = transform[tool][3] = 0.0;
    transform[tool][4] = transform[tool][5] = transform[tool][6] = 0.0;
    transform[tool][7] = 0.0;
  }

  // get the transforms for all tools from the NDI
  ndiCommand(this->Device,"TX:0801");
  //fprintf(stderr,"TX:0001 %s\n",ndiCommand(this->Device,"TX:0001"));
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

  // default to incrementing frame count by one (in case there are
  // no transforms for any tools)
  unsigned long nextcount = 0;

  for (tool = 0; tool < VTK_NDI_NTOOLS; tool++)
  {
    ph = this->PortHandle[tool];
    if (ph == 0)
    {
      continue;
    }

    absent[tool] = ndiGetTXTransform(this->Device, ph, transform[tool]);
    status[tool] = ndiGetTXPortStatus(this->Device, ph);
    frame[tool] = ndiGetTXFrame(this->Device, ph);
    if (!absent[tool] && frame[tool] > nextcount)
    { // 'nextcount' is max frame number returned
      nextcount = frame[tool];
    }
  }

  // if no transforms were returned, advance frame count by 1
  // (assume the NDI will be returning the empty records at
  // its maximum reporting rate of 60Hz)
  if (nextcount == 0)
  {
    nextcount = this->LastFrameNumber + 1;
  }

  // the timestamp is always created using the frame number of
  // the most recent transformation
  this->LastFrameNumber=nextcount;

  const double unfilteredTimestamp = vtkAccurateTimer::GetSystemTime();

  // check to see if any tools have been plugged in
  if (ndiGetTXSystemStatus(this->Device) & NDI_PORT_OCCUPIED)
  { // re-configure, a new tool has been plugged in
    this->EnableToolPorts();
  }
  else
  {
    for (tool = 0; tool < VTK_NDI_NTOOLS; tool++)
    {
      this->PortEnabled[tool] = ((status[tool] & mflags) == mflags);
    }
  }

  if (this->ReferenceTool >= 0)
  { // copy reference tool transform
    referenceTransform = transform[this->ReferenceTool];
  }

  for (tool = 0; tool < VTK_NDI_NTOOLS; tool++) 
  {
    // convert status flags from NDI to vtkTracker format
    int port_status = status[tool];
    flags = 0;
    if ((port_status & mflags) != mflags) 
    {
      flags = TOOL_MISSING;
    }
    else
    {
      if (absent[tool]) { flags = TOOL_OUT_OF_VIEW;  }
      if (port_status & NDI_OUT_OF_VOLUME){ flags = TOOL_OUT_OF_VOLUME; }
      //if (port_status & NDI_SWITCH_1_ON)  { flags = TOOL_SWITCH1_IS_ON; } // TODO all these button state flags are on regardless of the actual state
      //if (port_status & NDI_SWITCH_2_ON)  { flags = TOOL_SWITCH2_IS_ON; }
      //if (port_status & NDI_SWITCH_3_ON)  { flags = TOOL_SWITCH3_IS_ON; }
    }

    // if tracking relative to another tool
    if (this->ReferenceTool >= 0 && tool != this->ReferenceTool)
    {
      if (!absent[tool])
      {
        if (absent[this->ReferenceTool])
        {
          flags = TOOL_OUT_OF_VIEW;
        }
        if (status[this->ReferenceTool] & NDI_OUT_OF_VOLUME)
        {
          flags = TOOL_OUT_OF_VOLUME;
        }
      }
      // pre-multiply transform by inverse of relative tool transform
      ndiRelativeTransform(transform[tool],referenceTransform,transform[tool]);
    }

    ndiTransformToMatrixd(transform[tool],*this->SendMatrix->Element);
    this->SendMatrix->Transpose();

    // by default (if there is no camera frame number associated with
    // the tool transformation) the most recent timestamp is used.
    double tooltimestamp = unfilteredTimestamp;
    double toolframe = this->LastFrameNumber;
    if (!absent[tool] && frame[tool])
    {
      // this will create a timestamp from the frame number      
      toolframe = frame[tool];
    }
    
    std::ostringstream toolPortName; 
    toolPortName << tool; 
    vtkTrackerTool* trackerTool = NULL; 
    if ( this->GetToolByPortName(toolPortName.str().c_str(), trackerTool) != PLUS_SUCCESS )
    {
      if (flags != TOOL_MISSING)
      {
        LOG_ERROR("Failed to get tool by port name: " << toolPortName.str() ); 
      }
    }
    else
    {
      // send the matrix and status to the tool's vtkTrackerBuffer
      this->ToolTimeStampedUpdate(trackerTool->GetToolName(), this->SendMatrix, (ToolStatus)flags, toolframe, tooltimestamp);
    }
  }
  
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkNDITracker::LoadVirtualSROM(int tool, const char *filename)
{
  char buff[1024];
  if( !this->ServerMode )// client & Normal
  {
    FILE *file = fopen(filename,"rb");
    if (file == NULL)
    {
      LOG_ERROR("couldn't find srom file " << filename);
      return PLUS_FAIL;
    }

    if (this->VirtualSROM[tool] == 0)
    {
      this->VirtualSROM[tool] = new unsigned char[1024];
    }

    memset(this->VirtualSROM[tool],0,1024);
    fread(this->VirtualSROM[tool],1,1024,file);
    memset(buff,0,1024);
    memcpy(buff, this->VirtualSROM[tool], 1024);

    fclose(file);

  }
  if(!this->ServerMode && this->RemoteAddress) // client
  {
    int len[1]={1044};
    char msg[1045];
    memcpy(msg, "LoadVirtualSROM:", 16);
    memcpy(msg+16, "3:", 2);
    memcpy(msg+18, this->VirtualSROM[tool], 1024);
    memcpy(msg+1042, ":", 1);

    if(this->SocketCommunicator->GetIsConnected()>0)
    {
      if(this->SocketCommunicator->Send(len,1, 1,11))
      {
        if(!this->SocketCommunicator->Send(msg,len[0],1,22))
        {
          LOG_ERROR("Message could not be sent. \n");
        }
      }
      else
      {
        LOG_ERROR("Could not send length. \n");
      }
    }
  }
  if( this->ServerMode || !this->RemoteAddress) // server  & normal
  {
    if (this->Tracking)
    {

      this->RequestUpdateMutex->Lock();
      this->UpdateMutex->Lock();
      this->RequestUpdateMutex->Unlock();
      if (this->IsDeviceTracking)
      {
        ndiCommand(this->Device,"TSTOP:");
      }

      this->InternalLoadVirtualSROM(tool,this->VirtualSROM[tool]);

      if (this->IsDeviceTracking)
      {
        ndiCommand(this->Device,"TSTART:");
      }
      this->UpdateMutex->Unlock();
    }
  }
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
void vtkNDITracker::ClearVirtualSROM(int tool)
{
  if (this->VirtualSROM[tool] != 0)
  {
    delete [] this->VirtualSROM[tool];
  }

  this->VirtualSROM[tool] = 0;

  if (this->Tracking)
  {
    this->RequestUpdateMutex->Lock();
    this->UpdateMutex->Lock();
    this->RequestUpdateMutex->Unlock();
    if (this->IsDeviceTracking)
    {
      ndiCommand(this->Device,"TSTOP:");
    }
    this->InternalClearVirtualSROM(tool);
    if (this->IsDeviceTracking)
    {
      ndiCommand(this->Device,"TSTART:");
    }
    this->UpdateMutex->Unlock();
  }
}  

//----------------------------------------------------------------------------
// Protected Methods

//----------------------------------------------------------------------------
// Enable all tool ports that have tools plugged into them.
// The reference port is enabled with NDI_STATIC.
void vtkNDITracker::EnableToolPorts()
{
  int errnum = 0;
  int tool;
  int ph;
  int port;
  int mode;
  int ntools;
  int status;
  char identity[34];
  char location[14];
  char partNumber[24];

  // reset our information about the tool ports
  for (tool = 0; tool < VTK_NDI_NTOOLS; tool++)
  {
    if (tool < 3)
    { // only reset port handle for wired tools
      this->PortHandle[tool] = 0;
    }
    this->PortEnabled[tool] = 0;
  }

  // stop tracking
  if (this->IsDeviceTracking)
  {
    ndiCommand(this->Device,"TSTOP:");
    errnum = ndiGetError(this->Device);
    if (errnum)
    { 
      LOG_ERROR(ndiErrorString(errnum));
    }    
  }
  // free ports that are waiting to be freed
  ndiCommand(this->Device,"PHSR:01");
  ntools = ndiGetPHSRNumberOfHandles(this->Device);
  for (tool = 0; tool < ntools; tool++)
  {
    ph = ndiGetPHSRHandle(this->Device,tool);
    port = this->GetToolFromHandle(ph);
    ndiCommand(this->Device,"PHF:%02X",ph);
    //fprintf(stderr,"PHF:%02X\n",ph);
    errnum = ndiGetError(this->Device);
    if (errnum)
    { 
      LOG_ERROR(ndiErrorString(errnum));
    }
  }

  // initialize ports waiting to be initialized
  do // repeat as necessary (in case multi-channel tools are used) 
  {
    ndiCommand(this->Device,"PHSR:02");
    ntools = ndiGetPHSRNumberOfHandles(this->Device);
    for (tool = 0; tool < ntools; tool++)
    {
      ph = ndiGetPHSRHandle(this->Device,tool);
      ndiCommand(this->Device,"PINIT:%02X",ph);
      //fprintf(stderr,"PINIT:%02X\n",ph);
      errnum = ndiGetError(this->Device);
      if (errnum)
      { 
        LOG_ERROR(ndiErrorString(errnum));
      }
    }
  }
  while (ntools > 0 && errnum == 0);
  // enable initialized tools
  ndiCommand(this->Device,"PHSR:03");
  ntools = ndiGetPHSRNumberOfHandles(this->Device);
  for (tool = 0; tool < ntools; tool++)
  {
    ph = ndiGetPHSRHandle(this->Device,tool);
    ndiCommand(this->Device,"PHINF:%02X0001",ph);
    ndiGetPHINFToolInfo(this->Device,identity);
    if (identity[1] == 0x03) // button-box
    {
      mode = 'B';
    }
    else if (identity[1] == 0x01) // reference
    {
      mode = 'S';
    }
    else // anything else
    {
      mode = 'D';
    }

    // enable the tool
    ndiCommand(this->Device,"PENA:%02X%c",ph,mode);
    //fprintf(stderr,"PENA:%02X%c\n",ph,mode);
    errnum = ndiGetError(this->Device);
    if (errnum)
    {
      LOG_ERROR(ndiErrorString(errnum));
    }
  }

  // get information for all tools
  ndiCommand(this->Device,"PHSR:00");
  ntools = ndiGetPHSRNumberOfHandles(this->Device);
  ToolIteratorType it;
  for ( it = this->GetToolIteratorBegin(), tool = 0; it != this->GetToolIteratorEnd(); ++it, ++tool)
  {
    ph = ndiGetPHSRHandle(this->Device,tool);
    ndiCommand(this->Device,"PHINF:%02X0025",ph);
    errnum = ndiGetError(this->Device);
    if (errnum)
    { 
      LOG_ERROR(ndiErrorString(errnum));
      continue;
    }    
    // get the physical port identifier
    ndiGetPHINFPortLocation(this->Device,location);

    // check to see if the tool is wired
    if (location[9] == '0')
    {
      port = (location[10]-'0')*10 + (location[11]-'0') - 1;
      if (port >= 0 && port < VTK_NDI_NTOOLS)
      {
        this->PortHandle[port] = ph;
      }
    }
    else // wireless tool: find the port handle
    {
      for (port = 3; port < VTK_NDI_NTOOLS; port++)
      {
        if (this->VirtualSROM[port] && this->PortHandle[port] == ph)
        {
          break;
        }
      }
    }

    vtkTrackerTool* trackerTool = NULL; 
    if ( this->GetToolByPortName(it->second->GetPortName(), trackerTool) != PLUS_SUCCESS )
    {
      LOG_ERROR("Failed to get tool by port name: " << it->second->GetPortName() ); 
      continue; 
    }

    // decompose identity string from end to front
    ndiGetPHINFToolInfo(this->Device, identity);
    identity[31] = '\0';
    trackerTool->SetToolSerialNumber(PlusCommon::Trim(&identity[23]).c_str());
    identity[23] = '\0';
    trackerTool->SetToolRevision(PlusCommon::Trim(&identity[20]).c_str());
    identity[20] = '\0';
    trackerTool->SetToolManufacturer(PlusCommon::Trim(&identity[8]).c_str());
    identity[8] = '\0';
    //trackerTool->SetToolName(PlusCommon::Trim(&identity[0]).c_str()); // Here the tool name that comes from the configuration would be overridden by the tool number (setting also fails)
    ndiGetPHINFPartNumber(this->Device, partNumber);
    partNumber[20] = '\0';
    trackerTool->SetToolPartNumber(PlusCommon::Trim(&partNumber[0]).c_str());
    status = ndiGetPHINFPortStatus(this->Device);

    // send the Tool Info to the server
    if(this->ServerMode)
    {
      if(this->SocketCommunicator->GetIsConnected()>0)
      {
        char msg[40];
        int len = 40;
        sprintf(msg, "SetToolSerialNumber:%d:%s",
          port, trackerTool->GetToolSerialNumber());
        len = strlen(msg) +1;

        if( this->SocketCommunicator->Send(&len, 1, 1, 11) )
        {
          if( !this->SocketCommunicator->Send(msg, len, 1, 22) )
          {
            LOG_ERROR("Could not Send SetToolSerialNumber");
          }
        }

        //  ca->Delete();

        sprintf(msg, "SetToolRevision:%d:%s",
          port, trackerTool->GetToolRevision());
        len = strlen(msg) + 1;

        if( this->SocketCommunicator->Send(&len, 1, 1, 11) )
        {
          if( !this->SocketCommunicator->Send(msg, len, 1, 22) )
          {
            LOG_ERROR("Could not Send SetToolSerialNumber");
          }
        }
        sprintf(msg, "SetToolManufacturer:%d:%s",
          port, trackerTool->GetToolManufacturer());
        len = strlen(msg) +1;
        vtkCharArray *ca2 = vtkCharArray::New();
        ca2->SetNumberOfComponents(len);
        ca2->SetArray(msg, len, 1);
        if( this->SocketCommunicator->Send(&len, 1, 1, 11) )
        {
          if( !this->SocketCommunicator->Send(msg, len, 1, 22) )
          {
            LOG_ERROR("Could not Send SetToolSerialNumber");
          }
        }

        sprintf(msg, "SetToolPartNumber:%d:%s",
          port, trackerTool->GetToolPartNumber());
        len = strlen(msg) + 1;
        vtkCharArray *ca4 = vtkCharArray::New();
        ca4->SetNumberOfComponents(len);
        ca4->SetArray(msg,  len, 1);
        if( this->SocketCommunicator->Send(&len, 1, 1, 11) )
        {
          if( !this->SocketCommunicator->Send(msg, len, 1, 22) )
          {
            LOG_ERROR("Could not Send SetToolSerialNumber");
          }
        }
      }
    }
    // done sending the Tool Info

    this->PortEnabled[port] = ((status & NDI_ENABLED) != 0);

    if (trackerTool->GetLED1())
    {
      this->InternalSetToolLED(tool,1,trackerTool->GetLED1());
    }
    if (trackerTool->GetLED2())
    {
      this->InternalSetToolLED(tool,2,trackerTool->GetLED2());
    }
    if (trackerTool->GetLED3())
    {
      this->InternalSetToolLED(tool,3,trackerTool->GetLED3());
    }
  }

  // re-start the tracking
  if (this->IsDeviceTracking)
  {
    ndiCommand(this->Device,"TSTART:");
    errnum = ndiGetError(this->Device);
    if (errnum)
    { 
      LOG_ERROR(ndiErrorString(errnum));
    }
  }
}

//----------------------------------------------------------------------------
// Disable all enabled tool ports.
void vtkNDITracker::DisableToolPorts()
{
  int errnum = 0;
  int ph;
  int tool;
  int ntools;

  // stop tracking
  if (this->IsDeviceTracking)
  {
    ndiCommand(this->Device,"TSTOP:");
    errnum = ndiGetError(this->Device);
    if (errnum)
    { 
      LOG_ERROR(ndiErrorString(errnum));
    }    
  }

  // disable all enabled tools
  ndiCommand(this->Device,"PHSR:04");
  ntools = ndiGetPHSRNumberOfHandles(this->Device);
  for (tool = 0; tool < ntools; tool++)
  {
    ph = ndiGetPHSRHandle(this->Device,tool);
    ndiCommand(this->Device,"PDIS:%02X",ph);
    //fprintf(stderr,"PDIS:%02X\n",ph);
    errnum = ndiGetError(this->Device);
    if (errnum)
    { 
      LOG_ERROR(ndiErrorString(errnum));
    }    
  }

  // disable the enabled ports
  for (tool = 0; tool < VTK_NDI_NTOOLS; tool++)
  {
    this->PortEnabled[tool] = 0;
  }

  // re-start the tracking
  if (this->IsDeviceTracking)
  {
    ndiCommand(this->Device,"TSTART:");
    errnum = ndiGetError(this->Device);
    if (errnum)
    { 
      LOG_ERROR(ndiErrorString(errnum));
    }
  }
}

//----------------------------------------------------------------------------
int vtkNDITracker::GetFullTX(int tool, double transform[9]) 
{

  int ph = this->PortHandle[tool];
  if (ph == 0)
  {
    return -2;
  }
  int status = (double) ndiGetTXTransform(this->Device, ph, transform);
  return status;
}

//----------------------------------------------------------------------------
int vtkNDITracker::GetToolFromHandle(int handle)
{
  int tool;

  for (tool = 0; tool < VTK_NDI_NTOOLS; tool++)
  {
    if (this->PortHandle[tool] == handle)
    {
      return tool;
    }
  }

  return -1;
}

//----------------------------------------------------------------------------
// cause the NDI system to beep
PlusStatus vtkNDITracker::InternalBeep(int n)
{
  int errnum; 

  if (n > 9)
  {
    n = 9;
  }
  if (n < 0)
  {
    n = 0;
  }

  if (this->Tracking)
  {
    ndiCommand(this->Device,"BEEP:%i",n);
    errnum = ndiGetError(this->Device);
    /*
    if (errnum && errnum != NDI_NO_TOOL)
    {
    LOG_ERROR(ndiErrorString(errnum));
    return PLUS_FAIL;
    }
    */
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
// change the state of an LED on the tool
PlusStatus  vtkNDITracker::InternalSetToolLED(int tool, int led, int state)
{
  int plstate = NDI_BLANK;
  int errnum; 

  switch (state)
  {
  case 0: plstate = NDI_BLANK; break;
  case 1: plstate = NDI_SOLID; break;
  case 2: plstate = NDI_FLASH; break;
  }

  if (this->Tracking && tool >= 0 && tool < 3 && led > 0 && led < 3)
  {
    int ph = this->PortHandle[tool];
    if (ph == 0)
    {
      return PLUS_FAIL;
    }

    ndiCommand(this->Device, "LED:%02X%d%c", ph, led+1, plstate);
    errnum = ndiGetError(this->Device);
    /*
    if (errnum && errnum != NDI_NO_TOOL)
    {
    LOG_ERROR(ndiErrorString(errnum));
    return 0;
    }
    */
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkNDITracker::InternalLoadVirtualSROM(int tool,
                                            const unsigned char data[1024])
{
  if (data == NULL)
  {
    LOG_ERROR("InternalLoadVirtualSROM failed, invalid input data");
    return PLUS_FAIL;
  }

  int errnum;
  int ph = 0;
  int n, i;
  char hexbuffer[128];
  char location[14];

  if (tool >= 0 && tool < 3) // wired tools
  {
    ndiCommand(this->Device, "PHSR:00");
    n = ndiGetPHSRNumberOfHandles(this->Device);
    for (i = 0; i < n; i++)
    {
      if (ndiGetPHSRInformation(this->Device,i) & NDI_TOOL_IN_PORT)
      {
        ph = ndiGetPHSRHandle(this->Device,i);
        ndiCommand(this->Device,"PHINF:%02X0021",ph);
        ndiGetPHINFPortLocation(this->Device,location);
        if (tool == (location[10]-'0')*10 + (location[11]-'0') - 1)
        {
          break;
        }
      }
    }
    if (i == n)
    {
      LOG_ERROR("can't load SROM: no tool found in port " << tool);
      return PLUS_FAIL;
    }
  }
  else if (tool < VTK_NDI_NTOOLS) // wireless tools
  {
    ndiCommand(this->Device, "PHRQ:*********1****");
    ph = ndiGetPHRQHandle(this->Device);
    this->PortHandle[tool] = ph;
  }
  else
  {
    LOG_ERROR("LoadVirtualSROM: Tool number " << tool
      << "is out of range");
    return PLUS_FAIL;
  }

  errnum = ndiGetError(this->Device);
  if (errnum)
  {
    LOG_ERROR(ndiErrorString(errnum));
    return PLUS_FAIL;
  }

  for ( i = 0; i < 1024; i += 64)
  {
    ndiCommand(this->Device," VER 0");
    ndiCommand(this->Device, "PVWR:%02X%04X%.128s",
      ph, i, ndiHexEncode(hexbuffer, &data[i], 64));
  }  
  
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkNDITracker::InternalClearVirtualSROM(int tool)
{
  if (tool < 0 || tool >= VTK_NDI_NTOOLS)
  {
    LOG_ERROR("ClearVirtualSROM: Tool number " << tool
      << "is out of range");
    return PLUS_FAIL;
  }

  int ph = this->PortHandle[tool];
  ndiCommand(this->Device, "PHF:%02X", ph);
  this->PortEnabled[tool] = 0;
  this->PortHandle[tool] = 0;

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkNDITracker::InternalInterpretCommand( char * messageText)
{
  // TODO: Need to revise this code 
/*  if( !messageText)
  {
    LOG_ERROR("InternalInterpretCommand failed, messageText is invalid");
    return PLUS_FAIL;
  }

  int tool = 0;
  char *token1 = NULL;
  char *token2= NULL;
  char *token3 = NULL;
  token1 = strtok(messageText,":");
  int port;

  if( token1 && !strcmp( token1, "LoadVirtualSROM" ))
  {
    token2 = strtok(NULL,":");
    tool = atoi(token2);
    if (this->VirtualSROM[tool] == 0)
    {
      this->VirtualSROM[tool] = new unsigned char[1024];
    }

    memset(this->VirtualSROM[tool],0,1024);
    // copy the 1024 bytes from messageText to VirtualSROM
    memcpy(this->VirtualSROM[tool], messageText+18, 1024);
    this->LoadVirtualSROM(tool, NULL);
    return PLUS_SUCCESS;
  }

  if( token1 && !strcmp( token1, "SetToolManufacturer" ))
  {
    token2 = strtok(NULL,":");
    if(token2)
    {
      port = atoi(token2);
      token3 = strtok(NULL,":");
    }
    this->Tools[port]->SetToolManufacturer(token3);
    return PLUS_SUCCESS;
  }
  if( token1 && !strcmp( token1, "SetToolRevision" ))
  {
    token2 = strtok(NULL,":");
    if(token2)
    {
      port = atoi(token2);
      token3 = strtok(NULL,":");
    }
    this->Tools[port]->SetToolRevision( token3);
    return PLUS_SUCCESS;
  }

  if( token1 && !strcmp( token1, "SetToolType" ))
  {
    token2 = strtok(NULL,":");
    if(token2)
    {
      port = atoi(token2);
      token3 = strtok(NULL,":");
    }
    this->Tools[port]->SetToolType( TRACKER_TOOL_GENERAL); // TODO: maybe the tool type should be set, based on the token3 value
    this->Tools[port]->SetToolName( token3);
    return PLUS_SUCCESS;
  }

  if( token1 && !strcmp( token1, "SetToolPartNumber" ))
  {
    token2 = strtok(NULL,":");
    if(token2)
    {
      port = atoi(token2);
      token3 = strtok(NULL,":");
    }
    this->Tools[port]->SetToolPartNumber( token3);
    return PLUS_SUCCESS;
  }

  if( token1 && !strcmp( token1, "SetToolSerialNumber" ))
  {
    token2 = strtok(NULL,":");
    if(token2)
    {
      port = atoi(token2);
      token3 = strtok(NULL,":");
    }
    this->Tools[port]->SetToolSerialNumber( token3);
    return PLUS_SUCCESS;
  }
  if( token1 && !strcmp( token1, "InternalStopTrackingSuccessful" ))
  {
    this->IsDeviceTracking = 0;
    this->Tracking = 0;
    for ( tool=0; tool<VTK_NDI_NTOOLS; tool++ )
    {
      if (this->VirtualSROM[tool] != 0)
      {
        delete [] this->VirtualSROM[tool];
      }
      this->VirtualSROM[tool] = 0;
      this->PortEnabled[tool] = 0;
      this->PortHandle[tool] = 0;
    }
    return PLUS_SUCCESS;
  }

  LOG_ERROR("Unknown command: "<<token1);
  */
  return PLUS_FAIL;
}

//----------------------------------------------------------------------------
PlusStatus vtkNDITracker::ReadConfiguration(vtkXMLDataElement* config)
{
  // Read superclass configuration first
  Superclass::ReadConfiguration(config); 

  if ( config == NULL ) 
  {
    LOG_WARNING("Unable to find NDITracker XML data element");
    return PLUS_FAIL; 
  }

  vtkXMLDataElement* dataCollectionConfig = config->FindNestedElementWithName("DataCollection");
  if (dataCollectionConfig == NULL)
  {
    LOG_ERROR("Cannot find DataCollection element in XML tree!");
    return PLUS_FAIL;
  }

  vtkXMLDataElement* trackerConfig = dataCollectionConfig->FindNestedElementWithName("Tracker"); 
  if (trackerConfig == NULL) 
  {
    LOG_ERROR("Cannot find Tracker element in XML tree!");
    return PLUS_FAIL;
  }

  unsigned long serialPort(0); 
  if ( trackerConfig->GetScalarAttribute("SerialPort", serialPort) ) 
  {
    if ( !this->IsTracking() )
    {
      this->SetSerialPort(serialPort); 
    }
  }

  // Read ROM files for tools
  for ( int tool = 0; tool < trackerConfig->GetNumberOfNestedElements(); tool++ )
  {
    vtkXMLDataElement* toolDataElement = trackerConfig->GetNestedElement(tool); 
    if ( STRCASECMP(toolDataElement->GetName(), "Tool") != 0 )
    {
      // if this is not a Tool element, skip it
      continue; 
    }

    const char* portName = toolDataElement->GetAttribute("PortName"); 
    int portNumber = -1;
	  if ( portName != NULL ) 
	  {
      portNumber = atoi(portName);
      if (portNumber > 12)
      {
        LOG_WARNING("Port number has to be 12 or smaller!");
        continue;
      }
	  }
	  else
	  {
		  LOG_ERROR("Unable to find PortName! This attribute is mandatory in tool definition."); 
		  continue;
	  }

    const char* romFileName = toolDataElement->GetAttribute("RomFile");
    if (romFileName)
    {
      // Passive tools (that need Rom files) must have port number 4 or higher
      if (portNumber < 4)
      {
        LOG_ERROR("Invalid port number for passive marker! It has to be at least 4!");
        continue;
      }

      this->LoadVirtualSROM(portNumber, romFileName);
    }
  }

  return PLUS_SUCCESS;
}
