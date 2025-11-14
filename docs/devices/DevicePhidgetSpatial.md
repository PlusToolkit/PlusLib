# Phidgets Spatial 3/3/3 magnetic, angular rate, and gravity (MARG) sensor

## Supported hardware devices

- Contains accelerometer, gyroscope, and magnetometer sensors, each with 3 DOF
- Small (few centimeters by few centimeters), low-cost (about $150) device
- Link to manufacturer's website: http://www.phidgets.com/products.php?product_id=1044_0

## Supported platforms
- [***Windows 32 bits***](/SupportedPlatforms/#windows-32-bits)
- [***Windows 64 bits***](/SupportedPlatforms/#windows-64-bits)
- [***Windows XP embedded 32 bits***](/SupportedPlatforms/#windows-xp-embedded-32-bits)

The device manufacturer supports additional platforms. On request, Plus toolkit will be extended to support this device on other platforms, too.

## Installation

## Device configuration settings

Tracker coordinate system: South-West-Down. Sensor coordinate system is drawn on the sensor PCB. If the PCB is laying flat on the table with cable socket towards East then the axis directions are: South-West-Down.

If magnetic fields are nearby the sensor have non-negligible effect then compass can be ignored by choosing an IMU AHRS algorithm (as opposed to MARG) or compass correction may be performed (see http://www.phidgets.com/docs/Compass_Primer). The magnetometer has an internal calibration mechanism, which is running continuously. During this internal calibration no measurement data can be retrieved from the magnetometer. When magnetometer data is not available then the associated tool status is set to INVALID.

- **Type**: `PhidgetSpatial`
- **AcquisitionRate**: (Optional, default: `125`)
- **SerialNumber**: If specified then connection is made to the devices that has a matching SerialNumber. Useful if multiple devices are connected to the computer. If the value is -1 then connections is made to the first available device
- **LocalTimeOffsetSec**: (Optional, default: `0`)
- **ToolReferenceFrame**: (Optional, default: `Tracker`)

- **ZeroGyroscopeOnConnect**: A flag for zeroing the gyroscope when connecting to the device. If enabled then the sensor should not be moved for 2 seconds while connecting to the device if the `OrientationSensor` or `Accelerometer` tool is used. The device can also be zero'd at any time if the client application
- **TiltSensorWestAxisIndex**: In tilt sensor mode we don't use the magnetometer, so we have to provide a direction reference. The orientation is specified by specifying an axis that will always point to the (`West`) direction. (Recommended values:
    - If sensor axis 0 points down (the sensor plane is about vertical) => **TiltSensorDownAxisIndex** `=(`2`).`
    - If sensor axis 1 points down (the sensor plane is about vertical) => **TiltSensorDownAxisIndex** `=(`0`).`
    - If sensor axis 2 points down (the sensor plane is about horizontal) => **TiltSensorDownAxisIndex** `=(`1`).`
- **FilteredTiltSensorWestAxisIndex**: Same as **TiltSensorWestAxisIndex** but for the filtered tool.
- **AhrsAlgorithm**: It is a combined parameter. The first part specifies the AHRS algorithm method `MADGWICK` (only one parameter) or `MAHONY` (two parameters proportional and integral). The second part determines whether the magnetometer is used (`MARG` where heading estimated using magnetometer data, it is less noisy but sensitive to magnetic filed disortions) or not (`IMU` where heading estimated using gyroscope data only, it is more noisy but not sensitive to magnetic filed disortions). Supported types: (`MADGWICK_MARG`, `MADGWICK_IMU`, `MAHONY_MARG`, or `MAHONY_IMU`) (Optional, default: `MADGWICK_MARG`)
- **FilteredTiltSensorAhrsAlgorithm**: It is a combined parameter that specifies the AHRS algorithm type for Filtered Tilt. The first part specifies the AHRS algorithm method `MADGWICK`
- **AhrsAlgorithmGain**: Gain values used by the AHRS algorithm
- **FilteredTiltSensorAhrsAlgorithmGain**: Same as **AhrsAlgorithmGain** but for the filtered tool.
- **CompassCorrectionParameters**: Sets correction parameters for the magnetometer sensor. This is for filtering out hard and soft iron offsets, and scaling the output to match the local field strength. These parameters can be obtained from the compass calibration program provided by Phidgets Inc (<http://www.phidgets.com/docs/Compass_Primer>). The following 13 parameter values are have to be provided (separated by spaces):
    - magField: The reference field to use. Generally, use the local expected field strength, or 1.0.
    - offset0,1,2: Applies offset to the compass data in axes 0,1,2.
    - gain0,1,2: Applies gain corrections in axes 0,1,2.
    - T0,1,2,3,4,5: Applies corrections for non-orthogonality of the ellipsoid.

- **DataSources**: One `DataSource` child element for each sensor. (Required)
    - **DataSource**: (Required)
        - **PortName**: (Required)
            - `Accelerometer` Raw sensor measurement. The values are stored in the translation part of the transformation matrix. The rotation part is identity.
            - `Gyroscope` Raw sensor measurement. The values are stored in the translation part of the transformation matrix. The rotation part is identity.
            - `Magnetometer` Raw sensor measurement. The values are stored in the translation part of the transformation matrix. The rotation part is identity.
            - `TiltSensor` 2-DOF sensor tilt is computed as a rotation matrix. Only the accelerometer is used.
            - `OrientationSensor` 3-DOF sensor orientation is computed using sensor fusion. With IMU algorithm only the accelerometer and gyroscope data are used. With AHRS algorithm accelerometer, gyroscope, and magnetometer data are used.
        - **BufferSize**: (Optional, default: `150`)
        - **AveragedItemsForFiltering**: (Optional, default: `20`)

## Example configuration file PlusDeviceSet_Server_PhidgetSpatial.xml

```xml
<PlusConfiguration version="2.1">

  <DataCollection StartupDelaySec="1.0">
    <DeviceSet
      Name="PlusServer: PhidgetSpatial MARG sensor"
      Description="Broadcasting tool tracking data through OpenIGTLink
Keep the device stationary for 2 seconds after connect for accurate gyroscope initialization."
    />
    <Device
      Id="TrackerDevice"
      Type="PhidgetSpatial"
      AhrsAlgorithm="MADGWICK_MARG"
      AhrsAlgorithmGain="1.5"
      TiltSensorWestAxisIndex="1"
      FilteredTiltSensorAhrsAlgorithm="MADGWICK_IMU"
      FilteredTiltSensorAhrsAlgorithmGain="0.01"
      FilteredTiltSensorWestAxisIndex="1"
      ToolReferenceFrame="Tracker" >
      <DataSources>
        <DataSource Type="Tool" Id="OrientationSensor" PortName="OrientationSensor" />
        <DataSource Type="Tool" Id="TiltSensor" PortName="TiltSensor" />
        <DataSource Type="Tool" Id="FilteredTiltSensor" PortName="FilteredTiltSensor" />
        <DataSource Type="Tool" Id="Accelerometer" PortName="Accelerometer" />
        <DataSource Type="Tool" Id="Gyroscope" PortName="Gyroscope" />
        <DataSource Type="Tool" Id="Magnetometer" PortName="Magnetometer" />
      </DataSources>
      <OutputChannels>
        <OutputChannel Id="TrackerStream" >
          <DataSource Id="OrientationSensor"/>
          <DataSource Id="TiltSensor"/>
          <DataSource Id="FilteredTiltSensor"/>
          <DataSource Id="Accelerometer"/>
          <DataSource Id="Gyroscope"/>
          <DataSource Id="Magnetometer"/>
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
        <Transform Name="OrientationSensorToTracker" />
        <Transform Name="TiltSensorToTracker" />
        <Transform Name="FilteredTiltSensorToTracker" />
        <xTransform Name="AccelerometerToTracker" />
        <xTransform Name="GyroscopeToTracker" />
        <xTransform Name="MagnetometerToTracker" />
      </TransformNames>
    </DefaultClientInfo>
  </PlusOpenIGTLinkServer>

</PlusConfiguration>
```