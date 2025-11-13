# OptiTrack

## Supported hardware devices
- Any OptiTrack tracker which is compatible with Motive.
- OptiTrack trackers can be purchased at [www.optitrack.com](https://www.optitrack.com/)

## Supported platforms

- [***Windows 32 bits***](../SupportedPlatforms.md#windows-32-bits)
- [***Windows 64 bits***](../SupportedPlatforms.md#windows-64-bits)

## Installation
- For 32-bit PLUS or v2.6 and below: download and install [Motive 1.10.3](http://optitrack.com/downloads/).
- For 64-bit PLUS v2.8: download and install [Motive 2.1.1](http://optitrack.com/downloads/).
- For newer 64-bit versions: download and install [Motive 2.1.2](http://optitrack.com/downloads/).

Known limitation: Some web cameras and OptiTrack Duo cannot be used on the same computer. According to NaturalPoint support team, [this issue cannot be fixed](https://discourse.slicer.org/t/optitrack-duo-and-external-webcam-problem/12730/7).

## Important sections of OptiTrack Motive Documentation
  - Read the following sections of the [Motive User Manual](http://v110.wiki.optitrack.com/index.php?title=OptiTrack_Documentation_Wiki)
    - [Motive basics](http://v110.wiki.optitrack.com/index.php?title=Motive_Basics): This will give you a basic understanding of how the Motive software works.
    - [Calibration](http://v110.wiki.optitrack.com/index.php?title=Calibration): This process defines the positions of the cameras in multi-camera systems with respect to each other. This is not necessary if using a tracking bar camera.
    - [Rigid body tracking](http://v110.wiki.optitrack.com/index.php?title=Rigid_Body_Tracking): This section gives essential information about creating rigid bodies for tracking. It also provides helpful background on selecting good marker positions.

## Motive File Types
  - ***Profile file***: .xml files saved from Motive which contain the camera parameters, calibration (if using multi-camera systems) and rigid body definitions
  - ***Rigid body file*** .tra files saved from Motive which contain the definition of a single rigid body asset

## Setting up Tracking
- ***Simple Configuration*** (using only a Motive project file)
    - 1: Run Motive and perform a calibration
    - 2: In Motive's 3D view, select all the markers corresponding to a rigid body and right click. Cursor "Rigid Body" then click "Create From Selected Markers".
    - 3: Select the newly created rigid body in the (`Project`) tab and change the rigid body's (`Name`) attribute to something descriptive of the rigid body. This will be the rigid body's ID in the PLUS config file.
    - 4: Repeat steps 2 & 3 for each rigid body to be tracked.
    - 5: Save the Motive project to the same directory as the PLUS config file using File->Save Motive Project As.
    - 6: Modify the sample PLUS calibration file PlusDeviceSet_Server_OptiTrack_Profile.xml to reflect the name of your Motive profile file and update the DataSources element to contain one entry for each rigid body (with the same ID as the rigid body's Motive name attribute).
- ***Flexible Configuration*** (using a project file for the calibration, providing the rigid bodies as TRA files)
    - 1: Run Motive and perform a calibration
    - 2: Save the calibration as a Motive profile xml file.
    - 2: In Motive's 3D view, select all the markers corresponding to a rigid body and right click. Cursor "Rigid Body" then click "Create From Selected Markers".
    - 3: Select the newly created rigid body in the (`Project`) tab and change the rigid body's (`Name`) attribute to something descriptive of the rigid body. This will be the rigid body's ID in the PLUS config file.
    - 4: Right click the rigid body's name in the (`Project`) tab and click "Export Rigid Body". Save the resulting file in the PLUS config file directory. We recommend naming this file `<Motive-name>.tra`, replacing `<Motive-name>` with the name attribute of the rigid body.
    - 6: Modify the sample PLUS calibration file PlusDeviceSet_Server_OptiTrack_AddMarkersUsingTRA.xml to reflect the name of your Motive profile xml file and update the DataSources element to contain one entry for each rigid body (with the same ID as the rigid body's Motive name attribute, and TRA file containing the relative path of the rigid body's associated TRA file).

## Device configuration settings
- **DeviceType**: Must be `OptiTrack`. (Required)
- **ToolReferenceFrame**: (Optional, default: `Tracker`)
- **ProjectFile**: The Motive project file (.TTP) to load (Required)
- **AttachToRunningMotive**: Set to `TRUE` to make Plus connect to an already running instance of Motive application (on the same computer where Plus is running). If set to `FALSE`, PLUS will internally start Motive application with its graphical user interface hidden.
- **MotiveDataDescriptionsUpdateTimeSec**: Only used if Motive GUI is shown. Defines time how frequently list of tools is queried from Motive. Each update consumes a small amount of memory, therefore it is recommended to use >=0 value only during development. If set to a negative value, tool descriptions are only updated when connecting to the device.
- **DataSources**: One `DataSource` child element is required for each rigid body to be tracked. (Required)
    - **DataSource**: (Required)
        - **Id**: The rigid body name. The same name should be assigned to the rigid body in Motive. (Required)
        - **RigidBodyFile**: The filename of the TRA file that defines the rigid body. (Optional)

## Simple Configuration

### Example configuration file with all rigid bodies saved in the Motive profile xml file (PlusDeviceSet_Server_OptiTrack_Profile.xml)

```xml
<PlusConfiguration version="2.7">

  <DataCollection StartupDelaySec="1.0">
    <DeviceSet
      Name="PlusServer: OptiTrack (Profile only)"
      Description="Broadcasting tracking data through OpenIGTLink."
    />
    <Device
      Id="TrackerDevice"
      Type="OptiTrack"
      ToolReferenceFrame="Tracker"
      Profile="OptiTrack/SampleProfile.xml"
      Calibration="OptiTrack/SampleCalibration.cal"
      AttachToRunningMotive="FALSE"
      MotiveDataDescriptionsUpdateTimeSec="1.0" >
      <DataSources>
        <DataSource Type="Tool" Id="Stylus" />
        <DataSource Type="Tool" Id="Reference" />
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

## Flexible Configuration

### Example configuration file with rigid bodies supplied as .TRA files (PlusDeviceSet_Server_OptiTrack_AddMarkersUsingTRA.xml)

```xml
<PlusConfiguration version="2.7">

  <DataCollection StartupDelaySec="1.0">
    <DeviceSet
      Name="PlusServer: OptiTrack (Profile file and additional rigid body TRA files)"
      Description="Broadcasting tracking data through OpenIGTLink."
    />
    <Device
      Id="TrackerDevice"
      Type="OptiTrack"
      ToolReferenceFrame="Tracker"
      Profile="OptiTrack/EmptyProfile.xml"
      AttachToRunningMotive="FALSE"
      MotiveDataDescriptionsUpdateTimeSec="1.0" >
      <DataSources>
        <DataSource Type="Tool" Id="Stylus" RigidBodyFile="OptiTrackStylus.tra" />
        <DataSource Type="Tool" Id="Reference" RigidBodyFile="OptiTrack/Reference.tra" />
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
