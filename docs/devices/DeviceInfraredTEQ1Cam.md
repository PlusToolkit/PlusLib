# Infrared Thermal Expert Q1 camera

## This device allow you to use a Infrared TEQ1 Pro Camera to capture and send infrared thermal images.

This device only works in Windows. We hope in the next months to integrate in Linux.

## You can found the license in:
 "PlusLib/src/Utilities/InfraredTEQ1Camera/README.md"

## Device configuration settings

- **Type**: `InfraredTEQ1Cam`
- **DataSources**: Exactly one `DataSource` child element is required. (Required)
    - **DataSource**: (Required)
        - **PortUsImageOrientation**: (Required)

## Example configuration file PlusDeviceSet_Server_InfraredTEQ1Camera.xml

```xml
<PlusConfiguration version="2.1">

  <DataCollection StartupDelaySec="1.0" >
    <DeviceSet
      Name="PlusServer: Infrared Thermal Expert Q1 Camera"
      Description="Broadcasting acquired video through PlusServer from a Thermal Expert Q1 Camera"
    />
    <Device
      Id="VideoDevice"
      Type="InfraredTEQ1Cam"
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
        1 0 0 0
        0 1 0 0
        0 0 1 0
        0 0 0 1" />
  </CoordinateDefinitions>

  <PlusOpenIGTLinkServer
    MaxNumberOfIgtlMessagesToSend="1"
    MaxTimeSpentWithProcessingMs="50"
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