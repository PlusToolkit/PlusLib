/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkPlusTransformRepository_h
#define __vtkPlusTransformRepository_h

// Local includes
#include "PlusConfigure.h"
#include "PlusCommon.h"
#include "vtkPlusCommonExport.h"

// VTK includes
#include <vtkObject.h>

// STL includes
#include <list>
#include <map>

class PlusTrackedFrame;
class vtkMatrix4x4;
class vtkPlusRecursiveCriticalSection;
class vtkTransform;

/*!
\class vtkPlusTransformRepository
\brief Combine multiple transforms to get a transform between arbitrary coordinate frames

The vtkPlusTransformRepository stores a number of transforms between coordinate frames and
it can multiply these transforms (or the inverse of these transforms) to
compute the transform between any two coordinate frames.

Example usage:
\code
  transformRepository->SetTransform("Probe", "Tracker", mxProbeToTracker);
  transformRepository->SetTransform("Image", "Probe", mxImageToProbe);
  ...
  vtkSmartPointer<vtkMatrix4x4> mxImageToTracker=vtkSmartPointer<vtkMatrix4x4>::New();
  vtkPlusTransformRepository::TransformStatus status=vtkPlusTransformRepository::TRANSFORM_INVALID;
  transformRepository->GetTransform("Image", "Tracker", mxImageToTracker, &status);
\endcode

The following coordinate frames are used commonly:
  \li Image: image frame coordinate system, origin is the bottom-left corner, unit is pixel
  \li Tool: coordinate system of the DRB attached to the probe, unit is mm
  \li Reference: coordinate system of the DRB attached to the reference body, unit is mm
  \li Tracker: coordinate system of the tracker, unit is mm
  \li World: world coordinate system, orientation is usually patient RAS, unit is mm

\ingroup PlusLibCommon
*/
class vtkPlusCommonExport vtkPlusTransformRepository : public vtkObject
{
public:
  static vtkPlusTransformRepository* New();
  vtkTypeMacro(vtkPlusTransformRepository, vtkObject);
  virtual void PrintSelf(ostream& os, vtkIndent indent) VTK_OVERRIDE;

  /*!
    Set a transform matrix between two coordinate frames. The method fails if the transform
    can be already constructed by concatenating/inverting already stored transforms. Changing an already
    set transform is allowed. The transform is computed even if one or more of the used transforms
    have non valid status.
  */
  virtual PlusStatus SetTransform(const PlusTransformName& aTransformName, vtkMatrix4x4* matrix, ToolStatus toolStatus = TOOL_OK);

  /*!
    Set all transform matrices between two coordinate frames stored in TrackedFrame. The method fails if any of the transforms
    can be already constructed by concatenating/inverting already stored transforms. Changing an already
    set transform is allowed. The transform is computed even if one or more of the used transforms
    have non valid statuses.
  */
  virtual PlusStatus SetTransforms(PlusTrackedFrame& trackedFrame);

  /*!
    Set the valid status of a transform matrix between two coordinate frames. A transform is normally valid,
    but temporarily it can be set to non valid (e.g., when a tracked tool gets out of view).
  */
  virtual PlusStatus SetTransformStatus(const PlusTransformName& aTransformName, ToolStatus toolStatus);

  /*!
    Set the persistent status of a transform matrix between two coordinate frames. A transform is non persistent by default.
    Transforms with status persistent will be written into config file on WriteConfiguration call.
  */
  virtual PlusStatus SetTransformPersistent(const PlusTransformName& aTransformName, bool isPersistent);

  /*! Set the computation error of the transform matrix between two coordinate frames. */
  virtual PlusStatus SetTransformError(const PlusTransformName& aTransformName, double aError);

  /*! Get the computation error of the transform matrix between two coordinate frames. */
  virtual PlusStatus GetTransformError(const PlusTransformName& aTransformName, double& aError, bool quiet = false);

  /*! Set the computation date of the transform matrix between two coordinate frames. */
  virtual PlusStatus SetTransformDate(const PlusTransformName& aTransformName, const std::string& aDate);

  /*! Get the computation date of the transform matrix between two coordinate frames. */
  virtual PlusStatus GetTransformDate(const PlusTransformName& aTransformName, std::string& aDate, bool quiet = false);

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
    Delete all transforms from XML data CoordinateDefinitions element then write all transform matrices that are persistent and non-persistent if boolean
    is true, only persistent if the boolean is false into the xml data CoordinateDefinitions element. The function will give a warning message
    in case of any non valid persistent transform.
  */
  virtual PlusStatus WriteConfigurationGeneric(vtkXMLDataElement* configRootElement, bool copyAllTransforms);

  /*!
    Get a transform matrix between two coordinate frames. The method fails if the transform
    cannot be already constructed by combining/inverting already stored transforms.
    \param aTransformName name of the transform to retrieve from the repository
    \param matrix the retrieved transform is copied into this matrix
    \param toolStatus if this parameter is not NULL then the transforms' status is returned at that memory address
  */
  virtual PlusStatus GetTransform(const PlusTransformName& aTransformName, vtkMatrix4x4* matrix, ToolStatus* toolStatus = NULL) const;

  /*!
    Get the valid status of a transform matrix between two coordinate frames.
    The status is typically invalid when a tracked tool is out of view.
  */
  virtual PlusStatus GetTransformValid(const PlusTransformName& aTransformName, bool& isValid);

  /*! Get the persistent status of a transform matrix between two coordinate frames. */
  virtual PlusStatus GetTransformPersistent(const PlusTransformName& aTransformName, bool& isPersistent);

  /*! Removes a transform from the repository */
  virtual PlusStatus DeleteTransform(const PlusTransformName& aTransformName);

  /*! Removes all the transforms from the repository */
  void Clear();

  /*! Checks if a transform exist */
  virtual PlusStatus IsExistingTransform(const PlusTransformName aTransformName, bool aSilent = true);

  /*! Copies the persistent and non-persistent contents if boolean is true, only persistent contents if fase */
  virtual PlusStatus DeepCopy(vtkPlusTransformRepository* sourceRepositoryName, bool copyAllTransforms);

protected:
  vtkPlusTransformRepository();
  ~vtkPlusTransformRepository();

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

    bool IsValid() { return m_ToolStatus == TOOL_OK; }

    /*! TransformInfo storing the transformation matrix between two coordinate frames */
    vtkTransform* m_Transform;
    /*! Describes the state of the tool status */
    ToolStatus m_ToolStatus;
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
  TransformInfo* GetOriginalTransform(const PlusTransformName& aTransformName) const;

  /*!
    Find a transform path between the specified coordinate frames.
    \param aTransformName name of the transform to find
    \param transformInfoList Stores the list of transforms to get from the fromCoordFrameName to toCoordFrameName
    \param skipCoordFrameName This is the name of a coordinate system that should be ignored (e.g., because it was checked previously already)
    \param silent Don't log an error if path cannot be found (it's normal while searching in branches of the graph)
    \return returns PLUS_SUCCESS if a path can be found, PLUS_FAIL otherwise
  */
  PlusStatus FindPath(const PlusTransformName& aTransformName, TransformInfoListType& transformInfoList, const char* skipCoordFrameName = NULL, bool silent = false) const;

  mutable CoordFrameToCoordFrameToTransformMapType CoordinateFrames;

  vtkPlusRecursiveCriticalSection* CriticalSection;

  TransformInfo TransformToSelf;

private:
  vtkPlusTransformRepository(const vtkPlusTransformRepository&);
  void operator=(const vtkPlusTransformRepository&);
};

#endif

