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
  \brief  Contains an optional timestamped circular buffer containing the video images and a number of timestamped circular buffer of StreamBufferItems for the transforms. 
          StreamBufferItems are essentially a class that contains both a single video frame and/or a 4x4 matrix.

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
  PlusStatus ReadConfiguration(vtkXMLDataElement* aChannelElement);
  /*!
    Write the details about the stream to XML
  */
  PlusStatus WriteConfiguration(vtkXMLDataElement* aChannelElement);

  PlusStatus GetVideoSource( vtkPlusDataSource*& aVideoSource ) const;
  void SetVideoSource( vtkPlusDataSource* aSource );
  bool HasVideoSource() const;

  int ToolCount() const { return this->Tools.size(); }
  PlusStatus AddTool(vtkPlusDataSource* aTool );
  PlusStatus RemoveTool(const char* toolName);
  PlusStatus GetTool(vtkPlusDataSource*& aTool, const char* toolName);
  DataSourceContainerIterator GetToolsStartIterator();
  DataSourceContainerIterator GetToolsEndIterator();
  DataSourceContainerConstIterator GetToolsStartConstIterator() const;
  DataSourceContainerConstIterator GetToolsEndConstIterator() const;

  PlusStatus Clear();

  virtual void ShallowCopy(const vtkPlusChannel& aChannel);

  PlusStatus GetLatestTimestamp(double& aTimestamp) const;

  vtkSetObjectMacro(OwnerDevice, vtkPlusDevice);
  vtkGetObjectMacro(OwnerDevice, vtkPlusDevice);

  vtkSetStringMacro(ChannelId);
  vtkGetStringMacro(ChannelId);

protected:
  DataSourceContainer       Tools;
  vtkPlusDataSource*        VideoSource;
  vtkPlusDevice*            OwnerDevice;
  char *                    ChannelId;

  vtkPlusChannel(void);
  virtual ~vtkPlusChannel(void);

private:
  vtkPlusChannel(const vtkPlusChannel&);
  void operator=(const vtkPlusChannel&);
};

#endif