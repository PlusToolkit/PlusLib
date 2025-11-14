# Generic serial device

This device communicates with external hardware using serial (RS-232) connection
It does not collect tracking or image data, only allows sending and receiving data
using OpenIGTLink commands (see [`PlusServerCommands`](/PlusServerCommands)).

A (customizable) newline string is appended after each sent command and the device listens
for reponse that is terminated by the same newline string.

## Supported hardware devices

- Any device communicating through serial (RS-232) interface.

## Supported platforms

- [***Windows 32 bits***](/SupportedPlatforms/#windows-32-bits)
- [***Windows 64 bits***](/SupportedPlatforms/#windows-64-bits)
- [***Windows XP embedded 32 bits***](/SupportedPlatforms/#windows-xp-embedded-32-bits)

## Installation

- Connect the device to the serial port. Make sure communication speed matches the setting in the configuration file.
- If a USB to serial converter is used then the port number can be obtained (and modified, if needed) in the Device Manager.

## Device configuration settings

  - **Type**: `GenericSerial`
  - **AcquisitionRate**: Defines how frequently Plus should read data sent by the serial device (Optional, default: `10`)
  - **SerialPort**: Used COM port number for serial communication (ComPort: 1 => Port name: `COM1`). (Required)
  - **BaudRate**: Baud rate for serial communication. (Optional, default: `9600`)
  - **MaximumReplyDelaySec**: Maximum time to wait for the device to start replying. (Optional, default: `0.100`)
  - **MaximumReplyDurationSec**: Maximum time to wait for the device to finish replying.(Optional, default: `0.300`)
  - **LineEnding**: Line ending character(s). Used when sending and receiving text to the device. Each character encoded as 2-digit hexadecimal, separated by spaces. For example: CR line ending is "0d", CR/LF line ending is "0d 0a" (Optional, default: `0d`)
  - **DataSources**: No `DataSource` should be defined

## Example configuration file PlusDeviceSet_Server_GenericSerial.xml

```xml
<PlusConfiguration version="2.1">

  <DataCollection StartupDelaySec="1.0">
    <DeviceSet
      Name="PlusServer: Generic serial device"
      Description="Send text and receive response through OpenIGTLink"
    />
    <Device
      Id="SerialDevice"
      Type="GenericSerialDevice"
      SerialPort="5"
      BaudRate="9600"
      LineEnding="0d"
      >
      <DataSources>
        <DataSource Id="SerialData" Type="FieldData"/>
      </DataSources>
      <OutputChannels>
        <OutputChannel Id="FieldChannel">
          <DataSource Id="SerialData"/>
        </OutputChannel>
      </OutputChannels>
    </Device>
  </DataCollection>

  <PlusOpenIGTLinkServer
    MaxNumberOfIgtlMessagesToSend="1"
    MaxTimeSpentWithProcessingMs="50"
    ListeningPort="18944"
    LogWarningOnNoDataAvailable="false"
    OutputChannelId="FieldChannel" >
    <DefaultClientInfo>
      <MessageTypes>
        <Message Type="STRING" />
      </MessageTypes>
      <StringNames>
        <String Name="SerialData" />
      </StringNames>
    </DefaultClientInfo>
  </PlusOpenIGTLinkServer>

</PlusConfiguration>
```

## Example script for sending command from 3D Slicer

```c++
connectorNode = slicer.vtkMRMLIGTLConnectorNode()
connectorNode.SetTypeClient('127.0.0.1', 18944)
slicer.mrmlScene.AddNode(connectorNode)
connectorNode.Start()

arduinoCmd = slicer.vtkSlicerOpenIGTLinkCommand()
arduinoCmd.SetCommandName('SendText')
arduinoCmd.SetCommandAttribute('DeviceId','SerialDevice')
arduinoCmd.SetCommandTimeoutSec(1.0)

arduinoCmd.SetCommandAttribute('Text', "123456")
slicer.modules.openigtlinkremote.logic().SendCommand(arduinoCmd, connectorNode.GetID())

def onArduinoCmdCompleted(observer, eventid):
  print("Command completed with status: " + arduinoCmd.StatusToString(arduinoCmd.GetStatus()))
  print("Response message: " + arduinoCmd.GetResponseMessage())
  print("Full response: " + arduinoCmd.GetResponseText())

arduinoCmd.AddObserver(arduinoCmd.CommandCompletedEvent, onArduinoCmdCompleted)
```
