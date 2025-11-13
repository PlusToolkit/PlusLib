# Microchip MM7150 magnetic, angular rate, and gravity (MARG) sensor

## Supported hardware devices

- Link to manufacturer's website: <http://www.microchip.com/wwwProducts/Devices.aspx?product=MM7150>

## Supported platforms

- [***Windows 32 bits***](../SupportedPlatforms.md#windows-32-bits)
- [***Windows 64 bits***](../SupportedPlatforms.md#windows-64-bits)
- [***Windows XP embedded 32 bits***](../SupportedPlatforms.md#windows-xp-embedded-32-bits)

## Installation

- Switch the sensor to orientation mode
- Make sure the serial port number matches the COM port number the device is connected to

## Device configuration settings

- **Type**: `Microchip`
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

## Example configuration file PlusDeviceSet_Server_Microchip.xml

```xml
<PlusConfiguration version="2.1">

  <DataCollection StartupDelaySec="1.0">
    <DeviceSet
      Name="PlusServer: Microchip motion module"
      Description="Collecting orientation data from Microchip MM7150 motion module and broadcasting through OpenIGTLink"
    />
    <Device
      Id="TrackerDevice"
      Type="Microchip"
      SerialPort="3"
      BaudRate="19200"
      ToolReferenceFrame="Tracker" >
      <DataSources>
        <DataSource Type="Tool" Id="OrientationSensor" PortName="OrientationSensor" />
      </DataSources>
      <OutputChannels>
        <OutputChannel Id="TrackerStream">
          <DataSource Id="OrientationSensor"/>
        </OutputChannel>
      </OutputChannels>
    </Device>
    <Device
      Id="CaptureDevice"
      Type="VirtualCapture"
      BaseFilename="RecordingTest.igs.mha"
      EnableCapturingOnStart="FALSE" >
      <InputChannels>
        <InputChannel Id="TrackerStream" />
      </InputChannels>
    </Device>
  </DataCollection>

  <PlusOpenIGTLinkServer
    MaxNumberOfIgtlMessagesToSend="1"
    MaxTimeSpentWithProcessingMs="50"
    ListeningPort="18944"
    OutputChannelId="TrackerStream"
    >
    <DefaultClientInfo>
      <MessageTypes>
        <Message Type="TRANSFORM" />
      </MessageTypes>
      <TransformNames>
        <Transform Name="OrientationSensorToTracker" />
      </TransformNames>
    </DefaultClientInfo>
  </PlusOpenIGTLinkServer>

</PlusConfiguration>
```