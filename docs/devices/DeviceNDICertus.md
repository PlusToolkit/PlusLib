# NDI Optotrak Certus optical pose tracker

## Supported hardware devices

- Link to manufacturer's website: <http://www.ndigital.com/msci/products/optotrak-certus/>

## Supported platforms

- [***Windows 32 bits***](/SupportedPlatforms/#windows-32-bits)
- [***Windows XP embedded 32 bits***](/SupportedPlatforms/#windows-xp-embedded-32-bits)

## Installation

### Install common software

Before setting up the device or the connection make sure the followings are installed on the computer where Plus will be used:
- Optotrak device drivers (OptotrakDeviceDrivers_v3.5.2) with updates (OptoUpdate_3.5.2 is needed for ethernet connection)
    - When installing OptoUpdate click on the `'Update the system to v3.01`' button and select the `'Do Not Search for Hardware Components (Update Software Only)`' choice.
- NDI First Principles software (FirstPrinciples_v1.2.2)

### Communicating Certus tracker by PCI card

PCI card installation. To install an NDI PCI drivers card you will need:
- Optotrak PCI Interface Card Installation Guide
- NDI PCI interface card
- NDI PCI drivers CD (NDIDeviceDrivers_v3.00.66)

### Communicating Certus tracker by via ethernet

#### How to set up your PC

Install the following software:

- 6DA_Optotrak (6DA_Optotrak_1.3.3) needed for ethernet connection
- NDI DataView (NDI_DataView_Version_1.00.03):
- Update standard.cam and system.nif
- Change NDI settings file (by default optotrak.ini file is located in the "C:\ndigital\settings" folder)
- Set `Driver` to `etherlink.dll`
  - Copy `etherlink.dll` to the ```c:\ndigital\programs"``` folder from ```c:\ndigital\drivers```
- Set `ResetTimeout` to 500 and `PropagationTimeout` to 5000 (recommended in the manual but maybe not vital)
- Set `Server Address` to the NDI SCU IP (130.15.7.98)
- `Control Port` should be 9999, `Data Port` should be 10000
- `Data Timeout` should be 10000 and `Reset Delay` should be between 5000 and 10000 (also recommended in the manual)

If you made these changes, the NDI will work via ethernet. If you want to use the PCI card again, you have to undo these changes (the most important thing is to set `Driver` to `pcilink.dll`)

#### How to set up the System Control Unit (SCU)

Change ethernet settings in NDI SCU `(Note: this has to be done only when SCU, gateway or DNS IP address is changed)`

- Connect Serial cable to the SCU and a Serial-to-USB adaptor cable, then the USB into a computer
- Connect to SCU using MTTTY (COM port can be read from Device Manager)
- Commands (Read: ```[Read command name][Space][Enter]```, Write: ```[Write command name][Space][Value][Enter]```)
  - `ENDHRD` Reads the DHCP status
  - `ENDHWR` Sets the DHCP status
  - `ENGWRD` Reads the gateway address
  - `ENGWWR` Sets the gateway address
  - `ENIPRD` Reads the IP address
  - `ENIPWR` Sets the IP address
  - `ENNMRD` Reads the Ethernet host name
  - `ENNMWR` Sets the Ethernet host name
  - `ENSMRD` Reads the subnet mask
  - `ENSMWR` Sets the subnet mask

### Coordinate systems

Tracker `(Left-handed)`
- X+: up (against gravity)
- Y+: to the right (if we face the camera)
- Z+: away from the camera

Reference DRB `(Right-handed).` The orientation of the DRB is the following: the side with the writing on it is on the far side from the camera and the screw points to the ground.
  - X+: to the left (if we face the camera)
  - Y+: up (against gravity)
  - Z+: towards the camera

Tracking with NDI Certus: To achieve stable tracking, the NDI Certus camera must be at least 2 meters away from the tracked DRBs. For more information, please see the following link: <http://www.ndigital.com/medical/certus-techspecs-fullvolume.php>.

No "beeps" when attempting to connect to a Certus tracker => Power off the OptoTrak Certus camera and communication unit for a few seconds and try to connect again
If no Certus device is attached to the system then the application crashes (due to the TransputerDetermineSystemCfg call in NDI's toolkit, which determines the system configuration causes the application to crash)

## Device configuration settings

- **Type**: `CertusTracker`
- **AcquisitionRate**: (Optional, default: `50`)
- **LocalTimeOffsetSec**: (Optional, default: `0`)
- **ToolReferenceFrame**: (Optional, default: `Tracker`)
- **DataSources**: One `DataSource` child element for each marker. (Required)
    - **DataSource**: (Required)
        - **PortName**: (Required)
        - **BufferSize**: (Optional, default: `150`)
        - **AveragedItemsForFiltering**: (Optional, default: `20`)

## Example configuration file PlusDeviceSet_Server_NDICertus.xml

```xml
<PlusConfiguration version="2.1">

  <DataCollection StartupDelaySec="1.0">
    <DeviceSet
      Name="PlusServer: NDI Certus tracker"
      Description="Broadcasting tool tracking data through OpenIGTLink
Tracking a single needle.
The computer needs to be on the same local network as Certus SCU unit. The needle has to be plugged into the first slot"
    />
    <Device
      Id="TrackerDevice"
      Type="CertusTracker"
      ToolReferenceFrame="Tracker" >
      <DataSources>
        <DataSource Type="Tool" Id="Probe" PortName="0" />
        <DataSource Type="Tool" Id="Reference" PortName="1" />
        <DataSource Type="Tool" Id="Stylus" PortName="2" />
        <DataSource Type="Tool" Id="Needle" PortName="3" />
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
    SendValidTransformsOnly="true"
    OutputChannelId="TrackerStream" >
    <DefaultClientInfo>
      <MessageTypes>
        <Message Type="TRANSFORM" />
      </MessageTypes>
      <TransformNames>
        <Transform Name="NeedleToTracker" />
      </TransformNames>
    </DefaultClientInfo>
  </PlusOpenIGTLinkServer>

</PlusConfiguration>
```