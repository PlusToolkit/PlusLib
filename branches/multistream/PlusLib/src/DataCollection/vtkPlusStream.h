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
  static int MAX_PORT;

  static vtkPlusStream *New();
  vtkTypeRevisionMacro(vtkPlusStream, vtkObject);

  PlusStatus ReadConfiguration(vtkXMLDataElement* aStreamElement);
  PlusStatus AddBuffer(vtkPlusStreamBuffer* aBuffer, int& outNewPort);
  PlusStatus GetBuffer(vtkPlusStreamBuffer*& aBuffer, int port);
  StreamBufferMapContainerConstIterator GetBuffersStartConstIterator() const;
  StreamBufferMapContainerConstIterator GetBuffersEndConstIterator() const;
  PlusStatus AddTool(vtkPlusStreamTool* aTool);
  PlusStatus RemoveTool(const char* toolName);
  PlusStatus GetTool(vtkPlusStreamTool*& aTool, const char* toolName);
  ToolContainerConstIteratorType GetToolBuffersStartConstIterator() const;
  ToolContainerIteratorType GetToolBuffersStartIterator();
  ToolContainerConstIteratorType GetToolBuffersEndConstIterator() const;
  ToolContainerIteratorType GetToolBuffersEndIterator();

  PlusStatus Clear();

  vtkSetObjectMacro(OwnerDevice, vtkPlusDevice);
  vtkGetObjectMacro(OwnerDevice, vtkPlusDevice);

  vtkSetStringMacro(StreamId);
  vtkGetStringMacro(StreamId);

protected:
  StreamBufferMapContainer  StreamBuffers;
  ToolContainerType         Tools;
  vtkPlusDevice*            OwnerDevice;
  char *                    StreamId;

  vtkPlusStream(void);
  virtual ~vtkPlusStream(void);

private:
  vtkPlusStream(const vtkPlusStream&);
  void operator=(const vtkPlusStream&);
};

#endif