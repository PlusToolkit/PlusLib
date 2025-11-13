# Any OpenCV compatible capture device

## Supported hardware devices
Any compatible video device through the OpenCV capture interface</a>.

## Device configuration settings

- **Type**: `OpenCVVideo` (Required)
- **VideoURL**: Complete URL of the file or stream to capture. Either VideoURL or DeviceIndex must be specified.
- **DeviceIndex**: Index of the video device to use. Either VideoURL or DeviceIndex must be specified.
- **CaptureAPI** The capture API to use for this device. (Optional, default: `CAP_ANY`)
- **FrameSize** The 2 dimensional frame size to use for capturing (example: `FrameSize="1920 1080"`) (Optional)
- **AcquisitionRate** The FPS to request of the camera (Optional)
- **FourCC** The four character code for pixel format (Example: `MJPG`) (Optional)
- **CameraMatrix** A 9 valued entry specifying the 3x3 intrinsic camera matrix. Both CameraMatrix and DistortionCoefficients must be specified for undistortion to occur. (Optional)
- **DistortionCoefficients** Up to 8 value entry specifying the camera distortion coefficients. Both CameraMatrix and DistortionCoefficients must be specified for undistortion to occur. (Optional)
- **AutofocusEnabled** A boolean value (`TRUE` or `FALSE`) specifying whether the camera can autofocus. (Optional, default: `FALSE`)
- **AutoexposureEnabled** A boolean value (`TRUE` or `FALSE`) specifying whether the camera can automatically set the exposure. (Optional, default: `FALSE`)
- **DataSources**: Exactly one `DataSource` child element is required. (Required)
    - **DataSource**: (Required)
        - **PortUsImageOrientation**: (Required)
        - **ImageType**: (Optional, default: `RGB_COLOR`)
        - **BufferSize**: (Optional, default: `150`)
        - **AveragedItemsForFiltering**: (Optional, default: `20`)
        - **ClipRectangleOrigin**: (Optional, default: `0 0 0`)
        - **ClipRectangleSize**: (Optional, default: `0 0 0`)

## Example configuration file PlusDeviceSet_Server_OpenCVVideoSource.xml

```xml
<PlusConfiguration version="2.1">

  <DataCollection StartupDelaySec="1.0" >
    <DeviceSet
      Name="PlusServer: OpenCV device capturing an RTSP stream"
      Description="Broadcasting acquired video through OpenIGTLink"
    />
    <Device
      Id="VideoDevice"
      Type="OpenCVVideo"
      RequestedCaptureApi="CAP_FFMPEG"
      StreamURL="rtsp://127.0.0.1:8554/hello.sdp">
      <DataSources>
        <DataSource Type="Video" Id="Video" ImageType="RGB_COLOR" PortUsImageOrientation="MF"  />
      </DataSources>
      <OutputChannels>
        <OutputChannel Id="VideoStream" VideoDataSourceId="Video" />
      </OutputChannels>
    </Device>
  </DataCollection>

  <CoordinateDefinitions>
    <Transform From="Image" To="Reference"
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
      </MessageTypes>
      <ImageNames>
        <Image Name="Image" EmbeddedTransformToFrame="Reference" />
      </ImageNames>
    </DefaultClientInfo>
  </PlusOpenIGTLinkServer>

</PlusConfiguration>
```