/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/ 

#include "vtkTrackedFrameList.h" 
#include "TrackedFrame.h"
#include "PlusMath.h"

#include <math.h>
#include "vtkObjectFactory.h"
#include "vtksys/SystemTools.hxx"
#include "vtkXMLUtilities.h"
#include "vtkMetaImageSequenceIO.h"



//----------------------------------------------------------------------------
// ************************* vtkTrackedFrameList *****************************
//----------------------------------------------------------------------------
vtkCxxRevisionMacro(vtkTrackedFrameList, "$Revision: 1.0 $");
vtkStandardNewMacro(vtkTrackedFrameList); 

//----------------------------------------------------------------------------
vtkTrackedFrameList::vtkTrackedFrameList()
{
  this->CustomFields["UltrasoundImageOrientation"] = "MF"; 

  this->SetNumberOfUniqueFrames(5); 

  this->MinRequiredTranslationDifferenceMm=0.0;
  this->MinRequiredAngleDifferenceDeg=0.0;
  this->MaxAllowedTranslationSpeedMmPerSec=0.0;
  this->MaxAllowedRotationSpeedDegPerSec=0.0;
  this->ValidationRequirements = 0; 

}

//----------------------------------------------------------------------------
vtkTrackedFrameList::~vtkTrackedFrameList()
{
  this->Clear(); 
}

