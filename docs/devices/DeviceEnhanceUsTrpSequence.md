# Enhance US sequence (EnhanceUsTrpSequence)

This device applies a series of vtk image filter processing steps to US images.
Filter parameters are read from the ImageProcessingOperations element of the config file.

## Device configuration settings

- **Type**: `ImageProcessor`
    - **Processor**
        - **Type**: `vtkPlusTransverseProcessEnhancer`
        - **NumberOfScanlines**: (Required)
        - **NumberOfSamplesPerScanline**: (Required)
        - **ScanConversion**
            - **TransducerName**: (Required)
            - **TransducerGeometry**: (Required)
            - **RadiusStartMm**: (Optional, default: `50`)
            - **RadiusStopMm**: (Optional, default: `120`)
            - **ThetaStartDeg**: (Optional, default: `-24`)
            - **ThetaStopDeg**: (Optional, default: `24`)
            - **OutputImageSizePixel**: (Optional, default: `820 616`)
            - **TransducerCenterPixel**: (Optional, default: `410 35`)
            - **OutputImageSpacingMmPerPixel**: (Optional, default: `0.1526 0.1526`)
        - **ImageProcessingOperations**
            - **SaveIntermediateResults**: (Optional, default: `FALSE`)
            - **ReturnToFanImage**: (Optional, default: `TRUE`)
            - **GaussianSmoothing**
                - **GaussianStdDev**: (Optional, default: `3.0`)
                - **GaussianKernelSize**: (Optional, default: `5.0`)
        - **Thresholding**
            - **ThresholdOutValue**: (Optional, default: `0.0`)
            - **LowerThreshold**: (Optional, default: `30.0`)
            - **UpperThreshold**: (Optional, default: `255.0`)
        - **IslandRemoval**
            - **IslandAreaThreshold**: (Optional, default: `700`)
        - **Erosion**
            - **ErosionKernelSize**: (Optional, default: `5 5`)
        - **Dilation**
            - **DilationKernelSize**: (Optional, default: `10 5`)
        - **vtkPlusUsSimulatorAlgo**
            - **NumberOfScanlines**: (Required)
            - **NumberOfSamplesPerScanline**: (Required)

## Example configuration file PlusDeviceSet_Server_Ultrasonix_C5-2_TransverseProcessEnhancer_2Processing.xml

```xml
<PlusConfiguration version="2.1">
  <DataCollection StartupDelaySec="1.0" >
    <DeviceSet
      Name="PlusServer: Ultrasound transverse process enhancer component 2: Processing - with tracking"
      Description="Runs on 64-bit processing machine. Receives tracked ultrasound frames at port 18944 from the 32-bit PlusServer running on the Ultrasonix machine (change ServerAddress from localhost to Ultrasonix IP), segments bone surfaces, and sends tracked bone frames to an IGTLink client at port 18945 in Slicer" />

    <Device
      Id="OpenIGTLinkVideoReceiveDevice"
      Type="OpenIGTLinkVideo"
      MessageType="TRACKEDFRAME"
      ServerAddress="localhost"
      ServerPort="18944"
      ReconnectOnReceiveTimeout="false"
      UseReceivedTimestamps="false"
      IgtlMessageCrcCheckEnabled="false"
      AcquisitionRate="30" >
      <DataSources>
        <DataSource Type="Video" Id="Video" BufferSize="50" PortUsImageOrientation="UF" />
      </DataSources>
      <OutputChannels>
        <OutputChannel Id="TrackedVideoStream" VideoDataSourceId="Video" />
      </OutputChannels>
    </Device>

    <Device
      Id="BoneEnhancer"
      Type="ImageProcessor" >
      <DataSources>
        <DataSource Type="Video" Id="Video" PortUsImageOrientation="UF" />
      </DataSources>
      <InputChannels>
        <InputChannel Id="TrackedVideoStream" />
      </InputChannels>
      <OutputChannels>
        <OutputChannel Id="BoneVideoStream" VideoDataSourceId="Video" />
      </OutputChannels>
      <Processor Type="vtkPlusBoneEnhancer" NumberOfScanLines="200" NumberOfSamplesPerScanLine="210">
        <ScanConversion
          TransducerName="Ultrasonix_C5-2"
          TransducerGeometry="CURVILINEAR"
          RadiusStartMm="50.0"
          RadiusStopMm="120.0"
          ThetaStartDeg="-24"
          ThetaStopDeg="24"
          OutputImageSizePixel="820 616"
          TransducerCenterPixel="320 35"
          OutputImageSpacingMmPerPixel="0.1526 0.1526"
          NumberOfSamplesPerScanLine="210"/>
        <ImageProcessingOperations
          SaveIntermediateResults="True"
          ConvertToLinesImage="True"
          ThresholdingEnabled="True"
          GaussianEnabled="True"
          EdgeDetectorEnabled="True"
          IslandRemovalEnabled="True"
          ErosionEnabled="True"
          DilationEnabled="True"
          ReconvertBinaryToGreyscale="True"
          ReturnToFanImage="True">

          <GaussianSmoothing GaussianStdDev="3" GaussianKernelSize="5"/>
          <Thresholding ThresholdOutValue="0" LowerThreshold="30" UpperThreshold="255"/>
          <IslandRemoval IslandAreaThreshold="700"/>
          <Erosion ErosionKernelSize="5 5"/>
          <Dilation DilationKernelSize="10 5"/>

          <vtkPlusUsSimulatorAlgo
          IncomingIntensityMwPerCm2="300"
          BrightnessConversionGamma="0.2"
          BrighntessConversionOffset="30"
          NumberOfScanlines="128"
          NumberOfSamplesPerScanline="200"
          NoiseAmplitude="5.0"
          NoiseFrequency="2.5 3.5 1"
          NoisePhase="50 20 0"
          />
        </ImageProcessingOperations>
      </Processor>
  </Device>

    <!-- These capture devices allow you to save and compare the same sequence, before and after processing, if both EnableCapturingOnStart="True". -->
    <Device
      Id="RawCaptureDevice"
      Type="VirtualCapture"
      BaseFilename="RawRecording.igs.mha"
      EnableCapturingOnStart="False" >
      <InputChannels>
        <InputChannel Id="TrackedVideoStream" />
      </InputChannels>
    </Device>

    <Device
      Id="ProcessedCaptureDevice"
      Type="VirtualCapture"
      BaseFilename="ProcessedRecording.igs.mha"
      EnableCapturingOnStart="False" >
      <InputChannels>
        <InputChannel Id="BoneVideoStream" />
      </InputChannels>
    </Device>

  </DataCollection>

  <CoordinateDefinitions>
  <Transform From="BoneImage" To="Image"
      Matrix="
        1 0 0 0
        0 1 0 0
        0 0 1 0
        0 0 0 1" />
  </CoordinateDefinitions>

  <!-- Enable this Server's port in Slicer to see the processed image -->
  <PlusOpenIGTLinkServer
    MaxNumberOfIgtlMessagesToSend="1"
    MaxTimeSpentWithProcessingMs="50"
    ListeningPort="18945"
    SendValidTransformsOnly="true"
    OutputChannelId="BoneVideoStream" >
    <DefaultClientInfo>
      <MessageTypes>
        <Message Type="IMAGE" />
      </MessageTypes>

      <ImageNames>
        <Image Name="BoneImage" EmbeddedTransformToFrame="Image" />
      </ImageNames>

    </DefaultClientInfo>
  </PlusOpenIGTLinkServer>

</PlusConfiguration>
```