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
class vtkTransform;

/*! \class vtkBrachyStepperPhantomRegistrationAlgo 
 *
 *  \brief Phantom registration algorithm for image to probe calibration with brachy stepper 
 *
 *  \ingroup PlusLibCalibrationAlgorithm
 *
 */ 
class VTK_EXPORT vtkBrachyStepperPhantomRegistrationAlgo : public vtkObject
{
public:

  static vtkBrachyStepperPhantomRegistrationAlgo *New();
  vtkTypeRevisionMacro(vtkBrachyStepperPhantomRegistrationAlgo , vtkObject);
  virtual void PrintSelf(ostream& os, vtkIndent indent); 

  /*! Set inputs: 
    \param trackedFrameList Tracked frames with segmentation results 
    \param spacing Image spacing (mm/px)
    \param centerOfRotationPx Ultrasound image rotation center in px
    \param nWires Phantom definition structure 
  */
  virtual void SetInputs( vtkTrackedFrameList* trackedFrameList, double spacing[2], double centerOfRotationPx[2], const std::vector<NWire>& nWires ); 

  /*! Get phantom to reference transform */
  virtual PlusStatus GetPhantomToReferenceTransform( vtkTransform* phantomToReferenceTransform);

  // TODO: remove it just use it for getting the same result as the baseline
  vtkSetObjectMacro(TransformTemplateHolderToPhantom, vtkTransform); 
  vtkGetObjectMacro(TransformTemplateHolderToPhantom, vtkTransform); 
  vtkGetObjectMacro(TransformReferenceToTemplateHolder, vtkTransform); 
  
protected:
  vtkBrachyStepperPhantomRegistrationAlgo();
  virtual ~vtkBrachyStepperPhantomRegistrationAlgo(); 

  /*! Bring this algorithm's outputs up-to-date. */
  virtual PlusStatus Update();

  /*! Set/get tracked frame list */
  vtkSetObjectMacro(TrackedFrameList, vtkTrackedFrameList); 
  vtkGetObjectMacro(TrackedFrameList, vtkTrackedFrameList); 

  /*! Set spacing */
  vtkSetVector2Macro(Spacing, double); 

  /*! Set center of rotation in px  */
  vtkSetVector2Macro(CenterOfRotationPx, double); 

  /*! Image scaling factors (x: lateral axis, y: axial axis) */
  double Spacing[2];

  /*! Rotation center position of the image in px */
  double CenterOfRotationPx[2]; 

  /*! Tracked frame list with segmentation results */
  vtkTrackedFrameList* TrackedFrameList; 

  /*! Phantom definition structure */
  std::vector<NWire> NWires; 

  /*! Phantom to Reference transform */
  vtkTransform* PhantomToReferenceTransform; 
  
  // TODO remove it!
  vtkTransform* TransformTemplateHolderToPhantom; 
  vtkTransform* TransformReferenceToTemplateHolder; 

  /*! When was this data last updated */
  vtkTimeStamp UpdateTime;  

}; 

#endif