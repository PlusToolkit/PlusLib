# Draw clip region (DrawClipRegion)

This is a command-line tool for drawing clip rectangle and fan over ultrasound images.
Clipping parameters are read from `ElementVolumeReconstruction` "VolumeReconstruction" element of the device set configuration file.

If automatic fan angle detection is enabled then the maximum considered fan region is shown with dotted line and the actually detected
and used fan region is shown with solid line. See demo video at <https://www.youtube.com/watch?v=ss7ZTRTNWio> .

![Draw clip region example](../images/ApplicationDrawClipRegionExample.png)

## Examples

    DrawClipRegion --config-file=PlusDeviceSet_VolumeReconstructionOnly_FanAnglesAutoDetect.xml --source-seq-file=UsSimulatorOutputSpinePhantom2CurvilinearBaseline.mha --output-seq-file=UsSimulatorOutputSpinePhantom2CurvilinearBaselineWithClipRegion.mha

## Command-line parameters reference

\verbinclude "DrawClipRegionHelp.txt"