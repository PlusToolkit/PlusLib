# Clarius ultrasound systems (OEM API)

## ClariusOEM Device vs. Clarius Device

PLUS supports two different interfaces to Clarius probes. This documentation is for the OEM API based PLUS device, which connects directly with the probe using the Bluetooth Low Energy (BLE) and Wi-Fi interfaces of the PC running PLUS. This device also exposes US controls for the Clarius, and real-time battery / button state streaming accessible as MRML node attributes on the Image node in 3D Slicer. The primary limitation of this interface is that for now it's only available on Windows.

In contrast, the original Clarius support uses the Clarius Cast API. Using this approach, a phone/tablet is connected to the probe, and the PC running PLUS retrieves images indirectly via this phone/tablet. Documentation for the Cast API based PLUS device is available at `DeviceClarius`

## Supported hardware devices

- This device is used for image acquisition from Clarius ultrasound systems using the OEM API.
- Link to manufacturer's website: <https://clarius.com>
- Link to OEM API: <https://github.com/clariusdev/oem>

## Supported platforms

- [***Windows 64 bits***](/SupportedPlatforms/#windows-64-bits)

## Updating Clarius probe

The current version of the ClariusOEM device is built to use Clarius device software v9.1.0. Updating of the Clarius probe software should be done using the Clarius phone app.

## How to setup your probe with ClariusOEM

This section outlines the general steps required to setup a new PC to use PLUS with the ClariusOEM device. Each step is covered in more detail in the following sections.

-# Download Clarius probe certificate, used to validate your device during connection to the probe. See `SetupDownloadCert`
-# Connect to the Clarius probe's Wi-Fi network. The Wi-Fi connection is used for streaming all data between the probe and your PC. See `SetupWiFi`
-# Enable Windows "Developer Mode". See `SetupDevMode`
-# Connect to the Clarius probe's Bluetooth LE network. This network is used for controlling the probe power state (on/off) as well as in the process of initiating connections to the probe. See `SetupBluetooth`
-# Edit the ClariusOEM config file to reflect the info for your probe, and your desired settings. See `SetupConfigFile`

## Download Clarius certificate

To connect to the Clarius probe we need to provide a certificate to authenticate our application.
Certificates can be obtained by following the steps listed here: <https://github.com/clariusdev/solum?tab=readme-ov-file#probe-certificate>.
This will retrieve the certificates for all probes on the account. The certificate must be saved into a file, beginning with "-----BEGIN CERTIFICATE-----" and ending with "-----END CERTIFICATE-----".
Save the certificate file to the ClariusCert folder in your PLUS config directory (you may need to create this folder).

## Connect to Clarius Wi-Fi

The second step is setting up the Clarius probe using Windows Wi-Fi.
This step only needs to be completed once. During regular use switching to the probe’s network after bootup is
automatically handled by PLUS.
-# Retrieve your Clarius scanners Wi-Fi SSID and password:
  - Open the Clarius app
  - Exit the current scan
  - Tap on the three horizontal lines on the top of the screen
  - Tap on “Status”
  - Record the Serial Number, SSID and Network Password for your probe
-# Close the Clarius Ultrasound App on your phone, but leave the probe powered on
-# Click on the “Wifi” icon on the Windows taskbar
-# Click on the network name matching your probe’s SSID
-# Enter the password and ensure you’re connected.
It’s normal to see you are connected to the DIRECT_C***######## network, but that it does not have internet
access.

## Enable Windows Developer Mode

The third step is to enable Windows Developer Mode, so that PLUS can communicate with the Clarius probe over Bluetooth LE. This step only needs to be completed once. To do this:
-# Open Windows Settings
-# Click on "Update & Security"
-# Click the "For developers" tab
-# Toggle the "Developer Mode" setting to ON

## Connect to Clarius Bluetooth LE

The next step to connecting to the Clarius probe is to pair it using Windows Bluetooth. This step only needs to be completed once. To do this:
-# Open Windows Settings
-# Click on “Devices” then “Bluetooth & other devices”
-# Click “Add Bluetooth or other device”
-# Click “Bluetooth: Mice, keyboards, pens, or audio and other kinds of Bluetooth devices”
-# Wait for your Clarius probe to appear in the list of devices, and initiate pairing
-# Check the probe appears in the list of paired devices in Windows Bluetooth. If not, re-pair following steps
3-5.

## Edit ClariusOEM config file

Finally, we need to edit the default Clarius OEM PLUS configuration file to reflect the specific Clarius probe we
want to connect to and our desired options. The list below is the minimum number of attributes that must be edited, a complete reference and sample config can be found at the bottom of this page in the `ClariusOEMConfigSettings` and `ClariusOEMExampleConfigFile` sections.

-# ProbeSerialNum: This is easy to find in your Bluetooth settings, the serial number of your probe should be
the name of your connected Clarius device
-# PathToCert: This must be set to the path to the certificate file you created earlier, relative to the PLUS
config file
-# ProbeType: Must match the type of Clarius probe you are trying to connect to (it’s also the first 4
characters of the serial number)
-# ImagingApplication: Options are listed by PLUS if you leave this field blank or enter an invalid option
-# UsImagingParameters: PLUS support setting a number of ultrasound parameters, provided as a reference in `UsImagingParameters`. The ClariusOEM device supports the following parameters:
  - Depth (mm)
  - Gain (%)
  - Dynamic Range (%)
  - Time Gain Compensation (dB)

