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

/**
* Coordinate systems to be used in this class:
*
* Image     : Pixels. Origin: a corner of the US slice. TODO: which corner?
*             Takes the whole saved image into account. Usually 640x480.
*             Not cropped to US content of the image.
* Tool      : Millimeters. Origin: origin of the sensor/DRB mounted on the tracked tool.
* Tracker   : Millimeters. Origin: origin of the tracker device (usually camera or EM transmitter).
* Reference : Millimeters. Origin: origin of the sensor/DRB mounted on the reference tool
*             (when a reference DRB is used).
* 
* The standard output system is Reference. In case no reference DRB is used,
* use Identity ReferenceToTracker transforms, and Reference will be the same
* as Tracker. So we can still refer to the output system as Reference.
*/
class VTK_EXPORT vtkVolumeReconstructor : public vtkImageAlgorithm
{
public:

  static vtkVolumeReconstructor *New();
  vtkTypeRevisionMacro(vtkVolumeReconstructor, vtkImageAlgorithm);
  virtual void PrintSelf(ostream& os, vtkIndent indent);

  virtual PlusStatus ReadConfiguration( vtkXMLDataElement* config); 
  virtual PlusStatus WriteConfiguration(vtkXMLDataElement *config);

  virtual PlusStatus SetOutputExtentFromFrameList(vtkTrackedFrameList* trackedFrameList);

  virtual PlusStatus AddTrackedFrame(TrackedFrame* frame);

  virtual PlusStatus GetReconstructedVolume(vtkImageData* reconstructedVolume);

  vtkTransform* GetImageToToolTransform();
  void GetImageToReferenceTransformMatrix(vtkMatrix4x4* toolToReferenceTransformMatrix, vtkMatrix4x4* imageToReferenceTransformMatrix);
  PlusStatus GetImageToReferenceTransformMatrix(TrackedFrame* frame, vtkMatrix4x4* imageToReferenceTransformMatrix);

protected: 

  vtkVolumeReconstructor();
  virtual ~vtkVolumeReconstructor();

  static void AddImageToExtent( vtkImageData *image, vtkMatrix4x4* mImageToReference, double* extent_Ref);

  vtkPasteSliceIntoVolume* Reconstructor; 
  int FillHoles;
  
  // The calibration matrix, constant throughout the reconstruction
  vtkTransform* ImageToToolTransform;

private: 

  vtkVolumeReconstructor(const vtkVolumeReconstructor&);  // Not implemented.
  void operator=(const vtkVolumeReconstructor&);  // Not implemented.
}; 

#endif
