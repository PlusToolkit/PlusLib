# PCO Ultraviolet camera

## This device allow you to use a PCO Ultraviolet Camera to capture and send ultraviolet images.

This device only works in Windows (2023-01-19).

## You can found the license in:
<https://github.com/PlusToolkit/PlusLib/tree/master/src/Utilities/UltravioletPCOUVCamera>

## Device configuration settings

- **Type**: `UltravioletPCOUVCam`
- **DataSources**: Exactly one `DataSource` child element is required. (Required)
    - **DataSource**: (Required)
        - **PortUsImageOrientation**: (Required)

## Example configuration file PlusDeviceSet_Server_UltravioletPCOUVCamera.xml

```xml
<PlusConfiguration version="2.1">

  <DataCollection StartupDelaySec="1.0" >
    <DeviceSet
      Name="PlusServer: PCO Ultraviolet Camera"
      Description="Broadcasting acquired video through PlusServer from a PCO Ultraviolet Camera"
    />
    <Device
      Id="VideoDevice"
      Type="UltravioletPCOUVCam"
      ExposureTime="100"
      TimeBaseExposure="2"
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