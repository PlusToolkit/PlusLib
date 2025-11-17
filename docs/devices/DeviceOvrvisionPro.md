# OvrvisionPro stereo camera

## Supported hardware devices

- Support for the <http://ovrvision.com/entop/>Ovrvision Pro USB3.0 Stereo Camera</a>.

## Supported platforms

- [***Windows 32 bits***](../SupportedPlatforms.md#windows-32-bits)
- [***Windows 64 bits***](../SupportedPlatforms.md#windows-64-bits)

The device manufacturer supports additional platforms. On request, Plus toolkit will be extended to support this device on other platforms, too.

## Installation

Need to install the drivers and software from [VGA2USB-LR download](http://www.epiphan.com/products/frame-grabbers/vga2usb-lr/download/).

## Device configuration settings

- **Type**: `OvrvisionPro`
- **DirectShowFilterID**: Defines the filter ID that corresponds to this device. Windows Only.
- **Resolution**: and **Framerate** define the requested capture format of the camera. Typical value is `640x480@30`. Supported values are (USB3) `2560x1920@15fps`, `1920x1080@30fps`, `1280x960@45fps`, `960x950@60fps`, `1280x800@60fps`, `640x480@90fps`, `320x240@120fps`, (USB2) `1280x960@15fps`, `640x480@30fps` (Required)
- **CameraSync**: Defines whether to keep the stereo camera acquisition in sync or not
- **Vendor**: Identifies which vendor of GPU is available in the computer. Supported values must match [clGetPlatformInfo]
- **ProcessingModeName**: Defines what post-processing to perform on the image. Supported values are `OV_CAMQT_DMSRMP`, `OV_CAMQT_DMS`, `OV_CAMQT_NONE` (Optional, default: `OV_CAMQT_NONE`)
- **LeftEyeDataSourceName**: Identifies which data source is for the left eye (Required)
- **RightEyeDataSourceName**: Identifies which data source is for the right eye (Required)
- **DataSources**: Exactly two `DataSource` child elements are required. (Required)
    - **DataSource**: (Required)
        - **PortUsImageOrientation**: (Required)
        - **ImageType**: Color image acquisition is mandatory, value must be `RGB_COLOR` (Required)
        - **BufferSize**: (Optional, default: `150`)
        - **AveragedItemsForFiltering**: (Optional, default: `20`)
        - **ClipRectangleOrigin**: Software clipping, applied on top of the hardware clipping region. (Optional, default: `0 0 0`)
        - **ClipRectangleSize**: Software clipping, applied on top of the hardware clipping region. (Optional, default: `0 0 0`)

## Example configuration file PlusDeviceSet_Server_OvrvisionProVideoCapture.xml

```xml
<PlusConfiguration version="2.1">

  <DataCollection StartupDelaySec="1.0" >
    <DeviceSet
      Name="PlusServer: OvrvisionPro stereo video capture device"
      Description="Broadcasting acquired video through OpenIGTLink"
    />
    <Device
      Id="VideoDevice"
      Type="OvrvisionPro"
      RequestedFormat="OV_CAMVR_FULL"
      CameraSync="TRUE"
      Exposure="7808"
      Vendor="NVIDIA Corporation"
      ProcessingModeName="OV_CAMQT_DMS"
      LeftEyeDataSourceName="LeftVideo"
      RightEyeDataSourceName="RightVideo" >
      <DataSources>
        <DataSource Type="Video" Id="LeftVideo" PortUsImageOrientation="MN" ImageType="RGB_COLOR" />
        <DataSource Type="Video" Id="RightVideo" PortUsImageOrientation="MN" ImageType="RGB_COLOR" />
      </DataSources>
      <OutputChannels>
        <OutputChannel Id="LeftVideoStream" VideoDataSourceId="LeftVideo" />
        <OutputChannel Id="RightVideoStream" VideoDataSourceId="RightVideo" />
      </OutputChannels>
    </Device>
  </DataCollection>

  <CoordinateDefinitions>
    <Transform From="LeftImage" To="Reference"
      Matrix="
        1 0 0 0
        0 1 0 0
        0 0 1 0
        0 0 0 1" />
    <Transform From="RightImage" To="Reference"
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
    OutputChannelId="LeftVideoStream" >
    <DefaultClientInfo>
      <MessageTypes>
        <Message Type="IMAGE" />
      </MessageTypes>
      <ImageNames>
        <Image Name="LeftImage" EmbeddedTransformToFrame="Reference" />
      </ImageNames>
    </DefaultClientInfo>
  </PlusOpenIGTLinkServer>

  <PlusOpenIGTLinkServer
    MaxNumberOfIgtlMessagesToSend="1"
    MaxTimeSpentWithProcessingMs="50"
    ListeningPort="18945"
    SendValidTransformsOnly="true"
    OutputChannelId="RightVideoStream" >
    <DefaultClientInfo>
      <MessageTypes>
        <Message Type="IMAGE" />
      </MessageTypes>
      <ImageNames>
        <Image Name="RightImage" EmbeddedTransformToFrame="Reference" />
      </ImageNames>
    </DefaultClientInfo>
  </PlusOpenIGTLinkServer>
</PlusConfiguration>
>