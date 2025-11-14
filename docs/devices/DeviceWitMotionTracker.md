# WitMotion BWT901CL: Nine axis Bluetooth attitude angle sensor

## Supported hardware devices

- Link to manufacturer's website: <http://www.wit-motion.com/english.php?m=text&a=index&classify_id=46395>

## Supported platforms

- [***Windows 32 bits***](/SupportedPlatforms/#windows-32-bits)
- [***Windows 64 bits***](/SupportedPlatforms/#windows-64-bits)

## BlueTooth Installation

- Other WitMotion devices may be used with this configuration but this section is
meant for setting up the BlueTooth version on Windows 10
  - Make sure Bluetooth is enabled (Settings-Devices-Bluetooth On)
  - Click Add Bluetooth or other device
  - Click Bluetooth, make sure accelerometer 1 is turned on and flashing blue
  - Enter PIN to pair (1234)
  - Should be labelled as HC-06 (Or Unknown Device)
  - In Bluetooth settings, click on more Bluetooth options
  - Click on COM Ports and take note of the outgoing COM Port number
  - Use this COM Port number for Serial Port number in the config file

## Device configuration settings

- **Type**: `WitMotionTracker` (Required)
- **AcquisitionRate**: (Optional, default: `20`)
- **LocalTimeOffsetSec**: (Optional, default: `0`)
- **ToolReferenceFrame**: (Optional, default: `Tracker`)
- **SerialPort**: Used COM port number for serial communication
- **BaudRate**: Baud rate for serial communication.
- **DataSources**: Exactly one `DataSource` child element is required. (Required)
    - **DataSource**: (Required)
    - **PortName**: = `OrientationSensor` (Required)
    - **BufferSize**: (Optional, default: `150`)
    - **AveragedItemsForFiltering**: (Optional, default: `20`)

## Example configuration file PlusDeviceSet_Server_WitMotionTracker.xml

```xml
<PlusConfiguration version="2.1">
  <DataCollection StartupDelaySec="1.0">
    <DeviceSet
      Name="PlusServer: WitMotion motion module"
      Description="Collecting orientation data from a WitMotion JY901 motion module and broadcasting through OpenIGTLink"
    />
    <Device
      Id="TrackerDevice"
      Type="WitMotionTracker"
      SerialPort="9"
      BaudRate="115200"
      ToolReferenceFrame="Tracker" >
      <DataSources>
        <DataSource Type="Tool" Id="Accel" PortName="OrientationSensor" />
      </DataSources>
      <OutputChannels>
        <OutputChannel Id="TrackerChannel">
          <DataSource Id="Accel"/>
        </OutputChannel>
      </OutputChannels>
    </Device>
    <Device
      Id="CaptureDevice"
      Type="VirtualCapture"
      BaseFilename="RecordingTest.igs.mha"
      EnableCapturingOnStart="FALSE" >
      <InputChannels>
        <InputChannel Id="TrackerChannel" />
      </InputChannels>
    </Device>
  </DataCollection>

  <PlusOpenIGTLinkServer
    MaxNumberOfIgtlMessagesToSend="1"
    MaxTimeSpentWithProcessingMs="50"
    ListeningPort="18944"
    OutputChannelId="TrackerChannel"
    >
    <DefaultClientInfo>
      <MessageTypes>
        <Message Type="TRANSFORM" />
      </MessageTypes>
      <TransformNames>
        <Transform Name="AccelToTracker" />
      </TransformNames>
    </DefaultClientInfo>
  </PlusOpenIGTLinkServer>
</PlusConfiguration>
```