//----------------------------------------------------------------------------
PlusStatus vtkTrackedFrameList::RemoveTrackedFrame( int frameNumber )
{
  if ( frameNumber < 0 || (unsigned int)frameNumber >= this->GetNumberOfTrackedFrames() )
  {
    LOG_WARNING("Failed to remove tracked frame from list - invalid frame number: " << frameNumber ); 
    return PLUS_FAIL; 
  }

  delete this->TrackedFrameList[frameNumber]; 
  this->TrackedFrameList.erase(this->TrackedFrameList.begin()+frameNumber); 

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkTrackedFrameList::RemoveTrackedFrameRange( int frameNumberFrom, int frameNumberTo )
{
  if ( frameNumberFrom < 0 || (unsigned int)frameNumberTo >= this->GetNumberOfTrackedFrames() || frameNumberFrom > frameNumberTo)
  {
    LOG_WARNING("Failed to remove tracked frame from list - invalid frame number range: (" << frameNumberFrom << ", " << frameNumberTo << ")" ); 
    return PLUS_FAIL; 
  }

  for (int i=frameNumberFrom; i<=frameNumberTo; ++i)
  {
    delete this->TrackedFrameList[i];
  }

  this->TrackedFrameList.erase(this->TrackedFrameList.begin()+frameNumberFrom, this->TrackedFrameList.begin()+frameNumberTo+1);

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
void vtkTrackedFrameList::Clear()
{
  for ( unsigned int i = 0; i < this->TrackedFrameList.size(); i++ )
  {
    if (this->TrackedFrameList[i] != NULL )
    {
      delete this->TrackedFrameList[i]; 
      this->TrackedFrameList[i] = NULL; 
    }
  }
  this->TrackedFrameList.clear(); 
}

//----------------------------------------------------------------------------
void vtkTrackedFrameList::PrintSelf(std::ostream &os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);

  os << indent << "Number of frames = " << GetNumberOfTrackedFrames();
  for ( FieldMapType::const_iterator it = this->CustomFields.begin(); it != this->CustomFields.end(); it++) 
  {
    os << indent << it->first << " = " << it->second << std::endl; 
  }  
}

//----------------------------------------------------------------------------
TrackedFrame* vtkTrackedFrameList::GetTrackedFrame(int frameNumber)
{
  if ( (unsigned int)frameNumber >= this->GetNumberOfTrackedFrames() )
  {
    LOG_ERROR("vtkTrackedFrameList::GetTrackedFrame requested a non-existing frame (framenumber="<<frameNumber);
    return NULL; 
  }
  return this->TrackedFrameList[frameNumber];   
}

//----------------------------------------------------------------------------
PlusStatus vtkTrackedFrameList::AddTrackedFrameList(vtkTrackedFrameList* inTrackedFrameList)
{
  PlusStatus status = PLUS_SUCCESS; 
  for ( unsigned int i = 0; i < inTrackedFrameList->GetNumberOfTrackedFrames(); ++i )
  {
    if ( this->AddTrackedFrame( inTrackedFrameList->GetTrackedFrame(i) ) != PLUS_SUCCESS )
    {
      LOG_ERROR("Failed to add tracked frame to the list!"); 
      status = PLUS_FAIL; 
      continue; 
    }
  }

  return status; 
}

//----------------------------------------------------------------------------
PlusStatus vtkTrackedFrameList::AddTrackedFrame(TrackedFrame *trackedFrame, InvalidFrameAction action/*=ADD_INVALID_FRAME_AND_REPORT_ERROR*/ )
{
  bool isFrameValid = true; 
  if ( action != ADD_INVALID_FRAME )
  {
    isFrameValid = this->ValidateData(trackedFrame); 
  }

  if ( !isFrameValid )
  {
    switch(action)
    {
    case ADD_INVALID_FRAME_AND_REPORT_ERROR: 
      LOG_ERROR("A similar frame is already found in the tracked frame list, but invalid frame added to the list."); 
      break; 
    case ADD_INVALID_FRAME: 
      LOG_DEBUG("A similar frame is already found in the tracked frame list, but invalid frame added to the list.");
      break; 
    case SKIP_INVALID_FRAME_AND_REPORT_ERROR: 
      LOG_ERROR("A similar frame is already found in the tracked frame list, invalid frame skipped."); 
      return PLUS_FAIL;
    case SKIP_INVALID_FRAME: 
      LOG_DEBUG("A similar frame is already found in the tracked frame list, invalid frame skipped.");
      return PLUS_SUCCESS; 
    }
  }

  // Make a copy and add frame to the list 
  TrackedFrame* pTrackedFrame = new TrackedFrame(*trackedFrame); 
  this->TrackedFrameList.push_back(pTrackedFrame); 
  return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
bool vtkTrackedFrameList::ValidateData(TrackedFrame* trackedFrame )
{
  if ( this->ValidationRequirements == 0 )
  {
    // If we don't want to validate return immediately
    return true; 
  }

  if ( this->ValidationRequirements & REQUIRE_UNIQUE_TIMESTAMP )
  {
    if (! this->ValidateTimestamp(trackedFrame))
    {
      LOG_DEBUG("Validation failed - timestamp is not unique!"); 
      return false;
    }
  }

  if ( this->ValidationRequirements & REQUIRE_TRACKING_OK )
  {
    if (! this->ValidateStatus(trackedFrame))
    {
      LOG_DEBUG("Validation failed - tracking status in not OK!"); 
      return false;
    }
  }
  
  if ( this->ValidationRequirements & REQUIRE_CHANGED_TRANSFORM )
  {
    if (! this->ValidateTransform(trackedFrame))
    {
      LOG_DEBUG("Validation failed - transform is not changed!"); 
      return false;
    }
  }

  if ( this->ValidationRequirements & REQUIRE_CHANGED_ENCODER_POSITION )
  {
    if (! this->ValidateEncoderPosition(trackedFrame))
    {
      LOG_DEBUG("Validation failed - encoder position is not changed!"); 
      return false;
    }
  }


  if ( this->ValidationRequirements & REQUIRE_SPEED_BELOW_THRESHOLD )
  {
    if (! this->ValidateSpeed(trackedFrame))
    {
      LOG_DEBUG("Validation failed - speed is higher than threshold!"); 
      return false;
    }
  }

  return true; 
}

//----------------------------------------------------------------------------
bool vtkTrackedFrameList::ValidateTimestamp(TrackedFrame* trackedFrame)
{
  if ( this->TrackedFrameList.size() > 0 )
  {
    const bool isTimestampUnique = std::find_if(this->TrackedFrameList.begin(), this->TrackedFrameList.end(), TrackedFrameTimestampFinder(trackedFrame) ) == this->TrackedFrameList.end(); 

    if ( !isTimestampUnique )
    {
      LOG_DEBUG("Tracked frame timestamp validation result: we've already inserted this frame to container!"); 
      return false; 
    }

    return true; 
  }

  return true; 
}

//----------------------------------------------------------------------------
bool vtkTrackedFrameList::ValidateEncoderPosition( TrackedFrame* trackedFrame )
{
  TrackedFrameListType::iterator searchIndex; 
  const int containerSize = this->TrackedFrameList.size(); 
  if (containerSize < this->NumberOfUniqueFrames )
  {
    searchIndex = this->TrackedFrameList.begin(); 
  }
  else
  {
    searchIndex =this->TrackedFrameList.end() - this->NumberOfUniqueFrames; 
  }

  if (std::find_if(searchIndex, this->TrackedFrameList.end(), TrackedFrameEncoderPositionFinder(trackedFrame, 
    this->MinRequiredTranslationDifferenceMm, this->MinRequiredAngleDifferenceDeg) ) != this->TrackedFrameList.end() )
  {
    // We've already inserted this frame 
    LOG_DEBUG("Tracked frame encoder position validation result: we've already inserted this frame to container!"); 
    return false; 
  }
  return true; 	
}

//----------------------------------------------------------------------------
bool vtkTrackedFrameList::ValidateTransform(TrackedFrame* trackedFrame)
{
  TrackedFrameListType::iterator searchIndex; 
  const int containerSize = this->TrackedFrameList.size(); 
  if (containerSize < this->NumberOfUniqueFrames )
  {
    searchIndex = this->TrackedFrameList.begin(); 
  }
  else
  {
    searchIndex =this->TrackedFrameList.end() - this->NumberOfUniqueFrames; 
  }

  if (std::find_if(searchIndex, this->TrackedFrameList.end(), TrackedFrameTransformFinder(trackedFrame, this->FrameTransformNameForValidation,
    this->MinRequiredTranslationDifferenceMm, this->MinRequiredAngleDifferenceDeg) ) != this->TrackedFrameList.end() )
  {
    // We've already inserted this frame 
    LOG_DEBUG("Tracked frame transform validation result: we've already inserted this frame to container!"); 
    return false; 
  }

  return true; 	
}

//----------------------------------------------------------------------------
bool vtkTrackedFrameList::ValidateStatus(TrackedFrame* trackedFrame)
{
  TrackedFrameFieldStatus status = FIELD_INVALID;
  std::string transformName; 
  if ( this->FrameTransformNameForValidation.GetTransformName(transformName) != PLUS_SUCCESS )
  {
    LOG_WARNING("Failed to validate transform status - transform name for validation is incorrect!"); 
    return false; 
  }

  std::string toolStatusFrameFieldName = transformName + std::string("TransformStatus");
  status = TrackedFrame::ConvertFieldStatusFromString( trackedFrame->GetCustomFrameField( toolStatusFrameFieldName.c_str() ) );

  if ( status != FIELD_OK )
  {
    LOG_DEBUG("Tracked frame status validation result: tracked frame status invalid for tool " << transformName); 
    return false;
  }

  return true;
}

//----------------------------------------------------------------------------
bool vtkTrackedFrameList::ValidateSpeed(TrackedFrame* trackedFrame)
{
  if ( this->TrackedFrameList.size() < 1 ) {
    return true;
  }

  TrackedFrameListType::iterator latestFrameInList = this->TrackedFrameList.end() - 1;

  // Compute difference between the last two timestamps
  double diffTimeSec = fabs( trackedFrame->GetTimestamp() - (*latestFrameInList)->GetTimestamp() );
  if (diffTimeSec < 0.0001) 
  { 
    // the frames are almost acquired at the same time, cannot compute speed reliably
    // better to invalidate the frame
    return false;
  }

  vtkSmartPointer<vtkMatrix4x4> inputTransformMatrix = vtkSmartPointer<vtkMatrix4x4>::New(); 
  double inputTransformVector[16]={0}; 
  if ( trackedFrame->GetCustomFrameTransform(this->FrameTransformNameForValidation, inputTransformVector) )
  {
    inputTransformMatrix->DeepCopy(inputTransformVector); 
  }
  else
  {
    std::string strFrameTransformName; 
    this->FrameTransformNameForValidation.GetTransformName(strFrameTransformName); 
    LOG_ERROR("Unable to get frame transform '"<<strFrameTransformName<<"' from input tracked frame!");
    return false;
  }

  vtkSmartPointer<vtkMatrix4x4> latestTransformMatrix = vtkSmartPointer<vtkMatrix4x4>::New(); 
  double latestTransformVector[16]={0}; 
  if ( (*latestFrameInList)->GetCustomFrameTransform(this->FrameTransformNameForValidation, latestTransformVector) )
  {
    latestTransformMatrix->DeepCopy(latestTransformVector); 
  }
  else
  {
    LOG_ERROR("Unable to get default frame transform for latest frame!");
    return false;
  }
 
  if ( this->MaxAllowedTranslationSpeedMmPerSec>0)
  {
    // Compute difference between the last two positions
    double diffPosition = PlusMath::GetPositionDifference( inputTransformMatrix, latestTransformMatrix );
    double velocityPositionMmPerSec = fabs( diffPosition / diffTimeSec );
    if (velocityPositionMmPerSec > this->MaxAllowedTranslationSpeedMmPerSec)
    {
      LOG_DEBUG("Tracked frame speed validation result: tracked frame position change too fast (VelocityPosition = " << velocityPositionMmPerSec << ">" << this->MaxAllowedTranslationSpeedMmPerSec << " mm/sec)"); 
      return false; 
    }
  }

  if ( this->MaxAllowedRotationSpeedDegPerSec>0)
  {
    // Compute difference between the last two orientations
    double diffOrientationDeg = PlusMath::GetOrientationDifference( inputTransformMatrix, latestTransformMatrix );
    double velocityOrientationDegPerSec = fabs( diffOrientationDeg / diffTimeSec );
    if (velocityOrientationDegPerSec > this->MaxAllowedRotationSpeedDegPerSec )
    {
      LOG_DEBUG("Tracked frame speed validation result: tracked frame angle change too fast VelocityOrientation = " << velocityOrientationDegPerSec << ">" << this->MaxAllowedRotationSpeedDegPerSec << " deg/sec)"); 
      return false; 
    }
  }

  return true; 
}

//----------------------------------------------------------------------------
int vtkTrackedFrameList::GetNumberOfBitsPerPixel()
{
  int numberOfBitsPerPixel = 0; 
  if ( this->GetNumberOfTrackedFrames() > 0 )
  {
    numberOfBitsPerPixel = this->GetTrackedFrame(0)->GetNumberOfBitsPerPixel(); 
  }
  else
  {
    LOG_WARNING("Unable to get frame size: there is no frame in the tracked frame list!"); 
  }

  return numberOfBitsPerPixel; 
}

//----------------------------------------------------------------------------
PlusStatus vtkTrackedFrameList::ReadFromSequenceMetafile(const char* trackedSequenceDataFileName)
{
  vtkSmartPointer<vtkMetaImageSequenceIO> reader=vtkSmartPointer<vtkMetaImageSequenceIO>::New();
  reader->SetFileName(trackedSequenceDataFileName);
  reader->SetTrackedFrameList(this);
  if (reader->Read()!=PLUS_SUCCESS)
  {		
    LOG_ERROR("Couldn't read sequence metafile: " <<  trackedSequenceDataFileName ); 
    return PLUS_FAIL;
  }	
  
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkTrackedFrameList::SaveToSequenceMetafile(const char* outputFolder, const char* sequenceDataFileName, SEQ_METAFILE_EXTENSION extension /*=SEQ_METAFILE_MHA*/ , bool useCompression /*=true*/)
{
  vtkSmartPointer<vtkMetaImageSequenceIO> writer=vtkSmartPointer<vtkMetaImageSequenceIO>::New();
  std::string trackedSequenceDataFileName=std::string(outputFolder)+std::string("/")+std::string(sequenceDataFileName);
  if (extension==SEQ_METAFILE_MHA)
  {
    trackedSequenceDataFileName+=".mha";
  }
  else
  {
    trackedSequenceDataFileName+=".mhd";
  }
  writer->SetFileName(trackedSequenceDataFileName.c_str());
  writer->SetTrackedFrameList(this);
  writer->SetUseCompression(useCompression);
  if (writer->Write()!=PLUS_SUCCESS)
  {		
    LOG_ERROR("Couldn't write sequence metafile: " <<  trackedSequenceDataFileName ); 
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusCommon::ITKScalarPixelType vtkTrackedFrameList::GetPixelType()
{
  if ( this->GetNumberOfTrackedFrames() < 1 )
  {
    LOG_ERROR("Unable to get pixel type size: there is no frame in the tracked frame list!"); 
    return itk::ImageIOBase::UNKNOWNCOMPONENTTYPE;
  }
  
  for ( unsigned int i = 0; i < this->GetNumberOfTrackedFrames(); ++i )
  {
    if ( this->GetTrackedFrame(i)->GetImageData()->IsImageValid() )
    {
      return this->GetTrackedFrame(i)->GetImageData()->GetITKScalarPixelType();
    }
  }

  LOG_DEBUG("There are no valid images in the tracked frame list."); 
  return itk::ImageIOBase::UNKNOWNCOMPONENTTYPE;
}

//-----------------------------------------------------------------------------
US_IMAGE_ORIENTATION vtkTrackedFrameList::GetImageOrientation()
{
  if ( this->GetNumberOfTrackedFrames() < 1 )
  {
    LOG_ERROR("Unable to get image orientation: there is no frame in the tracked frame list!"); 
    return US_IMG_ORIENT_XX;
  }
  
  for ( unsigned int i = 0; i < this->GetNumberOfTrackedFrames(); ++i )
  {
    if ( this->GetTrackedFrame(i)->GetImageData()->IsImageValid() )
    {
      return this->GetTrackedFrame(i)->GetImageData()->GetImageOrientation();
    }
  }

  LOG_DEBUG("There are no valid images in the tracked frame list."); 
  return US_IMG_ORIENT_XX;
}

//-----------------------------------------------------------------------------
US_IMAGE_TYPE vtkTrackedFrameList::GetImageType()
{
  if ( this->GetNumberOfTrackedFrames() < 1 )
  {
    LOG_ERROR("Unable to get image type: there is no frame in the tracked frame list!"); 
    return US_IMG_TYPE_XX;
  }
  
  for ( unsigned int i = 0; i < this->GetNumberOfTrackedFrames(); ++i )
  {
    if ( this->GetTrackedFrame(i)->GetImageData()->IsImageValid() )
    {
      return this->GetTrackedFrame(i)->GetImageData()->GetImageType();
    }
  }

  LOG_DEBUG("There are no valid images in the tracked frame list."); 
  return US_IMG_TYPE_XX;
}

//----------------------------------------------------------------------------
const char* vtkTrackedFrameList::GetCustomString( const char* fieldName )
{
  FieldMapType::iterator fieldIterator; 
  fieldIterator = this->CustomFields.find(fieldName); 
  if ( fieldIterator != this->CustomFields.end() )
  {
    return fieldIterator->second.c_str(); 
  }
  return NULL; 
}

//----------------------------------------------------------------------------
PlusStatus vtkTrackedFrameList::GetCustomTransform( const char* frameTransformName, vtkMatrix4x4* transformMatrix )
{
  double transform[16] = { 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1 };
  const PlusStatus retValue = this->GetCustomTransform(frameTransformName, transform); 
  transformMatrix->DeepCopy(transform); 

  return retValue; 
}

//----------------------------------------------------------------------------
PlusStatus vtkTrackedFrameList::GetCustomTransform( const char* frameTransformName, double* transformMatrix )
{
  if (frameTransformName==NULL)
  {
    LOG_ERROR("Invalid frame transform name");
    return PLUS_FAIL; 
  }
  const char* customString = this->GetCustomString(frameTransformName); 
  if ( customString == NULL )
  {
    LOG_ERROR("Cannot find frame transform "<<frameTransformName);
    return PLUS_FAIL; 
  }

  std::istringstream transformFieldValue(customString); 
  double item; 
  int i = 0; 
  while ( transformFieldValue >> item )
  {
    transformMatrix[i++] = item; 
  }

  return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
void vtkTrackedFrameList::SetCustomTransform( const char* frameTransformName, vtkMatrix4x4* transformMatrix )
{
  double transform[16]; 
  for ( int i = 0; i < 4; i++ ) 
  {
    for ( int j = 0; j < 4; j++ ) 
    {
      transform[i*4+j] = transformMatrix->GetElement(i, j); 
    }
  }

  this->SetCustomTransform(frameTransformName, transform); 
}

//----------------------------------------------------------------------------
void vtkTrackedFrameList::SetCustomTransform( const char* frameTransformName, double* transformMatrix )
{
  std::ostringstream transform; 

  transform	<< transformMatrix[0]  << " " << transformMatrix[1]  << " " << transformMatrix[2]  << " " << transformMatrix[3]  << " " 
    << transformMatrix[4]  << " " << transformMatrix[5]  << " " << transformMatrix[6]  << " " << transformMatrix[7]  << " " 
    << transformMatrix[8]  << " " << transformMatrix[9]  << " " << transformMatrix[10] << " " << transformMatrix[11] << " " 
    << transformMatrix[12] << " " << transformMatrix[13] << " " << transformMatrix[14] << " " << transformMatrix[15] << " "; 

  this->SetCustomString(frameTransformName, transform.str().c_str()); 
}

//----------------------------------------------------------------------------
PlusStatus vtkTrackedFrameList::SetCustomString(const char* fieldName, const char* fieldValue)
{
  if (fieldName==NULL)
  {
    LOG_ERROR("Field name is invalid");
    return PLUS_FAIL;
  }
  if (fieldValue==NULL)
  {
    this->CustomFields.erase(fieldName);
    return PLUS_SUCCESS;
  }
  this->CustomFields[fieldName]=fieldValue;
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
void vtkTrackedFrameList::GetCustomFieldNameList(std::vector<std::string> &fieldNames)
{
  fieldNames.clear();
  for ( FieldMapType::const_iterator it = this->CustomFields.begin(); it != this->CustomFields.end(); it++) 
  {
    fieldNames.push_back(it->first); 
  }  
}

//----------------------------------------------------------------------------
PlusStatus vtkTrackedFrameList::GetGlobalTransform(vtkMatrix4x4* globalTransform)
{
  const char* offsetStr=GetCustomString("Offset");
  if (offsetStr==NULL)
  {
    LOG_ERROR("Cannot determine global transform, Offset is undefined");
    return PLUS_FAIL;
  }
  const char* transformStr=GetCustomString("TransformMatrix");
  if (transformStr==NULL)
  {
    LOG_ERROR("Cannot determine global transform, TransformMatrix is undefined");
    return PLUS_FAIL;
  }

  double item;
  int i=0;

  const int offsetLen=3;
  double offset[offsetLen];
  std::istringstream offsetFieldValue(offsetStr); 
  for (i = 0; offsetFieldValue>>item && i<offsetLen; i++)
  {
    offset[i] = item; 
  }
  if (i<offsetLen)
  {
    LOG_ERROR("Not enough elements in the Offset field (expected "<<offsetLen<<", found "<<i<<")");
    return PLUS_FAIL;
  }

  const int transformLen=9;
  double transform[transformLen];
  std::istringstream transformFieldValue(transformStr); 
  for (i = 0; transformFieldValue>>item && i<transformLen; i++)
  {
    transform[i] = item; 
  }
  if (i<transformLen)
  {
    LOG_ERROR("Not enough elements in the TransformMatrix field (expected "<<transformLen<<", found "<<i<<")");
    return PLUS_FAIL;
  }

  globalTransform->Identity();

  globalTransform->SetElement(0, 3, offset[0]);
  globalTransform->SetElement(1, 3, offset[1]);
  globalTransform->SetElement(2, 3, offset[2]);

  globalTransform->SetElement(0, 0, transform[0]);
  globalTransform->SetElement(0, 1, transform[1]);
  globalTransform->SetElement(0, 2, transform[2]);
  globalTransform->SetElement(1, 0, transform[3]);
  globalTransform->SetElement(1, 1, transform[4]);
  globalTransform->SetElement(1, 2, transform[5]);
  globalTransform->SetElement(2, 0, transform[6]);
  globalTransform->SetElement(2, 1, transform[7]);
  globalTransform->SetElement(2, 2, transform[8]);
  
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkTrackedFrameList::SetGlobalTransform(vtkMatrix4x4* globalTransform)
{
  std::ostringstream strOffset;
  strOffset << globalTransform->GetElement(0,3)
    << " " << globalTransform->GetElement(1,3)
    << " " << globalTransform->GetElement(2,3);  
  SetCustomString("Offset", strOffset.str().c_str());

  std::ostringstream strTransform;
  strTransform << globalTransform->GetElement(0,0) << " " << globalTransform->GetElement(0,1) << " " << globalTransform->GetElement(0,2) << " ";  
  strTransform << globalTransform->GetElement(1,0) << " " << globalTransform->GetElement(1,1) << " " << globalTransform->GetElement(1,2) << " ";  
  strTransform << globalTransform->GetElement(2,0) << " " << globalTransform->GetElement(2,1) << " " << globalTransform->GetElement(2,2);  
  SetCustomString("TransformMatrix", strTransform.str().c_str());

  return PLUS_SUCCESS;
}

PlusStatus vtkTrackedFrameList::VerifyProperties(vtkTrackedFrameList* trackedFrameList, US_IMAGE_ORIENTATION expectedOrientation, US_IMAGE_TYPE expectedType)
{
  if (trackedFrameList == NULL)
  {
    LOG_ERROR("vtkTrackedFrameList::VerifyProperties failed: tracked frame list is NULL!"); 
    return PLUS_FAIL; 
  }
  if (trackedFrameList->GetImageOrientation()!=expectedOrientation)
  {
    LOG_ERROR("vtkTrackedFrameList::VerifyProperties failed: expected image orientation is "
      <<PlusVideoFrame::GetStringFromUsImageOrientation(expectedOrientation)
      << ", actual orientation is "
      <<PlusVideoFrame::GetStringFromUsImageOrientation(trackedFrameList->GetImageOrientation())  );
    return PLUS_FAIL;
  }
  if (trackedFrameList->GetImageType()!=expectedType)
  {
    LOG_ERROR("vtkTrackedFrameList::VerifyProperties failed: expected image type is "
      <<PlusVideoFrame::GetStringFromUsImageType(expectedType)
      << ", actual type is "
      <<PlusVideoFrame::GetStringFromUsImageType(trackedFrameList->GetImageType())  );
    return PLUS_FAIL;
  }
  
  return PLUS_SUCCESS;
}
