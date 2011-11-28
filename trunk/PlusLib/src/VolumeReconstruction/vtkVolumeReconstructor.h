/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

// .NAME vtkVolumeReconstructor - reconstructs a volume from tracked frames
// .SECTION Description
// This is a convenience class for inserting tracked frames into a volume
// using the vtkPasteSliceIntoVolume algorithm.
// It reads/writes reconstruction parameters and the calibration matrix from
// configuration XML data element and can also compute the size of the output
// volume that can contain all the frames.
//
// Coordinate systems to be used in this class:
// Image     : Coordinate system aligned to the frame. Unit: pixels. Origin: the first pixel in the image pixel array as stored in memory.
//             Not cropped to US content of the image.
// Tool      : Unit: mm. Origin: origin of the sensor/DRB mounted on the tracked tool (probe).
// Tracker   : Unit: mm. Origin: origin of the tracker device (usually camera or EM transmitter).
// Reference : Unit: mm. Origin: origin of the sensor/DRB mounted on the reference tool (DRB fixed to the imaged object)
// 
// If no reference DRB is used then use Identity ReferenceToTracker transforms, and so
// Reference will be the same as Tracker. So we can still refer to the output system as Reference.
//
// .SECTION see also
// vtkPasteSliceIntoVolume

#ifndef __vtkVolumeReconstructor_h
#define __vtkVolumeReconstructor_h

#include "PlusConfigure.h"
#include "vtkImageAlgorithm.h"
#include "vtkPasteSliceIntoVolume.h"
#include "vtkSmartPointer.h"
#include "vtkTransform.h"

class vtkPasteSliceIntoVolume;
class vtkTrackedFrameList;
class TrackedFrame;

class VTK_EXPORT vtkVolumeReconstructor : public vtkImageAlgorithm
{
public:

  static vtkVolumeReconstructor *New();
  vtkTypeRevisionMacro(vtkVolumeReconstructor, vtkImageAlgorithm);
  virtual void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Read/write configuration data (volume reconstruction options and calibration matrix)
  virtual PlusStatus ReadConfiguration( vtkXMLDataElement* config); 
  virtual PlusStatus WriteConfiguration(vtkXMLDataElement *config);

  // Description:
  // Automatically adjusts the reconstruced volume size to enclose all the
  // frames in the supplied vtkTrackedFrameList. It clears the reconstructed volume.
  virtual PlusStatus SetOutputExtentFromFrameList(vtkTrackedFrameList* trackedFrameList);

  // Description:
  // Inserts the tracked frame into the volume.
  virtual PlusStatus AddTrackedFrame(TrackedFrame* frame, PlusTransformName& toolToReferenceTransformName);

  // Description:
  // Returns the reconstructed volume.
  virtual PlusStatus GetReconstructedVolume(vtkImageData* reconstructedVolume);

  // Description:
  // Returns the alpha channel of the reconstructed volume. If a voxel is filled in the
  // reconstructed volume, then the corresponding voxel in the alpha channel is non-zero.
  virtual PlusStatus GetReconstructedVolumeAlpha(vtkImageData* reconstructedVolume);

  // Description:
  // Get the image to tool transform (the calibration matrix)
  vtkTransform* GetImageToToolTransform();

  // Description:
  // Get the image to reference transform (the image frame pose in the reference coordinate system)
  // from the tool to reference transform (the transform provided by the tracked, which is different for each frame)
  PlusStatus GetImageToReferenceTransformMatrix(vtkMatrix4x4* toolToReferenceTransformMatrix, vtkMatrix4x4* imageToReferenceTransformMatrix);
  PlusStatus GetImageToReferenceTransformMatrix(TrackedFrame* frame, PlusTransformName& toolToReferenceTransformName, vtkMatrix4x4* imageToReferenceTransformMatrix);

protected: 
  vtkVolumeReconstructor();
  virtual ~vtkVolumeReconstructor();

  // Description:
  // Helper function for computing the extent of the reconstructed volume that encloses
  // all the frames.
  static void AddImageToExtent( vtkImageData *image, vtkMatrix4x4* mImageToReference, double* extent_Ref);


  vtkPasteSliceIntoVolume* Reconstructor; 
  
  // If enabled then the hole filling will be applied on output reconstructed volume
  int FillHoles;
  
  // The calibration matrix, constant throughout the reconstruction
  vtkTransform* ImageToToolTransform;

private: 

  vtkVolumeReconstructor(const vtkVolumeReconstructor&);  // Not implemented.
  void operator=(const vtkVolumeReconstructor&);  // Not implemented.
}; 

#endif
