/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"
#include "PlusStreamBufferItem.h"
#include "vtkMatrix4x4.h"

//----------------------------------------------------------------------------
//            DataBufferItem
//----------------------------------------------------------------------------
StreamBufferItem::StreamBufferItem()
  : FilteredTimeStamp(0)
  , UnfilteredTimeStamp(0)
  , Index(0)
  , Uid(0)
  , ValidTransformData(false)
  , Matrix(vtkSmartPointer<vtkMatrix4x4>::New())
  , Status(TOOL_OK)
{
}

//----------------------------------------------------------------------------
StreamBufferItem::~StreamBufferItem()
{
}

//----------------------------------------------------------------------------
StreamBufferItem::StreamBufferItem(const StreamBufferItem& dataItem)
{
  this->Matrix = vtkSmartPointer<vtkMatrix4x4>::New();
  this->Status = TOOL_OK;
  *this = dataItem;
}

//----------------------------------------------------------------------------
StreamBufferItem& StreamBufferItem::operator=(StreamBufferItem const& dataItem)
{
  // Handle self-assignment
  if (this == &dataItem)
  {
    return *this;
  }

  this->Frame = dataItem.Frame;
  this->FilteredTimeStamp = dataItem.FilteredTimeStamp;
  this->UnfilteredTimeStamp = dataItem.UnfilteredTimeStamp;
  this->Index = dataItem.Index;
  this->Uid = dataItem.Uid;
  this->FrameFields = dataItem.FrameFields;
  this->Status = dataItem.Status;
  this->Matrix->DeepCopy(dataItem.Matrix);
  this->ValidTransformData = dataItem.ValidTransformData;

  return *this;
}

//----------------------------------------------------------------------------
void StreamBufferItem::SetFrameField(std::string fieldName, std::string fieldValue, igsioFrameFieldFlags flags)
{
  this->FrameFields[fieldName].first = flags;
  this->FrameFields[fieldName].second = fieldValue;
}

//----------------------------------------------------------------------------
std::string StreamBufferItem::GetFrameField(const std::string& fieldName) const
{
  if (fieldName.empty())
  {
    LOG_ERROR("Unable to get frame field: field name is NULL!");
    return "";
  }

  igsioFieldMapType::const_iterator fieldIterator;
  fieldIterator = this->FrameFields.find(fieldName);
  if (fieldIterator != this->FrameFields.end())
  {
    return fieldIterator->second.second;
  }
  return "";
}

//----------------------------------------------------------------------------
PlusStatus StreamBufferItem::DeleteFrameField(const char* fieldName)
{
  if (fieldName == NULL)
  {
    LOG_DEBUG("Failed to delete frame field - field name is NULL!");
    return PLUS_FAIL;
  }

  igsioFieldMapType::iterator field = this->FrameFields.find(fieldName);
  if (field != this->FrameFields.end())
  {
    this->FrameFields.erase(field);
    return PLUS_SUCCESS;
  }
  LOG_DEBUG("Failed to delete frame field - could find field " << fieldName);
  return PLUS_FAIL;
}

//----------------------------------------------------------------------------
PlusStatus StreamBufferItem::DeleteFrameField(const std::string& fieldName)
{
  return this->DeleteFrameField(fieldName.c_str());
}

//----------------------------------------------------------------------------
PlusStatus StreamBufferItem::DeepCopy(StreamBufferItem* dataItem)
{
  if (dataItem == NULL)
  {
    LOG_ERROR("Failed to deep copy data buffer item - buffer item NULL!");
    return PLUS_FAIL;
  }

  (*this) = (*dataItem);

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus StreamBufferItem::SetMatrix(vtkMatrix4x4* matrix)
{
  if (matrix == NULL)
  {
    LOG_ERROR("Failed to set matrix - input matrix is NULL!");
    return PLUS_FAIL;
  }

  ValidTransformData = true;

  this->Matrix->DeepCopy(matrix);

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus StreamBufferItem::GetMatrix(vtkMatrix4x4* outputMatrix)
{
  if (outputMatrix == NULL)
  {
    LOG_ERROR("Failed to copy matrix - output matrix is NULL!");
    return PLUS_FAIL;
  }

  outputMatrix->DeepCopy(this->Matrix);

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
void StreamBufferItem::SetStatus(ToolStatus status)
{
  this->Status = status;
}

//----------------------------------------------------------------------------
ToolStatus StreamBufferItem::GetStatus() const
{
  return this->Status;
}

//----------------------------------------------------------------------------
bool StreamBufferItem::HasValidFieldData() const
{
  return this->FrameFields.size() > 0;
}