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

// Local includes
#include "PlusConfigure.h"
#include "vtkIGSIORecursiveCriticalSection.h"
#include "vtkPlusDataSource.h"
#include "vtkPlusNDITracker.h"

// NDI includes
#include <ndicapi.h>
#include <ndicapi_math.h>
#include <ndicapi_serial.h>

// VTK includes
#include <vtkCharArray.h>
#include <vtkMath.h>
#include <vtkMatrix4x4.h>
#include <vtkObjectFactory.h>
#include <vtkSocketCommunicator.h>
#include <vtkTimerLog.h>
#include <vtkTransform.h>

// System includes
#include <ctype.h>
#include <float.h>
#include <math.h>
#include <stdarg.h>

#if defined(HAVE_FUTURE)
  #include <future>
#endif

namespace
{
  const int VIRTUAL_SROM_SIZE = 1024;
}

//----------------------------------------------------------------------------

vtkStandardNewMacro(vtkPlusNDITracker);

//----------------------------------------------------------------------------
vtkPlusNDITracker::vtkPlusNDITracker()
  : LastFrameNumber(0)
  , Device(nullptr)
  , SerialDevice("")
  , SerialPort(-1)
  , BaudRate(0)
  , IsDeviceTracking(0)
  , LeaveDeviceOpenAfterProbe(false)
  , CheckDSR(true)
  , MeasurementVolumeNumber(0)
  , NetworkHostname("")
  , NetworkPort(8765)
  , CommandMutex(vtkIGSIORecursiveCriticalSection::New())
{
  memset(this->CommandReply, 0, VTK_NDI_REPLY_LEN);

  // PortName for data source is not required if RomFile is specified, so we don't need to enable this->RequirePortNameInDeviceSetConfiguration

  // No callback function provided by the device, so the data capture thread will be used to poll the hardware and add new items to the buffer
  this->StartThreadForInternalUpdates = true;
  this->AcquisitionRate = 50;
}

//----------------------------------------------------------------------------
vtkPlusNDITracker::~vtkPlusNDITracker()
{
  if (this->Recording)
  {
    this->StopRecording();
  }
  for (NdiToolDescriptorsType::iterator toolDescriptorIt = this->NdiToolDescriptors.begin(); toolDescriptorIt != this->NdiToolDescriptors.end(); ++toolDescriptorIt)
  {
    delete [] toolDescriptorIt->second.VirtualSROM;
    toolDescriptorIt->second.VirtualSROM = NULL;
  }
  this->CommandMutex->Delete();
  this->CommandMutex = nullptr;
}

//----------------------------------------------------------------------------
void vtkPlusNDITracker::PrintSelf(ostream& os, vtkIndent indent)
{
  Superclass::PrintSelf(os, indent);

  char ndiLog[USHRT_MAX];
  ndiLogState(this->Device, ndiLog);

  os << indent << "SerialDevice: " << this->SerialDevice << std::endl;
  os << indent << "SerialPort: " << this->SerialPort << std::endl;
  os << indent << "BaudRate: " << this->BaudRate << std::endl;
  os << indent << "IsDeviceTracking: " << this->IsDeviceTracking << std::endl;
  os << indent << "MeasurementVolumeNumber: " << this->MeasurementVolumeNumber << std::endl;
  os << indent << "CommandReply: " << this->CommandReply << std::endl;
  os << indent << "LastFrameNumber: " << this->LastFrameNumber << std::endl;
  os << indent << "LeaveDeviceOpenAfterProbe: " << this->LeaveDeviceOpenAfterProbe << std::endl;
  os << indent << "CheckDSR: " << this->CheckDSR << std::endl;
  for (auto iter = this->NdiToolDescriptors.begin(); iter != this->NdiToolDescriptors.end(); ++iter)
  {
    os << indent << iter->first << ": " << std::endl;
    os << indent << "  " << "PortEnabled: " << iter->second.PortEnabled << std::endl;
    os << indent << "  " << "PortHandle: " << iter->second.PortHandle << std::endl;
    os << indent << "  " << "VirtualSROM: " << iter->second.VirtualSROM << std::endl;
    os << indent << "  " << "WiredPortNumber: " << iter->second.WiredPortNumber << std::endl;
  }
}

