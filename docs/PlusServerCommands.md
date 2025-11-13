# PlusServer commands

## Remote control example

### Server setup

- Start PlusServer:
```console
PlusServer.exe --config-file=..\..\PlusLib\data\ConfigFiles\Testing\PlusConfiguration_OpenIGTLinkCommandsTest.xml
```

### Control the server using the example client in Plus

#### Example: simple test commands

- ```PlusServerRemoteControl.exe --command=GET_CHANNEL_IDS```
    - PlusServer log: `<Command Name="RequestChannelIDs" />`
    - PlusServerRemoteControl reply data: `TrackedVideoStream`

- ```PlusServerRemoteControl.exe --command=START_ACQUISITION --device="CaptureDevice" --output-file="PlusServerRecording.mha"```
    - PlusServer log:  `<Command Name="StartRecording" CaptureDeviceId="CaptureDevice" OutputFilename="PlusServerRecording.mha" EnableCompression="False"/>`
    - PlusServerRemoteControl reply data: `VirtualStreamCapture (CaptureDevice) StartRecording completed successfully`

- ```PlusServerRemoteControl.exe --command=STOP_ACQUISITION --device="CaptureDevice"```
    - PlusServer log:  `<Command Name="StopRecording" CaptureDeviceId="CaptureDevice"/>`
    - PlusServerRemoteControl reply data:  `VirtualStreamCapture (CaptureDevice) StopRecording completed successfully`

- ```PlusServerRemoteControl.exe --command=GET_EXAM_DATA --volumeEmbeddedTransformToFrame="Reference"```
    - PlusServer log: `<Command Name="ExamData" VolumeEmbeddedTransformToFrame="Reference"`
    - PlusServerRemoteControl reply data: `"Volume sending completed successfully, volume sent as: SLD-001" `

#### Example: volume reconstruction in batch mode (acquiring all frames then reconstruct volume at once)

If `--output-file` is specified then the reconstructed volume will be written to file. If output-image-name is specified then the reconstructed volume will be sent through OpenIGTLink.
```console
PlusServerRemoteControl.exe --command=START_ACQUISITION
timeout /t 10
PlusServerRemoteControl.exe --command=STOP_ACQUISITION
PlusServerRemoteControl.exe --command=RECONSTRUCT --output-file=recvol.mha --output-image-name=recvol_Reference
```

#### Example: live volume reconstruction with intermediate snapshots

```console
PlusServerRemoteControl.exe --command=START_RECONSTRUCTION
PlusServerRemoteControl.exe --command=GET_RECONSTRUCTION_SNAPSHOT --output-image-name=recvol_Reference
PlusServerRemoteControl.exe --command=SUSPEND_RECONSTRUCTION
@echo Please start volume rendering of the sent volume
timeout /t 10
PlusServerRemoteControl.exe --command=RESUME_RECONSTRUCTION
PlusServerRemoteControl.exe --command=GET_RECONSTRUCTION_SNAPSHOT --output-image-name=recvol_Reference
PlusServerRemoteControl.exe --command=GET_RECONSTRUCTION_SNAPSHOT --output-image-name=recvol_Reference
PlusServerRemoteControl.exe --command=GET_RECONSTRUCTION_SNAPSHOT --output-image-name=recvol_Reference
PlusServerRemoteControl.exe --command=GET_RECONSTRUCTION_SNAPSHOT --output-image-name=recvol_Reference
PlusServerRemoteControl.exe --command=GET_RECONSTRUCTION_SNAPSHOT --output-image-name=recvol_Reference
PlusServerRemoteControl.exe --command=GET_RECONSTRUCTION_SNAPSHOT --output-image-name=recvol_Reference
PlusServerRemoteControl.exe --command=GET_RECONSTRUCTION_SNAPSHOT --output-image-name=recvol_Reference
PlusServerRemoteControl.exe --command=GET_RECONSTRUCTION_SNAPSHOT --output-image-name=recvol_Reference
PlusServerRemoteControl.exe --command=GET_RECONSTRUCTION_SNAPSHOT --output-image-name=recvol_Reference
PlusServerRemoteControl.exe --command=GET_RECONSTRUCTION_SNAPSHOT --output-image-name=recvol_Reference
PlusServerRemoteControl.exe --command=STOP_RECONSTRUCTION --output-image-name=recvol_Reference
```

#### Example: Acquiring exam image data from StealthStation

```console
PlusServerRemoteControl.exe --command=GET_EXAM_DATA --device=StealthLinkDevice
PlusServerRemoteControl.exe --command=GET_EXAM_DATA --device=StealthLinkDevice -keepReceivedDicomFiles --volumeEmbeddedTransformToFrame= "Reference" --dicom-directory="C:/StealthLinkOutput"
```

#### Control the server from 3D Slicer

- In the OpenIGTLinkIF module in Slicer create a new connection, set as client, check Active
- Set up slice visualization with VolumeResliceDriver
- Control PlusServer using the OpenIGTLinkRemote module in Slicer: paste the XML strings below into the Command Console's Command box and click "Send command"

