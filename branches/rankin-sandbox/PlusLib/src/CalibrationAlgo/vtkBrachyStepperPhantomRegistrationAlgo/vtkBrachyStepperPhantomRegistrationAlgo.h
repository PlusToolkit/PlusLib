/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/ 

#ifndef __vtkBrachyStepperPhantomRegistrationAlgo_h
#define __vtkBrachyStepperPhantomRegistrationAlgo_h

#include "PlusConfigure.h"
#include "vtkObject.h"
#include "vtkTrackedFrameList.h"
#include "vtkTable.h"
#include "FidPatternRecognitionCommon.h"

class vtkHTMLGenerator; 
class vtkGnuplotExecuter; 
class vtkMatrix4x4;
class vtkTransformRepository;

/*!
  \class vtkBrachyStepperPhantomRegistrationAlgo 
  \brief Phantom registration algorithm for image to probe calibration with brachy stepper 

  This algorithm determines the phantom to reference transform (the spatial relationship
  between the phantom and the stepper coordinate system).
  The images shall be taken of a calibration phantom and the frames shall be segmented
  (the fiducial point coordinates shall be computed) before calling this algorithm.

  \ingroup PlusLibCalibrationAlgorithm
*/ 
class VTK_EXPORT vtkBrachyStepperPhantomRegistrationAlgo : public vtkObject
{
public:

  static vtkBrachyStepperPhantomRegistrationAlgo *New();
  vtkTypeRevisionMacro(vtkBrachyStepperPhantomRegistrationAlgo , vtkObject);
  virtual void PrintSelf(ostream& os, vtkIndent indent); 

  /*!
    Set all algorithm inputs.
    \param trackedFrameList Tracked frames with segmentation results 
    \param spacing Image spacing (mm/px)
    \param centerOfRotationPx Ultrasound image rotation center in px
    \param transformRepository Transform repositoyr to set the result into
    \param nWires Phantom definition structure 
  */
  virtual void SetInputs( vtkTrackedFrameList* trackedFrameList, double spacing[2], double centerOfRotationPx[2], vtkTransformRepository* transformRepository, const std::vector<NWire>& nWires ); 

  /*!
    Read phantom definition (landmarks)
    \param aConfig Root XML data element containing the tool calibration
  */
  PlusStatus ReadConfiguration(vtkXMLDataElement* aConfig);

  /*! Get phantom to reference transform */
  virtual PlusStatus GetPhantomToReferenceTransformMatrix( vtkMatrix4x4* phantomToReferenceTransform);

  /*! Get phantom coordinate frame */
  vtkGetStringMacro(PhantomCoordinateFrame);

  /*! Get reference coordinate frame */
  vtkGetStringMacro(ReferenceCoordinateFrame);
  
protected:
  vtkBrachyStepperPhantomRegistrationAlgo();
  virtual ~vtkBrachyStepperPhantomRegistrationAlgo(); 

  /*! Bring this algorithm's outputs up-to-date. */
  virtual PlusStatus Update();

  /*! Set the input tracked frame list */
  vtkSetObjectMacro(TrackedFrameList, vtkTrackedFrameList); 

  /*! Get the input tracked frame list */
  vtkGetObjectMacro(TrackedFrameList, vtkTrackedFrameList); 

  /*! Set spacing */
  vtkSetVector2Macro(Spacing, double); 

  /*! Set center of rotation in px */
  vtkSetVector2Macro(CenterOfRotationPx, double); 

  /*! Set phantom coordinate frame */
  vtkSetStringMacro(PhantomCoordinateFrame);

  /*! Set reference coordinate frame */
  vtkSetStringMacro(ReferenceCoordinateFrame);

  /*! Set input transform repository */
  void SetTransformRepository(vtkTransformRepository*);

protected:
  /*! Image spacing (mm/pixel). Spacing[0]: lateral axis, Spacing[1]: axial axis */
  double Spacing[2];

  /*! Rotation center position of the image in px */
  double CenterOfRotationPx[2]; 

  /*! Tracked frame list with segmentation results */
  vtkTrackedFrameList* TrackedFrameList; 

  /*! Phantom definition structure */
  std::vector<NWire> NWires; 

  /*! Phantom to Reference transform */
  vtkMatrix4x4* PhantomToReferenceTransformMatrix;
  
  /*! When the results were computed. The result is recomputed only if the inputs changed more recently than UpdateTime. */
  vtkTimeStamp UpdateTime;  

  /*! Transform repository object into that the result is set */
  vtkTransformRepository* TransformRepository;

  /*! Name of the phantom coordinate frame (eg. Phantom) */
  char*                     PhantomCoordinateFrame;

  /*! Name of the reference coordinate frame (eg. Reference) */
  char*                     ReferenceCoordinateFrame;
}; 

#endif
