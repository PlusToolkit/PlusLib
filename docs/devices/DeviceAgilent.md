# Oscilloscope interface

## Supported hardware devices

- Acquiris D310 Oscilloscope

## Supported platforms

- [***Windows 32 bits***](../SupportedPlatforms.md#windows-32-bits)

## Device configuration settings

  - **Type**: `AgilentScope`

  - **AcquisitionRate**: (Optional, default: `50`)
  - **LocalTimeOffsetSec**: (Optional, default: `0`)
  - **ToolReferenceFrame**: (Optional, default: `Tracker`)
  - **SampleFrequencyHz**: The frequency to collect samples.
  - **DelayTimeSec**: The time to delay before processing data
  - **SampleCountPerAcquisition**: (Optional, default: `11350`)
  - **FullScale**: (Optional, default: `1.0`)
  - **Offset**: (Optional, default: `0.0`)
  - **Coupling**: (Optional, default: `3`)
  - **SpeedOfSound**: Used for processing peak distance
  - **Bandwidth**: (Optional, default: `0`)
  - **TrigCoupling**: (Optional, default: `0`)
  - **Slope**: (Optional, default: `0`)
  - **Level**: (Optional, default: `20.0`)
  - **MinPeakDistance**: The minimum allowed distance between detected peaks (Optional, default: `280.0`)

  - **DataSources**: One `DataSource` child element for each entry. (Required)

      - **DataSource**: (Required)
          - **Type**: `Video` (Required)
          - **Id**: Must be `1DSignal` to collect the raw data signal
          - **BufferSize**: (Optional, default: `150`)
          - **AveragedItemsForFiltering**: (Optional, default: `20`)

      - **DataSource**: (Required)
          - **Type**: `Tool` (Required)
          - **Id**: Must be `FirstPeak` to collect the first peak transform
          - **BufferSize**: (Optional, default: `150`)
          - **AveragedItemsForFiltering**: (Optional, default: `20`)

    - **DataSource**: (Required)
        - **Type**: `Tool`
            - **Id**: Must be `SecondPeak` to collect the second peak transform
            - **BufferSize**: (Optional, default: `150`)
            - **AveragedItemsForFiltering**: (Optional, default: `20`)
    - **DataSource**: (Required)
        - **Type**: = `Tool` (Required)
            - **Id**: Must be `ThirdPeak` to collect the third peak transform
            - **BufferSize**: (Optional, default: `150`)
            - **AveragedItemsForFiltering**: (Optional, default: `20`)

## Example configuration file PlusDeviceSet_Server_Agilent.xml

```xml
<PlusConfiguration version="2.1">
  <DataCollection StartupDelaySec="1.0" >
    <DeviceSet
      Name="PlusServer: Broadcast envelope detected Z offset from Agilent scope connected to single element transducer."
      Description="Broadcast envelope detected Z offset from Agilent scope connected to single element transducer." />
    <Device
      Id="DP310"
      Type="AgilentScope"
      SampleFrequencyHz="420000000"
      DelayTimeSec="0.000002"
      SampleCountPerAcquisition="11350"
      FullScale="1.0"
      Offset="0.0"
      Coupling="3"
      SpeedOfSound="1480"
      Bandwidth="0"
      TrigCoupling="0"
      Slope="0"
      Level="20.0"
      ToolReferenceFrame="Tracker">
      <DataSources>
        <DataSource Type="Tool" Id="FirstPeak" />
        <DataSource Type="Tool" Id="SecondPeak" />
        <DataSource Type="Tool" Id="ThirdPeak" />
        <DataSource Type="Video" Id="1DSignal" PortUsImageOrientation="MF"/>
      </DataSources>
      <OutputChannels>
        <OutputChannel Id="ScopeChannel" VideoDataSourceId="1DSignal">
          <DataSource Id="FirstPeak"/>
          <DataSource Id="SecondPeak"/>
          <DataSource Id="ThirdPeak"/>
        </OutputChannel>
      </OutputChannels>
    </Device>
  </DataCollection>

  <PlusOpenIGTLinkServer
    MaxNumberOfIgtlMessagesToSend="1"
    MaxTimeSpentWithProcessingMs="50"
    ListeningPort="18944"
    SendValidTransformsOnly="true"
    OutputChannelId="ScopeChannel" >
    <DefaultClientInfo>
      <MessageTypes>
        <Message Type="TRANSFORM" />
        <Message Type="IMAGE" />
      </MessageTypes>
      <TransformNames>
        <Transform Name="FirstPeakToTracker" />
        <Transform Name="SecondPeakToTracker" />
        <Transform Name="ThirdPeakToTracker" />
      </TransformNames>
      <ImageNames>
        <Image Name="Image" EmbeddedTransformToFrame="Image" />
      </ImageNames>
    </DefaultClientInfo>
  </PlusOpenIGTLinkServer>
</PlusConfiguration>
```