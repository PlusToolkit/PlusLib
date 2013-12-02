/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"
#include "TrackedFrame.h"
#include "vtkObjectFactory.h"
#include "vtkRecursiveCriticalSection.h"
#include "vtkTransform.h"
#include "vtkTransformRepository.h"
#include "vtksys/SystemTools.hxx" 

//----------------------------------------------------------------------------

vtkStandardNewMacro(vtkTransformRepository);

//----------------------------------------------------------------------------
vtkTransformRepository::TransformInfo::TransformInfo()
: m_Transform(vtkTransform::New())
, m_IsValid(true)
, m_IsComputed(false)
, m_IsPersistent(false)
, m_Error(-1.0)
{

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
  m_Transform = obj.m_Transform;
  if (m_Transform != NULL)
  {
    m_Transform->Register(NULL);
  }
  m_IsComputed = obj.m_IsComputed;
  m_IsValid = obj.m_IsValid;
  m_IsPersistent = obj.m_IsPersistent; 
  m_Date = obj.m_Date; 
  m_Error = obj.m_Error; 

}
//----------------------------------------------------------------------------
vtkTransformRepository::TransformInfo& vtkTransformRepository::TransformInfo::operator=(const TransformInfo& obj) 
{
  if (m_Transform != NULL)
  {
    m_Transform->Delete();
    m_Transform = NULL;
  }
  m_Transform = obj.m_Transform;
  if (m_Transform != NULL)
  {
    m_Transform->Register(NULL);
  }
  m_IsComputed = obj.m_IsComputed;
  m_IsValid = obj.m_IsValid;
  m_IsPersistent = obj.m_IsPersistent; 
  m_Date = obj.m_Date; 
  m_Error = obj.m_Error;
  return *this;
}

//----------------------------------------------------------------------------
vtkTransformRepository::vtkTransformRepository()
: CriticalSection(vtkRecursiveCriticalSection::New())
{

}

