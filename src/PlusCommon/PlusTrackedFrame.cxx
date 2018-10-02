/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"
#include "PlusMath.h"
#include "PlusTrackedFrame.h"
#include "itkCastImageFilter.h"
#include "itkImageFileWriter.h"
#include "metaImage.h"
#include "vtkImageData.h"
#include "vtkMatrix4x4.h"
#include "vtkPoints.h"
#include "vtkXMLUtilities.h"

//----------------------------------------------------------------------------
// ************************* TrackedFrame ************************************
//----------------------------------------------------------------------------

const char* PlusTrackedFrame::FIELD_FRIENDLY_DEVICE_NAME = "FriendlyDeviceName";
const std::string PlusTrackedFrame::TransformPostfix = "Transform";
const std::string PlusTrackedFrame::TransformStatusPostfix = "TransformStatus";
const int FLOATING_POINT_PRECISION = 16; // Number of digits used when writing transforms and timestamps

//----------------------------------------------------------------------------
PlusTrackedFrame::PlusTrackedFrame()
{
  this->Timestamp = 0;
  this->FrameSize[0] = 0;
  this->FrameSize[1] = 0;
  this->FrameSize[2] = 1; // single-slice frame by default
  this->FiducialPointsCoordinatePx = NULL;
}

//----------------------------------------------------------------------------
PlusTrackedFrame::~PlusTrackedFrame()
{
  this->SetFiducialPointsCoordinatePx(NULL);
}

//----------------------------------------------------------------------------
PlusTrackedFrame::PlusTrackedFrame(const PlusTrackedFrame& frame)
{
  this->Timestamp = 0;
  this->FrameSize[0] = 0;
  this->FrameSize[1] = 0;
  this->FrameSize[2] = 1; // single-slice frame by default
  this->FiducialPointsCoordinatePx = NULL;

  *this = frame;
}

//----------------------------------------------------------------------------
PlusTrackedFrame& PlusTrackedFrame::operator=(PlusTrackedFrame const& trackedFrame)
{
  // Handle self-assignment
  if (this == &trackedFrame)
  {
    return *this;
  }

  this->FrameFields = trackedFrame.FrameFields;
  this->ImageData = trackedFrame.ImageData;
  this->Timestamp = trackedFrame.Timestamp;
  this->FrameSize[0] = trackedFrame.FrameSize[0];
  this->FrameSize[1] = trackedFrame.FrameSize[1];
  this->FrameSize[2] = trackedFrame.FrameSize[2];
  this->SetFiducialPointsCoordinatePx(trackedFrame.FiducialPointsCoordinatePx);

  return *this;
}

//----------------------------------------------------------------------------
PlusStatus PlusTrackedFrame::GetTrackedFrameInXmlData(std::string& strXmlData, const std::vector<PlusTransformName>& requestedTransforms)
{
  vtkSmartPointer<vtkXMLDataElement> xmlData = vtkSmartPointer<vtkXMLDataElement>::New();
  PlusStatus status = this->PrintToXML(xmlData, requestedTransforms);

  std::ostringstream os;
  PlusCommon::XML::PrintXML(os, vtkIndent(0), xmlData);

  strXmlData = os.str();

  return status;
}

