# Interson USB ultrasound systems (new SDK)

## Supported hardware devices
- This device is used for image acquisition from Interson USB ultrasound systems with `SeeMore` probes with the .NET (new) interface. Probes with old C++ interface should be used with `DeviceInterson`.
- Link to manufacturer's website: <http://www.interson.com/products/seemore-153-usb-probes>

## Supported platforms

- [***Windows 32 bits***](../SupportedPlatforms.md#windows-32-bits)

## Installation
- Requires SDK provided by Interson for compilation and USB drivers provided by Interson for running (otherwise Plus application will not start due to missing Interson.dll).

## Device configuration settings

  - **Type**: `IntersonVideoSDKCxx`
  - **AcquisitionRate**: (Optional, default: `30`)
  - **LocalTimeOffsetSec**: (Optional, default: `0`)
  - **DynRangeDb**: Dynamic Range `[dB]` of B-mode ultrasound.
  - **FrequencyMhz**: Ultrasound transmitter frequency `[MHz].
  - **PulseVoltage**: Set the excitation pulse voltage in volts. `[MHz].
  - **RfDecimation**: Set whether the RF decimator is used. If value is (`1`) then the sampling rate is halved, but the imaged depth is doubled.
  - **DataSources**: Exactly one `DataSource` child element is required. (Required)
    - **DataSource**: (Required)
        - **PortUsImageOrientation**: (Required)
        - **ImageType**: Supported imaging modes: B-mode (Optional, default: `BRIGHTNESS`)
        - **BufferSize**: (Optional, default: `150`)
        - **AveragedItemsForFiltering**: (Optional, default: `20`)
        - **ClipRectangleOrigin**: (Optional, default: `0 0 0`)
        - **ClipRectangleSize**: (Optional, default: `0 0 0`)
  - **OutputChannels**:
    - **OutputChannel**:
      - [**RfProcessing**](../algorithms/AlgorithmRfProcessing.md): Required if scan-converted B-mode image output is needed.

## Example configuration file for B-mode image acquisition PlusDeviceSet_Server_IntersonSDKCxx_GP3.5.xml

```xml
<PlusConfiguration version="2.3">

  <DataCollection StartupDelaySec="1.0">
    <DeviceSet
      Name="PlusServer: Interson USB ultrasound device with Interson SDK."
      Description="Broadcasting acquired video through OpenIGTLink"
    />
    <Device
      Id="VideoDevice"
      Type="IntersonSDKCxxVideo"
      PulseVoltage="1"
      AcquisitionRate="12"
      DynRangeDb="0.4"
      FrequencyMhz="5.0" >
      <DataSources>
        <DataSource Type="Video" Id="Video" PortName="B" PortUsImageOrientation="UF" />
      </DataSources>
      <OutputChannels>
        <OutputChannel Id="VideoStream" VideoDataSourceId="Video" >
          <RfProcessing>
            <ScanConversion
              TransducerName="GP3.5"
              TransducerGeometry="CURVILINEAR"
              RadiusStartMm="32.8"
              RadiusStopMm="139.0"
              ThetaStartDeg="-45.0"
              ThetaStopDeg="45.0"
              OutputImageSizePixel="512 512"
              TransducerCenterPixel="256 17"
              OutputImageSpacingMmPerPixel="0.156 0.156" />
          </RfProcessing>
        </OutputChannel>
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
## Example configuration file for simultaneous B-mode and RF-mode image acquisition PlusDeviceSet_Server_IntersonSDKCxx_GP3.5_Rf.xml

```xml
<PlusConfiguration version="2.3">

  <DataCollection StartupDelaySec="5.0">
    <DeviceSet
      Name="PlusServer: Interson USB ultrasound device with Interson SDK, RF."
      Description="Broadcasting acquired video through OpenIGTLink"
    />
    <Device
      Id="VideoDevice"
      Type="IntersonSDKCxxVideo"
      RfDecimation="1"
      PulseVoltage="40"
      AcquisitionRate="12"
      FrequencyMhz="3.50" >
      <DataSources>
        <DataSource Type="Video" Id="BmodeVideo" PortName="B" PortUsImageOrientation="UF" />
        <DataSource Type="Video" Id="RfVideo" PortName="Rf" PortUsImageOrientation="FU" />
      </DataSources>
      <OutputChannels>
        <OutputChannel Id="BmodeVideoStream" VideoDataSourceId="BmodeVideo">
          <RfProcessing>
            <RfToBrightnessConversion
              NumberOfHilbertFilterCoeffs="32.0"
              BrightnessScale="20.0" />
            <ScanConversion
              TransducerName="GP 3.5MHz"
              TransducerGeometry="CURVILINEAR"
              RadiusStartMm="70.0"
              RadiusStopMm="175.1"
              ThetaStartDeg="-45.0"
              ThetaStopDeg="45.0"
              OutputImageSizePixel="800 600"
              TransducerCenterPixel="400 50"
              OutputImageSpacingMmPerPixel="0.20 0.20" />
          </RfProcessing>
        </OutputChannel>
        <OutputChannel Id="RfVideoStream" VideoDataSourceId="RfVideo" />
      </OutputChannels>
    </Device>

    <Device
      Id="CaptureDevice"
      Type="VirtualCapture"
      BaseFilename="IntersonRf.igs.mha"
      RequestedFrameRate="12"
      EnableCapturingOnStart="TRUE" >
      <InputChannels>
        <InputChannel Id="RfVideoStream" />
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
    OutputChannelId="BmodeVideoStream" >
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