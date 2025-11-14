# Ultrasound imaging parameters

## US imaging parameters

An ultrasound imaging parameters element inside of a device tag is detected and loaded for specific devices. 

## Supported devices

Current devices include 
 - `DeviceSonixVideo`
 - `DeviceInterson` devices.

## Device configuration settings

  - **Device
    - **UsImagingParameters Imaging parameters container ** (Required)
      - **Contrast**: Is the contrast of B-mode ultrasound that is used to create the LUT. ** (      - **DepthMm**: Depth `[mm]` of B-mode ultrasound. If not specified then the current setting is used. ** (      - **DynRangeDb**: DynRange `[dB]` of B-mode ultrasound. If not specified the current value is kept. ** (      - **FrequencyMhz**: Ultrasound transmitter frequency `[MHz]` If not specified then the current setting is used.** (      - **GainPercent** `[%]` of B-mode ultrasound; valid range: 0-100. If not specified then the current setting is used.** (Optional, default:  )
      - **TimeGainCompensation**: `[dB]` Time gain compensation `[N` int values] If not specified then the current setting is used.** (      - **Intensity**: Is the intensity of B-mode ultrasound that is used to create the LUT. ** (      - **Sector** `[%]` of B-mode ultrasound; valid range: 0-100. If not specified the current value is kept. ** (Optional, default:  )
      - **ZoomFactor** Zoom `[%]` of B-mode ultrasound; valid range: 0-100. If not specified then the current setting is used. ** (Optional, default:  )
      - **SoundVelocity**: The speed of sound `[m/s].`  The minimum and maximum allowed value is specified on the Ultrasound computer, in the Exam software directory in the `imaging.set.xml` file (      - **Voltage** (Optional, default:  )