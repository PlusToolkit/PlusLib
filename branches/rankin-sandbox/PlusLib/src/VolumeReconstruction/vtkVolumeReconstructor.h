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

  vtkGetMacro(SkipInterval,int);

  /*! Read configuration data (volume reconstruction options and calibration matrix) */
  virtual PlusStatus ReadConfiguration( vtkXMLDataElement* config); 
  /*! Write configuration data (volume reconstruction options and calibration matrix) */
  virtual PlusStatus WriteConfiguration(vtkXMLDataElement *config);

  /*!
    Automatically adjusts the reconstruced volume size to enclose all the
    frames in the supplied vtkTrackedFrameList. It clears the reconstructed volume.
  */
  virtual PlusStatus SetOutputExtentFromFrameList(vtkTrackedFrameList* trackedFrameList, vtkTransformRepository* transformRepository);

  /*! 
    Inserts the tracked frame into the volume. The origin, spacing, and extent of the output volume
    must be set before calling this method (either by calling the SetOutputExtentFromFrameList method
    or setting the OutputSpacing, OutputOrigin, and OutputExtent attributes in the configuration data
    element).
  */
  virtual PlusStatus AddTrackedFrame(TrackedFrame* frame, vtkTransformRepository* transformRepository, bool* insertedIntoVolume=NULL);

  /*! 
    Makes the reconstructed volume ready to be retrieved.
    The slices are pasted into the volume immediately, but hole filling is performed only when this method is called.
  */
  virtual PlusStatus UpdateReconstructedVolume();

  /*! Load the reconstructed volume into the volume pointer */
  virtual PlusStatus GetReconstructedVolume(vtkImageData* volume);

  /*! Apply hole filling to the reconstructed image, is called by UpdateReconstructedVolume so an explicit call is not needed */
  virtual PlusStatus GenerateHoleFilledVolume();

  /*! Returns the reconstructed volume gray levels from the provided volume */
  virtual PlusStatus ExtractGrayLevels(vtkImageData* volume);

  /*!
    Returns the accumulation buffer (alpha channel) of the provided volume.
    If a voxel is filled in the reconstructed volume, then the corresponding voxel 
    in the alpha channel is non-zero.
  */
  virtual PlusStatus ExtractAlpha(vtkImageData* volume);

  /*!
    Save reconstructed volume to metafile
    \param filename Path and filename of the output file
    \alpha True if alpha channel needs to be saved, false if gray levels (default)
    \useCompression True if compression is turned on (default), false otherwise
  */
  PlusStatus SaveReconstructedVolumeToMetafile(const char* filename, bool alpha=false, bool useCompression=true);

  /*!
    Save reconstructed volume to VTK file
    \param filename Path and filename of the output file
    \alpha True if alpha channel needs to be saved, false if gray levels (default)
  */
  PlusStatus SaveReconstructedVolumeToVtkFile(const char* filename, bool alpha=false);

  /*! Get/set the Image coordinate system name. It overrides the value read from the config file. */
  vtkGetStringMacro(ImageCoordinateFrame);
  vtkSetStringMacro(ImageCoordinateFrame);

  /*! Get/set the Reference coordinate system name. It overrides the value read from the config file. */
  vtkGetStringMacro(ReferenceCoordinateFrame);
  vtkSetStringMacro(ReferenceCoordinateFrame);

protected: 
  vtkVolumeReconstructor();
  virtual ~vtkVolumeReconstructor();

  /*! Helper function for computing the extent of the reconstructed volume that encloses all the frames */
  static void AddImageToExtent( vtkImageData *image, vtkMatrix4x4* imageToReference, double* extent_Ref);

  /*! Construct ImageToReference transform name from the image and reference coordinate frame member variables */
  PlusStatus GetImageToReferenceTransformName(PlusTransformName& imageToReferenceTransformName);

  vtkPasteSliceIntoVolume* Reconstructor; 
  vtkFillHolesInVolume* HoleFiller; 
 
  vtkSmartPointer<vtkImageData> ReconstructedVolume;

  /*! Defines the image coordinate system name: it corresponds to the image data in the tracked frame */
  char* ImageCoordinateFrame;

  /*! 
    Defines the Reference coordinate system name: the volume will be reconstructed in this coordinate system 
    (the volume axes are parallel to the Reference coordinate system axes and the volume origin position is defined in
    the Reference coordinate system)
  */
  char* ReferenceCoordinateFrame;

  /*! If enabled then the hole filling will be applied on output reconstructed volume */
  int FillHoles;

  /*! only every [SkipInterval] images from the input will be used in the reconstruction (Ie this is the number of frames that are skipped when the index is increased) */
  int SkipInterval;

  /*! Modified time when reconstructing. This is used to determine whether re-reconstruction is necessary */
  unsigned long ReconstructedVolumeUpdatedTime;

private: 
  vtkVolumeReconstructor(const vtkVolumeReconstructor&);  // Not implemented.
  void operator=(const vtkVolumeReconstructor&);  // Not implemented.
}; 

#endif