```console
<Command Name="RequestChannelIds" />
<Command Name="RequestDeviceIds" DeviceType="VirtualVolumeReconstructor" />
<Command Name="StartRecording" OutputFilename="PlusServerRecording.nrrd" EnableCompression="True"/>
<Command Name="StopRecording" />
<Command Name="ReconstructVolume" InputSeqFilename="PlusServerRecording.nrrd" OutputVolDeviceName="recvol_Reference" />
<Command Name="StartVolumeReconstruction" VolumeReconstructorDeviceId="VolumeReconstructorDevice" />
<Command Name="SuspendVolumeReconstruction" VolumeReconstructorDeviceId="VolumeReconstructorDevice" />
<Command Name="ResumeVolumeReconstruction" VolumeReconstructorDeviceId="VolumeReconstructorDevice" />
<Command Name="GetVolumeReconstructionSnapshot" VolumeReconstructorDeviceId="VolumeReconstructorDevice" OutputVolDeviceName="recvol_Reference" />
<Command Name="StopVolumeReconstruction" VolumeReconstructorDeviceId="VolumeReconstructorDevice" OutputVolDeviceName="recvol_Reference" OutputVolFilename="recvol_Reference.mha"/>
<Command Name="SetUsParameter" UsDeviceId="USDevice"><Parameter Name="Depth" Value="50"/></Command>
<Command Name="GetUsParameter" UsDeviceId="USDevice"><Parameter Name="Depth"/></Command>
```

## OpenIGTLinkRemoteExec interface specification

### Command

- Message type: `STRING`
- Device name: `CMD_`commandUid
- Contents: an XML element with the name `Command` and the following attributes:
    - `Name`: command name, defined by the user
    - Custom parameters: attributes and/or child elements

#### Example:

```xml
<Command Name="StartRecording" ... custom parameters ... />
```

### Command reply

