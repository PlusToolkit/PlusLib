# Sequence file viewer (ViewSequenceFile)

This is a simple tool for viewing frames of a sequence file interactively in 3D.
You can browse between the slices by pressing the '+' and '-' keys and rotate the view by using the mouse.

![View sequence file screenshot](../images/ApplicationViewSequenceFileScreenshot.png)

## Examples

    ViewSequenceFile.exe --config-file=SpinePhantomFreehandReconstructionOnly.xml --source-seq-file=SpinePhantomFreehand.mha --image-to-reference-transform=ImageToTracker

## Command-line parameters reference

\verbinclude "ViewSequenceFileHelp.txt"