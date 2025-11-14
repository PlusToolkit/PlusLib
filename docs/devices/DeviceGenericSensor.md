# Generic Sensor tracker

This device allows you to collect sensor data from sensors with drivers compliant with the Windows Sensor and Location platform.

## Supported sensors

For now, this device can handle the following type of sensors:

- accelerometer
- gyrometer

The sensor drivers must be compliant with the Windows Sensor and Location platform.

- Link to sensor API documentation: <https://docs.microsoft.com/en-us/windows/win32/sensorsapi/introduction-to-the-sensor-and-location-platform-in-windows>

The following sensors have been tested:

- STMicroelectronics 3-axis Accelerometer

## Supported platforms

- [***Windows 32 bits***](/SupportedPlatforms/#windows-32-bits)
- [***Windows 64 bits***](/SupportedPlatforms/#windows-64-bits)

Windows version: >= 7

## Installation

Install the sensor drivers if not installed by default on the target platform.

## Device configuration settings

  - **Type**: `GenericSensor`
  - **UseReportedTimestamp**: Choose whether to use timestamp reported by the sensor or timestamp computed according to the update time. (Optional, default: `FALSE`)

  - **DataSources**: One `DataSource` child element is required per sensor type. (Required)
      - **DataSource**: Data source (Required)
          - **PortName**: Port name (Required)
              - `Accelerometer` Raw sensor measurement. The values are stored in the translation part of           the transformation matrix. The rotation part is identity.
              - `Gyrometer` Raw sensor measurement. The values (angular velocity in degrees/s) are stored in the translation part of the transformation matrix. The rotation part is identity.
          - **SerialNumber**: Serial number (Optional)
              - If specified then connection is made to the device that has a matching SerialNumber. Otherwise, the first detected device is selected.

## Example configuration file PlusDeviceSet_Server_GenericSensor.xml

```xml
<PlusConfiguration version="2.1">

    <DataCollection StartupDelaySec="1.0">
        <DeviceSet Name="PlusServer: Generic sensor" Description="Broadcasting available sensors tracking data through OpenIGTLink" />
        <Device Id="TrackerDevice" Type="GenericSensor" ToolReferenceFrame="Tracker">
            <DataSources>
                <DataSource Type="Tool" Id="Accelerometer" PortName="Accelerometer" />
            </DataSources>
            <OutputChannels>
                <OutputChannel Id="TrackerStream">
                    <DataSource Id="Accelerometer"/>
                </OutputChannel>
            </OutputChannels>
        </Device>
        <Device Id="CaptureDevice" Type="VirtualCapture" BaseFilename="RecordingTest.igs.mha" EnableCapturingOnStart="FALSE">
            <InputChannels>
                <InputChannel Id="TrackerStream" />
            </InputChannels>
        </Device>
    </DataCollection>

    <PlusOpenIGTLinkServer MaxNumberOfIgtlMessagesToSend="1" MaxTimeSpentWithProcessingMs="50" ListeningPort="18944" OutputChannelId="TrackerStream">
        <DefaultClientInfo>
            <MessageTypes>
                <Message Type="TRANSFORM" />
            </MessageTypes>
            <TransformNames>
                <Transform Name="AccelerometerToTracker" />
            </TransformNames>
        </DefaultClientInfo>
    </PlusOpenIGTLinkServer>

</PlusConfiguration>
```