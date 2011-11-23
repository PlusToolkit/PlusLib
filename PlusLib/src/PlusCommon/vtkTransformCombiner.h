/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkTransformCombiner_h
#define __vtkTransformCombiner_h

#include "PlusConfigure.h"
#include "vtkObject.h"

class vtkMatrix4x4;
class vtkTransform;

/*!
\class vtkTransformCombiner 
\brief Combine multiple transforms to get a transform between arbitrary coordinate frames

The vtkTransformCombiner stores a number of transforms between coordinate frames and
it can multiply these transforms (or the inverse of these transforms) to
compute the transform between any two coordinate frames.

\ingroup PlusLibCommon
*/
class VTK_EXPORT vtkTransformCombiner : public vtkObject
{
public:
  static vtkTransformCombiner *New();
  vtkTypeMacro(vtkTransformCombiner,vtkObject);
  void PrintSelf(ostream& os, vtkIndent indent);

  enum TransformStatus
  {
    /*! The transform matrix is valid and up-to-date */
    TRANSFORM_VALID,
    /*! The transform matrix is temporarily unknown (e.g., tracked tool is out of view) */
    TRANSFORM_INVALID
  };

  /*! 
    Set a transform matrix between two coordinate frames. The method fails if the transform
    can be already constructed by concatenating/inverting already stored transforms. Changing an already
    set transform is allowed. The transform is computed even if one or more of the used transforms
    have TRANSFORM_INVALID status.
  */
  virtual PlusStatus SetTransform(const char* fromCoordFrameName, const char* toCoordFrameName, vtkMatrix4x4* matrix);
  
  /*! 
    Set the status of a transform matrix between two coordinate frames. A transform is normally TRANSFORM_VALID,
    but temporarily it can be set to TRANSFORM_INVALID (e.g., when a tracked tool gets out of view).    
  */
  virtual PlusStatus SetTransformStatus(const char* fromCoordFrameName, const char* toCoordFrameName, TransformStatus status);

  /*! 
    Get a transform matrix between two coordinate frames. The method fails if the transform
    cannot be already constructed by combining/inverting already stored transforms.
    The returned pointer will remain valid and its content gets updated automatically until all the transforms
  */
  virtual PlusStatus GetTransform(const char* fromCoordFrameName, const char* toCoordFrameName, vtkTransform** transform);

  /*! Get the status of a transform matrix between two coordinate frames. */
  virtual PlusStatus GetTransformStatus(const char* fromCoordFrameName, const char* toCoordFrameName, TransformStatus &status);

protected:
  vtkTransformCombiner();
  ~vtkTransformCombiner();

private:
  vtkTransformCombiner(const vtkTransformCombiner&);
  void operator=(const vtkTransformCombiner&);  
};

#endif

