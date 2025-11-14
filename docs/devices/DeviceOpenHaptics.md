# OpenHaptics Device

## Supported hardware devices

- Link to manufacturer's website: <https://www.3dsystems.com/haptics-devices/openhaptics>

## Supported platforms

- [***Windows 64 bits***](/SupportedPlatforms/#windows-64-bits)
- [***Windows 32 bits***](/SupportedPlatforms/#windows-32-bits)

## Installation
- Install OpenHaptics Developer SDK from: <http://developer.geomagic.com>

## Device configuration settings

The device will output three transforms: the tracking transform of the the stylus, the stylus velocity, and the stylus buttons.  In order to
transmit forces to the device, an input channel with a single tool DataSource (with portname "Force") should be provided.  The force vector should be stored in the
translation portion of the transformation matrix (RAS coordinates).

- **Type**: `OpenHaptics`
- **DeviceName**: Device Name. Name of device to connect to
- **AcquisitionRate**: (Optional, default: `20`)
- **LocalTimeOffsetSec**: (Optional, default: `0`)
- **ToolReferenceFrame**: (Optional, default: `Base`)
- **DataSources**: Three `DataSource` child elements are required. (Required)
    - **DataSource**: (Required)
        - **PortName**: (Required)
            - `Stylus` Tracking transform for stylus (RAS).
            - `StylusVelocity` Stylus velocity measurement (RAS). The values are stored in the translation part of the transformation matrix. The rotation part is identity.
            - `Buttons` States for buttons. The button values are stored in the first column of the matrix.  The inkwell switch is the first element in the second column.
        - **BufferSize**: (Optional, default: `150`)
        - **AveragedItemsForFiltering**: (Optional, default: `20`)
- **InputChannels**: An Input channel is required to send force data to the device (Required)
    - **InputChannel**: (Required)
    - **Id** Identifier of an output channel of another device containing a tool with PortName=(`Force`) (Required)

## Example configuration file PlusDeviceSet_Server_OpenHaptics_WithForces.xml

```xml
<PlusConfiguration version="2.1">
  <DataCollection StartupDelaySec="1.0">
    <DeviceSet
      Name="PlusServer: Open Haptics device tracking and force rendering"
      Description="Broadcasting  device stylyus tracking data and receiving force data from 3D Slicer through OpenIGTLink." />
    <Device
      Id="3DSlicerForceDataSource"
      Type="OpenIGTLinkTracker"
      MessageType="TRANSFORM"
      ToolReferenceFrame="Ref"
      ServerAddress="127.0.0.1"
      ServerPort="22222"
      IgtlMessageCrcCheckEnabled="true"
      AcquisitionRate="20"
      ReconnectOnReceiveTimeout="true"
      UseReceivedTimestamps ="FALSE"
      UseLastTransformsOnReceiveTimeout ="TRUE">
      <DataSources>
        <DataSource Type="Tool" Id="Force" PortName="Force"/>
      </DataSources>
      <OutputChannels>
        <OutputChannel Id="ForceStream">
          <DataSource Id="Force" />
        </OutputChannel>
      </OutputChannels>
    </Device>
    <Device
      Id="TrackerDevice"
      Type="OpenHaptics"
      AcquisitionRate="50"
      ToolReferenceFrame="Base"
      DeviceName="PLUS"  >
      <DataSources>
        <DataSource Type="Tool" Id="Stylus"  PortName="Stylus" />
        <DataSource Type="Tool" Id="StylusVelocity"  PortName="StylusVelocity" />
        <DataSource Type="Tool" Id="Buttons"  PortName="Buttons" />
      </DataSources>
      <InputChannels>
        <InputChannel Id="ForceStream" />
      </InputChannels>
      <OutputChannels>
        <OutputChannel Id="NeedleStream">
          <DataSource Id="Stylus" />
          <DataSource Id="StylusVelocity" />
          <DataSource Id="Buttons" />
        </OutputChannel>
      </OutputChannels>
    </Device>
  </DataCollection>
  <PlusOpenIGTLinkServer
    MaxNumberOfIgtlMessagesToSend="1"
    MaxTimeSpentWithProcessingMs="50"
    ListeningPort="18944"
    OutputChannelId="NeedleStream" >
    <DefaultClientInfo>
      <MessageTypes>
        <Message Type="TRANSFORM" />
      </MessageTypes>
      <TransformNames>
        <Transform Name="StylusToBase" />
        <Transform Name="StylusVelocityToBase" />
        <Transform Name="ButtonsToBase" />
      </TransformNames>
    </DefaultClientInfo>
  </PlusOpenIGTLinkServer>
</PlusConfiguration>
```

## Example configuration file PlusDeviceSet_Server_OpenHaptics.xml

```xml
<PlusConfiguration version="2.1">
  <DataCollection StartupDelaySec="1.0">
    <DeviceSet
      Name="PlusServer: Open Haptics device tracking"
      Description="Broadcasting  device stylyus tracking data through OpenIGTLink." />
    <Device
      Id="TrackerDevice"
      Type="OpenHaptics"
      AcquisitionRate="50"
      ToolReferenceFrame="Base"
      DeviceName="PLUS"  >
      <DataSources>
        <DataSource Type="Tool" Id="Stylus"  PortName="Stylus" />
        <DataSource Type="Tool" Id="StylusVelocity"  PortName="StylusVelocity" />
        <DataSource Type="Tool" Id="Buttons"  PortName="Buttons" />
      </DataSources>
      <OutputChannels>
        <OutputChannel Id="NeedleStream">
          <DataSource Id="Stylus" />
          <DataSource Id="StylusVelocity" />
          <DataSource Id="Buttons" />
        </OutputChannel>
      </OutputChannels>
    </Device>
  </DataCollection>
  <PlusOpenIGTLinkServer
    MaxNumberOfIgtlMessagesToSend="1"
    MaxTimeSpentWithProcessingMs="50"
    ListeningPort="18944"
    OutputChannelId="NeedleStream" >
    <DefaultClientInfo>
      <MessageTypes>
        <Message Type="TRANSFORM" />
      </MessageTypes>
      <TransformNames>
        <Transform Name="StylusToBase" />
        <Transform Name="StylusVelocityToBase" />
        <Transform Name="ButtonsToBase" />
      </TransformNames>
    </DefaultClientInfo>
  </PlusOpenIGTLinkServer>
</PlusConfiguration>
```
