/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkVolumeReconstructor_h
#define __vtkVolumeReconstructor_h

#include "PlusConfigure.h"
#include "vtkImageAlgorithm.h"
#include "vtkPasteSliceIntoVolume.h"
#include "vtkSmartPointer.h"
#include "vtkTransform.h"
#include "vtkFillHolesInVolume.h"

class vtkPasteSliceIntoVolume;
class vtkTrackedFrameList;
class TrackedFrame;
class vtkTransformRepository; 

/*!
  \class vtkVolumeReconstructor
  \brief Reconstructs a volume from tracked frames

  This is a convenience class for inserting tracked frames into a volume
  using the vtkPasteSliceIntoVolume algorithm.
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

  \sa vtkPasteSliceIntoVolume
  \ingroup PlusLibVolumeReconstruction
*/
class VTK_EXPORT vtkVolumeReconstructor : public vtkImageAlgorithm
{
public:

  static vtkVolumeReconstructor *New();
  vtkTypeRevisionMacro(vtkVolumeReconstructor, vtkImageAlgorithm);
  virtual void PrintSelf(ostream& os, vtkIndent indent);

  /*! Read configuration data (volume reconstruction options and calibration matrix) */
  virtual PlusStatus ReadConfiguration( vtkXMLDataElement* config); 
  /*! Write configuration data (volume reconstruction options and calibration matrix) */
  virtual PlusStatus WriteConfiguration(vtkXMLDataElement *config);

  /*!
    Automatically adjusts the reconstruced volume size to enclose all the
    frames in the supplied vtkTrackedFrameList. It clears the reconstructed volume.
  */
  virtual PlusStatus SetOutputExtentFromFrameList(vtkTrackedFrameList* trackedFrameList, vtkTransformRepository* transformRepository, PlusTransformName& imageToReferenceTransformName);

  /*! 
    Inserts the tracked frame into the volume. The origin, spacing, and extent of the output volume
    must be set before calling this method (either by calling the SetOutputExtentFromFrameList method
    or setting the OutputSpacing, OutputOrigin, and OutputExtent attributes in the configuration data
    element).
  */
  virtual PlusStatus AddTrackedFrame(TrackedFrame* frame, vtkTransformRepository* transformRepository, PlusTransformName& imageToReferenceTransformName, bool* insertedIntoVolume=NULL);

  /*! Returns the reconstructed volume */
  virtual PlusStatus GetReconstructedVolume(vtkImageData* reconstructedVolume);

  /*!
    Returns the accumulation buffer (alpha channel) of the reconstructed volume.
    If a voxel is filled in the reconstructed volume, then the corresponding voxel 
    in the alpha channel is non-zero.
  */
  virtual PlusStatus GetReconstructedVolumeAlpha(vtkImageData* reconstructedVolume);

protected: 
  vtkVolumeReconstructor();
  virtual ~vtkVolumeReconstructor();

  /*! Helper function for computing the extent of the reconstructed volume that encloses all the frames */
  static void AddImageToExtent( vtkImageData *image, vtkMatrix4x4* mImageToReference, double* extent_Ref);

  vtkPasteSliceIntoVolume* Reconstructor; 
  vtkFillHolesInVolume* HoleFiller; 
  
  /*! If enabled then the hole filling will be applied on output reconstructed volume */
  int FillHoles;

private: 
  vtkVolumeReconstructor(const vtkVolumeReconstructor&);  // Not implemented.
  void operator=(const vtkVolumeReconstructor&);  // Not implemented.
}; 

#endif
