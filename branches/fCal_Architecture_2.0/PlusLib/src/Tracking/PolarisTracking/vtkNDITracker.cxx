/*=========================================================================

  Program:   AtamaiTracking for VTK
  Module:    $RCSfile: vtkNDITracker.cxx,v $
  Creator:   David Gobbi <dgobbi@atamai.com>
  Language:  C++
  Author:    $Author: kcharbon $
  Date:      $Date: 2008/07/31 14:17:49 $
  Version:   $Revision: 1.16 $

==========================================================================

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
#include "vtkFrameToTimeConverter.h"
#include "vtkObjectFactory.h"
#include "vtkSocketCommunicator.h"
#include <string.h>
#include "vtkCharArray.h"

//----------------------------------------------------------------------------
vtkNDITracker* vtkNDITracker::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkNDITracker");
  if(ret)
    {
    return (vtkNDITracker*)ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkNDITracker;
}

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
  this->SetNumberOfTools(VTK_NDI_NTOOLS);

  for (int i = 0; i < VTK_NDI_NTOOLS; i++)
    {
    this->PortHandle[i] = 0;
    this->PortEnabled[i] = 0;
    this->VirtualSROM[i] = 0;
    }

  // for accurate timing
  this->Timer = vtkFrameToTimeConverter::New();
  this->Timer->SetNominalFrequency(60.0);
}

//----------------------------------------------------------------------------
vtkNDITracker::~vtkNDITracker() 
{
  if (this->Tracking)
    {
    this->StopTracking();
    }
  this->SendMatrix->Delete();
  for (int i = 0; i < VTK_NDI_NTOOLS; i++)
    {
    if (this->VirtualSROM[i] != 0)
      {
      delete [] this->VirtualSROM[i];
      }
    }
  if (this->Version)
    {
    delete [] this->Version;
    }
  if (this->Timer)
    {
    this->Timer->Delete();
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
int vtkNDITracker::Probe()
{
  int errnum = NDI_OPEN_ERROR;;
  char *devicename = this->SerialDevice;

  if (this->IsDeviceTracking)
    {
    return 1;
    }
  //client
  if(!this->ServerMode && this->RemoteAddress)
    {
    int success[1] = {0};
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
    if(!this->SocketCommunicator->Receive(success, 1,1, 11))
      {  
      vtkErrorMacro("Could not receive the success information"
        " from server Probe()\n");
      }
    }
  }
      else
  {
  vtkErrorMacro("Could not send message Probe");
  }
      }
      return success[0];//0;
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
  vtkErrorMacro("Could not send the success information.\n");
  }
      }
    return 1;
    }
  int success[1] = {0};
  // server
  if(this->ServerMode && this->SocketCommunicator->GetIsConnected()>0)
    {
    if(!this->SocketCommunicator->Send(success, 1, 1, 11))
      {
      vtkErrorMacro("Could not send the success information.\n");
      }
    }
  return 0;
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
      vtkErrorMacro(<< ndiErrorString(NDI_OPEN_ERROR));
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
int vtkNDITracker::InternalStartTracking()
{
  int errnum, tool;
  int baud;

  if (this->IsDeviceTracking)
    {
    return 1;
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
      vtkErrorMacro(<< "Illegal baud rate");
      return 0;
    }

  char *devicename = this->SerialDevice;
  if (devicename == 0 || devicename[0] == '\0')
    {
    devicename = ndiDeviceName(this->SerialPort-1);
    }
  this->Device = ndiOpen(devicename);
  if (this->Device == 0) 
    {
    vtkErrorMacro(<< ndiErrorString(NDI_OPEN_ERROR));
    return 0;
    }
  // initialize Device
  ndiCommand(this->Device,"INIT:");
  if (ndiGetError(this->Device))
    {
    ndiRESET(this->Device);
    errnum = ndiGetError(this->Device);
    if (errnum) 
      {
      vtkErrorMacro(<< ndiErrorString(errnum));
      ndiClose(this->Device);
      this->Device = 0;
      return 0;
      }
    ndiCommand(this->Device,"INIT:");
    if (errnum) 
      {
      vtkErrorMacro(<< ndiErrorString(errnum));
      ndiClose(this->Device);
      this->Device = 0;
      return 0;
      }
    }

  // set the baud rate
  // also: NOHANDSHAKE cuts down on CRC errs and timeouts
  ndiCommand(this->Device,"COMM:%d%03d%d",baud,NDI_8N1,NDI_NOHANDSHAKE);
  errnum = ndiGetError(this->Device);
  if (errnum) 
    {
    vtkErrorMacro(<< ndiErrorString(errnum));
    ndiClose(this->Device);
    this->Device = 0;
    return 0;
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
    vtkErrorMacro(<< ndiErrorString(errnum));
    ndiClose(this->Device);
    this->Device = 0;
    return 0;
    }

  // for accurate timing
  this->Timer->Initialize();

  this->IsDeviceTracking = 1;

  return 1;
}

//----------------------------------------------------------------------------
int vtkNDITracker::InternalStopTracking()
{
  if (this->Device == 0)
    {
    return 0;
    }

  int errnum, tool;

  ndiCommand(this->Device,"TSTOP:");
  errnum = ndiGetError(this->Device);
  if (errnum) 
    {
    vtkErrorMacro(<< ndiErrorString(errnum));
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
    vtkErrorMacro(<< ndiErrorString(errnum));
    }
  ndiClose(this->Device);
  this->Device = 0;

  return 1;
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

void vtkNDITracker::InternalUpdate()
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
    vtkWarningMacro( << "called Update() when NDI was not tracking");
    return;
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
      vtkWarningMacro(<< ndiErrorString(errnum));
      }
    else
      {
      vtkErrorMacro(<< ndiErrorString(errnum));
      }
    return;
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
    nextcount = this->Timer->GetLastFrame() + 1;
    }

  // the timestamp is always created using the frame number of
  // the most recent transformation
  this->Timer->SetLastFrame(nextcount);
  double timestamp = this->Timer->GetTimeStampForFrame(nextcount);

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
      flags |= TR_MISSING;
      }
    else
      {
      if (absent[tool]) { flags |= TR_OUT_OF_VIEW;  }
      if (port_status & NDI_OUT_OF_VOLUME){ flags |= TR_OUT_OF_VOLUME; }
      if (port_status & NDI_SWITCH_1_ON)  { flags |= TR_SWITCH1_IS_ON; }
      if (port_status & NDI_SWITCH_2_ON)  { flags |= TR_SWITCH2_IS_ON; }
      if (port_status & NDI_SWITCH_3_ON)  { flags |= TR_SWITCH3_IS_ON; }
      }

    // if tracking relative to another tool
    if (this->ReferenceTool >= 0 && tool != this->ReferenceTool)
      {
      if (!absent[tool])
        {
        if (absent[this->ReferenceTool])
          {
          flags |= TR_OUT_OF_VIEW;
          }
        if (status[this->ReferenceTool] & NDI_OUT_OF_VOLUME)
          {
          flags |= TR_OUT_OF_VOLUME;
          }
        }
      // pre-multiply transform by inverse of relative tool transform
      ndiRelativeTransform(transform[tool],referenceTransform,transform[tool]);
      }
    ndiTransformToMatrixd(transform[tool],*this->SendMatrix->Element);
    this->SendMatrix->Transpose();

    // by default (if there is no camera frame number associated with
    // the tool transformation) the most recent timestamp is used.
    double tooltimestamp = timestamp;
    if (!absent[tool] && frame[tool])
      {
      // this will create a timestamp from the frame number      
      tooltimestamp = this->Timer->GetTimeStampForFrame(frame[tool]);
      }
    // send the matrix and flags to the tool
   
    this->ToolUpdate(tool,this->SendMatrix,flags,tooltimestamp);   
    }
}

//----------------------------------------------------------------------------
void vtkNDITracker::LoadVirtualSROM(int tool, const char *filename)
{
  char buff[1024];
  if( !this->ServerMode )// client & Normal
    {
    FILE *file = fopen(filename,"rb");
    if (file == NULL)
      {
      vtkErrorMacro("couldn't find srom file " << filename);
      return;
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
    vtkErrorMacro("Message could not be sent. \n");
    }
  }
      else
  {
  vtkErrorMacro("Could not send length. \n");
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
  return;
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

// helper method to strip whitespace
static char *vtkStripWhitespace(char *text)
{
  int n = strlen(text);
  // strip from right
  while (--n >= 0) {
    if (isspace(text[n])) {
      text[n] = '\0';
    }
    else {
      break;
    }
  }
  // strip from left
  while (isspace(*text)) {
    text++;
  }
  return text;
}

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
      vtkErrorMacro(<< ndiErrorString(errnum));
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
      vtkErrorMacro(<< ndiErrorString(errnum));
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
        vtkErrorMacro(<< ndiErrorString(errnum));
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
      vtkErrorMacro(<< ndiErrorString(errnum));
      }
    }

  // get information for all tools
  ndiCommand(this->Device,"PHSR:00");
  ntools = ndiGetPHSRNumberOfHandles(this->Device);
  for (tool = 0; tool < ntools; tool++)
    {
    ph = ndiGetPHSRHandle(this->Device,tool);
    ndiCommand(this->Device,"PHINF:%02X0025",ph);
    errnum = ndiGetError(this->Device);
    if (errnum)
      { 
      vtkErrorMacro(<< ndiErrorString(errnum));
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
    // decompose identity string from end to front
    ndiGetPHINFToolInfo(this->Device, identity);
    identity[31] = '\0';
    this->Tools[port]->SetToolSerialNumber(vtkStripWhitespace(&identity[23]));
    identity[23] = '\0';
    this->Tools[port]->SetToolRevision(vtkStripWhitespace(&identity[20]));
    identity[20] = '\0';
    this->Tools[port]->SetToolManufacturer(vtkStripWhitespace(&identity[8]));
    identity[8] = '\0';
    this->Tools[port]->SetToolType(vtkStripWhitespace(&identity[0]));
    ndiGetPHINFPartNumber(this->Device, partNumber);
    partNumber[20] = '\0';
    this->Tools[port]->SetToolPartNumber(vtkStripWhitespace(partNumber));
    status = ndiGetPHINFPortStatus(this->Device);

    // send the Tool Info to the server
    if(this->ServerMode)
      {
      if(this->SocketCommunicator->GetIsConnected()>0)
  {
  char msg[40];
  int len = 40;
  sprintf(msg, "SetToolSerialNumber:%d:%s",
    port, this->Tools[port]->GetToolSerialNumber());
  len = strlen(msg) +1;

  if( this->SocketCommunicator->Send(&len, 1, 1, 11) )
    {
    if( !this->SocketCommunicator->Send(msg, len, 1, 22) )
      {
      vtkErrorMacro("Could not Send SetToolSerialNumber");
      }
    }
  
//  ca->Delete();
  
  sprintf(msg, "SetToolRevision:%d:%s",
    port, this->Tools[port]->GetToolRevision());
  len = strlen(msg) + 1;

  if( this->SocketCommunicator->Send(&len, 1, 1, 11) )
    {
    if( !this->SocketCommunicator->Send(msg, len, 1, 22) )
      {
      vtkErrorMacro("Could not Send SetToolSerialNumber");
      }
    }
  sprintf(msg, "SetToolManufacturer:%d:%s",
    port, this->Tools[port]->GetToolManufacturer());
  len = strlen(msg) +1;
  vtkCharArray *ca2 = vtkCharArray::New();
  ca2->SetNumberOfComponents(len);
  ca2->SetArray(msg, len, 1);
  if( this->SocketCommunicator->Send(&len, 1, 1, 11) )
    {
    if( !this->SocketCommunicator->Send(msg, len, 1, 22) )
      {
      vtkErrorMacro("Could not Send SetToolSerialNumber");
      }
    }
  
  sprintf(msg, "SetToolType:%d:%s",
    port, this->Tools[port]->GetToolType());
  len = strlen(msg) +1;
  vtkCharArray *ca3 = vtkCharArray::New();
  ca3->SetNumberOfComponents(len);
  ca3->SetArray(msg, len, 1);//
  if( this->SocketCommunicator->Send(&len, 1, 1, 11) )
    {
    if( !this->SocketCommunicator->Send(msg, len, 1, 22) )
      {
      vtkErrorMacro("Could not Send SetToolSerialNumber");
      }
    }

  sprintf(msg, "SetToolPartNumber:%d:%s",
    port, this->Tools[port]->GetToolPartNumber());
  len = strlen(msg) + 1;
  vtkCharArray *ca4 = vtkCharArray::New();
  ca4->SetNumberOfComponents(len);
  ca4->SetArray(msg,  len, 1);
  if( this->SocketCommunicator->Send(&len, 1, 1, 11) )
    {
    if( !this->SocketCommunicator->Send(msg, len, 1, 22) )
      {
      vtkErrorMacro("Could not Send SetToolSerialNumber");
      }
    }
  }
      }
      // done sending the Tool Info
      
    this->PortEnabled[port] = ((status & NDI_ENABLED) != 0);
    
    if (this->Tools[port]->GetLED1())
      {
      this->InternalSetToolLED(tool,1,this->Tools[port]->GetLED1());
      }
    if (this->Tools[port]->GetLED2())
      {
      this->InternalSetToolLED(tool,2,this->Tools[port]->GetLED2());
      }
    if (this->Tools[port]->GetLED3())
      {
      this->InternalSetToolLED(tool,3,this->Tools[port]->GetLED3());
      }
    }
 
  // re-start the tracking
  if (this->IsDeviceTracking)
    {
    ndiCommand(this->Device,"TSTART:");
    errnum = ndiGetError(this->Device);
    if (errnum)
      { 
      vtkErrorMacro(<< ndiErrorString(errnum));
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
      vtkErrorMacro(<< ndiErrorString(errnum));
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
      vtkErrorMacro(<< ndiErrorString(errnum));
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
      vtkErrorMacro(<< ndiErrorString(errnum));
      }
    }
}

//----------------------------------------------------------------------------
int vtkNDITracker::GetFullTX(int tool, double transform[9]) {
  
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
int vtkNDITracker::InternalBeep(int n)
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
      vtkErrorMacro(<< ndiErrorString(errnum));
      return 0;
      }
    */
    }

  return 1;
}

