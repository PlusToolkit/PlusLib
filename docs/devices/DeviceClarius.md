# Clarius ultrasound systems (CAST API)

## Clarius Device vs. ClariusOEM Device

PLUS supports two different interfaces to Clarius probes. This documentation is for the CAST API based PLUS device. Using this approach, a phone/tablet is connected to the probe, and the PC running PLUS retrieves images indirectly via this phone/tablet.

In contrast, the (relatively newer) Clarius support uses the Clarius OEM API which connects directly with the probe using the Bluetooth Low Energy (BLE) and Wi-Fi interfaces of the PC running PLUS. This device also exposes US controls for the Clarius, and real-time battery / button state streaming accessible as MRML node attributes on the Image node in 3D Slicer. The primary limitation of this interface is that for now it's only available on Windows. If possible, we recommend using this device. Documentation for the OEM API based PLUS device is available at `DeviceClariusOEM`

## Supported hardware devices

- This device is used for image acquisition from Clarius ultrasound systems using the CAST API.
- Link to manufacturer's website: <https://clarius.com>
- Link to the CAST API: <https://github.com/clariusdev/cast>
- Link to the IMU coordinate system documentation: <https://github.com/clariusdev/motion>
- Link to the raw data documentation: <https://github.com/clariusdev/raw>

## Supported platforms

