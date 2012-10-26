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
  static vtkPlusStream *New();
  vtkTypeRevisionMacro(vtkPlusStream, vtkObject);

  PlusStatus ReadConfiguration(vtkXMLDataElement* aStreamElement);

  PlusStatus GetBuffer(vtkPlusStreamBuffer*& aBuffer, int port);
  StreamBufferContainerConstIterator GetBuffersStartConstIterator() const;
  StreamBufferContainerConstIterator GetBuffersEndConstIterator() const;
  PlusStatus GetTool(vtkPlusStreamTool*& aTool, const char* toolName);

  vtkSetObjectMacro(OwnerDevice, vtkPlusDevice);
  vtkGetObjectMacro(OwnerDevice, vtkPlusDevice);

  vtkSetStringMacro(StreamId);
  vtkGetStringMacro(StreamId);

protected:
  StreamBufferContainer   StreamBuffers;
  ToolContainerType       Tools;
  vtkPlusDevice*          OwnerDevice;
  char *                  StreamId;

  vtkPlusStream(void);
  virtual ~vtkPlusStream(void);

private:
  vtkPlusStream(const vtkPlusStream&);
  void operator=(const vtkPlusStream&);
};

#endif