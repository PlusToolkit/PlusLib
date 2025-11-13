# Virtual Text Recognizer

This device can recognize text (in the language specified by `Language`) from a number of input channels.

## Device configuration settings

- **Device**:
    - **Type**: `VirtualTextRecognizer`
    - **Language**: Language to be recognized. (Optional, default: `eng`)
    - **TessdataDirectory**: Path to the parent of the (`tessdata`) directory containing the language files. If this is not set, it will default to the (`TESSDATA_PREFIX`) environment variable.
    - **TextFields**: Multiple `Field` child elements are allowed, one for each parameter to recognize (Required)
        - **Field**: (Required)
            - **Channel**: The input channel to pull data from for recognition (Required)
            - **Name**: Name of the variable to broadcast on detected change (Required)
            - **InputRegionOrigin**: The origin in the input image to extract a sub-image (Required)
            - **InputRegionSize**: The size of the subimage to extract (Required)
    - **InputChannels**: Multiple `InputChannel` child elements are allowed, one for each input channel to be available to query (Required)
        - **InputChannel**: (Required)
	          - **Id**: The id of this input channel

## Example configuration file PlusDeviceSet_PlusServer_tesseract.xml

```xml
<PlusConfiguration version="2.3">
  <DataCollection StartupDelaySec="1.0">
    <DeviceSet
      Name="PlusServer: Text recognition sample on saved data"
      Description="Broadcasting acquired video through OpenIGTLink" />
    <Device
      Id="TrackedVideoDevice"
      Type="SavedDataSource"
      SequenceFile="ie33_biplane_LeftCaptureDevice_compressed.igs.mha"
      UseData="IMAGE_AND_TRANSFORM"
      UseOriginalTimestamps="FALSE"
      RepeatEnabled="TRUE" >
      <DataSources>
        <DataSource Type="Video" Id="Video" PortUsImageOrientation="MFA"/>
      </DataSources>
      <OutputChannels>
        <OutputChannel Id="TrackedVideoStream" VideoDataSourceId="Video" />
      </OutputChannels>
    </Device>

    <Device
      Id="TextRecognizerDevice"
      Type="VirtualTextRecognizer"
      MissingInputGracePeriodSec="1"
      Language="eng" >
      <DataSources>
        <DataSource Type="FieldData" Id="ScreenFields"/>
      </DataSources>
      <TextFields>
        <Field Channel="TrackedVideoStream" Name="patient-last-name" InputRegionOrigin="260 885" InputRegionSize="82 20" />
        <Field Channel="TrackedVideoStream" Name="tee-temperature" InputRegionOrigin="75 185" InputRegionSize="55 20" />
      </TextFields>
      <InputChannels>
        <InputChannel Id="TrackedVideoStream" />
      </InputChannels>
      <OutputChannels>
        <OutputChannel Id="DataChannel">
          <DataSource Id="ScreenFields"/>
        </OutputChannel>
      </OutputChannels>
    </Device>

    <Device
      Id="Mixer"
      Type="VirtualMixer" >
      <InputChannels>
        <InputChannel Id="DataChannel" />
        <InputChannel Id="TrackedVideoStream" />
      </InputChannels>
      <OutputChannels>
        <OutputChannel Id="TrackedVideoStreamData"/>
      </OutputChannels>
    </Device>
  </DataCollection>

  <CoordinateDefinitions>
    <Transform From="Image" To="Probe"
      Matrix="
        1 0 0 0
        0 1 0 0
        0 0 1 0
        0 0 0 1"/>
  </CoordinateDefinitions>

  <PlusOpenIGTLinkServer
    MaxNumberOfIgtlMessagesToSend="1"
    MaxTimeSpentWithProcessingMs="50"
    ListeningPort="18944"
    SendValidTransformsOnly="true"
    OutputChannelId="TrackedVideoStreamData" >
    <DefaultClientInfo>
      <MessageTypes>
        <Message Type="STRING" />
        <Message Type="IMAGE" />
        <Message Type="TRANSFORM" />
      </MessageTypes>
      <StringNames>
        <String Name="patient-last-name" />
        <String Name="tee-temperature" />
      </StringNames>
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