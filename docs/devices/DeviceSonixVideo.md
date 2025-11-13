# Ultrasonix ultrasound systems

## Supported hardware devices

- Supported models: Ultrasonix SonixRP, SonixMDP, SonixTouch, SonixTablet, etc.
- As all models use the same research interface, there is no difference in using Plus with any of the Ultrasonix scanner models (e.g., a sample config file for SonixRP works well for a SonixMDP scanner). However, the ultrasound firmware and Exam software on the scanner has to match the Ultrasonix SDK version that Plus uses. Plus with Ultrasonix SDK 5.7.x works through Ulterius interface with Exam software 5.7.x-6.0.2. Plus with Ultrasonix SDK 6.1.x works through Ulterius interface with Exam software 6.0.7 and later. Use the `ApplicationPlusVersion` to find out which Ultrasonix SDK version is used by Plus.
- If a probe with built-in tracking sensor is used (L14-5/38 GPS or C5-2/60 GPS) then Plus can use the probe calibration information provided by the manufacturer. Select the configuration file: "PlusServer: Ultrasonix US ... - calibrated".
- Supported interfaces: Ulterius and Porta.
- Link to manufacturer's website: <http://www.analogicultrasound.com/ultrasonix>

## Supported platforms
- [***Windows 32 bits***](../SupportedPlatforms.md#windows-32-bits)
- [***Windows XP embedded 32 bits***](../SupportedPlatforms.md#windows-xp-embedded-32-bits)

## Installation

- If tracking does not work (you get "No BIRDs were found anywhere" or similar error) make sure that all the Ultrasonix Exam software features that use SonixGPS are disabled (in Administration / Licenses).
- If tracking does not work then try to copy the Ascension tracker DLL file (ATC3DG.dll) from the Ultrasonix exam software directory to the Plus directory (as the DLL version that is shipped with Plus may not match the installed tracker hardware/software version).
- The Ultrasonix L14-5 linear probe does not work well with the NDI Aurora electromagnetic tracker. If the L14 probe is stationary, we can get consistent readings, but if the probe is moving, the tracking is not stable at all.  We know there is a big piece of aluminium inside L14-5. Thus I would advise not to use Aurora with Ultrasonix probe at this time. (Reported by Elvis Chen, Feb 29, 2012) Note that Ascension electromagnetic trackers work just fine even if they are very close to the transducer.
- The B-mode image processing (scan conversion, de-speckle filtering, etc.) is performed in the CPU. If higher frame rates are needed and speckle is tolerable then it is advisable to turn off de-speckle filtering.
- Some users observed slow volume reconstruction when running Exam software 6.1 due the Exam software using all CPU resources. A workaround is to raise the priority of PlusServer in the Windows Task Manager. A setting (`high`) already provides acceptable user experience and no loss of data. Updating to exam software 6.1.1 may also improve performance.
- If there is horizontal striping/repetition of image contents (typically occurs in water tank), then set the number of focal points to 1. It usually removes the artifact. (Reported by Andras Lasso, Nov 15, 2012; Exam software version 5.7)
- Ulterius interface (SonixVideo device): Requires the Ultrasonix Exam software to be running. Plus may run either on the Ultrasonix system or on a different computer (with network connection to the Ultrasonix system).
- Porta interface (SonixPortaVideo device): Plus must run on the Ultrasonix system and the Exam software must not be running. Controlling of the motorized probe is supported. 3D/4D probe has to be connected to port-0

Notes:
- When the "Current exam has not been assigned to a patient. would you like to assign the exam to a patient record?" dialog box is displayed then the freeze button is unable to unfreeze the image acquisition on the ultrasound machine. Close the dialog box and press the freeze button to enable image acquisition.

## Device configuration settings

### Common attributes:

- **Device**
    - **AcquisitionRate**: (Optional, default: `30`)
    - **LocalTimeOffsetSec**: (Optional, default: `0`)
    - **Timeout**: Timeout `[ms]` for receiving new frames by network function calls. If not specified then the current setting is used.
    - **UsImagingParameters**: Imaging parameters container (Required)
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
    - **DataSources**: Multiple `DataSource` child elements are allowed, one for each data type (`PortName` indicates the data type) (Required)
        - **DataSource**: (Required)
            - **PortName**: (Required)
                - `B` B-mode image data
                - `Rf` RF data
            - **PortUsImageOrientation**: (Required)
            - **ImageType**: (Optional, default: `BRIGHTNESS`)
            - **BufferSize**: (Optional, default: `150`)
            - **AveragedItemsForFiltering**: (Optional, default: `20`)
            - **ClipRectangleOrigin**: (Optional, default: `0 0 0`)
            - **ClipRectangleSize**: (Optional, default: `0 0 0`)

### Ulterius interface specific Device attributes:

  - **Type**: `SonixVideo`
  - **IP**: Host name or IP address of the Ultrasound system. If Plus runs on the Ultrasound system then the 127.0.0.1 address can be used.
  - **AutoClipEnabled**: Automatically clip the image to the region of interest specified by the ultrasound system.
      - `TRUE`: Image will be clipped to the region of interest that was specified at the time when the connection to the device was established. The image size depends on the imaging depth.
      - `FALSE`: Regardless of imaging depth, zoom, etc. settings, the image size is always the same. Black border appears outside the region of interest.
  - **ImageToTransducerTransformName**: If specified then Image To Transducer transform is added as a custom field to the image data with the specified name
  - **SoundVelocity**: The speed of sound `[m/s].`  The minimum and maximum allowed value is specified on the Ultrasound computer, in the Exam software directory in the `imaging.set.xml` file
  - **DynRange**: Dynamic range `[dB]` of B-mode ultrasound. If not specified the current value is kept.
  - **Sector**: Sector `[%]` of B-mode ultrasound; valid range: 0-100. If not specified the current value is kept. (Optional, default:  )
  - **CompressionStatus**: When imaging with the EC59 probe, if compression is disabled
  - **TimeGainCompensation**: Time gain compensation `[8` int values] If not specified then the current setting is used.
  - **SharedMemoryStatus**: When this flag is enabled
  - **ConnectionSetupDelayMs**: Time required for setting up the connection. The value depends on the probe type, typical values are between 2000-3000 `[ms].`
  - **ImageGeometryOutputEnabled**: Add image geometry information Porta interface specific Device attributes:

### Porta interface specific Device attributes:

- **Type**: `SonixPortaVideo`
- **ImagingMode**: Only BMode supported.
- **FramePerVolume** Number of frames that will acquired in one sweep of the motor. Range: 3-125 (always odd as there always is a central frame). (Optional, default: 80)
- **BModeFrameSize**: Size of the BMode frame buffer `[pixels]`
- **StepPerFrame**: The number of steps the motor will move while acquiring a single frame. Range: 2, 4, 8, 16, 32. Larger value results in faster scanning. (Optional, default: `4`)
- **Usm**: Ultrasound module version. Typical values: 2, 3, 4. If an incorrect value is set then Porta initialization will fail, therefore the Usm and Pci values can be determined by trial and error. (Required)
- **Pci**: PCI card version.  Typical values: 2, 3, 4. If an incorrect value is set then Porta initialization will fail, therefore the Usm and Pci values can be determined by trial and error. (Required)
- **HighVoltage**: Set it to 1 in case of a high-voltage board is used
- **Channels**: Porta initialization parameter. Typical value is (`64`).
- **PortaLUTPath**: Path to the Porta LUT directory. Typical value: "D:/Ultrasonix Settings/LUTS/" (Required)
- **PortaSettingPath**: Path to the Porta Setting directory. Example value: "D:/sdk/porta/dat/" (Required)
- **PortaLicensePath**: Path to the licenses text file. Example value: "D:/Ultrasonix Settings/" (Required)
- **PortaFirmwarePath**: Path to the Porta Firmware directory. Example value: "D:/sdk/porta/fw/" (Required)

## Example config file

```xml
<PlusConfiguration version="2.1">

  <DataCollection StartupDelaySec="1.0" >
    <DeviceSet
      Name="PlusServer: Ultrasonix US (L14-5/38 GPS probe) + Ascension3DG tracker (Probe, Reference, Stylus) - calibrated"
      Description="Broadcasting ultrasound images acquired from the Ultrasonix system through OpenIGTLink. Probe is spatially calibrated for any imaging depth. If PlusServer does not run on the Ultrasonix PC then update the IP attribute in the Device element with the Ultrasonix PC's IP address. Ascension3DG sensors should be plugged in to the Ascension3DG DriveBay mounted on Ultrasonix US in the following order from to leftmost slot (Transducer 1) to the right: 1 Probe, 2 Reference, 3 Stylus." />
    <Device
      Id="TrackerDevice"
      Type="Ascension3DG"
      LocalTimeOffsetSec="0.0"
      FilterAcWideNotch="1"
      ToolReferenceFrame="Tracker" >
      <DataSources>
        <DataSource Type="Tool" Id="Probe" PortName="0"  />
        <DataSource Type="Tool" Id="Reference" PortName="1"  />
        <DataSource Type="Tool" Id="Stylus" PortName="2"  />
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
      Id="VideoDevice"
      Type="SonixVideo"
      AcquisitionRate="30"
      LocalTimeOffsetSec="-0.2976"
      IP="127.0.0.1"
      AutoClipEnabled="TRUE"
      ImageToTransducerTransformName="ImageToTransducer" >
      <DataSources>
        <DataSource Type="Video" Id="Video" PortName="B" PortUsImageOrientation="UF"  />
      </DataSources>
      <OutputChannels>
        <OutputChannel Id="VideoStream" VideoDataSourceId="Video"/>
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
    <Device
      Id="CaptureDevice"
      Type="VirtualCapture"
      BaseFilename="Recording.igs.mhd"
      EnableCapturingOnStart="FALSE"
      RequestedFrameRate="15" >
      <InputChannels>
        <InputChannel Id="TrackedVideoStream" />
      </InputChannels>
    </Device>
    <Device
      Id="VolumeReconstructorDevice"
      Type="VirtualVolumeReconstructor"
      OutputVolDeviceName="Volume_Reference"
      EnableReconstruction="FALSE" >
      <InputChannels>
        <InputChannel Id="TrackedVideoStream" />
      </InputChannels>
      <VolumeReconstruction
        ImageCoordinateFrame="Image" ReferenceCoordinateFrame="Reference"
        CompoundingMode="MEAN" Interpolation="LINEAR"
        Optimization="FULL" NumberOfThreads="2"
        ClipRectangleOrigin="0 0" ClipRectangleSize="820 616" PixelRejectionThreshold="1"
        OutputSpacing="0.5 0.5 0.5"
        FillHoles="ON" >
        <HoleFilling>
          <HoleFillingElement Type="GAUSSIAN" Size="5" Stdev="0.6667" MinimumKnownVoxelsRatio="0.50001" />
          <HoleFillingElement Type="STICK" StickLengthLimit="9" NumberOfSticksToUse="1" />
        </HoleFilling>
      </VolumeReconstruction>
    </Device>
  </DataCollection>

  <CoordinateDefinitions>
    <!-- TransducerToProbe transform matrix is computed from calibration matrices provided by Ultrasonix for linear GPS probe -->
    <Transform From="Transducer" To="Probe"
      Matrix="
        0.0018    0.9477   -0.0175   14.8449
       -1.0000    0.0016   -0.0052   15.0061
       -0.0052    0.0166    0.9998    0.1638
             0         0         0    1.0000" />
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
      <TransformNames>
        <Transform Name="ProbeToReference" />
        <Transform Name="StylusToReference" />
        <Transform Name="ReferenceToTracker" />
      </TransformNames>
      <ImageNames>
        <Image Name="Image" EmbeddedTransformToFrame="Reference" />
      </ImageNames>
    </DefaultClientInfo>
  </PlusOpenIGTLinkServer>

</PlusConfiguration>
```

## Example configuration file for tracked ultrasound acquisition using calibration matrix provided by Ultrasonix for C5-2/60 GPS probe

The calibration remains valid even if the imaging depth is changed.

```xml
<PlusConfiguration version="2.1">

  <DataCollection StartupDelaySec="1.0" >
    <DeviceSet
      Name="PlusServer: Ultrasonix US (C5-2/60 GPS probe) + Ascension3DG tracker (Probe, Reference, Stylus) - calibrated"
      Description="Broadcasting ultrasound images acquired from the Ultrasonix system through OpenIGTLink. Probe is spatially calibrated for any imaging depth. If PlusServer does not run on the Ultrasonix PC then update the IP attribute in the Device element with the Ultrasonix PC's IP address. Ascension3DG sensors should be plugged in to the Ascension3DG DriveBay mounted on Ultrasonix US in the following order from to leftmost slot (Transducer 1) to the right: 1 Probe, 2 Reference, 3 Stylus." />
    <Device
      Id="TrackerDevice"
      Type="Ascension3DG"
      LocalTimeOffsetSec="0.0"
      FilterAcWideNotch="1"
      ToolReferenceFrame="Tracker" >
      <DataSources>
        <DataSource Type="Tool" Id="Probe" PortName="0"  />
        <DataSource Type="Tool" Id="Reference" PortName="1"  />
        <DataSource Type="Tool" Id="Stylus" PortName="2"  />
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
      Id="VideoDevice"
      Type="SonixVideo"
      AcquisitionRate="30"
      LocalTimeOffsetSec="-0.2976"
      IP="127.0.0.1"
      AutoClipEnabled="TRUE"
      ImageToTransducerTransformName="ImageToTransducer" >
      <DataSources>
        <DataSource Type="Video" Id="Video" PortName="B" PortUsImageOrientation="UF"  />
      </DataSources>
      <OutputChannels>
        <OutputChannel Id="VideoStream" VideoDataSourceId="Video"/>
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
    <Device
      Id="CaptureDevice"
      Type="VirtualCapture"
      BaseFilename="Recording.igs.mhd"
      EnableCapturingOnStart="FALSE"
      RequestedFrameRate="15" >
      <InputChannels>
        <InputChannel Id="TrackedVideoStream" />
      </InputChannels>
    </Device>
    <Device
      Id="VolumeReconstructorDevice"
      Type="VirtualVolumeReconstructor"
      OutputVolDeviceName="Volume_Reference"
      EnableReconstruction="FALSE" >
      <InputChannels>
        <InputChannel Id="TrackedVideoStream" />
      </InputChannels>
      <VolumeReconstruction
        ImageCoordinateFrame="Image" ReferenceCoordinateFrame="Reference"
        CompoundingMode="MEAN" Interpolation="LINEAR"
        Optimization="FULL" NumberOfThreads="2"
        ClipRectangleOrigin="0 0" ClipRectangleSize="820 616" PixelRejectionThreshold="1"
        OutputSpacing="0.5 0.5 0.5"
        FillHoles="ON" >
        <HoleFilling>
          <HoleFillingElement Type="GAUSSIAN" Size="5" Stdev="0.6667" MinimumKnownVoxelsRatio="0.50001" />
          <HoleFillingElement Type="STICK" StickLengthLimit="9" NumberOfSticksToUse="1" />
        </HoleFilling>
      </VolumeReconstruction>
    </Device>
  </DataCollection>

  <CoordinateDefinitions>
    <!-- TransducerToProbe transform matrix is computed from calibration matrices provided by Ultrasonix for convex GPS probe -->
    <Transform From="Transducer" To="Probe"
      Matrix="
        0.0034    0.9433   -0.0140  -29.7558
       -1.0000    0.0033    0.0087   -0.0870
        0.0087    0.0132    0.9999   -0.7053
             0         0         0    1.0000" />
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
      <TransformNames>
        <Transform Name="ProbeToReference" />
        <Transform Name="StylusToReference" />
        <Transform Name="ReferenceToTracker" />
      </TransformNames>
      <ImageNames>
        <Image Name="Image" EmbeddedTransformToFrame="Reference" />
      </ImageNames>
    </DefaultClientInfo>
  </PlusOpenIGTLinkServer>

</PlusConfiguration>
```

## Example configuration file for B-mode acquisition using Ulterius interface

```xml
<PlusConfiguration version="2.1">

  <DataCollection StartupDelaySec="1.0" >
    <DeviceSet
      Name="PlusServer: Ultrasonix ultrasound imaging device"
      Description="Broadcasting ultrasound images acquired from the Ultrasonix system through OpenIGTLink. If PlusServer does not run on the Ultrasonix PC then update the IP attribute in the Device element with the Ultrasonix PC's IP address."
    />
    <Device
      Id="VideoDevice"
      Type="SonixVideo"
      AcquisitionRate="30"
      AutoClipEnabled="TRUE"
      ImageGeometryOutputEnabled="TRUE"
      ImageToTransducerTransformName="ImageToTransducer"
      IP="127.0.0.1" >
      <DataSources>
        <DataSource Type="Video" Id="Video" PortName="B" PortUsImageOrientation="UF"  />
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
        <Message Type="STRING" />
      </MessageTypes>
      <ImageNames>
        <Image Name="Image" EmbeddedTransformToFrame="Transducer" />
      </ImageNames>
      <StringNames>
        <String Name="DepthMm" />
        <String Name="PixelSpacingMm" />
        <String Name="TransducerOriginPix" />
      </StringNames>
    </DefaultClientInfo>
  </PlusOpenIGTLinkServer>

</PlusConfiguration>
```

## Example configuration file for 3D B-mode acquisition by a motorized probe using Porta interface

```xml
<PlusConfiguration version="2.1">

  <DataCollection StartupDelaySec="1.0" >
    <DeviceSet
      Name="PlusServer: Ultrasonix US (4DL14-5/38 motorized probe) - calibrated"
      Description="Broadcasting ultrasound images acquired from the Ultrasonix system through OpenIGTLink. Update PortaFirmwarePath, PortaSettingPath, PortaLicensePath, PortaLUTPath attributes in the device set configuration file. PlusServer has to run on the Ultrasonix PC. The motorized probe has to be plugged into the first probe connector."
    />
    <Device
      Id="VideoDevice"
      Type="SonixPortaVideo"
      LocalTimeOffsetSec="0"
      PortaBModeWidth="380"
      PortaBModeHeight="400"
      Depth="40"
      Gain="50"
      Frequency="10000000"
      ImageToTransducerTransformName="ImageToTransducer"
      MotorRotationRangeDeg="29"
      StepPerFrame="4"
      PortaFirmwarePath="D:/devel/PLTools/Ultrasonix/sdk-6.1.1/porta/fw/"
      PortaSettingPath="D:/devel/PLTools/Ultrasonix/sdk-6.1.1/porta/dat/"
      PortaLicensePath="D:/Ultrasonix Settings/"
      PortaLUTPath="D:/Ultrasonix Settings/LUTS/"
      Usm="4"
      Pci="3"
      >
      <DataSources>
        <DataSource Type="Video" Id="Video" PortName="B" PortUsImageOrientation="UF" />
      </DataSources>
      <OutputChannels>
        <OutputChannel Id="VideoStream" VideoDataSourceId="Video" />
      </OutputChannels>
    </Device>

    <Device
      Id="CaptureDevice"
      Type="VirtualCapture"
      RequestedFrameRate="100"
      EnableCapturing="FALSE" >
      <InputChannels>
        <InputChannel Id="VideoStream" />
      </InputChannels>
    </Device>

    <Device
      Id="VolumeReconstructorDevice"
      Type="VirtualVolumeReconstructor"
      OutputVolDeviceName="RecVol">
      <InputChannels>
        <InputChannel Id="VideoStream" />
      </InputChannels>
      <VolumeReconstruction
        ImageCoordinateFrame="Image" ReferenceCoordinateFrame="Ras"
        Optimization="FULL" CompoundingMode="MEAN" FillHoles="OFF"
        Interpolation="NEAREST_NEIGHBOR" NumberOfThreads="4"
        ClipRectangleOrigin="50 0" ClipRectangleSize="285 295"
        OutputOrigin="-55 -110 -62" OutputExtent="0 220 0 180 0 248" OutputSpacing="0.5 0.5 0.5" >
        <HoleFilling>
          <HoleFillingElement Type="GAUSSIAN" Size="3" Stdev="0.6667" MinimumKnownVoxelsRatio="0.1" />
        </HoleFilling>
      </VolumeReconstruction>
    </Device>

  </DataCollection>

  <CoordinateDefinitions>
    <Transform From="Ras" To="Motor"
      Matrix="
      1 0 0 0
      0 1 0 0
      0 0 1 0
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
        <Message Type="TRANSFORM" />
      </MessageTypes>
      <TransformNames>
        <Transform Name="TransducerToRas" />
        <Transform Name="MotorToRas" />
      </TransformNames>
      <ImageNames>
        <Image Name="Image" EmbeddedTransformToFrame="Ras" />
      </ImageNames>
    </DefaultClientInfo>
  </PlusOpenIGTLinkServer>

</PlusConfiguration>
```

## Example configuration file for dual B-mode and RF-mode acquisition using Ulterius interface

```xml
<PlusConfiguration version="2.3">

  <DataCollection StartupDelaySec="1.0" >
    <DeviceSet
      Name="TEST Ultrasonix US with Fake tracker - B and Rf"
      Description="Test mode for vtkDataCollectorTest with Ultrasonix US and Fake tracker. The Ultrasonix US device should be in research mode." />

    <Device
      Id="TrackerDevice"
      Type="FakeTracker"
      AcquisitionRate="30"
      Mode="PivotCalibration"
      ReferenceToolName="Reference" >
      <DataSources>
        <DataSource Type="Tool" Id="Reference"  PortName="0"/>
        <DataSource Type="Tool" Id="Stylus"  PortName="1" />
      </DataSources>
      <OutputChannels>
        <OutputChannel Id="TrackerStream">
          <DataSource Id="Reference"/>
          <DataSource Id="Stylus"/>
        </OutputChannel>
      </OutputChannels>
    </Device>

    <Device
      Id="VideoDevice"
      Type="SonixVideo"
      AcquisitionRate="-1"
      IP="130.15.7.20" >
      <DataSources>
        <DataSource Type="Video" Id="BModeVideo" PortName="B" PortUsImageOrientation="MF"  />
        <DataSource Type="Video" Id="RfModeVideo" PortName="Rf" PortUsImageOrientation="FM" />
      </DataSources>
      <OutputChannels>
        <OutputChannel Id="BModeVideoStream" VideoDataSourceId="BModeVideo" />
        <OutputChannel Id="RfModeVideoStream" VideoDataSourceId="RfModeVideo" />
      </OutputChannels>
    </Device>

    <Device
      Id="TrackedBVideoDevice"
      Type="VirtualMixer" >
      <InputChannels>
        <InputChannel Id="TrackerStream" />
        <InputChannel Id="BModeVideoStream" />
      </InputChannels>
      <OutputChannels>
        <OutputChannel Id="TrackedBModeVideoStream" />
      </OutputChannels>
    </Device>

    <Device
      Id="TrackedRfVideoDevice"
      Type="VirtualMixer" >
      <InputChannels>
        <InputChannel Id="TrackerStream" />
        <InputChannel Id="RfModeVideoStream" />
      </InputChannels>
      <OutputChannels>
        <OutputChannel Id="TrackedRfModeVideoStream" />
      </OutputChannels>
    </Device>
  </DataCollection>

  <fCal
    DefaultSelectedChannelId="TrackedBModeVideoStream"
    />

</PlusConfiguration>
```

## Using calibration matrices provided by Ultrasonix

Ultrasonix provides calibration matrices for its GPS probes:
<http://www.ultrasonix.com/wikisonix/index.php/GPS_Data_Collection>

TransducerToProbe transform matrix in Plus-compatible format can be determined using these computations (script executable in Matlab):

```matlab
    function transducerToProbe = getTransducerToProbeTransform(transducerToProbeUltrasonix)
    % Get TransducerToProbeTransform matrix from Ultrasonix probe calibration matrix

    % Ultrasonix uses [1yx] coordinates (first column is origin, second column is Y axis, third column is X axis)

    % Transducer's origin in the Probe sensor coordinate system
    transducerOrigin_Probe = [transducerToProbeUltrasonix(1,1); transducerToProbeUltrasonix(2,1); transducerToProbeUltrasonix(3,1)];

    % Transducer's X, Y, Z axes in the Probe sensor coordinate system:
    % (Plus's transducer X coordinate system axis points to M direction, while
    % in Ultrasonix it is the opposite, U direction, therefore the minus sign
    % at the transducerX_Probe computation)
    transducerX_Probe = -[transducerToProbeUltrasonix(1,3); transducerToProbeUltrasonix(2,3); transducerToProbeUltrasonix(3,3)];
    transducerY_Probe = [transducerToProbeUltrasonix(1,2); transducerToProbeUltrasonix(2,2); transducerToProbeUltrasonix(3,2)];

    % We do not want the TransducerToProbe matrix to be a projection matrix
    % because that cannot be inverted and cannot be used for rendering 3D
    % objects (a projection matrix would flatten everything to the image plane).
    % Therefore, we compute the Z axis as the cross product of the X and Y vector.
    transducerZ_Probe = cross(transducerX_Probe, transducerY_Probe);
    transducerZ_Probe = transducerZ_Probe/norm(transducerZ_Probe);

    transducerToProbe = [[transducerX_Probe, transducerY_Probe, transducerZ_Probe, transducerOrigin_Probe]; 0 0 0 1];
```

Example use:

```matlab
    %% Get TransducerToProbe transform matrix in Plus-compatible format

  % Transforms provided by Ultrasonix (<http://www.ultrasonix.com/wikisonix/index.php/GPS_Data_Collection>)
    TransducerToProbeLinear = getTransducerToProbeTransform([ 14.8449 0.9477 -0.0018; 15.0061	0.0016	1.00; 0.1638	0.0166	0.0052 ])
    TransducerToProbeConvex = getTransducerToProbeTransform([ -29.7558 0.9433 -0.0034; -0.087 0.0033 1.00; -0.7053 0.0132 -0.0087])

    %% Example for computing spatial calibration

    % ImageToTransducer is provided by Plus if ImageToTransducerTransformName is specified:
    ImageToTransducer = [0.087 0 0 -19.401; 0 0.087 0 -0; 0 0 0.087 0; 0 0 0 1];

    % Compute the ImageToProbe transform matrix:
    ImageToProbe = TransducerToProbeLinear * ImageToTransducer

    % By Plus
    ImageToProbePlus = [0.00222042	0.0866475	0.000440035	13.1131; -0.0889464	0.00184866	-0.00957158	33.8075; -0.0097339	-0.000233354	0.0875636	1.58554; 0	0	0	1]

Results:

    TransducerToProbeLinear =

        0.0018    0.9477   -0.0175   14.8449
       -1.0000    0.0016   -0.0052   15.0061
       -0.0052    0.0166    0.9998    0.1638
             0         0         0    1.0000

    TransducerToProbeConvex =

        0.0034    0.9433   -0.0140  -29.7558
       -1.0000    0.0033    0.0087   -0.0870
        0.0087    0.0132    0.9999   -0.7053
             0         0         0    1.0000

    ImageToProbe =

        0.0002    0.0824   -0.0015   14.8100
       -0.0870    0.0001   -0.0005   34.4071
       -0.0005    0.0014    0.0870    0.2647
             0         0         0    1.0000

    ImageToProbePlus =

        0.0022    0.0866    0.0004   13.1131
       -0.0889    0.0018   -0.0096   33.8075
       -0.0097   -0.0002    0.0876    1.5855
             0         0         0    1.0000
```

## Depth switching (experimental)

Plus allows using different spatial calibration parameters when the imaging depth and/or imaging probe is changed. To enable mode switching, plane switching and/or depth switching, for each output channel you must define one or more of:
- ProbeId: BPC, L14-5, more to come
- PortName: B, Rf
- Depth: supported by your device, ex: 4, 4.5, etc. (in cm)

If you change to a parameter that is not supported by your configuration (ie.: a depth you didn't specify) the data collection will stop!
If you did not specify the global parameters above, Plus will attempt to configure the Sonix hardware with the parameters from the first output channel.

## Example configuration file for tracked ultrasound acquisition using calibration matrix provided by Ultrasonix for L14-5/38 GPS probe

The calibration remains valid even if the imaging depth is changed.