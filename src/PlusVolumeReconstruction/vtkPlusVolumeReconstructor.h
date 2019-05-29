/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkPlusVolumeReconstructor_h
#define __vtkPlusVolumeReconstructor_h

#include "PlusConfigure.h"
#include "vtkPlusVolumeReconstructionExport.h"

// IGSIO includes
#include <igsioCommon.h>
#include <vtkIGSIOVolumeReconstructor.h>

/*!
  \class vtkPlusVolumeReconstructor
  \brief Reconstructs a volume from tracked frames

  This is a convenience class for inserting tracked frames into a volume
  using the vtkPlusPasteSliceIntoVolume algorithm.
  It reads/writes reconstruction parameters and the calibration matrix from
  configuration XML data element and can also compute the size of the output
  volume that can contain all the frames.

  Coordinate systems to be used in this class:
  Image: Coordinate system aligned to the frame. Unit: pixels.
    Origin: the first pixel in the image pixel array as stored in memory. Not cropped
    to US content of the image.
  Tool: Unit: mm. Origin: origin of the sensor/DRB mounted on the tracked tool (probe).
  Tracker: Unit: mm. Origin: origin of the tracker device (usually camera or EM transmitter).
  Reference: Unit: mm. Origin: origin of the sensor/DRB mounted on the reference tool
    (DRB fixed to the imaged object)

  If no reference DRB is used then use Identity ReferenceToTracker transforms, and so
  Reference will be the same as Tracker. So we can still refer to the output system as Reference.

  \sa vtkPlusPasteSliceIntoVolume
  \ingroup PlusLibVolumeReconstruction
*/
class vtkPlusVolumeReconstructionExport vtkPlusVolumeReconstructor : public vtkIGSIOVolumeReconstructor
{
public:

  static vtkPlusVolumeReconstructor* New();
  vtkTypeMacro(vtkPlusVolumeReconstructor, vtkIGSIOVolumeReconstructor);
  virtual void PrintSelf(ostream& os, vtkIndent indent) override;

  virtual PlusStatus UpdateImportanceMask() override;

  /*!
    Save reconstructed volume to file
    \param filename Path and filename of the output file
    \accumulation True if accumulation buffer needs to be saved, false if gray levels (default)
    \useCompression True if compression is turned on (default), false otherwise
  */
  virtual PlusStatus SaveReconstructedVolumeToFile(const std::string& filename, bool accumulation = false, bool useCompression = true) override;
  virtual PlusStatus SaveReconstructedVolumeToMetafile(const std::string& filename, bool accumulation = false, bool useCompression = true) { return SaveReconstructedVolumeToFile(filename, accumulation, useCompression); }

  /*!
    Save reconstructed volume to file
    \param volumeToSave Reconstructed volume to be saved
    \param filename Path and filename of the output file
    \useCompression True if compression is turned on (default), false otherwise
  */
  static PlusStatus SaveReconstructedVolumeToFile(vtkImageData* volumeToSave, const std::string& filename, bool useCompression = true);
  static PlusStatus SaveReconstructedVolumeToMetafile(vtkImageData* volumeToSave, const std::string& filename, bool useCompression = true) { return vtkPlusVolumeReconstructor::SaveReconstructedVolumeToFile(volumeToSave, filename, useCompression); }

protected:
  vtkPlusVolumeReconstructor();
  virtual ~vtkPlusVolumeReconstructor();

private:
  vtkPlusVolumeReconstructor(const vtkPlusVolumeReconstructor&);  // Not implemented.
  void operator=(const vtkPlusVolumeReconstructor&);  // Not implemented.
};

#endif
