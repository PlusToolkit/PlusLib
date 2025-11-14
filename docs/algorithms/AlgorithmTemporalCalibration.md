# Temporal calibration

Temporal calibration algorithms estimate time offset between data streams acquired by different devices.

Time difference between a hardware device acquires data and Plus receives the data is slightly different in each device.
This time difference can be up to a few hundred milliseconds (depending on the hardware devices, data acquisition settings,
computer hardware, connection type, network or CPU load, ...).

Hardware devices often do not provide an accurate timestamp for the acquisition of each frame, and even if they do,
the time reference (clock) of the various hardware devices are typically not synchronized. To compensate for these inaccuracies
and temporally synchronize data streams, Plus allows estimation and compensation of fixed time delays and can provide timestamp jitter
filtering.

# Time offset estimation

Temporal calibration method in Plus assumes that timestamps on the received data items have a constant time offset compared to other clocks.

The calibration algorithm requires data acquired while performing continuous linear quasi-periodic motion for at least
about 5 full periods (typically about 10-15 seconds). Although the algorithm may work for fewer periods it has not been tested
under these conditions. When an image stream is used the data assumed to be collected by an ultrasound probe imaging a planar object.
Furthermore, it is assumed that the probe is undergoing periodic motion in the direction perpendicular to the
plane's face (e.g., moving the probe in a repeating up-and-down fashion while imaging the bottom of a water bath).
The first step of the algorithm is the computation of a normalized position signal from each data stream.

Time offset can be computed between two devices at a time (using either tracking of imaging data). If there are more
devices then each device has to be included in at least one temporal calibration.

Notes:
- The time offset is determined by computing the time offset that leads to the highest correlation between the two signals.
  For the correlation computation the moving signal is linearly interpolated at the time positions
  where the fixed signal is known. As the acquisition rate of imaging data is typically lower than the acquisition rate
  of tracking data, it is preferable to use video as fixed signal and tracker as moving signal.
- The fixed and moving signal is cropped to the common time range. The moving signal is further cropped to the common range
  with `SetMaximumMovingLagSec` margin.
- The visual inspection of the aligned signals provides useful diagnostic information
  about the root cause of temporal misalignments, such as large occasional delays or varying acquisition rate.

# Timestamp jitter filtering

Timestamping on a personal computer with a non-real-time operating system is generally feasible with about
1ms precision when using multimedia timers. However, due to the non-real-time nature of general-purpose operating
systems, slight variations in the timing may occur due to hardware interrupts and varying processor load. Also,
some hardware devices transfer the data through Ethernet network, in which case there may be an additional
unpredictable delay before the data collector receives the data.

If the acquisition hardware device acquires data items at regular time intervals and reports missing data items
(e.g., by providing a monotonously increasing index for each frame), then it is possible to apply a timestamp
jitter filtering method that ensures regular time intervals between the generated timestamps. The filtering method
can also detect items with unreliable timestamps (items that are received with unusually long delay). In order to
prevent data corruption, a data item is discarded if the difference between the measured and filtered timestamp
is larger than a predefined threshold.

A line is fitted to the last N pairs of item indexes (independent variable) and unfiltered timestamps (measured variable)
using simple linear regression. The filtered timestamp is the time corresponding to the frame index according to the fitted line.

#### Configuration settings

  - **vtkPlusTemporalCalibrationAlgo**
    - **ClipRectangleOrigin** and **ClipRectangleSize**: Define a region to take into account when line detection is performed on images. Optional attribute, if not specified then the whole image is used.
    - **SaveIntermediateImages**: Controls if diagnostic information of line detection in images is written to files. (Optional, default: `FALSE`).
        - `FALSE` No debug information will be written.
        - `TRUE` Image files are written to the output directory that show the lines along image intensity is sampled and the detected line.
    - **SetMaximumMovingLagSec**: Maximum time lag that will be considered by the algorithm, in seconds. (Optional, default: `0.5`)

#### Example configuration file

