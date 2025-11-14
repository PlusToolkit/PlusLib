# Infrared Thermal Expert EV2 camera

## This device allow you to use a Infrared Thermal Expert EV2 Camera to capture and send infrared thermal images.

This device only works in Windows (2023-01-03).

## You can found the license in:
 "PlusLib/src/Utilities/InfraredTEEV2Camera/README.md"

## Device configuration settings

- **Type**: `InfraredTEEV2Cam`
- **DataSources**: Exactly one `DataSource` child element is required. (Required)
   - **DataSource**: (Required)
      - **PortUsImageOrientation**: (Required)

## Example configuration file PlusDeviceSet_Server_InfraredTEEV2Camera.xml

```xml
<PlusConfiguration version="2.1">

  <DataCollection StartupDelaySec="1.0" >
    <DeviceSet
      Name="PlusServer: Infrared Thermal Expert EV2 Camera"
      Description="Broadcasting acquired video through PlusServer from a Thermal Expert EV2 Camera"
    />
    <Device
      Id="VideoDevice"
      Type="InfraredTEEV2Cam"
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