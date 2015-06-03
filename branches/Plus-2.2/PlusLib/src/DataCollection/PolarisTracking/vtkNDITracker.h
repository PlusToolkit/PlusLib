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

#ifndef __vtkNDITracker_h
#define __vtkNDITracker_h

#include "vtkDataCollectionExport.h"

#include "vtkPlusDevice.h"

class vtkSocketCommunicator;
struct ndicapi;

// the number of tools the polaris can handle
#define VTK_NDI_REPLY_LEN 2048

/*!
  \class vtkNDITracker
  \brief Interface class for Northern Digital's tracking devices

  The vtkNDITracker class provides an  interface to the AURORA and POLARIS
  (Northern Digital Inc., Waterloo, Canada) using the new "combined API" and
  should also support all newer NDI tracking devices.  Any POLARIS systems
  purchased before 2002 will not support the combined API.

  For active (wired) tools specify PortName attribute. For example,
  PortName="0" is the first port, PortName="1" is the second, etc.

  If multi-channel tools are used then the PortName is <ChannelNumber><PortNumber>,
  for example for two 5-DOF sensors plugged into the first connector:
  PortName="0" and PortName="100", for two 5-DOF sensors plugged into the second connector:
  PortName="1" and PortName="101".

  For passive (wireless) tools specify RomFile attribute. For example,
  RomFile="NdiToolDefinitions/8700339.rom".

  Important notes on the data collection rate of the Polaris:

  The camera frame rate is 60Hz, and therefore the maximum data
  collection rate is also 60Hz.  The maximum data transfer rate
  to the computer is also 60Hz.

  Depending on the number of enabled tools, the data collection
  rate might be reduced.  Each of the active tools requires one
  camera frame, and all the passive tools (if any are enabled)
  collectively require one camera frame.

  Therefore if there are two enabled active tools, the data rate
  is reduced to 30Hz.  Ditto for an active tool and a passive tool.
  If all tools are passive, the data rate is 60Hz.  With 3 active
  tools and one or more passive tools, the data rate is 15Hz.
  With 3 active tools, or 2 active and one or more passive tools,
  the data rate is 20Hz.

  The data transfer rate to the computer is independent of the data
  collection rate, and there might be duplicated records.  The
  data tranfer rate is limited by the speed of the serial port
  and by the number of characters sent per data record.  If tools
  are marked as 'missing' then the number of characters that
  are sent will be reduced.

  \ingroup PlusLibDataCollection
*/
class vtkDataCollectionExport vtkNDITracker : public vtkPlusDevice
{
public:

  static vtkNDITracker *New();
  vtkTypeMacro(vtkNDITracker,vtkPlusDevice);
  void PrintSelf(ostream& os, vtkIndent indent);
  
  /*! Flags for tool LEDs */
  enum LedState
  {
    TR_LED_OFF   = 0,
    TR_LED_ON    = 1,
    TR_LED_FLASH = 2
  };
 
  virtual bool IsTracker() const { return true; }

  /*! Hardware device SDK version. */
  virtual std::string GetSdkVersion();

  /*!
    Probe to see if the tracking system is present on the
    specified serial port.  If the SerialPort is set to -1,
    then all serial ports will be checked.
  */
  PlusStatus Probe();

  /*!
    Send a command to the NDI in the format INIT: or VER:0 (the
    command should include a colon).  Commands can only be done after
    either Probe() or StartTracking() has been called.
    The text reply from the NDI is returned, without the CRC or
    final carriage return.
  */
  char *Command(const char *command);

  /*!
    Get the a string (perhaps a long one) describing the type and version
    of the device.
  */
  vtkGetStringMacro(Version);

  /*! Set which serial port to use, 1 through 4 */
  vtkSetMacro(SerialPort, int);
  vtkGetMacro(SerialPort, int);

  /*! Set the desired baud rate.  Default: 9600. */
  vtkSetMacro(BaudRate, int);
  vtkGetMacro(BaudRate, int);

