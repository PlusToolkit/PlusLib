# Claron MicronTracker optical pose tracker

## Supported hardware devices

- Link to manufacturer's website: <http://www.clarontech.com/microntracker.php>

## Supported platforms

- [***Windows 32 bits***](../SupportedPlatforms.md#windows-32-bits)

The device manufacturer supports additional platforms. On request, Plus toolkit will be extended to support this device on other platforms, too.

## Installation
- Each tool is identified by its unique marker pattern - by the number and relative position of the constituting "x-marks".
  MicronTracker's CPPDemo tool (included in the MicronTracker software package) can identify a marker and
  create a marker descriptor "template" file. All the template files must be placed into the "TemplateDirectory" specified in the configuration file.
- If Plus does not find any tool then run CPPDemo and verify that the marker is in the field of view and recognizable.
- The 1394 interface that your computer already has may not work with the camera. If the MicronTracker demo programs don't work then use the interface card that came with your MicronTracker package.

## Device configuration settings

- **Type**: `MicronTracker` (Required)
- **IniFile**: `MicronTracker.ini` Path to the initialization file.  Relative to `FileApplicationConfiguration` (`DeviceSetConfigurationDirectory`).
- **TemplateDirectory**: `Markers` Path to the directory that contains the marker files. Relative to `FileApplicationConfiguration` (`DeviceSetConfigurationDirectory`).
- **AcquisitionRate**: (Optional, default: `20`)
- **LocalTimeOffsetSec**: (Optional, default: `0`)
- **ToolReferenceFrame**: (Optional, default: `Tracker`)
- **DataSources**: Exactly one `DataSource` child element is required. (Required)
    - **DataSource**: (Required)
        - **PortName**: Name of the template file that describes the marker's geometry. The file is typically created by CPPDemo. (Required)
        - **BufferSize**: (Optional, default: `150`)
        - **AveragedItemsForFiltering**: (Optional, default: `20`)

## Example configuration file PlusDeviceSet_Server_MicronTracker.xml

```xml
<PlusConfiguration version="2.3">

  <DataCollection StartupDelaySec="1.0" >
    <DeviceSet
      Name="PlusServer: Claron MicronTracker tracker"
      Description="Broadcasting tool tracking data through OpenIGTLink
Tools: Probe (1b), Reference (TTBlock), Stylus (2b) tools"
    />
    <Device
      Id="TrackerDevice"
      Type="MicronTracker"
      TemplateDirectory="Markers"
      IniFile="MicronTracker.ini"
      ToolReferenceFrame="Tracker" >
      <DataSources>
        <DataSource Type="Tool" Id="Probe" PortName="1b" />
        <DataSource Type="Tool" Id="Reference"  PortName="TTblock" />
        <DataSource Type="Tool" Id="Stylus" PortName="2b" />
      </DataSources>
      <OutputChannels>
        <OutputChannel Id="TrackerStream" >
          <DataSource Id="Probe"/>
          <DataSource Id="Reference"/>
          <DataSource Id="Stylus"/>
        </OutputChannel>
      </OutputChannels>
    </Device>
    <Device
      Id="CaptureDevice"
      Type="VirtualCapture"
      BaseFilename="RecordingTest.igs.mha"
      EnableCapturingOnStart="FALSE" >
      <InputChannels>
        <InputChannel Id="TrackerStream" />
      </InputChannels>
    </Device>
  </DataCollection>

  <PlusOpenIGTLinkServer
    MaxNumberOfIgtlMessagesToSend="1"
    MaxTimeSpentWithProcessingMs="50"
    ListeningPort="18944"
    OutputChannelId="TrackerStream" >
    <DefaultClientInfo>
      <MessageTypes>
        <Message Type="TRANSFORM" />
      </MessageTypes>
      <TransformNames>
        <Transform Name="ProbeToTracker" />
        <Transform Name="ReferenceToTracker" />
        <Transform Name="StylusToTracker" />
      </TransformNames>
    </DefaultClientInfo>
  </PlusOpenIGTLinkServer>

</PlusConfiguration>
```