//----------------------------------------------------------------------------
// change the state of an LED on the tool
int vtkNDITracker::InternalSetToolLED(int tool, int led, int state)
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
      return 0;
      }

    ndiCommand(this->Device, "LED:%02X%d%c", ph, led+1, plstate);
    errnum = ndiGetError(this->Device);
    /*
    if (errnum && errnum != NDI_NO_TOOL)
      {
      vtkErrorMacro(<< ndiErrorString(errnum));
      return 0;
      }
    */
    }

  return 1;
}

//----------------------------------------------------------------------------
void vtkNDITracker::InternalLoadVirtualSROM(int tool,
              const unsigned char data[1024])
{
  if (data == NULL)
    {
    return;
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
      vtkErrorMacro(<< "can't load SROM: no tool found in port " << tool);
      return;
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
    vtkErrorMacro(<< "LoadVirtualSROM: Tool number " << tool
      << "is out of range");
    return;
    }

  errnum = ndiGetError(this->Device);
  if (errnum)
    {
    vtkErrorMacro(<< ndiErrorString(errnum));
    return;
    }

  for ( i = 0; i < 1024; i += 64)
    {
    ndiCommand(this->Device," VER 0");
    ndiCommand(this->Device, "PVWR:%02X%04X%.128s",
         ph, i, ndiHexEncode(hexbuffer, &data[i], 64));
    }  
}

//----------------------------------------------------------------------------
void vtkNDITracker::InternalClearVirtualSROM(int tool)
{
  if (tool < 0 || tool >= VTK_NDI_NTOOLS)
    {
    vtkErrorMacro(<< "ClearVirtualSROM: Tool number " << tool
      << "is out of range");
    return;
    }

  int ph = this->PortHandle[tool];
  ndiCommand(this->Device, "PHF:%02X", ph);
  this->PortEnabled[tool] = 0;
  this->PortHandle[tool] = 0;
}

//----------------------------------------------------------------------------
void vtkNDITracker::InternalInterpretCommand( char * messageText)
{
  if( !messageText)
    {
    return;
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
    return ;
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
    return ;
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
    return ;
    }

  if( token1 && !strcmp( token1, "SetToolType" ))
    {
    token2 = strtok(NULL,":");
    if(token2)
      {
      port = atoi(token2);
      token3 = strtok(NULL,":");
      }
    this->Tools[port]->SetToolType( token3);
    return ;
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
    return ;
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
    return ;
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
    return ;
    }
}


