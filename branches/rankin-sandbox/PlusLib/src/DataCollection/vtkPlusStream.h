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

  /*!
    Parse the XML, read the details about the stream
  */
  PlusStatus ReadConfiguration(vtkXMLDataElement* aStreamElement);
  /*!
    Write the details about the stream to XML
  */
  PlusStatus WriteConfiguration(vtkXMLDataElement* aStreamElement);

  int ImageCount() const { return this->Images.size(); }
  PlusStatus AddImage(vtkPlusStreamImage* anImage);
  PlusStatus RemoveImage(const char* imageName);
  PlusStatus GetImage( vtkPlusStreamImage*& anImage, const char* name );
  ImageContainerIterator GetImagesStartIterator();
  ImageContainerIterator GetImagesEndIterator();
  ImageContainerConstIterator GetImagesStartConstIterator() const;
  ImageContainerConstIterator GetImagesEndConstIterator() const;

  int ToolCount() const { return this->Tools.size(); }
  PlusStatus AddTool(vtkPlusStreamTool* aTool );
  PlusStatus RemoveTool(const char* toolName);
  PlusStatus GetTool(vtkPlusStreamTool*& aTool, const char* toolName);
  ToolContainerIterator GetToolsStartIterator();
  ToolContainerIterator GetToolsEndIterator();
  ToolContainerConstIterator GetToolsStartConstIterator() const;
  ToolContainerConstIterator GetToolsEndConstIterator() const;

  PlusStatus Clear();

  virtual void DeepCopy(const vtkPlusStream& aStream);
  virtual void ShallowCopy(const vtkPlusStream& aStream);

  PlusStatus GetLatestTimestamp(double& aTimestamp) const;

  vtkSetObjectMacro(OwnerDevice, vtkPlusDevice);
  vtkGetObjectMacro(OwnerDevice, vtkPlusDevice);

  vtkSetStringMacro(StreamId);
  vtkGetStringMacro(StreamId);

protected:
  ToolContainer             Tools;
  ImageContainer            Images;
  vtkPlusDevice*            OwnerDevice;
  char *                    StreamId;

  vtkPlusStream(void);
  virtual ~vtkPlusStream(void);

private:
  vtkPlusStream(const vtkPlusStream&);
  void operator=(const vtkPlusStream&);
};

#endif