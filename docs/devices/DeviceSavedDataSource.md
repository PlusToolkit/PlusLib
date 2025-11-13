# Replay recorded data from file

## Supported hardware devices

For hardware-free testing and simulation purposes, any previous recording (saved into a sequence metafile) can be replayed as a live acquisition

## Device configuration settings

- **Type**: `SavedDataSource`
- **AcquisitionRate**: (Optional, default: `10`)
- **LocalTimeOffsetSec**: (Optional, default: `0`)
- **ToolReferenceFrame**: (Optional, default: `Tracker`)
- **SequenceMetafile**: Name of input sequence metafile with path to tracking buffer data.
- **RepeatEnabled**: Flag to enable saved dataset looping. If it's enabled, the video source will continuously play saved data (starts playing from the beginning when the end is reached). (Optional, default: `FALSE`)
- **UseOriginalTimestamps**: Flag to read the timestamps from the file and use them in the output (Optional, default: `FALSE`)
- **UseData**: Three types of data that can be used: (Optional, default: `IMAGE`)
    - `IMAGE` The device provides a video stream. Metadata stored in custom field data is ignored.
    - `TRANSFORM` The device provides a tracker stream
    - `IMAGE_AND_TRANSFORM`  The device provides a video stream with tracking data and other metadata added as fields.
- **DataSources**: Exactly one `DataSource` child element is required. (Required)
    - **DataSource**: (Required)
        - **PortUsImageOrientation**: (Required)
        - **BufferSize**: (Optional, default: `150`)
        - **AveragedItemsForFiltering**: (Optional, default: `20`)
        - **ClipRectangleOrigin**: (Optional, default: `0 0 0`)
        - **ClipRectangleSize**: (Optional, default: `0 0 0`)

## Example configuration file for simple replay of all image and transform data - PlusDeviceSet_Server_Sim_NwirePhantom.xml

```xml
<PlusConfiguration version="2.1">

  <DataCollection StartupDelaySec="1.0" >
    <DeviceSet
      Name="PlusServer: Replay fCal phantom scan with Ultrasonix US (L14-5 probe) + Ascension3DG tracker (Probe, Reference, Stylus)"
      Description="Free-hand probe motion, imaging 3 NWires, after spatial calibration. OpenIGTLink broadcasting through PlusServer of image and tool tracking data." />
    <Device
      Id="TrackedVideoDevice"
      Type="SavedDataSource"
      SequenceFile="fCal_Test_Calibration_3NWires.igs.mha"
      UseData="IMAGE_AND_TRANSFORM"
      UseOriginalTimestamps="TRUE"
      RepeatEnabled="TRUE" >
      <DataSources>
        <DataSource Type="Video" Id="Video" />
      </DataSources>
      <OutputChannels>
        <OutputChannel Id="TrackedVideoStream" VideoDataSourceId="Video" />
      </OutputChannels>
    </Device>
    <Device
      Id="CaptureDevice"
      Type="VirtualCapture"
      BaseFilename="RecordingTest.igs.mha"
      EnableCapturingOnStart="FALSE" >
      <InputChannels>
        <InputChannel Id="TrackedVideoStream" />
      </InputChannels>
    </Device>
    <Device
      Id="VolumeReconstructorDevice"
      Type="VirtualVolumeReconstructor"
      OutputVolDeviceName="RecVol_Reference">
      <InputChannels>
        <InputChannel Id="TrackedVideoStream" />
      </InputChannels>
      <VolumeReconstruction
        ImageCoordinateFrame="Image" ReferenceCoordinateFrame="Reference"
        Interpolation="LINEAR" Optimization="NONE" CompoundingMode="MEAN" FillHoles="OFF" NumberOfThreads="2"
        ClipRectangleOrigin="0 0" ClipRectangleSize="820 616"
        OutputOrigin="-15 -15 30" OutputExtent="0 300 0 300 0 300" OutputSpacing="0.15 0.15 0.15" />
    </Device>
  </DataCollection>

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
        <Transform Name="ProbeToTracker" />
        <Transform Name="ReferenceToTracker" />
        <Transform Name="ProbeToReference" />
      </TransformNames>
      <ImageNames>
        <Image Name="Image" EmbeddedTransformToFrame="Reference" />
      </ImageNames>
    </DefaultClientInfo>
  </PlusOpenIGTLinkServer>

  <CoordinateDefinitions>
    <Transform From="Image" To="TransducerOriginPixel"
      Matrix="
        1 0 0 -410
        0 1 0 5
        0 0 1 0
        0 0 0 1"
      Date="2011.12.06 17:57:00" />
    <Transform From="Phantom" To="Reference"
      Matrix="
        0.00898765 -0.0136951 -0.999866 9.59137
        0.0246163 -0.9996 0.0139127 36.0012
        -0.999657 -0.024738 -0.00864693 87.8909
        0 0 0 1"
      Error="0.404711" Date="012712_152104" />
    <Transform From="StylusTip" To="Stylus"
      Matrix="
        1 0 0 203.772
        0 1 0 -12.283
        0 0 1 1.17469
        0 0 0 1"
      Error="0.582721" Date="012712_152030" />
    <Transform From="Image" To="Probe"
      Matrix="
        0     0.08    0     11
        -0.08 0       0     50
        0     0       0.08  0
        0     0       0     1"
      Error="0.582721" Date="012712_152030" />
  </CoordinateDefinitions>

</PlusConfiguration>
## Example configuration file for replaying imaging and tracking data separately - PlusDeviceSet_DataCollectionOnly_SavedDataset.xml

```xml
<PlusConfiguration version="2.1">

  <DataCollection StartupDelaySec="1.0" >
    <DeviceSet
      Name="TEST Tracking and video data is read from sequence metafile."
      Description="Test configuration file for vtkDataCollectorTest to use saved dataset. The SequenceMetafile names should be defined either from command line argument or from configuration file!" />

    <Device
      Id="TrackerDevice"
      Type="SavedDataSource"
      UseData="TRANSFORM"
      AcquisitionRate="10"
      LocalTimeOffsetSec="0.0"
      ToolReferenceFrame="Tracker"
      SequenceFile="NwirePhantomFreehand.igs.mha" >
      <DataSources>
        <DataSource Type="Tool" Id="Probe"  />
      </DataSources>
      <OutputChannels>
        <OutputChannel Id="TrackerStream">
          <DataSource Id="Probe"/>
        </OutputChannel>
      </OutputChannels>
    </Device>

    <Device
      Id="VideoDevice"
      Type="SavedDataSource"
      UseData="IMAGE"
      AcquisitionRate="10"
      LocalTimeOffsetSec="0.0"
      SequenceFile="NwirePhantomFreehand.igs.mha" >
      <DataSources>
        <DataSource Type="Video" Id="Video" PortUsImageOrientation="MF"  />
      </DataSources>
      <OutputChannels>
        <OutputChannel Id="VideoStream" VideoDataSourceId="Video" />
      </OutputChannels>
    </Device>

    <Device Id="TrackedVideoDevice" Type="VirtualMixer">
      <InputChannels>
        <InputChannel Id="TrackerStream" />
        <InputChannel Id="VideoStream" />
      </InputChannels>
      <OutputChannels>
        <OutputChannel Id="TrackedVideoStream"/>
      </OutputChannels>
    </Device>

  </DataCollection>

</PlusConfiguration>
```