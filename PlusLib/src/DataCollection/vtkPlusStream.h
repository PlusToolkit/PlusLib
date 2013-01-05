/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/ 

#ifndef __vtkPlusStream_h
#define __vtkPlusStream_h

#include "PlusConfigure.h"
#include "vtkDataObject.h"
#include "vtkPlusDevice.h"

class vtkPlusDevice;

/*!
  \class vtkPlusStream 
  \brief Contains a number of timestamped circular buffer of StreamBufferItems. StreamBufferItems are essentially a class that contains both a single video frame and/or a 4x4 matrix.

  \ingroup PlusLibDataCollection
*/
class VTK_EXPORT vtkPlusStream : public vtkDataObject
{
public:
  static const int FIND_PORT;
  static int MAX_PORT;

  static vtkPlusStream *New();
  vtkTypeRevisionMacro(vtkPlusStream, vtkObject);

  /*!
    Parse the XML, read the details about the stream
  */
  PlusStatus ReadConfiguration(vtkXMLDataElement* aStreamElement, bool RequireFrameBufferSizeInDeviceSetConfiguration = false, bool RequireAveragedItemsForFilteringInDeviceSetConfiguration = false);
  /*!
    Write the details about the stream to XML
  */
  PlusStatus WriteConfiguration(vtkXMLDataElement* aStreamElement);
  /*!
    Write select details about the stream to XML
  */
  PlusStatus WriteCompactConfiguration(vtkXMLDataElement* aStreamElement);

  int BufferCount() const { return this->StreamBuffers.size(); }
  PlusStatus AddBuffer(vtkPlusStreamBuffer* aBuffer, int aPort);
  PlusStatus GetBuffer(vtkPlusStreamBuffer*& aBuffer, int port);
  StreamBufferMapContainerConstIterator GetBuffersStartConstIterator() const;
  StreamBufferMapContainerConstIterator GetBuffersEndConstIterator() const;

  int ToolCount() const { return this->Tools.size(); }
  PlusStatus AddTool(vtkPlusStreamTool* aTool );
  PlusStatus RemoveTool(const char* toolName);
  PlusStatus GetTool(vtkPlusStreamTool*& aTool, const char* toolName);
  ToolContainerConstIterator GetToolBuffersStartConstIterator() const;
  ToolContainerIterator GetToolBuffersStartIterator();
  ToolContainerConstIterator GetToolBuffersEndConstIterator() const;
  ToolContainerIterator GetToolBuffersEndIterator();

  PlusStatus Clear();

  virtual void DeepCopy(const vtkPlusStream& aStream);
  virtual void ShallowCopy(const vtkPlusStream& aStream);

  PlusStatus GetLatestTimestamp(double& aTimestamp) const;

  vtkSetObjectMacro(OwnerDevice, vtkPlusDevice);
  vtkGetObjectMacro(OwnerDevice, vtkPlusDevice);

  vtkSetStringMacro(StreamId);
  vtkGetStringMacro(StreamId);

protected:
  StreamBufferMapContainer  StreamBuffers;

  ToolContainer             Tools;
  vtkPlusDevice*            OwnerDevice;
  char *                    StreamId;

  vtkPlusStream(void);
  virtual ~vtkPlusStream(void);

private:
  vtkPlusStream(const vtkPlusStream&);
  void operator=(const vtkPlusStream&);
};

#endif