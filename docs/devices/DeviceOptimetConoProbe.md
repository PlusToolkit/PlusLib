# Optimet ConoProbe single-point measuring sensor

## Supported hardware devices

- The ConoProbe is a robust optical sensor for high precision distance, 2D profiles and 3D measurements
- Link to manufacturer's website: <http://www.optimet.com/index.php>

## Supported platforms

- [***Windows 32 bits***](/SupportedPlatforms/#windows-32-bits)

This device is not included in any of the standard packages

The device manufacturer supports additional platforms. On request, Plus toolkit will be extended to support this device on other platforms, too.

## Installation
- Need to install the Smart32 SDK from [here](http://www.optimet.com/smart32-sdk.php).

## Device configuration settings

- **Type**: `OptimetConoProbe`
- **ToolReferenceFrame**: (Optional, default: `Tracker`)
- **LocalTimeOffsetSec**: (Optional, default: `0`)
- **AcquisitionRate**: (Optional, default: `100`)
- **LensOriginAlignment**: Defines the position of the lens origin w.r.t. a tracking tool. Defined as the unit vector
- **DelayBetweenMeasurements**: The time between successive measurements in micro seconds.
- **Frequency**: The measurement working frequency.
- **CoarseLaserPower**: The measurement coarse laser power.
- **FineLaserPower**: The measurement fine laser power.
- **DataSources**: Exactly two `DataSource` child element are required. (Required)
    - **DataSource**: (Required)
        - **PortName**: (Required)
            - `Measurement` The measured distance, either as the raw sensor measurement in mm (in all of the translation elements of the matrix), or aligned with the lens origin (using the LensOriginAlignment parameter described above).
            - `Parameters` Raw sensor measurements: Distance (mm), Signal-to-noise ratio (%) and Total. The values are stores in the translation part of the matrix (m(0,3) = Distance, m(1,3) = SNR, m(2,3) = Total). These values should only be used as a means of acquiring real-time parameter info.
        - **BufferSize**: (Optional, default: `150`)
        - **AveragedItemsForFiltering**: (Optional, default: `20`)

## Example configuration file PlusDeviceSet_Server_OptimetConoProbe.xml

```xml
<PlusConfiguration version="2.1">

  <DataCollection StartupDelaySec="1.0">
    <DeviceSet
      Name="PlusServer: OptimetConoprobe"
      Description="Broadcasting single-point distance data through OpenIGTLink."
    />

    <Device
      Id="MeasurerDevice"
      Type="OptimetConoProbe"
      ToolReferenceFrame="Measurer"
      LocalTimeOffsetSec="0"
      AcquisitionRate="100"
      LensOriginAlignment="1.0 1.0 1.0 0.0 0.0 0.0"
      DelayBetweenMeasurements="1"
      Frequency="100"
      CoarseLaserPower="13"
      FineLaserPower="0">
      <DataSources>
        <DataSource Type="Tool" Id="Measurement" PortName="Measurement" />
        <DataSource Type="Tool" Id="Parameters" PortName="Parameters" />
      </DataSources>
      <OutputChannels>
        <OutputChannel Id="MeasurerStream">
          <DataSource Id="Measurement" />
          <DataSource Id="Parameters" />
        </OutputChannel>
      </OutputChannels>
    </Device>

  </DataCollection>

  <PlusOpenIGTLinkServer
    MaxNumberOfIgtlMessagesToSend="1"
    MaxTimeSpentWithProcessingMs="50"
    ListeningPort="18944"
    SendValidTransformsOnly="true"
    OutputChannelId="MeasurerStream" >
    <DefaultClientInfo>
      <MessageTypes>
        <Message Type="TRANSFORM" />
      </MessageTypes>
      <TransformNames>
        <Transform Name="MeasurementToMeasurer" />
        <Transform Name="ParametersToMeasurer" />
      </TransformNames>
    </DefaultClientInfo>
  </PlusOpenIGTLinkServer>

</PlusConfiguration>
```