- Message type: `STRING`
- Device name: `ACK_`commandUid (the commandUid matches the command's UID)
- Contents: an XML element with the name `CommandReply` and the following attributes:
    - `Status`: `SUCCESS` or `FAIL`
    - Custom parameters: attributes and/or child elements
#### Example:
```xml
<CommandReply Status="SUCCESS" Message="StartRecording completed successfully" ... other custom parameters ... />
```

### Commands

- **RequestChannelIds**: returns a list of available channel IDs
- **RequestDeviceIds**: returns a list of available device IDs
    - **DeviceType**: restrict the returned list of devices to a specific type (VirtualCapture, VirtualVolumeReconstructor, etc.)
- **StartRecording**: starts recording to file
    - **CaptureDeviceId** (Required)
    - **OutputFilename**
    - **EnableCompression**: Set capture device to record compressed data. Only supported with .nrrd capture.
- **StopRecording**: stops recording to file
    - **CaptureDeviceId**: (Required)
    - **OutputFilename**: overrides the value that was defined in StartRecording
- **ReconstructVolume**: reconstruct a volume from a file and writes the result to a file and/or sends through OpenIGTLink; sets OutputOrigin and OutputExtent so that all the frames fit
    - **VolumeReconstructorDeviceId**: name of the volume reconstructor device that contains the reconstruction parameters (if not specified then the first volume reconstructor device will be used)
    - **InputSeqFilename**: name of the input sequence metafile name that contains the list of frames ** (Required)
    - **OutputVolFilename**: name of the output volume file name (optional)
    - **OutputVolDeviceName**: name of the OpenIGTLink device for the IMAGE message (optional)
- **StartVolumeReconstruction**: start adding acquired frames to the volume
    - **VolumeReconstructorDeviceId**: name of the volume reconstructor device that contains the reconstruction parameters and defines the input data (if not specified then the first volume reconstructor device will be used)
    - **OutputVolFilename**: name of the output volume file name (optional, if saving of the reconstructed volume to file is not needed or the value is already set)
    - **OutputVolDeviceName**: name of the OpenIGTLink device for the IMAGE message (optional, if sending of the reconstructed volume is not needed or the value is already set)
    - **OutputSpacing**: spacing of the output volume, 3 floating point values in the reference coordinate system (example: "0.3 0.3 0.3") (optional, if already set)
    - **OutputOrigin**: origin of the output volume, 3 floating point values in the reference coordinate system (example: "-15.0 -20.0 40.0") (optional, if already set)
    - **OutputExtent**: extent in voxels, 6 integer values, in voxel coordinates of the output volume: iStart, iStop, jStart, jStop, kStart, kStop (for example"0 150 0 180 0 200"); typically the start values are 0, and the stop values are dimension-1 (optional, if already set)
- **SuspendVolumeReconstruction**: suspend adding acquired frames to the volume
    - **VolumeReconstructorDeviceId**: name of the volume reconstructor device (optional, if not specified then the first volume reconstructor device will be used)
- **ResumeVolumeReconstruction**: resume adding acquired frames to the volume
    - **VolumeReconstructorDeviceId**: name of the volume reconstructor device (optional, if not specified then the first volume reconstructor device will be used)
- **StopVolumeReconstruction**: stop adding acquired frames to the volume, finalize reconstruction, and save/send the results.
    - **VolumeReconstructorDeviceId**: name of the volume reconstructor device (optional, if not specified then the first volume reconstructor device will be used)
    - **OutputVolFilename**: name of the output volume file name (optional)
    - **OutputVolDeviceName**: name of the OpenIGTLink device for the IMAGE message (optional)
- **GetVolumeReconstructionSnapshot**: request a snapshot of the live reconstruction result to be saved/sent.
    - **VolumeReconstructorDeviceId**: name of the volume reconstructor device (if not specified then the first volume reconstructor device will be used)
    - **OutputVolFilename**: name of the output volume file name (optional, if saving of the reconstructed volume to file is not needed or the value is already set)
    - **OutputVolDeviceName**: name of the OpenIGTLink device for the IMAGE message (optional, if sending of the reconstructed volume is not needed or the value is already set)
    - **ApplyHoleFilling**: if FALSE then holes will not be filled (optional, default: TRUE)
- **UpdateTransform**: updates a transform in the transform repository
    - **TransformName**: transform name in CoordinateSystem1ToCoordinateSystem2 format
    - **TransformValue**: 4x4 matrix, separated by spaces
    - **TransformPersistent**: TRUE (default) or FALSE; if FALSE then the transform will not be saved in the device set config file
    - **TransformError**: any >=0 value, currently the value is just stored, not used by algorithms
    - **TransformDate**: date of the transform in any string format, just for reference
- **GetTransform**: retrieves a transform in the transform repository
    - **TransformName**: transform name in CoordinateSystem1ToCoordinateSystem2 format
- **SaveConfig**: save the config file
    - **Filename: target filename, if not specified then the current device set configuration file will be updated
- **GetExamData**: acquire the current image from the StealthStation. This command can only be used for stealthlink connection.
    - **volumeEmbeddedTransformToFrame**: Specify in which coordinate system the volume will be represented. Example: Ras, Reference, Tracker, or any other coordinate system defined in PLUS (The default value is Ras)
    - **dicomDirectory**: The directory where the dicom images will be stored. (The default value is the same as PLUS output directory)
    - **keepReceivedDicomFiles**: StealthLink sends the volume as dicom images. Leave this if you want to keep the dicom files in your computer's directory. (The default value is FALSE, as in the files will be deleted after volume has been created)
- **SendText**: sends text to the device. This command can only be used for GenericSerialDevice. Returns the command response received from the serial device.
    - **DeviceId**: Device ID of the GenericSerialDevice ** (Required)
    - **Text**: String to be sent to the serial device ** (Required)
- **GetPolydata**: requests a polydata file from the server. Returns a command response from the server with the success/fail message and if successful, the polydata.
    - **FileName**: The filename of the polydata to send ** (Required)

### Ultrasound imaging parameter commands

PlusServer can change and retrieve ultrasound imaging parameters over OpenIGTLink using the SetUSParameter and GetUsParameter commands.

#### Commands

- **SetUsParameter**: sets the imaging parameters of the specified ultrasound device
    - **UsDeviceId**: Device ID of the ultrasound device
    - **Parameter**
      - **Name**: name of the parameter to be changed
      - **Value**: value to change the specified parameter to
- **GetUsParameter**: gets the imaging parameters of the specified ultrasound device
    - **UsDeviceId** Device ID of the ultrasound device
    - **Parameter**
      - **Name**: name of the parameter to be retrieved

#### Command replies

- **SetUsParameter**
    - **Parameter**
      - **Name**: name of the changed parameter
      - **Success**: "true" if the value was changed successfully, otherwise "false"
- **GetUsParameter**
    - **UsDeviceId**: Device ID of the ultrasound device
    - **Parameter**
      - **Name**: name of the retrieved parameter
      - **Success**: "true" if the value was retrieved successfully, otherwise "false"
      - **Value**: current value of the ultrasound parameter

### OpenIGTLinkRemoteExec Slicer module API

Basic API:
```c++
int commandId = remoteExecLogic->ExecuteCommand(myConnectorNode, "StartRecording");
int status = remoteExecLogic->GetCommandReply(commandId);
```

Advanced calls:
```c++
int commandId = remoteExecLogic->ExecuteCommand(myConnectorNode, "StartRecording", customParamsXmlDataElement);
int status = remoteExecLogic->GetCommandReply(commandId, customResultsXmlDataElement);
remoteExecLogic->GetCommandNode(commandId);
remoteExecLogic->GetCommandReplyNode(commandId);
```

A new pair of command and reply node is created for each call.

If the caller does not need the command and reply node anymore than the caller has to delete them.
