# Probe calibration algorithm

This algorithm determines the transformation between the ultrasound image and a tracker marker rigidly attached to somewhere on the ultrasound probe.

## Configuration settings

  - **vtkPlusProbeCalibrationAlgo**
    - **ImageCoordinateFrame**: (Required)
    - **ProbeCoordinateFrame**: (Required)
    - **PhantomCoordinateFrame**: (Required)
    - **ReferenceCoordinateFrame**: (Required)
    - **OptimizationMethod**: Additional optimization step to force the computed ImageToProbe matrix to be orthogonal. Usually 2D method provides slightly better results. See this paper for details: <http://perk.cs.queensu.ca/contents/improving-n-wire-phantom-based-freehand-ultrasound-calibration> (Optional, default: `NONE`)
        - `2D` Distance of actual and expected fiducial line intersection point is minimized in the image plane.
        - `3D` Distance of actual fiducial point is and the fiducial line is minimized in 3D.
    - **IsotropicPixelSpacing**: Specifies if during optimization an isotropic horizontal and vertical spacing in the image is enforced. Only used if `OptimizationMethod` is not `NONE` (Optional, default: `FALSE`)

  - **Segmentation**: Segmentation and pattern recognition parameters. Can be checked and modified using SegmentationParameterDialogTest or fCal (FreehandClibration toolbox) applications
    - **ApproximateSpacingMmPerPixel**
    - **MorphologicalOpeningCircleRadiusMm**
    - **MorphologicalOpeningBarSizeMm**
    - **ClipRectangleOrigin**
    - **ClipRectangleSize**
    - **MaxLinePairDistanceErrorPercent**
    - **AngleToleranceDegrees**
    - **MaxAngleDifferenceDegrees**
    - **MinThetaDegrees**
    - **MaxThetaDegrees**
    - **ThresholdImagePercent**
    - **CollinearPointsMaxDistanceFromLineMm**
    - **UseOriginalImageIntensityForDotIntensityScore**

  - **PhantomDefinition**
    - **Description**
        - **Name**
        - **Type**
        - **Version**
        - **WiringVersion**
        - **Institution**
    - **Geometry**
        - **Pattern**
            - **Wire**
                - **Name**
                - **EndPointFront**
                - **EndPointBack**
      - **Landmarks**
          - **Landmark**
              - **Name**
              - **Position**

## Phantom definition

We recommend using 3 N-wires as we have found this to be more robust and accurate than using just 2 N-wires but still can be robustly detected and kept in the field of view. Specification of the wire pattern is available at https://github.com/PlusToolkit/PlusDoc/raw/master/tutorials/PlusTutorialBuildingfCalPrintedPhantom.pptx. XML description can be found in the example device set configuration files. 

You can add any number of N-wires with any angles between the side and middle wires (angle can be different for each N-wire) with the following conditions:
- all N-wires shall be completely visible at the same time in the ultrasound image
- planes of N-wires shall be parallel
- only one N-wire is allowed on a plane (they should not be placed side-by-side, only above/below each other)

N-wires should cover about the same area of the ultrasound image (same size and location) during registration as your region of interest (e.g., if you image
a 40mm diameter object 50mm from the transducer then during calibration the N-wires should cover about 40mm range, about 50mm from the transducer).

You can find the CAD models and snapshots of fCal calibration phantoms in the Plus 3D model catalog:
http://perk-software.cs.queensu.ca/plus/doc/nightly/modelcatalog/

fCal-2.x is recommended for smaller field of view (up to about 10cm imaging depth). The hole positions are the same for fCal-1.x and fCal-2.x (only some text labels and marker mounting holes are added) but fCal_2.x has extra holes that allow using 3 N-wires, which greatly increases the accuracy and robustness (compared to using just 2 N-wires), therefore using fCal-1.x is not recommended.

If you image large structures then you probably need fCal 3 phantom. However, fCal-3.x is assembled from multiple pieces (to limit reduce 3D printing time and costs), and so it may be somewhat less accurate then the smaller, unibody fCal-2.x.

