# Video for Windows compatible imaging device

## Supported hardware devices

This device is deprecated, use MmfVideo device instead.

Compatible frame grabbers and webcams through Video for Windows interface.

Limitations: currently only certain color modes are supported, minor additional conversion routines may be needed for some devices.

## Supported platforms
- [***Windows 32 bits***](/SupportedPlatforms/#windows-32-bits)
- [***Windows 64 bits***](/SupportedPlatforms/#windows-64-bits)
- [***Windows XP embedded 32 bits***](/SupportedPlatforms/#windows-xp-embedded-32-bits)

## Installation

## Device configuration settings

- **Type**: `VFWVideo`
- **AcquisitionRate**: (Optional, default: `30`)
- **LocalTimeOffsetSec**: (Optional, default: `0`)
- **DataSources**: Exactly one `DataSource` child element is required (Required)
    - **DataSource**: (Required)
    - **PortUsImageOrientation**: (Required)
    - **ImageType**: (Optional, default: `BRIGHTNESS`)
    - **BufferSize**: (Optional, default: `150`)
    - **AveragedItemsForFiltering**: (Optional, default: `20`)
    - **ClipRectangleOrigin**: (Optional, default: `0 0 0`)
    - **ClipRectangleSize**: (Optional, default: `0 0 0`)

## Example configuration file PlusDeviceSet_Server_VfwVideoCapture.xml

```xml
<PlusConfiguration version="2.0">

  <DataCollection StartupDelaySec="2.0" >
    <DeviceSet
      Name="PlusServer: Video for Windows video capture device"
      Description="Broadcasting acquired video through OpenIGTLink" />
    <Device
      Id="VideoDevice"
      Type="VFWVideo" >
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