## Physically correct US frame size and handling changing depths

The Clarius OEM interface is able to maintain correct physical image size automatically, even when the US depth changes. It is able to provide an `ImageToTransd` transform, between the Image coordinates (0,0 corner of the ultrasound frame) and the Transd coordinates (center of the probe element). When calibrating the Clarius, you may use a rigid transform between the Transd coordinate system and the coordinate system of the probe mounted marker. During calibrating, changes in depth are possible and recommended as it should enhance the accuracy of your calibration across the imaging depths supported by Clarius. There are two ways to utilize the automatic depth feature.

## Method 1: Transform hierarchy with the ImageToTransd transform

For more information about 3D Slicer, please see it's website and tutorials at <https://www.slicer.org/>

This is the configuration demonstrated in the \ClariusOEMExampleConfigFile

## Method 2: Embed the transform directly in the image

- In contrast to Method 1, this method includes the ImageToTransd transform embedded within the sent image.
- If you choose this approach no further steps are needed in 3D Slicer to achieve physically correct US frame size
- The only changes from the provided sample configuration file are to the PlusOpenIGTLinkServer element:

```
  <PlusOpenIGTLinkServer
    MaxNumberOfIgtlMessagesToSend="1"
    MaxTimeSpentWithProcessingMs="50"
    ListeningPort="18944"
    SendValidTransformsOnly="false"
    OutputChannelId="VideoStream" >
    <DefaultClientInfo>
      <MessageTypes>
        <Message Type="IMAGE" />
      </MessageTypes>
      <ImageNames>
        <Image Name="Image" EmbeddedTransformToFrame="Transd" />
      </ImageNames>
    </DefaultClientInfo>
  </PlusOpenIGTLinkServer>
```

