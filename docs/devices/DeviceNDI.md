# NDI Vega, Polaris and Aurora pose trackers

## Supported hardware devices
Any NDI tracking device is supported that uses the common API, such as [Polaris ](<http://www.ndigital.com/medical/products/polaris-family/>) optical and [Aurora ](<http://www.ndigital.com/medical/products/aurora/>) electromagnetic pose trackers.

## Supported platforms
- [***Windows 32 bits***](/SupportedPlatforms/#windows-32-bits)
- [***Windows 64 bits***](/SupportedPlatforms/#windows-64-bits)
- [***Windows XP embedded 32 bits***](/SupportedPlatforms/#windows-xp-embedded-32-bits)
- [***Mac OS X***](/SupportedPlatforms/#macos)
- [***Linux***](/SupportedPlatforms/#linux)


## Installation

- If USB connection is used then drivers should be installed to allow the computer to connect to the system control unit (SCU).
- If you have just turned on the tracker and attempt to use it right away, you may get the error message "command is invalid while in current mode"; wait for the leftmost LED on the position sensor to stop flashing and try again.
- Default settings provide maximum compatibility (work with most trackers), but most of the time they do not provide optimal performance. It is recommended to use maximum **BaudRate** (typically `921600` works well) to have high refresh rate and low latency. Tracking may be also improved by choosing the most appropriate tracking volume by adjusting the **MeasurementVolumeNumber** attribute.

## Device configuration settings

  - **Type**: `NDITracker`
  - **AcquisitionRate**: (Optional, default: `50`)
  - **LocalTimeOffsetSec**: (Optional, default: `0`)
  - **ToolReferenceFrame**: (Optional, default: `Tracker`)
  - **SerialPort**: Parameter must correlate to the com port number used by the NDI SCU Port. This port number was determined during driver installation, to check its value go to control panel->Device Manager->Ports(COM&LPT) and you should see an icon labelled NDI Polaris Spectra SCU Port(COMx), with x representing the COM port number you need to specify in the config file. If SerialPort = -1 or not specified, then probe the first 20 serial ports. (Optional, default: `-1`)
  - **BaudRate**: specifies the speed of the COM port, the recommended value is 115200. Valid values: `9600`, `14400`, `19200`, `38400`, `5760`, `115200`, `921600`, `1228739`. Values of `14400` and `19200` have been known to cause issues and are not recommended (Optional, default: `9600`)
  - **NetworkHostname**: this is the hostname of a network enabled NDI device (NDI Vega). If this attribute is specified, all serial port fucntionality is disabled. (Optional)
  - **NetworkPort**:  the port number for API connections (not the camera port!) (Optional, default: `8765`)
  - **CheckDSR**: whether or not to check the DSR when using a serial connection. (Optional, default: `TRUE`)
  - **MeasurementVolumeNumber**: Measurement volume number. It can be used for defining volume type (dome, cube) and size. First valid volume number is 1. 0 means that the default volume is used. If an invalid value is set (for example -1) then the list of available volumes is logged. See VSEL command in the NDI API documentation for details. (Optional, default: `0`)

  - **DataSources**:
    - **DataSource**: there must be one child tool element (Required)
        - **Type**: `Tool`
        - **Id**: A string that identifies the tool element.
        - **PortName**: For wired tools only (should not be defined for wireless tools). Contains the port number, starting from 0 (e.g., name of the first port is "0", name of the second port is "1"). If multi-channel tools are used then the PortName is constructed as (ChannelNumber)(PortNumber). For example, for two 5-DOF sensors plugged into the first connector: PortName="0" and PortName="100"; for two 5-DOF sensors plugged into the second connector: PortName="1" and PortName="101". (Optional)
        - **RomFile**: For wireless tools only (should not be defined for wired tools, unless the ROM content in the tool has to be overridden). Name of the tool definition file (*.rom file). The file location is relative to the configuration file location. Standard tool rom files are available on the NDI Polaris Spectra Tool Kit cd in the Tool Definition Files folder. (Optional)
        - **BufferSize**: (Optional, default: `150`)
        - **AveragedItemsForFiltering**: (Optional, default: `20`)

## Example configuration file Vega PlusDeviceSet_Server_NDIVega.xml

```xml
<PlusConfiguration version="2.1">

  <DataCollection StartupDelaySec="1.0" >
    <DeviceSet
      Name="PlusServer: NDI Vega tracker with passive markers - by IP"
      Description="Broadcasting tool tracking data through OpenIGTLink
For NDI Vega: Tool (8700339), Stylus (8700340), Reference (8700449)" />
    <Device
      Id="TrackerDevice"
      Type="NDITracker"
      NetworkHostname="192.168.0.30"
      NetworkPort="8765"
      ToolReferenceFrame="Reference" >
      <DataSources>
        <DataSource Type="Tool" Id="Stylus" RomFile="NdiToolDefinitions/8700248.rom"  />
      </DataSources>
      <OutputChannels>
        <OutputChannel Id="TrackerStream" >
          <DataSource Id="Stylus"/>
        </OutputChannel>
      </OutputChannels>
    </Device>

    <Device
      Id="VideoDevice"
      Type="OpenCVVideo"
      VideoURL="rtsp://192.168.0.30/video">
      <DataSources>
        <DataSource Type="Video" Id="Video" ImageType="RGB_COLOR" PortUsImageOrientation="MF"  />
      </DataSources>
      <OutputChannels>
        <OutputChannel Id="VideoStream" VideoDataSourceId="Video" />
      </OutputChannels>
    </Device>

    <Device
      Id="TrackedVideoDevice"
      Type="VirtualMixer" >
      <InputChannels>
        <InputChannel Id="TrackerStream" />
        <InputChannel Id="VideoStream" />
      </InputChannels>
      <OutputChannels>
        <OutputChannel Id="TrackedVideoStream"/>
      </OutputChannels>
    </Device>
  </DataCollection>

  <CoordinateDefinitions>
    <Transform From="Image" To="Reference"
      Matrix="
        1	0	0 0
        0 1 0 0
        0 0 1 0
        0 0 0 1"
       Error="0.554951" Date="012617_105449" />
    <Transform From="StylusTip" To="Stylus"
      Matrix="
        1	0	0.000203823	0.0180449
        3.31529e-09	-1	-1.62655e-05	-0.00144002
        0.000203823	1.62655e-05	-1	-88.5321
        0	0	0	1"
       Error="0.554951" Date="012617_105449" />
  </CoordinateDefinitions>

  <PlusOpenIGTLinkServer
    MaxNumberOfIgtlMessagesToSend="1"
    MaxTimeSpentWithProcessingMs="50"
    ListeningPort="18944"
    SendValidTransformsOnly="true"
    OutputChannelId="TrackedVideoStream" >
    <DefaultClientInfo>
      <MessageTypes>
        <Message Type="IMAGE" />
        <Message Type="TRANSFORM" />
      </MessageTypes>
      <ImageNames>
        <Image Name="Image" EmbeddedTransformToFrame="Reference" />
      </ImageNames>
      <TransformNames>
        <Transform Name="StylusTipToReference" />
      </TransformNames>
    </DefaultClientInfo>
  </PlusOpenIGTLinkServer>

</PlusConfiguration>
```

## Example configuration file Polaris PlusDeviceSet_Server_NDIAurora_NDIPolaris.xml

```xml
<PlusConfiguration version="2.1">

  <DataCollection StartupDelaySec="1.0" >
    <DeviceSet
      Name="PlusServer: NDI Polaris tracker with passive markers"
      Description="Broadcasting tool tracking data through OpenIGTLink
For NDI Polaris passive marker starting kit: Tool (8700339), Stylus (8700340), Reference (8700449)" />
    <Device
      Id="TrackerDevice"
      Type="PolarisTracker"
      ToolReferenceFrame="Tracker" >
      <DataSources>
        <DataSource Type="Tool" Id="Tool" RomFile="NdiToolDefinitions/8700339.rom"  />
        <DataSource Type="Tool" Id="Stylus" RomFile="NdiToolDefinitions/8700340.rom"  />
        <DataSource Type="Tool" Id="Reference" RomFile="NdiToolDefinitions/8700449.rom"  />
      </DataSources>
      <OutputChannels>
        <OutputChannel Id="TrackerStream" >
          <DataSource Id="Tool"/>
          <DataSource Id="Stylus"/>
          <DataSource Id="Reference"/>
        </OutputChannel>
      </OutputChannels>
    </Device>
    <Device
      Id="CaptureDevice"
      Type="VirtualCapture"
      BaseFilename="RecordingTest.igs.nrrd"
      EnableFileCompression="TRUE"
      EnableCapturingOnStart="TRUE" >
      <InputChannels>
        <InputChannel Id="TrackerStream" />
      </InputChannels>
    </Device>
  </DataCollection>

  <CoordinateDefinitions>
    <Transform From="StylusTip" To="Stylus"
      Matrix="
        1	0	0.000203823	0.0180449
        3.31529e-09	-1	-1.62655e-05	-0.00144002
        0.000203823	1.62655e-05	-1	-88.5321
        0	0	0	1"
       Error="0.554951" Date="012617_105449" />
  </CoordinateDefinitions>

  <PlusOpenIGTLinkServer
    MaxNumberOfIgtlMessagesToSend="1"
    MaxTimeSpentWithProcessingMs="50"
    ListeningPort="18944"
    SendValidTransformsOnly="true"
    OutputChannelId="TrackerStream" >
    <DefaultClientInfo>
      <MessageTypes>
        <Message Type="TRANSFORM" />
      </MessageTypes>
      <TransformNames>
        <Transform Name="StylusTipToReference" />
        <Transform Name="ToolToReference" />
        <Transform Name="ToolToTracker" />
        <Transform Name="StylusToTracker" />
        <Transform Name="ReferenceToTracker" />
      </TransformNames>
    </DefaultClientInfo>
  </PlusOpenIGTLinkServer>

</PlusConfiguration>
```

## Example configuration file Aurora PlusDeviceSet_Server_NDIAurora_NDIPolaris.xml

```xml
<PlusConfiguration version="2.1">

  <DataCollection StartupDelaySec="1.0">
    <DeviceSet
      Name="PlusServer: NDI Aurora and Polaris tracker"
      Description="Broadcasting tool tracking data through OpenIGTLink
      Tracking a single EM pointer and an optical pointer and reference." />

    <Device
      Id="EmTracker"
      Type="AuroraTracker"
      SerialPort="5"
      BaudRate="115200"
      AcquisitionRate="20"
      LocalTimeOffsetSec="0.0"
      ToolReferenceFrame="EmTracker" >
      <DataSources>
        <DataSource Type="Tool" Id="Pointer" PortName="0" />
      </DataSources>
      <OutputChannels>
        <OutputChannel Id="EmTrackerStream">
          <DataSource Id="Pointer" />
        </OutputChannel>
      </OutputChannels>
    </Device>

    <Device
      Id="OpticalTracker"
      Type="PolarisTracker"
      SerialPort="6"
      BaudRate="115200"
      AcquisitionRate="20"
      LocalTimeOffsetSec="0.0"
      ToolReferenceFrame="OpticalTracker" >
      <DataSources>
        <DataSource Type="Tool" Id="Stylus" RomFile="NdiToolDefinitions/8700340.rom" />
        <DataSource Type="Tool" Id="Reference" RomFile="NdiToolDefinitions/8700449.rom" />
      </DataSources>
      <OutputChannels>
        <OutputChannel Id="OpticalTrackerStream" >
          <DataSource Id="Stylus"/>
          <DataSource Id="Reference"/>
        </OutputChannel>
      </OutputChannels>
    </Device>

    <Device
      Id="CombinedTracker"
      Type="VirtualMixer" >
      <InputChannels>
        <InputChannel Id="EmTrackerStream" />
        <InputChannel Id="OpticalTrackerStream" />
      </InputChannels>
      <OutputChannels>
        <OutputChannel Id="CombinedTrackerStream"/>
      </OutputChannels>
    </Device>

  </DataCollection>

  <PlusOpenIGTLinkServer
    MaxNumberOfIgtlMessagesToSend="1"
    MaxTimeSpentWithProcessingMs="50"
    ListeningPort="18944"
    SendValidTransformsOnly="true"
    OutputChannelId="CombinedTrackerStream" >
    <DefaultClientInfo>
      <MessageTypes>
        <Message Type="TRANSFORM" />
      </MessageTypes>
      <TransformNames>
        <Transform Name="ReferenceToOpticalTracker" />
        <Transform Name="StylusToOpticalTracker" />
        <Transform Name="PointerToEmTracker" />
      </TransformNames>
    </DefaultClientInfo>
  </PlusOpenIGTLinkServer>

</PlusConfiguration>
```
