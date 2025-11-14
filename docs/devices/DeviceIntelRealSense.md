# IntelRealSense camera
This device allow you to process two kind of images. The first image is in RGB and the second image is a Depth image.

## Supported hardware devices

- Link to manufacturer's website: <https://software.intel.com/en-us/realsense/home>

## Supported platforms

- [***Windows 32 bits***](/SupportedPlatforms/#windows-32-bits)
- [***Windows 64 bits***](/SupportedPlatforms/#windows-64-bits)

Requires recent Intel processor.

## License

This module allow you to use the IntelRealSense suite of combined RGB and Depth cameras.

Developed by MACBIOIDI-ULPGC & IACTEC-IAC group

```
The MIT License (MIT)

Copyright (c) 2017 Eavise Research Group

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
```

## Installation
- Install IntelRealSense SDK 2.0 or higer (<https://github.com/IntelRealSense/librealsense/releases/tag/v2.11.0>)

## Device configuration settings

  - **Type**: `IntelRealSense` (Required)
  - **UseRealSenseColorizer** Choose whether or not to use the RealSense colorized or send raw depth data. (Optional, default: `FALSE`)
  - **AlignDepthStream** Choose whether to align RGB and depth streams. You must have both and RGB and a depth stream in your config to enable this option. (Optional, default: `FALSE`)
  - **DataSources**: One `DataSource` child element is required per stream from the RealSense. (Required)
    - **DataSource**: (Required)
        - **FrameType** Type of stream to capture. (Required)
            - `RGB`
            - `DEPTH`
        - **FrameSize** Size of the video stream in pixels. Consult the RealSense documentation for your device to find appropriate frame size / frame rate combinations.
        - **FrameRate** Acquisition frequence for this stream.
        - **PortUsImageOrientation**: (Optional, default: `UN`)

## Example configuration file PlusDeviceSet_Server_IntelRealSenseVideo.xml

```xml
<PlusConfiguration version="2.3">

  <DataCollection StartupDelaySec="1.0" >
    <DeviceSet
      Name="PlusServer: Intel RealSense camera "
      Description="Broadcasting acquired RGB and Depth video through PlusServer from Intel RealSense 3D camera"
	/>

    <Device
      Id="VideoDevice"
      Type="IntelRealSense"
      UseRealSenseColorizer="TRUE"
      AlignDepthStream="TRUE">
      <DataSources>
        <DataSource Type="Video" Id="VideoRGB" FrameType="RGB" FrameSize="640 480" FrameRate="30" PortUsImageOrientation="UN" />
        <DataSource Type="Video" Id="VideoDEPTH" FrameType="DEPTH" FrameSize="640 480" FrameRate="30" PortUsImageOrientation="UN" />
      </DataSources>
      <OutputChannels>
        <OutputChannel Id="VideoStreamRGB" VideoDataSourceId="VideoRGB" />
        <OutputChannel Id="VideoStreamDEPTH" VideoDataSourceId="VideoDEPTH" />
      </OutputChannels>
    </Device>
  </DataCollection>

  <PlusOpenIGTLinkServer
    MaxNumberOfIgtlMessagesToSend="1"
    MaxTimeSpentWithProcessingMs="50"
    ListeningPort="18944"
    SendValidTransformsOnly="true"
    OutputChannelId="VideoStreamRGB" >
    <DefaultClientInfo>
      <MessageTypes>
        <Message Type="IMAGE" />
      </MessageTypes>
      <ImageNames>
        <Image Name="ImageRGB" EmbeddedTransformToFrame="ImageRGB" />
      </ImageNames>
    </DefaultClientInfo>
  </PlusOpenIGTLinkServer>
  <PlusOpenIGTLinkServer
    MaxNumberOfIgtlMessagesToSend="1"
    MaxTimeSpentWithProcessingMs="50"
    ListeningPort="18945"
    SendValidTransformsOnly="true"
    OutputChannelId="VideoStreamDEPTH" >
    <DefaultClientInfo>
      <MessageTypes>
        <Message Type="IMAGE" />
      </MessageTypes>
      <ImageNames>
        <Image Name="ImageDEPTH" EmbeddedTransformToFrame="ImageDEPTH" />
      </ImageNames>
    </DefaultClientInfo>
  </PlusOpenIGTLinkServer>

</PlusConfiguration>
```