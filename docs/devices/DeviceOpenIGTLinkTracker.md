# Any OpenIGTLink compatible tracker

## Supported hardware devices

- Any compatible tracker device through [OpenIGTLink interface](http://openigtlink.org/).
- Any tracker device that are supported by the  [IGSTK toolkit](http://public.kitware.com/IGSTKWIKI/index.php/Main_Page), using IGSTK's OpenIGTLinkTrackingBroadcaster application.
- Receiving values of transforms from 3D Slicer application
  - Use these settings: **UseReceivedTimestamps** `=(`FALSE`)` and **UseLastTransformsOnReceiveTimeout** `=(`TRUE`)` (as Slicer only sends transforms when they change)
- BrainLab surgical navigation system
  - BrainLab OpenIGTLink interface may be purchased as a license option with VectorVision Cranial 2.1.
  - Contact BrainLab for information about licensing and for supported OpenIGTLink options.
  - For tracking data, the BrainLab OpenIGTLink interface uses the [TDATA](http://www.na-mic.org/Wiki/index.php/OpenIGTLink/ProtocolV2/Type/TrackingData) message type, which is supported by PLUS 2.0 as of r2337
  - In the configuration section for OpenIGTLinkTracker Device, set **MessageType** `=(`TDATA`)` and **ReconnectOnReceiveTimeout** `=(`FALSE`)` (to avoid reconnects, as BrainLab asks for manual confirmation on each connect request)
  - See ConfigFiles/BWH directory for example configurations using BrainLab tracking.

## Device configuration settings

- **Type**: `OpenIGTLinkTracker`
- **ServerAddress**: Host name or IP address of the OpenIGTLink server that sends the data to this device.
- **ServerPort**: Port of the OpenIGTLink server that sends the data to this device.
- **MessageType**: The device will request this message type from the remote server. If not specified, the default message type configured on the remote server will be used. (Optional, default: `TRANSFORM`)
- **TrackerInternalCoordinateSystemName**: Used for TDATA messages to specify what coordinate systems should be the common (`To`) coordinate system.
- **UseLastTransformsOnReceiveTimeout**: Use the latest known value for a transform if new value for a transform is not received.
    - `TRUE` If there is no new value received for a transform then the last known value is used. It is useful for software that only sends a transform when it is changed, such when sending transforms from 3D Slicer.
    - `FALSE` If there is no new value received for a transform then it is treated as an error.
- **ReceiveTimeoutSec**: Time to allow for the device to receive a message, in seconds.
- **SendTimeoutSec**: Time to allow for the device to send a message, in seconds.
- **IgtlMessageCrcCheckEnabled**: Enable CRC check on the received OpenIGTLink messages
- **UseReceivedTimestamps**: Use the timestamps that are stored in the OpenIGTLink messages.
    - `TRUE` Timestamp in the OpenIGTLink message header is used as acquisition time for the item. If the remote server is on a different computer then the clocks of the remote server computer and the computer that runs PlusServer must be accurately synchronized (e.g., using NTP).
    - `FALSE` Time of receiving the message is used as timestamp. Variable network delays may cause jitter in the timestamps.

- **ReconnectOnReceiveTimeout**: If this option is enabled and the server becomes unresponsive then the device tries to reconnect repeatedly
- **AcquisitionRate**: The device checks for new available messages on the remote server at this rate. In case of TRANSFORM or POSITIOn messages, the acquisition rate should be equal or higher than the rate the server sends the data, otherwise the data is queued in the socket and arrives with a long delay. (Optional, default: `30`)
- **LocalTimeOffsetSec**: (Optional, default: `0`)

## Example configuration file PlusDeviceSet_Server_NDICertus.xml PlusDeviceSet_OpenIGTLinkTracker_TDATA.xml

```xml
<PlusConfiguration version="2.1">

  <DataCollection StartupDelaySec="1.0" >
    <DeviceSet
      Name="BrainLabTrackerSim with video from file"
      Description="To be used with the BrainLabTrackerSim simulator or with a real BrainLab tracker" />

    <Device
      Id="TrackerDevice"
      Type="OpenIGTLinkTracker"
      MessageType="TDATA"
      TrackerInternalCoordinateSystemName="Reference"
      ToolReferenceFrame="Reference"
      ServerAddress="127.0.0.1"
      ServerPort="22222"
      IgtlMessageCrcCheckEnabled="true"
      AcquisitionRate="20"
      ReconnectOnReceiveTimeout="false" >
      <DataSources>
        <DataSource Type="Tool" Id="ReferenceStar" />
        <DataSource Type="Tool" Id="Pointer" />
        <DataSource Type="Tool" Id="Ultrasound" />
      </DataSources>
      <OutputChannels>
        <OutputChannel Id="TrackerStream">
          <DataSource Id="ReferenceStar" />
          <DataSource Id="Pointer" />
          <DataSource Id="Ultrasound" />
        </OutputChannel>
      </OutputChannels>
    </Device>

    <Device
      Id="VideoDevice"
      Type="SavedDataSource"
      SequenceFile="fCal_Test_Calibration_3NWires_fCal2.0.igs.mha"
      UseData="IMAGE"
      UseOriginalTimestamps="FALSE"
      RepeatEnabled="TRUE">
      <DataSources>
        <DataSource Type="Video" Id="Video"  />
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


  <PlusOpenIGTLinkServer
    MaxNumberOfIgtlMessagesToSend="1"
    MaxTimeSpentWithProcessingMs="10"
    ListeningPort="18944"
    OutputChannelId="TrackedVideoStream" >
    <DefaultClientInfo>
      <MessageTypes>
        <Message Type="TRANSFORM" />
        <Message Type="IMAGE" />
      </MessageTypes>
      <TransformNames>
        <Transform Name="ReferenceStarToTracker" />
        <Transform Name="PointerToTracker" />
        <Transform Name="UltrasoundToTracker" />
      </TransformNames>
      <ImageNames>
        <Image Name="Image" EmbeddedTransformToFrame="Tracker" />
      </ImageNames>
    </DefaultClientInfo>
  </PlusOpenIGTLinkServer>

  <CoordinateDefinitions>
    <Transform From="Image" To="Ultrasound"
      Matrix="
        0.107535	0.00094824	0.0044213	-65.9013
        0.0044901	-0.00238041	-0.106347	-3.05698
        -0.000844189	0.105271	-0.00244457	-17.1613
        0	0	0	1"
      Error="0.804961" Date="121012_223643"/>
  </CoordinateDefinitions>

</PlusConfiguration>
```