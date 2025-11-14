# LeapMotion hand tracker

## Supported hardware devices

- Link to manufacturer's website: <https://www.leapmotion.com/>

## Supported platforms

- [***Windows 32 bits***](/SupportedPlatforms/#windows-32-bits)
- [***Windows 64 bits***](/SupportedPlatforms/#windows-64-bits)

## Device configuration settings

- **Type**: `LeapMotion`
- **AcquisitionRate**: (Optional, default: `20`)
- **LocalTimeOffsetSec**: (Optional, default: `0`)
- **ToolReferenceFrame**: (Optional, default: `Tracker`)
- **LeapHMDPolicy**: If the leap is attached to an HMD, set this attribute to `TRUE` (Optional, default: `FALSE`)
- **RefusePauseResumePolicy**: The leap motion gets very hot, so by default Plus requests permission to pause and resume the Leap service (this affects ALL leap motion apps) to minimize the temperature. Set to TRUE to refuse these permissions. (Optional, default: `FALSE`)

- **DataSources**: One datasource per joint `DataSource` child element is required. (Required)
    - **DataSource**: (Required)
        - **BufferSize**: (Optional, default: `150`)
        - **Id**: The leap motion device uses these Ids to identify which joint transform should be stored in this data source. Please see the section on transform names in this document for details. (Required)
        - **AveragedItemsForFiltering**: (Optional, default: `20`)

## Data source ID transform names

This device uses the data source "Id" as lookup for which joint to store. The list of acceptable "Id" values are combinations of Hand ("Left", "Right"), Finger ("Thumb", "Index", "Middle", "Ring", "Pinky"), and Joint ("Metacarpal", "Proximal", "Intermediate", "Distal").
Some examples are:
  - LeftIndexMetacarpal
  - RightMiddleProximal
  - LeftThumbDistal

The only exception is that the Thumb does not have a metacarpal joint.

Non-finger joints:
  - LeftPalm
  - RightPalm

## Example configuration file PlusDeviceSet_Server_LeapMotionTracker.xml

```xml
<PlusConfiguration version="2.1">
  <DataCollection StartupDelaySec="1.0">
    <DeviceSet
      Name="PlusServer: LeapMotion"
      Description="Collecting joint pose data from a LeapMotion hand tracker and broadcasting through OpenIGTLink"
    />
    <Device
      AcquisitionRate="240"
      Id="TrackerDevice"
      Type="LeapMotion"
      ToolReferenceFrame="Tracker" >
      <DataSources>
        <DataSource Type="Video" Id="LeftCamera"/>
        <DataSource Type="Video" Id="RightCamera"/>

        <DataSource Type="Tool" Id="LeftThumbDistal"/>
        <DataSource Type="Tool" Id="LeftIndexDistal"/>
        <DataSource Type="Tool" Id="LeftMiddleDistal"/>
        <DataSource Type="Tool" Id="LeftRingDistal"/>
        <DataSource Type="Tool" Id="LeftPinkyDistal"/>

        <DataSource Type="Tool" Id="LeftThumbIntermediate"/>
        <DataSource Type="Tool" Id="LeftIndexIntermediate"/>
        <DataSource Type="Tool" Id="LeftMiddleIntermediate"/>
        <DataSource Type="Tool" Id="LeftRingIntermediate"/>
        <DataSource Type="Tool" Id="LeftPinkyIntermediate"/>

        <DataSource Type="Tool" Id="LeftThumbProximal"/>
        <DataSource Type="Tool" Id="LeftIndexProximal"/>
        <DataSource Type="Tool" Id="LeftMiddleProximal"/>
        <DataSource Type="Tool" Id="LeftRingProximal"/>
        <DataSource Type="Tool" Id="LeftPinkyProximal"/>

        <DataSource Type="Tool" Id="RightThumbDistal"/>
        <DataSource Type="Tool" Id="RightIndexDistal"/>
        <DataSource Type="Tool" Id="RightMiddleDistal"/>
        <DataSource Type="Tool" Id="RightRingDistal"/>
        <DataSource Type="Tool" Id="RightPinkyDistal"/>

        <DataSource Type="Tool" Id="RightThumbIntermediate"/>
        <DataSource Type="Tool" Id="RightIndexIntermediate"/>
        <DataSource Type="Tool" Id="RightMiddleIntermediate"/>
        <DataSource Type="Tool" Id="RightRingIntermediate"/>
        <DataSource Type="Tool" Id="RightPinkyIntermediate"/>

        <DataSource Type="Tool" Id="RightThumbProximal"/>
        <DataSource Type="Tool" Id="RightIndexProximal"/>
        <DataSource Type="Tool" Id="RightMiddleProximal"/>
        <DataSource Type="Tool" Id="RightRingProximal"/>
        <DataSource Type="Tool" Id="RightPinkyProximal"/>
      </DataSources>
      <OutputChannels>
        <OutputChannel Id="LeftChannel" VideoDataSourceId="LeftCamera"/>
        <OutputChannel Id="RightChannel" VideoDataSourceId="RightCamera"/>

        <OutputChannel Id="TrackerChannel">
          <DataSource Id="LeftThumbDistal"/>
          <DataSource Id="LeftIndexDistal"/>
          <DataSource Id="LeftMiddleDistal"/>
          <DataSource Id="LeftRingDistal"/>
          <DataSource Id="LeftPinkyDistal"/>

          <DataSource Id="LeftThumbIntermediate"/>
          <DataSource Id="LeftIndexIntermediate"/>
          <DataSource Id="LeftMiddleIntermediate"/>
          <DataSource Id="LeftRingIntermediate"/>
          <DataSource Id="LeftPinkyIntermediate"/>

          <DataSource Id="LeftThumbProximal"/>
          <DataSource Id="LeftIndexProximal"/>
          <DataSource Id="LeftMiddleProximal"/>
          <DataSource Id="LeftRingProximal"/>
          <DataSource Id="LeftPinkyProximal"/>

          <DataSource Id="RightThumbDistal"/>
          <DataSource Id="RightIndexDistal"/>
          <DataSource Id="RightMiddleDistal"/>
          <DataSource Id="RightRingDistal"/>
          <DataSource Id="RightPinkyDistal"/>

          <DataSource Id="RightThumbIntermediate"/>
          <DataSource Id="RightIndexIntermediate"/>
          <DataSource Id="RightMiddleIntermediate"/>
          <DataSource Id="RightRingIntermediate"/>
          <DataSource Id="RightPinkyIntermediate"/>

          <DataSource Id="RightThumbProximal"/>
          <DataSource Id="RightIndexProximal"/>
          <DataSource Id="RightMiddleProximal"/>
          <DataSource Id="RightRingProximal"/>
          <DataSource Id="RightPinkyProximal"/>
        </OutputChannel>
      </OutputChannels>
    </Device>
    <Device
      Id="CaptureDevice"
      Type="VirtualCapture"
      BaseFilename="RecordingTest.igs.nrrd"
      EnableCompression="TRUE"
      EnableCapturingOnStart="FALSE" >
      <InputChannels>
        <InputChannel Id="TrackerChannel" />
      </InputChannels>
    </Device>
  </DataCollection>

  <CoordinateDefinitions>
    <Transform From="LeftCamera" To="Tracker"
      Matrix="1 0 0 0
              0 1 0 0
              0 0 1 0
              0 0 0 1"
      Date="2011.12.06 17:57:00" Error="0.0" />
  </CoordinateDefinitions>

  <PlusOpenIGTLinkServer
    MaxNumberOfIgtlMessagesToSend="1"
    MaxTimeSpentLeaphProcessingMs="50"
    ListeningPort="18945"
    OutputChannelId="LeftChannel"
    >
    <DefaultClientInfo>
      <MessageTypes>
        <Message Type="IMAGE" />
      </MessageTypes>
      <ImageNames>
        <Image Name="LeftCamera" EmbeddedTransformToFrame="Tracker"/>
      </ImageNames>
    </DefaultClientInfo>
  </PlusOpenIGTLinkServer>

  <PlusOpenIGTLinkServer
    MaxNumberOfIgtlMessagesToSend="1"
    MaxTimeSpentLeaphProcessingMs="50"
    ListeningPort="18944"
    OutputChannelId="TrackerChannel"
    >
    <DefaultClientInfo>
      <MessageTypes>
        <Message Type="TRANSFORM" />
      </MessageTypes>
      <TransformNames>
        <Transform Name="LeftThumbDistalToTracker"/>
        <Transform Name="LeftIndexDistalToTracker"/>
        <Transform Name="LeftMiddleDistalToTracker"/>
        <Transform Name="LeftRingDistalToTracker"/>
        <Transform Name="LeftPinkyDistalToTracker"/>

        <Transform Name="LeftThumbIntermediateToTracker"/>
        <Transform Name="LeftIndexIntermediateToTracker"/>
        <Transform Name="LeftMiddleIntermediateToTracker"/>
        <Transform Name="LeftRingIntermediateToTracker"/>
        <Transform Name="LeftPinkyIntermediateToTracker"/>

        <Transform Name="LeftThumbProximalToTracker"/>
        <Transform Name="LeftIndexProximalToTracker"/>
        <Transform Name="LeftMiddleProximalToTracker"/>
        <Transform Name="LeftRingProximalToTracker"/>
        <Transform Name="LeftPinkyProximalToTracker"/>

        <Transform Name="RightThumbDistalToTracker"/>
        <Transform Name="RightIndexDistalToTracker"/>
        <Transform Name="RightMiddleDistalToTracker"/>
        <Transform Name="RightRingDistalToTracker"/>
        <Transform Name="RightPinkyDistalToTracker"/>

        <Transform Name="RightThumbIntermediateToTracker"/>
        <Transform Name="RightIndexIntermediateToTracker"/>
        <Transform Name="RightMiddleIntermediateToTracker"/>
        <Transform Name="RightRingIntermediateToTracker"/>
        <Transform Name="RightPinkyIntermediateToTracker"/>

        <Transform Name="RightThumbProximalToTracker"/>
        <Transform Name="RightIndexProximalToTracker"/>
        <Transform Name="RightMiddleProximalToTracker"/>
        <Transform Name="RightRingProximalToTracker"/>
        <Transform Name="RightPinkyProximalToTracker"/>
      </TransformNames>
    </DefaultClientInfo>
  </PlusOpenIGTLinkServer>
</PlusConfiguration>
```