# CHRobotics CHR-UM6 magnetic, angular rate, and gravity (MARG) sensor

## Supported hardware devices

- Link to manufacturer's website: <http://www.chrobotics.com/shop/orientation-sensor-um6>

## Supported platforms

- [***Windows 32 bits***](/SupportedPlatforms/#windows-32-bits)
- [***Windows 64 bits***](/SupportedPlatforms/#windows-64-bits)
- [***Windows XP embedded 32 bits***](/SupportedPlatforms/#windows-xp-embedded-32-bits)

## Installation

- Contains accelerometer, gyroscope, and magnetometer sensors, each with 3 DOF.
- Small (few centimeters by few centimeters), low-cost (about $200) device.
- Sensor fusion algorithm is integrated into the device.
- Need to enable the following options in the firmware: (`Broadcast`) and "Euler Output".
- Gyroscope bias correction may (depending on settings in the Firmware) be performed automatically when powering up the device. Therefore after the powering the device it shall be kept stationary for a few seconds.

## Device configuration settings

  - **Type**: `ChRobotics`
  - **AcquisitionRate**: (Optional, default: `20`)
  - **LocalTimeOffsetSec**: (Optional, default: `0`)
  - **ToolReferenceFrame**: (Optional, default: `Tracker`)
  - **SerialPort**: Used COM port number for serial communication
  - **BaudRate**: Baud rate for serial communication.
  - **FirmwareDirectory**: Directory path containing the XML files that describes the device firmware
  - **DataSources**: Exactly one `DataSource` child element is required. (Required)
     - **DataSource**: (Required)
      - **PortName**: = `(`OrientationSensor`)` (Required)
      - **BufferSize**: (Optional, default: 150)
      - **AveragedItemsForFiltering**: (Optional, default: 20)

## Example configuration file PlusDeviceSet_Server_ChRobotics.xml

```xml
<PlusConfiguration version="2.1">

  <DataCollection StartupDelaySec="1.0">
    <DeviceSet
      Name="PlusServer: CHRobotics CH3-UM6 MARG sensor"
      Description="Broadcasting tool tracking data through OpenIGTLink"
    />
    <Device
      Id="TrackerDevice"
      Type="ChRobotics"
      SerialPort="5"
      BaudRate="115200"
      FirmwareDirectory="ChRoboticsFirmware"
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
        <xTransform Name="AccelerometerToTracker" />
        <xTransform Name="GyroscopeToTracker" />
        <xTransform Name="MagnetometerToTracker" />
      </TransformNames>
    </DefaultClientInfo>
  </PlusOpenIGTLinkServer>

</PlusConfiguration>
```