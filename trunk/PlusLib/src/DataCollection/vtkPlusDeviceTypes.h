/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

#ifndef __vtkPlusDeviceTypes_h
#define __vtkPlusDeviceTypes_h

class vtkPlusDevice;
class vtkPlusStream;
class vtkPlusStreamBuffer;
class vtkPlusStreamTool;
class vtkVirtualStreamMixer;

#include "PlusVideoFrame.h"
#include "vtkMatrix4x4.h"
#include "vtkSmartPointer.h"
#include "vtkTimestampedCircularBuffer.h"
#include <vector>

/*! Flags for tool LEDs (specifically for the POLARIS) */
enum {
  TR_LED_OFF   = 0,
  TR_LED_ON    = 1,
  TR_LED_FLASH = 2
};

/*! Flags for tool statuses */
enum ToolStatus 
{
  TOOL_OK,			      /*!< Tool OK */
  TOOL_MISSING,       /*!< Tool or tool port is not available */
  TOOL_OUT_OF_VIEW,   /*!< Cannot obtain transform for tool */
  TOOL_OUT_OF_VOLUME, /*!< Tool is not within the sweet spot of system */
  TOOL_SWITCH1_IS_ON, /*!< Various buttons/switches on tool */
  TOOL_SWITCH2_IS_ON, /*!< Various buttons/switches on tool */
  TOOL_SWITCH3_IS_ON, /*!< Various buttons/switches on tool */
  TOOL_REQ_TIMEOUT,   /*!< Request timeout status */
  TOOL_INVALID        /*!< Invalid tool status */
};

typedef std::vector<vtkPlusStream*> StreamContainer;
typedef StreamContainer::const_iterator StreamContainerConstIterator;
typedef StreamContainer::iterator StreamContainerIterator;

typedef std::vector<vtkPlusStreamBuffer*> StreamBufferContainer;
typedef StreamBufferContainer::const_iterator StreamBufferContainerConstIterator;
typedef StreamBufferContainer::iterator StreamBufferContainerIterator;

typedef std::map<int, vtkPlusStreamBuffer*> StreamBufferMapContainer;
typedef StreamBufferMapContainer::const_iterator StreamBufferMapContainerConstIterator;
typedef StreamBufferMapContainer::iterator StreamBufferMapContainerIterator;

typedef std::map<std::string, vtkPlusStreamTool*> ToolContainer;
typedef ToolContainer::iterator ToolContainerIterator;
typedef ToolContainer::const_iterator ToolContainerConstIterator;

typedef std::vector<vtkPlusDevice*> DeviceCollection;
typedef std::vector<vtkPlusDevice*>::iterator DeviceCollectionIterator;
typedef std::vector<vtkPlusDevice*>::const_iterator DeviceCollectionConstIterator;

/*!
  \class DataBufferItem 
  \brief Stores a single video frame OR a single transform
  \ingroup PlusLibDataCollection
*/
class VTK_EXPORT StreamBufferItem : public TimestampedBufferItem
{
public:

  StreamBufferItem();
  virtual ~StreamBufferItem();

  StreamBufferItem(const StreamBufferItem& dataItem); 
  StreamBufferItem& operator=(StreamBufferItem const& dataItem); 

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

  bool HasValidTransformData() const { return ValidTransformData; }
  bool HasValidVideoData() const 
  { 
    int frameSize[2] = {0,0};
    if( Frame.GetFrameSize(frameSize) != PLUS_SUCCESS)
    { 
      return false;
    }
    else
    {
      return frameSize[0] > 0 && frameSize[1] > 0;
    }
  }

private:
  bool ValidTransformData;
  PlusVideoFrame Frame;
  vtkSmartPointer<vtkMatrix4x4> Matrix;
  ToolStatus Status;
};

#endif
