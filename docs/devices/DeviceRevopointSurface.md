# Revopoint 3D cameras

This device allows you to process depth data from Revopoint 3D cameras. The following data
can be retrieved:

- The depth map
- The points cloud

## Supported hardware devices

- Link to manufacturer's website: https://3dcamera.revopoint3d.com/html/areascan/index.html

## Supported platforms

- [***Windows 64 bits***](../SupportedPlatforms.md#windows-64-bits)
- [***Linux***](../SupportedPlatforms.md#linux) (Ubuntu >= 18.04)

## SDK Tested Versions

- Windows 10: 2.4.0
- Ubuntu 18.04: 2.4.0

## Installation

- Install Revopoint 3D Camera SDK (<https://3dcamera.revopoint3d.com/html/Areascan2/index.html>)
- On Linux, you might have to add udev rules on your system (<https://github.com/latimagine/PlusHelpers/tree/main/udev_rules>)

## Device configuration settings

- **Type**: `Revopoint3DCamera`
- **DataSources**: One `DataSource` child element is required per stream. (Required)
    - **DataSource**: Unique data source (Required)
        - **FrameType**: Type of stream to capture. (Required)
            - `DEPTH`
            - `PCL` (The points cloud is sent as an image with 3 scalar components representing the x-y-z coordinates of the points)
        - **FrameWidth**: Width of the depth stream. (Required)
            - Check Depth image in hardware specifications (<https://3dcamera.revopoint3d.com/html/areascan/index.html>)
        - **FrameRate Acquisition**: Frequency for the depth stream. (Required)
            - Check Depth image in hardware specifications (<https://3dcamera.revopoint3d.com/html/areascan/index.html>).
        - **DepthRange**: Depth range of acquisition in mm. (Optional, default: `5 500`)
            - Check Optimal/Max working distance in hardware specifications (<https://3dcamera.revopoint3d.com/html/areascan/index.html>).
        - **PortUsImageOrientation**: Image orientation (Optional, default: `UN`)

## Example configuration file PlusDeviceSet_Server_Revopoint3DCamera.xml

```xml
<PlusConfiguration version="2.3">
  <DataCollection StartupDelaySec="1.0">
    <DeviceSet Name="PlusServer: Revopoint 3D Camera" Description="Broadcasting acquired depth video through PlusServer from Revopoint 3D camera" />
    <Device Id="VideoDevice" Type="Revopoint3DCamera">
      <DataSources>
        <DataSource Type="Video" Id="VideoDEPTH" FrameType="DEPTH" FrameWidth="1920" FrameRate="8" DepthRange="5 500" PortUsImageOrientation="UN" />
        <DataSource Type="Video" Id="VideoPCL" FrameType="PCL" PortUsImageOrientation="UN" />
      </DataSources>
      <OutputChannels>
        <OutputChannel Id="VideoStreamDEPTH" VideoDataSourceId="VideoDEPTH" />
        <OutputChannel Id="VideoStreamPCL" VideoDataSourceId="VideoPCL" />
      </OutputChannels>
    </Device>
  </DataCollection>
  <PlusOpenIGTLinkServer MaxNumberOfIgtlMessagesToSend="1" MaxTimeSpentWithProcessingMs="50" ListeningPort="18944" SendValidTransformsOnly="true" OutputChannelId="VideoStreamDEPTH">
    <DefaultClientInfo>
      <MessageTypes>
        <Message Type="IMAGE" />
      </MessageTypes>
      <ImageNames>
        <Image Name="VideoStreamDEPTH" EmbeddedTransformToFrame="VideoStreamDEPTH" />
      </ImageNames>
    </DefaultClientInfo>
  </PlusOpenIGTLinkServer>
  <PlusOpenIGTLinkServer MaxNumberOfIgtlMessagesToSend="1" MaxTimeSpentWithProcessingMs="50" ListeningPort="18945" SendValidTransformsOnly="true" OutputChannelId="VideoStreamPCL">
    <DefaultClientInfo>
      <MessageTypes>
        <Message Type="IMAGE" />
      </MessageTypes>
      <ImageNames>
        <Image Name="VideoStreamPCL" EmbeddedTransformToFrame="VideoStreamPCL" />
      </ImageNames>
    </DefaultClientInfo>
  </PlusOpenIGTLinkServer>
</PlusConfiguration>
```