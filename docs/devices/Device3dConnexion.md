# 3dConnexion 3D mouse devices

## Supported hardware devices

- All 3dConnexion 3D mouse models are supported.
- Tested with 3dConnexion SpaceNavigator 3D mouse.
- Link to manufacturer's website: <http://www.3dconnexion.com/products/spacenavigator.html>.

## Supported platforms

- [***Windows 32 bits***](../SupportedPlatforms.md#windows-32-bits)
- [***Windows 64 bits***](../SupportedPlatforms.md#windows-64-bits)
- [***Windows XP embedded 32 bits***](../SupportedPlatforms.md#windows-xp-embedded-32-bits)

## Installation

- No need to install any driver or software. If the 3DxWare software is installed: when a window is activated then the mouse may interact with that window as well (e.g., by default tilt front/back simulates mouse wheel). To avoid this either A. uninstall 3DxWare, B. activate a window that does not respond to mouse functions, or C. disable mouse functions in the 3DxWare settings.

## Device configuration settings

  - **Type**: `3dConnexion`

  - **AcquisitionRate**: (Optional, default: `125`)
  - **LocalTimeOffsetSec**: (Optional, default: `0`)
  - **ToolReferenceFrame**: (Optional, default: `Tracker`)

  - **Mode**: Operation mode of the device. (  - `MOUSE` The transform is the current pose. If the mouse is released then the transforms reverts to identity.
  - `JOYSTICK` The transform is changing while the mouse is translated or rotated. If the mouse is released then the transform kept unchanged.

  - **TranslationScales**: Sensitivity and direction of the 3 translation axes. Higher values mean higher sensitivity. Negative values can be used for inverting axes.
  - **RotationScales**: Sensitivity and direction of the 3 rotation axes. Higher values mean higher sensitivity. Negative values can be used for inverting axes.
  - **DataSources**: Exactly one `DataSource` child element is required. (Required)
    - **DataSource**: (Required)
      - **PortName**: = `(`SpaceNavigator`)` (Required)
      - **BufferSize**: (Optional, default: 150)
      - **AveragedItemsForFiltering**: (Optional, default: 20)

## Use cases

### Usage in 3D Slicer

- Start PlusServer
- Select the configuration `PlusServer: 3dConnexion SpaceNavigator 3D mouse`
- Click Connect
- Start 3D Slicer
- Go to `OpenIGTLinkIF` module
- Click "+" to create a new connection, select the new connection, click (`Client`), click (`Active`)
- The `NavigatorToTracker` transform is created in Slicer and it can be edited by moving the 3D mouse

Settings for rotating/translating an image in the axial, sagittal, coronal slice views in 3D Slicer:

    Mode="JOYSTICK" TranslationScales="-0.001 -0.001 -0.001" RotationScales="0.001 0.001 0.001"

Settings for rotating/translating an object in the 3D view in 3D Slicer:

    Mode="JOYSTICK" TranslationScales="-0.001 0.001 -0.001" RotationScales="-0.001 0.001 -0.001"

## Example configuration file PlusDeviceSet_Server_3dConnexion.xml

```xml
<PlusConfiguration version="2.1">

  <DataCollection StartupDelaySec="1.0">
    <DeviceSet 
      Name="PlusServer: 3dConnexion SpaceNavigator 3D mouse" 
      Description="Broadcasting tool tracking data through OpenIGTLink" />
    <Device
      Id="TrackerDevice"
      Type="3dConnexion"
      Mode="JOYSTICK"
      TranslationScales="0.001 -0.001 -0.001"
      RotationScales="0.001 -0.001 -0.001"
      ToolReferenceFrame="Tracker" >
      <DataSources>
        <DataSource Type="Tool" Id="Navigator" PortName="SpaceNavigator" />    
      </DataSources>
      <OutputChannels>
        <OutputChannel Id="TrackerStream" >
          <DataSource Id="Navigator" />
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
        <Transform Name="NavigatorToTracker" /> 
      </TransformNames>
    </DefaultClientInfo>
  </PlusOpenIGTLinkServer>
  
</PlusConfiguration>
```