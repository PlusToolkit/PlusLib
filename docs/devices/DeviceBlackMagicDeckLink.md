# BlackMagic DeckLink

## Supported hardware devices

## Supported platforms

- [***Windows 32 bits***](../SupportedPlatforms.md#windows-32-bits)
- [***Windows 64 bits***](../SupportedPlatforms.md#windows-64-bits)
- [***Mac OS X***](../SupportedPlatforms.md#macos)
- [***Linux***](../SupportedPlatforms.md#linux)

## BlackMagic DeckLink SDK Versions
Please build PLUS BlackMagic support against the following DeckLink SDK versions. Other versions may work, but are not regularly tested by the PLUS developer team.

- DeckLink v 11.1

## Building the BlackMagic DeckLink PLUS support

## Device configuration settings
  - **Type**: `BlackMagic`
  - **ToolReferenceFrame**: (Optional, default: `Tracker`)
  - **DataSources**: (Required)
    - **DataSource**: (Required)

## Config File

```xml
<PlusConfiguration version="2.0">

  <DataCollection StartupDelaySec="1.0" >
    <DeviceSet
      Name="PlusServer: DeckLink capture API - color"
      Description="Broadcasting acquired video through OpenIGTLink" />
    <Device
      Id="VideoDevice"
      Type="DeckLinkVideo"
      FrameSize="1920 1080"
      DisplayMode="HD1080p5994"
      PixelFormat="8BitYUV"
      CaptureDeviceId="0" >
      <DataSources>
        <DataSource Type="Video" Id="Video" PortUsImageOrientation="UN" ImageType="RGB_COLOR"  />
      </DataSources>
      <OutputChannels>
        <OutputChannel Id="VideoStream" VideoDataSourceId="Video" />
      </OutputChannels>
    </Device>

    <Device
      Id="CaptureDevice"
      Type="VirtualCapture"
      BaseFilename="decklink.igs.nrrd"
      EnableCapturingOnStart="FALSE"
      EnableCompression="TRUE"	  >
      <InputChannels>
        <InputChannel Id="VideoStream" />
      </InputChannels>
    </Device>
  </DataCollection>

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
        <Image Name="Image" EmbeddedTransformToFrame="Image" />
      </ImageNames>
    </DefaultClientInfo>
  </PlusOpenIGTLinkServer>

</PlusConfiguration>
```