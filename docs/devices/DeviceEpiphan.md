# Epiphan frame grabber

## Supported hardware devices

- Recommended model for VGA and DVI capture: [DVI2USB 3.0](<http://www.epiphan.com/products/dvi2usb-3-0/>). The basic VGA2USB model is not suitable for most applications, because it provides interlaced output at a very low frame rate.
- This device is not compatible with Epiphan AV.io frame grabbers. If using one of these devices, use the `DeviceMicrosoftMediaFoundation` or `DeviceOpenCVVideo` imaging devices to connect to the frame grabber.

- Link to manufacturer's website: <http://www.epiphan.com/>.

## Supported platforms

- [***Windows 32 bits***](../SupportedPlatforms.md#windows-32-bits)
- [***Windows 64 bits***](../SupportedPlatforms.md#windows-64-bits)
- [***Windows XP embedded 32 bits***](../SupportedPlatforms.md#windows-xp-embedded-32-bits)
- [***Mac OS X***](../SupportedPlatforms.md#macos)
- [***Linux***](../SupportedPlatforms.md#linux)

The device manufacturer supports additional platforms. On request, Plus toolkit will be extended to support this device on other platforms, too.

## Installation

Need to install the drivers and software from [VGA2USB-LR download](http://www.epiphan.com/products/frame-grabbers/vga2usb-lr/download/).

## Device configuration settings

  - **Type**: `Epiphan` (Required)
  - **AcquisitionRate**: (Optional, default: `30`)
  - **SerialNumber**: it is an optional parameter to set the `ID` of the preferred a frame grabber device
  - **ClipRectangleOrigin**: and **ClipRectangleSize** define a screen region to capture information from, the clip rectangle width has to be typically a mulitple of 4
  - **GrabberLocation**: it is used to choose a specific epiphan device
  - **RotationMode**: it is used to configure the rotation parameter of the captured images. Options are None, `Left90`, `Right90`, `180`
  - **ScaleMode**: it is used to configure the scaling parameter of the captured images. Options are `NearestNeighbor`, `WeightedAverage`, `FastBilinear`, `Bilinear`, `Bicubic`, `Experimental`, `Point`, `Area`, `BicubLin`, `Sinc`, `Lanczos`, `Spline`, `Hardware`
  - **DataSources**: Exactly one `DataSource` child element is required. (Required)
     - **DataSource**: (Required)
      - **PortUsImageOrientation**: (Required)
      - **ImageType**. Color image acquisition is supported by setting the imaging device's common attribute `RGB_COLOR` as shown in the Epiphan Color Video Capture example. (Optional, default: `BRIGHTNESS`)
      - **BufferSize**: (Optional, default: `150`)
      - **AveragedItemsForFiltering**: (Optional, default: `20`)
      - **ClipRectangleOrigin**: Software clipping, applied on top of the hardware clipping region. (Optional, default: `0 0 0`)
      - **ClipRectangleSize**: Software clipping, applied on top of the hardware clipping region. (Optional, default: `0 0 0`)

## Example configuration file PlusDeviceSet_Server_EpiphanVideoCapture.xml

```xml
<PlusConfiguration version="2.1">

  <DataCollection StartupDelaySec="1.0" >
    <DeviceSet
      Name="PlusServer: Epiphan video capture device"
      Description="Broadcasting acquired video through OpenIGTLink"
    />
    <Device
      Id="VideoDevice"
      Type="Epiphan" >
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

## Epiphan Color Video Capture Example configuration file PlusDeviceSet_Server_EpiphanColorVideoCapture.xml

```xml
<PlusConfiguration version="2.1">

  <DataCollection StartupDelaySec="1.0" >
    <DeviceSet
      Name="PlusServer: Epiphan video capture device - color"
      Description="Broadcasting acquired video through OpenIGTLink"
    />
    <Device
      Id="VideoDevice"
      Type="Epiphan" >
      <DataSources>
        <DataSource Type="Video" Id="Video" PortUsImageOrientation="MF"  ImageType="RGB_COLOR" />
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
>
```
