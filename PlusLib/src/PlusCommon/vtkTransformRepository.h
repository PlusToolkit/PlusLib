/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkTransformRepository_h
#define __vtkTransformRepository_h

#include "PlusConfigure.h"

#include <map>
#include <list>

#include "vtkObject.h"

class vtkMatrix4x4;
class vtkTransform;

/*!
\class vtkTransformRepository 
\brief Combine multiple transforms to get a transform between arbitrary coordinate frames

The vtkTransformRepository stores a number of transforms between coordinate frames and
it can multiply these transforms (or the inverse of these transforms) to
compute the transform between any two coordinate frames.

Example usage:
  transformRepository->SetTransform("Probe", "Tracker", mxProbeToTracker);
  transformRepository->SetTransform("Image", "Probe", mxImageToProbe);
  ...
  vtkSmartPointer<vtkMatrix4x4> mxImageToTracker=vtkSmartPointer<vtkMatrix4x4>::New();
  vtkTransformRepository::TransformStatus status=vtkTransformRepository::TRANSFORM_INVALID;
  transformRepository->GetTransform("Image", "Tracker", mxImageToTracker, &status);

\ingroup PlusLibCommon
*/
class VTK_EXPORT vtkTransformRepository : public vtkObject
{
public:
  static vtkTransformRepository *New();
  vtkTypeMacro(vtkTransformRepository,vtkObject);
  void PrintSelf(ostream& os, vtkIndent indent);

  enum TransformStatus
  {
    /*! The transform matrix is valid and up-to-date */
    TRANSFORM_VALID,
    /*! The transform matrix is temporarily unknown (e.g., tracked tool is out of view) */
    TRANSFORM_INVALID,
    /*! The status is unknown, so keep using the current status */
    KEEP_CURRENT_STATUS
  };

  /*! 
    Set a transform matrix between two coordinate frames. The method fails if the transform
    can be already constructed by concatenating/inverting already stored transforms. Changing an already
    set transform is allowed. The transform is computed even if one or more of the used transforms
    have TRANSFORM_INVALID status.
  */
  virtual PlusStatus SetTransform(const char* fromCoordFrameName, const char* toCoordFrameName, vtkMatrix4x4* matrix, TransformStatus status = KEEP_CURRENT_STATUS);
  
  /*! 
    Set the status of a transform matrix between two coordinate frames. A transform is normally TRANSFORM_VALID,
    but temporarily it can be set to TRANSFORM_INVALID (e.g., when a tracked tool gets out of view).    
  */
  virtual PlusStatus SetTransformStatus(const char* fromCoordFrameName, const char* toCoordFrameName, TransformStatus status);

  /*! 
    Get a transform matrix between two coordinate frames. The method fails if the transform
    cannot be already constructed by combining/inverting already stored transforms.
    \param matrix the retrieved transform is copied into this matrix 
    \param status if this parameter is not NULL then the transform's status is returned at that memory address 
  */
  virtual PlusStatus GetTransform(const char* fromCoordFrameName, const char* toCoordFrameName, vtkMatrix4x4* matrix, TransformStatus* status=NULL);

  /*! Get the status of a transform matrix between two coordinate frames. */
  virtual PlusStatus GetTransformStatus(const char* fromCoordFrameName, const char* toCoordFrameName, TransformStatus &status);

  /*! Removes a transform from the repository */
  virtual PlusStatus DeleteTransform(const char* fromCoordFrameName, const char* toCoordFrameName);

  /*! Removes all the transforms from the repository */
  void Clear();

protected:
  vtkTransformRepository();
  ~vtkTransformRepository();  

  /*! Stores a transformation matrix and some additional information (valid or not, computed or not)*/ 
  class TransformInfo
  {
  public:
    TransformInfo();
    virtual ~TransformInfo();
    TransformInfo(const TransformInfo& obj);
    TransformInfo& operator=(const TransformInfo& obj);

    /*! TransformInfo storing the transformation matrix between two coordinate frames */
    vtkTransform* m_Transform;
    /*! If it is true it means that the transform is known (e.g., tracked tool is visible) */
    bool m_IsValid;
    /*!
      If the value is true then it means that the transform is computed from
      another transform (by inverting that). If the value is false it means
      that it is an original transform (set by the user by a SetTransform() method call)
    */
    bool m_IsComputed;
  };

  /*! For each "to" coordinate frame name (first) stores a transform (second)*/
  typedef std::map<std::string, TransformInfo> CoordFrameToTransformMapType;
  /*! For each "from" coordinate frame (first) stores an array of transforms (second) */
  typedef std::map<std::string, CoordFrameToTransformMapType> CoordFrameToCoordFrameToTransformMapType;

  /*! List of transforms */
  typedef std::list<TransformInfo*> TransformInfoListType;

  /*! Get a user-defined input transform (or its inverse). Does not combine user-defined input transforms. */ 
  TransformInfo* GetInputTransform(const char* fromCoordFrameName, const char* toCoordFrameName);

  /*! 
    Find a transform path between the specified coordinate frames.
    \param transformInfoList Stores the list of transforms to get from the fromCoordFrameName to toCoordFrameName
    \param skipCoordFrameName This is the name of a coordinate system that should be ignored (e.g., because it was checked previously already)
    \param silent Don't log an error if path cannot be found (it's normal while searching in the graph)
    \return returns PLUS_SUCCESS if a path can be found, PLUS_FAIL otherwise
  */ 
  PlusStatus FindPath(const char* fromCoordFrameName, const char* toCoordFrameName, TransformInfoListType &transformInfoList, const char* skipCoordFrameName=NULL, bool silent=false);

  CoordFrameToCoordFrameToTransformMapType CoordinateFrames;

private:
  vtkTransformRepository(const vtkTransformRepository&);
  void operator=(const vtkTransformRepository&);  
};

#endif

