# Ascension electromagnetic tracking devices (and Ultrasonix SonixGPS)

## Supported hardware devices

- Ascension trakSTAR, driveBAY, medSAFE models.
- Ultrasonix SonixGPS (it is an Ascension driveBAY).
- Link to manufacturer's website: <http://www.ascension-tech.com/medical/index.php>

## Supported platforms

- [***Windows 32 bits***](/SupportedPlatforms/#windows-32-bits)
- [***Windows XP embedded 32 bits***](/SupportedPlatforms/#windows-xp-embedded-32-bits)

Support for medSAFE models is not included in any of the standard packages, as they use different versions of the same DLLs as non-medSAFE trackers.

To support medSAFE models Plus have to be built with PLUS_USE_Ascension3DGm configuration option enabled as described at <https://github.com/PlusToolkit/PlusBuild/blob/master/Docs/BuildInstructionsWindows.md>.

## Installation

- Sensors must be all located in front of the frontplate of the field generator (the side with large the Ascension motif on it): tools are still tracked behind the frontplate, but their orientation may be inverted.
- If pose measurement seems to be incorrect (somewhat translated/rotated) then unplug the sensor and plug it in again.
- Tracking a Stylus: It is recommended to place the EM sensor as close to the needle tip as possible. The best accuracy is expected from a thick needle with a sensor near the needle tip.
- If you are using the tracker of an Ultrasonix SonixGPS system and the Exam software is running (e.g., because images are acquired using the Ulterius interface), then you need to disable the SonixGPS license to prevent the Exam software from using the tracker. This is done because only one application can communicate with the tracker at a time (either your application or the Exam software). To disable SonixGPS press menu on the touchscreen. Go to Administrator, Licenses and unselect SonixGPS from the licenses menu.

## Device configuration settings

  - **Type**: `Ascension3DG`

  - **AcquisitionRate**: (Optional, default: `50`)
  - **LocalTimeOffsetSec**: (Optional, default: `0`)
  - **ToolReferenceFrame**: (Optional, default: `Tracker`)


  - **FilterAcWideNotch**: AC wide notch filter status flag.
  - **FilterAcNarrowNotch**: AC narrow notch filter status flag.
  - **FilterDcAdaptive**: DC adaptive filter parameter
  - **FilterLargeChange**: Large change filter status flag.
  - **FilterAlpha**: Alpha filter status flag.
  - **Hemisphere**: Defines in which hemisphere, centered about the transmitter, the sensors are located. (   - `FRONT` The FRONT is the forward hemisphere in front of the transmitter. The front of the transmitter is the side with the Ascension logo molded into the case. It is the side opposite the side with the 2 positioning holes. This is the default.
   - `BACK` The BACK is the opposite hemisphere to the FRONT hemisphere.
   - `TOP` The TOP hemisphere is the upper hemisphere. When the transmitter is sitting on a flat surface with the locating holes on the surface the TOP hemisphere is above the transmitter.
   - `BOTTOM` The BOTTOM hemisphere is the opposite hemisphere to the TOP hemisphere.
   - `LEFT` The LEFT hemisphere is the hemisphere to the left of the observer when looking at the transmitter from the back.
   - `RIGHT` The RIGHT hemisphere is the opposite hemisphere to the LEFT hemisphere. The LEFT hemisphere is on the left side of the observer when looking at the transmitter from the back.
  - **DataSources**: One `DataSource` child element for each tool. (Required)
     - **DataSource**: (Required)
      - **Type**: = ` (`Tool`)` (Required)

      - **PortName**: Identifies the connector
      - **QualityErrorSlope**: The slope should have a value between -127 and +127.
      - **QualityErrorOffset**: default: The offset should have a value between -127 and +127. (Optional, default: 0)
      - **QualityErrorSensitivity**: The sensitivity should have a value between 0 and 127 (Optional, default: 2)
      - **QualityFilterAlpha**: The alpha should have a value between 0 and 127. (Optional, default: 12)
      - **BufferSize**: (Optional, default: 150)
      - **AveragedItemsForFiltering**: (Optional, default: 20)

## Quality values

The tool ports `quality1,` `quality2,` and `quality3`  provide the quality values for all the tools.

The quality numbers are stored in the translation component of the transformation matrix. If a value is not available then it is set to -1.
- `quality1:` quality values for sensor port 0-2.
- `quality2:` quality values for sensor port 3-5 (there are 4 ports on one control unit, so quality values for port 4 and 5 may not be available).
- `quality3:` quality values for sensor port 6-7 (there are 8 ports if two control units are used, so quality values for port 8 and 9 may not be available).

## Example configuration file PlusDeviceSet_Server_Ascension3DG.xml

```xml
<PlusConfiguration version="2.3">

  <DataCollection StartupDelaySec="1.0" >
    <DeviceSet 
      Name="PlusServer: Ascension 3DG tracker"
      Description="Broadcasting tool tracking data through OpenIGTLink
Ascension3DG sensors should be plugged in to the Ascension3DG DriveBay mounted on Ultrasonix US in the following order from to leftmost slot (Transducer 1) to the right: 1 Probe, 2 Reference, 3 Stylus." 
    />
    <Device
      Id="TrackerDevice" 
      Type="Ascension3DG" 
      FilterAcWideNotch="1"
      ToolReferenceFrame="Tracker" >
      <DataSources>
        <DataSource Type="Tool" Id="Probe" PortName="0"  />
        <DataSource Type="Tool" Id="Reference" PortName="1"  />
        <DataSource Type="Tool" Id="Stylus" PortName="2"  />
      </DataSources>
      <OutputChannels>
        <OutputChannel Id="TrackerStream" >
          <DataSource Id="Probe"/>
          <DataSource Id="Reference"/>
          <DataSource Id="Stylus"/>
        </OutputChannel>
      </OutputChannels>
    </Device>
    <Device
      Id="CaptureDevice"
      Type="VirtualCapture"
      BaseFilename="RecordingTest.igs.mha"
      EnableCapturingOnStart="FALSE" >
      <InputChannels>
        <InputChannel Id="TrackerStream" />
      </InputChannels>
    </Device>
  </DataCollection>

  <PlusOpenIGTLinkServer 
    MaxNumberOfIgtlMessagesToSend="1" 
    MaxTimeSpentWithProcessingMs="50" 
    ListeningPort="18944" 
    OutputChannelId="TrackerStream" > 
    <DefaultClientInfo> 
      <MessageTypes> 
        <Message Type="TRANSFORM" />
      </MessageTypes>
      <TransformNames> 
        <Transform Name="ProbeToTracker" />
        <Transform Name="ReferenceToTracker" />
        <Transform Name="StylusToTracker" />
      </TransformNames>
    </DefaultClientInfo>
  </PlusOpenIGTLinkServer>

</PlusConfiguration>
```