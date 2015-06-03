/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

#ifndef __StreamBufferItem_h
#define __StreamBufferItem_h

#include "vtkDataCollectionExport.h"

#include "PlusCommon.h"
#include "PlusVideoFrame.h"

#include "vtkSmartPointer.h"

#include <vector>

class vtkMatrix4x4;
class vtkPlusDevice;
class vtkPlusChannel;
class vtkPlusDataSource;
class vtkPlusDataSource;
class vtkVirtualMixer;

#ifdef _WIN32
  typedef unsigned __int64 BufferItemUidType;
#else
  typedef unsigned long long BufferItemUidType;
#endif

/*! Flags for tool statuses */
enum ToolStatus 
{
  TOOL_OK,            /*!< Tool OK */
  TOOL_MISSING,       /*!< Tool or tool port is not available */
  TOOL_OUT_OF_VIEW,   /*!< Cannot obtain transform for tool */
  TOOL_OUT_OF_VOLUME, /*!< Tool is not within the sweet spot of system */
  TOOL_SWITCH1_IS_ON, /*!< Various buttons/switches on tool */
  TOOL_SWITCH2_IS_ON, /*!< Various buttons/switches on tool */
  TOOL_SWITCH3_IS_ON, /*!< Various buttons/switches on tool */
  TOOL_REQ_TIMEOUT,   /*!< Request timeout status */
  TOOL_INVALID        /*!< Invalid tool status */
};

/*!
  \class DataBufferItem 
  \brief Stores a single video frame OR a single transform with a timestamp. This object can be stored in a timestamped buffer.
  \ingroup PlusLibDataCollection
*/
class vtkDataCollectionExport StreamBufferItem
{
public:
  typedef std::map<std::string, std::string> FieldMapType;

  StreamBufferItem();
  virtual ~StreamBufferItem();

  StreamBufferItem(const StreamBufferItem& dataItem); 
  StreamBufferItem& operator=(StreamBufferItem const& dataItem); 

  /*! Get timestamp for the current buffer item in global time (global = local + offset) */
  double GetTimestamp( double localTimeOffsetSec) { return this->GetFilteredTimestamp(localTimeOffsetSec); }
  
  /*! Get filtered timestamp in global time (global = local + offset) */
  double GetFilteredTimestamp( double localTimeOffsetSec) { return this->FilteredTimeStamp + localTimeOffsetSec; }
  
  /*! Set filtered timestamp */
  void SetFilteredTimestamp( double filteredTimestamp) { this->FilteredTimeStamp = filteredTimestamp; }

  /*! Get unfiltered timestamp in global time (global = local + offset) */
  double GetUnfilteredTimestamp( double localTimeOffsetSec) { return this->UnfilteredTimeStamp + localTimeOffsetSec; }
  
  /*! Set unfiltered timestamp */
  void SetUnfilteredTimestamp( double unfilteredTimestamp) { this->UnfilteredTimeStamp = unfilteredTimestamp; }

  /*!
    Set/get index assigned by the data acquisition system (usually a counter)
    If frames are skipped then the counter should be increased by the number of skipped frames, therefore
    the index difference between subsequent frames be more than 1.
  */
  unsigned long GetIndex() { return this->Index; }; 
  void SetIndex(unsigned long index) { this->Index = index; }; 

  /*! Set/get unique identifier assigned by the storage buffer */
  BufferItemUidType GetUid() { return this->Uid; }; 
  void SetUid(BufferItemUidType uid) { this->Uid = uid; }; 

  /*! Set custom frame field */
  void SetCustomFrameField(std::string fieldName, std::string fieldValue) { this->CustomFrameFields[fieldName] = fieldValue; }

  /*! Get custom frame field value */ 
  const char* GetCustomFrameField(const char* fieldName)
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
  /*! Get custom frame field map */
  FieldMapType& GetCustomFrameFieldMap()
  {
    return this->CustomFrameFields;
  }
  /*! Delete custom frame field */
  PlusStatus DeleteCustomFrameField( const char* fieldName )
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
  
  /*! Copy stream buffer item */
  PlusStatus DeepCopy(StreamBufferItem* dataItem); 
  
  PlusVideoFrame& GetFrame() { return this->Frame; };

  /*! Set tracker matrix */
  PlusStatus SetMatrix(vtkMatrix4x4* matrix); 
  /*! Get tracker matrix */
  PlusStatus GetMatrix(vtkMatrix4x4* outputMatrix);

  /*! Set tracker item status */
  void SetStatus(ToolStatus status);
  /*! Get tracker item status */
  ToolStatus GetStatus() const;

  void SetValidTransformData(bool aValid) { ValidTransformData = aValid; }
  bool HasValidTransformData() const { return ValidTransformData; }
  bool HasValidVideoData() const 
  { 
    return Frame.IsImageValid();
  }

protected:

  double FilteredTimeStamp;
  double UnfilteredTimeStamp;

  /*! index assigned by the data acuiqisition system (usually a counter) */
  unsigned long Index; 

  /*! unique identifier assigned by the storage buffer, it is guaranteed to increase monotonously, by one for each frame that is added to the buffer*/
  BufferItemUidType Uid; 

  /*! Custom frame fields */
  FieldMapType CustomFrameFields;

  bool ValidTransformData;
  PlusVideoFrame Frame;
  vtkSmartPointer<vtkMatrix4x4> Matrix;
  ToolStatus Status;
};

#endif
