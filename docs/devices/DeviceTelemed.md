# Telemed USB ultrasound systems

## Supported hardware devices

- This device is used for image acquisition from Telemed ultrasound systems.
- Tested only with MicrUs devices (<http://www.pcultrasound.com/products/micrus/index.html>) but other Telemed devices should be compatible, too.
- Link to manufacturer's website: <http://www.pcultrasound.com>

## Supported platforms

- [***Windows 32 bits***](../SupportedPlatforms.md#windows-32-bits)
- [***Windows 64 bits***](../SupportedPlatforms.md#windows-64-bits)

## Installation

- Requires drivers provided by Telemed for running (otherwise Plus application will not start).
- It is recommended to run the Telemed imaging software to set all imaging parameters, as Plus currently can set only a subset of all imaging parameters.

## Device configuration settings

- **Type**: `TelemedVideo`
- **AcquisitionRate**: (Optional, default: `30`)
- **LocalTimeOffsetSec**: (Optional, default: `0`)
- **FrameSize**: Maximum size of an image frame in pixels. The image is scaled so that image vertically fills the specified rectangle size, therefore if the frame size is too narrow (first component is too small) then the two sides of the image may be clipped; if the frame is too wide then there will be solid filled stripes on the left and right sides. If larger values are specified then a higher-resolution image is created. (Optional, default: `512 512`)
- **DepthMm**: Set the depth [mm] of B-mode ultrasound. If not specified (or value is <0) then the current value is kept.
- **FrequencyMhz**: Ultrasound transmitter frequency. If not specified (or value is <0) then the current value is kept. [MHz].
- **DynRangeDb**: Dynamic Range [dB] of B-mode ultrasound. If not specified (or value is <0) then the current value is kept.
- **GainPercent**: Is the gain percentage of B-mode ultrasound. If not specified (or value is <0) then the current value is kept. Valid range: 0-100.
- **PowerPercent**: Power to be used for imaging, as a percentage of maximum available power. If not specified (or value is <0) then the current value is kept. Valid range is 0-100.
- **SpeckleReductionEnabled**: Enable (`TRUE`) or disable (`FALSE`) speckle reduction (ClearView) filtering of B-mode ultrasound. If not specified then the current value is kept.
- **SpeckleReductionMethod**: Speckle reduction filter method, specified as either a value of the Telemed `CLEAR_VIEW` enumeration (for example `306`) or a human-readable enum token (for example `SRF_NVC6`). The value encodes the filter family (hundreds digit) and the filtration level (1-16 within each family, higher = stronger filtering). If not specified (or value is <0) then the current value is kept.

    | Values | Filter family | Notes |
    |---|---|---|
    | 1-16 | ClearView (`SRF_CV1..16`) | Requires purchased license and inserted USB dongle key |
    | 101-116 | PureView (`SRF_PV1..16`) | Generally available |
    | 201-216 | NeatView GPU (`SRF_NV1..16`) | Availability depends on graphics adapter and drivers |
    | 301-316 | NeatView CPU (`SRF_NVC1..16`) | Generally available |
    | 401-416 | QuickView (`SRF_QV1..16`) | Reserved for future use by Telemed |
    | 501-516 | `SRF_NVO1..16` | Present in SDK 4.1.0 headers, undocumented |

    Per the Telemed SDK programmer's guide, in most cases only PureView and CPU-based NeatView are available; Telemed's own default scanning parameter files use `306` (NeatView CPU, level 6, equivalent to `SRF_NVC6`). Note that speckle reduction filters may reduce the frame rate. Both speckle reduction parameters can also be changed at runtime with the `SET_US_PARAMETER` OpenIGTLink command (parameter names `SpeckleReductionEnabled` and `SpeckleReductionMethod`).
- **DataSources**: Exactly one `DataSource` child element is required (Required)
    - **DataSource**: (Required)
        - **PortUsImageOrientation**: (Required)
        - **BufferSize**: (Optional, default: `150`)
        - **AveragedItemsForFiltering**: (Optional, default: `20`)
        - **ClipRectangleOrigin**: (Optional, default: `0 0 0`)
        - **ClipRectangleSize**: (Optional, default: `0 0 0`)

## Example configuration file PlusDeviceSet_Server_TelemedVideoCapture.xml

```xml
<PlusConfiguration version="2.1">

  <DataCollection StartupDelaySec="1.0" >
    <DeviceSet
      Name="PlusServer: Telemed ultrasound device"
      Description="Broadcasting acquired video through OpenIGTLink"
    />
    <Device
      Id="VideoDevice"
      Type="TelemedVideo" >
      <DataSources>
        <DataSource Type="Video" Id="Video" PortUsImageOrientation="UN"  />
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