## Device configuration settings

  - **Type**: `ClariusOEM`
  - **ToolReferenceFrame**: `Transd` (Required)
  - **ProbeSerialNum**: The probe's serial number (Required)
  - **PathToCert**: Path to the certificate retrieved in `SetupDownloadCert` (Required)
  - **ProbeType**: Type of Clarius probe in use (first 4 chars of serial number) (Required)
  - **ImagingApplication**: Imaging application type to load. Launching with an invalid / empty application type will list available types in the PLUS log file. (Required)
  - **FrameSize**: Desired size of the Clarius image frame (Optional, default: `512, 512`)
  - **EnableAutoGain**: Should automatic imaging gain be used? (Optional, default: `FALSE`)
  - **Enable5v**: Should the external 5V rail on the probe be powered? (Optional, default: `FALSE`)
  - **FreezeOnPoorWifiSignal**: Should the probe automatically freeze when the wifi signal is poor? (Optional, default: `TRUE`)
  - **ContactDetectionTimeoutSec**: Probe framerate is reduced after no contact is detected for this number of seconds. Valid range 0-30. A value of 0 turns the timeout off. (Optional, default: `15`)
  - **AutoFreezeTimeoutSec**: Acquisition of image data is frozen after no contact is detected for this number of seconds. Valid range 0-120. A value of 0 turns the timeout off. (Optional, default: `60`)
  - **KeepAwakeTimeoutMin**: Probe will automatically power off after no contact is detected for this number of minutes. Valid range 0-120. A value of 0 turns the timeout off. (Optional, default: `60`)
  - **UpButtonMode**: What should the up button on the probe do? (Optional, default: `DISABLED`) Options:
      - `DISABLED` up button fully disabled
      - `FREEZE` up button freezes and un-freezes ultrasound image acquisition
      - `USER` up button sends metadata to Slicer over OpenIGTLink indicating the button was clicked, and how many times in a row (1, 2, 3 or 4). This can be retrieved as a MRML node attribute of the Clarius image node.
  - **DownButtonMode**: What should the down button on the probe do? See UpButtonMode for options. (Optional, default: `FREEZE`)

## Example configuration file PlusDeviceSet_Server_ClariusOEMVideoCapture.xml

```xml
<PlusConfiguration version="2.1">
  <DataCollection StartupDelaySec="1.0" >
    <DeviceSet
      Name="PlusServer: Clarius ultrasound device (OEM API)"
      Description="Broadcasting acquired video through OpenIGTLink"/>
    <Device
      Id="VideoDevice"
      Type="ClariusOEM"
      ToolReferenceFrame="Transd"
      ProbeSerialNum="C3HD01234567890"
      PathToCert="ClariusCert/C3HD01234567890.cert"
      ProbeType="C3HD"
      ImagingApplication="MSK"
      FrameSize="512 512"
      EnableAutoGain="FALSE"
      Enable5v="FALSE"
      FreezeOnPoorWifiSignal="TRUE"
      ContactDetectionTimeoutSec="15"
      AutoFreezeTimeoutSec="60"
      KeepAwakeTimeoutMin="60"
      UpButtonMode="DISABLED"
      DownButtonMode="FREEZE"
      >
      <UsImagingParameters>
        <Parameter Name="DepthMm" Value="100" />
        <Parameter Name="GainPercent" Value="80" />
        <Parameter Name="DynRangeDb" Value="80" />
        <Parameter Name="TimeGainCompensation" Value="5 5 5" />
      </UsImagingParameters>

      <DataSources>
        <DataSource Type="Video" Id="Video" PortName="B" PortUsImageOrientation="UF"/>
        <DataSource Type="Tool" Id="Image" PortName="Transd" />
      </DataSources>

      <OutputChannels>
        <OutputChannel Id="VideoStream" VideoDataSourceId="Video" >
          <DataSource Type="Tool" Id="Image" />
        </OutputChannel>
      </OutputChannels>

    </Device>

  </DataCollection>

  <PlusOpenIGTLinkServer
    MaxNumberOfIgtlMessagesToSend="1"
    MaxTimeSpentWithProcessingMs="50"
    ListeningPort="18944"
    SendValidTransformsOnly="false"
    OutputChannelId="VideoStream" >
    <DefaultClientInfo>
      <MessageTypes>
        <Message Type="IMAGE" />
        <Message Type="TRANSFORM" />
      </MessageTypes>
      <ImageNames>
        <Image Name="Image" EmbeddedTransformToFrame="Image" />
      </ImageNames>
      <TransformNames>
        <Transform Name="ImageToTransd" />
      </TransformNames>
    </DefaultClientInfo>
  </PlusOpenIGTLinkServer>
</PlusConfiguration>
```