//----------------------------------------------------------------------------
vtkTransformRepository::~vtkTransformRepository()
{
  this->CriticalSection->Delete();
  this->CriticalSection = NULL;
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
vtkTransformRepository::TransformInfo* vtkTransformRepository::GetOriginalTransform(const PlusTransformName& aTransformName)
{
  CoordFrameToTransformMapType& fromCoordFrame=this->CoordinateFrames[aTransformName.From()];

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
  std::vector<PlusTransformName> transformNames; 
  trackedFrame.GetCustomFrameTransformNameList(transformNames); 

  int numberOfErrors(0); 

  for ( std::vector<PlusTransformName>::iterator it = transformNames.begin(); it != transformNames.end(); ++it)
  {
    std::string trName; 
    it->GetTransformName(trName); 

    vtkSmartPointer<vtkMatrix4x4> matrix = vtkSmartPointer<vtkMatrix4x4>::New(); 
    if ( trackedFrame.GetCustomFrameTransform( *it, matrix) != PLUS_SUCCESS )
    {
      LOG_ERROR("Failed to get custom frame transform from tracked frame: " << trName ); 
      numberOfErrors++; 
      continue; 
    }

    TrackedFrameFieldStatus status = FIELD_INVALID; 
    if ( trackedFrame.GetCustomFrameTransformStatus( *it, status) != PLUS_SUCCESS )
    {
      LOG_ERROR("Failed to get custom frame transform from tracked frame: " << trName ); 
      numberOfErrors++; 
      continue; 
    }

    if ( this->SetTransform(*it, matrix, status == FIELD_OK ) != PLUS_SUCCESS )
    {
      LOG_ERROR("Failed to set transform to repository: " << trName ); 
      numberOfErrors++; 
      continue; 
    }
  }
  
  return (numberOfErrors == 0 ? PLUS_SUCCESS : PLUS_FAIL ); 
}

//----------------------------------------------------------------------------
PlusStatus vtkTransformRepository::SetTransform(const PlusTransformName& aTransformName, vtkMatrix4x4* matrix, bool isValid/*=true*/ )
{
  if ( !aTransformName.IsValid() )
  {
    LOG_ERROR("Transform name is invalid");
    return PLUS_FAIL;
  }

  PlusLockGuard<vtkRecursiveCriticalSection> accessGuard(this->CriticalSection);

  // Check if the transform already exist
  TransformInfo* fromToTransformInfo = GetOriginalTransform(aTransformName);
  if (fromToTransformInfo != NULL)
  {
    // Transform already exists
    if ( fromToTransformInfo->m_IsComputed )
    {
      // The transform already exists and it is computed (not original), so reject the transformation update
      LOG_ERROR("The " << aTransformName.From() << "To" << aTransformName.To() << 
        " transform cannot be set, as the inverse (" << aTransformName.To() << "To" << 
        aTransformName.From() << ") transform already exists");
      return PLUS_FAIL;
    }

    // This is an original transform that already exists, just update it
    // Update the matrix (the inverse matrix is automatically updated using vtkTransform pipeline)
    if (matrix != NULL)
    {
      fromToTransformInfo->m_Transform->SetMatrix(matrix);
    }
    // Set the status of the original transform
    fromToTransformInfo->m_IsValid = isValid;

    // Set the same status for the computed inverse transform
    PlusTransformName toFromTransformName(aTransformName.To(), aTransformName.From());
    TransformInfo* toFromTransformInfo = GetOriginalTransform(toFromTransformName);
    if (toFromTransformInfo==NULL)
    {
      LOG_ERROR("The computed " << aTransformName.To() <<"To" << aTransformName.From()
        << " transform is missing. Cannot set its status");
      return PLUS_FAIL;
    }
    toFromTransformInfo->m_IsValid = isValid;
    return PLUS_SUCCESS;
  }
  // The transform does not exist yet, add it now

  TransformInfoListType transformInfoList;
  if (FindPath(aTransformName, transformInfoList, NULL, true /*silent*/)==PLUS_SUCCESS)
  {
    // a path already exist between the two coordinate frames
    // adding a new transform between these would result in a circle
    LOG_ERROR("A transform path already exists between " << aTransformName.From() <<
      " and " << aTransformName.To());
    return PLUS_FAIL;
  }

  // Create the from->to transform
  CoordFrameToTransformMapType& fromCoordFrame = this->CoordinateFrames[aTransformName.From()];
  fromCoordFrame[aTransformName.To()].m_IsComputed = false;
  if (matrix != NULL)
  {
    fromCoordFrame[aTransformName.To()].m_Transform->SetMatrix(matrix);
  }

  fromCoordFrame[aTransformName.To()].m_IsValid = isValid; 

  // Create the to->from inverse transform
  CoordFrameToTransformMapType& toCoordFrame = this->CoordinateFrames[aTransformName.To()];
  toCoordFrame[aTransformName.From()].m_IsComputed = true;
  toCoordFrame[aTransformName.From()].m_Transform->SetInput(fromCoordFrame[aTransformName.To()].m_Transform);
  toCoordFrame[aTransformName.From()].m_Transform->Inverse();
  toCoordFrame[aTransformName.From()].m_IsValid = isValid;
  return PLUS_SUCCESS;
}
  
//----------------------------------------------------------------------------
PlusStatus vtkTransformRepository::SetTransformValid(const PlusTransformName& aTransformName, bool isValid)
{
  return SetTransform(aTransformName, NULL, isValid);
}
  
//----------------------------------------------------------------------------
PlusStatus vtkTransformRepository::GetTransform(const PlusTransformName& aTransformName, vtkMatrix4x4* matrix, bool* isValid /*=NULL*/ )
{
  if ( !aTransformName.IsValid() )
  {
    LOG_ERROR("Transform name is invalid");
    return PLUS_FAIL;
  }

  PlusLockGuard<vtkRecursiveCriticalSection> accessGuard(this->CriticalSection);

  // Check if we can find the transform by combining the input transforms
  // To improve performance the already found paths could be stored in a map of transform name -> transformInfoList
  TransformInfoListType transformInfoList;
  if (FindPath(aTransformName, transformInfoList) != PLUS_SUCCESS)
  {
    // the transform cannot be computed, error has been already logged by FindPath
    return PLUS_FAIL;
  }

  // Create transform chain and compute transform status
  vtkSmartPointer<vtkTransform> combinedTransform = vtkSmartPointer<vtkTransform>::New();
  bool combinedTransformValid(true);
  for (TransformInfoListType::iterator transformInfo=transformInfoList.begin(); transformInfo!=transformInfoList.end(); ++transformInfo)
  {
    combinedTransform->Concatenate((*transformInfo)->m_Transform);
    if (!(*transformInfo)->m_IsValid)
    {
      combinedTransformValid = false;
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
PlusStatus vtkTransformRepository::GetTransformValid(const PlusTransformName& aTransformName, bool &isValid)
{
  return GetTransform(aTransformName, NULL, &isValid);
}

//----------------------------------------------------------------------------
PlusStatus vtkTransformRepository::SetTransformPersistent(const PlusTransformName& aTransformName, bool isPersistent)
{
  PlusLockGuard<vtkRecursiveCriticalSection> accessGuard(this->CriticalSection);

  TransformInfo* fromToTransformInfo = GetOriginalTransform(aTransformName);
  if (fromToTransformInfo != NULL)
  {
    fromToTransformInfo->m_IsPersistent = isPersistent; 
    return PLUS_SUCCESS; 
  }
  LOG_ERROR("The original " << aTransformName.From() << "To" << aTransformName.To() <<
    " transform is missing. Cannot set its persistent status");
  return PLUS_FAIL;
}

//----------------------------------------------------------------------------
PlusStatus vtkTransformRepository::GetTransformPersistent(const PlusTransformName& aTransformName, bool &isPersistent)
{
  PlusLockGuard<vtkRecursiveCriticalSection> accessGuard(this->CriticalSection);

  TransformInfo* fromToTransformInfo = GetOriginalTransform(aTransformName);
  if (fromToTransformInfo != NULL)
  {
    isPersistent = fromToTransformInfo->m_IsPersistent; 
    return PLUS_SUCCESS; 
  }
  LOG_ERROR("The original " << aTransformName.From() << "To" << aTransformName.To() << 
    " transform is missing. Cannot get its persistent status");
  return PLUS_FAIL;
}

//----------------------------------------------------------------------------
PlusStatus vtkTransformRepository::SetTransformError(const PlusTransformName& aTransformName, double aError)
{
  PlusLockGuard<vtkRecursiveCriticalSection> accessGuard(this->CriticalSection);

  TransformInfo* fromToTransformInfo = GetOriginalTransform(aTransformName);
  if (fromToTransformInfo!=NULL)
  {
    fromToTransformInfo->m_Error = aError; 
    return PLUS_SUCCESS; 
  }
  LOG_ERROR("The original "<<aTransformName.From()<<"To"<<aTransformName.To()<<" transform is missing. Cannot set computation error value.");
  return PLUS_FAIL;
}

//----------------------------------------------------------------------------
PlusStatus vtkTransformRepository::GetTransformError(const PlusTransformName& aTransformName, double &aError)
{
  PlusLockGuard<vtkRecursiveCriticalSection> accessGuard(this->CriticalSection);
  TransformInfo* fromToTransformInfo = GetOriginalTransform(aTransformName);
  if (fromToTransformInfo != NULL)
  {
    aError = fromToTransformInfo->m_Error; 
    return PLUS_SUCCESS; 
  }
  LOG_ERROR("The original "<<aTransformName.From()<<"To"<<aTransformName.To()<<" transform is missing. Cannot get computation error value.");
  return PLUS_FAIL;
}

//----------------------------------------------------------------------------
PlusStatus vtkTransformRepository::SetTransformDate(const PlusTransformName& aTransformName, const char* aDate)
{
  if ( aDate == NULL )
  {
    LOG_ERROR("Cannot set computation date if it's NULL.");
    return PLUS_FAIL;
  }

  PlusLockGuard<vtkRecursiveCriticalSection> accessGuard(this->CriticalSection);

  TransformInfo* fromToTransformInfo = GetOriginalTransform(aTransformName);
  if (fromToTransformInfo != NULL)
  {
    fromToTransformInfo->m_Date = aDate; 
    return PLUS_SUCCESS; 
  }
  LOG_ERROR("The original "<<aTransformName.From()<<"To"<<aTransformName.To()<<" transform is missing. Cannot set computation date.");
  return PLUS_FAIL;
}

//----------------------------------------------------------------------------
PlusStatus vtkTransformRepository::GetTransformDate(const PlusTransformName& aTransformName, std::string& aDate)
{
  PlusLockGuard<vtkRecursiveCriticalSection> accessGuard(this->CriticalSection);

  TransformInfo* fromToTransformInfo=GetOriginalTransform(aTransformName);
  if (fromToTransformInfo != NULL)
  {
    aDate = fromToTransformInfo->m_Date; 
    return PLUS_SUCCESS; 
  }
  LOG_ERROR("The original "<<aTransformName.From()<<"To"<<aTransformName.To()<<" transform is missing. Cannot get computation date.");
  return PLUS_FAIL;
}

//----------------------------------------------------------------------------
PlusStatus vtkTransformRepository::FindPath(const PlusTransformName& aTransformName, TransformInfoListType &transformInfoList, const char* skipCoordFrameName /*=NULL*/, bool silent /*=false*/)
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
    // Print available transforms into a string, for troubleshooting information
    std::ostringstream osAvailableTransforms; 
    bool firstPrintedTransform=true;
    for (CoordFrameToCoordFrameToTransformMapType::iterator coordFrame=this->CoordinateFrames.begin(); coordFrame!=this->CoordinateFrames.end(); ++coordFrame)
    {        
      for (CoordFrameToTransformMapType::iterator transformInfo=coordFrame->second.begin(); transformInfo!=coordFrame->second.end(); ++transformInfo)
      {
        if (transformInfo->second.m_IsComputed)
        {
          // only print original transforms
          continue;
        }
        // don't print separator before the first transform
        if (firstPrintedTransform)
        {
          firstPrintedTransform=false;
        }
        else
        {
          osAvailableTransforms << ", ";
        }
        osAvailableTransforms << coordFrame->first << "To" << transformInfo->first << " (" 
          << (transformInfo->second.m_IsValid?"valid":"invalid") << ", " 
          << (transformInfo->second.m_IsPersistent?"persistent":"non-persistent") << ")";
      }
    }
    LOG_ERROR("Transform path not found from "<<aTransformName.From()<<" to "<<aTransformName.To()<<" coordinate system."
      <<" Available transforms in the repository (including the inverse of these transforms): "<<osAvailableTransforms.str());
  }
  return PLUS_FAIL;
}

//----------------------------------------------------------------------------
PlusStatus vtkTransformRepository::IsExistingTransform(PlusTransformName aTransformName, bool aSilent/* = true*/)
{
  PlusLockGuard<vtkRecursiveCriticalSection> accessGuard(this->CriticalSection);
  TransformInfoListType transformInfoList;
  return FindPath(aTransformName, transformInfoList, NULL, aSilent);
}

//----------------------------------------------------------------------------
PlusStatus vtkTransformRepository::DeleteTransform(const PlusTransformName& aTransformName)
{
  PlusLockGuard<vtkRecursiveCriticalSection> accessGuard(this->CriticalSection);

  CoordFrameToTransformMapType& fromCoordFrame = this->CoordinateFrames[aTransformName.From()];
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
  if ( configRootElement == NULL )
  {
    LOG_ERROR("Failed read transform from CoordinateDefinitions - config root element is NULL"); 
    return PLUS_FAIL; 
  }

  vtkXMLDataElement* coordinateDefinitions = configRootElement->FindNestedElementWithName("CoordinateDefinitions");
  if ( coordinateDefinitions == NULL )
  {
    LOG_DEBUG("Couldn't read transform from CoordinateDefinitions - CoordinateDefinitions element not found"); 
    return PLUS_SUCCESS;  
  }

  PlusLockGuard<vtkRecursiveCriticalSection> accessGuard(this->CriticalSection);

  // Clear the transforms
  this->Clear(); 

  int numberOfErrors(0); 
  for ( int nestedElementIndex = 0; nestedElementIndex < coordinateDefinitions->GetNumberOfNestedElements(); ++nestedElementIndex )
  {
    vtkXMLDataElement* nestedElement = coordinateDefinitions->GetNestedElement(nestedElementIndex); 
    if ( STRCASECMP(nestedElement->GetName(), "Transform" ) != 0 )
    {
      // Not a transform element, skip it
      continue; 
    }

    const char* fromAttribute = nestedElement->GetAttribute("From"); 
    const char* toAttribute = nestedElement->GetAttribute("To"); 

    if ( !fromAttribute || !toAttribute )
    {
      LOG_ERROR("Failed to read transform of CoordinateDefinitions (nested element index: " << nestedElementIndex << ") - check 'From' and 'To' attributes in the configuration file!"); 
      numberOfErrors++; 
      continue; 
    }

    PlusTransformName transformName(fromAttribute, toAttribute); 
    if ( !transformName.IsValid() )
    {
      LOG_ERROR("Invalid transform name (From: '" <<  fromAttribute << "'  To: '" << toAttribute << "')"); 
      numberOfErrors++; 
      continue;  
    }

    vtkSmartPointer<vtkMatrix4x4> transformMatrix = vtkSmartPointer<vtkMatrix4x4>::New(); 
    double vectorMatrix[16]={0}; 
    if ( nestedElement->GetVectorAttribute("Matrix", 16, vectorMatrix) )
    {
      transformMatrix->DeepCopy(vectorMatrix); 
    }
    else
    {
      LOG_ERROR("Unable to find 'Matrix' attribute of '" << fromAttribute << "' to '" << toAttribute << "' transform among the CoordinateDefinitions in the configuration file"); 
      numberOfErrors++; 
      continue; 
    }

    if ( this->SetTransform(transformName, transformMatrix) != PLUS_SUCCESS )
    {
      LOG_ERROR("Unable to set transform: '" << fromAttribute << "' to '" << toAttribute << "' transform"); 
      numberOfErrors++; 
      continue; 
    }

    if ( this->SetTransformPersistent(transformName, true) != PLUS_SUCCESS )
    {
      LOG_ERROR("Unable to set transform to persistent: '" << fromAttribute << "' to '" << toAttribute << "' transform"); 
      numberOfErrors++; 
      continue; 
    }

    double error(0); 
    if ( nestedElement->GetScalarAttribute("Error", error) )
    {
      if ( this->SetTransformError(transformName, error) != PLUS_SUCCESS )
      {
        LOG_ERROR("Unable to set transform error: '" << fromAttribute << "' to '" << toAttribute << "' transform"); 
        numberOfErrors++; 
        continue; 
      }
    }

    const char* date =  nestedElement->GetAttribute("Date"); 
    if ( date != NULL )
    {
      if ( this->SetTransformDate(transformName, date) != PLUS_SUCCESS )
      {
        LOG_ERROR("Unable to set transform date: '" << fromAttribute << "' to '" << toAttribute << "' transform"); 
        numberOfErrors++; 
        continue; 
      }
    }
  }

  return (numberOfErrors == 0 ? PLUS_SUCCESS : PLUS_FAIL ); 
}

//----------------------------------------------------------------------------
PlusStatus vtkTransformRepository::WriteConfiguration(vtkXMLDataElement* configRootElement)
{
  
  if ( configRootElement == NULL )
  {
    LOG_ERROR("Failed to write transforms to CoordinateDefinitions - config root element is NULL"); 
    return PLUS_FAIL; 
  }

  vtkSmartPointer<vtkXMLDataElement> coordinateDefinitions = configRootElement->FindNestedElementWithName("CoordinateDefinitions");
  if ( coordinateDefinitions != NULL )
  {
    coordinateDefinitions->RemoveAllNestedElements(); 
  }
  else
  {
    coordinateDefinitions = vtkSmartPointer<vtkXMLDataElement>::New(); 
    coordinateDefinitions->SetName("CoordinateDefinitions"); 
    configRootElement->AddNestedElement(coordinateDefinitions); 
  }

  int numberOfErrors(0); 
  for (CoordFrameToCoordFrameToTransformMapType::iterator coordFrame=this->CoordinateFrames.begin(); coordFrame!=this->CoordinateFrames.end(); ++coordFrame)
  {
    for (CoordFrameToTransformMapType::iterator transformInfo=coordFrame->second.begin(); transformInfo!=coordFrame->second.end(); ++transformInfo)
    {
      if ( transformInfo->second.m_IsPersistent && !transformInfo->second.m_IsComputed )
      {
        std::string fromCoordinateFrame = coordFrame->first; 
        std::string toCoordinateFrame = transformInfo->first; 

        if ( transformInfo->second.m_Transform == NULL )
        {
          LOG_ERROR("Transformation matrix is NULL between '" << fromCoordinateFrame << "' to '" << toCoordinateFrame << "' coordinate frames."); 
          numberOfErrors++; 
          continue; 
        }

        if ( !transformInfo->second.m_IsValid )
        {
          LOG_WARNING("Invalid transform saved to CoordinateDefinitions from  '" << fromCoordinateFrame << "' to '" << toCoordinateFrame << "' coordinate frame." ); 
        }
      
        double vectorMatrix[16]={0}; 
        vtkMatrix4x4::DeepCopy(vectorMatrix,transformInfo->second.m_Transform->GetMatrix() ); 

        vtkSmartPointer<vtkXMLDataElement> newTransformElement = vtkSmartPointer<vtkXMLDataElement>::New();
        newTransformElement->SetName("Transform"); 
        newTransformElement->SetAttribute("From", fromCoordinateFrame.c_str()); 
        newTransformElement->SetAttribute("To", toCoordinateFrame.c_str()); 
        newTransformElement->SetVectorAttribute("Matrix", 16, vectorMatrix); 

        if ( transformInfo->second.m_Error > 0 ) 
        {
          newTransformElement->SetDoubleAttribute("Error", transformInfo->second.m_Error); 
        }

        if ( !transformInfo->second.m_Date.empty() )
        {
          newTransformElement->SetAttribute("Date", transformInfo->second.m_Date.c_str() ); 
        }
        else // Add current date if it was not explicitly specified
        {
          newTransformElement->SetAttribute("Date", vtksys::SystemTools::GetCurrentDateTime("%Y.%m.%d %X").c_str() );
        }

        coordinateDefinitions->AddNestedElement(newTransformElement); 

      }
    }
  }
  return (numberOfErrors == 0 ? PLUS_SUCCESS : PLUS_FAIL ); 
}

//----------------------------------------------------------------------------
PlusStatus vtkTransformRepository::DeepCopy(vtkTransformRepository* sourceRepositoryName)
{
  PlusLockGuard<vtkRecursiveCriticalSection> accessGuard(this->CriticalSection);
  vtkSmartPointer<vtkXMLDataElement> configRootElement=vtkSmartPointer<vtkXMLDataElement>::New();
  sourceRepositoryName->WriteConfiguration(configRootElement);
  return ReadConfiguration(configRootElement);
}
