# Imaging controls analog USB frame grabber

## Supported hardware devices
Supported model: DFG/USB2pro analog frame grabbers with USB interface

- Link to manufacturer's website: <http://www.theimagingsource.com/en_US/products/converters/>

## Supported platforms
- [***Windows 32 bits***](/SupportedPlatforms/#windows-32-bits)
- [***Windows 64 bits***](/SupportedPlatforms/#windows-64-bits)
- [***Windows XP embedded 32 bits***](/SupportedPlatforms/#windows-xp-embedded-32-bits)

## Installation

## Device configuration settings

- **Type**: `ICCapturing`
- **AcquisitionRate**: (Optional, default: `125`)
- **LocalTimeOffsetSec**: (Optional, default: `0`)
- **DeviceName**: Requested device name
- **VideoNorm**: Requested video norm
- **VideoFormat**: Requested video format string
- **FrameSize**: Requested size of the video frame.
- **InputChannel**: Name of the IC capturing device input channel
- **ICBufferSize**: Number of frames the IC capturing device should store in a temporary buffer.
- **DataSources**: Exactly one `DataSource` child element is required (Required)
    - **DataSource**: (Required)
        - **PortUsImageOrientation**: (Required)
        - **ImageType**. Color image acquisition is supported. (Optional, default: `BRIGHTNESS`)
        - **BufferSize**: (Optional, default: `150`)
        - **AveragedItemsForFiltering**: (Optional, default: `20`)
        - **ClipRectangleOrigin**: (Optional, default: `0 0 0`)
        - **ClipRectangleSize**: (Optional, default: `0 0 0`)

## Example configuration file PlusDeviceSet_Server_ImagingControlsVideoCapture.xml

```xml
<PlusConfiguration version="2.0">

  <DataCollection StartupDelaySec="1.0" >
    <DeviceSet
      Name="PlusServer: ImagingControls video capture device"
      Description="Broadcasting acquired video through OpenIGTLink"
    />
    <Device
      Id="VideoDevice"
      Type="ICCapturing"
      DeviceName="DFG/USB2-lt"
      VideoNorm="NTSC_M"
      VideoFormat="Y800"
      FrameSize="640 480"
      InputChannel="01 Video: SVideo" >
      <DataSources>
        <DataSource Type="Video" Id="Video" PortUsImageOrientation="MF"  />
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