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
  \class vtkPlusChannel 
  \brief Contains a number of timestamped circular buffer of StreamBufferItems. StreamBufferItems are essentially a class that contains both a single video frame and/or a 4x4 matrix.

  \ingroup PlusLibDataCollection
*/
class VTK_EXPORT vtkPlusChannel : public vtkDataObject
{
public:
  static vtkPlusChannel *New();
  vtkTypeRevisionMacro(vtkPlusChannel, vtkObject);

  /*!
    Parse the XML, read the details about the stream
  */
  PlusStatus ReadConfiguration(vtkXMLDataElement* aStreamElement);
  /*!
    Write the details about the stream to XML
  */
  PlusStatus WriteConfiguration(vtkXMLDataElement* aStreamElement);

  int ImageCount() const { return this->Images.size(); }
  PlusStatus AddImage(vtkPlusDataSource* anImage);
  PlusStatus RemoveImage(const char* imageName);
  PlusStatus GetImage( vtkPlusDataSource*& anImage, const char* name );
  ImageContainerIterator GetImagesStartIterator();
  ImageContainerIterator GetImagesEndIterator();
  ImageContainerConstIterator GetImagesStartConstIterator() const;
  ImageContainerConstIterator GetImagesEndConstIterator() const;

  int ToolCount() const { return this->Tools.size(); }
  PlusStatus AddTool(vtkPlusDataSource* aTool );
  PlusStatus RemoveTool(const char* toolName);
  PlusStatus GetTool(vtkPlusDataSource*& aTool, const char* toolName);
  ToolContainerIterator GetToolsStartIterator();
  ToolContainerIterator GetToolsEndIterator();
  ToolContainerConstIterator GetToolsStartConstIterator() const;
  ToolContainerConstIterator GetToolsEndConstIterator() const;

  PlusStatus Clear();

  virtual void DeepCopy(const vtkPlusChannel& aStream);
  virtual void ShallowCopy(const vtkPlusChannel& aStream);

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

  vtkPlusChannel(void);
  virtual ~vtkPlusChannel(void);

private:
  vtkPlusChannel(const vtkPlusChannel&);
  void operator=(const vtkPlusChannel&);
};

#endif