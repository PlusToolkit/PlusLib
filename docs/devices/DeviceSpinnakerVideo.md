# Spinnaker Video Source

## Copmatible Devices
- Tested with FLIR Blackfly S PGE-13Y3C-C
- Other FLIR Spinnaker compatible cameras should be supported. If you discover issue with your hardware please provide a bug report on the [PlusLib GitHub issue tracker](https://github.com/PlusToolkit/PlusLib/issues). If you have feedback or would like a feature added, please create a discussion on the [PlusLib GitHub discussion forum](https://github.com/PlusToolkit/PlusLib/discussions).

## Supported platforms
- [***Windows 64 bits***](../SupportedPlatforms.md#windows-64-bits)
- [***Linux***](../SupportedPlatforms.md#linux) (Ubuntu 16.04 / 18.04)

## Spinnaker SDK Tested Versions
Please build PLUS Point Grey Spinnaker support against the following Spinnaker SDK versions. Other versions may work, but are not regularly tested by the PLUS developer team.

- Windows: 1.20.0.15
- Ubuntu 16.04 Xenial: 1.13.0.31
- Ubuntu 18.04 Bionic: 1.20.0.14

## Building the SpinnakerVideo PLUS support

- To build the software, please download Point Grey's Spinnaker SDK from the [FLIR downloads portal](https://www.ptgrey.com/support/downloads). Alternatively, confirmed working files needed to build the support for Spinnaker compatible cameras on Windows & Linux are available in the PLTools repository.

## Device configuration settings
Please check your device's technical specifications (available from the [FLIR downloads portal](https://www.ptgrey.com/support/downloads)) for the appropriate min & max values of these parameters for your hardware.

- **Type**: `SpinnakerVideo`
- **CameraNumber**: Camera number to connect to.
- **PixelEncoding**: (Optional, default: `RGB24`)
    - `RGB24` Standard RGB 24-bit pixel encoding.
    - `MONO8` 8-bit monochrome pixel encoding.
- **FrameSize**: Size of the image frame
- **FrameRate**: Number of frames acquired per second.
- **ExposureMode**: Enable manual / automatic control of the camera's exposure.
    - `TIMED` Manual control, the ExposureMicroSec attribute must be provided to use this option.
    - `AUTO_ONCE` Automatically set the exposure once on camera start.
    - `AUTO_CONTINUOUS` Continuously adjust the exposure of the camera during the acquisition.
- **ExposureMicroSec**: Exposure time as a float in micro seconds.
- **GainMode**: Enable manual / automatic control of the camera's gain.
    - `MANUAL` Manual control, the GainDB attribute must be provided to use this option.
    - `AUTO_ONCE` Automatically set the gain once on camera start.
    - `AUTO_CONTINUOUS` Continuously adjust the gain of the camera during the acquisition.
- **GainDB**: Gain as a float in decibels.
- **WhiteBalanceMode**: Enable manual / automatic control of the camera's white balance.
    - `MANUAL` Manual control, the WhiteBalanceRed & WhiteBalanceBlue attributes must be provided to use this option.
    - `AUTO_ONCE` Automatically set the white balance once on camera start.
    - `AUTO_CONTINUOUS` Continuously adjust the white balance of the camera during the acquisition.
- **WhiteBalanceRed**: White balance red value as a float.
- **WhiteBalanceBlue**: White balance blue value as a float.

## Minimal config File

```xml
<PlusConfiguration version="2.7">

  <DataCollection StartupDelaySec="1.0" >
    <DeviceSet
      Name="PlusServer: Point Grey Spinnaker API Video Acquisition."
      Description="Broadcasting acquired video through OpenIGTLink"
    />
    <Device
      Id="VideoDevice"
      Type="SpinnakerVideo"
      >
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
## Config file demonstrating some manual controls

```xml
<PlusConfiguration version="2.7">

  <DataCollection StartupDelaySec="1.0" >
    <DeviceSet
      Name="PlusServer: Point Grey Spinnaker API Video Acquisition with parameters"
      Description="Broadcasting acquired video through OpenIGTLink"
    />
    <Device
      Id="VideoDevice"
      Type="SpinnakerVideo"
      CameraNumber="0"
      PixelEncoding="RGB24"
      FrameSize="800 600"
      FrameRate="20"
      ExposureMode="TIMED"
      ExposureMicroSec="20"
      GainMode="MANUAL"
      GainDB="5"
      WhiteBalanceMode="AUTO_CONTINUOUS"
      >
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