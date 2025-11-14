# Philips ultrasound systems

This class acquires 4D (3D+t) image data from Philips ultrasound systems.

## Supported hardware devices

- Philips iE33 3D ultrasound imaging devices.
- Link to manufacturer's website: <http://www.healthcare.philips.com/main/products/ultrasound/systems/ie33/>

## Supported platforms

- [***Windows 32 bits***](/SupportedPlatforms/#windows-32-bits)

This device is not included in any of the standard packages, as it requires the Philips API, which is not available publicly.
If the software is obtained from Philips then Plus have to be built with PLUS_USE_PHILIPS_3D_ULTRASOUND.

## Device configuration settings

- **Type**: `iE33Video` (Required)
- **AcquisitionRate**: (Optional, default: `30`)
- **LocalTimeOffsetSec**: (Optional, default: `0`)
- **IPAddress**: Network address of the ultrasound system
- **Port**: Network port of the ultrasound system
- **ForceZQuantize**: Parameter sent to the Philips stream manager (Optional, default: `FALSE`)
- **ResolutionFactor**: Parameter sent to the Philips stream manager (Optional, default: `2.5`)
- **IntegerZ**: Parameter sent to the Philips stream manager (Optional, default: `TRUE`)
- **Isotropic**: Parameter sent to the Philips stream manager (Optional, default: `FALSE`)
- **QuantizeDim**: Parameter sent to the Philips stream manager (Optional, default: `TRUE`)
- **ZDecimation**: Parameter sent to the Philips stream manager (Optional, default: `2`)
- **Set4PtFIR**: Parameter sent to the Philips stream manager (Optional, default: `TRUE`)
- **LatAndElevSmoothingIndex**: Parameter sent to the Philips stream manager (Optional, default: `4`)
- **DataSources**: Exactly one `DataSource` child element is required (Required)
    - **DataSource**: (Required)
        - **PortUsImageOrientation**: Only (`AMF`) is supported. (Required)
        - **BufferSize**: (Optional, default: `150`)
        - **AveragedItemsForFiltering**: (Optional, default: `20`)
        - **ClipRectangleOrigin**: (Optional, default: `0 0 0`)
        - **ClipRectangleSize**: (Optional, default: `0 0 0`)

## Example configuration file PlusDeviceSet_Server_Philips_ie33.xml

```xml
<PlusConfiguration version="2.0">

  <DataCollection StartupDelaySec="1.0" >
    <DeviceSet
      Name="PlusServer: Philips IE33 3D Probe"
      Description="Broadcasting acquired 3D ultrasound video acquired on the Philips IE33 system through OpenIGTLink" />
    <Device
      Id="VideoDevice"
      Type="iE33Video"
      IPAddress="129.100.44.8"
      Port="4013" >
      <DataSources>
        <DataSource Type="Video" Id="Video" PortUsImageOrientation="AMF"  />
      </DataSources>
      <OutputChannels>
        <OutputChannel Id="VideoStream" VideoDataSourceId="Video" />
      </OutputChannels>
    </Device>

    <Device
      Id="CaptureDevice"
      Type="VirtualCapture"
      BaseFilename="ie33Philips3DCapture.igs.nrrd"
      EnableCapturing="FALSE" >
      <InputChannels>
        <InputChannel Id="VideoStream" />
      </InputChannels>
    </Device>
  </DataCollection>

  <CoordinateDefinitions>
    <Transform From="Image" To="Probe"
      Matrix="1 0 0 0
              0 1 0 0
              0 0 1 0
              0 0 0 1 "
       Error="0" Date="072915_125658" />
    <Transform From="StylusTip" To="Stylus"
      Matrix="
        0.999866	0	0.0163861	1.30947
        -0.00012887	0.999969	0.00786353	0.628403
        -0.0163856	-0.00786459	0.999835	79.9003
        0	0	0	1"
       Error="0.129734" Date="072415_125525" />
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
        <Image Name="Image" EmbeddedTransformToFrame="Probe" />
      </ImageNames>
    </DefaultClientInfo>
  </PlusOpenIGTLinkServer>

</PlusConfiguration>
```