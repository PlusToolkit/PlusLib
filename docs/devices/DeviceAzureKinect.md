# Azure Kinect camera
This device allow you to retrieve different kind of data from Azure Kinect RGB-D camera:

- The rgb image
- The depth map
- The reconstructed points cloud

The depth map resolution can be aligned to the rgb image resolution.

The points cloud resolution matches the rgb image resolution or the depth map resolution if no rgb data
is retrieved. In the first case, the color data of the rgb image can be used to enhance the points cloud.

## Supported hardware devices

- Link to manufacturer's website: <https://docs.microsoft.com/en-us/azure/kinect-dk/system-requirements>

## Supported platforms

- [***Windows 64 bits***](/SupportedPlatforms/#windows-64-bits)
- [***Linux***](/SupportedPlatforms/#linux) (Ubuntu 18.04 / 20.04)

## SDK Tested Versions

- Windows 10: 1.4.1
- Ubuntu 18.04: 1.4.1
- Ubuntu 20.04: 1.4.1

## License

The Azure Kinect SDK is released under the MIT license (<https://github.com/microsoft/Azure-Kinect-Sensor-SDK/blob/release/1.4.x/LICENSE>)

## Installation

- Install Azure Kinect SDK 1.4.1 (<https://docs.microsoft.com/en-us/azure/kinect-dk/sensor-sdk-download>)
- Special tips for Ubuntu 20.04 (not supported by Microsoft): <https://github.com/microsoft/Azure-Kinect-Sensor-SDK/issues/1263>

## Device configuration settings

  - **Type**: `AzureKinect`
  - **AlignDepthStream** Choose whether to align RGB and depth streams. You must have both and RGB and a depth stream in your config to enable this option. (Optional, default: `FALSE`)

  - **DataSources**: One `DataSource` child element is required per stream from the Azure Kinect. (Required)
    - **DataSource**: (Required)
    - **FrameType** Type of stream to capture. (Required)
    - `RGB`
    - `DEPTH`
    - `PCL` (The points cloud is sent as an image with 3 scalar components representing the x-y-z coordinates of the points)
    - **FrameSize** Size of the video/depth stream. This must be the first number of the Color/Depth resolutions shown in the hardware specifications (<https://github.com/MicrosoftDocs/azure-docs/blob/master/articles/kinect-dk/hardware-specification.md>).
    - **FrameRate** Acquisition frequence for this stream. See hardware specifications (<https://github.com/MicrosoftDocs/azure-docs/blob/master/articles/kinect-dk/hardware-specification.md>).
    - **PortUsImageOrientation**: (Optional, default: `UN`)

## Example configuration file PlusDeviceSet_Server_AzureKinect.xml

```xml
<PlusConfiguration version="2.3">
  <DataCollection StartupDelaySec="1.0">
    <DeviceSet Name="PlusServer: Azure Kinect Camera " Description="Broadcasting acquired RGB and Depth video through PlusServer from Azure Kinect camera" />
    <Device Id="VideoDevice" Type="AzureKinect" AlignDepthStream="TRUE">
      <DataSources>
        <DataSource Type="Video" Id="VideoRGB" FrameType="RGB" FrameSize="1920" FrameRate="15" PortUsImageOrientation="UN" />
        <DataSource Type="Video" Id="VideoDEPTH" FrameType="DEPTH" FrameSize="1024" FrameRate="15" PortUsImageOrientation="UN" />
        <DataSource Type="Video" Id="VideoPCL" FrameType="PCL" FrameSize="1920" FrameRate="15" PortUsImageOrientation="UN" />
      </DataSources>
      <OutputChannels>
        <OutputChannel Id="VideoStreamRGB" VideoDataSourceId="VideoRGB" />
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
  <PlusOpenIGTLinkServer MaxNumberOfIgtlMessagesToSend="1" MaxTimeSpentWithProcessingMs="50" ListeningPort="18945" SendValidTransformsOnly="true" OutputChannelId="VideoStreamRGB">
    <DefaultClientInfo>
      <MessageTypes>
        <Message Type="IMAGE" />
      </MessageTypes>
      <ImageNames>
        <Image Name="VideoStreamRGB" EmbeddedTransformToFrame="VideoStreamRGB" />
      </ImageNames>
    </DefaultClientInfo>
  </PlusOpenIGTLinkServer>
  <PlusOpenIGTLinkServer MaxNumberOfIgtlMessagesToSend="1" MaxTimeSpentWithProcessingMs="50" ListeningPort="18946" SendValidTransformsOnly="true" OutputChannelId="VideoStreamPCL">
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