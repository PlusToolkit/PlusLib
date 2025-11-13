# grabber for CameraLink camera

## This device allow you to use a DAQ USB-FRM13-B grabber to capture and send images from a CameraLink camera.

This device only works in Windows (2023-03-16).

## You can found the license in:
 "PlusLib/src/Utilities/DAQVideoSource/README.md"

## Device configuration settings

  - **Type**: `DAQVideoSource`
  - **DataSources**: Exactly one `DataSource` child element is required. (Required)
     - **DataSource**: (Required)
      - **PortUsImageOrientation**: (Required)
`
## Example configuration file PlusDeviceSet_Server_DAQVideoSource.xml

```xml
<PlusConfiguration version="2.1">

  <DataCollection StartupDelaySec="1.0" >
    <DeviceSet
      Name="PlusServer: DAQ USB3-FRM13-B Grabber (CameraLink)"
      Description="Broadcasting acquired video through PlusServer from a DAQ USB3-FRM13-B Grabber (CameraLink)"
    />
    <Device
      Id="VideoDevice"
      Type="DAQVideoSource"
      DataMode="16"
      >
      <DataSources>
        <DataSource Type="Video" Id="Video" PortUsImageOrientation="UF"/>
      </DataSources>
      <OutputChannels>
        <OutputChannel Id="VideoStream" VideoDataSourceId="Video" />
      </OutputChannels>
    </Device>
  </DataCollection>

  <CoordinateDefinitions>
    <Transform From="Image" To="Reference"
      Matrix="
        0.5 0 0 0
        0 -0.5 0 0
        0 0 0.5 0
        0 0 0 1" />
  </CoordinateDefinitions>

  <PlusOpenIGTLinkServer
    MaxNumberOfIgtlMessagesToSend="1"
    MaxTimeSpentWithProcessingMs="33"
    ListeningPort="18944"
    SendValidTransformsOnly="true"
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