/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkPlusBrachyStepperPhantomRegistrationAlgo_h
#define __vtkPlusBrachyStepperPhantomRegistrationAlgo_h

#include "PlusConfigure.h"
#include "vtkPlusCalibrationExport.h"

#include "vtkObject.h"
#include "vtkIGSIOTrackedFrameList.h"
#include "PlusFidPatternRecognitionCommon.h"

class vtkMatrix4x4;
//class vtkIGSIOTransformRepository;

/*!
  \class vtkPlusBrachyStepperPhantomRegistrationAlgo
  \brief Phantom registration algorithm for image to probe calibration with brachy stepper

  This algorithm determines the phantom to reference transform (the spatial relationship
  between the phantom and the stepper coordinate system).
  The images shall be taken of a calibration phantom and the frames shall be segmented
  (the fiducial point coordinates shall be computed) before calling this algorithm.

  \ingroup PlusLibCalibrationAlgorithm
*/
class vtkPlusCalibrationExport vtkPlusBrachyStepperPhantomRegistrationAlgo : public vtkObject
{
public:

  static vtkPlusBrachyStepperPhantomRegistrationAlgo* New();
  vtkTypeMacro(vtkPlusBrachyStepperPhantomRegistrationAlgo , vtkObject);
  virtual void PrintSelf(ostream& os, vtkIndent indent) VTK_OVERRIDE;

  /*!
    Set all algorithm inputs.
    \param trackedFrameList Tracked frames with segmentation results
    \param spacing Image spacing (mm/px)
    \param centerOfRotationPx Ultrasound image rotation center in px
    \param transformRepository Transform repository to set the result into
    \param nWires Phantom definition structure
  */
  virtual void SetInputs(vtkIGSIOTrackedFrameList* trackedFrameList, double spacing[2], double centerOfRotationPx[2], vtkIGSIOTransformRepository* transformRepository, const std::vector<PlusNWire>& nWires);

  /*!
    Read phantom definition (landmarks)
    \param aConfig Root XML data element containing the tool calibration
  */
  PlusStatus ReadConfiguration(vtkXMLDataElement* aConfig);

  /*! Get phantom to reference transform */
  virtual PlusStatus GetPhantomToReferenceTransformMatrix(vtkMatrix4x4* phantomToReferenceTransform);

  /*! Get phantom coordinate frame */
  vtkGetStringMacro(PhantomCoordinateFrame);

  /*! Get reference coordinate frame */
  vtkGetStringMacro(ReferenceCoordinateFrame);

protected:
  vtkPlusBrachyStepperPhantomRegistrationAlgo();
  virtual ~vtkPlusBrachyStepperPhantomRegistrationAlgo();

  /*! Bring this algorithm's outputs up-to-date. */
  virtual PlusStatus Update();

  /*! Set the input tracked frame list */
  vtkSetObjectMacro(TrackedFrameList, vtkIGSIOTrackedFrameList);

  /*! Get the input tracked frame list */
  vtkGetObjectMacro(TrackedFrameList, vtkIGSIOTrackedFrameList);

  /*! Set spacing */
  vtkSetVector2Macro(Spacing, double);

  /*! Set center of rotation in px */
  vtkSetVector2Macro(CenterOfRotationPx, double);

  /*! Set phantom coordinate frame */
  vtkSetStringMacro(PhantomCoordinateFrame);

  /*! Set reference coordinate frame */
  vtkSetStringMacro(ReferenceCoordinateFrame);

  /*! Set input transform repository */
  void SetTransformRepository(vtkIGSIOTransformRepository*);

protected:
  /*! Image spacing (mm/pixel). Spacing[0]: lateral axis, Spacing[1]: axial axis */
  double Spacing[2];

  /*! Rotation center position of the image in px */
  double CenterOfRotationPx[2];

  /*! Tracked frame list with segmentation results */
  vtkIGSIOTrackedFrameList* TrackedFrameList;

  /*! Phantom definition structure */
  std::vector<PlusNWire> NWires;

  /*! Phantom to Reference transform */
  vtkMatrix4x4* PhantomToReferenceTransformMatrix;

  /*! When the results were computed. The result is recomputed only if the inputs changed more recently than UpdateTime. */
  vtkTimeStamp UpdateTime;

  /*! Transform repository object into that the result is set */
  vtkIGSIOTransformRepository* TransformRepository;

  /*! Name of the phantom coordinate frame (eg. Phantom) */
  char*                     PhantomCoordinateFrame;

  /*! Name of the reference coordinate frame (eg. Reference) */
  char*                     ReferenceCoordinateFrame;
};

#endif