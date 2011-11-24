/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"
#include "vtkTransformRepository.h"
#include "vtkObjectFactory.h"
#include "vtkTransform.h"

vtkStandardNewMacro(vtkTransformRepository);

//----------------------------------------------------------------------------
vtkTransformRepository::TransformInfo::TransformInfo()
{
  m_Transform=vtkTransform::New();
  m_IsValid=true;
  m_IsComputed=false;
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
vtkTransformRepository::TransformInfo* vtkTransformRepository::GetInputTransform(const char* fromCoordFrameName, const char* toCoordFrameName)
{
  CoordFrameToTransformMapType& fromCoordFrame=this->CoordinateFrames[fromCoordFrameName];
  CoordFrameToTransformMapType& toCoordFrame=this->CoordinateFrames[toCoordFrameName];

  // Check if the transform already exist
  CoordFrameToTransformMapType::iterator fromToTransformInfoIt=fromCoordFrame.find(toCoordFrameName);
  if (fromToTransformInfoIt!=fromCoordFrame.end())
  {
    // transform is found
    return &(fromToTransformInfoIt->second);
  }
  // transform is not found
  return NULL;
}

//----------------------------------------------------------------------------
PlusStatus vtkTransformRepository::SetTransform(const char* fromCoordFrameName, const char* toCoordFrameName, vtkMatrix4x4* matrix, TransformStatus status /* = KEEP_CURRENT_STATUS*/ )
{
  if (fromCoordFrameName==NULL)
  {
    LOG_ERROR("From coordinate frame name is invalid");
    return PLUS_FAIL;
  }
  if (toCoordFrameName==NULL)
  {
    LOG_ERROR("To coordinate frame name is invalid");
    return PLUS_FAIL;
  }

  // Check if the transform already exist
  TransformInfo* fromToTransformInfo=GetInputTransform(fromCoordFrameName, toCoordFrameName);
  if (fromToTransformInfo!=NULL)
  {
    // Transform already exists
    if (fromToTransformInfo->m_IsComputed)
    {
      // The transform already exists and it is computed (not original), so reject the transformation update
      LOG_ERROR("The "<<fromCoordFrameName<<"To"<<toCoordFrameName<<" transform cannot be set, as the inverse ("
        <<toCoordFrameName<<"To"<<fromCoordFrameName<<") transform already exists");
      return PLUS_FAIL;
    }
    // This is an original transform that already exists, just update it
    // Update the matrix (the inverse matrix is automatically updated using vtkTransform pipeline)
    if (matrix!=NULL)
    {
      fromToTransformInfo->m_Transform->SetMatrix(matrix);
    }
    // Update the status
    if (status!=KEEP_CURRENT_STATUS)
    {
      // Set the status of the original transform
      fromToTransformInfo->m_IsValid=(status==TRANSFORM_VALID);
      // Set the same status for the computed inverse transform
      TransformInfo* toFromTransformInfo=GetInputTransform(toCoordFrameName, fromCoordFrameName);
      if (toFromTransformInfo==NULL)
      {
        LOG_ERROR("The computed "<<toCoordFrameName<<"To"<<fromCoordFrameName<<" transform is missing. Cannot set its status");
        return PLUS_FAIL;
      }
      toFromTransformInfo->m_IsValid=(status==TRANSFORM_VALID);      
    }
    return PLUS_SUCCESS;
  }
  // The transform does not exist yet, add it now

  TransformInfoListType transformInfoList;
  if (FindPath(fromCoordFrameName, toCoordFrameName, transformInfoList, NULL, true /*silent*/)==PLUS_SUCCESS)
  {
    // a path already exist between the two coordinate frames
    // adding a new transform between these would result in a circle
    LOG_ERROR("A transform path already exists between "<<fromCoordFrameName<<" and "<<toCoordFrameName);
    return PLUS_FAIL;
  }

  // Create the from->to transform
  CoordFrameToTransformMapType& fromCoordFrame=this->CoordinateFrames[fromCoordFrameName];
  fromCoordFrame[toCoordFrameName].m_IsComputed=false;
  if (matrix!=NULL)
  {
    fromCoordFrame[toCoordFrameName].m_Transform->SetMatrix(matrix);
  }
  if (status!=KEEP_CURRENT_STATUS)
  {
    fromCoordFrame[toCoordFrameName].m_IsValid=(status==TRANSFORM_VALID);
  }
  // Create the to->from inverse transform
  CoordFrameToTransformMapType& toCoordFrame=this->CoordinateFrames[toCoordFrameName];
  toCoordFrame[fromCoordFrameName].m_IsComputed=true;
  toCoordFrame[fromCoordFrameName].m_Transform->SetInput(fromCoordFrame[toCoordFrameName].m_Transform);
  toCoordFrame[fromCoordFrameName].m_Transform->Inverse();
  if (status!=KEEP_CURRENT_STATUS)
  {
    toCoordFrame[toCoordFrameName].m_IsValid=(status==TRANSFORM_VALID);
  }
  return PLUS_SUCCESS;
}
  
//----------------------------------------------------------------------------
PlusStatus vtkTransformRepository::SetTransformStatus(const char* fromCoordFrameName, const char* toCoordFrameName, TransformStatus status)
{
  return SetTransform(fromCoordFrameName, toCoordFrameName, NULL, status);
}
  
//----------------------------------------------------------------------------
PlusStatus vtkTransformRepository::GetTransform(const char* fromCoordFrameName, const char* toCoordFrameName, vtkMatrix4x4* matrix, TransformStatus* status /*=NULL*/ )
{
  if (fromCoordFrameName==NULL)
  {
    LOG_ERROR("From coordinate frame name is invalid");
    return PLUS_FAIL;
  }
  if (toCoordFrameName==NULL)
  {
    LOG_ERROR("To coordinate frame name is invalid");
    return PLUS_FAIL;
  }
  // Check if we can find the transform by combining the input transforms
  // To improve performance the already found paths could be stored in a map of transform name -> transformInfoList
  TransformInfoListType transformInfoList;
  if (FindPath(fromCoordFrameName, toCoordFrameName, transformInfoList)!=PLUS_SUCCESS)
  {
    LOG_ERROR("Not enough transforms defined to get the transformation from "<<fromCoordFrameName<<" to "<<toCoordFrameName);
    return PLUS_FAIL;
  }
  // Create transform chain and compute transform status
  vtkSmartPointer<vtkTransform> combinedTransform=vtkSmartPointer<vtkTransform>::New();
  bool combinedTransformValid=true;
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
  if (status!=NULL)
  {
    (*status)=combinedTransformValid?TRANSFORM_VALID:TRANSFORM_INVALID;
  }
  return PLUS_SUCCESS;
}
  
//----------------------------------------------------------------------------
PlusStatus vtkTransformRepository::GetTransformStatus(const char* fromCoordFrameName, const char* toCoordFrameName, TransformStatus &status)
{
  return GetTransform(fromCoordFrameName, toCoordFrameName, NULL, &status);
}

PlusStatus vtkTransformRepository::FindPath(const char* fromCoordFrameName, const char* toCoordFrameName, TransformInfoListType &transformInfoList, const char* skipCoordFrameName /*=NULL*/, bool silent /*=false*/)
{
  TransformInfo* fromToTransformInfo=GetInputTransform(fromCoordFrameName, toCoordFrameName);
  if (fromToTransformInfo!=NULL)
  {
    // found a transform
    transformInfoList.push_back(fromToTransformInfo);
    return PLUS_SUCCESS;
  }
  // not found, so try to find a path through all the connected coordinate frames
  CoordFrameToTransformMapType& fromCoordFrame=this->CoordinateFrames[fromCoordFrameName];
  for (CoordFrameToTransformMapType::iterator transformInfoIt=fromCoordFrame.begin(); transformInfoIt!=fromCoordFrame.end(); ++transformInfoIt)
  {
    if (skipCoordFrameName!=NULL && transformInfoIt->first.compare(skipCoordFrameName)==0)
    {
      // coordinate frame shall be ignored
      // (probably it would just go back to the previous coordinate frame where we come from)
      continue;
    }
    if (FindPath(transformInfoIt->first.c_str(), toCoordFrameName, transformInfoList, fromCoordFrameName, true /*silent*/)==PLUS_SUCCESS)
    {
      transformInfoList.push_back(&(transformInfoIt->second));
      return PLUS_SUCCESS;      
    }
  }
  if (!silent)
  {
    LOG_ERROR("Path not found from "<<fromCoordFrameName<<" to "<<toCoordFrameName);
  }
  return PLUS_FAIL;
}
