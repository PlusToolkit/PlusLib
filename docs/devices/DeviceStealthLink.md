# Medtronic Stealth Station

This class talks with StealthLink Server and acquires the transformation matrix data for both the current frame
and the instrument. Only one instrument and one frame can be active at a time.

Nomenclature:
- Medtronic: localizer = Plus: tracker
- Medtronic: frame = Plus: reference marker (sensor or marker attached to the reference body)
- Medtronic: instrument = Plus: tool marker (sensor or marker attached to the tool)

## Supported hardware devices

- Medtronic StealthStation that has StealthLink licence.
- Link to manufacturer's website: <http://www.medtronic.com/for-healthcare-professionals/products-therapies/spinal/surgical-navigation-imaging/surgical-navigation-systems/systems-software-instruments/>

## Supported platforms

- [***Windows 32 bits***](../SupportedPlatforms.md#windows-32-bits)
- [***Linux***](../SupportedPlatforms.md#linux)

This device is not included in any of the standard packages, as it requires StealthLink library from Medtronic.
If the StealthLink library is obtained from Medtronic then Plus have to be built with the StealthLink option enabled as described at <https://github.com/PlusToolkit/PlusBuild/blob/master/Docs/BuildInstructionsWindows.md>.
The device is tested on Windows, but should also work on other platforms.

## Installation

No need to install any extra software. The StealthStation needs to be connected through LAN and the connection is set via IP address and port number.

## Device configuration settings

- **Type**: `StealthLink`
- **AcquisitionRate**: (Optional, default: `30`)
- **LocalTimeOffsetSec**: (Optional, default: `0`)
- **ToolReferenceFrame**: (Optional, default: `Tracker`)
- **ServerAddress**. IP address of the StealthStation. (Required)
- **ServerPort**. Port number of the StealthStation. (Required)
- **ImageTransferRequiresPatientRegistration**: (Optional, default: `TRUE`)
    - If `TRUE`, the process of downloading an image from the StealthStation will fail if there is no registration.
    - If `FALSE`, the process of downloading an image from the StealthStation will continue using a default transform if there is no registration.
- **DataSources**: One `DataSource` child element is required for each tool or reference marker (Required)
    - **DataSource**: (Required)
        - **BufferSize**: (Optional, default: `150`)
        - **PortName**: (Required)
            - Any instrument name. Instrument names defined in StealthStation must match tools' port names defined in the configuration file (example: `Passive Planar, Blunt`, `Navigus Probe`).
          - Any frame name. Frame names defined in StealthStation must match tools' port names defined in the configuration file (example: `Small Passive Cranial Frame`).
          - `RasRegistration.` Refers to the image's RAS coordinate system (therefore it is recommended to set the Tool Id to `Ras`). This virtual tool will only give valid value when an image is acquired from the server (GetImage or GetExamData).
        - **AlwaysTrack**: (Optional, default: `FALSE`)
            - The StealthStation will normally only track the current tool that is closest to the reference frame.
            - If specified as `TRUE`, this tool will always be tracked, even when it is not the closest tool to the reference frame.
            - The option is `FALSE` by default, since framerate is negatively affected as more tools are always tracked.

## Use cases

Both optical and Em tracker can be used.

### Usage from PlusServerRemoteControl

Start plus server with the configuration file as the input.

    PlusServer.exe --config-file=PlusDeviceSet_Server_StealthLinkTracker.xml

With this you can track the tools that are inside the view of the tracker.

Stealth can receive DICOM image files from the stealth server and send them as volume through OpenIGTLink:

    PlusServerRemoteControl.exe --command=GET_EXAM_DATA --device=SLD --dicom-directory="C:\StealthLinkDicomOutput\" --volumeEmbeddedTransformToFrame="Ras" --keepReceivedDicomFiles

It will get the current exam information and the registration information and send volume and its pose in Ras coordinate system.
Ras is calculated after having read the information from the received dicom files ((0020,0032) ImagePositionPatient and (0020,0037) ImageOrientationPatient).

 - `dicom-directory` you can specifiy a folder and this folder will be where the DICOM files are stored. (Optional, default: plus output directory).
 - `volumeEmbeddedTransformToFrame.` you can define in what coordinate system you want the image to be in `(`Reference`)` or `(`Ras`).` (Optional, default: Ras)
 - `keepReceivedDicomFiles.` If this exists then the DICOM files will be stored if not deleted. The default is delete.

### Usage in 3D Slicer

Start plus server with the configuration file as the input.

    PlusServer.exe --config-file=PlusDeviceSet_Server_StealthLinkTracker.xml

 - Start 3D Slicer
 - Go to `OpenIGTLinkIF` module
 - Click "+" to create a new connection, select the new connection, click (`Client`), click (`Active`)
 - The `StylusToTracker` transform is created in Slicer and it is updated by the stylus tracking. The `ReferenceToTracker` and other transforms are available too.
 - Go to `OpenIGTLinkRemote` module
 - Select the StealthLink connection in the Connector list
 - Click (`Update`) to query the available image
 - Select the image and click "Get selected items" to load it into Slicer

Using OpenIGTLink messages `GET_IMGMETA` and `GET_IMAGE(from` a slicer client for example)

- `GET_IMGMETA` plus will receive all the image meta data information from all of the connected devices and send it through openigtlink
- `GET_IMAGE` will send the volume and the orientation and position in Ras coordinate system.This system should be defined in the configuration file.

## Example configuration file PlusDeviceSet_Server_StealthLinkTracker.xml

```xml
<PlusConfiguration version="2.3">
  <DataCollection StartupDelaySec="1.0" >
    <DeviceSet
      Name="PlusServer: Medtronic StealthLink tracker"
      Description="Broadcasting tool tracking data through OpenIGTLink
Tools: Stylus (Passive Planar, Blunt) , Reference (Small Passive Cranial Frame)"
    />
    <Device
      Id="SLD"
      Type="StealthLinkTracker"
      ServerAddress="130.15.7.88"
      ServerPort="6996"
      RegistrationUpdatePeriod="0.0"
      ToolReferenceFrame="Tracker" >
      <DataSources>
        <!-- Remove tools that are not defined by the StealthStation -->
        <!-- Corresponding tools also need to be removed from the OutputChannels section -->
        <DataSource Type="Tool" Id="Stylus" PortName="Passive Planar, Blunt" />
        <DataSource Type="Tool" Id="NProbe" 	 PortName="Navigus Probe" />
        <DataSource Type="Tool" Id="TProbe" 	 PortName="Touch-n-Go Probe" />
        <DataSource Type="Tool" Id="Reference" PortName="Small Passive Cranial Frame" />
        <!--  The Ras virtual tool will only give a valid value when an image is acquired from the server (GetImage or GetExamData). -->
        <DataSource Type="Tool" Id="Ras" 	    PortName="RasRegistration" />
      </DataSources>
      <OutputChannels>
        <OutputChannel Id="TrackerStream">
          <!-- Remove tools that are not defined by the StealthStation -->
          <DataSource  Id="NProbe" />
          <DataSource  Id="TProbe" />
          <DataSource  Id="Stylus" />
          <DataSource  Id="Reference" />
          <DataSource  Id="Ras" />
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

  <CoordinateDefinitions>
    <Transform From="Ras" To="Lps"
      Matrix="
        -1  0  0  0
         0 -1  0  0
         0  0  1  0
         0  0  0  1" />
  </CoordinateDefinitions>

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
        <!-- To avoid "Transform path not found" warnings, remove transforms for tools that are not defined by the StealthStation -->
        <Transform Name="StylusToTracker" />
        <Transform Name="ReferenceToTracker" />
        <!-- RasToReference will not be available until an image is acquired from the server (GetImage or GetExamData). -->
        <Transform Name="RasToReference"  />
        <Transform Name="StylusToReference"  />
        <Transform Name="NProbeToReference"  />
        <Transform Name="TProbeToReference"  />
      </TransformNames>
    </DefaultClientInfo>
  </PlusOpenIGTLinkServer>

</PlusConfiguration>
```