//----------------------------------------------------------------------------
std::string vtkPlusNDITracker::GetSdkVersion()
{
  std::ostringstream version;
  version << "NDICAPI-" << NDICAPI_MAJOR_VERSION << "." << NDICAPI_MINOR_VERSION;
  return version.str();
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusNDITracker::Probe()
{
  if (this->IsDeviceTracking)
  {
    return PLUS_SUCCESS;
  }

  if (this->SerialPort > 0)
  {
    const char* devicename = NULL;
    int errnum = NDI_OPEN_ERROR;
    devicename = ndiSerialDeviceName(this->SerialPort - 1);
    if (devicename)
    {
      errnum = ndiSerialProbe(devicename, this->CheckDSR);
      LOG_DEBUG("Serial port " << devicename << " probe error (" << errnum << "): " << ndiErrorString(errnum));
    }

    if (errnum != NDI_OKAY)
    {
      return PLUS_FAIL;
    }

    this->Device = ndiOpenSerial(devicename);
    if (this->Device && !this->LeaveDeviceOpenAfterProbe)
    {
      CloseDevice(this->Device);
    }
    return PLUS_SUCCESS;
  }
  else if (this->NetworkHostname.empty())
  {
#if defined(HAVE_FUTURE)
    return ProbeSerialInternal();
#else
    // if SerialPort is set to -1 (default), then probe the first N serial ports
    const char* devicename = NULL;
    int errnum = NDI_OPEN_ERROR;

    const int MAX_SERIAL_PORT_NUMBER = 20; // the serial port is almost surely less than this number
    for (int i = 0; i < MAX_SERIAL_PORT_NUMBER; i++)
    {
      devicename = ndiSerialDeviceName(i);
      LOG_DEBUG("Testing serial port: " << devicename);
      if (devicename)
      {
        errnum = ndiSerialProbe(devicename, this->CheckDSR);
        LOG_DEBUG("Serial port " << devicename << " probe error (" << errnum << "): " << ndiErrorString(errnum));
        if (errnum == NDI_OKAY)
        {
          this->SerialPort = i + 1;
          this->Device = ndiOpenSerial(devicename);
          LOG_DEBUG("device: " << (this->Device == nullptr));
          if (this->Device && !this->LeaveDeviceOpenAfterProbe)
          {
            CloseDevice(this->Device);
          }
          return PLUS_SUCCESS;
        }
      }
    }
#endif
  }
  else
  {
    // TODO: probe network?
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
std::string vtkPlusNDITracker::Command(const char* format, ...)
{
  va_list ap;            // see stdarg.h
  va_start(ap, format);

  this->CommandReply[0] = '\0';

  char command[2048];
  if (format != nullptr)
  {
    vsprintf(command, format, ap);
    LOG_DEBUG("NDI Command:" << command);
  }
  else
  {
    LOG_DEBUG("NDI Command:send serial break");
  }

  // Linux and MacOSX require resetting of va parameters
  va_end(ap);
  va_start(ap, format);

  if (this->Device)
  {
    igsioLockGuard<vtkIGSIORecursiveCriticalSection> lock(this->CommandMutex);
    strncpy(this->CommandReply, ndiCommandVA(this->Device, format, ap), VTK_NDI_REPLY_LEN - 1);
    this->CommandReply[VTK_NDI_REPLY_LEN - 1] = '\0';
  }

  std::string cmd(command);
  if (cmd.find(':') != std::string::npos)
  {
    cmd = cmd.substr(0, cmd.find(':'));
  }
  else if (cmd.find(' ') != std::string::npos)
  {
    cmd = cmd.substr(0, cmd.find(' '));
  }
  bool isBinary = (cmd == "BX" || cmd == "GETLOG" || cmd == "VGET");
  if (!isBinary)
  {
    LOG_DEBUG("NDI Reply: " << this->CommandReply);
  }
  else
  {
    std::stringstream ss;
    ss << "NDI Reply: ";
    ss << std::hex;
    for (unsigned short i = 0 ; i < ndiGetBXReplyLength(this->Device); ++i)
    {
      ss << std::setfill('0') << std::setw(2) << (unsigned int)(unsigned char)this->CommandReply[i];
    }
    ss << std::endl;
    LOG_DEBUG(ss.str());
  }

  va_end(ap);

  return this->CommandReply;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusNDITracker::InternalConnect()
{
  PlusStatus result;
  if (!this->NetworkHostname.empty())
  {
    result = InternalConnectNetwork();
  }
  else
  {
    result = InternalConnectSerial();
  }

  if (result != PLUS_SUCCESS)
  {
    LOG_ERROR("Unable to connect to NDI device.");
    return result;
  }

  SelectMeasurementVolume();

  if (this->EnableToolPorts() != PLUS_SUCCESS)
  {
    LOG_ERROR("Failed to enable tool ports");
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusNDITracker::InternalConnectSerial()
{
  this->LeaveDeviceOpenAfterProbe = true;
  if (this->Probe() == PLUS_FAIL)
  {
    LOG_ERROR("Failed to detect device" << (this->SerialPort < 0 ? ". Port scanning failed. " : " on serial port " + std::string(ndiSerialDeviceName(this->SerialPort)) + " (index " + igsioCommon::ToString<int>(this->SerialPort) + "). ") << ndiErrorString(NDI_OPEN_ERROR));
    return PLUS_FAIL;
  }

  if (this->BaudRate != 0)
  {
    int baudVal = vtkPlusNDITracker::ConvertBaudToNDIEnum(this->BaudRate);
    // BaudRate has been requested, let's attempt it before falling back to best available
    this->Command("COMM:%X%03d%d", baudVal, NDI_8N1, NDI_NOHANDSHAKE);
    int errnum = ndiGetError(this->Device);
    if (errnum == NDI_OKAY)
    {
      return PLUS_SUCCESS;
    }
    else
    {
      LOG_WARNING("Unable to set requested baud rate. Reverting to auto-select.");
    }
  }

#if defined(WIN32)
  // TODO: use the lines in this comment to replace next 5 lines when VS2010 support is dropped:
  //auto baudRates = { NDI_1228739, NDI_921600, NDI_230400, NDI_115200, NDI_57600, NDI_38400, NDI_19200, NDI_14400, NDI_9600 };
  //for (auto baud : baudRates)
  const unsigned int numberOfBaudRates = 9;
  int baudRates[numberOfBaudRates] = { NDI_1228739, NDI_921600, NDI_230400, NDI_115200, NDI_57600, NDI_38400, NDI_19200, NDI_14400, NDI_9600 };
#elif defined(__APPLE__)
  const unsigned int numberOfBaudRates = 6;
  int baudRates[numberOfBaudRates] = { NDI_115200, NDI_57600, NDI_38400, NDI_19200, NDI_14400, NDI_9600 };
#elif defined(__linux__)
  const unsigned int numberOfBaudRates = 6;
  int baudRates[numberOfBaudRates] = { NDI_115200, NDI_57600, NDI_38400, NDI_19200, NDI_14400, NDI_9600 };
#else
  const unsigned int numberOfBaudRates = 6;
  int baudRates[numberOfBaudRates] = { NDI_115200, NDI_57600, NDI_38400, NDI_19200, NDI_14400, NDI_9600 };
#endif
  for (unsigned int baudIndex = 0; baudIndex < numberOfBaudRates; baudIndex++)
  {
    int baud = baudRates[baudIndex];

    this->Command("COMM:%X%03d%d", baud, NDI_8N1, NDI_NOHANDSHAKE);
    int errnum = ndiGetError(this->Device);
    if (errnum != NDI_OKAY && errnum != NDI_COMM_FAIL)
    {
      // A real problem, abort
      LOG_ERROR(ndiErrorString(errnum));
      ndiCloseSerial(this->Device);
      this->Device = nullptr;
      return PLUS_FAIL;
    }
    if (errnum == NDI_OKAY)
    {
      // Fastest baud rate set, end
      break;
    }
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusNDITracker::InternalConnectNetwork()
{
  this->Device = ndiOpenNetwork(this->NetworkHostname.c_str(), this->NetworkPort);

  if (this->Device == nullptr)
  {
    LOG_ERROR("Unable to connect to " << this->NetworkHostname << ":" << this->NetworkPort);
    return PLUS_FAIL;
  }

  // First init can take up to 5 seconds
  ndiTimeoutSocket(this->Device, 5000);

  auto reply = this->Command("INIT");

  // Subsequent commands should be much faster
  ndiTimeoutSocket(this->Device, 100);

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusNDITracker::InternalDisconnect()
{
  for (NdiToolDescriptorsType::iterator toolDescriptorIt = this->NdiToolDescriptors.begin(); toolDescriptorIt != this->NdiToolDescriptors.end(); ++toolDescriptorIt)
  {
    this->ClearVirtualSromInTracker(toolDescriptorIt->second);
  }

  this->DisableToolPorts();

  // return to default comm settings
  this->Command("COMM:00000");
  int errnum = ndiGetError(this->Device);
  if (errnum)
  {
    LOG_ERROR(ndiErrorString(errnum));
  }
  CloseDevice(this->Device);

  return PLUS_SUCCESS;
}


//----------------------------------------------------------------------------
PlusStatus vtkPlusNDITracker::InternalStartRecording()
{
  if (this->IsDeviceTracking)
  {
    return PLUS_SUCCESS;
  }

  this->Command("TSTART:");
  int errnum = ndiGetError(this->Device);
  if (errnum)
  {
    LOG_ERROR("Failed TSTART: " << ndiErrorString(errnum));
    CloseDevice(this->Device);
    return PLUS_FAIL;
  }

  this->IsDeviceTracking = 1;

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusNDITracker::InternalStopRecording()
{
  if (this->Device == 0)
  {
    return PLUS_FAIL;
  }

  this->Command("TSTOP:");
  int errnum = ndiGetError(this->Device);
  if (errnum)
  {
    LOG_ERROR(ndiErrorString(errnum));
  }
  this->IsDeviceTracking = 0;

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusNDITracker::InternalUpdate()
{
  if (!this->IsDeviceTracking)
  {
    LOG_ERROR("called Update() when NDI was not tracking");
    return PLUS_FAIL;
  }

  int errnum = 0;
  // get the transforms for all tools from the NDI
  this->Command("BX:0801");
  errnum = ndiGetError(this->Device);
  if (errnum)
  {
    if (errnum == NDI_BAD_CRC || errnum == NDI_TIMEOUT)   // common errors
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
  const double toolTimestamp = vtkIGSIOAccurateTimer::GetSystemTime(); // unfiltered timestamp
  vtkSmartPointer<vtkMatrix4x4> toolToTrackerTransform = vtkSmartPointer<vtkMatrix4x4>::New();
  for (DataSourceContainerConstIterator it = this->GetToolIteratorBegin(); it != this->GetToolIteratorEnd(); ++it)
  {
    ToolStatus toolFlags = TOOL_OK;
    toolToTrackerTransform->Identity();
    unsigned long toolFrameNumber = defaultToolFrameNumber;
    vtkPlusDataSource* trackerTool = it->second;
    std::string toolSourceId = trackerTool->GetId();
    NdiToolDescriptorsType::iterator ndiToolDescriptorIt = this->NdiToolDescriptors.find(toolSourceId);
    if (ndiToolDescriptorIt == this->NdiToolDescriptors.end())
    {
      LOG_ERROR("Tool descriptor is not found for tool " << toolSourceId);
      this->ToolTimeStampedUpdate(trackerTool->GetId(), toolToTrackerTransform, toolFlags, toolFrameNumber, toolTimestamp);
      continue;
    }
    int portHandle = ndiToolDescriptorIt->second.PortHandle;
    if (portHandle <= 0)
    {
      LOG_ERROR("Port handle is invalid for tool " << toolSourceId);
      this->ToolTimeStampedUpdate(toolSourceId.c_str(), toolToTrackerTransform, toolFlags, toolFrameNumber, toolTimestamp);
      continue;
    }

    float ndiTransform[8] = {1, 0, 0, 0, 0, 0, 0, 0};
    int ndiToolAbsent = ndiGetBXTransform(this->Device, portHandle, ndiTransform);
    int ndiPortStatus = ndiGetBXPortStatus(this->Device, portHandle);
    unsigned long ndiFrameIndex = ndiGetBXFrame(this->Device, portHandle);

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

    ndiTransformToMatrixfd(ndiTransform, *toolToTrackerTransform->Element);
    toolToTrackerTransform->Transpose();

    // by default (if there is no camera frame number associated with
    // the tool transformation) the most recent timestamp is used.
    if (!ndiToolAbsent && ndiFrameIndex)
    {
      // this will create a timestamp from the frame number
      toolFrameNumber = ndiFrameIndex;
      if (ndiFrameIndex > this->LastFrameNumber)
      {
        this->LastFrameNumber = ndiFrameIndex;
      }
    }

    // send the matrix and status to the tool's vtkPlusDataBuffer
    this->ToolTimeStampedUpdate(toolSourceId.c_str(), toolToTrackerTransform, toolFlags, toolFrameNumber, toolTimestamp);
  }

  // Update tool connections if a wired tool is plugged in
  if (ndiGetBXSystemStatus(this->Device) & NDI_PORT_OCCUPIED)
  {
    LOG_WARNING("A wired tool has been plugged into tracker " << (this->GetDeviceId().empty() ? this->GetDeviceId() : "(unknown NDI tracker"));
    // Make the newly connected tools available
    this->EnableToolPorts();
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusNDITracker::ReadSromFromFile(NdiToolDescriptor& toolDescriptor, const char* filename)
{
  FILE* file = fopen(filename, "rb");
  if (file == NULL)
  {
    LOG_ERROR("Couldn't find srom file " << filename);
    return PLUS_FAIL;
  }

  if (toolDescriptor.VirtualSROM == 0)
  {
    toolDescriptor.VirtualSROM = new unsigned char[VIRTUAL_SROM_SIZE];
  }

  memset(toolDescriptor.VirtualSROM, 0, VIRTUAL_SROM_SIZE);
  fread(toolDescriptor.VirtualSROM, 1, VIRTUAL_SROM_SIZE, file);
  fclose(file);
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusNDITracker::EnableToolPorts()
{
  PlusStatus status = PLUS_SUCCESS;

  // stop tracking
  if (this->IsDeviceTracking)
  {
    this->Command("TSTOP:");
    int errnum = ndiGetError(this->Device);
    if (errnum)
    {
      LOG_ERROR(ndiErrorString(errnum));
      status = PLUS_FAIL;
    }
  }

  // free ports that are waiting to be freed
  this->Command("PHSR:01");
  int errnum = ndiGetError(this->Device);
  if (errnum != NDI_OKAY)
  {
    LOG_ERROR("Unable to get the number of handles. Error: " << ndiErrorString(errnum));
    return PLUS_FAIL;
  }

  int ntools = ndiGetPHSRNumberOfHandles(this->Device);
  for (int ndiToolIndex = 0; ndiToolIndex < ntools; ndiToolIndex++)
  {
    int portHandle = ndiGetPHSRHandle(this->Device, ndiToolIndex);
    this->Command("PHF:%02X", portHandle);
    int errnum = ndiGetError(this->Device);
    if (errnum)
    {
      LOG_ERROR(ndiErrorString(errnum));
      status = PLUS_FAIL;
    }
  }

  // Set port handles and send SROM files to tracker
  // We need to do this before initializing and enabling
  // the ports waiting to be initialized.
  for (NdiToolDescriptorsType::iterator toolDescriptorIt = this->NdiToolDescriptors.begin(); toolDescriptorIt != this->NdiToolDescriptors.end(); ++toolDescriptorIt)
  {
    if (toolDescriptorIt->second.VirtualSROM != NULL)   // wireless tool (or wired tool with virtual rom)
    {
      if (this->UpdatePortHandle(toolDescriptorIt->second) != PLUS_SUCCESS)
      {
        LOG_ERROR("Failed to determine NDI port handle for tool " << toolDescriptorIt->first);
        return PLUS_FAIL;
      }
      if (this->SendSromToTracker(toolDescriptorIt->second) != PLUS_SUCCESS)
      {
        LOG_ERROR("Failed send SROM to NDI tool " << toolDescriptorIt->first);
        return PLUS_FAIL;
      }
    }
  }

  // initialize ports waiting to be initialized
  errnum = 0;
  ntools = 0;
  do // repeat as necessary (in case multi-channel tools are used)
  {
    this->Command("PHSR:02");
    ntools = ndiGetPHSRNumberOfHandles(this->Device);
    for (int ndiToolIndex = 0; ndiToolIndex < ntools; ndiToolIndex++)
    {
      int portHandle = ndiGetPHSRHandle(this->Device, ndiToolIndex);
      this->Command("PINIT:%02X", portHandle);
      errnum = ndiGetError(this->Device);
      if (errnum)
      {
        std::stringstream ss;
        ss << ndiErrorString(errnum) << ". errnum: " << errnum;
        LOG_ERROR(ss.str());
        status = PLUS_FAIL;
      }
    }
  }
  while (ntools > 0 && errnum == 0);

  // enable initialized tools
  this->Command("PHSR:03");
  ntools = ndiGetPHSRNumberOfHandles(this->Device);
  for (int ndiToolIndex = 0; ndiToolIndex < ntools; ndiToolIndex++)
  {
    int portHandle = ndiGetPHSRHandle(this->Device, ndiToolIndex);
    this->Command("PHINF:%02X0001", portHandle);
    char identity[34];
    ndiGetPHINFToolInfo(this->Device, identity);
    int mode = 'D'; // default
    if (identity[1] == 0x03)   // button-box
    {
      mode = 'B';
    }
    else if (identity[1] == 0x01)   // reference
    {
      mode = 'S';
    }
    // enable the tool
    this->Command("PENA:%02X%c", portHandle, mode);
    int errnum = ndiGetError(this->Device);
    if (errnum)
    {
      LOG_ERROR(ndiErrorString(errnum));
      status = PLUS_FAIL;
    }
  }

  // Set wired port handles and send SROM files to tracker
  // We need to do this after enabling all the tools because tools on
  // splitters (two 5-DOF tools with one connector) only appear after the tool is enabled.
  for (NdiToolDescriptorsType::iterator toolDescriptorIt = this->NdiToolDescriptors.begin(); toolDescriptorIt != this->NdiToolDescriptors.end(); ++toolDescriptorIt)
  {
    if (toolDescriptorIt->second.WiredPortNumber >= 0 && toolDescriptorIt->second.VirtualSROM == NULL)   //wired tool, no virtual ROM
    {
      if (this->UpdatePortHandle(toolDescriptorIt->second) != PLUS_SUCCESS)
      {
        LOG_ERROR("Failed to determine NDI port handle for tool " << toolDescriptorIt->first);
        return PLUS_FAIL;
      }
      if (this->SendSromToTracker(toolDescriptorIt->second) != PLUS_SUCCESS)
      {
        LOG_ERROR("Failed send SROM to NDI tool " << toolDescriptorIt->first);
        return PLUS_FAIL;
      }
    }
  }

  // Update tool info
  this->Command("PHSR:00");

  for (NdiToolDescriptorsType::iterator toolDescriptorIt = this->NdiToolDescriptors.begin(); toolDescriptorIt != this->NdiToolDescriptors.end(); ++toolDescriptorIt)
  {
    vtkPlusDataSource* trackerTool = NULL;
    if (this->GetTool(toolDescriptorIt->first, trackerTool) != PLUS_SUCCESS)
    {
      LOG_ERROR("Failed to get NDI tool: " << toolDescriptorIt->first);
      status = PLUS_FAIL;
      continue;
    }

    this->Command("PHINF:%02X0025", toolDescriptorIt->second.PortHandle);
    int errnum = ndiGetError(this->Device);
    if (errnum)
    {
      LOG_ERROR(ndiErrorString(errnum));
      status = PLUS_FAIL;
      continue;
    }

    // decompose identity string from end to front
    char identity[34];
    ndiGetPHINFToolInfo(this->Device, identity);
    identity[31] = '\0';
    std::string serialNumber(&identity[23]);
    igsioCommon::Trim(serialNumber);
    trackerTool->SetCustomProperty("SerialNumber", serialNumber);
    identity[23] = '\0';
    std::string toolRevision(&identity[20]);
    igsioCommon::Trim(toolRevision);
    trackerTool->SetCustomProperty("Revision", toolRevision);
    identity[20] = '\0';
    std::string toolManufacturer(&identity[8]);
    igsioCommon::Trim(toolManufacturer);
    trackerTool->SetCustomProperty("Manufacturer", toolManufacturer);
    identity[8] = '\0';
    std::string ndiIdentity(&identity[0]);
    igsioCommon::Trim(ndiIdentity);
    trackerTool->SetCustomProperty("NdiIdentity", ndiIdentity);
    char partNumber[24];
    ndiGetPHINFPartNumber(this->Device, partNumber);
    partNumber[20] = '\0';
    std::string toolPartNumber(&partNumber[0]);
    igsioCommon::Trim(toolPartNumber);
    trackerTool->SetCustomProperty("PartNumber", toolPartNumber);
    int status = ndiGetPHINFPortStatus(this->Device);

    toolDescriptorIt->second.PortEnabled = ((status & NDI_ENABLED) != 0);
    if (!toolDescriptorIt->second.PortEnabled)
    {
      LOG_ERROR("Failed to enable NDI tool " << toolDescriptorIt->first);
      status = PLUS_FAIL;
    }
  }

  // re-start the tracking
  if (this->IsDeviceTracking)
  {
    this->Command("TSTART:");
    int errnum = ndiGetError(this->Device);
    if (errnum)
    {
      LOG_ERROR("Failed TSTART: " << ndiErrorString(errnum));
      status = PLUS_FAIL;
    }
  }

  return status;
}

//----------------------------------------------------------------------------
// Disable all enabled tool ports.
void vtkPlusNDITracker::DisableToolPorts()
{
  // stop tracking
  if (this->IsDeviceTracking)
  {
    this->Command("TSTOP:");
    int errnum = ndiGetError(this->Device);
    if (errnum)
    {
      LOG_ERROR(ndiErrorString(errnum));
    }
  }

  // disable all enabled tools
  this->Command("PHSR:04");
  int ntools = ndiGetPHSRNumberOfHandles(this->Device);
  for (int ndiToolIndex = 0; ndiToolIndex < ntools; ndiToolIndex++)
  {
    int portHandle = ndiGetPHSRHandle(this->Device, ndiToolIndex);
    this->Command("PDIS:%02X", portHandle);
    int errnum = ndiGetError(this->Device);
    if (errnum)
    {
      LOG_ERROR(ndiErrorString(errnum));
    }
  }

  // disable the enabled ports
  for (NdiToolDescriptorsType::iterator toolDescriptorIt = this->NdiToolDescriptors.begin(); toolDescriptorIt != this->NdiToolDescriptors.end(); ++toolDescriptorIt)
  {
    toolDescriptorIt->second.PortEnabled = false;
  }

  // re-start the tracking
  if (this->IsDeviceTracking)
  {
    this->Command("TSTART:");
    int errnum = ndiGetError(this->Device);
    if (errnum)
    {
      LOG_ERROR(ndiErrorString(errnum));
    }
  }
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusNDITracker::Beep(int n)
{
  if (this->Recording)
  {
    LOG_ERROR("vtkPlusNDITracker::Beep failed: not connected to the device");
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
  this->Command("BEEP:%i", n);
  int errnum = ndiGetError(this->Device);

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus  vtkPlusNDITracker::SetToolLED(const char* sourceId, int led, LedState state)
{
  if (!this->Recording)
  {
    LOG_ERROR("vtkPlusNDITracker::InternalSetToolLED failed: not recording");
    return PLUS_FAIL;
  }
  NdiToolDescriptorsType::iterator ndiToolDescriptorIt = this->NdiToolDescriptors.find(sourceId);
  if (ndiToolDescriptorIt == this->NdiToolDescriptors.end())
  {
    LOG_ERROR("InternalSetToolLED failed: Tool descriptor is not found for tool " << sourceId);
    return PLUS_FAIL;
  }
  int portHandle = ndiToolDescriptorIt->second.PortHandle;
  if (portHandle <= 0)
  {
    LOG_ERROR("vtkPlusNDITracker::InternalSetToolLED failed: invalid port handle");
    return PLUS_FAIL;
  }

  int plstate = NDI_BLANK;
  switch (state)
  {
    case TR_LED_OFF:
      plstate = NDI_BLANK;
      break;
    case TR_LED_ON:
      plstate = NDI_SOLID;
      break;
    case TR_LED_FLASH:
      plstate = NDI_FLASH;
      break;
    default:
      LOG_ERROR("vtkPlusNDITracker::InternalSetToolLED failed: unsupported LED state: " << state);
      return PLUS_FAIL;
  }

  this->Command("LED:%02X%d%c", portHandle, led + 1, plstate);
  int errnum = ndiGetError(this->Device);

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusNDITracker::UpdatePortHandle(NdiToolDescriptor& toolDescriptor)
{
  if (toolDescriptor.WiredPortNumber >= 0)   // wired tool
  {
    this->Command("PHSR:00");
    int ntools = ndiGetPHSRNumberOfHandles(this->Device);
    int ndiToolIndex = 0;
    for (; ndiToolIndex < ntools; ndiToolIndex++)
    {
      if (ndiGetPHSRInformation(this->Device, ndiToolIndex) & NDI_TOOL_IN_PORT)
      {
        int portHandle = ndiGetPHSRHandle(this->Device, ndiToolIndex);
        this->Command("PHINF:%02X0021", portHandle);
        char location[14];
        ndiGetPHINFPortLocation(this->Device, location);
        int foundWiredPortNumber = (location[10] - '0') * 10 + (location[11] - '0') - 1;
        int foundWiredPortChannel = (location[12] - '0') * 10 + (location[13] - '0');     // this is nonzero if 5-DOF tools with splitter
        int combinedPortAndChannelNumber = foundWiredPortChannel * 100 + foundWiredPortNumber;
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
      LOG_ERROR("Active NDI tool not found in port " << toolDescriptor.WiredPortNumber << ". Make sure the tool is plugged in.");
      return PLUS_FAIL;
    }
  }
  else // wireless tool
  {
    this->Command("PHRQ:*********1****");
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
PlusStatus vtkPlusNDITracker::SendSromToTracker(const NdiToolDescriptor& toolDescriptor)
{
  if (toolDescriptor.VirtualSROM == NULL)
  {
    // nothing to load
    return PLUS_SUCCESS;
  }

  igsioLockGuard<vtkIGSIORecursiveCriticalSection> lock(this->CommandMutex);
  const int TRANSFER_BLOCK_SIZE = 64; // in bytes
  char hexbuffer[TRANSFER_BLOCK_SIZE * 2];
  for (int i = 0; i < VIRTUAL_SROM_SIZE; i += TRANSFER_BLOCK_SIZE)
  {
    RETRY_UNTIL_TRUE(
      strcmp(this->Command("PVWR:%02X%04X%.128s", toolDescriptor.PortHandle, i,
                           ndiHexEncode(hexbuffer, &(toolDescriptor.VirtualSROM[i]), TRANSFER_BLOCK_SIZE)).c_str(), "OKAY") == 0,
      10, 0.1);

    int errnum = ndiGetError(this->Device);
    if (errnum)
    {
      std::stringstream ss;
      ss << "Failed to send SROM to NDI tracker: " << ndiErrorString(errnum);
      LOG_ERROR(ss.str());
      return PLUS_FAIL;
    }
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusNDITracker::ClearVirtualSromInTracker(NdiToolDescriptor& toolDescriptor)
{
  if (toolDescriptor.VirtualSROM == NULL)
  {
    // nothing to clear
    return PLUS_SUCCESS;
  }

  this->Command("PHF:%02X", toolDescriptor.PortHandle);
  toolDescriptor.PortEnabled = false;
  toolDescriptor.PortHandle = 0;

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusNDITracker::ReadConfiguration(vtkXMLDataElement* rootConfigElement)
{
  // Clean up any previously read config data
  for (NdiToolDescriptorsType::iterator toolDescriptorIt = this->NdiToolDescriptors.begin(); toolDescriptorIt != this->NdiToolDescriptors.end(); ++toolDescriptorIt)
  {
    delete [] toolDescriptorIt->second.VirtualSROM;
    toolDescriptorIt->second.VirtualSROM = NULL;
  }
  this->NdiToolDescriptors.clear();

  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_READING(deviceConfig, rootConfigElement);

  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(unsigned long, SerialPort, deviceConfig);
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(unsigned long, BaudRate, deviceConfig);
  if (deviceConfig->GetAttribute("BaudRate") != NULL && vtkPlusNDITracker::ConvertBaudToNDIEnum(this->BaudRate) == -1)
  {
    LOG_WARNING("Invalid baud rate specified, reverting to auto-select.");
    this->BaudRate = 0;
  }
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(int, MeasurementVolumeNumber, deviceConfig);

  XML_READ_STRING_ATTRIBUTE_OPTIONAL(NetworkHostname, deviceConfig);
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(int, NetworkPort, deviceConfig);
  XML_READ_BOOL_ATTRIBUTE_OPTIONAL(CheckDSR, deviceConfig);

  XML_FIND_NESTED_ELEMENT_REQUIRED(dataSourcesElement, deviceConfig, "DataSources");

  for (int nestedElementIndex = 0; nestedElementIndex < dataSourcesElement->GetNumberOfNestedElements(); nestedElementIndex++)
  {
    vtkXMLDataElement* toolDataElement = dataSourcesElement->GetNestedElement(nestedElementIndex);
    if (STRCASECMP(toolDataElement->GetName(), "DataSource") != 0)
    {
      // if this is not a data source element, skip it
      continue;
    }
    bool isEqual(false);
    if (igsioCommon::XML::SafeCheckAttributeValueInsensitive(*toolDataElement, "Type", vtkPlusDataSource::DATA_SOURCE_TYPE_TOOL_TAG, isEqual) != PLUS_SUCCESS || !isEqual)
    {
      // if this is not a Tool element, skip it
      continue;
    }
    const char* toolId = toolDataElement->GetAttribute("Id");
    if (toolId == NULL)
    {
      LOG_ERROR("Failed to initialize NDI tool: DataSource Id is missing");
      continue;
    }
    igsioTransformName toolTransformName(toolId, this->GetToolReferenceFrameName());
    std::string toolSourceId = toolTransformName.GetTransformName();
    vtkPlusDataSource* trackerTool = NULL;
    if (this->GetTool(toolSourceId, trackerTool) != PLUS_SUCCESS || trackerTool == NULL)
    {
      LOG_ERROR("Failed to get NDI tool: " << toolSourceId);
      continue;
    }

    int wiredPortNumber = -1;
    if (toolDataElement->GetAttribute("PortName") != NULL)
    {
      if (!toolDataElement->GetScalarAttribute("PortName", wiredPortNumber))
      {
        LOG_WARNING("NDI wired tool's PortName attribute has to be an integer >=0");
        continue;
      }
    }

    NdiToolDescriptor toolDescriptor;
    toolDescriptor.PortEnabled = false;
    toolDescriptor.PortHandle = 0;
    toolDescriptor.VirtualSROM = NULL;
    toolDescriptor.WiredPortNumber = wiredPortNumber;

    const char* romFileName = toolDataElement->GetAttribute("RomFile");
    if (romFileName)
    {
      // Passive (wireless) tool or wired tool with virtual ROM
      if (wiredPortNumber >= 0)
      {
        LOG_WARNING("NDI PortName and RomFile are both specified for tool " << toolSourceId << ". Assuming broken wired rom, using virtual rom instead");
      }
      std::string romFilePath = vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationPath(romFileName);
      this->ReadSromFromFile(toolDescriptor, romFilePath.c_str());
    }

    this->NdiToolDescriptors[toolSourceId] = toolDescriptor;
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusNDITracker::WriteConfiguration(vtkXMLDataElement* rootConfig)
{
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_WRITING(trackerConfig, rootConfig);
  if (this->SerialPort > 0)
  {
    trackerConfig->SetIntAttribute("SerialPort", this->SerialPort);
  }

  XML_WRITE_STRING_ATTRIBUTE_IF_NOT_EMPTY(NetworkHostname, trackerConfig);
  if (!this->NetworkHostname.empty())
  {
    trackerConfig->SetIntAttribute("NetworkPort", this->NetworkPort);
  }

  if (this->BaudRate > 0)
  {
    trackerConfig->SetIntAttribute("BaudRate", this->BaudRate);
  }
  if (this->MeasurementVolumeNumber > 0)
  {
    trackerConfig->SetIntAttribute("MeasurementVolumeNumber", this->MeasurementVolumeNumber);
  }
  trackerConfig->SetAttribute("CheckDSR", this->CheckDSR ? "true" : "false");

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
void vtkPlusNDITracker::LogVolumeList(int selectedVolume, vtkIGSIOLogger::LogLevelType logLevel)
{
  auto reply = this->Command("GETINFO:Param.Tracking.Available Volumes");

  unsigned int numVolumes(0);
  int errnum = ndiGetError(this->Device);
  if (errnum)
  {
    const unsigned char MODE_GET_VOLUMES_LIST = 0x03; // list of volumes available
    auto volumeListCommandReply = this->Command("SFLIST:%02X", MODE_GET_VOLUMES_LIST);
    numVolumes = ndiHexToUnsignedInt(volumeListCommandReply.c_str(), 1);
  }
  else
  {
    auto tokens = igsioCommon::SplitStringIntoTokens(reply, '=', false);
    auto dataFields = igsioCommon::SplitStringIntoTokens(tokens[1], ';', true);
    numVolumes = dataFields.size() / 7;
  }

  if (selectedVolume == 0)
  {
    LOG_DYNAMIC("Number of available measurement volumes: " << numVolumes, logLevel);
  }

  reply = this->Command("GETINFO:Features.Volumes.*");
  if (reply.find("ERROR") != std::string::npos)
  {
    // Most likely error 34 "Parameter not found", revert to SFLIST method
    LogVolumeListSFLIST(numVolumes, selectedVolume, logLevel);
    return;
  }

  auto lines = igsioCommon::SplitStringIntoTokens(reply, '\n', false);

  for (auto iter = begin(lines); iter != end(lines); ++iter)
  {
    auto tokens = igsioCommon::SplitStringIntoTokens(*iter, '=', true);
    auto dataFields = igsioCommon::SplitStringIntoTokens(tokens[1], ';', true);
    if (selectedVolume > 0 &&
        tokens[0].find("Features.Volumes.Index") != std::string::npos &&
        dataFields[0][0] - '0' == selectedVolume - 1) // NDI index by 0, Plus index by 1
    {
      LOG_DYNAMIC("Measurement volume " << selectedVolume, logLevel);

      // Selected index, next 13 lines are details about the selected volume
      // Features.Volumes.Name        The volume name                                   Read
      // Features.Volumes.Shape       The shape type                                    Read
      // Features.Volumes.Wavelengths Which wavelengths are supported in the volume     Read
      // Features.Volumes.Paramn      Shape parameters as described in SFLIST           Read
      for (int i = 0; i < 13; ++i)
      {
        auto tokens = igsioCommon::SplitStringIntoTokens(*(iter + i), '=', true);
        auto names = igsioCommon::SplitStringIntoTokens(tokens[0], '.', false);
        auto dataFields = igsioCommon::SplitStringIntoTokens(tokens[1], ';', true);
        LOG_DYNAMIC(" " << names[2] << ": " << dataFields[0], logLevel);
      }

      return;
    }

    auto names = igsioCommon::SplitStringIntoTokens(tokens[0], '.', false);
    LOG_DYNAMIC(names[2] << ": " << dataFields[0], logLevel);
  }
}

//----------------------------------------------------------------------------
void vtkPlusNDITracker::LogVolumeListSFLIST(unsigned int numVolumes, int selectedVolume, vtkIGSIOLogger::LogLevelType logLevel)
{
  std::string volumeListCommandReply = this->Command("SFLIST:03");
  for (unsigned int volIndex = 0; volIndex < numVolumes; ++volIndex)
  {
    const char* volDescriptor = volumeListCommandReply.c_str() + 1 + volIndex * 74;

    LOG_DYNAMIC("Measurement volume " << volIndex + 1, logLevel);

    std::string shapeType;
    bool isOptical(false);
    switch (volDescriptor[0])
    {
      case '9':
        shapeType = "Cube";
        break;
      case 'A':
        shapeType = "Dome";
        break;
      case '5':
        shapeType = "Polaris (Pyramid or extended pyramid)";
        isOptical = true;
        break;
      case '7':
        shapeType = "Vicra (Pyramid)";
        isOptical = true;
        break;
      default:
        shapeType = "unknown";
    }
    LOG_DYNAMIC(" Shape type: " << shapeType << " (" << volDescriptor[0] << ")", logLevel);

    LOG_DYNAMIC(" D1  = " << ndiSignedToLong(volDescriptor + 1, 7) / 100, logLevel);
    LOG_DYNAMIC(" D2  = " << ndiSignedToLong(volDescriptor + 8, 7) / 100, logLevel);
    LOG_DYNAMIC(" D3  = " << ndiSignedToLong(volDescriptor + 15, 7) / 100, logLevel);
    LOG_DYNAMIC(" D4  = " << ndiSignedToLong(volDescriptor + 22, 7) / 100, logLevel);
    LOG_DYNAMIC(" D5  = " << ndiSignedToLong(volDescriptor + 29, 7) / 100, logLevel);
    LOG_DYNAMIC(" D6  = " << ndiSignedToLong(volDescriptor + 36, 7) / 100, logLevel);
    LOG_DYNAMIC(" D7  = " << ndiSignedToLong(volDescriptor + 43, 7) / 100, logLevel);
    LOG_DYNAMIC(" D8  = " << ndiSignedToLong(volDescriptor + 50, 7) / 100, logLevel);
    LOG_DYNAMIC(" D9  = " << ndiSignedToLong(volDescriptor + 57, 7) / 100, logLevel);
    LOG_DYNAMIC(" D10 = " << ndiSignedToLong(volDescriptor + 64, 7) / 100, logLevel);

    if (!isOptical)
    {
      LOG_DYNAMIC(" Reserved: " << volDescriptor[71], logLevel);

      std::string metalResistant;
      switch (volDescriptor[72])
      {
        case '0':
          metalResistant = "no information";
          break;
        case '1':
          metalResistant = "metal resistant";
          break;
        case '2':
          metalResistant = "not metal resistant";
          break;
        default:
          metalResistant = "unknown";
      }
      LOG_DYNAMIC(" Metal resistant: " << metalResistant << " (" << volDescriptor[72] << ")", logLevel);
    }
    else
    {
      unsigned int numWavelengths = volDescriptor[71] - '0';
      LOG_DYNAMIC(" Number of wavelengths supported: " << numWavelengths, logLevel);
      for (unsigned int i = 0; i < numWavelengths; ++i)
      {
        switch (volDescriptor[72 + i])
        {
          case '0':
            LOG_DYNAMIC("  Wavelength " << i << ": 930nm", logLevel);
            break;
          case '1':
            LOG_DYNAMIC("  Wavelength " << i << ": 880nm", logLevel);
            break;
          case '4':
            LOG_DYNAMIC("  Wavelength " << i << ": 870nm", logLevel);
            break;
          default:
            LOG_DYNAMIC("  Wavelength " << i << ": unknown (" << volDescriptor[72 + i] << ")", logLevel);
            break;
        }
      }
    }
  }
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusNDITracker::CloseDevice(ndicapi*& device)
{
  if (this->NetworkHostname.empty())
  {
    ndiCloseSerial(device);
  }
  else
  {
    ndiCloseNetwork(device);
  }
  device = nullptr;

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusNDITracker::SelectMeasurementVolume()
{
  if (this->MeasurementVolumeNumber > 0)
  {
    std::string reply = this->Command("GETINFO:Param.Tracking.Available Volumes");
    int errnum = ndiGetError(this->Device);
    if (errnum)
    {
      return SelectMeasurementVolumeDeprecated();
    }
    else
    {
      auto tokens = igsioCommon::SplitStringIntoTokens(reply, '=', false);
      auto dataFields = igsioCommon::SplitStringIntoTokens(tokens[1], ';', true);

      // <Value>;<Type>;<Attribute>;<Minimum>;<Maximum>;<Enumeration>;<Description>
      if (static_cast<unsigned int>(this->MeasurementVolumeNumber) - 1 > dataFields.size() / 7)
      {
        LOG_ERROR("Selected measurement volume does not exist. Using default.");
        LogVolumeList(this->MeasurementVolumeNumber, vtkIGSIOLogger::LOG_LEVEL_DEBUG);
        return PLUS_FAIL;
      }
      else
      {
        // Use SET command with parameter
        std::string volumeSelectCommandReply = this->Command("SET:Param.Tracking.Selected Volume=%d", this->MeasurementVolumeNumber);
        int errnum = ndiGetError(this->Device);
        if (errnum)
        {
          LOG_ERROR("Failed to set measurement volume " << this->MeasurementVolumeNumber << ": " << ndiErrorString(errnum));
          LogVolumeList(0, vtkIGSIOLogger::LOG_LEVEL_INFO);
          CloseDevice(this->Device);
          return PLUS_FAIL;
        }
      }
    }
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusNDITracker::SelectMeasurementVolumeDeprecated()
{
  auto volumeSelectCommandReply = this->Command("VSEL:%d", this->MeasurementVolumeNumber);
  int errnum = ndiGetError(this->Device);
  if (errnum)
  {
    LOG_ERROR("Failed to set measurement volume " << this->MeasurementVolumeNumber << ": " << ndiErrorString(errnum));
    LogVolumeList(this->MeasurementVolumeNumber, vtkIGSIOLogger::LOG_LEVEL_DEBUG);
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
int vtkPlusNDITracker::ConvertBaudToNDIEnum(int baudRate)
{
  switch (baudRate)
  {
    case 9600:
      return NDI_9600;
    case 14400:
      return NDI_14400;
    case 19200:
      return NDI_19200;
    case 38400:
      return NDI_38400;
    case 57600:
      return NDI_57600;
    case 115200:
      return NDI_115200;
    case 230400:
      return NDI_230400;
    case 921600:
      return NDI_921600;
    case 1228739:
      return NDI_1228739;
    default:
      return -1;
  }
}

#if defined(HAVE_FUTURE)
//----------------------------------------------------------------------------
PlusStatus vtkPlusNDITracker::ProbeSerialInternal()
{
  const int MAX_SERIAL_PORT_NUMBER = 20; // the serial port is almost surely less than this number
  std::vector<bool> deviceExists(MAX_SERIAL_PORT_NUMBER);
  std::fill(begin(deviceExists), end(deviceExists), false);
  std::vector<std::future<void>> tasks;
  for (int i = 0; i < MAX_SERIAL_PORT_NUMBER; i++)
  {
    const char* dev = ndiSerialDeviceName(i);
    LOG_DEBUG("Testing serial port: " << dev);
    if (dev != nullptr)
    {
      std::string devName = std::string(dev);
      std::future<void> result = std::async([this, i, &deviceExists, devName]()
      {
        int errnum = ndiSerialProbe(devName.c_str(), this->CheckDSR);
        LOG_DEBUG("Serial port " << devName << " probe error (" << errnum << "): " << ndiErrorString(errnum));
        if (errnum == NDI_OKAY)
        {
          deviceExists[i] = true;
        }
      });
      tasks.push_back(std::move(result));
    }
  }
  for (int i = 0; i < MAX_SERIAL_PORT_NUMBER; i++)
  {
    tasks[i].wait();
  }
  for (int i = 0; i < MAX_SERIAL_PORT_NUMBER; i++)
  {
    // use first device found
    if (deviceExists[i] == true)
    {
      const char* devicename = ndiSerialDeviceName(i);
      this->SerialPort = i + 1;
      if (this->LeaveDeviceOpenAfterProbe)
      {
        this->Device = ndiOpenSerial(devicename);
      }
      return PLUS_SUCCESS;
    }
  }

  return PLUS_FAIL;
}
#endif