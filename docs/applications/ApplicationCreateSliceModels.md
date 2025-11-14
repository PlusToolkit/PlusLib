# Create slice models (CreateSliceModels)

The CreateSliceModels.exe utility (part of PlusLib) can be used to verify the slice positions. From a metafile containing
tracked image frames it creates a surface model file of all the tracked frame positions. The surface model file can be
visualized in Slicer or ParaView.

![Create slice models](../images/ApplicationCreateSliceModels.png)

## Examples

~~~
CreateSliceModels.exe --source-seq-file=NwirePhantomFreehand.mha --output-model-file=GeneratedSliceModels.vtk --config-file=PlusDeviceSet_NwirePhantomFreehand_vtkPlusVolumeReconstructorTest2.xml --image-to-reference-transform=ProbeToReference
~~~

## Command-line parameters reference

\verbinclude "CreateSliceModelsHelp.txt"