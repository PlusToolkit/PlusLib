# Virtual Capture

This device allows writing acquired data to disk. Data can be recorded in memory and written to disk (for short, high-throughput acquisitions) or directly written to disk (for continuous acquisition of large amount of data, requires fast disk access for high-throughput acquisition).

The file is saved as `FileSequenceMetafile` format. If single file output format is used (file extension is ) then stopping of the recording may take some time (as temporary recording output has to be merged into one file). If multiple long sequences have to be recorded then use the header+data file format (.mhd extension of the filename): in this case a the capture device can start a new acquisition immediately after stopping the previous recording.

## Device configuration settings

- **Type**: `VirtualCapture`
- **AcquisitionRate**: Defines how frequently the device copies frames from the input data source to the disk. (Optional, default: `30`)
- **LocalTimeOffsetSec**: (Optional, default: `0`)
- **BaseFilename**: File to write, path relative to output directory.
- **EnableFileCompression**: Flag to write it compressed.
    - **Warning! Beware file limits on old FAT32 disks (4GB maximum file size)**
- **EnableCapturingOnStart**: Enable capturing when device is connected
- **RequestedFrameRate**: Requested frame rate for recording [frames/second]. If the input data source provides data at a higher rate then frames will be skipped. If the input data has lower frame rate then requested then all the frames in the input data will be recorded.
- **FrameBufferSize**: Number of frames stored in memory before dumping to file. Increases memory need but allows higher recording frame rate ## Example configuration file PlusDeviceSet_Server_Sim_NwirePhantom.xml

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
```