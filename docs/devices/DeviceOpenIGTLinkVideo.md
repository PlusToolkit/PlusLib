# Any OpenIGTLink compatible imaging device

## Supported hardware devices
Any compatible video device through [OpenIGTLink interface](http://openigtlink.org/).
There are commercial systems that provide tracking video and image data through OpenIGTLink, such as BrainLab, Siemens MRI scanners

## Device configuration settings

- **Type**: `OpenIGTLinkVideo`
- **ServerAddress**: Host name or IP address of the OpenIGTLink server that sends the data to this device.
- **ServerPort**: Port of the OpenIGTLink server that sends the data to this device.
- **ImageMessageEmbeddedTransformName**: If `IMAGE` message is received and this attribute is defined then
  the transform embedded in the message will be recorded as a transform, with the specified name
 (e.g., "ImageToReference"). If the attribute is not defined then the embedded transform is ignored.
  If the message type is not `IMAGE` then the attribute is ignored. (Optional)
- **MessageType**: The device will request this message type from the remote server. If the MessageType is not specified then the default message type will be used
    - `IMAGE` Request sending only image data in `IMAGE` OpenIGTLink messages.
    - `TRACKEDFRAME` Request sending image+tracking data in `TRACKEDFRAME` OpenIGTLink messages.
- **IgtlMessageCrcCheckEnabled**: Enable CRC check on the received OpenIGTLink messages
- **UseReceivedTimestamps**: Use the timestamps that are stored in the OpenIGTLink messages.
    - `TRUE` Timestamp in the OpenIGTLink message header is used as acquisition time for the item. If the remote server is on a different computer then the clocks of the remote server computer and the computer that runs PlusServer must be accurately synchronized (e.g., using NTP).
    - `FALSE` Time of receiving the message is used as timestamp. Variable network delays may cause jitter in the timestamps.
- **ReconnectOnReceiveTimeout**: If this option is enabled and the server becomes unresponsive then the device tries to reconnect repeatedly
- **ReceiveTimeoutSec**: Time to allow for the device to receive a message, in seconds.
- **SendTimeoutSec**: Time to allow for the device to send a message, in seconds.
- **AcquisitionRate**: The device checks for new available messages on the remove server at this rate. (Optional, default: `30`)
- **LocalTimeOffsetSec**: (Optional, default: `0`)
- **DataSources**: Exactly one `DataSource` child element is required. (Required)
    - **DataSource**: (Required)
        - **PortUsImageOrientation**: (Required)
        - **ImageType**: (Optional, default: `BRIGHTNESS`)
        - **BufferSize**: (Optional, default: `150`)
        - **AveragedItemsForFiltering**: (Optional, default: `20`)
        - **ClipRectangleOrigin**: (Optional, default: `0 0 0`)
        - **ClipRectangleSize**: (Optional, default: `0 0 0`)

## Example configuration file PlusDeviceSet_OpenIGTLinkVideoSource.xml

```xml
<PlusConfiguration version="2.1">

  <DataCollection StartupDelaySec="1.0">
    <DeviceSet
      Name="TEST Data collection uses OpenIGTLink video source on localhost."
      Description="Test configuration file for vtkDataCollectorFileTest to use a tracked frame list. The SequenceMetafile names should be defined either from command line argument or from configuration file!" />
    <Device
        Id="OpenIGTLinkVideoSenderDevice"
        Type="OpenIGTLinkVideo"
        MessageType="TrackedFrame"
        ServerAddress="127.0.0.1"
        ServerPort="18944"
        IgtlMessageCrcCheckEnabled="false" >
        <DataSources>
          <DataSource Type="Video" Id="Video" PortUsImageOrientation="MF"  />
        </DataSources>
        <OutputChannels>
          <OutputChannel Id="TrackedVideoSenderStream" VideoDataSourceId="Video" />
        </OutputChannels>
      </Device>
  </DataCollection>

</PlusConfiguration>
```

This device can be used for transferring tracked video data to another computer for processing, recording, and broadcasting. This is useful in the following cases:
- the computer where the data is acquired is not fast enough (for example, if frames are skipped during volume reconstruction: "Volume reconstruction cannot keep up with the acquisition. Skip ... seconds of the data stream to catch up.")
- 32-bit devices are used or a 32-bit computer is used for data acquisition, but high-resolution volume reconstruction requires 64-bit memory space

## Example configuration file for data acquisition and sending (it acquires data from a file and forwards it through OpenIGTLink TRACKEDFRAME message to the processing server):

```xml
<PlusConfiguration version="2.1">
  <DataCollection StartupDelaySec="1.0" >
    <DeviceSet
      Name="PlusServer: TRACKEDFRAME acquisition server. fCal phantom scan with Ultrasonix US (L14-5 probe) + Ascension3DG tracker (Probe, Reference, Stylus)"
      Description="Free-hand probe motion, imaging 3 NWires, after spatial calibration. OpenIGTLink broadcasting of TRACKEDFRAME message through PlusServer." />
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
  </DataCollection>
  <PlusOpenIGTLinkServer
    MaxNumberOfIgtlMessagesToSend="1"
    MaxTimeSpentWithProcessingMs="50"
    ListeningPort="18000"
    SendValidTransformsOnly="true"
    OutputChannelId="TrackedVideoStream" >
    <DefaultClientInfo>
      <MessageTypes>
        <Message Type="TRACKEDFRAME" />
      </MessageTypes>
    </DefaultClientInfo>
  </PlusOpenIGTLinkServer>
</PlusConfiguration>
```

## Example configuration file for data receiving and processing (it acquires data from the acquisition server and allows recording, volume reconstruction, and broadcasting of IMAGE and TRANSFORM messages):

```xml
<PlusConfiguration version="2.1">

  <DataCollection StartupDelaySec="1.0" >
    <DeviceSet
      Name="PlusServer: TRACKEDFRAME processing server"
      Description="To be used with TRACKEDFRAME acquisition server. Receives tracked frames through OpenIGTLink and it can record sequences, reconstruct volumes, and sends IMAGE and TRANSFORM messages through OpenIGTLink." />
    <Device
      Id="TrackedVideoDevice"
      Type="OpenIGTLinkVideo"
      MessageType="TRACKEDFRAME"
      ServerAddress="127.0.0.1"
      ServerPort="18000"
      IgtlMessageCrcCheckEnabled="false"
      LocalTimeOffsetSec="0" >
      <DataSources>
        <DataSource Type="Video" Id="Video" PortUsImageOrientation="MF"  />
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
