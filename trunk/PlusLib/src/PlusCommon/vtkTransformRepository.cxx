/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"
#include "vtkTransformRepository.h"
#include "vtkObjectFactory.h"
#include "vtkTransform.h"
#include "vtkTrackedFrameList.h"

vtkStandardNewMacro(vtkTransformRepository);

//----------------------------------------------------------------------------
vtkTransformRepository::TransformInfo::TransformInfo()
{
  m_Transform=vtkTransform::New();
  m_IsValid=true;
  m_IsComputed=false;
  m_IsPersistent=false; 
  m_Error=0.0; 
}

//----------------------------------------------------------------------------
vtkTransformRepository::TransformInfo::~TransformInfo()
{
  if (m_Transform!=NULL)
  {
    m_Transform->Delete();
    m_Transform=NULL;
  }
}

//----------------------------------------------------------------------------
vtkTransformRepository::TransformInfo::TransformInfo(const TransformInfo& obj)
{
  m_Transform=obj.m_Transform;
  if (m_Transform!=NULL)
  {
    m_Transform->Register(NULL);
  }
  m_IsComputed=obj.m_IsComputed;
  m_IsValid=obj.m_IsValid;
  m_IsPersistent=obj.m_IsPersistent; 
  m_Date=obj.m_Date; 
  m_Error=obj.m_Error; 

}
//----------------------------------------------------------------------------
vtkTransformRepository::TransformInfo& vtkTransformRepository::TransformInfo::operator=(const TransformInfo& obj) 
{
  if (m_Transform!=NULL)
  {
    m_Transform->Delete();
    m_Transform=NULL;
  }
  m_Transform=obj.m_Transform;
  if (m_Transform!=NULL)
  {
    m_Transform->Register(NULL);
  }
  m_IsComputed=obj.m_IsComputed;
  m_IsValid=obj.m_IsValid;
  m_IsPersistent=obj.m_IsPersistent; 
  m_Date=obj.m_Date; 
  m_Error=obj.m_Error;
  return *this;
}

//----------------------------------------------------------------------------
vtkTransformRepository::vtkTransformRepository()
{
}

//----------------------------------------------------------------------------
vtkTransformRepository::~vtkTransformRepository()
{
}

//----------------------------------------------------------------------------
void vtkTransformRepository::PrintSelf(ostream& os, vtkIndent indent)
{
  vtkObject::PrintSelf(os,indent);

  for (CoordFrameToCoordFrameToTransformMapType::iterator coordFrame=this->CoordinateFrames.begin(); coordFrame!=this->CoordinateFrames.end(); ++coordFrame)
  {
    os << indent << coordFrame->first << " coordinate frame transforms:\n";
    for (CoordFrameToTransformMapType::iterator transformInfo=coordFrame->second.begin(); transformInfo!=coordFrame->second.end(); ++transformInfo)
    {
      os << indent << "  To " << transformInfo->first << ": " 
        << (transformInfo->second.m_IsValid?"valid":"invalid") << ", " 
        << (transformInfo->second.m_IsPersistent?"persistent":"non-persistent") << ", " 
        << (transformInfo->second.m_IsComputed?"computed":"original") << "\n";
      if (transformInfo->second.m_Transform!=NULL && transformInfo->second.m_Transform->GetMatrix()!=NULL)
      {
        vtkMatrix4x4* transformMx=transformInfo->second.m_Transform->GetMatrix();
        os << indent << "     " << transformMx->Element[0][0] << " " << transformMx->Element[0][1] << " " << transformMx->Element[0][2] << " " << transformMx->Element[0][3] << " "<< "\n";
        os << indent << "     " << transformMx->Element[1][0] << " " << transformMx->Element[1][1] << " " << transformMx->Element[1][2] << " " << transformMx->Element[1][3] << " "<< "\n";
        os << indent << "     " << transformMx->Element[2][0] << " " << transformMx->Element[2][1] << " " << transformMx->Element[2][2] << " " << transformMx->Element[2][3] << " "<< "\n";
        os << indent << "     " << transformMx->Element[3][0] << " " << transformMx->Element[3][1] << " " << transformMx->Element[3][2] << " " << transformMx->Element[3][3] << " "<< "\n";
      }
      else
      {
        os << indent << "     No transform is available\n";
      }      
    }
  }
}

