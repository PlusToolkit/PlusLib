# BK ultrasound systems

## Supported hardware devices

- BK ProFocus, FlexFocus, and various other BK ultrasound imaging devices.
- Image acquisition is available through both the OEM interface (TCP/IP) and CameraLink interface (high-speed interface for RF data acquisition).
- Link to manufacturer's website: <http://www.bkmed.com/systems_en.htm>

## Supported platforms

- [***Windows 32 bits***](../SupportedPlatforms.md#windows-32-bits)
- [***Windows 64 bits***](../SupportedPlatforms.md#windows-64-bits)

- This device is not included in any of the standard packages, as it requires license from BK.
- If the license is obtained from BK then Plus have to be built with PLUS_USE_BKPROFOCUS_VIDEO and optionally with PLUS_USE_BKPROFOCUS_CAMERALINK configuration options enabled as described at <https://github.com/PlusToolkit/PlusBuild/blob/master/Docs/BuildInstructionsWindows.md>.
- The device is tested on Windows, but may also work on other platforms.

## Installation

- Turn off "CRC Check" and "Require Acknowledge" on the BK scanner
- To use **ContinuousStreamingEnabled**: lock image size on the BK scanner (customize dialog -> Split/Size -> check "Lock Image Size" checkbox)
- Limitation of B-mode image acquisition through the OEM interface: the application crashes on disconnect (see [#753](https://plustoolkit.github.io/legacytickets))
- RF acquisition through TeledyneDalsa CameraLink interface card is supported, using Sapera API.
  - B-mode acquisition (using the internal RF->B-mode conversion algorithm) is available for the BK 8848 transducer (<http://www.bkmed.com/8848_en.htm>).
  - Need to install the DALSA Sapera package, otherwise the applications will not start because of missing the SapClassBasic72.dll. This dll can only be used with 64-bit Plus build on 64-bit OS and 32-bit Plus build on a 32-bit OS. Therefore the bitness of the Plus package shall match the bitness of the operating system.

## Device configuration settings

### OEM interface
  - **Type**: `BkProFocusOem` (Required)

  - **AcquisitionRate**: Image retrieval may slow down the exam software, so keep the frame rate low by default (Optional, default: `1`)
  - **LocalTimeOffsetSec**: (Optional, default: `0`)

  - **IniFileName**. INI file name. Is the name of the BK ini file that stores connection and acquisition settings.
  If a relative path is specified then it is relative to the device set configuration directory.
  See an example `inifile` "INI file" below. ** (Required)

  - **ContinuousStreamingEnabled**: Uses continuous image streaming for significantly faster image transfer.
  - TRUE: Use fast image transfer useing GRAB_FRAME command. This feature may require additional license from BK and locking image size on the BK scanner (see in `BkProFocusInstallation` section).
  - FALSE: Request transfer of each frame. The responsiveness of the BK scanner software may be impacted if images are acquired continuously at a frame rate higher than a few frames per second.

  - **DataSources**: Exactly one `DataSource` child element is required. (Required)
    - **DataSource**: (Required)
      - **PortUsImageOrientation**: (Required)
      - **ImageType**: (Optional, default: BRIGHTNESS)
      - **BufferSize**: (Optional, default: 150)
      - **AveragedItemsForFiltering**: (Optional, default: 20)
      - **ClipRectangleOrigin**: (Optional, default: 0 0 0)
      - **ClipRectangleSize**: (Optional, default: 0 0 0)

### RF acquisition through the CameraLink interface

  - **DeviceType**: Must be `BkProFocus`. (Required)
  - **DeviceAcquisitionRate**: (`AcquisitionRate`) (Optional, default: 125)
  - **LocalTimeOffsetSec**: (Optional, default: 0)

  - **IniFileName**: INI file name of the BK ini file that stores connection and acquisition settings. If a relative path is specified then it is relative to the device set configuration directory. See an example `inifile` "INI file" below.
  - **ImagingMode**: Set what kind of images are sent to the video source. The default is RF mode, when the raw RF data is recorded. Applications (  - `BMode`
  - `RfMode` Acquire RF mode data. For previewing B-mode images during acquisition, add an `AlgorithmRfProcessing` (`RfProcessing`) child element to the `OutputChannel.`

  - **SubscribeScanPlane**: Enable support for dynamic scan plane switching on 8848 probes
  - **DataSources**:
    - **DataSource**: There must be one child element. (Required)
      - **PortUsImageOrientation**: (Required)
      - **ImageType**: (Optional, default: BRIGHTNESS)
      - **BufferSize**: (Optional, default: 150)
      - **AveragedItemsForFiltering**: (Optional, default: 20)

\anchor inifile
## Example INI file

```xml
## Example BK ini file IniFile.ini

```ini
[OemToolboxConnection]
OemPort =7915
ToolboxPort =5001
ScannerAddress =10.1.1.100



[RESEARCH_INTERFACE]
;; The research interface creates a virtual com port.  It is use to get status and to set some parameters.
;comport = 3
;baud_rate=9600
;comport = -1
;baud_rate = 9600
comport=-1
baud_rate=9600

comport_name =\\.\X64-CL_Express_1_Serial_0
;comport_name =\\.\X64-CL_Express_1_Serial_0
setup_commands=CLE;LID 1;FLT 15;TAF

;; CCF_FILE is a initialization file used by SapAcquisition for initialization.
;; The path can be either relative or absolute
;; By default the configuration file is in the same directory.
ccf_file=IniFile.ccf
baudRate=9600



[Command And Control]
useToolbox=0
useConsole=1
useConsoleLastOkUseCase=0
autoUpdate=0
## Example configuration file PlusDeviceSet_fCal_BkProFocus_OpenIGTLinkTracker.xml

```xml
<PlusConfiguration version="2.2">

  <DataCollection StartupDelaySec="1.0" >
    <DeviceSet
      Name="fCal: BK ProFocus US CameraLink + OpenIGTLink tracker connection (Probe, Reference)"
      Description="BK ProFocus ultrasound scanner and OpenIGTLink TRANSFORM messages + fCal 1.2. Plus must be built in 64-bit if the operating system is 64-bit."
    />

    <Device
      Id="VideoDevice"
      Type="BkProFocus"
      IniFileName="BkSettings/IniFile.ini"
      AcquisitionRate="30"
      LocalTimeOffsetSec="0"
      ImagingMode="RfMode" >
      <DataSources>
        <DataSource Type="Video" Id="Video" PortUsImageOrientation="FM"  />
      </DataSources>
      <OutputChannels>
        <OutputChannel Id="VideoStream" VideoDataSourceId="Video" >
          <RfProcessing>
            <RfToBrightnessConversion
              NumberOfHilbertFilterCoeffs="32.0"
              BrightnessScale="20.0" />
            <ScanConversion
              TransducerName="BK_8848-axial"
              TransducerGeometry="CURVILINEAR"
              RadiusStartMm="10.0"
              RadiusStopMm="60.0"
              ThetaStartDeg="-70.0"
              ThetaStopDeg="70.0"
              OutputImageSizePixel="600 400"
              TransducerCenterPixel="300 53"
              OutputImageSpacingMmPerPixel="0.15 0.15" />
          </RfProcessing>
        </OutputChannel>
      </OutputChannels>
    </Device>

    <Device
      Id="TrackerDevice"
      Type="OpenIGTLinkTracker"
      MessageType="TRANSFORM"
      ServerAddress="127.0.0.1"
      ServerPort="18944"
      IgtlMessageCrcCheckEnabled="true"
      AcquisitionRate="30"
      LocalTimeOffset="0.0"
      ToolReferenceFrame="Tracker" >
      <DataSources>
        <DataSource Type="Tool" Id="Probe" />
        <DataSource Type="Tool" Id="Reference" />
      </DataSources>
      <OutputChannels>
        <OutputChannel Id="TrackerStream">
          <DataSource Id="Probe" />
          <DataSource Id="Reference" />
        </OutputChannel>
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
    <Transform From="Phantom" To="Reference"
      Matrix="0.9968 0.0075 -0.0795 -35.6089
              0.0794 0.0055 0.9968 -124.9852
              0.0079 -1.0000 0.0049 -16.7
              0 0 0 1"
      Date="2011.12.01 17:57:00" Error="0.0" />
    <Transform From="StylusTip" To="Stylus"
      Matrix="1 0 0 210.0
              0 1 0 0
              0 0 1 0
              0 0 0 1"
      Date="2011.12.01 17:22:12" />
    <Transform From="Image" To="TransducerOriginPixel"
      Matrix="1 0 0 -410
              0 1 0 5
              0 0 1 0
              0 0 0 1"
      Date="2011.12.06 17:57:00" Error="0.0" />
  </CoordinateDefinitions>

  <Rendering WorldCoordinateFrame="Reference">
    <DisplayableObject Type="Model" ObjectCoordinateFrame="TransducerOrigin" Id="ProbeModel"
      File="Probe_L14-5_38.stl"
      ModelToObjectTransform="
        -1 0 0 29.7
        0 -1 0 1.5
        0 0 1 -14
        0 0 0 1" />
    <DisplayableObject Type="Model" ObjectCoordinateFrame="StylusTip" Id="StylusModel" File="Stylus_Example.stl" />
  <DisplayableObject Type="Model" ObjectCoordinateFrame="Reference" Id="Volume"/>
    <DisplayableObject Id="PhantomModel" Type="Model" ObjectCoordinateFrame="Phantom"
      Opacity="0.6"
      File="fCal_1.2.stl"
      ModelToObjectTransform="
        1 0 0 -25.0
        0 0 -1 45.0
        0 1 0 -5.0
        0 0 0 1" />
    <DisplayableObject Type="Image" ObjectCoordinateFrame="Image" Id="LiveImage"/>
  </Rendering>

  <Segmentation
    ApproximateSpacingMmPerPixel="0.078"
    MorphologicalOpeningCircleRadiusMm="0.27"
    MorphologicalOpeningBarSizeMm="2.0"
    ClipRectangleOrigin="27 27"
    ClipRectangleSize="766 562"
    MaxLinePairDistanceErrorPercent="10"
    AngleToleranceDegrees="10"
    MaxAngleDifferenceDegrees="10"
    MinThetaDegrees="-70"
    MaxThetaDegrees="70"
    MaxLineShiftMm="10.0"
    ThresholdImagePercent="10"
    CollinearPointsMaxDistanceFromLineMm="0.6"
    UseOriginalImageIntensityForDotIntensityScore="FALSE"
    NumberOfMaximumFiducialPointCandidates="20" />

  <PhantomDefinition>
    <Description
      Name="fCAL"
      Type="Multi-N"
      Version="1.2"
      WiringVersion="1.1"
      Institution="Queen's University PerkLab" />
    <Geometry>
      <Pattern Type="NWire">
        <Wire Name="1:E2_e2" EndPointFront="20.0 0.0 10.0" EndPointBack="20.0 40.0 10.0" />
        <Wire Name="2:I2_f2" EndPointFront="40.0 0.0 10.0" EndPointBack="25.0 40.0 10.0" />
        <Wire Name="3:J2_j2" EndPointFront="45.0 0.0 10.0" EndPointBack="45.0 40.0 10.0" />
      </Pattern>
      <Pattern Type="NWire">
        <Wire Name="4:E3_e3" EndPointFront="20.0 0.0 5.0" EndPointBack="20.0 40.0 5.0" />
        <Wire Name="5:F3_j3" EndPointFront="25.0 0.0 5.0" EndPointBack="45.0 40.0 5.0" />
        <Wire Name="6:K3_k3" EndPointFront="50.0 0.0 5.0" EndPointBack="50.0 40.0 5.0" />
      </Pattern>
      <Pattern Type="NWire">
        <Wire Name="7:E4_e4" EndPointFront="20.0 0.0 0.0" EndPointBack="20.0 40.0 0.0" />
        <Wire Name="8:J4_f4" EndPointFront="45.0 0.0 0.0" EndPointBack="25.0 40.0 0.0" />
        <Wire Name="9:K4_k4" EndPointFront="50.0 0.0 0.0" EndPointBack="50.0 40.0 0.0" />
      </Pattern>
      <Landmarks>
        <Landmark Name="#1" Position="95.0 5.0 15.0" />
        <Landmark Name="#2" Position="95.0 40.0 15.0" />
        <Landmark Name="#3" Position="95.0 40.0 0.0" />
        <Landmark Name="#4" Position="95.0 0.0 0.0" />
        <Landmark Name="#5" Position="-25.0 40.0 15.0" />
        <Landmark Name="#6" Position="-25.0 0.0 10.0" />
        <Landmark Name="#7" Position="-25.0 0.0 0.0" />
        <Landmark Name="#8" Position="-25.0 40.0 0.0" />
      </Landmarks>
    </Geometry>
  </PhantomDefinition>

  <VolumeReconstruction OutputSpacing="0.5 0.5 0.5"
    ClipRectangleOrigin="0 0" ClipRectangleSize="820 616"
    Interpolation="LINEAR" Optimization="FULL" CompoundingMode="MEAN" FillHoles="OFF" />

  <fCal
    PhantomModelId="PhantomModel"
    ReconstructedVolumeId="Volume"
    TransducerModelId="ProbeModel"
    StylusModelId="StylusModel"
    ImageDisplayableObjectId="LiveImage"
    NumberOfCalibrationImagesToAcquire="200"
    NumberOfValidationImagesToAcquire="100"
    NumberOfStylusCalibrationPointsToAcquire="200"
    RecordingIntervalMs="100"
    MaxTimeSpentWithProcessingMs="70"
    ImageCoordinateFrame="Image"
    ProbeCoordinateFrame="Probe"
    ReferenceCoordinateFrame="Reference"
    TransducerOriginCoordinateFrame="TransducerOrigin"
    TransducerOriginPixelCoordinateFrame="TransducerOriginPixel"
    TemporalCalibrationDurationSec="10"
    FixedChannelId="VideoStream"
    FixedSourceId="Video"
    MovingChannelId="TrackerStream"
    MovingSourceId="ProbeToTracker"
    DefaultSelectedChannelId="TrackedVideoStream" />

  <vtkPlusPivotCalibrationAlgo
    ObjectMarkerCoordinateFrame="Stylus"
    ReferenceCoordinateFrame="Reference"
    ObjectPivotPointCoordinateFrame="StylusTip" />

  <vtkPlusPhantomLandmarkRegistrationAlgo
    PhantomCoordinateFrame="Phantom"
    ReferenceCoordinateFrame="Reference"
    StylusTipCoordinateFrame="StylusTip" />

  <vtkPhantomLinearObjectRegistrationAlgo
    PhantomCoordinateFrame="Phantom"
    ReferenceCoordinateFrame="Reference"
    StylusTipCoordinateFrame="StylusTip" />

  <vtkPlusProbeCalibrationAlgo
    ImageCoordinateFrame="Image"
    ProbeCoordinateFrame="Probe"
    PhantomCoordinateFrame="Phantom"
    ReferenceCoordinateFrame="Reference" />

</PlusConfiguration>
```