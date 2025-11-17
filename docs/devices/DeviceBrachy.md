# Prostate brachytherapy steppers

## Supported hardware devices
Civco brachy steppers

- Link to manufacturer's website: <http://www.civco.com/mmi/products/brachytherapy/Steppers.htm>

## Supported platforms
- [***Windows 32 bits***](../SupportedPlatforms.md#windows-32-bits)
- [***Windows 64 bits***](../SupportedPlatforms.md#windows-64-bits)
- [***Windows XP embedded 32 bits***](../SupportedPlatforms.md#windows-xp-embedded-32-bits)

## Installation

## Device configuration settings

  - **Type**: `Brachy`
  - **AcquisitionRate**: (Optional, default: `20`) 
  - **LocalTimeOffsetSec**: (Optional, default: `0`)
  - **ToolReferenceFrame**: (Optional, default: `Tracker`)

  - **BrachyStepperType**: It could be any of the brachy stepper types: (Required)
      - `BURDETTE_MEDICAL_SYSTEMS_DIGITAL_STEPPER`
      - `BURDETTE_MEDICAL_SYSTEMS_DIGITAL_MOTORIZED_STEPPER`
      - `CMS_ACCUSEED_DS300`
      - `CIVCO_STEPPER`

  - **SerialPort**: Scalar value that defines the serial port to use, COM1 through COM4. (Optional, default: `0`)
  - **BaudRate**: Baud rate for communication with the stepper. (Optional, default: `19200`)
  - **ModelNumber**: Stepper model number STRING (Optional, default: `NULL`)
  - **ModelVersion**:  A string (perhaps a long one) describing the type and version of the device. (Optional, default: `NULL`)
  - **ModelSerialNumber**: STRING stepper serial number. (Optional, default: `NULL`)
  - **StepperCalibration**:
    - **AlgorithmVersion**. (Optional, default: `NULL`)
    - **Date**. (Optional, default: `NULL`)
    - **ProbeTranslationAxisOrientation**: VECTOR `[Tx, Ty, 1]`. (Optional, default: `0 0 1`)
    - **TemplateTranslationAxisOrientation**: VECTOR `[Tx, Ty, 1]`. (Optional, default: `0 0 1`)
    - **ProbeRotationAxisOrientation**: VECTOR `[Trx, Try, 1]`. (Optional, default: `0 0 1`)
    - **ProbeRotationEncoderScale**: SCALAR (Optional, default: `1`)
    - **DataSources**: One `DataSource` child element for each tool. (Required)
        - **DataSource**: (Required)
            - **PortName**: (Required)
                - `0` ProbeHomeToProbe transform
                - `1` TemplateHomeToTemplate transform
                - `2` Raw encoder values stored in the position component of the transform (x = probe translation, y = probe rotation, z = template translation)
            - **BufferSize**: (Optional, default: `150`)
            - **AveragedItemsForFiltering**: (Optional, default: `20`)
