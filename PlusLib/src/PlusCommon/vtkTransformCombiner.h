/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkTransformCombiner_h
#define __vtkTransformCombiner_h

#include "PlusConfigure.h"

#include <map>
#include <list>

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

protected:
  vtkTransformCombiner();
  ~vtkTransformCombiner();  

  /*! An edge in the transform graph, representing a transform */ 
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
      that it is an original transform (set by a SetTransform() method call)
    */
    bool m_IsComputed;
  };

  typedef std::map<std::string, TransformInfo> TransformListType;

  /*! A vertex in the transform graph, representing a coordinate frame */ 
  class CoordinateFrameInfo
  {
  public:
    CoordinateFrameInfo()
    {
    }
    virtual ~CoordinateFrameInfo()
    {
    }
    /*! List of edges (transforms) that are connected to this vertex (coordinate frame) */
    TransformListType m_Transforms;
  };

  typedef std::map<std::string, CoordinateFrameInfo> CoordinateFrameListType;
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

  CoordinateFrameListType CoordinateFrames;

private:
  vtkTransformCombiner(const vtkTransformCombiner&);
  void operator=(const vtkTransformCombiner&);  
};

#endif

