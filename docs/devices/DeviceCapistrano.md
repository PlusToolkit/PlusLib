# Capistrano Labs USB ultrasound systems

## Supported hardware devices

- This device is used for image acquisition from Capistrano Labs USB ultrasound
  systems.
- Link to manufacturer's website: <http://www.capolabs.com/>

## Supported platforms

- [***Windows 32 bits***](../SupportedPlatforms.md#windows-32-bits)
- [***Windows 64 bits***](../SupportedPlatforms.md#windows-64-bits)

## Installation

- Requires SDK provided by Capistrano Labs for compilation. Windows 64 bits platform only supported by cSDK2018 and newer
  - cSDK2023:   BmodeUSB DLL v317, USBprobe DLL v164
  - cSDK2019.3: BmodeUSB DLL v316, USBprobe DLL v159
  - cSDK2019.2: BmodeUSB DLL v315, USBprobe DLL v157
  - cSDK2019  : BmodeUSB DLL v314, USBprobe DLL v154
  - cSDK2018  : BmodeUSB DLL v314, USBprobe DLL v151
  - cSDK2016  : BmodeUSB DLL v310, USBprobe DLL v127
  - cSDK2013  : BmodeUSB DLL v309, USBprobe DLL v126
- Requires USB drivers provided by Capistrano Labs for running. There are specific USB drivers for each cSDK version.
  - If using cSDK2019.3, cSDK2019.2, cSDK2019, cSDK2018 or cSDK2016, Plus application will not start due to missing WDAPI1140.dll
  - If using cSDK2013, Plus application will not start due to missing WDAPI1010.dll

## Device configuration settings

  - **Type**: `CapistranoVideo`
  - **ImageSize**: Image size in pixels
  - **DataSources**: Exactly one `DataSource` child element is required (Required)
      - **DataSource**: (Required)
          - **PortUsImageOrientation**: (Required)
          - **ImageType**: Supported imaging modes: B-mode (Optional, default: `BRIGHTNESS`)
          - **BufferSize**: (Optional, default: `150`)
          - **AveragedItemsForFiltering**: (Optional, default: `20`)
          - **ClipRectangleOrigin**: (Optional, default: `0 0 0`)
          - **ClipRectangleSize**: (Optional, default: `0 0 0`)

## Example configuration file PlusDeviceSet_CapistranoVideoSourceTest.xml

```xml
<PlusConfiguration version="2.3">

 <DataCollection StartupDelaySec="1.0" >
    <DeviceSet
      Name="TEST Data collection for CapistranoVideo Device and PlusOpenIGTLinkServer at default address"
      Description="Defines CapistranoVideo and " />
    <Device
      Id="VideoDevice"
      Type="CapistranoVideo"
      UpdateParameters="TRUE"
      BidirectionalMode="FALSE"
      CineBuffers="64"
      Interpolate="TRUE"
      AverageMode="FALSE"
      BModeViewOption="4"
      JitterCompensation="60"
      LutCenter="125"
      LutWindow="250">
      <UsImagingParameters>
        <Parameter name="FrequencyMhz" value="35.0"/>
        <Parameter name="Voltage" value="50"/>
        <Parameter name="SoundVelocity" value="1532"/>
        <Parameter name="DepthMm" value="54"/>
        <Parameter name="ImageSize" value="550 700"/>
        <Parameter name="Intensity" value="120"/>
        <Parameter name="Contrast" value="240"/>
        <Parameter name="GainPercent" value="50 60 70"/>
      </UsImagingParameters>
      <DataSources>
        <DataSource Type="Video" Id="Video" PortUsImageOrientation="NU"/>
      </DataSources>
      <OutputChannels>
        <OutputChannel Id="VideoStream" VideoDataSourceId="Video" />
      </OutputChannels>
    </Device>
  </DataCollection>

  <PlusOpenIGTLinkServer
    ListeningPort="18944"
    OutputChannelId="VideoStream" >
    <DefaultClientInfo>
      <MessageTypes>
        <Message Type="IMAGE" />
      </MessageTypes>
      <ImageNames>
        <Image Name="US" EmbeddedTransformToFrame="Reference" />
      </ImageNames>
    </DefaultClientInfo>
  </PlusOpenIGTLinkServer>

  <CoordinateDefinitions>
    <Transform From="Reference" To="US"
      Matrix="
        1.0 0.0 0.0 100.0
        0.0 1.0 0.0 200.0
        0.0 0.0 1.0 300.0
        0.0 0.0 0.0 1.0"
      Date="2016.01.06 17:57:00" />
  </CoordinateDefinitions>

</PlusConfiguration>
```