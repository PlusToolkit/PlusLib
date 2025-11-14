# Scan conversion (ScanConvert)

This is a command-line tool performing scan conversion on the input images. Input images must be rectangular, containing scan lines of the geometrically correct B-mode images.
Scan line parameters are read from ScanConversion element of the device set configuration file.
Scan line images can be generated from original geometrically correct images using the ExtractScanLines tool.

This is an example scan lines image:

![Spine ultrasound scan lines](../images/SpineUsScanLines.png)

This is the corresponding output image after scan conversion:

![Spine ultrasound converted](../images/SpineUsConverted.png)

## Examples

    ApplicationScanConvert
    --config-file=SpineUltrasound-Lumbar-C5_config.xml
    --input-seq-file=SpineUltrasound-Lumbar-C5_ScanLines.mha
    --output-seq-file=SpineUltrasound-Lumbar-C5_ScanConverted.mha 

## Command-line parameters reference

\verbinclude "ScanConvertHelp.txt"