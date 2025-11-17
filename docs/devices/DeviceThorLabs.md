# ThorLabs compact spectrometer

## Supported hardware devices

ThorLabs compact CCD Spectrometers.

Manufacturer's website: http://www.thorlabs.com/newgrouppage9.cfm?objectgroup_id=3482

## Supported platforms

- [***Windows 32 bits***](../SupportedPlatforms.md#windows-32-bits)
- [***Windows 64 bits***](../SupportedPlatforms.md#windows-64-bits)

## Installation

Thorlabs "full installer for our GUI control software for our OSA and CCD spectrometers" has to be installed.

## Output

The device provides its output as an image. The pixels of the image are double-precision floating point numbers.
The first row of the image contains wavelength values in nm. The second row of the image contains intensity values.

## Device configuration settings

- **Type**: `ThorLabsVideo`
- **AcquisitionRate**: (Optional, default: `30`)
- **LocalTimeOffsetSec**: (Optional, default: `0`)
- **IntegrationTimeSec**: Sensor integration time. Longer time makes the sensor more sensitive and reduces the measurement noise, at the cost of increased acquisition time and higher chance of saturation at higher intensities.
- **DataSources**: Exactly one `DataSource` child element is required (Required)
    - **DataSource**: (Required)
        - **BufferSize**: (Optional, default: `150`)
        - **AveragedItemsForFiltering**: (Optional, default: `20`)
        - **ClipRectangleOrigin**: Three integer values. Specify if only a part of the spectrum should be acquired. First parameter specifies the starting point of the spectrum, in sensor index value (between 0 and the number of sensor pixel elements). Second parameter is 0 if wavelength values will be included, 1 if wavelength values will be excluded. Third parameter must be 0. (Optional, default: `0 0 0`)
        - **ClipRectangleSize**: Three integer values. Specify if only a part of the spectrum should be acquired. First parameter specifies the size of the acquired spectrum, in sensor index value (between 1 and the number of sensor pixel elements). Second parameter is 1 if only wavelength or intensity values will be included, 2 if both wavelength and intensity values will be included. Third parameter must be 1. If all parameters are zero (default) then no clipping is performed. (Optional, default: `0 0 0`)

## Example for clipping the acquired spectrum

## Example configuration file PlusDeviceSet_Server_ThorLabsVideoCapture.xml

```xml
<PlusConfiguration version="2.0">

  <DataCollection StartupDelaySec="2.0" >
    <DeviceSet
      Name="PlusServer: ThorLabs spectrometer"
      Description="Broadcasting acquired spectrum through OpenIGTLink. First image row contains wavelength value, second image row contains intensities." />
    <Device
      Id="VideoDevice"
      Type="ThorLabsVideo" >
      <DataSources>
        <DataSource Type="Video" Id="Video" />
      </DataSources>
      <OutputChannels>
        <OutputChannel Id="VideoStream" VideoDataSourceId="Video" />
      </OutputChannels>
    </Device>
    <Device
      Id="CaptureDevice"
      Type="VirtualCapture"
      BaseFilename="RecordingTest.igs.mha"
      EnableCapturingOnStart="FALSE" >
      <InputChannels>
        <InputChannel Id="VideoStream" />
      </InputChannels>
    </Device>
  </DataCollection>

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
        <Image Name="Image" EmbeddedTransformToFrame="Image" />
      </ImageNames>
    </DefaultClientInfo>
  </PlusOpenIGTLinkServer>

</PlusConfiguration>
```