```xml
<PlusConfiguration version="2.1">

  <DataCollection StartupDelaySec="1.0">
    <DeviceSet
      Name="fCal: Simulation mode for temporal calibration"
      Description="Replays a recorded sequence of imaging the bottom of a water tank. Image and tracking data is provided by separate devices." />

    <Device
      Id="TrackerDeviceSavedDataset"
      Type="SavedDataSource"
      UseData="TRANSFORM"
      AcquisitionRate="10"
      LocalTimeOffsetSec="0.0"
      SequenceFile="WaterTankBottomTranslationTrackerBuffer-trimmed.igs.mha"
      UseOriginalTimestamps="TRUE"
      RepeatEnabled="TRUE"
      ToolReferenceFrame="Tracker" >
      <DataSources>
        <DataSource Type="Tool" Id="Probe" />
        <DataSource Type="Tool" Id="Reference" />
        <DataSource Type="Tool" Id="Stylus" />
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
      Id="VideoDeviceSavedDataset"
      Type="SavedDataSource"
      UseData="IMAGE"
      AcquisitionRate="10"
      LocalTimeOffsetSec="0.0"
      SequenceFile="WaterTankBottomTranslationVideoBuffer.igs.mha"
      UseOriginalTimestamps="TRUE"
      RepeatEnabled="TRUE" >
      <DataSources>
        <DataSource Type="Video" Id="Video" PortUsImageOrientation="MF"  />
      </DataSources>
      <OutputChannels>
        <OutputChannel Id="VideoStream" VideoDataSourceId="Video" />
      </OutputChannels>
    </Device>

    <Device
      Id="TrackedVideoDevice"
      Type="VirtualMixer" >
      <InputChannels>
        <InputChannel Id="TrackerStream" />
        <InputChannel Id="VideoStream" />
      </InputChannels>
      <OutputChannels>
        <OutputChannel Id="TrackedVideoStream"/>
      </OutputChannels>
    </Device>

  </DataCollection>

  <CoordinateDefinitions>
    <Transform From="StylusTip" To="Stylus"
      Matrix="1 0 0 210.0
              0 1 0 0
              0 0 1 0
              0 0 0 1"
      Date="2011.12.01 17:22:12" />
    <Transform From="Image" To="TransducerOriginPixel"
      Matrix="1 0 0 -410
              0 1 0 5
              0 0 1 0
              0 0 0 1"
      Date="2011.12.06 17:57:00" Error="0.0" />
  </CoordinateDefinitions>

  <Rendering WorldCoordinateFrame="Reference" DisplayedImageOrientation="MF" >
    <DisplayableObject Type="Model" ObjectCoordinateFrame="TransducerOrigin" Id="ProbeModel"
      File="Probe_L14-5_38.stl"
      ModelToObjectTransform="
        -1 0 0 29.7
        0 -1 0 1.5
        0 0 1 -14
        0 0 0 1" />
    <DisplayableObject Type="Model" ObjectCoordinateFrame="Reference" Id="Volume"/>
    <DisplayableObject Type="Model" ObjectCoordinateFrame="StylusTip" Id="StylusModel" File="Stylus_Example.stl" />
    <DisplayableObject Id="PhantomModel" Type="Model" ObjectCoordinateFrame="Phantom"
      Opacity="0.6"
      File="fCal_1.0.stl"
      ModelToObjectTransform="
        1 0 0 -15.0
        0 1 0 10.0
        0 0 1 -5.0
        0 0 0 1" />
    <DisplayableObject Type="Image" ObjectCoordinateFrame="Image" Id="LiveImage"/>
  </Rendering>

  <Segmentation
    ApproximateSpacingMmPerPixel="0.078"
    MorphologicalOpeningCircleRadiusMm="0.37"
    MorphologicalOpeningBarSizeMm="2.0"
    ClipRectangleOrigin="215 41"
    ClipRectangleSize="368 510"
    MaxLinePairDistanceErrorPercent="10"
    AngleToleranceDegrees="10"
    MaxAngleDifferenceDegrees="10"
    MinThetaDegrees="-70"
    MaxThetaDegrees="70"
    MaxLineShiftMm="10.0"
    ThresholdImagePercent="10"
    CollinearPointsMaxDistanceFromLineMm="0.6"
    UseOriginalImageIntensityForDotIntensityScore="FALSE"
    NumberOfMaximumFiducialPointCandidates="20" />

  <PhantomDefinition>
    <Description
      Name="fCAL"
      Type="Multi-N"
      Version="1.0"
      WiringVersion="1.0"
      Institution="Queen's University PerkLab" />
    <Geometry>
      <Pattern Type="NWire">
        <Wire Name="1:E3_e3" EndPointFront="20.0 0.0 5.0" EndPointBack="20.0 40.0 5.0" />
        <Wire Name="2:F3_j3" EndPointFront="25.0 0.0 5.0" EndPointBack="45.0 40.0 5.0" />
        <Wire Name="3:K3_k3" EndPointFront="50.0 0.0 5.0" EndPointBack="50.0 40.0 5.0" />
      </Pattern>
      <Pattern Type="NWire">
        <Wire Name="4:E4_e4" EndPointFront="20.0 0.0 0.0" EndPointBack="20.0 40.0 0.0" />
        <Wire Name="5:J4_f4" EndPointFront="45.0 0.0 0.0" EndPointBack="25.0 40.0 0.0" />
        <Wire Name="6:K4_k4" EndPointFront="50.0 0.0 0.0" EndPointBack="50.0 40.0 0.0" />
      </Pattern>
      <Landmarks>
        <Landmark Name="#1" Position="95.0 5.0 15.0" />
        <Landmark Name="#2" Position="95.0 40.0 15.0" />
        <Landmark Name="#3" Position="95.0 40.0 0.0" />
        <Landmark Name="#4" Position="95.0 0.0 0.0" />
        <Landmark Name="#5" Position="-25.0 40.0 15.0" />
        <Landmark Name="#6" Position="-25.0 0.0 10.0" />
        <Landmark Name="#7" Position="-25.0 0.0 0.0" />
        <Landmark Name="#8" Position="-25.0 40.0 0.0" />
      </Landmarks>
    </Geometry>
  </PhantomDefinition>

  <VolumeReconstruction OutputSpacing="0.5 0.5 0.5"
    ClipRectangleOrigin="0 0" ClipRectangleSize="820 616"
    Interpolation="LINEAR" Optimization="FULL" CompoundingMode="MEAN" FillHoles="OFF" />

  <fCal
    PhantomModelId="PhantomModel"
    ReconstructedVolumeId="Volume"
    TransducerModelId="ProbeModel"
    StylusModelId="StylusModel"
    ImageDisplayableObjectId="LiveImage"
    NumberOfCalibrationImagesToAcquire="200"
    NumberOfValidationImagesToAcquire="100"
    NumberOfStylusCalibrationPointsToAcquire="200"
    RecordingIntervalMs="100"
    MaxTimeSpentWithProcessingMs="70"
    ImageCoordinateFrame="Image"
    ProbeCoordinateFrame="Probe"
    ReferenceCoordinateFrame="Reference"
    TransducerOriginCoordinateFrame="TransducerOrigin"
    TransducerOriginPixelCoordinateFrame="TransducerOriginPixel"
    TemporalCalibrationDurationSec="10"
    FixedChannelId="VideoStream"
    FixedSourceId="Video"
    MovingChannelId="TrackerStream"
    MovingSourceId="ProbeToTracker"
    DefaultSelectedChannelId="TrackedVideoStream"
    FreeHandStartupDelaySec="3" />

  <vtkPlusPivotCalibrationAlgo
    ObjectMarkerCoordinateFrame="Stylus"
    ReferenceCoordinateFrame="Reference"
    ObjectPivotPointCoordinateFrame="StylusTip" />

  <vtkTemporalCalibrationAlgo
    ClipRectangleOrigin="27 27"
    ClipRectangleSize="766 562" />

  <vtkPlusPhantomLandmarkRegistrationAlgo
    PhantomCoordinateFrame="Phantom"
    ReferenceCoordinateFrame="Reference"
    StylusTipCoordinateFrame="StylusTip" />

  <vtkPhantomLinearObjectRegistrationAlgo
    PhantomCoordinateFrame="Phantom"
    ReferenceCoordinateFrame="Reference"
    StylusTipCoordinateFrame="StylusTip" />

  <vtkPlusProbeCalibrationAlgo
    ImageCoordinateFrame="Image"
    ProbeCoordinateFrame="Probe"
    PhantomCoordinateFrame="Phantom"
    ReferenceCoordinateFrame="Reference" />

</PlusConfiguration>
```