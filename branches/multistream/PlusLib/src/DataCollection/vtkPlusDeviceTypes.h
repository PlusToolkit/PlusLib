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

typedef std::map<std::string, vtkPlusStreamTool*> ToolContainerType;
typedef ToolContainerType::const_iterator ToolContainerConstIteratorType;

typedef std::vector<vtkPlusDevice*> DeviceCollection;
typedef std::vector<vtkPlusDevice*>::iterator DeviceCollectionIterator;
typedef std::vector<vtkPlusDevice*>::const_iterator DeviceCollectionConstIterator;

typedef std::vector<vtkVirtualStreamMixer*> StreamMixerCollection;
typedef std::vector<vtkVirtualStreamMixer*>::iterator StreamMixerCollectionIterator;
typedef std::vector<vtkVirtualStreamMixer*>::const_iterator StreamMixerCollectionConstIterator;

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

  /*! Copy video buffer item */
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

private:
  PlusVideoFrame Frame;
  vtkSmartPointer<vtkMatrix4x4> Matrix;
  ToolStatus Status;
};

#endif