//----------------------------------------------------------------------------
PlusStatus PlusTrackedFrame::PrintToXML(vtkXMLDataElement* trackedFrame, const std::vector<PlusTransformName>& requestedTransforms)
{
  if (trackedFrame == NULL)
  {
    LOG_ERROR("Unable to print tracked frame to XML - input XML data is NULL");
    return PLUS_FAIL;
  }

  trackedFrame->SetName("TrackedFrame");
  trackedFrame->SetDoubleAttribute("Timestamp", this->Timestamp);
  trackedFrame->SetAttribute("ImageDataValid", (this->GetImageData()->IsImageValid() ? "true" : "false"));

  if (this->GetImageData()->IsImageValid())
  {
    trackedFrame->SetIntAttribute("NumberOfBits", this->GetNumberOfBitsPerScalar());
    unsigned int numberOfScalarComponents(1);
    if (this->GetNumberOfScalarComponents(numberOfScalarComponents) == PLUS_FAIL)
    {
      LOG_ERROR("Unable to retrieve number of scalar components.");
      return PLUS_FAIL;
    }
    trackedFrame->SetIntAttribute("NumberOfScalarComponents", numberOfScalarComponents);
    if (FrameSize[0] > static_cast<unsigned int>(std::numeric_limits<int>::max()) ||
        FrameSize[1] > static_cast<unsigned int>(std::numeric_limits<int>::max()) ||
        FrameSize[2] > static_cast<unsigned int>(std::numeric_limits<int>::max()))
    {
      LOG_ERROR("Unable to save frame size elements larger than: " << std::numeric_limits<int>::max());
      return PLUS_FAIL;
    }
    int frameSizeSigned[3] = { static_cast<int>(FrameSize[0]), static_cast<int>(FrameSize[1]), static_cast<int>(FrameSize[2]) };
    trackedFrame->SetVectorAttribute("FrameSize", 3, frameSizeSigned);
  }

  for (auto fieldIter = FrameFields.begin(); fieldIter != FrameFields.end(); ++fieldIter)
  {
    // Only use requested transforms mechanism if the vector is not empty
    if (!requestedTransforms.empty() && (IsTransform(fieldIter->first) || IsTransformStatus(fieldIter->first)))
    {
      if (IsTransformStatus(fieldIter->first))
      {
        continue;
      }
      if (std::find(requestedTransforms.begin(), requestedTransforms.end(), PlusTransformName(fieldIter->first)) == requestedTransforms.end())
      {
        continue;
      }
      auto statusName = fieldIter->first;
      statusName = statusName.substr(0, fieldIter->first.length() - TransformPostfix.length());
      statusName = statusName.append(TransformStatusPostfix);
      vtkSmartPointer<vtkXMLDataElement> customField = vtkSmartPointer<vtkXMLDataElement>::New();
      customField->SetName("FrameField");
      customField->SetAttribute("Name", statusName.c_str());
      customField->SetAttribute("Value", FrameFields[statusName].c_str());
      trackedFrame->AddNestedElement(customField);
    }
    vtkSmartPointer<vtkXMLDataElement> customField = vtkSmartPointer<vtkXMLDataElement>::New();
    customField->SetName("FrameField");
    customField->SetAttribute("Name", fieldIter->first.c_str());
    customField->SetAttribute("Value", fieldIter->second.c_str());
    trackedFrame->AddNestedElement(customField);
  }

  if (FiducialPointsCoordinatePx != NULL)
  {
    vtkSmartPointer<vtkXMLDataElement> segmentation = vtkSmartPointer<vtkXMLDataElement>::New();
    segmentation->SetName("Segmentation");

    if (FiducialPointsCoordinatePx->GetNumberOfPoints() == 0)
    {
      segmentation->SetAttribute("SegmentationStatus", "Failed");
    }
    else if (FiducialPointsCoordinatePx->GetNumberOfPoints() % 3 != 0)
    {
      segmentation->SetAttribute("SegmentationStatus", "InvalidPatterns");
    }
    else
    {
      segmentation->SetAttribute("SegmentationStatus", "OK");
    }

    vtkSmartPointer<vtkXMLDataElement> segmentedPoints = vtkSmartPointer<vtkXMLDataElement>::New();
    segmentedPoints->SetName("SegmentedPoints");

    for (int i = 0; i < FiducialPointsCoordinatePx->GetNumberOfPoints(); i++)
    {
      double point[3] = {0};
      FiducialPointsCoordinatePx->GetPoint(i, point);

      vtkSmartPointer<vtkXMLDataElement> pointElement = vtkSmartPointer<vtkXMLDataElement>::New();
      pointElement->SetName("Point");
      pointElement->SetIntAttribute("ID", i);
      pointElement->SetVectorAttribute("Position", 3, point);
      segmentedPoints->AddNestedElement(pointElement);
    }

    segmentation->AddNestedElement(segmentedPoints);
    trackedFrame->AddNestedElement(segmentation);
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus PlusTrackedFrame::SetTrackedFrameFromXmlData(const std::string& xmlData)
{
  return this->SetTrackedFrameFromXmlData(xmlData.c_str());
}

//----------------------------------------------------------------------------
PlusStatus PlusTrackedFrame::SetTrackedFrameFromXmlData(const char* strXmlData)
{
  if (strXmlData == NULL)
  {
    LOG_ERROR("Failed to set TrackedFrame from xml data - input xml data string is NULL!");
    return PLUS_FAIL;
  }

  vtkSmartPointer<vtkXMLDataElement> trackedFrame = vtkSmartPointer<vtkXMLDataElement>::Take(vtkXMLUtilities::ReadElementFromString(strXmlData));

  if (trackedFrame == NULL)
  {
    LOG_ERROR("Failed to set TrackedFrame from xml data - invalid xml data string!");
    return PLUS_FAIL;
  }

  // Add custom fields to tracked frame
  for (int i = 0; i < trackedFrame->GetNumberOfNestedElements(); ++i)
  {
    vtkXMLDataElement* nestedElement = trackedFrame->GetNestedElement(i);
    if (!PlusCommon::IsEqualInsensitive(nestedElement->GetName(), "FrameField"))
    {
      continue;
    }

    const char* fieldName = nestedElement->GetAttribute("Name");
    if (fieldName == NULL)
    {
      LOG_WARNING("Unable to find FrameField Name attribute");
      continue;
    }

    const char* fieldValue = nestedElement->GetAttribute("Value");
    if (fieldValue == NULL)
    {
      LOG_WARNING("Unable to find FrameField Value attribute");
      continue;
    }

    this->SetFrameField(fieldName, fieldValue);
  }

  vtkXMLDataElement* segmentation = trackedFrame->FindNestedElementWithName("Segmentation");

  if (segmentation != NULL)
  {
    this->FiducialPointsCoordinatePx = vtkSmartPointer<vtkPoints>::New();

    vtkXMLDataElement* segmentedPoints = segmentation->FindNestedElementWithName("SegmentedPoints");
    if (segmentedPoints != NULL)   // Segmentation was successful
    {
      for (int i = 0; i < segmentedPoints->GetNumberOfNestedElements(); ++i)
      {
        vtkXMLDataElement* pointElement = segmentedPoints->GetNestedElement(i);
        if (STRCASECMP(pointElement->GetName(), "Point") != 0)
        {
          continue;
        }

        double pos[3] = {0};
        if (pointElement->GetVectorAttribute("Position", 3, pos))
        {
          this->FiducialPointsCoordinatePx->InsertNextPoint(pos);
        }
      }
    }
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
FrameSizeType PlusTrackedFrame::GetFrameSize()
{
  this->ImageData.GetFrameSize(this->FrameSize);
  return this->FrameSize;
}

//----------------------------------------------------------------------------
void PlusTrackedFrame::SetImageData(const PlusVideoFrame& value)
{
  this->ImageData = value;

  // Update our cached frame size
  this->ImageData.GetFrameSize(this->FrameSize);
}

//----------------------------------------------------------------------------
void PlusTrackedFrame::SetTimestamp(double value)
{
  this->Timestamp = value;
  std::ostringstream strTimestamp;
  strTimestamp << std::setprecision(FLOATING_POINT_PRECISION) << this->Timestamp;
  this->FrameFields["Timestamp"] = strTimestamp.str();
}

//----------------------------------------------------------------------------
int PlusTrackedFrame::GetNumberOfBitsPerScalar()
{
  int numberOfBitsPerScalar(0);
  numberOfBitsPerScalar = this->ImageData.GetNumberOfBytesPerScalar() * 8;
  return numberOfBitsPerScalar;
}

//----------------------------------------------------------------------------
int PlusTrackedFrame::GetNumberOfBitsPerPixel()
{
  int numberOfBitsPerScalar(0);
  unsigned int numberOfScalarComponents(1);
  if (this->GetNumberOfScalarComponents(numberOfScalarComponents) == PLUS_FAIL)
  {
    LOG_ERROR("Unable to retrieve number of scalar components.");
    return -1;
  }
  numberOfBitsPerScalar = this->ImageData.GetNumberOfBytesPerScalar() * 8 * numberOfScalarComponents;
  return numberOfBitsPerScalar;
}

//----------------------------------------------------------------------------
PlusStatus PlusTrackedFrame::GetNumberOfScalarComponents(unsigned int& numberOfScalarComponents)
{
  return this->ImageData.GetNumberOfScalarComponents(numberOfScalarComponents);
}

//----------------------------------------------------------------------------
void PlusTrackedFrame::SetFiducialPointsCoordinatePx(vtkPoints* fiducialPoints)
{
  if (this->FiducialPointsCoordinatePx != fiducialPoints)
  {
    vtkPoints* tempFiducialPoints = this->FiducialPointsCoordinatePx;

    this->FiducialPointsCoordinatePx = fiducialPoints;
    if (this->FiducialPointsCoordinatePx != NULL)
    {
      this->FiducialPointsCoordinatePx->Register(NULL);
    }

    if (tempFiducialPoints != NULL)
    {
      tempFiducialPoints->UnRegister(NULL);
    }
  }
}

//----------------------------------------------------------------------------
void PlusTrackedFrame::SetFrameField(std::string name, std::string value)
{
  if (STRCASECMP(name.c_str(), "Timestamp") == 0)
  {
    double timestamp(0);
    if (PlusCommon::StringToDouble(value.c_str(), timestamp) != PLUS_SUCCESS)
    {
      LOG_ERROR("Unable to convert Timestamp '" << value << "' to double");
    }
    else
    {
      this->Timestamp = timestamp;
    }
  }

  this->FrameFields[name] = value;
}

//----------------------------------------------------------------------------
const char* PlusTrackedFrame::GetFrameField(const char* fieldName)
{
  if (fieldName == NULL)
  {
    LOG_ERROR("Unable to get frame field: field name is NULL!");
    return NULL;
  }

  FieldMapType::iterator fieldIterator;
  fieldIterator = this->FrameFields.find(fieldName);
  if (fieldIterator != this->FrameFields.end())
  {
    return fieldIterator->second.c_str();
  }
  return NULL;
}

//----------------------------------------------------------------------------
const char* PlusTrackedFrame::GetFrameField(const std::string& fieldName)
{
  return this->GetFrameField(fieldName.c_str());
}

//----------------------------------------------------------------------------
PlusStatus PlusTrackedFrame::DeleteFrameField(const char* fieldName)
{
  if (fieldName == NULL)
  {
    LOG_DEBUG("Failed to delete frame field - field name is NULL!");
    return PLUS_FAIL;
  }

  FieldMapType::iterator field = this->FrameFields.find(fieldName);
  if (field != this->FrameFields.end())
  {
    this->FrameFields.erase(field);
    return PLUS_SUCCESS;
  }
  LOG_DEBUG("Failed to delete frame field - could find field " << fieldName);
  return PLUS_FAIL;
}


//----------------------------------------------------------------------------
bool PlusTrackedFrame::IsFrameTransformNameDefined(const PlusTransformName& transformName)
{
  std::string toolTransformName;
  if (transformName.GetTransformName(toolTransformName) != PLUS_SUCCESS)
  {
    return false;
  }
  // Append Transform to the end of the transform name
  if (!IsTransform(toolTransformName))
  {
    toolTransformName.append(TransformPostfix);
  }

  return this->IsFrameFieldDefined(toolTransformName.c_str());
}

//----------------------------------------------------------------------------
bool PlusTrackedFrame::IsFrameFieldDefined(const char* fieldName)
{
  if (fieldName == NULL)
  {
    LOG_ERROR("Unable to find frame field: field name is NULL!");
    return false;
  }

  FieldMapType::iterator fieldIterator;
  fieldIterator = this->FrameFields.find(fieldName);
  if (fieldIterator != this->FrameFields.end())
  {
    // field is found
    return true;
  }
  // field is undefined
  return false;
}

//----------------------------------------------------------------------------
PlusStatus PlusTrackedFrame::GetFrameTransform(const PlusTransformName& frameTransformName, double transform[16])
{
  std::string transformName;
  if (frameTransformName.GetTransformName(transformName) != PLUS_SUCCESS)
  {
    LOG_ERROR("Unable to get custom transform, transform name is wrong!");
    return PLUS_FAIL;
  }

  // Append Transform to the end of the transform name
  if (!IsTransform(transformName))
  {
    transformName.append(TransformPostfix);
  }

  const char* frameTransformStr = GetFrameField(transformName.c_str());
  if (frameTransformStr == NULL)
  {
    LOG_ERROR("Unable to get custom transform from name: " << transformName);
    return PLUS_FAIL;
  }

  // Find default frame transform
  std::istringstream transformFieldValue(frameTransformStr);
  double item;
  int i = 0;
  while (transformFieldValue >> item && i < 16)
  {
    transform[i++] = item;
  }
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus PlusTrackedFrame::GetFrameTransform(const PlusTransformName& frameTransformName, vtkMatrix4x4* transformMatrix)
{
  double transform[16] = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
  const PlusStatus retValue = this->GetFrameTransform(frameTransformName, transform);
  transformMatrix->DeepCopy(transform);

  return retValue;
}

//----------------------------------------------------------------------------
PlusStatus PlusTrackedFrame::GetFrameTransformStatus(const PlusTransformName& frameTransformName, TrackedFrameFieldStatus& status)
{
  status = FIELD_INVALID;
  std::string transformStatusName;
  if (frameTransformName.GetTransformName(transformStatusName) != PLUS_SUCCESS)
  {
    LOG_ERROR("Unable to get custom transform status, transform name is wrong!");
    return PLUS_FAIL;
  }

  // Append TransformStatus to the end of the transform name
  if (IsTransform(transformStatusName))
  {
    transformStatusName.append("Status");
  }
  else if (!IsTransformStatus(transformStatusName))
  {
    transformStatusName.append(TransformStatusPostfix);
  }

  const char* strStatus = this->GetFrameField(transformStatusName.c_str());
  if (strStatus == NULL)
  {
    LOG_ERROR("Unable to get custom transform status from name: " << transformStatusName);
    return PLUS_FAIL;
  }

  status = PlusTrackedFrame::ConvertFieldStatusFromString(strStatus);

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus PlusTrackedFrame::SetFrameTransformStatus(const PlusTransformName& frameTransformName, TrackedFrameFieldStatus status)
{
  std::string transformStatusName;
  if (frameTransformName.GetTransformName(transformStatusName) != PLUS_SUCCESS)
  {
    LOG_ERROR("Unable to set custom transform status, transform name is wrong!");
    return PLUS_FAIL;
  }

  // Append TransformStatus to the end of the transform name
  if (IsTransform(transformStatusName))
  {
    transformStatusName.append("Status");
  }
  else if (!IsTransformStatus(transformStatusName))
  {
    transformStatusName.append(TransformStatusPostfix);
  }

  std::string strStatus = PlusTrackedFrame::ConvertFieldStatusToString(status);

  this->SetFrameField(transformStatusName, strStatus);

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus PlusTrackedFrame::SetFrameTransform(const PlusTransformName& frameTransformName, double transform[16])
{
  std::ostringstream strTransform;
  for (int i = 0; i < 16; ++i)
  {
    strTransform << std::setprecision(FLOATING_POINT_PRECISION) << transform[ i ] << " ";
  }

  std::string transformName;
  if (frameTransformName.GetTransformName(transformName) != PLUS_SUCCESS)
  {
    LOG_ERROR("Unable to get custom transform, transform name is wrong!");
    return PLUS_FAIL;
  }

  // Append Transform to the end of the transform name
  if (!IsTransform(transformName))
  {
    transformName.append(TransformPostfix);
  }

  SetFrameField(transformName, strTransform.str());

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus PlusTrackedFrame::SetFrameTransform(const PlusTransformName& frameTransformName, vtkMatrix4x4* transform)
{
  double dTransform[ 16 ];
  vtkMatrix4x4::DeepCopy(dTransform, transform);
  return SetFrameTransform(frameTransformName, dTransform);
}

//----------------------------------------------------------------------------
TrackedFrameFieldStatus PlusTrackedFrame::ConvertFieldStatusFromString(const char* statusStr)
{
  if (statusStr == NULL)
  {
    LOG_ERROR("Failed to get field status from string if it's NULL!");
    return FIELD_INVALID;
  }

  TrackedFrameFieldStatus status = FIELD_INVALID;
  std::string strFlag(statusStr);
  if (STRCASECMP("OK", statusStr) == 0)
  {
    status = FIELD_OK;
  }

  return status;
}

//----------------------------------------------------------------------------
std::string PlusTrackedFrame::ConvertFieldStatusToString(TrackedFrameFieldStatus status)
{
  std::string strStatus;
  if (status == FIELD_OK)
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
PlusStatus PlusTrackedFrame::WriteToFile(const std::string& filename, vtkMatrix4x4* imageToTracker)
{
  vtkImageData* volumeToSave = this->GetImageData()->GetImage();
  MET_ValueEnumType scalarType = MET_NONE;
  switch (volumeToSave->GetScalarType())
  {
  case VTK_UNSIGNED_CHAR:
    scalarType = MET_UCHAR;
    break;
  case VTK_FLOAT:
    scalarType = MET_FLOAT;
    break;
  default:
    LOG_ERROR("Scalar type is not supported!");
    return PLUS_FAIL;
  }

  MetaImage metaImage(volumeToSave->GetDimensions()[0], volumeToSave->GetDimensions()[1], volumeToSave->GetDimensions()[2],
                      volumeToSave->GetSpacing()[0], volumeToSave->GetSpacing()[1], volumeToSave->GetSpacing()[2],
                      scalarType, volumeToSave->GetNumberOfScalarComponents(), volumeToSave->GetScalarPointer());
  double origin[3];
  origin[0] = imageToTracker->Element[0][3];
  origin[1] = imageToTracker->Element[1][3];
  origin[2] = imageToTracker->Element[2][3];
  metaImage.Origin(origin);
  for (int i = 0; i < 3; ++i)
  {
    for (int j = 0; j < 3; ++j)
    {
      metaImage.Orientation(i, j, imageToTracker->Element[i][j]);
    }
  }
  // By definition, LPS orientation in DICOM sense = RAI orientation in MetaIO. See details at:
  // http://www.itk.org/Wiki/Proposals:Orientation#Some_notes_on_the_DICOM_convention_and_current_ITK_usage
  metaImage.AnatomicalOrientation("RAI");
  metaImage.BinaryData(true);
  metaImage.CompressedData(true);
  metaImage.ElementDataFileName("LOCAL");
  if (metaImage.Write(filename.c_str()) == false)
  {
    LOG_ERROR("Failed to save reconstructed volume in sequence metafile!");
    return PLUS_FAIL;
  }
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
void PlusTrackedFrame::GetFrameFieldNameList(std::vector<std::string>& fieldNames)
{
  fieldNames.clear();
  for (FieldMapType::const_iterator it = this->FrameFields.begin(); it != this->FrameFields.end(); it++)
  {
    fieldNames.push_back(it->first);
  }
}

//----------------------------------------------------------------------------
void PlusTrackedFrame::GetFrameTransformNameList(std::vector<PlusTransformName>& transformNames)
{
  transformNames.clear();
  for (FieldMapType::const_iterator it = this->FrameFields.begin(); it != this->FrameFields.end(); it++)
  {
    if (IsTransform(it->first))
    {
      PlusTransformName trName;
      trName.SetTransformName(it->first.substr(0, it->first.length() - TransformPostfix.length()).c_str());
      transformNames.push_back(trName);
    }
  }
}

//----------------------------------------------------------------------------
bool PlusTrackedFrame::IsTransform(std::string str)
{
  if (str.length() <= TransformPostfix.length())
  {
    return false;
  }

  return PlusCommon::IsEqualInsensitive(str.substr(str.length() - TransformPostfix.length()), TransformPostfix);
}

//----------------------------------------------------------------------------
bool PlusTrackedFrame::IsTransformStatus(std::string str)
{
  if (str.length() <= TransformStatusPostfix.length())
  {
    return false;
  }

  return PlusCommon::IsEqualInsensitive(str.substr(str.length() - TransformStatusPostfix.length()), TransformStatusPostfix);
}

//----------------------------------------------------------------------------
// ****************** TrackedFrameEncoderPositionFinder **********************
//----------------------------------------------------------------------------
PlusTrackedFrameEncoderPositionFinder::PlusTrackedFrameEncoderPositionFinder(PlusTrackedFrame* frame, double minRequiredTranslationDifferenceMm, double minRequiredAngleDifferenceDeg)
  : mTrackedFrame(frame),
    mMinRequiredTranslationDifferenceMm(minRequiredTranslationDifferenceMm),
    mMinRequiredAngleDifferenceDeg(minRequiredAngleDifferenceDeg)
{

}

//----------------------------------------------------------------------------
PlusTrackedFrameEncoderPositionFinder::~PlusTrackedFrameEncoderPositionFinder()
{

}

//----------------------------------------------------------------------------
PlusStatus PlusTrackedFrameEncoderPositionFinder::GetStepperEncoderValues(PlusTrackedFrame* trackedFrame, double& probePosition, double& probeRotation, double& templatePosition)
{
  if (trackedFrame == NULL)
  {
    LOG_ERROR("Unable to get stepper encoder values - input tracked frame is NULL!");
    return PLUS_FAIL;
  }

  // Get the probe position from tracked frame info
  const char* cProbePos = trackedFrame->GetFrameField("ProbePosition");
  if (cProbePos == NULL)
  {
    LOG_ERROR("Couldn't find ProbePosition field in tracked frame!");
    return PLUS_FAIL;
  }

  if (PlusCommon::StringToDouble(cProbePos, probePosition) != PLUS_SUCCESS)
  {
    LOG_ERROR("Failed to convert probe position " << cProbePos << " to double!");
    return PLUS_FAIL;
  }

  // Get the probe rotation from tracked frame info
  const char* cProbeRot = trackedFrame->GetFrameField("ProbeRotation");
  if (cProbeRot == NULL)
  {
    LOG_ERROR("Couldn't find ProbeRotation field in tracked frame!");
    return PLUS_FAIL;
  }

  if (PlusCommon::StringToDouble(cProbeRot, probeRotation) != PLUS_SUCCESS)
  {
    LOG_ERROR("Failed to convert probe rotation " << cProbeRot << " to double!");
    return PLUS_FAIL;
  }

  // Get the template position from tracked frame info
  const char* cTemplatePos = trackedFrame->GetFrameField("TemplatePosition");
  if (cTemplatePos == NULL)
  {
    LOG_ERROR("Couldn't find TemplatePosition field in tracked frame!");
    return PLUS_FAIL;
  }

  if (PlusCommon::StringToDouble(cTemplatePos, templatePosition) != PLUS_SUCCESS)
  {
    LOG_ERROR("Failed to convert template position " << cTemplatePos << " to double!");
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
bool PlusTrackedFrameEncoderPositionFinder::operator()(PlusTrackedFrame* newFrame)
{
  if (mMinRequiredTranslationDifferenceMm <= 0 || mMinRequiredAngleDifferenceDeg <= 0)
  {
    // threshold is zero, so the frames are different for sure
    return false;
  }

  double baseProbePos(0), baseProbeRot(0), baseTemplatePos(0);


  if (GetStepperEncoderValues(mTrackedFrame, baseProbePos, baseProbeRot, baseTemplatePos) != PLUS_SUCCESS)
  {
    LOG_WARNING("Unable to get raw encoder values from tracked frame!");
    return false;
  }

  double newProbePos(0), newProbeRot(0), newTemplatePos(0);
  if (GetStepperEncoderValues(newFrame, newProbePos, newProbeRot, newTemplatePos) != PLUS_SUCCESS)
  {
    LOG_WARNING("Unable to get raw encoder values from tracked frame!");
    return false;
  }

  double positionDifference = fabs(baseProbePos - newProbePos) + fabs(baseTemplatePos - newTemplatePos);
  double rotationDifference = fabs(baseProbeRot - newProbeRot);

  if (positionDifference < this->mMinRequiredTranslationDifferenceMm && rotationDifference < this->mMinRequiredAngleDifferenceDeg)
  {
    // same as the reference frame
    return true;
  }
  return false;
}



//----------------------------------------------------------------------------
// ****************** TrackedFrameTransformFinder ****************************
//----------------------------------------------------------------------------
TrackedFrameTransformFinder::TrackedFrameTransformFinder(PlusTrackedFrame* frame, const PlusTransformName& frameTransformName, double minRequiredTranslationDifferenceMm, double minRequiredAngleDifferenceDeg)
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
bool TrackedFrameTransformFinder::operator()(PlusTrackedFrame* newFrame)
{
  if (mMinRequiredTranslationDifferenceMm <= 0 || mMinRequiredAngleDifferenceDeg <= 0)
  {
    // threshold is zero, so the frames are different for sure
    return false;
  }

  vtkSmartPointer<vtkMatrix4x4> baseTransformMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
  double baseTransMatrix[16] = {0};
  if (mTrackedFrame->GetFrameTransform(mFrameTransformName, baseTransMatrix))
  {
    baseTransformMatrix->DeepCopy(baseTransMatrix);
  }
  else
  {
    LOG_ERROR("TrackedFramePositionFinder: Unable to find base frame transform name for tracked frame validation!");
    return false;
  }

  vtkSmartPointer<vtkMatrix4x4> newTransformMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
  double newTransMatrix[16] = {0};
  if (newFrame->GetFrameTransform(mFrameTransformName, newTransMatrix))
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

  if (fabs(positionDifference) < this->mMinRequiredTranslationDifferenceMm && fabs(angleDifference) < this->mMinRequiredAngleDifferenceDeg)
  {
    // same as the reference frame
    return true;
  }

  return false;
}
