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
class TrackedFrame; 

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

The following coordinate frames are used commonly: 
  Image: image frame coordinate system, origin is the bottom-left corner, unit is pixel
  Tool: coordinate system of the DRB attached to the probe, unit is mm
  Reference: coordinate system of the DRB attached to the reference body, unit is mm
  Tracker: coordinate system of the tracker, unit is mm
  World: world coordinate system, orientation is usually patient RAS, unit is mm

\ingroup PlusLibCommon
*/
class VTK_EXPORT vtkTransformRepository : public vtkObject
{
public:
  static vtkTransformRepository *New();
  vtkTypeMacro(vtkTransformRepository,vtkObject);
  void PrintSelf(ostream& os, vtkIndent indent);

  /*! 
    Set a transform matrix between two coordinate frames. The method fails if the transform
    can be already constructed by concatenating/inverting already stored transforms. Changing an already
    set transform is allowed. The transform is computed even if one or more of the used transforms
    have non valid status.
  */
  virtual PlusStatus SetTransform(PlusTransformName& aTransformName, vtkMatrix4x4* matrix, bool isValid = true);

  /*! 
    Set all transform matrices between two coordinate frames stored in TrackedFrame. The method fails if any of the transforms
    can be already constructed by concatenating/inverting already stored transforms. Changing an already
    set transform is allowed. The transform is computed even if one or more of the used transforms
    have non valid statuses.
  */
  virtual PlusStatus SetTransforms(TrackedFrame& trackedFrame);
  
  /*! 
    Set the valid status of a transform matrix between two coordinate frames. A transform is normally valid,
    but temporarily it can be set to non valid (e.g., when a tracked tool gets out of view). 
  */
  virtual PlusStatus SetTransformValid(PlusTransformName& aTransformName, bool isValid);
  
  /*! 
    Set the persistent status of a transform matrix between two coordinate frames. A transform is non persistent by default. 
    Transforms with status persistent will be written into config file on WriteConfiguration call. 
  */
  virtual PlusStatus SetTransformPersistent(PlusTransformName& aTransformName, bool isPersistent);

  /*! Set the computation error of the transform matrix between two coordinate frames. */
  virtual PlusStatus SetTransformError(PlusTransformName& aTransformName, double aError);
  
  /*! Get the computation error of the transform matrix between two coordinate frames. */
  virtual PlusStatus GetTransformError(PlusTransformName& aTransformName, double &aError);

  /*! Set the computation date of the transform matrix between two coordinate frames. */
  virtual PlusStatus SetTransformDate(PlusTransformName& aTransformName, const char* aDate);
  
  /*! Get the computation date of the transform matrix between two coordinate frames. */
  virtual PlusStatus GetTransformDate(PlusTransformName& aTransformName, std::string& aDate);

  /*! 
    Read all transformations from XML data CoordinateDefinitions element and add them to the transforms with 
    persistent and valid status. The method fails if any of the transforms
    can be already constructed by concatenating/inverting already stored transforms. Changing an already
    set transform is allowed.
  */
  virtual PlusStatus ReadConfiguration(vtkXMLDataElement* configRootElement); 

  /*! 
    Delete all transforms from XML data CoordinateDefinitions element then write all transform matrices with persistent status 
    into the xml data CoordinateDefinitions element. The function will give a warning message in case of any non valid persistent transform. 
  */
  virtual PlusStatus WriteConfiguration(vtkXMLDataElement* configRootElement); 
  
  /*! 
    Get a transform matrix between two coordinate frames. The method fails if the transform
    cannot be already constructed by combining/inverting already stored transforms.
    \param matrix the retrieved transform is copied into this matrix 
    \param status if this parameter is not NULL then the transform's status is returned at that memory address 
  */
  virtual PlusStatus GetTransform(PlusTransformName& aTransformName, vtkMatrix4x4* matrix, bool* isValid=NULL);

  /*! Get the valid status of a transform matrix between two coordinate frames. */
  virtual PlusStatus GetTransformValid(PlusTransformName& aTransformName, bool &isValid);

  /*! Get the persistent status of a transform matrix between two coordinate frames. */
  virtual PlusStatus GetTransformPersistent(PlusTransformName& aTransformName, bool &isPersistent);

  /*! Removes a transform from the repository */
  virtual PlusStatus DeleteTransform(PlusTransformName& aTransformName);

  /*! Removes all the transforms from the repository */
  void Clear();

protected:
  vtkTransformRepository();
  ~vtkTransformRepository();  

  /*!
    \struct TransformInfo
    \brief Stores a transformation matrix and some additional information (valid or not, computed or not)
    \ingroup PlusLibCommon
  */
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
    /*!
      If the value is true then it means that the transform is persistent
      and won't change, so we can save it to config file as a coordinate definition; 
    */
    bool m_IsPersistent;
    /*! Persistent transform creation date, saved to configuration file */
    std::string m_Date; 
    /*! Persistent transform calculation error (e.g calibration error) */
    double m_Error; 
  };

  /*! For each "to" coordinate frame name (first) stores a transform (second)*/
  typedef std::map<std::string, TransformInfo> CoordFrameToTransformMapType;
  /*! For each "from" coordinate frame (first) stores an array of transforms (second) */
  typedef std::map<std::string, CoordFrameToTransformMapType> CoordFrameToCoordFrameToTransformMapType;

  /*! List of transforms */
  typedef std::list<TransformInfo*> TransformInfoListType;

  /*! Get a user-defined original input transform (or its inverse). Does not combine user-defined input transforms. */ 
  TransformInfo* GetOriginalTransform(PlusTransformName& aTransformName);

  /*! 
    Find a transform path between the specified coordinate frames.
    \param transformInfoList Stores the list of transforms to get from the fromCoordFrameName to toCoordFrameName
    \param skipCoordFrameName This is the name of a coordinate system that should be ignored (e.g., because it was checked previously already)
    \param silent Don't log an error if path cannot be found (it's normal while searching in the graph)
    \return returns PLUS_SUCCESS if a path can be found, PLUS_FAIL otherwise
  */ 
  PlusStatus FindPath(PlusTransformName& aTransformName, TransformInfoListType &transformInfoList, const char* skipCoordFrameName=NULL, bool silent=false);

  CoordFrameToCoordFrameToTransformMapType CoordinateFrames;

private:
  vtkTransformRepository(const vtkTransformRepository&);
  void operator=(const vtkTransformRepository&);  
};

#endif

