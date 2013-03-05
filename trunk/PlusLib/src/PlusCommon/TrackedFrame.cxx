#include "PlusConfigure.h"
#include "TrackedFrame.h"
#include "PlusMath.h"

#include "vtkMatrix4x4.h"
#include "vtkPoints.h"
#include "vtkXMLUtilities.h"

#include "itkCastImageFilter.h"
#include "itkImageFileWriter.h"

//----------------------------------------------------------------------------
// ************************* TrackedFrame ************************************
//----------------------------------------------------------------------------

const std::string TrackedFrame::TransformPostfix = "Transform";
const std::string TrackedFrame::TransformStatusPostfix = "TransformStatus";

//----------------------------------------------------------------------------
TrackedFrame::TrackedFrame()
{
  this->Timestamp = 0; 
  this->FrameSize[0] = 0; 
  this->FrameSize[1] = 0; 
  this->FiducialPointsCoordinatePx = NULL; 
}

//----------------------------------------------------------------------------
TrackedFrame::~TrackedFrame()
{
  this->SetFiducialPointsCoordinatePx(NULL); 
}

//----------------------------------------------------------------------------
TrackedFrame::TrackedFrame(const TrackedFrame& frame)
{
  this->Timestamp = 0; 
  this->FrameSize[0] = 0; 
  this->FrameSize[1] = 0; 
  this->FiducialPointsCoordinatePx = NULL; 

  *this = frame; 
}

//----------------------------------------------------------------------------
TrackedFrame& TrackedFrame::operator=(TrackedFrame const&trackedFrame)
{
  // Handle self-assignment
  if (this == &trackedFrame)
  {
    return *this;
  }

  this->CustomFrameFields = trackedFrame.CustomFrameFields; 
  this->ImageData = trackedFrame.ImageData; 
  this->Timestamp = trackedFrame.Timestamp;
  this->FrameSize[0] = trackedFrame.FrameSize[0]; 
  this->FrameSize[1] = trackedFrame.FrameSize[1]; 
  this->SetFiducialPointsCoordinatePx(trackedFrame.FiducialPointsCoordinatePx);

  return *this;
}

//----------------------------------------------------------------------------
PlusStatus TrackedFrame::GetTrackedFrameInXmlData( std::string& strXmlData )
{
  vtkSmartPointer<vtkXMLDataElement> xmlData = vtkSmartPointer<vtkXMLDataElement>::New(); 
  PlusStatus status = this->PrintToXML(xmlData); 

  std::ostringstream os; 
  PlusCommon::PrintXML(os, vtkIndent(0), xmlData); 
    
  strXmlData = os.str(); 

  return status; 
}

