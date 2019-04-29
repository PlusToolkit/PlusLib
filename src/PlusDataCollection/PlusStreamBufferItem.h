/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __StreamBufferItem_h
#define __StreamBufferItem_h

#include "vtkPlusDataCollectionExport.h"

// IGSIO includes
#include <igsioCommon.h>

// VTK includes
#include <vtkSmartPointer.h>

#include <vector>

class vtkMatrix4x4;
class vtkPlusDevice;
class vtkPlusChannel;
class vtkPlusDataSource;
class vtkPlusDataSource;
class vtkPlusVirtualMixer;

#ifdef _WIN32
  typedef unsigned __int64 BufferItemUidType;
#else
  typedef unsigned long long BufferItemUidType;
#endif

/*!
  \class DataBufferItem
  \brief Stores a single video frame OR a single transform with a timestamp. This object can be stored in a timestamped buffer.
  \ingroup PlusLibDataCollection
*/
class vtkPlusDataCollectionExport StreamBufferItem
{
public:
  StreamBufferItem();
  virtual ~StreamBufferItem();

  StreamBufferItem(const StreamBufferItem& dataItem);
  StreamBufferItem& operator=(StreamBufferItem const& dataItem);

  /*! Get timestamp for the current buffer item in global time (global = local + offset) */
  double GetTimestamp(double localTimeOffsetSec) { return this->GetFilteredTimestamp(localTimeOffsetSec); }

  /*! Get filtered timestamp in global time (global = local + offset) */
  double GetFilteredTimestamp(double localTimeOffsetSec) { return this->FilteredTimeStamp + localTimeOffsetSec; }

  /*! Set filtered timestamp */
  void SetFilteredTimestamp(double filteredTimestamp) { this->FilteredTimeStamp = filteredTimestamp; }

  /*! Get unfiltered timestamp in global time (global = local + offset) */
  double GetUnfilteredTimestamp(double localTimeOffsetSec) { return this->UnfilteredTimeStamp + localTimeOffsetSec; }

  /*! Set unfiltered timestamp */
  void SetUnfilteredTimestamp(double unfilteredTimestamp) { this->UnfilteredTimeStamp = unfilteredTimestamp; }

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

  /*! Set frame field */
  void SetFrameField(std::string fieldName, std::string fieldValue, igsioFrameFieldFlags flags = FRAMEFIELD_NONE);

  /*! Get frame field value */
  std::string GetFrameField(const std::string& fieldName) const;
  /*! Get frame field map */
  igsioFieldMapType GetFrameFieldMap() {return this->FrameFields;}
  /*! Delete frame field */
  PlusStatus DeleteFrameField(const char* fieldName);
  PlusStatus DeleteFrameField(const std::string& fieldName);

  /*! Copy stream buffer item */
  PlusStatus DeepCopy(StreamBufferItem* dataItem);

  igsioVideoFrame& GetFrame() { return this->Frame; };

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
  bool HasValidFieldData() const;
  bool HasValidVideoData() const
  {
    return Frame.IsImageValid();
  }

protected:
  double FilteredTimeStamp;
  double UnfilteredTimeStamp;

  /*! index assigned by the data acquisition system (usually a counter) */
  unsigned long Index;

  /*! unique identifier assigned by the storage buffer, it is guaranteed to increase monotonously, by one for each frame that is added to the buffer*/
  BufferItemUidType Uid;

  /*! Custom frame fields */
  igsioFieldMapType FrameFields;

  bool ValidTransformData;
  igsioVideoFrame Frame;
  vtkSmartPointer<vtkMatrix4x4> Matrix;
  ToolStatus Status;
};

#endif
