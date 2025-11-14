# Fake tracking system as a simulator
Simulates a fake tracking system with tools that have predetermined behavior. This allows someone who doesn't have access to a tracking system to test code that relies on having one active.

## Device configuration settings

  - **Type**: `FakeTracker`
  - **AcquisitionRate**: (Optional, default: `30`)
  - **LocalTimeOffsetSec**: (Optional, default: `0`)
  - **ToolReferenceFrame**: (Optional, default: `Tracker`)
  - **Mode**: The possible modes have different simulation behaviour: (Optional, default: `Undefined`)
      - `Default` Spins the tools around different axis to fake movement
      - `SmoothMove`
      - `PivotCalibration` Moves around a stylus with the tip fixed to a position
      - `RecordPhantomLandmarks` Touches some positions with 1 sec difference
      - `ToolState` Changes the state of the tool from time to time

  - **PhantomDefinition**: (Required) if **Mode** is `RecordPhantomLandmarks`.
    - **Geometry**: or **Landmarks**
      - **Position**: Landmark 3D position specified as a vector `0.0, 0.0, 0.0`

  - **DataSources**: One `DataSource` child element for each tool. (Required)
      - **DataSource**: (Required)
          - **PortName**: (Required)
              - `0` Reference
              - `1` Stylus
          - **BufferSize**: (Optional, default: `150`)
          - **AveragedItemsForFiltering**: (Optional, default: `20`)

## Example configuration file PlusDeviceSet_FakeTracker_ToolState.xml

```xml
<PlusConfiguration version="2.1">
  <DataCollection StartupDelaySec="1.0">
    <DeviceSet
      Name="TEST No Video with Fake tracker in Tool state mode"
      Description="ToolStateDisplayWidgetTest uses this configuration" />

    <Device
      Id="TrackerDevice"
      Type="FakeTracker"
      AcquisitionRate="50"
      Mode="ToolState" >
      <DataSources>
        <DataSource Type="Tool" Id="Test" PortName="0" />
      </DataSources>
      <OutputChannels>
        <OutputChannel Id="TrackerStream" >
          <DataSource Id="Test"/>
        </OutputChannel>
      </OutputChannels>
    </Device>
  </DataCollection>
</PlusConfiguration>
```