- [***Windows 64 bits***](/SupportedPlatforms/#windows-64-bits)

## Connection
- Requires a connection to the same network as the Clarius
- Directly connecting to the Clarius's network is possible, however it can easily become congested
- It is recommended to use a local Wi-Fi network instead:
    - [Establishing a local Wi-Fi connection](<https://support.clarius.com/hc/en-us/articles/360019514951-Connecting-to-the-Scanner-via-a-Local-Wi-Fi-Network>)

## Device configuration settings

- **Device** element attributes:
  - **Type**: `Clarius`
  - **IpAddress**: IP address of probe on connected network. (Required)
  - **TcpPort**: Clarius CAST TCP port. (Required)
  - **FrameWidth**: Desired frame width. If the frame size is not supported the default will be used. (Optional, default: 640)
  - **FrameHeight**: Desired frame height. If the frame size is not supported the default will be used. (Optional, default: 480)
  - **ImuEnabled**: Enable IMU data streaming (`TRUE`/`FALSE`). (Optional, default: FALSE)
  - **ImuOutputFileName**: Filename to store IMU data when writing to disk. (Optional, default: ClariusImuData.csv)
  - **WriteImagesToDisk**: Save received images to disk (`TRUE`/`FALSE`). (Optional, default: (`FALSE`))
  - **DataSources**: Multiple `DataSource` child elements are allowed, one for each data type (`PortName` indicates the data type). (Required)
    - **DataSource**: (Required)
      - **DataSourceType**: Should be `Video` for images or `Tool` for IMU data. (Required)
      - **PortName**: One of the supported data kinds below. (Required)
        - `B` B-mode image data
        - `Rf` RF data
        - `Accelerometer` IMU accelerometer data
        - `Gyroscope` IMU gyroscope data
        - `Magnetometer` IMU magnetometer data
        - `OrientationSensor` IMU orientation data

## Example configuration file for Clarius B-mode image acquisition

```xml
<PlusConfiguration version="2.1">
  <DataCollection StartupDelaySec="1.0" >
    <DeviceSet
      Name="PlusServer: Clarius ultrasound device"
      Description="Broadcasting acquired video through OpenIGTLink"/>
    <Device Id="VideoDevice"
      Type="Clarius"
      IpAddress = "192.168.1.1"
      TcpPort = "5828"
      FrameWidth = "640"
      FrameHeight = "480"
      ImuEnabled = "TRUE"
      ImuOutputFileName = "ImuOutput.csv"
      WriteImagesToDisk = "FALSE">
      <DataSources>
        <DataSource Type="Video" Id="Video" PortName="B" PortUsImageOrientation="UN"/>
      </DataSources>
      <OutputChannels>
        <OutputChannel Id="VideoStream" VideoDataSourceId="Video" />
      </OutputChannels>
    </Device>
    <Device
      Id="CaptureDevice"
      Type="VirtualCapture"
      BaseFilename= "ClariusRecording.igs.mha"
      EnableCapturingOnStart="FALSE"
      RequestedFrameRate="25"
      EnableCapturing="TRUE" >
      <InputChannels>
        <InputChannel Id="VideoStream" />
      </InputChannels>
    </Device>
  </DataCollection>
  <CoordinateDefinitions>
    <Transform From="Image" To="Reference"
      Matrix="
      0.2 0.0 0.0 0.0
      0.0 0.2 0.0 0.0
      0.0 0.0 0.2 0.0
      0 0 0 1" />
  </CoordinateDefinitions>
  <PlusOpenIGTLinkServer
    MaxNumberOfIgtlMessagesToSend="1"
    MaxTimeSpentWithProcessingMs="50"
    ListeningPort="18944"
    SendValidTransformsOnly="false"
    OutputChannelId="VideoStream" >
    <DefaultClientInfo>
      <MessageTypes>
        <Message Type="IMAGE" />
      </MessageTypes>
      <ImageNames>
        <Image Name="Image" EmbeddedTransformToFrame="Reference" />
      </ImageNames>
    </DefaultClientInfo>
  </PlusOpenIGTLinkServer>
</PlusConfiguration>
```

## Example configuration file for Clarius B-mode image acquisition with IMU data

```xml
<PlusConfiguration version="2.1">
  <DataCollection StartupDelaySec="1.0" >
    <DeviceSet
      Name="PlusServer: Clarius ultrasound device"
      Description="Broadcasting acquired video through OpenIGTLink"/>
    <Device Id="VideoDevice"
      Type="Clarius"
      IpAddress = "192.168.1.1"
      TcpPort = "5828"
      FrameWidth = "640"
      FrameHeight = "480"
      ImuEnabled = "TRUE"
      ImuOutputFileName = "ImuOutput.csv"
      WriteImagesToDisk = "FALSE">
      <DataSources>
        <DataSource Type="Video" Id="Video" PortName="B" PortUsImageOrientation="UN"/>
        <DataSource Type="Tool" Id="Accelerometer" PortName="Accelerometer" />
        <DataSource Type="Tool" Id="Gyroscope" PortName="Gyroscope" />
        <DataSource Type="Tool" Id="Magnetometer" PortName="Magnetometer" />
        <DataSource Type="Tool" Id="TiltSensor" PortName="TiltSensor" />
        <DataSource Type="Tool" Id="FilteredTiltSensor" PortName="FilteredTiltSensor" />
        <DataSource Type="Tool" Id="OrientationSensor" PortName="OrientationSensor" />
      </DataSources>
      <OutputChannels>
        <OutputChannel Id="VideoStream" VideoDataSourceId="Video">
          <DataSource Id="Accelerometer"/>
          <DataSource Id="Gyroscope"/>
          <DataSource Id="Magnetometer"/>
          <DataSource Id="TiltSensor"/>
          <DataSource Id="FilteredTiltSensor"/>
          <DataSource Id="OrientationSensor"/>
        </OutputChannel>
      </OutputChannels>
    </Device>
    <Device
      Id="CaptureDevice"
      Type="VirtualCapture"
      BaseFilename= "ClariusRecording.igs.mha"
      EnableCapturingOnStart="FALSE"
      RequestedFrameRate="25"
      EnableCapturing="TRUE" >
      <InputChannels>
        <InputChannel Id="VideoStream" />
      </InputChannels>
    </Device>
  </DataCollection>
  <CoordinateDefinitions>
    <Transform From="Image" To="Reference"
      Matrix="
      0.2 0.0 0.0 0.0
      0.0 0.2 0.0 0.0
      0.0 0.0 0.2 0.0
      0 0 0 1" />
  </CoordinateDefinitions>
  <PlusOpenIGTLinkServer
    MaxNumberOfIgtlMessagesToSend="1"
    MaxTimeSpentWithProcessingMs="50"
    ListeningPort="18944"
    SendValidTransformsOnly="false"
    OutputChannelId="VideoStream" >
    <DefaultClientInfo>
      <MessageTypes>
        <Message Type="IMAGE" />
        <Message Type="TRANSFORM" />
      </MessageTypes>
      <ImageNames>
        <Image Name="Image" EmbeddedTransformToFrame="Reference" />
      </ImageNames>
      <TransformNames>
        <Transform Name="AccelerometerToVideoDevice" />
        <Transform Name="GyroscopeToVideoDevice" />
        <Transform Name="MagnetometerToVideoDevice" />
        <Transform Name="TiltSensorToVideoDevice" />
        <Transform Name="FilteredTiltSensorToVideoDevice" />
        <Transform Name="OrientationSensorToVideoDevice" />
      </TransformNames>
    </DefaultClientInfo>
  </PlusOpenIGTLinkServer>
</PlusConfiguration>
```

## Example configuration file for Clarius B-mode image and RF data acquisition

```xml
<PlusConfiguration version="2.1">

  <DataCollection StartupDelaySec="1.0" >
    <DeviceSet
      Name="PlusServer: Clarius ultrasound device (B-Mode and RF)"
      Description="Broadcasting acquired video from Clarius cast through OpenIGTLink. Port 18944 contains B-Mode images, while port 18945 contains RF images."/>

    <Device
      Id="VideoDevice"
      Type="Clarius"
      IpAddress = "192.168.1.1"
      TcpPort = "5828"
      FrameWidth = "640"
      FrameHeight = "480"
      ImuEnabled = "TRUE"
      ImuOutputFileName = "ImuOutput.csv"
      WriteImagesToDisk = "FALSE">
      <DataSources>
        <DataSource Type="Video" Id="BModeVideo" PortName="B" PortUsImageOrientation="UN"/>
        <DataSource Type="Video" Id="RfVideo" PortName="Rf" PortUsImageOrientation="UN"/>
      </DataSources>
      <OutputChannels>
        <OutputChannel Id="BModeVideoStream" VideoDataSourceId="BModeVideo" />
        <OutputChannel Id="RfVideoStream" VideoDataSourceId="RfVideo" />
      </OutputChannels>
    </Device>

  </DataCollection>

  <PlusOpenIGTLinkServer
    MaxNumberOfIgtlMessagesToSend="1"
    MaxTimeSpentWithProcessingMs="50"
    ListeningPort="18944"
    SendValidTransformsOnly="false"
    OutputChannelId="BModeVideoStream" >
    <DefaultClientInfo>
      <MessageTypes>
        <Message Type="IMAGE" />
      </MessageTypes>
      <ImageNames>
        <Image Name="BModeVideo" EmbeddedTransformToFrame="BModeVideo" />
      </ImageNames>
    </DefaultClientInfo>
  </PlusOpenIGTLinkServer>

  <PlusOpenIGTLinkServer
    MaxNumberOfIgtlMessagesToSend="1"
    MaxTimeSpentWithProcessingMs="50"
    ListeningPort="18945"
    SendValidTransformsOnly="false"
    OutputChannelId="RfVideoStream" >
    <DefaultClientInfo>
      <MessageTypes>
        <Message Type="IMAGE" />
      </MessageTypes>
      <ImageNames>
        <Image Name="RfVideo" EmbeddedTransformToFrame="RfVideo" />
      </ImageNames>
    </DefaultClientInfo>
  </PlusOpenIGTLinkServer>

</PlusConfiguration>
```
