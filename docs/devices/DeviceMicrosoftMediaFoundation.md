# Microsoft Media Foundation compatible imaging device

## Supported hardware devices
Any compatible frame grabbers, surgical microscopes (for example Zeiss Pentero), endoscopes, webcams, etc. through Microsoft Media Foundation interface.

## Supported platforms

- [***Windows 32 bits***](/SupportedPlatforms/#windows-32-bits)
- [***Windows 64 bits***](/SupportedPlatforms/#windows-64-bits)

## Installation

Media foundation require Microsoft Windows 7 or later.

If your framegrabber uses a "crossbar" for switching between multiple inputs (e.g., composite, s-video) then you may need to connect to your device through a virtual camera software (such as <http://splitcamera.com/> or <https://manycam.com/>). It is recommended to upgrade to a more recent UVC 1.1 compatible device (<http://en.wikipedia.org/wiki/USB_video_device_class>) or use
`DeviceICCapturing` or `DeviceEpiphan` imaging devices that Plus has built-in native support for.

## Device configuration settings

  - **Type**: `MmfVideo`
  - **AcquisitionRate**: (Optional, default: `30`)
  - **LocalTimeOffsetSec**: (Optional, default: `0`)
  - **FrameSize**: Size of the video frame.
  - **VideoFormat**: It specifies the video subtype format. All the available formats are described [here.]
  - **CaptureDeviceId**: Identifier of the capture device. The first device id is 0.
  - **CaptureStreamIndex**: Index of the output stream of the device. The first stream index is 0.
  - **DataSources**: Exactly one `DataSource` child element is required. (Required)
     - **DataSource**: (Required)
      - **PortUsImageOrientation**: (Required)
      - **ImageType**: Color image acquisition is supported by setting the imaging device's common attribute `RGB_COLOR`. (Optional, default: `BRIGHTNESS`)
      - **BufferSize**: (Optional, default: `150`)
      - **AveragedItemsForFiltering**: (Optional, default: `20`)
      - **ClipRectangleOrigin**: (Optional, default: `0 0 0`)
      - **ClipRectangleSize**: (Optional, default: `0 0 0`)

## Example configuration file PlusDeviceSet_Server_MmfColorVideoCapture.xml

```xml
<PlusConfiguration version="2.0">

  <DataCollection StartupDelaySec="1.0" >
    <DeviceSet
      Name="PlusServer: Media Foundation video capture device - color"
      Description="Broadcasting acquired video through OpenIGTLink" />
    <Device
      Id="VideoDevice"
      Type="MmfVideo"
      FrameSize="640 480"
      VideoFormat="YUY2"
      CaptureDeviceId="0" >
      <DataSources>
        <DataSource Type="Video" Id="Video" PortUsImageOrientation="MF" ImageType="RGB_COLOR"  />
      </DataSources>
      <OutputChannels>
        <OutputChannel Id="VideoStream" VideoDataSourceId="Video" />
      </OutputChannels>
    </Device>
    <Device
      Id="CaptureDevice"
      Type="VirtualCapture"
      BaseFilename="RecordingTest.igs.mha"
      EnableCapturingOnStart="FALSE" >
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