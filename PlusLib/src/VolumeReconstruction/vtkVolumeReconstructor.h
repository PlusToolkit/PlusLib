#ifndef __vtkVolumeReconstructor_h
#define __vtkVolumeReconstructor_h


#include "vtkBufferedTracker.h"
#include "vtkBufferedVideoSource.h"
#include "vtkImageAlgorithm.h"
#include "vtkFreehandUltrasound2Dynamic.h"
#include "vtkSmartPointer.h"
#include "vtkTransform.h"



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

  virtual void Initialize(); 

  virtual PlusStatus StartReconstruction(); 

  virtual void FillHoles(); 

  virtual PlusStatus ReadConfiguration( const char* configFileName ); 
  virtual PlusStatus ReadConfiguration(); 

  virtual void AddTrackedFrame( vtkImageData* frame, US_IMAGE_ORIENTATION usImageOrientation, vtkMatrix4x4* mToolToReference, double timestamp); 

  virtual void AddTrackedFrame( unsigned char* imageData, 
    US_IMAGE_ORIENTATION usImageOrientation, 
    const int imageWidthInPixels, 
    const int imageHeightInPixels, 
    const double transformMatrix[16], 
    double timestamp); 

  virtual void AddTrackedFrame( unsigned char* imageData, 
    US_IMAGE_ORIENTATION usImageOrientation, 
    const int imageWidthInPixels, 
    const int imageHeightInPixels, 
    vtkMatrix4x4* transformMatrix, 
    double timestamp); 

  virtual void FindOutputExtent( vtkMatrix4x4* frame2TrackerTransMatrix, int* frameExtent); 

  vtkSetVector2Macro( FrameSize, int ); 
  vtkGetVector2Macro( FrameSize, int ); 

  vtkSetMacro(NumberOfBitsPerPixel, int); 
  vtkGetMacro(NumberOfBitsPerPixel, int); 

  vtkSetMacro( NumberOfFrames, int ); 
  vtkGetMacro( NumberOfFrames, int ); 

  vtkSetMacro( Initialized, bool ); 
  vtkGetMacro( Initialized, bool ); 
  vtkBooleanMacro( Initialized, bool ); 

  vtkSetVector3Macro(VolumeExtentMin, double);
  vtkGetVector3Macro(VolumeExtentMin, double);

  vtkSetVector3Macro(VolumeExtentMax, double);
  vtkGetVector3Macro(VolumeExtentMax, double);

  vtkSetObjectMacro( Tracker, vtkBufferedTracker ); 
  vtkGetObjectMacro( Tracker, vtkBufferedTracker ); 

  vtkSetObjectMacro( VideoSource, vtkBufferedVideoSource ); 
  vtkGetObjectMacro( VideoSource, vtkBufferedVideoSource ); 

  vtkSetObjectMacro( Reconstructor, vtkFreehandUltrasound2Dynamic ); 
  vtkGetObjectMacro( Reconstructor, vtkFreehandUltrasound2Dynamic ); 

  vtkSetStringMacro( ConfigFileName ); 
  vtkGetStringMacro( ConfigFileName ); 


  vtkSmartPointer< vtkTransform > GetImageToToolTransform();


protected: 

  vtkVolumeReconstructor();
  virtual ~vtkVolumeReconstructor();

  vtkBufferedTracker* Tracker; 
  vtkBufferedVideoSource* VideoSource; 
  vtkFreehandUltrasound2Dynamic* Reconstructor; 

  int NumberOfFrames; 
  int FrameSize[2];
  int NumberOfBitsPerPixel; 

  bool Initialized; 

  char* ConfigFileName; 

  double VolumeExtentMin[ 3 ]; // X Y Z. In Reference coordinate system.
  double VolumeExtentMax[ 3 ]; // X Y Z. In Reference coordinate system.


private: 

  vtkVolumeReconstructor(const vtkVolumeReconstructor&);  // Not implemented.
  void operator=(const vtkVolumeReconstructor&);  // Not implemented.

  int TrackerToolID;

}; 

#endif