//----------------------------------------------------------------------------
PlusStatus TrackedFrame::PrintToXML(vtkXMLDataElement* trackedFrame)
{
  if ( trackedFrame == NULL )
  {
    LOG_ERROR("Unable to print tracked frame to XML - input XML data is NULL"); 
    return PLUS_FAIL; 
  }
  
  trackedFrame->SetName("TrackedFrame"); 
  trackedFrame->SetDoubleAttribute("Timestamp", this->Timestamp); 
  trackedFrame->SetAttribute("ImageDataValid", (this->GetImageData()->IsImageValid()?"true":"false")); 
  if ( this->GetImageData()->IsImageValid() )
  {
    trackedFrame->SetIntAttribute("NumberOfBits", this->GetNumberOfBitsPerPixel() ); 
    trackedFrame->SetVectorAttribute("FrameSize", 2, this->GetFrameSize()); 
  }
  for ( FieldMapType::const_iterator it = this->CustomFrameFields.begin(); it != this->CustomFrameFields.end(); it++) 
  {
    vtkSmartPointer<vtkXMLDataElement> customField = vtkSmartPointer<vtkXMLDataElement>::New(); 
    customField->SetName("CustomFrameField");
    customField->SetAttribute("Name", it->first.c_str() );
    customField->SetAttribute("Value", it->second.c_str() );
    trackedFrame->AddNestedElement( customField );
  }

  if ( FiducialPointsCoordinatePx != NULL )
  {
    vtkSmartPointer<vtkXMLDataElement> segmentation = vtkSmartPointer<vtkXMLDataElement>::New(); 
    segmentation->SetName("Segmentation");

    if ( FiducialPointsCoordinatePx->GetNumberOfPoints() == 0 )
    {
      segmentation->SetAttribute("SegmentationStatus", "Failed");
    }
    else if ( FiducialPointsCoordinatePx->GetNumberOfPoints() % 3 != 0 )
    {
      segmentation->SetAttribute("SegmentationStatus", "InvalidPatterns");
    }
    else
    {
      segmentation->SetAttribute("SegmentationStatus", "OK");
    }
    
    vtkSmartPointer<vtkXMLDataElement> segmentedPoints = vtkSmartPointer<vtkXMLDataElement>::New(); 
    segmentedPoints->SetName("SegmentedPoints");

    for (int i=0; i<FiducialPointsCoordinatePx->GetNumberOfPoints(); i++)
    {
      double point[3]={0};
      FiducialPointsCoordinatePx->GetPoint(i, point);

      vtkSmartPointer<vtkXMLDataElement> pointElement = vtkSmartPointer<vtkXMLDataElement>::New(); 
      pointElement->SetName("Point");
      pointElement->SetIntAttribute("ID", i);
      pointElement->SetVectorAttribute("Position", 3, point);
      segmentedPoints->AddNestedElement( pointElement );
    }
    segmentation->AddNestedElement(segmentedPoints); 
    trackedFrame->AddNestedElement(segmentation); 
  }
  return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
PlusStatus TrackedFrame::SetTrackedFrameFromXmlData( const char* strXmlData )
{
  if ( strXmlData == NULL )
  {
    LOG_ERROR("Failed to set TrackedFrame from xml data - input xml data string is NULL!" ); 
    return PLUS_FAIL; 
  }

  vtkSmartPointer<vtkXMLDataElement> trackedFrame = vtkSmartPointer<vtkXMLDataElement>::Take( vtkXMLUtilities::ReadElementFromString(strXmlData) );

  if ( trackedFrame == NULL )
  {
    LOG_ERROR("Failed to set TrackedFrame from xml data - invalid xml data string!"); 
    return PLUS_FAIL; 
  }
  
  // Add custom fields to tracked frame
  for ( int i = 0; i < trackedFrame->GetNumberOfNestedElements(); ++i )
  {
    vtkXMLDataElement* nestedElement = trackedFrame->GetNestedElement(i); 
    if ( STRCASECMP(nestedElement->GetName(), "CustomFrameField") != 0 )
    {
      continue; 
    }

    const char* fieldName = nestedElement->GetAttribute("Name"); 
    if ( fieldName == NULL )
    {
      LOG_WARNING("Unable to find CustomFrameField Name attribute"); 
      continue; 
    }

    const char* fieldValue = nestedElement->GetAttribute("Value"); 
    if ( fieldValue == NULL )
    {
      LOG_WARNING("Unable to find CustomFrameField Value attribute"); 
      continue; 
    }
  
    this->SetCustomFrameField(fieldName, fieldValue); 
  }

  vtkXMLDataElement* segmentation = trackedFrame->FindNestedElementWithName("Segmentation"); 
  
  if ( segmentation != NULL )
  {
    this->FiducialPointsCoordinatePx = vtkSmartPointer<vtkPoints>::New(); 

    vtkXMLDataElement* segmentedPoints = segmentation->FindNestedElementWithName("SegmentedPoints"); 
    if ( segmentedPoints != NULL ) // Segmentation was successful 
    {
      for ( int i = 0; i < segmentedPoints->GetNumberOfNestedElements(); ++i )
      {
        vtkXMLDataElement* pointElement = segmentedPoints->GetNestedElement(i); 
        if ( STRCASECMP(pointElement->GetName(), "Point") != 0 )
        {
          continue; 
        }

        double pos[3]={0}; 
        if ( pointElement->GetVectorAttribute("Position", 3, pos) )
        {
          this->FiducialPointsCoordinatePx->InsertNextPoint(pos); 
        }
      }
    }
  }

  return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
int* TrackedFrame::GetFrameSize()
{
  this->ImageData.GetFrameSize(this->FrameSize);
  return this->FrameSize; 
}

//----------------------------------------------------------------------------
void TrackedFrame::GetFrameSize(int dim[2])
{
  this->ImageData.GetFrameSize(this->FrameSize);
  dim[0] = this->FrameSize[0];
  dim[1] = this->FrameSize[1];
}

//----------------------------------------------------------------------------
void TrackedFrame::SetImageData(const PlusVideoFrame &value)
{
  this->ImageData = value;

  // Set frame size too
  this->ImageData.GetFrameSize(this->FrameSize);
}

//----------------------------------------------------------------------------
void TrackedFrame::SetTimestamp(double value) 
{ 
  this->Timestamp = value; 
  std::ostringstream strTimestamp; 
  strTimestamp << this->Timestamp; 
  this->CustomFrameFields["Timestamp"]=strTimestamp.str();
}

//----------------------------------------------------------------------------
int TrackedFrame::GetNumberOfBitsPerPixel()
{
  int numberOfBitsPerPixel(0); 
  numberOfBitsPerPixel = this->ImageData.GetNumberOfBytesPerPixel()*8;
  return numberOfBitsPerPixel; 
}

//----------------------------------------------------------------------------
void TrackedFrame::SetFiducialPointsCoordinatePx(vtkPoints* fiducialPoints)
{
  if ( this->FiducialPointsCoordinatePx != fiducialPoints )
  {
    vtkPoints* tempFiducialPoints = this->FiducialPointsCoordinatePx; 

    this->FiducialPointsCoordinatePx = fiducialPoints; 
    if ( this->FiducialPointsCoordinatePx != NULL ) 
    {
      this->FiducialPointsCoordinatePx->Register(NULL); 
    } 

    if ( tempFiducialPoints != NULL ) 
    {
      tempFiducialPoints->UnRegister(NULL); 
    }
  }
}

//----------------------------------------------------------------------------
void TrackedFrame::SetCustomFrameField( std::string name, std::string value )
{
  if ( STRCASECMP(name.c_str(), "Timestamp") == 0 )
  {
    double timestamp(0); 
    if ( PlusCommon::StringToDouble(value.c_str(), timestamp) != PLUS_SUCCESS )
    {
      LOG_ERROR("Unable to convert Timestamp '"<< value << "' to double"); 
    }
    else
    {
      this->Timestamp = timestamp; 
    }
  }

  this->CustomFrameFields[name]=value;
}

//----------------------------------------------------------------------------
const char* TrackedFrame::GetCustomFrameField(const char* fieldName)
{
  if (fieldName == NULL )
  {
    LOG_ERROR("Unable to get custom frame field: field name is NULL!"); 
    return NULL; 
  }

  FieldMapType::iterator fieldIterator; 
  fieldIterator = this->CustomFrameFields.find(fieldName); 
  if ( fieldIterator != this->CustomFrameFields.end() )
  {
    return fieldIterator->second.c_str(); 
  }
  return NULL; 
}

//----------------------------------------------------------------------------
PlusStatus TrackedFrame::DeleteCustomFrameField( const char* fieldName )
{
  if ( fieldName == NULL )
  {
    LOG_DEBUG("Failed to delete custom frame field - field name is NULL!"); 
    return PLUS_FAIL; 
  }

  FieldMapType::iterator field = this->CustomFrameFields.find(fieldName); 
  if ( field != this->CustomFrameFields.end() )
  {
    this->CustomFrameFields.erase(field); 
    return PLUS_SUCCESS; 
  }
  LOG_DEBUG("Failed to delete custom frame field - could find field " << fieldName ); 
  return PLUS_FAIL; 
}
 

//----------------------------------------------------------------------------
bool TrackedFrame::IsCustomFrameTransformNameDefined(const PlusTransformName& transformName)
{
  std::string toolTransformName; 
  if ( transformName.GetTransformName(toolTransformName) != PLUS_SUCCESS )
  {
    return false; 
  }
  // Append Transform to the end of the transform name
  if ( !IsTransform(toolTransformName) )
  {
    toolTransformName.append(TransformPostfix); 
  }

  return this->IsCustomFrameFieldDefined(toolTransformName.c_str()); 
}

//----------------------------------------------------------------------------
bool TrackedFrame::IsCustomFrameFieldDefined(const char* fieldName)
{
  if (fieldName == NULL )
  {
    LOG_ERROR("Unable to find custom frame field: field name is NULL!"); 
    return false; 
  }

  FieldMapType::iterator fieldIterator; 
  fieldIterator = this->CustomFrameFields.find(fieldName); 
  if ( fieldIterator != this->CustomFrameFields.end() )
  {
    // field is found
    return true; 
  }
  // field is undefined
  return false; 
}

//----------------------------------------------------------------------------
PlusStatus TrackedFrame::GetCustomFrameTransform(const PlusTransformName& frameTransformName, double transform[16]) 
{
  std::string transformName; 
  if ( frameTransformName.GetTransformName(transformName) != PLUS_SUCCESS )
  {
    LOG_ERROR("Unable to get custom transform, transform name is wrong!"); 
    return PLUS_FAIL; 
  }

  // Append Transform to the end of the transform name
  if ( !IsTransform(transformName) )
  {
    transformName.append(TransformPostfix); 
  }

  const char* frameTransformStr = GetCustomFrameField(transformName.c_str());
  if (frameTransformStr == NULL )
  {
    LOG_ERROR("Unable to get custom transform from name: " << transformName); 
    return PLUS_FAIL; 
  }

  // Find default frame transform 
  std::istringstream transformFieldValue(frameTransformStr); 
  double item; 
  int i = 0; 
  while ( transformFieldValue>>item && i<16)
  {
    transform[i++] = item; 
  }
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus TrackedFrame::GetCustomFrameTransform(const PlusTransformName& frameTransformName, vtkMatrix4x4* transformMatrix )
{
  double transform[16] = { 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1 };
  const PlusStatus retValue = this->GetCustomFrameTransform(frameTransformName, transform); 
  transformMatrix->DeepCopy(transform); 

  return retValue; 
}

//----------------------------------------------------------------------------
PlusStatus TrackedFrame::GetCustomFrameTransformStatus(const PlusTransformName& frameTransformName, TrackedFrameFieldStatus& status)
{
  status = FIELD_INVALID; 
  std::string transformStatusName; 
  if ( frameTransformName.GetTransformName(transformStatusName) != PLUS_SUCCESS )
  {
    LOG_ERROR("Unable to get custom transform status, transform name is wrong!"); 
    return PLUS_FAIL; 
  }

  // Append TransformStatus to the end of the transform name
  if ( IsTransform(transformStatusName) )
  {
    transformStatusName.append("Status");
  }
  else if ( !IsTransformStatus(transformStatusName) )
  {
    transformStatusName.append(TransformStatusPostfix);
  }

  const char* strStatus = this->GetCustomFrameField(transformStatusName.c_str()); 
  if (strStatus == NULL )
  {
    LOG_ERROR("Unable to get custom transform status from name: " << transformStatusName);
    return PLUS_FAIL; 
  }

  status = TrackedFrame::ConvertFieldStatusFromString( strStatus );

  return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
PlusStatus TrackedFrame::SetCustomFrameTransformStatus(const PlusTransformName& frameTransformName, TrackedFrameFieldStatus status)
{
  std::string transformStatusName; 
  if ( frameTransformName.GetTransformName(transformStatusName) != PLUS_SUCCESS )
  {
    LOG_ERROR("Unable to set custom transform status, transform name is wrong!"); 
    return PLUS_FAIL; 
  }

  // Append TransformStatus to the end of the transform name
  if ( IsTransform(transformStatusName) )
  {
    transformStatusName.append("Status");
  }
  else if ( !IsTransformStatus(transformStatusName) )
  {
    transformStatusName.append(TransformStatusPostfix);
  }

  std::string strStatus = TrackedFrame::ConvertFieldStatusToString(status);

  this->SetCustomFrameField(transformStatusName, strStatus );

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus TrackedFrame::SetCustomFrameTransform(const PlusTransformName& frameTransformName, double transform[16]) 
{
  std::ostringstream strTransform; 
  for ( int i = 0; i < 16; ++i )
  {
    strTransform << transform[ i ] << " ";
  }

  std::string transformName; 
  if ( frameTransformName.GetTransformName(transformName) != PLUS_SUCCESS )
  {
    LOG_ERROR("Unable to get custom transform, transform name is wrong!"); 
    return PLUS_FAIL; 
  }

  // Append Transform to the end of the transform name
  if ( !IsTransform(transformName) )
  {
    transformName.append(TransformPostfix); 
  }

  SetCustomFrameField(transformName, strTransform.str());

  return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
PlusStatus TrackedFrame::SetCustomFrameTransform(const PlusTransformName& frameTransformName, vtkMatrix4x4* transform) 
{
  double dTransform[ 16 ];
  vtkMatrix4x4::DeepCopy( dTransform, transform );
  return SetCustomFrameTransform(frameTransformName, dTransform); 
}

//----------------------------------------------------------------------------
TrackedFrameFieldStatus TrackedFrame::ConvertFieldStatusFromString(const char* statusStr) 
{
  if ( statusStr == NULL ) 
  {
    LOG_ERROR("Failed to get field status from string if it's NULL!"); 
    return FIELD_INVALID; 
  }

  TrackedFrameFieldStatus status = FIELD_INVALID;
  std::string strFlag(statusStr); 
  if ( STRCASECMP("OK", statusStr) == 0 )
  {
    status = FIELD_OK;
  }

  return status;
}

//----------------------------------------------------------------------------
std::string TrackedFrame::ConvertFieldStatusToString(TrackedFrameFieldStatus status)
{
  std::string strStatus; 
  if ( status == FIELD_OK )
  {
    strStatus = "OK"; 
  }
  else
  {
    strStatus = "INVALID"; 
  }
  return strStatus;
}

//----------------------------------------------------------------------------
PlusStatus TrackedFrame::WriteToFile(const std::string &filename, vtkMatrix4x4* mImageToTracker)
{
  typedef unsigned char      PixelType; 
  typedef itk::Image< PixelType, 2 > Image2dType; 
  typedef itk::Image< PixelType, 3 > Image3dType; 

  typedef itk::CastImageFilter< Image2dType, Image3dType> CastFilterType;
  CastFilterType::Pointer castFilter = CastFilterType::New(); 
  try 
  {      
    castFilter->SetInput(this->ImageData.GetDisplayableImage());
    castFilter->Update();
  }
  catch(itk::ExceptionObject & err)
  {
    LOG_ERROR("Failed to cast image for writing it to file: " << err.GetDescription() ); 
    return PLUS_FAIL; 
  }

  Image3dType::Pointer image=castFilter->GetOutput();     

  double origin[3]=
  {
    mImageToTracker->Element[0][3],
    mImageToTracker->Element[1][3],
    mImageToTracker->Element[2][3],
  };
  image->SetOrigin(origin);

  Image3dType::DirectionType dir;
  for (int r=0; r<3; r++)
  {
    for (int c=0; c<3; c++)
    {
      dir.GetVnlMatrix().put(r,c, mImageToTracker->Element[r][c]);
    }
  }
  image->SetDirection(dir);

  typedef itk::ImageFileWriter< Image3dType > WriterType; 
  WriterType::Pointer writeImage = WriterType::New();  

  writeImage->SetFileName(filename.c_str());  
  writeImage->SetInput( image );
  try
  {
    writeImage->Update(); 
  }
  catch (itk::ExceptionObject & err) 
  {    
    LOG_ERROR(" Exception! writer did not update. Error: "<< err); 
    return PLUS_FAIL;
  }
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
void TrackedFrame::GetCustomFrameFieldNameList(std::vector<std::string> &fieldNames)
{
  fieldNames.clear();
  for ( FieldMapType::const_iterator it = this->CustomFrameFields.begin(); it != this->CustomFrameFields.end(); it++) 
  {
    fieldNames.push_back(it->first); 
  }  
}

//----------------------------------------------------------------------------
void TrackedFrame::GetCustomFrameTransformNameList(std::vector<PlusTransformName> &transformNames)
{
  transformNames.clear();
  for ( FieldMapType::const_iterator it = this->CustomFrameFields.begin(); it != this->CustomFrameFields.end(); it++) 
  {
    if ( IsTransform(it->first) )
    {
      PlusTransformName trName;
      trName.SetTransformName(it->first.substr(0, it->first.length()-TransformPostfix.length()).c_str());
      transformNames.push_back(trName); 
    }
  }
}

//----------------------------------------------------------------------------
bool TrackedFrame::IsTransform(std::string str)
{
  if (str.length() <= TransformPostfix.length())
  {
    return false;
  }

  return !str.substr(str.length()-TransformPostfix.length()).compare(TransformPostfix);
}

//----------------------------------------------------------------------------
bool TrackedFrame::IsTransformStatus(std::string str)
{
  if (str.length() <= TransformStatusPostfix.length())
  {
    return false;
  }

  return !str.substr(str.length()-TransformStatusPostfix.length()).compare(TransformStatusPostfix);
}

//----------------------------------------------------------------------------
// ****************** TrackedFrameEncoderPositionFinder **********************
//----------------------------------------------------------------------------
TrackedFrameEncoderPositionFinder::TrackedFrameEncoderPositionFinder(TrackedFrame* frame, double minRequiredTranslationDifferenceMm, double minRequiredAngleDifferenceDeg)
: mTrackedFrame(frame), 
mMinRequiredTranslationDifferenceMm(minRequiredTranslationDifferenceMm),
mMinRequiredAngleDifferenceDeg(minRequiredAngleDifferenceDeg)
{

}

//----------------------------------------------------------------------------
TrackedFrameEncoderPositionFinder::~TrackedFrameEncoderPositionFinder()
{

}

//----------------------------------------------------------------------------
PlusStatus TrackedFrameEncoderPositionFinder::GetStepperEncoderValues( TrackedFrame* trackedFrame, double &probePosition, double &probeRotation, double &templatePosition )
{ 
  // Get the probe position from tracked frame info
  const char* cProbePos = trackedFrame->GetCustomFrameField("ProbePosition"); 
  if ( cProbePos != NULL )
  {
    if ( PlusCommon::StringToDouble(cProbePos, probePosition) != PLUS_SUCCESS )
    {
      LOG_ERROR("Unable to convert ProbePosition '"<< cProbePos << "' to double"); 
      return PLUS_FAIL;
    }
  }
  else
  {
    LOG_ERROR("Unable to find frame field: ProbePosition"); 
    return PLUS_FAIL;
  }

  // Get the probe rotation from tracked frame info
  const char* cProbeRot = trackedFrame->GetCustomFrameField("ProbeRotation"); 
  if ( cProbeRot != NULL )
  {
    if ( PlusCommon::StringToDouble(cProbeRot, probeRotation) != PLUS_SUCCESS )
    {
      LOG_ERROR("Unable to convert ProbeRotation '"<< cProbeRot << "' to double"); 
      return PLUS_FAIL;
    }
  }
  else
  {
    LOG_ERROR("Unable to find frame field: ProbeRotation"); 
    return PLUS_FAIL;
  }

  // Get the template position from tracked frame info
  const char* cTemplatePos = trackedFrame->GetCustomFrameField("TemplatePosition"); 
  if ( cTemplatePos != NULL )
  {
    if ( PlusCommon::StringToDouble(cTemplatePos, templatePosition) != PLUS_SUCCESS )
    {
      LOG_ERROR("Unable to convert TemplatePosition '"<< cTemplatePos << "' to double"); 
      return PLUS_FAIL;
    }
  }
  else
  {
    LOG_ERROR("Unable to find frame field: TemplatePosition"); 
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS; 


}

//----------------------------------------------------------------------------
bool TrackedFrameEncoderPositionFinder::operator()( TrackedFrame *newFrame )  
{
  if (mMinRequiredTranslationDifferenceMm<=0 || mMinRequiredAngleDifferenceDeg<=0)
  {
    // threshold is zero, so the frames are different for sure
    return false;
  }

  double baseProbePos(0), baseProbeRot(0), baseTemplatePos(0); 


  if ( GetStepperEncoderValues(mTrackedFrame, baseProbePos, baseProbeRot, baseTemplatePos ) != PLUS_SUCCESS ) 
  {
    LOG_WARNING("Unable to get raw encoder values from tracked frame!" ); 
    return false; 
  }

  double newProbePos(0), newProbeRot(0), newTemplatePos(0); 
  if ( GetStepperEncoderValues(newFrame, newProbePos, newProbeRot, newTemplatePos ) != PLUS_SUCCESS ) 
  {
    LOG_WARNING("Unable to get raw encoder values from tracked frame!" ); 
    return false; 
  }

  double positionDifference = fabs(baseProbePos - newProbePos) + fabs(baseTemplatePos - newTemplatePos); 
  double rotationDifference = fabs(baseProbeRot - newProbeRot); 

  if ( positionDifference < this->mMinRequiredTranslationDifferenceMm && rotationDifference < this->mMinRequiredAngleDifferenceDeg )
  {
    // same as the reference frame
    return true; 
  }
  return false; 
}  



//----------------------------------------------------------------------------
// ****************** TrackedFrameTransformFinder ****************************
//----------------------------------------------------------------------------
TrackedFrameTransformFinder::TrackedFrameTransformFinder(TrackedFrame* frame, const PlusTransformName& frameTransformName, double minRequiredTranslationDifferenceMm, double minRequiredAngleDifferenceDeg)
: mTrackedFrame(frame), 
mMinRequiredTranslationDifferenceMm(minRequiredTranslationDifferenceMm),
mMinRequiredAngleDifferenceDeg(minRequiredAngleDifferenceDeg),
mFrameTransformName(frameTransformName)
{

}

//----------------------------------------------------------------------------
TrackedFrameTransformFinder::~TrackedFrameTransformFinder()
{

}

//----------------------------------------------------------------------------
bool TrackedFrameTransformFinder::operator()( TrackedFrame *newFrame )
{
  if (mMinRequiredTranslationDifferenceMm<=0 || mMinRequiredAngleDifferenceDeg<=0)
  {
    // threshold is zero, so the frames are different for sure
    return false;
  }

  vtkSmartPointer<vtkMatrix4x4> baseTransformMatrix = vtkSmartPointer<vtkMatrix4x4>::New(); 
  double baseTransMatrix[16]={0}; 
  if ( mTrackedFrame->GetCustomFrameTransform(mFrameTransformName, baseTransMatrix) )
  {
    baseTransformMatrix->DeepCopy(baseTransMatrix); 
  }
  else
  {
    LOG_ERROR("TrackedFramePositionFinder: Unable to find base frame transform name for tracked frame validation!"); 
    return false; 
  }

  vtkSmartPointer<vtkMatrix4x4> newTransformMatrix = vtkSmartPointer<vtkMatrix4x4>::New(); 
  double newTransMatrix[16]={0}; 
  if ( newFrame->GetCustomFrameTransform(mFrameTransformName, newTransMatrix) )
  {
    newTransformMatrix->DeepCopy(newTransMatrix); 
  }
  else
  {
    LOG_ERROR("TrackedFramePositionFinder: Unable to find frame transform name for new tracked frame validation!"); 
    return false; 
  }

  double positionDifference = PlusMath::GetPositionDifference(baseTransformMatrix, newTransformMatrix); 
  double angleDifference = PlusMath::GetOrientationDifference(baseTransformMatrix, newTransformMatrix); 

  if ( abs(positionDifference) < this->mMinRequiredTranslationDifferenceMm && abs(angleDifference) < this->mMinRequiredAngleDifferenceDeg )
  {
    // same as the reference frame
    return true; 
  }

  return false; 
}