//----------------------------------------------------------------------------
vtkTransformRepository::TransformInfo* vtkTransformRepository::GetOriginalTransform(PlusTransformName& aTransformName)
{
  CoordFrameToTransformMapType& fromCoordFrame=this->CoordinateFrames[aTransformName.From()];
  CoordFrameToTransformMapType& toCoordFrame=this->CoordinateFrames[aTransformName.To()];

  // Check if the transform already exist
  CoordFrameToTransformMapType::iterator fromToTransformInfoIt=fromCoordFrame.find(aTransformName.To());
  if (fromToTransformInfoIt!=fromCoordFrame.end())
  {
    // transform is found
    return &(fromToTransformInfoIt->second);
  }
  // transform is not found
  return NULL;
}

//----------------------------------------------------------------------------
PlusStatus vtkTransformRepository::SetTransforms(TrackedFrame& trackedFrame)
{
  LOG_WARNING("Not yet implemented!"); 
  return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
PlusStatus vtkTransformRepository::SetTransform(PlusTransformName& aTransformName, vtkMatrix4x4* matrix, bool isValid/*=true*/ )
{
  if ( ! aTransformName.IsValid() )
  {
    LOG_ERROR("Transform name is invalid");
    return PLUS_FAIL;
  }

  // Check if the transform already exist
  TransformInfo* fromToTransformInfo=GetOriginalTransform(aTransformName);
  if (fromToTransformInfo!=NULL)
  {
    // Transform already exists
    if (fromToTransformInfo->m_IsComputed)
    {
      // The transform already exists and it is computed (not original), so reject the transformation update
      LOG_ERROR("The "<<aTransformName.From()<<"To"<<aTransformName.To()<<" transform cannot be set, as the inverse ("
        <<aTransformName.To()<<"To"<<aTransformName.From()<<") transform already exists");
      return PLUS_FAIL;
    }
    // This is an original transform that already exists, just update it
    // Update the matrix (the inverse matrix is automatically updated using vtkTransform pipeline)
    if (matrix!=NULL)
    {
      fromToTransformInfo->m_Transform->SetMatrix(matrix);
    }
    // Set the status of the original transform
    fromToTransformInfo->m_IsValid=isValid;

    // Set the same status for the computed inverse transform
    TransformInfo* toFromTransformInfo=GetOriginalTransform(aTransformName);
    if (toFromTransformInfo==NULL)
    {
      LOG_ERROR("The computed "<<aTransformName.To()<<"To"<<aTransformName.From()<<" transform is missing. Cannot set its status");
      return PLUS_FAIL;
    }
    toFromTransformInfo->m_IsValid=isValid;
    return PLUS_SUCCESS;
  }
  // The transform does not exist yet, add it now

  TransformInfoListType transformInfoList;
  if (FindPath(aTransformName, transformInfoList, NULL, true /*silent*/)==PLUS_SUCCESS)
  {
    // a path already exist between the two coordinate frames
    // adding a new transform between these would result in a circle
    LOG_ERROR("A transform path already exists between "<<aTransformName.From()<<" and "<<aTransformName.To());
    return PLUS_FAIL;
  }

  // Create the from->to transform
  CoordFrameToTransformMapType& fromCoordFrame=this->CoordinateFrames[aTransformName.From()];
  fromCoordFrame[aTransformName.To()].m_IsComputed=false;
  if (matrix!=NULL)
  {
    fromCoordFrame[aTransformName.To()].m_Transform->SetMatrix(matrix);
  }

  fromCoordFrame[aTransformName.To()].m_IsValid=isValid; 

  // Create the to->from inverse transform
  CoordFrameToTransformMapType& toCoordFrame=this->CoordinateFrames[aTransformName.To()];
  toCoordFrame[aTransformName.From()].m_IsComputed=true;
  toCoordFrame[aTransformName.From()].m_Transform->SetInput(fromCoordFrame[aTransformName.To()].m_Transform);
  toCoordFrame[aTransformName.From()].m_Transform->Inverse();
  toCoordFrame[aTransformName.From()].m_IsValid=isValid;
  return PLUS_SUCCESS;
}
  
//----------------------------------------------------------------------------
PlusStatus vtkTransformRepository::SetTransformValid(PlusTransformName& aTransformName, bool isValid)
{
  return SetTransform(aTransformName, NULL, isValid);
}
  
//----------------------------------------------------------------------------
PlusStatus vtkTransformRepository::GetTransform(PlusTransformName& aTransformName, vtkMatrix4x4* matrix, bool* isValid /*=NULL*/ )
{
  if ( !aTransformName.IsValid() )
  {
    LOG_ERROR("Transform name is invalid");
    return PLUS_FAIL;
  }

  // Check if we can find the transform by combining the input transforms
  // To improve performance the already found paths could be stored in a map of transform name -> transformInfoList
  TransformInfoListType transformInfoList;
  if (FindPath(aTransformName, transformInfoList)!=PLUS_SUCCESS)
  {
    // the transform cannot be computed, error has been already logged by FindPath
    return PLUS_FAIL;
  }
  // Create transform chain and compute transform status
  vtkSmartPointer<vtkTransform> combinedTransform=vtkSmartPointer<vtkTransform>::New();
  bool combinedTransformValid=true;
  bool combinedTransformPersistent=true;
  for (TransformInfoListType::iterator transformInfo=transformInfoList.begin(); transformInfo!=transformInfoList.end(); ++transformInfo)
  {
    combinedTransform->Concatenate((*transformInfo)->m_Transform);
    if (!(*transformInfo)->m_IsValid)
    {
      combinedTransformValid=false;
    }
  }
  // Save the results
  if (matrix!=NULL)
  {
    matrix->DeepCopy(combinedTransform->GetMatrix());
  }

  if (isValid!=NULL)
  {
    (*isValid) = combinedTransformValid; 
  }

  return PLUS_SUCCESS;
}
  
//----------------------------------------------------------------------------
PlusStatus vtkTransformRepository::GetTransformValid(PlusTransformName& aTransformName, bool &isValid)
{
  return GetTransform(aTransformName, NULL, &isValid);
}

//----------------------------------------------------------------------------
PlusStatus vtkTransformRepository::SetTransformPersistent(PlusTransformName& aTransformName, bool isPersistent)
{
  TransformInfo* fromToTransformInfo=GetOriginalTransform(aTransformName);
  if (fromToTransformInfo!=NULL)
  {
    fromToTransformInfo->m_IsPersistent = isPersistent; 
    return PLUS_SUCCESS; 
  }
  LOG_ERROR("The original "<<aTransformName.From()<<"To"<<aTransformName.To()<<" transform is missing. Cannot set its persistent status");
  return PLUS_FAIL;
}

//----------------------------------------------------------------------------
PlusStatus vtkTransformRepository::GetTransformPersistent(PlusTransformName& aTransformName, bool &isPersistent)
{
  TransformInfo* fromToTransformInfo=GetOriginalTransform(aTransformName);
  if (fromToTransformInfo!=NULL)
  {
    isPersistent = fromToTransformInfo->m_IsPersistent; 
    return PLUS_SUCCESS; 
  }
  LOG_ERROR("The original "<<aTransformName.From()<<"To"<<aTransformName.To()<<" transform is missing. Cannot get its persistent status");
  return PLUS_FAIL;
}

//----------------------------------------------------------------------------
PlusStatus vtkTransformRepository::FindPath(PlusTransformName& aTransformName, TransformInfoListType &transformInfoList, const char* skipCoordFrameName /*=NULL*/, bool silent /*=false*/)
{
  TransformInfo* fromToTransformInfo=GetOriginalTransform(aTransformName);
  if (fromToTransformInfo!=NULL)
  {
    // found a transform
    transformInfoList.push_back(fromToTransformInfo);
    return PLUS_SUCCESS;
  }
  // not found, so try to find a path through all the connected coordinate frames
  CoordFrameToTransformMapType& fromCoordFrame=this->CoordinateFrames[aTransformName.From()];
  for (CoordFrameToTransformMapType::iterator transformInfoIt=fromCoordFrame.begin(); transformInfoIt!=fromCoordFrame.end(); ++transformInfoIt)
  {
    if (skipCoordFrameName!=NULL && transformInfoIt->first.compare(skipCoordFrameName)==0)
    {
      // coordinate frame shall be ignored
      // (probably it would just go back to the previous coordinate frame where we come from)
      continue;
    }
    PlusTransformName newTransformName(transformInfoIt->first, aTransformName.To()); 
    if (FindPath(newTransformName, transformInfoList, aTransformName.From().c_str(), true /*silent*/)==PLUS_SUCCESS)
    {
      transformInfoList.push_back(&(transformInfoIt->second));
      return PLUS_SUCCESS;      
    }
  }
  if (!silent)
  {
    LOG_ERROR("Path not found from "<<aTransformName.From()<<" to "<<aTransformName.To());
  }
  return PLUS_FAIL;
}

//----------------------------------------------------------------------------
PlusStatus vtkTransformRepository::DeleteTransform(PlusTransformName& aTransformName)
{
  CoordFrameToTransformMapType& fromCoordFrame=this->CoordinateFrames[aTransformName.From()];
  CoordFrameToTransformMapType::iterator fromToTransformInfoIt=fromCoordFrame.find(aTransformName.To());
  
  if (fromToTransformInfoIt!=fromCoordFrame.end())
  {
    // from->to transform is found
    if (fromToTransformInfoIt->second.m_IsComputed)
    {
      // this is not an original transform (has not been set by the user)
      LOG_ERROR("The "<<aTransformName.From()<<" to "<<aTransformName.To()
        <<" transform cannot be deleted, only the inverse of the transform has been set in the repository ("
        <<aTransformName.From()<<" to "<<aTransformName.To()<<")");
      return PLUS_FAIL;
    }
    fromCoordFrame.erase(fromToTransformInfoIt);
  }
  else
  {
    LOG_ERROR("Delete transform failed: could not find the "<<aTransformName.From()<<" to "<<aTransformName.To()<<" transform");
    // don't return yet, try to delete the inverse
    return PLUS_FAIL;
  }
  
  CoordFrameToTransformMapType& toCoordFrame=this->CoordinateFrames[aTransformName.To()];
  CoordFrameToTransformMapType::iterator toFromTransformInfoIt=toCoordFrame.find(aTransformName.From());
  if (toFromTransformInfoIt!=toCoordFrame.end())
  {
    // to->from transform is found
    toCoordFrame.erase(toFromTransformInfoIt);
  }
  else
  {
    LOG_ERROR("Delete transform failed: could not find the "<<aTransformName.To()<<" to "<<aTransformName.From()<<" transform");
    return PLUS_FAIL;
  }
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
void vtkTransformRepository::Clear()
{
  this->CoordinateFrames.clear();
}

//----------------------------------------------------------------------------
PlusStatus vtkTransformRepository::ReadConfiguration(vtkXMLDataElement* configRootElement)
{
  LOG_WARNING("Not yet implemented!"); 
  return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
PlusStatus vtkTransformRepository::WriteConfiguration(vtkXMLDataElement* configRootElement)
{
  LOG_WARNING("Not yet implemented!"); 
  return PLUS_SUCCESS; 
}

