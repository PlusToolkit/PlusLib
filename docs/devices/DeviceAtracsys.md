# Atracsys

## Supported hardware devices
- spryTrack 180
- fusionTrack 250
- fusionTrack 500

## Supported platforms

- [***Windows 64 bits***](/SupportedPlatforms/#windows-64-bits)
- [***Linux***](/SupportedPlatforms/#linux)

## Atracsys SDK Versions
Please build PLUS Atracsys support against the following Atracsys SDK versions. Other versions may work, but are not regularly tested by the PLUS developer team.

- spryTrack: RC11
- fusionTrack: 4.5.2

## Updating camera firmware
Please check the release notes for your version of the Atracsys SDK to ensure that these notes are current.

- spryTrack 180
    - Verify that all cables are free & firmly connected. Loss off power to the device during firmware update could cause memory corruption.
    - Open a command prompt and run the flash_firmware.bat (or flash_firmware.sh on Linux) script with the new firmware file as an argument.
    - The console will log the progress on uploading the new firmware to the device, do not disconnect until this process is complete.

- fusionTrack 250 or fusionTrack 500
    - Verify that all cables are free & firmly connected. Loss off power to the device during firmware update could cause memory corruption.
    - Connect the device to a Windows PC (it appears there is no Linux binary for flashing new firmware to the tracker).
    - Put the new firmware image in the /bin directory.
    - Turn on the power to the device, and then immediately start the atnet_64.exe binary. The updater is connected to the tracker when the trackers SN is printed to the console and the device status LED blinks blue.
    - In the atnet_x64 console, the following command will load the firmware image (where `<tracker_SN>` is replaced with the tracker’s serial #, and `<application_firmware_name>` is replaced with the firmware filename.
```
srec <tracker_SN> <application_firmware_name>.flash   i.e.   srec * App_fTk_1.1.6.65.flash
```
    - Next, we commit the firmware image to the fusionTrack memory. Be VERY careful to write only to region 1, writing to any other region could corrupt the device. Atracsys – there should be a confirmation dialog for any region other than 1. Replace `<size>` with the loaded firmware size printed to the console in the above step.
```
commit <tracker_SN> 1 <size>   i.e.   commit * 1 2408603
```
    - Wait for “–->OK” to be printed to the console to indicate a successful firmware update. Power the device off, and after waiting 5s power the tracker back on and verify the firmware version in demo_x[64|86].exe. Note: the firmware is printed with each pair of two numbers represented as hex digits (possibly with a missing leading zero).

## Building the Atracsys PLUS support

- To build the software, please download Atracsys' spryTrack SDK from the web link provided when you purchased the system. Alternatively, confirmed working files needed to build the support for spryTrack and fusionTrack systems on Windows & Linux are available in the PLTools repository.

## Creating a passive marker geometry
  - Included in the PLUS build bin directory is an executable called AtracsysMarkerCreator. It can be used to create .ini files describing the passive geometry of any IR reflective marker pattern (i.e. reflective spheres, infrared markers compatible with Atracsys trackers, circular reflective tape, etc).
  -To use the executable, you must provide at least the --marker-name, --description, and --geometryId arguments. geometryId must be unique among the markers you wish to track in a single PLUS config file. We recomment choosing unique numbers for every tool to prevent geometryId conflicts.

  Options are:
```
  --background-subtraction  Remove background fiducials from data considered by
                            the creator.

  --description=opt         Decsription of marker (i.e. purpose, color, size,
                            and any other desired metadata).

  --destination-path=opt    Where the generated marker geometry ini file will
                            be written to.

  --geometryId=opt          Id of the geometry we are creating. Must be unique.

  --help                    Print this help.

  --marker-name=opt         Name of marker.

  --num-frames=opt          Number of frames to use in generating marker
                            geometry ini file.

  --verbose=opt             Verbose level (1=error only, 2=warning, 3=info,
                            4=debug, 5=trace).
```

  - To use the background subtraction, provide the --background-subtraction argument when you launch the executable. It will collect background frames and then pause to allow you to position the marker you wish to create a geometry for in the camera's field of view.

## Using Atracsys active markers
  - At this time, there is only support in the Atracsys SDKs for use of the active markers with the spryTrack. Once support is available for using these markers with the fusionTrack we will update the PLUS support to use this feature.

## Common Issues
- If you aren't seeing any marker tracking information being sent over OpenIGTLink, please:
  - Ensure your geometry file (.ini) is correct for the tool you are using
  - Try increasing the MaxMeanRegistrationErrorMm, if there's more than this amount of error in matching the geometry to its corresponding reference file then transform containing this tool won't be sent over OpenIGTLink (this is to prevent transmission of inaccurate tracking information).

## Device configuration settings
  - **Type**: `AtracsysTracker`
  - **MaxMissingFiducials**: Max number of missing fiducials to still track a marker with. (Optional, default: `1`)
  - **MaxMeanRegistrationErrorMm**: Maximum error in fitting marker geometry to visible fiducials to consider a marker tracked. (Optional, default: `2.0`)
  - **ActiveMarkerPairingTimeSec**: Time tracker waits for active markers to pair before beginning tracking. (Optional, default: `0`)
  - **SymmetriseCoordinates**: Change the referential of the tracker from the left camera (`0`) to the center of the device (`1`). (Optional, default: `0`)
  - **EnableLasers**: Turn lasers on (`1`) or off (`0`). (Optional, default: `0`)
  - **ToolReferenceFrame**: (Optional, default: `Tracker`)
  - **DataSources**: (Required)
    - **DataSource**: (Required)
    - **TrackingType**: The type of marker this is.
        - `ACTIVE`
        - `PASSIVE`
    - **GeometryFile**: If this is a PASSIVE marker, provide the GeometryFile attribute with the path to the markers geometry file (.ini).
    - **GeometryId**: If the marker is an ACTIVE marker, provide the Atracsys assigned Id of the marker.

## Config File

```xml
<PlusConfiguration version="2.7">

  <DataCollection StartupDelaySec="1.0">
    <DeviceSet
      Name="PlusServer: Atracsys"
      Description="Broadcasting tracking data from Atracsys spryTrack180, fusionTrack250 or fusionTrack500 through OpenIGTLink."
    />
    <Device
      Id="TrackerDevice"
      Type="AtracsysTracker"
      MaxMissingFiducials="1"
      MaxMeanRegistrationErrorMm="1.0"
      ActiveMarkerPairingTimeSec="15"
      ToolReferenceFrame="Tracker" >
      <DataSources>
        <DataSource Type="Tool" Id="Stylus" TrackingType="PASSIVE" GeometryFile="AtracsysTools/stylus.ini" />
        <DataSource Type="Tool" Id="Reference" TrackingType="ACTIVE" GeometryId="371000" />
      </DataSources>
      <OutputChannels>
        <OutputChannel Id="TrackerStream">
        <DataSource Type="Tool" Id="Stylus" />
        <DataSource Type="Tool" Id="Reference" />
        </OutputChannel>
      </OutputChannels>
    </Device>
  </DataCollection>

  <PlusOpenIGTLinkServer
    MaxNumberOfIgtlMessagesToSend="1"
    MaxTimeSpentWithProcessingMs="50"
    ListeningPort="18944"
    SendValidTransformsOnly="TRUE"
    OutputChannelId="TrackerStream" >
    <DefaultClientInfo>
      <MessageTypes>
        <Message Type="TRANSFORM" />
      </MessageTypes>
      <TransformNames>
        <Transform Name="StylusToTracker" />
        <Transform Name="ReferenceToTracker" />
      </TransformNames>
    </DefaultClientInfo>
  </PlusOpenIGTLinkServer>

</PlusConfiguration>
```