  /*!
    Measurement volume number. It can be used for defining volume type (dome, cube) and size.
    Default is 0, which means that the default volume is used.
    First valid volume number is 1.
    If an invalid value is set (for example -1) then a list of available volumes is logged.
    See VSEL command in the NDI API documentation for details.
  */
  vtkSetMacro(MeasurementVolumeNumber, int);
  vtkGetMacro(MeasurementVolumeNumber, int);  

  /*!
    Get an update from the tracking system and push the new transforms
    to the tools.  This should only be used within vtkTracker.cxx.
  */
  PlusStatus InternalUpdate();
  
  /*! Read NDI tracker configuration from xml data */
  virtual PlusStatus ReadConfiguration(vtkXMLDataElement* config); 

  /*! Read NDI tracker configuration from xml data */
  virtual PlusStatus WriteConfiguration(vtkXMLDataElement* config); 
  
  /*! Set the specified tool LED to the specified state */
  PlusStatus SetToolLED(const char* portName, int led, LedState state);

protected:
  vtkNDITracker();
  ~vtkNDITracker();

  struct NdiToolDescriptor
  {
    int WiredPortNumber; // >=0 for wired tools
    unsigned char *VirtualSROM; // nonzero for wireless tools
    bool PortEnabled; // true if the tool is successfully enabled in the tracker
    int PortHandle; // this number identifies the tool in the tracker
  };

  /*! Set the version information */
  vtkSetStringMacro(Version);

  /*! Connect to the tracker hardware */
  PlusStatus InternalConnect();
  /*! Disconnect from the tracker hardware */
  PlusStatus InternalDisconnect();

  /*!
    Start the tracking system.  The tracking system is brought from
    its ground state into full tracking mode.  The device will
    only be reset if communication cannot be established without
    a reset.
  */
  PlusStatus InternalStartRecording();

  /*!
    Stop the tracking system and bring it back to its ground state:
    Initialized, not tracking, at 9600 Baud.
  */
  PlusStatus InternalStopRecording();

  /*! Cause the device to beep the specified number of times */
  PlusStatus Beep(int n);

  /*! Read a virtual SROM from file and store it in the tool descriptor */
  PlusStatus ReadSromFromFile(NdiToolDescriptor& toolDescriptor, const char *filename);

  /*!
    Sets the port handle in the descriptor. For wired tools it
    iterates through the existing connections, for wireless tools
    it requests a new handle.
  */
  PlusStatus UpdatePortHandle(NdiToolDescriptor& toolDescriptor);

  /*!
    This is a low-level method for loading a virtual SROM.
    You must halt the tracking thread and take the device
    out of tracking mode before you use it.
    This call also sets the port handle in the descriptor.
  */
  PlusStatus SendSromToTracker(const NdiToolDescriptor& toolDescriptor);
  /*!
    This is a low-level method for loading a virtual SROM.
    You must halt the tracking thread and take the device
    out of tracking mode before you use it.
  */
  PlusStatus ClearVirtualSromInTracker(NdiToolDescriptor& toolDescriptor);

  /*!
    Methods for detecting which ports have tools in them, and
    auto-enabling those tools.
  */
  PlusStatus EnableToolPorts();
  /*!
    Methods for detecting which ports have tools in them, and
    auto-enabling those tools.
  */
  void DisableToolPorts();

  /*! Parse and log available volume list response */
  void LogVolumeList(const char* ndiVolumeListCommandReply, int selectedVolume, vtkPlusLogger::LogLevelType logLevel);

  /*! Index of the last frame number. This is used for providing a frame number when the tracker doesn't return any transform */
  unsigned long LastFrameNumber;

  ndicapi *Device;
  char *Version;
  char *SerialDevice;

  int SerialPort; 
  int BaudRate;
  int IsDeviceTracking;

  int MeasurementVolumeNumber;

  typedef std::map<std::string, NdiToolDescriptor> NdiToolDescriptorsType;
  /*! Maps Plus tool source IDs to NDI tool descriptors */
  NdiToolDescriptorsType NdiToolDescriptors;

  char CommandReply[VTK_NDI_REPLY_LEN];

private:
  vtkNDITracker(const vtkNDITracker&);
  void operator=(const vtkNDITracker&);  
};

#endif
