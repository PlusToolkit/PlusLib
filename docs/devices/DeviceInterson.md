# Interson USB ultrasound systems (old SDK)

## Supported hardware devices

- This device is used for image acquisition from Interson USB ultrasound systems with `SeeMore` probes with the C++ (old) interface. Probes with .NET interface should be used with `DeviceIntersonSDKCxx`.
- Link to manufacturer's website: <http://www.interson.com/products/seemore-153-usb-probes>

## Supported platforms

- [***Windows 32 bits***](../SupportedPlatforms.md#windows-32-bits)

## Installation

- Requires SDK provided by Interson for compilation and USB drivers provided by Interson for running (otherwise Plus application will not start due to missing WDAPI1010.dll).
- Limitations: imaging parameters are hardcoded, makeing them configurable is a work in progress, see details in [#866 ](https://plustoolkit.github.io/legacytickets), [#867 ](https://plustoolkit.github.io/legacytickets) and [#868 ](https://plustoolkit.github.io/legacytickets)

## Device configuration settings

- **Device**
    - **Type**: `IntersonVideo`
    - **AcquisitionRate**: (Optional, default: `30`)
    - **LocalTimeOffsetSec**: (Optional, default: `0`)
    - **EnableProbeButtonMonitoring**: controls if probe button press count should be included in the output channel in the first translation component of the `ProbeButtonToDummyTransform.` The value is incremented each time the physical button on the probe is pressed.
        - `FALSE` Probe button press count is not included in the output.
        - `TRUE` Probe button press count is included in the output.
    - **ImageSize**
    - **UsImagingParameters**: (Required)
        - **Contrast**
        - **DepthMm**
        - **DynRangeDb**
        - **FrequencyMhz**
        - **GainPercent**
        - **TimeGainCompensation**
        - **Intensity**
        - **Sector**
        - **ZoomFactor**
        - **SoundVelocity**
    - **DataSources**: Exactly one `DataSource` child element is required (Required)
        - **DataSource**: (Required)
            - **PortUsImageOrientation**: (Required)
            - **ImageType**: Supported imaging modes: B-mode (Optional, default: `BRIGHTNESS`)
            - **BufferSize**: (Optional, default: `150`)
            - **AveragedItemsForFiltering**: (Optional, default: `20`)
            - **ClipRectangleOrigin**: (Optional, default: `0 0 0`)
            - **ClipRectangleSize**: (Optional, default: `0 0 0`)

## Example configuration file PlusDeviceSet_Server_IntersonVideoCapture_WithParameters.xml

```xml
<PlusConfiguration version="2.1">

  <DataCollection StartupDelaySec="1.0" >
    <DeviceSet
      Name="PlusServer: Interson USB ultrasound device. Some parameters are set here."
      Description="Broadcasting acquired video through OpenIGTLink"
    />
    <Device
      Id="VideoDevice"
      Type="IntersonVideo"
      SoundVelocity="1541"
      DepthMm="100"
      Intensity="128"
      Contrast="256"
      GainPercent="0 0 60"
      ImageSize="800 512"
      SectorPercent="-1"
      DynRangeDb="-1"
      ZoomFactor="-1"
      FrequencyMhz="-1" >
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