## Example configuration file PlusDeviceSet_fCal_Ultrasonix_L14-5_Ascension3DG_2.0.xml

```xml
<PlusConfiguration version="2.3">
  <DataCollection StartupDelaySec="1.0" >
    <DeviceSet 
      Name="fCal: Ultrasonix US (L14-5 probe) + Ascension3DG tracker (Probe, Reference, Stylus) - fCal Phantom 2.0"
      Description="Ascension3DG sensors should be plugged in to the Ascension3DG DriveBay mounted on Ultrasonix US in the following order from to leftmost slot (Transducer 1) to the right: 1 Probe, 2 Reference, 3 Stylus." />
    <Device
      Id="TrackerDevice" 
      Type="Ascension3DG" 
      LocalTimeOffsetSec="0.0" 
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
      Id="VideoDevice"
      Type="SonixVideo" 
      AcquisitionRate="30" 
      LocalTimeOffsetSec="-0.2976"
      IP="127.0.0.1"
      AutoClipEnabled="TRUE" >
      <DataSources>
        <DataSource Type="Video" Id="Video" PortName="B" PortUsImageOrientation="UF"  />
      </DataSources>
      <OutputChannels>
        <OutputChannel Id="VideoStream" VideoDataSourceId="Video"/>
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
    <Transform From="Image" To="TransducerOriginPixel"
      Matrix="1 0 0 -410
              0 1 0 5
              0 0 1 0
              0 0 0 1"
      Date="2011.12.06 17:57:00" Error="0.0" />    
  </CoordinateDefinitions> 
  <Rendering WorldCoordinateFrame="Reference">
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
      File="fCal_2.0.stl"
      ModelToObjectTransform="
        1 0 0 -35.0
        0 1 0 -10.0
        0 0 1 -5.0
        0 0 0 1" />
    <DisplayableObject Type="Image" ObjectCoordinateFrame="Image" Id="LiveImage"/>
  </Rendering>
<Segmentation
    ApproximateSpacingMmPerPixel="0.078"
    MorphologicalOpeningCircleRadiusMm="0.27"
    MorphologicalOpeningBarSizeMm="2.0"
    ClipRectangleOrigin="27 27" 
    ClipRectangleSize="766 562" 
    MaxLinePairDistanceErrorPercent="10"
    AngleToleranceDegrees="10"
    MaxAngleDifferenceDegrees="10"
    MinThetaDegrees="-70"
    MaxThetaDegrees="70"
    MaxLineShiftMm="10.0"
    ThresholdImagePercent="10"
    CollinearPointsMaxDistanceFromLineMm="0.6"
    UseOriginalImageIntensityForDotIntensityScore="FALSE"
    NumberOfMaximumFiducialPointCandidates="20"
  />  <PhantomDefinition>
    <Description
      Name="fCAL"
      Type="Multi-N"
      Version="2.0"
      WiringVersion="2.0"
      Institution="Queen's University PerkLab"
    />
    <Geometry>
      <Pattern Type="NWire">
        <Wire Name="7:G1_g1" EndPointFront="30.0 0.0 20.0" EndPointBack="30.0 40.0 20.0" />
        <Wire Name="8:L1_h1" EndPointFront="55.0 0.0 20.0" EndPointBack="35.0 40.0 20.0" />
        <Wire Name="9:M1_m1" EndPointFront="60.0 0.0 20.0" EndPointBack="60.0 40.0 20.0" />
      </Pattern>
      <Pattern Type="NWire">
        <Wire Name="4:G3_g3" EndPointFront="30.0 0.0 10.0" EndPointBack="30.0 40.0 10.0" />
        <Wire Name="5:H3_l3" EndPointFront="35.0 0.0 10.0" EndPointBack="55.0 40.0 10.0" />
        <Wire Name="6:M3_m3" EndPointFront="60.0 0.0 10.0" EndPointBack="60.0 40.0 10.0" />
      </Pattern>
      <Pattern Type="NWire">
        <Wire Name="1:H5_h5" EndPointFront="35.0 0.0 0.0" EndPointBack="35.0 40.0 0.0" />
        <Wire Name="2:L5_i5" EndPointFront="55.0 0.0 0.0" EndPointBack="40.0 40.0 0.0" />
        <Wire Name="3:M5_m5" EndPointFront="60.0 0.0 0.0" EndPointBack="60.0 40.0 0.0" />
      </Pattern>
      <Landmarks>
        <Landmark Name="#1" Position="104.3 5.0 20.0" />
        <Landmark Name="#2" Position="104.3 45.0 20.0" />
        <Landmark Name="#3" Position="104.3 45.0 0.0" />
        <Landmark Name="#4" Position="104.3 -5.0 0.0" />
        
        <Landmark Name="#5" Position="-34.3 45.0 15.0" />
        <Landmark Name="#6" Position="-34.3 -5.0 20.0" />
        <Landmark Name="#7" Position="-34.3 -5.0 0.0" />
        <Landmark Name="#8" Position="-34.3 45.0 0.0" />
      </Landmarks>      
      <Planes>
        <Plane Name="Superior" BasePoint="5.5 3.0 35.0" EndPoint1="135.0 3.0 35.0" EndPoint2="135.0 55.0 35.0" />
        <Plane Name="Inferior" BasePoint="5.5 55.0 0.0" EndPoint1="135.0 55.0 0.0" EndPoint2="135.0 5.5 0.0" />
        <Plane Name="Outside Unmarked" BasePoint="140.0 5.0 30.0" EndPoint1="140.0 50.0 30.0" EndPoint2="140.0 50.0 5.0" />
        <Plane Name="Inside Unmarked" BasePoint="125.0 40.0 30.0" EndPoint1="125.0 20.0 10.0" EndPoint2="125.0 40.0 10.0" />
        <Plane Name="Outside Marked" BasePoint="0.0 40.0 30.0" EndPoint1="0.0 40.0 5.5" EndPoint2="0.0 15.5 5.5" />
        <Plane Name="Outside Posterior" BasePoint="130.0 0.0 30.0" EndPoint1="20.0 0.0 30.0" EndPoint2="20.0 0.0 15.0" />
        <Plane Name="Inside Posterior" BasePoint="110.0 10.0 30.0" EndPoint1="40.0 10.0 30.0" EndPoint2="30.0 10.0 10.0" />
        <Plane Name="Outside Anterior" BasePoint="130.0 60.0 30.0" EndPoint1="20.0 60.0 30.0" EndPoint2="20.0 60.0 5.0" />
      </Planes>      
      <References>
        <Reference Name="#1" Position="104.3 5.0 20.0" />
        <Reference Name="#2" Position="104.3 45.0 20.0" />
        <Reference Name="#3" Position="104.3 45.0 0.0" />
        <Reference Name="#4" Position="104.3 -5.0 0.0" />
        <Reference Name="#5" Position="-34.3 45.0 15.0" />
        <Reference Name="#6" Position="-34.3 -5.0 20.0" />
        <Reference Name="#7" Position="-34.3 -5.0 0.0" />
        <Reference Name="#8" Position="-34.3 45.0 0.0" />
      </References>
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
    FreeHandStartupDelaySec="5" />

  <vtkPlusPivotCalibrationAlgo
    ObjectMarkerCoordinateFrame="Stylus"
    ReferenceCoordinateFrame="Reference"
    ObjectPivotPointCoordinateFrame="StylusTip" />

  <vtkPlusPhantomLandmarkRegistrationAlgo
    PhantomCoordinateFrame="Phantom"
    ReferenceCoordinateFrame="Reference"
    StylusTipCoordinateFrame="StylusTip" 
    DetectionTimeSec="1.0"
    StylusTipMaximumDisplacementThresholdMm="1.8" />

  <vtkPlusProbeCalibrationAlgo
    ImageCoordinateFrame="Image"
    ProbeCoordinateFrame="Probe"
    PhantomCoordinateFrame="Phantom"
    ReferenceCoordinateFrame="Reference" />

</PlusConfiguration>
```