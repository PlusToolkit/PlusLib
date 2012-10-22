/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/ 

#ifndef __vtkPlusStream_h
#define __vtkPlusStream_h

#include "PlusConfigure.h"
#include "vtkDataObject.h"
#include "vtkPlusStreamBuffer.h"

#include <vector>

/*!
  \class vtkPlusStream 
  \brief Contains a number of timestamped circular buffer of StreamBufferItems. StreamBufferItems are essentially a class that contains both a single video frame and/or a 4x4 matrix.

  \ingroup PlusLibDataCollection
*/
class VTK_EXPORT vtkPlusStream : public vtkDataObject
{
public:
  typedef std::vector<vtkPlusStreamBuffer*> StreamBufferContainer;
  typedef StreamBufferContainer::const_iterator StreamBufferContainerConstIterator;
  typedef StreamBufferContainer::iterator StreamBufferContainerIterator;

  static vtkPlusStream *New();
  vtkTypeRevisionMacro(vtkPlusStream, vtkObject);

  PlusStatus ReadConfiguration(vtkXMLDataElement* aStreamElement);

protected:
  StreamBufferContainer   StreamBuffers;

  vtkPlusStream(void);
  virtual ~vtkPlusStream(void);

private:
  vtkPlusStream(const vtkPlusStream&);
  void operator=(const vtkPlusStream&);
};

#endif